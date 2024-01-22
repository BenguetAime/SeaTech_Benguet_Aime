#include "UTLN_QEI.h"
#include <math.h>
#include "Define.h"
#include "RobotObj.h"
#include "UTLN_LS7366R.h"
#include "UTLN_Odometrie.h"
#include "UTLN_QEI.h"

#define MotorIndex(x) (x-1)
#define METER_PER_REV 0.39
#define B3508_REDUCTION_RATIO 19
double freqEchQei=0;

float pointToPositionSingleMotor[8];        //conversion points codeurs vers position (en radian ou mètre selon contexte)

PropulsionMode propulsionMode = FOUR_WHEELS;

double Qei1Position = 0;
double Qei2Position = 0;
double Qei3Position = 0;
double Qei4Position = 0;
double Qei5Position = 0;
double Qei6Position = 0;
double Qei7Position = 0;
double Qei8Position = 0;


double Qei1Position_T_1 = 0;
double Qei2Position_T_1 = 0;
double Qei3Position_T_1 = 0;
double Qei4Position_T_1 = 0;
double Qei5Position_T_1 = 0;
double Qei6Position_T_1 = 0;
double Qei7Position_T_1 = 0;
double Qei8Position_T_1 = 0;

double delta_1 = 0, delta_2 = 0, delta_3 = 0, delta_4 = 0, delta_5 = 0, delta_6 = 0,delta_7 =0,delta_8 =0, delta_theta = 0, dx = 0;

void SetPointToPositionSingleMotor(uint8 nbMotor, float value)
{
    pointToPositionSingleMotor[MotorIndex(nbMotor)] = value;
}

void QEIUpdateRawData(void)
{
    //On sauvegarde les anciennes valeurs
    Qei1Position_T_1 = Qei1Position;
    Qei2Position_T_1 = Qei2Position;
    Qei3Position_T_1 = Qei3Position;
    Qei4Position_T_1 = Qei4Position;
    Qei5Position_T_1 = Qei5Position;
    Qei6Position_T_1 = Qei6Position;
    Qei7Position_T_1 = Qei7Position;
    Qei8Position_T_1 = Qei8Position;

    //On réactualise les valeurs des positions    
    int32 QEI1RawValue = LS7366ReadCnt(1); //ENCODEUR 1
    int32 QEI2RawValue = LS7366ReadCnt(2); //ENCODEUR 2
    int32 QEI3RawValue = LS7366ReadCnt(3); //ENCODEUR 3
#ifdef WORKAROUND_EUROBOT
    int32 QEI4RawValue = LS7366ReadCnt(5); //ENCODEUR 4
#else
    int32 QEI4RawValue = LS7366ReadCnt(4); //ENCODEUR 4
#endif
    int32 QEI5RawValue = LS7366ReadCnt(5); //ENCODEUR 1
    int32 QEI6RawValue = LS7366ReadCnt(6); //ENCODEUR 2
    int32 QEI7RawValue = LS7366ReadCnt(7); //ENCODEUR 3
    int32 QEI8RawValue = LS7366ReadCnt(8); //ENCODEUR 4
    
    robotState.motor1EncoderRawValue=QEI1RawValue;
    robotState.motor2EncoderRawValue=QEI2RawValue;
    robotState.motor3EncoderRawValue=QEI3RawValue;
    robotState.motor4EncoderRawValue=QEI4RawValue;
    robotState.motor5EncoderRawValue=QEI5RawValue;
    robotState.motor6EncoderRawValue=QEI6RawValue;
    robotState.motor7EncoderRawValue=QEI7RawValue;
    robotState.motor8EncoderRawValue=QEI8RawValue;
    
    robotState.motorCurrentPosition[MotorIndex(1)] = pointToPositionSingleMotor[MotorIndex(1)] * QEI1RawValue;
    robotState.motorCurrentPosition[MotorIndex(2)] = pointToPositionSingleMotor[MotorIndex(2)] * QEI2RawValue;
    robotState.motorCurrentPosition[MotorIndex(3)] = pointToPositionSingleMotor[MotorIndex(3)] * QEI3RawValue;
    robotState.motorCurrentPosition[MotorIndex(4)] = pointToPositionSingleMotor[MotorIndex(4)] * QEI4RawValue;
    robotState.motorCurrentPosition[MotorIndex(5)] = pointToPositionSingleMotor[MotorIndex(5)] * QEI5RawValue;
    robotState.motorCurrentPosition[MotorIndex(6)] = pointToPositionSingleMotor[MotorIndex(6)] * QEI6RawValue;
    robotState.motorCurrentPosition[MotorIndex(7)] = pointToPositionSingleMotor[MotorIndex(7)] * QEI7RawValue;
    robotState.motorCurrentPosition[MotorIndex(8)] = pointToPositionSingleMotor[MotorIndex(8)] * QEI8RawValue;

}

