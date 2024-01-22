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

#include <p24EP512GU810.h>
#include "system.h"
#include "usb_device.h"
#include "leds.h"
#include "buttons.h"

/** CONFIGURATION Bits **********************************************/

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Initial Oscillator Source Selection bits (Primary Oscillator (XT, HS, EC) with PLL)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select bits (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#pragma config BOREN = OFF              // Brown-out Reset (BOR) Detection Enable bit (BOR is disabled)
#pragma config ALTI2C1 = OFF            // Alternate I2C pins for I2C1 (SDA1/SCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = OFF            // Alternate I2C pins for I2C2 (SDA2/SCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Aux Flash may be written)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)

 
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
            ANSELA = 0x0000;
            ANSELB = 0x0000;
            ANSELC = 0x0000;
            ANSELD = 0x0000;
            ANSELE = 0x0000;
            ANSELG = 0x0000;

            // Configure the device PLL to obtain 60 MIPS operation. The crystal
            // frequency is 8MHz. Divide 8MHz by 2, multiply by 60 and divide by
            // 2. This results in Fosc of 120MHz. The CPU clock frequency is
            // Fcy = Fosc/2 = 60MHz. Wait for the Primary PLL to lock and then
            // configure the auxilliary PLL to provide 48MHz needed for USB
            // Operation.

            PLLFBD = 58;                        /* M  = 60  */
            CLKDIVbits.PLLPOST = 0;             /* N1 = 2   */
            CLKDIVbits.PLLPRE = 0;              /* N2 = 2   */
            OSCTUN = 0;

            /*	Initiate Clock Switch to Primary
             *	Oscillator with PLL (NOSC= 0x3)*/
            __builtin_write_OSCCONH(0x03);
            __builtin_write_OSCCONL(0x01);

            while (OSCCONbits.COSC != 0x3);

            // Configuring the auxiliary PLL, since the primary
            // oscillator provides the source clock to the auxiliary
            // PLL, the auxiliary oscillator is disabled. Note that
            // the AUX PLL is enabled. The input 8MHz clock is divided
            // by 2, multiplied by 24 and then divided by 2. Wait till
            // the AUX PLL locks.

            ACLKCON3 = 0x24C1;
            ACLKDIV3 = 0x7;

            ACLKCON3bits.ENAPLL = 1;
            while(ACLKCON3bits.APLLCK != 1);

            LED_Enable(LED_USB_DEVICE_STATE);
            BUTTON_Enable(BUTTON_DEVICE_AUDIO_MICROPHONE_SAMPLE);
            break;

        case SYSTEM_STATE_USB_SUSPEND:
            break;

        case SYSTEM_STATE_USB_RESUME:
            break;
    }
}

#if defined(USB_INTERRUPT)
void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
    USBDeviceTasks();
}
#endif
