/*******************************************************************************
  File Name:
    winc1500_simple_growl.c

  Summary:
    WINC1500 simple growl demo.

  Description:
    This demo performs the following steps:
        1) This board works as AP mode,
        2) PC connect to this board, and access webpage of this board: http://192.168.0.1,
        3) In webpage, fill in Another AP's ssid and security password,
        4) re-direct this board to another AP, 
        5) send message to android phone
 
    The configuration defines for this demo are:    
        WLAN_AP_SEC                -- Security mode
        WLAN_AP_WEP_KEY            -- Security key        "1234567890"
        WLAN_AP_SSID_MODE          -- This AP's ssid is visible or hidden
        HTTP_PROV_DOMAIN_NAME      -- AP's Domain name

    The demo uses these callback functions to handle events:
        socket_cb() 
        resolve_cb()
        wifi_cb()
 
    More information about how to use this demo:
        This application supports sending GROWL notifications to the following servers.
        -# PROWL for iOS push notifications (https://www.prowlapp.com/).
        -# NMA for Android push notifications (http://www.notifymyandroid.com/).
        
        In order to enable the GROWL application (for sending notifications), apply the following instructions.
        -# Create a NMA account at http://www.notifymyandroid.com/ and create an API key. Copy the obtained key string in the file
        in the MACRO NMA_API_KEY as the following.
        -# Create a PROWL account at https://www.prowlapp.com/ and create an API key. Copy the obtained API key string in the file
        in the MACRO PROWL_API_KEY as the following.
        #define NMA_API_KEY      "f8bd3e7c9c5c10183751ab010e57d8f73494b32da73292f6"
        #define PROWL_API_KEY    "117911f8a4f2935b2d84abc934be9ff77d883678"
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

#if defined(USING_SIMPLE_GROWL)
#include "growl.h"

#define WF_AP_SEC               M2M_WIFI_SEC_OPEN
#define WF_AP_WEP_KEY           "1234567890"
#define WF_AP_SSID_MODE         SSID_MODE_VISIBLE

#define HTTP_PROV_DOMAIN_NAME   "atmelconfig.com"

#define WF_DEVICE_NAME          "WINC1500_00:00"
#define MAC_ADDRESS             {0xf8, 0xf0, 0x05, 0x45, 0xD4, 0x84}
#define HEX2ASCII(x)            (((x) >= 10) ? (((x) - 10) + 'A') : ((x) + '0'))

/** Growl Options */
#define PROWL_API_KEY           "6ce3b9ff6c29e5c5b8960b28d9e987aec5ed603a"
#define NMA_API_KEY             "775ff1ac4ad88e536be8cf9c7aecc2b7768b6c819269a583"
#define SSL_CONNECTION          1
#define NORMAL_CONNECTION       0
#define PROWL_CONNECTION_TYPE   NORMAL_CONNECTION
#define NMA_CONNECTION_TYPE     SSL_CONNECTION

#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WORKING,  
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

static tstrM2MAPConfig apConfig = {
    WF_DEVICE_NAME, 1, 0, M2M_WIFI_WEP_40_KEY_STRING_SIZE, WF_AP_WEP_KEY, (uint8_t)WF_AP_SEC, WF_AP_SSID_MODE
};

static const char s_HttpProvDomainName[] = HTTP_PROV_DOMAIN_NAME;
static uint8_t s_MacAddr[] = MAC_ADDRESS;
static int8_t s_DeviceName[] = WF_DEVICE_NAME;
static volatile bool s_RespProvInfo = false;

static void set_dev_name_to_mac(uint8_t *name, uint8_t *mac_addr);
static int growl_send_message_handler(void);
static void wifi_cb(uint8_t msgType, void *pvMsg);
void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);
void resolve_cb(char *pu8DomainName, uint32_t serverIP);

