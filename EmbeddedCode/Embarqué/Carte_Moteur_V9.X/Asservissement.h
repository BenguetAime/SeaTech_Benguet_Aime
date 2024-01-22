/* 
 * File:   Asservissement.h
 * Author: E105-POSTE4
 *
 * Created on 3 mars 2017, 09:19
 */

#ifndef ASSERVISSEMENT_H
#define	ASSERVISSEMENT_H

#include "Asservissement.h"
#include <math.h>
#include "Define.h"
#include "UTLN_PWM.h"
#include "UTLN_LS7366R.h"
#include "Utilities.h"


typedef enum {
    ASSERVISSEMENT_DISABLED_4_WHEELS = 0,
    ASSERVISSEMENT_POLAIRE_4_WHEELS = 1,
    ASSERVISSEMENT_INDEPENDANT_4_WHEELS = 2,
    ASSERVISSEMENT_DISABLED_2_WHEELS = 10,
    ASSERVISSEMENT_POLAIRE_2_WHEELS=11,
    ASSERVISSEMENT_INDEPENDANT_2_WHEELS = 12,
}AsservissementVitesseState;

typedef enum {
    ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND= 0,
    ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED = 1,
    ASSERVISSEMENT_MOTOR_INDIVIDUAL_POSITION_ENABLED = 2,
}IndividualMotorAsservissementState;

typedef enum {
    ASSERVISSEMENT_POSITION_OFF = 0,
    ASSERVISSEMENT_POSITION_ON = 1,
}AsservissementPositionState;


typedef struct _PidCorrector
{
    double Kp;
    double Ki;
    double Kd;
    double erreurProportionelleMax;
    double erreurIntegraleMax;
    double erreurDeriveeMax;
    double erreurIntegrale;
    double epsilon_1;
    double erreur;
    
    //For Debug only
    double corrP;
    double corrI;
    double corrD;
}PidCorrector;

//void SetupPidAsservissementX(double Kp, double Ki, double Kd, double proportionelleMaxX, double integralMaxX, double deriveeMaxX);
void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMaxX, double integralMaxX, double deriveeMaxX);
double Correcteur(volatile PidCorrector* PidCorr, double erreur);
void ResetCorrecteur(volatile PidCorrector* PidCorr, double erreur);
void ResetAllSpeedPid();
void ResetIndividualMotorSpeedPid(unsigned char numMotor);
void SetupPidAsservissementY(double Kp, double Ki, double Kd, double proportionelleMaxY, double integralMaxY, double deriveeMaxY);
double CorrecteurY(double erreur);
void ResetCorrecteurY(double epsilon);
void SetupPidAsservissementTheta(double Kp, double Ki, double Kd, double proportionelleMaxTheta, double integralMaxTheta, double deriveeMaxTheta);
double CorrecteurTheta(double erreur);
void ResetCorrecteurTheta(double erreur);
void UpdateAsservissementVitesse();
//void SetAsservissementParameter(void);

#endif	/* ASSERVISSEMENT_H */

