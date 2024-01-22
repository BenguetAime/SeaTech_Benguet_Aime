/*******************************************************************************
  File Name:
    winc1500_mac_address.c

  Summary:
    Read Mac Address.

  Description:
    This is an example code for functions:
    void m2m_wifi_get_otp_mac_address(uint8_t *pu8MacAddr, uint8_t *pu8IsValid);
    void m2m_wifi_set_mac_address(uint8_t *au8MacAddress);
    void m2m_wifi_get_mac_address(uint8_t *pu8MacAddr);
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

#if defined(USING_MAC_ADDRESS)
//==============================================================================
// DEMO CONFIGURATION
//==============================================================================
// configure these defines for the AP the demo is attempting to connect to
#define WLAN_SSID   "DEMO_AP"             // target AP
#define WLAN_AUTH   M2M_WIFI_SEC_WPA_PSK       // AP Security 
#define WLAN_PSK    "12345678"              // security password

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
static uint8_t mac_addr[M2M_MAC_ADDRESS_LEN];
const char user_define_mac_address[] = {0xf8, 0xf0, 0x05, 0x20, 0x0b, 0x09};

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
        printf("MAC Address Demo\r\n");
        printf("=========\r\n");

        uint8_t result;
        m2m_wifi_get_otp_mac_address(mac_addr, &result);
        if (!result)
        {
            printf("USER MAC Address : ");
            /* Cannot found MAC Address from OTP. Set user define MAC address. */
            m2m_wifi_set_mac_address((uint8_t *)user_define_mac_address);
        } 
        else 
        {
            printf("OTP MAC Address : ");
        }

        /* Get MAC Address. */
        m2m_wifi_get_mac_address(mac_addr);
        printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n",
            mac_addr[0], mac_addr[1], mac_addr[2],
            mac_addr[3], mac_addr[4], mac_addr[5]);
        printf("Done.\r\n\r\n");
        SetAppState(APP_STATE_DONE);
        break;
        
    case APP_STATE_DONE:
        break;
    }
}

#endif

