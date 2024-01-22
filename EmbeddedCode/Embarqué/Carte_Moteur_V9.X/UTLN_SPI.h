#ifndef UTLN_SPI_H
#define UTLN_SPI_H

#include "define.h"

#define MAX_SPI1_SS (8+1)
#define MAX_SPI2_SS (4+1)
/*******************************************************************************
 * CONFIG DU SPI EN FONCTIONS DE LA PLATEFORME (MACRO)
 ******************************************************************************/

typedef struct SPI_SS_CONFIG{
  volatile unsigned int * port;
  unsigned char pinNumber;
  }SPI_SS_CONFIG;
  
/****************************************************************************************************/
// Déclarations des prototypes de fonctions
/****************************************************************************************************/
#ifdef USE_SPI1
void SPI1Init(void);

void SPI1RegisterChipSelect(volatile unsigned int* pPort, uint8 pinNum, uint8 device);
void SPI1Invert_CKE(void);
void SPI1Invert_CKP(void);
void SPI1Set_CKP_CKE(int ckp,int cke);
void SPI1SetHighSpeedClk(unsigned char yn,unsigned char device);
void SPI1ChipSelectSetValue( unsigned int value, unsigned int device);


void SPI1WriteSingleCommand( unsigned char dat, unsigned char device );
void SPI1WriteMultipleCommand( unsigned char commands[], int nbCommands, unsigned char device );
void SPI1WriteSingleCommandMultipleRead(unsigned char command, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE);
void SPI1WriteMultipleCommandMultipleRead(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE );
void SPI1WriteSingleCommandPlusNOP( unsigned char dat, unsigned char device );
void SPI1WriteMultipleCommandMultipleReadDelay(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE );
void SPI1WriteSingleCommandDelay( unsigned char command, unsigned char device);
void SPI1WriteMultipleCommandDelay( unsigned char commands[], int nbCommands, unsigned char device );

void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt (void);
#endif

#ifdef USE_SPI2
void SPI2Init(void);
void Spi2InvertCke(void);
void Spi2InvertCkp(void);
void Spi2SetCkpCke(int ckp,int cke);
void Spi2AcquireLock();
void Spi2ReleaseLock();
void SPI2ChipSelectSetValue( unsigned int value, unsigned int device);
void SPI2RegisterChipSelect(volatile unsigned int* pPort, uint8 pinNum, uint8 device);

void WriteSingleCommandSpi2( unsigned char dat, unsigned char device );
void WriteMultipleCommandSpi2( unsigned char commands[], int nbCommands, unsigned char device );
void WriteMultipleCommandSpi2Delay( unsigned char commands[], int nbCommands, unsigned char device );
void WriteSingleCommandPlusNOPSpi2( unsigned char dat, unsigned char device );
void WriteSingleCommandMultipleReadSpi2(unsigned char command, unsigned char results[], int nbResults, unsigned char device);
void SPI2WriteMultipleCommandMultipleRead(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device);
void WriteMultipleCommandMultipleReadSpi2Delay(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device);
void WriteSingleCommandSpi2Delay( unsigned char command, unsigned char device);

void IncrementSpi2AntiBlockCounter(void);
void ResetSpi2AntiBlockCounter(void);
void ResetSpi2LockAntiBlockCounter(void);
void SetSpi2Crash(void);
void ResetSpi2Crash(void);
unsigned char IsSpi2Crashed(void);

void __attribute__((interrupt, no_auto_psv)) _SPI2Interrupt (void);
#endif
#ifdef USE_SPI4
void InitSPI4(void);

void invertSPI4_CKE(void);
void invertSPI4_CKP(void);
void ChipSelectSpi4SetValue( unsigned int value, unsigned char device);
void setSPI4_CKP_CKE(int ckp,int cke);
void SPI1SetHighSpeedClk(unsigned char yn,unsigned char device);

void WriteSingleCommandSpi4( unsigned char dat, unsigned char device );
void WriteMultipleCommandSpi4( unsigned char commands[], int nbCommands, unsigned char device );
void WriteSingleCommandMultipleReadSpi4(unsigned char command, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE);
void WriteMultipleCommandMultipleReadSpi4(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE );
void WriteSingleCommandPlusNOPSpi4( unsigned char dat, unsigned char device );
void WriteMultipleCommandMultipleReadSpi4Delay(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE );
void WriteSingleCommandSpi4Delay( unsigned char command, unsigned char device);
void WriteMultipleCommandSpi4Delay( unsigned char commands[], int nbCommands, unsigned char device );
#endif
#endif

