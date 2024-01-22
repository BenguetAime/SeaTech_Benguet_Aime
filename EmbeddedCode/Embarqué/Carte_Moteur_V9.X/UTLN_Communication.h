/* 
 * File:   ustv_MainFunc.h
 * Author: Val
 *
 * Created on 21 janvier 2013, 16:20
 */

#ifndef USTV_COMMUNICATION_H
#define	USTV_COMMUNICATION_H

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "OSAL.h"
#include "TrajectoryGeneratorHolonome.h"

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
extern volatile unsigned long g_longTimeStamp;

#define UTLN_PROTOCOL_V2
#define SOF 0xFE
#ifdef UTLN_PROTOCOL_V1
#define UTLN_FRAME_SIZE 6
#else
#define UTLN_FRAME_SIZE 8           //SoF-CMD1-CMD0-LEN1-LEN0-NBPKT1-NBPKT0-PAYLOAD-CHKSUM
#endif


#define SET_LED_VALUE 0x0001
#define SET_TIMER_FREQUENCY 0x0002
#define SET_IO_DIR 0x0003
#define SET_IO_MODE 0x0004
#define SET_IO_VALUE 0x0005
#define SET_VIRTUAL_TIMER_FREQUENCY 0x0006

#define POLOLU_SERVO_SET_POSITION 0x0084



#define R2PC_WELCOME_MESSAGE 0x0100
#define R2PC_ERROR_MESSAGE 0x0101

#define R2PC_IMU_DATA 0x0110                            //Timestamp(4L) - AccX(4F) - AccY(4F) - AccZ(4F) - GyroX(4F) - GyroY(4F) - GyroZ(4F)
#define R2PC_IO_MONITORING 0x0120                       //Timestamp(4) - Jack| Config1 | Config2 | Config3 | Config4
#define R2PC_IO_ANALOG_MONITORING 0x0121                //Timestamp(4) - ANA0(4F) - ANA8(4F) - ANA9(4F) - ANA10(4F) - ANA11(4F)
#define R2PC_POWER_MONITORING 0x0130                    //Timestamp(4L) - BattCmdVoltage(4F) - BattCmdCurrent(4F) - BattPwrVoltage(4F) - BattPwrCurrent(4F)
#define R2PC_ENCODER_RAW_DATA 0x0140                    //Timestamp(4L) - Enc Motor 1 Value(4L) - ... - Enc Motor 7 Value(4L)

