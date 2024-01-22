#include <xc.h> // include processor files - each processor file is guarded.  
#include "Define.h"
#include "UTLN_uart.h"
#include "UTLN_Communication.h"
#include "UTLN_Flag.h"
#include "UTLN_CAN.h"
#include "RobotObj.h"

// <editor-fold defaultstate="collapsed" desc="UARTs Interrupts">
#ifdef USE_UART1
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    IFS0bits.U1RXIF = 0; // clear RX interrupt flag

    //Hardware Rx FIFO de 4 octets, donc on verifie de recuperer tout les octets
    while(U1STAbits.URXDA)
        {
            #ifdef USE_UART1
                UART1ReadToRxBuffer();
            #endif
            /*
            //Des donn?es sont dispo dans le buffer de r?ception
            ProcessUartReceiveChar();
            */
        }

    _LED_B1=!_LED_B1;
    
    //Des donn?es sont dispo dans le buffer de r?ception
   //Uart1DecodeMessage();
    //ProcessUartReceiveChar();

    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}



void __attribute__((__interrupt__, no_auto_psv)) _U1ErrInterrupt(void)//ISR2(_U1ErrInterrupt)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif

      IFS4bits.U1EIF = 0;
      if (U1STAbits.PERR == 1)
      {
          U1STAbits.PERR = 0;
          flagError.UART1ParityError=1;
      }
      if (U1STAbits.FERR == 1)
      {
          U1STAbits.FERR = 0;
          flagError.UART1FramingError=1;
      }
      if (U1STAbits.OERR == 1)
      {
          U1STAbits.OERR = 0;
          flagError.UART1OverrunError=1;
      }
      
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}


void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    //Attention : il faut acquitter l'interrupt en premier !
    //Ne pas modifier l'emplacement de la ligne de code suivante
    IFS0bits.U1TXIF = 0; // clear TX interrupt flag
    
    //On vient de terminer l'envoi d'un caract?re
    if(UART1IsDataReadyInTxBuffer()==TRUE)
    {
        //On renvoie un nouveau caract?re
        UART1WriteFromTxBuffer();
    }
    else
    {
        flagSystem.UART1TxTransmissionEnCours = FALSE;
    }
    //putCFromTxBuffer();
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif    
}
#endif
void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    IFS1bits.U2RXIF = 0; // clear RX interrupt flag

        //Hardware Rx FIFO de 4 octets, donc on verifie de recuperer tout les octets
        while(U2STAbits.URXDA)
        {
            #ifdef USE_UART2_CIRCULAR_BUFFER
                UART2ReadToRxBuffer();
            #endif
        }

//    ProcessUart2ReceiveChar();
    

    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}

#ifndef USE_DMA
void __attribute__((__interrupt__, no_auto_psv)) _U2TXInterrupt(void)
{    
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    IFS1bits.U2TXIF = 0; // clear TX interrupt flag
        //On vient de terminer l'envoi d'un caract?re
    if(UART2IsDataReadyInTxBuffer()==TRUE)
    {
        //On renvoie un nouveau caract?re
        UART2WriteFromTxBuffer();
    }
    else
    {
        flagSystem.UART2TxTransmissionEnCours = FALSE;
    }
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}
#endif

void __attribute__((__interrupt__, no_auto_psv)) _U2ErrInterrupt(void)//ISR2(_U1ErrInterrupt)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    IFS4bits.U2EIF = 0; // Clear the UART2 Error Interrupt Flag
      if (U2STAbits.PERR == 1)
      {
          U2STAbits.PERR = 0;
          flagError.UART2ParityError=1;
      }
      if (U2STAbits.FERR == 1)
      {
          U2STAbits.FERR = 0;
          flagError.UART2FramingError=1;
      }
      if (U2STAbits.OERR == 1)
      {
          U2STAbits.OERR = 0;
          flagError.UART2OverrunError=1;
      }

    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}


void __attribute__((__interrupt__, no_auto_psv)) _U3ErrInterrupt(void)//ISR2(_U1ErrInterrupt)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    IFS5bits.U3EIF = 0; // Clear the UART3 Error Interrupt Flag
    if (U3STAbits.PERR == 1)
    {
      U3STAbits.PERR = 0;
      flagError.UART3ParityError=1;
    }
    if (U3STAbits.FERR == 1)
    {
      U3STAbits.FERR = 0;
      flagError.UART3FramingError=1;
    }
    if (U3STAbits.OERR == 1)
    {
      U3STAbits.OERR = 0;
      flagError.UART3OverrunError=1;
    }

    //LED_ERR=1;
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}

void __attribute__((__interrupt__, no_auto_psv)) _U3RXInterrupt(void)
{

    IFS5bits.U3RXIF = 0; // clear RX interrupt flag
    //On verifie que l'on ai pas eu d'overflow du buffer
    if(U3STAbits.OERR == 1)
    {
     U3STAbits.OERR = 0; // Clear Overrun Error to receive data
    }
    //Si on a pas d'erreur
    else if ( (U3STAbits.FERR ==0) &&(U3STAbits.PERR ==0) )    //
    {
        //Hardware Rx FIFO de 4 octets, donc on verifie de recuperer tout les octets
        while(U3STAbits.URXDA)
        {
            #ifdef USE_UART3
                UART3ReadToRxBuffer();
            #endif
            /*
            //Des donn?es sont dispo dans le buffer de r?ception
            ProcessUartReceiveChar();
            */
        }
    }
    else
    {
        //LED_ERR=1;
    }
    _LED_B1=!_LED_B1;
}

