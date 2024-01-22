/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
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
 *******************************************************************/

#include "system_config.h"
#include "system.h"
#include <xc.h>
#include <stdbool.h>
#include <sd_spi.h>
#include "drv_spi_config.h"

/** CONFIGURATION Bits **********************************************/
#pragma config GWRP = OFF
#pragma config GSS = OFF
#pragma config GSSK = OFF
#pragma config FNOSC = PRIPLL
#pragma config IESO = OFF
#pragma config POSCMD = HS
#pragma config OSCIOFNC = OFF
#pragma config IOL1WAY = ON
#pragma config FCKSM = CSDCMD
#pragma config FWDTEN = OFF
#pragma config ICS = PGD1
#pragma config JTAGEN = OFF

void SYSTEM_Initialize (void)
{
    
}

#if defined(DRV_SPI_CONFIG_CHANNEL_1_ENABLE)
/*==============================================================================
 * SPI 1
 * 
 * Hardware Configuration: Pin 1 of J3 must be shorted to pin 1 of JP8 for this 
 * configuration to work.  This jumping can be done by shorting pins 1 and 3 of 
 * J3 and 1 and 2 of J8 or by a direct jump.
 * 
 * On the SD-card PICTail+ board
 *  1) Short pins 1 and 2 on JP1
 *  2) Short pins 1 and 2 on JP2
 *  3) Short pins 1 and 2 on JP3
 *============================================================================*/ 
void USER_SdSpiConfigurePins (void)
{
    ANSELBbits.ANSB1 = 0;
    TRISGbits.TRISG15 = 1;
    
    //Initialize the SPI
    RPINR20bits.SDI1R = 127;    //Set SDI1 to RP127/RG15
    RPOR0bits.RP64R = 6;        //Set SCK1 to RP64/RD0
    RPOR11bits.RP104R = 5;      //Set SDO1 to RP104/RF8
        
    //enable a pull-up for the card detect, just in case the SD-Card isn't attached
    //  then lets have a pull-up to make sure we don't think it is there.
    CNPUFbits.CNPUF0 = 1;
    
    // Deassert the chip select pin
    LATBbits.LATB1 = 1;
    // Configure CS pin as an output
    TRISBbits.TRISB1 = 0;
    // Configure CD pin as an input
    TRISFbits.TRISF0 = 1;
    // Configure WP pin as an input
    TRISFbits.TRISF1 = 1;
}

inline void USER_SdSpiSetCs(uint8_t a)
{
    LATBbits.LATB1 = a;
}

inline bool USER_SdSpiGetCd(void)
{
    return (!PORTFbits.RF0) ? true : false;
}

inline bool USER_SdSpiGetWp(void)
{
    return (PORTFbits.RF1) ? true : false;
}

// The sdCardMediaParameters structure defines user-implemented functions needed by the SD-SPI fileio driver.
// The driver will call these when necessary.  For the SD-SPI driver, the user must provide
// parameters/functions to define which SPI module to use, Set/Clear the chip select pin,
// get the status of the card detect and write protect pins, and configure the CS, CD, and WP
// pins as inputs/outputs (as appropriate).
// For this demo, these functions are implemented in system.c, since the functionality will change
// depending on which demo board/microcontroller you're using.
// This structure must be maintained as long as the user wishes to access the specified drive.
FILEIO_SD_DRIVE_CONFIG sdCardMediaParameters =
{
    1,                                  // Use SPI module 2
    USER_SdSpiSetCs,                    // User-specified function to set/clear the Chip Select pin.
    USER_SdSpiGetCd,                    // User-specified function to get the status of the Card Detect pin.
    USER_SdSpiGetWp,                    // User-specified function to get the status of the Write Protect pin.
    USER_SdSpiConfigurePins             // User-specified function to configure the pins' TRIS bits.
};


#elif defined(DRV_SPI_CONFIG_CHANNEL_2_ENABLE)
/*==============================================================================
 * SPI 2
 * 
 * Hardware Configuration: This configuration requires that the SD-card PICTail+
 * board be plugged into the second slot of the PICTail+ connector (the one
 * located in the middle of the connector).  
 * 
 * On the SD-card PICTail+ board
 *  1) Short pins 1 and 2 on JP1
 *  2) Short pins 1 and 2 on JP2
 *  3) Short pins 1 and 2 on JP3
 * 
 * On the PIM:
 *  1) Short pins 1 and 2 of J9
 *  2) Short pins 1 and 2 of J10
 *  All other jumpers are do not care
 *============================================================================*/ 
void USER_SdSpiConfigurePins (void)
{
    ANSELGbits.ANSG6 = 0;
    ANSELGbits.ANSG7 = 0;
    ANSELGbits.ANSG8 = 0;
    ANSELGbits.ANSG9 = 0;
        
    //enable a pull-up for the card detect, just in case the SD-Card isn't attached
    //  then lets have a pull-up to make sure we don't think it is there.
    CNPUGbits.CNPUG0 = 1;
    
    // Deassert the chip select pin
    LATBbits.LATB9 = 1;
    // Configure CS pin as an output
    TRISBbits.TRISB9 = 0;
    // Configure CD pin as an input
    TRISGbits.TRISG0 = 1;
    // Configure WP pin as an input
    TRISGbits.TRISG1 = 1;
}

inline void USER_SdSpiSetCs(uint8_t a)
{
    LATBbits.LATB9 = a;
}

inline bool USER_SdSpiGetCd(void)
{
    return (!PORTGbits.RG0) ? true : false;
}

inline bool USER_SdSpiGetWp(void)
{
    return (PORTGbits.RG1) ? true : false;
}

// The sdCardMediaParameters structure defines user-implemented functions needed by the SD-SPI fileio driver.
// The driver will call these when necessary.  For the SD-SPI driver, the user must provide
// parameters/functions to define which SPI module to use, Set/Clear the chip select pin,
// get the status of the card detect and write protect pins, and configure the CS, CD, and WP
// pins as inputs/outputs (as appropriate).
// For this demo, these functions are implemented in system.c, since the functionality will change
// depending on which demo board/microcontroller you're using.
// This structure must be maintained as long as the user wishes to access the specified drive.
FILEIO_SD_DRIVE_CONFIG sdCardMediaParameters =
{
    2,                                  // Use SPI module 2
    USER_SdSpiSetCs,                    // User-specified function to set/clear the Chip Select pin.
    USER_SdSpiGetCd,                    // User-specified function to get the status of the Card Detect pin.
    USER_SdSpiGetWp,                    // User-specified function to get the status of the Write Protect pin.
    USER_SdSpiConfigurePins             // User-specified function to configure the pins' TRIS bits.
};
#else
#error "SPI module selected in drv_spi_config.h is not supported."
#endif