/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File: UTLN_CMPS12.h  
 * Author: Valentin BARCHASZ
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTLN_CMPS12_H
#define	UTLN_CMPS12_H

#include <xc.h> // include processor files - each processor file is guarded.  

#ifndef NULL
    #define NULL 0
#endif

#define MAX_CMPS_DEVICE 1
#define CMPS12_I2C_DEFAULT_ADDRESS 0xC0

//==============REGISTERS DEFINITION FOR I2C MODE=============================//
#define CMPS12_COMMAND_REG 0x00
#define CMPS12_COMPASS_BEARING 0x01
#define CMPS12_COMPASS_BEARING_MSB 0x02
#define CMPS12_COMPASS_BEARING_LSB 0x03
#define CMPS12_PITCH_ANGLE 0x04
#define CMPS12_ROLL_ANGLE 0x05
#define CMPS12_MAG_X_RAW_MSB 0x06
#define CMPS12_MAG_X_RAW_LSB 0x07
#define CMPS12_MAG_Y_RAW_MSB 0x08
#define CMPS12_MAG_Y_RAW_LSB 0x09
#define CMPS12_MAG_Z_RAW_MSB 0x0A
#define CMPS12_MAG_Z_RAW_LSB 0x0B
#define CMPS12_ACC_X_RAW_MSB 0x0C
#define CMPS12_ACC_X_RAW_LSB 0x0D
#define CMPS12_ACC_Y_RAW_MSB 0x0E
#define CMPS12_ACC_Y_RAW_LSB 0x0F
#define CMPS12_ACC_Z_RAW_MSB 0x10
#define CMPS12_ACC_Z_RAW_LSB 0x11
#define CMPS12_GYR_X_RAW_MSB 0x12
#define CMPS12_GYR_X_RAW_LSB 0x13
#define CMPS12_GYR_Y_RAW_MSB 0x14
#define CMPS12_GYR_Y_RAW_LSB 0x15
#define CMPS12_GYR_Z_RAW_MSB 0x16
#define CMPS12_GYR_Z_RAW_LSB 0x17
#define CMPS12_TEMP_MSB 0x18
#define CMPS12_TEMP_LSB 0x19
#define CMPS12_COMPASS_BEARING_16BIT_BNO55_MSB 0x1A
#define CMPS12_COMPASS_BEARING_16BIT_BNO55_LSB 0x1B
#define CMPS12_COMPASS_PITCH_ANGLE_MSB 0x1C
#define CMPS12_COMPASS_PITCH_ANGLE_LSB 0x1D
#define CMPS12_CALIBRATION_STATE 0x1E


//==============REGISTERS DEFINITION FOR UART MODE============================//
#define GET_VERSION 		0x11
#define GET_ANGLE8  		0x12
#define GET_ANGLE16 		0x13
#define GET_PITCH   		0x14
#define GET_ROLL    		0x15
#define GET_MAG_RAW 		0x21
#define GET_ACC_RAW 		0x22
#define READ_ANGLE_TILT 	0x23
#define CAL_BYTE1   		0x31
#define CAL_BYTE2   		0x45
#define CAL_BYTE3   		0x5A
#define CALIBRATE   		0x5E
#define CAL_EXIT    		0x90
#define RESTORE_CAL_BYTE1 	0x6A
#define RESTORE_CAL_BYTE2 	0x7C
#define RESTORE_CAL_BYTE3 	0x81


//Definition du pointeur de fonction des fonctions I2C
typedef void (*CMPS12USER_I2C_Func)( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* data, unsigned int length );

typedef struct CMPS12CONFIG{
  unsigned int* pModePort;
  unsigned char modePin;
  unsigned char mode;
  unsigned char i2cAddress;
}CMPS12Config;

typedef struct XYZVALUES{
    short x;
    short y;
    short z;
}XYZValues;

#endif	/* XC_HEADER_TEMPLATE_H */

