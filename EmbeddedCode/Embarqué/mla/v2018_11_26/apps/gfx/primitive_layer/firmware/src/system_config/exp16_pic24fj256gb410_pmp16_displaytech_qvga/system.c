/*******************************************************************************
  System Specific Initializations

  Company:
    Microchip Technology Inc.

  File Name:
    system.c

  Summary:
    System level initializations for the specific Microchip Development Board used.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// Section: Includes
// *****************************************************************************
#include <xc.h>
#include <stdlib.h>
#include <system.h>
#include "driver/gfx/drv_gfx_display.h"

// *****************************************************************************
// Configuration bits
// *****************************************************************************
// PIC24FJ256GA410 Configuration Bit Settings
// FSEC
#pragma config BWRP = OFF               // Boot Segment Write Protect (Boot segment may be written)
#pragma config BSS = HI                 // Boot segment Protect (High Security)
#pragma config BSEN = ON                // Boot Segment Control bit (Boot Segment size determined by FBSLIM)
#pragma config GWRP = ON                // General Segment Write Protect (General segment is write-protected)
#pragma config GSS = HI                 // General Segment Code Protect (High Security)
#pragma config CWRP = ON                // Configuration Segment Program Write Protection bit (Configuration Segment is write protected)
#pragma config CSS = HI                 // Configuration Segment Code Protection Level bits (High Security)
#pragma config AIVTDIS = DISABLE        // Alternate Interrupt Vector Table Disable bit (Disable AIVT)

// FBSLIM
// BSLIM = No Setting

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (XTPLL, HSPLL, ECPLL))
#pragma config PLLMODE = PLL96DIV2      // Frequency Multiplier Select Bits (96 MHz PLL. Oscillator input is divided by 2 (8 MHz input).)
#pragma config IESO = ON                // Internal External Switchover (Start up device with FRC, then switch to user-selected oscillator source)

// FOSC
#pragma config POSCMOD = XT             // Primary Oscillator Select (XT oscillator mode selected)
#pragma config OSCIOFCN = OFF           // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config SOSCSEL = OFF            // SOSC Power Selection Configuration bits (Digital (SCLKI) mode)
#pragma config PLLSS = PLL_FRC          // PLL Secondary Selection Configuration bit (PLL is fed by the on-chip Fast RC (FRC) oscillator)
#pragma config IOL1WAY = ON             // IOLOCK One-Way Set Enable (Once set the IOLOCK bit cannot be cleared)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS8              // Watchdog Timer Postscaler (1:8)
#pragma config FWPSA = PR32             // WDT Prescaler (Prescaler ratio of 1:32)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config WINDIS = ON              // Windowed Watchdog Timer Disable bit (Windowed WDT enabled)
#pragma config WDTWIN = PS75_0          // Watchdog Window Select bits (Watch Dog Timer Window Width is 75 percent)
#pragma config WDTCMX = LPRC            // WDT Clock Source Select bits (WDT always uses LPRC as its clock source)
#pragma config WDTCLK = SYSCLK          // WDT Clock Source Select bits (WDT uses system clock when active, LPRC while in Sleep mode)

// FPOR
#pragma config BOREN = ON               // Brown-out Reset Enable bits (Brown-out Reset Enable)
#pragma config LPCFG = ON               // Low power regulator control (Enabled)

// FICD
#pragma config ICS = PGx2               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC2/PGED2)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)
#pragma config BTSWP = ON               // BOOTSWP Instruction Enable bit (BOOTSWP instruction is enabled)

// FDS
#pragma config DSWDTPS = DSWDTPS03      // Deep Sleep Watchdog Timer Postscale Select bits (1:256 (8.3 ms))
#pragma config DSWDTOSC = SOSC          // DSWDT Reference Clock Select bit (DSWDT uses Secondary Oscillator (SOSC))
#pragma config DSBOREN = OFF            // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR disabled in Deep Sleep)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)

// FDEVOPT1
#pragma config ALTCMPI = DISABLE        // Alternate Comparator Input Enable bit (C1INC, C2INC, and C3INC are on their standard pin locations)
#pragma config TMPRPIN = ON             // Tamper Pin Enable bit (TMPRN pin function is enabled)
#pragma config TMPRWIPE = ON            // RAM Based Entryption Key Wipe Enable bit (Cryptographic Engine Key RAM is erased when a TMPR pin event is detected)
#pragma config ALTVREF = ALTVREFEN      // Alternate VREF location Enable (VREF is on an alternate pin (VREF+ on RB0 and VREF- on RB1))

// *****************************************************************************
// void SYSTEM_BoardInitialize(void)
// *****************************************************************************
void SYSTEM_BoardInitialize(void)
{

    // ---------------------------------------------------------
    // Make sure the display DO NOT flicker at start up
    // ---------------------------------------------------------
    DisplayBacklightConfig();
    DisplayPowerConfig();
    DisplayBacklightOff();

    // ---------------------------------------------------------
    // ADC Explorer 16 Development Board Errata (work around 2)
    // RB15 should be output
    // ---------------------------------------------------------
    LATBbits.LATB15 = 0;
    TRISBbits.TRISB15 = 0;

    // set pins with analog features to be digital
    ANSAbits.ANSA7  = 0;        // display reset pin
    ANSAbits.ANSA6  = 0;        // display RS pin
    ANSDbits.ANSD11 = 0;        // display CS pin

    ANSEbits.ANSE0  = 0;        // PMP Data (PMD0)
    ANSEbits.ANSE1  = 0;        // PMP Data (PMD1)
    ANSEbits.ANSE2  = 0;        // PMP Data (PMD2)
    ANSEbits.ANSE3  = 0;        // PMP Data (PMD3)
    ANSEbits.ANSE4  = 0;        // PMP Data (PMD4)
    ANSEbits.ANSE5  = 0;        // PMP Data (PMD5)
    ANSEbits.ANSE6  = 0;        // PMP Data (PMD6)
    ANSEbits.ANSE7  = 0;        // PMP Data (PMD7)

    ANSGbits.ANSG0  = 0;        // PMP Data (PMD8)
    ANSGbits.ANSG1  = 0;        // PMP Data (PMD9)
    ANSFbits.ANSF1  = 0;        // PMP Data (PMD10)
    ANSFbits.ANSF0  = 0;        // PMP Data (PMD11)
    ANSDbits.ANSD12 = 0;        // PMP Data (PMD12)
    ANSDbits.ANSD13 = 0;        // PMP Data (PMD13)
    ANSDbits.ANSD6  = 0;        // PMP Data (PMD14)
    ANSDbits.ANSD7  = 0;        // PMP Data (PMD15)

    ANSFbits.ANSF2  = 0;        // SPI chip select
    ANSBbits.ANSB14 = 0;        // SPI clock
    ANSCbits.ANSC3  = 0;        // SPI data input
    ANSFbits.ANSF8  = 0;        // SPI data output
    
    ANSBbits.ANSB0  = 0;        // Resistive Touch X+
    ANSBbits.ANSB1  = 0;        // Resistive Touch Y+ 
    ANSBbits.ANSB3  = 0;        // Resistive Touch Y- 
    ANSDbits.ANSD8  = 0;        // Resistive Touch X-

    ANSFbits.ANSF4  = 0;
    ANSFbits.ANSF5  = 0;
    
    ANSGbits.ANSG9  = 0;        // 
    TRISGbits.TRISG9 = 0;
    LATGbits.LATG9 = 1;
    
    
    // ---------------------------------------------------------
    // Explorer 16 Development Board MCHP25LC256 chip select signal,
    // even if not used must be driven to high so it does not
    // interfere with other SPI peripherals that uses the same SPI signals.
    // ---------------------------------------------------------
    TRISDbits.TRISD12 = 0;
    LATDbits.LATD12 = 1;
    
    // ---------------------------------------------------------
    // Initialize the Display Driver
    // ---------------------------------------------------------
    DRV_GFX_Initialize();

}


