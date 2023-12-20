#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <math.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "TacheFFTClassification.h"
#include "FFT.h"
#include "PeakDetector.h"

#define TacheFFTClassification_TASK_PRIORITY 2
#define TacheFFTClassification_TASK_STACK_SIZE 1024


#define FFT_WINDOW_SIZE 256
float FFTSerieReal[FFT_WINDOW_SIZE];
float FFTSerieIm[FFT_WINDOW_SIZE];


Task_Struct TacheFFTClassification;
uint8_t TacheFFTClassificationStack[TacheFFTClassification_TASK_STACK_SIZE];

Semaphore_Struct semTacheFFTClassificationStruct;
Semaphore_Handle semTacheFFTClassificationHandle;

void TacheFFTClassification_CreateTask(void){
    Semaphore_Params semParams;
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = TacheFFTClassificationStack;
    taskParams.stackSize = TacheFFTClassification_TASK_STACK_SIZE;
    taskParams.priority = TacheFFTClassification_TASK_PRIORITY;

    Task_construct(&TacheFFTClassification, TacheFFTClassification_taskFxn,
    &taskParams, NULL);

    /* Construct a Semaphore object to be used as a resource lock, initial count 0*/
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semTacheFFTClassificationStruct, 0, &semParams);
    /* Obtain instance handle */
    semTacheFFTClassificationHandle = Semaphore_handle(&semTacheFFTClassificationStruct);
}

static void TacheFFTClassification_taskFxn(UArg a0, UArg a1)
{
    //Initialisations

    //Boucle infinie
    for (;;)
    {
        Semaphore_pend(semTacheFFTClassificationHandle, BIOS_WAIT_FOREVER);

        //On utilise FFTSerieReal comme serie des parties reelles du signal temporel
        //On utilise FFTSerieIm comme serie des parties imaginaires du signal temporel
        //que l’on met a 0
        for (int i=0; i<256; i++)
        {
            FFTSerieIm[i] = 0;
        }

        //Calcul de la FFT
        fft(FFTSerieReal, FFTSerieIm, 8, 1);

        //Extraction de la norme de la FFT dans FFTDataY
        for (int i=0; i<256; i++)
            FFTSerieReal[i] = sqrtf((FFTSerieReal[i]*FFTSerieReal[i]+FFTSerieIm[i]*FFTSerieIm[i]));

    }
}


void FFTClassificationTrigger(float serie[]){
    //Copie locale des buffer de data pour la FFT
    for (int i=0; i< FFT_WINDOW_SIZE; i++)
    {
    FFTSerieReal[i] = serie[i];
    }
    //Lancement de la tache FFT Classification
    Semaphore_post(semTacheFFTClassificationHandle);
}
