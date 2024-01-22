#include "taskDeplacementRelatif.h"
#include "RobotObj.h"
#include "TaskManager.h"
#include "Define.h"
#include "TrajectoryGenerator.h"
#include "Asservissement.h"
#include "main.h"

TaskState stateDeplacementRelatif;

unsigned char taskDeplacementRelatifFinished = 0;

void TaskDeplacementRelatifInit(double xDist, double yDist, double deltaAngle)
{   
    //Initialisation du controleur de trajectoire
    TrajectoryGeneratorInitNewTrajectory(xDist,yDist,deltaAngle);
    
    //Lancement de la tache de deplacement
    stateDeplacementRelatif.stateNumber = STATE_TASK_DEPLACEMENT_EN_COURS;
    stateDeplacementRelatif.initialStateActionDone = 0;
    taskDeplacementRelatifFinished = 0;
}

void TaskDeplacementRelatifLoop(void)
{
    switch (stateDeplacementRelatif.stateNumber)
    {
        case STATE_TASK_DEPLACEMENT_EN_COURS:
        {
            TrajectoryGeneratorGetNextPosition();
//            robotState.xErreurDistance = robotState.trajectoryGeneratorCourant.xDistanceCouranteTrajectoireTheorique - robotState.xDistanceOdometrie;
//            robotState.yErreurDistance = robotState.trajectoryGeneratorCourant.yDistanceCouranteTrajectoireTheorique - robotState.yDistanceOdometrie;
//            robotState.thetaErreurAngle = robotState.trajectoryGeneratorCourant.thetaAngleCourantTrajectoireTheorique - robotState.thetaAngleRadianOdometry;
            //Calcul de l'ecart d'avancement entre la theorie et le realise
            if(robotState.trajectoryGeneratorCourant.isMovementFinished)
            {
                robotState.xErreur = 0;
                robotState.yErreur = 0;
                robotState.thetaErreur = 0;
                stateDeplacementRelatif.stateNumber = STATE_TASK_DEPLACEMENT_FINISHED;
                stateDeplacementRelatif.initialStateActionDone = 0;
            }
            break;
        }
        case STATE_TASK_DEPLACEMENT_FINISHED:
        {
            taskDeplacementRelatifFinished = 1;
            robotState.xVitesseConsigne = 0;
            robotState.yVitesseConsigne = 0;
            break;
        }
    }
}

unsigned char TaskDeplacementRelatifIsFinished()
{
    return taskDeplacementRelatifFinished;
}