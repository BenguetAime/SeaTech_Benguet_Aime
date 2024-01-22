#ifndef __DEMO_CONFIG_H
#define __DEMO_CONFIG_H

#define USING_AP_SCAN
//#define USING_MAC_ADDRESS
//#define USING_MODE_AP
//#define USING_TCP_SERVER
//#define USING_TCP_CLIENT
//#define USING_UDP_SERVER
//#define USING_UDP_CLIENT
//#define USING_PROVISION_HTTP
//#define USING_MODE_CHANGE
//#define USING_MODE_P2P
//#define USING_MODE_STA
//#define USING_SECURITY_WPS
//#define USING_SECURITY_WEP_WPA
//#define USING_POWER_SAVE_MODE
//#define USING_SIGNAL_STRENGTH
//#define USING_TIME_CLIENT
//#define USING_LOCATE_IP_ADDRESS
//#define USING_HTTP_DOWNLOAD
//#define USING_SEND_EMAIL
//#define USING_OTA_UPDATE
//#define USING_WEATHER_CLIENT
//#define USING_PROVISION_AP
//#define USING_PUBNUB
//#define USING_SIMPLE_GROWL
//#define USING_SSL_CONNECT
//#define USING_FW_UPDATE_OVER_SERIAL

#if defined(USING_HTTP_DOWNLOAD) || defined(USING_LOCATE_IP_ADDRESS)
#define USING_SUPPORT_IOT
#endif

// helper functions to read and clear application flags set in the event handlers.
void ClearWiFiEventStates(void);
bool isDriverInitComplete(void);
bool isScanResultReady(void);
bool isConnectionStateChanged(void);
bool isIpAddressAssigned(void);
bool isScanComplete(void);
bool isRssiReady(void);
bool isProvisionInfoReady(void);
bool isWpsReady(void);
bool isPrngReady(void);

void ClearSocketEventStates(void);
bool isSocketBindOccurred(void);
bool isSocketListenOccurred(void);
bool isSocketAcceptOccurred(void);
bool isSocketConnectOccurred(void);
bool isSocketRecvOccurred(void);
bool isSocketRecvFromOccurred(void);
bool isSocketSendOccurred(void);
bool isSocketSendToOccurred(void);
bool isPingReplyOccurred(void);
bool isDnsResolved(void);

typedef void (*tpfAppWifiCb) (uint8_t u8MsgType, void * pvMsg);
void ApplicationTask(void);
void registerWifiCallback(tpfAppWifiCb pfAppWifiCb);
void registerSocketCallback(tpfAppSocketCb pfAppSocketCb, tpfAppResolveCb pfAppResolveCb);
void RegisterSocketCB(tpfAppSocketCb socketCB, tpfAppResolveCb resoveCB);
#endif // __DEMO_CONFIG_H
