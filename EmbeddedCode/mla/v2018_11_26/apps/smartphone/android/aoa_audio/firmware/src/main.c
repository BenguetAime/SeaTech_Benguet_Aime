/*******************************************************************************
Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the "Company") for its PICmicro(R) Microcontroller is intended and
supplied to you, the Company's customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*******************************************************************************/

// Include files
#include <stdbool.h>
#include <stdint.h>

#include <adc.h>

#include <system.h>

#include <usb.h>
#include <usb_host_android.h>
#include <usb_host_audio_v1.h>

#include <app_android_audio.h>

// If a maximum current rating hasn't been defined, then define 500mA by default
#ifndef MAX_ALLOWED_CURRENT
    #define MAX_ALLOWED_CURRENT             (500)         // Maximum power we can supply in mA
#endif

/****************************************************************************
  Function:
    int main(void)

  Summary:
    main function

  Description:
    main function

  Precondition:
    None

  Parameters:
    None

  Return Values:
    int - exit code for main function

  Remarks:
    None
  ***************************************************************************/
int main(void)
{
    SYSTEM_Initialize(SYSTEM_STATE_DEFAULT);

    USBInitialize(0);
    APP_AndroidAudioInitialize();

    while(1)
    {
        //Keep the USB stack running
        USBTasks();

        APP_AndroidAudioTasks();

    } //while(1) main loop
}



/****************************************************************************
  Function:
    bool USB_ApplicationDataEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )

  Summary:
    Handles USB data application events

  Description:
    Handles USB data application events

  Precondition:
    None

  Parameters:
    uint8_t address - address of the device causing the event
    USB_EVENT event - the event that has occurred
    void* data - data associated with the event
    uint32_t size - the size of the data in the data field

  Return Values:
    bool - Return true of the event was processed.  Return false if the event
           wasn't handled.

  Remarks:
    None
  ***************************************************************************/
bool USB_ApplicationDataEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
{
    switch((int)event)
    {
        case EVENT_AUDIO_STREAM_RECEIVED:
            return true;

        default:
            break;
    }

    return false;
}


/****************************************************************************
  Function:
    bool USB_ApplicationEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )

  Summary:
    Handles USB application events

  Description:
    Handles USB application events

  Precondition:
    None

  Parameters:
    uint8_t address - address of the device causing the event
    USB_EVENT event - the event that has occurred
    void* data - data associated with the event
    uint32_t size - the size of the data in the data field

  Return Values:
    bool - Return true of the event was processed.  Return false if the event
           wasn't handled.

  Remarks:
    None
  ***************************************************************************/
bool USB_ApplicationEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
{
    switch( (int)event )
    {
        case EVENT_VBUS_REQUEST_POWER:
            // The data pointer points to a uint8_t that represents the amount of power
            // requested in mA, divided by two.  If the device wants too much power,
            // we reject it.
            if (((USB_VBUS_POWER_EVENT_DATA*)data)->current <= (MAX_ALLOWED_CURRENT / 2))
            {
                return true;
            }
            else
            {
            }
            break;

        case EVENT_VBUS_RELEASE_POWER:
        case EVENT_HUB_ATTACH:
        case EVENT_UNSUPPORTED_DEVICE:
        case EVENT_CANNOT_ENUMERATE:
        case EVENT_CLIENT_INIT_ERROR:
        case EVENT_OUT_OF_MEMORY:
        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
        case EVENT_DETACH:                   // USB cable has been detached (data: uint8_t, address of device)
        case EVENT_ANDROID_DETACH:
        case EVENT_AUDIO_ATTACH:
        case EVENT_AUDIO_INTERFACE_SET:
        case EVENT_AUDIO_DETACH:
        case EVENT_ANDROID_HID_REGISTRATION_COMPLETE:
        case EVENT_ANDROID_HID_SEND_EVENT_COMPLETE:
        case EVENT_ANDROID_ATTACH:
            return APP_AndroidAudioEventHandler(address, event, data, size);

        default:
            break;
    }
    return false;
}



