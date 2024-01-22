#include <xc.h>
#include "TrajectoryGenerator.h"

#include "QEI.h"
#include "Math.h"
#include "RobotObj.h"
#include "Define.h"
#include "main.h"
#include "HardwareProfile_CM_V6.h"
#include "Asservissement.h"
#include "Utilities.h"
#include "Uart.h"
#include "UART_Protocol.h"

float tgXDistanceCible = 0;
float tgYDistanceCible = 0;
double tgThetaAngleCible = 0;
#define SEUIL_FIN_MVT_LINEAIRE 1.0f
#define SEUIL_FIN_MVT_ANGULAIRE 0.5/180.0*PI
#define SEUIL_DISTANCE_MVT_CHAINED 60
#define SEUIL_ECART_COLLISION 30.0f

int collisionOccured = 0;

void TrajectoryGeneratorSetParameters(float xMaxSpeed, float yMaxSpeed, float thetaMaxSpeed, float xAccel, float yAccel, float thetaAccel)
{
    robotState.xMaxSpeed = xMaxSpeed;
    robotState.yMaxSpeed = yMaxSpeed;
    robotState.thetaMaxSpeed = thetaMaxSpeed;
    robotState.xAccel = xAccel;
    robotState.yAccel = yAccel;
    robotState.thetaAccel = thetaAccel;
}

void TrajectoryGeneratorSetTrajectoryType(int chainedMode)
{
    robotState.TrajectoryType = chainedMode;
}

void TrajectoryGeneratorSetPositionToCurrentPosition()
{   
    robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique = robotState.thetaAngleRadianOdometry; //indispensable en sortie d'un set position Robot TODO:Kalman  
}

void TrajectoryGeneratorInitNewTrajectory(float distanceX, float distanceY, float angle)
{
    robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique = robotState.xDistanceOdometry;
    robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique = robotState.yDistanceOdometry;
    robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique = robotState.thetaAngleRadianOdometry;
    robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique = robotState.xAbsoluVitesseFromOdometry; //TODO : KALMAN
    robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique = robotState.yAbsoluVitesseFromOdometry; //TODO : KALMAN
    robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique = robotState.thetaVitesseFromOdometry; //TODO : KALMAN
    
    if(robotState.TrajectoryType == Reverse)
    {
        distanceX = -distanceX;
        angle = Modulo2PIAngleRadian(angle + PI);
    }
    tgXDistanceCible = robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique + distanceX;
    tgYDistanceCible = robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique + distanceY;
    tgThetaAngleCible = angle + robotState.thetaAngleRadianOdometry;
    
    robotState.trajectoryGeneratorCourant.isMovementFinished = false;
    robotState.trajectoryGeneratorCourant.isMovementFinishing = false;
}

long timestampBlockDetection = 0;
int blockDetectionInProgress = 0;

