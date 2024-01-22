/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_config.h

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

#ifndef __WF_CONFIG_H_
#define __WF_CONFIG_H_

#include <stdint.h>

/*******************************************************************************
 *                             DEFINES
 ********************************************************************************/

#define WF_TCPIP_DEMO

/****************************************************************************
 * MRF24WB0MA/B  (supports only 1/2 Mbps)
 *      Client in infrastructure network                     (ALL security)
 *      Adhoc network                                        (OPEN, WEP security)
 *
 * MRF24WG0MA/B
 *      Client in infrastructure network                     (ALL security)
 *      Adhoc network                                        (OPEN, WEP security)
 *      Group client (GC) in WFii direct (P2P) network
 *      SoftAP                                               (OPEN, WEP security)
 *      Supports WPS security connection
 *
 * Available documentation
 * DS52108A  Microchip MRF24W Getting started Guide for MRF24WB0MAB, MRF24WG0MA/B for MLA v5
 *****************************************************************************/

/*****************************************************************************/
/*****************************************************************************/
/*                             WIFI SECURITY COMPILE-TIME DEFAULTS           */
/*****************************************************************************/
/*****************************************************************************
 * Security modes available on WiFi network:
 *   WF_SECURITY_OPEN                      : No security
 *   WF_SECURITY_WEP_40                    : WEP Encryption using 40 bit keys
 *   WF_SECURITY_WEP_104                   : WEP Encryption using 104 bit keys
 *   WF_SECURITY_WPA_AUTO_WITH_KEY         : WPA-PSK or WPA2-PSK where 64 hex characters key is given to MRF24W for connecting
 *   WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE : WPA-PSK or WPA2-PSK where passphrase is given and converted by MRF24W to 64 hex characters key
 *   WF_SECURITY_WPA_WITH_KEY         : WPA-PSK where 64 hex characters key is given to MRF24W for connecting
 *   WF_SECURITY_WPA_WITH_PASS_PHRASE : WPA-PSK where passphrase is given and converted by MRF24W to 64 hex characters key
 *   WF_SECURITY_WPA2_WITH_KEY         : WPA2-PSK where 64 hex characters key is given to MRF24W for connecting
 *   WF_SECURITY_WPA2_WITH_PASS_PHRASE : WPA2-PSK where passphrase is given and converted by MRF24W to 64 hex characters key
 *   WF_SECURITY_WPS_PUSH_BUTTON           : WPS PBC method
 *   WF_SECURITY_WPS_PIN                   : WPS PIN method
 ******************************************************************************/

// Comments: CFG_WF_SOFT_AP is only available in EZ Config
#define CFG_WF_INFRASTRUCTURE 1
#define CFG_WF_ADHOC          2
#define CFG_WF_P2P            3

#define MY_DEFAULT_NETWORK_TYPE             CFG_WF_INFRASTRUCTURE /* CFG_WF_INFRASTRUCTURE, CFG_WF_ADHOC, CFG_WF_P2P */
#define MY_DEFAULT_DOMAIN                   WF_DOMAIN_FCC
#define MY_DEFAULT_LIST_RETRY_COUNT         WF_RETRY_FOREVER /* Number of times to try to connect to the SSID when using Infrastructure network type */

#if MY_DEFAULT_NETWORK_TYPE == CFG_WF_ADHOC
#undef MY_DEFAULT_LIST_RETRY_COUNT
#define MY_DEFAULT_LIST_RETRY_COUNT         3
#endif

/*--------------------------------------------------------*/
/* if starting this demo in infrastructure or ad-hoc mode */
/*--------------------------------------------------------*/
#if MY_DEFAULT_NETWORK_TYPE == CFG_WF_INFRASTRUCTURE || MY_DEFAULT_NETWORK_TYPE == CFG_WF_ADHOC
#define MY_DEFAULT_WIFI_SECURITY_MODE       WF_SECURITY_OPEN
#define MY_DEFAULT_SCAN_TYPE                WF_ACTIVE_SCAN            /* WF_ACTIVE_SCAN or WF_PASSIVE_SCAN */
#define MY_DEFAULT_BEACON_TIMEOUT           (40)                      /* Number of beacon periods          */
#define MY_DEFAULT_SSID_NAME                "MicrochipDemoAPP"        /* if WF_SECURITY_WPS_PUSH_BUTTON must be "" (empty string) */
#if defined(__XC8)
#define MY_DEFAULT_CHANNEL_LIST             {1,2,3,4,5,6,7,8,9,10,11} /* Default channel list for FCC */
#else
#define MY_DEFAULT_CHANNEL_LIST             {}                        /* Default channel list for FCC */
#endif
/* Select Infrastructure Power Save Mode */
#define MY_DEFAULT_PS_POLL                  WF_DISABLED               /* PS is not supported in Adhoc */
/*------------------------------------------------------*/
/* else if starting this demo in P2P(Wi-Fi Direct) mode */
/*------------------------------------------------------*/
#elif MY_DEFAULT_NETWORK_TYPE == CFG_WF_P2P
#if defined (MRF24WG)
/*
 * Wi-Fi Direct has been tested with Samsung Galaxy Tab 2 7.0 ( Android 4.0.3, Ice cream Sandwitch),
 * Galaxy-Nexus and Galaxy S III(Android 4.04). MRF24W supports only GC function.
 */
