/******************************************************************************
  Filename:       ustv_Communication.c
  Revised:        $Date: 2012-11-19 18:52:21 $
  Revision:       $Revision: 00000 $

  Description:   USTV functions for USTV Communication (UART Routine,
  USTV Parser, ...)

  Copyright 2012 Universitï¿½ du Sud Toulon Var. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer .You may not use this
  Software unless you agree to abide by the terms of the License. Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact DPT GEII at www.univ-tln.fr.

 *****************************************************************************/
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "Define.h"
#include "UTLN_Communication.h"
#include "UTLN_PWM.h"
//#include "UTLN_Message.h"
#include "UTLN_uart.h"
#include "UTLN_OSAL.h"
#include "UTLN_Timers.h"
#include "UTLN_CoilGun.h"
#include "UTLN_Odometrie.h"
#include "main.h"
#include "Utilities.h"
#include "RobotObj.h"
#include "Asservissement.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "USB/app_device_vendor_basic.h"
#include "UTLN_CircularBuffer.h"
#include "UTLN_OutputCompare.h"
#include "TrajectoryGeneratorSingleMotor.h"
#include "UTLN_KalmanFilter.h"
#include "UTLN_Imu_Processor.h"

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/


/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
extern volatile unsigned long g_longTimeStamp;
extern volatile unsigned long autoStopMotorOnNoCommandTimeStamp;

extern volatile PidCorrector PID_X;
extern volatile PidCorrector PID_Y;
extern volatile PidCorrector PID_Theta;

extern volatile TRAJECTORY_GENERATOR_SINGLE_MOTOR_BITS trajectoryGeneratorMotor[8];

/*******************************************************************************
 * FONCTIONS
 ******************************************************************************/
unsigned char UartCalculateChecksum(unsigned int msgFunction,
        unsigned int msgPayloadLength, unsigned char * msgPayload) {
    unsigned char checksum = 0;
    unsigned int i = 0;
    //checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction);
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength);
    for (i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }
    return checksum;
}

unsigned char USBCalculateChecksum(unsigned int msgFunction,
        unsigned int messageNumber, unsigned int msgPayloadLength, unsigned char * msgPayload) {
    unsigned char checksum = 0;
    unsigned int i = 0;
    //checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction);
    checksum ^= (unsigned char) (messageNumber >> 8);
    checksum ^= (unsigned char) (messageNumber);
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength);
    for (i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }
    return checksum;
}
// <editor-fold defaultstate="collapsed" desc="UART1">
#ifdef USE_UART1
RECEPTION_STATE rx1ReceptionState = RECEPTION_WAIT;
unsigned int rx1ReceivedFunction;
unsigned int rx1ReceivedPayloadLength;
unsigned char rx1ReceivedPayload[UART1_CIRCULAR_BUFFER_SIZE];
unsigned int rx1ReceivedPayloadIndex = 0;

void Uart1DecodeMessage() {
    unsigned char c;
    //Si des donnï¿½es sont disponibles sur le port sï¿½rie
    while (UART1IsDataReadyInRxBuffer() == TRUE) {
        c = UART1ReadFromRxBuffer();
        switch (rx1ReceptionState) {
            case RECEPTION_WAIT:
                if (c == 0xFE)
                    rx1ReceptionState = RECEPTION_FUNCTION_MSB;
                break;
            case RECEPTION_FUNCTION_MSB:
                rx1ReceivedFunction = (unsigned int) (c << 8);
                rx1ReceptionState = RECEPTION_FUNCTION_LSB;
                break;
            case RECEPTION_FUNCTION_LSB:
                rx1ReceivedFunction += (unsigned int) c;
                rx1ReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_MSB:
                rx1ReceivedPayloadLength = (unsigned int) (c << 8);
                rx1ReceptionState = RECEPTION_PAYLOAD_LENGTH_LSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_LSB:
                rx1ReceivedPayloadLength += (unsigned int) c;
                if (rx1ReceivedPayloadLength > UART1_CIRCULAR_BUFFER_SIZE)
                    rx1ReceptionState = RECEPTION_WAIT;
                else if (rx1ReceivedPayloadLength == 0)
                    rx1ReceptionState = RECEPTION_CHECKSUM;
                else
                    rx1ReceptionState = RECEPTION_PAYLOAD;
                break;
            case RECEPTION_PAYLOAD:
                rx1ReceivedPayload[rx1ReceivedPayloadIndex] = c;
                rx1ReceivedPayloadIndex++;
                if (rx1ReceivedPayloadIndex == rx1ReceivedPayloadLength) {
                    rx1ReceivedPayloadIndex = 0;
                    rx1ReceptionState = RECEPTION_CHECKSUM;
                }
                break;
            case RECEPTION_CHECKSUM:
                if (c == UartCalculateChecksum(rx1ReceivedFunction,
                        rx1ReceivedPayloadLength, rx1ReceivedPayload)) {
                    //Message valide
                    ProcessMessage(rx1ReceivedFunction, rx1ReceivedPayloadLength, rx1ReceivedPayload);

                }
                rx1ReceptionState = RECEPTION_WAIT;
                break;
            default:
                rx1ReceptionState = RECEPTION_WAIT;
                break;
        }
    }
}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="UART2">
#ifdef USE_UART2
RECEPTION_STATE rx2ReceptionState = RECEPTION_WAIT;
unsigned int rx2ReceivedFunction;
unsigned int rx2ReceivedPayloadLength;
unsigned char rx2ReceivedPayload[UART1_CIRCULAR_BUFFER_SIZE];
unsigned int rx2ReceivedPayloadIndex = 0;

void ProcessUart2ReceiveChar(void) {
    unsigned char c;
    //Si des donnï¿½es sont disponibles sur le port sï¿½rie
    while (UART2IsDataReadyInRxBuffer() == TRUE) {
        c = UART2ReadFromRxBuffer();
        switch (rx2ReceptionState) {
            case RECEPTION_WAIT:
                if (c == 0xFE)
                    rx2ReceptionState = RECEPTION_FUNCTION_MSB;
                break;
            case RECEPTION_FUNCTION_MSB:
                rx2ReceivedFunction = (unsigned int) (c << 8);
                rx2ReceptionState = RECEPTION_FUNCTION_LSB;
                break;
            case RECEPTION_FUNCTION_LSB:
                rx2ReceivedFunction += (unsigned int) c;
                rx2ReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_MSB:
                rx2ReceivedPayloadLength = (unsigned int) (c << 8);
                rx2ReceptionState = RECEPTION_PAYLOAD_LENGTH_LSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_LSB:
                rx2ReceivedPayloadLength += (unsigned int) c;
                if (rx2ReceivedPayloadLength > UART2_CIRCULAR_BUFFER_SIZE)
                    rx2ReceptionState = RECEPTION_WAIT;
                else if (rx2ReceivedPayloadLength == 0)
                    rx2ReceptionState = RECEPTION_CHECKSUM;
                else
                    rx2ReceptionState = RECEPTION_PAYLOAD;
                break;
            case RECEPTION_PAYLOAD:
                rx2ReceivedPayload[rx2ReceivedPayloadIndex] = c;
                rx2ReceivedPayloadIndex++;
                if (rx2ReceivedPayloadIndex == rx2ReceivedPayloadLength) {
                    rx2ReceivedPayloadIndex = 0;
                    rx2ReceptionState = RECEPTION_CHECKSUM;
                }
                break;
            case RECEPTION_CHECKSUM:
                if (c == UartCalculateChecksum(rx2ReceivedFunction,
                        rx2ReceivedPayloadLength, rx2ReceivedPayload)) {
                    //Message valide
                    ProcessMessage(rx2ReceivedFunction, rx2ReceivedPayloadLength, rx2ReceivedPayload);

                }
                rx2ReceptionState = RECEPTION_WAIT;
                break;
            default:
                rx2ReceptionState = RECEPTION_WAIT;
                break;
        }
    }
}

void ForwardUart4ReceiveData(void) {
    unsigned char currentIndex = 0;
    unsigned char outPayload[128 + 5];
    if (UART4IsDataReadyInRxBuffer() == TRUE) {
        //Si des donnï¿½es sont disponibles sur le port sï¿½rie
        while (UART4IsDataReadyInRxBuffer() == TRUE && currentIndex < (128 - UTLN_FRAME_SIZE)) {
            outPayload[currentIndex++] = UART4ReadFromRxBuffer();
        }
        MakeAndSendMessageWithUTLNProtocol(R2PC_UART4_FORWARD, currentIndex, outPayload);
    }
}

void ForwardUart3ReceiveData(void) {
    unsigned char currentIndex = 0;
    unsigned char outPayload[128 + 5];
    if (UART3IsDataReadyInRxBuffer() == TRUE) {
        //Si des donnï¿½es sont disponibles sur le port sï¿½rie
        while (UART3IsDataReadyInRxBuffer() == TRUE && currentIndex < (128 - UTLN_FRAME_SIZE)) {
            outPayload[currentIndex++] = UART3ReadFromRxBuffer();
        }
        MakeAndSendMessageWithUTLNProtocol(R2PC_UART3_FORWARD, currentIndex, outPayload);
    }
}

void ForwardUart2ReceiveData(void) {
    unsigned char currentIndex = 0;
    unsigned char outPayload[128 + 5];
    if (UART2IsDataReadyInRxBuffer() == TRUE) {
        //Si des donnï¿½es sont disponibles sur le port sï¿½rie
        while (UART2IsDataReadyInRxBuffer() == TRUE && currentIndex < (128 - UTLN_FRAME_SIZE)) {
            outPayload[currentIndex++] = UART2ReadFromRxBuffer();
        }
        MakeAndSendMessageWithUTLNProtocol(R2PC_UART2_FORWARD, currentIndex, outPayload);
    }
}
#ifdef USE_UART1

void ForwardUart1ReceiveData(void) {
    unsigned char currentIndex = 0;
    unsigned char outPayload[128 + 5];
    if (UART1IsDataReadyInRxBuffer() == TRUE) {
        //Si des donnï¿½es sont disponibles sur le port sï¿½rie
        while (UART1IsDataReadyInRxBuffer() == TRUE && currentIndex < (128 - UTLN_FRAME_SIZE)) {
            outPayload[currentIndex++] = UART1ReadFromRxBuffer();
        }
        MakeAndSendMessageWithUTLNProtocol(R2PC_UART1_FORWARD, currentIndex, outPayload);
    }
}
#endif

unsigned char packetTestNumber = 0;

