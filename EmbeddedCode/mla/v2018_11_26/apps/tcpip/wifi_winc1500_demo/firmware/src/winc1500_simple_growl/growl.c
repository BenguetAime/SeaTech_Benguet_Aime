/*******************************************************************************
  File Name:
    growl.c

  Summary:
    Growl Client Interface.

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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "winc1500_api.h"
#include "demo_config.h"
#include "wf_common.h"

#if defined(USING_SIMPLE_GROWL)
#include "growl.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


#define MAX_REGISTERED_EVENTS               1
#define GROWL_CONNECT_RETRY                 3
#define GROWL_DNS_RETRY                     3


#define PROWL_DOMAIN_NAME                   "api.prowlapp.com"//"www.google.com.eg"//
#define PROWL_CLIENT_STRING_ID              "prowl"//"google"//
#define PROWL_API_KEY_SIZE                  40

#define NMA_DOMAIN_NAME                     "www.notifymyandroid.com"
#define NMA_CLIENT_STRING_ID                "notifymyandroid"
#define NMA_API_KEY_SIZE                    48

#define GROWL_HTTP_PORT                     80
#define GROWL_HTTPS_PORT                    443
#define GROWL_MSG_SIZE                      256


#define GROWL_STATE_IDLE                    ((uint8_t)0)
#define GROWL_STATE_REQ_PENDING             ((uint8_t)1)
#define GROWL_STATE_WAITING_RESPONSE        ((uint8_t)2)

#define GROWL_RX_TIMEOUT                    (25 * 1000)

#define M2M_DBG printf
#define M2M_ERR printf

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
PRIVATE DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@struct    \
    tstrNotification

@brief

*/
typedef struct{
    uint32_t    serverIPAddress;
    uint16_t    port;
    SOCKET      Socket;
    uint8_t     state;
    uint8_t     *pApp;
    uint8_t     *pEvent;
    uint8_t     *pMsg;
}tstrNotification;


/*####################################################################################
GLOBALS
####################################################################################*/

static volatile tstrNotification        gstrNotificationProwl;
static volatile tstrNotification        gstrNotificationNMA;
static const uint8_t                    hexDigits[] = "0123456789abcdef";
static volatile uint8_t                 *prwKey;
static volatile uint8_t                 *nmaKey;
static volatile uint8_t                 msg[GROWL_DESCRIPTION_MAX_LENGTH];



uint16_t Encode(uint8_t *pcStr, uint8_t *pcEncoded);

uint16_t FormatMsg(uint8_t clientName, uint8_t *pMsg);

uint8_t GetResponseCode(uint8_t *p_rxBuf, uint16_t bufferSize);
void resolve_cb(char* p_HostName, uint32_t serverIP);

uint16_t Encode(uint8_t *pcStr,uint8_t *pcEncoded)
{
    uint8_t        *pcTmp = pcStr;
    uint8_t        *pcbuf = pcEncoded;
    uint16_t        u16Count = 0;

    while(*pcTmp)
    {
        if (
            ((*pcTmp >= '0') && (*pcTmp <= '9'))        ||
            ((*pcTmp >= 'a') && (*pcTmp <= 'z'))     ||
            ((*pcTmp >= 'A') && (*pcTmp <= 'Z'))     ||
            (*pcTmp == '-') ||
            (*pcTmp == '_') ||
            (*pcTmp == '.') ||
            (*pcTmp == '~')
            )
        {
            *pcbuf++ = *pcTmp;
        }
        else
        {
            *pcbuf++ = '%';
            *pcbuf++ = hexDigits[(*pcTmp>> 4) & 0x0F];
            *pcbuf++ = hexDigits[(*pcTmp & 0x0F)];
            u16Count += 2;
        }
        pcTmp++;
        u16Count ++;
    }
    return u16Count;
}

