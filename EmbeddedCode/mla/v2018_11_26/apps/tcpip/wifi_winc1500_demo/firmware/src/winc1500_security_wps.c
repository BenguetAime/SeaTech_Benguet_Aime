/*******************************************************************************
  File Name:
    winc1500_security_wps.c

  Summary:
    WINC1500 security wps demo.

  Description:
    This demo performs the following steps:
        1) Power on the board
        2) in console port, type in 'Y' or 'y' to begin WPS testing
        3) get wps information from AP
        4) connect to AP
        
    The configuration defines for this demo are: 
        WPS_PUSH_BUTTON_FEATURE    -- "WPS Push Button" or "WPS PIN"
        WPS_PIN_NUMBER             -- WPS PIN number if using "WPS PIN"
        devName                    -- devic name of AP
        
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

#if defined(USING_SECURITY_WPS)

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

/** WPS Push Button Feature */
#define WPS_PUSH_BUTTON_FEATURE     true

/** WPS PIN number */
#define WPS_PIN_NUMBER          "12345670"



char devName[] = "DEMO_AP";

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_KEYBOARD,
    APP_STATE_WAIT_FOR_CONNECT_AND_DHCP,
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static bool wifiConnected = false;
static void wifi_cb(uint8_t msgType, void *pvMsg);

// application state machine called from main()
void ApplicationTask(void)
{
    uint8_t chr_read = 0;

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
        printf("Security WPS Demo\r\n");
        if (WPS_PUSH_BUTTON_FEATURE) 
        {
            printf("Testing WPS_PUSH_BUTTON!\r\n");
        } 
        else 
        {
            printf("Testing WPS_PIN! pin number = %s\r\n", (const char *)WPS_PIN_NUMBER);
        }
        printf("=========\r\n");
        printf("please type in Y or y to begin WPS testing!\r\n");

        registerWifiCallback(wifi_cb);
        /* Device name must be set before enabling WPS mode. */
        m2m_wifi_set_device_name(devName, strlen((char *)devName));
        SetAppState(APP_STATE_WAIT_KEYBOARD);
        break;  
        
    case APP_STATE_WAIT_KEYBOARD:
        chr_read = m2m_wifi_console_read_data();
        if ((chr_read == 'y') || (chr_read == 'Y'))
        {
            printf("Starting WPS...\r\n");
            if (WPS_PUSH_BUTTON_FEATURE) 
            {
                m2m_wifi_wps(WPS_PBC_TRIGGER, NULL);
            } 
            else 
            {
                m2m_wifi_wps(WPS_PIN_TRIGGER, (const char *)WPS_PIN_NUMBER);
            }
            SetAppState(APP_STATE_WAIT_FOR_CONNECT_AND_DHCP);
        }
        else
        {
            printf("please type in Y or y to begin WPS testing!\r\n");
        }
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
            printf("disconnected\r\n");
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

	case M2M_WIFI_WPS_EVENT:
    {
    	tstrM2MWPSInfo *p_wfWpsInfo = (tstrM2MWPSInfo *)pvMsg;
    	printf("Wi-Fi request WPS\r\n");
    	printf("SSID : %s, authtyp : %d pw : %s\n", p_wfWpsInfo->au8SSID, p_wfWpsInfo->u8AuthType, p_wfWpsInfo->au8PSK);
    	if (p_wfWpsInfo->u8AuthType == 0) 
        {
        	printf("WPS is not enabled OR Timedout\r\n");
        	m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
            /* WPS is not enabled by firmware OR WPS monitor timeout.*/
        } 
        else 
        {
        	printf("Request Wi-Fi connect\r\n");
        	m2m_wifi_connect((char *)p_wfWpsInfo->au8SSID, strlen((char *)p_wfWpsInfo->au8SSID), p_wfWpsInfo->u8AuthType, p_wfWpsInfo->au8PSK, p_wfWpsInfo->u8Ch);
        }

    	break;
    }
    default:
    {
        break;
    }
    }
}

#endif