void SendUSBTestData(void) {
    unsigned char currentIndex = 0;
    unsigned char outPayload[512 + 16 + 5];

    outPayload[0] = SOF;
    outPayload[1] = MSB_UINT16(PC2R_USB_TEST_FRAME);
    outPayload[2] = LSB_UINT16(PC2R_USB_TEST_FRAME);

    int i = 0;
    for (i = 0; i < 16; i++) {
        outPayload[5 + currentIndex++] = packetTestNumber;
    }

    for (i = 0; i < 256; i++) {
        outPayload[5 + currentIndex++] = MSB_UINT16(i);
        outPayload[5 + currentIndex++] = LSB_UINT16(i);
    }
    outPayload[3] = MSB_UINT16(currentIndex);
    outPayload[4] = LSB_UINT16(currentIndex);
    //outPayload[5+currentIndex]=UartCalculateChecksum(R2PC_UART1_FORWARD,currentIndex, outPayload+5);
    SendMessageToUSB(UTLN_FRAME_SIZE + currentIndex, outPayload);
    packetTestNumber++;

}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="UART3">
RECEPTION_STATE rx3ReceptionState = RECEPTION_WAIT;
unsigned int rx3ReceivedFunction;
unsigned int rx3ReceivedPayloadLength;
unsigned char rx3ReceivedPayload[UART3_CIRCULAR_BUFFER_SIZE];
unsigned int rx3ReceivedPayloadIndex = 0;
unsigned int rx3ErrorCount = 0;