uint16_t FormatMsg(uint8_t clientName, uint8_t *pMsg)
{
    uint16_t    u16Tmp;
    uint16_t    u16MsgOffset = 0;
    tstrNotification* strNotification;

    if (clientName == NMA_CLIENT)
    {
        strNotification = (tstrNotification*)(&gstrNotificationNMA);
        /* Put the start of the HTTP Request message. */
        u16Tmp = sizeof("GET /publicapi/notify?apikey=") - 1;
        memcpy(&pMsg[u16MsgOffset],(uint8_t*)"GET /publicapi/notify?apikey=",u16Tmp);
        u16MsgOffset += u16Tmp;

        /* Add the API Key to the message. */
        memcpy(&pMsg[u16MsgOffset],(uint8_t*)nmaKey,NMA_API_KEY_SIZE);
        u16MsgOffset+= NMA_API_KEY_SIZE;
    }
    else if (clientName == PROWL_CLIENT)
    {
        strNotification = (tstrNotification*)(&gstrNotificationProwl);

        /* Put the start of the HTTP Request message. */
        u16Tmp = sizeof("GET /publicapi/add?apikey=") - 1;
        memcpy(&pMsg[u16MsgOffset],(uint8_t*)"GET /publicapi/add?apikey=",u16Tmp);
        u16MsgOffset += u16Tmp;

        /* Add the API Key to the message. */
        memcpy(&pMsg[u16MsgOffset],(uint8_t*)prwKey,PROWL_API_KEY_SIZE);
        u16MsgOffset+= PROWL_API_KEY_SIZE;
    }
    else
        return 0;

    /* Encode the Application name and append it to the message. */
    u16Tmp = sizeof("&application=") - 1;
    memcpy(&pMsg[u16MsgOffset],(uint8_t*)"&application=",u16Tmp);
    u16MsgOffset += u16Tmp;
    u16Tmp = Encode((uint8_t*)strNotification->pApp,&pMsg[u16MsgOffset]);
    u16MsgOffset += u16Tmp;

    /* Encode the Event name and append it to the message. */
    u16Tmp = sizeof("&event=") - 1;
    memcpy(&pMsg[u16MsgOffset],(uint8_t*)"&event=",u16Tmp);
    u16MsgOffset += u16Tmp;
    u16Tmp = Encode((uint8_t*)strNotification->pEvent,&pMsg[u16MsgOffset]);
    u16MsgOffset += u16Tmp;

    /* Encode the Description message and append it to the message. */
    u16Tmp = sizeof("&description=") - 1;
    memcpy(&pMsg[u16MsgOffset],(uint8_t*)"&description=",u16Tmp);
    u16MsgOffset += u16Tmp;
    u16Tmp = Encode((uint8_t*)strNotification->pMsg,&pMsg[u16MsgOffset]);
    u16MsgOffset += u16Tmp;

    u16Tmp = sizeof(" HTTP/1.1\r\nHost: ") - 1;
    memcpy(&pMsg[u16MsgOffset],(uint8_t*)" HTTP/1.1\r\nHost: ",u16Tmp);
    u16MsgOffset += u16Tmp;

    if (clientName == NMA_CLIENT)
    {
        u16Tmp = sizeof(NMA_DOMAIN_NAME) - 1;
        memcpy(&pMsg[u16MsgOffset],(uint8_t*)NMA_DOMAIN_NAME,u16Tmp);
        u16MsgOffset += u16Tmp;
    }
    else if (clientName == PROWL_CLIENT)
    {
        u16Tmp = sizeof(PROWL_DOMAIN_NAME) - 1;
        memcpy(&pMsg[u16MsgOffset],(uint8_t*)PROWL_DOMAIN_NAME,u16Tmp);
        u16MsgOffset += u16Tmp;

    }

    u16Tmp = sizeof("\r\n\r\n") - 1;
    memcpy(&pMsg[u16MsgOffset],(uint8_t*)"\r\n\r\n",u16Tmp);
    u16MsgOffset += u16Tmp;
    pMsg[u16MsgOffset] = '\0';
    u16MsgOffset ++;

    return u16MsgOffset;
}

