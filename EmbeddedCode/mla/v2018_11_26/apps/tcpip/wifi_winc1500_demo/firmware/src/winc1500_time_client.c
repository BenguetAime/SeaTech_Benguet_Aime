/*******************************************************************************
  File Name:
    winc1500_time_client.c

  Summary:
    WINC1500 time client demo.

  Description:
    This demo performs the following steps:
        1) Starts Wi-Fi connection to the specified Access Point (AP)
        2) Waits for the connection and IP address
        3) Creates a UDP socket
        4) enable DNS
        5) Sends a UDP packet to request time
        6) Waits for UDP packet from NTP server
        7) If receive data from server, print time information, demo is done
        8) Waits 5 seconds, if cannot receive data from NTP server, 
           print warning message, demo is done
    
    The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        NTP_POOL_HOSTNAME     -- NTP server Domain. The demo default of ntp.microchip.com is only
                              -- reachable within the Microchip network.  An external time
                              -- server to try is wwv.nist.gov
        SERVER_PORT_FOR_UDP   -- NTP port number
        
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

#if defined(USING_TIME_CLIENT) 

// State macros
#define SetAppState(state)   g_appState = state
#define GetAppState()        g_appState

#define WLAN_SSID            "DEMO_AP"             // target AP
#define WLAN_AUTH            M2M_WIFI_SEC_WPA_PSK   // AP Security 
#define WLAN_PSK             "12345678"            // security password

#define MICROCHIP_NTP_POOL_HOSTNAME        "ntp.microchip.com" 
#define WORLDWIDE_NTP_POOL_HOSTNAME        "pool.ntp.org"
#define ASIA_NTP_POOL_HOSTNAME             "asia.pool.ntp.org"
#define EUROPE_NTP_POOL_HOSTNAME           "europe.pool.ntp.org"
#define NAMERICA_NTP_POOL_HOSTNAME         "north-america.pool.ntp.org"
#define OCEANIA_NTP_POOL_HOSTNAME          "oceania.pool.ntp.org"
#define SAMERICA_NTP_POOL_HOSTNAME         "south-america.pool.ntp.org"

#define NTP_POOL_HOSTNAME    MICROCHIP_NTP_POOL_HOSTNAME
//#define NTP_POOL_HOSTNAME    WORLDWIDE_NTP_POOL_HOSTNAME
#define SERVER_PORT_FOR_UDP  (123)
#define DEFAULT_ADDRESS      0xFFFFFFFF /* "255.255.255.255" */
#define DEFAULT_PORT         (6666)
#define WIFI_BUFFER_SIZE     1460
#define WAITING_FOR_OVERTIME (5000)
// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_FOR_CONNECT_AND_DHCP,
    APP_STATE_WAIT_FOR_TIMER,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

static SOCKET udp_socket = -1;
static struct sockaddr_in addr;
static uint8_t socketBuffer[WIFI_BUFFER_SIZE];
static bool wifi_connected = false;
static bool s_demoSuccess = false;
static char g_ssid[] = {WLAN_SSID};
static void wifi_cb(uint8_t msgType, void *pvMsg);
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);
static void resolve_cb(char *pu8DomainName, uint32_t serverIP);