void Uart3DecodeMessage(void) {
    unsigned char c;
    //Si des donnï¿½es sont disponibles sur le port sï¿½rie
    while (UART3IsDataReadyInRxBuffer() == TRUE) {
        //On lit un caractï¿½re
        c = UART3ReadFromRxBuffer();
        switch (rx3ReceptionState) {
            case RECEPTION_WAIT:
                if (c == 0xFE)
                    rx3ReceptionState = RECEPTION_FUNCTION_MSB;
                break;
            case RECEPTION_FUNCTION_MSB:
                rx3ReceivedFunction = (unsigned int) (c << 8);
                rx3ReceptionState = RECEPTION_FUNCTION_LSB;
                break;
            case RECEPTION_FUNCTION_LSB:
                rx3ReceivedFunction += (unsigned int) c;
                rx3ReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_MSB:
                rx3ReceivedPayloadLength = (unsigned int) (c << 8);
                rx3ReceptionState = RECEPTION_PAYLOAD_LENGTH_LSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_LSB:
                rx3ReceivedPayloadLength += (unsigned int) c;
                if (rx3ReceivedPayloadLength > UART3_CIRCULAR_BUFFER_SIZE)
                    rx3ReceptionState = RECEPTION_WAIT;
                else if (rx3ReceivedPayloadLength == 0)
                    rx3ReceptionState = RECEPTION_CHECKSUM;
                else
                    rx3ReceptionState = RECEPTION_PAYLOAD;
                break;
            case RECEPTION_PAYLOAD:
                rx3ReceivedPayload[rx3ReceivedPayloadIndex] = c;
                rx3ReceivedPayloadIndex++;
                if (rx3ReceivedPayloadIndex == rx3ReceivedPayloadLength) {
                    rx3ReceivedPayloadIndex = 0;
                    rx3ReceptionState = RECEPTION_CHECKSUM;
                }
                break;
            case RECEPTION_CHECKSUM:
                if (c == UartCalculateChecksum(rx3ReceivedFunction,
                        rx3ReceivedPayloadLength, rx3ReceivedPayload)) {
                    //Message valide
                    ProcessMessage(rx3ReceivedFunction, rx3ReceivedPayloadLength, rx3ReceivedPayload);
                } else {
                    rx3ErrorCount++;
                }
                rx3ReceptionState = RECEPTION_WAIT;
                break;
            default:
                rx3ReceptionState = RECEPTION_WAIT;
                break;
        }
    }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="USB">
RECEPTION_STATE usbReceptionState = RECEPTION_WAIT;
unsigned int usbReceivedFunction;
unsigned int packetNumber;
unsigned int usbReceivedPayloadLength;
unsigned char usbReceivedPayload[USB_CIRCULAR_BUFFER_SIZE];
unsigned int usbReceivedPayloadIndex = 0;
unsigned int usbErrorCount = 0;
extern unsigned int usbTimeout;

void USBDecodeMessage(unsigned char* buffer, unsigned char bufferLength) {
    unsigned char c;
    int i;
    for (i = 0; i < bufferLength; i++) {
        //On lit un caractï¿½re
        c = buffer[i];
        switch (usbReceptionState) {
            case RECEPTION_WAIT:
                if (c == 0xFE)
                    usbReceptionState = RECEPTION_FUNCTION_MSB;
                break;
            case RECEPTION_FUNCTION_MSB:
                usbReceivedFunction = (unsigned int) (c << 8);
                usbReceptionState = RECEPTION_FUNCTION_LSB;
                break;
            case RECEPTION_FUNCTION_LSB:
                usbReceivedFunction += (unsigned int) c;
#ifdef UTLN_PROTOCOL_V2
                usbReceptionState = RECEPTION_PACKET_NUMBER_MSB;
#else
                usbReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
#endif
                break;
            case RECEPTION_PACKET_NUMBER_MSB:
                packetNumber = (unsigned int) (c << 8);
                usbReceptionState = RECEPTION_PACKET_NUMBER_LSB;
                break;
            case RECEPTION_PACKET_NUMBER_LSB:
                packetNumber += (unsigned int) c;
                usbReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_MSB:
                usbReceivedPayloadLength = (unsigned int) (c << 8);
                usbReceptionState = RECEPTION_PAYLOAD_LENGTH_LSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_LSB:
                usbReceivedPayloadLength += (unsigned int) c;
                if (usbReceivedPayloadLength > USB_CIRCULAR_BUFFER_SIZE)
                    usbReceptionState = RECEPTION_WAIT;
                else if (usbReceivedPayloadLength == 0)
                    usbReceptionState = RECEPTION_CHECKSUM;
                else
                    usbReceptionState = RECEPTION_PAYLOAD;
                break;
            case RECEPTION_PAYLOAD:
                usbReceivedPayload[usbReceivedPayloadIndex] = c;
                usbReceivedPayloadIndex++;
                if (usbReceivedPayloadIndex == usbReceivedPayloadLength) {
                    usbReceivedPayloadIndex = 0;
                    usbReceptionState = RECEPTION_CHECKSUM;
                }
                break;
            case RECEPTION_CHECKSUM:
#ifdef UTLN_PROTOCOL_V2
                if (c == USBCalculateChecksum(usbReceivedFunction, packetNumber,
                        usbReceivedPayloadLength, usbReceivedPayload)) {
                    //Message valide
                    ProcessMessage(usbReceivedFunction, usbReceivedPayloadLength, usbReceivedPayload);
                } else {
                    usbErrorCount++;
                    flagError.USBChkSumError = 1;
                }
#else
                if (c == UartCalculateChecksum(usbReceivedFunction,
                        usbReceivedPayloadLength, usbReceivedPayload)) {
                    //Message valide
                    ProcessMessage(usbReceivedFunction, usbReceivedPayloadLength, usbReceivedPayload);
                } else {
                    usbErrorCount++;
                }
#endif
                usbReceptionState = RECEPTION_WAIT;
                break;
            default:
                usbReceptionState = RECEPTION_WAIT;
                break;
        }
    }
}
// </editor-fold>

//<editor-fold defaultstate="collapsed" desc="UART4">
unsigned char rx4ReceptionState = RECEPTION_WAIT;
unsigned int rx4ReceivedFunction;
unsigned int rx4ReceivedPayloadLength;
unsigned char rx4ReceivedPayload[UART4_CIRCULAR_BUFFER_SIZE];
unsigned int rx4ReceivedPayloadIndex = 0;
unsigned char c;
unsigned int rx4ErrorCount = 0;

void Uart4DecodeMessage(void) {
    //Si des donnï¿½es sont disponibles sur le port sï¿½rie
    while (UART4IsDataReadyInRxBuffer() == TRUE) {
        //On lit un caractï¿½re
        c = UART4ReadFromRxBuffer();
        switch (rx4ReceptionState) {
            case RECEPTION_WAIT:
                if (c == 0xFE)
                    rx4ReceptionState = RECEPTION_FUNCTION_MSB;
                break;
            case RECEPTION_FUNCTION_MSB:
                rx4ReceivedFunction = (unsigned int) (c << 8);
                rx4ReceptionState = RECEPTION_FUNCTION_LSB;
                break;
            case RECEPTION_FUNCTION_LSB:
                rx4ReceivedFunction += (unsigned int) c;
                rx4ReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_MSB:
                rx4ReceivedPayloadLength = (unsigned int) (c << 8);
                rx4ReceptionState = RECEPTION_PAYLOAD_LENGTH_LSB;
                break;
            case RECEPTION_PAYLOAD_LENGTH_LSB:
                rx4ReceivedPayloadLength += (unsigned int) c;
                if (rx4ReceivedPayloadLength > UART4_CIRCULAR_BUFFER_SIZE)
                    rx4ReceptionState = RECEPTION_WAIT;
                else if (rx4ReceivedPayloadLength == 0)
                    rx4ReceptionState = RECEPTION_CHECKSUM;
                else
                    rx4ReceptionState = RECEPTION_PAYLOAD;
                break;
            case RECEPTION_PAYLOAD:
                rx4ReceivedPayload[rx4ReceivedPayloadIndex] = c;
                rx4ReceivedPayloadIndex++;
                if (rx4ReceivedPayloadIndex == rx4ReceivedPayloadLength) {
                    rx4ReceivedPayloadIndex = 0;
                    rx4ReceptionState = RECEPTION_CHECKSUM;
                }
                break;
            case RECEPTION_CHECKSUM:
                if (c == UartCalculateChecksum(rx4ReceivedFunction,
                        rx4ReceivedPayloadLength, rx4ReceivedPayload)) {
                    //Message valide
                    ProcessMessage(rx4ReceivedFunction, rx4ReceivedPayloadLength, rx4ReceivedPayload);
                } else {
                    rx4ErrorCount++;
                }
                rx4ReceptionState = RECEPTION_WAIT;
                break;
            default:
                rx4ReceptionState = RECEPTION_WAIT;
                break;
        }
    }
}
//</editor-fold>

/*******************************************************************************
 * @fn      ProcessEndDeviceMessage
 *
 * @brief   Fonction permettant le traitement des message recus via le ZigBee.
 *          (Ex: Demande d'acquisition sur Magnetometre, demande de mesure de
 *          batterie, allumer une LED, ...). Une reponse peut etre renvoyï¿½e
 *          au travers du reseau .
 *
 * @param   command - 2 byte - Commande a executer (Propre au protocole USTV)
 *          payload - 1-256 byte - Tableau dans lequel est enregistrï¿½e la
 *          Payload.
 *          length - 1 byte - Taille de la payload.
 *
 * @return  None
 *
 ******************************************************************************/
void ProcessMessage(unsigned short int command, unsigned short int length, unsigned char payload[]) {
    unsigned char blockMessage = 0;
    unsigned short int msgTxUARTPayloadLength;
    static unsigned char msgTxUARTPayload[BUFFER_TX_UART_SIZE];

    //Valeur par dï¿½faut pour ï¿½viter de renvoyer la payload prï¿½cï¿½dente si la valeur n'est pas renseignï¿½e
    msgTxUARTPayloadLength = 0;
    unsigned char pos = 0;
    autoStopMotorOnNoCommandTimeStamp = 0;
    switch (command) {
            //SET_LED
        case SET_LED_VALUE:
            switch (payload[0]) {
                case 1:
                    _LED_V1 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_V1;
                    break;
                case 2:
                    _LED_R1 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_R1;
                    break;
                case 3:
                    _LED_O1 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_O1;
                    break;
                case 4:
                    _LED_BLE1 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_BLE1;
                    break;
                case 5:
                    _LED_B1 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_B1;
                    break;
                case 6:
                    _LED_V2 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_V2;
                    break;
                case 7:
                    _LED_R2 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_R2;
                    break;
                case 8:
                    _LED_O2 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_O2;
                    break;
                case 9:
                    _LED_BLE2 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_BLE2;
                    break;
                case 10:
                    _LED_B2 = payload[1];
                    msgTxUARTPayload[pos++] = (unsigned char) _LED_B2;
                    break;
            }

            //On prepare un message
            msgTxUARTPayloadLength = pos;
            break;

        case SET_TIMER_FREQUENCY:
        {
            uint8_t timerID = payload[0];
            float timerFreq = getFloat(payload, 1);
            SetTimerFreq(timerID, timerFreq);
        }
            break;
            //Permet de regler les directions des pins IO
        case SET_IO_DIR:
        {
            uint8_t pinNumber = payload[0];
            uint8_t direction = payload[1];
            SetCMV8IODirection(pinNumber, direction);
        }
            break;
            //Permet de regler les modes des pins IO (Ana, Digital))
        case SET_IO_MODE:
        {
            uint8_t pinNumber = payload[0];
            uint8_t mode = payload[1];
            SetCMV8IOMode(pinNumber, mode);
        }
            break;
            //Permet de regler les valeurs des pins IO
        case SET_IO_VALUE:
        {
            uint8_t pinNumber = payload[0];
            uint8_t value = payload[1];
            SetCMV8IOValue(pinNumber, value);
        }
            break;


            //Set TimeStamp
        case 0x79:
            blockMessage = 1;
            g_longTimeStamp = payload[0] + ((unsigned long) payload[1] << 8) + ((unsigned long) payload[2] << 16) + ((unsigned long) payload[3] << 24);
            //On prepare un message
            msgTxUARTPayloadLength = 0;
            break;
            //Set IO Polling Frequency    
        case PC2R_IO_POLLING_SET_FREQUENCY:
        {
            unsigned char freq = payload[0];
            unsigned char period = 1000 / freq;
#ifdef USE_OSAL
            OSAL_start_reload_timer(USTV_SYSTEM_PROCESSING_TASK, USTV_VIRTUAL_TIMER_1_TIMOUT, period); //IO Refresh rate
#endif
        }
            break;
            //Set Speed consigne (VX, VY, VTh)    
        case PC2R_SPEED_POLAR_SET_CONSIGNE:
            blockMessage = 1;
            float XSpeedConsigne = getFloat(payload, 0);
            float YSpeedConsigne = getFloat(payload, 4);
            float ThetaSpeedConsigne = getFloat(payload, 8);
            // On ne transmet les commandes que si on est en asservissement de position OFF
            // Sinon elles sont générées par le générateur de trajectoire et le PID de position
            if(robotState.robotPositionAsservissementState == ASSERVISSEMENT_POSITION_OFF)
                SetSpeedConsigneHolonome(XSpeedConsigne, YSpeedConsigne, ThetaSpeedConsigne);
            
            
            _LED_V2 = !_LED_V2;
            _SPI2_SS1 = 0;
            break;

        case PC2R_POSITION_GNSS:
            robotState.XPositionGNSS = getFloat(payload, 0);
            robotState.YPositionGNSS = getFloat(payload, 4);
            robotState.ThetaPositionGNSS = getFloat(payload, 8);
            UpdateThetaOnGNSS(robotState.ThetaPositionGNSS);
            UpdateXYOnGNSS(robotState.XPositionGNSS, robotState.YPositionGNSS);
            break;

        case PC2R_SET_FORCED_LOCATION:
        {
            float XPos = getFloat(payload, 0);
            float YPos = getFloat(payload, 4);
            float ThetaPos = getFloat(payload, 8);
            InitRobotPosition(XPos, YPos, ThetaPos);
        }
            break;

        case PC2R_SET_WAYPOINT:
        {
            float XPos = getFloat(payload, 0);
            float YPos = getFloat(payload, 4);
            float ThetaPos = getFloat(payload, 8);
            SetWaypointLocation(XPos, YPos, ThetaPos);
        }
        break;
          
        case PC2R_SET_TRAJECTORY_CONSTANTS:
        {
            float accelLineaire = getFloat(payload, 0);
            float accelRotationCap = getFloat(payload, 4);
            float accelRotationOrientation = getFloat(payload, 8);
            
            float vitesseLineaireMax = getFloat(payload, 12);
            float vitesseRotationCapMax = getFloat(payload, 16);
            float vitesseRotationOrientationMax = getFloat(payload, 20);
            float vitesseLineaireFreinage = getFloat(payload,24);
            
            SetTrajectoryAccelerationSpeedConstants(accelLineaire, accelRotationCap, accelRotationOrientation, vitesseLineaireMax, vitesseRotationCapMax, vitesseRotationOrientationMax,vitesseLineaireFreinage);
        }
        break;  
        case PC2R_SET_KALMAN_ALPHAS:
        {
            float alphaTheta = getFloat(payload, 0);
            float alphaSpeed = getFloat(payload, 4);
            float alphaPosition = getFloat(payload, 8);
            SetAlphaTheta(alphaTheta);
            SetAlphaSpeed(alphaSpeed);
            SetAlphaPosition(alphaPosition);
        }
        break;             
        //Set Polar PID Gains and Limits    
        case PC2R_4WHEELS_SPEED_POLAR_PID_SET_GAINS:
            blockMessage = 1;
            robotState.PidSpeedX.Kp = (double) getFloat(payload, 0);
            robotState.PidSpeedX.Ki = getFloat(payload, 4);
            robotState.PidSpeedX.Kd = getFloat(payload, 8);
            robotState.PidSpeedY.Kp = getFloat(payload, 12);
            robotState.PidSpeedY.Ki = getFloat(payload, 16);
            robotState.PidSpeedY.Kd = getFloat(payload, 20);
            robotState.PidSpeedTheta.Kp = getFloat(payload, 24);
            robotState.PidSpeedTheta.Ki = getFloat(payload, 28);
            robotState.PidSpeedTheta.Kd = getFloat(payload, 32);
            robotState.PidSpeedX.erreurProportionelleMax = getFloat(payload, 36);
            robotState.PidSpeedX.erreurIntegraleMax = getFloat(payload, 40);
            robotState.PidSpeedX.erreurDeriveeMax = getFloat(payload, 44);
            robotState.PidSpeedY.erreurProportionelleMax = getFloat(payload, 48);
            robotState.PidSpeedY.erreurIntegraleMax = getFloat(payload, 52);
            robotState.PidSpeedY.erreurDeriveeMax = getFloat(payload, 56);
            robotState.PidSpeedTheta.erreurProportionelleMax = getFloat(payload, 60);
            robotState.PidSpeedTheta.erreurIntegraleMax = getFloat(payload, 64);
            robotState.PidSpeedTheta.erreurDeriveeMax = getFloat(payload, 68);
            break;

            //Set Polar PID Gains and Limits    
        case PC2R_2WHEELS_SPEED_POLAR_PID_SET_GAINS:
            blockMessage = 1;
            robotState.PidSpeedX.Kp = (double) getFloat(payload, 0);
            robotState.PidSpeedX.Ki = getFloat(payload, 4);
            robotState.PidSpeedX.Kd = getFloat(payload, 8);
            robotState.PidSpeedTheta.Kp = getFloat(payload, 12);
            robotState.PidSpeedTheta.Ki = getFloat(payload, 16);
            robotState.PidSpeedTheta.Kd = getFloat(payload, 20);
            robotState.PidSpeedX.erreurProportionelleMax = getFloat(payload, 24);
            robotState.PidSpeedX.erreurIntegraleMax = getFloat(payload, 28);
            robotState.PidSpeedX.erreurDeriveeMax = getFloat(payload, 32);
            robotState.PidSpeedTheta.erreurProportionelleMax = getFloat(payload, 36);
            robotState.PidSpeedTheta.erreurIntegraleMax = getFloat(payload, 40);
            robotState.PidSpeedTheta.erreurDeriveeMax = getFloat(payload, 44);
            break;

        //Set Independant PID Gains and Limits    
        case PC2R_4WHEELS_SPEED_INDEPENDANT_PID_SET_GAINS:
            blockMessage = 1;

            robotState.PidSpeedMotor1.Kp = (double) getFloat(payload, 0);
            robotState.PidSpeedMotor1.Ki = getFloat(payload, 4);
            robotState.PidSpeedMotor1.Kd = getFloat(payload, 8);
            robotState.PidSpeedMotor2.Kp = (double) getFloat(payload, 12);
            robotState.PidSpeedMotor2.Ki = getFloat(payload, 16);
            robotState.PidSpeedMotor2.Kd = getFloat(payload, 20);
            robotState.PidSpeedMotor3.Kp = (double) getFloat(payload, 24);
            robotState.PidSpeedMotor3.Ki = getFloat(payload, 28);
            robotState.PidSpeedMotor3.Kd = getFloat(payload, 32);
            robotState.PidSpeedMotor4.Kp = (double) getFloat(payload, 36);
            robotState.PidSpeedMotor4.Ki = getFloat(payload, 40);
            robotState.PidSpeedMotor4.Kd = getFloat(payload, 44);
            robotState.PidSpeedMotor1.erreurProportionelleMax = getFloat(payload, 48);
            robotState.PidSpeedMotor1.erreurIntegraleMax = getFloat(payload, 52);
            robotState.PidSpeedMotor1.erreurDeriveeMax = getFloat(payload, 56);
            robotState.PidSpeedMotor2.erreurProportionelleMax = getFloat(payload, 60);
            robotState.PidSpeedMotor2.erreurIntegraleMax = getFloat(payload, 64);
            robotState.PidSpeedMotor2.erreurDeriveeMax = getFloat(payload, 68);
            robotState.PidSpeedMotor3.erreurProportionelleMax = getFloat(payload, 72);
            robotState.PidSpeedMotor3.erreurIntegraleMax = getFloat(payload, 76);
            robotState.PidSpeedMotor3.erreurDeriveeMax = getFloat(payload, 80);
            robotState.PidSpeedMotor4.erreurProportionelleMax = getFloat(payload, 84);
            robotState.PidSpeedMotor4.erreurIntegraleMax = getFloat(payload, 88);
            robotState.PidSpeedMotor4.erreurDeriveeMax = getFloat(payload, 92);
            break;
            
            //Set Independant PID Gains and Limits    
        case PC2R_2WHEELS_SPEED_INDEPENDANT_PID_SET_GAINS:
            blockMessage = 1;
            robotState.PidSpeedMotor1.Kp = (double) getFloat(payload, 0);
            robotState.PidSpeedMotor1.Ki = getFloat(payload, 4);
            robotState.PidSpeedMotor1.Kd = getFloat(payload, 8);
            robotState.PidSpeedMotor2.Kp = (double) getFloat(payload, 12);
            robotState.PidSpeedMotor2.Ki = getFloat(payload, 16);
            robotState.PidSpeedMotor2.Kd = getFloat(payload, 20);
            robotState.PidSpeedMotor1.erreurProportionelleMax = getFloat(payload, 24);
            robotState.PidSpeedMotor1.erreurIntegraleMax = getFloat(payload, 28);
            robotState.PidSpeedMotor1.erreurDeriveeMax = getFloat(payload, 32);
            robotState.PidSpeedMotor2.erreurProportionelleMax = getFloat(payload, 36);
            robotState.PidSpeedMotor2.erreurIntegraleMax = getFloat(payload, 40);
            robotState.PidSpeedMotor2.erreurDeriveeMax = getFloat(payload, 44);
            break;
            
        case PC2R_HOLONOMIC_POSITION_PID_SET_GAINS:
        {
            blockMessage = 1;            
            float xKp = getFloatSafe(payload, 0);
            float xKi = getFloatSafe(payload, 4);
            float xKd = getFloatSafe(payload, 8);
            float yKp = getFloat(payload, 12);
            float yKi = getFloat(payload, 16);
            float yKd = getFloat(payload, 20);
            float thetaKp = getFloat(payload, 24);
            float thetaKi = getFloat(payload, 28);
            float thetaKd = getFloat(payload, 32);
            float xKpLimit = getFloat(payload, 36);
            float xKiLimit = getFloat(payload, 40);
            float xKdLimit = getFloat(payload, 44);
            float yKpLimit = getFloat(payload, 48);
            float yKiLimit = getFloat(payload, 52);
            float yKdLimit = getFloat(payload, 56);
            float thetaKpLimit = getFloat(payload, 60);
            float thetaKiLimit = getFloat(payload, 64);
            float thetaKdLimit = getFloat(payload, 68);

            PIDSetup((PidCorrector*)&PID_X, xKp, xKi, xKd, xKpLimit, xKiLimit, xKdLimit);
            PIDSetup((PidCorrector*)&PID_Y, yKp, yKi, yKd, yKpLimit, yKiLimit, yKdLimit);
            PIDSetup((PidCorrector*)&PID_Theta, thetaKp, thetaKi, thetaKd, thetaKpLimit, thetaKiLimit, thetaKdLimit);
        }   
            break;
        case PC2R_IMU_SET_REFERENTIAL:
        {
            ImuReferential ref=payload[0];
            SetImuReferential(ref);
        }
            break;
            
        case PC2R_IMU_SET_OFFSET:
        {
            float offsetAccelX=getFloatSafe(payload,0);
            float offsetAccelY=getFloatSafe(payload,4);
            float offsetAccelZ=getFloatSafe(payload,8);
            float offsetGyroX=getFloatSafe(payload,12);
            float offsetGyroY=getFloatSafe(payload,16);
            float offsetGyroZ=getFloatSafe(payload,20);
            SetImuOffsets(offsetAccelX,offsetAccelY,offsetAccelZ,offsetGyroX,offsetGyroY,offsetGyroZ);
        }
            break;    
            
        case PC2R_USE_C620_ODOMETRIE_INPUT:
            robotState.useC620ForOdometrie=(bool)payload[0];            
            break;
            //Set Independant PID Gains and Limits for motor 5  
        case PC2R_INDIVIDUAL_MOTOR_SPEED_PID_SET_GAINS:
        {
            blockMessage = 1;
            unsigned char nbMotor = payload[0];
            double kp = (double) getFloatSafe(payload, 1);
            double ki = (double) getFloatSafe(payload, 5);
            double kd = (double) getFloatSafe(payload, 9);
            double ePMax = (double) getFloatSafe(payload, 13);
            double eIMax = (double) getFloatSafe(payload, 17);
            double eDMax = (double) getFloatSafe(payload, 21);

            switch (nbMotor) {
                case 1:
                    robotState.PidSpeedMotor1.Kp = kp;
                    robotState.PidSpeedMotor1.Ki = ki;
                    robotState.PidSpeedMotor1.Kd = kd;
                    robotState.PidSpeedMotor1.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor1.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor1.erreurDeriveeMax = eDMax;
                case 2:
                    robotState.PidSpeedMotor2.Kp = kp;
                    robotState.PidSpeedMotor2.Ki = ki;
                    robotState.PidSpeedMotor2.Kd = kd;
                    robotState.PidSpeedMotor2.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor2.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor2.erreurDeriveeMax = eDMax;
                case 3:
                    robotState.PidSpeedMotor3.Kp = kp;
                    robotState.PidSpeedMotor3.Ki = ki;
                    robotState.PidSpeedMotor3.Kd = kd;
                    robotState.PidSpeedMotor3.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor3.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor3.erreurDeriveeMax = eDMax;
                case 4:
                    robotState.PidSpeedMotor4.Kp = kp;
                    robotState.PidSpeedMotor4.Ki = ki;
                    robotState.PidSpeedMotor4.Kd = kd;
                    robotState.PidSpeedMotor4.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor4.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor4.erreurDeriveeMax = eDMax;
                case 5:
                    robotState.PidSpeedMotor5.Kp = kp;
                    robotState.PidSpeedMotor5.Ki = ki;
                    robotState.PidSpeedMotor5.Kd = kd;
                    robotState.PidSpeedMotor5.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor5.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor5.erreurDeriveeMax = eDMax;
                case 6:
                    robotState.PidSpeedMotor6.Kp = kp;
                    robotState.PidSpeedMotor6.Ki = ki;
                    robotState.PidSpeedMotor6.Kd = kd;
                    robotState.PidSpeedMotor6.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor6.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor6.erreurDeriveeMax = eDMax;
                case 7:
                    robotState.PidSpeedMotor7.Kp = kp;
                    robotState.PidSpeedMotor7.Ki = ki;
                    robotState.PidSpeedMotor7.Kd = kd;
                    robotState.PidSpeedMotor7.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor7.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor7.erreurDeriveeMax = eDMax;
                case 8:
                    robotState.PidSpeedMotor8.Kp = kp;
                    robotState.PidSpeedMotor8.Ki = ki;
                    robotState.PidSpeedMotor8.Kd = kd;
                    robotState.PidSpeedMotor8.erreurProportionelleMax = ePMax;
                    robotState.PidSpeedMotor8.erreurIntegraleMax = eIMax;
                    robotState.PidSpeedMotor8.erreurDeriveeMax = eDMax;
                    break;
            }
        }
            break;

        case PC2R_SPEED_PID_RESET:
            blockMessage = 1;
            ResetAllSpeedPid();
            break;
        case PC2R_POSITION_PID_RESET:
            blockMessage = 1;
            //PIDPositionReset();
            ResetGhostToCurrentPosition();
            break;    
        case PC2R_INDIVIDUAL_MOTOR_SPEED_PID_RESET:
            blockMessage = 1;
            ResetIndividualMotorSpeedPid(payload[0]);
            break;
            //Set Speed consigne to Motor
        case PC2R_INDIVIDUAL_MOTOR_SET_SPEED_CONSIGNE:
            blockMessage = 1;
            unsigned char motorNum = payload[0];
            float motorSpeed = getFloatSafe(payload, 1);
            switch (motorNum) {
                case 1:
                    robotState.Motor1SpeedConsigne = motorSpeed;
                    break;
                case 2:
                    robotState.Motor2SpeedConsigne = motorSpeed;
                    break;
                case 3:
                    robotState.Motor3SpeedConsigne = motorSpeed;
                    break;
                case 4:
                    robotState.Motor4SpeedConsigne = motorSpeed;
                    break;
                case 5:
                    robotState.Motor5SpeedConsigne = motorSpeed;
                    break;
                case 6:
                    robotState.Motor6SpeedConsigne = motorSpeed;
                    break;
                case 7:
                    robotState.Motor7SpeedConsigne = motorSpeed;
                    break;
                case 8:
                    robotState.Motor8SpeedConsigne = motorSpeed;
                    break;
                default:
                    break;
            }
            break;
            //Set Motors ON/OFF
        case PC2R_INDIVIDUAL_MOTOR_ENABLE_DISABLE:
        {
            blockMessage = 0;
            msgTxUARTPayloadLength = 2;
            unsigned char motorNum = payload[0];
            unsigned char onOff = payload[1];
            if (onOff) {
                EnablePWM(motorNum, true);
            } else {
                EnablePWM(motorNum, false);
            }
            command = R2PC_INDIVIDUAL_MOTORS_ENABLE_DISABLE_STATUS;
            msgTxUARTPayload[0] = (unsigned char) motorNum;
            msgTxUARTPayload[1] = (unsigned char) onOff;
        }
            break;

            //Set Motors ON/OFF (Propulsion)
        case PC2R_PROPULSION_MOTORS_ENABLE_DISABLE:
        {
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            unsigned char onOff = payload[0];
            if (onOff) {
                robotState.motorEnable = 1;
                ResetAllSpeedPid();
                PWMSetSpeedConsignePolaire4Wheels(0, 0, 0);
                PWMUpdateSpeed();
                EnablePWM(MOTOR_1, true);
                EnablePWM(MOTOR_2, true);
                EnablePWM(MOTOR_3, true);
                EnablePWM(MOTOR_4, true);
            } else {
                robotState.motorEnable = 0;
                EnablePWM(MOTOR_1, false);
                EnablePWM(MOTOR_2, false);
                EnablePWM(MOTOR_3, false);
                EnablePWM(MOTOR_4, false);

            }
            command = R2PC_PROPULSION_MOTORS_ENABLE_DISABLE_STATUS;
            msgTxUARTPayload[0] = (unsigned char) robotState.motorEnable;
        }
            break;
            //Set Carte tir ON/OFF (Power)
        case PC2R_TIR_ENABLE_DISABLE:
        {
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            unsigned char onOff = payload[0];
            if (onOff) {
                ENABLE_TIR = 1;
            } else {
                ENABLE_TIR = 0;
            }
            command = R2PC_TIR_ENABLE_DISABLE_STATUS;
            msgTxUARTPayload[0] = (unsigned char) ENABLE_TIR;
        }
            break;

            //Enable Power Monitoring Datas
        case PC2R_POWER_MONITORING_ENABLE:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0]) {
                robotState.enablePowerMonitoring = true;
            } else {
                robotState.enablePowerMonitoring = false;
            }
            command = R2PC_POWER_MONITORING_ENABLE_STATUS;
            msgTxUARTPayload[0] = robotState.enablePowerMonitoring;
            break;
            //Enable IO Polling
        case PC2R_IO_POLLING_ENABLE:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0]) {
                robotState.enableIOPolling = true;
            } else {
                robotState.enableIOPolling = false;
            }
            command = R2PC_IO_POLLING_ENABLE_STATUS;
            msgTxUARTPayload[0] = robotState.enableIOPolling;
            break;
            //Enable IO Polling
        case PC2R_IO_ANALOG_POLLING_ENABLE:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0]) {
                robotState.enableIOAnalogPolling = true;
            } else {
                robotState.enableIOAnalogPolling = false;
            }
            command = R2PC_IO_ANALOG_POLLING_ENABLE_STATUS;
            msgTxUARTPayload[0] = robotState.enableIOAnalogPolling;
            break;
            //Enable UART RAW RX Datas Forward
        case PC2R_FORWARD_UART_RAW_DATA_ENABLE:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0]) {
                robotState.enableUartRAWForward = true;
            } else {
                robotState.enableUartRAWForward = false;
            }
            command = R2PC_UART_RAW_DATA_FORWARD_STATUS;
            msgTxUARTPayload[0] = robotState.enableUartRAWForward;
            break;
            //Enable Disable Asservissement
        case PC2R_SET_SPEED_ASSERVISSEMENT_MODE:
            blockMessage = 1;
            msgTxUARTPayloadLength = 1;
            switch (payload[0]) {
                case ASSERVISSEMENT_DISABLED_2_WHEELS:
                    robotState.robotSpeedAsservissementState = ASSERVISSEMENT_DISABLED_2_WHEELS;
                    break;
                case ASSERVISSEMENT_POLAIRE_2_WHEELS:
                    robotState.robotSpeedAsservissementState = ASSERVISSEMENT_POLAIRE_2_WHEELS;
                    break;
                case ASSERVISSEMENT_INDEPENDANT_2_WHEELS:
                    robotState.robotSpeedAsservissementState = ASSERVISSEMENT_INDEPENDANT_2_WHEELS;
                    break;
                case ASSERVISSEMENT_DISABLED_4_WHEELS:
                    robotState.robotSpeedAsservissementState = ASSERVISSEMENT_DISABLED_4_WHEELS;
                    break;
                case ASSERVISSEMENT_POLAIRE_4_WHEELS:
                    robotState.robotSpeedAsservissementState = ASSERVISSEMENT_POLAIRE_4_WHEELS;
                    break;
                case ASSERVISSEMENT_INDEPENDANT_4_WHEELS:
                    robotState.robotSpeedAsservissementState = ASSERVISSEMENT_INDEPENDANT_4_WHEELS;
                    break;
                default:
                    robotState.robotSpeedAsservissementState = ASSERVISSEMENT_DISABLED_4_WHEELS;
                    break;
            }
            command = R2PC_ASSERVISSEMENT_MODE_STATUS;
            msgTxUARTPayload[0] = robotState.robotSpeedAsservissementState;
            break;
        case PC2R_SET_POSITION_ASSERVISSEMENT_MODE:
            blockMessage = 1;
            switch (payload[0]) {
                case ASSERVISSEMENT_POSITION_OFF:
                    robotState.robotPositionAsservissementState = ASSERVISSEMENT_POSITION_OFF;
                    break;
                case ASSERVISSEMENT_POSITION_ON:
                    robotState.robotPositionAsservissementState = ASSERVISSEMENT_POSITION_ON;
                    break;
            }
            //Enable Disable Asservissement
        case PC2R_INDIVIDUAL_MOTOR_SET_ASSERVISSEMENT_MODE:
            blockMessage = 1;
            msgTxUARTPayloadLength = 2;

            switch (payload[0]) {
                case 1:
                    robotState.asservissementMoteur1State = payload[1];
                    
                    break;
                case 2:
                    robotState.asservissementMoteur2State = payload[1];
                    break;
                case 3:
                    robotState.asservissementMoteur3State = payload[1];
                    break;
                case 4:
                    robotState.asservissementMoteur4State = payload[1];
                    break;
                case 5:
                    robotState.asservissementMoteur5State = payload[1];
                    break;
                case 6:
                    robotState.asservissementMoteur6State = payload[1];
                    break;
                case 7:
                    robotState.asservissementMoteur7State = payload[1];
                    break;
                case 8:
                    robotState.asservissementMoteur8State = payload[1];
                    break;
                default://robotState.asservissementMoteur5State = ASSERVISSEMENT_DISABLED_5;
                    break;
            }
            command = R2PC_ASSERVISSEMENT_MOTOR_MODE_STATUS;
            ResetIndividualMotorToCurrentPosition(payload[0]);
            msgTxUARTPayload[0] = payload[0];
            msgTxUARTPayload[1] = payload[1];
            break;

        case PC2R_INDIVIDUAL_MOTOR_SET_TRAJECTORY_PARAMETERS:
        {
            blockMessage = 1;
            uint8 nbMotor = payload[0];
            float accelMax = getFloatSafe(payload, 1);
            float speedMax = getFloatSafe(payload, 5);
            SetSingleMotorTrajectoryParameters(nbMotor, accelMax, speedMax);
        }
            break;

        case PC2R_INDIVIDUAL_MOTOR_SET_TARGET_POSITION:
        {
            blockMessage = 1;
            uint8 nbMotor = payload[0];
            float targetPosition = getFloatSafe(payload, 1);
            SetSingleMotorTargetPosition(nbMotor, targetPosition);
        }
            break;

        case PC2R_INDIVIDUAL_MOTOR_SET_POINT_TO_POSITION_CONSTANT:
        {
            blockMessage = 1;
            uint8 nbMotor = payload[0];
            float pointToPositionConstant = getFloatSafe(payload, 1);
            SetPointToPositionSingleMotor(nbMotor, pointToPositionConstant);
        }
            break;

        case PC2R_INDIVIDUAL_MOTOR_SET_PID_PARAMETERS:
        {
            blockMessage = 1;
            uint8 nbMotor = payload[0];
            float Kp = getFloatSafe(payload, 1);
            float Ki = getFloatSafe(payload, 5);
            float Kd = getFloatSafe(payload, 9);
            float corrPMax = getFloatSafe(payload, 13);
            float corrIMax = getFloatSafe(payload, 17);
            float corrDMax = getFloatSafe(payload, 21);
            SetSingleMotorPidParameters(nbMotor, Kp, Ki, Kd, corrPMax, corrIMax, corrDMax);
        }
            break;
        case PC2R_INDIVIDUAL_MOTOR_SET_GHOST_ERROR_MAX:
        {
            blockMessage = 1;
            uint8 nbMotor = payload[0];
            float errorMax = getFloatSafe(payload, 1);
            
            SetSingleMotorTrajectoryErrorMaxParameter(nbMotor, errorMax);
        }
            break;
        case PC2R_INDIVIDUAL_MOTOR_GET_CURRENT_POSITION:
        {
            uint8_t numeroMoteur=payload[0];
            if(numeroMoteur>8)
                break;
            SendIndividualMotorPosition(numeroMoteur);
        }
            break;
            //Enable Disable Motor Current
        case PC2R_MOTOR_CURRENT_MONITORING_ENABLE:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0]) {
                robotState.enableMotorCurrentData = true;
            } else {
                robotState.enableMotorCurrentData = false;
            }
            command = R2PC_MOTOR_CURRENT_MONITORING_ENABLE_STATUS;
            msgTxUARTPayload[0] = robotState.enableMotorCurrentData;
            break;
            //Enable Disable Encoder Raw Data
        case PC2R_ENCODER_RAW_MONITORING_ENABLE:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0]) {
                robotState.enableEncoderRawData = true;
            } else {
                robotState.enableEncoderRawData = false;
            }
            command = R2PC_ENCODER_RAW_MONITORING_ENABLE_STATUS;
            msgTxUARTPayload[0] = robotState.enableEncoderRawData;
            break;

        case PC2R_ODOMETRY_POINT_TO_METER:
            blockMessage = 1;
            robotState.pointToDistance = (double) getFloat(payload, 0);
            break;

        case PC2R_PROPULSION_SPEED_TO_PERCENT_SET:
            blockMessage = 1;
            double mToPercentProp = (double) getFloat(payload, 0);
            Set4WheelsMToPercentProp(mToPercentProp);
            break;
        case PC2R_M_TO_PERCENT_MOTOR_N_SET:
            blockMessage = 1;
            double mToPercent = (double) getFloatSafe(payload, 1);
            SetMotorNMToPercent(payload[0], mToPercent);
            break;
            //        //Set Wheels angles in 4 wheels configuration
            //        case PC2R_4WHEELS_ANGLE_SET:
            //        {
            //            blockMessage=1;
            //            double angleMotor1 = getFloat(payload,0);
            //            double angleMotor2 = getFloat(payload,4);
            //            double angleMotor3 = getFloat(payload,8);
            //            double angleMotor4 = getFloat(payload,12);
            //            Init4WheelsAngles(angleMotor1, angleMotor2, angleMotor3, angleMotor4);
            //        }
            //        break;
            //        
            //        case PC2R_2WHEELS_ANGLE_SET:
            //        {
            //            blockMessage=1;      
            //            double angleMotor1 = getFloat(payload,0);
            //            double angleMotor2 = getFloat(payload,4);      
            //            Init2WheelsAngles(angleMotor1, angleMotor2);
            //        }
            //        break;

            //Set Wheels angles in 4 wheels configurationInit4WheelsToPolarMatrix
        case PC2R_4WHEELS_TO_POLAR_MATRIX_SET:
        {
            blockMessage = 1;
            double mx1 = getFloat(payload, 0);
            double mx2 = getFloat(payload, 4);
            double mx3 = getFloat(payload, 8);
            double mx4 = getFloat(payload, 12);
            double my1 = getFloat(payload, 16);
            double my2 = getFloat(payload, 20);
            double my3 = getFloat(payload, 24);
            double my4 = getFloat(payload, 28);
            double mtheta1 = getFloat(payload, 32);
            double mtheta2 = getFloat(payload, 36);
            double mtheta3 = getFloat(payload, 40);
            double mtheta4 = getFloat(payload, 44);
            Init4WheelsToPolarMatrix(mx1, mx2, mx3, mx4,
                    my1, my2, my3, my4,
                    mtheta1, mtheta2, mtheta3, mtheta4);
        }
            break;
            //Set Wheels angles in 4 wheels configurationInit4WheelsToPolarMatrix
        case PC2R_POLAR_TO_4WHEELS_MATRIX_SET:
        {
            blockMessage = 1;
            double m1x = getFloat(payload, 0);
            double m1y = getFloat(payload, 4);
            double m1t = getFloat(payload, 8);
            double m2x = getFloat(payload, 12);
            double m2y = getFloat(payload, 16);
            double m2t = getFloat(payload, 20);
            double m3x = getFloat(payload, 24);
            double m3y = getFloat(payload, 28);
            double m3t = getFloat(payload, 32);
            double m4x = getFloat(payload, 36);
            double m4y = getFloat(payload, 40);
            double m4t = getFloat(payload, 44);
            InitPolarTo4WheelsMatrix(m1x, m1y, m1t,
                    m2x, m2y, m2t,
                    m3x, m3y, m3t,
                    m4x, m4y, m4t);
        }
            break;

            //Set Wheels angles in 4 wheels configurationInit4WheelsToPolarMatrix
        case PC2R_2WHEELS_TO_POLAR_MATRIX_SET:
        {
            blockMessage = 1;
            double mx1 = getFloat(payload, 0);
            double mx2 = getFloat(payload, 4);
            double mtheta1 = getFloat(payload, 8);
            double mtheta2 = getFloat(payload, 12);
            Init2WheelsToPolarMatrix(mx1, mx2,
                    mtheta1, mtheta2);
        }
            break;

            //        //Enable Disable Motors Speed Consigne Datas
            //        case PC2R_SPEED_CONSIGNE_MONITORING_ENABLE:
            //            blockMessage=0;
            //            msgTxUARTPayloadLength=1;
            //            if(payload[0])
            //            {
            //                robotState.enableMotorSpeedConsigneData=true;
            //            }
            //            else
            //            {
            //                robotState.enableMotorSpeedConsigneData=false;
            //            }
            //            command = R2PC_SPEED_CONSIGNE_MONITORING_ENABLE_STATUS;
            //            msgTxUARTPayload[0]=robotState.enableMotorSpeedConsigneData;
            //            break; 
            //Enable Disable Asservissement Debug Datas
        case PC2R_SPEED_PID_ENABLE_DEBUG_INTERNAL:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0])
                robotState.enablePIDDebugInternal = true;
            else
                robotState.enablePIDDebugInternal = false;

            command = R2PC_SPEED_PID_ENABLE_DEBUG_INTERNAL_STATUS;
            msgTxUARTPayload[0] = robotState.enablePIDDebugInternal;
            break;
            //Command Tir
        case PC2R_TIR_COMMAND:
            blockMessage = 1;
            uint16_t coil1US = BUILD_UINT16(payload[1], payload[0]);
            uint16_t coil2US = BUILD_UINT16(payload[3], payload[2]);
            uint16_t coil3US = BUILD_UINT16(payload[5], payload[4]);
            uint16_t coil4US = BUILD_UINT16(payload[7], payload[6]);
            uint16_t coil1OffsetUS = BUILD_UINT16(payload[9], payload[8]);
            uint16_t coil2OffsetUS = BUILD_UINT16(payload[11], payload[10]);
            uint16_t coil3OffsetUS = BUILD_UINT16(payload[13], payload[12]);
            uint16_t coil4OffsetUS = BUILD_UINT16(payload[15], payload[14]);
            GenerateTir4Coil(coil1US, coil2US, coil3US, coil4US, coil1OffsetUS, coil2OffsetUS, coil3OffsetUS, coil4OffsetUS);
            break;
            //Set Servo Up
        case PC2R_TIR_MOVE_UP:
            blockMessage = 1;
            robotState.servo6Us = 1240;
            break;
            //Set Servo Down
        case PC2R_TIR_MOVE_DOWN:
            blockMessage = 1;
            robotState.servo6Us = 1690;
            break;
            //Set Servo Custom
        case PC2R_TIR_MOVE_SERVO:
            blockMessage = 1;
            robotState.servo6Us = BUILD_UINT32(payload[0], payload[1], payload[2], payload[3]);
            break;
            //Enable PID Advanced debug Data
        case PC2R_SPEED_PID_ENABLE_DEBUG_ERROR_CORRECTION_CONSIGNE:
            blockMessage = 0;
            msgTxUARTPayloadLength = 1;
            if (payload[0])
                robotState.enablePIDDebugErrorCorrectionConsigne = true;
            else
                robotState.enablePIDDebugErrorCorrectionConsigne = false;
            command = R2PC_SPEED_PID_ENABLE_DEBUG_ERROR_CORRECTION_CONSIGNE_STATUS;
            msgTxUARTPayload[0] = robotState.enablePIDDebugErrorCorrectionConsigne;
            break;
        case PC2R_EMERGENCY_STOP:
            blockMessage = 1;
            if (payload[0]) {
                robotState.motorEnable = 0;
                robotState.robotSpeedAsservissementState = ASSERVISSEMENT_DISABLED_4_WHEELS; //TODO choisir entre 2 et 4 roues selon le mode du robot
                SetSpeedConsigneHolonome(0, 0, 0);
                PWMUpdateSpeed();
            } else {
                robotState.motorEnable = 0;
            }
            break;
        case PC2R_UART1_FORWARD:
        {
            blockMessage = 1;
#ifdef USE_UART1
            SendMessageToUart1(payload, length);
#endif
        }
            break;
        case PC2R_UART1_SET_BAUDRATE:
        {
            blockMessage = 1;
#ifdef USE_UART
            unsigned long baudrate = BUILD_UINT32(payload[0], payload[1], payload[2], payload[3]);
            UART1SetBaudRate(baudrate);
#endif
        }
            break;
        case PC2R_UART2_FORWARD:
        {
            blockMessage = 1;
            SendMessageToUart2(payload, length);
        }
            break;
        case PC2R_UART2_SET_BAUDRATE:
        {
            blockMessage = 1;
            unsigned long baudrate = BUILD_UINT32(payload[0], payload[1], payload[2], payload[3]);
            UART2SetBaudRate(baudrate);
        }
            break;
        case PC2R_UART3_FORWARD:
        {
            blockMessage = 1;
            SendMessageToUart3(payload, length);
        }
            break;
        case PC2R_UART3_SET_BAUDRATE:
        {
            blockMessage = 1;
            unsigned long baudrate = BUILD_UINT32(payload[0], payload[1], payload[2], payload[3]);
            UART3SetBaudRate(baudrate);
        }
            break;
        case PC2R_UART4_FORWARD:
        {
            blockMessage = 1;
            SendMessageToUart4(payload, length);
        }
            break;
        case PC2R_UART4_SET_BAUDRATE:
        {
            blockMessage = 1;
            unsigned long baudrate = BUILD_UINT32(payload[0], payload[1], payload[2], payload[3]);
            UART4SetBaudRate(baudrate);
        }
            break;
        case PC2R_CLEAR_ERROR:
            blockMessage = 1;
            flagError.ALL = 0;
            break;
        case PC2R_RESET_EMBEDDED:
            blockMessage = 1;
            Reset();
            break;

            // Unknown command
        default:
        {
            blockMessage = 1;
            msgTxUARTPayloadLength = 1;
            msgTxUARTPayload[0] = (unsigned char) 0;
        }
            break;
    }
    //On envoie le message
    if (blockMessage == 0) {
        MakeAndSendMessageWithUTLNProtocol(command, msgTxUARTPayloadLength, msgTxUARTPayload);
    }
    blockMessage = 0;
}

