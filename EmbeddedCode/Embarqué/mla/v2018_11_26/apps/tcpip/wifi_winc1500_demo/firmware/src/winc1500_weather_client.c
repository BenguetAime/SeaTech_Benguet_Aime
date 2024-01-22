/*******************************************************************************
  File Name:
    winc1500_weather_client.c

  Summary:
    WINC1500 weather client demo.

  Description:
    This demo performs the following steps:
        1) Starts Wi-Fi connection to the specified Access Point (AP)
        2) Waits for the connection and get IP address
        3) Send DNS for server's IP address
        4) Send weather request to weather server.
        5) print weather information to UART port.
 
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

#if defined(USING_WEATHER_CLIENT)

//==============================================================================
// DEMO CONFIGURATION
//==============================================================================

/** Wi-Fi AP Settings. */
#define WLAN_SSID              "DEMO_AP"             // target AP
#define WLAN_AUTH              M2M_WIFI_SEC_WPA_PSK   // AP Security 
#define WLAN_PSK               "12345678"            // security password

#define WIFI_BUFFER_SIZE       1400                  // Receive buffer size.
#define SERVER_PORT            (80)                  // Using broadcast address for simplicity

#define PREFIX_BUFFER          "GET /data/2.5/weather?q="
#define POST_BUFFER            "&appid=c592e14137c3471fa9627b44f6649db4&mode=xml&units=metric HTTP/1.1\r\nHost: api.openweathermap.org\r\nAccept: */*\r\n\r\n"
#define WEATHER_SERVER_NAME    "api.openweathermap.org"  // Weather information provider server
#define CITY_NAME              "paris"    // Input City Name

#define IPV4_BYTE(val, index)  ((val >> (index * 8)) & 0xFF)  // IP address parsing.
#define HEX2ASCII(x)           (((x) >= 10) ? (((x) - 10) + 'A') : ((x) + '0'))

#define SetAppState(state)     g_appState = state
#define GetAppState()          g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_CONNECT_AND_DHCP,
    APP_STATE_WORKING,  
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static uint8_t s_ReceivedBuffer[WIFI_BUFFER_SIZE] = {0}; // Receive buffer
static SOCKET tcp_client_socket = -1; // TCP client socket handlers
static struct sockaddr_in addr_in;
static uint32_t s_HostIp = 0;         // IP address of host.
static bool s_ConnectedWifi = false;  // Wi-Fi status
static bool s_HostIpByName = false;   // host IP status
static bool s_TcpConnection = false;  // TCP Connection status
static char g_ssid[] = {WLAN_SSID};

