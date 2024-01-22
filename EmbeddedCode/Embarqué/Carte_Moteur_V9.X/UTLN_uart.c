/*********************************************************************
 * INCLUDES
 */
#include "UTLN_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <p33EP512MU814.h>
#include "define.h"


//#define URXINV RXINV
//#define U1EIE U1ERIE
#define U1EIF U1ERIF


#define BUFFER_TX_DMA_SIZE 16


#define BAUDRATE_UART1 115200//115200//115200//38400//57600
#define BAUDRATE_UART2 115200
#define BAUDRATE_UART3 115200//57600//38400//57600
#define BAUDRATE_UART4 115200//57600//38400//57600
unsigned long _baudRateUART1=BAUDRATE_UART1;
unsigned long _baudRateUART2=BAUDRATE_UART2;
unsigned long _baudRateUART3=BAUDRATE_UART3;
unsigned long _baudRateUART4=BAUDRATE_UART4;
#define BRGVAL_UART1 ((FCY/_baudRateUART1)/16) - 1		// High Baud Rate
#define BRGVAL_UART2 ((FCY/_baudRateUART2)/16) - 1       // High Baud Rate
#define BRGVAL_UART3 ((FCY/_baudRateUART3)/16) - 1		// High Baud Rate
#define BRGVAL_UART4 ((FCY/_baudRateUART4)/16) - 1		// High Baud Rate



int antiBlockCounterUartTx;

//unsigned char intRxUART2Buffer[UART2_BUFFER_SIZE];
//size_t intRxUART2BufferEndPos=0;

//volatile BOOL UART1TxTransmissionEnCours=FALSE;


#if defined( USE_DMA) && !defined(USE_UART1_CIRCULAR_BUFFER)
unsigned int BufferDMA1A[1] __attribute__((space(dma)));
unsigned int BufferDMA1B[1] __attribute__((space(dma)));
unsigned char BufferDMA3A[BUFFER_TX_DMA_SIZE] __attribute__((space(dma)));
unsigned char BufferDMA3B[BUFFER_TX_DMA_SIZE] __attribute__((space(dma)));
static unsigned int BufferDMA3PingPong = 0; // Keep record of which buffer contains Rx Data
static unsigned int BufferDMA3Pos=0;
#endif

#if defined( USE_DMA) && !defined(USE_UART2_CIRCULAR_BUFFER)
unsigned int BufferDMA2A[1] __attribute__((space(dma)));
unsigned int BufferDMA2B[1] __attribute__((space(dma)));
unsigned char BufferDMA4A[BUFFER_TX_DMA_SIZE] __attribute__((space(dma)));
unsigned char BufferDMA4B[BUFFER_TX_DMA_SIZE] __attribute__((space(dma)));
static unsigned int BufferDMA4PingPong = 0; // Keep record of which buffer contains Rx Data
static unsigned int BufferDMA4Pos=0;
#endif

//volatile unsigned int flagRxUART1DataAvailable;
volatile unsigned char flagRxUART2DataAvailable;
volatile BOOL UART3TxTransmissionEnCours=FALSE;
volatile BOOL UART4TxTransmissionEnCours=FALSE;

#ifdef USE_UART1_CIRCULAR_BUFFER
    unsigned int UART1CircularRxBufferHead = 0;
    unsigned int UART1CircularRxBufferTail = 0;
    volatile unsigned int UART1CircularTxBufferHead = 0;
    volatile unsigned int UART1CircularTxBufferTail = 0;
    unsigned char UART1RxBuffer[UART1_CIRCULAR_BUFFER_SIZE];
    volatile unsigned char UART1TxBuffer[UART1_CIRCULAR_BUFFER_SIZE];
#endif
    
#ifdef USE_UART2_CIRCULAR_BUFFER
    unsigned char UART2RxBuffer[UART2_CIRCULAR_BUFFER_SIZE];
    unsigned int UART2CircularRxBufferHead = 0;
    unsigned int UART2CircularRxBufferTail = 0;
    volatile unsigned int UART2CircularTxBufferHead = 0;
    volatile unsigned int UART2CircularTxBufferTail = 0;
    volatile unsigned char UART2TxBuffer[UART1_CIRCULAR_BUFFER_SIZE];
#endif

