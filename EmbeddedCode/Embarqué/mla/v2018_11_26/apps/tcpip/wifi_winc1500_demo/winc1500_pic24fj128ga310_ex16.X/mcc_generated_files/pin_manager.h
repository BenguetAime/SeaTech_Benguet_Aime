/**
  System Interrupts Generated Driver File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the generated manager file for the MPLAB(c) Code Configurator device.  This manager
    configures the pins direction, initial state, analog setting.
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Description:
    This source file provides implementations for MPLAB(c) Code Configurator interrupts.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 3.16
        Device            :  PIC24FJ128GA310
        Version           :  1.02
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.26
        MPLAB             :  MPLAB X 3.20

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

*/

#ifndef _PIN_MANAGER_H
#define _PIN_MANAGER_H
/**
    Section: Includes
*/
#include <xc.h>
/**
    Section: Device Pin Macros
*/
/**
  @Summary
    Sets the GPIO pin, RA10, high using LATA10.

  @Description
    Sets the GPIO pin, RA10, high using LATA10.

  @Preconditions
    The RA10 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RA10 high (1)
    IO_RA10_WAKE_SetHigh();
    </code>

*/
#define IO_RA10_WAKE_SetHigh()          _LATA10 = 1
/**
  @Summary
    Sets the GPIO pin, RA10, low using LATA10.

  @Description
    Sets the GPIO pin, RA10, low using LATA10.

  @Preconditions
    The RA10 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RA10 low (0)
    IO_RA10_WAKE_SetLow();
    </code>

*/
#define IO_RA10_WAKE_SetLow()           _LATA10 = 0
/**
  @Summary
    Toggles the GPIO pin, RA10, using LATA10.

  @Description
    Toggles the GPIO pin, RA10, using LATA10.

  @Preconditions
    The RA10 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RA10
    IO_RA10_WAKE_Toggle();
    </code>

*/
#define IO_RA10_WAKE_Toggle()           _LATA10 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RA10.

  @Description
    Reads the value of the GPIO pin, RA10.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RA10
    postValue = IO_RA10_WAKE_GetValue();
    </code>

*/
#define IO_RA10_WAKE_GetValue()         _RA10
/**
  @Summary
    Configures the GPIO pin, RA10, as an input.

  @Description
    Configures the GPIO pin, RA10, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RA10 as an input
    IO_RA10_WAKE_SetDigitalInput();
    </code>

*/
#define IO_RA10_WAKE_SetDigitalInput()  _TRISA10 = 1
/**
  @Summary
    Configures the GPIO pin, RA10, as an output.

  @Description
    Configures the GPIO pin, RA10, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RA10 as an output
    IO_RA10_WAKE_SetDigitalOutput();
    </code>

*/
#define IO_RA10_WAKE_SetDigitalOutput() _TRISA10 = 0
/**
  @Summary
    Sets the GPIO pin, RA14, high using LATA14.

  @Description
    Sets the GPIO pin, RA14, high using LATA14.

  @Preconditions
    The RA14 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RA14 high (1)
    IO_RA14_EXINT_SetHigh();
    </code>

*/
#define IO_RA14_EXINT_SetHigh()          _LATA14 = 1
/**
  @Summary
    Sets the GPIO pin, RA14, low using LATA14.

  @Description
    Sets the GPIO pin, RA14, low using LATA14.

  @Preconditions
    The RA14 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RA14 low (0)
    IO_RA14_EXINT_SetLow();
    </code>

*/
#define IO_RA14_EXINT_SetLow()           _LATA14 = 0
/**
  @Summary
    Toggles the GPIO pin, RA14, using LATA14.

  @Description
    Toggles the GPIO pin, RA14, using LATA14.

  @Preconditions
    The RA14 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RA14
    IO_RA14_EXINT_Toggle();
    </code>

*/
#define IO_RA14_EXINT_Toggle()           _LATA14 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RA14.

  @Description
    Reads the value of the GPIO pin, RA14.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RA14
    postValue = IO_RA14_EXINT_GetValue();
    </code>

