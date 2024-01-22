/*
 * File:   main.c
 * Author: Valentin
 *
 * Created on 21 juin 2016, 16:47
 */
#include "main.h"
#include "xc.h"
#include "Define.h"
#ifdef USE_OSAL
    #include "OSAL.h"
    #include "OSAL_Timers.h"
    #include "UTLN_OSAL.h"
#endif
#include "UTLN_Flag.h"
#include "UTLN_adc.h"
#include "UTLN_Timers.h"
#include "UTLN_uart.h"
#include "UTLN_Button.h"
#include "UTLN_Led.h"
//#include "UTLN_Message.h"       //Messages Uart
#include "UTLN_Communication.h"
#include "UTLN_PWM.h"
#include "ustv_i2c_interrupt.h"
#include "UTLN_SPI.h"
#include "RobotObj.h"
//#include "OLED_Display.h"
#include <libpic30.h>
#include "Utilities.h"
#include "Asservissement.h"
#include "UTLN_Odometrie.h"
#include "UTLN_ICM20948.h"
#include "UTLN_LS7366R.h"
#include "UTLN_QEI.h"
#include "UTLN_Odometrie.h"
#include "UTLN_POLOLU_Servo.h"
#include "UTLN_CoilGun.h"
#include "UTLN_OutputCompare.h"
#include "UTLN_KalmanFilter.h"
#include "UTLN_Imu_Processor.h"
#include "TrajectoryGeneratorSingleMotor.h"

#ifdef USE_USB
#include "system.h"
#include "app_device_vendor_basic.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_generic.h"
#endif

#include "UTLN_CAN.h"

#ifdef BUTTON_V2
void Sw1OnSimpleClickCallBack(void);
void Sw2OnSimpleClickCallBack(void);
void InitSW1(void);
void InitSW2(void);
#endif
#ifdef TIMER_V2
inline void Timer2CallBack(void);
inline void Timer3CallBack(void);
inline void Timer4CallBack(void);
void Timer5CallBack(void);
void Timer6CallBack(void);
void Timer7CallBack(void);
void Timer8CallBack(void);
#endif



/****************************************************************************************************/
// Declarations des variables
/****************************************************************************************************/
unsigned volatile long g_longTimeStamp;
unsigned volatile long g_longTimeStampTirUs;
int i,k;
unsigned char state=0;
unsigned long enteredTimeStamp;
unsigned char tmpVal;
volatile unsigned long autoStopMotorOnNoCommandTimeStamp=0;
unsigned char blockSW1=0;
volatile unsigned int usbTimeout=0;
extern volatile unsigned char noUSBOutput;
volatile unsigned char resetUSBNeeded=0;
ResetUSBState_t USBResetState=USB_IDLE;
unsigned long USBWaitingTimestamp=0;



extern uint16_t can1MotBuff0Watchdog;
extern uint16_t can1MotBuff1Watchdog;
extern uint16_t can1MotBuff2Watchdog;
extern uint16_t can1MotBuff3Watchdog;

