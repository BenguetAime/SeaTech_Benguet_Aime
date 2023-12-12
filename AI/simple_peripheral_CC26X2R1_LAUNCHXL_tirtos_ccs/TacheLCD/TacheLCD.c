/*
 * TacheLCD.c
 *
 *  Created on: 10 févr. 2020
 *      Author: Sebastian MARZETTI
 */
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

#include <TacheADC/TacheADC.h>
#include <TacheLCD/TacheLCD.h>
#include "TacheLCD/LCD_LAUNCHPAD/OLED_Display.h"
//#include <GPIO_Drivers/PIN_SMIOT.h>

//#include <board.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
//#include <ti/display/lcd/LCDDogm1286.h>
#include <ti/display/Display.h>

#include <TacheLCD/SPISMIOT/SPISMIOT.h>
//#include <ti/display/DisplayDogm1286.h>

#include <TacheLCD/LCD_LAUNCHPAD/LCD_LAUNCHPAD.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* Driver configuration */
#include "ti_drivers_config.h"

// Task configuration
#define TacheLCD_TASK_PRIORITY                     1

#ifndef TacheLCD_TASK_STACK_SIZE
#define TacheLCD_TASK_STACK_SIZE                   1024
#endif

// Task configuration
Task_Struct TacheLCD;


uint8_t TacheLCDStack[TacheLCD_TASK_STACK_SIZE];

Semaphore_Struct semLCDStruct;
Semaphore_Handle semLCDHandle;

extern void TacheLCD_init(void);
void LCD_Init(void);
void floatToString(char* ax, float AX);
void floatToString1d(char* ax, float AX);
void intToString(char* ax, float AX);
float Ax,Ay,Az;
float featuresToPrint[6];
int state;
int lastClusterDetectedToPrint;
float nClusters, nAnomalies;
bool firstTime = true;


//LCD_Buffer LCD_Buffer1;
//LCD_Params LCD_Params1;
//uint8_t    nBuffers1;
//LCD_Handle LCD_config;
Display_Handle display_handle1;
Display_Params display_params1;

void afficherDonnées(float accx, float accy, float accz){
    Ax = accx;
    Ay = accy;
    Az = accz;
    Semaphore_post(semLCDHandle);
}


void TacheLCD_CreateTask(void){

    Semaphore_Params semLCDParams;
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = TacheLCDStack;
    taskParams.stackSize = TacheLCD_TASK_STACK_SIZE;
    taskParams.priority = TacheLCD_TASK_PRIORITY;

    Task_construct(&TacheLCD, TacheLCD_taskFxn, &taskParams, NULL);

    /* Construct a Semaphore object to be used as a resource lock, inital count 0 */
        Semaphore_Params_init(&semLCDParams);
        Semaphore_construct(&semLCDStruct, 0, &semLCDParams);
    /* Obtain instance handle */
        semLCDHandle = Semaphore_handle(&semLCDStruct);
}

