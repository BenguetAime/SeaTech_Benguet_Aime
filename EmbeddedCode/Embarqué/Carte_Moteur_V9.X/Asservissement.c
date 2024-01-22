#include "Asservissement.h"
#include <math.h>
#include "Define.h"
#include "RobotObj.h"
#include "UTLN_PWM.h"
#include "UTLN_LS7366R.h"
#include "Utilities.h"
#include "UTLN_Odometrie.h"
#include "TrajectoryGeneratorSingleMotor.h"

#define DEBUG_PID
double Te = 1.0 / FREQ_ECH_QEI;

//************************** CORRECTEUR VITESSE X *****************************

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax, double deriveeMax)
{    
    PidCorr->Kp = Kp;       
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd; 
    PidCorr->erreurDeriveeMax = deriveeMax; 
}

double Correcteur(volatile PidCorrector* PidCorr, double erreur)
{
    //double erreurProportionnelleX = erreur;
    PidCorr->erreur = erreur;
    double erreurProportionnelle = LimitToInterval(erreur, -PidCorr->erreurProportionelleMax/PidCorr->Kp, PidCorr->erreurProportionelleMax/PidCorr->Kp);
    PidCorr->corrP = PidCorr->Kp*erreurProportionnelle;
    
    PidCorr->erreurIntegrale += erreur*Te;    
    PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale, -PidCorr->erreurIntegraleMax/PidCorr->Ki, PidCorr->erreurIntegraleMax/PidCorr->Ki);
    PidCorr->corrI = PidCorr->Ki*PidCorr->erreurIntegrale;
    
    double erreurDerivee = (erreur - PidCorr->epsilon_1)*FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax/PidCorr->Kd, PidCorr->erreurDeriveeMax/PidCorr->Kd);
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;
    
    return PidCorr->corrP+PidCorr->corrI+PidCorr->corrD;
}

void ResetCorrecteur(volatile PidCorrector* pidCorr, double erreur)
{
    pidCorr->erreurIntegrale = 0;
    pidCorr->epsilon_1 = erreur;
}

void ResetAllSpeedPid()
{
    ResetCorrecteur(&robotState.PidSpeedX,0);
    ResetCorrecteur(&robotState.PidSpeedY,0);
    ResetCorrecteur(&robotState.PidSpeedTheta,0);
    ResetCorrecteur(&robotState.PidSpeedMotor1,0);
    ResetCorrecteur(&robotState.PidSpeedMotor2,0);
    ResetCorrecteur(&robotState.PidSpeedMotor3,0);
    ResetCorrecteur(&robotState.PidSpeedMotor4,0);
    ResetCorrecteur(&robotState.PidSpeedMotor5,0);
    ResetCorrecteur(&robotState.PidSpeedMotor6,0);
    ResetCorrecteur(&robotState.PidSpeedMotor7,0);
    ResetCorrecteur(&robotState.PidSpeedMotor8,0);    
}

void ResetIndividualMotorSpeedPid(unsigned char numMotor)
{
    switch(numMotor)
    {
        case 1:
            ResetCorrecteur(&robotState.PidSpeedMotor1,0);
            break;
        case 2:
            ResetCorrecteur(&robotState.PidSpeedMotor2,0);
            break;
        case 3:
            ResetCorrecteur(&robotState.PidSpeedMotor3,0);
            break;
        case 4:
            ResetCorrecteur(&robotState.PidSpeedMotor4,0);
            break;
        case 5:
            ResetCorrecteur(&robotState.PidSpeedMotor5,0);
            break;
        case 6:
            ResetCorrecteur(&robotState.PidSpeedMotor6,0);
            break;
        case 7:
            ResetCorrecteur(&robotState.PidSpeedMotor7,0);
            break;
        case 8:
            ResetCorrecteur(&robotState.PidSpeedMotor8,0);
            break;
    }
}

