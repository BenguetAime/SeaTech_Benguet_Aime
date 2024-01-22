/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    ftp_client_demo.c

  Summary:
    FTP Client Demo

  Description:


 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#include <stdint.h>
#include "system_config.h"
#include "tcpip/tcpip.h"

#ifdef STACK_USE_FTP_CLIENT

#if defined(WF_DEBUG)
#define WF_MODULE_NUMBER WF_MODULE_FTP_CLIENT_DEMO
#endif

// Defines the port to be accessed for this application
static uint16_t Port_FtpCltCmd = 21;
static uint16_t Port_FtpCltData = 59136;

// Defines the server to be accessed for this application
static uint8_t ServerName[] = ""; // type the target FTP server name
static uint8_t UserName[] = ""; // type the user name
static uint8_t PassWord[] = ""; //type the password
static uint8_t FileName[] = ""; //type the filename

static void AU_print_string(uint8_t *buf, uint8_t length)
{
    int i;

    for (i = 0; i < length; i++)
        putcUART(buf[i]);

}

/*****************************************************************************
  Function:
    void FTPClient(void)

  Summary:

  Description:

  Precondition:
    TCP is initialized.

  Parameters:
    None

  Returns:
    None
 ***************************************************************************/
void FTPClient(void)
{
    short w;
    uint8_t vBuffer[32];
    static uint32_t Timer;
    static TCP_SOCKET Socket_FtpCltCmd = INVALID_SOCKET;
    static TCP_SOCKET Socket_FtpCltData = INVALID_SOCKET;
    uint16_t lenB;

    typedef enum _FTP_RESPONSE {
        FTP_CLT_RESP_BANNER,
        FTP_CLT_RESP_USER_OK,
        FTP_CLT_RESP_PASS_OK,
        FTP_CLT_RESP_QUIT_OK,
        FTP_CLT_RESP_STOR_OK,
        FTP_CLT_RESP_UNKNOWN,
        FTP_CLT_RESP_LOGIN,
        FTP_CLT_RESP_DATA_OPEN,
        FTP_CLT_RESP_DATA_READY,
        FTP_CLT_RESP_DATA_CLOSE,
        FTP_CLT_RESP_DATA_NO_SOCKET,
        FTP_CLT_RESP_PWD,
        FTP_CLT_RESP_OK,

        FTP_RESP_NONE // This must always be the last
        // There is no corresponding string.
    } FTP_RESPONSE;

    char * FtpCltResponseString[] = {
        "220 ", // FTP_CLT_RESP_BANNER // Ready\r\n
        "331 ", // FTP_CLT_RESP_USER_OK // Password required\r\n
        "230 ", // FTP_CLT_RESP_PASS_OK // Logged in\r\n
        "221 ", // FTP_CLT_RESP_QUIT_OK // Bye\r\n
        "500 ", // FTP_CLT_RESP_STOR_OK
        "502 ", // FTP_CLT_RESP_UNKNOWN // Not implemented\r\n
        "530 ", // FTP_CLT_RESP_LOGIN // Login required\r\n
        "150 ", // FTP_CLT_RESP_DATA_OPEN // Transferring data...\r\n
        "125 ", // FTP_CLT_RESP_DATA_READY // Done\r\n
        "226 ", // FTP_CLT_RESP_DATA_CLOSE // Transfer Complete\r\n
        "425 ", // FTP_CLT_RESP_DATA_NO_SOCKET // Can't create data socket.\r\n
        "257 ", // FTP_CLT_RESP_PWD //\"/\" is current\r\n
        "200 " // FTP_CLT_RESP_OK // Ok\r\n
    };

    static enum _FtpClientCmdState {
        SM_FTP_CLIENT_COMMAND_HOME = 0,
        SM_FTP_CLIENT_COMMAND_SOCKET_OBTAINED,
        SM_FTP_CLIENT_COMMAND_USRNAME,
        SM_FTP_CLIENT_COMMAND_PASSWORD1,
        SM_FTP_CLIENT_COMMAND_PASSWORD2,
        SM_FTP_CLIENT_COMMAND_LOGIN1,
        SM_FTP_CLIENT_COMMAND_LOGIN2,
        SM_FTP_CLIENT_COMMAND_DATA1,
        SM_FTP_CLIENT_COMMAND_DATA2,
        SM_FTP_CLIENT_COMMAND_DATA3,
        SM_FTP_CLIENT_COMMAND_DATA4,
        SM_FTP_CLIENT_COMMAND_QUIT1,
        SM_FTP_CLIENT_COMMAND_QUIT2,
        SM_FTP_CLIENT_COMMAND_DISCONNECT,
        SM_FTP_CLIENT_COMMAND_DONE
    } FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DONE;

    static enum _FtpClientDataState {
        SM_FTP_CLIENT_DATA_HOME = 0,
        SM_FTP_CLIENT_DATA_WAIT,
        SM_FTP_CLIENT_DATA_DATA,
        SM_FTP_CLIENT_DATA_DISCONNECT,
        SM_FTP_CLIENT_DATA_DONE
    } FtpClientDataState = SM_FTP_CLIENT_DATA_DONE;

    switch (FtpClientCmdState) {
    case SM_FTP_CLIENT_COMMAND_HOME:
        // Connect a socket to the remote TCP server
        Socket_FtpCltCmd = TCPOpen((uint32_t) ((unsigned int) &ServerName[0]), TCP_OPEN_RAM_HOST, Port_FtpCltCmd, TCP_PURPOSE_GENERIC_TCP_CLIENT);
        if (Socket_FtpCltCmd == INVALID_SOCKET)
            break;

#if defined(STACK_USE_UART)
        putrsUART((ROM char *) "\r\n\r\nFTP Client...\r\n");
#endif

        FtpClientCmdState++;
        Timer = TickGet();
        break;

    case SM_FTP_CLIENT_COMMAND_SOCKET_OBTAINED:
        // Wait for the remote server to accept our connection request
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            // Time out if too much time is spent in this state
            if (TickGet() - Timer > 5 * TICK_SECOND) {
                // Close the socket so it can be used by other modules
                TCPDisconnect(Socket_FtpCltCmd);
                Socket_FtpCltCmd = INVALID_SOCKET;
                FtpClientCmdState--;
            }
            break;
        }

        Timer = TickGet();
        w = TCPIsGetReady(Socket_FtpCltCmd);
        if (w == 0)
            break;

        if (0xFFFFu != TCPFindROMArray(Socket_FtpCltCmd, (ROM uint8_t *) FtpCltResponseString[FTP_CLT_RESP_BANNER]/*"220 "*/, 4, 0, false)) {
            putsUART("\r\n@Receive:220\r\n");
            w = TCPIsGetReady(Socket_FtpCltCmd);
            while (w > 0) {
                lenB = TCPGetArray(Socket_FtpCltCmd, vBuffer, ((w <= sizeof (vBuffer)) ? w : sizeof (vBuffer)));
                AU_print_string(vBuffer, lenB);
                w -= lenB;
            }

            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_USRNAME;
        }
        break;

    case SM_FTP_CLIENT_COMMAND_USRNAME:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }
        FtpClientDataState = SM_FTP_CLIENT_DATA_HOME;
        if (TCPIsPutReady(Socket_FtpCltCmd) < 30u)
            break;

        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "USER ");
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) UserName);
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "\r\n");
        // Send the packet
        TCPFlush(Socket_FtpCltCmd);
        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_PASSWORD1;
        break;

    case SM_FTP_CLIENT_COMMAND_PASSWORD1:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }

        w = TCPIsGetReady(Socket_FtpCltCmd);
        if (w == 0)
            break;

        if (0xffffu != TCPFindROMArray(Socket_FtpCltCmd, (ROM uint8_t *) /* 331 */ FtpCltResponseString[FTP_CLT_RESP_USER_OK], 4, 0, false)) {
            putsUART("\r\n@Receive:331\r\n");
            w = TCPIsGetReady(Socket_FtpCltCmd);
            while (w > 0) {
                lenB = TCPGetArray(Socket_FtpCltCmd, vBuffer, ((w <= sizeof (vBuffer)) ? w : sizeof (vBuffer)));
                AU_print_string(vBuffer, lenB);
                w -= lenB;
            }

            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_PASSWORD2;
        }

        break;

    case SM_FTP_CLIENT_COMMAND_PASSWORD2:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }

        if (TCPIsPutReady(Socket_FtpCltCmd) < 30u)
            break;

        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "PASS ");
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) PassWord);
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "\r\n");
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) PassWord);
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "\r\n");
        TCPFlush(Socket_FtpCltCmd);
        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_LOGIN1;

        break;

    case SM_FTP_CLIENT_COMMAND_LOGIN1:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }
        w = TCPIsGetReady(Socket_FtpCltCmd);
        if (w == 0)
            break;

        if (0xFFFFu != TCPFindROMArray(Socket_FtpCltCmd, (ROM uint8_t *) /*230*/FtpCltResponseString[FTP_CLT_RESP_PASS_OK], 4, 0, false)) {
            putsUART("\r\n@Receive:230\r\n");
            w = TCPIsGetReady(Socket_FtpCltCmd);
            while (w > 0) {
                lenB = TCPGetArray(Socket_FtpCltCmd, vBuffer, ((w <= sizeof (vBuffer)) ? w : sizeof (vBuffer)));
                AU_print_string(vBuffer, lenB);
                w -= lenB;
            }

            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_LOGIN2;
        }
        break;

    case SM_FTP_CLIENT_COMMAND_LOGIN2:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }
        putsUART("Now, login\n");

        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DATA1;
        break;

    case SM_FTP_CLIENT_COMMAND_DATA1:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }

        if (TCPIsPutReady(Socket_FtpCltCmd) < 30u)
            break;

        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "PORT 10,128,22,61,231,0\r\n");
        TCPFlush(Socket_FtpCltCmd);
        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DATA2;
        break;

    case SM_FTP_CLIENT_COMMAND_DATA2:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }

        w = TCPIsGetReady(Socket_FtpCltCmd);
        if (w == 0)
            break;

        if (0xFFFFu != TCPFindROMArray(Socket_FtpCltCmd, (ROM uint8_t *) /*200*/FtpCltResponseString[FTP_CLT_RESP_OK], 4, 0, false)) {
            putsUART("\r\n@Receive:200\r\n");
            w = TCPIsGetReady(Socket_FtpCltCmd);
            while (w > 0) {
                lenB = TCPGetArray(Socket_FtpCltCmd, vBuffer, ((w <= sizeof (vBuffer)) ? w : sizeof (vBuffer)));
                AU_print_string(vBuffer, lenB);
                w -= lenB;
            }

            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DATA3;
        }
        break;

    case SM_FTP_CLIENT_COMMAND_DATA3:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }

        if (TCPIsPutReady(Socket_FtpCltCmd) < 30u)
            break;

        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "RETR ");
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) FileName);
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "\r\n");
        TCPFlush(Socket_FtpCltCmd);
        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DATA4;
        break;

    case SM_FTP_CLIENT_COMMAND_DATA4:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }

        w = TCPIsGetReady(Socket_FtpCltCmd);
        while (w > 0) {
            lenB = TCPGetArray(Socket_FtpCltCmd, vBuffer, ((w <= sizeof (vBuffer)) ? w : sizeof (vBuffer)));
            AU_print_string(vBuffer, lenB);
            w -= lenB;
        }

        //FtpClientCmdState = SM_FTP_CLIENT_COMMAND_QUIT1;
        break;

    case SM_FTP_CLIENT_COMMAND_QUIT1:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }
        if (TCPIsPutReady(Socket_FtpCltCmd) < 30u)
            break;
        TCPPutROMString(Socket_FtpCltCmd, (ROM uint8_t *) "QUIT\r\n");
        TCPFlush(Socket_FtpCltCmd);
        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_QUIT2;
        break;
    case SM_FTP_CLIENT_COMMAND_QUIT2:
        if (!TCPIsConnected(Socket_FtpCltCmd)) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
            break;
        }
        w = TCPIsGetReady(Socket_FtpCltCmd);
        if (w == 0)
            break;

        if (0xFFFFu != TCPFindROMArray(Socket_FtpCltCmd, (ROM uint8_t *) /*221*/FtpCltResponseString[FTP_CLT_RESP_QUIT_OK], 4, 0, false)) {
            putsUART("\r\n@Receive:221\r\n");
            w = TCPIsGetReady(Socket_FtpCltCmd);
            while (w > 0) {
                lenB = TCPGetArray(Socket_FtpCltCmd, vBuffer, ((w <= sizeof (vBuffer)) ? w : sizeof (vBuffer)));
                AU_print_string(vBuffer, lenB);
                w -= lenB;
            }

            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DISCONNECT;
        }
        break;

    case SM_FTP_CLIENT_COMMAND_DISCONNECT:
        putsUART("\r\nClosed---\r\n");
        TCPDisconnect(Socket_FtpCltCmd);
        Socket_FtpCltCmd = INVALID_SOCKET;
        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_DONE;
        break;

    case SM_FTP_CLIENT_COMMAND_DONE:
        // Do nothing unless the user pushes BUTTON1 and wants to restart the whole connection/download process
        if (BUTTON3_IO == 0u) {
            FtpClientCmdState = SM_FTP_CLIENT_COMMAND_HOME;

        }
        break;
    }

    // FTP Client DATA State
    switch (FtpClientDataState) {
    case SM_FTP_CLIENT_DATA_HOME:
        Socket_FtpCltData = TCPOpen(0, TCP_OPEN_SERVER, /*59136*/ Port_FtpCltData, TCP_PURPOSE_FTP_DATA);
        if (Socket_FtpCltData == INVALID_SOCKET) {
            putsUART("Can not create FTP Data Server\r\n");
            WF_ASSERT(false);
        }
        FtpClientDataState = SM_FTP_CLIENT_DATA_WAIT;
        break;

    case SM_FTP_CLIENT_DATA_WAIT:
        if (TCPIsConnected(Socket_FtpCltData)) {
            putsUART("\r\n!FTP data connected\r\n");
            FtpClientDataState = SM_FTP_CLIENT_DATA_DATA;
        }
        break;

    case SM_FTP_CLIENT_DATA_DATA:
        if (!TCPIsConnected(Socket_FtpCltData)) {
            FtpClientDataState = SM_FTP_CLIENT_DATA_DISCONNECT;
            break;
        }

        w = TCPIsGetReady(Socket_FtpCltData);
        while (w > 0) {
            lenB = TCPGetArray(Socket_FtpCltData, vBuffer, ((w <= sizeof (vBuffer)) ? w : sizeof (vBuffer)));
            AU_print_string(vBuffer, lenB);
            w -= lenB;
        }

        break;

    case SM_FTP_CLIENT_DATA_DISCONNECT:
        putsUART("\r\nFTP Data Disconneted---\r\n");
        FtpClientCmdState = SM_FTP_CLIENT_COMMAND_QUIT1;
        FtpClientDataState = SM_FTP_CLIENT_DATA_DONE;
        break;

    case SM_FTP_CLIENT_DATA_DONE:
        break;
    }
}

#endif
