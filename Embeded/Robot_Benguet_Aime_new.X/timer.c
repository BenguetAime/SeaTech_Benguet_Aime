#include <xc.h>
#include "timer.h"
#include "IO.h"
#include "PWM.h"
#include "ADC.h"
#include "main.h"

unsigned char toggle = 0;
unsigned long timestamp;


// I n i t i a l i s a t i o n d ?un time r 32 b i t s

void InitTimer23(void) {
    T3CONbits .TON = 0; // Stop any 16?b i t Timer3 o p e r a ti o n
    T2CONbits .TON = 0; // Stop any 16/32? b i t Timer3 o p e r a ti o n
    T2CONbits . T32 = 1; // Enable 32?b i t Timer mode
    T2CONbits .TCS = 0; // S e l e c t i n t e r n a l i n s t r u c t i o n c y c l e cl o c k
    T2CONbits .TCKPS = 0b00; // S e l e c t 1: 1 P r e s c a l e r
    TMR3 = 0x00; // Cle a r 32?b i t Timer (msw)
    TMR2 = 0x00; // Cle a r 32?b i t Timer ( l sw )
    PR3 = 0x04C4; // Load 32?b i t p e ri o d v al u e (msw)
    PR2 = 0xB400; // Load 32?b i t p e ri o d v al u e ( l sw )
    IPC2bits . T3IP = 0x01; // Se t Timer3 I n t e r r u p t P r i o r i t y L e v el
    IFS0bits . T3IF = 0; // Cle a r Timer3 I n t e r r u p t Flag
    IEC0bits . T3IE = 1; // Enable Timer3 i n t e r r u p t
    T2CONbits .TON = 1; // S t a r t 32?b i t Timer
    //? Example code for Timer3ISR ?//
}


// I n t e r r u p t i o n du time r 32 b i t s s u r 2?3

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits . T3IF = 0; // Cle a r Timer3 I n t e r r u p t Flag
    LED_ORANGE = !LED_ORANGE;

    if (toggle == 0) {
        PWMSetSpeedConsigne(30, MOTEUR_DROIT);
        PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
        toggle = 1;
    } else {
        PWMSetSpeedConsigne(-30, MOTEUR_DROIT);
        PWMSetSpeedConsigne(-30, MOTEUR_GAUCHE);
        toggle = 0;
    }
}

void InitTimer1(float freq) {
    //Timer1 pour h o r o d a t e r l e s mesures ( 1ms)
    T1CONbits .TON = 0; // Di s a bl e Timer
    //T1CONbits .TCKPS = 0b01; // P r e s c a l e r                         //////////a été commentée au 3.de la partie 1
    // 11 = 1: 2 5 6 p r e s c a l e v al u e
    // 10 = 1: 6 4 p r e s c a l e v al u e
    // 01 = 1: 8 p r e s c a l e v al u e
    // 00 = 1: 1 p r e s c a l e v al u e


    T1CONbits .TCS = 0; // cl o c k s o u r c e = i n t e r n a l cl o c k
    //PR1 = 40000000/64/50;                                                //////////a été commentée au 3.de la partie 1
    
    SetFreqTimer1(freq);
    IFS0bits.T1IF = 0; // Cle a r Timer I n t e r r u p t Flag
    IEC0bits.T1IE = 1; // Enable Timer i n t e r r u p t
    T1CONbits.TON = 1; // Enable Timer
    
    
    
}


//INIT TIMER 4

void InitTimer4(float freq) {
    //Timer1 pour h o r o d a t e r l e s mesures ( 1ms)
    T4CONbits .TON = 0; // Di s a bl e Timer
    //T1CONbits .TCKPS = 0b01; // P r e s c a l e r                         //////////a été commentée au 3.de la partie 1
    // 11 = 1: 2 5 6 p r e s c a l e v al u e
    // 10 = 1: 6 4 p r e s c a l e v al u e
    // 01 = 1: 8 p r e s c a l e v al u e
    // 00 = 1: 1 p r e s c a l e v al u e


    T4CONbits .TCS = 0; // cl o c k s o u r c e = i n t e r n a l cl o c k
    //PR1 = 40000000/64/50;                                                //////////a été commentée au 3.de la partie 1
    
    SetFreqTimer4(freq);
    IFS1bits.T4IF = 0; // Cle a r Timer I n t e r r u p t Flag
    IEC1bits.T4IE = 1; // Enable Timer i n t e r r u p t
    T4CONbits.TON = 1; // Enable Timer
     
}




// I n t e r r u p t i o n du time r 1

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits . T1IF = 0;
    //LED_BLANCHE = !LED_BLANCHE;
    //PWMUpdateSpeed();
    //InitADC1();
    ADC1StartConversionSequence(); 
    //LED_BLEUE=!LED_BLEUE;
}




// I n t e r r u p t i o n du time r 4

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void) {
    IFS1bits . T4IF = 0;
    //LED_BLANCHE = !LED_BLANCHE;
    //PWMUpdateSpeed();
    //InitADC1();
    //ADC1StartConversionSequence(); 
    //LED_BLEUE=!LED_BLEUE;
    timestamp=timestamp+1;
    OperatingSystemLoop();
}





// SET FREQ TIMER 1

void SetFreqTimer1(float freq) {
    T1CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T1CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T1CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T1CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR1 = (int) (FCY / freq / 256);
            } else
                PR1 = (int) (FCY / freq / 64);
        } else
            PR1 = (int) (FCY / freq / 8);
    } else
        PR1 = (int) (FCY / freq);
}


void SetFreqTimer4(float freq) {
    T1CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535) {
        T4CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535) {
            T4CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535) {
                T4CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR1 = (int) (FCY / freq / 256);
            } else
                PR4 = (int) (FCY / freq / 64);
        } else
            PR4 = (int) (FCY / freq / 8);
    } else
        PR4 = (int) (FCY / freq);
}