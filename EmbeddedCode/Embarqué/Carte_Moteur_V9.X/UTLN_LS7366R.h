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
 * File: UTLN_LS7366R.h  
 * Author: Valentin B.
 * Comments:
 * Revision history: 1
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTLN_LS7366R_H
#define	UTLN_LS7366R_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "UTLN_Typedefs.h"
//Count modes 
#define NQUAD 0x00                   //non-quadrature mode 
#define QUADRX1 0x01              //X1 quadrature mode 
#define QUADRX2 0x02              //X2 quadrature mode 
#define QUADRX4 0x03              //X4 quadrature mode 
//Running modes 
#define FREE_RUN 0x00 
#define SINGE_CYCLE 0x04 
#define RANGE_LIMIT 0x08 
#define MODULO_N 0x0C 
//Index modes 
#define DISABLE_INDX 0x00         //index_disabled 
#define INDX_LOADC 0x10       

//index_load_CNTR 
#define INDX_RESETC 0x20          //index_rest_CNTR 
#define INDX_LOADO 0x30          //index_load_OL 

#define ASYNCH_INDX 0x00        //asynchronous index 
#define SYNCH_INDX 0x80          //synchronous index 
//Clock filter modes 
#define FILTER_1 0x00            //filter clock frequncy division factor 1 
#define FILTER_2 0x80            //filter clock frequncy division factor 2 
/* **MDR1 configuration data; any of these*** 
  ***data segments can be ORed together***/ 
//Fl
#define NO_FLAGS 0x00 //all flags disabled
#define IDX_FLAG 0x10  //IDX flag 
#define CMP_FLAG 0x20  //CMP flag 
#define BW_FLAG 0x40   //BW flag 
#define CY_FLAG 0x80  //CY flag 
//1 to 4 bytes data-width 
#define BYTE_4 0x00  //four byte mode           
#define BYTE_3 0x01  //three byte mode           
#define BYTE_2 0x02  //two byte mode           
#define BYTE_1 0x03  //one byte mode  
//Enable/disable counter  
#define EN_CNTR 0x00               //counting enabled 
#define DIS_CNTR 0x04              //counting disabled 
/* LS7366R op-code list */ 
#define CLR_MDR0 0x08    
#define CLR_MDR1 0x10 
#define CLR_CNTR 0x20 
#define CLR_STR 0x30 
#define READ_MDR0 0x48 
#define READ_MDR1 0x50 
#define READ_CNTR   0x60 
#define READ_OTR    0x68 
#define READ_STR    0x70 
#define WRITE_MDR1 0x90 
#define WRITE_MDR0 0x88 
#define WRITE_DTR   0x98 
#define LOAD_CNTR   0xE0    
#define LOAD_OTR    0xE4    

bool LS7366Init(uint8 counterID);
int32 LS7366ReadCnt(uint8 counterID);
uint8 LS7366ReadStatus(uint8 counterID);
void LS7366ClearCounter(uint8 counterID);
void LS7366WriteDTR32bits(uint32 value,uint8 counterID);

#endif	/* UTLN_LS7366R_H */

