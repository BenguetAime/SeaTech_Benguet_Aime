#include <xc.h> // library xc.h inclut tous les uC
#include "HardwareProfile_CM_V9.h"
#include "Define.h"
#include "UTLN_PWM.h"
#include "RobotObj.h"
#include <math.h>
#include "UTLN_Timers.h"
#include "UTLN_OutputCompare.h"
#include "UTLN_Odometrie.h"
#include "Utilities.h"


#define PWMPER 40.0    
double talon = 50;
double m_toPercentProp=0;
double m_toPercentM1=0;
double m_toPercentM2=0;
double m_toPercentM3=0;
double m_toPercentM4=0;
double m_toPercentM5=0;
double m_toPercentM6=0;
double m_toPercentM7=0;
double m_toPercentM8=0;

void Set4WheelsMToPercentProp(double mToPercent)
{
    m_toPercentProp=mToPercent;
}
double Get4WheelsMToPercentProp(void)
{
    return m_toPercentProp;
}
void SetMotorNMToPercent(uint8_t motorNum,double mToPercent)
{
    switch(motorNum)
    {
        case 1:m_toPercentM1=mToPercent; break;
        case 2:m_toPercentM2=mToPercent;  break;
        case 3:m_toPercentM3=mToPercent;  break;
        case 4:m_toPercentM4=mToPercent;  break;
        case 5:m_toPercentM5=mToPercent;  break;
        case 6:m_toPercentM6=mToPercent;  break;
        case 7:m_toPercentM7=mToPercent;  break;
        case 8:m_toPercentM8=mToPercent;  break;
        default: break;
    }
}

#define DRIVE_MODE_INVERSE
void InitPWM(void) 
{
    //Test
    PWMCON1bits.IUE=1;  //1 = Updates to the active MDC/PDCx/SDCx registers are immediate
                        //0 = Updates to the active PDCx registers are synchronized to the PWM time base
    
    PTCON2bits.PCLKDIV = 0b000; //Divide by 1    
    PTPER = 100 * PWMPER; //Période en pourcentage
    

    //Réglage PWM moteur 1 sur hacheur 1
    IOCON1bits.PMOD=0b11;   //PWM I/O pin pair is in the True Independent Output mode
    FCLCON1 = 0x0003; //Désactive la gestion des faults
//    MOTOR1_ENLreg = 1;
//    MOTOR1_ENHreg = 1;
    
    IOCON2bits.PMOD=0b11;   //PWM I/O pin pair is in the True Independent Output mode
    FCLCON2 = 0x0003; //Désactive la gestion des faults
//    MOTOR2_ENLreg = 1;
//    MOTOR2_ENHreg = 1;
    
    IOCON3bits.PMOD=0b11;   //PWM I/O pin pair is in the True Independent Output mode
    FCLCON3 = 0x0003; //Désactive la gestion des faults
//    MOTOR3_ENLreg = 1;
//    MOTOR3_ENHreg = 1;
    
    IOCON4bits.PMOD=0b11;   //PWM I/O pin pair is in the True Independent Output mode
    FCLCON4 = 0x0003; //Désactive la gestion des faults
//    MOTOR4_ENLreg = 1;
//    MOTOR4_ENHreg = 1;
    
    IOCON5bits.PMOD=0b11;   //PWM I/O pin pair is in the True Independent Output mode
    FCLCON5 = 0x0003; //Désactive la gestion des faults
//    MOTOR5_ENLreg = 1;
//    MOTOR5_ENHreg = 1;

    IOCON6bits.PMOD=0b11;   //PWM I/O pin pair is in the True Independent Output mode
    FCLCON6 = 0x0003; //Désactive la gestion des faults
//    MOTOR6_ENLreg = 1;
//    MOTOR6_ENHreg = 1;

    IOCON7bits.PMOD=0b11;   //PWM I/O pin pair is in the True Independent Output mode
    FCLCON7 = 0x0003; //Désactive la gestion des faults
//    MOTOR7_ENLreg = 1;
//    MOTOR7_ENHreg = 1;

    /* Enable PWM Module */
    PTCONbits.PTEN = 1;
}
//
//void PWMSetSpeedConsignePercent(double vitesseEnPourcents, unsigned char moteur) {
//    //On borne les valeurs d'entrée des vitesse en pourcentage de la vitesse max.
//    //vitesseEnPourcents = LimitToInterval(vitesseEnPourcents, -100, 100);
//
//    switch (moteur) {
//        case MOTOR_1:
//            robotState.vitesseMoteur1CommandePercent = vitesseEnPourcents;
//            break;
//        case MOTOR_2:
//            robotState.vitesseMoteur2CommandePercent = vitesseEnPourcents;
//            break;
//        case MOTOR_3:
//            robotState.vitesseMoteur3CommandePercent = vitesseEnPourcents;
//            break;
//        case MOTOR_4:
//            robotState.vitesseMoteur4CommandePercent = vitesseEnPourcents;
//            break;
//        case MOTOR_5:
//            robotState.vitesseMoteur5CommandePercent = vitesseEnPourcents;
//            break;
//        case MOTOR_6:
//            robotState.vitesseMoteur6CommandePercent = vitesseEnPourcents;
//            break;
//        case MOTOR_7:
//            robotState.vitesseMoteur7CommandePercent = vitesseEnPourcents;
//            break;    
//        case MOTOR_8:
//            robotState.vitesseMoteur8CommandePercent = vitesseEnPourcents;
//            break;  
//    }
//}