uint8_t GetResponseCode
(
 uint8_t    *p_rxBuf,
 uint16_t    bufferSize
)
{
    uint8_t    u8Code = 0xFF;
    if ((p_rxBuf != NULL) && (bufferSize > 0))
    {
        uint16_t    u16Offset = 0;
        do
        {
            if (!memcmp(&p_rxBuf[u16Offset], (uint8_t*)"HTTP/1.1 ", 9))
            {
                u16Offset += 9;
                if (!memcmp(&p_rxBuf[u16Offset], (uint8_t*)"200", 3))
                {
                    u8Code = 20;
                }
                else if (!memcmp(&p_rxBuf[u16Offset], (uint8_t*)"400", 3))
                {
                    u8Code = 40;
                }
                else if (!memcmp(&p_rxBuf[u16Offset], (uint8_t*)"401", 3))
                {
                    u8Code = 41;
                }
                else if (!memcmp(&p_rxBuf[u16Offset], (uint8_t*)"402", 3))
                {
                    u8Code = 42;
                }
                else if (!memcmp(&p_rxBuf[u16Offset], (uint8_t*)"406", 3))
                {
                    u8Code = 46;
                }
                else if (!memcmp(&p_rxBuf[u16Offset], (uint8_t*)"409", 3))
                {
                    u8Code = 49;
                }
                else
                {
                    u8Code = 50;
                }
                break;
            }
            u16Offset ++;
        } while (u16Offset < (bufferSize - 9));
    }
    return u8Code;
}

void socket_cb(SOCKET sock, uint8_t message, void * pvMsg)
{
    tstrNotification    *pstrNotification;
    uint8_t                 clientID;

    if (sock == gstrNotificationNMA.Socket)
    {
        pstrNotification = (tstrNotification*)(&gstrNotificationNMA);
        clientID = NMA_CLIENT;
    }
    else if (sock == gstrNotificationProwl.Socket)
    {
        pstrNotification = (tstrNotification*)(&gstrNotificationProwl);
        clientID = PROWL_CLIENT;

    }
    else
    {
        return;
    }

    if (message == M2M_SOCKET_CONNECT_EVENT)
    {
        static uint8_t            u8Retry = GROWL_CONNECT_RETRY;
        t_socketConnect    *pstrConnect = (t_socketConnect*)pvMsg;
        if (pstrConnect->error == 0)
        {
            uint8_t    acBuffer[GROWL_MSG_SIZE];
            uint16_t    u16MsgSize;

            u16MsgSize = FormatMsg(clientID, acBuffer);
            send(sock, acBuffer, u16MsgSize, 0);
            recv(pstrNotification->Socket, (void*)msg,GROWL_DESCRIPTION_MAX_LENGTH, GROWL_RX_TIMEOUT);
            u8Retry = GROWL_CONNECT_RETRY;
        }
        else
        {
            if ((u8Retry--) > 0)
            {
                M2M_DBG("Retry %s\n",(clientID == NMA_CLIENT) ? "NMA" : "PROWL");
                if (clientID == NMA_CLIENT)
                {
                    resolve_cb((char*)NMA_DOMAIN_NAME, pstrNotification->serverIPAddress);
                }
                else if (clientID == PROWL_CLIENT)
                {
                    resolve_cb((char*)PROWL_DOMAIN_NAME, pstrNotification->serverIPAddress);
                }
            }
            else
            {
                M2M_DBG("%s Connection Failed\n",(clientID == NMA_CLIENT) ? "NMA" : "PROWL");
                close(pstrNotification->Socket);
                pstrNotification->Socket = 0xFF;
                pstrNotification->state = GROWL_STATE_IDLE;
                GrowlCb(GROWL_ERR_CONN_FAILED,clientID);
                u8Retry = GROWL_CONNECT_RETRY;
            }
        }
    }
    else if (message == M2M_SOCKET_RECV_EVENT)
    {
        t_socketRecv        *pstrRecvMsg = (t_socketRecv*)pvMsg;
        static uint8_t            u8Error = 0xFF;
        uint8_t                    u8Reset = 1;

        if ((pstrRecvMsg->p_rxBuf != NULL) && (pstrRecvMsg->bufSize > 0))
        {
            if (u8Error == 0xFF)
            {
                u8Error = GetResponseCode(pstrRecvMsg->p_rxBuf,pstrRecvMsg->bufSize);
            }
        }
        if ((pstrRecvMsg->bufSize > 0) && (pstrRecvMsg->remainingSize != 0))
        {
            u8Reset = 0;
        }
        if (u8Reset)
        {
            close(pstrNotification->Socket);
            pstrNotification->Socket    = 0xFF;
            pstrNotification->state    = GROWL_STATE_IDLE;
            GrowlCb(u8Error,clientID);
            u8Error = 0xFF;
        }
    }
    else if (message == M2M_SOCKET_SEND_EVENT)
    {
        int16_t    s16Sent = *((int16_t*)pvMsg);
        if (s16Sent <= 0)
        {
            M2M_ERR("GROWL Send Error %d\n",s16Sent);
        }
    }
}

