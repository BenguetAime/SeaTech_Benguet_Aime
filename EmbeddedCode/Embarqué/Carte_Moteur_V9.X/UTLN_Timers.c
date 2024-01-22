/* 
 * File:   UTLN_Timers.h
 * Author: Valentin
 *
 * Created on 10 septembre 2018, 10:29
 */

/*********************************************************************
 * INCLUDES
 */
#include "UTLN_Timers.h"


/*********************************************************************
 * GLOBAL VARIABLES
 */
TIMER_CallBack callBack[TIMER_COUNT];               //Declaration d'un tableau de pointeur de fonctions (type: TIMER_CallBack))

/*********************************************************************
 * @fn      SetTimerFreq
 *
 * @brief   Set the prescaler and the period from frequency input
 *
 * @param   timer : ID of timer to set
 * @param   freq  : frequency of timer
 *
 * @return  void
 */
void SetTimerFreq(TIMER_ID timer,float freq)
{
    switch(timer)
    {
        #ifdef TMR1
        case TIMER1_ID:
            T1CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T1CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T1CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T1CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR1 = (int) (FCY / freq / 256);
                    }
                    else
                        PR1 = (int) (FCY / freq / 64);
                }
                else
                    PR1 = (int) (FCY / freq / 8);
            }
            else
                PR1 = (int) (FCY / freq);            
            break;
        #endif
        #ifdef TMR2
        case TIMER2_ID:
            T2CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T2CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T2CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T2CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR2 = (int) (FCY / freq / 256);
                    }
                    else
                        PR2 = (int) (FCY / freq / 64);
                }
                else
                    PR2 = (int) (FCY / freq / 8);
            }
            else
                PR2 = (int) (FCY / freq);
            break;
        #endif
        #ifdef TMR3
        case TIMER3_ID:
            T3CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T3CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T3CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T3CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR3 = (int) (FCY / freq / 256);
                    }
                    else
                        PR3 = (int) (FCY / freq / 64);
                }
                else
                    PR3 = (int) (FCY / freq / 8);
            }
            else
                PR3 = (int) (FCY / freq);            
            break;
        #endif
        #ifdef TMR4
        case TIMER4_ID:
            T4CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T4CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T4CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T4CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR4 = (int) (FCY / freq / 256);
                    }
                    else
                        PR4 = (int) (FCY / freq / 64);
                }
                else
                    PR4 = (int) (FCY / freq / 8);
            }
            else
                PR4 = (int) (FCY / freq);            
            break;
        #endif
        #ifdef TMR5
        case TIMER5_ID:
            T5CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T5CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T5CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T5CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR5 = (int) (FCY / freq / 256);
                    }
                    else
                        PR5 = (int) (FCY / freq / 64);
                }
                else
                    PR5 = (int) (FCY / freq / 8);
            }
            else
                PR5 = (int) (FCY / freq);            
            break;
        #endif
        #ifdef TMR6
        case TIMER6_ID:
            T6CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T6CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T6CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T6CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR6 = (int) (FCY / freq / 256);
                    }
                    else
                        PR6 = (int) (FCY / freq / 64);
                }
                else
                    PR6 = (int) (FCY / freq / 8);
            }
            else
                PR6 = (int) (FCY / freq);            
            break;
        #endif
        #ifdef TMR7
        case TIMER7_ID:
            T7CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T7CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T7CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T7CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR7 = (int) (FCY / freq / 256);
                    }
                    else
                        PR7 = (int) (FCY / freq / 64);
                }
                else
                    PR7 = (int) (FCY / freq / 8);
            }
            else
                PR7 = (int) (FCY / freq);            
            break;
        #endif
        #ifdef TMR8
        case TIMER8_ID:
            T8CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T8CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T8CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T8CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR8 = (int) (FCY / freq / 256);
                    }
                    else
                        PR8 = (int) (FCY / freq / 64);
                }
                else
                    PR8 = (int) (FCY / freq / 8);
            }
            else
                PR8 = (int) (FCY / freq);            
            break;
        #endif
        #ifdef TMR9
        case TIMER9_ID:
            T9CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
            if (FCY / freq > 65535)
            {
                T9CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
                if (FCY / freq / 8 > 65535)
                {
                    T9CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
                    if (FCY / freq / 64 > 65535)
                    {
                        T9CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                        PR9 = (int) (FCY / freq / 256);
                    }
                    else
                        PR9 = (int) (FCY / freq / 64);
                }
                else
                    PR9 = (int) (FCY / freq / 8);
            }
            else
                PR9 = (int) (FCY / freq);            
            break;
        #endif
        default:
            break;            
    }   
}
/*********************************************************************
 * @fn      SetTimerInterruptPriority
 *
 * @brief   Set the interrupt priority and if enable fo timer id
 *
 * @param   timer : ID of timer to set
 * @param   interruptEnable  : interrupt enable
 * @param   interruptPri  : priority of interrupt
 *
 * @return  void
 */
