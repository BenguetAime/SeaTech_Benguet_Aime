/* 
 * File:   TrajectoryGeneratorHolonome.h
 * Author: valen
 *
 * Created on 3 mai 2023, 09:18
 */

#ifndef TRAJECTORYGENERATORHOLONOME_H
#define	TRAJECTORYGENERATORHOLONOME_H

#include "Asservissement.h"

void ResetAllPositionPid();
void SetTrajectoryAccelerationSpeedConstants(float accelLineaire, float accelRotationCap, float accelRotationOrientation,
        float vitesseLineaire, float vitesseRotationCap, float vitesseRotationOrientation, float accelLineaireFreinage);
void InitRobotPosition(float x, float y, float theta);
void SetWaypointLocation(float x, float y, float theta);
void ResetGhostToCurrentPosition();
void PIDSpeedReset();
void PIDPositionReset();
void PIDSetup( PidCorrector* p_pid, float kp, float ki, float kd, float kpLimit, float kiLimit, float kdLimit);
void CalculateGhostPosition();
void UpdateAsservissementPosition();

#endif	/* TRAJECTORYGENERATORHOLONOME_H */

