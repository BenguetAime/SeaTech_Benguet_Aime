/*******************************************************************************
  File Name:
    winc1500_send_email.c

  Summary:
    WINC1500 send email demo.

  Description:
    This demo performs the following steps:
        1) connect to AP,
        2) send email by smtp.google.com, 
 
    The configuration defines for this demo are:    
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security

    The demo uses these callback functions to handle events:
        socket_cb()
        resolve_cb() 
        wifi_cb()
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (c) 2012-2014, Microchip Technology, Inc. <www.microchip.com>
* Contact Microchip for the latest version.
*
* This program is free software; distributed under the terms of BSD
* license:
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//==============================================================================
// INCLUDES
//==============================================================================
#include "winc1500_api.h"
#include "demo_config.h"
#include "wf_common.h"

#if defined(USING_SEND_EMAIL)
/** Wi-Fi Settings */
#define WLAN_SSID                  "DEMO_AP" /**< Destination SSID */
#define WLAN_AUTH                  M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define WLAN_PSK                   "12345678" /**< Password for Destination SSID */

/** Using IP address. */
#define IPV4_BYTE(val, index)           ((val >> (index * 8)) & 0xFF)

/** All SMTP defines */
#define SMTP_BUF_LEN               1024
#define GMAIL_HOST_NAME            "smtp.gmail.com"
#define GMAIL_HOST_PORT            465
#define SENDER_RFC                 "<wifiatmel@gmail.com>" /* Set Sender Email Address */

#define RECIPIENT_RFC              "<wifiatmel@gmail.com>"  /* Set Recipient Email Address */
#define EMAIL_SUBJECT              "Hello from WINC1500!"
#define TO_ADDRESS                 "wifiatmel@gmail.com" /* Set To Email Address */
#define FROM_ADDRESS               "wifiatmel@gmail.com" /* Set From Email Address */
#define FROM_PASSWORD              "atmel123"              /* Set Sender Email Password */
#define EMAIL_MSG                  "This mail is sent from Send Email Example."
#define WAITING_TIME               30000
#define RETRY_COUNT                3

typedef enum {
    SocketInit = 0,
    SocketConnect,
    SocketWaiting,
    SocketComplete,
    SocketError
} eSocketStatus;

typedef enum {
    SMTP_INACTIVE = 0,
    SMTP_INIT,
    SMTP_HELO,
    SMTP_AUTH,
    SMTP_AUTH_USERNAME,
    SMTP_AUTH_PASSWORD,
    SMTP_FROM,
    SMTP_RCPT,
    SMTP_DATA,
    SMTP_MESSAGE_SUBJECT,
    SMTP_MESSAGE_TO,
    SMTP_MESSAGE_FROM,
    SMTP_MESSAGE_CRLF,
    SMTP_MESSAGE_BODY,
    SMTP_MESSAGE_DATAEND,
    SMTP_QUIT,
    SMTP_ERROR
} eSMTPStatus;

/* Initialize error handling flags for smtp state machine */
typedef enum {
    EMAIL_ERROR_FAILED = -1,
    EMAIL_ERROR_NONE = 0,
    EMAIL_ERROR_INIT,
    EMAIL_ERROR_HELO,
    EMAIL_ERROR_AUTH,
    EMAIL_ERROR_AUTH_USERNAME,
    EMAIL_ERROR_AUTH_PASSWORD,
    EMAIL_ERROR_FROM,
    EMAIL_ERROR_RCPT,
    EMAIL_ERROR_DATA,
    EMAIL_ERROR_MESSAGE,
    EMAIL_ERROR_QUIT
} eMainEmailError;

/** Return Codes */
const char cSmtpCodeReady[] = {'2', '2', '0', '\0'};
const char cSmtpCodeOkReply[] = {'2', '5', '0', '\0'};
const char cSmtpCodeIntermedReply[] = {'3', '5', '4', '\0'};
const char cSmtpCodeAuthReply[] = {'3', '3', '4', '\0'};
const char cSmtpCodeAuthSuccess[] = {'2', '3', '5', '\0'};

