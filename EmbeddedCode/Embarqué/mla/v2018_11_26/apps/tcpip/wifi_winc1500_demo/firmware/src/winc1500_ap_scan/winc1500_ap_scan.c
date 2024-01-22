/*******************************************************************************
   File Name:
    winc1500_ap_scan.c

  Summary:
    WINC1500 scan and connection demo.

  Description:
    This demo performs the following steps:
        1) scans all channels looking for the specified AP
        2) Connect to the specified AP
        3) Requests RSSI level
        4) Send one or more pings to a known IP address on the AP network
 
    The configuration defines for this demo are:    
        WLAN_SSID       -- AP to search for
        WLAN_AUTH       -- Security type for that AP
        WLAN_PSK        -- If using security, the passphrase
        PING_ADDRESS    -- IP address to ping after successful connection
        PING_COUNT      -- Number of times to ping
        PING_INTERVAL   -- Time between pings, in milliseconds
 
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

#if defined(USING_AP_SCAN)

//==============================================================================
// DEMO CONFIGURATION
//==============================================================================
#define WLAN_SSID       "Test_AP"                // target AP
#define WLAN_AUTH       M2M_WIFI_SEC_OPEN       // AP Security (see tenuM2mSecType)
#define WLAN_PSK        "myPassword"            // security passphrase (if security used)
#define PING_ADDRESS    "192.168.1.1"           // address to ping after connection
#define PING_COUNT      3                       // number of times to ping
#define PING_INTERVAL   100                     // wait 100 ms between pings

// only applicable for WEP security
#define WEP_KEY_INDEX     M2M_WIFI_WEP_KEY_INDEX_1
#define WEP_KEY_SIZE  WEP_104_KEY_STRING_SIZE
#define WEP_KEY_CODE  "90e96780c739409da50034fcaa"

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_FOR_SCAN_COMPLETE,
    APP_STATE_WAIT_FOR_SCAN_RESULT,
    APP_STATE_WAIT_FOR_CONNECT,
    APP_STATE_WAIT_DHCP_CLIENT,
    APP_STATE_WAIT_FOR_RSSI,
    APP_STATE_WAIT_FOR_PING_RESPONSE,
    APP_PING_DELAY,            
    APP_STATE_DONE
} t_AppState;

//==============================================================================
// Local Globals
//==============================================================================
t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static char ssid[] = {WLAN_SSID};

tstrM2mWifiWepParams g_wepSecurity;

// application state machine called from main()
void ApplicationTask(void)
{
    static uint8_t  u8NumofCh = 0;
    static uint8_t  scanIndex;
    static uint32_t pingCount;
    static uint32_t destIpAddress;
    static uint32_t timer;
    tstrM2mWifiscanResult  *p_scanResult;
    t_pingReply *   p_pingInfo;
    uint8_t         mac[6];
    
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
            printf("Scan Demo\r\n");
            printf("=========\r\n");
            ClearWiFiEventStates();     // clear all event flags
            ClearSocketEventStates();
            m2m_wifi_get_mac_address(mac);
            printf("WINC1500 MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
            scanIndex = 0;
            pingCount = PING_COUNT;
            inet_pton4(PING_ADDRESS, &destIpAddress);
            printf("Starting scan\r\n");
            m2m_wifi_request_scan(M2M_WIFI_CH_ALL);  // request a scan of all channels
            SetAppState(APP_STATE_WAIT_FOR_SCAN_COMPLETE);
            break;
            
        case APP_STATE_WAIT_FOR_SCAN_COMPLETE:
            if (isScanComplete())
            {
                // get number of scan results from the event data and save
                u8NumofCh = m2m_wifi_get_wifi_event_data()->scanDone.u8NumofCh;
                printf("  Found %d AP's\r\n", u8NumofCh);
                if (u8NumofCh > 0)
                {
                    printf("    Requesting scan results\r\n");
                    m2m_wifi_req_scan_result(0);   // get first scan result
                    SetAppState(APP_STATE_WAIT_FOR_SCAN_RESULT);
                }
                else
                {
                    m2m_wifi_request_scan(M2M_WIFI_CH_ALL);  // try again
                }
            }
            break;
            
        case APP_STATE_WAIT_FOR_SCAN_RESULT:
            if (isScanResultReady())
            {
                // point to scan result in event data union
                p_scanResult = &(m2m_wifi_get_wifi_event_data()->scanResult);
                printf("      [%d] SSID: %s\r\n", scanIndex, p_scanResult->au8SSID);
                
                // if we found the AP we were looking for
                if (strcmp(p_scanResult->au8SSID, WLAN_SSID) == 0)
                {
                    // connect to that AP
                    printf("Found %s and attempting to connect\r\n", p_scanResult->au8SSID);
                    uint8_t security = WLAN_AUTH;
                    if (security == M2M_WIFI_SEC_WEP)
                    {
                        g_wepSecurity.u8KeyIndx = WEP_KEY_INDEX;
                        g_wepSecurity.u8KeySz  = sizeof(WEP_KEY_CODE);
                        memcpy(g_wepSecurity.au8WepKey, WEP_KEY_CODE, sizeof(WEP_KEY_CODE));
                        m2m_wifi_connect(ssid,
                                         strlen(ssid),                                
                                         WLAN_AUTH,
                                         (void *)&g_wepSecurity,
                                         M2M_WIFI_CH_ALL);                        
                    }
                    else
                    {
                        m2m_wifi_connect(ssid,
                                         strlen(ssid),                                  
                                         WLAN_AUTH,
                                         (void *)WLAN_PSK,
                                         M2M_WIFI_CH_ALL); 
                    }
                    SetAppState(APP_STATE_WAIT_FOR_CONNECT);
                }
                // else get the next scan result
                else
                {
                    // if have not yet retrieved all scan results
                    ++scanIndex;
                    if (scanIndex < (u8NumofCh))
                    {
                        // get next scan result
                        m2m_wifi_req_scan_result(scanIndex);
                    }
                    // else we did not find desired AP, so scan again
                    else
                    {
                        printf("%s not found, retrying...\r\n", WLAN_SSID);
                        SetAppState(APP_STATE_START);
                    }
                }
            }
            break;
            
        case APP_STATE_WAIT_FOR_CONNECT:
            if (isConnectionStateChanged())
            {
                // if connection successful
                if (m2m_wifi_get_wifi_event_data()->connState.u8CurrState == M2M_WIFI_CONNECTED)
                {
                    // start DHCP client to get an IP address
                    printf("  Connected -- waiting for DHCP client\r\n");
                    SetAppState(APP_STATE_WAIT_DHCP_CLIENT);
                }
                // else failed to connect to AP
                else
                {
                    printf("  Unable to connect (check security settings)\r\n");
                    SetAppState(APP_STATE_DONE);
                }
            }
            break;
            
        case APP_STATE_WAIT_DHCP_CLIENT:
            // if IP address assigned
            if (isIpAddressAssigned())
            {
                // read IP address from event data
                char buf[M2M_INET4_ADDRSTRLEN];  
                tstrM2MIPConfig *p_ipConfig = &m2m_wifi_get_wifi_event_data()->ipConfig;  // pointer to result structure

                // convert binary IP address to string
                inet_ntop4(p_ipConfig->u32StaticIp, buf);
                printf("\nIP address assigned: %s\r\n", buf);
                
                printf("\r\nRequesting RSSI\r\n");
                m2m_wifi_req_curr_rssi();
                SetAppState(APP_STATE_WAIT_FOR_RSSI);
            }
            break;
            
        case APP_STATE_WAIT_FOR_RSSI:
            if (isRssiReady())
            {
                uint8_t rssi = m2m_wifi_get_wifi_event_data()->rssi; 
                printf("  RSSI: %d\r\n", rssi);
                
                // Send first ping
                printf("\r\nPinging %s\r\n", PING_ADDRESS);
                m2m_ping_req(destIpAddress, 0);
                --pingCount;
                SetAppState(APP_STATE_WAIT_FOR_PING_RESPONSE);
            }
            break;
            
        case APP_STATE_WAIT_FOR_PING_RESPONSE:
            if (isPingReplyOccurred())
            {
                // get ping reply data
                p_pingInfo = &m2m_wifi_get_socket_event_data()->pingReply;
                switch (p_pingInfo->errorCode)
                {
                    case M2M_PING_SUCCESS:
                        printf(" Ping successful; RTT=%ld\r\n", p_pingInfo->rtt);
                        break;
                        
                    case M2M_PING_DEST_UNREACHABLE:
                        printf(" Ping failed; Destination unreachable\r\n");
                        break;
                        
                    case M2M_PING_TIMEOUT:
                        printf(" Ping failed; Timeout\r\n");                        
                        break;
                }
                
                // if not done pinging
                if (pingCount > 0)
                {
                    --pingCount;
                    timer = m2mStub_GetOneMsTimer();
                    SetAppState(APP_PING_DELAY);
                }
                // else sent all pings
                else
                {
                    printf("\n* Demo Complete *\r\n");
                    SetAppState(APP_STATE_DONE);
                }
            }
            break;
            
         case APP_PING_DELAY:
             // if ping interval expired
             if (m2m_get_elapsed_time(timer) >= PING_INTERVAL)
             {
                //send next ping 
                printf("Pinging %s\r\n", PING_ADDRESS);
                m2m_ping_req(destIpAddress, 0);
                SetAppState(APP_STATE_WAIT_FOR_PING_RESPONSE);
             }
             break;
             
        case APP_STATE_DONE:
            break;
    }
}

#endif // USING_AP_SCAN

//DOM-IGNORE-END