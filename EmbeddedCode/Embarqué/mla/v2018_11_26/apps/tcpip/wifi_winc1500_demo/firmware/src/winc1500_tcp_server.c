/*******************************************************************************
  File Name:
    winc1500_tcp_server.c

  Summary:
    WINC1500 tcp server demo.

Description:
    This demo performs the following steps:
        1) Starts Wi-Fi connection to the specified Access Point (AP)
        2) Waits for the connection event
        3) Waits for an IP address to be assigned to the WINC1500
        4) Creates a TCP server socket and wait for a connection requestion from tcp client
        5) Sends a TCP packet to the TCP client
        6) Waits for TCP send to complete (M2M_SOCKET_SEND_EVENT)
        7) Receive a TCP packet from TCP client
        8) repeat sending and receiving for 10 times
        9) Closes TCP socket, demo is done
    
    The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        TCP_SERVER_PORT       -- Port number of UDP server
        TCP_SEND_MESSAGE      -- Message which will be sent to TCP server
 
    For testing, please create a simple TCP client for this demo. 
    Use the netcat program on a Linux PC connected to the same AP as used by this demo.  
    The command line is:
        netcat 192.168.xx.xx 6001 (use IP address assigned to WINC1500 and port number as
                                      defined by TCP_SERVER_PORT)
 
    After entering the above command line (and after the Host is connected to the MCU), 
    type some text, followed by the <ENTER> key, which sends the previously typed characters 
    to the WINC1500.  The characters are echoed on the serial terminal.
    Then PC will receive a message "TCP Message from WINC1500 module\r\n" from MCU + WINC1500. 
    
    The demo uses these callback functions to handle events:
        socket_cb()
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

#if defined(USING_TCP_SERVER) 
#define WLAN_SSID   "DEMO_AP"             // target AP
#define WLAN_AUTH   M2M_WIFI_SEC_WPA_PSK   // AP Security 
#define WLAN_PSK    "12345678"            // security password

#define TCP_SEND_MESSAGE        "TCP Message from WINC1500 module\r\n"
#define TCP_SERVER_PORT         (6001)
#define WIFI_BUFFER_SIZE        1460
#define PRINT_SIZE_OF_RECEIVE   20 // if we receive a large size packet, we don't printf all data

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_CONNECT_AND_DHCP,
    APP_STATE_SOCKET_TEST,
    APP_STATE_WORK,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

/** Message format definitions. */
typedef struct s_msg_send {
    uint8_t message[sizeof(TCP_SEND_MESSAGE)];
} t_msg_send;

/** Message format declarations. */
static t_msg_send msg_tcp_send = {TCP_SEND_MESSAGE};

/** Receive buffer definition. */
static uint8_t socketTestBuffer[WIFI_BUFFER_SIZE];

/** Socket for TCP communication */
static SOCKET tcp_server_socket = -1;
static SOCKET tcp_client_socket = -1;
static char g_ssid[] = {WLAN_SSID};
struct sockaddr_in addr;
static bool wifi_connected = false;
static void wifi_cb(uint8_t msgType, void *pvMsg);
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);

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
        printf("TCP Server Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("port: %u\r\n", TCP_SERVER_PORT);
        printf("=========\r\n"); 
        printf("Starting ...\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, NULL);
        
        m2m_wifi_connect(g_ssid,
                         strlen(WLAN_SSID),
                         WLAN_AUTH,
                         (void *)WLAN_PSK,
                         M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WAIT_CONNECT_AND_DHCP);
        break;

    case APP_STATE_WAIT_CONNECT_AND_DHCP:
        if (wifi_connected)
        {
            SetAppState(APP_STATE_SOCKET_TEST); 
        }
        break;

    case APP_STATE_SOCKET_TEST:
        /* Initialize socket address structure. */
        addr.sin_family = AF_INET;
        addr.sin_port = _htons(TCP_SERVER_PORT);
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
            SetAppState(APP_STATE_WORK); 
        }
        break;
   
    case APP_STATE_WORK:
        break;
    case APP_STATE_DONE:
        break;
    }
}

static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg)
{
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
        tcp_client_socket = m2m_wifi_get_socket_event_data()->acceptResponse.sock;
        printf("accept success, socket = %d!\r\n", tcp_client_socket);
        recv(tcp_client_socket, socketTestBuffer, sizeof(socketTestBuffer), 0);
        break;

    case M2M_SOCKET_SEND_EVENT:
        printf("Send success, sock = %d, size = %d\r\n", sock, sizeof(t_msg_send));
        recv(tcp_client_socket, socketTestBuffer, sizeof(socketTestBuffer), 0);
        break;

    case M2M_SOCKET_RECV_EVENT:
        if (m2m_wifi_get_socket_event_data()->recvMsg.bufSize > 0) 
        {   
            int i, printCount; 
            char *p_send;

            t_socketRecv *pRecvMsg = &(m2m_wifi_get_socket_event_data()->recvMsg);

            printf("Recv success, sock = %d, size = %d, message = ", sock, pRecvMsg->bufSize);
            printCount = (pRecvMsg->bufSize < PRINT_SIZE_OF_RECEIVE) ? pRecvMsg->bufSize : PRINT_SIZE_OF_RECEIVE;
            for (i = 0; i< printCount; i++)
                printf("%c", pRecvMsg->p_rxBuf[i]);
            printf("\r\n");
            send(tcp_client_socket, &msg_tcp_send, sizeof(t_msg_send), 0);
            p_send = (char *)&msg_tcp_send;
            printf("send to client, message = ");
            for (i = 0; i< sizeof(t_msg_send); i++)
            {
                printf("%c", p_send[i]);
            }
        } 
        else 
        {
            printf("recv error or disconnect by remote client, sock = %d!\r\n", sock);
            close(sock);
        }
        break;

    default:
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
            printf("Connected -- starting DHCP client\r\n");
        } 
        else if (pWifiConnState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            wifi_connected = false;
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
        // read it from event data
        char buf[M2M_INET4_ADDRSTRLEN];  
        tstrM2MIPConfig *p_ipConfig = (tstrM2MIPConfig *)pvMsg;

        // convert binary IP address to string
        inet_ntop4(p_ipConfig->u32StaticIp, buf);
        printf("IP address assigned: %s\r\n", buf);
        wifi_connected = true;
        break;
    }

    default:
    {
        break;
    }
    }
}

#endif
