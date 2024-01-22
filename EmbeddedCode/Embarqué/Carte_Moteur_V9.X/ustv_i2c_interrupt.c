/******************************************************************************
  Filename:       ustv_i2c.c
  Revised:        $Date: 2012-11-19 18:52:21 $
  Revision:       $Revision: 00000 $

  Description:   USTV Functions to drive the I2C Module (I2C Driver)

  Copyright 2012 Université du Sud Toulon Var. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer .You may not use this
  Software unless you agree to abide by the terms of the License. Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact DPT GEII at www.univ-tln.fr.

 ******************************************************************************/
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "ustv_i2c_interrupt.h"
#include "define.h"
#include <Libpic30.h>

#define I2C_TIMEOUT 50

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
volatile unsigned char I2C1State = 0;
volatile I2CData currentI2CMsg;
volatile unsigned char I2C1TrnCounter = 0;
volatile unsigned char I2C1ReceptionState = 0;

volatile unsigned char I2C1CircularBufferHead = 0;
volatile unsigned char I2C1CircularBufferTail = 0;

unsigned int antiBlockCounterI2C1;
unsigned char I2C1Crashed=0;

#ifdef USE_I2C1_CIRCULAR_BUFFER
    I2CData I2C1Value[I2C1_CIRCULAR_BUFFER_SIZE];
#else
#endif


/*******************************************************************************
* FONCTIONS
*******************************************************************************/

/*******************************************************************************
 * @fn      InitI2C1()
 *
 * @brief   Fonction permettant l'initialisation du peripherique I2C.
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void InitI2C1(void)
{
    /* Baud rate is set for 100 kHz */
    double Fscl;
    Fscl= 400000;

    // Configure I2C1CON register
    I2C1CONbits.I2CEN = 0;      //Module I2C arrêté
    I2C_SCLtris &= ~(1U<<I2C_SCLpin);
    _I2C_SCL = 1;
    I2C_SDAtris &= ~(1U<<I2C_SDApin);
    _I2C_SDA = 1;


    I2C1CONbits.I2CSIDL = 0;    // 1 = Discontinue module operation when device enters an Idle mode
                                // 0 = Continue module operation in Idle mode
    I2C1CONbits.SCLREL = 1;     // operating as i2C slave only
                                // 1 = Release SCLx clock
                                // 0 = Hold SCLx clock low (clock stretch)
    I2C1CONbits.IPMIEN = 0;     // 1 = IPMI Support mode is enabled; all addresses Acknowledged
                                // 0 = IPMI Support mode disabled
    I2C1CONbits.A10M = 0;       // 1 = I2CxADD is a 10-bit slave address
                                // 0 = I2CxADD is a 7-bit slave address
    I2C1CONbits.DISSLW = 0;     // 1 = Slew rate control disabled
                                // 0 = Slew rate control enabled
    I2C1CONbits.SMEN = 0;       // 1 = Enable I/O pin thresholds compliant with SMBus specification
                                // 0 = Disable SMBus input thresholds
    I2C1CONbits.GCEN = 0;       // 1 = Enable interrupt when a general call address is received in the I2CxRSR (module is enabled for reception)
                                // 0 = General call address disabled
    I2C1CONbits.STREN = 0;      // 1 = Enable software or receive clock stretching
                                // 0 = Disable software or receive clock stretching
    I2C1CONbits.ACKDT = 1;      // 1 = Send NACK during Acknowledge
                                // 0 = Send ACK during Acknowledge

    //I2C1BRG = config2 = (int)(FCY/Fscl - FCY/1111111)+5; // 30,6 -> 31
    //I2C1BRG = (int)(FCY/Fscl - FCY/10000000-1);  //37.4 -> 37 //cf. datasheet
    //I2C1BRG = 60;
        I2C1BRG = (int)(FCY/Fscl - FCY/10000000-1);  //37.4 -> 37 //cf. datasheet
        ///////I2C1BRG = 34;               // Valeur donnant réellement 400kHz

        //I2C1BRG = 20;               // Valeur donnant réellement 1MHz
    

    //Attention : un minimum de 14 avec des pull-up de 1.2KOhm
    I2C1CONbits.I2CEN = 1;      // Module I2C démarré

    IEC1bits.MI2C1IE = 1;       //On active les interruptions I2C
    I2C1State = I2C_IDLE;
}

//******************************************************************************
//* Function Name: I2CByteRead *
//* Return Value: error condition status *
//* Parameters: EE memory device control, address and data *
//* bytes. *
//* Description: Write single data byte to I2C EE memory *
//* device. This routine can be used for any I2C *
//* EE memory device, which uses 2 bytes of *
//* address data as in the 24LC32A/64A/256A. *
//* *
//******************************************************************************

void I2C1Read1( unsigned char slaveAddress, unsigned char registerAddress, volatile unsigned char* data )
{
    I2C1ReadNInterrupt( slaveAddress, registerAddress, data, 1);
}