void SetTimerInterruptPriority(TIMER_ID id, bool interruptEnable, uint8 interruptPri)
{
    switch(id)
    {
        #if defined(TMR1)
        case TIMER1_ID:IFS0bits.T1IF = 0; // Clear Timer Interrupt Flag
                        IEC0bits.T1IE = interruptEnable; // Enable Timer interrupt
                        IPC0bits.T1IP=interruptPri;
                        break;
        #endif
        #if defined(TMR2)
        case TIMER2_ID:IFS0bits.T2IF = 0; // Clear Timer Interrupt Flag
                        IEC0bits.T2IE = interruptEnable; // Enable Timer interrupt
                        IPC1bits.T2IP=interruptPri;
                        break;
        #endif
        #if defined(TMR3)
        case TIMER3_ID:IFS0bits.T3IF = 0; // Clear Timer Interrupt Flag
                        IEC0bits.T3IE = interruptEnable; // Enable Timer interrupt
                        IPC2bits.T3IP=interruptPri;
                        break;
        #endif
        #if defined(TMR4)
        case TIMER4_ID:IFS1bits.T4IF = 0; // Clear Timer Interrupt Flag
                        IEC1bits.T4IE = interruptEnable; // Enable Timer interrupt
                        IPC6bits.T4IP=interruptPri;
                        break;
        #endif
        #if defined(TMR5)
        case TIMER5_ID:IFS1bits.T5IF = 0; // Clear Timer Interrupt Flag
                        IEC1bits.T5IE = interruptEnable; // Enable Timer interrupt
                        IPC7bits.T5IP=interruptPri;
                        break;
        #endif
        #if defined(TMR6)            
        case TIMER6_ID:IFS2bits.T6IF = 0; // Clear Timer Interrupt Flag
                        IEC2bits.T6IE = interruptEnable; // Enable Timer interrupt
                        IPC11bits.T6IP=interruptPri;
                        break;
        #endif
        #if defined(TMR7)   
        case TIMER7_ID:IFS3bits.T7IF = 0; // Clear Timer Interrupt Flag
                        IEC3bits.T7IE = interruptEnable; // Enable Timer interrupt
                        IPC12bits.T7IP=interruptPri;
                        break;
        #endif
        #if defined(TMR8)   
        case TIMER8_ID:IFS3bits.T8IF = 0; // Clear Timer Interrupt Flag
                        IEC3bits.T8IE = interruptEnable; // Enable Timer interrupt
                        IPC12bits.T8IP=interruptPri;
                        break;
        #endif
        #if defined(TMR9)                         
        case TIMER9_ID:IFS3bits.T9IF = 0; // Clear Timer Interrupt Flag
                        IEC3bits.T9IE = interruptEnable; // Enable Timer interrupt
                        IPC13bits.T9IP=interruptPri;
                        break;
        #endif                     
    }
}

/*********************************************************************
 * @fn      RegisterTimerCallBack
 *
 * @brief   Register a callBack for timer id
 *
 * @param   id : ID of timer to set
 * @param   callBack  : callBack of timer
 *
 * @return  void
 */
void RegisterTimerCallBack(TIMER_ID id, TIMER_CallBack callback)
{
    if(id >0 && id <= TIMER_COUNT)
        if(callback!=NULL)
            callBack[id]=callback;
}

/*********************************************************************
 * @fn      SetTimerPeriodMs
 *
 * @brief   Set the timer Period in MS
 *
 * @param   id : ID of timer to set
 * @param   ms  : periode in ms
 *
 * @return  void
 */
void SetTimerPeriodMs(TIMER_ID id, uint16  ms)
{
    float freq= 1.0/ ((float)ms/1000.0);
    SetTimerFreq( id, freq);
}

/*********************************************************************
 * @fn      SetTimerPeriodUs
 *
 * @brief   Set the timer Period in MS
 *
 * @param   id : ID of timer to set
 * @param   us  : periode in us
 *
 * @return  void
 */
void SetTimerPeriodUs(TIMER_ID timer, uint16  us)
{
    float freq= 1.0/ ((float)us/1000000.0);
    SetTimerFreq( timer, freq);
}

/*********************************************************************
 * @fn      TurnOnOffTimer
 *
 * @brief   Set the timer Period in MS
 *
 * @param   id : ID of timer to set
 * @param   on  : enable timer
 *
 * @return  void
 */
