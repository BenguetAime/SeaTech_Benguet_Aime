/*******************************************************************************
Graphics Resource Types Header for Microchip Graphics Library

  Company:
    Microchip Technology Inc.
 
  File Name:
    gfx_types_palette.h

  Summary:
    This header file defines the types used in palettes for the Microchip Graphics
    Library.

  Description:
    This header defines the different structures used for palette resources in the
    Microchip Graphics Library.

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

#ifndef GFX_TYPES_PALETTE_H
// DOM-IGNORE-BEGIN
    #define GFX_TYPES_PALETTE_H
// DOM-IGNORE-END

/*****************************************************************************
 * SECTION: Includes
 *****************************************************************************/
#include <stdint.h>


// *****************************************************************************
/*
    <GROUP primitive_types>

    Typedef:
        GFX_PALETTE_ENTRY

    Summary:
        Defines the union used for each entry in the palette table.

    Description:
        Defines the union used for each entry in the palette table
        specified by GFX_RESOURCE_PALETTE structure.

        There are two types of palette:
        - For TFT: color is defined as 5-6-5 RGB format
                   where 5-bits for RED, 6-bits for GREEN
                   and 5-bits for BLUE.
        - For Monochrome: 4 bits are used to represent the luma.

    Remarks:
        None.

*/
// *****************************************************************************
typedef union __attribute__ ((packed))
{
    uint16_t    value;                // a 16-bit value representing a color or palette entry
    struct __attribute__ ((packed))
    {
        uint16_t    r : 5;            // represents the RED component 
        uint16_t    g : 6;            // represents the GREEN component 
        uint16_t    b : 5;            // represents the BLUE component 
    } color;                          // color value in 5-6-5 RGB format

    struct __attribute__ ((packed))
    {
        uint16_t    reserved : 12;    // reserved, used as a filler 
        uint16_t    luma : 4;         // monochrome LUMA value
    } monochrome;                     // monochrome LUMA value

} GFX_PALETTE_ENTRY;

#endif  // GFX_TYPES_PALETTE_H
