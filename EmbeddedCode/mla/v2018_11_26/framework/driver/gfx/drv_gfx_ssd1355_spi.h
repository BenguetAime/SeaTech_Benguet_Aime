/*******************************************************************************
 Display Driver for Microchip Graphics Library - Display Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_ssd1355_spi.h

  Summary:
    Display Driver header file for use with the Microchip Graphics Library.

  Description:
    This module implements the display driver for the following controllers:
    * Sino Wealth Microelectronic SH1101A OLED controller driver
    * Solomon Systech SSD1303 LCD controller driver
    This module implements the basic Display Driver Layer API required by the
    Microchip Graphics Library to enable, initialize and render pixels
    to the display controller.
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


#ifndef _SH1101A_SSD1351_OLED_H
#define _SH1101A_SSD1351_OLED_H

#include <stdint.h>
#include "system_config.h"

    #ifndef DISP_HOR_RESOLUTION
        #error "DISP_HOR_RESOLUTION must be defined in system_config.h"
    #endif
    #ifndef DISP_VER_RESOLUTION
        #error "DISP_VER_RESOLUTION must be defined in system_config.h"
    #endif
    #if (GFX_CONFIG_COLOR_DEPTH != 16)
        #error "GFX_CONFIG_COLOR_DEPTH must be defined to 16 in gfx_config.h"
    #endif
    #ifndef DISP_ORIENTATION
        #error "DISP_ORIENTATION must be defined in system_config.h"
    #endif

    #ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        #error GFX_CONFIG_DOUBLE_BUFFERING_DISABLE must be defined in system_config.h. This driver do not support double buffering feature.
    #endif

/*********************************************************************
* Overview: Horizontal and vertical screen size.
*********************************************************************/
    #if (DISP_HOR_RESOLUTION > 128)
        #error "This driver do not support this resolution. Supports at most 128 pixels in horizontal direction."
    #endif
    #if (DISP_VER_RESOLUTION > 128)
        #error "This driver do not support this resolution. Supports at most 128 pixels in horizontal direction."
    #endif

/*********************************************************************
* Overview: Display orientation.
*********************************************************************/
    #if (DISP_ORIENTATION != 0) && (DISP_ORIENTATION != 180)
        #error "This driver do not support this orientation. Set the DISP_ORIENTATION to 0."
    #endif


/*********************************************************************
* Overview: Defines the display offset in x direction. Dependent on the display 
* used and how it is connected.
*********************************************************************/
#ifndef DRV_GFX_X_OFFSET
#define DRV_GFX_X_OFFSET 0
#endif

#ifndef DRV_GFX_Y_OFFSET
#define DRV_GFX_Y_OFFSET 0
#endif



/*********************************************************************
* Overview: Screen Saver parameters. 
*   - SSON - Means that screen saver will be enabled when
*            ScreenSaver(SSON) function is called with SSON
*            as parameter.
*   - SSOFF - Means that screen saver will be disabled when
*            ScreenSaver(SSOFF) function is called with SSOFF
*            as parameter.
*           
*********************************************************************/
    #define SSON    1           // screen saver is turned on
    #define SSOFF   0           // screen saver is turned off

// Memory pitch for line
    #define LINE_MEM_PITCH  0x100

#endif // _SH1101A_SSD1303_OLED_H
