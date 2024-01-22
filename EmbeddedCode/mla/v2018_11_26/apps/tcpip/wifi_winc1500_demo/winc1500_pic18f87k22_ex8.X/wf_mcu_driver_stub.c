/*==============================================================================
Copyright 2016 Microchip Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "mcc.h"
#include "winc1500_api.h"

uint32_t GetOneMsCounter(void); 

#define SPI_RX_IN_PROGRESS  0

#define WaitForDataByte()    while(SSP1STATbits.BF == SPI_RX_IN_PROGRESS) 

#if defined(USING_PICTAIL)
void m2mStub_PinSet_CE(t_m2mWifiPinAction action)
{
    if (action == M2M_WIFI_PIN_LOW)
    {
        IO_RB2_CHIP_EN_SetLow();
    }
    else
    {
        IO_RB2_CHIP_EN_SetHigh();
    }
}

void m2mStub_PinSet_RESET(t_m2mWifiPinAction action)
{
    if (action == M2M_WIFI_PIN_LOW)
    {
        IO_RB1_RESET_N_SetLow();
    }
    else
    {
        IO_RB1_RESET_N_SetHigh();
    }
}

void m2mStub_PinSet_SPI_SS(t_m2mWifiPinAction action)
{
     if (action == M2M_WIFI_PIN_LOW)
    {
         IO_RC2_SPI_SS_SetLow();
    }
    else
    {
         IO_RC2_SPI_SS_SetHigh();
    }
}
#elif defined(USING_CLICK_BOARD)
void m2mStub_PinSet_CE(t_m2mWifiPinAction action)
{
    if (action == M2M_WIFI_PIN_LOW)
    {
        IO_RC0_CHIP_EN_SetLow();
    }
    else   
    {
        IO_RC0_CHIP_EN_SetHigh();
    }
}

void m2mStub_PinSet_RESET(t_m2mWifiPinAction action)
{
    if (action == M2M_WIFI_PIN_LOW)
    {
        IO_RD7_RESET_N_SetLow();
    }
    else
    {
        IO_RD7_RESET_N_SetHigh();
    }
}

void m2mStub_PinSet_SPI_SS(t_m2mWifiPinAction action)
{
     if (action == M2M_WIFI_PIN_LOW)
    {
         IO_RE2_SPI_SS_SetLow();
    }
    else
    {
         IO_RE2_SPI_SS_SetHigh();
    }
}
#endif

uint32_t m2mStub_GetOneMsTimer(void)
{
    return GetOneMsCounter();   // function added to MCC generated timer1 code
}

void m2mStub_EintEnable(void)
{
    INTCONbits.INT0IE = 1;
}

void m2mStub_EintDisable(void)
{
    INTCONbits.INT0IE = 0;
}

/*******************************************************************************
  Function:
    void WFSpiTxRx(void)

  Summary:
    Transmits and receives SPI bytes

  Description:
    Transmits and receives N bytes of SPI data.

  Precondition:
    None

  Parameters:
    p_txBuf - pointer to SPI tx data
    txLen   - number of bytes to Tx
    p_rxBuf - pointer to where SPI rx data will be stored
    rxLen   - number of SPI rx bytes caller wants copied to p_rxBuf

  Returns:
    None

  Remarks:
    Will clock out the larger of txLen or rxLen, and pad if necessary.
 *******************************************************************************/
void m2mStub_SpiTxRx(uint8_t *p_txBuf,
                    uint16_t txLen,
                    uint8_t *p_rxBuf,
                    uint16_t rxLen)
{
    uint16_t byteCount;
    uint16_t i;
    
    /* total number of byte to clock is whichever is larger, txLen or rxLen */
    byteCount = (txLen >= rxLen) ? txLen : rxLen;

    for (i = 0; i < byteCount; ++i)
    {
        /* if still have bytes to transmit from tx buffer */
        if (txLen > 0)
        {
        
            SSP1CON1bits.WCOL = 0;      // Clear the Write Collision flag, to allow writing
            SSP1BUF = *p_txBuf++;
            --txLen;
        }
        /* else done writing bytes out from tx buffer */
        else
        {
            SSP1BUF = 0x00; /* clock out a "don't care" byte */
        }

        /* wait until tx/rx byte to completely clock out */
        WaitForDataByte();

        /* if still have bytes to read into rx buffer */
        if (rxLen > 0)
        {
            *p_rxBuf++ = SSP1BUF;
            --rxLen;
        }
            /* else done reading bytes into rx buffer */
        else
        {
            SSP1BUF; /* read and throw away byte */
        }
    } /* end for loop */
}

void SpiInit(void)
{
    // SMP Middle; CKE Active to Idle; 
    SSP1STAT = 0x40;
    
    // SSPEN enabled; WCOL no_collision; CKP Idle:Low, Active:High; SSPM FOSC/4; SSPOV no_overflow; 
    SSP1CON1 = 0x20;
    
    // SSP1ADD 0; 
    SSP1ADD = 0x00;  
}

#if defined(M2M_ENABLE_SPI_FLASH)
void m2m_wifi_console_write_data(uint16_t length, uint8_t *p_buf)
{
    uint16_t i;
    
    for (i = 0; i < length; ++i)
    {
        EUSART1_Write(p_buf[i]);
    }
}

uint8_t m2m_wifi_console_read_data(void)
{
    return EUSART1_Read();
}

bool m2m_wifi_console_is_read_data(void)
{
    if (PIR1bits.RC1IF == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif // M2M_ENABLE_SPI_FLASH