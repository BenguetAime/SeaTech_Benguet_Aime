/******************************************************************************
*
*                        Microchip File I/O Library
*
******************************************************************************
* FileName:           main.c
* Dependencies:       sd_spi.h
*                     fileio.h
*                     main.h
*                     rtcc.h
* Processor:          PIC24/dsPIC30/dsPIC33
* Compiler:           XC16
* Company:            Microchip Technology, Inc.
*
* Software License Agreement
*
* The software supplied herewith by Microchip Technology Incorporated
* (the "Company") for its PICmicro(R) Microcontroller is intended and
* supplied to you, the Company's customer, for use solely and
* exclusively on Microchip PICmicro Microcontroller products. The
* software is owned by the Company and/or its supplier, and is
* protected under applicable copyright laws. All rights are reserved.
* Any use in violation of the foregoing restrictions may subject the
* user to criminal sanctions under applicable laws, as well as to
* civil liability for the breach of the terms and conditions of this
* license.
*
* THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
********************************************************************/



#include "fileio.h"
#include <string.h>
#include "./main.h"
#include "rtcc.h"
#include "system.h"
#include "sd_spi.h"

// GetTimestamp will be called by the FILEIO library when it needs a timestamp for a file
void GetTimestamp (FILEIO_TIMESTAMP * timeStamp);

extern FILEIO_SD_DRIVE_CONFIG sdCardMediaParameters;

// The gSDDrive structure allows the user to specify which set of driver functions should be used by the
// FILEIO library to interface to the drive.
// This structure must be maintained as long as the user wishes to access the specified drive.
const FILEIO_DRIVE_CONFIG gSdDrive =
{
    (FILEIO_DRIVER_IOInitialize)FILEIO_SD_IOInitialize,                      // Function to initialize the I/O pins used by the driver.
    (FILEIO_DRIVER_MediaDetect)FILEIO_SD_MediaDetect,                       // Function to detect that the media is inserted.
    (FILEIO_DRIVER_MediaInitialize)FILEIO_SD_MediaInitialize,               // Function to initialize the media.
    (FILEIO_DRIVER_MediaDeinitialize)FILEIO_SD_MediaDeinitialize,           // Function to de-initialize the media.
    (FILEIO_DRIVER_SectorRead)FILEIO_SD_SectorRead,                         // Function to read a sector from the media.
    (FILEIO_DRIVER_SectorWrite)FILEIO_SD_SectorWrite,                       // Function to write a sector to the media.
    (FILEIO_DRIVER_WriteProtectStateGet)FILEIO_SD_WriteProtectStateGet,     // Function to determine if the media is write-protected.
};

// Declare a state machine for our device
typedef enum
{
    DEMO_STATE_NO_MEDIA = 0,
    DEMO_STATE_MEDIA_DETECTED,
    DEMO_STATE_DRIVE_MOUNTED,
    DEMO_STATE_DONE,
    DEMO_STATE_FAILED
} DEMO_STATE;

// Some sample data to write to the file
uint8_t sampleData[10] = "DATA,10\r\n";