#define R2PC_POSITION_ABSOLUE_GHOST_WAYPOINT 0x014D     //Timestamp(4L) - X(4F) - Y(4F) - Theta(4F) - Vx(4F) - Vy(4F) - VTheta(4F) - XGhost(4F) - YGhost(4F) - ThetaGhost(4F) - XWayPoint(4F) - YWayPoint(4F) - ThetaWayPoint(4F)
#define R2PC_POSITION_ABSOLUE 0x014E                    //Timestamp(4L) - X(4F) - Y(4F) - Theta(4F) - Vx(4F) - Vy(4F) - VTheta(4F) 
#define R2PC_SPEED_POLAR_AND_INDEPENDANT_ODOMETRY_AND_IMU 0x014F //Timestamp(4L) - Vx(4F) - Vy(4F) - VTheta(4F) - VM1(4F) - VM2(4F) - VM3(4F) - VM4(4F) - AccX(4F) - AccY(4F) - AccZ(4F) - GyroX(4F) - GyroY(4F) - GyroZ(4F) 
#define R2PC_SPEED_POLAR_AND_INDEPENDANT_ODOMETRY 0x0150 //Timestamp(4L) - Vx(4F) - Vy(4F) - VTheta(4F) - VM1(4F) - VM2(4F) - VM3(4F) - VM4(4F)
#define R2PC_SPEED_AUXILIARY_ODOMETRY 0x0151             //Timestamp(4L) - VM5(4F) - VM6(4F) - VM7(4F) 
#define R2PC_4WHEELS_SPEED_POLAR_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0152        //Timestamp(4L) - ErrX(4F) - ErrY(4F) - ErrTh(4F) - CorrX(4F) - CorrY(4F) - CorrTh(4F) - ConsX(4F) - ConsY(4F) - ConsTh(4F)
#define R2PC_4WHEELS_SPEED_INDEPENDANT_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0153  //Timestamp(4L) - ErrM1(4F) - ErrM2(4F) - ErrM3(4F) - ErrM4(4F) - CorrM1(4F) - CorrM2(4F) - CorrM3(4F) - CorrM4(4F) - ConsM(4F) - ConsM2(4F) - ConsM3(4F) - ConsM4(4F)
#define R2PC_4WHEELS_SPEED_POLAR_PID_CORRECTIONS 0x0154          //Timestamp(4L) - CorrPx(4F) - CorrIx(4F) - CorrDx(4F) - CorrPy(4F) - CorrIy(4F) - CorrDy(4F) - CorrPTh(4F) - CorrITh(4F) - CorrDTh(4F)  
#define R2PC_4WHEELS_SPEED_INDEPENDANT_PID_CORRECTIONS 0x0155    //Timestamp(4L) - CorrPM1(4F) - CorrIM1(4F) - CorrDM1(4F) - CorrPM2(4F) - CorrIM2(4F) - CorrDM2(4F) - CorrPM3(4F) - CorrIM3(4F) - CorrDM3(4F) - CorrPM4(4F) - CorrIM4(4F) - CorrDM4(4F)
#define R2PC_SPEED_MOTOR5_PID_CORRECTIONS 0x0157        //Timestamp(4L) - CorrPM5(4F) - CorrIM5(4F) - CorrDM5(4F)
#define R2PC_SPEED_MOTOR6_PID_CORRECTIONS 0x0158        //Timestamp(4L) - CorrPM6(4F) - CorrIM6(4F) - CorrDM6(4F)
#define R2PC_SPEED_MOTOR7_PID_CORRECTIONS 0x0159        //Timestamp(4L) - CorrPM7(4F) - CorrIM7(4F) - CorrDM7(4F)
#define R2PC_SPEED_MOTOR8_PID_CORRECTIONS 0x015A        //Timestamp(4L) - CorrPM8(4F) - CorrIM8(4F) - CorrDM8(4F)
#define R2PC_SPEED_MOTOR1_PID_CORRECTIONS 0x015B        //Timestamp(4L) - CorrPM1(4F) - CorrIM1(4F) - CorrDM1(4F)
#define R2PC_SPEED_MOTOR2_PID_CORRECTIONS 0x015C        //Timestamp(4L) - CorrPM2(4F) - CorrIM2(4F) - CorrDM2(4F)
#define R2PC_SPEED_MOTOR3_PID_CORRECTIONS 0x015D        //Timestamp(4L) - CorrPM3(4F) - CorrIM3(4F) - CorrDM3(4F)
#define R2PC_SPEED_MOTOR4_PID_CORRECTIONS 0x015E        //Timestamp(4L) - CorrPM4(4F) - CorrIM4(4F) - CorrDM4(4F)

