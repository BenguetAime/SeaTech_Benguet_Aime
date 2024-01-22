#include "TaskAttente.h"
#include "TaskManager.h"
#include "RobotObj.h"

TaskState stateAttente; 
unsigned char TaskAttenteFinished=0;

void TaskAttenteLoop(void) {
    switch (stateAttente.stateNumber) {
        case STATE_TASK_ATTENTE_INIT:
            if(stateAttente.initialStateActionDone==0)
            {
                stateAttente.initialStateActionDone = 1;
                robotState.timestamp = 0;
            }
            
            if (robotState.timestamp > 2000)
            {
                stateAttente.initialStateActionDone = 0;
                stateAttente.stateNumber = STATE_TASK_ATTENTE_FINISHED;
            }
            break; 
            
        case STATE_TASK_ATTENTE_FINISHED:            
            if(stateAttente.initialStateActionDone==0)
            {
                stateAttente.initialStateActionDone = 1;
                TaskAttenteFinished = 1;
            }
            //wait forever...
            break;
    }
}

unsigned char TaskAttenteIsFinished()
{
    return TaskAttenteFinished;
}

void TaskAttenteInit()
{
    stateAttente.stateNumber = STATE_TASK_ATTENTE_INIT;
    stateAttente.initialStateActionDone = 0;
}