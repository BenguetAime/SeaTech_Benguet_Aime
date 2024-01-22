/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTLN_OUTPUT_COMPARE_H
#define	UTLN_OUTPUT_COMPARE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define OCPER 20

void InitOuputComparePWM(void);
void InitOutputCompare34(void);
void EnableOutputCompare1PWM(void);
void EnableOutputCompare2PWM(void);
void DisableOutputCompare1PWM(void);
void DisableOutputCompare2PWM(void);
void SetOutputCompare1PWMDutyCycle(uint16_t duty);
void SetOutputCompare2PWMDutyCycle(uint16_t duty);
void SetOutputCompare1PWMPeriod(uint16_t per);
void SetOutputCompare2PWMPeriod(uint16_t per);
void SetOutputCompare34PWMDutyCycleUs(uint32_t dutyUs);
void SetOutputCompare34PWMPeriodUs(uint32_t perUs);
uint32_t GetPeriodeFromFreqOutputCompare34(uint16_t freq);

#endif	/* XC_HEADER_TEMPLATE_H */