void ApplicationTask(void)
{
    uint8_t mac_addr[6];
    uint8_t isMacAddrValid;
    
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
        printf("Simple Growl Demo\r\n");
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);

        m2m_wifi_get_otp_mac_address(mac_addr, &isMacAddrValid);
        if (!isMacAddrValid) 
        {
            m2m_wifi_set_mac_address(s_MacAddr);
        }

        m2m_wifi_get_mac_address(s_MacAddr);

        set_dev_name_to_mac((uint8_t *)s_DeviceName, s_MacAddr);
        set_dev_name_to_mac((uint8_t *)apConfig.au8SSID, s_MacAddr);
        m2m_wifi_set_device_name((char *)s_DeviceName, strlen((char *)s_DeviceName));
        apConfig.au8DHCPServerIP[0] = 0xC0; /* 192 */
        apConfig.au8DHCPServerIP[1] = 0xA8; /* 168 */
        apConfig.au8DHCPServerIP[2] = 0x01; /* 1 */
        apConfig.au8DHCPServerIP[3] = 0x01; /* 1 */

        m2m_wifi_start_provision_mode((tstrM2MAPConfig *)&apConfig, (char *)s_HttpProvDomainName, 1);
        printf("Provision Mode started.\r\nConnect to [%s] via AP[%s] and fill up the page.\r\n", HTTP_PROV_DOMAIN_NAME, apConfig.au8SSID);

        SetAppState(APP_STATE_WORKING);
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
 * \brief Growl notification callback.
 *    Pointer to a function delivering growl events.
 *
 *  \param    [u8Code] Possible error codes could be returned by the nma server and refer to the comments in the growl.h.
 *  - [20] GROWL_SUCCESS (@ref GROWL_SUCCESS)
 *  - [40] GROWL_ERR_BAD_REQUEST (@ref GROWL_ERR_BAD_REQUEST)
 *  - [41] GROWL_ERR_NOT_AUTHORIZED (@ref GROWL_ERR_NOT_AUTHORIZED)
 *  - [42] GROWL_ERR_NOT_ACCEPTED (@ref GROWL_ERR_NOT_ACCEPTED)
 *  - [46] GROWL_ERR_API_EXCEED (@ref GROWL_ERR_API_EXCEED)
 *  - [49] GROWL_ERR_NOT_APPROVED (@ref GROWL_ERR_NOT_APPROVED)
 *  - [50] GROWL_ERR_SERVER_ERROR (@ref GROWL_ERR_SERVER_ERROR)
 *  - [30] GROWL_ERR_LOCAL_ERROR (@ref GROWL_ERR_LOCAL_ERROR)
 *  - [10] GROWL_ERR_CONN_FAILED (@ref GROWL_ERR_CONN_FAILED)
 *  - [11] GROWL_ERR_RESOLVE_DNS (@GROWL_ERR_RESOLVE_DNS GROWL_RETRY)
 *  - [12] GROWL_RETRY (@ref GROWL_RETRY)
 *    \param    [clientID] client id returned by the nma server.
 */
void GrowlCb(uint8_t u8Code, uint8_t clientID)
{
    printf("Growl CB : %d \r\n", u8Code);
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
    switch (msgType) {
    case M2M_WIFI_CONN_STATE_CHANGED_EVENT:
    {
        tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
        if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) 
        {
            printf("Wi-Fi connected\r\n");
        } 
        else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            printf("Wi-Fi disconnected\r\n");
        }

        break;
    }

    case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
        printf("Wi-Fi IP is %u.%u.%u.%u\r\n",
                pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);

        if (s_RespProvInfo) 
        {
            /** init growl */
            NMI_GrowlInit((uint8_t *)PROWL_API_KEY, (uint8_t *)NMA_API_KEY);
            growl_send_message_handler();
        }

        break;
    }

    case M2M_WIFI_PROVISION_INFO_EVENT:
    {
        tstrM2MProvisionInfo *pstrProvInfo = (tstrM2MProvisionInfo *)pvMsg;
        printf("M2M_WIFI_PROVISION_INFO_EVENT.\r\n");

        if (pstrProvInfo->u8Status == M2M_SUCCESS) 
        {
            printf("connect to AP: %s\r\n", (char *)pstrProvInfo->au8SSID);
            m2m_wifi_connect((char *)pstrProvInfo->au8SSID, strlen((char *)pstrProvInfo->au8SSID), pstrProvInfo->u8SecType,
                    pstrProvInfo->au8Password, M2M_WIFI_CH_ALL);
            s_RespProvInfo = true;
        } 
        else 
        {
            printf("Provision failed.\r\n");
        }

        break;
    }

    default:
    {
        break;
    }
    }
}

static void set_dev_name_to_mac(uint8_t *name, uint8_t *mac_addr)
{
    /* Name must be in the format WINC1500_00:00 */
    uint16_t len;

    len = strlen((const char *)name);
    if (len >= 5) 
    {
        name[len - 1] = HEX2ASCII((mac_addr[5] >> 0) & 0x0f);
        name[len - 2] = HEX2ASCII((mac_addr[5] >> 4) & 0x0f);
        name[len - 4] = HEX2ASCII((mac_addr[4] >> 0) & 0x0f);
        name[len - 5] = HEX2ASCII((mac_addr[4] >> 4) & 0x0f);
    }
}

/**
 * \brief Send a specific notification to a registered Android(NMA) or IOS(PROWL)
 */
static int growl_send_message_handler(void)
{
    printf("send Growl message \r\n");
    /* NMI_GrowlSendNotification(PROWL_CLIENT, (uint8_t*)"Growl_Sample", (uint8_t*)"Growl_Event", (uint8_t*)msg_for_growl,PROWL_CONNECTION_TYPE); // send by PROWL */
    NMI_GrowlSendNotification(NMA_CLIENT, (uint8_t *)"Growl_Sample", (uint8_t *)"Growl_Event", (uint8_t *)"growl_test", NMA_CONNECTION_TYPE);           /* send by NMA */

    return 0;
}

#endif
