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

#include "usb_host_cdc.h"

/*********************************************************************
* Function: void APP_HostCDCBasicInitialize(void);
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
void APP_HostCDCBasicInitialize();

/*********************************************************************
* Function: void APP_HostCDCBasicTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized via
*   the APP_HostCDCBasicInitialize() function
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostCDCBasicTasks();

/*********************************************************************
* Function: bool APP_HostCDCBasicEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size );
*
* Overview: Handles USB events for the USB host CDC demo
*
* PreCondition: None
*
* Input: uint8_t - address of the device causing the event
*        USB_EVENT - the event that occured
*        void* - data accociated with the event
*        uint32_t - size of the data for the event
*
* Output: None
*
********************************************************************/
bool APP_HostCDCBasicEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size );
