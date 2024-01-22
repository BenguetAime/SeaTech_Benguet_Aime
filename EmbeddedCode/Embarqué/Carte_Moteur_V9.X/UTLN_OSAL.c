#include "UTLN_OSAL.h"
#include "Define.h"
#include "UTLN_Button.h"
#include "UTLN_Communication.h"
//#include "UTLN_Message.h"
#include "UTLN_Led.h"

#include "UTLN_PWM.h"
#include "main.h"
#include "RobotObj.h"
#include "UTLN_ICM20948.h"
#include "UTLN_ICM20948_REG.h"
#include "UTLN_ICM20948.h"
#include "UTLN_Imu_Processor.h"

#include <stdlib.h>
#include <string.h>


#ifdef USE_OSAL
    //OSAL definition
    const uint8 tasksCnt= USTV_TASK_COUNT;
    uint16 *tasksEvents;
#endif


volatile unsigned char OSALSensorEvent = 0b00000000;
volatile unsigned short OSALSystemEvent = 0b0000000000000000;

void SetOSALSensorEvent(unsigned char evt)
{
    OSALSensorEvent |= evt;
}

void ClearOSALSensorEvent(unsigned char evt)
{
    OSALSensorEvent &= (~evt);
}

BOOL IsOSALSensorEventTriggered(unsigned char evt)
{
    return evt & OSALSensorEvent;
}

void SetOSALSystemEvent(unsigned short evt)
{
    OSALSystemEvent |= evt;
}

void ClearOSALSystemEvent(unsigned short evt)
{
    OSALSystemEvent &= (~evt);
}

BOOL IsOSALSystemEventTriggered(unsigned short evt)
{
    return (evt & OSALSystemEvent);
}


void ProcessOSALMessageSend()
{

}



void ProcessOSALSystemEvent()
{   
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_TIMER_10HZ_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_TIMER_10HZ_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer10HzTimeout=1;
    }
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_TIMER_1HZ_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_TIMER_1HZ_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.flag1Hz=1;
    }
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_VIRTUAL_TIMER_1_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_VIRTUAL_TIMER_1_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer1Timeout=1;
    }
    
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_VIRTUAL_TIMER_2_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_VIRTUAL_TIMER_2_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer2Timeout=1;
    }
    
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_VIRTUAL_TIMER_3_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_VIRTUAL_TIMER_3_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer3Timeout=1;
    }
    
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_VIRTUAL_TIMER_4_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_VIRTUAL_TIMER_4_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer4Timeout=1;
    }
    
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_VIRTUAL_TIMER_5_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_VIRTUAL_TIMER_5_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer5Timeout=1;
    }
    
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_VIRTUAL_TIMER_6_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_VIRTUAL_TIMER_6_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer6Timeout=1;
    }
    
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_VIRTUAL_TIMER_7_TIMOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_VIRTUAL_TIMER_7_TIMOUT);                     //On clear l'event de la tache courante
        flagSystem.virtualTimer7Timeout=1;
    }

    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_TIMER_TIR_1_TIMEOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_TIMER_TIR_1_TIMEOUT);                     //On clear l'event de la tache courante
//        TIR_1=0;
//        _LED_B2=0;_LED_V2=0;_LED_BLE2=0;_LED_R2=0;
    }
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_TIMER_TIR_2_TIMEOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_TIMER_TIR_2_TIMEOUT);                     //On clear l'event de la tache courante
//        TIR_2=0;
//        _LED_B2=0;_LED_V2=0;_LED_BLE2=0;_LED_R2=0;
    }
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_TIMER_TIR_3_TIMEOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_TIMER_TIR_3_TIMEOUT);                     //On clear l'event de la tache courante
//        TIR_3=0;
//        _LED_B2=0;_LED_V2=0;_LED_BLE2=0;_LED_R2=0;
    }
    if(tasksEvents[USTV_SYSTEM_PROCESSING_TASK] & USTV_TIMER_TIR_4_TIMEOUT)
    {
        OSAL_clear_event(USTV_SYSTEM_PROCESSING_TASK,USTV_TIMER_TIR_4_TIMEOUT);                     //On clear l'event de la tache courante
//        TIR_4=0;
//        _LED_B2=0;_LED_V2=0;_LED_BLE2=0;_LED_R2=0;
    }
}

