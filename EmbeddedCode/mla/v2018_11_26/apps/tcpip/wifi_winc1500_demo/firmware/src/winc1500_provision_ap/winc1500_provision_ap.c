/*******************************************************************************
  File Name:
    winc1500_provision_ap.c

  Summary:
    WINC1500 provision ap demo.

  Description:
    This demo performs the following steps:
        1) Install provision_ap.apk on Android phone
        2) This board works as AP mode,
        3) Android phone's wifi connect to this board
        4) In cell phone, open App "provision_ap", fill in another AP's ssid, security password
        5) this Winc1500 board will re-direct to another AP.

    The configuration defines for this demo are:    
        WLAN_SSID                 -- This AP's ssid is visible or hidden
        WLAN_AUTH                 -- Security mode
        WLAN_CHANNEL              -- Channel number
    
    The demo uses these callback functions to handle events:
        socket_cb(), 
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

#if defined(USING_PROVISION_AP) 

/** AP mode Settings */
#define WLAN_SSID                 "DEMO_AP" /* < SSID */
#define WLAN_AUTH                 M2M_WIFI_SEC_OPEN /* < Security manner */
#define WLAN_CHANNEL              (6) /* < Channel number */

#define WLAN_SERVER_PORT          (80)
/** Receive buffer size. */
#define WLAN_BUFFER_SIZE          1024

// State macros
#define SetAppState(state)        g_appState = state
#define GetAppState()             g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_SOCKET_TEST,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

/** Receive buffer definition. */
static uint8_t socketTestBuffer[WLAN_BUFFER_SIZE];

/** Socket for TCP communication */
static SOCKET tcp_server_socket = -1;
static SOCKET tcp_client_socket = -1;
struct sockaddr_in addr;
tstrM2MAPConfig strAPConfig;
static bool ConnectToAP = false;

static char    str_ssid[M2M_MAX_SSID_LEN], str_pw[M2M_MAX_PSK_LEN];
static uint8_t sec_type = 0;

static void wifi_cb(uint8_t msgType, void *pvMsg);
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);

static bool get_AP_Parameter(uint8_t *str_in)
{
    uint8_t *pos;
    uint8_t *new_begin;
    uint8_t str_cmd[10], str_secType[4];

    memset(str_cmd, 0, 10);
    memset(str_ssid, 0, M2M_MAX_SSID_LEN);
    sec_type = 0;
    memset(str_pw, 0, M2M_MAX_PSK_LEN);
    
    // get command
    new_begin = str_in;
    pos = (uint8_t *)strstr((char *)new_begin, ",");
    if (pos == NULL)
        return false;
    if ((pos - new_begin) > 5) 
        return false;
    strncpy((char *)str_cmd, (char *)new_begin, pos - new_begin);
    if (strncmp((char *)str_cmd, "apply", 5))
        return false;
    printf("we got cmd: %s\r\n", str_cmd);
    
    // get SSID
    new_begin = pos + strlen(",");
    pos = (uint8_t *)strstr((char *)new_begin, ",");
    if (pos == NULL)
        return false;
    strncpy((char *)str_ssid, (char *)new_begin, pos - new_begin);
    printf("we got ssid: %s\r\n", str_ssid);

    // get Security Type
    new_begin = pos + strlen(",");
    pos = (uint8_t *)strstr((char *)new_begin, ",");
    if (pos == NULL)
        return false;
    strncpy((char *)str_secType, (char *)new_begin, pos - new_begin);
    sec_type = atoi((const char *)str_secType);
    printf("we got sec type: %d\r\n", sec_type);

    // get Security PassWord
    new_begin = pos + strlen(",");
    pos = (uint8_t *)strstr((char *)new_begin, ",");
    if (pos == NULL)
        return false;
    
    strncpy((char *)str_pw, (char *)new_begin, pos - new_begin);
    printf("got pw:%s\r\n-------------\r\n", str_pw);
    return true;
}

