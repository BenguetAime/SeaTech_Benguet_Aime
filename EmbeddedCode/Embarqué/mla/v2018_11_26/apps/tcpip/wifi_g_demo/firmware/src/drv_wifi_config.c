/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_config.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Customization

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

/*==========================================================================*/
/*                                  INCLUDES                                */
/*==========================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include "tcpip/tcpip.h"

#if defined ( EZ_CONFIG_SCAN )
#include "driver/wifi/mrf24w/src/drv_wifi_easy_config.h"
#endif /* EZ_CONFIG_SCAN */

// Differences to wifi comm demo board (MRF24WB0MA) :
//        Wifi comm demo board is centered on variable CPElements.
//        Wifi G demo board is centered on variable AppConfig, since this is the generic approach adopted by
//        TCPIP demo/ezconsole/ezconfig apps.

/*==========================================================================*/
/*                                  DEFINES                                 */
/*==========================================================================*/
extern uint8_t g_scan_done;

/*****************************************************************************
 * FUNCTION: void WF_Connect(void)
 *
 * RETURNS:  None
 *
 * PARAMS:   none
 *
 * NOTES:    This routine creates a Connection Profile Entry, Initializes it
 *                then creates the connection algorithm and establishes the WiFi
 *                connection according to the Profile settings.
 *                Customize this function as needed for your application.
 *
 *                Wifi Direct, ensure following parameters are set up properly
 *                  - AppConfig.ssid =  "DIRECT-"
 *                  - sizeof(channelList) == 3)
 *                  - channelList[0] == 1
 *                  - channelList[1] == 6
 *                  - channelList[2] == 11
 *
 *****************************************************************************/
