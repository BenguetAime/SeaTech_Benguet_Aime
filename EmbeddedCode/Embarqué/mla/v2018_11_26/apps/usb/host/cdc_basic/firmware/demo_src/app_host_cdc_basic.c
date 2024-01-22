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

#include "system.h"
#include "usb.h"
#include "usb_host_cdc.h"
#include "usb_host_cdc_interface.h"
#include <string.h>
#include <stdio.h>

// *****************************************************************************
// *****************************************************************************
// Internal Function Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************
#define MAX_NO_OF_IN_BYTES  64
#define MAX_NO_OF_OUT_BYTES 64

// *****************************************************************************
// *****************************************************************************
// Type definitions
// *****************************************************************************
// *****************************************************************************
typedef enum _APP_STATE
{
    DEVICE_NOT_CONNECTED,
    GET_IN_DATA,
    GET_IN_DATA_WAIT,
    ERROR_REPORTED      /* need to add application states for data interface */
} APP_STATE;

// *****************************************************************************
// *****************************************************************************
// Local Variables
// *****************************************************************************
// *****************************************************************************
volatile APP_STATE  demoState;

uint8_t inBuffer[MAX_NO_OF_IN_BYTES];
uint8_t outBuffer[MAX_NO_OF_OUT_BYTES];

bool buttonReleased;

/*********************************************************************
* Function: void APP_HostCDCBasicInitialize(void);
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
void APP_HostCDCBasicInitialize()
{
    demoState = DEVICE_NOT_CONNECTED;
    PRINT_ClearScreen();
    buttonReleased = true;
}

/*********************************************************************
* Function: void APP_HostCDCBasicTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized via
*   the APP_HostCDCBasicInitialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostCDCBasicTasks()
{
    uint8_t errorDriver;
    uint8_t numOfBytesRcvd;

    numOfBytesRcvd = 0;

    if(!USBHostCDC_ApiDeviceDetect()) /* true if device is enumerated without error */
    {
        LED_Off(LED_USB_HOST_CDC_BASIC_DEVICE_ATTACHED);
        demoState = DEVICE_NOT_CONNECTED;
    }

    switch(demoState)
    {
        case DEVICE_NOT_CONNECTED:
            if(USBHostCDC_ApiDeviceDetect()) /* true if device is enumerated without error */
            {
                demoState = GET_IN_DATA;
                LED_On(LED_USB_HOST_CDC_BASIC_DEVICE_ATTACHED);
            }
            break;

        case GET_IN_DATA:
            if(USBHostCDC_Api_Get_IN_Data(MAX_NO_OF_IN_BYTES, inBuffer))
            {
                demoState = GET_IN_DATA_WAIT;
            }
            break;

        case GET_IN_DATA_WAIT:
            if(USBHostCDC_ApiTransferIsComplete(&errorDriver, &numOfBytesRcvd))
            {   /* check for error */
                if( (errorDriver == USB_SUCCESS) &&
                    (numOfBytesRcvd != 0) )
                {
                    PRINT_String((char*)inBuffer, numOfBytesRcvd);
                }

                demoState = GET_IN_DATA;
            }
            break;
            
        default:
            break;
    }
}


/*********************************************************************
* Function: bool APP_HostCDCBasicEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size );
*
* Overview: Handles USB events for the USB host CDC demo
*
* PreCondition: None
*
* Input: uint8_t - address of the device causing the event
*        USB_EVENT - the event that occurred
*        void* - data associated with the event
*        uint32_t - size of the data for the event
*
* Output: None
*
********************************************************************/
bool APP_HostCDCBasicEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
{
    switch((int)event)
    {
        case EVENT_CDC_ATTACH:
            SYSTEM_Initialize(SYSTEM_STATE_USB_HOST_CDC_BASIC);
            return true;
            
        case EVENT_CDC_NONE:
        case EVENT_CDC_COMM_READ_DONE:
        case EVENT_CDC_COMM_WRITE_DONE:
        case EVENT_CDC_DATA_READ_DONE:
        case EVENT_CDC_DATA_WRITE_DONE:
        case EVENT_CDC_RESET:
        case EVENT_CDC_NAK_TIMEOUT:
            return true;
            break;
    }

    return false;
}


