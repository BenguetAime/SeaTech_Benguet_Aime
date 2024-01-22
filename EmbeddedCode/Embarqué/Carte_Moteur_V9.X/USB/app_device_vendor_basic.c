/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/** INCLUDES *******************************************************/
#include "../HardwareProfile_CM_V9.h"
#include "usb.h"
#include "usb_device_generic.h"
#include "system.h"
#include "../UTLN_CircularBuffer.h"
#include "../UTLN_Communication.h"


#include <stdint.h>

/** DECLARATIONS ***************************************************/

/** TYPE DEFINITIONS ************************************************/

/** VARIABLES ******************************************************/
/* Some processors have a limited range of RAM addresses where the USB module
 * is able to access.  The following section is for those devices.  This section
 * assigns the buffers that need to be used by the USB module into those
 * specific areas.
 */
#define USB_BUFFER_SIZE USBGEN_EP_SIZE
//User application buffer for receiving and holding IN packets from the host
unsigned char INPacket[USB_BUFFER_SIZE];

//User application buffer for sending OUT packets to the host
unsigned char OUTEvenPacket[USB_BUFFER_SIZE];
unsigned char OUTOddPacket[USB_BUFFER_SIZE];

static bool EP1OUTEvenNeedsServicingNext;	//Used to keep track of which buffer will contain the next sequential data packet.
static USB_HANDLE USBGenericOutEvenHandle;  //USB handle.  Must be initialized to 0 at startup.
static USB_HANDLE USBGenericOutOddHandle;  //USB handle.  Must be initialized to 0 at startup.
static USB_HANDLE USBGenericInHandle;   //USB handle.  Must be initialized to 0 at startup.

