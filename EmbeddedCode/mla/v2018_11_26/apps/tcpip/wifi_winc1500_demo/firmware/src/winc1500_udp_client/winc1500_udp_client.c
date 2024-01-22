/*******************************************************************************
  File Name:
    winc1500_udp_client.c

  Summary:
    WINC1500 udp client demo.

  Description:
    This demo performs the following steps:
        1) Starts Wi-Fi connection to the specified Access Point (AP)
        2) Waits for the connection event
        3) Waits for an IP address to be assigned to the WINC1500
        4) Creates a UDP client socket
        5) Sends a UDP packet to the UDP server
        6) Waits for UDP send to complete (M2M_SOCKET_SEND_EVENT)
        7) Repeats steps (5) and (6) until all UDP packets are sent
        8) Closes UDP client socket
    
    The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        UDP_SERVER_IP_ADDRESS -- IP address of UDP server
        UDP_SERVER_PORT       -- Port number of UDP server
        NUM_PACKETS_TO_SEND   -- Number of UDP packets to send to the server
 
    To set up a UDP server, on a Linux machine connected to the same AP as the WINC1500,
    run the following command from a console window:
        netcat -l -u 50008
 
     This runs a simple UDP server (port 50008) that displays any data it receives.
 
    Note: In the state, APP_STATE_SOCKET_WAIT_FOR_SEND, the return code from 
          sendto() is being checked.  If the return code indicates SOCK_ERR_BUFFER_FULL
          that means the WINC1500 buffers are full and it cannot accept any more 
          packets.  When sending packets back-to-back in rapid fashion, as occurs in 
          this demo, it is very possible for the WINC1500 buffers to become temporarily full.
          Thus, if the SOCK_ERR_BUFFER_FULL error occurs, the same packet is simply
          re-sent.
 
    The demo gets notified of events by application-specific flags flags set
    in m2m_wifi_handle_events() and m2m_socket_handle_events().
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

#if defined(USING_UDP_CLIENT) 

//=============================
// DEMO CONFIGURATION CONSTANTS
//=============================
#define WLAN_SSID               "DEMO_AP"           // target AP
#define WLAN_AUTH               M2M_WIFI_SEC_OPEN   // AP Security (see tenuM2mSecType)
#define WLAN_PSK                "12345678"          // security passphrase (if security used)
#define UDP_SERVER_IP_ADDRESS   "192.168.1.102"     // UDP server IP address
#define UDP_SERVER_PORT         50008               // UDP server port number
#define NUM_PACKETS_TO_SEND     10                  // number of UDP packets to send

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_FOR_CONNECT,
    APP_STATE_WAIT_DHCP_CLIENT,
    APP_STATE_CREATE_SOCKET,
    APP_STATE_SOCKET_WAIT_FOR_SEND,
    APP_STATE_SOCKET_CLOSE,
    APP_STATE_DONE
} t_AppState;

// State machine macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

//==============================================================================
// Local Globals
//==============================================================================
static t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static char g_ssid[] = {WLAN_SSID};

// 50 bytes of packet data (will be after the packet count)
static const char udpData[] = "01234567890123456789012345678901234567890123456789";

// application state machine called from main()
void ApplicationTask(void)
{
    int8_t retVal;
    char   pktBuf[128];
    static SOCKET udpSocket = -1;
    static uint16_t packetCnt = 0;  
    static struct sockaddr_in addr;
    
    switch (GetAppState())
    {
        case APP_STATE_WAIT_FOR_DRIVER_INIT:
            if (isDriverInitComplete())
            {
                SetAppState(APP_STATE_START);
            }
            break;

        case APP_STATE_START:
            ClearSocketEventStates();
            ClearWiFiEventStates();            
            printf("\r\n===============\r\n");            
            printf("UDP Client Demo\r\n");
            printf("===============\r\n");

            printf("Connecting to %s ...\r\n", WLAN_SSID);
            m2m_wifi_connect(g_ssid,
                             strlen(WLAN_SSID),                    
                             WLAN_AUTH,
                            (void *)WLAN_PSK,
                            M2M_WIFI_CH_ALL);
            SetAppState(APP_STATE_WAIT_FOR_CONNECT);
            break;
            
        case APP_STATE_WAIT_FOR_CONNECT:
            if (isConnectionStateChanged())
            {
                // if connection successful
                if (m2m_wifi_get_wifi_event_data()->connState.u8CurrState == M2M_WIFI_CONNECTED)
                {
                    // start DHCP client to get an IP address
                    printf("Connected -- starting DHCP client\r\n");
                    SetAppState(APP_STATE_WAIT_DHCP_CLIENT);
                }
                // else failed to connect to AP
                else
                {
                    printf("Unable to connect (check security settings)\r\n");
                    SetAppState(APP_STATE_DONE);
                }
            }
            break;
            
        case APP_STATE_WAIT_DHCP_CLIENT:
            // if IP address assigned
            if (isIpAddressAssigned())
            {
                // read it from event data
                char buf[M2M_INET4_ADDRSTRLEN];  
                tstrM2MIPConfig *p_ipConfig = &m2m_wifi_get_wifi_event_data()->ipConfig;  // pointer to result structure

                // convert binary IP address to string
                inet_ntop4(p_ipConfig->u32StaticIp, buf);
                printf("IP address assigned: %s\r\n", buf);

                SetAppState(APP_STATE_CREATE_SOCKET); 
            }
            break;
            
        case APP_STATE_CREATE_SOCKET:
            /* Initialize socket address structure. */
            addr.sin_family      = AF_INET;
            addr.sin_port        = _htons(UDP_SERVER_PORT);
            inet_pton4(UDP_SERVER_IP_ADDRESS, &addr.sin_addr.s_addr);

            printf("Preparing to send data to UDP server %s:%u\r\n", UDP_SERVER_IP_ADDRESS, (uint16_t)UDP_SERVER_PORT);
            if (udpSocket < 0) 
            {
                if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
                {
                    printf("Failed to create RX UDP Client socket error!\r\n");
                    break;
                }
            }
            sprintf(pktBuf, "Pkt %d: %s\r\n", packetCnt++, udpData);
            sendto(udpSocket, &pktBuf, strlen(pktBuf), 0, (struct sockaddr *)&addr, sizeof(addr));
            SetAppState(APP_STATE_SOCKET_WAIT_FOR_SEND); 
            break;
            
        case APP_STATE_SOCKET_WAIT_FOR_SEND:
            if (isSocketSendToOccurred())
            {
                if (packetCnt >= NUM_PACKETS_TO_SEND) 
                {
                    printf("%u packets sent to UDP server\r\n", NUM_PACKETS_TO_SEND);
                    SetAppState(APP_STATE_SOCKET_CLOSE);
                } 
                else 
                {
                    sprintf(pktBuf, "Pkt %d: %s\r\n", packetCnt++, udpData);
                    do {
                        retVal = sendto(udpSocket, &pktBuf, strlen(pktBuf), 0, (struct sockaddr *)&addr, sizeof(addr));
                    } while (retVal == SOCK_ERR_BUFFER_FULL); 
                }
            }
            break;
            
        case APP_STATE_SOCKET_CLOSE:
            printf("UDP client test Complete!\r\n");
            close(udpSocket);
            udpSocket = -1;
            SetAppState(APP_STATE_DONE);
            break;
            
        case APP_STATE_DONE:
            break;
    }
}

#endif // USING_UDP_CLIENT
