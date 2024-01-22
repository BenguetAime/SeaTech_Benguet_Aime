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
 * File: UTLN_CAN.h  
 * Author: Valentin B.
 * Comments:
 * Revision history: V1.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTLN_CAN_H
#define	UTLN_CAN_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define SPEEDPERCENT_TO_CAN_RATIO (-163.84)
void CAN1_Init(void);
void CAN1_Stop(void);
void CAN_SendStdFrame(uint16_t sid,uint8_t payload[], uint8_t payloadLength);
void CAN_SetAcceptanceFilter0(uint16_t sid);
void CAN_SetAcceptanceFilter1(uint16_t sid);
void CAN_SetAcceptanceFilter2(uint16_t sid);
void CAN_SetAcceptanceFilter3(uint16_t sid);
void CAN1_StartTransmissions(void);
void CAN_ReadValuesFromBuffer0(void);
void CAN_ReadValuesFromBuffer1(void);
void CAN_ReadValuesFromBuffer2(void);
void CAN_ReadValuesFromBuffer3(void);
void CAN_UpdateSpeed(void);

#endif	/* XC_HEADER_TEMPLATE_H */