#ifdef USE_UART3_CIRCULAR_BUFFER
    unsigned int UART3CircularRxBufferHead = 0;
    unsigned int UART3CircularRxBufferTail = 0;
    unsigned int UART3CircularTxBufferHead = 0;
    unsigned int UART3CircularTxBufferTail = 0;
    unsigned char UART3RxBuffer[UART3_CIRCULAR_BUFFER_SIZE];
    unsigned char UART3TxBuffer[UART3_CIRCULAR_BUFFER_SIZE];    //MODIF VAL OSAL
#endif
    
#ifdef USE_UART4_CIRCULAR_BUFFER
    unsigned int UART4CircularRxBufferHead = 0;
    unsigned int UART4CircularRxBufferTail = 0;
    unsigned int UART4CircularTxBufferHead = 0;
    unsigned int UART4CircularTxBufferTail = 0;
    unsigned char UART4RxBuffer[UART4_CIRCULAR_BUFFER_SIZE];
    unsigned char UART4TxBuffer[UART4_CIRCULAR_BUFFER_SIZE];    //MODIF VAL OSAL
#endif
    
/****************************************************************************************************/
// Configuration Port série 1
/****************************************************************************************************/
void initUART1(void)
{
    U1MODEbits.BRGH = 0;	// Low speed
    U1MODEbits.STSEL = 0; 	// 1-stop bit
    U1MODEbits.PDSEL = 0b00;    // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; 	// Auto-Baud Disabled
    U1MODEbits.URXINV=0;
    
    #ifdef USE_UART1_FLOW_CONTROL
        U1MODEbits.UEN=0b10;        //RTS, CTS, TX, RX are used
        U1MODEbits.RTSMD=0;         //Flow control mode
    #endif

    U1BRG = BRGVAL_UART1;	// BAUD Rate Setting for UART1

    U1STAbits.UTXISEL1 = 0; // ordre du champ <1:0>
    U1STAbits.UTXISEL0 = 0; // 11 = Reserved
                                                    // 10 = Interrupt generated when a character is transferred to the Transmit Shift register and the transmit buffer becomes empty
                                                    // 01 = Interrupt generated when the last transmission is over (i.e., the last character has been shifted out of the Transmit Shift register) and all the transmit operations are completed
                                                    // 00 = Interrupt generated when any character is transferred to the Transmit Shift Register (which implies at least one location is empty in the transmit buffer)
    U1STAbits.URXISEL = 0b00; 	// Interrupt after one RX character is received //00
    
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1TXIE = 1;
    IEC4bits.U1EIE = 1; 	//Enable interrupt on errors for UART1
    IFS0bits.U1RXIF = 0; // clear RX interrupt flag
    IFS0bits.U1TXIF = 0;
    
    IPC16bits.U1EIP = 7;    //Gestion des erreurs UART ? un niveau sup?rieur ? tout le reste
    IPC2bits.U1RXIP=7;
    IPC3bits.U1TXIP=5;

    U1MODEbits.UARTEN = 1; 	// Enable UART
    U1STAbits.UTXEN = 1; 	// Enable UART Tx
    
    unsigned char dummy;
    dummy=U1RXREG;          //to clear error
    //int i;
    //for(i = 0; i < 4160; i++)
    //{
    //    Nop();
    //}
    #ifdef USE_UART1_CIRCULAR_BUFFER
        UART1ResetRxBuffer();
        UART1ResetTxBuffer();
    #endif
}
void UART1SetBaudRate(unsigned long rate)
{
    _baudRateUART1=rate;
    initUART1();
}

#ifdef USE_UART1_CIRCULAR_BUFFER
void UART1ResetRxBuffer(void)
{
    UART1CircularRxBufferHead = 0;
    UART1CircularRxBufferTail = 0;
}

void UART1ResetTxBuffer(void)
{
    UART1CircularTxBufferHead = 0;
    UART1CircularTxBufferTail = 0;
}

unsigned char trameAruco[] = {'D', '2', ' ', 'U', '1', '7', ' ', '4', ' ', '3', ' ', '4', ' ', '5', ' ', '6', ' ', '7', ' ', '8', ' ', '9', ' ', '0', '\n'};
unsigned char lengthTrameAruco = 25;
unsigned char indexTrameAruco;
unsigned char test;

