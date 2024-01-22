/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using MPLAB(c) Code Configurator

  @Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.15
        Device            :  PIC18F87K22
        Version           :  1.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40

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


#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set IO_RB0_IRQ aliases
#define IO_RB0_IRQ_TRIS               TRISBbits.TRISB0
#define IO_RB0_IRQ_LAT                LATBbits.LATB0
#define IO_RB0_IRQ_PORT               PORTBbits.RB0
#define IO_RB0_IRQ_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define IO_RB0_IRQ_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define IO_RB0_IRQ_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define IO_RB0_IRQ_GetValue()           PORTBbits.RB0
#define IO_RB0_IRQ_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define IO_RB0_IRQ_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)

// get/set IO_RB1_RESET_N aliases
#define IO_RB1_RESET_N_TRIS               TRISBbits.TRISB1
#define IO_RB1_RESET_N_LAT                LATBbits.LATB1
#define IO_RB1_RESET_N_PORT               PORTBbits.RB1
#define IO_RB1_RESET_N_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define IO_RB1_RESET_N_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define IO_RB1_RESET_N_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define IO_RB1_RESET_N_GetValue()           PORTBbits.RB1
#define IO_RB1_RESET_N_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define IO_RB1_RESET_N_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)

// get/set IO_RB2_CHIP_EN aliases
#define IO_RB2_CHIP_EN_TRIS               TRISBbits.TRISB2
#define IO_RB2_CHIP_EN_LAT                LATBbits.LATB2
#define IO_RB2_CHIP_EN_PORT               PORTBbits.RB2
#define IO_RB2_CHIP_EN_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define IO_RB2_CHIP_EN_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define IO_RB2_CHIP_EN_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define IO_RB2_CHIP_EN_GetValue()           PORTBbits.RB2
#define IO_RB2_CHIP_EN_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define IO_RB2_CHIP_EN_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)

// get/set IO_RC0_CHIP_EN aliases
#define IO_RC0_CHIP_EN_TRIS               TRISCbits.TRISC0
#define IO_RC0_CHIP_EN_LAT                LATCbits.LATC0
#define IO_RC0_CHIP_EN_PORT               PORTCbits.RC0
#define IO_RC0_CHIP_EN_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define IO_RC0_CHIP_EN_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define IO_RC0_CHIP_EN_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define IO_RC0_CHIP_EN_GetValue()           PORTCbits.RC0
#define IO_RC0_CHIP_EN_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define IO_RC0_CHIP_EN_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)

// get/set IO_RC2_SPI_SS aliases
#define IO_RC2_SPI_SS_TRIS               TRISCbits.TRISC2
#define IO_RC2_SPI_SS_LAT                LATCbits.LATC2
#define IO_RC2_SPI_SS_PORT               PORTCbits.RC2
#define IO_RC2_SPI_SS_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define IO_RC2_SPI_SS_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define IO_RC2_SPI_SS_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define IO_RC2_SPI_SS_GetValue()           PORTCbits.RC2
#define IO_RC2_SPI_SS_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define IO_RC2_SPI_SS_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)

// get/set RC3 procedures
#define RC3_SetHigh()    do { LATCbits.LATC3 = 1; } while(0)
#define RC3_SetLow()   do { LATCbits.LATC3 = 0; } while(0)
#define RC3_Toggle()   do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define RC3_GetValue()         PORTCbits.RC3
#define RC3_SetDigitalInput()   do { TRISCbits.TRISC3 = 1; } while(0)
#define RC3_SetDigitalOutput()  do { TRISCbits.TRISC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()    do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()   do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()   do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()         PORTCbits.RC4
#define RC4_SetDigitalInput()   do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()  do { TRISCbits.TRISC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()    do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()   do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()   do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()         PORTCbits.RC5
#define RC5_SetDigitalInput()   do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()  do { TRISCbits.TRISC5 = 0; } while(0)

// get/set RC6 procedures
#define RC6_SetHigh()    do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()   do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()   do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()         PORTCbits.RC6
#define RC6_SetDigitalInput()   do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()  do { TRISCbits.TRISC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()    do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()   do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()   do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()         PORTCbits.RC7
#define RC7_SetDigitalInput()   do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()  do { TRISCbits.TRISC7 = 0; } while(0)

// get/set IO_RD0_LED aliases
#define IO_RD0_LED_TRIS               TRISDbits.TRISD0
#define IO_RD0_LED_LAT                LATDbits.LATD0
#define IO_RD0_LED_PORT               PORTDbits.RD0
#define IO_RD0_LED_SetHigh()            do { LATDbits.LATD0 = 1; } while(0)
#define IO_RD0_LED_SetLow()             do { LATDbits.LATD0 = 0; } while(0)
#define IO_RD0_LED_Toggle()             do { LATDbits.LATD0 = ~LATDbits.LATD0; } while(0)
#define IO_RD0_LED_GetValue()           PORTDbits.RD0
#define IO_RD0_LED_SetDigitalInput()    do { TRISDbits.TRISD0 = 1; } while(0)
#define IO_RD0_LED_SetDigitalOutput()   do { TRISDbits.TRISD0 = 0; } while(0)

// get/set IO_RD7_RESET_N aliases
#define IO_RD7_RESET_N_TRIS               TRISDbits.TRISD7
#define IO_RD7_RESET_N_LAT                LATDbits.LATD7
#define IO_RD7_RESET_N_PORT               PORTDbits.RD7
#define IO_RD7_RESET_N_SetHigh()            do { LATDbits.LATD7 = 1; } while(0)
#define IO_RD7_RESET_N_SetLow()             do { LATDbits.LATD7 = 0; } while(0)
#define IO_RD7_RESET_N_Toggle()             do { LATDbits.LATD7 = ~LATDbits.LATD7; } while(0)
#define IO_RD7_RESET_N_GetValue()           PORTDbits.RD7
#define IO_RD7_RESET_N_SetDigitalInput()    do { TRISDbits.TRISD7 = 1; } while(0)
#define IO_RD7_RESET_N_SetDigitalOutput()   do { TRISDbits.TRISD7 = 0; } while(0)

// get/set IO_RE2_SPI_SS aliases
#define IO_RE2_SPI_SS_TRIS               TRISEbits.TRISE2
#define IO_RE2_SPI_SS_LAT                LATEbits.LATE2
#define IO_RE2_SPI_SS_PORT               PORTEbits.RE2
#define IO_RE2_SPI_SS_SetHigh()            do { LATEbits.LATE2 = 1; } while(0)
#define IO_RE2_SPI_SS_SetLow()             do { LATEbits.LATE2 = 0; } while(0)
#define IO_RE2_SPI_SS_Toggle()             do { LATEbits.LATE2 = ~LATEbits.LATE2; } while(0)
#define IO_RE2_SPI_SS_GetValue()           PORTEbits.RE2
#define IO_RE2_SPI_SS_SetDigitalInput()    do { TRISEbits.TRISE2 = 1; } while(0)
#define IO_RE2_SPI_SS_SetDigitalOutput()   do { TRISEbits.TRISE2 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/