void ComputePolarSpeed4WheelsFromQEI()
{
    //Doit être appelé après la fonction QEIUpdateRawData
    
    //Conversion des points codeurs en position
    Qei1Position = robotState.pointToDistance*robotState.motor1EncoderRawValue;
    Qei2Position = robotState.pointToDistance*robotState.motor2EncoderRawValue;
    Qei3Position = robotState.pointToDistance*robotState.motor3EncoderRawValue;
    Qei4Position = robotState.pointToDistance*robotState.motor4EncoderRawValue;
    
    //Calcul des deltas de position des encodeurs
    delta_1 = Qei1Position - Qei1Position_T_1;
    delta_2 = Qei2Position - Qei2Position_T_1;
    delta_3 = Qei3Position - Qei3Position_T_1;
    delta_4 = Qei4Position - Qei4Position_T_1;
    
    //Calcul des vitesses    
    robotState.Motor1SpeedOdometry = delta_1*FREQ_ECH_QEI; //attention ï¿½ remultiplier par la frï¿½quence d'ï¿½chantillonnage
    robotState.Motor2SpeedOdometry = delta_2*FREQ_ECH_QEI;
    robotState.Motor3SpeedOdometry = delta_3*FREQ_ECH_QEI;
    robotState.Motor4SpeedOdometry = delta_4*FREQ_ECH_QEI;
    
    //Transformation des vitesses v1/v2/v3/v4 en X/Y/Th
    TransformSpeed4WheelsToPolar();
}
//Fonction appelée a la place de void ComputePolarSpeed4WheelsFromQEI() dans le cas
//de l'utilisation de moteurs Brushless M3508 (codeurs integrés))
void ComputePolarSpeed4WheelsFromC620(void)
{
    //Calcul des vitesses    
    robotState.Motor1SpeedOdometry = -robotState.brushlessStatus[0].rotorSpeed*METER_PER_REV/60.0/B3508_REDUCTION_RATIO;
    robotState.Motor2SpeedOdometry = -robotState.brushlessStatus[1].rotorSpeed*METER_PER_REV/60.0/B3508_REDUCTION_RATIO;
    robotState.Motor3SpeedOdometry = -robotState.brushlessStatus[2].rotorSpeed*METER_PER_REV/60.0/B3508_REDUCTION_RATIO;
    robotState.Motor4SpeedOdometry = -robotState.brushlessStatus[3].rotorSpeed*METER_PER_REV/60.0/B3508_REDUCTION_RATIO;
    
    //Transformation des vitesses v1/v2/v3/v4 en X/Y/Th
    TransformSpeed4WheelsToPolar();
}