*/
#define IO_RA14_EXINT_GetValue()         _RA14
/**
  @Summary
    Configures the GPIO pin, RA14, as an input.

  @Description
    Configures the GPIO pin, RA14, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RA14 as an input
    IO_RA14_EXINT_SetDigitalInput();
    </code>

*/
#define IO_RA14_EXINT_SetDigitalInput()  _TRISA14 = 1
/**
  @Summary
    Configures the GPIO pin, RA14, as an output.

  @Description
    Configures the GPIO pin, RA14, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RA14 as an output
    IO_RA14_EXINT_SetDigitalOutput();
    </code>

*/
#define IO_RA14_EXINT_SetDigitalOutput() _TRISA14 = 0
/**
  @Summary
    Sets the GPIO pin, RA15, high using LATA15.

  @Description
    Sets the GPIO pin, RA15, high using LATA15.

  @Preconditions
    The RA15 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RA15 high (1)
    IO_RA15_EXINT_SetHigh();
    </code>

*/
#define IO_RA15_EXINT_SetHigh()          _LATA15 = 1
/**
  @Summary
    Sets the GPIO pin, RA15, low using LATA15.

  @Description
    Sets the GPIO pin, RA15, low using LATA15.

  @Preconditions
    The RA15 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RA15 low (0)
    IO_RA15_EXINT_SetLow();
    </code>

*/
#define IO_RA15_EXINT_SetLow()           _LATA15 = 0
/**
  @Summary
    Toggles the GPIO pin, RA15, using LATA15.

  @Description
    Toggles the GPIO pin, RA15, using LATA15.

  @Preconditions
    The RA15 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RA15
    IO_RA15_EXINT_Toggle();
    </code>

*/
#define IO_RA15_EXINT_Toggle()           _LATA15 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RA15.

  @Description
    Reads the value of the GPIO pin, RA15.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RA15
    postValue = IO_RA15_EXINT_GetValue();
    </code>

*/
#define IO_RA15_EXINT_GetValue()         _RA15
/**
  @Summary
    Configures the GPIO pin, RA15, as an input.

  @Description
    Configures the GPIO pin, RA15, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RA15 as an input
    IO_RA15_EXINT_SetDigitalInput();
    </code>

*/
#define IO_RA15_EXINT_SetDigitalInput()  _TRISA15 = 1
/**
  @Summary
    Configures the GPIO pin, RA15, as an output.

  @Description
    Configures the GPIO pin, RA15, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RA15 as an output
    IO_RA15_EXINT_SetDigitalOutput();
    </code>

*/
#define IO_RA15_EXINT_SetDigitalOutput() _TRISA15 = 0
/**
  @Summary
    Sets the GPIO pin, RD0, high using LATD0.

  @Description
    Sets the GPIO pin, RD0, high using LATD0.

  @Preconditions
    The RD0 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RD0 high (1)
    IO_RD0_CE_SetHigh();
    </code>

*/
#define IO_RD0_CE_SetHigh()          _LATD0 = 1
/**
  @Summary
    Sets the GPIO pin, RD0, low using LATD0.

  @Description
    Sets the GPIO pin, RD0, low using LATD0.

  @Preconditions
    The RD0 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RD0 low (0)
    IO_RD0_CE_SetLow();
    </code>

*/
#define IO_RD0_CE_SetLow()           _LATD0 = 0
/**
  @Summary
    Toggles the GPIO pin, RD0, using LATD0.

  @Description
    Toggles the GPIO pin, RD0, using LATD0.

  @Preconditions
    The RD0 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RD0
    IO_RD0_CE_Toggle();
    </code>

*/
#define IO_RD0_CE_Toggle()           _LATD0 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RD0.

  @Description
    Reads the value of the GPIO pin, RD0.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RD0
    postValue = IO_RD0_CE_GetValue();
    </code>

*/
#define IO_RD0_CE_GetValue()         _RD0
/**
  @Summary
    Configures the GPIO pin, RD0, as an input.

  @Description
    Configures the GPIO pin, RD0, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RD0 as an input
    IO_RD0_CE_SetDigitalInput();
    </code>

*/
#define IO_RD0_CE_SetDigitalInput()  _TRISD0 = 1
/**
  @Summary
    Configures the GPIO pin, RD0, as an output.

  @Description
    Configures the GPIO pin, RD0, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RD0 as an output
    IO_RD0_CE_SetDigitalOutput();
    </code>

*/
#define IO_RD0_CE_SetDigitalOutput() _TRISD0 = 0
/**
  @Summary
    Sets the GPIO pin, RG0, high using LATG0.

  @Description
    Sets the GPIO pin, RG0, high using LATG0.

  @Preconditions
    The RG0 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG0 high (1)
    IO_RG0_RESET_SetHigh();
    </code>

*/
#define IO_RG0_RESET_SetHigh()          _LATG0 = 1
/**
  @Summary
    Sets the GPIO pin, RG0, low using LATG0.

  @Description
    Sets the GPIO pin, RG0, low using LATG0.

  @Preconditions
    The RG0 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG0 low (0)
    IO_RG0_RESET_SetLow();
    </code>

*/
#define IO_RG0_RESET_SetLow()           _LATG0 = 0
/**
  @Summary
    Toggles the GPIO pin, RG0, using LATG0.

  @Description
    Toggles the GPIO pin, RG0, using LATG0.

  @Preconditions
    The RG0 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RG0
    IO_RG0_RESET_Toggle();
    </code>

*/
#define IO_RG0_RESET_Toggle()           _LATG0 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RG0.

  @Description
    Reads the value of the GPIO pin, RG0.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RG0
    postValue = IO_RG0_RESET_GetValue();
    </code>