//void ConvertSpeedFromPolaireToIndependant(double vitesseX, double vitesseY, double vitesseTheta, double* vitesseMoteurs) 
//{
//    //On borne les valeurs d'entrée des vitesse en pourcentage de la vitesse max.
//    //vitesseEnPourcents = LimitToInterval(vitesseEnPourcents, -100, 100);
//    vitesseMoteurs[0]= -(sinAlpha1* vitesseX ) + cosAlpha1*vitesseY +(RayonRobot)*vitesseTheta;
//    vitesseMoteurs[1]= -(sinAlpha2* vitesseX ) + cosAlpha2*vitesseY +(RayonRobot)*vitesseTheta;
//    vitesseMoteurs[2]= -(sinAlpha3* vitesseX ) + cosAlpha3*vitesseY +(RayonRobot)*vitesseTheta;    
//    vitesseMoteurs[3]= -(sinAlpha4* vitesseX ) + cosAlpha4*vitesseY +(RayonRobot)*vitesseTheta;
//}

void PWMSetSpeedConsignePolaire4Wheels(double vitesseX, double vitesseY, double vitesseTheta) 
{    
    robotState.Motor1SpeedConsigne = ConvertSpeedFromPolaireToIndependant(vitesseX, vitesseY, vitesseTheta, 1);
    robotState.Motor2SpeedConsigne = ConvertSpeedFromPolaireToIndependant(vitesseX, vitesseY, vitesseTheta, 2);
    robotState.Motor3SpeedConsigne = ConvertSpeedFromPolaireToIndependant(vitesseX, vitesseY, vitesseTheta, 3);
    robotState.Motor4SpeedConsigne = ConvertSpeedFromPolaireToIndependant(vitesseX, vitesseY, vitesseTheta, 4);
}

void PWMSetSpeedConsignePolaire2Wheels(double vitesseX, double vitesseTheta) 
{    
    robotState.Motor1SpeedConsigne = ConvertSpeedFromPolaireToIndependant(vitesseX, 0, vitesseTheta, 1);
    robotState.Motor2SpeedConsigne = ConvertSpeedFromPolaireToIndependant(vitesseX, 0, vitesseTheta, 2);
}


