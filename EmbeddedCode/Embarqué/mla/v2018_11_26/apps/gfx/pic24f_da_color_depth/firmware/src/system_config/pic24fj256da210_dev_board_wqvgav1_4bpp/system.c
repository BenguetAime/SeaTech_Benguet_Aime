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
#include "driver/spi/drv_spi.h"
#include "driver/gfx/drv_gfx_display.h"
#include "driver/gfx/drv_gfx_da210.h"
#include "driver/nvm/drv_nvm_flash_spi_sst25vf016.h"
#include "driver/touch_screen/drv_touch_screen.h"

// *****************************************************************************
// Configuration bits
// *****************************************************************************
// PIC24FJ256DA210 Configuration Bit Settings

// CONFIG4

// CONFIG3
#pragma config WPFP = WPFP255           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config SOSCSEL = SOSC           // Secondary Oscillator Power Mode Select (Secondary oscillator is in Default (high drive strength) Oscillator mode)
#pragma config WUTSEL = LEG             // Voltage Regulator Wake-up Time Select (Default regulator start-up time is used)
#pragma config ALTPMP = ALTPMPEN        // Alternate PMP Pin Mapping (EPMP pins are in alternate location mode)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Segmented code protection is disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Last page (at the top of program memory) and Flash Configuration Words are not write-protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP)

// CONFIG2
#pragma config POSCMOD = XT             // Primary Oscillator Select (XT Oscillator mode is selected)
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable (The IOLOCK bit can be set and cleared as needed, provided the unlock sequence has been completed)
#pragma config OSCIOFNC = OFF           // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Fail-Safe Clock Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Initial Oscillator Select (Primary Oscillator with PLL module (XTPLL, HSPLL, ECPLL))
#pragma config PLL96MHZ = ON            // 96MHz PLL Startup Select (96 MHz PLL is enabled automatically on start-up)
#pragma config PLLDIV = DIV2            // 96 MHz PLL Prescaler Select (Oscillator input is divided by 2 (8 MHz input))
#pragma config IESO = OFF               // Internal External Switchover (IESO mode (Two-Speed Start-up) is disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config ALTVREF = ALTVREDIS      // Alternate VREF location Enable (VREF is on a default pin (VREF+ on RA9 and VREF- on RA10))
#pragma config WINDIS = OFF             // Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC2/PGED2)
#pragma config GWRP = OFF               // General Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)


// *****************************************************************************
// System Settings and Variables
// *****************************************************************************
const DRV_SPI_INIT_DATA SPI_Init_Data = {2, 3, 6, 0, SPI_BUS_MODE_3, 0};

volatile uint32_t  tick = 0, prevTick;     // tick counter,


// *****************************************************************************
/*  Function:
    void SYSTEM_BoardInitialize(void)

    Summary:
        This function initializes the system board modules and
        peripheral ports.

    Description:
        This function initializes the system board modules and
        peripheral ports that is specific to the PIC device
        used in this configuration.

*/
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
    // initialize the buttons on the PIC24FJ256DA210
    // Development Board (S1, S2, & S3)
    // ---------------------------------------------------------
    ANSGbits.ANSG8    = 0;  // set pin as digital pin
    ANSEbits.ANSE9    = 0;  // set pin as digital pin
    ANSBbits.ANSB5    = 0;  // set pin as digital pin
    TRISGbits.TRISG8  = 1;  // set as input
    TRISEbits.TRISE9  = 1;  // set as input
    TRISBbits.TRISB5  = 1;  // set as input

    // ---------------------------------------------------------
    // Timing Controller (TCON) pins 
    // ---------------------------------------------------------
    
    // TCON chip select pin
    TCON_CS_DIG();          // set as digital pin
    TCON_CS_TRIS = 0;       // set as output
    TCON_CS_LAT  = 1;       // set initial value as high

    // TCON data/command pin
    TCON_DC_DIG();          // set as digital pin
    TCON_DC_TRIS = 0;       // set as output
    TCON_DC_LAT  = 1;       // set initial value as high (data)

    // TCON spi clock pin
    TCON_SCL_DIG();         // set as digital pin
    TCON_SCL_TRIS = 0;      // set as output
    TCON_SCL_LAT  = 0;      // set initial value as high

    // TCON spi data out
    TCON_SDO_DIG();         // set as digital pin
    TCON_SDO_TRIS = 0;      // set as output
    TCON_SDO_LAT  = 1;      // set initial value as high

    // initialize the Timing Controller (TCON) driver
    // chip select signal
    TCON_CS_TRIS = 0;       // set as output
    TCON_CS_LAT  = 1;       // initialize to 1 (not selected)

    // ---------------------------------------------------------
    // Initialize the Display Driver
    // ---------------------------------------------------------
    DRV_GFX_Initialize();
    
    // ---------------------------------------------------------
    // PIC24FJ256DA210 Development Board - SPI-Flash Device pins 
    // ---------------------------------------------------------
    // chip select pin
    TRISAbits.TRISA14 = 0;  // set as output
    LATAbits.LATA14   = 1;  // initialize to 1 (not selected)
    // spi-clock pin
    TRISDbits.TRISD8  = 0;
    // spi-output pin
    TRISBbits.TRISB1  = 0;
    // spi-intput pin
    TRISBbits.TRISB0  = 1;

    ANSBbits.ANSB0    = 0;  // set pin as digital pin
    ANSBbits.ANSB1    = 0;  // set pin as digital pin

    // set the PPS pins
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPOR1bits.RP2R = 11;                    // assign RP2 for SCK2
    RPOR0bits.RP1R = 10;                    // assign RP1 for SDO2
    RPINR22bits.SDI2R = 0;                  // assign RP0 for SDI2
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS

    // initialize the Flash Memory driver
    DRV_NVM_SST25VF016_Initialize((DRV_SPI_INIT_DATA*)&SPI_Init_Data);

    // ---------------------------------------------------------
    // initialize system tick counter
    // ---------------------------------------------------------
    SYSTEM_TickInit();

    // ---------------------------------------------------------
    // initialize the components for Resistive Touch Screen
    // --------------------------------------------------------- 
    TouchInit(NVMWrite, NVMRead, NVMSectorErase, NULL);

}

// *****************************************************************************
/*  Function:
    __T3_ISR _T3Interrupt(void)

    Summary:
        This function initializes the interrupt used for the tick timer
        of the demo.

    Description:
        This function initializes the interrupt used for the tick timer
        of the demo. This is also used to sample the resistive touch
        screen module.

*/
// *****************************************************************************
#define __T3_ISR    __attribute__((interrupt, shadow, auto_psv))
void __T3_ISR _T3Interrupt(void)
{
    tick++;

    TMR3 = 0;
    // Clear flag
    IFS0bits.T3IF = 0;
    TouchDetectPosition();
}

/*********************************************************************
 * Section: Tick Delay
 *********************************************************************/
#define SAMPLE_PERIOD   500 // us
#define TICK_PERIOD     (SYS_CLK_FrequencyPeripheralGet() * SAMPLE_PERIOD) / 4000000

// *****************************************************************************
/*  Function:
    void SYSTEM_TickInit(void)

    Summary:
        Initializes the tick timer of the demo.

    Description:
        This function initializes the tick timer of the demo.

*/
// *****************************************************************************
void SYSTEM_TickInit(void)
{
    // Initialize Timer3
    TMR3 = 0;
    PR3 = TICK_PERIOD;
    IFS0bits.T3IF = 0;  //Clear flag
    IEC0bits.T3IE = 1;  //Enable interrupt
    T3CONbits.TON = 1;  //Run timer

}


