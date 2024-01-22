/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_tcon.h

  Summary:
    This header declares common API used in TCON drivers.
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

#ifndef _GFXTCON_H
    #define _GFXTCON_H

#include <stdint.h>
#include "driver/spi/drv_spi.h"

// typedef for write function pointer
typedef void (*TCON_WRITE_FUNC)(uint16_t index, uint16_t value);

/*********************************************************************
* Function:  DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc)
*
* PreCondition:  IO ports used by the driver should be initialized.
*
* Input: pWriteFunc - non-volatile memory write function pointer
*
* Output: None
*
* Side Effects: None
*
* Overview: initializes the timing controller used.
*
* Note: none
*
********************************************************************/
void DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc);

/*********************************************************************
* Function:  DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc)
*
* PreCondition:  IO ports used by the driver should be initialized.
*
* Input: pWriteFunc - non-volatile memory write function pointer
*
* Output: None
*
* Side Effects: None
*
* Overview: initializes the timing controller used.
*
* Note: none
*
********************************************************************/
void DRV_TCON_SPI_Initialize(DRV_SPI_INIT_DATA *pInitData);

/*********************************************************************
* Function:  DRV_TCON_BB_CommandWrite(uint16_t index, uint16_t value)
*
* PreCondition:  IO ports used by the driver should be initialized.
*
* Input: index - The index (or address) of the register to be written.
*        value - The value that will be written to the register.
*
* Output: None
*
* Side Effects: None
*
* Overview: Writes commands to the Timing Controller (TCON) using
*           bit-banged SPI.
*
* Note: none
*
********************************************************************/
void DRV_TCON_BB_CommandWrite(uint16_t index, uint16_t value);

/*********************************************************************
* Function:  DRV_TCON_SPI_CommandWrite(uint16_t index, uint16_t value)
*
* PreCondition:  SPI module used by the driver should be initialized.
*
* Input: index - The index (or address) of the register to be written.
*        value - The value that will be written to the register.
*
* Output: None
*
* Side Effects: None
*
* Overview: Writes commands to the Timing Controller (TCON) using
*           the pre-configured SPI channel.
*
* Note: none
*
********************************************************************/
void DRV_TCON_SPI_CommandWrite(uint16_t index, uint16_t value);

#endif // #ifndef _GFXTCON_H

