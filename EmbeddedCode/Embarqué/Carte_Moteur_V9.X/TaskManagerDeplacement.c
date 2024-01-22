#include "TaskManagerDeplacement.h"
#include "TaskManager.h"
#include "TaskAttente.h"
#include "taskDeplacementRelatif.h"
#include "RobotObj.h"
#include "HardwareProfile_CM_V6.h"
#include "Define.h"
#include "TrajectoryGenerator.h"
#include "QEI.h"
#include "TaskList.h"
#include "Asservissement.h"
#include <math.h>
#include <string.h>

TaskState stateTaskDeplacementManager;

TaskList taskDeplacementList;
TaskList oldListeDesTaches;
Task currentTask;

int TaskRobotManagerDeplacementFinished;

void TaskManagerDeplacementInit()
{
//    stateTaskDeplacementManager.stateNumber = TASK_DEBUT_MATCH;
    stateTaskDeplacementManager.initialStateActionDone = 0;
    InitTaskList(&taskDeplacementList);
}


int flagNewTaskDeplacementRequired = 0;

void TaskRobotManagerDeplacementAddTask(unsigned char taskOption, int taskPayloadLength, unsigned char * taskPayload) 
{
    //Toutes les tasks doivent commencer par un octet d'option d'insertion
    Task taskToAdd;
    taskToAdd.payloadLength = taskPayloadLength;
    int i;
    for (i = 0; i < taskPayloadLength; i++) {
        taskToAdd.payload[i] = taskPayload[i];
    }

    switch (taskOption) {
        case ReplaceAndClearAll:
            InitTaskList(&taskDeplacementList);
            Put(&taskDeplacementList, taskToAdd);
            flagNewTaskDeplacementRequired = 1; //On demande Ã  processer immÃ©diatement la prochaine tÃ¢che
            break;
        case TaskOptions_None:
            Put(&taskDeplacementList, taskToAdd);
            flagNewTaskDeplacementRequired = 1; //On demande Ã  processer immÃ©diatement la prochaine tÃ¢che
            break;
        case SetAsNextMove:
            PutInFirst(&taskDeplacementList, taskToAdd);
            flagNewTaskDeplacementRequired = 1; //On demande à processer immédiatement la prochaine tâche
            break;
    }

}
void TaskRobotManagerDeplacementAddGotoTask(unsigned char taskOptions, double X, double Y, double angle, unsigned char trajectoryType) 
{
    unsigned char payload[20];
    //TODO : modif importante qui doit etre supprimée
    //moveOptions = WithoutAvoidance;
    int pos = 0;
    X *= 1000;
    Y *= 1000;
    angle *= 1000;
    payload[pos++] = (unsigned char) (TaskType_GOTO);
    payload[pos++] = (unsigned char) ((long)X >> 24);
    payload[pos++] = (unsigned char) ((long)X >> 16);
    payload[pos++] = (unsigned char) ((long)X >> 8);
    payload[pos++] = (unsigned char) ((long)X >> 0);
    payload[pos++] = (unsigned char) ((long)Y >> 24);
    payload[pos++] = (unsigned char) ((long)Y >> 16);
    payload[pos++] = (unsigned char) ((long)Y >> 8);
    payload[pos++] = (unsigned char) ((long)Y >> 0);
    payload[pos++] = (unsigned char) ((long)angle >> 24);
    payload[pos++] = (unsigned char) ((long)angle >> 16);
    payload[pos++] = (unsigned char) ((long)angle >> 8);
    payload[pos++] = (unsigned char) ((long)angle >> 0);
    payload[pos++] = (unsigned char) trajectoryType;
    TaskRobotManagerDeplacementAddTask(taskOptions, pos, payload);
    
    TaskRobotManagerDeplacementFinished = 0;
}

double xDistance = 0;
double yDistance = 0;
double AngleCible = 0;
double XCible = 0;
double YCible = 0;
double angleRadian = 0;
TrajectoryTypes trajectoryType = Normal;
MoveOptions moveOptions;

double xSpeed = 0;
double ySpeed = 0;
double angularSpeed = 0;
double xAccel = 0;
double yAccel = 0;
double angularAccel = 0;
int duration = 0;