int main(void) {
    InitOscillator() ;
    /****************************************************************************************************/
    // Initialisation des Entrï¿½es/Sorties
    /****************************************************************************************************/
    InitIO();
    
    InitCN();
    /****************************************************************************************************/
    // Enregistrement des Leds
    /****************************************************************************************************/
    #ifdef USE_LED_BLINKING
        LEDRegister(LED_BLANCHE, &LED_BlanchePort, LED_BlanchePin);
        LEDRegister(LED_ORANGE, &LED_Oport, LED_Opin);
        LEDRegister(LED_VERTE, &LED_BleuPort, LED_BleuPin);
    #endif

    
    _LED_V1=1;
    #ifndef USE_CRYSTAL_OSCILLATOR
        _LED_R1=1;
    #endif
    _LED_O1=1;
    _LED_BLE1=1;
    _LED_B1=1;
    
    _LED_V2=1;
    _LED_R2=1;
    _LED_O2=1;
    _LED_BLE2=1;
    _LED_B2=1;
    
    
    /****************************************************************************************************/
    // Initialisation du system OSAL
    /****************************************************************************************************/
    #ifdef USE_OSAL
        OSAL_init_system();
    #endif

    InitADC1();                                   //Initialisation de l'ADC1 (10bit) pour la mesure des courants moteurs
    InitADC2();                                   //Initialisation de l'ADC2 pour la mesure des IOs    
    /****************************************************************************************************/
    // Initialisation des flags
    /****************************************************************************************************/
    
    //RegisterTimerWithCallBack(TIMER1_ID, 1000.0, ProcessOSALMessageEvent, true, 4, OFF);
    RegisterTimerWithCallBack(TIMER2_ID, 1000.0, Timer2CallBack, true, 4, ON);          //Interface + sensors
    RegisterTimerWithCallBack(TIMER3_ID, 1000.0, Timer3CallBack, true, 6, ON);          //TimeStamp priorite 3
    RegisterTimerWithCallBack(TIMER4_ID, FREQ_ECH_QEI, Timer4CallBack, true, 5, ON);    //QEI + Asservissement
    RegisterTimerWithCallBack(TIMER5_ID, 1000.0, Timer5CallBack, true, 4, ON);          //Systeme event + gestion du tir priorité 4
    RegisterTimerWithCallBack(TIMER6_ID, 100.0, Timer6CallBack, true, 4, ON);           //Timer PWMUpdate
    //RegisterTimerWithCallBack(TIMER7_ID, 50.0, Timer7CallBack, true, 4, ON);           //Timer PWMUpdate
    RegisterTimerWithCallBack(TIMER7_ID, 50.0, Timer7CallBack, true, 4, ON);           //Timer Envoi des infos IMU, odo et position
    RegisterTimerWithCallBack(TIMER8_ID, 10000.0, Timer8CallBack, true, 6, ON);           //Timer Tir

    
    #ifdef USE_UART1
        initUART1();       //Initialisation de l'uart1
    #endif
    #ifdef USE_UART2
       initUART2();        //Initialisation de l'uart2 (Non fonctionnelle sur CM V7))
    #endif
    #ifdef USE_UART3
        initUART3();        //Initialisation de l'uart3 (RS232)
    #endif
    #ifdef USE_UART4
        initUART4();        //Initialisation de l'uart3 ()
    #endif
    #ifdef USE_I2C1
        InitI2C1();
    #else
        I2C2CONbits.I2CEN=0;
    #endif

    #ifdef USE_SPI1
        SPI1Init();
    #endif
    #ifdef USE_SPI2        
        SPI2RegisterChipSelect( &SPI2_SS_IMU_PORT, SPI2_SS_IMU_PIN, ICM20948_SPICS_ID);
        SPI2ChipSelectSetValue( 1, ICM20948_SPICS_ID);
        SPI2Init();
    #endif

    #ifdef USE_EXTERNAL_QEI
        #ifdef USE_LS7366R
            SPI1RegisterChipSelect(&SPI1_SS_ENC1port, SPI1_SS_ENC1pin, 1);
            SPI1ChipSelectSetValue( 1, 1);
            SPI1RegisterChipSelect(&SPI1_SS_ENC2port, SPI1_SS_ENC2pin, 2);
            SPI1ChipSelectSetValue( 1, 2);
            SPI1RegisterChipSelect(&SPI1_SS_ENC3port, SPI1_SS_ENC3pin, 3);
            SPI1ChipSelectSetValue( 1, 3);
            SPI1RegisterChipSelect(&SPI1_SS_ENC4port, SPI1_SS_ENC4pin, 4);
            SPI1ChipSelectSetValue( 1, 4);
            SPI1RegisterChipSelect(&SPI1_SS_ENC5port, SPI1_SS_ENC5pin, 5);
            SPI1ChipSelectSetValue( 1, 5);
            SPI1RegisterChipSelect(&SPI1_SS_ENC6port, SPI1_SS_ENC6pin, 6);
            SPI1ChipSelectSetValue( 1, 6);
            SPI1RegisterChipSelect(&SPI1_SS_ENC7port, SPI1_SS_ENC7pin, 7);
            SPI1ChipSelectSetValue( 1, 7);
            SPI1RegisterChipSelect(&SPI1_SS_ENC8port, SPI1_SS_ENC8pin, 8);
            SPI1ChipSelectSetValue( 1, 8);
            robotState.pointToDistance=POINT_TO_DIST;
            //Initialisation des codeurs externes
            bool initLS7366OK=false;
            initLS7366OK=LS7366Init(1);                      //Init du compteur QEI 1
            if(!initLS7366OK)
            {
                SendErrorText("Error Init Encoder 1");
                flagError.LS7366_1InitError=1;
            }
            initLS7366OK=LS7366Init(2);                      //Init du compteur QEI 2
            if(!initLS7366OK)
            {
                SendErrorText("Error Init Encoder 2");
                flagError.LS7366_2InitError=1;
            }
            initLS7366OK=LS7366Init(3);                      //Init du compteur QEI 3
            if(!initLS7366OK)            
            {
                SendErrorText("Error Init Encoder 3");
                flagError.LS7366_3InitError=1;
            }
            initLS7366OK=LS7366Init(4);                      //Init du compteur QEI 4
            if(!initLS7366OK)            
            {
                SendErrorText("Error Init Encoder 4");
                flagError.LS7366_4InitError=1;
            }
            initLS7366OK=LS7366Init(5);                      //Init du compteur QEI 5
            if(!initLS7366OK)            
            {
                SendErrorText("Error Init Encoder 5");
                flagError.LS7366_5InitError=1;
            }
            initLS7366OK=LS7366Init(6);                      //Init du compteur QEI 6
            if(!initLS7366OK)            
            {
                SendErrorText("Error Init Encoder 6");
                flagError.LS7366_6InitError=1;
            }
            initLS7366OK=LS7366Init(7);                      //Init du compteur QEI 7
            if(!initLS7366OK)            
            {
                SendErrorText("Error Init Encoder 7");
                flagError.LS7366_7InitError=1;
            }
            initLS7366OK=LS7366Init(8);                      //Init du compteur QEI 8
            if(!initLS7366OK)            
            {
                SendErrorText("Error Init Encoder 8");
                flagError.LS7366_8InitError=1;
            }
            
            LS7366ClearCounter(1);
            LS7366ClearCounter(2);
            LS7366ClearCounter(3);
            LS7366ClearCounter(4);
            LS7366ClearCounter(5);
            LS7366ClearCounter(6);
            LS7366ClearCounter(7);
            LS7366ClearCounter(8);
        #endif
    #endif
    #ifdef USE_INTERNAL_QEI
        #ifdef USE_QEI1
            InitQEI1();
        #endif
        #ifdef USE_QEI2
            InitQEI2();
        #endif
    #endif

    #ifdef USE_ICM20948
        volatile unsigned char initMPU;
        ICM20948RegisterUserSPIFunc(SPI2WriteMultipleCommandMultipleRead);        //On associe la fonction de lecture/ecriture SPI a la lib ICM20948
        ICM20948RegisterUserSystemClock(FCY);
        initMPU=ICM20948Init_default(ICM20948_SPICS_ID);                        //Initialisation de la centrale inertielle
        if(initMPU)
        {
            _LED_B3=1;
            flagSystem.IsSensorOperationEnabled=TRUE;
            flagError.IMUInitError=0;
        }
        else
        {
            flagError.IMUInitError=1;
        }
    #endif
    #ifdef USE_POLOLU_SERVO
        PololuRegisterUserUartFunc((POLOLUUSER_UART_Func)&SendMessageToUart1);
        PololuSetTargetUs(5,SERVO_DOWN_US);
    #else
        InitOutputCompare34();
        SetOutputCompare34PWMPeriodUs(20000);       //20ms (50Hz update for servo)
        robotState.servo6Us=1500;
        SetOutputCompare34PWMDutyCycleUs(robotState.servo6Us);     //Set 0 Value
    #endif
  
    //Init par défaut aux coeff Eurobot
//    Init4WheelsToPolarMatrix(-3.967532e-01, -2.720655e-01, +2.720655e-01, 3.967532e-01,
//                             +3.776278e-01, -3.776278e-01, -3.776278e-01, 3.776278e-01,
//                             +2.106947e+00, +1.341329e+00, +1.341329e+00, +2.106947e+00);
            
    InitPWM();                                     //Initialisation des PWM pour la gestion des moteurs
    InitOuputComparePWM();                         //Initialisation de l'OC pour la gestion du moteur 8
    robotState.pointToAngleAuxiliaryMotors=2.0*M_PI/(19.2*8192);           //32768 point par tour codeur (2rad)
    
    #ifdef USE_LED_BLINKING                        
        LEDInit();                                      //Initialisation de la machine a etat de gestion des leds 
        LEDPowerOff();                                  //Extinction des LEds
        //LEDSetMode(LED_VERTE,LED_BLINK_CONT_1000_1000); //Reglage de la led1 en clignotement continu (1sec/sec)
    #endif
    
    #ifdef USE_OSAL
        //OSAL_start_reload_timer( USTV_SYSTEM_PROCESSING_TASK, USTV_TIMER_1SEC_TIMEOUT, 2000 );
        OSAL_start_reload_timer( USTV_SYSTEM_PROCESSING_TASK, USTV_TIMER_10HZ_TIMOUT, 100 );                //LCD_REFRESH_RATE
        OSAL_start_reload_timer( USTV_SYSTEM_PROCESSING_TASK, USTV_VIRTUAL_TIMER_1_TIMOUT, 100 );                //IO Refresh rate
        OSAL_start_reload_timer( USTV_SYSTEM_PROCESSING_TASK, USTV_TIMER_1HZ_TIMOUT, 1000 );                //IO Refresh rate
    #endif
    
    #ifdef USE_LCD_SSD1306
        /* Set up the OLED Display */
         OLEDSetUp();
         OLEDFill(CLEAR);
         OLEDText( 0, 17, " BOOTING ", SIZE_TWO, WHITE );
         OLEDUpdateDisplay ( DDGRAM_CLEAR );
         __delay_ms(500);
         OLEDText( 0, 8, "    IUT ", SIZE_TWO, WHITE );
         OLEDText( 0, 24, "   GEII ", SIZE_TWO, WHITE );
         OLEDText( 0, 42, "  TOULON ", SIZE_TWO, WHITE );
         OLEDUpdateDisplay ( DDGRAM_CLEAR );
         __delay_ms(500);
    #endif

    _LED_V1=0;
    #ifndef USE_CRYSTAL_OSCILLATOR
        _LED_R1=0;
    #endif
    _LED_O1=0;
    _LED_BLE1=0;
    _LED_B1=0;
    
    _LED_V2=0;
    _LED_R2=0;
    _LED_O2=0;
    _LED_BLE2=0;
    _LED_B2=0;
    
    //On active les ponts en H
    ENABLE_PWM=1;
    
    //Activation des PWM OC. Besoin de 2 pour piloter le hacheur 8
    EnableOutputCompare1PWM();
    EnableOutputCompare2PWM();
    
    //On active la generation des PWMs
    ActivatePWM(true);
  
    
    //Notification de connection au PC
    SendWelcomeMessage();
    
    //Default Configuration
    robotState.enableIOPolling=true;
    robotState.enablePowerMonitoring=true;
    robotState.enablePIDDebugErrorCorrectionConsigne=true;
    robotState.enablePIDDebugInternal=true;
    robotState.asservissementMoteur5State= ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
    robotState.asservissementMoteur6State= ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
    robotState.asservissementMoteur7State= ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
    robotState.asservissementMoteur8State= ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
    #ifdef USE_USB
        USBDeviceInit();
        USBDeviceAttach();
    #endif

#ifdef USE_M3508
        //Initialisation du bus CAN (1Mbps)
        CAN1_Init();
        //On filtre (et donc on accept) les messages en provenance du SID 0x201 (moteur1)
        CAN_SetAcceptanceFilter0(0x201); 
        //On filtre (et donc on accept) les messages en provenance du SID 0x202 (moteur2)
        CAN_SetAcceptanceFilter1(0x202); 
        //On filtre (et donc on accept) les messages en provenance du SID 0x203 (moteur3)
        CAN_SetAcceptanceFilter2(0x203);
        //On filtre (et donc on accept) les messages en provenance du SID 0x204 (moteur4)
        CAN_SetAcceptanceFilter3(0x204);         
        CAN1_StartTransmissions();        
#endif


#ifdef TEST_MOTOR
        TurnOnOffTimer(TIMER2_ID, OFF);
        TurnOnOffTimer(TIMER3_ID, OFF);
        TurnOnOffTimer(TIMER4_ID, OFF);
        TurnOnOffTimer(TIMER5_ID, OFF);
        TurnOnOffTimer(TIMER6_ID, OFF);
    while(1)
    {
        float speed =70.0;
        if(_SW2==0)
            speed=-70.0;
        EnablePWM(1-1,true);
        EnablePWM(2-1,true);
        EnablePWM(3-1,true);
        EnablePWM(4-1,true);
        EnablePWM(5-1,true);
        EnablePWM(6-1,true);
        EnablePWM(7-1,true);
        EnablePWM(8-1,true);
                
        PWMSetSpeedCommandPercentMotorN(1,speed);
        PWMSetSpeedCommandPercentMotorN(2,speed);
        PWMSetSpeedCommandPercentMotorN(3,speed);
        PWMSetSpeedCommandPercentMotorN(4,speed);
        PWMSetSpeedCommandPercentMotorN(5,speed);
        PWMSetSpeedCommandPercentMotorN(6,speed);
        PWMSetSpeedCommandPercentMotorN(7,speed);
        PWMSetSpeedCommandPercentMotorN(8,speed);

        PWMUpdateSpeed();

    }
#endif
        
    while(1)
    {
        //_LED_B1=!_LED_B1;
        #ifdef USE_LCD_DEMO
            OLED_DemoStateMachine();
        #else
            if(flagSystem.virtualTimer10HzTimeout)      //10Hz virtual timer
            {
                flagSystem.virtualTimer10HzTimeout=0;

                #ifdef USE_POLOLU_SERVO
                    PololuSetTargetUs(5,robotState.servo6Us);
                #else
                    #ifdef USE_OUTPUTCOMPARE34
                        SetOutputCompare34PWMDutyCycleUs(robotState.servo6Us);     //Set 0 Value
                    #endif
                #endif
                ADC1StartConversion();
                ADC2StartConversion();
            }
            
            //10Hz
            if(flagSystem.virtualTimer1Timeout)
            {

                _LED_BLE1=!_LED_BLE1;
                flagSystem.virtualTimer1Timeout=0;
                if(robotState.enableMotorCurrentData)
                    SendMotorCurrent();
                if(robotState.enableIOPolling)
                    SendIOPollingValues();
                if(robotState.enableIOAnalogPolling)
                    SendIOAnalogPollingValues();
                if(robotState.enablePowerMonitoring)
                    SendPowerMonitoringValues();
                if(robotState.enableEncoderRawData)
                    SendEncoderRawData();
                
                SendAsservissementModeStatus();
                if(robotState.enablePIDDebugErrorCorrectionConsigne)
                {
                    switch(robotState.robotSpeedAsservissementState)
                    {
                        case ASSERVISSEMENT_DISABLED_2_WHEELS:
                            break;
                        case ASSERVISSEMENT_POLAIRE_2_WHEELS :
                            Send2WheelsPolarPidConsigneErrorCorrectionData();
                            break;
                        case ASSERVISSEMENT_INDEPENDANT_2_WHEELS:
                            Send2WheelsIndependantPidConsigneErrorCorrectionData();
                            break;
                        case ASSERVISSEMENT_DISABLED_4_WHEELS: 
                            if(robotState.asservissementMoteur1State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor1PidConsigneErrorCorrectionData();
                            if(robotState.asservissementMoteur2State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor2PidConsigneErrorCorrectionData();
                            if(robotState.asservissementMoteur3State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor3PidConsigneErrorCorrectionData();
                            if(robotState.asservissementMoteur4State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor4PidConsigneErrorCorrectionData();
                            break;
                        case ASSERVISSEMENT_POLAIRE_4_WHEELS:                            
                            Send4WheelsPolarPidConsigneErrorCorrectionData();
                            break;
                        case ASSERVISSEMENT_INDEPENDANT_4_WHEELS:
                            Send4WheelsIndependantPidConsigneErrorCorrectionData();
                            break;
                        default:break;
                    }
                    
                    if(robotState.asservissementMoteur5State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor5PidConsigneErrorCorrectionData();
                    if(robotState.asservissementMoteur6State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor6PidConsigneErrorCorrectionData();
                    if(robotState.asservissementMoteur7State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor7PidConsigneErrorCorrectionData();
                    if(robotState.asservissementMoteur8State==ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor8PidConsigneErrorCorrectionData();
                    
                }
                if(robotState.enablePIDDebugInternal)
                {
                    switch(robotState.robotSpeedAsservissementState)
                    {
                        case ASSERVISSEMENT_DISABLED_2_WHEELS:
                            break;
                        case ASSERVISSEMENT_POLAIRE_2_WHEELS :
                            
                            break;
                        case ASSERVISSEMENT_INDEPENDANT_2_WHEELS:
                            
                            break;
                        case ASSERVISSEMENT_DISABLED_4_WHEELS: 
                            if(robotState.asservissementMoteur1State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor1PidCorrectionData();
                            if(robotState.asservissementMoteur2State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor2PidCorrectionData();
                            if(robotState.asservissementMoteur3State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor3PidCorrectionData();
                            if(robotState.asservissementMoteur4State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                                SendMotor4PidCorrectionData();
                            break;
                        case ASSERVISSEMENT_POLAIRE_4_WHEELS:                            
                            Send4WheelsPolarPidCorrectionData();
                            break;
                        case ASSERVISSEMENT_INDEPENDANT_4_WHEELS:
                            Send4WheelsIndependantPidCorrectionData();
                            break;
                        default:break;
                    }            
                    
                    if(robotState.asservissementMoteur5State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor5PidCorrectionData();
                    if(robotState.asservissementMoteur6State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor6PidCorrectionData();
                    if(robotState.asservissementMoteur7State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor7PidCorrectionData();
                    if(robotState.asservissementMoteur8State== ASSERVISSEMENT_MOTOR_INDIVIDUAL_SPEED_ENABLED)
                        SendMotor8PidCorrectionData();
                    
                    //if(robotState.enableMotorVitesseData)
                    SendAuxiliaryOdometryData();
                }
                
//                //TEST TIR
//                if(_SW1 && !blockSW1)
//                {
//                    blockSW1=1;
//                    GenerateTir4Coil(100,150 , 50, 0, 100, 200, 0);
//                    OSAL_start_timerEx( USTV_SYSTEM_PROCESSING_TASK, USTV_VIRTUAL_TIMER_2_TIMOUT, 1000 );  
//                }
//                
//                if(flagSystem.virtualTimer2Timeout)
//                {
//                    blockSW1=0;
//                    flagSystem.virtualTimer2Timeout=0;
//                }
            }
        #endif
            //Executé a 50Hz
            if(flagSystem.flag50Hz)
            {        
                _SPI2_SS1=1;
                SendPolarAndIndependantOdometrySpeedAndIMU();
                //SendPositionAbsolue();
                SendPositionAbsolueAndGhostAndWayPoint();
//                SendPolarAndIndependantOdometrySpeed();                
//                SendIMUData();
                
                if(IsIndividualMotorPositionGhostError())
                {
                    ClearIndividualMotorPositionGhostError();
                    SendMotorPositionGhostOnError();
                }
                if(robotState.enableUartRAWForward)
                {
                    #ifdef USE_UART1
                        if(UART1IsDataReadyInRxBuffer())
                        {
                            ForwardUart1ReceiveData();
                            //SendUSBTestData();
                        }
                    #endif
                    
                    if(UART2IsDataReadyInRxBuffer())
                    {
                        ForwardUart2ReceiveData();
                    }
                    
                    if(UART3IsDataReadyInRxBuffer())
                    {
                        ForwardUart3ReceiveData();
                    }
                    if(UART4IsDataReadyInRxBuffer())
                    {
                        ForwardUart4ReceiveData();
                    }
                }
                flagSystem.flag50Hz=0;
            }
            
            //Executé a 1Hz
            if(flagSystem.flag1Hz)
            {
                if(flagError.ALL!=0)
                {
                    SendErrorMessage();
                }
                flagSystem.flag1Hz=0;
            }
#ifdef USE_USB_DETACH_ATTACH
            //Routine de deplantage de l'usb
            switch(USBResetState)
            {
                case USB_IDLE:
                    if(resetUSBNeeded)
                    {
                        resetUSBNeeded=0;
                        _LED_B2=!_LED_B2;
                        USBResetState=USB_DETACH;
                    }
                    break;
                case USB_DETACH:
                    USBDeviceDetach();
                    USBWaitingTimestamp=g_longTimeStamp;
                    USBResetState=USB_WAITING;
                    break;
                case USB_WAITING:
                    if(USBWaitingTimestamp+3000<g_longTimeStamp)
                    {
                        USBResetState=USB_ATTACH;
                    }
                    break;
                case USB_ATTACH:
                    USBDeviceAttach();
                    USBResetState=USB_IDLE;
                    resetUSBNeeded=0;
                    noUSBOutput=0;
                    usbTimeout=0;
                    USBResetTxBuffer();
                    break;
                default:break;
            }
#endif
        #ifdef USE_USB    
            //Application specific tasks
            APP_DeviceVendorTasks();
        #endif
    }
    return 0;
}



#ifdef BUTTON_V2
void Sw1OnSimpleClickCallBack(void)
{
    flagInterface.Sw1ClickEvent = TRUE;
}

void InitSW1(void)
{
    //SW1
//    PADCONbits.IOCON=1;         //Interrupt on Change enable bit
//    IOCPAbits.IOCPA15=1;        //Enable Low to High transition on RA15
    IEC1bits.CNIE=1;
    INTCON2bits.GIE=1;          //Enable Global interrupt
}
#endif

//Timer appelé a 1000.0 Hz (Priorité 4)
inline void Timer2CallBack(void)
{
    ProcessOSALUpdateInterfaceState(); //On observe les changements dans l'interface utilisateur
    ProcessOSALSensorEvent();
       
    unsigned long time=g_longTimeStamp-OSAL_GetSystemClock();
    OSALTimerUpdate((unsigned short)time);
    
}


//Timer appelé a 1000.0 Hz (Priorité 6)
inline void Timer3CallBack(void)
{
    g_longTimeStamp += 1;
    autoStopMotorOnNoCommandTimeStamp++;
    if(g_longTimeStamp%50==0)
    {
        if(can1MotBuff0Watchdog<10 || can1MotBuff1Watchdog<10 || can1MotBuff2Watchdog<10 || can1MotBuff3Watchdog<10)
        {
            //On a un figeage du bus can en reception
            //On reinit le can
            CAN1_Stop();
            //On reset les infos des vitesses des encodeurs
            robotState.brushlessStatus[0].rotorSpeed=0;
            robotState.brushlessStatus[1].rotorSpeed=0;
            robotState.brushlessStatus[2].rotorSpeed=0;
            robotState.brushlessStatus[3].rotorSpeed=0;
            
            //Initialisation du bus CAN (1Mbps)
            CAN1_Init();
            //On filtre (et donc on accept) les messages en provenance du SID 0x201 (moteur1)
            CAN_SetAcceptanceFilter0(0x201); 
            //On filtre (et donc on accept) les messages en provenance du SID 0x202 (moteur2)
            CAN_SetAcceptanceFilter1(0x202); 
            //On filtre (et donc on accept) les messages en provenance du SID 0x203 (moteur3)
            CAN_SetAcceptanceFilter2(0x203);
            //On filtre (et donc on accept) les messages en provenance du SID 0x204 (moteur4)
            CAN_SetAcceptanceFilter3(0x204);         
            CAN1_StartTransmissions();      
            _LED_R3=!_LED_R3;
        }
        else
        {
            can1MotBuff0Watchdog=0;
            can1MotBuff1Watchdog=0;
            can1MotBuff2Watchdog=0;
            can1MotBuff3Watchdog=0;
        }
    }
}

//Timer 4 appelé a 250.0 Hz (Priorité 5)
inline void Timer4CallBack(void)
{
    _LED_O2=1;
    QEIUpdateRawData();
#ifndef TEST_MOTOR
    if(robotState.robotSpeedAsservissementState== ASSERVISSEMENT_INDEPENDANT_4_WHEELS || robotState.robotSpeedAsservissementState== ASSERVISSEMENT_POLAIRE_4_WHEELS || robotState.robotSpeedAsservissementState== ASSERVISSEMENT_DISABLED_4_WHEELS)
    {
        if(!robotState.useC620ForOdometrie)
            ComputePolarSpeed4WheelsFromQEI();
        else
            ComputePolarSpeed4WheelsFromC620();
    }
    else if(robotState.robotSpeedAsservissementState== ASSERVISSEMENT_INDEPENDANT_2_WHEELS || robotState.robotSpeedAsservissementState== ASSERVISSEMENT_POLAIRE_2_WHEELS || robotState.robotSpeedAsservissementState== ASSERVISSEMENT_DISABLED_2_WHEELS)
    {
        ComputePolarSpeed2WheelsFromQEI();
    }
    
    ComputeSpeedAuxiliaryMotorsFromQEI();    
    
    ///Attention, l'ordre compte !!!!
    UpdateVxVyUsingAccelAndOdometry();
    UpdateThetaUsingGyro();
    UpdateXYUsingKalmanSpeed();
    
    CalculateGhostPosition();
    if(robotState.robotPositionAsservissementState == ASSERVISSEMENT_POSITION_ON)
    {
        UpdateAsservissementPosition();
    }
    UpdateAsservissementVitesse();
    
    

    if(autoStopMotorOnNoCommandTimeStamp>200)
    {
        robotState.Motor1SpeedConsigne=0;
        robotState.Motor2SpeedConsigne=0;
        robotState.Motor3SpeedConsigne=0;
        robotState.Motor4SpeedConsigne=0;
        robotState.Motor5SpeedConsigne=0;
        robotState.Motor6SpeedConsigne=0;
        robotState.Motor7SpeedConsigne=0;
        robotState.Motor8SpeedConsigne=0;
        PWMSetSpeedCommandMotor1(0.0);
        PWMSetSpeedCommandMotor2(0.0);
        PWMSetSpeedCommandMotor3(0.0);
        PWMSetSpeedCommandMotor4(0.0);
        PWMSetSpeedCommandMotor5(0.0);
        PWMSetSpeedCommandMotor6(0.0);
        PWMSetSpeedCommandMotor7(0.0);
        PWMSetSpeedCommandMotor8(0.0);
        ResetAllSpeedPid();
        robotState.asservissementMoteur1State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.asservissementMoteur2State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.asservissementMoteur3State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.asservissementMoteur4State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.asservissementMoteur5State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.asservissementMoteur6State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.asservissementMoteur7State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.asservissementMoteur8State=ASSERVISSEMENT_MOTOR_INDIVIDUAL_DISABLED_DIRECT_COMMAND;
        robotState.robotSpeedAsservissementState=ASSERVISSEMENT_DISABLED_4_WHEELS;  //TODO à définir 2 ou 4 roues en fonction du contexte
       // _LED_O2=!_LED_O2;
    }
    _LED_O2=0;
#endif   
}


//Freq 1000 Hz Priorite 5 (gestion tir)
void Timer5CallBack(void)
{
    ProcessOSALSystemEvent();
}

//100Hz
void Timer6CallBack(void)
{
//                PWMSetSpeedConsignePercent(100.0, MOTOR_5);  
//        PWMSetSpeedConsignePercent(100.0, MOTOR_6); 
//            PWMSetSpeedConsignePercent(100.0, MOTOR_7);  
//        PWMSetSpeedConsignePercent(100.0, MOTOR_8); 
    #ifndef MOTOR_OFF
        #ifndef USE_CANBUS
            PWMUpdateSpeed();
        #else
            PWMUpdateSpeed();
            if(robotState.motorEnable)
                CAN_UpdateSpeed();
            else
            {
                robotState.vitesseMoteur1CommandePercent=0.0;
                robotState.vitesseMoteur2CommandePercent=0.0;
                robotState.vitesseMoteur3CommandePercent=0.0;
                robotState.vitesseMoteur4CommandePercent=0.0;
                CAN_UpdateSpeed();
            }
        #endif
    #endif

    if( ADC1IsConversionFinished() == 1)
    {
        float multiplicationFactor=3.3 / 1024.0;
        ADC1ClearConversionFinishedFlag() ;
        unsigned int * result = ADC1GetResult() ;
        float volts = ((float)result[8] )* multiplicationFactor;//*3.2 ;
        robotState.motor2Current = (volts*2-2.5)/ACS723_VOLTAGE_PER_A -ACS723_CURRENT_OFFSET;
        volts =((float)result[ 7 ] )* multiplicationFactor;//* 3.2 ;
        robotState.motor1Current = (volts*2-2.5)/ACS723_VOLTAGE_PER_A-ACS723_CURRENT_OFFSET;
        volts=((float)result[4])* multiplicationFactor ;//* 3.2 ;
        robotState.motor3Current = (volts*2-2.5)/ACS723_VOLTAGE_PER_A-ACS723_CURRENT_OFFSET;
        volts=((float)result[3])* multiplicationFactor ;//* 3.2 ;
        robotState.motor4Current = (volts*2-2.5)/ACS723_VOLTAGE_PER_A-ACS723_CURRENT_OFFSET;
        volts=((float)result[2])* multiplicationFactor ;//* 3.2 ;
        robotState.motor5Current = (volts*2-2.5)/ACS723_VOLTAGE_PER_A-ACS723_CURRENT_OFFSET;
        volts=((float)result[1])* multiplicationFactor ;//* 3.2 ;
        robotState.motor6Current = (volts*2-2.5)/ACS723_VOLTAGE_PER_A -ACS723_CURRENT_OFFSET;         
 
        #ifdef USE_CM_V10
            volts=((float)result[9])* multiplicationFactor ;//* 3.2 ;
            robotState.motor8Current=(volts*3-2.5)/ACS723_VOLTAGE_PER_A -ACS723_CURRENT_OFFSET;
            volts=((float)result[10])* multiplicationFactor ;//* 3.2 ;
        #else
            volts=((float)result[9])* multiplicationFactor ;//* 3.2 ;
        #endif
        robotState.battCMDVoltage=volts*7.5 +CMD_DIODE_FORWARD_VOLTAGE;;
        volts=((float)result[5])* multiplicationFactor ;//* 3.2 ;
        robotState.battPWRVoltage=volts*15;
        volts=((float)result[0])* multiplicationFactor ;//* 3.2 ;
        #ifdef USE_CM_V10
            robotState.battCMDCurrent=(volts*2-2.5)/ACS723_VOLTAGE_PER_A-0.2;
        #else
            robotState.battCMDCurrent=(volts*2-2.5)/ACS723_VOLTAGE_PER_A -1.2;
        #endif
        
        if(robotState.battCMDCurrent<0)
            robotState.battCMDCurrent=0;
        volts=((float)result[6])* multiplicationFactor ;//* 3.2 ;
        #ifdef USE_CM_V10
            robotState.battPWRCurrent=(volts*2-2.5)/ACS723_VOLTAGE_PER_A-0.1;
        #else
            robotState.battPWRCurrent=(volts*2-2.5)/ACS723_VOLTAGE_PER_A -1.4;
        #endif
        if(robotState.battPWRCurrent<0)
            robotState.battPWRCurrent=0;
    }    
        
    if( ADC2IsConversionFinished() == 1)
    {
        float multiplicationFactor=3.3 / 1024.0;
        ADC2ClearConversionFinishedFlag() ;
        unsigned int * result = ADC2GetResult() ;
        float volts = ((float)result[0] )* multiplicationFactor;//*3.2 ;
        robotState.ana0_B0Value = volts*5.0 ;
        volts=((float)result[1])* multiplicationFactor ;//* 3.2 ;
        robotState.ana8_B8Value=volts*5.0;
        volts=((float)result[2])* multiplicationFactor ;//* 3.2 ;
        robotState.ana9_B9Value=volts*5.0;
        volts=((float)result[3])* multiplicationFactor ;//* 3.2 ;
        robotState.ana10_B10Value=volts*5.0;
        volts=((float)result[4])* multiplicationFactor ;//* 3.2 ;
        #ifdef USE_CM_V10
            robotState.ana11_B11Value=(volts*3-2.5)/ACS723_VOLTAGE_PER_A-ACS723_CURRENT_OFFSET;;
            robotState.motor7Current=robotState.ana11_B11Value;
        #else
            robotState.ana11_B11Value=volts*5.0;
        #endif  
    }
}

//50Hz
void Timer7CallBack(void)
{
    flagSystem.flag50Hz=1;
}

//Freq 10000 Hz Priorite 5 (gestion tir)
void Timer8CallBack(void)
{
    g_longTimeStampTirUs+=100;
    TirStateMachine();
}