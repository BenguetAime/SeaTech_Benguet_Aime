/*
 * SPISMIOT.h
 *
 *  Created on: 28 janv. 2020
 *      Author: TP-EO-6
 */

#ifndef TASKSPISENSORS_SPISMIOT_SPISMIOT_H_
#define TASKSPISENSORS_SPISMIOT_SPISMIOT_H_

#define Board_ICM20948_ID           0x01
#define Board_LIS3MDL_ID            0x02
#define Board_LIS3DSH_ID            0x03
#define Board_ADS129X_ID            0x04

void SPI_SMIOT_Init(void);
void SPI_Communication(unsigned char command[],int nbWrite, unsigned char results[], int nbRead, unsigned char csID);


#endif /* TASKSPISENSORS_SPISMIOT_SPISMIOT_H_ */
