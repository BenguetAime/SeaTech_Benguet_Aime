/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_tcon_ssd1289.c

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
#include "system_config.h"
#include "system.h"
#include "driver/gfx/drv_gfx_tcon.h"

/************************************************************************
* Function: void DRV_TCON_Initialize(void)
*                                                                       
* Overview: Initialize the IOs to implement Bitbanged SPI interface
*           to connect to the Timing Controller through SPI.
*                                                                       
* Input: none
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc)
{

    pWriteFunc(0x0028, 0x0006);
    pWriteFunc(0x0000, 0x0001);
    __delay_ms(15);

    pWriteFunc(0x002B, 0x9532);
    pWriteFunc(0x0003, 0xAAAC);
    pWriteFunc(0x000C, 0x0002);
    pWriteFunc(0x000D, 0x000A);
    pWriteFunc(0x000E, 0x2C00);
    pWriteFunc(0x001E, 0x00AA);
    pWriteFunc(0x0025, 0x8000);
    __delay_ms(15);

    pWriteFunc(0x0001, 0x2B3F);
    pWriteFunc(0x0002, 0x0600);
    pWriteFunc(0x0010, 0x0000);
    __delay_ms(20);

    pWriteFunc(0x0005, 0x0000);
    pWriteFunc(0x0006, 0x0000);


    pWriteFunc(0x0016, 0xEF1C);
    pWriteFunc(0x0017, 0x0003);
    pWriteFunc(0x0007, 0x0233);
    pWriteFunc(0x000B, 0x5312);
    pWriteFunc(0x000F, 0x0000);
    __delay_ms(20);

    pWriteFunc(0x0041, 0x0000);
    pWriteFunc(0x0042, 0x0000);
    pWriteFunc(0x0048, 0x0000);
    pWriteFunc(0x0049, 0x013F);
    pWriteFunc(0x0044, 0xEF00);
    pWriteFunc(0x0045, 0x0000);
    pWriteFunc(0x0046, 0x013F);
    pWriteFunc(0x004A, 0x0000);
    pWriteFunc(0x004B, 0x0000);
    __delay_ms(20);

    pWriteFunc(0x0030, 0x0707);
    pWriteFunc(0x0031, 0x0704);
    pWriteFunc(0x0032, 0x0204);
    pWriteFunc(0x0033, 0x0201);
    pWriteFunc(0x0034, 0x0203);
    pWriteFunc(0x0035, 0x0204);
    pWriteFunc(0x0036, 0x0204);
    pWriteFunc(0x0037, 0x0502);
    pWriteFunc(0x003A, 0x0302);
    pWriteFunc(0x003B, 0x0500);
    __delay_ms(20);

}


