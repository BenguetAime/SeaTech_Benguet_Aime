
#include <math.h>
#include "TrajectoryGeneratorSingleMotor.h"
#include "Define.h"
#include "RobotObj.h"
#include <stdlib.h>


volatile TRAJECTORY_GENERATOR_SINGLE_MOTOR_BITS trajectoryGeneratorMotor[8];
volatile bool individualMotorPositionGhostError=false;
void SetSingleMotorTrajectoryParameters(uint8 nbMotor, float accelMax, float speedMax) {
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].accelMax = accelMax;
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].speedMax = speedMax;
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].ElapsedTimeBetweenCalculation = 1.0/FREQ_ECH_QEI;
}
bool IsIndividualMotorPositionGhostError(void)
{
    return individualMotorPositionGhostError;
}
void ClearIndividualMotorPositionGhostError(void)
{
    individualMotorPositionGhostError=false;
}
void SetSingleMotorTrajectoryErrorMaxParameter(uint8 nbMotor, float errorMax)
{
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].errorMax=errorMax;
}
void SetSingleMotorTargetPosition(uint8 nbMotor, float targetPosition) {
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].targetPosition = targetPosition;
}

void SetSingleMotorPidParameters(uint8 nbMotor, float Kp, float Ki, float Kd, float proportionelleMax, float integralMax, float deriveeMax)
{
    SetupPidAsservissement(&trajectoryGeneratorMotor[MotorIndex(nbMotor)].correcteur, Kp, Ki, Kd, proportionelleMax, integralMax, deriveeMax);
}

void ResetIndividualMotorToCurrentPosition(uint8_t nbMotor)
{
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].ghostPosition=trajectoryGeneratorMotor[MotorIndex(nbMotor)].currentPosition;
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].ghostSpeed=trajectoryGeneratorMotor[MotorIndex(nbMotor)].currentSpeed;
    
    ResetCorrecteur(&trajectoryGeneratorMotor[MotorIndex(nbMotor)].correcteur, 0);
    
    switch(nbMotor)
    {
        case 1:
            ResetCorrecteur(&robotState.PidSpeedMotor1, 0);
            robotState.Motor1SpeedConsigne = 0;
            break;
        case 2:
            ResetCorrecteur(&robotState.PidSpeedMotor2, 0);
            robotState.Motor2SpeedConsigne = 0;
            break;
        case 3:
            ResetCorrecteur(&robotState.PidSpeedMotor3, 0);
            robotState.Motor3SpeedConsigne = 0;
            break;
        case 4:
            ResetCorrecteur(&robotState.PidSpeedMotor4, 0);
            robotState.Motor4SpeedConsigne = 0;
            break;
        case 5:
            ResetCorrecteur(&robotState.PidSpeedMotor5, 0);
            robotState.Motor5SpeedConsigne = 0;
            break;
        case 6:
            ResetCorrecteur(&robotState.PidSpeedMotor6, 0);
            robotState.Motor6SpeedConsigne = 0;
            break;
        case 7:
            ResetCorrecteur(&robotState.PidSpeedMotor7, 0);
            robotState.Motor7SpeedConsigne = 0;
            break;
        case 8:
            ResetCorrecteur(&robotState.PidSpeedMotor8, 0);
            robotState.Motor8SpeedConsigne = 0;
            break;
        default: break;
    }
}

float UpdateSingleMotorTrajectory(uint8 nbMotor, float currentPosition) {
    
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].currentPosition = currentPosition;
    //Calcul de la distance au WayPoint
    double distanceDestination = trajectoryGeneratorMotor[MotorIndex(nbMotor)].targetPosition - trajectoryGeneratorMotor[MotorIndex(nbMotor)].ghostPosition;

    //Calcul de la vitesse linéaire du robot
    double ghostSpeed = trajectoryGeneratorMotor[MotorIndex(nbMotor)].ghostSpeed;

    //Calcul de la distance de freinage 
    double distanceFreinage = pow(ghostSpeed, 2) / (2 * trajectoryGeneratorMotor[MotorIndex(nbMotor)].accelMax);

    /* Fin du calcul des variéables intermédiaires */
    /************************ Ajustement de la vitesse linéaire du robot *******************************/
    // Si la distance à la destination est supérieure à la distance de freinage : on accélère en linéaire
    // sinon on freine
    double newSpeed;

    if (distanceDestination > 0) //La cible est devant
    {
        if (ABS(distanceDestination) > distanceFreinage)
            newSpeed = MIN(trajectoryGeneratorMotor[MotorIndex(nbMotor)].speedMax, ghostSpeed
                + trajectoryGeneratorMotor[MotorIndex(nbMotor)].accelMax * trajectoryGeneratorMotor[MotorIndex(nbMotor)].ElapsedTimeBetweenCalculation); //On accélère
        else
            //On détermine la valeur du freinage en fonction des conditions
            newSpeed = MAX(0, ghostSpeed
                - trajectoryGeneratorMotor[MotorIndex(nbMotor)].accelMax * trajectoryGeneratorMotor[MotorIndex(nbMotor)].ElapsedTimeBetweenCalculation); //On freine
    } else //Le WayPoint est derrière
    {
        if (ABS(distanceDestination) > distanceFreinage)
            newSpeed = MAX(-trajectoryGeneratorMotor[MotorIndex(nbMotor)].speedMax, ghostSpeed
                - trajectoryGeneratorMotor[MotorIndex(nbMotor)].accelMax * trajectoryGeneratorMotor[MotorIndex(nbMotor)].ElapsedTimeBetweenCalculation); //On accélère
        else
            //On détermine la valeur du freinage en fonction des conditions
            newSpeed = MIN(0, ghostSpeed
                + trajectoryGeneratorMotor[MotorIndex(nbMotor)].accelMax * trajectoryGeneratorMotor[MotorIndex(nbMotor)].ElapsedTimeBetweenCalculation); //On freine
    }
    
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].ghostSpeed = newSpeed;
    
    trajectoryGeneratorMotor[MotorIndex(nbMotor)].ghostPosition += newSpeed*trajectoryGeneratorMotor[MotorIndex(nbMotor)].ElapsedTimeBetweenCalculation;
    
    ///Calcul de l'assservissement
    float erreur = trajectoryGeneratorMotor[MotorIndex(nbMotor)].ghostPosition - currentPosition;
    float consigneMoteur=0;
    
    //Si on a une erreur trop importante
    if(ABS(erreur)>trajectoryGeneratorMotor[MotorIndex(nbMotor)].errorMax)
    {
        //On arrete le moteur
        consigneMoteur=0;
        trajectoryGeneratorMotor[MotorIndex(nbMotor)].hasGhostError=true;
        individualMotorPositionGhostError=true;//On indique q'une erreur a lieu
    }
    else
    {
        consigneMoteur = Correcteur(&trajectoryGeneratorMotor[MotorIndex(nbMotor)].correcteur, erreur);    
    }
    
    return consigneMoteur;
}