/** Send Codes */
const char cSmtpHelo[] = {'H', 'E', 'L', 'O', '\0'};
const char cSmtpMailFrom[] = {'M', 'A', 'I', 'L', ' ', 'F', 'R', 'O', 'M', ':', ' ', '\0'};
const char cSmtpRcpt[] = {'R', 'C', 'P', 'T', ' ', 'T', 'O', ':', ' ', '\0'};
const char cSmtpData[] = "DATA";
const char cSmtpCrlf[] = "\r\n";
const char cSmtpSubject[] = "Subject: ";
const char cSmtpTo[] = "To: ";
const char cSmtpFrom[] = "From: ";
const char cSmtpDataEnd[] = {'\r', '\n', '.', '\r', '\n', '\0'};
const char cSmtpQuit[] = {'Q', 'U', 'I', 'T', '\r', '\n', '\0'};
    
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WORKING,  
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

uint32_t g_HostIp = 0;                   // IP address of host
uint8_t g_SocketStatus = SocketInit;
uint8_t gu8SmtpStatus = SMTP_INIT;       // SMTP information
int8_t gs8EmailError = EMAIL_ERROR_NONE; // SMTP email error information
char gcSendRecvBuffer[SMTP_BUF_LEN];     // Send and receive buffer
char gcHandlerBuffer[SMTP_BUF_LEN];      // Handler buffer
char gcUserBasekey[128];                 // Username basekey
char gcPasswordBasekey[128];             // Password basekey
uint8_t gu8RetryCount = 0;               // Retry count

static SOCKET tcp_client_socket = -1;    // TCP client socket handler
static bool gbConnectedWifi = false;     // Wi-Fi status
static bool gbHostIpByName = false;      // Get host IP status
static const char g_ccB64Tbl[64]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void wifi_cb(uint8_t msgType, void *pvMsg);
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);
static void resolve_cb(char *pu8DomainName, uint32_t serverIP);

static void ConvertToBase64(char *pcOutStr, const char *pccInStr, int iLen)
{
    const char *pccIn = (const char *)pccInStr;
    char *pcOut;
    int iCount;
    pcOut = pcOutStr;

    /* Loop in for Multiple of 24Bits and Convert to Base 64 */
    for (iCount = 0; iLen - iCount >= 3; iCount += 3, pccIn += 3) 
    {
        *pcOut++ = g_ccB64Tbl[pccIn[0] >> 2];
        *pcOut++ = g_ccB64Tbl[((pccIn[0] & 0x03) << 4) | (pccIn[1] >> 4)];
        *pcOut++ = g_ccB64Tbl[((pccIn[1] & 0x0F) << 2) | (pccIn[2] >> 6)];
        *pcOut++ = g_ccB64Tbl[pccIn[2] & 0x3f];
    }

    /* Check if String is not multiple of 3 Bytes */
    if (iCount != iLen) 
    {
        unsigned char ucLastByte;

        *pcOut++ = g_ccB64Tbl[pccIn[0] >> 2];
        ucLastByte = ((pccIn[0] & 0x03) << 4);

        if (iLen - iCount > 1) 
        {
            /* If there are 2 Extra Bytes */
            ucLastByte |= (pccIn[1] >> 4);
            *pcOut++ = g_ccB64Tbl[ucLastByte];
            *pcOut++ = g_ccB64Tbl[((pccIn[1] & 0x0F) << 2)];
        } 
        else 
        {
            /* If there is only 1 Extra Byte */
            *pcOut++ = g_ccB64Tbl[ucLastByte];
            *pcOut++ = '=';
        }

        *pcOut++ = '=';
    }

    *pcOut  = '\0';
}
/**
 * \brief Creates and connects to an unsecure socket to be used for SMTP.
 *
 * \param[in] None.
 *
 * \return SOCK_ERR_NO_ERROR if success, -1 if socket create error, SOCK_ERR_INVALID if socket connect error.
 */
