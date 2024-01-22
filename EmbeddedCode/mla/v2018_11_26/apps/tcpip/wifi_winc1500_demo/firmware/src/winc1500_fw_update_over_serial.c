/*******************************************************************************
   File Name:
    winc1500_fw_update_over_serial.c

  Summary:
    WINC1500 firmware update demo

  Description:
    See WINC1500 Getting Started Guide
*******************************************************************************/

//DOM-IGNORE-BEGIN
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

//==============================================================================
// INCLUDES
//==============================================================================

#include "winc1500_api.h"
#include "demo_config.h"
#include "wf_spi.h"
#include "wf_spi_flash.h"

#if defined(USING_FW_UPDATE_OVER_SERIAL)

#define SERIAL_MAX_TRANSFER_SIZE 256

#define READ_REG	0
#define WRITE_REG	1
#define READ_BUFF	2
#define WRITE_BUFF	3
#define RESET		4
#define RECONFIGURE_UART	5

#define USART_CMD_HDR_LENGTH	sizeof(s_cmd_hdr)
#define SPI_TRANSFER_SIZE	512

/** UART COMMAND */
enum nm_usart_event_types {
	USART_PKT_RECEIVED = 0,
	USART_PKT_TRANSMITTED,
	USART_ERROR_ON_RECEPTION,
};

enum nm_usart_cmd_process_states {
	INIT = 0,
	WAIT_SYNC,
	WAITING,
	COLLECTING_HDR,
	COLLECTING_PAYLOAD,
	PROCESSING,
};

typedef struct s_cmd_hdr_t {
	unsigned long cmd;
	unsigned long addr;
	unsigned long val;
}s_cmd_hdr;

static uint16_t s_cmd_recv_buffer[8];
static uint16_t s_payload_buffer[512];
static uint8_t s_tx_buffer[16];
static uint8_t s_command_pending = 0;
static uint8_t *s_cmd_buf;
static s_cmd_hdr s_cmd;
static uint16_t s_next_rx_length = 0;
static uint8_t *s_pkt = NULL;
static uint8_t s_prot_status = INIT;
static uint8_t s_new_state = INIT;
static uint8_t s_change_state = 0;
static uint8_t s_reconfig = 0;


void ChipHardwareReset(void)
{
    m2mStub_PinSet_CE(M2M_WIFI_PIN_LOW);
    m2mStub_PinSet_RESET(M2M_WIFI_PIN_LOW);
    DelayMs(100);
    m2mStub_PinSet_CE(M2M_WIFI_PIN_HIGH);
    DelayMs(10);
    m2mStub_PinSet_RESET(M2M_WIFI_PIN_HIGH);
    DelayMs(10); 
}

void console_write_data(uint16_t length, uint8_t *p_buf)
{
    m2m_wifi_console_write_data(length, p_buf);
}

void console_read_data(uint16_t length, uint8_t *p_buf)
{
    int i;
    
    for (i = 0; i < length; i++)
    {        
        p_buf[i] = m2m_wifi_console_read_data();
    }
}

static void assert_UpdateBridge(bool status, char *message)
{
    if (status == false)
    {
        printf("Assert happen!\r\n");
        printf("%s\r\n", message);
        while (1)
            ;
    }
}

static void console_read_data2(uint16_t size, void *buf)
{
	static uint16_t offset = 0;

	s_next_rx_length = size;

	if (buf) 
    {
    	s_pkt = buf;
    	offset = size;
    } 
    else 
    {
    	s_pkt += offset;
    	offset += size;
    }

    console_read_data(size, s_pkt);

	if (s_change_state) 
    {
    	s_prot_status = s_new_state;
    	s_change_state = 0;
     }
}

static void console_write_data2(uint16_t size, uint8_t *buf)
{
    console_write_data(size, buf);

	if (s_change_state) 
    {
    	s_prot_status = s_new_state;
    	s_change_state = 0;
     }

}

static void nm_usart_send_regval2(uint8_t *tx_data,uint16_t length)
{
	uint8_t temp,i,j;
	for(i = 0, j = (length - 1); i < j; i++, j--) 
    {
    	temp = tx_data[i];
    	tx_data[i] = tx_data[j];
    	tx_data[j] = temp;
    }
	console_write_data2(length, tx_data);
}

