/*******************************************************************************
  File Name:
    winc1500_http_download.c

  Summary:
    WINC1500 http download demo.

  Description:
    This demo performs the following steps:
        1) connect to AP,
        2) send command to web server, 
        3) get reply, and print information
 
    The configuration defines for this demo are:    
        WLAN_SSID             -- AP to connect to
        WLAN_AUTH             -- Security type of the AP
        WLAN_PSK              -- Passphrase if using WPA security
        HTTP_FILE_URL         -- URI for download.
        
    At present, this demo is not supported in pic18 project. We will support it in pic18 in next release.
    
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

#if defined(USING_HTTP_DOWNLOAD)
#include "demo_support/iot/errno_local.h"
#include "demo_support/iot/http/http_client.h"

#define WLAN_SSID                       "DEMO_AP"           // Destination SSID
#define WLAN_AUTH                       M2M_WIFI_SEC_WPA_PSK // Security manner
#define WLAN_PSK                        "12345678"          // Password for Destination SSID
#define HTTP_FILE_URL                   "http://www.atmel.com/images/icon_pdf.gif"  // URI for download.

#define BUFFER_MAX_SIZE                 (1446)                         // Maximum size for packet buffer
#define IPV4_BYTE(val, index)           ((val >> (index * 8)) & 0xFF)  // IP address parsing

// State macros
#define SetAppState(state)      g_appState = state
#define GetAppState()           g_appState

typedef enum {
    NOT_READY = 0, /*!< Not ready. */
    STORAGE_READY = 0x01, /*!< Storage is ready. */
    WIFI_CONNECTED = 0x02, /*!< Wi-Fi is connected. */
    GET_REQUESTED = 0x04, /*!< GET request is sent. */
    DOWNLOADING = 0x08, /*!< Running to download. */
    COMPLETED = 0x10, /*!< Download completed. */
    CANCELED = 0x20 /*!< Download canceled. */
} download_state;

// application states
typedef enum
{
    APP_STATE_WAIT_FOR_DRIVER_INIT,
    APP_STATE_START,
    APP_STATE_WORKING,  
    APP_STATE_DONE
} t_AppState;

t_AppState g_appState = APP_STATE_WAIT_FOR_DRIVER_INIT;
static download_state down_state = NOT_READY;       // File download processing state
static uint32_t http_file_size = 0;                 // Http content length
static uint32_t received_file_size = 0;             // Receiving content length
struct sw_timer_module swt_module_inst;             // Instance of Timer module
struct http_client_module http_client_module_inst;  // Instance of HTTP client module

static void configure_http_client(void);
static void init_state(void);
static inline bool is_state_set(download_state mask);
static void wifi_cb(uint8_t msgType, void *pvMsg);
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg);
static void resolve_cb(char *pu8DomainName, uint32_t serverIP);

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
        printf("HTTP File Download Demo\r\n");
        printf("ssid: %s\r\n", WLAN_SSID);
        printf("file to be downloaded: %s\r\n",HTTP_FILE_URL);
        printf("=========\r\n");
        registerWifiCallback(wifi_cb);
        registerSocketCallback(socket_cb, resolve_cb);

        init_state();
        
        /* Initialize the HTTP client service. */
        configure_http_client();

        /* Connect to router. */
        printf("main: connecting to WiFi AP %s...\r\n", (char *)WLAN_SSID);
        m2m_wifi_connect((char *)WLAN_SSID, strlen(WLAN_SSID), WLAN_AUTH, (char *)WLAN_PSK, M2M_WIFI_CH_ALL);
        SetAppState(APP_STATE_WORKING);
        break;
      
    case APP_STATE_WORKING:       
        if (!(is_state_set(COMPLETED) || is_state_set(CANCELED))) 
        {
        /* Checks the timer timeout. */
        //sw_timer_task(&swt_module_inst);
        } 
        else 
        {
            printf("main: done.\r\n");
            SetAppState(APP_STATE_DONE);
            break;
        }
        break;
        
    case APP_STATE_DONE:
        
        break;
        
    default:
        break;
    }
}



/**
 * \brief Initialize download state to not ready.
 */
static void init_state(void)
{
    down_state = NOT_READY;
}

/**
 * \brief Clear state parameter at download processing state.
 * \param[in] mask Check download_state.
 */
