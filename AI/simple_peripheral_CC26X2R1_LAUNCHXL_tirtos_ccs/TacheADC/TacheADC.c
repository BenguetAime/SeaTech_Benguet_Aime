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

    for (;;)
    {
        Semaphore_pend(semTacheADCHandle, BIOS_WAIT_FOREVER);
        uint32_t DatasampledX = Sampling(CONFIG_ADC_0);
        uint32_t DatasampledY = Sampling(CONFIG_ADC_1);
        uint32_t DatasampledZ = Sampling(CONFIG_ADC_2);
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