void NMI_GrowlInit(uint8_t *pu8PrwKey,uint8_t *pu8NmaKey)
{
    //socketInit();
    if (pu8PrwKey)
    {
        prwKey = pu8PrwKey;
    }
    else
    {
        printf("Prowl key Not Vaild\n");
    }
    if (pu8NmaKey)
    {

        nmaKey = pu8NmaKey;
    }
    else
    {
        printf("NMA key Not Vaild\n");
    }
    memset((uint8_t*)&gstrNotificationProwl, 0, sizeof(tstrNotification));
    memset((uint8_t*)&gstrNotificationNMA, 0, sizeof(tstrNotification));
    gstrNotificationProwl.Socket = 0xFF;
    gstrNotificationNMA.Socket = 0xFF;
}

void NMI_GrowldeInit(void)
{
    memset((uint8_t*)&gstrNotificationProwl, 0, sizeof(tstrNotification));
    memset((uint8_t*)&gstrNotificationNMA, 0, sizeof(tstrNotification));
    if (gstrNotificationProwl.Socket != -1/*0xff*/)
    {
        close(gstrNotificationProwl.Socket);
    }
    if (gstrNotificationNMA.Socket != -1/*0xff*/)
    {
        close(gstrNotificationNMA.Socket);
    }
    gstrNotificationProwl.Socket = 0xFF;
    gstrNotificationNMA.Socket = 0xFF;
}

void resolve_cb(char* p_HostName, uint32_t serverIP)
{
    struct sockaddr_in      strAddr;
    tstrNotification        *pstrNotification = NULL;
    uint8_t                 clientID = 0;

    if (strstr((const char *)p_HostName, (const char *)NMA_CLIENT_STRING_ID))
    {
        pstrNotification = (tstrNotification*)(&gstrNotificationNMA);
        clientID = NMA_CLIENT;
    }
    else if (strstr((const char *)p_HostName, (char *)PROWL_CLIENT_STRING_ID))
    {
        pstrNotification = (tstrNotification*)(&gstrNotificationProwl);
        clientID = PROWL_CLIENT;
    }

    if (serverIP != 0)
    {

        if (pstrNotification->serverIPAddress == 0)
        {
            pstrNotification->serverIPAddress = serverIP;
        }

        strAddr.sin_family = AF_INET;
        strAddr.sin_port = pstrNotification->port;
        strAddr.sin_addr.s_addr = serverIP;

        connect(pstrNotification->Socket, (struct sockaddr*)&strAddr, sizeof(struct sockaddr_in));
    }
    else
    {
        static uint8_t    u8Retry = GROWL_DNS_RETRY;
        if (u8Retry--)
        {
            M2M_DBG("Retry Resolving DNS\n");
            if (strstr((const char *)p_HostName, (const char *)NMA_CLIENT_STRING_ID))
                gethostbyname((const char *)NMA_DOMAIN_NAME);
            else if (strstr((const char *)p_HostName, (const char *)PROWL_CLIENT_STRING_ID))
                gethostbyname((const char *)PROWL_DOMAIN_NAME);

        }
        else
        {
            close(pstrNotification->Socket);
            pstrNotification->Socket = 0xFF;
            pstrNotification->state = GROWL_STATE_IDLE;
            u8Retry = GROWL_DNS_RETRY;
            M2M_DBG("Failed to Resolve DNS\n\r");
            GrowlCb(GROWL_ERR_RESOLVE_DNS,clientID);
        }
    }
}