//Fonction appelée chaque milliqseconde
void ProcessOSALSensorEvent()
{
    if(flagSystem.IsSensorOperationEnabled == TRUE)
    {
        #ifdef USE_ICM20948
            if(IsOSALSensorEventTriggered(EVENT_SENSOR_MPU) || _SPI2_INT_IMU==1)
            {                
                ICM20948ReadRegister(INT_STATUS_1, ICM20948_SPICS_ID); //Clear interrupt
                unsigned char* dat=ICM20948GetAccelValues(ICM20948_SPICS_ID);
                unsigned char* dat2=ICM20948GetGyroValues(ICM20948_SPICS_ID);
                int16_t accelTemp[3];
                int16_t gyroTemp[3];
                //DataXYZ accelDataXYZ;
                //DataXYZ gyroDataXYZ;
                accelTemp[0] = ( (int16_t) (dat[0] << 8) | dat[1]);
                accelTemp[1] = ( (int16_t) (dat[2] << 8) | dat[3]);
                accelTemp[2] = ( (int16_t) (dat[4] << 8) | dat[5]);
                gyroTemp[0] = ( (int16_t) (dat2[0] << 8) | dat2[1]);
                gyroTemp[1] = ( (int16_t) (dat2[2] << 8) | dat2[3]);
                gyroTemp[2] = ( (int16_t) (dat2[4] << 8) | dat2[5]);
                
                //Normalisation en m.s-2
                float accelX=(double)accelTemp[0]* ICM20948GetAccelResolutionConstant();
                float accelY=(double)accelTemp[1]* ICM20948GetAccelResolutionConstant();
                float accelZ=(double)accelTemp[2]* ICM20948GetAccelResolutionConstant();
                
                //Normalisation de sensibilité 1000 dps en radians                
                float gyroX=(double)gyroTemp[0]* ICM20948GetGyroResolutionConstant();
                float gyroY=(double)gyroTemp[1]* ICM20948GetGyroResolutionConstant();
                float gyroZ=(double)gyroTemp[2]* ICM20948GetGyroResolutionConstant();
                
                ProcessImuData(accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
                
//                robotState.accelDataXYZ.X=(double)accelTemp[0]* ICM20948GetAccelResolutionConstant();
//                robotState.accelDataXYZ.Y=(double)accelTemp[1]* ICM20948GetAccelResolutionConstant();
//                robotState.accelDataXYZ.Z=(double)accelTemp[2]* ICM20948GetAccelResolutionConstant();
//                
//                //Normalisation de sensibilité 1000 dps en radians                
//                robotState.gyroDataXYZ.X=(double)gyroTemp[0]* ICM20948GetGyroResolutionConstant();
//                robotState.gyroDataXYZ.Y=(double)gyroTemp[1]* ICM20948GetGyroResolutionConstant();
//                robotState.gyroDataXYZ.Z=(double)gyroTemp[2]* ICM20948GetGyroResolutionConstant();
                
                ClearOSALSensorEvent(EVENT_SENSOR_MPU);
            }
        #endif
    }
}


void ProcessOSALUpdateInterfaceState(void)
{
    #ifdef HW_SW1
    if((_SW1 == SW1_ACTIVE_STATE && flagInterface.Sw1LastState == 0) || (flagInterface.Sw1Interrupt == TRUE))
    {
        //On a un appui sur le bouton SW1
        flagInterface.Sw1LastState = 1;
        flagInterface.Sw1Interrupt = FALSE;
        #ifdef BUTTON_V2
            SwPushed(SW1);
        #else
            Sw1Pushed();
        #endif
    }
    else if(_SW1 == !SW1_ACTIVE_STATE && flagInterface.Sw1LastState == 1)
    {
        //On a relaché le bouton SW1
        flagInterface.Sw1LastState = 0;
        #ifdef BUTTON_V2
            SwReleased(SW1);
        #else
            Sw1Released();
        #endif
    }
    #endif

    #ifdef HW_SW2
    if((_SW2 == SW2_ACTIVE_STATE && flagInterface.Sw2LastState == 0) || (flagInterface.Sw2Interrupt == TRUE))
    {
        //On a un appui sur le bouton SW2
        flagInterface.Sw2LastState = 1;
        flagInterface.Sw2Interrupt = FALSE;
        #ifdef BUTTON_V2
            SwPushed(SW2);
        #else
            Sw2Pushed();
        #endif
    }
    else if(_SW2 == !SW2_ACTIVE_STATE && flagInterface.Sw2LastState == 1)
    {
        //On a relaché le bouton SW2
        flagInterface.Sw2LastState = 0;
        #ifdef BUTTON_V2
            SwReleased(SW2);
        #else
            Sw2Released();
        #endif
    }
    #endif

    #ifdef HW_SW3
    if((SW3 == SW3_ACTIVE_STATE && flagInterface.Sw2LastState == 0) || (flagInterface.Sw2Interrupt == TRUE))
    {
        //On a un appui sur le bouton SW2
        flagInterface.Sw3LastState = 1;
        flagInterface.Sw3Interrupt = FALSE;
        Sw3Pushed();
    }
    else if(SW3 == !SW3_ACTIVE_STATE && flagInterface.Sw3LastState == 1)
    {
        //On a relaché le bouton SW2
        flagInterface.Sw3LastState = 0;
        Sw3Released();
    }
    #endif


    #if (defined(HW_SW1) || defined(HW_SW2)|| defined(HW_SW3))
        IsSequenceFinished();
    #endif

}

#ifdef USE_OSAL
void OSALInitTasks( void )
{
  tasksEvents = (uint16 *)OSAL_mem_alloc( sizeof( uint16 ) * tasksCnt);
  OSAL_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));
}
#endif