//void PWMSetSpeedCommandIndependant4Wheels(double vitesseMoteur1, double vitesseMoteur2, double vitesseMoteur3, double vitesseMoteur4) 
//{
////    On borne les valeurs d'entrée des vitesse en pourcentage de la vitesse max.
////    vitesseEnPourcents = LimitToInterval(vitesseEnPourcents, -100, 100);
//    robotState.vitesseMoteur1CommandePercent = vitesseMoteur1* m_toPercentProp;
//    robotState.vitesseMoteur2CommandePercent = vitesseMoteur2* m_toPercentProp;
//    robotState.vitesseMoteur3CommandePercent = vitesseMoteur3* m_toPercentProp;
//    robotState.vitesseMoteur4CommandePercent = vitesseMoteur4* m_toPercentProp;    
//}

//void PWMSetSpeedCommandIndependant2Wheels(double vitesseMoteur1, double vitesseMoteur2) 
//{
//    //On borne les valeurs d'entrée des vitesse en pourcentage de la vitesse max.
//    //vitesseEnPourcents = LimitToInterval(vitesseEnPourcents, -100, 100);
//    robotState.vitesseMoteur1CommandePercent = vitesseMoteur1* m_toPercentProp;
//    robotState.vitesseMoteur2CommandePercent = vitesseMoteur2* m_toPercentProp;
//}
void PWMSetSpeedCommandPercentMotorN(uint8_t motorNum,double vitesse)
{
    switch(motorNum)
    {
        case 1: robotState.vitesseMoteur1CommandePercent = vitesse; break;
        case 2: robotState.vitesseMoteur2CommandePercent = vitesse; break;
        case 3: robotState.vitesseMoteur3CommandePercent = vitesse; break;
        case 4: robotState.vitesseMoteur4CommandePercent = vitesse; break;
        case 5: robotState.vitesseMoteur5CommandePercent = vitesse; break;
        case 6: robotState.vitesseMoteur6CommandePercent = vitesse; break;
        case 7: robotState.vitesseMoteur7CommandePercent = vitesse; break;
        case 8: robotState.vitesseMoteur8CommandePercent = vitesse; break;
        default: break;
    }
    
}
void PWMSetSpeedCommandMotor1(double vitesse)
{
    robotState.vitesseMoteur1CommandePercent = LimitToInterval(vitesse * m_toPercentM1, -100, 100);
}
void PWMSetSpeedCommandMotor2(double vitesse)
{
    robotState.vitesseMoteur2CommandePercent = LimitToInterval(vitesse * m_toPercentM2, -100, 100);
}
void PWMSetSpeedCommandMotor3(double vitesse)
{
    robotState.vitesseMoteur3CommandePercent = LimitToInterval(vitesse * m_toPercentM3, -100, 100);
}
void PWMSetSpeedCommandMotor4(double vitesse)
{
    robotState.vitesseMoteur4CommandePercent = LimitToInterval(vitesse * m_toPercentM4, -100, 100);
}
void PWMSetSpeedCommandMotor5(double vitesse)
{
    robotState.vitesseMoteur5CommandePercent = LimitToInterval(vitesse * m_toPercentM5, -100, 100);
}
void PWMSetSpeedCommandMotor6(double vitesse)
{
    robotState.vitesseMoteur6CommandePercent = LimitToInterval(vitesse * m_toPercentM6, -100, 100);
}
void PWMSetSpeedCommandMotor7(double vitesse)
{
    robotState.vitesseMoteur7CommandePercent = LimitToInterval(vitesse * m_toPercentM7, -100, 100);
}
void PWMSetSpeedCommandMotor8(double vitesse)
{
    robotState.vitesseMoteur8CommandePercent = LimitToInterval(vitesse * m_toPercentM8, -100, 100);
}

