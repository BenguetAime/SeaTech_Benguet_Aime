#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <TacheADC/TacheADC.h>
#include <TacheLCD/TacheLCD.h>
#include <Filters/Filter.h>
#include <TacheFFTClassification/TacheFFTClassification.h>

/* Driver Header files */
#include <ti/drivers/ADC.h>
/* Driver configuration */
#include "ti_drivers_config.h"

#define TacheADC_TASK_PRIORITY 3
#define TacheADC_TASK_STACK_SIZE 1024
Task_Struct TacheADC;
uint8_t TacheADCStack[TacheADC_TASK_STACK_SIZE];
Semaphore_Struct semTacheADCStruct;
Semaphore_Handle semTacheADCHandle;
static Clock_Struct myClock;

Order1Filter LPFilterAccelX;
Order1Filter LPFilterAccelY;
Order1Filter LPFilterAccelZ;

Order1Filter HPFilterAccelX;
Order1Filter HPFilterAccelY;
Order1Filter HPFilterAccelZ;

Order1Filter HPFilterAccelNorme;
#define FFT_WINDOW_SIZE 256
float SerieNormeAccel[FFT_WINDOW_SIZE];
int indexFFT;

void myClockSwiFxn(uintptr_t arg0)
{
    Semaphore_post(semTacheADCHandle);
}


void TacheADC_taskFxn(UArg a0, UArg a1)
{
    // Declaration d’une structure clock_Params
    Clock_Params clockParams;
    // Initialisation de la structure
    Clock_Params_init(&clockParams);
    // Reglage de la periode a 10 ms
    clockParams.period = 10 * (1000/Clock_tickPeriod);
    // Initialisation du timer (Clock en RTOS)
    Clock_construct(&myClock, myClockSwiFxn, 0, &clockParams);
    //Lancement du timer
    Clock_start(Clock_handle(&myClock));

    //Initialisation du module ADC
    ADC_init();

    // Initialisation des filtres
    InitOrder1LPFilterEuler(&LPFilterAccelX, 1, 100);
    InitOrder1LPFilterEuler(&LPFilterAccelY, 1, 100);
    InitOrder1LPFilterEuler(&LPFilterAccelZ, 1, 100);

    InitOrder1HPFilterEuler(&HPFilterAccelX, 1, 100);
    InitOrder1HPFilterEuler(&HPFilterAccelY, 1, 100);
    InitOrder1HPFilterEuler(&HPFilterAccelZ, 1, 100);

    for (;;)
    {
        Semaphore_pend(semTacheADCHandle, BIOS_WAIT_FOREVER);
        uint32_t DatasampledX = Sampling(CONFIG_ADC_0);
        uint32_t DatasampledY = Sampling(CONFIG_ADC_1);
        uint32_t DatasampledZ = Sampling(CONFIG_ADC_2);

        float xG=uVToG_float(DatasampledX);
        float yG=uVToG_float(DatasampledY);
        float zG=uVToG_float(DatasampledZ);

        /*float features[6];
        features[0]= xG;
        features[1]= 0;
        features[2]= yG;
        features[3]= 0;
        features[4]= zG;
        features[5]= 0;
        LCD_PrintState(0, 0, 0, 0, features, 6);*/

        //Filtre passe-bas sur les 3 axes
        /*float AccelLPX = ComputeOrder1Filter(&LPFilterAccelX, xG);
        float AccelLPY = ComputeOrder1Filter(&LPFilterAccelY, yG);
        float AccelLPZ = ComputeOrder1Filter(&LPFilterAccelZ, zG);
        float features[6];
        features[0]= AccelLPX;
        features[1]= 0;
        features[2]= AccelLPY;
        features[3]= 0;
        features[4]= AccelLPZ;
        features[5]= 0;
        LCD_PrintState(0, 0, 0, 0, features, 6);*/

        //Filtre passe-haut sur les 3 axes
        float AccelHPX = ComputeOrder1Filter(&HPFilterAccelX, xG);
        float AccelHPY = ComputeOrder1Filter(&HPFilterAccelY, yG);
        float AccelHPZ = ComputeOrder1Filter(&HPFilterAccelZ, zG);
        float features[6];
        features[0]= AccelHPX;
        features[1]= 0;
        features[2]= AccelHPY;
        features[3]= 0;
        features[4]= AccelHPZ;
        features[5]= 0;
        LCD_PrintState(0, 0, 0, 0, features, 6);

        float normeAccel = sqrtf(AccelHPX*AccelHPX+AccelHPY*AccelHPY+AccelHPZ*AccelHPZ);
        float normeAccelHP = ComputeOrder1Filter(&HPFilterAccelNorme, normeAccel);
        SerieNormeAccel[indexFFT] = normeAccelHP;
        indexFFT++;
        if(indexFFT>=FFT_WINDOW_SIZE)
        {
        //On lance la tache de calcul de la FFT et classification
        FFTClassificationTrigger(SerieNormeAccel);
        //Le resultat est recupere dans DataYFFT
        indexFFT = 0;
        }
    }
}

void TacheADC_CreateTask(void){
    Semaphore_Params semParams;
    Task_Params taskParams;
    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = TacheADCStack;
    taskParams.stackSize = TacheADC_TASK_STACK_SIZE;
    taskParams.priority = TacheADC_TASK_PRIORITY;
    Task_construct(&TacheADC, TacheADC_taskFxn, &taskParams, NULL);
    /* Construct a Semaphore object
    to be used as a resource lock, initial count 0 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semTacheADCStruct, 0, &semParams);
    /* Obtain instance handle */
    semTacheADCHandle = Semaphore_handle(&semTacheADCStruct);
}


uint32_t Sampling(uint_least8_t Board_ADC_Number){
    ADC_Handle adc;
    ADC_Params params;
    ADC_Params_init(&params);
    uint16_t adcValue;
    uint32_t adcValue1MicroVolt;
    adc = ADC_open(Board_ADC_Number, &params);
    ADC_convert(adc, &adcValue);
    adcValue1MicroVolt = ADC_convertRawToMicroVolts(adc, adcValue);
    ADC_close(adc);
    return adcValue1MicroVolt;
}