void TaskManagerRobotLoop() {

    if (flagNewTaskDeplacementRequired > 0) {
        flagNewTaskDeplacementRequired = 0;
        int nbRemainingTasks = GetNumberOfRemainingTasks(&taskDeplacementList);
        if (nbRemainingTasks > 0) {
            currentTask = Get(&taskDeplacementList);
            unsigned char pos = 0;
            unsigned char taskFunction = currentTask.payload[pos++];
            switch (taskFunction) {
                case TaskType_GOTO:
                {
                    double X = (double) ((long) currentTask.payload[pos] << 24)+ ((long) currentTask.payload[pos + 1] << 16) + ((long) currentTask.payload[pos + 2] << 8) + ((long) currentTask.payload[pos + 3] << 0);
                    pos += 4;
                    double Y = (double) ((long) currentTask.payload[pos] << 24) + ((long) currentTask.payload[pos + 1] << 16) + ((long) currentTask.payload[pos + 2] << 8) + ((long) currentTask.payload[pos + 3] << 0);
                    pos += 4;
                    double angle = (double) ((long) currentTask.payload[pos] << 24) + ((long) currentTask.payload[pos + 1] << 16) + ((long) currentTask.payload[pos + 2] << 8) + ((long) currentTask.payload[pos + 3] << 0);
                    pos += 4;
                    
                    unsigned char trajType = currentTask.payload[pos++];
                    trajectoryType = trajType;
                    X /= 1000.0;
                    Y /= 1000.0;
                    angle = angle /= 1000.0;
                    AngleCible = DegreeToRadian(angle);
                    xDistance = X - robotState.xPosFromOdometry;
                    yDistance = Y - robotState.yPosFromOdometry;

                    stateTaskDeplacementManager.stateNumber = TASK_GOTO;
                    stateTaskDeplacementManager.initialStateActionDone = 0;
                    break;
                }
            }
        } else {
            //il n'y aplus de task en attente
            stateTaskDeplacementManager.stateNumber = TASK_ATTENTE;
            stateTaskDeplacementManager.initialStateActionDone = 0;
            //SendDebugMessageToInterface((unsigned char*)"TaskRobotManagerDeplacement : ATTENTE\n");
        }
    } else if (flagNewTaskDeplacementRequired == 0) {
    }


    //Gestion des déplacements à l'aide d'une tâche 
    switch (stateTaskDeplacementManager.stateNumber) {
        case TASK_GOTO:
        {
            //Action a faire une fois
            if (stateTaskDeplacementManager.initialStateActionDone == 0) {
                stateTaskDeplacementManager.initialStateActionDone = 1;
                
                TrajectoryGeneratorSetTrajectoryType(trajectoryType);
                TrajectoryGeneratorSetParameters(robotState.xMaxSpeed, robotState.yMaxSpeed, robotState.thetaMaxSpeed, robotState.xAccel, robotState.yAccel, robotState.thetaAccel);
                TaskDeplacementRelatifInit(xDistance, yDistance, AngleCible);
                TaskRobotManagerDeplacementFinished = 0;
            }
            //Attente
            TaskDeplacementRelatifLoop();

            //Sortie du mode Deplacement Chaine
            if (TaskDeplacementRelatifIsFinished()) {
                stateTaskDeplacementManager.initialStateActionDone = 0;
                flagNewTaskDeplacementRequired = 1;
            }
            break;
        }

        case TASK_ATTENTE:
        {
            //Action a faire une fois
            if (stateTaskDeplacementManager.initialStateActionDone == 0) {
                stateTaskDeplacementManager.initialStateActionDone = 1;
                //Arrêt temporaire du robot
                robotState.thetaVitesseConsigne = 0;
                robotState.xVitesseConsigne = 0;
                robotState.yVitesseConsigne = 0;
                TaskRobotManagerDeplacementFinished = 1;
            }
            if (GetNumberOfRemainingTasks(&taskDeplacementList) > 0) {
                flagNewTaskDeplacementRequired = 1;
                stateTaskDeplacementManager.initialStateActionDone = 0;
            }
            break;
        }
    }
}

int TaskRobotManagerDeplacementIsFinished() {
    if (GetNumberOfRemainingTasks(&taskDeplacementList) == 0) {
        return TaskRobotManagerDeplacementFinished;
    } else {
        return 0;
    }

}

Task GetCurrentTask() {
    return currentTask;
}