void UART1ReadToRxBuffer(void)
{
    ////unsigned char toto = U1RXREG; 
    //UART1RxBuffer[UART1CircularRxBufferHead++] = trameAruco[indexTrameAruco++];
    UART1RxBuffer[UART1CircularRxBufferHead++] = U1RXREG;
    if(indexTrameAruco>=lengthTrameAruco)
        indexTrameAruco = 0;
            
    if(UART1CircularRxBufferHead>=UART1_CIRCULAR_BUFFER_SIZE)
        UART1CircularRxBufferHead=0;
}

BOOL UART1IsDataReadyInRxBuffer(void)
{
    if(UART1CircularRxBufferHead != UART1CircularRxBufferTail)
        return TRUE;
    else
        return FALSE;
}

unsigned char UART1ReadFromRxBuffer(void)
{
    unsigned char data = UART1RxBuffer[UART1CircularRxBufferTail++];
    if(UART1CircularRxBufferTail>=UART1_CIRCULAR_BUFFER_SIZE)
        UART1CircularRxBufferTail=0;
    return data;
}
unsigned int UART1GetNbOfValuesInRxBuffer(void)
{
    if(UART1CircularRxBufferHead>=UART1CircularRxBufferTail)
        return UART1CircularRxBufferHead-UART1CircularRxBufferTail;
    else
        return UART1_CIRCULAR_BUFFER_SIZE - UART1CircularRxBufferTail + UART1CircularRxBufferHead;
}

#endif

#ifdef USE_UART1_CIRCULAR_BUFFER
unsigned char test;
void UART1WriteToTxBuffer(unsigned char value)
{
    UART1TxBuffer[UART1CircularTxBufferHead++] = value;
    if(UART1CircularTxBufferHead>=UART1_CIRCULAR_BUFFER_SIZE)
        UART1CircularTxBufferHead=0;

    //Une fois l'écriture effectuée, il faut transmettre vers l'UART
    //Si le buffer n'est pas en cours de vidage, on déclenche la transmission manuellement
     if(flagSystem.UART1TxTransmissionEnCours == FALSE)
     {         
         UART1WriteFromTxBuffer();
     }
}

BOOL UART1IsDataReadyInTxBuffer(void)
{
    if(UART1CircularTxBufferHead != UART1CircularTxBufferTail)
        return TRUE;
    else
        return FALSE;
}

void UART1WriteFromTxBuffer(void)
{
    //On passe la tranmission en état actif
    flagSystem.UART1TxTransmissionEnCours = TRUE;
    unsigned char data = UART1TxBuffer[UART1CircularTxBufferTail++];
    if(UART1CircularTxBufferTail>=UART1_CIRCULAR_BUFFER_SIZE)
        UART1CircularTxBufferTail=0;    
    //IEC0bits.U1TXIE = 1;
    U1TXREG = data;
    //antiBlockCounterUartTx = 0;
}

unsigned int UART1GetNbOfValuesInTxBuffer(void)
{
    if(UART1CircularTxBufferHead>=UART1CircularTxBufferTail)
        return UART1CircularTxBufferHead-UART1CircularTxBufferTail;
    else
        return UART1_CIRCULAR_BUFFER_SIZE - UART1CircularTxBufferTail + UART1CircularTxBufferHead;
}


unsigned int UART1GetRemainingSpaceInTxBuffer(void)
{
    return UART1_CIRCULAR_BUFFER_SIZE - UART1GetNbOfValuesInTxBuffer();
}
#endif