static void clear_state(download_state mask)
{
    down_state &= ~mask;
}

/**
 * \brief Add state parameter at download processing state.
 * \param[in] mask Check download_state.
 */
static void add_state(download_state mask)
{
    down_state |= mask;
}

/**
 * \brief File download processing state check.
 * \param[in] mask Check download_state.
 * \return true if this state is set, false otherwise.
 */

static inline bool is_state_set(download_state mask)
{
    return ((down_state & mask) != 0);
}

/**
 * \brief Start file download via HTTP connection.
 */
static void start_download(void)
{
    if (!is_state_set(WIFI_CONNECTED)) 
    {
        printf("start_download: Wi-Fi is not connected.\r\n");
        return;
    }

    if (is_state_set(GET_REQUESTED)) 
    {
        printf("start_download: request is sent already.\r\n");
        return;
    }

    if (is_state_set(DOWNLOADING)) 
    {
        printf("start_download: running download already.\r\n");
        return;
    }

    /* Send the HTTP request. */
    printf("start_download: sending HTTP request...\r\n");
    http_client_send_request(&http_client_module_inst, HTTP_FILE_URL, HTTP_METHOD_GET, NULL, NULL);
}

/**
 * \brief Store received packet to file.
 * \param[in] data Packet data.
 * \param[in] length Packet data length.
 */
