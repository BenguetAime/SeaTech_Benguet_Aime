/*==============================================================================
Copyright 2016 Microchip Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "mcc.h"
#include "bsp.h"

static void InitializeBoard(void);

void BspInit(void)
{
    SYSTEM_Initialize(); // initializes PIC24 system clock, interrupt, timer, 
                         //  GPIO's, and UART via MCC-generated code.  This is an
                         //  MCC-generated function.
    
    SpiInit();           // Initialize SPI with custom driver (not using MCC for SPI)
    
    // Initialize Explorer16 LED/button ports
    InitializeBoard();
}

/****************************************************************************
  Function:
    void InitializeBoard(void)

  Description:
    This routine initializes the Explorer16 LED's
 
  Parameters:
    None

  Returns:
    None
 ***************************************************************************/
static void InitializeBoard(void)
{
    // LEDs
    LED0_TRIS = 0;
    LED1_TRIS = 0;
    LED2_TRIS = 0;
    LED3_TRIS = 0;
    LED4_TRIS = 0;
    LED5_TRIS = 0;
    LED6_TRIS = 0;
    LED7_TRIS = 0;
    LED_PUT(0x00);
}

 void ToggleLed(void)
 {
    LED0_IO ^= 1;   // toggle LED
 }