void MakeAndSendMessageWithUTLNProtocol(unsigned short command, unsigned int payloadLength, unsigned char* payload) {
    unsigned char outPayload[payloadLength + UTLN_FRAME_SIZE];
    int i;
    outPayload[0] = SOF;
    outPayload[1] = MSB_UINT16(command);
    outPayload[2] = LSB_UINT16(command);
#ifdef UTLN_PROTOCOL_V2
    static unsigned int packetNumber = 0;
    outPayload[3] = MSB_UINT16(packetNumber);
    outPayload[4] = LSB_UINT16(packetNumber);
    outPayload[5] = MSB_UINT16(payloadLength);
    outPayload[6] = LSB_UINT16(payloadLength);

    for (i = 0; i < payloadLength; i++) {
        outPayload[7 + i] = payload[i];
    }
    outPayload[7 + payloadLength] = USBCalculateChecksum(command, packetNumber++, payloadLength, payload);
#else
    outPayload[3] = MSB_UINT16(payloadLength);
    outPayload[4] = LSB_UINT16(payloadLength);
    for (i = 0; i < payloadLength; i++) {
        !outPayload[5 + i] = payload[i];
    }
    outPayload[5 + payloadLength] = UartCalculateChecksum(command, payloadLength, payload);
#endif

    //SendMessageToRS232( UTLN_FRAME_SIZE+payloadLength, outPayload);
    SendMessageToUSB(UTLN_FRAME_SIZE + payloadLength, outPayload);
}

