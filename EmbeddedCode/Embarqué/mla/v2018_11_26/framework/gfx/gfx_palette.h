/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_palette.h

  Summary:
    The header file defines all palette APIs used in the graphics library.
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
 
#ifndef _GFX_PALETTE_H
// DOM-IGNORE-BEGIN
    #define _GFX_PALETTE_H
// DOM-IGNORE-END

#include <stdint.h>
#include "gfx_config.h"
#include "gfx/gfx_types_macros.h"
#include "gfx/gfx_types_resource.h"
#include "gfx/gfx_types_palette.h"

#ifndef GFX_CONFIG_PALETTE_DISABLE

// *****************************************************************************
/*
    <GROUP palette_functions>

    Function:
        GFX_STATUS DRV_GFX_PaletteInitialize()

    Summary:
        This function initializes the palette feature in the driver.
 
    Description:
        This function initializes and enables the palette feature 
        in the driver. This function should be called in the driver
        initialization routine and should be ready for usage after
        the driver has been initialized. 
 
    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

*/
// *****************************************************************************
void DRV_GFX_PaletteInitialize(void);

// *****************************************************************************
/*
    <GROUP palette_functions>

    Function:
        GFX_FEATURE_STATUS GFX_PaletteStatusGet()

     Summary:
        This function returns the status of the palette mode feature
        of the library.

    Description:
        This function returns the current status of the palette mode
        feature if enabled or disabled.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Status of the palette mode feature.
        GFX_FEATURE_ENABLED - feature is enabled.
        GFX_FEATURE_DISABLED - feature is disabled.
*/
// *****************************************************************************
GFX_FEATURE_STATUS GFX_PaletteStatusGet(void);

// *****************************************************************************
/*
    <GROUP palette_functions>

    Function:
        GFX_STATUS DRV_GFX_PaletteEnable()

     Summary:
        This function enables the palette mode feature of the library.

    Description:
        This function enables the palette mode feature of the library.
        This is a driver implemented function since the palette mode
        is dependent on the hardware used to support the feature.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Status of the palette mode feature enabling.
        GFX_STATUS_SUCCESS - feature was enabled successfully.
        GFX_STATUS_FAILURE - feature was not enabled.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteEnable(void);

// *****************************************************************************
/*
    <GROUP palette_functions>

    Function:
        GFX_STATUS DRV_GFX_PaletteDisable()

     Summary:
        This function disables the palette mode feature of the library.

    Description:
        This function disables the palette mode feature of the library.
        This is a driver implemented function since the palette mode
        is dependent on the hardware used to support the feature.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Status of the palette mode feature disabling.
        GFX_STATUS_SUCCESS - feature was disabled successfully.
        GFX_STATUS_FAILURE - feature was not disabled.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteDisable(void);

// *****************************************************************************
/*
    <GROUP palette_functions>

    Function:
        GFX_STATUS DRV_GFX_PaletteColorDepthSet()

     Summary:
        This function sets the color depth of the palette to be used
        with the palette mode feature of the library.

    Description:
        This function sets the color depth of the palette to be used
        with the palette mode feature of the library.
        This is a driver implemented function since the palette mode
        is dependent on the hardware used to support the feature.

    Precondition:
        None.

    Parameters:
        depth - the color depth to be set. Valid values are 1, 4 and 8.

    Returns:
        Status of the color depth modification.
        GFX_STATUS_SUCCESS - color depth setting was successful.
        GFX_STATUS_FAILURE - color depth setting modification failed.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteColorDepthSet(uint8_t depth);

// *****************************************************************************
/*
    <GROUP palette_functions>

    Function:
        GFX_STATUS GFX_PaletteSet()

    Summary:
        This function sets the palette to be used to render pixels
        to the display.

    Description:
        This function sets the palette to be used to render pixels.
        Any pixels rendered to the display buffer will be using the
        index of the color in the palette that is specified by this
        function. It is possible to modify a section of the palette
        by specifying the start and count parameters.

        The start and count must be within bounds of the supported
        palette size. The function will return GFX_STATUS_FAILURE
        if the start value and/or start + count exceeds the size
        of the palette. Size of the palette is dependent on the
        color depth set by GFX_CONFIG_COLOR_DEPTH configuration.

    Precondition:
        None.

    Parameters:
        pPalette - pointer to the palette resource to be used.
        start - specifies the starting index of the entries to
                be modified.
        count - specifies the number entries to be modified.

    Returns:
        Status of the palette modification.
        GFX_STATUS_SUCCESS - all entries specified were modified.
        GFX_STATUS_FAILURE - one or more entries was not modified or
                             the specified start and start + count
                             exceeds the palette size.

    Example:
        <code>

            extern const GFX_RESOURCE_HDR screen1_8bpp_palette;
            uint16_t count;

            count = screen1_8bpp_palette.resource.palette.numberOfEntries;

            GFX_PaletteSet((void*)&screen1_8bpp_palette, 0, count);
 
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_PaletteSet(
                                GFX_RESOURCE_HDR *pPalette,
                                uint16_t start,
                                uint16_t count);

// *****************************************************************************
/*
    <GROUP palette_functions>

    Function:
        GFX_STATUS DRV_GFX_PaletteSet()

    Summary:
        This is a driver specific palette set function.

    Description:
        This is a driver specific implementation of the palette set
        function. This function may change implementation from one
        driver to another. The implementation will depend on the
        hardware support of the palette feature.

        The start and count must be within bounds of the supported
        palette size. The function will return GFX_STATUS_FAILURE
        if the start value and/or start + count exceeds the size
        of the palette. Size of the palette is dependent on the
        color depth set by GFX_CONFIG_COLOR_DEPTH configuration.

    Precondition:
        DRV_GFX_PaletteInitialize() should be called prior to the
        first call of this function.

    Parameters:
        pPaletteEntry - pointer to the palette entries to be used.
        start - specifies the starting index of the entries to
                be modified.
        count - specifies the number entries to be modified.

    Returns:
        Status of the palette modification.
        GFX_STATUS_SUCCESS - all entries specified were modified.
        GFX_STATUS_FAILURE - one or more entries was not modified or
                             the specified start and start + count
                             exceeds the palette size.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteSet(
                                const GFX_PALETTE_ENTRY *pPaletteEntry,
                                uint16_t start,
                                uint16_t count);

#endif
#endif // _GFX_PALETTE_H