// application state machine called from main()
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
        printf("Provision AP Demo\r\n");
        printf("=========\r\n");
        printf("Starting ...\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, NULL);
        
        memset(&strAPConfig, 0x00, sizeof(tstrM2MAPConfig));
        strcpy((char *)&strAPConfig.au8SSID, WLAN_SSID);
        strAPConfig.u8ListenChannel = WLAN_CHANNEL;
        strAPConfig.u8SecType = WLAN_AUTH;
        strAPConfig.au8DHCPServerIP[0] = 0xC0; /* 192 */
        strAPConfig.au8DHCPServerIP[1] = 0xA8; /* 168 */
        strAPConfig.au8DHCPServerIP[2] = 0x01; /* 1 */
        strAPConfig.au8DHCPServerIP[3] = 0x01; /* 1 */

        /* Bring up AP mode with parameters structure. */
        m2m_wifi_enable_ap(&strAPConfig);
        printf("Provision Mode started.\r\nUsing Android device to connect AP: [");
        printf((const char *)strAPConfig.au8SSID);
        printf("] and run APP\r\n"); 
        SetAppState(APP_STATE_SOCKET_TEST);
        break;

    case APP_STATE_SOCKET_TEST:
        if (ConnectToAP == true) 
        {
            addr.sin_family = AF_INET;
            addr.sin_port = _htons(WLAN_SERVER_PORT);
            addr.sin_addr.s_addr = 0;
            
            if (tcp_server_socket < 0) 
            {
                /* Open TCP server socket */
                if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                {
                    printf("main: failed to create TCP server socket error!\r\n");
                    break;
                }

                /* Bind service*/
                bind(tcp_server_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
                SetAppState(APP_STATE_DONE); 
            }
        }
        break;

    case APP_STATE_DONE:
        break;
    }
}

static void wifi_cb(uint8_t msgType, void *pvMsg)
{
    switch (msgType) {
    case M2M_WIFI_CONN_STATE_CHANGED_EVENT:
    {
        tstrM2mWifiStateChanged *pWifiConnState = (tstrM2mWifiStateChanged *)pvMsg;
        if (pWifiConnState->u8CurrState == M2M_WIFI_CONNECTED)
        {    
            printf("Connected\r\n");
        }
        else
        {
            printf("Disconnected\r\n");
        }

        break;
    }

    case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        // read it from event data
        char buf[M2M_INET4_ADDRSTRLEN];  
        tstrM2MIPConfig *p_ipConfig = (tstrM2MIPConfig *)pvMsg;

        // convert binary IP address to string
        inet_ntop4(p_ipConfig->u32StaticIp, buf);
        printf("IP address assigned: %s\r\n", buf);
        ConnectToAP = true; 
        break;
    }

    default:
        break;
    }
}

static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg)
{
    uint32_t startTime = 0;
    
    switch (message) 
    {
    case M2M_SOCKET_BIND_EVENT:
        if (m2m_wifi_get_socket_event_data()->bindStatus == 0) 
        {
            printf("bind success!\r\n");
            listen(tcp_server_socket, 0);
        }
        else
        {
            printf("bind error!\r\n");
            close(tcp_server_socket);
            tcp_server_socket = -1;
        }
        break;

    case M2M_SOCKET_LISTEN_EVENT:
        if (m2m_wifi_get_socket_event_data()->listenStatus == 0) 
        {
            printf("listen success!\r\n");
            accept(tcp_server_socket, NULL, NULL);
        }
        else
        {
            printf("listen error!\r\n");
            close(tcp_server_socket);
            tcp_server_socket = -1;
        }
        break;

    case M2M_SOCKET_ACCEPT_EVENT:
        printf("accept success!\r\n");
        tcp_client_socket = m2m_wifi_get_socket_event_data()->acceptResponse.sock;
        recv(tcp_client_socket, socketTestBuffer, sizeof(socketTestBuffer), 0);
        break;

    case M2M_SOCKET_RECV_EVENT:
        if (m2m_wifi_get_socket_event_data()->recvMsg.bufSize > 0) 
        {   
            t_socketRecv *pRecvMsg = &(m2m_wifi_get_socket_event_data()->recvMsg);

            printf("Recv success, size = %d\r\n", pRecvMsg->bufSize);            
            if (true == get_AP_Parameter(m2m_wifi_get_socket_event_data()->recvMsg.p_rxBuf)) 
            {
                close(tcp_server_socket);
                printf("Disable AP mode\r\n");
                m2m_wifi_disable_ap();
                printf("Waiting 1 sec ...\r\n");
                startTime = m2mStub_GetOneMsTimer();
                while (m2m_get_elapsed_time(startTime) < 1000) ;
                printf("Connecting to %s.\r\n", (char *)str_ssid);
                m2m_wifi_connect((char *)str_ssid, strlen(str_ssid), sec_type, str_pw, M2M_WIFI_CH_ALL);
                break;
            }

            memset(socketTestBuffer, 0, sizeof(socketTestBuffer));
            recv(tcp_client_socket, socketTestBuffer, sizeof(socketTestBuffer), 0);
        } 
        else 
        {
            printf("recv error!\r\n");
            //close(tcp_server_socket);
            //tcp_server_socket = -1;
        }
        break;

    default:
        break;
    }
}

#endif