void SendMessageToRS232(unsigned short int payloadLength, unsigned char* payload) {
    if (UART3GetRemainingSpaceInTxBuffer() > payloadLength) {
        int i;
        for (i = 0; i < payloadLength; i++) {
            UART3WriteToTxBuffer(payload[i]);
        }
    }
}
#ifdef USE_UART1

void SendMessageToUart1(unsigned char* payload, unsigned short int payloadLength) {
    if (UART1GetRemainingSpaceInTxBuffer() > payloadLength) {
        int i;
        for (i = 0; i < payloadLength; i++) {
            UART1WriteToTxBuffer(payload[i]);
        }
    }
}
#endif

void SendMessageToUart2(unsigned char* payload, unsigned short int payloadLength) {
    if (UART2GetRemainingSpaceInTxBuffer() > payloadLength) {
        int i;
        for (i = 0; i < payloadLength; i++) {
            UART2WriteToTxBuffer(payload[i]);
        }
    }
}

void SendMessageToUart3(unsigned char* payload, unsigned short int payloadLength) {
    if (UART3GetRemainingSpaceInTxBuffer() > payloadLength) {
        int i;
        for (i = 0; i < payloadLength; i++) {
            UART3WriteToTxBuffer(payload[i]);
        }
    }
}

void SendMessageToUart4(unsigned char* payload, unsigned short int payloadLength) {
    if (UART4GetRemainingSpaceInTxBuffer() > payloadLength) {
        int i;
        for (i = 0; i < payloadLength; i++) {
            UART4WriteToTxBuffer(payload[i]);
        }
    }
}
//============================================================================//
// <editor-fold defaultstate="collapsed" desc="Messages robot">