/****************************************************************************************************/
// Configuration Port série 2
/****************************************************************************************************/
void initUART2(void)
{
    U2MODEbits.BRGH = 0;  // Low speed
    U2MODEbits.STSEL = 0; // 1-stop bit
    U2MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U2MODEbits.ABAUD = 0; // Auto-Baud Disabled

    U2BRG = BRGVAL_UART2; // BAUD Rate Setting for UART2
    U2MODEbits.URXINV=0;

    U2STAbits.UTXISEL1 = 0;
    U2STAbits.UTXISEL0 = 0; // 11 = Reserved
                                                    // 10 = Interrupt generated when a character is transferred to the Transmit Shift register and the transmit buffer becomes empty
                                                    // 01 = Interrupt generated when the last transmission is over (i.e., the last character has been shifted out of the Transmit Shift register) and all the transmit operations are completed
                                                    // 00 = Interrupt generated when any character is transferred to the Transmit Shift Register (which implies at least one location is empty in the transmit buffer)
    U2STAbits.URXISEL = 0b00; // Interrupt after one RX character is received
    IPC7bits.U2RXIP=7;
    IPC16bits.U2EIP=7;
    IEC1bits.U2RXIE = 1;
    IEC1bits.U2TXIE = 1;
    IEC4bits.U2EIE = 1; 	//Enable interrupt on errors for UART1
    IFS1bits.U2RXIF = 0; // clear RX interrupt flag
    IFS1bits.U2TXIF = 0;
    U2MODEbits.UARTEN = 1; // Enable UART
    U2STAbits.UTXEN = 1; // Enable UART Tx

    int i;
    for(i = 0; i < 4160; i++)
    {
        Nop();
    }
}
void UART2SetBaudRate(unsigned long rate)
{
    _baudRateUART2=rate;
    initUART2();
}




#ifdef USE_UART2_CIRCULAR_BUFFER
void UART2ReadToRxBuffer(void)
{
    UART2RxBuffer[UART2CircularRxBufferHead] = U2RXREG;
    if(UART2CircularRxBufferHead<UART2_CIRCULAR_BUFFER_SIZE-1)
        UART2CircularRxBufferHead+=1;
    else
        UART2CircularRxBufferHead=0;
}

BOOL UART2IsDataReadyInRxBuffer(void)
{
    if(UART2CircularRxBufferHead != UART2CircularRxBufferTail)
        return TRUE;
    else
        return FALSE;
}

unsigned char UART2ReadFromRxBuffer(void)
{
    unsigned char data = UART2RxBuffer[UART2CircularRxBufferTail];
    if(UART2CircularRxBufferTail<UART2_CIRCULAR_BUFFER_SIZE-1)
        UART2CircularRxBufferTail+=1;
    else
        UART2CircularRxBufferTail=0;
    return data;
}

void UART2WriteToTxBuffer(unsigned char value)
{
    UART2TxBuffer[UART2CircularTxBufferHead] = value;
    if(UART2CircularTxBufferHead<UART2_CIRCULAR_BUFFER_SIZE-1)
        UART2CircularTxBufferHead+=1;
    else
        UART2CircularTxBufferHead=0;

    //Une fois l'écriture effectuée, il faut transmettre vers l'UART
    //Si le buffer n'est pas en cours de vidage, on déclenche la transmission manuellement
     if(flagSystem.UART2TxTransmissionEnCours == FALSE)
     {         
         UART2WriteFromTxBuffer();
     }
}

BOOL UART2IsDataReadyInTxBuffer(void)
{
    if(UART2CircularTxBufferHead != UART2CircularTxBufferTail)
        return TRUE;
    else
        return FALSE;
}
unsigned int UART2GetNbOfValuesInRxBuffer(void)
{
    if(UART2CircularRxBufferHead>=UART2CircularRxBufferTail)
        return UART2CircularRxBufferHead-UART2CircularRxBufferTail;
    else
        return UART2_CIRCULAR_BUFFER_SIZE - UART2CircularRxBufferTail + UART2CircularRxBufferHead;
}

void UART2WriteFromTxBuffer(void)
{
    //On passe la tranmission en état actif
    flagSystem.UART2TxTransmissionEnCours = TRUE;

    unsigned char data = UART2TxBuffer[UART2CircularTxBufferTail];
    if(UART2CircularTxBufferTail<UART2_CIRCULAR_BUFFER_SIZE-1)
        UART2CircularTxBufferTail+=1;
    else
        UART2CircularTxBufferTail=0;
    IEC1bits.U2TXIE = 1;
    U2TXREG = data;
    antiBlockCounterUartTx = 0;
}

unsigned int UART2GetNbOfValuesInTxBuffer(void)
{
    if(UART2CircularTxBufferHead>=UART2CircularTxBufferTail)
        return UART2CircularTxBufferHead-UART2CircularTxBufferTail;
    else
        return UART2_CIRCULAR_BUFFER_SIZE - UART2CircularTxBufferTail + UART2CircularTxBufferHead;
}


unsigned int UART2GetRemainingSpaceInTxBuffer(void)
{
    return UART2_CIRCULAR_BUFFER_SIZE - UART2GetNbOfValuesInTxBuffer();
}

