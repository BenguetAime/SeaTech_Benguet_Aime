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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "usb_host_msd_scsi.h"

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
void APP_HostMSDSimpleInitialize();

/*********************************************************************
* Function: void APP_MountDrive (uint8_t address);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: address - Address of the USB device to mount
*
* Output: None
*
********************************************************************/
void APP_MountDrive(uint8_t address);

/*********************************************************************
* Function: void APP_HostMSDSimpleTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_HostMSDSimpleInitialize() and APP_HostMSDSimpleStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostMSDSimpleTasks();

