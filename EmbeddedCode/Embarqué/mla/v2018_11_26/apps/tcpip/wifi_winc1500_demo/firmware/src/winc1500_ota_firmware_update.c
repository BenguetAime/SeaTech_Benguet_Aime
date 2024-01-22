/*******************************************************************************
  File Name:
    winc1500_ota_firmware_update.c

  Summary:
    WINC1500 ota firmware update demo.

  Description:
    This demo performs the following steps:
        1) Connect to AP
        2) Begin to download Firmware
        3) Print information of result

    The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        OTA_URL               -- Url of firmware
        
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
#include "wf_common.h"

#if defined(USING_OTA_UPDATE)

/** Wi-Fi Settings */
#define WLAN_SSID        "DEMO_AP" /* < Destination SSID */
#define WLAN_AUTH        M2M_WIFI_SEC_WPA_PSK /* < Security manner */
#define WLAN_PSK         "12345678" /* < Password for Destination SSID */
#define OTA_URL          "http://192.168.1.11/m2m_ota_3a0.bin" //"http://192.168.0.4/winc1500_ota.bin"

#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WAIT_CONNECT_AND_DHCP,
    APP_STATE_WORKING,  
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static bool wifiConnected = false;
static void wifi_cb(uint8_t msgType, void *pvMsg);

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
        printf("OTA firmware upgrade example\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("ota url: %s\r\n", OTA_URL);
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        
        /* Connect to defined AP. */
        m2m_wifi_connect((char *)WLAN_SSID, strlen(WLAN_SSID), WLAN_AUTH, (void *)WLAN_PSK, M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WAIT_CONNECT_AND_DHCP);
        break;

    case APP_STATE_WAIT_CONNECT_AND_DHCP:
        if (wifiConnected)
        {
            /* Start OTA Firmware download. */
            m2m_ota_start_update((char *)OTA_URL);
            SetAppState(APP_STATE_WORKING);
        }
        break;
        
    case APP_STATE_WORKING:
        break;
        
    case APP_STATE_DONE:
        break;
        
    default:
        break;
    }
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] msgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_CONN_STATE_CHANGED_EVENT](@ref M2M_WIFI_CONN_STATE_CHANGED_EVENT)
 *  - [M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT](@ref M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type.
 */
static void wifi_cb(uint8_t msgType, void *pvMsg)
{
    switch (msgType) 
    {
    case M2M_WIFI_CONN_STATE_CHANGED_EVENT:
    {
        tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
        if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) 
        {
        } 
        else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            printf("Wi-Fi disconnected\r\n");

            /* Connect to defined AP. */
            m2m_wifi_connect((char *)WLAN_SSID, strlen(WLAN_SSID), WLAN_AUTH, (void *)WLAN_PSK, M2M_WIFI_CH_ALL);
        }

        break;
    }

    case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
        printf("Wi-Fi connected\r\n");
        printf("Wi-Fi IP is %u.%u.%u.%u\r\n",
                pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
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
