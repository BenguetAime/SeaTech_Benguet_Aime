/*******************************************************************************
  File Name:
    winc1500_pubnub.c

  Summary:
    WINC1500 pubnub demo.

  Description:
    This demo performs the following steps:
        1) Connect to AP, 
        2) run pubnub Application on android phone
        3) send message to android phone
 
    The configuration defines for this demo are:    
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        PUBNUB_PUBLISH_KEY    -- Publish Key
        PUBNUB_SUBSCRIBE_KEY  -- Subscribe Key
        PUBNUB_CHANNEL        -- Channel

    In order to test this demo, we should register an account on "www.pubnub.com", 
    create public key, subscribe key, channel name. 
    And fill these key values and channel name into this file (winc1500_pubnub.c): 
        #define PUBNUB_PUBLISH_KEY           "xxxxxxxxxxxxxxxx"
        #define PUBNUB_SUBSCRIBE_KEY         "xxxxxxxxxxxxxxxx"
        #define PUBNUB_CHANNEL               "xxxxxxxxxxxxxxxx"
    Compile the code and run it on the mcu+winc1500 board. After the board connect to AP, 
    then it will send message to www.pubnub.com. In your laptop, you can see the message
    in a console window of www.pubnub.com.
    
    The demo uses these cllback functions to handle events:
        socket_cb()
        resolve_cb()
        wifi_cb()
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
#include "wf_common.h"

#if defined(USING_PUBNUB)
#include "PubNub.h"

/** Wi-Fi settings. */
#define WLAN_SSID                    "DEMO_AP"
#define WLAN_PSK                     "12345678"
#define WLAN_AUTH                    M2M_WIFI_SEC_WPA_PSK

/** PubNub settings. */
#define PUBNUB_PUBLISH_KEY           "pub-c-d2223b13-a037-46f5-a1b3-1caaf2c8106c"
#define PUBNUB_SUBSCRIBE_KEY         "sub-c-3450148a-6314-11e6-80fa-0619f8945a4f"
#define PUBNUB_CHANNEL               "Channel-ifewt8j37"

#define PUBNUB_PUBLISH_INTERVAL      (3000)
#define PUBNUB_SUBSCRIBE_INTERVAL    (1000)    

#define SetAppState(state)           g_appState = state
#define GetAppState()                g_appState
#define IPV4_BYTE(val, index)        ((val >> (index * 8)) & 0xFF)

typedef enum wifi_status {
    WifiStateInit,
    WifiStateWaitingProv,
    WifiStateConnecting,
    WifiStateConnected,
    WifiStateDisConnected
} wifi_status_t;

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WORKING,  
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

volatile wifi_status_t gWifiState = WifiStateInit;  // WiFi status variable
volatile uint32_t g_MsTicks = 0;                    // SysTick counter to avoid busy wait delay

// Global counter delay for timer
static uint32_t g_publishDelay = 0;
static uint32_t g_subscribeDelay = 0;

// PubNub global variables
static const char PubNubPublishKey[] = PUBNUB_PUBLISH_KEY;
static const char PubNubSubscribeKey[] = PUBNUB_SUBSCRIBE_KEY;
static char PubNubChannel[] = PUBNUB_CHANNEL;
static pubnub_t *pPubNubCfg;

void SysTick_Handler(void);
static void wifi_cb(uint8_t msgType, void *pvMsg); 
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);
static void resolve_cb(char *pu8DomainName, uint32_t serverIP);