void UART2MonitorAndFixTxTransmission()
{
    //Lancé sur interruption timer à 10kHz
    antiBlockCounterUartTx+=1;
    if((flagSystem.UART2TxTransmissionEnCours == TRUE) && (antiBlockCounterUartTx >= UART_TX_TIMOUT))
    {
        //On a figé la liaison UART Tx en ratant une interruption
        if(UART2IsDataReadyInTxBuffer()==TRUE)
        {
            //On renvoie un nouveau caractère
            UART2WriteFromTxBuffer();
        }
        else
        {
            flagSystem.UART2TxTransmissionEnCours = FALSE;
        }
    }
}
#else

#endif


/****************************************************************************************************/
// Configuration Port série 3
/****************************************************************************************************/

void initUART3(void)
{
    U3MODEbits.BRGH = 0;	// Low speed
    U3MODEbits.STSEL = 0; 	// 1-stop bit
    U3MODEbits.PDSEL = 0b00;    // No Parity, 8-data bits
    U3MODEbits.ABAUD = 0; 	// Auto-Baud Disabled
    U3MODEbits.URXINV=0;
    //U3MODEbits.UEN=0b10;        //RTS, CTS, TX, RX are used
    //U3MODEbits.RTSMD=0;         //Flow control mode

    U3BRG = BRGVAL_UART3;	// BAUD Rate Setting for UART3

    U3STAbits.UTXISEL1 = 0; // ordre du champ <1:0>
    U3STAbits.UTXISEL0 = 0; // 11 = Reserved
                                                    // 10 = Interrupt generated when a character is transferred to the Transmit Shift register and the transmit buffer becomes empty
                                                    // 01 = Interrupt generated when the last transmission is over (i.e., the last character has been shifted out of the Transmit Shift register) and all the transmit operations are completed
                                                    // 00 = Interrupt generated when any character is transferred to the Transmit Shift Register (which implies at least one location is empty in the transmit buffer)
    U3STAbits.URXISEL = 0b00; 	// Interrupt after one RX character is received //00

    IPC20bits.U3EIP=7;
    IPC20bits.U3RXIP=7;
    IEC5bits.U3RXIE = 1;
    IEC5bits.U3TXIE = 1;
    IEC5bits.U3EIE = 1; 	//Enable interrupt on errors for UART3
    IFS5bits.U3RXIF = 0;        // clear RX interrupt flag
    IFS5bits.U3TXIF = 0;

    U3MODEbits.UARTEN = 1; 	// Enable UART
    U3STAbits.UTXEN = 1; 	// Enable UART Tx

    int i;
    for(i = 0; i < 4160; i++)
    {
        Nop();
    }
}

void UART3SetBaudRate(unsigned long rate)
{
    _baudRateUART3=rate;
    initUART3();
}


#ifdef USE_UART3_CIRCULAR_BUFFER
void UART3ReadToRxBuffer(void)
{
    UART3RxBuffer[UART3CircularRxBufferHead] = U3RXREG;
    if(UART3CircularRxBufferHead<UART3_CIRCULAR_BUFFER_SIZE-1)
        UART3CircularRxBufferHead+=1;
    else
        UART3CircularRxBufferHead=0;
}

BOOL UART3IsDataReadyInRxBuffer(void)
{
    if(UART3CircularRxBufferHead != UART3CircularRxBufferTail)
        return TRUE;
    else
        return FALSE;
}

unsigned char UART3ReadFromRxBuffer(void)
{
    unsigned char data = UART3RxBuffer[UART3CircularRxBufferTail];
    if(UART3CircularRxBufferTail<UART3_CIRCULAR_BUFFER_SIZE-1)
        UART3CircularRxBufferTail+=1;
    else
        UART3CircularRxBufferTail=0;
    return data;
}
unsigned int UART3GetNbOfValuesInRxBuffer(void)
{
    if(UART3CircularRxBufferHead>=UART3CircularRxBufferTail)
        return UART3CircularRxBufferHead-UART3CircularRxBufferTail;
    else
        return UART3_CIRCULAR_BUFFER_SIZE - UART3CircularRxBufferTail + UART3CircularRxBufferHead;
}