// Main loop.
// This demo program implements a simple data logger.
int main (void)
{
    DEMO_STATE demoState = DEMO_STATE_NO_MEDIA;
    FILEIO_OBJECT file;
    BSP_RTCC_DATETIME dateTime;
    
    SYSTEM_Initialize();
    
    dateTime.bcdFormat = false;
    RTCC_BuildTimeGet(&dateTime);
    RTCC_Initialize (&dateTime);

    // Initialize the library
    if (!FILEIO_Initialize())
    {
        while(1);
    }

    // Register the GetTimestamp function as the timestamp source for the library.
    FILEIO_RegisterTimestampGet (GetTimestamp);

    while(1)
    {
        switch (demoState)
        {
            case DEMO_STATE_NO_MEDIA:
                // Loop on this function until the SD Card is detected.
                if (FILEIO_MediaDetect(&gSdDrive, &sdCardMediaParameters) == true)
                {
                    demoState = DEMO_STATE_MEDIA_DETECTED;
                }
                break;
            case DEMO_STATE_MEDIA_DETECTED:
#if defined (__XC8__)
                // When initializing an SD Card on PIC18, the the SPI clock must be between 100 and 400 kHz.
                // The largest prescale divider in most PIC18 parts is 64x, which means the clock frequency must
                // be between 400 kHz (for a 4x divider) and 25.6 MHz (for a 64x divider).  In this demo, we are
                // achieving this by disabling the PIC's PLL during the drive mount operation (note that the
                // SYS_CLK_FrequencySystemGet() function must accurate reflect this change).  You could also map the "slow"
                // SPI functions used by the sd-spi driver to an SPI implementation that will run between
                // 100 kHz and 400 kHz at Fosc values greater than 25.6 MHz.  The macros to remap are located in
                // fileio_config.h.
                OSCTUNEbits.PLLEN = 0;
#endif
                // Try to mount the drive we've defined in the gSdDrive structure.
                // If mounted successfully, this drive will use the drive Id 'A'
                // Since this is the first drive we're mounting in this application, this
                // drive's root directory will also become the current working directory
                // for our library.
                if (FILEIO_DriveMount('A', &gSdDrive, &sdCardMediaParameters) == FILEIO_ERROR_NONE)
                {
                    demoState = DEMO_STATE_DRIVE_MOUNTED;
                }
                else
                {
                    demoState = DEMO_STATE_NO_MEDIA;
                }
#if defined (__XC8__)
                OSCTUNEbits.PLLEN = 1;
#endif
                break;
            case DEMO_STATE_DRIVE_MOUNTED:
                // Open TESTFILE.TXT.
                // Specifying CREATE will create the file is it doesn't exist.
                // Specifying APPEND will set the current read/write location to the end of the file.
                // Specifying WRITE will allow you to write to the code.
                if (FILEIO_Open (&file, "TESTFILE.CSV", FILEIO_OPEN_WRITE | FILEIO_OPEN_APPEND | FILEIO_OPEN_CREATE) == FILEIO_RESULT_FAILURE)
                {
                    demoState = DEMO_STATE_FAILED;
                    break;
                }

                // Write some sample data to the card
                if (FILEIO_Write (sampleData, 1, 9, &file) != 9)
                {
                    demoState = DEMO_STATE_FAILED;
                    break;
                }

                // Close the file to save the data
                if (FILEIO_Close (&file) != FILEIO_RESULT_SUCCESS)
                {
                    demoState = DEMO_STATE_FAILED;
                    break;
                }

                // We're done with this drive.
                // Unmount it.
                FILEIO_DriveUnmount ('A');
                demoState = DEMO_STATE_DONE;
                break;
            case DEMO_STATE_DONE:
                // Now that we've written all of the data we need to write in the application, wait for the user
                // to remove the card
                if (FILEIO_MediaDetect(&gSdDrive, &sdCardMediaParameters) == false)
                {
                    demoState = DEMO_STATE_NO_MEDIA;
                }
                break;
            case DEMO_STATE_FAILED:
                // An operation has failed.  Try to unmount the drive.  This will also try to
                // close all open files that use this drive (it will at least deallocate them).
                FILEIO_DriveUnmount ('A');
                // Return to the media-detect state
                demoState = DEMO_STATE_NO_MEDIA;
                break;
        }
    }
}

void GetTimestamp (FILEIO_TIMESTAMP * timeStamp)
{
    BSP_RTCC_DATETIME dateTime;

    dateTime.bcdFormat = false;

    RTCC_TimeGet(&dateTime);

    timeStamp->timeMs = 0;
    timeStamp->time.bitfield.hours = dateTime.hour;
    timeStamp->time.bitfield.minutes = dateTime.minute;
    timeStamp->time.bitfield.secondsDiv2 = dateTime.second / 2;

    timeStamp->date.bitfield.day = dateTime.day;
    timeStamp->date.bitfield.month = dateTime.month;
    // Years in the RTCC module go from 2000 to 2099.  Years in the FAT file system go from 1980-2108.
    timeStamp->date.bitfield.year = dateTime.year + 20;;
}

