
#ifndef UTLN_ADC_H
#define UTLN_ADC_H

/*********************************************************************
 * INCLUDES
 */
//#include <adc.h>


/*********************************************************************
 * FUNCTIONS
 */
void InitADC1(void);
void ADC1StartConversion(void);
unsigned char ADC1IsConversionFinished( void );
void ADC1ClearConversionFinishedFlag ( void );
unsigned int * ADC1GetResult( void );
void InitADC2(void);
void ADC2StartConversion();
unsigned char ADC2IsConversionFinished( void );
void ADC2ClearConversionFinishedFlag ( void );
unsigned int * ADC2GetResult( void );
//void __attribute__((interrupt, auto_psv)) _DMA0Interrupt (void);

#endif

