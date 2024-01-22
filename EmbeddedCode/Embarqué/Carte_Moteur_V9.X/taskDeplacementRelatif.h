/* 
 * File:   taskDeplacement.h
 * Author: E105-POSTE4
 *
 * Created on 8 mars 2017, 16:59
 */

#ifndef TASKDEPLACEMENT_H
#define	TASKDEPLACEMENT_H

#define STATE_TASK_DEPLACEMENT_EN_COURS 0
#define STATE_TASK_DEPLACEMENT_FINISHED 1

void TaskDeplacementRelatifInit(double xDist, double yDist, double deltaAngle);
void TaskDeplacementRelatifLoop(void);
unsigned char TaskDeplacementRelatifIsFinished();

#endif	/* TASKDEPLACEMENT_H */

