/* 
 * File:   ustv_OSAL.h
 * Author: Valentin
 *
 * Created on 25 septembre 2013, 10:29
 */

#ifndef USTV_OSAL_H
#define	USTV_OSAL_H
#include "UTLN_Typedefs.h"
#ifdef	__cplusplus
extern "C" {
#endif
    
#define UTLN_DELAY_BEFORE_SLEEP 1500

//Definition des events OSAL de type Sensor
#define EVENT_SENSOR_MPU 0b00000001
#define EVENT_SENSOR_GPS 0b00000010
#define EVENT_SENSOR_MPU2 0b00000100


//Definition des events OSAL de type System
#define EVENT_SYSTEM_RTCC (unsigned int)(0x0001)
#define EVENT_SYSTEM_AREQ (unsigned int)(0x0001 << 1)
#define EVENT_SYSTEM_SRSP (unsigned int)(0x0001 << 2)
#define EVENT_SYSTEM_SLEEP (unsigned int)(0x0001 << 3)
#define EVENT_SYSTEM_LIGHT_SLEEP (unsigned int)(0x0001 << 4)
#define EVENT_SYSTEM_BLE_INCOMMING_EVENT (unsigned int)(0x0001 << 5)
#define EVENT_SYSTEME_OSAL_TIMER_TIMOUT_EVENT (unsigned int) (0x0001 << 6)


//OSAL TASK COUNT
#define USTV_TASK_COUNT 5
//OSAL TASKs IDs
#define MZ_PROCESSING_TASK 0x00                     //Permet de gerer les messages entrants (PIC32,RS232,)
#define USTV_SYSTEM_PROCESSING_TASK 0x01            //Permet de gerer les timer virtuel, rtcc, ...
#define USTV_SENSOR_PROCESSING_TASK 0x02            //Permet de gerer les interruptions sensors (MPU, GPS)
#define USTV_OUTGOING_MSG_TASK 0x03                 //Permet de gerer l'emission de massage MZ/ BLE/ RS232
#define USTV_APP_TASK           0x04                //Permet de gerer l'application (Ecritures SD, actions sur evenement de bouton x...)(High level task, low priority)

//OSAL SYSTEM Events (event relatifs a la tache system)
#define USTV_TIMER_TIR_1_TIMEOUT (1<<0) 
#define USTV_TIMER_TIR_2_TIMEOUT (1<<1)
#define USTV_TIMER_TIR_3_TIMEOUT (1<<2)
#define USTV_TIMER_TIR_4_TIMEOUT (1<<3)  
#define USTV_TIMER_10HZ_TIMOUT   (1<<4)  
#define USTV_TIMER_1HZ_TIMOUT     (1<<5)
#define USTV_VIRTUAL_TIMER_1_TIMOUT     (1<<8)
#define USTV_VIRTUAL_TIMER_2_TIMOUT     (1<<9)
#define USTV_VIRTUAL_TIMER_3_TIMOUT     (1<<10)
#define USTV_VIRTUAL_TIMER_4_TIMOUT     (1<<11)
#define USTV_VIRTUAL_TIMER_5_TIMOUT     (1<<12)
#define USTV_VIRTUAL_TIMER_6_TIMOUT     (1<<13)
#define USTV_VIRTUAL_TIMER_7_TIMOUT     (1<<14)




extern volatile unsigned long g_longTimeStamp;
extern unsigned char state_display_usb_charge;

void SetOSALSensorEvent(unsigned char evt);
void ClearOSALSensorEvent(unsigned char evt);
BOOL IsOSALSensorEventTriggered(unsigned char evt);
void SetOSALSystemEvent(unsigned short evt);
void ClearOSALSystemEvent(unsigned short evt);
BOOL IsOSALSystemEventTriggered(unsigned short evt);
void ProcessOSALMessageEvent();
void ProcessOSALSensorEvent(void);
void ProcessOSALSystemEvent(void);
void ProcessOSALUpdateInterfaceState(void);
unsigned char GetAvailableSensors(void);
void ProcessOSALMessageSend();
unsigned char IsSrspReceivedOrTimout();
void RTCCProcessAndCommunication(void);
void OSALInitTasks( void );
void AdcRecordTask(void);

#ifdef	__cplusplus
}
#endif

#endif	/* USTV_OSAL_H */

