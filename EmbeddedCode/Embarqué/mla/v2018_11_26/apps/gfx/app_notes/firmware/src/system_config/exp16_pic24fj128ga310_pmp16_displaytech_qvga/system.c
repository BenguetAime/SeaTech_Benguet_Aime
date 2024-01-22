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
#include "driver/uart/drv_uart2.h"
#include "driver/spi/drv_spi.h"
#include "driver/gfx/drv_gfx_display.h"
#include "driver/gfx/drv_gfx_tft003.h"
#include "driver/nvm/nvm_flash_spi_sst26vf0xxb.h"
#include "driver/touch_screen/drv_touch_screen.h"
#include "memory_programmer/flash_programmer.h"


// *****************************************************************************
// Configuration bits
// *****************************************************************************
// PIC24FJ128GA310 Configuration Bit Settings
// CONFIG4
#pragma config DSWDTPS = DSWDTPS1F      // Deep Sleep Watchdog Timer Postscale Select bits (1:68719476736 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select (DSWDT uses LPRC as reference clock)
#pragma config DSBOREN = OFF            // Deep Sleep BOR Enable bit (DSBOR Disabled)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable (DSWDT Disabled)
#pragma config DSSWEN = ON              // DSEN Bit Enable (Deep Sleep is controlled by the register bit DSEN)

// CONFIG3
#pragma config WPFP = WPFP127           // Write Protection Flash Page Segment Boundary (Page 127 (0x1FC00))
#pragma config VBTBOR = OFF             // VBAT BOR enable bit (VBAT BOR disabled)
#pragma config SOSCSEL = OFF            // SOSC Selection bits (Digital (SCLKI) mode)
#pragma config WDTWIN = PS25_0          // Watch Dog Timer Window Width (Watch Dog Timer Window Width is 25 percent)
#pragma config BOREN = ON               // Brown-out Reset Enable (Brown-out Reset Enable)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Disabled)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Write Protect from WPFP to the last page of memory)

// CONFIG2
#pragma config POSCMD = XT              // Primary Oscillator Select (XT Oscillator Enabled)
#pragma config BOREN1 = EN              // BOR Override bit (BOR Enabled [When BOREN=1])
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable bit (The IOLOCK bit can be set and cleared using the unlock sequence)
#pragma config OSCIOFCN = OFF           // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSECMD           // Clock Switching and Fail-Safe Clock Monitor Configuration bits (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)
#pragma config FNOSC = PRIPLL           // Initial Oscillator Select (Primary Oscillator with PLL module (XTPLL,HSPLL, ECPLL))
#pragma config ALTVREF = DLT_AV_DLT_CV  // Alternate VREF/CVREF Pins Selection bit (Voltage reference input, ADC =RA9/RA10 Comparator =RA9,RA10)
#pragma config IESO = OFF               // Internal External Switchover (Disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler Select (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler Ratio Select (1:128)
#pragma config FWDTEN = WDT_DIS         // Watchdog Timer Enable (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WINDIS = OFF             // Windowed WDT Disable (Standard Watchdog Timer)
#pragma config ICS = PGx2               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC2/PGED2)
#pragma config LPCFG = OFF              // Low power regulator control (Disabled)
#pragma config GWRP = OFF               // General Segment Write Protect (Disabled)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (Disabled)

// *****************************************************************************
// System Settings and Variables
// *****************************************************************************
volatile uint32_t  tick = 0, prevTick;     // tick counter,

