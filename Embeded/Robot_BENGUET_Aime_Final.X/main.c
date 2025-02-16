#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "UART_Protocol.h"
#include <libpic30.h> 

unsigned int ADCValue0;
unsigned int ADCValue1;
unsigned int ADCValue2;
unsigned int ADCValue3;
unsigned int ADCValue4;

int main(void) {

    //???????????????????????????????????????????????????????????????????????????????????????
    // I n i t i a l i s a t i o n de l ? o s c i l l a t e u r
    //???????????????????????????????????????????????????????????????????????????????????????

    InitOscillator();

    //???????????????????????????????????????????????????????????????????????????????????????
    // C o n fi g u r a ti o n de s � e n t r e s s o r t i e s
    //????????????????????????????????????????????????????????

    InitIO();
    LED_ORANGE = 1;
    LED_BLEUE = 1;
    LED_BLANCHE = 1;


    //Initialisaation timers
    InitTimer23();
    InitTimer4(1000);
    InitTimer1(50);
    //Initialisaation timers

    //Initialisation fonctions PWM

    InitPWM();
    //PWMSetSpeedConsigne(20, MOTEUR_GAUCHE);
    //PWMSetSpeedConsigne(20, MOTEUR_DROIT);


    //Initialisation fonctions ADC
    InitADC1();

    /////////////////////////////////////
    InitUART();


    //? ?????????????????????????????????????????????????????????????????????????????????????
    // Boucle P r i n c i p a l e
    //? ?????????????????????????????????????????????????????????????????????????????????????


    while (1) {
        //LED_BLANCHE = !LED_BLANCHE;
        //unsigned int ? result=ADCGetResult( );


        if (ADCIsConversionFinished() == 1) {

            ADCClearConversionFinishedFlag();
            /*unsigned int * result=ADCGetResult();
        
            ADCValue0=result[0];
            ADCValue1=result[1];
            ADCValue2=result[2];*/



            unsigned int * result = ADCGetResult();
            float volts = ((float) result[1])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;

            volts = ((float) result[2])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;

            volts = ((float) result[4])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;

            volts = ((float) result[3])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremiteGauche = 34 / volts - 5;
            volts = ((float) result[0])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremiteDroit = 34 / volts - 5;


            unsigned char payload[3] = {robotState.distanceTelemetreGauche,
                robotState.distanceTelemetreCentre, robotState.distanceTelemetreDroit};
            unsigned char payload2[2] = {robotState.vitesseGaucheConsigne, robotState.vitesseDroiteConsigne};
            UartEncodeAndSendMessage(0x0030, 3, payload);
            UartEncodeAndSendMessage(0x0040, 2, payload2);




            if (robotState.distanceTelemetreDroit < 30 || robotState.distanceTelemetreExtremiteDroit < 30) {
                LED_ORANGE = 1;
            } else {
                LED_ORANGE = 0;
            }
            if (robotState.distanceTelemetreCentre < 30) {
                LED_BLEUE = 1;
            } else {
                LED_BLEUE = 0;
            }
            if (robotState.distanceTelemetreGauche < 30 || robotState.distanceTelemetreExtremiteGauche < 30) {
                LED_BLANCHE = 1;
            } else {
                LED_BLANCHE = 0;
            }
            //SendMessage((unsigned char*) "Bonjour", 7);

        }

        int i;
        //unsigned char payload[7] = {'B', 'o', 'n', 'j', 'o', 'u', 'r'};
        for (i = 0; i < CB_RX1_GetDataSize(); i++) {
            unsigned char c = CB_RX1_Get();
            UartDecodeMessage(c);
            //SendMessage(&c, 1);

        }
        //UartEncodeAndSendMessage(0x0080,7,payload);

        // __delay32(100000);
    }
}
unsigned char stateRobot;
float vitesse = 10;



////////////////////////////////////////////////////////////////////////////////

unsigned char autoControl = 1;

void SetRobotState(unsigned char State) {
    stateRobot = State;

}

void SetRobotAutoControlState(unsigned char command) {
    autoControl = 1;
    if (command == 0) {
        autoControl = 0;
        stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
    }
}


///////////////////////////////////////////////////////////////////////////////