void ComputePolarSpeed2WheelsFromQEI()
{
    //Doit être appelé après la fonction QEIUpdateRawData
    
    //Conversion des points codeurs en position
    Qei1Position = robotState.pointToDistance*robotState.motor1EncoderRawValue;
    Qei2Position = robotState.pointToDistance*robotState.motor2EncoderRawValue;
    //On update quand meme les valeures des QEI 3 et 4 pour continuer la mesure sur l'odometrie des roues independantes (robot 2m annee)
    Qei3Position = robotState.pointToDistance*robotState.motor3EncoderRawValue;
    Qei4Position = robotState.pointToDistance*robotState.motor4EncoderRawValue;
    
    //Calcul des deltas de position des encodeurs
    delta_1 = Qei1Position - Qei1Position_T_1;
    delta_2 = Qei2Position - Qei2Position_T_1;
    //On update quand meme les valeures des QEI 3 et 4 pour continuer la mesure sur l'odometrie des roues independantes (robot 2m annee)
    delta_3 = Qei3Position - Qei3Position_T_1;
    delta_4 = Qei4Position - Qei4Position_T_1;
    
    //Calcul des vitesses    
    robotState.Motor1SpeedOdometry = delta_1*FREQ_ECH_QEI; //attention ï¿½ remultiplier par la frï¿½quence d'ï¿½chantillonnage
    robotState.Motor2SpeedOdometry = delta_2*FREQ_ECH_QEI;
    
    //On update quand meme les valeures des QEI 3 et 4 pour continuer la mesure sur l'odometrie des roues independantes (robot 2m annee)
    robotState.Motor3SpeedOdometry = delta_3*FREQ_ECH_QEI; //attention ï¿½ remultiplier par la frï¿½quence d'ï¿½chantillonnage
    robotState.Motor4SpeedOdometry = delta_4*FREQ_ECH_QEI;
    
    //Transformation des vitesses v1/v2/v3/v4 en X/Y/Th
    TransformSpeed2Wheels();
}

void ComputeSpeedAuxiliaryMotorsFromQEI(void)
{
    //Doit être appelé après la fonction QEIUpdateRawData
    
    //Conversion des points codeurs en position
    Qei5Position = robotState.pointToAngleAuxiliaryMotors*robotState.motor5EncoderRawValue;
    Qei6Position = robotState.pointToAngleAuxiliaryMotors*robotState.motor6EncoderRawValue;
    //On update quand meme les valeures des QEI 3 et 4 pour continuer la mesure sur l'odometrie des roues independantes (robot 2m annee)
    Qei7Position = robotState.pointToAngleAuxiliaryMotors*robotState.motor7EncoderRawValue;
    Qei8Position = robotState.pointToAngleAuxiliaryMotors*robotState.motor8EncoderRawValue;
    
    //Calcul des deltas de position des encodeurs
    delta_5 = Qei5Position - Qei5Position_T_1;
    delta_6 = Qei6Position - Qei6Position_T_1;
    //On update quand meme les valeures des QEI 3 et 4 pour continuer la mesure sur l'odometrie des roues independantes (robot 2m annee)
    delta_7 = Qei7Position - Qei7Position_T_1;
    delta_8 = Qei8Position - Qei8Position_T_1;
    
    //Calcul des vitesses    
    robotState.Motor5SpeedOdometry = delta_5*FREQ_ECH_QEI; //attention ï¿½ remultiplier par la frï¿½quence d'ï¿½chantillonnage
    robotState.Motor6SpeedOdometry = delta_6*FREQ_ECH_QEI;
    
    //On update quand meme les valeures des QEI 3 et 4 pour continuer la mesure sur l'odometrie des roues independantes (robot 2m annee)
    robotState.Motor7SpeedOdometry = delta_7*FREQ_ECH_QEI; //attention ï¿½ remultiplier par la frï¿½quence d'ï¿½chantillonnage
    robotState.Motor8SpeedOdometry = delta_8*FREQ_ECH_QEI;
}

double MX1, MX2, MX3, MX4;
double MY1, MY2, MY3, MY4;
double MTheta1, MTheta2, MTheta3, MTheta4;