void I2C1Write1( unsigned char slaveAddress, unsigned char registerAddress, unsigned char data)
{
    unsigned char tab[1];
    tab[0]=data;
    I2C1WriteNInterrupt( slaveAddress, registerAddress, tab, 1) ;
}

/********************************************************************************************************/
/********************************************************************************************************/
/*                                  I2C en mode interrupt                                               */
/********************************************************************************************************/
/********************************************************************************************************/

void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void)
{
    #ifdef DEBUG_INTERRUPT
        LED1 = DEBUG_1;
    #endif
    IFS1bits.MI2C1IF = 0; // Clear CN interrupt

	//On vérifie que l'on n'a pas une collisison
	if(I2C1STATbits.BCL == 1)
	{
		//On a une collision
		if(I2C1State == I2C_START)
		{
			//La collision est arrivée sur le start
			//On remet les lignes en état IDLE
			I2C1CONbits.I2CEN = 0;      //Module I2C arrêté
            I2C_SCLtris &= ~(1U<<I2C_SCLpin);
            _I2C_SCL = 1;
            I2C_SDAtris &= ~(1U<<I2C_SDApin);
            _I2C_SDA = 1;

			//On clear l'erreur
			I2C1STATbits.BCL = 0;

			while(I2C1State != I2C_IDLE)
			{
				//On attend
				;
			}

			//On relance le start
			I2C1CONbits.SEN = 1;
			return;
		}
	}
	if(I2C1STATbits.IWCOL == 1)
	{
		I2C1STATbits.IWCOL = 0;
		return;
	}
	if(I2C1STATbits.I2COV == 1)
	{
		I2C1STATbits.I2COV = 0;
		return;
	}

    // Insert ISR code here
    switch(I2C1State)
    {
        case I2C_START:
            //On vient de terminer la sequence de start
            I2C1TrnCounter = 0;
            //On passe en transmission
            I2C1State = I2C_MASTER_TRANSMIT;
            I2C1TransmissionOperation();
            break;
        case I2C_MASTER_TRANSMIT:
            I2C1TransmissionOperation();
            break;
        case I2C_RESTART:
            //On vient de terminer la sequence de restart
            I2C1TrnCounter = 0;
            I2C1State = I2C_MASTER_TRANSMIT;
            I2C1TransmissionOperation();
            break;
        case I2C_STOP:
            //Transmission terminée
            I2C1State = I2C_IDLE;
            break;
        default:
            I2C1State = I2C_IDLE;
            break;
    }
    #ifdef DEBUG_INTERRUPT
        LED1 = DEBUG_0;
    #endif
}

void I2C1WriteNInterrupt( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* data, unsigned int length )
{
    I2CData msgCommandData;
            
    msgCommandData.RW = I2C_WRITE;
    msgCommandData.data[0] = slaveAddress & 0xFE;
    msgCommandData.data[1] = registerAddress;

    int i;
    for (i=0;i<length+2;i++)
    {
        msgCommandData.data[2+i] = data[i];
    }
    msgCommandData.length = 2+length;

    // On place les messages dans le buffer
    I2C1WriteToBuffer(msgCommandData);

    // On lance la tranmission si aucune transmission n'est en cours
    if(I2C1IsTransmissionActive()==FALSE)
        StartI2C1Message();

    //On est dans un read, on attend donc le résultat
    ResetI2C1AntiBlockCounter();
    while((I2C1IsTransmissionActive()==TRUE) && (antiBlockCounterI2C1 < I2C_TIMEOUT))
    {
        Nop();
    }
    if(antiBlockCounterI2C1 >= I2C_TIMEOUT)
    {
        SetI2C1Crash();
    }
}

void I2C1ReadNInterrupt( unsigned char slaveAddress, unsigned char registerAddress, volatile unsigned char* data, unsigned int length )
{
    I2CData msgCommand;
    I2CData msgData;

    msgCommand.RW = I2C_WRITE;
    msgCommand.data[0] = slaveAddress & 0xFE;
    msgCommand.data[1] = registerAddress;
    msgCommand.length = 2;

    msgData.RW = I2C_READ;
    //msgData.data = data;
    msgData.data[0] = slaveAddress  | 0x01;
    msgData.length = length;

    // On place les messages dans le buffer
    I2C1WriteToBuffer(msgCommand);
    I2C1WriteToBuffer(msgData);

    // On lance la tranmission si aucune transmission n'est en cours
    if(I2C1IsTransmissionActive()==FALSE)
        StartI2C1Message();

    //On est dans un read, on attend donc le résultat
    ResetI2C1AntiBlockCounter();
    while((I2C1IsTransmissionActive()==TRUE) && (antiBlockCounterI2C1 < I2C_TIMEOUT))
    {
        Nop();
    }
    if(antiBlockCounterI2C1 >= I2C_TIMEOUT)
    {
        SetI2C1Crash();
    }
//    if(antiBlockCounterI2C1 <= 0)
//    {
//        I2C1State = I2C_IDLE;
//        InitI2C1();
//    }
    
    int i=0;
    for (i=0; i<length; i++)
    {
        data[i] = currentI2CMsg.data[i];
    }
}


