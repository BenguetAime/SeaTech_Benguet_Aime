/* 
 * File:   ustv_timers.h
 * Author: Valentin
 *
 * Created on 25 septembre 2013, 18:07
 */

#ifndef UTLN_TIMERS_H
#define	UTLN_TIMERS_H

#include <xc.h>
#include "Define.h"                     //Include Hardware definition of target board (PIC reference, FCY definition,...)
#include "UTLN_Typedefs.h"              //Include common bool type

//Librairy Configuration
#define USE_TIMER_CALLBACK
#define TIMER_V2

//Detection automatique du nombre de timers existants (grace aux definitions incluses dans xc.h)
#ifdef TMR9
#define TIMER_COUNT 9
#elif defined(TMR8)
#define TIMER_COUNT 8
#elif defined(TMR7)
#define TIMER_COUNT 7
#elif defined(TMR6)
#define TIMER_COUNT 6
#elif defined(TMR5)
#define TIMER_COUNT 5
#elif defined(TMR4)
#define TIMER_COUNT 4
#elif defined(TMR3)
#define TIMER_COUNT 3
#elif defined(TMR2)
#define TIMER_COUNT 2
#elif defined(TMR1)
#define TIMER_COUNT 1
#endif
#define TIMER1_ID 0
#define TIMER2_ID 1
#define TIMER3_ID 2
#define TIMER4_ID 3
#define TIMER5_ID 4
#define TIMER6_ID 5
#define TIMER7_ID 6
#define TIMER8_ID 7
#define TIMER9_ID 8

#ifndef OFF
#define OFF 0
#endif
#ifndef ON
#define ON 1
#endif
#if defined(USE_TIMER_CALLBACK) && !defined(USE_EXTERNAL_TIMER_INT_ROUTINE)
    #if !defined( _T1Interrupt) && defined(TMR1)
    #define _T1Interrupt _T1Interrupt
    #endif
    #if !defined( _T2Interrupt) && defined(TMR2)
    #define _T2Interrupt _T2Interrupt
    #endif
    #if !defined( _T3Interrupt) && defined(TMR3)
    #define _T3Interrupt _T3Interrupt
    #endif
    #if !defined( _T4Interrupt) && defined(TMR4)
    #define _T4Interrupt _T4Interrupt
    #endif
    #if !defined( _T5Interrupt) && defined(TMR5)
    #define _T5Interrupt _T5Interrupt
    #endif
    #if !defined( _T6Interrupt) && defined(TMR6)
    #define _T6Interrupt _T6Interrupt
    #endif
    #if !defined( _T7Interrupt) && defined(TMR7)
    #define _T7Interrupt _T7Interrupt
    #endif
    #if !defined( _T8Interrupt) && defined(TMR8)
    #define _T8Interrupt _T8Interrupt
    #endif
    #if !defined( _T9Interrupt) && defined(TMR9)
    #define _T9Interrupt _T9Interrupt
    #endif
#endif
extern volatile unsigned long g_longTimeStamp;

typedef unsigned char TIMER_ID ;            //Definition d'un type TIMER_ID comme etant un unsigned char
typedef void (*TIMER_CallBack)(void);       //Definition d'un type TIMER_CallBack comme etant un pointeur de fonction
void SetTimerFreq(TIMER_ID timer,float freq);
void SetTimerInterruptPriority(TIMER_ID id, bool interruptEnable, uint8 interruptPri);
unsigned int GetClockDivisorIntFromBits(uint8 prescaler);
void RegisterTimerCallBack(TIMER_ID id, TIMER_CallBack callback);
void SetTimerPeriodMs(TIMER_ID id, uint16  ms);
void SetTimerPeriodUs(TIMER_ID timer, uint16  us);
void TurnOnOffTimer(TIMER_ID id,bool on);
void InitTimerWithCallBack(TIMER_ID id, float timerFreq, TIMER_CallBack callback, bool on);
void RegisterTimerWithCallBack(TIMER_ID id, float timerFreq, TIMER_CallBack callback, bool interruptEnable, uint8 interruptPri, bool on);


#endif	/* USTV_TIMERS_H */