void TrajectoryGeneratorGetNextPosition()
{
    float incrementDistanceXTheorique = 0;
    float incrementDistanceYTheorique = 0;

    //Calcul de l'angle à parcourir avant arret sur une rampe
    float angleThetaArret = robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique * robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique / (2.0 * robotState.thetaAccel);

    //Calcul de la distance à parcourir avant arret sur une rampe
    float distanceXArret = robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique * robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique / (2.0 * robotState.xAccel);
    float distanceYArret = robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique * robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique / (2.0 * robotState.yAccel);
    
    //GENERATION DES CONSIGNES POUR X//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //On calcule les valeurs théoriques des rampes de vitesse
    double xArret = tgXDistanceCible - robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique;
    if (distanceXArret >= ABS(xArret)) //il faut freiner en rampe jusqu'à 0
    {
        if (tgXDistanceCible > robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique) //Si la cible est devant
        {
            double xVTh = robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique - robotState.xAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique = MAX(xVTh, 0);
        }
        else
        {
            double xVTh = robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique + robotState.xAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique = MIN(xVTh, 0);
        }
    }
    else //Sinon on accélère jusqu'à la vitesse lineaire max
    {
        if (tgXDistanceCible > robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique) //Si la cible est devant
        {
            double xVTh = robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique + robotState.xAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique = MIN(xVTh, robotState.xMaxSpeed);
        }
        else
        {
            double xVTh = robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique - robotState.xAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique = MAX(xVTh, -robotState.xMaxSpeed);
        }
    }
    incrementDistanceXTheorique = robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique / FREQ_ECH_QEI;
    robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique += incrementDistanceXTheorique;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //GENERATION DES CONSIGNES POUR Y/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //On calcule les valeurs théoriques des rampes de vitesse
    double yArret = tgYDistanceCible - robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique;
    if (distanceYArret >= ABS(yArret)) //il faut freiner en rampe jusqu'à 0
    {
        if (tgYDistanceCible > robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique) //Si la cible est devant
        {
            double yVTh = robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique - robotState.yAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique = MAX(yVTh, 0);
        }
        else
        {
            double yVTh = robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique + robotState.yAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique = MIN(yVTh, 0);
        }
    }
    else //Sinon on accélère jusqu'à la vitesse lineaire max
    {
        if (tgYDistanceCible > robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique) //Si la cible est devant
        {
            double yVTh = robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique + robotState.yAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique = MIN(yVTh, robotState.yMaxSpeed);
        }
        else
        {
            double yVTh = robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique - robotState.yAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique = MAX(yVTh, -robotState.yMaxSpeed);
        }
    }
    incrementDistanceYTheorique = robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique / FREQ_ECH_QEI;
    robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique += incrementDistanceYTheorique;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //GENERATION DES CONSIGNES POUR Theta/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //On calcule les valeurs théoriques des rampes de vitesse
    double thetaArret = tgThetaAngleCible - robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique;
    if (angleThetaArret >= ABS(thetaArret)) //il faut freiner en rampe jusqu'à 0
    {
        if (tgThetaAngleCible > robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique) //Si la cible est devant
        {
            double thetaVTh = robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique - robotState.thetaAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique = MAX(thetaVTh, 0);
        }
        else
        {
            double thetaVTh = robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique + robotState.thetaAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique = MIN(thetaVTh, 0);
        }
    }
    else //Sinon on accélère jusqu'à la vitesse lineaire max
    {
        if (tgThetaAngleCible > robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique) //Si la cible est devant
        {
            double thetaVTh = robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique + robotState.thetaAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique = MIN(thetaVTh, robotState.thetaMaxSpeed);
        }
        else
        {
            double thetaVTh = robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique - robotState.thetaAccel / FREQ_ECH_QEI;
            robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique = MAX(thetaVTh, -robotState.thetaMaxSpeed);
        }
    }/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    float incrementAngleTheorique = robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique / FREQ_ECH_QEI;
    robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique += incrementAngleTheorique;

    robotState.trajectoryGeneratorCourant.xPosTheorique += incrementDistanceXTheorique;
    robotState.trajectoryGeneratorCourant.yPosTheorique += incrementDistanceYTheorique;
    
    double xErreur = tgXDistanceCible - robotState.xDistanceOdometry;
    double yErreur = tgYDistanceCible - robotState.yDistanceOdometry;
    double angleErreur = tgThetaAngleCible - robotState.thetaAngleRadianOdometry;
    //Determine si le mouvement est terminé ou pas
    if (ABS(xErreur) < 0.010) //Seuil de fin de mouvement linéaire
    {
        if (ABS(yErreur) < 0.010)
        {
            if (ABS(angleErreur) < (0.5 / 180 * PI)) //Fin de mouvement angulaire a 0.5degrée
            {
                robotState.trajectoryGeneratorCourant.isMovementFinished = true;
            }
        }
    }
    
    
    //si on est en mode chained, on passe a l'étape suivante quand on approche de la cible.
    if (robotState.TrajectoryType == Chained && ((ABS(tgXDistanceCible - robotState.xDistanceOdometry) < 0.60) //Seuil de fin de mvmt
                                                  && (ABS(tgYDistanceCible - robotState.yDistanceOdometry) < 0.60)))
    {
        robotState.trajectoryGeneratorCourant.isMovementFinished = true;
    }
}

void SendTrajectoryData()
{    
    static int sendCounter = 0;
    if(sendCounter > 10)
    {
        sendCounter = 0;
        unsigned char payload[40];

        getBytesFromFloat(payload, 0, robotState.trajectoryGeneratorCourant.xVitesseTrajectoireTheorique);
        getBytesFromFloat(payload, 4, robotState.trajectoryGeneratorCourant.yVitesseTrajectoireTheorique);
        getBytesFromFloat(payload, 8, robotState.trajectoryGeneratorCourant.thetaVitesseTrajectoireTheorique);
        
        getBytesFromFloat(payload, 12, robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique);
        getBytesFromFloat(payload, 16, robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique);
        getBytesFromFloat(payload, 20, robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique);
        
        getBytesFromFloat(payload, 24, robotState.trajectoryGeneratorCourant.xPosTheorique);
        getBytesFromFloat(payload, 28, robotState.trajectoryGeneratorCourant.yPosTheorique);
        getBytesFromFloat(payload, 32, robotState.trajectoryGeneratorCourant.isMovementFinishing);
        getBytesFromFloat(payload, 36, robotState.trajectoryGeneratorCourant.isMovementFinishing);

        UART_EncodeAndSendMessage(UART_1, TrajectoryData, 40, payload);
    }
    sendCounter++;
}
