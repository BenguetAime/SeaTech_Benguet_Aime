/*******************************************************************************
  File Name:
    winc1500_locate_ip_address.c

  Summary:
    WINC1500 locate ip address demo.

  Description:
    This demo performs the following steps:
        1) connect to AP,
        2) send command to GPS server, 
        3) get reply, and print locate information
 
    The configuration defines for this demo are: 
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        
    At present, this demo is not supported in pic18 project. 
    We will support it in pic18 in next release.
    
    The demo uses these callback functions to handle events:
        wifi_cb()
        socket_cb()
        resolve_cb()
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

#if defined(USING_LOCATE_IP_ADDRESS)
#include "demo_support/iot/errno_local.h"
#include "demo_support/iot/http/http_client.h"
#include "demo_support/iot/json.h"
#include "demo_support/iot/http/http_client.h"
#include "demo_support/iot/json.h"

#define HTTP_CLIENT_TEST_URL        "http://ipinfo.io"  // URL which will be requested

/** Wi-Fi Settings */
#define WLAN_SSID        "DEMO_AP" /* < Destination SSID */
#define WLAN_AUTH        M2M_WIFI_SEC_WPA_PSK /* < Security manner */
#define WLAN_PSK         "12345678" /* < Password for Destination SSID */

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

/** Instance of Timer module. */
struct sw_timer_module swt_module_inst;

/** Instance of HTTP client module. */
struct http_client_module http_client_module_inst;

static void configure_http_client(void);
static void http_client_callback(struct http_client_module *module_inst, int type, union http_client_data *data);
static void wifi_cb(uint8_t msgType, void *pvMsg);
static void resolve_cb(char *pu8DomainName, uint32_t serverIP); 
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);

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
        printf("Locate Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("=========\r\n");    
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);
        configure_http_client();

        /* Connect to router. */
        m2m_wifi_connect((char *)WLAN_SSID, strlen(WLAN_SSID), WLAN_AUTH, 
            (char *)WLAN_PSK, M2M_WIFI_CH_ALL);

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

static void wifi_cb(uint8_t msg_type, void *msg_data)
{
    switch (msg_type) 
    {
    case M2M_WIFI_CONN_STATE_CHANGED_EVENT:
    {
        tstrM2mWifiStateChanged *msg_wifi_state = (tstrM2mWifiStateChanged *)msg_data;
        if (msg_wifi_state->u8CurrState == M2M_WIFI_CONNECTED) 
        {
            /* If Wi-Fi is connected. */
            printf("Wi-Fi connected\r\n");
        } 
        else if (msg_wifi_state->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            /* If Wi-Fi is disconnected. */
            printf("Wi-Fi disconnected\r\n");
            m2m_wifi_connect((char *)WLAN_SSID, strlen(WLAN_SSID), WLAN_AUTH, 
                    (char *)WLAN_PSK, M2M_WIFI_CH_ALL);
        }
    }
    break;

    case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        uint8_t *msg_ip_addr = (uint8_t *)msg_data;
        printf("Wi-Fi IP is %u.%u.%u.%u\r\n",
                msg_ip_addr[0], msg_ip_addr[1], msg_ip_addr[2], msg_ip_addr[3]);
        /* Send the HTTP request. */
        http_client_send_request(&http_client_module_inst, HTTP_CLIENT_TEST_URL, HTTP_METHOD_GET, NULL, NULL);
    }
    break;

    default:
        break;
    }
}

/**
 * \brief Callback of the HTTP client.
 *
 * \param[in]  module_inst     Module instance of HTTP client module.
 * \param[in]  type            Type of event.
 * \param[in]  data            Data structure of the event. \refer http_client_data
 */
