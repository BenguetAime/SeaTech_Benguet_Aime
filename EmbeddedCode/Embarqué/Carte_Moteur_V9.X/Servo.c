#include "Servo.h"
#include "IO.h"
#include "Robot.h"
#include "Toolbox.h"

void Init_OC1(void)
{
    OC1CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC1CON2 = 0;
    OC1CON1bits.OCTSEL = 0x00; /* This selects the peripheral clock as the clock input to the OC module */
    OC1R = 1; /* This is just a typical number, user must calculate based on the waveform requirements and the system clock */
    OC1RS = 1000; 
    OC1CON1bits.OCM = 0b111; /* This selects and starts the toggle mode */
    OC1CON2bits.SYNCSEL = 0b00000;
    OC1CON2bits.OCTRIG = 1;
    OC1CON1bits.TRIGMODE = 0;
    _OC1IF = 0;
    _OC1IE = 1;
    _OC1IP = 7;
} 

void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void)
{
    _OC1IF = 0;
    OC1CON2bits.TRIGSTAT = 0;
    SERVO_1_PIN = 0;
}

void SetPosServo(unsigned char ServoNum ,double Angle)
{
    Angle=Min(Angle,180);
    
    double K=CoeffAngle*Angle+AngleMin;
    
    switch (ServoNum)
    {
        case SERVO1:
        {
            OC1RS=PERIOD_1_MS*K;
            robotState.angleServo1=Angle;
        }
        break;
        
        case SERVO2:
        {
            OC2RS=PERIOD_1_MS*K;
            robotState.angleServo2=Angle;
        }
        break;
        
        case SERVO3:
        {
            OC3RS=PERIOD_1_MS*K;
            robotState.angleServo3=Angle;
        }
        break;
        
        default:break;
    }
}


void Init_OC2(void)
{
    OC2CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC2CON2 = 0;
    OC2CON1bits.OCTSEL = 0x00; /* This selects the peripheral clock as the clock input to the OC module */
    OC2R = 1; /* This is just a typical number, user must calculate based on the waveform requirements and the system clock */
    OC2RS = 1000; 
    OC2CON1bits.OCM = 0b111; /* This selects and starts the toggle mode */
    OC2CON2bits.SYNCSEL = 0b00000;
    OC2CON2bits.OCTRIG = 1;
    OC2CON1bits.TRIGMODE = 0;
    _OC2IF = 0;
    _OC2IE = 1;
    _OC2IP = 7;
}

void __attribute__((interrupt, no_auto_psv)) _OC2Interrupt(void)
{
    _OC2IF = 0;
    OC2CON2bits.TRIGSTAT = 0;
    //SERVO_2_PIN = 0;
}
void Init_OC3(void)
{
    OC3CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC3CON2 = 0;
    OC3CON1bits.OCTSEL = 0x00; /* This selects the peripheral clock as the clock input to the OC module */
    OC3R = 1; /* This is just a typical number, user must calculate based on the waveform requirements and the system clock */
    OC3RS = 1000; 
    OC3CON1bits.OCM = 0b111; /* This selects and starts the toggle mode */
    OC3CON2bits.SYNCSEL = 0b00000;
    OC3CON2bits.OCTRIG = 1;
    OC3CON1bits.TRIGMODE = 0;
    _OC3IF = 0;
    _OC3IE = 1;
    _OC3IP = 7;
}

void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void)
{
    _OC3IF = 0;
    OC3CON2bits.TRIGSTAT = 0;
    //SERVO_3_PIN = 0;
}

void Init_OC4(void)
{
    OC4R = PERIOD_1_MS; // Specify the pulse width of the OC4 Output ,initialised for 5us width
    OC4RS = PERIOD_20_MS; // Initialize the secondary compare register// Initialize Output Compare Module
    OC4CON1 = 0x0; // Clear all control bits
    OC4CON2 = 0x0; // Clear all control bits
    OC4CON1bits.OCTSEL = 0x0; // Select timer2 clock as clock source
    OC4CON2bits.SYNCSEL = 0b01110; // Select Timer4 as sync source
    OC4CON1bits.OCM = 0x6; // Double compare continuous pulse mode
}

void Init_OC5(void)
{
    OC5R = PERIOD_1_MS; // Specify the pulse width of the OC5 Output ,initialised for 5us width
    OC5RS = PERIOD_20_MS; // Initialize the secondary compare register// Initialize Output Compare Module
    OC5CON1 = 0x0; // Clear all control bits
    OC5CON2 = 0x0; // Clear all control bits
    OC5CON1bits.OCTSEL = 0x0; // Select timer2 clock as clock source
    OC5CON2bits.SYNCSEL = 0b01110; // Select Timer2 as sync source
    OC5CON1bits.OCM = 0x6; // Double compare continuous pulse mode
}

