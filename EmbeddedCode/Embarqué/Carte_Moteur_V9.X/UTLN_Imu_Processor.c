#include "UTLN_Imu_Processor.h"
#include "RobotObj.h"

float offsetAccelX = 0;
float offsetAccelY = 0;
float offsetAccelZ = 0;
float offsetGyroX = 0;
float offsetGyroY = 0;
float offsetGyroZ = 0;

ImuReferential ImuRef = RoboCup;

void ProcessImuData(float accelX, float accelY, float accelZ, float gyroX, float gyroY, float gyroZ) {
    switch (ImuRef) {
        case XYZ:
            robotState.accelDataXYZ.X = accelX;
            robotState.accelDataXYZ.Y = accelY;
            robotState.accelDataXYZ.Z = accelZ;

            robotState.gyroDataXYZ.X = gyroX;
            robotState.gyroDataXYZ.Y = gyroY;
            robotState.gyroDataXYZ.Z = gyroZ;

            break;
        case RoboCup:
            robotState.accelDataXYZ.X = accelY;
            robotState.accelDataXYZ.Y = -accelX;
            robotState.accelDataXYZ.Z = accelZ;

            robotState.gyroDataXYZ.X = gyroY;
            robotState.gyroDataXYZ.Y = -gyroX;
            robotState.gyroDataXYZ.Z = gyroZ;
            break;
        case Eurobot:
            robotState.accelDataXYZ.X = accelX;
            robotState.accelDataXYZ.Y = -accelZ;
            robotState.accelDataXYZ.Z = -accelY;

            robotState.gyroDataXYZ.X = gyroX;
            robotState.gyroDataXYZ.Y = -gyroZ;
            robotState.gyroDataXYZ.Z = gyroY;
            break;
    }
    robotState.accelDataXYZ.X -= offsetAccelX;
    robotState.accelDataXYZ.Y -= offsetAccelY;
    robotState.accelDataXYZ.Z -= offsetAccelZ;

    robotState.gyroDataXYZ.X -= offsetGyroX;
    robotState.gyroDataXYZ.Y -= offsetGyroY;
    robotState.gyroDataXYZ.Z -= offsetGyroZ;
}

void SetImuReferential(ImuReferential ref)
{
    ImuRef = ref;
}

void SetImuOffsets(float accelXOffset, float accelYOffset, float accelZOffset, float gyroXOffset, float gyroYOffset, float gyroZOffset)
{
    offsetAccelX = accelXOffset;
    offsetAccelY = accelYOffset;
    offsetAccelZ = accelZOffset;
    offsetGyroX = gyroXOffset;
    offsetGyroY = gyroYOffset;
    offsetGyroZ = gyroZOffset;
}