unsigned int noUSBInput=0;
/*********************************************************************
* Function: void APP_DeviceVendorBasicDemoInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceVendorBasicDemoInitialize(void)
{  
    //initialize the variable holding the handle for the last
    // transmission
    USBGenericOutEvenHandle = 0;
    USBGenericOutOddHandle=0;
    USBGenericInHandle = 0;

    //Enable the application endpoints
    USBEnableEndpoint(USBGEN_EP_NUM,USB_OUT_ENABLED|USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    //Arm the application IN endpoint, so it can receive a packet from the host
    USBGenericInHandle = USBGenRead(USBGEN_EP_NUM,(uint8_t*)&INPacket,USBGEN_EP_SIZE);
}//end UserInit

/*********************************************************************
* Function: void APP_DeviceVendorBasicDemoTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceVendorBasicDemoInitialize() and APP_DeviceVendorBasicDemoStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceVendorTasks(void)
{  
    /* If the USB device isn't configured yet, we can't really do anything
     * else since we don't have a host to talk to.  So jump back to the
     * top of the while loop. */
    if( USBGetDeviceState() < CONFIGURED_STATE )
    {
        return;
    }

    /* If we are currently suspended, then we need to see if we need to
     * issue a remote wakeup.  In either case, we shouldn't process any
     * keyboard commands since we aren't currently communicating to the host
     * thus just continue back to the start of the while loop. */
    if( USBIsDeviceSuspended()== true )
    {
        return;
    }
    
   
    
    //As the device completes the enumeration process, the USBCBInitEP() function will
    //get called.  In this function, we initialize the user application endpoints (in this
    //example code, the user application makes use of endpoint 1 IN and endpoint 1 OUT).
    //The USBGenRead() function call in the USBCBInitEP() function initializes endpoint 1 OUT
    //and "arms" it so that it can receive a packet of data from the host.  Once the endpoint
    //has been armed, the host can then send data to it (assuming some kind of application software
    //is running on the host, and the application software tries to send data to the USB device).

    //If the host sends a packet of data to the endpoint 1 OUT buffer, the hardware of the SIE will
    //automatically receive it and store the data at the memory location pointed to when we called
    //USBGenRead().  Additionally, the endpoint handle (in this case USBGenericOutHandle) will indicate
    //that the endpoint is no longer busy.  At this point, it is safe for this firmware to begin reading
    //from the endpoint buffer, and processing the data.  In this example, we have implemented a few very
    //simple commands.  For example, if the host sends a packet of data to the endpoint 1 OUT buffer, with the
    //first byte = 0x80, this is being used as a command to indicate that the firmware should "Toggle LED(s)".
    if(!USBHandleBusy(USBGenericInHandle))		//Check if the endpoint has received any data from the host.
    {
        //Des données sont arrivées dans le buffer de reception
        uint8_t length=USBHandleGetLength(USBGenericInHandle);
        //On les process
        USBDecodeMessage(INPacket,length );

        //Re-arm the IN endpoint for the next packet:
        //The USBGenRead() function call "arms" the endpoint (and makes it "busy").  If the endpoint is armed, the SIE will
        //automatically accept data from the host, if the host tries to send a packet of data to the endpoint.  Once a data
        //packet addressed to this endpoint is received from the host, the endpoint will no longer be busy, and the application
        //can read the data which will be sitting in the buffer.
        USBGenericInHandle = USBGenRead(USBGEN_EP_NUM,(uint8_t*)&INPacket,USBGEN_EP_SIZE);
        
    }
    
    
    #ifdef USE_USB_CIRCULAR_BUFFER
    //Now check to make sure no previous attempts to send data to the host are still pending.  If any attempts are still
    //pending, we do not want to write to the endpoint 1 IN buffer again, until the previous transaction is complete.
    //Otherwise the unsent data waiting in the buffer will get overwritten and will result in unexpected behavior.
    if(EP1OUTEvenNeedsServicingNext)
    {
        if(!USBHandleBusy(USBGenericOutEvenHandle))
        {
            if(USBIsDataReadyInTxBuffer())
            {
                //On a des datas pretes a etre envoyées
                int nbBytesToSend=USBGetNbOfValuesInTxBuffer();

                //The endpoint was not "busy", therefore it is safe to write to the buffer and arm the endpoint.
                if(nbBytesToSend<=USB_BUFFER_SIZE)
                {
                    //On copie les datas dans le buffer d'emission
                    int i;
                    for(i=0; i<nbBytesToSend; i++)
                        OUTEvenPacket[i]=USBGetOneFromTxBuffer();
                    //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                    //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                    //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                    //will indicate the the endpoint is no longer busy.
                    USBGenericOutEvenHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTEvenPacket,nbBytesToSend);
                    _LED_O1=!_LED_O1;
                }
                else
                {
                    if(!USBHandleBusy(USBGenericOutEvenHandle))
                    {
                        if(nbBytesToSend>USB_BUFFER_SIZE)
                        {
                            //On copie les datas dans le buffer d'emission
                            int i;
                            for(i=0;i<USB_BUFFER_SIZE;i++)
                                OUTEvenPacket[i]=USBGetOneFromTxBuffer();
                            //nbBytesToSend-=USB_BUFFER_SIZE;
                            //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                            //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                            //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                            //will indicate the the endpoint is no longer busy.
                            USBGenericOutEvenHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTEvenPacket,USB_BUFFER_SIZE);
                        }
                        else
                        {
                            //On copie les datas dans le buffer d'emission
                            int i;
                            for(i=0;i<nbBytesToSend;i++)
                                OUTEvenPacket[i]=USBGetOneFromTxBuffer();
                            //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                            //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                            //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                            //will indicate the the endpoint is no longer busy.
                            USBGenericOutEvenHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTEvenPacket,nbBytesToSend);
                            nbBytesToSend=0;
                        }
                        //_LED_O1=!_LED_O1;
                    }
                }
            }
            EP1OUTEvenNeedsServicingNext=false;
        }
        
    }
    else
    {
        if(!USBHandleBusy(USBGenericOutOddHandle))
        {
            if(USBIsDataReadyInTxBuffer())
            {
                //On a des datas pretes a etre envoyées
                int nbBytesToSend=USBGetNbOfValuesInTxBuffer();

                //The endpoint was not "busy", therefore it is safe to write to the buffer and arm the endpoint.
                if(nbBytesToSend<=USB_BUFFER_SIZE)
                {
                    //On copie les datas dans le buffer d'emission
                    int i;
                    for(i=0; i<nbBytesToSend; i++)
                        OUTOddPacket[i]=USBGetOneFromTxBuffer();
                    //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                    //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                    //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                    //will indicate the the endpoint is no longer busy.
                    USBGenericOutOddHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTOddPacket,nbBytesToSend);
                    _LED_O1=!_LED_O1;
                }
                else
                {
                    if(!USBHandleBusy(USBGenericOutOddHandle))
                    {
                        if(nbBytesToSend>USB_BUFFER_SIZE)
                        {
                            //On copie les datas dans le buffer d'emission
                            int i;
                            for(i=0;i<USB_BUFFER_SIZE;i++)
                                OUTOddPacket[i]=USBGetOneFromTxBuffer();
                            //nbBytesToSend-=USB_BUFFER_SIZE;
                            //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                            //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                            //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                            //will indicate the the endpoint is no longer busy.
                            USBGenericOutOddHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTOddPacket,USB_BUFFER_SIZE);
                        }
                        else
                        {
                            //On copie les datas dans le buffer d'emission
                            int i;
                            for(i=0;i<nbBytesToSend;i++)
                                OUTOddPacket[i]=USBGetOneFromTxBuffer();
                            //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                            //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                            //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                            //will indicate the the endpoint is no longer busy.
                            USBGenericOutOddHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTOddPacket,nbBytesToSend);
                            nbBytesToSend=0;
                        }
                        //_LED_O1=!_LED_O1;
                    }
                }
            }
            EP1OUTEvenNeedsServicingNext=true;
        }
    }
    #else

    #endif

}//end ProcessIO

