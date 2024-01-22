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

#include "system.h"
#include "system_config.h"

#include "usb.h"
#include "usb_device_msd.h"

#include "sd_spi.h"


/** VARIABLES ******************************************************/
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
    USER_SPI_MODULE_NUM,                // Use SPI module number defined in the system.h file
    USER_SdSpiSetCs_1,                  // User-specified function to set/clear the Chip Select pin.
    USER_SdSpiGetCd_1,                  // User-specified function to get the status of the Card Detect pin.
    USER_SdSpiGetWp_1,                  // User-specified function to get the status of the Write Protect pin.
    USER_SdSpiConfigurePins_1           // User-specified function to configure the pins' TRIS bits.
};

//The LUN variable definition is critical to the MSD function driver.  This
//  array is a structure of function pointers that are the functions that
//  will take care of each of the physical media.  For each additional LUN
//  that is added to the system, an entry into this array needs to be added
//  so that the stack can know where to find the physical layer functions.
//  In this example the media initialization function is named
//  "MediaInitialize", the read capacity function is named "ReadCapacity",
//  etc.
LUN_FUNCTIONS LUN[MAX_LUN + 1] =
{
    {
        (FILEIO_MEDIA_INFORMATION* (*)(void *))&FILEIO_SD_MediaInitialize,
        (uint32_t (*)(void *))&FILEIO_SD_CapacityRead,
        (uint16_t (*)(void *))&FILEIO_SD_SectorSizeRead,
        (bool  (*)(void *))&FILEIO_SD_MediaDetect,
        (uint8_t  (*)(void *, uint32_t, uint8_t*))&FILEIO_SD_SectorRead,
        (uint8_t  (*)(void *))&FILEIO_SD_WriteProtectStateGet,
        (uint8_t  (*)(void *, uint32_t, uint8_t *, uint8_t))&FILEIO_SD_SectorWrite,
        (void *)&sdCardMediaParameters,
        (uint8_t (*)(void *, void*))&FILEIO_SD_AsyncWriteTasks,
        (uint8_t (*)(void *, void*))&FILEIO_SD_AsyncReadTasks
    }
};

/* Standard Response to INQUIRY command stored in ROM 	*/
const InquiryResponse inq_resp = {
	0x00,		// peripheral device is connected, direct access block device
	0x80,       // removable
	0x04,	 	// version = 00=> does not conform to any standard, 4=> SPC-2
	0x02,		// response is in format specified by SPC-2
	0x20,		// n-4 = 36-4=32= 0x20
	0x00,		// sccs etc.
	0x00,		// bque=1 and cmdque=0,indicates simple queuing 00 is obsolete,
                // but as in case of other device, we are just using 00
	0x00,		// 00 obsolete, 0x80 for basic task queuing
	'M','i','c','r','o','c','h','p',    	// this is the T10 assigned Vendor ID
	'M','a','s','s',' ','S','t','o','r','a','g','e',' ',' ',' ',' ',
	'0','0','0','1'
};


/*********************************************************************
* Function: void APP_DeviceMSDInitialize(void);
*
* Overview: Initializes the Custom HID demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceMSDInitialize()
{
    #if (MSD_DATA_IN_EP == MSD_DATA_OUT_EP)
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    #else
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
        USBEnableEndpoint(MSD_DATA_OUT_EP,USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    #endif

    USBMSDInit();
}

/*********************************************************************
* Function: void APP_DeviceMSDTasks(void);
*
* Overview: Keeps the Custom HID demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceMSDInitialize() and APP_DeviceMSDStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceMSDTasks()
{
    MSDTasks();
}