void WF_Connect(void)
{
    uint8_t ConnectionProfileID;
    uint8_t channelList[] = MY_DEFAULT_CHANNEL_LIST;
    uint8_t channelList_postscan[] = MY_DEFAULT_CHANNEL_LIST_POSTSCAN;

    /* create a Connection Profile */
    WF_CPCreate(&ConnectionProfileID);

    AppConfig.passphraseToBinaryKeyFlag = 0;

    // Enables or disables the MRF24W Regional Domain with RF module FW version 0x3106 or earlier
    // With RF module FW version 0x3107 and future releases, this function is NOT supported due to changes
    // in FCC requirements, which does not allow programming of the regional domain.
    WF_SetRegionalDomain(MY_DEFAULT_DOMAIN);

    if ((AppConfig.networkType == WF_SOFT_AP) || (AppConfig.networkType == WF_ADHOC)) {
        /**********************************************************************/
        /* Append Last 4 digits to MAC address to SSID - Creating unique SSID */
        /* Wifi comm demo SSID : MCHP_xxxx                                    */
        /* Wifi G demo SSID : MCHP_G_xxxx                                     */
        /**********************************************************************/
        sprintf((char *) AppConfig.ssid, "MCHP_G_%02x%02x", AppConfig.MyMACAddr.v[4], AppConfig.MyMACAddr.v[5]);
        sprintf((char *) AppConfig.NetBIOSName, "%s%02x%02x", MY_DEFAULT_HOST_NAME, AppConfig.MyMACAddr.v[4], AppConfig.MyMACAddr.v[5]);

        size_t i = 0;
        // in the following while loop, all lower case letters have been changed to upper case ones
        // because browser automatically translates lower case letters to upper case
        while (i < sizeof (AppConfig.NetBIOSName)) {
            if (((*((char *) AppConfig.NetBIOSName + i)) >= 'a') & ((*((char *) AppConfig.NetBIOSName + i)) <= 'z')) {
                *((char *) AppConfig.NetBIOSName + i) = *((char *) AppConfig.NetBIOSName + i) - 32;
            } else if (*((char *) AppConfig.NetBIOSName + i) == '\0')
                break;
            i++;
        }
        FormatNetBIOSName(AppConfig.NetBIOSName);
    }

    AppConfig.ssidLen = strlen((char *) (AppConfig.ssid));

    WF_CPSetSsid(ConnectionProfileID, AppConfig.ssid, AppConfig.ssidLen);

    WF_CPSetNetworkType(ConnectionProfileID, AppConfig.networkType);
    if (AppConfig.networkType == WF_ADHOC) {
        WF_CPSetAdHocBehavior(ConnectionProfileID, WF_ADHOC_CONNECT_THEN_START);
    }

#if !defined(MRF24WG)
    Delay10us(10); /* required for MRF24WB */
#endif

#if WF_HOST_DERIVE_KEY_FROM_PASSPHRASE == WF_ENABLED
    if (AppConfig.securityMode == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE ||
        AppConfig.securityMode == WF_SECURITY_WPA_WITH_PASS_PHRASE ||
        AppConfig.securityMode == WF_SECURITY_WPA2_WITH_PASS_PHRASE) {
        WF_DeriveKey(AppConfig.securityKeyLength, AppConfig.securityKey, AppConfig.ssidLen, AppConfig.ssid);
        AppConfig.securityMode--;
        AppConfig.securityKeyLength = 32;
        AppConfig.passphraseToBinaryKeyFlag = 1;
    }
#endif /* #if WF_HOST_DERIVE_KEY_FROM_PASSPHRASE == WF_ENABLED */

    switch (AppConfig.securityMode) {
    case WF_SECURITY_OPEN:
        WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_OPEN, 0, NULL, 0);
        break;
    case WF_SECURITY_WEP_40:
        // assume key 0
        WF_CPSetSecurity(ConnectionProfileID, AppConfig.securityMode, 0, AppConfig.securityKey, 5);
        break;
    case WF_SECURITY_WEP_104:
        // assume key 0
        WF_CPSetSecurity(ConnectionProfileID, AppConfig.securityMode, 0, AppConfig.securityKey, 13);
        break;
    case WF_SECURITY_WPA_AUTO_WITH_KEY:
        WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WPA_AUTO_WITH_KEY,
                0, AppConfig.securityKey, 32);
        break;
    case WF_SECURITY_WPA_WITH_KEY:
        WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WPA_WITH_KEY,
                0, AppConfig.securityKey, 32);
        break;
    case WF_SECURITY_WPA2_WITH_KEY:
        WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WPA2_WITH_KEY,
                0, AppConfig.securityKey, 32);
        break;
    default:
        WF_ASSERT(false);
        break;
    }

    WF_CASetScanType(MY_DEFAULT_SCAN_TYPE);

    if (((CFGCXT.networkType != WF_SOFT_AP)&&(CFGCXT.prevNetworkType == WF_SOFT_AP)) || (AppConfig.networkType != WF_SOFT_AP)) {
        WF_CASetChannelList(channelList_postscan, sizeof (channelList_postscan));
    } else {
        WF_CASetChannelList(channelList, sizeof (channelList));
    }

    // The Retry Count parameter tells the WiFi Connection manager how many attempts to make when trying
    // to connect to an existing network.  In the Infrastructure case, the default is to retry forever so that
    // if the AP is turned off or out of range, the radio will continue to attempt a connection until the
    // AP is eventually back on or in range.  In the Adhoc case, the default is to retry 3 times since the
    // purpose of attempting to establish a network in the Adhoc case is only to verify that one does not
    // initially exist.  If the retry count was set to WF_RETRY_FOREVER in the AdHoc mode, an AdHoc network
    // would never be established.  The constants MY_DEFAULT_LIST_RETRY_COUNT_ADHOC and
    // MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE have been created specifically for the June 2011 MAL release.
    if ((AppConfig.networkType == CFG_WF_ADHOC) || (AppConfig.networkType == CFG_WF_SOFT_AP)) {
        WF_CASetListRetryCount(ADHOC_RETRY_COUNT);
    } else /* AppConfig.networkType == CFG_WF_INFRASTRUCTURE */ {
        WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE);
    }

    WF_CASetEventNotificationAction(MY_DEFAULT_EVENT_NOTIFICATION_LIST);

#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
#if (MY_DEFAULT_PS_POLL == WF_ENABLED)
    WF_PsPollEnable(true);
#else
    WF_PsPollDisable();
#endif
#endif

    WF_CASetBeaconTimeout(MY_DEFAULT_BEACON_TIMEOUT);

#if defined(STACK_USE_UART)
    WF_OutputConnectionInfo(&AppConfig);
