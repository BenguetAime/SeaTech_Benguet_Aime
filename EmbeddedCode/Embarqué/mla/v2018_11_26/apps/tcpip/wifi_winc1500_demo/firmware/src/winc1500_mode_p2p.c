/*******************************************************************************
  File Name:
    winc1500_mode_p2p.c

  Summary:
    WINC1500 mode p2p demo.

  Description:
    This demo performs the following steps:
        1) This board run as p2p
        2) On the computer, open and configure a terminal application
    
    The configuration defines for this demo are: 
        WLAN_DEVICE_NAME     -- "WINC1500_P2P"
        WLAN_CHANNEL         -- Channel number
        
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
#include "wf_socket.h"

#if defined(USING_MODE_P2P)

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

#define WLAN_DEVICE_NAME    "WINC1500_P2P"
#define WLAN_CHANNEL        (M2M_WIFI_CH_6)

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WORK
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static char g_deviceName[] = {WLAN_DEVICE_NAME};

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
        printf("Mode p2p Demo\r\n");
        printf("Device name is: %s\r\n", WLAN_DEVICE_NAME);
        printf("Channel is: %d\r\n", WLAN_CHANNEL);
        printf("=========\r\n");
        printf("Starting ...\r\n");
        registerWifiCallback(wifi_cb);
        
        /* Set device name to be shown in peer device. */
        m2m_wifi_set_device_name(g_deviceName, strlen(WLAN_DEVICE_NAME));

        /* Bring up P2P mode with channel number. */
        m2m_wifi_p2p(WLAN_CHANNEL);
        
        printf("P2P mode started. You can connect to %s.\r\n", (char *)WLAN_DEVICE_NAME);
        SetAppState(APP_STATE_WORK);
        break;
    
    case APP_STATE_WORK:
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
            printf("WiFi disconnected\r\n");
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
        printf("WiFI client connected!\r\n");
        printf("Client's IP is: %s\r\n", buf);
        break;
    }

    default:
        break;
    }
}

#endif