/*******************************************************************************
 * @fn      Gestion des crash
 *
 * @brief
 *
 * @param
 *
 * @return  None
 *
 ******************************************************************************/
void IncrementI2C1AntiBlockCounter(void)
{
    antiBlockCounterI2C1+=1;
}

void ResetI2C1AntiBlockCounter(void)
{
    antiBlockCounterI2C1=0;
}

void SetI2C1Crash()
{
    I2C1Crashed = TRUE;
}

void ResetI2CCrash()
{
    I2C1Crashed = FALSE;
}

unsigned char IsI2CCrashed(void)
{
    return I2C1Crashed;
}

void StartI2C1Message(void)
{
    while(I2C1State != I2C_IDLE)
    {
        //On attend
        ;
    }

    if(I2C1IsDataReadyInBuffer() == TRUE)
    {
        currentI2CMsg = I2C1ReadFromBuffer();

        //On s'assure que la priorité de l'I2C est bien maximale
        IPC4bits.MI2C1IP = 7;

        //On initie une transaction par un start
        //Attention ! l'ordre des deux instructions suivantes est essentiel pour éviter une interruption
        //alors que on est toujours en I2C_IDLE
        I2C1State = I2C_START;
        I2C1CONbits.SEN = 1;
    }
    else
    {
        ;
    }
}


void I2C1TransmissionOperation()
{

    if(currentI2CMsg.RW==I2C_READ)
    {
        if(I2C1TrnCounter==0)
        {
            //On transmet la slave address modifiée par | 0x01
            I2C1TRN = currentI2CMsg.data[I2C1TrnCounter];
            I2C1TrnCounter++;
        }
        else if(I2C1TrnCounter-1 < currentI2CMsg.length)
        {
            //Pour chaque octet à recevoir
            switch(I2C1ReceptionState)
            {
                case 0 :
                    //On lance le transfert d'un octet en déclenchant RCEN
                    I2C1CONbits.RCEN = 1;
                    I2C1ReceptionState = 1;
                    break;
                case 1:
                    currentI2CMsg.data[I2C1TrnCounter-1] = I2C1RCV;
                    if(I2C1TrnCounter==currentI2CMsg.length)
                        I2C1CONbits.ACKDT = 1;
                    else
                        I2C1CONbits.ACKDT = 0;
                    I2C1CONbits.ACKEN = 1;
                    I2C1ReceptionState = 0;
                    I2C1TrnCounter += 1;
                    break;
                default:
                    I2C1ReceptionState = 0;
                    break;
            }
        }
        else
        {
            //On remet à 0 le compteur de transmissions
            I2C1TrnCounter = 0;

            //On arrête la transmission : obligatoire sur un read
            I2C1State = I2C_STOP;
            I2C1CONbits.PEN=1;
        }
    }
    else
    {
        if(I2C1TrnCounter < currentI2CMsg.length)
        {
            //On transmet l'octet suivant
            I2C1TRN = currentI2CMsg.data[I2C1TrnCounter];
            I2C1TrnCounter++;
        }
        else
        {
            //On a termine la transmission
            if(I2C1IsDataReadyInBuffer()==TRUE)
            {
                //On relance une nouvelle transmission
                currentI2CMsg = I2C1ReadFromBuffer();
                I2C1State = I2C_RESTART;
                I2C1CONbits.RSEN=1;
            }
            else
            {
                //on termine la transmission
                I2C1State = I2C_STOP;
                I2C1CONbits.PEN=1;
            }
        }
    }
}

BOOL I2C1IsTransmissionActive(void)
{
    if(I2C1State != I2C_IDLE)
        return TRUE;
    else
        return FALSE;
}

#ifdef USE_I2C1_CIRCULAR_BUFFER
void I2C1WriteToBuffer(I2CData data )
{
    I2C1Value[I2C1CircularBufferHead] = data;
    if(I2C1CircularBufferHead<I2C1_CIRCULAR_BUFFER_SIZE-1)
        I2C1CircularBufferHead+=1;
    else
        I2C1CircularBufferHead=0;
}

BOOL I2C1IsDataReadyInBuffer(void)
{
    if(I2C1CircularBufferHead != I2C1CircularBufferTail)
        return TRUE;
    else
        return FALSE;
}

I2CData I2C1ReadFromBuffer(void)
{
    I2CData data = I2C1Value[I2C1CircularBufferTail];
    if(I2C1CircularBufferTail<I2C1_CIRCULAR_BUFFER_SIZE-1)
        I2C1CircularBufferTail+=1;
    else
        I2C1CircularBufferTail=0;
    return data;
}
#endif

