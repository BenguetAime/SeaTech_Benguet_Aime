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
 * Author: Valentin BARCHASZ
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTLN_CIRCULAR_BUFFER_H
#define	UTLN_CIRCULAR_BUFFER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "UTLN_Typedefs.h"

#define USB_CIRCULAR_BUFFER_SIZE 2048

void USBResetRxBuffer(void);
void USBResetTxBuffer(void);
void USBReadToRxBuffer(void);
BOOL USBIsDataReadyInRxBuffer(void);
unsigned char USBReadFromRxBuffer(void);
void USBWriteToTxBuffer(unsigned char value);
BOOL USBIsDataReadyInTxBuffer(void);
void USBWriteFromTxBuffer(void);
unsigned char USBGetOneFromTxBuffer(void);
unsigned int USBGetNbOfValuesInTxBuffer(void);
unsigned int USBGetRemainingSpaceInTxBuffer(void);

#endif	/* XC_HEADER_TEMPLATE_H */

