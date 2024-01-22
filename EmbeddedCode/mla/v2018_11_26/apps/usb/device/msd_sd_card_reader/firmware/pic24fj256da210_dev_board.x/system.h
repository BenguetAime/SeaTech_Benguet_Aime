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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "drv_spi.h"

#include "io_mapping.h"

// Definition for system clock
#define SYS_CLK_FrequencySystemGet()        32000000
// Definition for peripheral clock
#define SYS_CLK_FrequencyPeripheralGet()    SYS_CLK_FrequencySystemGet()
// Definition for instruction clock
#define SYS_CLK_FrequencyInstructionGet()   (SYS_CLK_FrequencySystemGet() / 2)

#define MAIN_RETURN int

//Definitions letting SPI driver code know which SPI module to use
#define DRV_SPI_CONFIG_CHANNEL_1_ENABLE      // Enable SPI channel 1
//#define DRV_SPI_CONFIG_CHANNEL_2_ENABLE      // Enable SPI channel 2
//#define DRV_SPI_CONFIG_CHANNEL_3_ENABLE      // Enable SPI channel 3
//#define DRV_SPI_CONFIG_CHANNEL_4_ENABLE      // Enable SPI channel 4

#define USER_SPI_MODULE_NUM   1			//MSSP number used on this device

// Disable SPI FIFO mode in parts where it is not supported
// Enabling the FIFO mode will improve library performance.
// In this demo this definition is sometimes disabled because early versions of the PIC24FJ128GA010s have an errata preventing this feature from being used.
#if defined (__XC8__) || defined (__PIC24FJ128GA010__)
#define DRV_SPI_CONFIG_ENHANCED_BUFFER_DISABLE
#endif

/*********************************************************************
* Function: void SYSTEM_Initialize(void)
*
* Overview: Initializes the system.
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void SYSTEM_Initialize(void);

/*********************************************************************
* Function: void SYSTEM_Tasks(void)
*
* Overview: Runs system level tasks that keep the system running
*
* PreCondition: System has been initalized with SYSTEM_Initialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
#define SYSTEM_Tasks()

// User-defined function to set the chip select for our example drive
void USER_SdSpiSetCs_1 (uint8_t a);
// User-defined function to get the card detection status for our example drive
bool USER_SdSpiGetCd_1 (void);
// User-defined function to get the write-protect status for our example drive
bool USER_SdSpiGetWp_1 (void);
// User-defined function to initialize tristate bits for CS, CD, and WP
void USER_SdSpiConfigurePins_1 (void);

#endif