#endif

    if (AppConfig.networkType == CFG_WF_SOFT_AP) {
#if (WF_SOFTAP_SEND_KEEP_ALIVE == WF_ENABLED)
        WF_SetLinkDownThreshold(WF_SOFTAP_LINK_LOST_THRESHOLD);
#endif
    } else { // AppConfig.networkType != CFG_WF_SOFT_AP
#if (WF_CHECK_LINK_STATUS == WF_ENABLED)
        WF_SetLinkDownThreshold(WF_LINK_LOST_THRESHOLD);
#endif
    }

    // Initiates connection to BSS
    WF_CMConnect(ConnectionProfileID);
}

/*****************************************************************************
 * FUNCTION: WF_ProcessEvent
 *
 * RETURNS:  None
 *
 * PARAMS:   event      -- event that occurred
 *           eventInfo  -- additional information about the event.  Not all events
 *           have associated info, in which case this value will be
 *           set to WF_NO_ADDITIONAL_INFO (0xff)
 *           extraInfo - more additional information about the event
 *
 *  NOTES:   The Host application must NOT directly call this function.  This
 *           function is called by the WiFi Driver code when an event occurs
 *           that may need processing by the Host CPU.
 *
 *           No other WiFi Driver function should be called from this function, with the
 *           exception of WF_ASSERT.  It is recommended that if the application wishes to be
 *           notified of an event that it simply set a flag and let application code in the
 *           main loop handle the event.
 *
 *           WFSetFuncState must be called when entering and leaving this function.
 *           When WF_DEBUG is enabled this allows a runtime check if any illegal WF functions
 *           are called from within this function.
 *
 *           For events that the application is not interested in simply leave the
 *           case statement empty.
 *
 *           Customize this function as needed for your application.
 *****************************************************************************/
void WF_ProcessEvent(uint8_t event, uint16_t eventInfo, uint8_t *extraInfo)
{
#if defined(STACK_USE_UART)
    char buf[8];
    tMgmtIndicateSoftAPEvent *softAPEvent;
#endif

    /* this function tells the WF driver that we are in this function */
    WFSetFuncState(WF_PROCESS_EVENT_FUNC, WF_ENTERING_FUNCTION);

    switch (event) {
        /*--------------------------------------*/
    case WF_EVENT_CONNECTION_SUCCESSFUL:
        /*--------------------------------------*/
#if defined(EZ_CONFIG_STORE)
        AppConfig.saveSecurityInfo = true;
#endif
        break;

        /*--------------------------------------*/
    case WF_EVENT_CONNECTION_FAILED:
    case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
    case WF_EVENT_CONNECTION_PERMANENTLY_LOST:
        /*--------------------------------------*/
        break;

        /*--------------------------------------*/
    case WF_EVENT_CONNECTION_REESTABLISHED:
        /*--------------------------------------*/
        break;

        /*--------------------------------------*/
    case WF_EVENT_SCAN_RESULTS_READY:
        /*--------------------------------------*/
#if defined ( EZ_CONFIG_SCAN )
        WFScanEventHandler(eventInfo);
#endif /* EZ_CONFIG_SCAN */
        g_scan_done = 1; // WF_PRESCAN
        break;

    case WF_EVENT_SOFT_AP_EVENT:
#if defined(STACK_USE_UART)
    {
        softAPEvent = (tMgmtIndicateSoftAPEvent *) extraInfo;
        char str[96];
        char *result = "None";
        char *reason = "None";
        uint8_t *addr = softAPEvent->address;
        putrsUART("Event: SoftAP, ");
        if (softAPEvent->event == SOFTAP_EVENT_CONNECTED) {
            result = "Connected";
        } else if (softAPEvent->event == SOFTAP_EVENT_DISCONNECTED) {
            result = "Disconnected";
            if (softAPEvent->reason == SOFTAP_EVENT_LINK_LOST)
                reason = "LinkLost";
            else if (softAPEvent->reason == SOFTAP_EVENT_RECEIVED_DEAUTH)
                reason = "ReceivedDeauth";
        }
        sprintf(str, "%s, %s, %x:%x:%x:%x:%x:%x", result, reason, addr[0], addr[1], addr[2],
                addr[3], addr[4], addr[5]);
        putsUART(str);
        putrsUART("\r\n");
    }
#endif /* STACK_USE_UART */
        break;

    default:
        WF_ASSERT(false); /* unknown event */
        break;
    }

    /* Informs the WF driver that we are leaving this function */
    WFSetFuncState(WF_PROCESS_EVENT_FUNC, WF_LEAVING_FUNCTION);
}
