/*******************************************************************************
  File Name:
    winc1500_mode_change.c

  Summary:
    WINC1500 mode change demo.

  Description:
    This demo performs the following steps:
        1) run as AP mode
        2) wait 3 seconds
        3) stop AP mode
        4) run as p2p mode
        5) wait 3 seonds
        6) stop P2P mode
    
    The configuration defines for this demo are:    
        WLAN_SSID                 -- For AP mode, ssid
        WLAN_AUTH                 -- For AP mode, Security type
        WLAN_AP_CHANNEL           -- For AP mode, Channel number
        WLAN_DEVICE_NAME          -- For P2P mode, device name
        WLAN_P2P_CHANNEL          -- For P2P mode, Channel number
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

#if defined(USING_MODE_CHANGE)

/** AP mode Settings */
#define WLAN_SSID               "DEMO_AP" /* < SSID */
#define WLAN_AUTH               M2M_WIFI_SEC_OPEN /* < Security manner */
#define WLAN_AP_CHANNEL         M2M_WIFI_CH_6 /* < AP Channel number */

/** P2P mode Settings */
#define WLAN_DEVICE_NAME        "WINC1500_P2P" /* < P2P Device Name */
#define WLAN_P2P_CHANNEL        M2M_WIFI_CH_6 /* < P2P Channel number */

#define HOLD_TIME_IN_MODE               (30000)
#define DELAY_FOR_MODE_CHANGE           (2000)

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_MODE_AP_BEGIN,
    APP_STATE_MODE_AP_WAITING,
    APP_STATE_HOLDING_WAITING,
    APP_STATE_MODE_P2P_BEGIN,
    APP_STATE_MODE_P2P_WAITING,
    APP_STATE_MODE_P2P_STOP,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
tstrM2MAPConfig strAPConfig;
static char g_deviceName[] = {WLAN_DEVICE_NAME};

// application state machine called from main()
void ApplicationTask(void)
{
    static uint32_t startTime = 0;
    tstrM2MAPConfig strAPConfig;
    
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
        printf("Mode change Demo\r\n");
        printf("=========\r\n");

        SetAppState(APP_STATE_MODE_AP_BEGIN);
        break;
        
    case APP_STATE_MODE_AP_BEGIN:
        printf("AP mode, start\r\n");
        /* Configure AP. */
        memset(&strAPConfig, 0x00, sizeof(tstrM2MAPConfig));
        strcpy((char *)&strAPConfig.au8SSID, WLAN_SSID);
        strAPConfig.u8ListenChannel = WLAN_AP_CHANNEL;
        strAPConfig.u8SecType = WLAN_AUTH;
        strAPConfig.au8DHCPServerIP[0] = 0xC0; /* 192 */
        strAPConfig.au8DHCPServerIP[1] = 0xA8; /* 168 */
        strAPConfig.au8DHCPServerIP[2] = 0x01; /* 1 */
        strAPConfig.au8DHCPServerIP[3] = 0x01; /* 1 */
        /* Start AP mode. */
        m2m_wifi_enable_ap(&strAPConfig);

        startTime = m2mStub_GetOneMsTimer();
        SetAppState(APP_STATE_MODE_AP_WAITING);
        break;
        
    case APP_STATE_MODE_AP_WAITING:
        if (m2m_get_elapsed_time(startTime) > HOLD_TIME_IN_MODE)
        {
            m2m_wifi_disable_ap();
            printf("AP mode, end\r\n");
            startTime = m2mStub_GetOneMsTimer();
            SetAppState(APP_STATE_HOLDING_WAITING);
        }
        break;

    case APP_STATE_HOLDING_WAITING:
        if (m2m_get_elapsed_time(startTime) > DELAY_FOR_MODE_CHANGE)
        {
            SetAppState(APP_STATE_MODE_P2P_BEGIN);
        }
        break;
        
    case APP_STATE_MODE_P2P_BEGIN:
        printf("P2P mode, start\r\n");
        /* Set device name. */
        m2m_wifi_set_device_name(g_deviceName, strlen(WLAN_DEVICE_NAME));
        /* Start P2P with channel number. */
        m2m_wifi_p2p(WLAN_P2P_CHANNEL);
        startTime = m2mStub_GetOneMsTimer();
        SetAppState(APP_STATE_MODE_P2P_WAITING);
        break;
        
    case APP_STATE_MODE_P2P_WAITING:
        if (m2m_get_elapsed_time(startTime) > HOLD_TIME_IN_MODE)
        {
            SetAppState(APP_STATE_MODE_P2P_STOP);
        }
        break;

    case APP_STATE_MODE_P2P_STOP:
        /* Stop P2P mode. */
        m2m_wifi_p2p_disconnect();
        printf("P2P mode, end\r\n");
        printf("Testing is done\r\n");
        SetAppState(APP_STATE_DONE);
        break;
    
    case APP_STATE_DONE:
        break;
    }
}

#endif