static void wifi_cb(uint8_t msgType, void *pvMsg);
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);
static void resolve_cb(char *pu8DomainName, uint32_t serverIP);

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
        printf("Weather Client Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("host: %s\r\n", WEATHER_SERVER_NAME);
        printf("port: %u\r\n", SERVER_PORT);
        printf("=========\r\n");
        
        printf("-- WINC1500 weather client example --\r\n");
        printf("Starting ...\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);
        
        m2m_wifi_connect(g_ssid,
                         strlen(WLAN_SSID),
                         WLAN_AUTH,
                         (void *)WLAN_PSK,
                         M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WAIT_CONNECT_AND_DHCP);
        break;

    case APP_STATE_WAIT_CONNECT_AND_DHCP:
        if (s_ConnectedWifi)
        {
            if (s_HostIpByName) 
            {
                /* Open TCP client socket. */
                if (tcp_client_socket < 0) 
                {
                    if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                    {
                        printf("main: failed to create TCP client socket error!\r\n");
                        SetAppState(APP_STATE_DONE);
                        break;
                    }
                }

                /* Connect TCP client socket. */
                addr_in.sin_family = AF_INET;
                addr_in.sin_port = _htons(SERVER_PORT);
                addr_in.sin_addr.s_addr = s_HostIp;
                if (connect(tcp_client_socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) 
                {
                    printf("main: failed to connect socket error!\r\n");
                    SetAppState(APP_STATE_DONE);
                    break;
                }

                s_TcpConnection = true;
                SetAppState(APP_STATE_WORKING);
            }
        }
        break;

    case APP_STATE_WORKING:       
            
        break;
        
    case APP_STATE_DONE:
        break;
    default:
        break;
    }
}

static void resolve_cb(char *hostName, uint32_t hostIp)
{
    s_HostIp = hostIp;
    s_HostIpByName = true;
    printf("%s IP address is %d.%d.%d.%d\r\n\r\n", hostName,
            (int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
            (int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
}

static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg)
{
    /* Check for socket event on TCP socket. */
    if (sock == tcp_client_socket) 
    {
        switch (message) {
        case M2M_SOCKET_CONNECT_EVENT:
        {
            if (s_TcpConnection) 
            {
                memset(s_ReceivedBuffer, 0, sizeof(s_ReceivedBuffer));
                sprintf((char *)s_ReceivedBuffer, "%s%s%s", PREFIX_BUFFER, (char *)CITY_NAME, POST_BUFFER);

                t_socketConnect *pstrConnect = (t_socketConnect *)pvMsg;
                if (pstrConnect && pstrConnect->error >= SOCK_ERR_NO_ERROR) 
                {
                    send(tcp_client_socket, s_ReceivedBuffer, strlen((char *)s_ReceivedBuffer), 0);
                    memset(s_ReceivedBuffer, 0, WIFI_BUFFER_SIZE);
                    recv(tcp_client_socket, &s_ReceivedBuffer[0], WIFI_BUFFER_SIZE, 0);
                } 
                else 
                {
                    printf("connect error!\r\n");
                    s_TcpConnection = false;
                    close(tcp_client_socket);
                    tcp_client_socket = -1;
                }
            }
        }
        break;

        case M2M_SOCKET_RECV_EVENT:
        {
            char *pcIndxPtr;
            char *pcEndPtr;

            t_socketRecv *pstrRecv = (t_socketRecv *)pvMsg;
            if (pstrRecv && pstrRecv->bufSize > 0) 
            {
                /* Get city name. */
                pcIndxPtr = strstr((char *)pstrRecv->p_rxBuf, "name=");
                printf("City: ");
                if (NULL != pcIndxPtr) 
                {
                    pcIndxPtr = pcIndxPtr + strlen("name=") + 1;
                    pcEndPtr = strstr(pcIndxPtr, "\">");
                    if (NULL != pcEndPtr) 
                    {
                        *pcEndPtr = 0;
                    }

                    printf("%s\r\n", pcIndxPtr);
                } 
                else 
                {
                    printf("N/A\r\n");
                    break;
                }

                /* Get temperature. */
                pcIndxPtr = strstr(pcEndPtr + 1, "temperature value");
                printf("Temperature: ");
                if (NULL != pcIndxPtr) 
                {
                    pcIndxPtr = pcIndxPtr + strlen("temperature value") + 2;
                    pcEndPtr = strstr(pcIndxPtr, "\" ");
                    if (NULL != pcEndPtr) 
                    {
                        *pcEndPtr = 0;
                    }

                    printf("%s\r\n", pcIndxPtr);
                } 
                else 
                {
                    printf("N/A\r\n");
                    break;
                }

                /* Get weather condition. */
                pcIndxPtr = strstr(pcEndPtr + 1, "weather number");
                if (NULL != pcIndxPtr) 
                {
                    printf("Weather Condition: ");
                    pcIndxPtr = pcIndxPtr + strlen("weather number") + 14;
                    pcEndPtr = strstr(pcIndxPtr, "\" ");
                    if (NULL != pcEndPtr) 
                    {
                        *pcEndPtr = 0;
                    }
                    printf("%s\r\n", pcIndxPtr);
                    
                    /* Response processed, now close connection. */
                    close(tcp_client_socket);
                    tcp_client_socket = -1;
                    break;
                }

                memset(s_ReceivedBuffer, 0, sizeof(s_ReceivedBuffer));
                recv(tcp_client_socket, &s_ReceivedBuffer[0], WIFI_BUFFER_SIZE, 0);
            } 
            else 
            {
                printf("recv error!\r\n");
                close(tcp_client_socket);
                tcp_client_socket = -1;
            }
        }
        break;

        default:
            break;
        }
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
            printf("Connected -- starting DHCP client\r\n");
        }
        else if (pWifiConnState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            s_ConnectedWifi = false;
            printf("disconnected -- starting connect again\r\n");
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
        printf("IP address is %u.%u.%u.%u\r\n",
                pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
        s_ConnectedWifi = true;
        /* Obtain the IP Address by network name */
        gethostbyname((const char *)WEATHER_SERVER_NAME);
        break;
    }

    default:
    {
        break;
    }
    }
}

#endif