volatile unsigned char noUSBOutput=0;
void SendMessageToUSB( unsigned short int payloadLength, unsigned char* payload)
{
    #ifdef USE_USB_CIRCULAR_BUFFER
    if(USBGetRemainingSpaceInTxBuffer()>payloadLength)
    {
        int i;
        for(i=0;i<payloadLength;i++)
        {
            USBWriteToTxBuffer(payload[i]);
        }
        noUSBOutput=0;
    }
    else
    {
        _LED_R2=!_LED_R2;
        noUSBOutput=1;
        flagError.USBBufferFull=1;
    }
    #else
    /* If the USB device isn't configured yet, we can't really do anything
     * else since we don't have a host to talk to.  So jump back to the
     * top of the while loop. */
    if( USBGetDeviceState() < CONFIGURED_STATE )
    {
        return;
    }

    /* If we are currently suspended, then we need to see if we need to
     * issue a remote wakeup.  In either case, we shouldn't process any
     * keyboard commands since we aren't currently communicating to the host
     * thus just continue back to the start of the while loop. */
    if( USBIsDeviceSuspended()== true )
    {
        return;
    }
    
    //Now check to make sure no previous attempts to send data to the host are still pending.  If any attempts are still
    //pending, we do not want to write to the endpoint 1 IN buffer again, until the previous transaction is complete.
    //Otherwise the unsent data waiting in the buffer will get overwritten and will result in unexpected behavior.
    if(!USBHandleBusy(USBGenericOutEvenHandle))
    {
        //The endpoint was not "busy", therefore it is safe to write to the buffer and arm the endpoint.
        if(payloadLength<=USBGEN_EP_SIZE)
        {
            //On copie les datas dans le buffer d'emission
            memcpy(OUTEvenPacket,payload,payloadLength);
            //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
            //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
            //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
            //will indicate the the endpoint is no longer busy.
            USBGenericOutEvenHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTEvenPacket,USBGEN_EP_SIZE);
        }
        else
        {
            int bytesToWrite=payloadLength;
            while(bytesToWrite>0)
            {
                if(bytesToWrite>USBGEN_EP_SIZE)
                {
                    //On copie les datas dans le buffer d'emission
                    memcpy(OUTEvenPacket,payload,bytesToWrite);
                    bytesToWrite-=USBGEN_EP_SIZE;
                    //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                    //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                    //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                    //will indicate the the endpoint is no longer busy.
                    USBGenericOutEvenHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTEvenPacket,USBGEN_EP_SIZE);
                }
                else
                {
                    //On copie les datas dans le buffer d'emission
                    memcpy(OUTEvenPacket,payload,bytesToWrite);
                    bytesToWrite=0;
                    //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                    //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                    //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                    //will indicate the the endpoint is no longer busy.
                    USBGenericOutEvenHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&OUTEvenPacket,bytesToWrite);
                }
            }
        }
    }
    else
    {
        _LED_R2=!_LED_R2;
    }
    #endif
}

void APP_LEDUpdateUSBStatus(void)
{
    static uint16_t ledCount = 0;

    if(USBIsDeviceSuspended() == true)
    {
        _LED_V1=0;
        return;
    }

    switch(USBGetDeviceState())
    {
        case CONFIGURED_STATE:
            /* We are configured.  Blink fast.
             * On for 75ms, off for 75ms, then reset/repeat. */
            if(ledCount == 1)
            {
                _LED_V1=1;
            }
            else if(ledCount == 75)
            {
                _LED_V1=0;
            }
            else if(ledCount > 150)
            {
                ledCount = 0;
            }
            break;

        default:
            /* We aren't configured yet, but we aren't suspended so let's blink with
             * a slow pulse. On for 50ms, then off for 950ms, then reset/repeat. */
            if(ledCount == 1)
            {
                _LED_V1=1;
            }
            else if(ledCount == 50)
            {
                _LED_V1=0;;
            }
            else if(ledCount > 950)
            {
                ledCount = 0;
            }
            break;
    }

    /* Increment the millisecond counter. */
    ledCount++;
}