void UpdateAsservissementVitesse()
{
    //Asservissement state for 2 or 4 wheels robots
    switch(robotState.robotSpeedAsservissementState)
    {      
        case ASSERVISSEMENT_DISABLED_2_WHEELS:
            ResetCorrecteur(&robotState.PidSpeedX, 0);
            ResetCorrecteur(&robotState.PidSpeedY, 0);
            ResetCorrecteur(&robotState.PidSpeedTheta, 0);
            //On Set les M_ToPercent des moteurs individuellement
            SetMotorNMToPercent(1,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(2,Get4WheelsMToPercentProp());
            PWMSetSpeedConsignePolaire2Wheels(robotState.XSpeedConsigne, robotState.ThetaSpeedConsigne);            
            break;
            
        case ASSERVISSEMENT_POLAIRE_2_WHEELS:
        {
            /********************************************ASSERVISSEMENT VITESSE*********************************************************************/
            robotState.PidSpeedX.erreur = robotState.XSpeedConsigne - robotState.xVitesseFromOdometry;
            robotState.PidSpeedTheta.erreur = robotState.ThetaSpeedConsigne - robotState.thetaVitesseFromOdometry;
            
            robotState.xCorrectionSpeedPercent = Correcteur(&robotState.PidSpeedX, robotState.PidSpeedX.erreur);    
            robotState.thetaCorrectionSpeedPercent = Correcteur(&robotState.PidSpeedTheta, robotState.PidSpeedTheta.erreur);            
            //On desactive les asservissement independants des moteurs 1 a 2
            robotState.asservissementMoteur1State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            robotState.asservissementMoteur2State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            //On Set les M_ToPercent des moteurs individuellement
            SetMotorNMToPercent(1,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(2,Get4WheelsMToPercentProp());
            PWMSetSpeedConsignePolaire2Wheels(robotState.xCorrectionSpeedPercent, robotState.thetaCorrectionSpeedPercent);
            break;           
        }
        case ASSERVISSEMENT_INDEPENDANT_2_WHEELS:
        {  
            robotState.Motor1SpeedConsigne = ConvertSpeedFromPolaireToIndependant(robotState.XSpeedConsigne, robotState.YSpeedConsigne, robotState.ThetaSpeedConsigne, 1);
            robotState.Motor2SpeedConsigne = ConvertSpeedFromPolaireToIndependant(robotState.XSpeedConsigne, robotState.YSpeedConsigne, robotState.ThetaSpeedConsigne, 2);
            
//            robotState.PidMotor1.erreur = robotState.Motor1SpeedConsigne - robotState.Motor1SpeedOdometry;
//            robotState.PidMotor2.erreur = robotState.Motor2SpeedConsigne - robotState.Motor2SpeedOdometry;
//            
//            robotState.Motor1SpeedCommand = Correcteur(&robotState.PidMotor1, robotState.PidMotor1.erreur);    
//            robotState.Motor2SpeedCommand = Correcteur(&robotState.PidMotor2, robotState.PidMotor2.erreur);  
            
            //On enable les asservissement independants des moteurs 1 a 2
            robotState.asservissementMoteur1State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED;
            robotState.asservissementMoteur2State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED;
            break;
        }
        
        case ASSERVISSEMENT_DISABLED_4_WHEELS:
            ResetCorrecteur(&robotState.PidSpeedX, 0);
            ResetCorrecteur(&robotState.PidSpeedY, 0);
            ResetCorrecteur(&robotState.PidSpeedTheta, 0);
            //On desactive les asservissement independants des moteurs 1 a 4
            robotState.asservissementMoteur1State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            robotState.asservissementMoteur2State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            robotState.asservissementMoteur3State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            robotState.asservissementMoteur4State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            //On Set les M_ToPercent des moteurs individuellement
            SetMotorNMToPercent(1,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(2,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(3,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(4,Get4WheelsMToPercentProp());
            PWMSetSpeedConsignePolaire4Wheels(robotState.XSpeedConsigne, robotState.YSpeedConsigne, robotState.ThetaSpeedConsigne);            
            break;
            
        case ASSERVISSEMENT_POLAIRE_4_WHEELS:
        {
            /********************************************ASSERVISSEMENT VITESSE*********************************************************************/
            robotState.PidSpeedX.erreur = robotState.XSpeedConsigne - robotState.xVitesseFromOdometry;
            robotState.PidSpeedY.erreur = robotState.YSpeedConsigne - robotState.yVitesseFromOdometry;
            robotState.PidSpeedTheta.erreur = robotState.ThetaSpeedConsigne - robotState.thetaVitesseFromOdometry;
            
            //robotState.xCorrectionVitessePourcent = CorrecteurX(robotState.xErreur);
            robotState.xCorrectionSpeedPercent = Correcteur(&robotState.PidSpeedX, robotState.PidSpeedX.erreur);    
            robotState.yCorrectionSpeedPercent = Correcteur(&robotState.PidSpeedY, robotState.PidSpeedY.erreur);    
            robotState.thetaCorrectionSpeedPercent = Correcteur(&robotState.PidSpeedTheta, robotState.PidSpeedTheta.erreur);            
                        
//            robotState.yCorrectionVitessePourcent = CorrecteurY(robotState.yErreur);
//            robotState.thetaCorrectionVitessePourcent = CorrecteurTheta(robotState.thetaErreur);
            //On desactive les asservissement independants des moteurs 1 a 4
            robotState.asservissementMoteur1State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            robotState.asservissementMoteur2State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            robotState.asservissementMoteur3State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            robotState.asservissementMoteur4State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
            //On Set les M_ToPercent des moteurs individuellement
            SetMotorNMToPercent(1,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(2,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(3,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(4,Get4WheelsMToPercentProp());
            //On set les consignes des 4 moteurs en fonction de VX, VY, VTheta
            PWMSetSpeedConsignePolaire4Wheels(robotState.xCorrectionSpeedPercent, robotState.yCorrectionSpeedPercent, robotState.thetaCorrectionSpeedPercent);
            break;           
        }
            
        case ASSERVISSEMENT_INDEPENDANT_4_WHEELS:
        {  
            robotState.Motor1SpeedConsigne = ConvertSpeedFromPolaireToIndependant(robotState.XSpeedConsigne, robotState.YSpeedConsigne, robotState.ThetaSpeedConsigne, 1);
            robotState.Motor2SpeedConsigne = ConvertSpeedFromPolaireToIndependant(robotState.XSpeedConsigne, robotState.YSpeedConsigne, robotState.ThetaSpeedConsigne, 2);
            robotState.Motor3SpeedConsigne = ConvertSpeedFromPolaireToIndependant(robotState.XSpeedConsigne, robotState.YSpeedConsigne, robotState.ThetaSpeedConsigne, 3);
            robotState.Motor4SpeedConsigne = ConvertSpeedFromPolaireToIndependant(robotState.XSpeedConsigne, robotState.YSpeedConsigne, robotState.ThetaSpeedConsigne, 4);
            
//            robotState.PidMotor1.erreur = robotState.Motor1SpeedConsigne - robotState.Motor1SpeedOdometry;
//            robotState.PidMotor2.erreur = robotState.Motor2SpeedConsigne - robotState.Motor2SpeedOdometry;
//            robotState.PidMotor3.erreur = robotState.Motor3SpeedConsigne - robotState.Motor3SpeedOdometry;
//            robotState.PidMotor4.erreur = robotState.Motor4SpeedConsigne - robotState.Motor4SpeedOdometry;
//            
//            robotState.Motor1SpeedCommand = Correcteur(&robotState.PidMotor1, robotState.PidMotor1.erreur);    
//            robotState.Motor2SpeedCommand = Correcteur(&robotState.PidMotor2, robotState.PidMotor2.erreur);  
//            robotState.Motor3SpeedCommand = Correcteur(&robotState.PidMotor3, robotState.PidMotor3.erreur);  
//            robotState.Motor4SpeedCommand = Correcteur(&robotState.PidMotor4, robotState.PidMotor4.erreur);  
            
//            PWMSetSpeedCommandIndependant4Wheels(
//                    robotState.Motor1SpeedCommand,robotState.Motor2SpeedCommand,
//                    robotState.Motor3SpeedCommand,robotState.Motor4SpeedCommand);
            //On enable les asservissement independants des moteurs 1 a 4
            robotState.asservissementMoteur1State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED;
            robotState.asservissementMoteur2State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED;
            robotState.asservissementMoteur3State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED;
            robotState.asservissementMoteur4State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED;
            SetMotorNMToPercent(1,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(2,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(3,Get4WheelsMToPercentProp());
            SetMotorNMToPercent(4,Get4WheelsMToPercentProp());
        }
            break;
        default:break;
    }
    
    //Gestion des asservissement vitesse des moteurs individuellement
    switch(robotState.asservissementMoteur1State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:
            PWMSetSpeedCommandMotor1(robotState.Motor1SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor1.erreur = robotState.Motor1SpeedConsigne - robotState.Motor1SpeedOdometry;
            robotState.Motor1SpeedCommand = Correcteur(&robotState.PidSpeedMotor1, robotState.PidSpeedMotor1.erreur); 
            PWMSetSpeedCommandMotor1(robotState.Motor1SpeedCommand);
        break;
        default:break;
    }
    switch(robotState.asservissementMoteur2State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:
            PWMSetSpeedCommandMotor2(robotState.Motor2SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor2.erreur = robotState.Motor2SpeedConsigne - robotState.Motor2SpeedOdometry;
            robotState.Motor2SpeedCommand = Correcteur(&robotState.PidSpeedMotor2, robotState.PidSpeedMotor2.erreur); 
            PWMSetSpeedCommandMotor2(robotState.Motor2SpeedCommand);
        break;
        default:break;
    }
    switch(robotState.asservissementMoteur3State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:
            PWMSetSpeedCommandMotor3(robotState.Motor3SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor3.erreur = robotState.Motor3SpeedConsigne - robotState.Motor3SpeedOdometry;
            robotState.Motor3SpeedCommand = Correcteur(&robotState.PidSpeedMotor3, robotState.PidSpeedMotor3.erreur); 
            PWMSetSpeedCommandMotor3(robotState.Motor3SpeedCommand);
        break;
        default:break;
    }
    switch(robotState.asservissementMoteur4State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:
            PWMSetSpeedCommandMotor4(robotState.Motor4SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor4.erreur = robotState.Motor4SpeedConsigne - robotState.Motor4SpeedOdometry;
            robotState.Motor4SpeedCommand = Correcteur(&robotState.PidSpeedMotor4, robotState.PidSpeedMotor4.erreur); 
            PWMSetSpeedCommandMotor4(robotState.Motor4SpeedCommand);
        break;
        default:break;
    }
    switch(robotState.asservissementMoteur5State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:
            PWMSetSpeedCommandMotor5(robotState.Motor5SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor5.erreur = robotState.Motor5SpeedConsigne - robotState.Motor5SpeedOdometry;
            robotState.Motor5SpeedCommand = Correcteur(&robotState.PidSpeedMotor5, robotState.PidSpeedMotor5.erreur); 
            PWMSetSpeedCommandMotor5(robotState.Motor5SpeedCommand);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_POSITION_ENABLED:
        {            
            double speedConsigne = UpdateSingleMotorTrajectory(5, robotState.motorCurrentPosition[MotorIndex(5)]);
            robotState.PidSpeedMotor5.erreur = speedConsigne - robotState.Motor5SpeedOdometry;
            robotState.Motor5SpeedCommand = Correcteur(&robotState.PidSpeedMotor5, robotState.PidSpeedMotor5.erreur); 
            PWMSetSpeedCommandMotor5(robotState.Motor5SpeedCommand);
        }
        break;
        default:break;
    }
    
    switch(robotState.asservissementMoteur6State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:                                    
            PWMSetSpeedCommandMotor6(robotState.Motor6SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor6.erreur = robotState.Motor6SpeedConsigne - robotState.Motor6SpeedOdometry;
            robotState.Motor6SpeedCommand = Correcteur(&robotState.PidSpeedMotor6, robotState.PidSpeedMotor6.erreur); 
            PWMSetSpeedCommandMotor6(robotState.Motor6SpeedCommand);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_POSITION_ENABLED:
        {            
            double speedConsigne = UpdateSingleMotorTrajectory(6, robotState.motorCurrentPosition[MotorIndex(6)]);
            robotState.PidSpeedMotor6.erreur = speedConsigne - robotState.Motor6SpeedOdometry;
            robotState.Motor6SpeedCommand = Correcteur(&robotState.PidSpeedMotor6, robotState.PidSpeedMotor6.erreur); 
            PWMSetSpeedCommandMotor6(robotState.Motor6SpeedCommand);
        }
        break;
        default:break;
    }
        
    switch(robotState.asservissementMoteur7State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:
            PWMSetSpeedCommandMotor7(robotState.Motor7SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor7.erreur = robotState.Motor7SpeedConsigne - robotState.Motor7SpeedOdometry;
            robotState.Motor7SpeedCommand = Correcteur(&robotState.PidSpeedMotor7, robotState.PidSpeedMotor7.erreur); 
            PWMSetSpeedCommandMotor7(robotState.Motor7SpeedCommand);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_POSITION_ENABLED:
        {            
            double speedConsigne = UpdateSingleMotorTrajectory(7, robotState.motorCurrentPosition[MotorIndex(7)]);
            robotState.PidSpeedMotor7.erreur = speedConsigne - robotState.Motor7SpeedOdometry;
            robotState.Motor7SpeedCommand = Correcteur(&robotState.PidSpeedMotor7, robotState.PidSpeedMotor7.erreur); 
            PWMSetSpeedCommandMotor7(robotState.Motor7SpeedCommand);
        }
        break;
        
        default:break;
    }
            
    switch(robotState.asservissementMoteur8State)
    {
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND:
            PWMSetSpeedCommandMotor8(robotState.Motor8SpeedConsigne);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED:
            robotState.PidSpeedMotor8.erreur = robotState.Motor8SpeedConsigne - robotState.Motor8SpeedOdometry;
            robotState.Motor8SpeedCommand = Correcteur(&robotState.PidSpeedMotor8, robotState.PidSpeedMotor8.erreur); 
            PWMSetSpeedCommandMotor8(robotState.Motor8SpeedCommand);
        break;
        case ASSERVISSEMENT_MOTOR_INDIVIDUAL_POSITION_ENABLED:
        {            
            double speedConsigne = UpdateSingleMotorTrajectory(8, robotState.motorCurrentPosition[MotorIndex(8)]);
            robotState.PidSpeedMotor8.erreur = speedConsigne - robotState.Motor8SpeedOdometry;
            robotState.Motor8SpeedCommand = Correcteur(&robotState.PidSpeedMotor8, robotState.PidSpeedMotor8.erreur); 
            PWMSetSpeedCommandMotor8(robotState.Motor8SpeedCommand);
        }
        break;
        default:break;
    }
}


