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
#ifndef UTLN_COIL_GUN_H
#define	UTLN_COIL_GUN_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdbool.h>


typedef struct _TirParameters
{
    bool tirInProgress;
    
    uint8_t numberOfCoilUsed;
    
    uint16_t usOffsetForCoil1;
    uint16_t msOffsetForCoil2;
    uint16_t msOffsetForCoil3;
    uint16_t msOffsetForCoil4;
    
    uint16_t msCoil1;
    uint16_t msCoil2;
    uint16_t msCoil3;
    uint16_t msCoil4;
    
    long tsDebutTirCoil1;
    long tsFinTirCoil1;
    long tsDebutTirCoil2;
    long tsFinTirCoil2;
    long tsDebutTirCoil3;
    long tsFinTirCoil3;
    long tsDebutTirCoil4;
    long tsFinTirCoil4;  
}TirParameters;

//void GenerateTir1Coil(int msCoil1);
//void GenerateTir2Coil(int msCoil1, int msCoil2, int msInterCoil2);
//void GenerateTir3Coil(uint16_t msCoil1, uint16_t msCoil2,  uint16_t msCoil3, uint16_t msInterCoil2,uint16_t msInterCoil3);
void GenerateTir4Coil(uint16_t usCoil1, uint16_t usCoil2,  uint16_t usCoil3, uint16_t usCoil4, uint16_t usInterCoil1, uint16_t usInterCoil2, uint16_t usInterCoil3, uint16_t usInterCoil4);
inline void TirStateMachine(void);
//
//void Coil1StateMachine(void);
//void Coil2StateMachine(void);
//void Coil3StateMachine(void);
//void Coil4StateMachine(void);

#endif	/* XC_HEADER_TEMPLATE_H */