static void nm_usart_protocol_handler(enum nm_usart_event_types event_name)
{
	static uint16_t payload_length = 0;
	uint8_t checksum = 0;
	uint8_t i;

	switch(s_prot_status) {
	case INIT:
    	if((event_name == USART_PKT_RECEIVED) && (s_next_rx_length == 1)) 
        {
        	if((s_pkt[0] == 0x12))
            {
            	s_prot_status = WAIT_SYNC;
            	s_cmd_recv_buffer[0] = 0xFF;
            	s_tx_buffer[0] = 0x5B;
            	console_write_data2(1, s_tx_buffer);
            	console_read_data2(1, s_cmd_recv_buffer);
            }
        	else 
            {
            	s_tx_buffer[0] = s_pkt[0];
            	console_write_data2(1, s_tx_buffer);
            	console_read_data2(1, s_cmd_recv_buffer);
            }
        }
    	else 
        {
        	s_cmd_recv_buffer[0] = 0xFF;
        	s_tx_buffer[0] = 0xEA;
        	console_write_data2(1, s_tx_buffer);
        	console_read_data2(1, s_cmd_recv_buffer);
        }
    	break;
	case WAIT_SYNC:
    	if(event_name == USART_PKT_RECEIVED) 
        {
        	if(s_pkt[0] == 0xA5) 
            {
            	uint8_t * s_cmd_recv_buffer_u8 = (uint8_t*)&s_cmd_recv_buffer[0];
            	s_prot_status = WAITING;
            	s_cmd_recv_buffer_u8[4] = 0xFF;
            	console_read_data2(1, &s_cmd_recv_buffer[2]);
            }
        	else if(s_pkt[0] == 0x12)  //uart identification command
            {    
            	s_tx_buffer[0] = 0x5B;
            	s_cmd_recv_buffer[0] = 0xFF;
            	console_write_data2(1, s_tx_buffer);
            	console_read_data2(1, s_cmd_recv_buffer);
            }
        	else 
            {
            	if(!s_reconfig) 
                {
                	s_tx_buffer[0] = 0x5A;
                	s_cmd_recv_buffer[0] = 0xFF;
                	console_write_data2(1, s_tx_buffer);
                	console_read_data2(1, s_cmd_recv_buffer);
                }
            	else 
                {
                	console_read_data2(1, s_cmd_recv_buffer);
                }
            }
        }
        	break;
	case WAITING:
    	if(event_name == USART_PKT_RECEIVED) 
        {
        	s_prot_status = COLLECTING_HDR;
        	s_cmd_buf = s_pkt;
        	console_read_data2(USART_CMD_HDR_LENGTH - 1, NULL);
        }
    	else 
        {
        	s_prot_status = WAIT_SYNC;
        	s_tx_buffer[0] = 0xEA;
        	console_write_data2(1, s_tx_buffer);
        	console_read_data2(1, s_cmd_recv_buffer);
        }
    	break;
	case COLLECTING_HDR:
    	if(event_name == USART_PKT_RECEIVED) 
        {
            //Verify check sum
        	for(i = 0; i < (USART_CMD_HDR_LENGTH); i++) 
            {
            	checksum ^= *(((uint8_t *)s_cmd_buf) + i);
            }
        	if(checksum != 0) 
            {
            	s_prot_status = WAIT_SYNC;
            	s_cmd_recv_buffer[0] = 0xFF;
            	s_tx_buffer[0] = 0x5A;
            	console_write_data2(1, s_tx_buffer);
            	console_read_data2(1, s_cmd_recv_buffer);
            }
        	else 
            {
            	memcpy(&s_cmd, s_cmd_buf, sizeof(s_cmd_hdr));
                //Process the Command.
            	if((s_cmd.cmd & 0xFF) == WRITE_BUFF) 
                {
                	s_prot_status = COLLECTING_PAYLOAD;
                	payload_length = (s_cmd.cmd >> 16) & 0xFFFF;
                	s_tx_buffer[0] = 0xAC;
                	console_write_data2(1, s_tx_buffer);
                	console_read_data2(payload_length, (uint8_t *)s_payload_buffer);
                } 
            	else if((s_cmd.cmd & 0xFF) == WRITE_REG) 
                {
                	s_command_pending = 1;
                	s_prot_status = PROCESSING;
                }
            	else 
                {
                	s_command_pending = 1;
                	s_change_state = 1;
                	s_new_state = PROCESSING;
                	s_tx_buffer[0] = 0xAC;
                	console_write_data2(1, s_tx_buffer);
                }
            }
        }
    	else if(event_name == USART_ERROR_ON_RECEPTION) 
        {
        	assert_UpdateBridge(false, (char*)"Should not come here.");
        }
    	break;
	case COLLECTING_PAYLOAD:
    	if((event_name == USART_PKT_RECEIVED) && (s_next_rx_length == payload_length)) 
        {
        	s_command_pending = 1;
        	s_prot_status = PROCESSING;
        }
    	else if(event_name == USART_ERROR_ON_RECEPTION) 
        {
        	s_prot_status = WAIT_SYNC;
        	s_tx_buffer[0] = 0xEA;
        	s_cmd_recv_buffer[0] = 0xFF;
        	console_write_data2(1, s_tx_buffer);
        	console_read_data2(1, s_cmd_recv_buffer);
        }
    	else 
        {
        	s_prot_status = WAIT_SYNC;
        	s_tx_buffer[0] = 0x5A;
        	s_cmd_recv_buffer[0] = 0xFF;
        	console_write_data2(1, s_tx_buffer);
        	console_read_data2(1, s_cmd_recv_buffer);
        }
    	break;
	default:
    	s_prot_status = WAIT_SYNC;        
    	break;
    }
}