void Init4WheelsToPolarMatrix(double mx1, double mx2, double mx3, double mx4,
                              double my1, double my2, double my3, double my4,
                              double mtheta1, double mtheta2, double mtheta3, double mtheta4)
{
    MX1 = mx1;
    MX2 = mx2;
    MX3 = mx3;
    MX4 = mx4;
    MY1 = my1;
    MY2 = my2;
    MY3 = my3;
    MY4 = my4;
    MTheta1 = mtheta1;
    MTheta2 = mtheta2;
    MTheta3 = mtheta3;
    MTheta4 = mtheta4;    
    //Règle de le mode de propulsion
    propulsionMode = FOUR_WHEELS;
}


double M1X, M1Y, M1Theta;
double M2X, M2Y, M2Theta;
double M3X, M3Y, M3Theta;
double M4X, M4Y, M4Theta;

void InitPolarTo4WheelsMatrix(double m1x, double m1y, double m1t,
                              double m2x, double m2y, double m2t,
                              double m3x, double m3y, double m3t,
                              double m4x, double m4y, double m4t)
{
    M1X = m1x;
    M1Y = m1y;
    M1Theta = m1t;
    M2X = m2x;
    M2Y = m2y;
    M2Theta = m2t;
    M3X = m3x;
    M3Y = m3y;
    M3Theta = m3t;
    M4X = m4x;
    M4Y = m4y;
    M4Theta = m4t;
}

void Init2WheelsToPolarMatrix(double mx1, double mx2,
                              double mtheta1, double mtheta2)
{
    MX1 = mx1;
    MX2 = mx2;
    MX3 = 0;
    MX4 = 0;
    MY1 = 0;
    MY2 = 0;
    MY3 = 0;
    MY4 = 0;    
    MTheta1 = mtheta1;
    MTheta2 = mtheta2;
    MTheta3 = 0;
    MTheta4 = 0;
    //Règle de le mode de propulsion
    propulsionMode = TWO_WHEELS;
}


double ConvertSpeedFromPolaireToIndependant(double vitesseX, double vitesseY, double vitesseTheta, int motorNumber) 
{
    switch(motorNumber)
    {
        case 1:
            return M1X* vitesseX + M1Y*vitesseY + M1Theta * vitesseTheta;
            //return -(sinAlpha1* vitesseX ) + cosAlpha1*vitesseY +(RayonRobot)*vitesseTheta;
            break;
        case 2:
            return M2X* vitesseX + M2Y*vitesseY + M2Theta * vitesseTheta;
            //return -(sinAlpha2* vitesseX ) + cosAlpha2*vitesseY +(RayonRobot)*vitesseTheta;
            break;
        case 3:
            return M3X* vitesseX + M3Y*vitesseY + M3Theta * vitesseTheta;
            //return -(sinAlpha3* vitesseX ) + cosAlpha3*vitesseY +(RayonRobot)*vitesseTheta;
            break;
        case 4:
            return M4X* vitesseX + M4Y*vitesseY + M4Theta * vitesseTheta;
            //return -(sinAlpha4* vitesseX ) + cosAlpha4*vitesseY +(RayonRobot)*vitesseTheta;
            break;
        default:
            return 0;
            break;
    }   
}

void TransformSpeed4WheelsToPolar()
{
    double v1 = robotState.Motor1SpeedOdometry;
    double v2 = robotState.Motor2SpeedOdometry;
    double v3 = robotState.Motor3SpeedOdometry;
    double v4 = robotState.Motor4SpeedOdometry;
    
    robotState.xVitesseFromOdometry = MX1 * v1 + MX2 * v2 + MX3 * v3 + MX4*v4;
    robotState.yVitesseFromOdometry = MY1 * v1 + MY2 * v2 + MY3 * v3 + MY4*v4;
    robotState.thetaVitesseFromOdometry = MTheta1 * v1 + MTheta2 * v2 + MTheta3 * v3 + MTheta4*v4;
}

void TransformSpeed2Wheels()
{
    double v1 = robotState.Motor1SpeedOdometry;
    double v2 = robotState.Motor2SpeedOdometry;
    
    robotState.xVitesseFromOdometry = MX1 * v1 + MX2 * v2;
    robotState.thetaVitesseFromOdometry = MTheta1 * v1 + MTheta2 * v2;
}

