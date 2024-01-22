#include "utln_adc.h"
#include "define.h"

unsigned char ADCResultIndex = 0 ;
volatile unsigned int ADC1Result[11] ;
unsigned char ADC1ConversionFinishedFlag ;
static unsigned int ADC2Result[9] ;
unsigned char ADC2ConversionFinishedFlag ;
/****************************************************************************************************/
// Configuration ADC
/****************************************************************************************************/

void InitADC1(void)
{
    // GLOBAL SETTINGS
    AD1CON1=0;
    AD1CON2=0;
    AD1CON3=0;

    //ADCON1
    AD1CON1bits.ADON = 0;                    //A/D Converter is disabled
    AD1CON1bits.ADSIDL = 0;                  //Continue in idle mode
    AD1CON1bits.FORM = 0b0000;               //result format : Integer (0000 dddd ddd ddd)
    AD1CON1bits.AD12B= 0;                   //10-bit operation
    AD1CON1bits.SSRC = 0b111;                   //Clearing the SAMP bit ends sampling and starts conversion
    AD1CON1bits.ASAM = 1;                  //0 = Sampling begins when the SAMP bit is set

    //ADCON2
    AD1CON2bits.VCFG = 0b000;
    AD1CON2bits.BUFM = 0;                  // Buffer is configured as one 16-word buffer (ADC1BUFn<15:0>)
    #ifdef USE_CM_V10
        AD1CON2bits.SMPI = 10;                 // Interrupts are at 10+1 conversion
    #else
        AD1CON2bits.SMPI = 9;                 // Interrupts are at 9+1 conversion
    #endif
    AD1CON2bits.CHPS = 0;
    AD1CON2bits.CSCNA = 1;                //enable channel scan  
    AD1CON2bits.ALTS = 0;                   // 0 = Always uses MUX A input multiplexer settings
    
    
    
    
    AD1CSSHbits.CSS23=1;                    //AN23 (BATT_CMD_VOLTAGE)
    #ifdef USE_CM_V10
        AD1CSSHbits.CSS22=1;                     //AN22 (ANA22-A6)
    #endif
    AD1CSSHbits.CSS21=1;                    //AN21 (Motor2 ISense)
    AD1CSSHbits.CSS20=1;                    //AN20 (Motor1 ISense)
    AD1CSSLbits.CSS7=1;                    //AN7 (BATT_POWER_CURRENT)
    AD1CSSLbits.CSS6=1;                    //AN6 (BATT_POWER_VOLTAGE)
    AD1CSSLbits.CSS5=1;                    //AN5 (Motor3 ISense)
    AD1CSSLbits.CSS4=1;                    //AN4 (Motor4 ISense)
    AD1CSSLbits.CSS3=1;                    //AN3 (Motor5 ISense)
    AD1CSSLbits.CSS2=1;                    //AN2 (Motor6 ISense)
    AD1CSSLbits.CSS1=1;                    //AN1 (BATT_CMD_CURRENT)

    // ADCON3
    AD1CON3bits.ADRC = 0;                    // Conversion clock derived from system clock.
    AD1CON3bits.ADCS = 15;                   // Divide system clock by 16(TAD = 1uS @ 16 MIPS).
    AD1CON3bits.SAMC = 15;//15;                   // Sample time is 15 TAD.

    // ADL0CONH Sample List 0 settings
    AD1CHS0bits.CH0NA = 0;                   // 0 = Channel 0 negative input is VR-
    AD1CHS0bits.CH0SA = 0;                   // 5 = AN5 (MOTOR7 ISense)
    
    AD1CON4bits.ADDMAEN=0;                  //1 = Conversion results are stored in ADCxBUF0 register for transferring to RAM using DMA
                                            //0 = Conversion results are stored in ADCxBUF0 through ADCxBUFF registers; DMA will not be used

    // ENABLE A/D
    IFS0bits.AD1IF = 0;
    IPC3bits.AD1IP = 5;
    IEC0bits.AD1IE = 1;                     //Enable ADC interrupt
    
    //while(ADSTATHbits.ADREADY == 0); // Wait for ready flag set.


    // ENABLE A/D
    AD1CON1bits.ADON = 1; // Enable A/D.
    //AD1CON1bits.SAMP = 1; // Close sample switch.

}
void ADC1StartConversion()
{
    AD1CON1bits.SAMP = 1;                 
}
unsigned int * ADC1GetResult( void )
{
    return (unsigned int *)ADC1Result ;
}
unsigned char ADC1IsConversionFinished( void )
{
    return ADC1ConversionFinishedFlag ;
}
void ADC1ClearConversionFinishedFlag ( void )
{
    ADC1ConversionFinishedFlag = 0 ;
}

