#ifndef UTLN_LED_H
#define	UTLN_LED_H
#include "UTLN_Typedefs.h"
#include "HardwareProfile_CM_V9.h"

#define LED_COUNT UTLN_LED_COUNT
#define _ON 1
#define _OFF 0


extern volatile unsigned long timeStamp;
typedef enum {
    LED_NOT_INITIALISED,
    LED_ON,
    LED_OFF,
    LED_BLINK_1X_100_100,
    LED_BLINK_CONT_100_100,
    LED_BLINK_CONT_100_1000,
    LED_BLINK_CONT_100_2000,
    LED_BLINK_CONT_1000_1000,
    LED_BLINK_CONT_1000_2000,
    LED_BLINK_CONT_100_5000,
    LED_BLINK_1X_1000_1000,
    LED_BLINK_2X_1000_1000,
    LED_BLINK_3X_1000_1000,
    LED_BLINK_3X_166_166, 
    LED_CUSTOM,
    }LED_Mode;

typedef enum {
    LIGHT_ON=1,
    LIGHT_OFF=0,
    }LED_State;

typedef struct LED_MGMT_STRUCT{
    volatile unsigned int* port;
    uint8 pin;
    LED_Mode mode;    
    LED_Mode nextMode;
    unsigned char customModeNumber;
    unsigned long onTime;
    unsigned long offTime;
    int blinksRemaining;
        
    unsigned long nextLedEventTime;
    LED_State state;
}LED_MGMTLeds;

void LEDInhibit(void);
void LEDDesInhibit(void);
bool LEDIsInhibed(void);
void LEDRegister(uint8 ledNum, volatile unsigned int *port, uint8 ledPinNum);
void LEDInit();
void SetLedState(unsigned char led1State, unsigned char led2State, unsigned char led3State);
void LEDPowerOff(void);
void LEDSetMode(unsigned char ledNum, LED_Mode mode);
bool LEDGetState(unsigned char ledNum);
void LEDSetCustomMode(unsigned char ledNum, unsigned char customModeNumber, unsigned long onTime, unsigned long offTime, int blinks, int phase);
void LEDSetTimings(unsigned char ledNum, LED_Mode mode);
void LEDSetCustomTimings(unsigned char ledNum, unsigned long onTime, unsigned long offTime, int blinksRemaining, int phase);
void LEDStateMachine(void);
#endif