void ApplicationTask(void)
{
    static int messageNumber = 0;
    static char buf[256] = {0};
    static uint32_t startTime = 0;
    
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
        printf("PubNub Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);
        
        /* Initialize PubNub API. */
        printf("main: PubNub configured with following settings:\r\n");
        printf("main:  - Publish key: \"%s\", Subscribe key: \"%s\", Channel: \"%s\".\r\n\r\n",
        PubNubPublishKey, PubNubSubscribeKey, PubNubChannel);
        pPubNubCfg = pubnub_get_ctx(0);
        pubnub_init(pPubNubCfg, PubNubPublishKey, PubNubSubscribeKey);

        /* Connect to AP using Wi-Fi settings from main.h. */
        printf("main: Wi-Fi connecting to AP using hardcoded credentials...\r\n");
        m2m_wifi_connect((char *)WLAN_SSID,
                strlen(WLAN_SSID),
                WLAN_AUTH, 
                (char *)WLAN_PSK, 
                M2M_WIFI_CH_ALL);     
        SetAppState(APP_STATE_WORKING);
        startTime = m2mStub_GetOneMsTimer();
        break;
               
    case APP_STATE_WORKING:   
        if (m2m_get_elapsed_time(startTime) > 1)
        {
            startTime = m2mStub_GetOneMsTimer();
            SysTick_Handler();
        }
        
        /* Device is connected to AP. */
        if (gWifiState == WifiStateConnected) 
        {
            /* PubNub: read event from the cloud. */
            if (pPubNubCfg->state == PS_IDLE) 
            {
                /* Subscribe at the beginning and re-subscribe after every publish. */
                if ((pPubNubCfg->trans == PBTT_NONE) ||
                    (pPubNubCfg->trans == PBTT_PUBLISH && pPubNubCfg->last_result == PNR_OK)) 
                {
                    printf("main: subscribe event, PNR_OK\r\n");
                    pubnub_subscribe(pPubNubCfg, PubNubChannel);                
                }

                /* Process any received messages from the channel we subscribed. */
                while (1) 
                {
                    char const *msg = pubnub_get(pPubNubCfg);
                    if (NULL == msg) 
                    {
                        /* No more message to process. */
                        break;
                    }
                }

                /* Subscribe to receive pending messages. */
                if (g_MsTicks - g_subscribeDelay > PUBNUB_SUBSCRIBE_INTERVAL) 
                {
                    g_subscribeDelay = g_MsTicks;
                    printf("main: subscribe event, interval.\r\n");
                    pubnub_subscribe(pPubNubCfg, PubNubChannel);
                }
            }

            /* Publish the temperature measurements periodically. */
            if (g_MsTicks - g_publishDelay > PUBNUB_PUBLISH_INTERVAL) 
            {
                g_publishDelay = g_MsTicks;
                messageNumber ++;
                sprintf(buf, "{\"device\":\"%s\", \"message number\":\"%d\"}",
                        PubNubChannel,
                        (int)messageNumber);
                printf("main: publish event: {%s}\r\n", buf);
                close(pPubNubCfg->tcp_socket);
                pPubNubCfg->state = PS_IDLE;
                pPubNubCfg->last_result = PNR_IO_ERROR;
                pubnub_publish(pPubNubCfg, PubNubChannel, buf);
            }
        }
        break;
        
    case APP_STATE_DONE:
        break;
        
    default:
        break;
    }
}

void SysTick_Handler(void)
{
    g_MsTicks++;
}

/**
 * \brief Callback to get the Socket event.
 *
 * \param[in] Socket descriptor.
 * \param[in] msg_type type of Socket notification. Possible types are:
 *  - [M2M_SOCKET_CONNECT_EVENT](@ref M2M_SOCKET_CONNECT_EVENT)
 *  - [M2M_SOCKET_BIND_EVENT](@ref M2M_SOCKET_BIND_EVENT)
 *  - [M2M_SOCKET_LISTEN_EVENT](@ref M2M_SOCKET_LISTEN_EVENT)
 *  - [M2M_SOCKET_ACCEPT_EVENT](@ref M2M_SOCKET_ACCEPT_EVENT)
 *  - [M2M_SOCKET_RECV_EVENT](@ref M2M_SOCKET_RECV_EVENT)
 *  - [M2M_SOCKET_SEND_EVENT](@ref M2M_SOCKET_SEND_EVENT)
 *  - [M2M_SOCKET_SENDTO_EVENT](@ref M2M_SOCKET_SENDTO_EVENT)
 *  - [M2M_SOCKET_RECVFROM_EVENT](@ref M2M_SOCKET_RECVFROM_EVENT)
 * \param[in] msg_data A structure contains notification informations.
 */
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg)
{
    handle_tcpip(sock, message, pvMsg);
}

/**
 * \brief Callback of gethostbyname function.
 *
 * \param[in] doamin_name Domain name.
 * \param[in] server_ip IP of server.
 */
static void resolve_cb(char *hostName, uint32_t hostIp)
{
    printf("%s resolved with IP %d.%d.%d.%d\r\n",
            hostName,
            (int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
            (int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
    handle_dns_found(hostName, hostIp);
}

static void wifi_cb(uint8_t msgType, void *pvMsg)
{
    switch (msgType) 
    {
    case M2M_WIFI_CONN_STATE_CHANGED_EVENT:
    {
        tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
        if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) 
        {
            printf("wifi_state: M2M_WIFI_CONN_STATE_CHANGED_EVENT: CONNECTED\r\n");
        } 
        else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            printf("wifi_state: M2M_WIFI_CONN_STATE_CHANGED_EVENT: DISCONNECTED\r\n");
            if (WifiStateConnected == gWifiState) 
            {
                gWifiState = WifiStateDisConnected;
                m2m_wifi_connect((char *)WLAN_SSID,
                        strlen(WLAN_SSID),
                        WLAN_AUTH, 
                        (char *)WLAN_PSK, 
                        M2M_WIFI_CH_ALL);
            }
        }

        break;
    }

    case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
        printf("wifi_state: M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT: IP is %u.%u.%u.%u\r\n",
                pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
        gWifiState = WifiStateConnected;

        break;
    }

    default:
    {
        break;
    }
    }
}
#endif
