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
 * File:  Define.h 
 * Author: Valentin BARCHASZ
 * Comments:
 * Revision history: 1.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _DEFINE_H_
#define	_DEFINE_H_
#include <xc.h> // include processor files - each processor file is guarded.  
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
//#include <libpic30.h>
#include "UTLN_Typedefs.h"
#include "HardwareProfile_CM_V9.h"
#include "UTLN_Flag.h"

#define MAXMEMHEAP 4096

#define M_PI 3.14159265358979323846f
#define M_PI_DIV2 1.570796326794f
#define M_2PI 6.28318530717958647692f
#ifndef DEBUG
    #define DEBUG 0
#endif
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

#define EUROBOT

#define DEBUG_1 1
#define DEBUG_0 0

#define NULLACCU 0

#define MPU9250 1
#define MPU9250_2 2

#define FREQ_ECH_QEI 250.0
#define T_ECH_QEI 0.004f

#ifdef ROBOCUP
    #define POINT_TO_DIST  (4.261590376344e-6) //Validé Valentin reduc (0.127*PI*1.4)/(16*8192) pt.tour-1 (old_Value) 6.0879862519211386304436601207962e-6) 
#endif
#ifdef EUROBOT
    #define POINT_TO_DIST  (1.211037464120243e-06) //Validé Valentin reduc (0.06*PI)/(19*8192) pt.tour-1 (old_Value) 6.0879862519211386304436601207962e-6) 
#endif
/*******************************************************************************
 * PORT MAPPING Values
 ******************************************************************************/
#define INPUT 1
#define OUTPUT 0
#define ON 1
#define OFF 0
#define LOW 0
#define HIGH 1
#define TRUE 1
#define FALSE 0
#define CLEAR 0
#define RESET 0
#define UNUSED(expr) do { (void)(expr); } while (0)

/*******************************************************************************
 * FRAMES IDs
 ******************************************************************************/


#define TRAME_ACCEL_DATA 0xA0

/*******************************************************************************
 * FRAMES GLOBALS PARAMETERS
 ******************************************************************************/
#define NB_ACCEL_PACKET_BY_TRAME 8
#define ACCEL_PACKET_SIZE       10
#define NB_MAGNETO_PACKET_BY_TRAME 4
#define MAGNETO_PACKET_SIZE     10
#define NB_GYRO_PACKET_BY_TRAME 4
#define GYRO_PACKET_SIZE 10

#define BUFFER_TX_UART_SIZE 128

/*******************************************************************************
 * MACROS
 ******************************************************************************/
#define CLRWDT()  asm(" clrwdt")
#define BUILD_UINT16(hiByte, loByte) \
          ((unsigned short int)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
#define BUILD_INT16(hiByte, loByte) \
          ((int)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
#define MSB_UINT16(a) (((a) >> 8) & 0xFF)
#define LSB_UINT16(a) ((a) & 0xFF)
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((unsigned long)((unsigned long)((Byte0) & 0x00FF) \
          + ((unsigned long)((Byte1) & 0x00FF) << 8) \
          + ((unsigned long)((Byte2) & 0x00FF) << 16) \
          + ((unsigned long)((Byte3) & 0x00FF) << 24)))
#define BUILD_INT32(Byte0, Byte1, Byte2, Byte3) \
          ((long)((long)((Byte0) & 0x00FF) \
          + ((long)((Byte1) & 0x00FF) << 8) \
          + ((long)((Byte2) & 0x00FF) << 16) \
          + ((long)((Byte3) & 0x00FF) << 24)))
#define BREAK_UINT32( var, ByteNum ) \
          (unsigned char)((unsigned int)(((var) >>((ByteNum) * 8)) & 0x00FF))
#define MSB_LSB_INVERT(uShort) (unsigned short)(((((unsigned short)uShort) & 0x00FF)<<8) | (((unsigned short)uShort) & 0xFF00)>>8)
#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif
#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif
#ifndef ABS
#define ABS(x) ((x>0) ? x : (-x))
#endif
#ifndef ABS_Q16
#define ABS_Q16(x) ((_itofQ16(x)>0) ? x : _Q16neg(x))
#endif
#ifndef CEILING
#define CEILING(x) ((int)x == (double)x) ? (int)x : ((int)x) + 1;
#endif
#ifndef DegreeToRadian
#define DegreeToRadian(value) ((double)value * M_PI / 180.0)
#endif

#define MotorIndex(x) (x-1)


/*******************************************************************************
 * DEFINE neccessaire a la fonction AWAITING
 ******************************************************************************/
#define MAX_DELAY_TIMOUT 50
#define CC2530_SRSP_TIMOUT 150
#define UTLN_SRSP_TIMOUT 250
#define CC2530_AREQ_TIMOUT 1000
#define CC2530_BROADCAST_TIMOUT 1000
#define UART_TX_TIMOUT 1000

/*******************************************************************************
 * STRUCTURES PROPRIETAIRES
 ******************************************************************************/
#define BALISAGE_SIZE 2


//Macro permettant de desactiver rapidement (1 cycle) toutes les interruption de niveau <= a 6
//Ces macros sont tres importantes pour le fonctionnement du OSAL (Permet d'etre certain qu'aucun acces concurrants a la memoire seront executes)
#define HAL_ENTER_CRITICAL_SECTION() __asm__ volatile("disi #0x3FFF"); /* disable interrupts */
#define HAL_EXIT_CRITICAL_SECTION() __asm__ volatile("disi #0x0000"); /* enableable interrupts */

#define HAL_ASSERT(expr)  if(!( expr )) while(1);       //Permet de bloquer si expr est vrai

/*** Generic Status Return Values ***/
#define SUCCESS                   0x00
#define FAILURE                   0x01
#define INVALIDPARAMETER          0x02
#define INVALID_TASK              0x03
#define MSG_BUFFER_NOT_AVAIL      0x04
#define INVALID_MSG_POINTER       0x05
#define INVALID_EVENT_ID          0x06
#define INVALID_INTERRUPT_ID      0x07
#define NO_TIMER_AVAIL            0x08
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define INVALID_MEM_SIZE          0x0B
#define NV_BAD_ITEM_LEN           0x0C



#endif	/* _DEFINE_H_ */

