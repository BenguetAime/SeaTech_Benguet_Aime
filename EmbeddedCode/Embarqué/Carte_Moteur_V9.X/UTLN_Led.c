#include "UTLN_Led.h"
#include <stdbool.h>

static LED_MGMTLeds SystemLed[LED_COUNT];
static uint8 ledRegistered=0;
extern unsigned volatile long g_longTimeStamp;
bool inhibitLed;


void LEDInhibit(void)
{
    inhibitLed=true;
}
void LEDDesInhibit(void)
{
    inhibitLed=false;
}
bool LEDIsInhibed(void)
{
    return inhibitLed;
}
void LEDRegister(uint8 ledNum, volatile unsigned int *port, uint8 ledPinNum)
{
    if(ledRegistered<LED_COUNT)
    {
        SystemLed[ledNum].port=port;
        SystemLed[ledNum].pin=ledPinNum;
        ledRegistered++;
    }
}
void LEDInit(void)
{
    uint8 i;
    for(i=0;i<LED_COUNT;i++)
        SystemLed[i].mode = LED_NOT_INITIALISED;    
}

void LEDPowerOff(void)
{
    uint8 i;
    for(i=0;i<ledRegistered;i++)
        LEDSetMode(i, LED_OFF);  
}

void LEDSetMode(unsigned char ledNum, LED_Mode mode)
{   
    if(ledNum<=ledRegistered)
        if(SystemLed[ledNum].mode != mode)
        {
            SystemLed[ledNum].mode = mode;
            SystemLed[ledNum].customModeNumber = 0;
            LEDSetTimings(ledNum, SystemLed[ledNum].mode);
        }
}

bool LEDGetState(unsigned char ledNum)
{   
    if(ledNum<=ledRegistered)
        return *SystemLed[ledNum].port & (1<<SystemLed[ledNum].pin);
    else 
        return 0;
}

void LEDSetCustomMode(unsigned char ledNum, unsigned char customModeNumber, unsigned long onTime, unsigned long offTime, int blinks, int phase)
{
    if(ledNum<=ledRegistered)
        if(SystemLed[ledNum].mode != LED_CUSTOM || SystemLed[ledNum].customModeNumber != customModeNumber)
        {
            SystemLed[ledNum].mode = LED_CUSTOM;
            SystemLed[ledNum].customModeNumber = customModeNumber;
            LEDSetCustomTimings(ledNum, onTime, offTime, blinks, phase);
        }
}

void LEDSetNextMode(unsigned char ledNum, LED_Mode mode)
{    
    SystemLed[ledNum].nextMode = mode;
}

void LEDSetTimings(unsigned char ledNum, LED_Mode mode)
{
    unsigned long onTime=0;
    unsigned long offTime=0;
    int blinksRemaining=0;
    
    switch(mode)
    {
        case LED_OFF:
            onTime = 0;            
            offTime = 100;         
            blinksRemaining = -1;
            break;
        case LED_ON:
            onTime = 100;            
            offTime = 0;         
            blinksRemaining = -1;
            break;
        case LED_BLINK_1X_100_100:            
            onTime = 100;            
            offTime = 100;         
            blinksRemaining = 1;
            break;
        case LED_BLINK_CONT_100_100:    
            onTime = 100;            
            offTime = 100;         
            blinksRemaining = -1;
            break;
        case LED_BLINK_CONT_100_1000:    
            onTime = 100;            
            offTime = 1000;         
            blinksRemaining = -1;
            break;
        case LED_BLINK_CONT_100_2000:    
            onTime = 100;            
            offTime = 2000;         
            blinksRemaining = -1;
            break;
        case LED_BLINK_1X_1000_1000:    
            onTime = 1000;            
            offTime = 1000;         
            blinksRemaining = 1;
            break;
        case LED_BLINK_CONT_1000_1000:    
            onTime = 1000;            
            offTime = 1000;         
            blinksRemaining = -1;
            break;

        case LED_BLINK_CONT_1000_2000:    
            onTime = 1000;            
            offTime = 2000;         
            blinksRemaining = -1;
            break;
        case LED_BLINK_CONT_100_5000:    
            onTime = 100;            
            offTime = 5000;         
            blinksRemaining = -1;
            break;
        case LED_BLINK_2X_1000_1000:    
            onTime = 1000;            
            offTime = 1000;         
            blinksRemaining = 2;
            break;
        case LED_BLINK_3X_1000_1000:    
            onTime = 1000;            
            offTime = 1000;         
            blinksRemaining = 3;
            break;
        case LED_BLINK_3X_166_166:    
            onTime = 166;            
            offTime = 166;         
            blinksRemaining = 3;
            break;
        default:
            break;
    }
    
    
    SystemLed[ledNum].onTime = onTime;            
    SystemLed[ledNum].offTime = offTime;            
    SystemLed[ledNum].blinksRemaining = blinksRemaining;      
    SystemLed[ledNum].state = LIGHT_ON;
    SystemLed[ledNum].nextLedEventTime = g_longTimeStamp+SystemLed[ledNum].onTime;
}