static int8_t smtpConnect(void)
{
    struct sockaddr_in addr_in;

    addr_in.sin_family = AF_INET;
    addr_in.sin_port = _htons(GMAIL_HOST_PORT);
    addr_in.sin_addr.s_addr = g_HostIp;

    /* Create secure socket */
    if (tcp_client_socket < 0) 
    {
        tcp_client_socket = socket(AF_INET, SOCK_STREAM, SOCKET_FLAGS_SSL);
    }

    /* Check if socket was created successfully */
    if (tcp_client_socket == -1) 
    {
        printf("socket error.\r\n");
        close(tcp_client_socket);
        return -1;
    }

    /* If success, connect to socket */
    if (connect(tcp_client_socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) 
    {
        printf("connect error.\r\n");
        return SOCK_ERR_INVALID;
    }

    /* Success */
    return SOCK_ERR_NO_ERROR;
}

/**
 * \brief Generates Base64 Key needed for authentication.
 *
 * \param[in] input is the string to be converted to base64.
 * \param[in] basekey1 is the base64 converted output.
 *
 * \return None.
 */
static void generateBase64Key(char *input, char *basekey)
{
    /* In case the input string needs to be modified before conversion, define */
    /*  new string to pass-through Use InputStr and *pIn */
    int16_t InputLen = strlen(input);
    char InputStr[128];
    char *pIn = (char *)InputStr;

    /* Generate Base64 string, right now is only the function input parameter */
    memcpy(pIn, input, InputLen);
    pIn += InputLen;

    /* to64frombits function */
    ConvertToBase64(basekey, (void *)InputStr, InputLen);
}

/**
 * \brief Sends an SMTP command and provides the server response.
 *
 * \param[in] socket is the socket descriptor to be used for sending.
 * \param[in] cmd is the string of the command.
 * \param[in] cmdpara is the command parameter.
 * \param[in] respBuf is a pointer to the SMTP response from the server.
 *
 * \return None.
 */
static void smtpSendRecv(long socket, char *cmd, char *cmdparam, char *respBuf)
{
    uint16_t sendLen = 0;
    memset(gcSendRecvBuffer, 0, sizeof(gcSendRecvBuffer));

    if (cmd != NULL) 
    {
        sendLen = strlen(cmd);
        memcpy(gcSendRecvBuffer, cmd, strlen(cmd));
    }

    if (cmdparam != NULL) 
    {
        memcpy(&gcSendRecvBuffer[sendLen], cmdparam, strlen(cmdparam));
        sendLen += strlen(cmdparam);
    }

    memcpy(&gcSendRecvBuffer[sendLen], cSmtpCrlf, strlen(cSmtpCrlf));
    sendLen += strlen(cSmtpCrlf);
    send(socket, gcSendRecvBuffer, sendLen, 0);

    if (respBuf != NULL) 
    {
        memset(respBuf, 0, SMTP_BUF_LEN);
        recv(socket, respBuf, SMTP_BUF_LEN, 0);
    }
}

/**
 * \brief SMTP state handler.
 *
 * \param[in] None.
 *
 * \return EMAIL_ERROR_NONE if success, EMAIL_ERROR_FAILED if handler error.
 */
static int8_t smtpStateHandler(void)
{
    /* Check for acknowledge from SMTP server */
    switch (gu8SmtpStatus) 
    {
    /* Send Introductory "HELO" to SMTP server */
    case SMTP_HELO:
        smtpSendRecv(tcp_client_socket, (char *)"HELO localhost", NULL, gcHandlerBuffer);
        break;

    /* Send request to server for authentication */
    case SMTP_AUTH:
        smtpSendRecv(tcp_client_socket, (char *)"AUTH LOGIN", NULL, gcHandlerBuffer);
        break;

    /* Handle Authentication with server username */
    case SMTP_AUTH_USERNAME:
        smtpSendRecv(tcp_client_socket, gcUserBasekey, NULL, gcHandlerBuffer);
        break;

    /* Handle Authentication with server password */
    case SMTP_AUTH_PASSWORD:
        smtpSendRecv(tcp_client_socket, gcPasswordBasekey, NULL, gcHandlerBuffer);
        break;

    /* Send source email to the SMTP server */
    case SMTP_FROM:
        smtpSendRecv(tcp_client_socket, (char *)cSmtpMailFrom, (char *)SENDER_RFC, gcHandlerBuffer);
        break;

    /* Send the destination email to the SMTP server */
    case SMTP_RCPT:
        smtpSendRecv(tcp_client_socket, (char *)cSmtpRcpt, (char *)RECIPIENT_RFC, gcHandlerBuffer);
        break;

    /* Send the "DATA" message to the server */
    case SMTP_DATA:
        smtpSendRecv(tcp_client_socket, (char *)cSmtpData, NULL, gcHandlerBuffer);
        break;

    /* Send actual Message, preceded by From, To and Subject */
    case SMTP_MESSAGE_SUBJECT:
        /* Start with E-Mail's "Subject:" field */
        smtpSendRecv(tcp_client_socket, (char *)cSmtpSubject, (char *)EMAIL_SUBJECT, NULL);
        break;

    case SMTP_MESSAGE_TO:
        /* Add E-mail's "To:" field */
        printf("Recipient email address is %s\r\n", (char *)TO_ADDRESS);
        smtpSendRecv(tcp_client_socket, (char *)cSmtpTo, (char *)TO_ADDRESS, NULL);
        break;

    case SMTP_MESSAGE_FROM:
        /* Add E-mail's "From:" field */
        smtpSendRecv(tcp_client_socket, (char *)cSmtpFrom, (char *)FROM_ADDRESS, NULL);
        break;

    case SMTP_MESSAGE_CRLF:
        /* Send CRLF */
        send(tcp_client_socket, (char *)cSmtpCrlf, strlen(cSmtpCrlf), 0);
        break;

    case SMTP_MESSAGE_BODY:
        /* Send body of message */
        smtpSendRecv(tcp_client_socket, (char *)EMAIL_MSG, NULL, NULL);
        break;

    case SMTP_MESSAGE_DATAEND:
        /* End Message */
        smtpSendRecv(tcp_client_socket, (char *)cSmtpDataEnd, NULL, gcHandlerBuffer);
        break;

    case SMTP_QUIT:
        send(tcp_client_socket, (char *)cSmtpQuit, strlen(cSmtpQuit), 0);
        break;

    /* Error Handling for SMTP */
    case SMTP_ERROR:
        return EMAIL_ERROR_FAILED;

    default:
        break;
    }
    return EMAIL_ERROR_NONE;
}

/**
 * \brief Callback function of IP address.
 *
 * \param[in] hostName Domain name.
 * \param[in] hostIp Server IP.
 *
 * \return None.
 */
static void resolve_cb(char *hostName, uint32_t hostIp)
{
    g_HostIp = hostIp;
    gbHostIpByName = true;
    printf("Host IP is %d.%d.%d.%d\r\n", (int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
            (int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
    printf("Host Name is %s\r\n", hostName);
}

/**
 * \brief Callback function of TCP client socket.
 *
 * \param[in] sock socket handler.
 * \param[in] message Type of Socket notification
 * \param[in] pvMsg A structure contains notification informations.
 *
 * \return None.
 */
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg)
{
    /* Check for socket event on TCP socket. */
    if (sock == tcp_client_socket) 
    {
        switch (message) 
        {
        case M2M_SOCKET_CONNECT_EVENT:
        {
            t_socketConnect *pstrConnect = (t_socketConnect *)pvMsg;
            if (pstrConnect && pstrConnect->error >= SOCK_ERR_NO_ERROR) 
            {
                memset(gcHandlerBuffer, 0, SMTP_BUF_LEN);
                recv(tcp_client_socket, gcHandlerBuffer, sizeof(gcHandlerBuffer), 0);
            } 
            else 
            {
                printf("M2M_SOCKET_CONNECT_EVENT : connect error!\r\n");
                g_SocketStatus = SocketError;
            }
        }
        break;

        case M2M_SOCKET_SEND_EVENT:
        {
            switch (gu8SmtpStatus) 
            {
            case SMTP_MESSAGE_SUBJECT:
                g_SocketStatus = SocketConnect;
                gu8SmtpStatus = SMTP_MESSAGE_TO;
                break;

            case SMTP_MESSAGE_TO:
                g_SocketStatus = SocketConnect;
                gu8SmtpStatus = SMTP_MESSAGE_FROM;
                break;

            case SMTP_MESSAGE_FROM:
                g_SocketStatus = SocketConnect;
                gu8SmtpStatus = SMTP_MESSAGE_CRLF;
                break;

            case SMTP_MESSAGE_CRLF:
                g_SocketStatus = SocketConnect;
                gu8SmtpStatus = SMTP_MESSAGE_BODY;
                break;

            case SMTP_MESSAGE_BODY:
                g_SocketStatus = SocketConnect;
                gu8SmtpStatus = SMTP_MESSAGE_DATAEND;
                break;

            case SMTP_QUIT:
                g_SocketStatus = SocketComplete;
                gu8SmtpStatus = SMTP_INIT;
                break;

            default:
                break;
            }
        }
        break;

        case M2M_SOCKET_RECV_EVENT:
        {
            t_socketRecv *pstrRecv = (t_socketRecv *)pvMsg;

            if (g_SocketStatus == SocketWaiting) 
            {
                g_SocketStatus = SocketConnect;
                switch (gu8SmtpStatus) 
                {
                case SMTP_INIT:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        /* If buffer has 220 'OK' from server, set state to HELO */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeReady[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeReady[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeReady[2]) 
                        {
                            gu8SmtpStatus = SMTP_HELO;
                        } 
                        else 
                        {
                            printf("No response from server.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_INIT;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_INIT : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_INIT;
                    }

                    break;

                case SMTP_HELO:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        /* If buffer has 220, set state to HELO */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeOkReply[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeOkReply[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeOkReply[2]) 
                        {
                            gu8SmtpStatus = SMTP_AUTH;
                        } 
                        else 
                        {
                            printf("No response for HELO.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_HELO;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_HELO : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_HELO;
                    }

                    break;

                case SMTP_AUTH:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        /* Function handles authentication for all services */
                        generateBase64Key((char *)FROM_ADDRESS, gcUserBasekey);

                        /* If buffer is 334, give username in base64 */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeAuthReply[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeAuthReply[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeAuthReply[2]) 
                        {
                            gu8SmtpStatus = SMTP_AUTH_USERNAME;
                        } 
                        else 
                        {
                            printf("No response for authentication.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_AUTH;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_AUTH : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_AUTH;
                    }

                    break;

                case SMTP_AUTH_USERNAME:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        generateBase64Key((char *)FROM_PASSWORD, gcPasswordBasekey);

                        /* If buffer is 334, give password in base64 */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeAuthReply[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeAuthReply[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeAuthReply[2]) 
                        {
                            gu8SmtpStatus = SMTP_AUTH_PASSWORD;
                        } 
                        else
                        {
                            printf("No response for username authentication.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_AUTH_USERNAME;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_AUTH_USERNAME : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_AUTH_USERNAME;
                    }

                    break;

                case SMTP_AUTH_PASSWORD:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeAuthSuccess[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeAuthSuccess[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeAuthSuccess[2]) 
                       {
                            /* Authentication was successful, set state to FROM */
                            gu8SmtpStatus = SMTP_FROM;
                        } 
                        else 
                        {
                            printf("No response for password authentication.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_AUTH_PASSWORD;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_AUTH_PASSWORD : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_AUTH_PASSWORD;
                    }

                    break;

                case SMTP_FROM:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        /* If buffer has 250, set state to RCPT */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeOkReply[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeOkReply[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeOkReply[2]) 
                        {
                            gu8SmtpStatus = SMTP_RCPT;
                        } 
                        else
                        {
                            printf("No response for sender transmission.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_FROM;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_FROM : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_FROM;
                    }

                    break;

                case SMTP_RCPT:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        /* If buffer has 250, set state to DATA */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeOkReply[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeOkReply[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeOkReply[2]) 
                        {
                            gu8SmtpStatus = SMTP_DATA;
                        } 
                        else 
                        {
                            printf("No response for recipient transmission.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_RCPT;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_RCPT : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_RCPT;
                    }

                    break;

                case SMTP_DATA:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        /* If buffer has 250, set state to DATA */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeIntermedReply[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeIntermedReply[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeIntermedReply[2]) 
                        {
                            gu8SmtpStatus = SMTP_MESSAGE_SUBJECT;
                        } 
                        else 
                        {
                            printf("No response for data transmission.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_DATA;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_DATA : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_DATA;
                    }

                    break;

                case SMTP_MESSAGE_DATAEND:
                    if (pstrRecv && pstrRecv->bufSize > 0) 
                    {
                        /* If buffer has 250, set state to DATA */
                        if (pstrRecv->p_rxBuf[0] == cSmtpCodeOkReply[0] &&
                                pstrRecv->p_rxBuf[1] == cSmtpCodeOkReply[1] &&
                                pstrRecv->p_rxBuf[2] == cSmtpCodeOkReply[2]) 
                        {
                            gu8SmtpStatus = SMTP_QUIT;
                        } 
                        else 
                        {
                            printf("No response for dataend transmission.\r\n");
                            gu8SmtpStatus = SMTP_ERROR;
                            gs8EmailError = EMAIL_ERROR_MESSAGE;
                        }
                    } 
                    else 
                    {
                        printf("SMTP_MESSAGE_DATAEND : recv error!\r\n");
                        gu8SmtpStatus = SMTP_ERROR;
                        gs8EmailError = EMAIL_ERROR_MESSAGE;
                    }

                    break;

                default:
                    break;
                }
            }
        }
        break;

        default:
            break;
        }
    }
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] msgType Type of Wi-Fi notification.
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters.
 *
 * \return None.
 */
static void wifi_cb(uint8_t msgType, void *pvMsg)
{
    switch (msgType) {
    case M2M_WIFI_CONN_STATE_CHANGED_EVENT:
    {
        tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
        if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) 
        {
            printf("M2M_WIFI_CONN_STATE_CHANGED_EVENT: CONNECTED\r\n");
        } 
        else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            printf("M2M_WIFI_CONN_STATE_CHANGED_EVENT: DISCONNECTED\r\n");
            gbConnectedWifi = false;
            gbHostIpByName = false;
            m2m_wifi_connect((char *)WLAN_SSID,
                             strlen(WLAN_SSID),
                             WLAN_AUTH, 
                             (char *)WLAN_PSK, 
                             M2M_WIFI_CH_ALL);
        }

        break;
    }

    case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
        /* Turn LED0 on to declare that IP address received. */
        printf("M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT: IP is %u.%u.%u.%u\r\n",
                pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
        gbConnectedWifi = true;

        /* Obtain the IP Address by network name */
        gethostbyname((const char *)GMAIL_HOST_NAME);
        break;
    }

    default:
    {
        break;
    }
    }
}

/**
 * \brief Close socket function.
 * \return None.
 */
static void close_socket(void)
{
    close(tcp_client_socket);
    tcp_client_socket = -1;
}

/**
 * \brief Retry SMTP server function.
 * \return None.
 */
static void retry_smtp_server(void)
{
    close_socket();
    g_SocketStatus = SocketInit;
    gu8SmtpStatus = SMTP_INIT;
    gbHostIpByName = false;
    DelayMs(WAITING_TIME);
    m2m_wifi_disconnect();
}

void ApplicationTask(void)
{
    switch (GetAppState())
    {
    case APP_STATE_WAIT_FOR_DRIVER_INIT:
        if (isDriverInitComplete())
        {
            SetAppState(APP_STATE_START);
        }
        break;
        
    case APP_STATE_START:
        printf("\r\n=========\r\n");            
        printf("Sending Email Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);
        
        /* Connect to router. */
        m2m_wifi_connect((char *)WLAN_SSID,
                         strlen(WLAN_SSID),
                         WLAN_AUTH, 
                         (char *)WLAN_PSK, 
                         M2M_WIFI_CH_ALL);        

        SetAppState(APP_STATE_WORKING);
        break;
      
    case APP_STATE_WORKING:       
        if (gbConnectedWifi && gbHostIpByName) 
        {
            if (g_SocketStatus == SocketInit) 
            {
                if (tcp_client_socket < 0) 
                {
                    g_SocketStatus = SocketWaiting;
                    sslEnableCertExpirationCheck(0);
                    if (smtpConnect() != SOCK_ERR_NO_ERROR) 
                    {
                        g_SocketStatus = SocketInit;
                    }
                }
            } 
            else if (g_SocketStatus == SocketConnect) 
            {
                g_SocketStatus = SocketWaiting;
                if (smtpStateHandler() != EMAIL_ERROR_NONE) 
                {
                    if (gs8EmailError == EMAIL_ERROR_INIT) 
                    {
                        g_SocketStatus = SocketError;
                    } 
                    else 
                    {
                        close_socket();
                        SetAppState(APP_STATE_DONE);
                        break;
                    }
                }
            } 
            else if (g_SocketStatus == SocketComplete) 
            {
                printf("main: Email was successfully sent.\r\n");
                close_socket();
                SetAppState(APP_STATE_DONE);
                break;
            } 
            else if (g_SocketStatus == SocketError) 
            {
                if (gu8RetryCount < RETRY_COUNT)
                {
                    gu8RetryCount++;
                    printf("main: Waiting to connect server.(30 seconds)\r\n\r\n");
                    retry_smtp_server();
                } 
                else 
                {
                    printf("main: Failed retry to server. Press reset your board.\r\n");
                    gu8RetryCount = 0;
                    close_socket();
                    SetAppState(APP_STATE_DONE);
                    break;
                }
            }
        }            
        break;
        
    case APP_STATE_DONE:
        break;
    default:
        break;
    }
}

#endif