void SendWelcomeMessage(void) {
    unsigned char payload[1];
    MakeAndSendMessageWithUTLNProtocol(R2PC_WELCOME_MESSAGE, 0, payload);
}

void SendErrorMessage(void) {
    unsigned char payload[4];
    payload[0] = BREAK_UINT32(flagError.ALL, 3);
    payload[1] = BREAK_UINT32(flagError.ALL, 2);
    payload[2] = BREAK_UINT32(flagError.ALL, 1);
    payload[3] = BREAK_UINT32(flagError.ALL, 0);
    MakeAndSendMessageWithUTLNProtocol(R2PC_ERROR_FRAME, 4, payload);
}

void Send4WheelsPolarPidConsigneErrorCorrectionData(void) {
    unsigned char payload[40];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedX.erreur);
    getBytesFromFloat(payload, 8, robotState.PidSpeedY.erreur);
    getBytesFromFloat(payload, 12, robotState.PidSpeedTheta.erreur);

    getBytesFromFloat(payload, 16, robotState.xCorrectionSpeedPercent);
    getBytesFromFloat(payload, 20, robotState.yCorrectionSpeedPercent);
    getBytesFromFloat(payload, 24, robotState.thetaCorrectionSpeedPercent);

    getBytesFromFloat(payload, 28, robotState.XSpeedConsigne);
    getBytesFromFloat(payload, 32, robotState.YSpeedConsigne);
    getBytesFromFloat(payload, 36, robotState.ThetaSpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_4WHEELS_SPEED_POLAR_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 40, payload);
}

