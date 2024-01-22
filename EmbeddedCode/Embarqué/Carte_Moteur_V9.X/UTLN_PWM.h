/* 
 * File:   PWM.h
 * Author: E105-POSTE3
 *
 * Created on 14 septembre 2015, 11:51
 */

#ifndef PWM_H
#define	PWM_H

//#include "main.h"


//#define USE_LMD18200 

#define FORWARD 1
#define BACKWARD 0

#define MOTOR_1 1
#define MOTOR_2 2
#define MOTOR_3 3
#define MOTOR_4 4
#define MOTOR_5 5
#define MOTOR_6 6
#define MOTOR_7 7
#define MOTOR_8 8

//Definitions RoboCup
//#ifdef ROBOCUP 
//#define ALPHA1 1.2566370614359172953850573533118//(2*M_PI)/5.0
//#define ALPHA2 2.5132741228718345907701147066236//(4*M_PI)/5.0
//#define ALPHA3 3.7699111843077518861551720599354//(6*M_PI)/5.0
//#define ALPHA4 5.0265482457436691815402294132472//(8*M_PI)/5.0
//#endif
//#ifdef EUROBOT
//#define ALPHA1 1.029744258676655 //59°
//#define ALPHA2 2.513274122871835 //144°
//#define ALPHA3 3.769911184307752 //216°
//#define ALPHA4 5.253441048502932 //301°
//#endif

//#define MtoPERCENT 12.0
//#define MtoPERCENT_M5 12.0
//#define MtoPERCENT_M6 12.0
//#define MtoPERCENT_M7 12.0
//#define MtoPERCENT_M8 12.0

//#define PWM_FAST_DECAY
//#define USE_BD63150_BRAKE_MODE
//void Init4WheelsAngles(double alpha1, double alpha2, double alpha3, double alpha4);
//void Init2WheelsAngles(double alpha1, double alpha2);
void Set4WheelsMToPercentProp(double mToPercent);
double Get4WheelsMToPercentProp(void);
void SetMotorNMToPercent(uint8_t motorNum,double mToPercent);
void InitPWM(void);
//void PWMSetSpeedConsignePercent( double vitesseEnPourcents,unsigned char moteur);
void PWMSetSpeedCommandPercentMotorN(uint8_t motorNum,double vitesse);
void PWMSetSpeedConsignePolaire4Wheels(double vitesseX, double vitesseEnPourcentsY, double vitesseTheta);
void PWMSetSpeedConsignePolaire2Wheels(double vitesseX, double vitesseTheta);
void PWMSetSpeedCommandIndependant4Wheels(double vitesseMoteur1, double vitesseMoteur2, double vitesseMoteur3, double vitesseMoteur4);
//void PWMSetSpeedCommandIndependant2Wheels(double vitesseMoteur1, double vitesseMoteur2);
void PWMUpdateSpeed(void);
void PWMSetSpeedCommandMotor1(double vitesse);
void PWMSetSpeedCommandMotor2(double vitesse);
void PWMSetSpeedCommandMotor3(double vitesse);
void PWMSetSpeedCommandMotor4(double vitesse);
void PWMSetSpeedCommandMotor5(double vitesse);
void PWMSetSpeedCommandMotor6(double vitesse);
void PWMSetSpeedCommandMotor7(double vitesse);
void PWMSetSpeedCommandMotor8(double vitesse);
void ActivatePWM(bool enable);
void EnablePWM(uint8_t motorNum,bool enable);
#endif	/* PWM_H */

