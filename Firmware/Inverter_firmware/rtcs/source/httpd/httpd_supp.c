/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved                       
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: httpd_supp.c$
* $Version : 3.8.28.0$
* $Date    : Aug-31-2012$
*
* Comments:
*
*   HTTPD support functions.
*
*END************************************************************************/

#include "httpd.h"
#include <string.h>

/** Send data to socket.
 * \param s < socket
 * \param buf < pointer to data buffer
 * \param len < data length
 * \return length of sent data
 */
static _mqx_int httpd_send(int s, char *buf, int len) {
    int res;

    HTTPD_DEBUG(4, "httpd_send buf:%p len:%d", buf, len);

#if RTCS_MINIMUM_FOOTPRINT
    // for better performance on small ram devices, split data to teo separate packets
    if (len > 2) {
        res = send(s, buf, len / 2, 0);
        
        if (res != RTCS_ERROR)
            res += send(s, buf + res, len - res, 0);
    }
    else
#endif
        res = send(s, buf, len, 0);

    HTTPD_DEBUG(4, "result:%d\n", res);

    return res;
}

_mqx_int httpd_sendstr(_mqx_int s, char *str) {
    return httpd_send(s, str, strlen(str));
}

/** Send extended data to socket.
 * \param s < socket
 * \param buf < pointer to data buffer
 * \param len < data length
 * \return length of sent data
 */
static _mqx_int httpd_sendext(_mqx_int s, char *buf, _mqx_int len) {
    (void)s;
    (void)buf;
    (void)len;

    return 0;
}

/**
 */
static _mqx_int httpd_call_fn(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session, char *fn_name) {
    HTTPD_FN_LINK_STRUCT *ptr = server->params->fn_lnk_tbl;

    if (ptr) {
        while (*(ptr->callback)) {
            if (0 == strcmp(fn_name, ptr->fn_name)) {
                ptr->callback(session);
                break;
            }

            ptr++;
        }
    }

    return 0;
}

_mqx_int httpd_cgi(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session, char *cgi_name) {
    HTTPD_CGI_LINK_STRUCT *ptr = server->params->cgi_lnk_tbl;
    int res = -1;

    if (ptr) {
        while (*(ptr->callback)) {
            if (0 == strcmp(cgi_name, ptr->cgi_name)) {
                res = ptr->callback(session);
                break;
            }

            ptr++;
        }
    }

    return res;
}

/** Send extended string to socket.
 * \param s < socket
 * \param str < pointer to string
 * \return length of sent data
 */
static _mqx_int httpd_sendextstr(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session, char *str) {
    char *src;
    int len, res;
    char fname[HTTPDCFG_MAX_SCRIPT_LN + 1];

    HTTPD_ASSERT(server && session && str);

    fname[0] = 0;

    if (!session->response.data)
    {
        session->response.data = str;
    }

    src = session->response.data;

    if (session->response.script_token)
    {
        // script token found
        len = (int)strcspn(src, " ;%<>\r\n\t");

        if (len > 1 && len < HTTPDCFG_MAX_SCRIPT_LN)
        {
            strncpy(fname, src, (unsigned long)len);
            fname[len] = 0;

            // call fn
            HTTPD_DEBUG(2, "script ln: %s\r\n", fname);

            httpd_call_fn(server, session, fname);
        }

        if (src[len] == '%' && src[len + 1] == '>')
        {
            session->response.script_token = 0;
            len += 1;
        }

        len++;

        session->response.data = src + len;
    }
    else
    {
        for (len = 0; *src && len < HTTPDCFG_MAX_BYTES_TO_SEND; src++, len++)
        {
            if (*src == '<' && *(src + 1) == '%')
            {
                session->response.script_token = 1;
                src += 2;

                break;
            }
        }

        res = httpd_send(session->sock, session->response.data, len);

        if (res < 0) {
            session->response.script_token = 0;
            res = errno;

            if (res != EAGAIN)
                len = 0;
        }
        else {
            if (len == res) {
                session->response.data = src;
            }
            else {
                session->response.script_token = 0;
                session->response.data += res;
            }
        }
    }

    return len;
}