#define R2PC_MOTOR_CURRENTS_MONITORING 0x0160           //Timestamp(4L) - Motor Current 1 (4F) - ... - Motor Current 7 (4F)
#define R2PC_2WHEELS_SPEED_POLAR_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0162        //Timestamp(4L) - ErrX(4F) - ErrTh(4F) - CorrX(4F) - CorrTh(4F) - ConsX(4F) - ConsTh(4F)
#define R2PC_2WHEELS_SPEED_INDEPENDANT_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0163  //Timestamp(4L) - ErrM1(4F) - ErrM2(4F) - ErrM3(4F) - ErrM4(4F) - CorrM1(4F) - CorrM2(4F) - CorrM3(4F) - CorrM4(4F) - ConsM(4F) - ConsM2(4F) - ConsM3(4F) - ConsM4(4F)
#define R2PC_2WHEELS_SPEED_POLAR_PID_CORRECTIONS 0x0164          //Timestamp(4L) - CorrPx(4F) - CorrIx(4F) - CorrDx(4F) - CorrPy(4F) - CorrIy(4F) - CorrDy(4F) - CorrPTh(4F) - CorrITh(4F) - CorrDTh(4F)  
#define R2PC_2WHEELS_SPEED_INDEPENDANT_PID_CORRECTIONS 0x0165    //Timestamp(4L) - CorrPM1(4F) - CorrIM1(4F) - CorrDM1(4F) - CorrPM2(4F) - CorrIM2(4F) - CorrDM2(4F) - CorrPM3(4F) - CorrIM3(4F) - CorrDM3(4F) - CorrPM4(4F) - CorrIM4(4F) - CorrDM4(4F)
#define R2PC_SPEED_MOTOR5_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0166  //Timestamp(4L) - ErrM5(4F) - CorrM5(4F) - ConsM5(4F)
#define R2PC_SPEED_MOTOR6_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0167  //Timestamp(4L) - ErrM6(4F) - CorrM6(4F) - ConsM6(4F)
#define R2PC_SPEED_MOTOR7_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0168  //Timestamp(4L) - ErrM7(4F) - CorrM7(4F) - ConsM7(4F)
#define R2PC_SPEED_MOTOR8_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x0169  //Timestamp(4L) - ErrM8(4F) - CorrM8(4F) - ConsM8(4F)
#define R2PC_SPEED_MOTOR1_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x016A  //Timestamp(4L) - ErrM5(4F) - CorrM5(4F) - ConsM1(4F)
#define R2PC_SPEED_MOTOR2_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x016B  //Timestamp(4L) - ErrM6(4F) - CorrM6(4F) - ConsM2(4F)
#define R2PC_SPEED_MOTOR3_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x016C  //Timestamp(4L) - ErrM7(4F) - CorrM7(4F) - ConsM3(4F)
#define R2PC_SPEED_MOTOR4_PID_COMMAND_ERROR_CORRECTION_CONSIGNE 0x016D  //Timestamp(4L) - ErrM8(4F) - CorrM8(4F) - ConsM4(4F)
#define R2PC_MOTOR_POSITION_ON_GHOST_ERROR 0x016E
#define R2PC_INDIVIDUAL_MOTOR_POSITION 0x016F

#define R2PC_INDIVIDUAL_MOTORS_ENABLE_DISABLE_STATUS 0x017E                 //Enable-Disable (2 Byte)-> MotorNum(1 byte) - EnableDisable(1 byte)
#define R2PC_PREHENSION_MOTORS_ENABLE_DISABLE_STATUS 0x017F                 //Enable-Disable (1 Byte)
#define R2PC_IO_POLLING_ENABLE_STATUS 0x0180
#define R2PC_POWER_MONITORING_ENABLE_STATUS 0x0181                         //Enable-Disable (1 Byte)
#define R2PC_ENCODER_RAW_MONITORING_ENABLE_STATUS 0x0182                   //Enable-Disable (1 Byte)
#define R2PC_ASSERVISSEMENT_MODE_STATUS 0x0183                         //Enable-Disable (1 Byte)
#define R2PC_SPEED_PID_ENABLE_DEBUG_ERROR_CORRECTION_CONSIGNE_STATUS 0x0184
#define R2PC_SPEED_PID_ENABLE_DEBUG_INTERNAL_STATUS 0x0185                     //Enable-Disable (1 Byte)
#define R2PC_SPEED_CONSIGNE_MONITORING_ENABLE_STATUS 0x0186                //Enable-Disable (1 Byte)
#define R2PC_PROPULSION_MOTORS_ENABLE_DISABLE_STATUS 0x0187                           //Enable-Disable (1 Byte)
#define R2PC_MOTOR_CURRENT_MONITORING_ENABLE_STATUS 0x0188                 //Enable-Disable (1 Byte)
#define R2PC_TIR_ENABLE_DISABLE_STATUS 0x0189                              //Enable-Disable (1 Byte)
#define R2PC_UART_RAW_DATA_FORWARD_STATUS 0x018A
#define R2PC_IO_ANALOG_POLLING_ENABLE_STATUS 0x018B
#define R2PC_ASSERVISSEMENT_MOTOR_MODE_STATUS 0x018C  

