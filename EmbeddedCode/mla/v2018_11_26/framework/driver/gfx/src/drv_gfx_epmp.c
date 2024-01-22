/*******************************************************************************
 Module for Microchip Graphics Library - Enhanced Parallel Master Port (EPMP) Driver 

  Company:
    Microchip Technology Inc.

  File Name:
    gfxempm.c

  Summary:
    Driver for use with the Microchip Graphics Library.

  Description:
    This module maps the memory space located in Extended Data Space (EDS).       
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

#include "system.h"
#include <stdint.h>

#ifdef USE_GFX_EPMP

// this is just a dummy address to set up EDS access on epmp
#if defined (GFX_EPMP_CS1_BASE_ADDRESS)
    // we are adding a -2 here so we do not overlap the base address of CS1
    // if CS1 base address is used. Since this address is dummy we do not 
    // care the value. We just want the EDS access to be set up.
    #define EPMP_REGION_START_ADDRESS (GFX_EPMP_CS1_BASE_ADDRESS-2)
#else    
    #define EPMP_REGION_START_ADDRESS (0x00020000ul)
#endif

#ifdef USE_16BIT_PMP
volatile __eds__ uint16_t __attribute__((eds,address(EPMP_REGION_START_ADDRESS),noload,section("epmp_data_for_CPU_access"))) epmp_data;

#else
volatile __eds__ uint8_t __attribute__((eds,address(EPMP_REGION_START_ADDRESS),noload,section("epmp_data_for_CPU_access"))) epmp_data;
#endif

#endif // USE_GFX_EPMP