static void store_file_packet(char *data, uint32_t length)
{
    //FRESULT ret;
    if ((data == NULL) || (length < 1)) 
    {
        printf("store_file_packet: empty data.\r\n");
        return;
    }

    if (!is_state_set(DOWNLOADING)) 
    {
        received_file_size = 0;
        add_state(DOWNLOADING);
    }

    if (data != NULL) 
    {
        unsigned int wsize = 0;
        {
            int i;
            printf("Download:[");
            for(i=0;i<length;i++)
                printf("%02x", data[i]);
            printf("]\r\n");
        }

        received_file_size += wsize;
        printf("store_file_packet: received[%lu], file size[%lu]\r\n", (unsigned long)received_file_size, (unsigned long)http_file_size);
        if (received_file_size >= http_file_size) 
        {
            printf("store_file_packet: file downloaded successfully.\r\n");
            add_state(COMPLETED);
            return;
        }
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
    switch (type) 
    {
    case HTTP_CLIENT_CALLBACK_SOCK_CONNECTED:
        printf("http_client_callback: HTTP client socket connected.\r\n");
        break;

    case HTTP_CLIENT_CALLBACK_REQUESTED:
        printf("http_client_callback: request completed.\r\n");
        add_state(GET_REQUESTED);
        break;

    case HTTP_CLIENT_CALLBACK_RECV_RESPONSE:
        printf("http_client_callback: received response %u data size %u\r\n",
                (unsigned int)data->recv_response.response_code,
                (unsigned int)data->recv_response.content_length);
        if ((unsigned int)data->recv_response.response_code == 200) 
        {
            http_file_size = data->recv_response.content_length;
            received_file_size = 0;
        } 
        else
        {
            add_state(CANCELED);
            return;
        }
        if (data->recv_response.content_length <= BUFFER_MAX_SIZE) 
        {
            store_file_packet(data->recv_response.content, data->recv_response.content_length);
            add_state(COMPLETED);
        }
        break;

    case HTTP_CLIENT_CALLBACK_RECV_CHUNKED_DATA:
        store_file_packet(data->recv_chunked_data.data, data->recv_chunked_data.length);
        if (data->recv_chunked_data.is_complete) 
        {
            add_state(COMPLETED);
        }

        break;

    case HTTP_CLIENT_CALLBACK_DISCONNECTED:
        printf("http_client_callback: disconnection reason:%d\r\n", data->disconnected.reason);

        /* If disconnect reason is equal to -ECONNRESET(-104),
         * It means the server has closed the connection (timeout).
         * This is normal operation.
         */
        if (data->disconnected.reason == -EAGAIN) 
        {
            /* Server has not responded. Retry immediately. */
            if (is_state_set(DOWNLOADING)) 
            {
                clear_state(DOWNLOADING);
            }

            if (is_state_set(GET_REQUESTED)) 
            {
                clear_state(GET_REQUESTED);
            }
#if !defined(__XC8)  // PIC18 cannot support recursive calling
            start_download();
#endif
        }

        break;
    }
}

/**
 * \brief Callback to get the data from socket.
 *
 * \param[in] sock socket handler.
 * \param[in] message socket event type. Possible values are:
 *  - M2M_SOCKET_BIND_EVENT
 *  - M2M_SOCKET_LISTEN_EVENT
 *  - M2M_SOCKET_ACCEPT_EVENT
 *  - M2M_SOCKET_CONNECT_EVENT
 *  - M2M_SOCKET_RECV_EVENT
 *  - M2M_SOCKET_SEND_EVENT
 *  - M2M_SOCKET_SENDTO_EVENT
 *  - M2M_SOCKET_RECVFROM_EVENT
 * \param[in] pvMsg is a pointer to message structure. Existing types are:
 *  - tstrSocketBindMsg
 *  - tstrSocketListenMsg
 *  - tstrSocketAcceptMsg
 *  - t_socketConnect
 *  - t_socketRecv
 */
static void socket_cb(SOCKET sock, uint8_t message, void *pvMsg)
{
    http_client_socket_event_handler(sock, message, pvMsg);
}

/**
 * \brief Callback for the gethostbyname function (DNS Resolution callback).
 * \param[in] pu8DomainName Domain name of the host.
 * \param[in] serverIP Server IPv4 address encoded in NW byte order format. If it is Zero, then the DNS resolution failed.
 */
static void resolve_cb(char *pu8DomainName, uint32_t serverIP)
{
    printf("%s IP address is %d.%d.%d.%d\r\n\r\n", pu8DomainName,
            (int)IPV4_BYTE(serverIP, 0), (int)IPV4_BYTE(serverIP, 1),
            (int)IPV4_BYTE(serverIP, 2), (int)IPV4_BYTE(serverIP, 3));
    http_client_socket_resolve_handler((uint8_t *)pu8DomainName, serverIP);
}

static void wifi_cb(uint8_t msgType, void *pvMsg)
{
    switch (msgType) 
    {
    case M2M_WIFI_CONN_STATE_CHANGED_EVENT:
    {
        tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
        if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) 
        {
            printf("M2M_WIFI_CONNECTED\r\n");
        } 
        else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) 
        {
            printf("M2M_WIFI_DISCONNECTED\r\n");
            clear_state(WIFI_CONNECTED);
            if (is_state_set(DOWNLOADING)) 
            {
                clear_state(DOWNLOADING);
            }

            if (is_state_set(GET_REQUESTED)) 
            {
                clear_state(GET_REQUESTED);
            }

            m2m_wifi_connect((char *)WLAN_SSID, 
                             strlen(WLAN_SSID),
                             WLAN_AUTH, 
                             (char *)WLAN_PSK, 
                             M2M_WIFI_CH_ALL);
        }

        break;
    }

        case M2M_WIFI_IP_ADDRESS_ASSIGNED_EVENT:
    {
        uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
        printf("IP address is %u.%u.%u.%u\r\n",
                pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
        add_state(WIFI_CONNECTED);
        start_download();
        break;
    }

    default:
        break;
    }
}

/**
 * \brief Configure HTTP client module.
 */
static void configure_http_client(void)
{
    struct http_client_config httpc_conf;
    int ret;

    http_client_get_config_defaults(&httpc_conf);

    httpc_conf.recv_buffer_size = BUFFER_MAX_SIZE;
    httpc_conf.timer_inst = &swt_module_inst;

    ret = http_client_init(&http_client_module_inst, &httpc_conf);
    if (ret < 0) 
    {
        printf("configure_http_client: HTTP client initialization failed! (res %d)\r\n", ret);
        while (1) 
        {
        } /* Loop forever. */
    }

    http_client_register_callback(&http_client_module_inst, http_client_callback);
}

#if defined(__XC8) // PIC18 cannot support recursive calling
void m2m_socket_handle_events_Pic18WaiteHttpSend(SOCKET sock, t_m2mSocketEventType eventCode, t_socketEventData *p_eventData)
{
    http_client_socket_event_handler_Pic18WaiteHttpSend(sock, eventCode, p_eventData);
}
#endif

#endif
