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
#include "system.h"
#include "usb.h"
#include "leds.h"
#include "buttons.h"


//------------------------------------------------------------------------------
//Example USB compatible oscillator settings.  Uncomment only one of the below.
//------------------------------------------------------------------------------
//#define USB_RUN_FROM_FRCPLL_WITH_ACTIVE_CLOCK_TUNING
#define USB_RUN_FROM_EXPLORER_16_8MHZ_CRYSTAL



/** CONFIGURATION Bits **********************************************/
// FSEC
#pragma config BWRP = OFF               // Boot Segment Write Protect (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write Protect (Writes to program memory are allowed)
#pragma config GSS = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config CWRP = OFF               // Configuration Segment Program Write Protection bit (Configuration Segment may be written)
#pragma config CSS = DIS                // Configuration Segment Code Protection Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = DISABLE        // Alternate Interrupt Vector Table Disable bit (Disable AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Code Flash Page Address Limit bits (Boot Segment Flash page address limit)


// FOSCSEL and FOSC
#if defined(USB_RUN_FROM_EXPLORER_16_8MHZ_CRYSTAL)
    #pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (XTPLL, HSPLL, ECPLL))
    #pragma config PLLMODE = PLL96DIV2      // Frequency Multiplier Select Bits (96 MHz PLL. Oscillator input is divided by 2 (8 MHz input).)
    #pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
    #pragma config PLLSS = PLL_PRI          // PLL Secondary Selection Configuration bit (PLL is fed by the Primary oscillator)
#else
    #pragma config FNOSC = FRCPLL           // Oscillator Select (FRC with postscalar with PLL module)
    #pragma config PLLMODE = PLL96DIV1      // Frequency Multiplier Select Bits (96 MHz PLL. Oscillator input is divided by 1 (4 MHz input expected - Note: The 8MHz FRC postscalar [OSCCON<RCDIV>] must be maintained at the 1:2 setting (to generate 4MHz for the PLL), which is the POR default).)
    #pragma config POSCMOD = NONE           // Primary Oscillator Select (disabled - using FRCPLL instead)
    #pragma config PLLSS = PLL_FRC          // PLL Secondary Selection Configuration bit (PLL is fed by the FRC)
#endif
#pragma config IESO = OFF               // Internal External Switchover (Start up with user-selected oscillator source)
#pragma config OSCIOFCN = ON            // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as port I/O (RC15))
#pragma config SOSCSEL = ON             // SOSC Power Selection Configuration bits (SOSC is used in crystal (SOSCI/SOSCO) mode)
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable (The IOLOCK bit can be set and cleared using the unlock sequence)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching enabled, Fail-Safe Clock Monitor disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard Watchdog Timer enabled (Windowed-mode is disabled))
#pragma config WDTWIN = PS25_0          // Watchdog Window Select bits (Watch Dog Timer Window Width is 25 percent)
#pragma config WDTCMX = WDTCLK          // WDT Clock Source Select bits (WDT clock source is determined by the WDTCLK Configuration bits)
#pragma config WDTCLK = LPRC            // WDT Clock Source Select bits (WDT uses LPRC)

// FPOR
#pragma config BOREN = ON               // Brown-out Reset Enable bits (Brown-out Reset Enabled)
#pragma config LPCFG = OFF              // Low power regulator control (Disabled)

// FICD
#pragma config ICS = PGx1               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC1/PGED1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)
#pragma config BTSWP = OFF              // BOOTSWP Instruction Enable bit (BOOTSWP instruction is disabled)

// FDS
#pragma config DSWDTPS = DSWDTPS1F      // Deep Sleep Watchdog Timer Postscale Select bits (1:68,719,476,736 (25.7 days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses Low Power RC Oscillator (LPRC))
#pragma config DSBOREN = OFF            // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR disabled in Deep Sleep)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)

// FDEVOPT1
#pragma config ALTCMPI = DISABLE        // Alternate Comparator Input Enable bit (C1INC, C2INC, and C3INC are on their standard pin locations)
#pragma config TMPRPIN = OFF            // Tamper Pin Enable bit (TMPRN pin function is disabled)
#pragma config TMPRWIPE = OFF           // RAM Based Entryption Key Wipe Enable bit (Cryptographic Engine Key RAM is not erased onTMPR pin events)
#pragma config ALTVREF = ALTVREFDIS     // Alternate VREF location Enable (VREF is on a default pin (VREF+ on RA10 and VREF- on RA9))
 
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
            //Make sure that the general purpose output driver multiplexed with
            //the VBUS pin is always consistently configured to be tri-stated in
            //USB applications, so as to avoid any possible contention with the host.
            //(ex: maintain TRISFbits.TRISF7 = 1 at all times).
            TRISFbits.TRISF7 = 1;

            //Configure clock settings to be compatible with USB operation.  
            //This could involve doing nothing (ex: if the config bit settings
            //already result in a USB compatible clock frequency at bootup), or,
            //could involve clock switching and/or turning on the PLL and/or turning 
            //on the active clock tuning feature.
 
            #if defined(USB_RUN_FROM_EXPLORER_16_8MHZ_CRYSTAL)
                //Don't need to do anything, with the default config bit settings for this 
                //oscillator setting, since the settings will already be USB compatible.
            #else
                //Assume USB_RUN_FROM_FRCPLL_WITH_ACTIVE_CLOCK_TUNING is selected instead.
                CLKDIV = 0x3120;    //Set FRC to 4MHz output mode, and CPUDIV to 1:1 (which is a USB compatible setting)
               __builtin_write_OSCCONH(0x01);  //0x01 = FRCPLL with postscalar
               __builtin_write_OSCCONL(OSCCON | 0x0001);   //Set the OSWEN bit, to initiate clock switching operation.
               //Wait for PLL lock
               while(OSCCONbits.LOCK == 0);
               //Turn on the active clock tuning for USB operation
               OSCTUN = 0x9000;        //Enable active clock tuning from USB host reference
               //The oscillator settings should now be USB compatible.           
            #endif
            
            //Configure other USB or application related resources.
            LED_Enable(LED_USB_DEVICE_STATE);
            LED_Enable(LED_DEVICE_VENDOR_BASIC_DEMO);
            BUTTON_Enable(BUTTON_DEVICE_VENDOR_BASIC_DEMO);
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


        
