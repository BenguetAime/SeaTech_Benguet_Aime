/* 
 * File:   TrajectoryGeneratorSingleMotor.h
 * Author: valen
 *
 * Created on 24 avril 2023, 10:23
 */

#ifndef TRAJECTORYGENERATORSINGLEMOTOR_H
#define	TRAJECTORYGENERATORSINGLEMOTOR_H

#include "Asservissement.h"

typedef struct trajectoryGeneratorSingleMotorBITS {
    float ghostPosition;
    float ghostSpeed;
    float currentPosition;
    float currentSpeed;
    float targetPosition;
    
    float errorMax;
    bool hasGhostError;

    float accelMax;
    float speedMax;
    
    float ElapsedTimeBetweenCalculation;

    PidCorrector correcteur;
} TRAJECTORY_GENERATOR_SINGLE_MOTOR_BITS;

void SetSingleMotorTrajectoryParameters(uint8 nbMotor, float accelMax, float speedMax);
bool IsIndividualMotorPositionGhostError(void);
void ClearIndividualMotorPositionGhostError(void);
void SetSingleMotorTrajectoryErrorMaxParameter(uint8 nbMotor, float errorMax);
void SetSingleMotorTargetPosition(uint8 nbMotor, float targetPosition);
void SetSingleMotorPidParameters(uint8 nbMotor, float Kp, float Ki, float Kd, float proportionelleMax, float integralMax, float deriveeMax);
float UpdateSingleMotorTrajectory(uint8 nbMotor, float currentPosition);
void ResetIndividualMotorToCurrentPosition(uint8_t nbMotor);

#endif	/* TRAJECTORYGENERATORSINGLEMOTOR_H */

