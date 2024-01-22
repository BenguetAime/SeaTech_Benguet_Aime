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

/** INCLUDES *******************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "usb.h"

#include "fileio.h"

#include "app_host_msd_simple.h"

static FILEIO_OBJECT myFile;

typedef enum
{
    WAITING_FOR_ATTACH,
    WRITING_TO_DEVICE,
    WAITING_FOR_DETACH
} USB_HOST_MSD_SIMPLE_DEMO_STATE;

static USB_HOST_MSD_SIMPLE_DEMO_STATE demoState;
static uint8_t deviceAddress = 0;

// Declare a FILEIO_DRIVE_CONFIG structure to describe which functions the File I/O library will use to communicate with the media
const FILEIO_DRIVE_CONFIG gUSBDrive =
{
    (FILEIO_DRIVER_IOInitialize)NULL,                     // Function to initialize the I/O pins used by the driver.
    (FILEIO_DRIVER_MediaDetect)USBHostMSDSCSIMediaDetect,                   // Function to detect that the media is inserted.
    (FILEIO_DRIVER_MediaInitialize)USBHostMSDSCSIMediaInitialize,           // Function to initialize the media.
    (FILEIO_DRIVER_MediaDeinitialize)USBHostMSDSCSIMediaDeinitialize,              // Function to de-initialize the media.
    (FILEIO_DRIVER_SectorRead)USBHostMSDSCSISectorRead,                     // Function to read a sector from the media.
    (FILEIO_DRIVER_SectorWrite)USBHostMSDSCSISectorWrite,                   // Function to write a sector to the media.
    (FILEIO_DRIVER_WriteProtectStateGet)USBHostMSDSCSIWriteProtectState,    // Function to determine if the media is write-protected.
};

/*********************************************************************
* Function: void APP_HostMSDSimpleInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostMSDSimpleInitialize()
{
    demoState = WAITING_FOR_ATTACH;
}

void APP_MountDrive(uint8_t address)
{
    if (demoState == WAITING_FOR_ATTACH)
    {
        deviceAddress = address;
        demoState = WRITING_TO_DEVICE;
    }
}

/*********************************************************************
* Function: void APP_HostMSDSimpleTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized via
*   the APP_HostMSDSimpleInitialize()  
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostMSDSimpleTasks()
{
    int status;
    switch(demoState)
    {
        case WAITING_FOR_ATTACH:
            break;

        case WRITING_TO_DEVICE:
            if(FILEIO_MediaDetect(&gUSBDrive, &deviceAddress) == true)
            {
                //Now that we've found a device, we can try to write to it.
                demoState = WRITING_TO_DEVICE;
            }
            else
            {
                break;
            }

            // Attempt to mount the drive described by gUSBDrive as drive 'A'
            // The deviceAddress parameter describes the USB address of the device; it is initialized by the application in the 
            // USB_ApplicationEventHandler function when a new device is detected.
            if(FILEIO_DriveMount ('A', &gUSBDrive, &deviceAddress) == FILEIO_ERROR_NONE)
            {
                // Opening a file with the FILEIO_OPEN_WRITE option allows us to write to the file.
                // Opening a file with the FILEIO_OPEN_CREATE file will create the file if it does not already exist.
                // Opening a file with the FILEIO_OPEN_TRUNCATE file will truncate it to a 0-length file if it already exists.
                status = FILEIO_DirectoryMake ("DIR1");
//#define WORK_AROUND                
#ifdef WORK_AROUND                
                status = FILEIO_DirectoryChange("DIR1");
                if(FILEIO_Open(&myFile, "TEST.TXT", FILEIO_OPEN_WRITE | FILEIO_OPEN_CREATE | FILEIO_OPEN_TRUNCATE) == FILEIO_RESULT_FAILURE)
#else                    
                if(FILEIO_Open(&myFile, "DIR1/TEST.TXT", FILEIO_OPEN_WRITE | FILEIO_OPEN_CREATE | FILEIO_OPEN_TRUNCATE) == FILEIO_RESULT_FAILURE)
#endif                    

                {
                    //Opening the file failed.  Since we can't write to the
                    //  device, abort the write attempt and wait for the device
                    //  to be removed.
                    demoState = WAITING_FOR_DETACH;
                    break;
                }

                //Write some data to the new file.
                FILEIO_Write("This is a test.", 1, 15, &myFile);

                //Always make sure to close the file so that the data gets
                //  written to the drive.
                FILEIO_Close(&myFile);

                // Unmount the drive since it is no longer in use.
                FILEIO_DriveUnmount ('A');

                //Now that we are done writing, we can do nothing until the
                //  drive is removed.
                demoState = WAITING_FOR_DETACH;
            }
            break;

        case WAITING_FOR_DETACH:
            if(FILEIO_MediaDetect(&gUSBDrive, &deviceAddress) == false)
            {
                //The device has been removed.  Now we should wait for a new
                //  device to be attached.
                demoState = WAITING_FOR_ATTACH;
            }
            break;
    }
}
