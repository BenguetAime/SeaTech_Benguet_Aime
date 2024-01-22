/*******************************************************************************
  File Name:
    winc1500_provision_http.c

  Summary:
    WINC1500 provision http demo.

  Description:
    This demo performs the following steps:
        1) This board works as AP mode,
        2) PC connect to this board, and access webpage of this board: http://192.168.0.1,
        3) In webpage, fill in Another AP's ssid and security password,
        3) re-direct this board to another AP.
 
    The configuration defines for this demo are:    
        WLAN_AP_SEC                          -- Security mode
        WLAN_AP_WEP_KEY                      -- Security key        "1234567890"
        WLAN_AP_SSID_MODE                    -- This AP's ssid is visible or hidden
        HTTP_PROV_SERVER_DOMAIN_NAME         -- AP's Domain name
        WLAN_DEVICE_NAME                     -- AP's name

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

#if defined(USING_PROVISION_HTTP)

//==============================================================================
// DEMO CONFIGURATION
//==============================================================================

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

#define WLAN_AP_SEC                  M2M_WIFI_SEC_OPEN
#define WLAN_AP_WEP_KEY              "1234567890"
#define WLAN_AP_SSID_MODE            SSID_MODE_VISIBLE

#define HTTP_PROV_SERVER_DOMAIN_NAME    "atmelconfig.com"

#define WLAN_DEVICE_NAME                 "WINC1500_00:00"
#define MAC_ADDRESS                     {0xf8, 0xf0, 0x05, 0x45, 0xD4, 0x84}

#define HEX2ASCII(x) (((x) >= 10) ? (((x) - 10) + 'A') : ((x) + '0'))

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START, 
    APP_STATE_DONE
} t_AppState;

//==============================================================================
// Local Globals
//==============================================================================
t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;

static tstrM2MAPConfig apConfig = {
    WLAN_DEVICE_NAME, 1, 0, M2M_WIFI_WEP_40_KEY_STRING_SIZE, WLAN_AP_WEP_KEY, (uint8_t)WLAN_AP_SEC, WLAN_AP_SSID_MODE
};

static const char httpProvDomainName[] = HTTP_PROV_SERVER_DOMAIN_NAME;

static uint8_t s_MacAddr[] = MAC_ADDRESS;
static int8_t s_DeviceName[] = WLAN_DEVICE_NAME;
static uint8_t mac_addr[6];
static void wifi_cb(uint8_t msgType, void *pvMsg);

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
        printf("Provision http Demo\r\n");
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        
        uint8_t result;
        m2m_wifi_get_otp_mac_address(mac_addr, &result);
        if (!result)
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

        m2m_wifi_start_provision_mode((tstrM2MAPConfig *)&apConfig, (char *)httpProvDomainName, 1);
        printf("Provision Mode started.\r\nPlease connect to ");
        printf(HTTP_PROV_SERVER_DOMAIN_NAME);
        printf(" and fill up the page.\r\n");   
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
        	printf("Connected -- starting DHCP client\r\n");
        } 
        else if (pWifiConnState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
        	printf("disconnected\r\n");
        }
    }
	break;

	case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        // read it from event data
        char buf[M2M_INET4_ADDRSTRLEN];  
        tstrM2MIPConfig *p_ipConfig = (tstrM2MIPConfig *)pvMsg;
        
        // convert binary IP address to string
        inet_ntop4(p_ipConfig->u32StaticIp, buf);
        printf("IP address assigned: %s\r\n", buf);
    }
	break;

	case M2M_WIFI_PROVISION_INFO_EVENT:
    {
        tstrM2MProvisionInfo *p_provisionInfo = (tstrM2MProvisionInfo *)pvMsg;
        if (p_provisionInfo->u8Status == M2M_SUCCESS) 
        {
            printf("Provision succeed.\r\n");
            printf("The AP is: %s\r\n", (char *)p_provisionInfo->au8SSID);
            printf("Now connect to AP!\r\n");
            m2m_wifi_connect((char *)p_provisionInfo->au8SSID,
                             strlen((char *)p_provisionInfo->au8SSID),
                             p_provisionInfo->u8SecType,
                             (void *)p_provisionInfo->au8Password,
                             M2M_WIFI_CH_ALL);
        }
        else
        {
            printf("Provision failed.\r\n");
            SetAppState(APP_STATE_DONE);
            break;
        }
    }
	break;

	default:
    	break;
    }
}

#endif
