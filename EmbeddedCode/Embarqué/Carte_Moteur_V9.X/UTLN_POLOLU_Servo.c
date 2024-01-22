#include "UTLN_POLOLU_Servo.h"
#include "UTLN_Communication.h"

POLOLUUSER_UART_Func pololuUartFunc;
void PololuRegisterUserUartFunc(POLOLUUSER_UART_Func uartFunc)
{
    if(uartFunc != NULL)
        pololuUartFunc = *uartFunc;
}

void PololuSetTargetPos(unsigned char servoChannel,unsigned char degrees)
{
    unsigned char command[4];
    unsigned int pwmUs=1000+ ((float)degrees/180.0)*950;
    pwmUs=pwmUs*4;
    command[0]=POLOLU_SERVO_SET_POSITION;
    command[1]=servoChannel;
    command[2]=pwmUs & 0x7F;
    command[3]=(pwmUs>>7)&0x7F;
        if(pololuUartFunc!=NULL)
        (*pololuUartFunc)(command,4);
}

void PololuSetTargetUs(unsigned char servoChannel,unsigned int us)
{
    unsigned char command[4];
    unsigned int pwmUs=us;
    pwmUs=pwmUs*4;
    command[0]=POLOLU_SERVO_SET_POSITION;
    command[1]=servoChannel;
    command[2]=pwmUs & 0x7F;
    command[3]=(pwmUs>>7)&0x7F;
        if(pololuUartFunc!=NULL)
            (*pololuUartFunc)(command,4);
}