//Slow Decay
void PWMUpdateSpeed(void) {
    //La gestion des rampes se fait à l'extérieur de la fonction
    if (robotState.vitesseMoteur1CommandePercent >= 0)
    {
        //FORWARD
        #ifndef DRIVE_MODE_INVERSE
            MOTOR1_DUTY_CYCLEreg = ABS(robotState.vitesseMoteur1CommandePercent) * PWMPER + 10;
            MOTOR1_DUTY_CYCLE_SECONDARYreg = 10;
        #else
            MOTOR1_DUTY_CYCLEreg = 100*PWMPER - talon;// ABS(robotState.vitesseMoteur1CommandePercent) * PWMPER + 10;
            MOTOR1_DUTY_CYCLE_SECONDARYreg = (100-ABS(robotState.vitesseMoteur1CommandePercent))*PWMPER + talon;
        #endif
    }
    else
    {   
        //BACKWARD
        #ifndef DRIVE_MODE_INVERSE
            MOTOR1_DUTY_CYCLEreg = 10;
            MOTOR1_DUTY_CYCLE_SECONDARYreg =  ABS(robotState.vitesseMoteur1CommandePercent) * PWMPER+ 10;
        #else
            MOTOR1_DUTY_CYCLEreg = (100-ABS(robotState.vitesseMoteur1CommandePercent))*PWMPER + talon;
            MOTOR1_DUTY_CYCLE_SECONDARYreg =  100*PWMPER - talon;
        #endif
    }
          
    if (robotState.vitesseMoteur2CommandePercent >= 0)
    {
        //FORWARD
        #ifndef DRIVE_MODE_INVERSE
            MOTOR2_DUTY_CYCLEreg = ABS(robotState.vitesseMoteur2CommandePercent) * PWMPER + 10;
            MOTOR2_DUTY_CYCLE_SECONDARYreg = 10;
        #else
            MOTOR2_DUTY_CYCLEreg = 100*PWMPER - talon;// ABS(robotState.vitesseMoteur2CommandePercent) * PWMPER + 10;
            MOTOR2_DUTY_CYCLE_SECONDARYreg = (100-ABS(robotState.vitesseMoteur2CommandePercent))*PWMPER + talon;
        #endif

    }
    else
    {   
        //BACKWARD             
        #ifndef DRIVE_MODE_INVERSE
            MOTOR2_DUTY_CYCLEreg = 10;
            MOTOR2_DUTY_CYCLE_SECONDARYreg =  ABS(robotState.vitesseMoteur2CommandePercent) * PWMPER+ 10;
        #else
            MOTOR2_DUTY_CYCLEreg = (100-ABS(robotState.vitesseMoteur2CommandePercent))*PWMPER + talon;
            MOTOR2_DUTY_CYCLE_SECONDARYreg =  100*PWMPER - talon;
        #endif
    }
        
    if (robotState.vitesseMoteur3CommandePercent >= 0)
    {
        #ifndef DRIVE_MODE_INVERSE
            MOTOR3_DUTY_CYCLEreg = ABS(robotState.vitesseMoteur3CommandePercent) * PWMPER + 10;
            MOTOR3_DUTY_CYCLE_SECONDARYreg = 10;
        #else
            MOTOR3_DUTY_CYCLEreg = 100*PWMPER - talon;// ABS(robotState.vitesseMoteur3CommandePercent) * PWMPER + 10;
            MOTOR3_DUTY_CYCLE_SECONDARYreg = (100-ABS(robotState.vitesseMoteur3CommandePercent))*PWMPER + talon;
        #endif
    }
    else
    {   
        //BACKWARD             
        #ifndef DRIVE_MODE_INVERSE
                MOTOR3_DUTY_CYCLEreg = 10;
                MOTOR3_DUTY_CYCLE_SECONDARYreg =  ABS(robotState.vitesseMoteur3CommandePercent) * PWMPER+ 10;
        #else
                MOTOR3_DUTY_CYCLEreg = (100-ABS(robotState.vitesseMoteur3CommandePercent))*PWMPER + talon;
                MOTOR3_DUTY_CYCLE_SECONDARYreg =  100*PWMPER - talon;
        #endif
    }
    
    if (robotState.vitesseMoteur4CommandePercent >= 0)
    {
        #ifndef DRIVE_MODE_INVERSE
            MOTOR4_DUTY_CYCLEreg = ABS(robotState.vitesseMoteur4CommandePercent) * PWMPER + 10;
            MOTOR4_DUTY_CYCLE_SECONDARYreg = 10;
        #else
            MOTOR4_DUTY_CYCLEreg = 100*PWMPER - talon;// ABS(robotState.vitesseMoteur4CommandePercent) * PWMPER + 10;
            MOTOR4_DUTY_CYCLE_SECONDARYreg = (100-ABS(robotState.vitesseMoteur4CommandePercent))*PWMPER + talon;
        #endif
    }
    else
    {   
        //BACKWARD             
        #ifndef DRIVE_MODE_INVERSE
            MOTOR4_DUTY_CYCLEreg = 10;
            MOTOR4_DUTY_CYCLE_SECONDARYreg =  ABS(robotState.vitesseMoteur4CommandePercent) * PWMPER+ 10;
        #else
            MOTOR4_DUTY_CYCLEreg = (100-ABS(robotState.vitesseMoteur4CommandePercent))*PWMPER + talon;
            MOTOR4_DUTY_CYCLE_SECONDARYreg =  100*PWMPER - talon;
        #endif
    }

    if (robotState.vitesseMoteur5CommandePercent >= 0)
    {
        #ifndef DRIVE_MODE_INVERSE
            MOTOR5_DUTY_CYCLEreg = ABS(robotState.vitesseMoteur5CommandePercent) * PWMPER + 10;
            MOTOR5_DUTY_CYCLE_SECONDARYreg = 10;
        #else
            MOTOR5_DUTY_CYCLEreg = 100*PWMPER - talon;// ABS(robotState.vitesseMoteur5CommandePercent) * PWMPER + 10;
            MOTOR5_DUTY_CYCLE_SECONDARYreg = (100-ABS(robotState.vitesseMoteur5CommandePercent))*PWMPER + talon;
        #endif
    }
    else
    {   
        //BACKWARD             
        #ifndef DRIVE_MODE_INVERSE
            MOTOR5_DUTY_CYCLEreg = 10;
            MOTOR5_DUTY_CYCLE_SECONDARYreg =  ABS(robotState.vitesseMoteur5CommandePercent) * PWMPER+ 10;
        #else
            MOTOR5_DUTY_CYCLEreg = (100-ABS(robotState.vitesseMoteur5CommandePercent))*PWMPER + talon;
            MOTOR5_DUTY_CYCLE_SECONDARYreg =  100*PWMPER - talon;
        #endif
    }
    
    if (robotState.vitesseMoteur6CommandePercent >= 0)
    {
        #ifndef DRIVE_MODE_INVERSE
            MOTOR6_DUTY_CYCLEreg = ABS(robotState.vitesseMoteur6CommandePercent) * PWMPER + 10;
            MOTOR6_DUTY_CYCLE_SECONDARYreg = 10;
        #else
            MOTOR6_DUTY_CYCLEreg = 100*PWMPER - talon;// ABS(robotState.vitesseMoteur6CommandePercent) * PWMPER + 10;
            MOTOR6_DUTY_CYCLE_SECONDARYreg = (100-ABS(robotState.vitesseMoteur6CommandePercent))*PWMPER + talon;
        #endif
    }
    else
    {   
        //BACKWARD             
        #ifndef DRIVE_MODE_INVERSE
            MOTOR6_DUTY_CYCLEreg = 10;
            MOTOR6_DUTY_CYCLE_SECONDARYreg =  ABS(robotState.vitesseMoteur6CommandePercent) * PWMPER+ 10;
        #else
            MOTOR6_DUTY_CYCLEreg = (100-ABS(robotState.vitesseMoteur6CommandePercent))*PWMPER + talon;
            MOTOR6_DUTY_CYCLE_SECONDARYreg =  100*PWMPER - talon;
        #endif
    }
    
    if (robotState.vitesseMoteur7CommandePercent >= 0)
    {
        #ifndef DRIVE_MODE_INVERSE
            MOTOR7_DUTY_CYCLEreg = ABS(robotState.vitesseMoteur7CommandePercent) * PWMPER + 10;
            MOTOR7_DUTY_CYCLE_SECONDARYreg = 10;
        #else
            MOTOR7_DUTY_CYCLEreg = 100*PWMPER - talon;// ABS(robotState.vitesseMoteur7CommandePercent) * PWMPER + 10;
            MOTOR7_DUTY_CYCLE_SECONDARYreg = (100-ABS(robotState.vitesseMoteur7CommandePercent))*PWMPER + talon;
        #endif
    }
    else
    {   
        //BACKWARD             
        #ifndef DRIVE_MODE_INVERSE
            MOTOR7_DUTY_CYCLEreg = 10;
            MOTOR7_DUTY_CYCLE_SECONDARYreg =  ABS(robotState.vitesseMoteur7CommandePercent) * PWMPER+ 10;
        #else
            MOTOR7_DUTY_CYCLEreg = (100-ABS(robotState.vitesseMoteur7CommandePercent))*PWMPER + talon;
            MOTOR7_DUTY_CYCLE_SECONDARYreg =  100*PWMPER - talon;
        #endif
    }
    
    
    //ATTENTION, pour le moteur 8, on utilise le module output compare en mode PWM
    //Les Timings et le fonctionnement sont differents 
    if (robotState.vitesseMoteur8CommandePercent >= 0)
    {
        //FORWARD
        SetOutputCompare1PWMDutyCycle(5);
        SetOutputCompare2PWMDutyCycle( ABS(robotState.vitesseMoteur8CommandePercent) * OCPER + 5);
    }
    else
    {   
        //BACKWARD    *
        SetOutputCompare1PWMDutyCycle( ABS(robotState.vitesseMoteur8CommandePercent) *OCPER  + 5);
        SetOutputCompare2PWMDutyCycle(5);
        
    }
}

