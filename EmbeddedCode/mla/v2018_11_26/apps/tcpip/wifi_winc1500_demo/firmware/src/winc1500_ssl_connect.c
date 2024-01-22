/*******************************************************************************
  File Name:
    winc1500_ssl_connect.c

  Summary:
    WINC1500 ssl connect demo.

  Description:
    The configuration defines for this demo are:
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        HOST_NAME             -- Host name           "www.google.com"
        HOST_PORT             -- Host port           443

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

#if defined(USING_SSL_CONNECT)

/** Wi-Fi Settings */
#define WLAN_SSID                  "DEMO_AP" /**< Destination SSID */
#define WLAN_AUTH                  M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define WLAN_PSK                   "12345678" /**< Password for Destination SSID */

/** Using IP address. */
#define IPV4_BYTE(val, index)      ((val >> (index * 8)) & 0xFF)

/** All SSL defines */
#define HOST_NAME                  "www.google.com"
#define HOST_PORT                  443

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

typedef enum {
    SocketInit = 0,
    SocketConnect,
    SocketWaiting,
    SocketComplete,
    SocketError
} eSocketStatus;

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WORK,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

/** IP address of host. */
uint32_t g_HostIp = 0;

uint8_t g_SocketStatus = SocketInit;

/** TCP client socket handler. */
static SOCKET tcp_client_socket = -1;
/** Wi-Fi status variable. */
static bool gbConnectedWifi = false;

/** Get host IP status variable. */
static bool gbHostIpByName = false;

static int8_t sslConnect(void);
static void wifi_cb(uint8_t msgType, void *pvMsg);
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);
static void resolve_cb(char *pu8DomainName, uint32_t serverIP);

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
        printf("SSL Connect Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("host: %s\r\n", HOST_NAME);
        printf("port: %u\r\n", HOST_PORT);
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);

        /* Connect to router. */
        m2m_wifi_connect((char *)WLAN_SSID,
                         strlen(WLAN_SSID),
                         WLAN_AUTH,
                         (char *)WLAN_PSK,
                         M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WORK);
        break;

    case APP_STATE_WORK:
        if (gbConnectedWifi && gbHostIpByName)
        {
            if (g_SocketStatus == SocketInit)
            {
                if (tcp_client_socket < 0)
                {
                    g_SocketStatus = SocketWaiting;
                    sslEnableCertExpirationCheck(0);
                    if (sslConnect() != SOCK_ERR_NO_ERROR)
                    {
                        g_SocketStatus = SocketInit;
                    }
                }
            }
        }
        break;

    case APP_STATE_DONE:
        break;
    }
}

/**
 * \brief Creates and connects to a secure socket to be used for SSL.
 *
 * \param[in] None.
 *
 * \return SOCK_ERR_NO_ERROR if success, -1 if socket create error, SOCK_ERR_INVALID if socket connect error.
 */
static int8_t sslConnect(void)
{
    struct sockaddr_in addr_in;

    addr_in.sin_family = AF_INET;
    addr_in.sin_port = _htons(HOST_PORT);
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
        switch (message) {
        case M2M_SOCKET_CONNECT_EVENT:
        {
            t_socketConnect *pstrConnect = (t_socketConnect *)pvMsg;
            if (pstrConnect && pstrConnect->error >= SOCK_ERR_NO_ERROR)
            {
                printf("Successfully connected.\r\n");
            }
            else
            {
                printf("Connect error! code(%d)\r\n", pstrConnect->error);
                g_SocketStatus = SocketError;
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
 * \param[in] msgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_CONN_STATE_CHANGED_EVENT](@ref M2M_WIFI_CONN_STATE_CHANGED_EVENT)
 *  - [M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT](@ref M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type.
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
        gethostbyname((const char *)HOST_NAME);
        break;
    }

    default:
    {
        break;
    }
    }
}
#endif
