#ifndef USTV_I2C_H
#define USTV_I2C_H

#define I2C_IDLE 0
#define I2C_START 1
#define I2C_MASTER_TRANSMIT 2
#define I2C_RESTART 3
#define I2C_STOP 4
#define I2C_MASTER_RECEIVE 5
#define I2C_MASTER_ACKNOWLEDGE 6

#define I2C_READ 0
#define I2C_WRITE 1

#define I2C_ACK 0
#define I2C_NACK 1

#define USE_I2C1_CIRCULAR_BUFFER
#define I2C1_CIRCULAR_BUFFER_SIZE 8
#define I2C1_MAX_PAYLOAD_SIZE 255

#include "define.h"


typedef struct I2C_DATA
{
    unsigned char RW;
    unsigned char data[I2C1_MAX_PAYLOAD_SIZE];
    unsigned char length;
}I2CData;
	
void InitI2C1(void);
void I2C1WriteN( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* chaine, unsigned int length );
void I2C1Write1( unsigned char slaveAddress, unsigned char registerAddress, unsigned char data) ;
void I2C1ReadN( unsigned char slaveAddress, unsigned char registerAddress, volatile unsigned char* data, unsigned int length );
void I2C1Read1( unsigned char slaveAddress, unsigned char registerAddress, volatile unsigned char* data );

void IncrementI2C1AntiBlockCounter(void);
void ResetI2C1AntiBlockCounter(void);
void SetI2C1Crash();
void ResetI2CCrash();
unsigned char IsI2CCrashed(void);

void StartI2C1Message(void);
void I2C1ReadNInterrupt( unsigned char slaveAddress, unsigned char registerAddress, volatile unsigned char* data, unsigned int length );
void I2C1WriteNInterrupt( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* data, unsigned int length );

void I2C1WriteToBuffer(I2CData data);
BOOL I2C1IsDataReadyInBuffer(void);
I2CData I2C1ReadFromBuffer(void);

BOOL I2C1IsTransmissionActive(void);
void I2C1TransmissionOperation();

void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void);

#endif
