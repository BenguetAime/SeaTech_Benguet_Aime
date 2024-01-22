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

#include "winc1500_api.h"
#include "demo_config.h"

#if defined(USING_TCP_SERVER) || defined(USING_TCP_CLIENT) || defined(USING_PROVISION_HTTP) || defined(USING_PROVISION_AP) || defined(USING_MAC_ADDRESS) || defined(USING_SSL_CONNECT) \
    || defined(USING_MODE_AP) || defined(USING_MODE_STA) || defined(USING_MODE_P2P) || defined(USING_MODE_CHANGE) || defined(USING_POWER_SAVE_MODE) || defined(USING_SIGNAL_STRENGTH) \
    || defined(USING_SECURITY_WEP_WPA) || defined(USING_SECURITY_WPS) || defined(USING_TIME_CLIENT) || defined(USING_WEATHER_CLIENT) || defined(USING_LOCATE_IP_ADDRESS) \
    || defined(USING_HTTP_DOWNLOAD) || defined(USING_PUBNUB) || defined(USING_SEND_EMAIL) || defined(USING_OTA_UPDATE) \
    || defined(USING_SIMPLE_GROWL) || defined(USING_FW_UPDATE_OVER_SERIAL)

static bool g_driverInitComplete = false;

volatile tpfAppWifiCb gpfAppWifiCb = NULL;
volatile tpfAppSocketCb  gpfAppSocketCb = NULL;
volatile tpfAppResolveCb gpfAppResolveCb = NULL;

void m2m_error_handle_events(uint32_t errorCode)
{
    dprintf("   \nERROR EVENT: %lu\n", errorCode);
}

void registerWifiCallback(tpfAppWifiCb pfAppWifiCb)
{
    gpfAppWifiCb = pfAppWifiCb;
}

void registerSocketCallback(tpfAppSocketCb pfAppSocketCb, tpfAppResolveCb pfAppResolveCb)
{
    gpfAppSocketCb = pfAppSocketCb;
    gpfAppResolveCb = pfAppResolveCb;
}

void m2m_wifi_handle_events(t_m2mWifiEventType eventCode, t_wifiEventData *p_eventData)
{
    if (eventCode == M2M_WIFI_DRIVER_INIT_EVENT)
    {
        g_driverInitComplete = true;
    }

    if (gpfAppWifiCb)
        gpfAppWifiCb(eventCode, p_eventData);
}

bool isDriverInitComplete(void)
{
    bool res = g_driverInitComplete;
    g_driverInitComplete = false;
    return res;
}

void m2m_socket_handle_events(SOCKET sock, t_m2mSocketEventType eventCode, t_socketEventData *p_eventData)
{
    if (eventCode == M2M_SOCKET_DNS_RESOLVE_EVENT)
    {
        if (gpfAppResolveCb)
            gpfAppResolveCb(m2m_wifi_get_socket_event_data()->dnsReply.hostName, m2m_wifi_get_socket_event_data()->dnsReply.hostIp);
    }
    else
    {
        if (gpfAppSocketCb)
            gpfAppSocketCb(sock, eventCode, p_eventData);
    }
}

/**
 * \brief Callback to get the OTA update event.
 *
 * \param[in] u8OtaUpdateStatusType type of OTA update status notification. Possible types are:
 * - [DL_STATUS](@ref DL_STATUS)
 * - [SW_STATUS](@ref SW_STATUS)
 * - [RB_STATUS](@ref RB_STATUS)
 * \param[in] u8OtaUpdateStatus type of OTA update status detail. Possible types are:
 * - [OTA_STATUS_SUCSESS](@ref OTA_STATUS_SUCSESS)
 * - [OTA_STATUS_FAIL](@ref OTA_STATUS_FAIL)
 * - [OTA_STATUS_INVAILD_ARG](@ref OTA_STATUS_INVAILD_ARG)
 * - [OTA_STATUS_INVAILD_RB_IMAGE](@ref OTA_STATUS_INVAILD_RB_IMAGE)
 * - [OTA_STATUS_INVAILD_FLASH_SIZE](@ref OTA_STATUS_INVAILD_FLASH_SIZE)
 * - [OTA_STATUS_AlREADY_ENABLED](@ref OTA_STATUS_AlREADY_ENABLED)
 * - [OTA_STATUS_UPDATE_INPROGRESS](@ref OTA_STATUS_UPDATE_INPROGRESS)
 */
static void OtaUpdateCb(uint8_t u8OtaUpdateStatusType, uint8_t u8OtaUpdateStatus)
{
    printf("OtaUpdateCb %d %d\r\n", u8OtaUpdateStatusType, u8OtaUpdateStatus);
    if (u8OtaUpdateStatusType == DL_STATUS) {
        if (u8OtaUpdateStatus == OTA_STATUS_SUCCESS) {
            /* Start Host Controller OTA HERE ... Before switching.... */
            printf("OtaUpdateCb m2m_ota_switch_firmware start.\r\n");
            m2m_ota_switch_firmware();
        } else {
            printf("OtaUpdateCb FAIL u8OtaUpdateStatus %d\r\n", u8OtaUpdateStatus);
        }
    } else if (u8OtaUpdateStatusType == SW_STATUS) {
        if (u8OtaUpdateStatus == OTA_STATUS_SUCCESS) {
            printf("OTA Success !!!\r\n");
            /* system_reset(); */
        }
    }
}

void m2m_ota_handle_events(t_m2mOtaEventType eventCode, t_m2mOtaEventData *p_eventData)
{
    switch (eventCode)
    {
        case M2M_OTA_STATUS_EVENT:
            // data in p_eventData->otaUpdateStatus
            OtaUpdateCb(p_eventData->otaUpdateStatus.u8OtaUpdateStatusType, p_eventData->otaUpdateStatus.u8OtaUpdateStatus);
            break;
    }
}

#if defined(__XC8)
#if !(defined(USING_HTTP_DOWNLOAD) || defined(USING_LOCATE_IP_ADDRESS))
void m2m_socket_handle_events_Pic18WaiteHttpSend(SOCKET sock, t_m2mSocketEventType eventCode, t_socketEventData *p_eventData)
{
}
#endif
#endif

#endif