*/
#define IO_RG0_RESET_GetValue()         _RG0
/**
  @Summary
    Configures the GPIO pin, RG0, as an input.

  @Description
    Configures the GPIO pin, RG0, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG0 as an input
    IO_RG0_RESET_SetDigitalInput();
    </code>

*/
#define IO_RG0_RESET_SetDigitalInput()  _TRISG0 = 1
/**
  @Summary
    Configures the GPIO pin, RG0, as an output.

  @Description
    Configures the GPIO pin, RG0, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG0 as an output
    IO_RG0_RESET_SetDigitalOutput();
    </code>

*/
#define IO_RG0_RESET_SetDigitalOutput() _TRISG0 = 0
/**
  @Summary
    Sets the GPIO pin, RG1, high using LATG1.

  @Description
    Sets the GPIO pin, RG1, high using LATG1.

  @Preconditions
    The RG1 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG1 high (1)
    IO_RG1_CE_SetHigh();
    </code>

*/
#define IO_RG1_CE_SetHigh()          _LATG1 = 1
/**
  @Summary
    Sets the GPIO pin, RG1, low using LATG1.

  @Description
    Sets the GPIO pin, RG1, low using LATG1.

  @Preconditions
    The RG1 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG1 low (0)
    IO_RG1_CE_SetLow();
    </code>

*/
#define IO_RG1_CE_SetLow()           _LATG1 = 0
/**
  @Summary
    Toggles the GPIO pin, RG1, using LATG1.

  @Description
    Toggles the GPIO pin, RG1, using LATG1.

  @Preconditions
    The RG1 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RG1
    IO_RG1_CE_Toggle();
    </code>

*/
#define IO_RG1_CE_Toggle()           _LATG1 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RG1.

  @Description
    Reads the value of the GPIO pin, RG1.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RG1
    postValue = IO_RG1_CE_GetValue();
    </code>

*/
#define IO_RG1_CE_GetValue()         _RG1
/**
  @Summary
    Configures the GPIO pin, RG1, as an input.

  @Description
    Configures the GPIO pin, RG1, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG1 as an input
    IO_RG1_CE_SetDigitalInput();
    </code>

*/
#define IO_RG1_CE_SetDigitalInput()  _TRISG1 = 1
/**
  @Summary
    Configures the GPIO pin, RG1, as an output.

  @Description
    Configures the GPIO pin, RG1, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG1 as an output
    IO_RG1_CE_SetDigitalOutput();
    </code>

*/
#define IO_RG1_CE_SetDigitalOutput() _TRISG1 = 0
/**
  @Summary
    Sets the GPIO pin, RG14, high using LATG14.

  @Description
    Sets the GPIO pin, RG14, high using LATG14.

  @Preconditions
    The RG14 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG14 high (1)
    IO_RG14_RESET_SetHigh();
    </code>

*/
#define IO_RG14_RESET_SetHigh()          _LATG14 = 1
/**
  @Summary
    Sets the GPIO pin, RG14, low using LATG14.

  @Description
    Sets the GPIO pin, RG14, low using LATG14.

  @Preconditions
    The RG14 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG14 low (0)
    IO_RG14_RESET_SetLow();
    </code>

*/
#define IO_RG14_RESET_SetLow()           _LATG14 = 0
/**
  @Summary
    Toggles the GPIO pin, RG14, using LATG14.

  @Description
    Toggles the GPIO pin, RG14, using LATG14.

  @Preconditions
    The RG14 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RG14
    IO_RG14_RESET_Toggle();
    </code>

*/
#define IO_RG14_RESET_Toggle()           _LATG14 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RG14.

  @Description
    Reads the value of the GPIO pin, RG14.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RG14
    postValue = IO_RG14_RESET_GetValue();
    </code>

