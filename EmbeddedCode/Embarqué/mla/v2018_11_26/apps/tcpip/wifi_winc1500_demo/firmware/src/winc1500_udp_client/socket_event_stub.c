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

#if defined(USING_UDP_CLIENT) 
static bool g_bindOccurred;
static bool g_listenOccurred;
static bool g_acceptOccurred;
static bool g_connectOccurred;
static bool g_dnsResolved;
static bool g_recv;
static bool g_recvFrom;
static bool g_sendTo;
static bool g_send;
static bool g_pingReply;


void m2m_socket_handle_events(SOCKET sock, t_m2mSocketEventType eventCode, t_socketEventData *p_eventData)
{
    switch (eventCode)
    {
        case M2M_SOCKET_BIND_EVENT:
            // event data in p_eventData->bindStatus 
            g_bindOccurred = true;
            break;
            
        case M2M_SOCKET_LISTEN_EVENT:
            // event data in p_eventData->listenStatus
            g_listenOccurred = true;
            break;
            
        case M2M_SOCKET_ACCEPT_EVENT:
            // event data in p_eventData->acceptResponse
            g_acceptOccurred = true;
            break;
            
        case M2M_SOCKET_CONNECT_EVENT:
            // event data in p_eventData->connectResponse;
            g_connectOccurred = true;
            break;
            
        case M2M_SOCKET_DNS_RESOLVE_EVENT:
            // sock parameter not valid for this event
            // event data in p_eventData->dnsReply
            g_dnsResolved = true;
            break;
            
        case M2M_SOCKET_RECV_EVENT:
            // event data is in p_eventData->recvMsg
            g_recv = true;
            break;
            
        case M2M_SOCKET_RECVFROM_EVENT:
            // event data is in p_eventData->recvMsg  
            g_recvFrom = true;
            break;

        case M2M_SOCKET_SEND_EVENT:
            // event data is in p_eventData->numSendBytes;            
            g_send = true;
            break;
            
        case M2M_SOCKET_SENDTO_EVENT:
            // event data is in p_eventData->numSendBytes;
            g_sendTo = true;
            break;
            
        case M2M_SOCKET_PING_RESPONSE_EVENT:
            // sock parameter not valid for this event
            // event data is in p_eventData->pingReply
            g_pingReply = true;
            break;
    }
}

void ClearSocketEventStates(void)
{
    g_bindOccurred    = false;
    g_listenOccurred  = false;
    g_acceptOccurred  = false;
    g_connectOccurred = false;
    g_dnsResolved     = false;
    g_recv            = false;
    g_recvFrom        = false;
    g_pingReply       = false;
}

bool isSocketBindOccurred(void)
{
    bool res = g_bindOccurred;
    g_bindOccurred = false;
    return res; 
}

bool isSocketListenOccurred(void)
{
    bool res = g_listenOccurred;
    g_listenOccurred = false;
    return res;
}

bool isSocketAcceptOccurred(void)
{
    bool res = g_acceptOccurred;
    g_acceptOccurred = false;
    return res;
}

bool isSocketConnectOccurred(void)
{
    bool res = g_connectOccurred;
    g_connectOccurred = false;
    return res;
}

bool isDnsResolved(void)
{
    bool res = g_dnsResolved;
    g_dnsResolved = false;
    return res;
}

bool isSocketRecvOccurred(void)
{
    bool res = g_recv;
    g_recv = false;
    return res;
}

bool isSocketRecvFromOccurred(void)
{
    bool res = g_recvFrom;
    g_recvFrom = false;
    return res;
}

bool isSocketSendOccurred(void)
{
    bool res = g_send;
    g_send = false;
    return res;
}

bool isSocketSendToOccurred(void)
{
    bool res = g_sendTo;
    g_sendTo = false;
    return res;
}

bool isPingReplyOccurred(void)
{
    bool res = g_pingReply;
    g_pingReply = false;
    return res;
}


#endif // USING_UDP_CLIENT