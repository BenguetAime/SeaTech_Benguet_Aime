#include "UTLN_OutputCompare.h"
#include "HardwareProfile_CM_V9.h"

//Voir Ref Manual DS70005159A

uint32_t _periodeOC34=0;
//Configuration de l'output compare 1 et 2 pour generation d'une double pwm
void InitOuputComparePWM(void)
{
    OC1CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC1CON2 = 0;
    OC1CON1bits.OCTSEL = 0x07; /* This selects the peripheral clock as the clock input to the OC module */
    OC1R = 1; /* This is just a typical number, user must calculate based on the
                waveform requirements and the system clock */
    OC1RS = 100*OCPER; /* Determines the Period */
    OC1CON2bits.SYNCSEL = 0x1F; /* This selects the synchronization source as itself */
    
    
    //Output Compare 2 synchronisé avec le 1
    OC2CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC2CON2 = 0;
    OC2CON1bits.OCTSEL = 0x07; /* This selects the peripheral clock as the clock input to the OC module */

    OC2R = 1; /* This is just a typical number, user must calculate based on the
                waveform requirements and the system clock */
    
    OC2RS=100*OCPER;
    OC2CON2bits.SYNCSEL = 0x1; /* This selects the synchronization source as OC1 */
}

void InitOutputCompare34(void)
{
    //Initialize OutputCompare 3 & 4 as 32bit outputCompare (Cascade Mode)
    OC3CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC3CON2 = 0;
    OC4CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC4CON2 = 0;
    
    OC3CON1bits.OCTSEL = 0x07; /* This selects the peripheral clock as the clock input to the OC module */
    OC4CON1bits.OCTSEL = 0x07; /* This selects the peripheral clock as the clock input to the OC module */
    OC3R = 1; /* This is just a typical number, user must calculate based on the
                waveform requirements and the system clock */
    OC4R = 1; /* This is just a typical number, user must calculate based on the
                waveform requirements and the system clock */
    OC3RS = 100*OCPER; /* Determines the Period */
    OC4RS = 100*OCPER; /* Determines the Period */
    OC3CON2bits.SYNCSEL = 0x1F; /* This selects the synchronization source as itself */
    OC4CON2bits.SYNCSEL = 0x1F; /* This selects the synchronization source as itself */
    
    OC3CON2bits.OCTRIS = 1; /* Odd module's output is not required */
    /* Even module must be enabled first */
    /* Odd module must be enabled last */
    OC4CON2bits.OC32 = 1;
    OC3CON2bits.OC32 = 1;
    OC4CON1bits.OCM = 6; /* This selects the Edge Aligned PWM mode */
    OC3CON1bits.OCM = 6; /* This starts the cascaded timer */
}

void EnableOutputCompare1PWM(void)
{
    OC1CON1bits.OCM = 6; /* This selects and starts the Edge Aligned PWM mode*/
}

void EnableOutputCompare2PWM(void)
{
    OC2CON1bits.OCM = 6; /* This selects and starts the Edge Aligned PWM mode*/
}

void EnableOutputCompare34PWM(void)
{
    OC4CON1bits.OCM = 6; /* This selects the Edge Aligned PWM mode */
    OC3CON1bits.OCM = 6; /* This starts the cascaded timer */
}

void DisableOutputCompare1PWM(void)
{
    OC1CON1bits.OCM = 0;
}

void DisableOutputCompare2PWM(void)
{
    OC2CON1bits.OCM = 0;
}

void DisableOutputCompare34PWM(void)
{
    OC4CON1bits.OCM = 0; /* This selects the Edge Aligned PWM mode */
    OC3CON1bits.OCM = 0; /* This starts the cascaded timer */
}
    
//Set Duty Cycle in Clock cycle
void SetOutputCompare1PWMDutyCycle(uint16_t duty)
{
    OC1R = duty;
}

//Set Duty Cycle in Clock cycle
void SetOutputCompare2PWMDutyCycle(uint16_t duty)
{
    OC2R = duty;
}

//Set Duty Cycle in Clock cycle
void SetOutputCompare34PWMDutyCycleUs(uint32_t dutyUs)
{
    uint32_t dutyUsReg=((float)dutyUs/1000000)*FCY;
    //MSB
    OC4R=(dutyUsReg>>16);
    //LSB
    OC3R = (uint16_t)dutyUsReg;
}

//Set Period in Clock cycle
void SetOutputCompare1PWMPeriod(uint16_t per)
{
    OC1RS = per;
}

//Set Period in Clock cycle
void SetOutputCompare2PWMPeriod(uint16_t per)
{
    OC2RS = per;
}

//Set Period in Clock cycle
void SetOutputCompare34PWMPeriod(uint32_t per)
{
    _periodeOC34=per;
    //MSB
    OC4RS=(per>>16);
    //LSB
    OC3RS = (uint16_t)per;
}
//Set Period in Us
void SetOutputCompare34PWMPeriodUs(uint32_t perUs)
{
    uint32_t periodeUsReg=((float)perUs/1000000)*FCY;
    //MSB
    OC4RS=(periodeUsReg>>16);
    //LSB
    OC3RS = (uint16_t)periodeUsReg;
}
uint32_t GetPeriodeFromFreqOutputCompare34(uint16_t freq)
{
    return (FCY/freq);
}