*/
#define IO_RG14_RESET_GetValue()         _RG14
/**
  @Summary
    Configures the GPIO pin, RG14, as an input.

  @Description
    Configures the GPIO pin, RG14, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG14 as an input
    IO_RG14_RESET_SetDigitalInput();
    </code>

*/
#define IO_RG14_RESET_SetDigitalInput()  _TRISG14 = 1
/**
  @Summary
    Configures the GPIO pin, RG14, as an output.

  @Description
    Configures the GPIO pin, RG14, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG14 as an output
    IO_RG14_RESET_SetDigitalOutput();
    </code>

*/
#define IO_RG14_RESET_SetDigitalOutput() _TRISG14 = 0
/**
  @Summary
    Sets the GPIO pin, RG7, high using LATG7.

  @Description
    Sets the GPIO pin, RG7, high using LATG7.

  @Preconditions
    The RG7 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG7 high (1)
    IO_RG7_MISO_SetHigh();
    </code>

*/
#define IO_RG7_MISO_SetHigh()          _LATG7 = 1
/**
  @Summary
    Sets the GPIO pin, RG7, low using LATG7.

  @Description
    Sets the GPIO pin, RG7, low using LATG7.

  @Preconditions
    The RG7 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG7 low (0)
    IO_RG7_MISO_SetLow();
    </code>

*/
#define IO_RG7_MISO_SetLow()           _LATG7 = 0
/**
  @Summary
    Toggles the GPIO pin, RG7, using LATG7.

  @Description
    Toggles the GPIO pin, RG7, using LATG7.

  @Preconditions
    The RG7 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RG7
    IO_RG7_MISO_Toggle();
    </code>

*/
#define IO_RG7_MISO_Toggle()           _LATG7 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RG7.

  @Description
    Reads the value of the GPIO pin, RG7.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RG7
    postValue = IO_RG7_MISO_GetValue();
    </code>

*/
#define IO_RG7_MISO_GetValue()         _RG7
/**
  @Summary
    Configures the GPIO pin, RG7, as an input.

  @Description
    Configures the GPIO pin, RG7, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG7 as an input
    IO_RG7_MISO_SetDigitalInput();
    </code>

*/
#define IO_RG7_MISO_SetDigitalInput()  _TRISG7 = 1
/**
  @Summary
    Configures the GPIO pin, RG7, as an output.

  @Description
    Configures the GPIO pin, RG7, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG7 as an output
    IO_RG7_MISO_SetDigitalOutput();
    </code>

*/
#define IO_RG7_MISO_SetDigitalOutput() _TRISG7 = 0
/**
  @Summary
    Sets the GPIO pin, RG9, high using LATG9.

  @Description
    Sets the GPIO pin, RG9, high using LATG9.

  @Preconditions
    The RG9 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG9 high (1)
    IO_RG9_SS2_SetHigh();
    </code>

*/
#define IO_RG9_SS2_SetHigh()          _LATG9 = 1
/**
  @Summary
    Sets the GPIO pin, RG9, low using LATG9.

  @Description
    Sets the GPIO pin, RG9, low using LATG9.

  @Preconditions
    The RG9 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Set RG9 low (0)
    IO_RG9_SS2_SetLow();
    </code>

*/
#define IO_RG9_SS2_SetLow()           _LATG9 = 0
/**
  @Summary
    Toggles the GPIO pin, RG9, using LATG9.

  @Description
    Toggles the GPIO pin, RG9, using LATG9.

  @Preconditions
    The RG9 must be set to an output.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Toggle RG9
    IO_RG9_SS2_Toggle();
    </code>

*/
#define IO_RG9_SS2_Toggle()           _LATG9 ^= 1
/**
  @Summary
    Reads the value of the GPIO pin, RG9.

  @Description
    Reads the value of the GPIO pin, RG9.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    uint16_t portValue;

    // Read RG9
    postValue = IO_RG9_SS2_GetValue();
    </code>

*/
#define IO_RG9_SS2_GetValue()         _RG9
/**
  @Summary
    Configures the GPIO pin, RG9, as an input.

  @Description
    Configures the GPIO pin, RG9, as an input.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG9 as an input
    IO_RG9_SS2_SetDigitalInput();
    </code>

*/
#define IO_RG9_SS2_SetDigitalInput()  _TRISG9 = 1
/**
  @Summary
    Configures the GPIO pin, RG9, as an output.

  @Description
    Configures the GPIO pin, RG9, as an output.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    // Sets the RG9 as an output
    IO_RG9_SS2_SetDigitalOutput();
    </code>

*/
#define IO_RG9_SS2_SetDigitalOutput() _TRISG9 = 0

/**
    Section: Function Prototypes
*/
/**
  @Summary
    Configures the pin settings of the PIC24FJ128GA310
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Description
    This is the generated manager file for the MPLAB(c) Code Configurator device.  This manager
    configures the pins direction, initial state, analog setting.
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    void SYSTEM_Initialize(void)
    {
        // Other initializers are called from this function
        PIN_MANAGER_Initialize();
    }
    </code>

*/
void PIN_MANAGER_Initialize(void);

#endif