void UART3WriteToTxBuffer(unsigned char value)
{
    UART3TxBuffer[UART3CircularTxBufferHead] = value;
    if(UART3CircularTxBufferHead<UART3_CIRCULAR_BUFFER_SIZE-1)
        UART3CircularTxBufferHead+=1;
    else
        UART3CircularTxBufferHead=0;

    //Une fois l'écriture effectuée, il faut transmettre vers l'UART
    //Si le buffer n'est pas en cours de vidage, on déclenche la transmission manuellement
     if(UART3IsTransmissionActive()==FALSE)
     {
         UART3WriteFromTxBuffer();
     }
}

BOOL UART3IsDataReadyInTxBuffer(void)
{
    if(UART3CircularTxBufferHead != UART3CircularTxBufferTail)
        return TRUE;
    else
        return FALSE;
}

BOOL UART3IsTransmissionActive(void)
{
    return UART3TxTransmissionEnCours;
}

void UART3WriteFromTxBuffer(void)
{
    //On passe la tranmission en état actif
    UART3TxTransmissionEnCours = TRUE;

    unsigned char data = UART3TxBuffer[UART3CircularTxBufferTail];
    if(UART3CircularTxBufferTail<UART3_CIRCULAR_BUFFER_SIZE-1)
        UART3CircularTxBufferTail+=1;
    else
        UART3CircularTxBufferTail=0;
    U3TXREG = data;
}

unsigned int UART3GetNbOfValuesInTxBuffer(void)
{
    if(UART3CircularTxBufferHead>=UART3CircularTxBufferTail)
        return UART3CircularTxBufferHead-UART3CircularTxBufferTail;
    else
        return UART3_CIRCULAR_BUFFER_SIZE - UART3CircularTxBufferTail + UART3CircularTxBufferHead;
}


unsigned int UART3GetRemainingSpaceInTxBuffer(void)
{
    return UART3_CIRCULAR_BUFFER_SIZE - UART3GetNbOfValuesInTxBuffer();
}


#endif

void initUART4(void)
{
    U4MODEbits.BRGH = 0;  // Low speed
    U4MODEbits.STSEL = 0; // 1-stop bit
    U4MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U4MODEbits.ABAUD = 0; // Auto-Baud Disabled

    U4BRG = BRGVAL_UART4; // BAUD Rate Setting for UART2
    U4MODEbits.URXINV=0;

    U4STAbits.UTXISEL1 = 0;
    U4STAbits.UTXISEL0 = 0; // 11 = Reserved
                                                    // 10 = Interrupt generated when a character is transferred to the Transmit Shift register and the transmit buffer becomes empty
                                                    // 01 = Interrupt generated when the last transmission is over (i.e., the last character has been shifted out of the Transmit Shift register) and all the transmit operations are completed
                                                    // 00 = Interrupt generated when any character is transferred to the Transmit Shift Register (which implies at least one location is empty in the transmit buffer)
    U4STAbits.URXISEL = 0b00; // Interrupt after one RX character is received
    IPC22bits.U4RXIP=7;
    IPC21bits.U4EIP=7;
    IEC5bits.U4RXIE = 1;
    IEC5bits.U4TXIE = 1;
    IEC5bits.U4EIE = 1; 	//Enable interrupt on errors for UART1
    IFS5bits.U4RXIF = 0; // clear RX interrupt flag
    IFS5bits.U4TXIF = 0;
    U4MODEbits.UARTEN = 1; // Enable UART
    U4STAbits.UTXEN = 1; // Enable UART Tx

    int i;
    for(i = 0; i < 4160; i++)
    {
        Nop();
    }
}
void UART4SetBaudRate(unsigned long rate)
{
    _baudRateUART4=rate;
    initUART4();
}

#ifdef USE_UART4_CIRCULAR_BUFFER
void UART4ReadToRxBuffer(void)
{
    UART4RxBuffer[UART4CircularRxBufferHead] = U4RXREG;
    if(UART4CircularRxBufferHead<UART4_CIRCULAR_BUFFER_SIZE-1)
        UART4CircularRxBufferHead+=1;
    else
        UART4CircularRxBufferHead=0;
}

BOOL UART4IsDataReadyInRxBuffer(void)
{
    if(UART4CircularRxBufferHead != UART4CircularRxBufferTail)
        return TRUE;
    else
        return FALSE;
}

