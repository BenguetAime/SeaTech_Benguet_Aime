#include "TaskList.h"

#ifndef TASKMANAGERDEPLACEMENT_H
#define	TASKMANAGERDEPLACEMENT_H

#include "TaskManager.h"

extern TaskState stateTaskDeplacementManager;

typedef enum MoveOptions MoveOptions;
enum MoveOptions
{
    //Normal = 0,
    WithoutAvoidance = 1
};

void TaskManagerDeplacementInit();
void TaskRobotManagerDeplacementAddTask(unsigned char taskOption, int taskPayloadLength, unsigned char * taskPayload);
void TaskRobotManagerDeplacementAddGotoTask(unsigned char taskOptions, double X, double Y, double angle, unsigned char trajectoryType);
void TaskManagerRobotLoop();
int TaskRobotManagerDeplacementIsFinished();
Task GetCurrentTask();

#endif	/* TASKMANAGERROBOT_H */

