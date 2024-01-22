/*******************************************************************************
  File Name:
    PubNub.c

  Summary:
    PubNub Example.

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

#include "winc1500_api.h"
#include "demo_config.h"
#include "wf_common.h"

#if defined(USING_PUBNUB)
#include "PubNub.h"

#define assert assert_PubNub

static struct pubnub m_aCtx[PUBNUB_CTX_MAX];
struct sockaddr_in pubnub_origin_addr;
static void assert_PubNub(bool status)
{
    if (status == false)
    {
        printf("Assert happen!\r\n");
        while (1)
            ;
    }
}
static char buf[PUBNUB_BUF_MAXLEN + sizeof(PUBNUB_REQUEST) + sizeof(PUBNUB_ORIGIN)] = { 0, }; //move out of function for compile issue in pic18 project
static void handle_transaction(pubnub_t *pb)
{
    if (pb->state == PS_WAIT_SEND)
    {
        //char buf[PUBNUB_BUF_MAXLEN + sizeof(PUBNUB_REQUEST) + sizeof(PUBNUB_ORIGIN)] = { 0, };
        sprintf( buf, PUBNUB_REQUEST, pb->http_buf.url, PUBNUB_ORIGIN );
        send(pb->tcp_socket, buf, strlen(buf), 0);
        PUBNUB_PRINTF(("handle_transaction: buf = %s", buf));
    }
    else if (pb->state == PS_WAIT_RECV)
    {
        PUBNUB_PRINTF(("handle_transaction: wait recv\r\n"));
        recv(pb->tcp_socket, pb->http_buf.url, PUBNUB_BUF_MAXLEN, 30 * 1000);
    }
    else if (pb->state == PS_RECV)
    {
    }
}

static bool valid_ctx_prt(pubnub_t const *pb)
{
    return ((pb >= m_aCtx) && (pb < m_aCtx + PUBNUB_CTX_MAX));
}

static pubnub_t *pubnub_find_ctx(SOCKET sock, enum pubnub_state state)
{
    pubnub_t *pb;

    for (pb = m_aCtx; pb != m_aCtx + PUBNUB_CTX_MAX; ++pb)
    {
        if (pb->state == state && pb->tcp_socket == sock) 
        {
            return pb;
        }
    }

    return NULL;
}

/** Handles start of a TCP(HTTP) connection. */
static void handle_start_connect(pubnub_t *pb)
{
    assert(valid_ctx_prt(pb));
    assert((pb->state == PS_IDLE) || (pb->state == PS_WAIT_DNS) || (pb->state == PS_WAIT_CONNECT));

    if (pb->state == PS_IDLE && pb->tcp_socket <= 0)
    {
        if ((pb->tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("failed to create TCP client socket error!\r\n");
            return;
        }

        if (pubnub_origin_addr.sin_addr.s_addr <= 0)
        {
            pubnub_origin_addr.sin_family = AF_INET;
            pubnub_origin_addr.sin_port = _htons(PUBNUB_ORIGIN_PORT);

            pb->state = PS_WAIT_DNS;
            printf("begin DNS\r\n");
            gethostbyname((const char *)PUBNUB_ORIGIN);
            return;
        }
    }

    connect(pb->tcp_socket, (struct sockaddr *)&pubnub_origin_addr, sizeof(struct sockaddr_in));
    pb->state = PS_WAIT_CONNECT;
}

/* Find the beginning of a JSON string that comes after comma and ends
 * at @c &buf[len].
 * @return position (index) of the found start or -1 on error. */
static int find_string_start(char const *buf, int len)
{
    int i;
    for (i = len - 1; i > 0; i--)
    {
        if (buf[i] == '"')
        {
            return (buf[i - 1] == ',') ? i : -1;
        }
    }
    return -1;
}

/** Split @p buf string containing a JSON array (with arbitrary
 * contents) to multiple NUL-terminated C strings, in-place.
 */
static bool split_array(char *buf)
{
    bool escaped = false;
    bool in_string = false;
    int bracket_level = 0;

    for ( ; *buf != '\0'; ++buf)
    {
        if (escaped)
        {
            escaped = false;
        } 
        else if ('"' == *buf)
        {
            in_string = !in_string;
        }
        else if (in_string)
        {
            escaped = ('\\' == *buf);
        } 
        else
        {
            switch (*buf) {
            case '[':
            case '{':
                bracket_level++; break;

            case ']':
            case '}':
                bracket_level--; break;

            /* if at root, split! */
            case ',':
                if (bracket_level == 0)
                {
                    *buf = '\0';
                }

                break;

            default:
                break;
            }
        }
    }

    return !(escaped || in_string || (bracket_level > 0));
}

static int parse_subscribe_response(pubnub_t *p)
{
    char *reply = p->http_reply;
    unsigned int replylen = strlen(reply);
    if (reply[replylen - 1] != ']' && replylen > 2)
    {
        replylen -= 2; /* XXX: this seems required by Manxiang */
    }

    if ((reply[0] != '[') || (reply[replylen - 1] != ']') || (reply[replylen - 2] != '"'))
    {
        return -1;
    }

    /* Extract the last argument. */
    int i = find_string_start(reply, replylen - 2);
    if (i < 0)
    {
        return -1;
    }

    reply[replylen - 2] = 0;

    /* Now, the last argument may either be a timetoken or a channel list. */
    if (reply[i - 2] == '"')
    {
        int k;

        /* It is a channel list, there is another string argument in front
         * of us. Process the channel list ... */
        p->chan_ofs = i + 1;
        p->chan_end = replylen - 1;
        for (k = p->chan_end - 1; k > p->chan_ofs; --k)
        {
            if (reply[k] == ',')
            {
                reply[k] = 0;
            }
        }

        /* ... and look for timetoken again. */
        reply[i - 2] = 0;
        i = find_string_start(reply, i - 2);
        if (i < 0) 
        {
            return -1;
        }
    } 
    else 
    {
        p->chan_ofs = 0;
        p->chan_end = 0;
    }

    /* Now, i points at
     * [[1,2,3],"5678"]
     * [[1,2,3],"5678","a,b,c"]
     *          ^-- here */

    /* Setup timetoken. */
    if (replylen >= sizeof(p->timetoken) + 2 + (i + 1))
    {
        return -1;
    }

    strcpy(p->timetoken, reply + i + 1);
    reply[i - 2] = 0; /* terminate the [] message array (before the ]!) */

    /* Set up the message list - offset, length and NUL-characters splitting the messages. */
    p->msg_ofs = 2;
    p->msg_end = i - 2;

    return split_array(reply + p->msg_ofs) ? 0 : -1;
}

static void handle_tcpip_connect(SOCKET sock, uint8_t message, void *pvMsg)
{
    pubnub_t *pb = pubnub_find_ctx(sock, PS_WAIT_CONNECT);

    if (pb != NULL)
    {
        t_socketConnect *pstrConnect = (t_socketConnect *)pvMsg;
        if (pstrConnect && pstrConnect->error >= 0)
        {
            PUBNUB_PRINTF("handle_tcpip_connect : connect success!\r\n");
            pb->state = PS_WAIT_SEND;

            handle_transaction(pb);
        }
        else
        {
            PUBNUB_PRINTF("handle_tcpip_connect : connect error!\r\n");
            close(pb->tcp_socket);

            pb->state = PS_IDLE;
            pb->last_result = PNR_IO_ERROR;
        }
    }
}

static void handle_tcpip_recv(SOCKET sock, uint8_t message, void *pvMsg)
{
    pubnub_t *pb;

    for (pb = m_aCtx; pb != m_aCtx + PUBNUB_CTX_MAX; ++pb)
    {
        if (pb->state == PS_WAIT_RECV && pb->tcp_socket == sock)
        {
            break;
        }
    }

    if (pb != NULL)
    {
        t_socketRecv *pstrRecv = (t_socketRecv *)pvMsg;

        if (pstrRecv->bufSize <= 0)
        {
            close(pb->tcp_socket);

            pb->state = PS_IDLE;
            pb->last_result = PNR_IO_ERROR;
            return;
        }

        if (pb->trans == PBTT_PUBLISH)
        {
            PUBNUB_PRINTF(("handle_tcpip_recv: PBTT_PUBLISH msg: %s\n", pstrRecv->p_rxBuf));

            if (pstrRecv->remainingSize == 0)
            {
                pb->last_result = PNR_OK;
                pb->state = PS_IDLE;
            }

            return;
        }

        if (pstrRecv->remainingSize > 0) 
        {
            pb->state = PS_WAIT_RECV;

            uint8_t *length = (uint8_t *)strstr((const char *)(pstrRecv->p_rxBuf), (const char *)"Content-Length: ") + 16;
            pb->http_content_len = atoi((const char *)length);
            pb->http_content_remaining_len = pstrRecv->remainingSize;
            PUBNUB_PRINTF(("Content-Length = %d\r\n", (int)pb->http_content_len));

            uint8_t *content = (uint8_t *)strstr((const char *)(pstrRecv->p_rxBuf), (const char *)"[");
            memcpy(pb->http_reply, content, pb->http_content_len - pstrRecv->remainingSize);
            PUBNUB_PRINTF(("http_reply = %s\r\n", pb->http_reply));
        }
        else if (pstrRecv->remainingSize == 0)
        {
            PUBNUB_PRINTF(("http_content_remaining_len = %d\r\n", (int)pb->http_content_remaining_len));

            memcpy(pb->http_reply + (pb->http_content_len - pb->http_content_remaining_len), pstrRecv->p_rxBuf, pstrRecv->bufSize);
            PUBNUB_PRINTF(("http_reply = %s\r\n", pb->http_reply));

            parse_subscribe_response(pb);

            PUBNUB_PRINTF(("timetoken = %s\r\n", pb->timetoken));
            pb->last_result = PNR_OK;
            pb->state = PS_IDLE;
        }
    }
}

void handle_dns_found(char const *name, uint32_t hostip)
{
    pubnub_t *pb;

    if (0 != strcmp(name, PUBNUB_ORIGIN))
    {
        return;
    }

    pubnub_origin_addr.sin_addr.s_addr = hostip;

    for (pb = m_aCtx; pb != m_aCtx + PUBNUB_CTX_MAX; ++pb)
    {
        if (pb->state == PS_WAIT_DNS) 
        {
            handle_start_connect(pb);
        }
    }
}

void handle_tcpip(SOCKET sock, uint8_t message, void *pvMsg)
{
    switch (message) {
    case M2M_SOCKET_CONNECT_EVENT:
    {
        handle_tcpip_connect(sock, message, pvMsg);
    }
    break;

    case M2M_SOCKET_SEND_EVENT:
    {
        pubnub_t *pb = pubnub_find_ctx(sock, PS_WAIT_SEND);
        if (pb != NULL)
        {
            pb->state = PS_WAIT_RECV;
            handle_transaction(pb);
        }
    }
    break;

    case M2M_SOCKET_RECV_EVENT:
    {
        t_socketRecv *pstrRecv = (t_socketRecv *)pvMsg;
        if (pstrRecv && pstrRecv->bufSize > 0) 
        {
            PUBNUB_PRINTF(("handle_tcpip: recv success!\r\n"));
            PUBNUB_PRINTF(("handle_tcpip: msg length %d:\r\n", pstrRecv->bufSize));
            PUBNUB_PRINTF(("handle_tcpip: msg remaining length %d\r\n", pstrRecv->remainingSize));
            PUBNUB_PRINTF(("handle_tcpip: msg: %s\r\n", pstrRecv->p_rxBuf));
        } 
        else 
        {
            PUBNUB_PRINTF(("handle_tcpip: recv error!\r\n"));
        }

        pubnub_t *pb = pubnub_find_ctx(sock, PS_WAIT_RECV);

        if (pb != NULL)
        {
        }

        handle_tcpip_recv(sock, message, pvMsg);
    }
    break;

    default:
        break;
    }
}

pubnub_t *pubnub_get_ctx(uint8_t index)
{
    assert(index < PUBNUB_CTX_MAX);
    return m_aCtx + index;
}

void pubnub_init(pubnub_t *pb, const char *publish_key, const char *subscribe_key)
{
    assert(valid_ctx_prt(pb));

    pb->publish_key = publish_key;
    pb->subscribe_key = subscribe_key;
    pb->timetoken[0] = '0';
    pb->timetoken[1] = '\0';
    pb->uuid = pb->auth = NULL;
    pb->tcp_socket = -1;
    pb->state = PS_IDLE;
    pb->last_result = PNR_IO_ERROR;
}

bool pubnub_publish(pubnub_t *pb, const char *channel, const char *message)
{
    assert(valid_ctx_prt(pb));

    if (pb->state != PS_IDLE) 
    {
        return false;
    }

    pb->trans = PBTT_PUBLISH;
   
#if 0   // temporarily for compile issue in pic18 project. pic18 don't support snprintf
   pb->http_buf_len = snprintf(pb->http_buf.url, sizeof(pb->http_buf.url), "/publish/%s/%s/0/%s/0/", pb->publish_key, pb->subscribe_key, channel);
#else
    {
    uint16_t size_string = strlen("/publish/") + strlen(pb->publish_key) + strlen("/") 
        + strlen(pb->subscribe_key) + strlen("/0/")
        + strlen(channel) + strlen("/0/");

    if (size_string > sizeof(pb->http_buf.url) - 1)
    {
        sprintf(pb->http_buf.url,
            "/publish///0//0/");
        pb->http_buf_len = strlen("/publish///0//0/");
    }
    else
    {
        sprintf(pb->http_buf.url, "/publish/%s/%s/0/%s/0/", pb->publish_key, pb->subscribe_key, channel);
        pb->http_buf_len = size_string;
    }
    
    }
#endif
    const char *pmessage = message;

    while (pmessage[0]) 
    {
        /* RFC 3986 Unreserved characters plus few safe reserved ones. */
        size_t okspan = strspn(pmessage, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~" ",=:;@[]");
        if (okspan > 0)
        {
            if (okspan > sizeof(pb->http_buf.url) - 1 - pb->http_buf_len) 
            {
                PUBNUB_PRINTF("Not enough buffer! Reduce the message or modify the PUBNUB_BUF_MAXLEN\r\n");
                pb->http_buf_len = 0;
                return false;
            }

            memcpy(pb->http_buf.url + pb->http_buf_len, pmessage, okspan);
            pb->http_buf_len += okspan;
            pb->http_buf.url[pb->http_buf_len] = 0;
            pmessage += okspan;
        }

        if (pmessage[0])
        {
            /* %-encode a non-ok character. */
            char enc[4] = {'%', };
            enc[1] = "0123456789ABCDEF"[pmessage[0] / 16];
            enc[2] = "0123456789ABCDEF"[pmessage[0] % 16];
            if (3 > sizeof(pb->http_buf.url) - 1 - pb->http_buf_len) 
            {
                PUBNUB_PRINTF("Not enough buffer! Reduce the message or modify the PUBNUB_BUF_MAXLEN\r\n");
                pb->http_buf_len = 0;
                return false;
            }

            memcpy(pb->http_buf.url + pb->http_buf_len, enc, 4);
            pb->http_buf_len += 3;
            ++pmessage;
        }
    }

    if (pb->last_result == PNR_OK) 
    {
        pb->state = PS_WAIT_SEND;
        handle_transaction(pb);
    } 
    else
    {
        handle_start_connect(pb);
    }

    return true;
}

bool pubnub_subscribe(pubnub_t *pb, const char *channel)
{
    assert(valid_ctx_prt(pb));

    if (pb->state != PS_IDLE) 
    {
        return false;
    }

    pb->trans = PBTT_SUBSCRIBE;

    memset(pb->http_reply, 0x0, PUBNUB_REPLY_MAXLEN);

#if 0 // temporarily for compile issue in pic18 project. pic18 don't support snprintf
    pb->http_buf_len = snprintf(pb->http_buf.url, sizeof(pb->http_buf.url),
            "/subscribe/%s/%s/0/%s?" "%s%s" "%s%s%s" "&pnsdk=WINC1500%s%%2F%s",
            pb->subscribe_key, channel, pb->timetoken,
            pb->uuid ? "uuid=" : "", pb->uuid ? pb->uuid : "",
            pb->uuid && pb->auth ? "&" : "",
            pb->uuid && pb->auth ? "auth=" : "", pb->uuid && pb->auth ? pb->auth : "",
            "", "0.1");
#else
    {

    uint16_t size_string = strlen("/subscribe/") + strlen(pb->subscribe_key) + strlen("/")
        + strlen(channel) + strlen("/0/") + strlen(pb->timetoken) + strlen("?")
        + strlen(pb->uuid ? "uuid=" : "") + strlen(pb->uuid ? pb->uuid : "")
        + strlen(pb->uuid && pb->auth ? "&" : "") + strlen(pb->uuid && pb->auth ? "auth=" : "") 
        + strlen(pb->uuid && pb->auth ? pb->auth : "") + strlen("&pnsdk=WINC1500")
        + strlen("%%2F") + strlen("0.1");
    if (size_string > sizeof(pb->http_buf.url) - 1)
    {
        sprintf(pb->http_buf.url,
            "/subscribe///0/?" "" "" "&pnsdk=WINC1500%%2F");
        pb->http_buf_len = strlen("/subscribe///0/?" "" "" "&pnsdk=WINC1500%%2F");
    }
    else
    {
        sprintf(pb->http_buf.url,
            "/subscribe/%s/%s/0/%s?" "%s%s" "%s%s%s" "&pnsdk=WINC1500%s%%2F%s",
            pb->subscribe_key, channel, pb->timetoken,
            pb->uuid ? "uuid=" : "", pb->uuid ? pb->uuid : "",
            pb->uuid && pb->auth ? "&" : "",
            pb->uuid && pb->auth ? "auth=" : "", pb->uuid && pb->auth ? pb->auth : "",
            "", "0.1");
        pb->http_buf_len = size_string;
    }
    
    }
#endif
    if (pb->last_result == PNR_OK)
    {
        pb->state = PS_WAIT_SEND;
        handle_transaction(pb);
    }
    else 
    {
        handle_start_connect(pb);
    }

    return true;
}

char const *pubnub_get(pubnub_t *pb)
{
    assert(valid_ctx_prt(pb));

    if (pb->msg_ofs < pb->msg_end)
    {
        char const *rslt = pb->http_reply + pb->msg_ofs;
        pb->msg_ofs += strlen(rslt);

        if (pb->msg_ofs++ <= pb->msg_end)
        {
            return rslt;
        }
    }

    return NULL;
}

#endif