static void http_client_callback(struct http_client_module *module_inst, int type, union http_client_data *data)
{
    struct json_obj json, loc;
    switch (type) 
    {
    case HTTP_CLIENT_CALLBACK_SOCK_CONNECTED:
        printf("Connected\r\n");
        break;

    case HTTP_CLIENT_CALLBACK_REQUESTED:
        printf("Request complete\r\n");
        break;

    case HTTP_CLIENT_CALLBACK_RECV_RESPONSE:
        printf("Received response %u data size %u\r\n",
                (unsigned int)data->recv_response.response_code,
                (unsigned int)data->recv_response.content_length);
        if (data->recv_response.content != NULL) 
        {

            int i;
            for (i = 0; i < data->recv_response.content_length; i++)
            {
                if (data->recv_response.content[i] == 0x0a)
                {
                    printf("%c", 0x0d);  // output CR
                }
                printf("%c", data->recv_response.content[i]);
            }
            printf("\r\n===================\r\n");
            
#if !defined(__XC8) // pic18 cannot run json function because memory limit
            if (json_create(&json, data->recv_response.content, data->recv_response.content_length) == 0 &&
                    json_find(&json, "loc", &loc) == 0) 
            {
                printf("Location : %s\r\n", loc.value.s);
            }
            printf("===================\r\nDemo is done!\r\n");
#endif
        }

        break;

    case HTTP_CLIENT_CALLBACK_DISCONNECTED:
        printf("Disconnected reason:%d\r\n", data->disconnected.reason);

        /* If disconnect reason is equals to -ECONNRESET(-104),
         * It means Server was disconnected your connection by the keep alive timeout.
         * This is normal operation.
         */
        if (data->disconnected.reason == -EAGAIN) 
        {
            /* Server has not responded. retry it immediately. */
#if !defined(__XC8)   // PIC18 cannot support recursive calling          
            http_client_send_request(&http_client_module_inst, HTTP_CLIENT_TEST_URL, HTTP_METHOD_GET, NULL, NULL);
#endif
        }

        break;
    }
}

/**
 * \brief Callback to get the Socket event.
 *
 * \param[in] Socket descriptor.
 * \param[in] msg_type type of Socket notification. Possible types are:
 *  - [M2M_SOCKET_CONNECT_EVENT](@ref M2M_SOCKET_CONNECT_EVENT)
 *  - [M2M_SOCKET_BIND_EVENT](@ref M2M_SOCKET_BIND_EVENT)
 *  - [M2M_SOCKET_LISTEN_EVENT](@ref M2M_SOCKET_LISTEN_EVENT)
 *  - [M2M_SOCKET_ACCEPT_EVENT](@ref M2M_SOCKET_ACCEPT_EVENT)
 *  - [M2M_SOCKET_RECV_EVENT](@ref M2M_SOCKET_RECV_EVENT)
 *  - [M2M_SOCKET_SEND_EVENT](@ref M2M_SOCKET_SEND_EVENT)
 *  - [M2M_SOCKET_SENDTO_EVENT](@ref M2M_SOCKET_SENDTO_EVENT)
 *  - [M2M_SOCKET_RECVFROM_EVENT](@ref M2M_SOCKET_RECVFROM_EVENT)
 * \param[in] msg_data A structure contains notification informations.
 */
static void socket_cb(SOCKET sock, uint8_t msg_type, void *msg_data)
{
    http_client_socket_event_handler(sock, msg_type, msg_data);
}

/**
 * \brief Callback of gethostbyname function.
 *
 * \param[in] doamin_name Domain name.
 * \param[in] server_ip IP of server.
 */
static void resolve_cb(char *domain_name, uint32_t server_ip)
{
    http_client_socket_resolve_handler((uint8_t *)domain_name, server_ip);
}

/**
 * \brief Configure HTTP client module.
 */
static void configure_http_client(void)
{
    struct http_client_config httpc_conf;
    int ret;

    http_client_get_config_defaults(&httpc_conf);

    httpc_conf.recv_buffer_size = 256;
    httpc_conf.timer_inst = &swt_module_inst;
    /* ipinfo.io send json format data if only client is a curl. */
    httpc_conf.user_agent = "curl/7.10.6";

    ret = http_client_init(&http_client_module_inst, &httpc_conf);
    if (ret < 0) 
    {
        printf("HTTP client initialization has failed(%d)\r\n", ret);
        while (1) 
        {
        } /* Loop forever. */
    }

    http_client_register_callback(&http_client_module_inst, http_client_callback);
}

#if defined(__XC8)
void m2m_socket_handle_events_Pic18WaiteHttpSend(SOCKET sock, t_m2mSocketEventType eventCode, t_socketEventData *p_eventData)
{
    http_client_socket_event_handler_Pic18WaiteHttpSend(sock, eventCode, p_eventData);
}
#endif
#endif