#define R2PC_ABSOLUTE_POSITION 0x0190


#define R2PC_UART1_FORWARD 0x0380
#define R2PC_UART2_FORWARD 0x0390
#define R2PC_UART3_FORWARD 0x03A0
#define R2PC_UART4_FORWARD 0x03B0



#define PC2R_EMERGENCY_STOP 0x0200

#define PC2R_IO_POLLING_ENABLE 0x0220
#define PC2R_IO_POLLING_SET_FREQUENCY 0x0221
#define PC2R_IO_ANALOG_POLLING_ENABLE 0x0222
#define PC2R_FORWARD_UART_RAW_DATA_ENABLE 0x0223

#define PC2R_POWER_MONITORING_ENABLE 0x0230

#define PC2R_ENCODER_RAW_MONITORING_ENABLE 0x0240
#define PC2R_ODOMETRY_POINT_TO_METER 0x241
#define PC2R_PROPULSION_SPEED_TO_PERCENT_SET 0x0242
#define PC2R_4WHEELS_TO_POLAR_MATRIX_SET 0x243
//#define PC2R_2WHEELS_ANGLE_SET 0x244
#define PC2R_2WHEELS_TO_POLAR_MATRIX_SET 0x245
#define PC2R_POLAR_TO_4WHEELS_MATRIX_SET 0x246
#define PC2R_M_TO_PERCENT_MOTOR_N_SET 0x0247


#define PC2R_SET_SPEED_ASSERVISSEMENT_MODE 0x0250                                     //Mode asservissement du robot
#define PC2R_SPEED_PID_ENABLE_DEBUG_ERROR_CORRECTION_CONSIGNE 0x0251
#define PC2R_SPEED_PID_ENABLE_DEBUG_INTERNAL 0x0252
#define PC2R_SPEED_CONSIGNE_MONITORING_ENABLE 0x0253
#define PC2R_4WHEELS_SPEED_POLAR_PID_SET_GAINS 0x0254
#define PC2R_4WHEELS_SPEED_INDEPENDANT_PID_SET_GAINS 0x0255
#define PC2R_SPEED_POLAR_SET_CONSIGNE 0x0256
#define PC2R_INDIVIDUAL_MOTOR_SET_SPEED_CONSIGNE 0x0257
#define PC2R_SPEED_PID_RESET 0x258
#define PC2R_INDIVIDUAL_MOTOR_SET_ASSERVISSEMENT_MODE 0x0259
#define PC2R_INDIVIDUAL_MOTOR_SPEED_PID_RESET 0x025A
#define PC2R_INDIVIDUAL_MOTOR_SET_TRAJECTORY_PARAMETERS 0x025B
#define PC2R_INDIVIDUAL_MOTOR_SET_TARGET_POSITION 0x025C
#define PC2R_INDIVIDUAL_MOTOR_SET_PID_PARAMETERS 0x025D


#define PC2R_INDIVIDUAL_MOTOR_ENABLE_DISABLE 0x025E
#define PC2R_PREHENSION_MOTORS_ENABLE_DISABLE 0x025F
#define PC2R_PROPULSION_MOTORS_ENABLE_DISABLE 0x0260
#define PC2R_MOTOR_CURRENT_MONITORING_ENABLE 0x0261
#define PC2R_2WHEELS_SPEED_POLAR_PID_SET_GAINS 0x0264
#define PC2R_2WHEELS_SPEED_INDEPENDANT_PID_SET_GAINS 0x0265
#define PC2R_INDIVIDUAL_MOTOR_SPEED_PID_SET_GAINS 0x0266
#define PC2R_INDIVIDUAL_MOTOR_SET_POINT_TO_POSITION_CONSTANT 0x0267
#define PC2R_POSITION_PID_RESET 0x0268
#define PC2R_INDIVIDUAL_MOTOR_SET_GHOST_ERROR_MAX 0x0269
#define PC2R_INDIVIDUAL_MOTOR_GET_CURRENT_POSITION 0x026A


