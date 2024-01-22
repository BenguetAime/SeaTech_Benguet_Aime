/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx.h

  Summary:
    The header file joins all header files used in the graphics library
    and contains compile options and defaults.

  Description:
    This header file includes all the header files required to use the
    Microchip Graphics Library. Library features and options defined
    in the Graphics Library configurations will be included in each build.
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

#ifndef _GFX_H
// DOM-IGNORE-BEGIN
    #define _GFX_H
// DOM-IGNORE-END

/////////////////////// GRAPHICS_LIBRARY_VERSION /////////////////////
// MSB is version, LSB is subversion
#define GRAPHICS_LIBRARY_VERSION    0x0490

////////////// GRAPHICS_LIBRARY_RESOURCE_FORMAT_VERSION //////////////
#define GFX_RESOURCE_FORMAT_VERSION 0x0101

////////////////////////////// INCLUDES //////////////////////////////
#include "gfx/gfx_types_macros.h"           
#include "driver/gfx/drv_gfx_display.h"
#include "gfx/gfx_primitive.h"
#include "gfx/gfx_colors.h"

#ifndef GFX_CONFIG_GOL_DISABLE
#include "gfx/gfx_gol.h"
#include "gfx/gfx_gol_scan_codes.h"
#include "gfx/gfx_gol_button.h"
#include "gfx/gfx_gol_check_box.h"
#include "gfx/gfx_gol_digital_meter.h"
#include "gfx/gfx_gol_edit_box.h"
#include "gfx/gfx_gol_group_box.h"
#include "gfx/gfx_gol_list_box.h"
#include "gfx/gfx_gol_meter.h"
#include "gfx/gfx_gol_picture.h"
#include "gfx/gfx_gol_progress_bar.h"
#include "gfx/gfx_gol_radio_button.h"
#include "gfx/gfx_gol_scroll_bar.h"
#include "gfx/gfx_gol_static_text.h"
#include "gfx/gfx_gol_text_entry.h"
#include "gfx/gfx_gol_window.h"
#endif

#endif // _GFX_H