void OperatingSystemLoop(void) {

    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;

        case STATE_ATTENTE_EN_COURS:
            if (autoControl == 1) {
                if (timestamp > 1000)
                    stateRobot = STATE_AVANCE;
            }
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;

            break;
        case STATE_AVANCE_EN_COURS:
            if (autoControl == 1)
                SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            if (autoControl == 1)
                SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            if (autoControl == 1)
                SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(vitesse / 2, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitesse / 2, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            if (autoControl == 1)
                SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-vitesse / 2, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesse / 2, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            if (autoControl == 1)
                SetNextRobotStateInAutomaticMode();
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}


unsigned char nextStateRobot = 0;
unsigned char StateRobot2 = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;



    //    //D�termination de la position des obstacles en fonction des t�l�m�tres
    //    if ((robotState.distanceTelemetreDroit < 30 &&
    //            robotState.distanceTelemetreCentre > 20 /*&&
    //           robotState.distanceTelemetreGauche > 30*/)    ||    (robotState.distanceTelemetreExtremiteDroit < 30 &&
    //            robotState.distanceTelemetreCentre > 20 /*&&
    //            robotState.distanceTelemetreGauche > 30*/) ) //Obstacle � droite
    //        positionObstacle = OBSTACLE_A_DROITE;
    //    
    //    else if ((/*robotState.distanceTelemetreDroit > 30 &&*/
    //            robotState.distanceTelemetreCentre > 20 &&
    //            robotState.distanceTelemetreGauche < 30)  ||  (/*robotState.distanceTelemetreDroit > 30 &&*/
    //            robotState.distanceTelemetreCentre > 20 &&
    //            robotState.distanceTelemetreExtremiteGauche < 20) ) //Obstacle � gauche) //Obstacle � gauche        
    //        positionObstacle = OBSTACLE_A_GAUCHE;
    //    
    //    else if (robotState.distanceTelemetreCentre < 20) //Obstacle en face
    //        positionObstacle = OBSTACLE_EN_FACE;
    //    
    //    else if (robotState.distanceTelemetreDroit > 30 &&
    //            robotState.distanceTelemetreCentre > 20 &&
    //            robotState.distanceTelemetreGauche > 30 &&//pas d?obstacle
    //            robotState.distanceTelemetreExtremiteGauche > 30 &&
    //            robotState.distanceTelemetreExtremiteDroit > 30  || robotState.distanceTelemetreDroit > 30 &&
    //            robotState.distanceTelemetreCentre > 20 &&
    //            robotState.distanceTelemetreGauche > 30 && robotState.distanceTelemetreExtremiteGauche < 30 &&
    //            robotState.distanceTelemetreExtremiteDroit < 30 )
    //        positionObstacle = PAS_D_OBSTACLE;
    //D�termination de la position des obstacles en fonction des t�l�m�tres
    if (robotState.distanceTelemetreExtremiteGauche > 30 && (robotState.distanceTelemetreExtremiteDroit < 30 ||
            (robotState.distanceTelemetreDroit < 30 && robotState.distanceTelemetreGauche > 30))) {
        positionObstacle = OBSTACLE_A_DROITE;
    } else if ((robotState.distanceTelemetreExtremiteGauche < 30 && robotState.distanceTelemetreExtremiteDroit > 30) ||
            (robotState.distanceTelemetreExtremiteGauche > 30 && robotState.distanceTelemetreGauche < 30 &&
            robotState.distanceTelemetreDroit > 30 && robotState.distanceTelemetreExtremiteDroit > 30)) {
        positionObstacle = OBSTACLE_A_GAUCHE;
    } else if ((robotState.distanceTelemetreExtremiteGauche < 30 && robotState.distanceTelemetreExtremiteDroit < 30) ||
            ((robotState.distanceTelemetreExtremiteGauche > 30 && robotState.distanceTelemetreExtremiteDroit > 30) &&
            ((robotState.distanceTelemetreDroit < 30 && robotState.distanceTelemetreGauche < 30) ||
            (robotState.distanceTelemetreGauche > 30 && robotState.distanceTelemetreDroit > 30 && robotState.distanceTelemetreCentre < 20)))) {
        positionObstacle = OBSTACLE_EN_FACE;
    }/*
        else if (robotState.distanceTelemetreCentre < 20){
            positionObstacle = OBSTACLE_EN_FACE;
        }*/

    else if ((robotState.distanceTelemetreExtremiteGauche < 20 && robotState.distanceTelemetreGauche > 30 &&
            robotState.distanceTelemetreCentre > 20) && (robotState.distanceTelemetreDroit > 30 && robotState.distanceTelemetreExtremiteDroit < 20)) {
        positionObstacle = PAS_D_OBSTACLE;
    } else if (robotState.distanceTelemetreDroit > 20 && robotState.distanceTelemetreGauche > 20 &&
            robotState.distanceTelemetreExtremiteGauche > 20 && robotState.distanceTelemetreExtremiteDroit > 20) {
        positionObstacle = PAS_D_OBSTACLE;
    }


    //D�termination de l?�tat � venir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    //Si l?on n?est pas dans la transition de l?�tape en cours
    if (nextStateRobot != stateRobot - 1) {
        stateRobot = nextStateRobot;
        //UartSendStateRobot(0x0050, stateRobot, timestamp);
    }




} // f i n main

//}    COMMENTEE