#define PC2R_TIR_ENABLE_DISABLE 0x0270
#define PC2R_TIR_COMMAND 0x0271
#define PC2R_TIR_MOVE_UP 0x0272
#define PC2R_TIR_MOVE_DOWN 0x0273
#define PC2R_TIR_MOVE_SERVO 0x0274

#define PC2R_UART1_FORWARD 0x0280
#define PC2R_UART1_SET_BAUDRATE 0x0281
#define PC2R_UART2_FORWARD 0x0290
#define PC2R_UART2_SET_BAUDRATE 0x0291
#define PC2R_UART3_FORWARD 0x02A0
#define PC2R_UART3_SET_BAUDRATE 0x02A1
#define PC2R_UART4_FORWARD 0x02B0
#define PC2R_UART4_SET_BAUDRATE 0x02B1

#define PC2R_IMU_SET_REFERENTIAL 0x02C0
#define PC2R_IMU_SET_OFFSET 0x02C1
#define PC2R_USE_C620_ODOMETRIE_INPUT 0x02C2


#define PC2R_POSITION_GNSS 0x02D0
#define PC2R_SET_FORCED_LOCATION 0x02D1
#define PC2R_SET_WAYPOINT 0x02D2    
#define PC2R_SET_TRAJECTORY_CONSTANTS 0x02D3                  
#define PC2R_SET_POSITION_ASSERVISSEMENT_MODE 0x02D4
#define PC2R_HOLONOMIC_POSITION_PID_SET_GAINS 0x02D5
#define PC2R_SET_KALMAN_ALPHAS 0x02D6

#define R2PC_ERROR_FRAME 0xEEEE
#define PC2R_CLEAR_ERROR 0xEEEF
#define PC2R_RESET_EMBEDDED 0xF000

#define PC2R_USB_TEST_FRAME 0x9999




typedef enum{
    RECEPTION_WAIT=0,
    RECEPTION_FUNCTION_MSB,
    RECEPTION_FUNCTION_LSB,
    RECEPTION_PACKET_NUMBER_MSB,
    RECEPTION_PACKET_NUMBER_LSB,
    RECEPTION_PAYLOAD_LENGTH_MSB,
    RECEPTION_PAYLOAD_LENGTH_LSB,
    RECEPTION_PAYLOAD, 
    RECEPTION_CHECKSUM
}RECEPTION_STATE;



/*******************************************************************************
 * PROTOTYPES
 ******************************************************************************/
unsigned char UartCalculateChecksum(unsigned int msgFunction,
        unsigned int msgPayloadLength, unsigned char * msgPayload);
unsigned char USBCalculateChecksum(unsigned int msgFunction,
        unsigned int messageNumber,unsigned int msgPayloadLength, unsigned char * msgPayload);
void Uart1DecodeMessage(void);
void SendMessageUARTMt(unsigned char* payload, unsigned char payloadLength);
void SendMessageUART(unsigned short int destAddress, unsigned short int cmd, unsigned short int payloadLength, unsigned char* payload);
void SendMessageUART2(unsigned short int destAddress, unsigned short int cmd, unsigned short int payloadLength, unsigned char* payload);
int SendMessageTxUART3(unsigned char* payload, unsigned char payloadLength);
int SendMessageTxUART(unsigned char* payload, unsigned char payloadLength);
void ProcessMessage( unsigned short int command, unsigned short int length, unsigned char payload[]);
void ProcessReadableMessage(unsigned char* payload, unsigned short payloadLength);
void ForwardUart4ReceiveData(void);
void ForwardUart3ReceiveData(void);
void ForwardUart2ReceiveData(void);
void ForwardUart1ReceiveData(void);

