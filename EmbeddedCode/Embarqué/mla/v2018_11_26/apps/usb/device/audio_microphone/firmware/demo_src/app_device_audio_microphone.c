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
#include <stdint.h>
#include <stdbool.h>

#include "system.h"

#include "usb.h"
#include "usb_device_audio.h"

/* This audio sample is a 200Hz sin wave, sampled at 8000Hz, with a 10-bit
 * resolution.  The audio input is specified in this example to be a 16-bit 
 * microphone, but during testing on various OSes showed that some systems have
 * trouble receiving more than a 10-bit input without causing distortion.
 *
 * The data below is generated via the following formula:
 *   audioSamples[x] = 1024 * sine(2 * pi * (x+1)/8000Hz/200Hz) for x=0:39
 * x=0:39 because there are 40 samples required for one full cycle of the sine
 * wave (8000Hz/200Hz = exactly 40 samples/cycle).
 */
const int16_t audioSamples[] =
{
    160,
    316,
    465,
    602,
    724,
    828,
    912,
    974,
    1011,
    1024,
    1011,
    974,
    912,
    828,
    724,
    602,
    465,
    316,
    160,
    0,
    -160,
    -316,
    -465,
    -602,
    -724,
    -828,
    -912,
    -974,
    -1011,
    -1024,
    -1011,
    -974,
    -912,
    -828,
    -724,
    -602,
    -465,
    -316,
    -160,
    0,
};

/** VARIABLES ******************************************************/
/* Some processors have a limited range of RAM addresses where the USB module
 * is able to access.  The following section is for those devices.  This section
 * assigns the buffers that need to be used by the USB module into those
 * specific areas.
 */
#if defined(FIXED_ADDRESS_MEMORY)
    #if defined(COMPILER_MPLAB_C18)
        #pragma udata DEVICE_AUDIO_MICROPHONE_DATA_BUFFER=DEVCE_AUDIO_MICROPHONE_DATA_BUFFER_ADDRESS
            static int16_t FrameData[8];
        #pragma udata
    #elif defined(__XC8)
        static int16_t FrameData[8] DEVCE_AUDIO_MICROPHONE_DATA_BUFFER_ADDRESS;
    #endif
#else
    static int16_t FrameData[8];
#endif
    
static USB_HANDLE USBTxHandle;    //USB handle.  Must be initialized to 0 at startup.
static unsigned long currentSample;

/*********************************************************************
* Function: void APP_DeviceAudioMicrophoneInitialize(void);
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
void APP_DeviceAudioMicrophoneInitialize()
{
    USBTxHandle = NULL;

    currentSample = 0;

    //enable the Audio Streaming(Isochronous) endpoint
    USBEnableEndpoint(USB_DEVICE_MICROPHONE_DEMO_ENDPOINT , USB_IN_ENABLED | USB_DISALLOW_SETUP);
}

/*********************************************************************
* Function: void APP_DeviceAudioMicrophoneInitialize(void);
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
void APP_DeviceAudioMicrophoneSOFHandler()
{
    
}

/*********************************************************************
* Function: void APP_DeviceAudioMicrophoneTasks(void);
*
* Overview: Keeps the Custom HID demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceAudioMicrophoneInitialize() and APP_DeviceAudioMicrophoneStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceAudioMicrophoneTasks()
{
    unsigned char frameCounter;
    
    /* If the device is not configured yet, or the device is suspended, then
     * we don't need to run the demo since we can't send any data.
     */
    if( (USBGetDeviceState() < CONFIGURED_STATE) ||
        (USBIsDeviceSuspended() == true))
    {
        return;
    }
    
    /* If the button is held down, play the audio file or continue playing the
     * audio file.  Stop playing the file when the button is released.
     */
    if(BUTTON_IsPressed(BUTTON_DEVICE_AUDIO_MICROPHONE_SAMPLE) == true)
    {
        /* if we aren't already busy sending data. */
        if(!USBHandleBusy(USBTxHandle))
        {
            /* then for each of the entries in the FrameData buffer... */
            for(frameCounter = 0; frameCounter < sizeof(FrameData)/sizeof(int16_t); frameCounter++)
            {
                /* copy the next sample from the audioSamples array. */
                FrameData[frameCounter] = audioSamples[currentSample++];

                /* If we have reached the end of the audioSamples array, jump
                 * back to the start in this case so that we keep playing the
                 * same file/tone continuously as long as the button is held.
                 */
                if( currentSample >= (sizeof(audioSamples)/sizeof(int16_t)) )
                {
                    currentSample = 0;
                }
            }

            /* actually send the data now. */
            USBTxHandle = USBTxOnePacket(USB_DEVICE_MICROPHONE_DEMO_ENDPOINT , (uint8_t*)FrameData, sizeof(FrameData));
        }
    }
    else
    {
        currentSample = 0;
    }
}