static void TacheLCD_taskFxn(UArg a0, UArg a1)
{
    // Initialize application
    TacheLCD_init();
    char stateTraining[] = "Training ";
    char stateDetection[] = "Detection";
    char stringClusterNumber[] = "Clusters";
    char stringAnomaliesNumber[] = "Anomaly";

    Fill_LCD(0xFF,0x00,0x00);//RGB
    OLEDText22( 20, 7, stateTraining, SIZE_TWO, 0x00, 0xFF, 0xFF );
    OLEDText22( 5, 30, stringClusterNumber, SIZE_ONE, 0x00, 0xFF, 0xFF );
    OLEDText22( 5, 42, stringAnomaliesNumber, SIZE_ONE, 0x00, 0xFF, 0xFF );

    // Application main loop
    for (;;)
    {
        //This semaphore is post by the interrupts of the SPI devices (IMU, accelerometer, gyro, ECG)
        Semaphore_pend(semLCDHandle, BIOS_WAIT_FOREVER);

          /*if(state){
              OLEDText22( 20, 7, stateTraining, SIZE_TWO, 0x00, 0xFF, 0xFF );
          }
          else{
              OLEDText22( 20, 7, stateDetection, SIZE_TWO, 0x00, 0xFF, 0xFF );
          }

          if(lastClusterDetectedToPrint>=0){
              char detectedCluster[3] = "";
              intToString(detectedCluster,lastClusterDetectedToPrint);
              OLEDText22( 64, 50, detectedCluster, SIZE_THREE, 0x00,  0xFF, 0xFF );
          }
          else{
              char detectedCluster[3] = "A ";
              OLEDText22( 64, 50, detectedCluster, SIZE_THREE, 0x00,  0xFF, 0xFF );
          }

          char numberCluster[3] = " ";
          char numberAnomalies[3] = " ";

          intToString(numberCluster,nClusters);
          intToString(numberAnomalies,nAnomalies);

          OLEDText22( 45, 30, numberCluster, SIZE_ONE, 0x00,  0xFF, 0xFF );
          OLEDText22( 45, 42, numberAnomalies, SIZE_ONE, 0x00,  0xFF, 0xFF );*/

          //Frequency
          char feature0[5] = " ";
          char feature2[5] = " ";
          char feature4[5] = " ";

          floatToString(feature0, featuresToPrint[0]);
          floatToString(feature2, featuresToPrint[2]);
          floatToString(feature4, featuresToPrint[4]);

          OLEDText22( 15, 90, feature0, SIZE_ONE, 0x00,  0xFF, 0xFF );
          OLEDText22( 45, 90, feature2, SIZE_ONE, 0x00,  0xFF, 0xFF );
          OLEDText22( 75, 90, feature4, SIZE_ONE, 0x00,  0xFF, 0xFF );

          //Magnitude
          char feature1[5] = " ";
          char feature3[5] = " ";
          char feature5[5] = " ";

          floatToString(feature1, featuresToPrint[1]);
          floatToString(feature3, featuresToPrint[3]);
          floatToString(feature5, featuresToPrint[5]);

          OLEDText22( 15, 102, feature1, SIZE_ONE, 0x00,  0xFF, 0xFF );
          OLEDText22( 45, 102, feature3, SIZE_ONE, 0x00,  0xFF, 0xFF );
          OLEDText22( 75, 102, feature5, SIZE_ONE, 0x00,  0xFF, 0xFF );


      }

//      char ax2[6] = " ";
//      char ay2[6] = " ";
//      char az2[6] = " ";
//
//      floatToString(ax2,Ax);
//      floatToString(ay2,Ay);
//      floatToString(az2,Az);

//      OLEDText( 38, 8, ax2, SIZE_ONE, 0x00,  0xFF, 0xFF );
//      OLEDText( 38, 33, ay2, SIZE_ONE, 0x00,  0xFF, 0xFF );
//      OLEDText( 38, 58, az2, SIZE_ONE, 0x00,  0xFF, 0xFF );
//      OLEDText22( 38, 8, ax2, SIZE_TWO, 0x00,  0xFF, 0xFF );
//      OLEDText22( 38, 33, ay2, SIZE_TWO, 0x00,  0xFF, 0xFF );
//      OLEDText22( 38, 58, az2, SIZE_TWO, 0x00,  0xFF, 0xFF );
//      Task_sleep(10000/Clock_tickPeriod);// Delay 10ms
//      Task_sleep(5000/Clock_tickPeriod);// Delay 5ms
//        OLEDText( 38, 8, ax2, SIZE_ONE, 0xFF,  0xFF, 0x00 );
//        OLEDText( 38, 33, ay2, SIZE_ONE, 0xFF,  0xFF, 0x00 );
//        OLEDText( 38, 58, az2, SIZE_ONE, 0xFF,  0xFF, 0x00 );

//      OLEDText2( 38, 8, ax2, SIZE_ONE, 0xFF,  0xFF, 0x00, 0x00,0x00,0xFF);
//      OLEDText2( 38, 33, ay2, SIZE_ONE, 0xFF,  0xFF, 0x00, 0x00,0x00,0xFF);
//      OLEDText2( 38, 58, az2, SIZE_ONE, 0xFF,  0xFF, 0x00, 0x00,0x00,0xFF);
    }




