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

/** CONFIGURATION Bits **********************************************/
#pragma config IESO = OFF
#pragma config FNOSC = PRIPLL
#pragma config FCKSM = CSDCMD
#pragma config OSCIOFNC = OFF
#pragma config POSCMOD = HS

#pragma config JTAGEN = OFF
#pragma config GCP = OFF
#pragma config GWRP = OFF
#pragma config ICS = PGx2
#pragma config FWDTEN = OFF

void SYSTEM_Initialize (void)
{
    ANSB = 0x0000 ;
    ANSA = 0x0000;
    ANSC = 0x0000;
    ANSD = 0x0000;

   // Enable PLL
    CLKDIVbits.PLLEN = 1;
}

void USER_SdSpiConfigurePins (void)
{
    // Configure SPI1 PPS pins (ENC28J60/ENCX24J600/MRF24WB0M or other PICtail Plus cards)
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS

    RPOR1bits.RP2R = 8;       // assign RP2 for SCK1
    RPOR0bits.RP1R = 7;       // assign RP1 for SDO1
    RPINR20bits.SDI1R = 0;    // assign RP0 for SDI1

    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS

    // Deassert the chip select pin
    LATGbits.LATG6 = 1;
    // Configure CS pin as an output
    TRISGbits.TRISG6 = 0;
    // Configure CD pin as an input
    TRISAbits.TRISA15 = 1;
    // Configure WP pin as an input
    TRISCbits.TRISC13 = 1;
}

inline void USER_SdSpiSetCs(uint8_t a)
{
    LATGbits.LATG6 = a;
}

inline bool USER_SdSpiGetCd(void)
{
    return (!PORTAbits.RA15) ? true : false;
}

inline bool USER_SdSpiGetWp(void)
{
    return (PORTCbits.RC13) ? true : false;
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