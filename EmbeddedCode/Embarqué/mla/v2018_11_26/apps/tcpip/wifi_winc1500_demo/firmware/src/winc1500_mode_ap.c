/*******************************************************************************
  File Name:
    winc1500_mode_ap.c

  Summary:
    WINC1500 AP mode demo.

  Description:
    This demo performs the following steps:
        1) This board run as AP
        2) Use laptop to connect this AP
        3) Use ping from laptop to this board
        4) Disconnect laptop from this AP

    The configuration defines for this demo are:    
        WLAN_SSID                 -- This AP's ssid is visible or hidden
        WLAN_AUTH                 -- Security mode, M2M_WIFI_SEC_WEP or M2M_WIFI_SEC_OPEN
        WLAN_WEP_KEY              -- Security Key in WEP Mode
        WLAN_CHANNEL              -- Channel number

    The demo uses these callback functions to handle events:
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

#if defined(USING_MODE_AP)

/** Security mode */
#define USE_WEP             (1) /*< Set to (1) to use WEP, and (0) to use OPEN */

/** AP mode Settings */
#define WLAN_SSID           "DEMO_AP_SERVER" /* < SSID */
#if USE_WEP
#define WLAN_AUTH           M2M_WIFI_SEC_WEP /* < Security manner */
#define WLAN_WEP_KEY        "1234567890" /* < Security Key in WEP Mode */
#define WLAN_WEP_KEY_INDEX  (0)
#else
#define WLAN_AUTH           M2M_WIFI_SEC_OPEN /* < Security manner */
#endif
#define WLAN_CHANNEL        (6) /* < Channel number */

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
tstrM2MAPConfig strAPConfig;

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
        printf("AP Demo\r\n");
        printf("AP's ssid is: %s\r\n", WLAN_SSID);
#if USE_WEP
        printf("Security mode is WEP\r\n");
#else
        printf("Security mode is Open\r\n");
#endif
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        
        /* Initialize AP mode parameters structure with SSID, channel and OPEN security type. */
        memset(&strAPConfig, 0x00, sizeof(tstrM2MAPConfig));
        strcpy((char *)&strAPConfig.au8SSID, WLAN_SSID);
        strAPConfig.u8ListenChannel = WLAN_CHANNEL;
        strAPConfig.u8SecType = WLAN_AUTH;

        strAPConfig.au8DHCPServerIP[0] = 192;
        strAPConfig.au8DHCPServerIP[1] = 168;
        strAPConfig.au8DHCPServerIP[2] = 1;
        strAPConfig.au8DHCPServerIP[3] = 1;

#if USE_WEP
        strcpy((char *)&strAPConfig.au8WepKey, WLAN_WEP_KEY);
        strAPConfig.u8KeySz = strlen(WLAN_WEP_KEY);
        strAPConfig.u8KeyIndx = WLAN_WEP_KEY_INDEX;
#endif

        /* Bring up AP mode with parameters structure. */
        m2m_wifi_enable_ap(&strAPConfig);

        printf("AP mode started. You can connect to ");
        printf((char *)WLAN_SSID);
        printf("\r\n");
        SetAppState(APP_STATE_DONE);
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
        }
        else
        {
            printf("Station disconnected\r\n");
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
        printf("Station connected!\r\n");
        printf("Station IP is: %s\r\n", buf);
        break;
    }

    default:
        break;
    }
}

#endif // USING_MODE_AP