static int8_t enter_wifi_firmware_download(void)
{
	int8_t ret = 0;
	static uint8_t done_programming = 0;

	nm_drv_init_download_mode();

    //Program the WiFi chip here

	console_read_data2(1, s_cmd_recv_buffer);

	while(!done_programming) 
    {

    	nm_usart_protocol_handler(USART_PKT_RECEIVED);
        
    	if(s_command_pending && (s_prot_status == PROCESSING)) 
        {
        	uint32_t temp;
        	switch((s_cmd.cmd) & 0xFF) {
                //Forward it to SERCOM0 SPI
            	case READ_REG:
                    //Transalate it to SPI Read register
                	temp = nm_read_reg(s_cmd.addr);
                	s_tx_buffer[0] = (uint8_t)(temp >> 0);
                	s_tx_buffer[1] = (uint8_t)(temp >> 8);
                	s_tx_buffer[2] = (uint8_t)(temp >> 16);
                	s_tx_buffer[3] = (uint8_t)(temp >> 24);
                	s_prot_status = WAIT_SYNC;
                	nm_usart_send_regval2(&s_tx_buffer[0],sizeof(uint32_t));
                	console_read_data2(1, s_cmd_recv_buffer);
                	break;
            	case WRITE_REG:
                    //Transalate it to SPI Write register
                	nm_write_reg(s_cmd.addr,s_cmd.val);
                	s_tx_buffer[0] = 0xAC;
                	s_prot_status = WAIT_SYNC;
                	console_write_data2(1, s_tx_buffer);
                	console_read_data2(1, s_cmd_recv_buffer);
                	break;
            	case READ_BUFF:
                //Transalate it to SPI Read buffer
                	nm_read_block(s_cmd.addr, (uint8_t *)&s_payload_buffer[0],((s_cmd.cmd >> 16) & 0xFFFF));
                	s_prot_status = WAIT_SYNC;
                	console_write_data2(((s_cmd.cmd >> 16) & 0xFFFF), (uint8_t *)s_payload_buffer);
                	console_read_data2(1, s_cmd_recv_buffer);
                	break;
            	case WRITE_BUFF:
                    //Transalate it to SPI Write buffer
                	nm_write_block(s_cmd.addr, (uint8_t *)&s_payload_buffer[0],((s_cmd.cmd >> 16) & 0xFFFF));
                	s_tx_buffer[0] = 0xAC;
                	s_prot_status = WAIT_SYNC;
                	console_write_data2(1, s_tx_buffer);
                	console_read_data2(1, s_cmd_recv_buffer);
                	break;
            	case RECONFIGURE_UART:
                    // Send the ack back
                	s_prot_status = WAIT_SYNC;
                	s_reconfig = 1;
                	console_read_data2(1, s_cmd_recv_buffer);
                	break;
            	case 10:
                	s_prot_status = WAIT_SYNC;
                	console_write_data2(1, s_tx_buffer);
                	console_read_data2(1, s_cmd_recv_buffer);
                	assert_UpdateBridge(false, (char*)"Should not come here.");
                	break;
            	default:
                	break;
            }
        	s_command_pending = 0;
        }
    }
	return ret;
}

void winc1500_fw_update(void)
{
    ChipHardwareReset();
    DelayMs(500);
    enter_wifi_firmware_download();
}

// application state machine called from main()
void ApplicationTask(void)
{
    winc1500_fw_update();
}

#endif // USING_FW_UPDATE_OVER_SERIAL