// *****************************************************************************
// void SYSTEM_BoardInitialize(void)
// *****************************************************************************
void SYSTEM_BoardInitialize(void)
{

    const DRV_SPI_INIT_DATA SPI_Init_Data = {1, 3, 6, 0, SPI_BUS_MODE_3, 0};
    
    uint8_t manufacturerID, deviceType, deviceID;
    uint16_t id;

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

    ANSEbits.ANSE4  = 0;        // PMP Data (PMD4)
    ANSEbits.ANSE5  = 0;        // PMP Data (PMD5)
    ANSEbits.ANSE6  = 0;        // PMP Data (PMD6)
    ANSEbits.ANSE7  = 0;        // PMP Data (PMD7)

    ANSDbits.ANSD6  = 0;        // PMP Data (PMD14)
    ANSDbits.ANSD7  = 0;        // PMP Data (PMD15)

    ANSBbits.ANSB4  = 0;
    ANSBbits.ANSB3  = 0;

    TRISDbits.TRISD13 = 1;

    // ---------------------------------------------------------
    // Explorer 16 Development Board MCHP25LC256 chip select signal,
    // even if not used must be driven to high so it does not
    // interfere with other SPI peripherals that uses the same SPI signals.
    // ---------------------------------------------------------
    TRISDbits.TRISD12 = 0;
    LATDbits.LATD12 = 1;

    // ---------------------------------------------------------
    // SPI-Flash Device pins
    // ---------------------------------------------------------
    // chip select pin
    TRISFbits.TRISF2 = 0;       //RF2 = chip select line when using AC243005-2 SPI/SQI external flash PICtail+ board.
    LATFbits.LATF2   = 1;       //Not selected = logic high.

    // spi-clock pin
    TRISDbits.TRISD8 = 0;
    // spi-output pin
    TRISFbits.TRISF8 = 0;
    // spi-intput pin
    TRISDbits.TRISD9 = 1;

    // ---------------------------------------------------------
    // UART pins
    // ---------------------------------------------------------
    // initialize the UART pins
    TRISFbits.TRISF5 = 0;
    TRISFbits.TRISF4 = 1;

    // unlock PPS
    __builtin_write_OSCCONL(OSCCON & 0xbf);

    // set UART pins
    RPINR19bits.U2RXR = 10; 	// assign RP10 to RX
    RPOR8bits.RP17R = 5;    	// assign RP17 to TX

    // set SPI pins
    RPOR1bits.RP2R = 0x08;      // RD8->SPI1:SCK1OUT
    RPINR20bits.SDI1R = 0x04;   // RD9->SPI1:SDI1
    RPOR7bits.RP15R = 0x07;     // RF8->SPI1:SDO1

    // lock   PPS
    __builtin_write_OSCCONL(OSCCON | 0x40);

    // ---------------------------------------------------------
    // Initialize the Display Driver
    // ---------------------------------------------------------
    DRV_GFX_Initialize();

    // initialize flash device
    NVM_SST26VF0XXB_Initialize((DRV_SPI_INIT_DATA*)&SPI_Init_Data);

    // check for the presence of the external flash
    NVM_SST26VF0XXB_JEDEC_ID_Check(&manufacturerID, &deviceType, &deviceID);
    
    // initialize system tick counter
    SYSTEM_TickInit();
    
    id = ((manufacturerID << 8) | deviceID);
    
    // if external flash does not exist, then do calibration always but
    // will not save the calibration data.
    switch(id)
    {
        // SST26VF064B
        case 0xBF43:
        // SST26VF032B
        case 0xBF42:
            // initialize the components for Resistive Touch Screen
            TouchInit(NVMWrite, NVMRead, NVMSectorErase, NULL);
            break;
        default:            
            // external flash does not exist
            TouchInit(NULL, NULL, NULL, NULL);
            break;
    }
    
    
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

// *****************************************************************************
/*  Function:
    void SYSTEM_ProgramExternalMemory(void)

    Summary:
        Routine that programs the external memory used by the
        application.

    Description:
        This function programs the external memory on the system.
        Use the UART as the module to receive data to program the
        external memory.

*/
// *****************************************************************************
void SYSTEM_ProgramExternalMemory()
{
    // start the external memory programming
    ProgramExternalMemory(DataRead, DataWrite, DataChipErase);

    // delay the reset to have time for the acknowledge data to
    // be sent to the host side
    __delay_ms(10);

}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_ExternalResourceCallback(
                                GFX_RESOURCE_HDR *pResource,
                                uint32_t offset,
                                uint16_t nCount,
                                void     *pBuffer)

    Summary:
        This function performs data fetch from external memory.

    Description:
        This function must be implemented in the application.
        The library will call this function each time when
        the external memory data will be required. The application
        must copy requested byte quantity into the buffer provided.
        Data start address in external memory is a sum of the address
        in GFX_RESOURCE_HDR structure and offset.

    Precondition:
        None.

    Parameters:
        pResource - Pointer to the external memory resource information.
        offset - offset of the data from the location of the resource
                 in external memory.
        nCount - Number of bytes to be transferred into the buffer.
        buffer - Pointer to the buffer that will hold the retrieved data.

    Returns:
        GFX_STATUS_SUCCESS when all the data was succesfully retrieved.
        GFX_STATUS_FAILURE when partial or no data was retrieved.

    Example:
        None.

*/
// *****************************************************************************
// If there are several memories in the system they can be selected by IDs.
// In this demo ID for memory chip installed on Graphics PICTail board is assumed to be 0.
#define SST39_MEMORY    0
/* */

GFX_STATUS GFX_ExternalResourceCallback(
                                GFX_RESOURCE_HDR *pResource,
                                uint32_t offset,
                                uint16_t nCount,
                                void     *pBuffer)
{
    uint32_t addr;

    // get the proper address
    switch (pResource->type)
    {
        case GFX_RESOURCE_FONT_EXTERNAL_NONE:
            addr = pResource->resource.font.location.extAddress;
            break;
        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE:
        case GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_NONE:
            addr = pResource->resource.image.location.extAddress;
            break;
        default:
            // type is incorrect
            return (GFX_STATUS_FAILURE);
    }
    addr += offset;

    if(pResource->ID == SST39_MEMORY)
    {
        // Read data requested into buffer provided
        NVMRead(addr, (uint8_t*)pBuffer, nCount);
    }

    return (GFX_STATUS_SUCCESS);
}


