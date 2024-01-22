#include "UTLN_KalmanFilter.h"
#include "RobotObj.h"

// Les variables utiles sont :
//        robotState.xVitesseFromOdometry - robotState.yVitesseFromOdometry  dans le référentiel Robot !!!!!!!!
//        robotState.thetaVitesseFromOdometry 

double alphaTheta = 0.01;
double alphaSpeed = 0.05;
double alphaPosition = 0.01;



void UpdateThetaUsingGyro()
{
    /// Mise à jour de l'état de Theta
    robotState.thetaPositionRefTerrain +=  robotState.gyroDataXYZ.Z / FREQ_ECH_QEI;    
    robotState.thetaSpeedRefTerrain = robotState.gyroDataXYZ.Z;
}

void UpdateThetaOnGNSS(float thetaGNSS)
{
    /// Intégration de l'observation
    robotState.thetaPositionRefTerrain = (1-alphaTheta)*robotState.thetaPositionRefTerrain + alphaTheta*thetaGNSS;
}

void SetAlphaTheta(float value)
{
    alphaTheta = value;
}

void SetAlphaSpeed(float value)
{
    alphaSpeed = value;
}
void SetAlphaPosition(float value)
{
    alphaPosition = value;
}
void UpdateVxVyUsingAccelAndOdometry()
{
    robotState.xSpeedKalmanRefRobot += robotState.accelDataXYZ.X / FREQ_ECH_QEI;
    robotState.ySpeedKalmanRefRobot += robotState.accelDataXYZ.Y / FREQ_ECH_QEI;
    
    robotState.xSpeedKalmanRefRobot = (1-alphaSpeed) * robotState.xSpeedKalmanRefRobot + alphaSpeed * robotState.xVitesseFromOdometry;
    robotState.ySpeedKalmanRefRobot = (1-alphaSpeed) * robotState.ySpeedKalmanRefRobot + alphaSpeed * robotState.yVitesseFromOdometry;
}

void UpdateXYUsingKalmanSpeed()
{
    robotState.xPositionRefTerrain += 
            (robotState.xSpeedKalmanRefRobot* cos(robotState.thetaPositionRefTerrain) 
            - robotState.ySpeedKalmanRefRobot* sin(robotState.thetaPositionRefTerrain)) /FREQ_ECH_QEI;
    robotState.yPositionRefTerrain += 
            (robotState.xSpeedKalmanRefRobot* sin(robotState.thetaPositionRefTerrain) 
            + robotState.ySpeedKalmanRefRobot* cos(robotState.thetaPositionRefTerrain)) /FREQ_ECH_QEI;
}

void UpdateXYOnGNSS(float xGNSS, float yGNSS)
{
    robotState.xPositionRefTerrain = (1-alphaPosition) * robotState.xPositionRefTerrain + alphaPosition * xGNSS;
    robotState.yPositionRefTerrain = (1-alphaPosition) * robotState.yPositionRefTerrain + alphaPosition * yGNSS;
}