#if HTTPDCFG_POLL_MODE
static _mqx_int httpd_readch(HTTPD_SESSION_STRUCT *session, char *ch) {
    int res;
    
    HTTPD_ASSERT(session && ch);
    HTTPD_ASSERT(session->recv_used >= 0 && session->recv_used < HTTPDCFG_RECV_BUF_LEN);

    if (session->recv_used == 0) {
        HTTPD_DEBUG(5, "recv BEGIN\n");
        res = recv(session->sock, session->recv_buf, HTTPDCFG_RECV_BUF_LEN, 0);
        HTTPD_DEBUG(5, "recv END\n");

        if (res <= 0) {
            *ch = 0;
            return res;
        }
        
        session->recv_used = res;
        session->recv_buf[session->recv_used] = 0;
        session->recv_rd = session->recv_buf;
    }

    *ch = *session->recv_rd++;
    session->recv_used--;

    return 1;
}
#endif

_mqx_int httpd_readln(HTTPD_SESSION_STRUCT *session, char *buf, _mqx_int max_len) {
    int used = 0, ret;
    char *dst = buf;
    uint_32 err;

    HTTPD_ASSERT(session && dst);
    
    HTTPD_DEBUG(5, "readln session(%p) buf(%p) max_len(%d)\n", session, buf, max_len);

    while (used < max_len) {
#if HTTPDCFG_POLL_MODE
        ret = httpd_readch(session, dst);
#else
        ret = recv(session->sock, dst, 1, 0);
#endif

        if (ret <= 0) {
            err = RTCS_geterror(session->sock);
            *buf = 0;
            HTTPD_DEBUG(5, "readln used(%d) ret(%d) err(0x%x)\n", used, ret, err);
            return ret;
        }

        dst++;
        used++;

        if (*(dst - 1) == '\n') {
            break;
        }
    }

    *dst = 0;
    
    HTTPD_DEBUG(5, "readln len(%d)\n", used);
    return used;
}

_mqx_int httpd_read(HTTPD_SESSION_STRUCT *session, char *dst, _mqx_int len) {
    int cnt = 0;

#if HTTPDCFG_POLL_MODE        
    while (len--) {
        if (!httpd_readch(session, dst++))
            break;

        cnt++;
    };
#else
    cnt = recv(session->sock, dst, len, 0);
#endif    

    return cnt;
}

/** string table item
 */
typedef struct {
    int id;
    char *str;
} tbl_str_itm_t;

/**
 */
static const tbl_str_itm_t reason_phrase[] = {
        { 200,  "OK" },
        { 404,  "Not Found" },
        { 0,    "" }
};

/** content type
 */
static const tbl_str_itm_t content_type[] = {
        { CONTENT_TYPE_PLAIN, "text/plain" },
        { CONTENT_TYPE_HTML, "text/html" },
        { CONTENT_TYPE_CSS, "text/css" },
        { CONTENT_TYPE_GIF, "image/gif" },
        { CONTENT_TYPE_JPG, "image/jpeg" },
        { CONTENT_TYPE_PNG, "image/png" },
        { CONTENT_TYPE_JS, "application/javascript"},
        { CONTENT_TYPE_OCTETSTREAM, "application/octet-stream" },
        { 0,    0 }
};

/** Get string from table by id
 * \param tbl < pointer to string table
 * \param id < string id
 */
static char* get_tbl_string(tbl_str_itm_t *tbl, const _mqx_int id) {
    tbl_str_itm_t *ptr = tbl;

    while (ptr->str) {
        if (id == ptr->id)
            break;

        ptr++;
    }

    return ptr->str;
}