extern volatile BOOL UART3TxTransmissionEnCours;
void __attribute__((__interrupt__, no_auto_psv)) _U3TXInterrupt(void)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    //Attention : il faut acquitter l'interrupt en premier !
    //Ne pas modifier l'emplacement de la ligne de code suivante
    IFS5bits.U3TXIF = 0; // clear TX interrupt flag
    #ifdef USE_UART3_CIRCULAR_BUFFER
        //On vient de terminer l'envoi d'un caract?re
        if(UART3IsDataReadyInTxBuffer()==TRUE)
        {
            //On renvoie un nouveau caract?re
            UART3WriteFromTxBuffer();
        }
        else
        {
            UART3TxTransmissionEnCours = 0;
        }
    #endif
    //putCFromTxBuffer();
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}

void __attribute__((__interrupt__, no_auto_psv)) _U4ErrInterrupt(void)//ISR2(_U1ErrInterrupt)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    IFS5bits.U4EIF = 0; // Clear the UART3 Error Interrupt Flag
    if (U4STAbits.PERR == 1)
    {
      U4STAbits.PERR = 0;
      flagError.UART4ParityError=1;
    }
    if (U4STAbits.FERR == 1)
    {
      U4STAbits.FERR = 0;
      flagError.UART4FramingError=1;
    }
    if (U4STAbits.OERR == 1)
    {
      U4STAbits.OERR = 0;
      flagError.UART4OverrunError=1;
    }
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}

void __attribute__((__interrupt__, no_auto_psv)) _U4RXInterrupt(void)
{

    IFS5bits.U4RXIF = 0; // clear RX interrupt flag
    //On verifie que l'on ai pas eu d'overflow du buffer
    if(U4STAbits.OERR == 1)
    {
     U4STAbits.OERR = 0; // Clear Overrun Error to receive data
    }
    //Si on a pas d'erreur
    else if ( (U4STAbits.FERR ==0) &&(U4STAbits.PERR ==0) )    //
    {
        //Hardware Rx FIFO de 4 octets, donc on verifie de recuperer tout les octets
        while(U4STAbits.URXDA)
        {
            #ifdef USE_UART4
                UART4ReadToRxBuffer();
            #endif
        }
    }
    else
    {
        //LED_ERR=1;
    }
    _LED_B1=!_LED_B1;
}

extern volatile BOOL UART4TxTransmissionEnCours;
void __attribute__((__interrupt__, no_auto_psv)) _U4TXInterrupt(void)
{
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_1;
    #endif
    //Attention : il faut acquitter l'interrupt en premier !
    //Ne pas modifier l'emplacement de la ligne de code suivante
    IFS5bits.U4TXIF = 0; // clear TX interrupt flag
    #ifdef USE_UART4_CIRCULAR_BUFFER
        //On vient de terminer l'envoi d'un caract?re
        if(UART4IsDataReadyInTxBuffer()==TRUE)
        {
            //On renvoie un nouveau caract?re
            UART4WriteFromTxBuffer();
        }
        else
        {
            UART4TxTransmissionEnCours = 0;
        }
    #endif
    #ifdef DEBUG_INTERRUPT
        DEBUG = DEBUG_0;
    #endif
}
// </editor-fold>

void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0; // clear DMA0 interrupt flag
}
void __attribute__((__interrupt__, no_auto_psv)) _DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0; // clear DMA1 interrupt flag
    
    _LED_V1=!_LED_V1;
    
}


extern uint16_t can1MotBuff0Watchdog;
extern uint16_t can1MotBuff1Watchdog;
extern uint16_t can1MotBuff2Watchdog;
extern uint16_t can1MotBuff3Watchdog;
void __attribute__((__interrupt__, no_auto_psv)) _C1Interrupt(void)
{
    if (C1INTFbits.TBIF) C1INTFbits.TBIF = 0;
    if (C1INTFbits.RBIF) 
    { 
        C1INTFbits.RBIF = 0; // Add code to read buffer 10
        if(C1RXFUL1bits.RXFUL10)
        {
            C1RXFUL1bits.RXFUL10 = 0; 
            can1MotBuff0Watchdog++;
            CAN_ReadValuesFromBuffer0();
        }
        if(C1RXFUL1bits.RXFUL11)
        {
            C1RXFUL1bits.RXFUL11 = 0; 
            can1MotBuff1Watchdog++;
            CAN_ReadValuesFromBuffer1();
        }
        if(C1RXFUL1bits.RXFUL12)
        {
            C1RXFUL1bits.RXFUL12 = 0;
            can1MotBuff2Watchdog++;
            CAN_ReadValuesFromBuffer2();
        }
        if(C1RXFUL1bits.RXFUL13)
        {
            C1RXFUL1bits.RXFUL13 = 0;
            can1MotBuff3Watchdog++;
            CAN_ReadValuesFromBuffer3();
        }
    }
    if(C1RXOVF1bits.RXOVF10)
        C1RXOVF1bits.RXOVF10=0;
        if(C1RXOVF1bits.RXOVF11)
        C1RXOVF1bits.RXOVF11=0;
    if (C1INTFbits.WAKIF) C1INTFbits.WAKIF = 0; // Add wake-up handler code
    if (C1INTFbits.ERRIF) C1INTFbits.ERRIF = 0; // Add error handler code
    if (C1INTFbits.IVRIF) C1INTFbits.IVRIF = 0;
    if(C1INTFbits.RBOVIF)C1INTFbits.RBOVIF=0;
    IFS2bits.C1IF = 0; // Clear CAN1 interrupt flag before returning
}