#define MY_DEFAULT_WIFI_SECURITY_MODE               WF_SECURITY_WPS_PUSH_BUTTON
#define MY_DEFAULT_SCAN_TYPE                        WF_ACTIVE_SCAN
#define MY_DEFAULT_SSID_NAME                        "DIRECT-" /* Fixed SSID. Do not change */
#define MY_DEFAULT_CHANNEL_LIST                     {1, 6, 11} /* Social channels. Do not change */
#define MY_DEFAULT_BEACON_TIMEOUT                   (40) /* Number of beacon periods */
#define MY_DEFAULT_PS_POLL                          WF_DISABLED
#else /* !defined (MRF24WG) */
#error "MRF24WB does not support Wi-Fi Direct (P2P)"
#endif /* defined (MRF24WG) */
#endif /* MY_DEFAULT_NETWORK_TYPE == CFG_WF_INFRASTRUCTURE */

#define WF_CHECK_LINK_STATUS   WF_ENABLED /* Gets the MRF to check the link status relying on Tx failures. */
#define WF_LINK_LOST_THRESHOLD 40 /* Consecutive Tx transmission failures to be considered the AP is gone away. */

/* 
 * MRF24W FW has a built-in connection manager, and it is enabled by default.
 * If you want to run your own connection manager in host side, you should
 * disable the FW connection manager to avoid possible conflict between the two.
 * Especially these two APIs can be affected if you do not disable it.
 * A) uint16_t WF_CMDisconnect(void)
 * B) uint16_t WF_Scan(uint8_t CpId)
 * These APIs will return failure when the conflict occurs.
 */
#define WF_MODULE_CONNECTION_MANAGER WF_ENABLED

#if defined(__XC32)
/* This is optional, which results faster key derivation */
#define WF_HOST_DERIVE_KEY_FROM_PASSPHRASE WF_ENABLED
#else
#define WF_HOST_DERIVE_KEY_FROM_PASSPHRASE WF_DISABLED
#endif /* defined(__XC32) */

#if defined (MRF24WG)
/* only used when security is WF_SECURITY_WPS_PIN */
#define MY_DEFAULT_WPS_PIN "12390212" 
/* 
 * An example PIN.
 * The last digit is the checksum of first 7 digits.
 */                                                       
#endif /* !defined (MRF24WG) */

//-----------------------------------------------------------------------------------
// WEP
// Default WEP keys used in WF_SECURITY_WEP_40  and WF_SECURITY_WEP_104 security mode
// Only WEP key index 0 is valid
//-----------------------------------------------------------------------------------
#define MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE  WF_SECURITY_WEP_OPENKEY
/* WF_SECURITY_WEP_OPENKEY (default) or                */
/* WF_SECURITY_WEP_SHAREDKEY - to be used only         */
/* with MRF24WG or 0x1209 and later version of MRF24WB */

#define MY_DEFAULT_WEP_PHRASE "WEP Phrase"

// string 4 40-bit WEP keys -- corresponding to passphrase of "WEP Phrase"
#define MY_DEFAULT_WEP_KEYS_40 (\
"\x5a\xfb\x6c\x8e\x77"\
"\xc1\x04\x49\xfd\x4e"\
"\x43\x18\x2b\x33\x88"\
"\xb0\x73\x69\xf4\x78")
// Do not indent above string as it will inject spaces

// string containing 4 104-bit WEP keys -- corresponding to passphrase of "WEP Phrase"
#define MY_DEFAULT_WEP_KEYS_104 (\
"\x90\xe9\x67\x80\xc7\x39\x40\x9d\xa5\x00\x34\xfc\xaa"\
"\x77\x4a\x69\x45\xa4\x3d\x66\x63\xfe\x5b\x1d\xb9\xfd"\
"\x82\x29\x87\x4c\x9b\xdc\x6d\xdf\x87\xd1\xcf\x17\x41"\
"\xcc\xd7\x62\xde\x92\xad\xba\x3b\x62\x2f\x7f\xbe\xfb")
// Do not indent above string as it will inject spaces

//-----------------------------------------------------------------------------------
// Default pass phrase used for WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE security mode
//-----------------------------------------------------------------------------------
#define MY_DEFAULT_PSK_PHRASE "Microchip 802.11 Secret PSK Password"

// If using security mode of WF_SECURITY_WPA_AUTO_WITH_KEY, then this section
// must be set to match the key for MY_DEFAULT_SSID_NAME and MY_DEFAULT_PSK_PHRASE
// combination.  The values below are derived from the SSID "MicrochipDemoAPP" and the pass phrase
// "Microchip 802.11 Secret PSK Password".
// The tool at http://www.wireshark.org/tools/wpa-psk.html can be used to generate this field.
#define MY_DEFAULT_PSK (\
"\x06\x19\xFC\x85\xEE\xFE\xD8\x2B"\
"\xE0\x19\x95\x77\x9A\xDA\x7B\x38"\
"\xD6\x73\x2A\x32\x98\x1C\x1B\x30"\
"\x32\x0A\xBF\xFA\x29\x70\x23\x52")
// Do not indent above string as it will inject spaces

/*** Selecting Event Notification Type ***/
#define MY_DEFAULT_EVENT_NOTIFICATION_LIST  (WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL  |         \
                                             WF_NOTIFY_CONNECTION_ATTEMPT_FAILED      |         \
                                             WF_NOTIFY_CONNECTION_TEMPORARILY_LOST    |         \
                                             WF_NOTIFY_CONNECTION_PERMANENTLY_LOST    |         \
                                             WF_NOTIFY_CONNECTION_REESTABLISHED)

#endif /* __WF_CONFIG_H_ */
