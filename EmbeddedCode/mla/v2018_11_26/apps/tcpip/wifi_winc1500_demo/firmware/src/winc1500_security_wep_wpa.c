/*******************************************************************************
  File Name:
    winc1500_security_wep_wpa.c

  Summary:
    WINC1500 security wep or wpa demo.

  Description:
    This demo performs the following steps:
        1) Starts Wi-Fi connection to the specified Access Point (AP)
        2) Waits for the connection event
        3) Waits for an IP address to be assigned to the WINC1500
        
    The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        TESTING_WEP           -- WEP or WPA
        for WEP:
            WLAN_WEP_KEY_INDEX    -- WEP key index
            WLAN_WEP_KEY_40       -- 64 bit WEP key. 10 hexadecimal (base 16) characters (0-9 and A-F)
            WLAN_WEP_KEY_104      -- 128 bit WEP key. 26 hexadecimal (base 16) characters (0-9 and A-F)
        for WPA:
            WLAN_PSK              -- Passphrase
    
    The demo uses this callback function to handle events:
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


#if defined(USING_SECURITY_WEP_WPA)

#define WLAN_SSID           "DEMO_AP"

//#define TESTING_WEP
#if defined(TESTING_WEP)
#define WLAN_WEP_KEY_INDEX   1 /**< WEP key index */
#define WLAN_WEP_KEY_40      "1234567890" /**< 64 bit WEP key. In case of WEP64, 10 hexadecimal (base 16) characters (0-9 and A-F) ) */
#define WLAN_WEP_KEY_104     "1234567890abcdef1234567890" /**< 128 bit WEP key. In case of WEP128, 26 hexadecimal (base 16) characters (0-9 and A-F) ) */

// for 64 bit WEP Encryption
tstrM2mWifiWepParams wep64_parameters = { WLAN_WEP_KEY_INDEX, sizeof(WLAN_WEP_KEY_40), WLAN_WEP_KEY_40};
// for 128 bit WEP Encryption
tstrM2mWifiWepParams wep128_parameters = { WLAN_WEP_KEY_INDEX, sizeof(WLAN_WEP_KEY_104), WLAN_WEP_KEY_104};

#else //defined(TESTING_WEP)

#define WLAN_PSK                   "12345678" /**< Password for Destination SSID */

#endif //defined(TESTING_WEP)

#define SetAppState(state)  g_appState = state
#define GetAppState()       g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_FOR_CONNECT_AND_DHCP,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static bool wifiConnected = false;
static void wifi_cb(uint8_t msgType, void *pvMsg); 

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
        printf("Security mode Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);

#if defined(TESTING_WEP)
        /* Case 1. Connect to AP with security type WEP. */
        printf("WEP test starting...\r\n");
        m2m_wifi_connect((char *)WLAN_SSID, 
                         strlen(WLAN_SSID),
                         M2M_WIFI_SEC_WEP, 
                         &wep64_parameters, 
                         M2M_WIFI_CH_ALL); 
#else
        /* Case 2. Connect to AP with security type WPA. */
        printf("WPA test starting...\r\n");
        m2m_wifi_connect((char *)WLAN_SSID, 
                         strlen(WLAN_SSID),
                         M2M_WIFI_SEC_WPA_PSK, 
                         (char *)WLAN_PSK, 
                         M2M_WIFI_CH_ALL); 
#endif
        SetAppState(APP_STATE_WAIT_FOR_CONNECT_AND_DHCP);
        break;
        
    case APP_STATE_WAIT_FOR_CONNECT_AND_DHCP:
        if (wifiConnected)
        {
            printf("Testing is done\r\n");
            SetAppState(APP_STATE_DONE);
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
            printf("Connected -- starting DHCP client\r\n");
        } 
        else if (pWifiConnState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            wifiConnected = false;
            printf("disconnected -- starting connect again\r\n");
#if defined(TESTING_WEP)
            /* Case 1. Connect to AP with security type WEP. */
            m2m_wifi_connect((char *)WLAN_SSID, strlen(WLAN_SSID), M2M_WIFI_SEC_WEP, &wep64_parameters, M2M_WIFI_CH_ALL); 
#else
            /* Case 2. Connect to AP with security type WPA. */
            m2m_wifi_connect((char *)WLAN_SSID, strlen(WLAN_SSID), M2M_WIFI_SEC_WPA_PSK, (char *)WLAN_PSK, M2M_WIFI_CH_ALL); 
#endif        
        }
        break;
    }

    case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        char buf[M2M_INET4_ADDRSTRLEN];  
        // read it from event data,
        tstrM2MIPConfig *p_ipConfig = (tstrM2MIPConfig *)pvMsg;
        // convert binary IP address to string
        inet_ntop4(p_ipConfig->u32StaticIp, buf);
        printf("IP address assigned: %s\r\n", buf);
        wifiConnected = true;
        break;
    }

    default:
    {
        break;
    }
    }
}

#endif