void Init_OC6(void)
{
    OC6R = PERIOD_1_MS; // Specify the pulse width of the OC6 Output ,initialised for 5us width
    OC6RS = PERIOD_20_MS; // Initialize the secondary compare register// Initialize Output Compare Module
    OC6CON1 = 0x0; // Clear all control bits
    OC6CON2 = 0x0; // Clear all control bits
    OC6CON1bits.OCTSEL = 0x0; // Select timer2 clock as clock source
    OC6CON2bits.SYNCSEL = 0b01110; // Select Timer2 as sync source
    OC6CON1bits.OCM = 0x6; // Double compare continuous pulse mode
}

void Init_OC7(void)
{
    OC7R = 100; // Specify the pulse width of the OC7 Output ,initialised for 5us width
    OC7RS = 0x000; // Initialize the secondary compare register// Initialize Output Compare Module
    OC7CON1 = 0x0; // Clear all control bits
    OC7CON2 = 0x0; // Clear all control bits
    OC7CON1bits.OCTSEL = 0x0; // Select timer2 clock as clock source
    OC7CON2bits.SYNCSEL = 0xc; // Select Timer2 as sync source
    OC7CON1bits.OCM = 0x6; // Double compare continuous pulse mode
}

void Init_OC8(void)
{
    OC8R = 100; // Specify the pulse width of the OC8 Output ,initialised for 5us width
    OC8RS = 0x000; // Initialize the secondary compare register// Initialize Output Compare Module
    OC8CON1 = 0x0; // Clear all control bits
    OC8CON2 = 0x0; // Clear all control bits
    OC8CON1bits.OCTSEL = 0x0; // Select timer2 clock as clock source
    OC8CON2bits.SYNCSEL = 0xc; // Select Timer2 as sync source
    OC8CON1bits.OCM = 0x6; // Double compare continuous pulse mode
}

void TurnOff_OC(void)
{
    OC1CON1bits.OCM = 0;
    OC2CON1bits.OCM = 0;
    OC3CON1bits.OCM = 0;
    OC4CON1bits.OCM = 0;
    OC5CON1bits.OCM = 0;
    OC6CON1bits.OCM = 0;
    OC7CON1bits.OCM = 0;

    OC8CON1bits.OCM = 0;
}

//inline uint16_t ConvertAngleToPeriod(uint8_t angle)
//{
//    return PERIOD_1_MS+(uint16_t)((float)PERIOD_1_MS*MAX((float)angle,180.0)/180.0);
//}
//uint16_t ConvertAngleToPeriodTuned(uint8_t angle)
//{
//    return (uint16_t) (PERIOD_0_5_MS_TUNED+((float)PERIOD_2_5_MS_TUNED*MIN(angle,180.0)/180.0));
//}
//void OCOpenClapets(uint8 clapetsMask)
//{
//    OCSetServoAngle(clapetsMask&(0x01<<2),SERVO1_OPEN_ANGLE);
//
//    headState.clapetsOpenedMask|=clapetsMask;
//}
//void OCCloseClapets(uint8 clapetsMask)
//{
//    OCSetServoAngle(clapetsMask&(0x01<<2),SERVO1_CLOSE_ANGLE);
//
//    headState.clapetsOpenedMask&=~clapetsMask;
//}


//0-90-180
//void OCSetServoAngle(uint8_t servo,uint8_t angle)
//{
//    switch(servo)
//    {
//        case 1:OC1R=ConvertAngleToPeriodTuned(angle); break;
//        case 2:OC2R=ConvertAngleToPeriodTuned(angle); break;
//        case 3:OC3R=ConvertAngleToPeriod(angle); break;
//        case 4:OC4R=ConvertAngleToPeriodTuned(angle); break;
//        case 5:OC5R=ConvertAngleToPeriodTuned(angle); break;
//        case 6:OC6R=ConvertAngleToPeriodTuned(angle); break;
//        case 7:OC7R=ConvertAngleToPeriod(angle); break;
//        case 8:OC8R=ConvertAngleToPeriod(angle); break;            
//        default:
//            break;
//    }
//}
//void OCPeriscopeInit(void)
//{
//    OCSetServoAngle(SERVO_PERISCOPE,90);
//    headState.periscope.currentAngle=90;
//}
//void OCPeriscopeSetAngle(uint8 angle)
//{
//    if(periscopeState!=PERISCOPE_STATE_PERISCOPE_IS_GETTING_IN && periscopeState!=PERISCOPE_STATE_PERISCOPE_IS_IN && periscopeState!=PERISCOPE_STATE_PERISCOPE_IS_GETTING_OUT)
//    {
//        OCSetServoAngle(SERVO_PERISCOPE,angle);
//        headState.periscope.currentAngle=angle;
//    }
//}