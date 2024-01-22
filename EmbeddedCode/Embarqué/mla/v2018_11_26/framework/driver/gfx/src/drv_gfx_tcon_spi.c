/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_tcon_spi.c

  Summary:
    This module initializes the SSD1289 Timing Controller (TCON) Module.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2018 Microchip Technology Inc. (www.microchip.com)

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
// DOM-IGNORE-END
#include <stdint.h>
#include <string.h>
#include "system.h"
#include "driver/gfx/drv_gfx_tcon.h"
#include "driver/spi/drv_spi.h"

/*
 * This is a generic Timing Controller (TCON) module driver using SPI as
 * the communication protocol. 
*/

static DRV_SPI_INIT_DATA spiInitData;

/******************************************************************************
  Function:
    void DRV_TCON_SPI_Initialize( const SYS_MODULE_INIT * const init )

  Summary:
    Initializes hardware and data for the given module

  Description:
    This routine initializes hardware for the instance of the NVM module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  Parameters:
    pInitData - Pointer to the data structure containing all data
                necessary to initialize the hardware. This pointer may
                be null if no data is required and static initialization
                values are to be used.

  Returns:
    None
******************************************************************************/
void DRV_TCON_SPI_Initialize(DRV_SPI_INIT_DATA *pInitData)
{
    // initialize the SPI channel to be used
    DRV_SPI_Initialize(pInitData);
    memcpy(&spiInitData, pInitData, sizeof(DRV_SPI_INIT_DATA));

}

/************************************************************************
* Function: void DRV_TCON_SPI_CommandWrite(uint8_t index, uint16_t value)
*                                                                       
* Overview: This writes a word to SPI module.
*                                                                       
* Input: index - The index (or address) of the register to be written.
*        value - The value that will be written to the register.
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void DRV_TCON_SPI_CommandWrite(uint16_t index, uint16_t value)
{
    typedef union
    {
        uint8_t  indexByte[2];
        uint16_t indexValue;
    }  GFX_TCON_INDEX;

    TCON_CS_LAT = 0;

    // Command
    TCON_DC_LAT = 0;
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)index).indexByte[1]);
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)index).indexByte[0]);

    TCON_CS_LAT = 1;
    __delay_us(10);
    TCON_CS_LAT = 0;

    // Data
    TCON_DC_LAT = 1;
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)value).indexByte[1]);
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)value).indexByte[0]);

    TCON_CS_LAT = 1;
    __delay_us(10);
}