void LEDSetCustomTimings(unsigned char ledNum, unsigned long onTime, unsigned long offTime, int blinksRemaining, int phase)
{
    SystemLed[ledNum].onTime = onTime;            
    SystemLed[ledNum].offTime = offTime;            
    SystemLed[ledNum].blinksRemaining = blinksRemaining;      
    SystemLed[ledNum].state = LIGHT_ON;
    SystemLed[ledNum].nextLedEventTime = g_longTimeStamp+SystemLed[ledNum].onTime-phase;
}

#define TIME_STAMP_RESET_LIMIT 10
void LEDStateMachine(void)
{
    //Gestion des overflow timestamp et des reset timestamp
    if(g_longTimeStamp < TIME_STAMP_RESET_LIMIT)
    {
        uint8 i;
        for(i=0;i<ledRegistered;i++)
            SystemLed[i].nextLedEventTime = g_longTimeStamp;
    }
    
    uint8 i;
    for(i=0;i<ledRegistered;i++)
    {
        switch(SystemLed[i].state)
        {
            case LIGHT_ON:
            {
                if(g_longTimeStamp > SystemLed[i].nextLedEventTime && SystemLed[i].offTime>0)
                {
                    SystemLed[i].state = LIGHT_OFF;                
                    SystemLed[i].nextLedEventTime = g_longTimeStamp+SystemLed[i].offTime;
                }
                break;
            }
            case LIGHT_OFF:
            {
                if(g_longTimeStamp > SystemLed[i].nextLedEventTime && SystemLed[i].onTime>0)
                {
                    //Mode nb impulsions limité
                    if(SystemLed[i].blinksRemaining > 0)
                    {
                        SystemLed[i].state = LIGHT_ON;
                        SystemLed[i].nextLedEventTime = g_longTimeStamp+SystemLed[i].onTime;
                        --SystemLed[i].blinksRemaining;
                    }
                    else if(SystemLed[i].blinksRemaining == -1)
                    {
                        //Mode continu
                        SystemLed[i].state = LIGHT_ON;
                        SystemLed[i].nextLedEventTime = g_longTimeStamp+SystemLed[i].onTime;
                    }
                    else
                    {
                        //Sequence terminée, on passe à la suite
                        SystemLed[i].mode = SystemLed[i].nextMode;
                        SystemLed[i].nextMode = LED_OFF;
                        LEDSetTimings(i, SystemLed[i].mode);    
                    }
                }
                break;
            }
        }
    
        if(SystemLed[i].state == LIGHT_ON && !inhibitLed)
            *SystemLed[i].port |= 1U << SystemLed[i].pin;
        else
            *SystemLed[i].port &= ~(1U << SystemLed[i].pin);
    }
}