void httpd_sendhdr(HTTPD_SESSION_STRUCT *session, _mqx_int content_len, _mqx_int mod_time) {
    char tmpBuf[100];

    HTTPD_ASSERT(session);

    if (session->response.hdrsent)
        return;

    sprintf(tmpBuf, "HTTP/1.0 %d %s\n",
            session->response.statuscode,
            get_tbl_string((tbl_str_itm_t*)reason_phrase, session->response.statuscode));
    httpd_sendstr(session->sock, tmpBuf);
    httpd_sendstr(session->sock, session->response.header);

    if (session->keep_alive) {
        httpd_sendstr(session->sock, "Connection: Keep-Alive\n");
    }
    else {
        httpd_sendstr(session->sock, "Connection: close\n");
    }

    sprintf(tmpBuf, "Content-Type: %s\n", get_tbl_string((tbl_str_itm_t*)content_type, session->response.contenttype));
    httpd_sendstr(session->sock, tmpBuf);

    //only non zero length cause to send Content-Length header field
    if (0 < content_len) {
        sprintf(tmpBuf, "Content-Length: %d\n", content_len);
        httpd_sendstr(session->sock, tmpBuf);
    }

    httpd_sendstr(session->sock, "\n");

    session->response.hdrsent = 1;

    HTTPD_DEBUG(4, "httpd_sendhdr\n");
}

void httpd_sendfile(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    char *ext;
    int expand = 0;
    int len;
    char buf[HTTPDCFG_MAX_BYTES_TO_SEND + 1];

    HTTPD_ASSERT(server && session);

    HTTPD_DEBUG(3, "httpd_sendfile(%p) %s\n", session, session->request.path);

    ext = strrchr(session->request.path, 46);
    if (ext) {
        if (0 == strcasecmp(ext, ".htm") || 0 == strcasecmp(ext, ".html")) {
            session->response.contenttype = CONTENT_TYPE_HTML;
            expand = 1;
        }
        else if (0 == strcasecmp(ext, ".gif"))
            session->response.contenttype = CONTENT_TYPE_GIF;
        else if (0 == strcasecmp(ext, ".jpg"))
            session->response.contenttype = CONTENT_TYPE_JPG;
        else if (0 == strcasecmp(ext, ".png"))
            session->response.contenttype = CONTENT_TYPE_PNG;
        else if (0 == strcasecmp(ext, ".css"))
            session->response.contenttype = CONTENT_TYPE_CSS;
        else if (0 == strcasecmp(ext, ".js"))
            session->response.contenttype = CONTENT_TYPE_JS;
        else
            session->response.contenttype = CONTENT_TYPE_OCTETSTREAM;
    }

    if (expand) {
        //if there will be any expansion there's no way how to calculate correct length
        //zero length prevents sending Content-Length header field
        httpd_sendhdr(session, 0, 0);
    } else {
        httpd_sendhdr(session, session->response.file->SIZE, 0);
    }

    if (expand) {
        session->response.data = buf;
        len = read(session->response.file, session->response.data, HTTPDCFG_MAX_BYTES_TO_SEND);

        if (len > 0) {
            buf[len] = 0;
			len = httpd_sendextstr(server, session, session->response.data);
            if (!len) {
                session->state = HTTPD_SES_END_REQ;
            }
            else {
                session->response.len += len;
                fseek(session->response.file, session->response.len, SEEK_SET);
            }
        }
        else
            session->state = HTTPD_SES_END_REQ;
    }
    else {
        session->response.data = buf;
        fseek(session->response.file, session->response.len, SEEK_SET);
        len = read(session->response.file, session->response.data, HTTPDCFG_MAX_BYTES_TO_SEND);
        
        HTTPD_DEBUG(4, "httpd_sendfile READ END %d\n", len);
        
        if (len > 0) {
            buf[len] = 0;

            len = httpd_send(session->sock, session->response.data, len);

            if (len < 0) {
                if (errno != EAGAIN)
                    session->state = HTTPD_SES_END_REQ;
            }
            else {
                session->response.len += len;
            }
        }
        else
            session->state = HTTPD_SES_END_REQ;
    }
    HTTPD_DEBUG(4, "httpd_sendfile END %s \n", session->request.path);
}

