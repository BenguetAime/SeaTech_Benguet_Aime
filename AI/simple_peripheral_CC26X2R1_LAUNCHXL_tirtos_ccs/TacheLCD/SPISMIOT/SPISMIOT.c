/*
 * SPISMIOT.c
 *
 *  Created on: 28 janv. 2020
 *      Author: TP-EO-6
 */

#include <stdbool.h>
#include <string.h>

//#include <TaskSPISensors/SPISensors/SPISensors.h>
#include <TacheLCD/SPISMIOT/SPISMIOT.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <driverlib/ssi.h>

/* Driver configuration */
#include "ti_drivers_config.h"

SPI_Handle      masterSpi;
SPI_Params      spiParams;
SPI_Transaction transaction;
bool            transferOK;

void SPI_SMIOT_Init(void){

      SPI_init();
      /* Open SPI as master (default) */
      SPI_Params_init(&spiParams);
      spiParams.frameFormat = SPI_POL0_PHA0;
      spiParams.bitRate = 20000000;
//      spiParams.dataSize = 8;//16;

      masterSpi = SPI_open(CONFIG_SPI_0, &spiParams);
      if (masterSpi == NULL) {
          //if it doesnt init it waits here
          while (1);
      }

      GPIO_write(SPI_LCD_CS,1);
      GPIO_write(SPI_LCD_RS,1);
}


void SPI_Communication(unsigned char command[],int nbWrite, unsigned char results[], int nbRead, unsigned char csID){
    uint8_t masterTxBuffer3[nbWrite];
    uint8_t masterRxBuffer3[nbRead + nbWrite];
    //uint8_t masterRxBuffer4[nbRead + nbWrite];//Because it also reads MISO during the transmissions
    //masterRxBuffer3 = masterRxBuffer4;
    for(int i=0;i<nbWrite;i++)
        masterTxBuffer3[i] = command[i];


    memset((void *) masterRxBuffer3, 0, nbWrite);
//    switch(csID){
//        case Board_ICM20948_ID :
//                    GPIO_write(Board_SPI_SS_IMU,0);
//            break;
//        case Board_LIS3MDL_ID :
//                    GPIO_write(Board_SPI_SS_MAG,0);
//            break;
//        case Board_LIS3DSH_ID :
//                    GPIO_write(Board_SPI_SS_ACC,0);
//            break;
//        case Board_ADS129X_ID :
//         SSIConfigSetExpClk(SSI1_BASE,48000000,SSI_FRF_MOTO_MODE_1,SSI_MODE_MASTER,1000000,8);
//            //#define SSI_FRF_MOTO_MODE_0     0x00000000  // Moto fmt, polarity 0, phase 0
//            //#define SSI_FRF_MOTO_MODE_1     0x00000002  // Moto fmt, polarity 0, phase 1
//            //#define SSI_FRF_MOTO_MODE_2     0x00000001  // Moto fmt, polarity 1, phase 0
//            //#define SSI_FRF_MOTO_MODE_3     0x00000003  // Moto fmt, polarity 1, phase 1
//            //#define SSI_FRF_TI              0x00000010  // TI frame format
//            //#define SSI_FRF_NMW             0x00000020  // National MicroWire frame format
//            GPIO_write(Board_SPI_SS_ECG,0);
//            break;
//        default :
//        break;
//    }

    GPIO_write(SPI_LCD_CS,0);

        transaction.count = nbWrite + nbRead;
        transaction.txBuf = (void *) masterTxBuffer3;
        transaction.rxBuf = (void *) masterRxBuffer3;

        /* Perform SPI transfer */
        transferOK = SPI_transfer(masterSpi, &transaction);

        for(int i=nbWrite;i<nbWrite + nbRead;i++)
            results[i-nbWrite] = masterRxBuffer3[i];

        GPIO_write(SPI_LCD_CS,1);
//    switch(csID){
//        case Board_ICM20948_ID :
//                    GPIO_write(Board_SPI_SS_IMU,1);
//            break;
//        case Board_LIS3MDL_ID :
//                    GPIO_write(Board_SPI_SS_MAG,1);
//            break;
//        case Board_LIS3DSH_ID :
//                    GPIO_write(Board_SPI_SS_ACC,1);
//            break;
//        case Board_ADS129X_ID :
//            SSIConfigSetExpClk(SSI1_BASE,48000000,SSI_FRF_MOTO_MODE_0,SSI_MODE_MASTER,4000000,8);
//                    GPIO_write(Board_SPI_SS_ECG,1);
//            break;
//        default :
//        break;
//    }
}