unsigned char UART4ReadFromRxBuffer(void)
{
    unsigned char data = UART4RxBuffer[UART4CircularRxBufferTail];
    if(UART4CircularRxBufferTail<UART4_CIRCULAR_BUFFER_SIZE-1)
        UART4CircularRxBufferTail+=1;
    else
        UART4CircularRxBufferTail=0;
    return data;
}

unsigned int UART4GetNbOfValuesInRxBuffer(void)
{
    if(UART4CircularRxBufferHead>=UART4CircularRxBufferTail)
        return UART4CircularRxBufferHead-UART4CircularRxBufferTail;
    else
        return UART4_CIRCULAR_BUFFER_SIZE - UART4CircularRxBufferTail + UART4CircularRxBufferHead;
}

void UART4WriteToTxBuffer(unsigned char value)
{
    UART4TxBuffer[UART4CircularTxBufferHead] = value;
    if(UART4CircularTxBufferHead<UART4_CIRCULAR_BUFFER_SIZE-1)
        UART4CircularTxBufferHead+=1;
    else
        UART4CircularTxBufferHead=0;

    //Une fois l'écriture effectuée, il faut transmettre vers l'UART
    //Si le buffer n'est pas en cours de vidage, on déclenche la transmission manuellement
     if(UART4IsTransmissionActive()==FALSE)
     {
         UART4WriteFromTxBuffer();
     }
}

BOOL UART4IsDataReadyInTxBuffer(void)
{
    if(UART4CircularTxBufferHead != UART4CircularTxBufferTail)
        return TRUE;
    else
        return FALSE;
}

BOOL UART4IsTransmissionActive(void)
{
    return UART4TxTransmissionEnCours;
}

void UART4WriteFromTxBuffer(void)
{
    //On passe la tranmission en état actif
    UART4TxTransmissionEnCours = TRUE;

    unsigned char data = UART4TxBuffer[UART4CircularTxBufferTail];
    if(UART4CircularTxBufferTail<UART4_CIRCULAR_BUFFER_SIZE-1)
        UART4CircularTxBufferTail+=1;
    else
        UART4CircularTxBufferTail=0;
    U4TXREG = data;
}

unsigned int UART4GetNbOfValuesInTxBuffer(void)
{
    if(UART4CircularTxBufferHead>=UART4CircularTxBufferTail)
        return UART4CircularTxBufferHead-UART4CircularTxBufferTail;
    else
        return UART4_CIRCULAR_BUFFER_SIZE - UART4CircularTxBufferTail + UART4CircularTxBufferHead;
}


unsigned int UART4GetRemainingSpaceInTxBuffer(void)
{
    return UART4_CIRCULAR_BUFFER_SIZE - UART4GetNbOfValuesInTxBuffer();
}
#endif

#ifdef USE_DMA
void putcUART1DMA(unsigned char c)
{
    //On reçoit les caractères dans chacun des buffers en alternance
    //Attention, c'est un ping-pong réalisé à la main, il n'a rien
    //à voir avec le mode ping-pong
    if(BufferDMA3PingPong==0)
        BufferDMA3A[BufferDMA3Pos]=c;
    else
        BufferDMA3B[BufferDMA3Pos]=c;

    BufferDMA3Pos++;

    //Si on a rempli le buffer
    if (BufferDMA3Pos==BUFFER_TX_DMA_SIZE)
    {
        sendUART1DMABuffer();
    }
}
#endif

#ifdef USE_DMA
void sendUART1DMABuffer(void)
{
    //On vérifie qu'il y a de la place dans la FIFO du Tx de l'UART
    while(U1STAbits.TRMT==0);
    //On envoie le buffer dans le DMA3 en alternance (mais vers la même destination)
    if(BufferDMA3PingPong==0)
    {
        DMA3STA = __builtin_dmaoffset(&BufferDMA3A);	// Point DMA 2 to data to be transmitted
    }
    else
    {
        DMA3STA = __builtin_dmaoffset(&BufferDMA3B);	// Point DMA 2 to data to be transmitted
    }

    //On déclenche à la main le DMA
    DMA3REQbits.FORCE = 0;
    DMA3CONbits.CHEN = 1;
    DMA3REQbits.FORCE = 1;

    //Réinitialisation du compteur de position buffer DMA
    BufferDMA3Pos=0;
    //Switch de buffer DMA
    BufferDMA3PingPong ^= 1;
}
#endif