void httpd_send401(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session)
{
    HTTPD_DEBUG(3, "http_send401\n");

    session->response.file = fopen(server->params->page401, "r");
    session->response.len = 0;

    if (session->response.file) {
        httpd_sendfile(server, session);
    }
    else {
        httpd_sendhdr(session, 0, 0);
        httpd_sendstr(session->sock, "<HTML><HEAD><TITLE>401 Unauthorized</TITLE></HEAD>\n");
        httpd_sendstr(session->sock, "<BODY><H1>Unauthorized!</H1>\n");
        httpd_sendstr(session->sock, "</BODY></HTML>\n");
    }
}

void httpd_send403(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    HTTPD_DEBUG(3, "http_send403\n");

    session->response.file = fopen(server->params->page403, "r");
    session->response.len = 0;

    if (session->response.file) {
        httpd_sendfile(server, session);
    }
    else {
        httpd_sendhdr(session, 0, 0);
        httpd_sendstr(session->sock, "<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\n");
        httpd_sendstr(session->sock, "<BODY><H1>Forbidden!</H1>\n");
        httpd_sendstr(session->sock, "</BODY></HTML>\n");
    }
}

void httpd_send404(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    HTTPD_DEBUG(3, "http_send404\n");

    session->response.file = fopen(server->params->page404, "r");
    session->response.len = 0;

    if (session->response.file) {
        httpd_sendfile(server, session);
    }
    else {
        httpd_sendhdr(session, 0, 0);
        httpd_sendstr(session->sock, "<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\n");
        httpd_sendstr(session->sock, "<BODY><H1>The request URL was not found!</H1>\n");
        httpd_sendstr(session->sock, "</BODY></HTML>\n");
    }
}

/** base64 decoding alphabet
 */
static const char decode_base64[] = "n###odefghijklm#######0123456789:;<=>?@ABCDEFGHI######JKLMNOPQRSTUVWXYZ[\\]^_`abc";

#define DECODE_BASE64_IDX(ch)       (decode_base64[ch - 43] - '0')

/** Decode base64 single block (4 char string).
 * \param in < 4 char input buffer
 * \param out < 3 char output buffer
 */
static void httpd_decode_b64_block(char in[4], char out[3]) {
    out[0] = (unsigned char)(in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char)(in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char)(((in[2] << 6) & 0xc0) | in[3]);
}


_mqx_int httpd_decode_base64(char *dst, char *src, _mqx_int dst_size) {
    char in[4];
    int i, len, free = dst_size;

    while (*src && free) {
        for (i = 0, len = 0; i < 4; i++) {
            if (*src) {
                if (!(*src < 43 || *src > 122) && *src != '=') {
                    len++;
                    in[i] = (unsigned char)DECODE_BASE64_IDX(*src);
                }
                else
                    in[i] = 0;

                src++;
            }
            else {
                in[i] = 0;
            }
        }

        if (len > 1 && free > len - 1) {
            httpd_decode_b64_block(in, dst);
            dst += len - 1;
            *dst = 0;
            free -= (len - 1);
        }
        else
            break;
    }

    return 0;
}

_mqx_int httpd_get_varval(HTTPD_SESSION_STRUCT *session, char *var_str, char *var_name, char *var_val, _mqx_int var_val_len) {
    char *var = var_str;    //session->request.urldata;
    int res = 0;
    int idx;

    HTTPD_ASSERT(session && var_str && var_name && var_val);

    var_val[0] = 0;

    while ((var = strstr(var, var_name)) != 0) {
        if (*(var + strlen(var_name)) == '=') {
            var += strlen(var_name) + 1;    // +1 bacause '='

            idx = (int)strcspn(var, "&");
            strncpy(var_val, var, (unsigned long)idx);
            var_val[idx] = 0;
            res = 1;
            break;
        }
        else {
            var = strchr(var, '&');
        }
    }

    return res;
}