void InitADC2(void)
{
    // GLOBAL SETTINGS
    AD2CON1=0;
    AD2CON2=0;
    AD2CON3=0;

    //ADCON1
    AD2CON1bits.ADON = 0;                    //A/D Converter is disabled
    AD2CON1bits.ADSIDL = 0;                  //Continue in idle mode
    AD2CON1bits.FORM = 0b0000;               //result format : Integer (0000 dddd ddd ddd)
//    AD2CON1bits.AD12B=1;                   //12-bit operation
    AD2CON1bits.SSRC = 0b111;                //Clearing the SAMP bit ends sampling and starts conversion
    AD2CON1bits.ASAM = 1;                  //0 = Sampling begins when the SAMP bit is set

    //ADCON2
    AD2CON2bits.VCFG=0b000;
    AD2CON2bits.BUFM = 0;                  // Buffer is configured as one 16-word buffer (ADC1BUFn<15:0>)
    AD2CON2bits.SMPI = 8;                 //0010 = Interrupts are at 8+1 conversion
    AD2CON2bits.CHPS=0;
    AD2CON2bits.CSCNA = 1;                //Enable channel scan  
    AD2CON2bits.ALTS = 0;                   // 0 = Always uses MUX A input multiplexer settings


    // ADCON3
    AD2CON3bits.ADRC = 0;                    // Conversion clock derived from system clock.
    AD2CON3bits.ADCS = 15;                   // Divide system clock by 16(TAD = 1uS @ 16 MIPS).
    AD2CON3bits.SAMC = 15;                   // Sample time is 15 TAD.
    
    //Enable channel scan on Chan:
    AD2CSSLbits.CSS0=1;                    //ANA0_B0
    AD2CSSLbits.CSS8=1;                    //ANA8_B8
    AD2CSSLbits.CSS9=1;                    //ANA9_B9
    AD2CSSLbits.CSS10=1;                    //ANA10_B10
    AD2CSSLbits.CSS11=1;                    //ANA11_B11
    AD2CSSLbits.CSS12=1;                    //ANA_12-B12
    AD2CSSLbits.CSS13=1;                    //ANA_13-B13
    AD2CSSLbits.CSS14=1;                    //ANA_14-B14
    AD2CSSLbits.CSS15=1;                    //ANA_15-B15
    // ADL0CONH Sample List 0 settings
    AD2CHS0bits.CH0NA = 0;                   // 0 = Channel 0 negative input is VR-
    AD2CHS0bits.CH0SA = 0;                   // 0 

    // ENABLE A/D
    IFS1bits.AD2IF = 0;
    IPC5bits.AD2IP = 5;
    IEC1bits.AD2IE = 1;                     //Enable ADC interrupt
    
    //while(ADSTATHbits.ADREADY == 0); // Wait for ready flag set.


    // ENABLE A/D
    AD2CON1bits.ADON = 1; // Enable A/D.

}

void ADC2StartConversion(void)
{
    AD2CON1bits.SAMP = 1;                 
}
unsigned int * ADC2GetResult( void )
{
    return ADC2Result ;
}
unsigned char ADC2IsConversionFinished( void )
{
    return ADC2ConversionFinishedFlag ;
}
void ADC2ClearConversionFinishedFlag ( void )
{
    ADC2ConversionFinishedFlag = 0 ;
}


void __attribute__((interrupt , no_auto_psv))_AD1Interrupt( void )
{
    IFS0bits.AD1IF = 0 ;
    ADC1Result[ 0 ] = ADC1BUF0;
    ADC1Result[ 1 ] = ADC1BUF1;
    ADC1Result[ 2 ] = ADC1BUF2;
    ADC1Result[ 3 ] = ADC1BUF3;
    ADC1Result[ 4 ] = ADC1BUF4;
    ADC1Result[ 5 ] = ADC1BUF5;
    ADC1Result[ 6 ] = ADC1BUF6;
    ADC1Result[ 7 ] = ADC1BUF7;
    ADC1Result[ 8 ] = ADC1BUF8;
    ADC1Result[ 9 ] = ADC1BUF9;
    ADC1Result[ 10 ] = ADC1BUFA;
    
    ADC1ConversionFinishedFlag = 1 ;
}

void __attribute__((interrupt , no_auto_psv))_AD2Interrupt( void )
{
    IFS1bits.AD2IF = 0 ;
    ADC2Result[ 0 ] = ADC2BUF0; // Read the AN?scan input 1 conversionresult
    ADC2Result[ 1 ] = ADC2BUF1; // Read the AN3 conversion result
    ADC2Result[ 2 ] = ADC2BUF2; // Read the AN5 conversion result
    ADC2Result[ 3 ] = ADC2BUF3; // Read the AN3 conversion result
    ADC2Result[ 4 ] = ADC2BUF4; // Read the AN5 conversion result
    ADC2Result[ 5 ] = ADC2BUF1; // Read the AN3 conversion result
    ADC2Result[ 6 ] = ADC2BUF2; // Read the AN5 conversion result
    ADC2Result[ 7 ] = ADC2BUF3; // Read the AN3 conversion result
    ADC2Result[ 8 ] = ADC2BUF4; // Read the AN5 conversion result
    ADC2ConversionFinishedFlag = 1 ;
}





