/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ROBOT_OBJ_H
#define	ROBOT_OBJ_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "UTLN_Typedefs.h"
#include "Asservissement.h"
#include "UTLN_RM_C620.h"

#ifdef ROBOCUP
#define RayonRobot 0.185         //0.185
#endif
#ifdef EUROBOT
#define RayonRobot 0.145         //0.185
#endif

//NOTES
typedef struct robotStateBITS {
    //union {
        struct {
            
            RM_MOTOR_STATUS brushlessStatus[4];
            
            bool useC620ForOdometrie;
            bool motorEnable;
            bool prehensionMotorEnable;
            AsservissementVitesseState robotSpeedAsservissementState;                       //Mode d'sservissement du robot
            AsservissementPositionState robotPositionAsservissementState;
            
            //Mode asservissement pour chacun des moteurs independants.
            IndividualMotorAsservissementState asservissementMoteur1State;                
            IndividualMotorAsservissementState asservissementMoteur2State;
            IndividualMotorAsservissementState asservissementMoteur3State;
            IndividualMotorAsservissementState asservissementMoteur4State;
            IndividualMotorAsservissementState asservissementMoteur5State;
            IndividualMotorAsservissementState asservissementMoteur6State;
            IndividualMotorAsservissementState asservissementMoteur7State;
            IndividualMotorAsservissementState asservissementMoteur8State;
            
            unsigned char taskEnCours ;            
            
            //Asservissement en vitesse
            double Motor1SpeedOdometry;     //en m.s-1
            double Motor2SpeedOdometry;     //en m.s-1       
            double Motor3SpeedOdometry;     //en m.s-1
            double Motor4SpeedOdometry;     //en m.s-1
            
            double Motor5SpeedOdometry;     //en m.s-1       
            double Motor6SpeedOdometry;     //en m.s-1
            double Motor7SpeedOdometry;     //en m.s-1
            double Motor8SpeedOdometry;     //en m.s-1
            
            //Vitesses polaires
            double vitesseLineaireFromOdometry;     //en mm.s-1
            double vitesseAngulaireFromOdometry;    //en rad.s-1
            double vitesseLineaireConsigne;     //en mm.s-1
            double vitesseAngulaireConsigne;    //en rad.s-1

            
            double wheelFromCenterRadius;
            double pointToDistance;
            double pointToAngleAuxiliaryMotors;
            double xVitesseFromOdometry;
            double yVitesseFromOdometry;
            double thetaVitesseFromOdometry;  
            
            double xPositionRefTerrain;
            double yPositionRefTerrain;
            double thetaPositionRefTerrain; 
            double xSpeedRefTerrain;
            double ySpeedRefTerrain;
            double thetaSpeedRefTerrain;  
            
            double xWaypoint;
            double yWaypoint;
            double thetaWaypoint;
            double xGhost;
            double yGhost;
            double thetaGhost;
            double vxGhost;
            double vyGhost;
            double vthetaGhost;
                        
            double xSpeedKalmanRefRobot;
            double ySpeedKalmanRefRobot;
                        
            double XPositionGNSS;
            double YPositionGNSS;
            double ThetaPositionGNSS; 
            
//            double xPosFromOdometry;
//            double yPosFromOdometry;
//            double xPosFromOdometry_1;
//            double yPosFromOdometry_1;
//            double angleRadianFromOdometry;
//            double angleRadianFromOdometry_1;
            
            //Gestion des variables asserv pour 4 roues
            double XSpeedConsigne;
            double YSpeedConsigne;
            double ThetaSpeedConsigne;
            
            double Motor1SpeedConsigne;
            double Motor2SpeedConsigne;
            double Motor3SpeedConsigne;
            double Motor4SpeedConsigne;
            double Motor5SpeedConsigne;
            double Motor6SpeedConsigne;
            double Motor7SpeedConsigne;
            double Motor8SpeedConsigne;      
                        
            //Gestion variables des correcteurs 4 wheels
            double xCorrectionSpeedPercent;
            double yCorrectionSpeedPercent;
            double thetaCorrectionSpeedPercent;
            
            //Variables de sortie des correcteur si asservissement
            double Motor1SpeedCommand;
            double Motor2SpeedCommand;
            double Motor3SpeedCommand;
            double Motor4SpeedCommand;
            double Motor5SpeedCommand;
            double Motor6SpeedCommand;
            double Motor7SpeedCommand;
            double Motor8SpeedCommand;
            
            //Variables utilisés par les fonction PWM Update Speed
            float vitesseMoteur1CommandePercent ;
            float vitesseMoteur2CommandePercent ;
            float vitesseMoteur3CommandePercent ;  
            float vitesseMoteur4CommandePercent ; 
            float vitesseMoteur5CommandePercent ; 
            float vitesseMoteur6CommandePercent ;
            float vitesseMoteur7CommandePercent ;  
            float vitesseMoteur8CommandePercent ;  
            
            PidCorrector PidSpeedX;
            PidCorrector PidSpeedY;
            PidCorrector PidSpeedTheta;
            
            PidCorrector PidSpeedMotor1;
            PidCorrector PidSpeedMotor2;
            PidCorrector PidSpeedMotor3;
            PidCorrector PidSpeedMotor4;
            
            //Correcteur moteurs annexes
            PidCorrector PidSpeedMotor5;
            PidCorrector PidSpeedMotor6;
            PidCorrector PidSpeedMotor7;
            PidCorrector PidSpeedMotor8;
                        
            float motor1Current;
            float motor2Current;
            float motor3Current;
            float motor4Current;
            float motor5Current;
            float motor6Current;
            float motor7Current;
            float motor8Current;
            
            float battCMDVoltage;
            float battPWRVoltage;
            float battCMDCurrent;
            float battPWRCurrent;
            
            unsigned int servo6Us;            
            
            //Debug
            bool enableEncoderRawData;
            bool enablePositionData;
            bool enableMotorCurrentData;
            bool enablePIDDebugErrorCorrectionConsigne;
            bool enablePIDDebugInternal;
            bool enableMotorVitesseData;
            bool enableIOPolling;
            bool enableIOAnalogPolling;
            bool enablePowerMonitoring;
            bool enableUartRAWForward;
            
            bool PIDParametersReceived;
            
            bool flagIMUDataReadyToSend;
            
            int32_t motor1EncoderRawValue;
            int32_t motor2EncoderRawValue;
            int32_t motor3EncoderRawValue;
            int32_t motor4EncoderRawValue;
            int32_t motor5EncoderRawValue;
            int32_t motor6EncoderRawValue;
            int32_t motor7EncoderRawValue;
            int32_t motor8EncoderRawValue;
            
            float motorCurrentPosition[8];
            
            DataXYZ accelDataXYZ;
            DataXYZ gyroDataXYZ;
            
            float ana0_B0Value;
            float ana8_B8Value;
            float ana9_B9Value;
            float ana10_B10Value;
            float ana11_B11Value;
        } ;
    //} ;
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState ;


typedef struct _Location
{
    float X;
    float Y;
    float Theta;
    float Vx;
    float Vy;
    float Vtheta;
}Location;
#endif	/* XC_HEADER_TEMPLATE_H */

