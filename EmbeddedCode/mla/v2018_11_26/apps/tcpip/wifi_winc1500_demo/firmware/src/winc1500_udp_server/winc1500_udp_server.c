/*******************************************************************************
  File Name:
    winc1500_udp_server.c

  Summary:
    WINC1500 udp server demo.

  Description:
    This demo performs the following steps:
        1) Starts Wi-Fi connection to the specified Access Point (AP)
        2) Waits for the connection event
        3) Waits for an IP address to be assigned to the WINC1500
        4) Creates a UDP server socket
        5) Waits forever for messages from a UDP client.

     The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        SERVER_PORT_NUMBER    -- port number for the UDP server
        BUFFER_SIZE           -- Size of the socket buffer holding the Rx data
 
    To create a simple UDP client for this demo, use the netcat program on a Linux
    PC connected to the same AP as used by this demo.  The command line is:
        netcat -u 192.168.1.100 50008 (use IP address assigned to WINC1500 and port number as
                                      defined by SERVER_PORT_NUMBER)
 
    After entering the above command line (and after the Host is connected to the MCU),
    type some text, followed by the <ENTER> key, which sends the previously typed characters 
    to the WINC1500.  The characters are echoed on the serial terminal.
 
    Another method of creating of UDP client is to run the python script at the bottom of this
    file using Python 3.x.
 
    This demo also shows an example of how to monitor if the Wi-Fi connection is lost, and
    how to regain the connection.  This can be tested by simply unplugging the AP and then
    plugging it back in.
 
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

#if defined(USING_UDP_SERVER)

//=============================
// DEMO CONFIGURATION CONSTANTS
//=============================
#define WLAN_SSID           "DEMO_AP"               // target AP
#define WLAN_AUTH           M2M_WIFI_SEC_OPEN       // AP Security 
#define WLAN_PSK            "12345678"              // security password
#define SERVER_PORT_NUMBER  50008                   // server port number
#define BUFFER_SIZE         1460                    // size of rx buffer

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_FOR_CONNECT,
    APP_STATE_WAIT_DHCP_CLIENT,
    APP_STATE_CREATE_SOCKET,
    APP_STATE_SOCKET_WAIT_FOR_BIND,
    APP_STATE_SOCKET_WAIT_FOR_RECV,
    APP_STATE_WAIT_FOR_CONNECTION_RECOVERY,            
    APP_STATE_DONE
} t_AppState;

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState


//==============================================================================
// Local Globals
//==============================================================================
static t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static char g_ssid[] = {WLAN_SSID};

// application state machine called from main()
void ApplicationTask(void)
{
    static uint8_t socketBuffer[BUFFER_SIZE];    
    static SOCKET udpSocket;
    static struct sockaddr_in addr;
    static int restoreState;
    static bool monitorConnection = false;
    int status;
    int i;
    
    
    // if currently monitoring for loss of Wi-Fi connection and we got an event
    // that the connection state changed and the connection was lost
    if ((monitorConnection)              && 
        (isConnectionStateChanged())     &&
        (m2m_wifi_get_wifi_event_data()->connState.u8CurrState == M2M_WIFI_DISCONNECTED)) 
    {
        printf("Lost Wi-Fi connection; attempting to recover\r\n");
        monitorConnection = false;      // will resume monitoring connection after it is restored        
        restoreState = GetAppState();   // remember state to go back to when connection restored
        m2m_wifi_connect(g_ssid,           // attempt to reconnect
                         strlen(WLAN_SSID),
                         WLAN_AUTH,
                         (void *)WLAN_PSK,
                         M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WAIT_FOR_CONNECTION_RECOVERY);
    }
    
    switch (GetAppState())
    {
        case APP_STATE_WAIT_FOR_DRIVER_INIT:
            if (isDriverInitComplete())
            {
                SetAppState(APP_STATE_START);
            }
            break;
            
        case APP_STATE_START:
            printf("\r\n===============\r\n");            
            printf("UDP Server Demo\r\n");
            printf("===============\r\n");
            printf("Connecting to %s ...\r\n", WLAN_SSID);
            monitorConnection = false;
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
                    monitorConnection = true;
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
            // create UDP server socket
            udpSocket = socket(AF_INET, SOCK_DGRAM, 0); 
            printf("Creating UDP server socket on port %u\r\n", (uint16_t)SERVER_PORT_NUMBER);
            
            // if successful
            if (udpSocket >= 0)
            {
                //Initialize socket address structure and bind socket to port
                addr.sin_family      = AF_INET;
                addr.sin_port        = _htons(SERVER_PORT_NUMBER);
                addr.sin_addr.s_addr = 0; // will use DHCP-assigned IP address
                bind(udpSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)); // bind socket
                SetAppState(APP_STATE_SOCKET_WAIT_FOR_BIND); 
            }
            else
            {
                printf("Failed to create RX UDP server socket error!\r\n");
                SetAppState(APP_STATE_DONE);
                break;
            }
            break;
            
        case APP_STATE_SOCKET_WAIT_FOR_BIND:
            // if socket bind event occurred
            if (isSocketBindOccurred())
            {
                // if bind was successful
                int8_t bindStatus = m2m_wifi_get_socket_event_data()->bindStatus;
                if (bindStatus == SOCK_ERR_NO_ERROR) 
                {
                    printf("Bind complete\r\n");
                    printf("Ready to receive data\r\n\r\n");
                   
                    // start listening for incoming packets
                    status = recvfrom(udpSocket, socketBuffer, BUFFER_SIZE, 0); 
                    if (status != SOCK_ERR_NO_ERROR)
                    {
                        printf("recvfrom error: %d\r\n", status);
                        SetAppState(APP_STATE_DONE);
                    }
                    SetAppState(APP_STATE_SOCKET_WAIT_FOR_RECV); 
                }
                else
                {
                    printf("Bind failed, error = %d", bindStatus);
                    SetAppState(APP_STATE_DONE);
                }
            }
            break;
     
        case APP_STATE_SOCKET_WAIT_FOR_RECV:
            // if received a packet
            if (isSocketRecvFromOccurred())
            {
                int packetSize;
                uint32_t u32StaticIp;
                uint16_t port;
                char ipAddrString[M2M_INET4_ADDRSTRLEN];
                t_socketRecv *p_recv = &m2m_wifi_get_socket_event_data()->recvMsg;
                
                // get the packet size from the event data
                packetSize = p_recv->bufSize;

                // get clients IP address and convert to string
                u32StaticIp = p_recv->ai_addr.sin_addr.s_addr;
                port      = p_recv->ai_addr.sin_port;
                inet_ntop4(u32StaticIp, ipAddrString);
                if (packetSize > 0) 
                {
                    printf("\r\nReceived UDP client message (%d bytes) from %s (port %u):\r\n   ", packetSize, ipAddrString, port);
                    // read the packet data from the WINC1500
                    status = recvfrom(udpSocket, socketBuffer, BUFFER_SIZE, 0);
                    if (status == SOCK_ERR_NO_ERROR)
                    {
                        // output the the packet data
                        for (i = 0; i < packetSize; ++i)
                        {
                            printf("%c", socketBuffer[i]);
                        }
                        printf("\r\n");
                    }
                    else
                    {
                        printf("recvfrom error: %d\r\n", status);
                        SetAppState(APP_STATE_DONE);
                    }
                }
            }
            break;
            
        case APP_STATE_WAIT_FOR_CONNECTION_RECOVERY:
            // if connection state changed
            if (isConnectionStateChanged()) 
            {
                // if connection was regained
                if (m2m_wifi_get_wifi_event_data()->connState.u8CurrState == M2M_WIFI_CONNECTED)
                {
                    printf("Connection restored\r\n");
                    SetAppState(restoreState);  // go back to state prior to connection loss
                    monitorConnection = true;   // resume monitoring connection
                }
                // else still no connection, try again
                else
                {
                    printf("Retrying...\r\n");
                    m2m_wifi_connect(g_ssid,           // attempt to reconnect
                                     strlen(WLAN_SSID),
                                     WLAN_AUTH,
                                     (void *)WLAN_PSK,
                                     M2M_WIFI_CH_ALL);
                }
            }
            break;
        
        case APP_STATE_DONE:
            break;
    }
}


/*==============================================================================
 Below is an example python 3.x script that creates a UDP client with which to 
 test the above UDP server demo.  Save the python code below into a file named
 udp_client.py.  Then run the script by the command line: 
        python.exe udp_client.py

#------------------- 
# script starts here
#------------------- 
import socket
import time
import array

serverIpAddress  = "192.168.1.100"      # change to IP address of server
serverPortNumber = 6666                 # change to port number of server

# data portion of UDP Packet
dataBlock = '''\
  This is line 1 of 4
  This is line 2 of 4
  This is line 3 of 4
  This is line 4 of 4
'''

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)    # create UDP client socket

for i in range(1, 10000):
    packet = "Packet {0}\n".format(i) + dataBlock                                   # create packet to send
    res = s.sendto(bytes(packet, "utf-8"), (serverIpAddress, serverPortNumber))     # send it
    print("Packet {0} of length {1} sent".format(i, res))                           # print number of bytes in packet
    time.sleep(0.5)                                                                 # sleep and repeat
 
#----------------- 
# script ends here
#----------------- 
 
*/    

#endif // USING_UDP_SERVER
