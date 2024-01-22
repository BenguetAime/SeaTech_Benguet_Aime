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
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTLN_ICM20948_H
#define	UTLN_ICM20948_H
#include "UTLN_Typedefs.h"
#include <xc.h> // include processor files - each processor file is guarded.  
#ifndef bool
#define bool unsigned char
#define true (1)
#define false (0)
#endif
#define SPI_DELAY 500
typedef void (*ICM20948USER_SPI_Func)(unsigned char command[],int nbWrite, unsigned char results[], int nbResults, unsigned char ChipSelectID);

void wait_ms(unsigned short ms);
void ICM20948RegisterUserSystemClock(unsigned long ticksPerSec);
void ICM20948RegisterUserSPIFunc(ICM20948USER_SPI_Func func);
void ICM20948SPIWriteSingleMultipleRead(unsigned char command, unsigned char results[], int nbRead, unsigned char csID);
void ICM20948SPIWriteMultipleMultipleRead(unsigned char command[],int nbWrite, unsigned char results[], int nbRead, unsigned char csID);
void ICM20948WriteRegister(unsigned char regAddress,unsigned char value, unsigned char csID);
void ICM20948WriteRegisterDelay(unsigned char regAddress,unsigned char value, unsigned char csID);
unsigned char ICM20948ReadRegister(unsigned char regAddress, unsigned char csID);
unsigned char ICM20948ReadRegisterDelay(unsigned char regAddress, unsigned char csID);
void ICM20948Reset(unsigned char csID);
unsigned char ICM20948Init_default(unsigned char csID);
void ICM20948SetGyroSMPLRTDiv(unsigned char div,unsigned char csID);
void ICM20948SetAccelSMPLRTDiv(unsigned short div,unsigned char csID);
unsigned char* ICM20948GetAccelValues(unsigned char csID);
void ICM20948GetAccelValues_(unsigned char* data,unsigned char csID);
unsigned char* ICM20948GetGyroValues(unsigned char csID);
unsigned char* ICM20948GetTempValues(unsigned char csID);
//========================ADVANCED FUNCTION===================================//
float ICM20948SetGyroSampleRate(float sampleRate, unsigned char csID);
float ICM20948SetAccelSampleRate(float sampleRate, unsigned char csID);
void ICM20948SetSampleRate(float sampleRate,unsigned char csID);
void ICM20948SetGyroBandwidth(uint8_t gyroBw, unsigned char csID);
void ICM20948SetAccelBandwidth(uint8_t accelBw, unsigned char csID);
void ICM20948SetAccelResolution(uint8_t accelFs, unsigned char csID);
//void ICM20948GetAccelResolution(float *accelRes, unsigned char csID);
float ICM20948GetAccelResolutionConstant();
void ICM20948SetGyroResolution(uint8_t accelFs, unsigned char csID);
//void ICM20948GetGyroResolution(float *accelRes, unsigned char csID);
float ICM20948GetGyroResolutionConstant();
void ICM20948SetGyroFullscale(uint8_t gyroFs, unsigned char csID);
void ICM20648EnableSleepmode(bool enable, unsigned char csID);
void ICM20648EnableCyclemode(bool enable, unsigned char csID);
void ICM20948EnableSensor(bool accel, bool gyro, bool temp, unsigned char csID);
void ICM20948EnterLowPowerMode(bool enAccel, bool enGyro, bool enTemp, unsigned char csID);
void ICM20948EnableIrq(bool dataReadyEnable, bool womEnable, unsigned char csID);
void ICM20948EnableWakeOnMotion(bool enable, uint8_t womThreshold, float sampleRate, unsigned char csID);
//void ICM20948Calibrate(float *accelBiasScaled, float *gyroBiasScaled,unsigned char csID);
//uint32_t ICM20948CalibrateGyro(float *gyroBiasScaled, unsigned char csID);
void ICM20948ReadTemperature(float *temperature, unsigned char csID);


void ICM20948ConfigureMstI2CBusForAK09916(unsigned char csID);
void ICM20948ReadFromAK09916(unsigned char reg,unsigned char * buf,unsigned char count, unsigned char csID);
void ICM20948ConfigureAK09916(unsigned char csID);
#endif	/* XC_HEADER_TEMPLATE_H */

