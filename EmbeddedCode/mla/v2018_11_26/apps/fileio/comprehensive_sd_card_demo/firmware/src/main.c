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

#include <stdint.h>
#include <string.h>

#include "./main.h"
#include "rtcc.h"
#include "fileio.h"
#include "sd_spi.h"

// GetTimestamp will be called by the FILEIO library when it needs a timestamp for a file
void GetTimestamp (FILEIO_TIMESTAMP * timeStamp);

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
    1,                                  // Use SPI module 1
    USER_SdSpiSetCs_1,                  // User-specified function to set/clear the Chip Select pin.
    USER_SdSpiGetCd_1,                  // User-specified function to get the status of the Card Detect pin.
    USER_SdSpiGetWp_1,                  // User-specified function to get the status of the Write Protect pin.
    USER_SdSpiConfigurePins_1           // User-specified function to configure the pins' TRIS bits.
};

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

// Some sample data to write to the file
uint8_t sampleData[10] = "DATA,10\r\n";

// Buffer for reading data
uint8_t buffer[20];

// Main loop.
// This program demonstrates how to use the functions in the File I/O library
int main (void)
{
    FILEIO_OBJECT file;
    uint8_t character;
    int result;
    FILEIO_ERROR_TYPE errorType;
    FILEIO_SEARCH_RECORD searchRecord;
    FILEIO_FILE_SYSTEM_TYPE fileSystemType;
    FILEIO_DRIVE_PROPERTIES driveProperties;
    RTCC_DATETIME dateTime;

    SYSTEM_Initialize();

    // Initialize the RTCC from some compiler-generated variables
    dateTime.bcdFormat = false;
    RTCC_BuildTimeGet(&dateTime);
    RTCC_Initialize(&dateTime);

    // Initialize the library
    if (FILEIO_Initialize() == false)
    {
        while(1);
    }

    // Register the GetTimestamp function as the timestamp source for the library.
    // Whenever a file is created or modified, the file's timestamp will be updated with the
    // Values returned by this function.
    FILEIO_RegisterTimestampGet (GetTimestamp);

    // Call the FILEIO_MediaDetect function until an SD Card is detected.
    while (FILEIO_MediaDetect (&gSdDrive, &sdCardMediaParameters) == false);

    // Format the card.
    // The format function is being called in a mode that will erase all of the card contents.  There is another mode that can
    // be used to recreate the device's Boot Sector, but this is typically unnecessary.
//    if (FILEIO_Format (&gSdDrive, &sdCardMediaParameters, FILEIO_FORMAT_ERASE, 0, "EXAMPLE) != FILEIO_RESULT_SUCCESS)
//    {
//        while(1);
//    }

    // Mount the SD card as a drive with drive letter 'A'
    if (FILEIO_DriveMount('A', &gSdDrive, &sdCardMediaParameters) != FILEIO_ERROR_NONE)
    {
        while(1);
    }

    // Get the file system type
    fileSystemType = FILEIO_FileSystemTypeGet('A');

    // Tell the FILEIO_DrivePropertiesGet function that we are making a new request
    driveProperties.new_request = true;

    // Make sure we have at least two clusters free on our drive
    do
    {
        FILEIO_DrivePropertiesGet(&driveProperties, 'A');
    } while ((driveProperties.results.free_clusters < 2) && (driveProperties.properties_status == FILEIO_GET_PROPERTIES_STILL_WORKING));


    // Open a file called TESTFILE.TXT and store the file data in the FILEIO_OBJECT "file."
    // This file is being opened with the CREATE flag, which will allow the library to create the file if it doesn't already exist, the
    // TRUNCATE flag, which will truncate the file to a 0-length file if it already exists, the WRITE flag, which will allow us to
    // write to the file, and the READ flag, which will allow us to read from it.
    if (FILEIO_Open (&file, "TESTFILE.TXT", FILEIO_OPEN_CREATE | FILEIO_OPEN_TRUNCATE | FILEIO_OPEN_WRITE | FILEIO_OPEN_READ) == FILEIO_RESULT_FAILURE)
    {
        while(1);
    }

    // Write some sample data to the card with the FILEIO_Write function.
    // "sampleData" is an array containing the data to be written.
    // The '1' parameter indicates that we will be writing one-byte blocks of data.
    // The '9' parameter indicates that we will be writing 9 one-byte blocks of data.
    // "file" is the file that we are writing to.
    if (FILEIO_Write (sampleData, 1, 9, &file) != 9)
    {
        while(1);
    }

    // Flush the file to save the data.  If the user calls Flush, they will be be able to continue
    // performing additional operations on the file.
    if (FILEIO_Flush (&file) != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Use the FILEIO_Seek function to change the current location in the file.
    // The parameters in this function call will set the current location in the
    // file to 5 bytes before the current position in the file.  Since we've already
    // written 9 bytes, this will set our offset in the file to '4.'
    if (FILEIO_Seek(&file, -5, FILEIO_SEEK_CUR) != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Use the FILEIO_Tell function to verify the current location in the file.
    if (FILEIO_Tell(&file) != 4)
    {
        while(1);
    }

    // Replace the character in the current position with a new character
    if (FILEIO_PutChar ('=', &file) != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Read a single character from the file
    if ((result = FILEIO_GetChar(&file)) != FILEIO_RESULT_FAILURE)
    {
        character = (uint8_t)result;
    }
    else
    {
        while(1);
    }

    // Keep reading characters until we get to the end of the file
    while ((result = FILEIO_GetChar(&file)) != FILEIO_RESULT_FAILURE)
    {
        character = (uint8_t)result;
    }

    // Check to see if the GetChar function failed because we reached the end of the file
    if (FILEIO_Eof(&file) != true)
    {
        while(1);
    }

    // Change the current position in the file back to the beginning.
    // In this case, the function arguments indicate that we're setting the
    // location to an offset of 0 bytes from the beginning of the file.
    if (FILEIO_Seek(&file, 0, FILEIO_SEEK_SET) != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Use the FILEIO_Read function to read the file data.
    // Our data buffer can hold 20 bytes, so we'll try to read 20 one-byte objects.
    // If the file is large than 20 bytes, 20 bytes will be read.  Otherwise, the
    // number of bytes read successfully with be returned.
    if (FILEIO_Read (buffer, 1, 20, &file) != 20)
    {
        // If we didn't read 20 bytes, make sure it's because we reached the end of the file.
        if (FILEIO_Eof(&file) != true)
        {
            while(1);
        }
    }

    // Close the file.  Close will save the file's data like Flush, but you
    // will not be able to continue using the file.
    if (FILEIO_Close(&file) != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Re-open our file in APPEND mode.  APPEND will set the current position in the file to
    // the end of the file.
    if (FILEIO_Open (&file, "TESTFILE.TXT", FILEIO_OPEN_APPEND) == FILEIO_RESULT_FAILURE)
    {
        while(1);
    }

    // Try to write some data to the end of the file.
    // This function should fail because we didn't open the file in WRITE mode.
    if (FILEIO_PutChar ('!', &file) == FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Check to see what kind of error caused the failure
    errorType = FILEIO_ErrorGet('A');
    if (errorType != FILEIO_ERROR_READ_ONLY)
    {
        while(1);
    }

    // Clear the error and close the file
    FILEIO_ErrorClear('A');
    FILEIO_Close (&file);

    // Rename our file.
    // Note that if you run this demo code more than once, it will fail here
    // because there will already be a file called NEWFILE.TXT on your card.
    if (FILEIO_Rename ("TESTFILE.TXT", "NEWFILE.TXT") != FILEIO_RESULT_SUCCESS)
    {
        // Check to see if an error occured because NEWFILE.TXT already exists.
        // If so, move on to the next function.  Otherwise, loop infinitely.
        if (FILEIO_ErrorGet('A') != FILEIO_ERROR_FILENAME_EXISTS)
        {
            while(1);
        }
        // Clear the error.
        FILEIO_ErrorClear('A');
    }

    // Create a new file and close it
    if ((FILEIO_Open (&file, "NEXTFILE.TXT", FILEIO_OPEN_CREATE) == FILEIO_RESULT_FAILURE) || (FILEIO_Close(&file) == FILEIO_RESULT_FAILURE))
    {
        while(1);
    }

    // Try to find some text files in our current directory on our device.  The '*' wildcard in the
    // path name will let us find files with any name and with the extension "TXT"
    // Specifying FILEIO_ATTRIBUTE_MASK will let us find files (or directories) with any combination of
    // attributes.
    // The found file's parameters will be stored in the FILEIO_SEARCH_RECORD structure.
    // Specifying 'true' as the fourth parameter indicates that this is the first search with these parameters in this directory.
    // If the user specifies 'false' for that parameter, this function will try to find the next file in the specified directory
    // that matches the given parameters.  The user must specify 'true' before specifying 'false.'
    // If there were no pre-existing text files on the device, this should find NEWFILE.TXT first.
    if (FILEIO_Find ("*.TXT", FILEIO_ATTRIBUTE_MASK, &searchRecord, true) != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Try to find another file in the same directory with the same parameters.  If this drive had no pre-existing files, this
    // should be "NEXTFILE.TXT."
    if (FILEIO_Find ("*.TXT", FILEIO_ATTRIBUTE_MASK, &searchRecord, false) != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Delete a file
    if (FILEIO_Remove ("NEXTFILE.TXT") != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Create a new directory
    if (FILEIO_DirectoryMake("DIRONE") != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }
    
    // Create an empty file with a relative path
    if ((FILEIO_Open(&file, "DIRONE/FILE.TXT", FILEIO_OPEN_CREATE) == FILEIO_RESULT_FAILURE) ||
            (FILEIO_Close(&file) == FILEIO_RESULT_FAILURE))
    {
        while(1);
    }

    // Change to the directory we created
    if (FILEIO_DirectoryChange("DIRONE") != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Create a directory in the root directory with an absolute path
    if (FILEIO_DirectoryMake("A:/DIRTWO") != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Get the current directory name
    {
        // The current directory name should be 9 characters ("A:/DIRONE")
        char buffer[20];
        uint16_t count;
        if ((count = FILEIO_DirectoryGetCurrent(buffer, 20)) != 9)
        {
            while(1);
        }
    }

    // Change to the parent directory of the current directory (the parent directory of DIRONE is the root directory
    if (FILEIO_DirectoryChange("..") != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Delete the directory we created earlier.  You aren't able to delete non-empty directories.
    if (FILEIO_DirectoryRemove("DIRTWO") != FILEIO_RESULT_SUCCESS)
    {
        while(1);
    }

    // Unmount the drive.  This will free the resources containing that drive's information so
    // that other drives can be mounted.  You won't be able to continue using a drive after
    // unmounting it.
    FILEIO_DriveUnmount ('A');

    while (1);
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