void TacheLCD_init(void){
//    PIN_SMIOT_init();
    SPI_SMIOT_Init();
    LCD_Init();
//    Display_Handle display_handle1;
//    Display_Params display_params1;
//    display_handle1 = Display_open(Display_Type_HOST, &params);
//     // Output to Display
//    Display_printf(display_handle1, 0, 0, "Hello World");
//    Display_close(display_handle1);
//    va_list vaa;
//    char TestLCD[6] = "HOLA";
//    DisplayDogm1286_init(display_handle1);
//    Display_Params_init(&display_params1);
//    display_handle1 = DisplayDogm1286_open(display_handle1, &display_params1);
//    DisplayDogm1286_vprintf(display_handle1, 10,
//                                 10, TestLCD,vaa);
//  while(1);

}

void LCD_Init(void){
    //Initialize the LCD controller
    Initialize_LCD();

    //Fill display with a given RGB value
    Fill_LCD(0xFF,0x00,0x00);//RGB
    Task_sleep(500000/Clock_tickPeriod);// Delay 100ms

    char DataLCD[] = "UTLN";
    char DataLCD2[] = "AI";
    char DataLCD3[] = "Push button S2 to train";

    OLEDText22( 40, 17, DataLCD, SIZE_TWO, 0x00, 0xFF, 0xFF );
    OLEDText22( 52, 34, DataLCD2, SIZE_TWO, 0x00, 0xFF,  0xFF );
    OLEDText22( 5, 100, DataLCD3, SIZE_ONE, 0x00, 0xFF,  0xFF );
    //Task_sleep(1000000/Clock_tickPeriod);// Delay 1s
}

void floatToString(char* ax, float AX){
    char convert[6];
    //char* ax = " ";
    if(AX<0){
        char moins[6] = "-";
        strcat(ax,moins);
        AX = AX*-1.0f;
    }
    char point[6] = ".";
    long Axlong = (long)AX;
    long Axdeclong = (long)((AX - (float)Axlong)*10);
    long Axcentlong = (long)(((AX - (float)Axlong)*10 - (float)Axdeclong)*10);
    ltoa(Axlong,convert,10);
    strcat(ax,convert);
    strcat(ax,point);
    ltoa(Axdeclong,convert,10);
    strcat(ax,convert);
    ltoa(Axcentlong,convert,10);
    strcat(ax,convert);
}

void floatToString1d(char* ax, float AX){
    char convert[6];
    //char* ax = " ";
    if(AX<0){
        char moins[6] = "-";
        strcat(ax,moins);
        AX = AX*-1.0f;
    }
    char point[6] = ".";
    long Axlong = (long)AX;
    long Axdeclong = (long)((AX - (float)Axlong)*10);
    ltoa(Axlong,convert,10);
    strcat(ax,convert);
    strcat(ax,point);
    ltoa(Axdeclong,convert,10);
    strcat(ax,convert);
}

void intToString(char* ax, float AX){
    char convert[6];
    //char* ax = " ";
    if(AX<0){
        char moins[6] = "-";
        strcat(ax,moins);
        AX = AX*-1.0f;
    }
    long Axlong = (long)AX;
    ltoa(Axlong,convert,10);
    strcat(ax,convert);
}


void LCD_PrintState(int State, float numClusters, int clusterDetected, float numAnomalies, float features[], int sizeFeatures){
    state = State;
    nClusters = numClusters;
    nAnomalies = numAnomalies;
    lastClusterDetectedToPrint = clusterDetected;
    for(int i=0; i<sizeFeatures; i++)
        featuresToPrint[i] = features[i];

    Semaphore_post(semLCDHandle);
}

float uVToG_float(uint32_t dataSampled)
{
    float dataG = ((float)dataSampled - 1650000)/660000;
    return dataG;
}
