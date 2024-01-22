/*******************************************************************************
  File Name:
    winc1500_power_save_mode.c

  Summary:
    WINC1500 power save mode demo.

  Description:
    This demo performs the following steps:
        1) Starts Wi-Fi connection to the specified Access Point (AP)
        2) Waits for the connection event
        3) Waits for an IP address to be assigned to the WINC1500
        4) Winc1500 enter 1 second sleep mode
        5) every 1 second, printf RSSI of the AP
        
    The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
    
    The demo uses this callback functions to handle events:
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

#if defined(USING_POWER_SAVE_MODE)

/** PowerSave status */
#define PS_WAKE             0
#define PS_SLEEP            1
#define PS_REQ_SLEEP        3
#define LISTEN_INTERVAL     1

/** PowerSave mode Settings */
#define PS_SLEEP_MODE       M2M_WIFI_PS_MANUAL          // M2M_WIFI_PS_MANUAL, M2M_WIFI_PS_DEEP_AUTOMATIC, M2M_WIFI_PS_MANUAL

/** Request sleep time for PowerSave manual mode */
#define REQUEST_SLEEP_TIME  1000

// State macros
#define SetAppState(state)  g_appState = state
#define GetAppState()       g_appState

#define WLAN_SSID           "DEMO_AP"             // target AP
#define WLAN_AUTH           M2M_WIFI_SEC_WPA_PSK   // AP Security 
#define WLAN_PSK            "12345678"            // security password

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_FOR_CONNECT_AND_DHCP,
    APP_STATE_POWER_SAVE_TEST,
    APP_STATE_DONE
} t_AppState;

static t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static uint8_t sleepStatus;
static uint32_t startTime = 0;
static bool wifiConnected = false;
static char wlanPsk[] = WLAN_PSK;
static char ssid[] = {WLAN_SSID};

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
        printf("Power Save Mode Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("=========\r\n");
        printf("Starting ...\r\n");
        registerWifiCallback(wifi_cb);
        
        /* Set defined sleep mode */
        if (PS_SLEEP_MODE == M2M_WIFI_PS_MANUAL) 
        {
            printf("M2M_WIFI_PS_MANUAL\r\n");
            m2m_wifi_set_sleep_mode(PS_SLEEP_MODE, 1);
        } 
        else if (PS_SLEEP_MODE == M2M_WIFI_PS_DEEP_AUTOMATIC) 
        {
            printf("M2M_WIFI_PS_DEEP_AUTOMATIC\r\n");
            m2m_wifi_set_sleep_mode(M2M_WIFI_PS_DEEP_AUTOMATIC, 1);
            tstrM2mLsnInt listenInterval;
            listenInterval.u16LsnInt = LISTEN_INTERVAL;
            m2m_wifi_set_lsn_int(&listenInterval);
        } 
        else 
        {
            printf("WF_PS_NO\r\n");
        }

        m2m_wifi_connect(ssid, 
                         strlen(ssid),
                         WLAN_AUTH,
                         (void *)wlanPsk,
                         M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WAIT_FOR_CONNECT_AND_DHCP);
        break;
        
    case APP_STATE_WAIT_FOR_CONNECT_AND_DHCP:
        if (wifiConnected)
        {
            startTime = m2mStub_GetOneMsTimer();
            SetAppState(APP_STATE_POWER_SAVE_TEST);
        }
        break;
        
    case APP_STATE_POWER_SAVE_TEST:
        if (sleepStatus == PS_REQ_SLEEP) 
        {
            if (PS_SLEEP_MODE == M2M_WIFI_PS_MANUAL) 
            {
                m2m_wifi_request_sleep(REQUEST_SLEEP_TIME);
                sleepStatus = PS_SLEEP;
            }
        }
        if (m2m_get_elapsed_time(startTime) > 1000)
        {
            startTime = m2mStub_GetOneMsTimer();
            m2m_wifi_req_curr_rssi();
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
        sleepStatus = PS_WAKE;
        tstrM2mWifiStateChanged *pWifiConnState = (tstrM2mWifiStateChanged *)pvMsg;
        if (pWifiConnState->u8CurrState == M2M_WIFI_CONNECTED) 
        {
            printf("Connected -- starting DHCP client\r\n");
        } 
        else if (pWifiConnState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            wifiConnected = false;
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
        char buf[M2M_INET4_ADDRSTRLEN];  
        // read it from event data,
        tstrM2MIPConfig *p_ipConfig = (tstrM2MIPConfig *)pvMsg;
        // convert binary IP address to string
        inet_ntop4(p_ipConfig->u32StaticIp, buf);
        printf("IP address assigned: %s\r\n", buf);
        sleepStatus = PS_REQ_SLEEP;
        wifiConnected = true;
        break;
    }
    
	case M2M_WIFI_RSSI_EVENT:
    {
        /* This message type is triggered by "m2m_wifi_req_curr_rssi()" function. */
    	int8_t *p_rssi = (int8_t *)pvMsg;
    	printf("RSSI for the current connected AP (%d)\r\n", (int8_t)(*p_rssi));
        sleepStatus = PS_REQ_SLEEP;
    	break;
    }
    
    default:
    {
        break;
    }
    }
}

#endif