// Note : it's required to keep the {pApp,pEvent,pu8Description} pointers const or global not temp val.
/**********************************************************************/
int8_t NMI_GrowlSendNotification(uint8_t clientName, uint8_t *pApp, uint8_t *pEvent, uint8_t *pu8Description,uint8_t bUseSSL)
{
    int8_t        retVal = 0;
    uint8_t        u8Flags = 0;

    if ((clientName > 0) && (pApp != NULL) && (pEvent != NULL) && (pu8Description != NULL))
    {
        tstrNotification* pstrNotification;
        if ((clientName == NMA_CLIENT))
        {
            if (nmaKey == NULL)
            {
                printf("NMA key Not Vaild\n");
                return -1;
            }
            pstrNotification = (tstrNotification*)(&gstrNotificationNMA);
        }
        else if ((clientName == PROWL_CLIENT))
        {
            if (prwKey == NULL)
            {
                printf("Prowl key Not Vaild\n");
                return -1;
            }
            pstrNotification = (tstrNotification*)(&gstrNotificationProwl);
        }
        else
        {
            return WF_ERR_FAIL;
        }

        if (pstrNotification->state == GROWL_STATE_IDLE)
        {
            if ((strlen((const char *)pu8Description) < GROWL_DESCRIPTION_MAX_LENGTH) &&
                (strlen((const char *)pApp) < GROWL_APPNAME_MAX_LENGTH) &&
                (strlen((const char *)pEvent) < GROWL_EVENT_MAX_LENGTH))
            {
                pstrNotification->pApp = pApp;
                pstrNotification->pEvent = pEvent;
                pstrNotification->pMsg = pu8Description;
                pstrNotification->port = _htons(GROWL_HTTP_PORT);

                /* Create Connection to the NMA Server. */
                if (bUseSSL)
                {
                    u8Flags = SOCKET_FLAGS_SSL;
                    pstrNotification->port = _htons(GROWL_HTTPS_PORT);
                }

                pstrNotification->Socket = socket(AF_INET,SOCK_STREAM,u8Flags);
                if (pstrNotification->Socket >= 0)
                {
                    pstrNotification->state = GROWL_STATE_REQ_PENDING;
                    if (clientName == NMA_CLIENT)
                        gethostbyname((const char *)NMA_DOMAIN_NAME);
                    else if (clientName == PROWL_CLIENT)
                        gethostbyname((const char *)PROWL_DOMAIN_NAME);
                }
                else
                {
                    M2M_ERR("No sockets available for the current request\n");
                    retVal = -1;
                }
            }
            else
            {
                M2M_ERR("Msg size is too long\n");
                retVal = -1;
            }
        }
        else
        {
            M2M_ERR("Another %s Request is pending\n",(clientName == NMA_CLIENT) ? "NMA" : "PROWL");
            retVal = -1;
        }
    }
    else
    {
        retVal = -1;
    }
    return retVal;
}

#endif