// application state machine called from main()
void ApplicationTask(void)
{
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
        printf("time client Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("host: %s\r\n", NTP_POOL_HOSTNAME);
        printf("=========\r\n");
        printf("Starting ...\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);
        
        m2m_wifi_connect(g_ssid,
                         strlen(WLAN_SSID),
                         WLAN_AUTH,
                         (void *)WLAN_PSK,
                         M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WAIT_FOR_CONNECT_AND_DHCP);
        break;
        
    case APP_STATE_WAIT_FOR_CONNECT_AND_DHCP:
        if (wifi_connected)
        {
            udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
            if (udp_socket < 0) 
            {
                printf("main: UDP Client Socket Creation Failed.\r\n");
                SetAppState(APP_STATE_DONE); 
                break;
            }

            /* Initialize default socket address structure. */
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = _htonl(DEFAULT_ADDRESS);
            addr.sin_port = _htons(DEFAULT_PORT);
            bind(udp_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

            startTime  = m2mStub_GetOneMsTimer();
            SetAppState(APP_STATE_WAIT_FOR_TIMER); 
        }
        break;
        
    case APP_STATE_WAIT_FOR_TIMER:
        if (m2m_get_elapsed_time(startTime) > WAITING_FOR_OVERTIME)
        {
            if (s_demoSuccess == false)
            {
                printf("Cannot receive information from server!\r\n");
                printf("Time client test Complete!\r\n");
                close(udp_socket);
            	udp_socket = -1;
            }
            SetAppState(APP_STATE_DONE); 
        }
        
        break;
        
    case APP_STATE_DONE:
        break;
    }
}

static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
    /* Check for socket event on socket. */
	int16_t ret;

	switch (u8Msg) {
	case M2M_SOCKET_BIND_EVENT:
    {
    	if (m2m_wifi_get_socket_event_data()->bindStatus == 0)  
        {
        	ret = recvfrom(udp_socket, socketBuffer, WIFI_BUFFER_SIZE, 0);
        	if (ret != SOCK_ERR_NO_ERROR) 
            {
            	printf("recv error!\r\n");
            }
        } 
        else 
        {
        	printf("bind error!\r\n");
        }

    	break;
    }

	case M2M_SOCKET_RECVFROM_EVENT:
    {
    	t_socketRecv *pstrRx = (t_socketRecv *)pvMsg;
    	if (pstrRx->p_rxBuf && pstrRx->bufSize) 
        {
        	uint8_t packetBuffer[48];
        	memcpy(&packetBuffer, pstrRx->p_rxBuf, sizeof(packetBuffer));

        	if ((packetBuffer[0] & 0x7) != 4)    /* expect only server response */
            {         
            	printf("Expecting response from Server Only!\r\n");
            	return;                          /* MODE is not server, abort */
            } 
            else 
            {
            	uint32_t secsSince1900 = (uint32_t)packetBuffer[40] << 24 |
                        (uint32_t)packetBuffer[41] << 16 |
                        (uint32_t)packetBuffer[42] << 8 |
                        (uint32_t)packetBuffer[43];

                /* Now convert NTP time into everyday time.
                 * Unix time starts on Jan 1 1970. In seconds, that's 2208988800.
                 * Subtract seventy years.
                 */
            	const uint32_t seventyYears = 2208988800UL;
            	uint32_t epoch = secsSince1900 - seventyYears;

                /* Print the hour, minute and second.
                 * GMT is the time at Greenwich Meridian.
                 */
            	printf("The GMT time is %lu:%02lu:%02lu\r\n",
                        (epoch  % 86400L) / 3600,           /* hour (86400 equals secs per day) */
                        (epoch  % 3600) / 60,               /* minute (3600 equals secs per minute) */
                    	epoch % 60);                        /* second */

            	close(sock);
            	udp_socket = -1;
            	printf("time client test Complete!\r\n");
                s_demoSuccess = true;
            }
        }
        break;
    }
    
	default:
    	break;
    }
}

static void resolve_cb(char *pDomainName, uint32_t serverIP)
{
	struct sockaddr_in addr;
	int8_t cDataBuf[48];
	int16_t ret;

	memset(cDataBuf, 0, sizeof(cDataBuf));
	cDataBuf[0] = '\x1b'; /* time query */

	printf("DomainName: %s\r\n", pDomainName);

	if (udp_socket >= 0) 
    {
        /* Set NTP server socket address structure. */
    	addr.sin_family = AF_INET;
    	addr.sin_port = _htons(SERVER_PORT_FOR_UDP);
    	addr.sin_addr.s_addr = serverIP;

        /*Send an NTP time query to the NTP server*/
    	ret = sendto(udp_socket, (int8_t *)&cDataBuf, sizeof(cDataBuf), 0, (struct sockaddr *)&addr, sizeof(addr));
    	if (ret != M2M_SUCCESS) 
        {
        	printf("failed to send  error!\r\n");
        	return;
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
        gethostbyname((const char *)NTP_POOL_HOSTNAME);
        break;
    }

    default:
    {
        break;
    }
    }
}
#endif