void Send2WheelsPolarPidConsigneErrorCorrectionData(void) {
    unsigned char payload[28];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedX.erreur);
    getBytesFromFloat(payload, 8, robotState.PidSpeedTheta.erreur);

    getBytesFromFloat(payload, 12, robotState.xCorrectionSpeedPercent);
    getBytesFromFloat(payload, 16, robotState.thetaCorrectionSpeedPercent);

    getBytesFromFloat(payload, 20, robotState.XSpeedConsigne);
    getBytesFromFloat(payload, 24, robotState.ThetaSpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_2WHEELS_SPEED_POLAR_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 28, payload);
}

void Send4WheelsIndependantPidConsigneErrorCorrectionData() {
    unsigned char payload[52];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor1.erreur);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor2.erreur);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor3.erreur);
    getBytesFromFloat(payload, 16, robotState.PidSpeedMotor4.erreur);

    getBytesFromFloat(payload, 20, robotState.Motor1SpeedCommand);
    getBytesFromFloat(payload, 24, robotState.Motor2SpeedCommand);
    getBytesFromFloat(payload, 28, robotState.Motor3SpeedCommand);
    getBytesFromFloat(payload, 32, robotState.Motor4SpeedCommand);

    getBytesFromFloat(payload, 36, robotState.Motor1SpeedConsigne);
    getBytesFromFloat(payload, 40, robotState.Motor2SpeedConsigne);
    getBytesFromFloat(payload, 44, robotState.Motor3SpeedConsigne);
    getBytesFromFloat(payload, 48, robotState.Motor4SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_4WHEELS_SPEED_INDEPENDANT_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 52, payload);
}

void Send2WheelsIndependantPidConsigneErrorCorrectionData() {
    unsigned char payload[28];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor1.erreur);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor2.erreur);

    getBytesFromFloat(payload, 12, robotState.Motor1SpeedCommand);
    getBytesFromFloat(payload, 16, robotState.Motor2SpeedCommand);

    getBytesFromFloat(payload, 20, robotState.Motor1SpeedConsigne);
    getBytesFromFloat(payload, 24, robotState.Motor2SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_2WHEELS_SPEED_INDEPENDANT_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 28, payload);
}

void SendMotor1PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor1.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur1CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor1SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR1_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void SendMotor2PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor2.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur2CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor2SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR2_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void SendMotor3PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor3.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur3CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor3SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR3_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void SendMotor4PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor4.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur4CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor4SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR4_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void SendMotor5PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor5.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur5CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor5SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR5_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void SendMotor6PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor6.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur6CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor6SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR6_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void SendMotor7PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor7.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur7CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor7SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR7_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void SendMotor8PidConsigneErrorCorrectionData() {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);

    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor8.erreur);
    getBytesFromFloat(payload, 8, robotState.vitesseMoteur8CommandePercent);
    getBytesFromFloat(payload, 12, robotState.Motor8SpeedConsigne);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR8_PID_COMMAND_ERROR_CORRECTION_CONSIGNE, 16, payload);
}

void Send4WheelsPolarPidCorrectionData(void) {
    unsigned char payload[40];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedX.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedX.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedX.corrD);

    getBytesFromFloat(payload, 16, robotState.PidSpeedY.corrP);
    getBytesFromFloat(payload, 20, robotState.PidSpeedY.corrI);
    getBytesFromFloat(payload, 24, robotState.PidSpeedY.corrD);

    getBytesFromFloat(payload, 28, robotState.PidSpeedTheta.corrP);
    getBytesFromFloat(payload, 32, robotState.PidSpeedTheta.corrI);
    getBytesFromFloat(payload, 36, robotState.PidSpeedTheta.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_4WHEELS_SPEED_POLAR_PID_CORRECTIONS, 40, payload);
}

void Send4WheelsIndependantPidCorrectionData(void) {
    unsigned char payload[52];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor1.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor1.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor1.corrD);

    getBytesFromFloat(payload, 16, robotState.PidSpeedMotor2.corrP);
    getBytesFromFloat(payload, 20, robotState.PidSpeedMotor2.corrI);
    getBytesFromFloat(payload, 24, robotState.PidSpeedMotor2.corrD);

    getBytesFromFloat(payload, 28, robotState.PidSpeedMotor3.corrP);
    getBytesFromFloat(payload, 32, robotState.PidSpeedMotor3.corrI);
    getBytesFromFloat(payload, 36, robotState.PidSpeedMotor3.corrD);

    getBytesFromFloat(payload, 40, robotState.PidSpeedMotor4.corrP);
    getBytesFromFloat(payload, 44, robotState.PidSpeedMotor4.corrI);
    getBytesFromFloat(payload, 48, robotState.PidSpeedMotor4.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_4WHEELS_SPEED_INDEPENDANT_PID_CORRECTIONS, 52, payload);
}

void Send2WheelsPolarPidCorrectionData(void) {
    unsigned char payload[28];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedX.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedX.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedX.corrD);

    getBytesFromFloat(payload, 16, robotState.PidSpeedTheta.corrP);
    getBytesFromFloat(payload, 20, robotState.PidSpeedTheta.corrI);
    getBytesFromFloat(payload, 24, robotState.PidSpeedTheta.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_2WHEELS_SPEED_POLAR_PID_CORRECTIONS, 28, payload);
}

void Send2WheelsIndependantPidCorrectionData(void) {
    unsigned char payload[28];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor1.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor1.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor1.corrD);

    getBytesFromFloat(payload, 16, robotState.PidSpeedMotor2.corrP);
    getBytesFromFloat(payload, 20, robotState.PidSpeedMotor2.corrI);
    getBytesFromFloat(payload, 24, robotState.PidSpeedMotor2.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_4WHEELS_SPEED_INDEPENDANT_PID_CORRECTIONS, 28, payload);
}

void SendMotor1PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor1.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor1.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor1.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR1_PID_CORRECTIONS, 16, payload);
}

void SendMotor2PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor2.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor2.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor2.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR2_PID_CORRECTIONS, 16, payload);
}

void SendMotor3PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor3.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor3.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor3.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR3_PID_CORRECTIONS, 16, payload);
}

void SendMotor4PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor4.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor4.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor4.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR4_PID_CORRECTIONS, 16, payload);
}

void SendMotor5PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor5.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor5.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor5.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR5_PID_CORRECTIONS, 16, payload);
}

void SendMotor6PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor6.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor6.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor6.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR6_PID_CORRECTIONS, 16, payload);
}

void SendMotor7PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor7.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor7.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor7.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR7_PID_CORRECTIONS, 16, payload);
}

void SendMotor8PidCorrectionData(void) {
    unsigned char payload[16];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.PidSpeedMotor8.corrP);
    getBytesFromFloat(payload, 8, robotState.PidSpeedMotor8.corrI);
    getBytesFromFloat(payload, 12, robotState.PidSpeedMotor8.corrD);

    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_MOTOR8_PID_CORRECTIONS, 16, payload);
}

void SendAsservissementModeStatus(void) {
    unsigned char payload[1];
    payload[0] = robotState.robotSpeedAsservissementState;
    MakeAndSendMessageWithUTLNProtocol(R2PC_ASSERVISSEMENT_MODE_STATUS, 1, payload);
}

void SendAuxiliaryOdometryData(void) {
    unsigned char payload[20];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.Motor5SpeedOdometry);
    getBytesFromFloat(payload, 8, robotState.Motor6SpeedOdometry);
    getBytesFromFloat(payload, 12, robotState.Motor7SpeedOdometry);
    getBytesFromFloat(payload, 16, robotState.Motor8SpeedOdometry);
    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_AUXILIARY_ODOMETRY, 20, payload);
}