void TurnOnOffTimer(TIMER_ID id,bool on)
{
    switch(id)
    {
        #if defined(TMR1)
        case TIMER1_ID:T1CONbits.TON=on;break;
        #endif  
        #if defined(TMR2)
        case TIMER2_ID:T2CONbits.TON=on;break;
        #endif
        #if defined(TMR3)
        case TIMER3_ID:T3CONbits.TON=on;break;
        #endif
        #if defined(TMR4)
        case TIMER4_ID:T4CONbits.TON=on;break;
        #endif
        #if defined(TMR5)
        case TIMER5_ID:T5CONbits.TON=on;break;
        #endif
        #if defined(TMR6)
        case TIMER6_ID:T6CONbits.TON=on;break;
        #endif
        #if defined(TMR7)
        case TIMER7_ID:T7CONbits.TON=on;break;
        #endif
        #if defined(TMR8)
        case TIMER8_ID:T8CONbits.TON=on;break;
        #endif
        #if defined(TMR9)
        case TIMER9_ID:T9CONbits.TON=on;break;
        #endif
        default: break;
    }
}

/*********************************************************************
 * @fn      InitTimerWithCallBack
 *
 * @brief   Initialise a timer at frequency and call its callback if enabled
 *
 * @param   id : ID of timer to set
 * @param   timerFreq  : frequency of timer
 * @param   callback  : callBack of timer
 * @param   on  : enable timer
 *
 * @return  void
 */
void InitTimerWithCallBack(TIMER_ID id, float timerFreq, TIMER_CallBack callback, bool on)
{
    TurnOnOffTimer(id, OFF);
    SetTimerFreq(id,timerFreq);
    RegisterTimerCallBack(id, callback);
    TurnOnOffTimer(id, on);
}

/*********************************************************************
 * @fn      RegisterTimerWithCallBack
 *
 * @brief   Register a timer at frequency, with specified interrupt priority and call its callback if enabled
 *
 * @param   id : ID of timer to set
 * @param   timerFreq  : frequency of timer
 * @param   callback  : callBack of timer
 * @param   interruptEnable  : enable interrupt
 * @param   interruptPri  : priority of interrupt
 * @param   on  : enable timer
 *
 * @return  void
 */
void RegisterTimerWithCallBack(TIMER_ID id, float timerFreq, TIMER_CallBack callback, bool interruptEnable, uint8 interruptPri, bool on)
{
    TurnOnOffTimer(id, OFF);
    SetTimerFreq(id,timerFreq);
    RegisterTimerCallBack(id, callback);
    SetTimerInterruptPriority(id, interruptEnable, interruptPri);
    TurnOnOffTimer(id, on);
}

#if defined( _T1Interrupt ) && defined(TMR1)
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
    {
        IFS0bits.T1IF = CLEAR;
        if(callBack[TIMER1_ID]!=NULL)
            (*callBack[TIMER1_ID])();
    }
#endif
#if defined( _T2Interrupt) && defined(TMR2)
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
    {
        IFS0bits.T2IF = CLEAR;
        if(callBack[TIMER2_ID]!=NULL)
            (*callBack[TIMER2_ID])();
    }
#endif
#if defined( _T3Interrupt) && defined(TMR3)
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
    {
        IFS0bits.T3IF = CLEAR;
        if(callBack[TIMER3_ID]!=NULL)
            (*callBack[TIMER3_ID])();
    }
#endif
#if defined( _T4Interrupt) && defined(TMR4)
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void)
    {
        IFS1bits.T4IF = CLEAR;
        if(callBack[TIMER4_ID]!=NULL)
            (*callBack[TIMER4_ID])();
    }
#endif
#if defined( _T5Interrupt) && defined(TMR5)
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void)
    {
        IFS1bits.T5IF = CLEAR;
        if(callBack[TIMER5_ID]!=NULL)
            (*callBack[TIMER5_ID])();
    }
#endif
#if defined( _T6Interrupt) && defined(TMR6)
void __attribute__((interrupt, no_auto_psv)) _T6Interrupt(void)
    {
        IFS2bits.T6IF = CLEAR;
        if(callBack[TIMER6_ID]!=NULL)
            (*callBack[TIMER6_ID])();
    }
#endif
#if defined( _T7Interrupt) && defined(TMR7)
void __attribute__((interrupt, no_auto_psv)) _T7Interrupt(void)
    {
        IFS3bits.T7IF = CLEAR;
        if(callBack[TIMER7_ID]!=NULL)
            (*callBack[TIMER7_ID])();
    }
#endif
#if defined(_T8Interrupt) && defined(TMR8)
void __attribute__((interrupt, no_auto_psv)) _T8Interrupt(void)
    {
        IFS3bits.T8IF = CLEAR;
        if(callBack[TIMER8_ID]!=NULL)
            (*callBack[TIMER8_ID])();
    }
#endif
#if defined( _T9Interrupt) && defined(TMR9)
void __attribute__((interrupt, no_auto_psv)) _T9Interrupt(void)
    {
        IFS3bits.T9IF = CLEAR;
        if(callBack[TIMER9_ID]!=NULL)
            (*callBack[TIMER9_ID])();
    }
#endif