void SendMessageToRS232( unsigned short int payloadLength, unsigned char* payload);

void ProcessUartReceiveChar(void);
void ProcessUart2ReceiveChar(void);
void ProcessUSB(void);
void ProcessUart3ReceiveChar(void);
void SendMessageToUart1(  unsigned char* payload,unsigned short int payloadLength);
void SendMessageToUart2(  unsigned char* payload,unsigned short int payloadLength);
void SendMessageToUart3(  unsigned char* payload,unsigned short int payloadLength);
void SendMessageToUart4(  unsigned char* payload,unsigned short int payloadLength);
void Uart4DecodeMessage(void);

void USBDecodeMessage(unsigned char* buffer, unsigned char bufferLength);

unsigned char GetTxBufferRemainingSpace(void);
unsigned char GetTxBufferUsedSpace(void);
void AddToTxBuffer(unsigned char value);
BOOL Awaiting(unsigned char condition);
unsigned char AwaitingCondition(unsigned char condition);
void UTLN_WaitForSRSP(unsigned short command,unsigned int timeout);
void Uart3DecodeMessage(void);

void ProcessUSBSerialReceiveBuffer(unsigned char* buffer, unsigned int bufferLength);
void SendMessageToUart1(  unsigned char* payload, unsigned short int payloadLength);
void MakeAndSendMessageWithUTLNProtocol(unsigned short command, unsigned int payloadLength, unsigned char* payload);
void SendWelcomeMessage(void);
void SendErrorMessage(void);
void Send4WheelsPolarPidConsigneErrorCorrectionData(void);
void Send2WheelsPolarPidConsigneErrorCorrectionData(void);
void Send4WheelsIndependantPidConsigneErrorCorrectionData();
void Send2WheelsIndependantPidConsigneErrorCorrectionData();
void SendMotor1PidConsigneErrorCorrectionData();
void SendMotor2PidConsigneErrorCorrectionData();
void SendMotor3PidConsigneErrorCorrectionData();
void SendMotor4PidConsigneErrorCorrectionData();
void SendMotor5PidConsigneErrorCorrectionData();
void SendMotor6PidConsigneErrorCorrectionData();
void SendMotor7PidConsigneErrorCorrectionData();
void SendMotor8PidConsigneErrorCorrectionData();

void Send4WheelsPolarPidCorrectionData(void);
void Send4WheelsIndependantPidCorrectionData(void);
void SendMotor1PidCorrectionData(void);
void SendMotor2PidCorrectionData(void);
void SendMotor3PidCorrectionData(void);
void SendMotor4PidCorrectionData(void);
void SendMotor5PidCorrectionData(void);
void SendMotor6PidCorrectionData(void);
void SendMotor7PidCorrectionData(void);
void SendMotor8PidCorrectionData(void);

//void SendPolarOdometrySpeed(void);
//void SendIndependantOdometrySpeed(void);
void SendPolarAndIndependantOdometrySpeed(void);
void SendPolarAndIndependantOdometrySpeedAndIMU(void);
void SendAsservissementModeStatus(void);
void SendAuxiliaryOdometryData(void);
void SendEncoderRawData(void);
void SendAuxiliaryConsigneSpeedData(void);
void SendPositionAbsolue(void);
void SendPositionAbsolueAndGhostAndWayPoint(void);
void SendIMUData(void);
void SendPowerMonitoringValues(void);
void SendIOPollingValues(void);
void SendIOAnalogPollingValues(void);
void SendMotorCurrent(void);
void SendErrorText(const char* str);
void SetSpeedConsigneHolonome(float vx, float vy, float vtheta);
void SendUSBTestData(void);
void SendMotorPositionGhostOnError(void);
void SendIndividualMotorPosition(uint8_t numeroMoteur);
#endif	/* USTV_MAINFUNC_H */

