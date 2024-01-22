/* 
 * File:   UTLN_Imu_Processor.h
 * Author: valen
 *
 * Created on 2 mai 2023, 12:08
 */

#ifndef UTLN_IMU_PROCESSOR_H
#define	UTLN_IMU_PROCESSOR_H

typedef enum {
    XYZ = 0,
    RoboCup = 1,
    Eurobot = 2,
} ImuReferential;

void ProcessImuData(float accelX, float accelY, float accelZ, float gyroX, float gyroY, float gyroZ);
void SetImuReferential(ImuReferential ref);
void SetImuOffsets(float accelXOffset, float accelYOffset, float accelZOffset, float gyroXOffset, float gyroYOffset, float gyroZOffset);

#endif	/* UTLN_IMU_PROCESSOR_H */