#ifdef USE_DMA
void putcUART2DMA(unsigned char c)
{
    //On reçoit les caractères dans chacun des buffers en alternance
    //Attention, c'est un ping-pong réalisé à la main, il n'a rien
    //à voir avec le mode ping-pong
    if(BufferDMA4PingPong==0)
        BufferDMA4A[BufferDMA3Pos]=c;
    else
        BufferDMA4B[BufferDMA3Pos]=c;

    BufferDMA4Pos++;

    //Si on a rempli le buffer
    if (BufferDMA4Pos==BUFFER_TX_DMA_SIZE)
    {
        sendUART2DMABuffer();
    }
}
#endif

#ifdef USE_DMA
void sendUART2DMABuffer(void)
{
    //On vérifie qu'il y a de la place dans la FIFO du Tx de l'UART
    while(U2STAbits.TRMT==0);
    //On envoie le buffer dans le DMA3 en alternance (mais vers la même destination)
    if(BufferDMA4PingPong==0)
    {
        DMA4STA = __builtin_dmaoffset(&BufferDMA4A);	// Point DMA 2 to data to be transmitted
    }
    else
    {
        DMA4STA = __builtin_dmaoffset(&BufferDMA4B);	// Point DMA 2 to data to be transmitted
    }

    //On déclenche à la main le DMA
    DMA4CONbits.CHEN = 1;
    DMA4REQbits.FORCE = 1;

    //Réinitialisation du compteur de position buffer DMA
    BufferDMA4Pos=0;
    //Switch de buffer DMA
    BufferDMA4PingPong ^= 1;
}
#endif

/****************************************************************************************************/
// Interrupt Service Routine
/****************************************************************************************************/


#ifdef USE_DMA
void __attribute__((__interrupt__, auto_psv)) _DMA1Interrupt(void)
{
    static unsigned int BufferDMA1Count = 0; // Keep record of which buffer contains Rx Data
    if(BufferDMA1Count == 0)
            intRxUART1Buffer[intRxUART1BufferEndPos] = BufferDMA1A[0];
    else
            intRxUART1Buffer[intRxUART1BufferEndPos] = BufferDMA1B[0];

    // modif de la position du pointeur dans le buffer tournant
    ++intRxUART1BufferEndPos;
    intRxUART1BufferEndPos &= UART1_BUFFER_SIZE-1;

    //Ping Pong sur le buffer DMA
    BufferDMA1Count ^= 1;

    IFS0bits.DMA1IF = 0;// Clear the DMA1 Interrupt Flag

    //Des données sont dispo dans le buffer de réception
    flagRxUART1DataAvailable = 1;
}
#endif

#ifdef USE_DMA
void __attribute__((__interrupt__, auto_psv)) _DMA2Interrupt(void)
{
    static unsigned int BufferDMA2Count = 0; // Keep record of which buffer contains Rx Data

    if(BufferDMA2Count == 0)
        intRxUART2Buffer[intRxUART2BufferEndPos] = BufferDMA2A[0];
    else
        intRxUART2Buffer[intRxUART2BufferEndPos] = BufferDMA2B[0];

    // modif de la position du pointeur dans le buffer tournant
    ++intRxUART2BufferEndPos;
    intRxUART2BufferEndPos &= UART2_BUFFER_SIZE-1;

    //Ping Pong sur le buffer DMA
    BufferDMA2Count ^= 1;

    IFS1bits.DMA2IF = 0;// Clear the DMA2 Interrupt Flag
}
#endif

#ifdef USE_DMA
void __attribute__((__interrupt__, auto_psv)) _DMA3Interrupt(void)
{
    IFS2bits.DMA3IF = 0;// Clear the DMA3 Interrupt Flag
}

void __attribute__((__interrupt__, auto_psv)) _DMA4Interrupt(void)
{
    IFS2bits.DMA4IF = 0;// Clear the DMA3 Interrupt Flag
}
#endif


#if defined(USE_DMA) && (defined(USE_UART1_CIRCULAR_BUFFER) || defined(USE_UART2_CIRCULAR_BUFFER) || defined(USE_UART3_CIRCULAR_BUFFER))
    #warning "Impossible d'utiliser le DMA en combinaison avec les buffer circulaires"
#endif
