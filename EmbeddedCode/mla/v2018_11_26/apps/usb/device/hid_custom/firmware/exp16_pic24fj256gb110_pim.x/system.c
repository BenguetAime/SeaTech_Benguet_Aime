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

#include <xc.h>
#include "usb_config.h"
#include "system.h"
#include "usb.h"
#include "leds.h"
#include "buttons.h"
#include "adc.h"

/** CONFIGURATION Bits **********************************************/
// CONFIG3
#pragma config WPFP = WPFP511           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable bit (Segmented code protection disabled)
#pragma config WPCFG = WPCFGDIS         // Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS oscillator mode selected)
#pragma config DISUVREG = OFF           // Internal USB 3.3V Regulator Disable bit (Regulator is disabled)
#pragma config IOL1WAY = ON             // IOLOCK One-Way Set Enable bit (Write RP Registers Once)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSCO functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary oscillator (XT, HS, EC) with PLL module (XTPLL,HSPLL, ECPLL))
#pragma config PLL_96MHZ = ON           // 96MHz PLL Disable (Enabled)
#pragma config PLLDIV = DIV2            // USB 96 MHz PLL Prescaler Select bits (Oscillator input divided by 2 (8MHz input))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-speed start-up)disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator functions are shared with PGEC2/PGED2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)
 
/*********************************************************************
* Function: void SYSTEM_Initialize( SYSTEM_STATE state )
*
* Overview: Initializes the system.
*
* PreCondition: None
*
* Input:  SYSTEM_STATE - the state to initialize the system into
*
* Output: None
*
********************************************************************/
void SYSTEM_Initialize( SYSTEM_STATE state )
{
    switch(state)
    {
        case SYSTEM_STATE_USB_START:
            LED_Enable(LED_USB_DEVICE_STATE);
            LED_Enable(LED_USB_DEVICE_HID_CUSTOM);
            
            BUTTON_Enable(BUTTON_USB_DEVICE_HID_CUSTOM);
            
            //Some Explorer 16 boards have a 5V LCD module installed on them, while others have a 3.3V LCD.
            //On the 5V LCD boards, the modules have 5V TTL inputs which can weakly pull connected (but tri-stated) I/O
            //pins above the datasheet max voltage specification (VDD+300mV) for non-5V tolerant I/O pins.  This can potentially
            //cause analog related problems as described in http://ww1.microchip.com/downloads/en/AppNotes/93013a.pdf 
            //To avoid this, the non-5V tolerant pins going to the LCD module can be driven as digital
            //outputs, thereby keeping them within the datasheet range.
            LATBbits.LATB15 = 0;        //Okay to remove this if using an Explorer 16 with 3.3V LCD module
            TRISBbits.TRISB15 = 0;      //Okay to remove this if using an Explorer 16 with 3.3V LCD module

            ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
            ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
            break;
            
        case SYSTEM_STATE_USB_SUSPEND:
            //If developing a bus powered USB device that needs to be USB compliant,
            //insert code here to reduce the I/O pin and microcontroller power consumption,
            //so that the total current is <2.5mA from the USB host's VBUS supply.
            //If developing a self powered application (or a bus powered device where
            //official USB compliance isn't critical), nothing strictly needs
            //to be done during USB suspend.

            USBSleepOnSuspend();
            break;

        case SYSTEM_STATE_USB_RESUME:
            //Here would be a good place to restore I/O pins and application states
            //back to their original values that may have been saved/changed at the 
            //start of the suspend event (ex: when the SYSTEM_Initialize(SYSTEM_STATE_USB_SUSPEND)
            //callback was called.
            
            //If you didn't change any I/O pin states prior to entry into suspend,
            //then nothing explicitly needs to be done here.  However, by the time
            //the firmware returns from this function, the full application should
            //be restored into effectively exactly the same state as the application
            //was in, prior to entering USB suspend.
            
            //Additionally, before returning from this function, make sure the microcontroller's
            //currently active clock settings are compatible with USB operation, including
            //taking into consideration all possible microcontroller features/effects
            //that can affect the oscillator settings (such as internal/external 
            //switchover (two speed start up), fail-safe clock monitor, PLL lock time,
            //external crystal/resonator startup time (if using a crystal/resonator),
            //etc.

            //Additionally, the official USB specifications dictate that USB devices
            //must become fully operational and ready for new host requests/normal 
            //USB communication after a 10ms "resume recovery interval" has elapsed.
            //In order to meet this timing requirement and avoid possible issues,
            //make sure that all necessary oscillator startup is complete and this
            //function has returned within less than this 10ms interval.

            break;

        default:
            break;
    }
}

#if defined(USB_INTERRUPT)
void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
    USBDeviceTasks();
}
#endif


        