void SendEncoderRawData(void) {
    unsigned char payload[36];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromInt32(payload, 4, robotState.motor1EncoderRawValue);
    getBytesFromInt32(payload, 8, robotState.motor2EncoderRawValue);
    getBytesFromInt32(payload, 12, robotState.motor3EncoderRawValue);
    getBytesFromInt32(payload, 16, robotState.motor4EncoderRawValue);
    getBytesFromInt32(payload, 20, robotState.motor5EncoderRawValue);
    getBytesFromInt32(payload, 24, robotState.motor6EncoderRawValue);
    getBytesFromInt32(payload, 28, robotState.motor7EncoderRawValue);
    getBytesFromInt32(payload, 32, robotState.motor8EncoderRawValue);
    MakeAndSendMessageWithUTLNProtocol(R2PC_ENCODER_RAW_DATA, 36, payload);
}

void SendPolarAndIndependantOdometrySpeed(void) {
    unsigned char payload[44];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.xVitesseFromOdometry);
    getBytesFromFloat(payload, 8, robotState.yVitesseFromOdometry);
    getBytesFromFloat(payload, 12, robotState.thetaVitesseFromOdometry);
    getBytesFromFloat(payload, 16, robotState.xSpeedKalmanRefRobot);
    getBytesFromFloat(payload, 20, robotState.ySpeedKalmanRefRobot);
    getBytesFromFloat(payload, 24, robotState.thetaSpeedRefTerrain);
    getBytesFromFloat(payload, 28, robotState.Motor1SpeedOdometry);
    getBytesFromFloat(payload, 32, robotState.Motor2SpeedOdometry);
    getBytesFromFloat(payload, 36, robotState.Motor3SpeedOdometry);
    getBytesFromFloat(payload, 40, robotState.Motor4SpeedOdometry);
    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_POLAR_AND_INDEPENDANT_ODOMETRY, 44, payload);
}

void SendPolarAndIndependantOdometrySpeedAndIMU(void) {
    unsigned char payload[68];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.xVitesseFromOdometry);
    getBytesFromFloat(payload, 8, robotState.yVitesseFromOdometry);
    getBytesFromFloat(payload, 12, robotState.thetaVitesseFromOdometry);
    getBytesFromFloat(payload, 16, robotState.xSpeedKalmanRefRobot);
    getBytesFromFloat(payload, 20, robotState.ySpeedKalmanRefRobot);
    getBytesFromFloat(payload, 24, robotState.thetaSpeedRefTerrain);
    getBytesFromFloat(payload, 28, robotState.Motor1SpeedOdometry);
    getBytesFromFloat(payload, 32, robotState.Motor2SpeedOdometry);
    getBytesFromFloat(payload, 36, robotState.Motor3SpeedOdometry);
    getBytesFromFloat(payload, 40, robotState.Motor4SpeedOdometry);
    getBytesFromFloat(payload, 44, (float) robotState.accelDataXYZ.X);
    getBytesFromFloat(payload, 48, (float) robotState.accelDataXYZ.Y);
    getBytesFromFloat(payload, 52, (float) robotState.accelDataXYZ.Z);
    getBytesFromFloat(payload, 56, (float) robotState.gyroDataXYZ.X);
    getBytesFromFloat(payload, 60, (float) robotState.gyroDataXYZ.Y);
    getBytesFromFloat(payload, 64, (float) robotState.gyroDataXYZ.Z);
    MakeAndSendMessageWithUTLNProtocol(R2PC_SPEED_POLAR_AND_INDEPENDANT_ODOMETRY_AND_IMU, 68, payload);
}

void SendPositionAbsolue(void) {
    unsigned char payload[28];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.xPositionRefTerrain);
    getBytesFromFloat(payload, 8, robotState.yPositionRefTerrain);
    getBytesFromFloat(payload, 12, robotState.thetaPositionRefTerrain);
    getBytesFromFloat(payload, 16, robotState.xSpeedRefTerrain);
    getBytesFromFloat(payload, 20, robotState.ySpeedRefTerrain);
    getBytesFromFloat(payload, 24, robotState.thetaSpeedRefTerrain);
    MakeAndSendMessageWithUTLNProtocol(R2PC_POSITION_ABSOLUE, 28, payload);
}

void SendPositionAbsolueAndGhostAndWayPoint(void) {
    unsigned char payload[52];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, robotState.xPositionRefTerrain);
    getBytesFromFloat(payload, 8, robotState.yPositionRefTerrain);
    getBytesFromFloat(payload, 12, robotState.thetaPositionRefTerrain);
    getBytesFromFloat(payload, 16, robotState.xSpeedRefTerrain);
    getBytesFromFloat(payload, 20, robotState.ySpeedRefTerrain);
    getBytesFromFloat(payload, 24, robotState.thetaSpeedRefTerrain);
    getBytesFromFloat(payload, 28, robotState.xGhost);
    getBytesFromFloat(payload, 32, robotState.yGhost);
    getBytesFromFloat(payload, 36, robotState.thetaGhost);
    getBytesFromFloat(payload, 40, robotState.xWaypoint);
    getBytesFromFloat(payload, 44, robotState.yWaypoint);
    getBytesFromFloat(payload, 48, robotState.thetaWaypoint);
    MakeAndSendMessageWithUTLNProtocol(R2PC_POSITION_ABSOLUE_GHOST_WAYPOINT, 52, payload);
}

void SendIMUData(void) {
    unsigned char payload[28];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, (float) robotState.accelDataXYZ.X);
    getBytesFromFloat(payload, 8, (float) robotState.accelDataXYZ.Y);
    getBytesFromFloat(payload, 12, (float) robotState.accelDataXYZ.Z);
    getBytesFromFloat(payload, 16, (float) robotState.gyroDataXYZ.X);
    getBytesFromFloat(payload, 20, (float) robotState.gyroDataXYZ.Y);
    getBytesFromFloat(payload, 24, (float) robotState.gyroDataXYZ.Z);
    MakeAndSendMessageWithUTLNProtocol(R2PC_IMU_DATA, 28, payload);
}

void SendPowerMonitoringValues(void) {
    unsigned char payload[32];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, (float) robotState.battCMDVoltage);
    getBytesFromFloat(payload, 8, (float) robotState.battCMDCurrent);
    getBytesFromFloat(payload, 12, (float) robotState.battPWRVoltage);
    getBytesFromFloat(payload, 16, (float) robotState.battPWRCurrent);
    MakeAndSendMessageWithUTLNProtocol(R2PC_POWER_MONITORING, 20, payload);
}

void SendIOPollingValues(void) {
    unsigned char payload[32];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    unsigned char ioValues = _JACK;
    ioValues |= (_CONFIG1 << 1) | (_CONFIG2 << 2) | (_CONFIG3 << 3) | (_CONFIG4 << 4);
    payload[4] = ioValues;
    MakeAndSendMessageWithUTLNProtocol(R2PC_IO_MONITORING, 5, payload);
}

void SendIOAnalogPollingValues(void) {
    unsigned char payload[32];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, (float) robotState.ana0_B0Value);
    getBytesFromFloat(payload, 8, (float) robotState.ana8_B8Value);
    getBytesFromFloat(payload, 12, (float) robotState.ana9_B9Value);
    getBytesFromFloat(payload, 16, (float) robotState.ana10_B10Value);
    getBytesFromFloat(payload, 20, (float) robotState.ana11_B11Value);
    MakeAndSendMessageWithUTLNProtocol(R2PC_IO_ANALOG_MONITORING, 24, payload);
}

void SendMotorCurrent(void) {
    unsigned char payload[36];
    payload[0] = BREAK_UINT32(g_longTimeStamp, 3);
    payload[1] = BREAK_UINT32(g_longTimeStamp, 2);
    payload[2] = BREAK_UINT32(g_longTimeStamp, 1);
    payload[3] = BREAK_UINT32(g_longTimeStamp, 0);
    getBytesFromFloat(payload, 4, (float) robotState.motor1Current);
    getBytesFromFloat(payload, 8, (float) robotState.motor2Current);
    getBytesFromFloat(payload, 12, (float) robotState.motor3Current);
    getBytesFromFloat(payload, 16, (float) robotState.motor4Current);
    getBytesFromFloat(payload, 20, (float) robotState.motor5Current);
    getBytesFromFloat(payload, 24, (float) robotState.motor6Current);
    getBytesFromFloat(payload, 28, (float) robotState.motor7Current);
    getBytesFromFloat(payload, 32, (float) robotState.motor8Current);
    MakeAndSendMessageWithUTLNProtocol(R2PC_MOTOR_CURRENTS_MONITORING, 36, payload);
}

void SendErrorText(const char* str) {
    int len = strlen(str);
    MakeAndSendMessageWithUTLNProtocol(R2PC_ERROR_MESSAGE, len, (unsigned char*) str);
}

void SetSpeedConsigneHolonome(float vx, float vy, float vtheta) {
    robotState.XSpeedConsigne = vx;
    robotState.YSpeedConsigne = vy;
    robotState.ThetaSpeedConsigne = vtheta;
}

void SendMotorPositionGhostOnError(void) {
    unsigned char payload[32];
    getBytesFromFloat(payload, 0, (float) trajectoryGeneratorMotor[MotorIndex(1)].currentPosition);
    getBytesFromFloat(payload, 4, (float) trajectoryGeneratorMotor[MotorIndex(2)].currentPosition);
    getBytesFromFloat(payload, 8, (float) trajectoryGeneratorMotor[MotorIndex(3)].currentPosition);
    getBytesFromFloat(payload, 12, (float) trajectoryGeneratorMotor[MotorIndex(4)].currentPosition);
    getBytesFromFloat(payload, 16, (float) trajectoryGeneratorMotor[MotorIndex(5)].currentPosition);
    getBytesFromFloat(payload, 20, (float) trajectoryGeneratorMotor[MotorIndex(6)].currentPosition);
    getBytesFromFloat(payload, 24, (float) trajectoryGeneratorMotor[MotorIndex(7)].currentPosition);
    getBytesFromFloat(payload, 28, (float) trajectoryGeneratorMotor[MotorIndex(8)].currentPosition);
    uint8_t errorFlag=0;
    int i;
    for(i=0;i<8;i++)
    {
        errorFlag|= (trajectoryGeneratorMotor[i].hasGhostError)?(1<<i):0;
    }
    payload[32]=errorFlag;
    MakeAndSendMessageWithUTLNProtocol(R2PC_MOTOR_POSITION_ON_GHOST_ERROR, 33, payload);
}

void SendIndividualMotorPosition(uint8_t numeroMoteur) {
    unsigned char payload[5];
    payload[0]=numeroMoteur;
    getBytesFromFloat(payload, 1, (float) robotState.motorCurrentPosition[MotorIndex(numeroMoteur)]);
    MakeAndSendMessageWithUTLNProtocol(R2PC_INDIVIDUAL_MOTOR_POSITION, 5, payload);
}
// </editor-fold>
/*******************************************************************************
 End of File
 */


