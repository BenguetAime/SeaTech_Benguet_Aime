#ifndef UTLN_OUTPUTCOMPARE_H
#define	UTLN_OUTPUTCOMPARE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "main.h"

#define CoeffAngle 0.009875
#define AngleMin 0.72

#define SERVO1 1
#define SERVO2 2
#define SERVO3 3

#ifdef GROS_ROBOT
    #define BLOCAGE SERVO2
    #define MANDIBULE SERVO3
    #define BLOCAGE_OUVERT 21
    #define BLOCAGE_FERME  60
    #define MANDIBULE_OUVERT 159
    #define MANDIBULE_FERME 10
#endif

#ifdef PETIT_ROBOT
    //nothing yet
#endif

#define PERIOD_1_MS 156*4
#define PERIOD_0_7_MS PERIOD_1_MS*0.72
#define PERIOD_1_1_MS PERIOD_1_MS*1.1
#define PERIOD_1_9_MS PERIOD_1_MS*1.9
#define PERIOD_2_3_MS PERIOD_1_MS*2.3
#define PERIOD_1_5_MS PERIOD_1_MS*1.5


#define PERIOD_0_5_MS_TUNED 345
#define PERIOD_2_5_MS_TUNED (1100)

#define SERVO1_OPEN_ANGLE 80
#define SERVO1_CLOSE_ANGLE 100

#define PERIOD_20_MS 3125

void Init_OC1( void );
void Init_OC2( void );
void Init_OC3( void );
void Init_OC4( void );
void Init_OC5( void );
void Init_OC6( void );
void Init_OC7( void );
void Init_OC8( void );
void SetPosServo(unsigned char ServoNum, double Angle);
//inline uint16_t ConvertAngleToPeriod(uint8_t angle);
//void OCOpenClapets(uint8 clapetsMask);
//void OCCloseClapets(uint8 clapetsMask);
//void OCSetServoAngle(uint8_t servo,uint8_t angle);
//void OCPeriscopeInit(void);
//void OCPeriscopeSetAngle(uint8 angle);
#endif	/* XC_HEADER_TEMPLATE_H */