void ActivatePWM(bool enable)
{
    if(enable)
        PTCONbits.PTEN = 1;
    else
        PTCONbits.PTEN = 0;
}

void EnablePWM(uint8_t motorNum,bool enable)
{
    switch(motorNum)
    {
        case MOTOR_1:
            if(enable)
            {
                MOTOR1_ENLreg = 1;
                MOTOR1_ENHreg = 1;
            }
            else
            {
                MOTOR1_ENLreg = 0;
                MOTOR1_ENHreg = 0;
            }
        break;
        case MOTOR_2:
            if(enable)
            {
                MOTOR2_ENLreg = 1;
                MOTOR2_ENHreg = 1;
            }
            else
            {
                MOTOR2_ENLreg = 0;
                MOTOR2_ENHreg = 0;
            }
        break;
        case MOTOR_3:
            if(enable)
            {
                MOTOR3_ENLreg = 1;
                MOTOR3_ENHreg = 1;
            }
            else
            {
                MOTOR3_ENLreg = 0;
                MOTOR3_ENHreg = 0;
            }
        break;
        case MOTOR_4:
            if(enable)
            {
                MOTOR4_ENLreg = 1;
                MOTOR4_ENHreg = 1;
            }
            else
            {
                MOTOR4_ENLreg = 0;
                MOTOR4_ENHreg = 0;
            }
        break;
        case MOTOR_5:
            if(enable)
            {
                MOTOR5_ENLreg = 1;
                MOTOR5_ENHreg = 1;
            }
            else
            {
                MOTOR5_ENLreg = 0;
                MOTOR5_ENHreg = 0;
            }
        break;
        case MOTOR_6:
            if(enable)
            {
                MOTOR6_ENLreg = 1;
                MOTOR6_ENHreg = 1;
            }
            else
            {
                MOTOR6_ENLreg = 0;
                MOTOR6_ENHreg = 0;
            }
        break;
        case MOTOR_7:
            if(enable)
            {
                MOTOR7_ENLreg = 1;
                MOTOR7_ENHreg = 1;
            }
            else
            {
                MOTOR7_ENLreg = 0;
                MOTOR7_ENHreg = 0;
            }
        break;
        case MOTOR_8:
            if(enable)
            {
                EnableOutputCompare1PWM();
                EnableOutputCompare2PWM();
            }
            else
            {
                DisableOutputCompare1PWM();
                DisableOutputCompare2PWM();
            }
        break;
        default: break;        
    }
}
