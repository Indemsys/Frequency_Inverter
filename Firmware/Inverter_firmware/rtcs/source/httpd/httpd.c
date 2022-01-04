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
* $FileName: httpd.c$
* $Version : 3.8.33.0$
* $Date    : Sep-21-2012$
*
* Comments:
*
*   This file contains the HTTPD implementation.
*
*END************************************************************************/

#include "httpd.h"
#include "httpd_supp.h"
#include "httpd_prv.h"

#if HTTPDCFG_POLL_MODE
#include <select.h>
#endif

#include <string.h>
#include <stdlib.h>

HTTPD_PARAMS_STRUCT* httpd_default_params(HTTPD_PARAMS_STRUCT *params) {
    HTTPD_PARAMS_STRUCT *ptr = NULL;
    uint_32 stack;

    if (params) {
        ptr = params;
    }
    else {
       ptr = HTTPD_MALLOC(sizeof(HTTPD_PARAMS_STRUCT));
       HTTPD_MEMTYPE(ptr,MEM_TYPE_HTTPD_PARAMS);
    }
    if (ptr) {
        HTTPD_MEMZERO(ptr, sizeof(HTTPD_PARAMS_STRUCT));

        ptr->port = HTTPDCFG_DEF_PORT;
        ptr->address = HTTPDCFG_DEF_ADDR;//default listen address

#if RTCSCFG_ENABLE_IP6
        IN6_ADDR_COPY((in6_addr*)(&(in6addr_any)),&(ptr->ipv6_address)); //set listen address to ANY
#endif
        ptr->max_uri = HTTPDCFG_DEF_URL_LEN;
        ptr->max_auth = HTTPDCFG_DEF_AUTH_LEN;

        ptr->cgi_lnk_tbl = 0;
        ptr->fn_lnk_tbl = 0;

        ptr->auth_fn = 0;

        ptr->root_dir = NULL;
        ptr->index_page = HTTPDCFG_DEF_INDEX_PAGE;

        ptr->max_ses = HTTPDCFG_DEF_SES_CNT;

#if HTTPDCFG_POLL_MODE
        ptr->max_line = HTTPD_MAX_LEN;
        stack = MAX(MAX(HTTPD_MAX_LEN, HTTPDCFG_MAX_BYTES_TO_SEND), HTTPDCFG_MAX_SCRIPT_LN) + 10 * sizeof(void*);
        stack = MAX(3 * stack, HTTPD_MIN_SES_STACK_SIZE);
#else
        stack = HTTPD_SERVER_STACK_SIZE;
#endif

        ptr->server_stack = stack;
        ptr->server_prio = HTTPDCFG_DEF_SERVER_PRIO;

#if !HTTPDCFG_POLL_MODE
        ptr->ses_prio = HTTPDCFG_DEF_SESSION_PRIO;

        stack = MAX(MAX(HTTPD_MAX_LEN, HTTPDCFG_MAX_BYTES_TO_SEND), HTTPDCFG_MAX_SCRIPT_LN) + 10 * sizeof(void*);
        stack = MAX(3 * stack, HTTPD_MIN_SES_STACK_SIZE);
        ptr->ses_stack = stack;
#endif
    }

    return ptr;
}

_mqx_int httpd_set_index_page(HTTPD_STRUCT *server, char *index_page)
{
    HTTPD_ASSERT(server && server->params && index_page);
    if (server && server->params && index_page)
    {
        server->params->index_page = index_page;
        return 0;
    }

    return 1;
}

HTTPD_STRUCT* httpd_init(HTTPD_PARAMS_STRUCT *params)
{
    sockaddr sin_sock_v4;
    sockaddr sin_sock_v6;

    HTTPD_STRUCT *server = NULL;
    int option, error;
  #if  HTTPDCFG_POLL_MODE /* FSL AB */
    int si;
  #endif  /* HTTPDCFG_POLL_MODE */

    HTTPD_ASSERT(params);

    if (params == NULL)
        return NULL;

    if ((server = HTTPD_MALLOC(sizeof(HTTPD_STRUCT))) == NULL) {
        HTTPD_DEBUG(1, "server: main structure allocation error\n");
        return NULL;
    }

    _mem_zero(server, sizeof(HTTPD_STRUCT));
    HTTPD_MEMTYPE(server,MEM_TYPE_HTTPD_STRUCT);
    server->params = params;

#if HTTPDCFG_POLL_MODE
    // session table allocation
    if ((server->session = HTTPD_MALLOC(sizeof(HTTPD_SESSION_STRUCT*) * server->params->max_ses)) == NULL) {
        HTTPD_DEBUG(1, "server: session table allocation error\n");
        goto ERROR_ALLOC;
    }

    HTTPD_MEMTYPE(server->session, MEM_TYPE_HTTPD_SESSION_STRUCT);

    // sessions allocation and sessions table initialization
    for (si = 0; si < server->params->max_ses; si++) {
        server->session[si] = httpd_ses_alloc(server);

        if (server->session[si] == NULL) {
            while (--si) {
                HTTPD_FREE(server->session[si]);
            }

            HTTPD_DEBUG(1, "server: session allocation error\n");
            goto ERROR_ALLOC;
        }
    }
#endif

    /* Init socket. */
    /* Open socket for IPv4. */
#if RTCSCFG_ENABLE_IP4
    if( ((params->af)&AF_INET) == AF_INET)
    {

        if ((server->sock_v4 = socket(AF_INET, SOCK_STREAM, 0)) == RTCS_ERROR)
        {
            HTTPD_DEBUG(1, "socket_v4: creation error\n");
            goto ERROR_ALLOC;
        }
        option = 500;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));
        option = 5000;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
        option = 1000;
        option = 1000;
        option = HTTPCFG_TIMEWAIT_TIMEOUT;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
    #if HTTPDCFG_POLL_MODE
        option = 500;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));
        option = TRUE;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
        option = TRUE;
    #else
        option = 0;     // 0 - RTCS default
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));
        option = FALSE; // FALSE - RTCS default
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
        option = TRUE;  // TRUE - RTCS default
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_RECEIVE_PUSH, &option, sizeof(option));
    #endif
        option = TRUE;
        option = TRUE;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));
        option = HTTPCFG_TX_WINDOW_SIZE;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
        option = HTTPCFG_RX_WINDOW_SIZE;
        error = setsockopt(server->sock_v4, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
        ((sockaddr_in *)&sin_sock_v4)->sin_port = server->params->port;
        ((sockaddr_in *)&sin_sock_v4)->sin_addr.s_addr = server->params->address;
        ((sockaddr_in *)&sin_sock_v4)->sin_family = AF_INET;
        error = (int_32) bind(server->sock_v4, &sin_sock_v4, sizeof(sin_sock_v4));
        if(error != 0)
        {
            HTTPD_DEBUG(1, "bind error\n");
            goto ERROR;
        }
        /* listen */
        error = (int_32)listen(server->sock_v4, 5);
        if (error != 0)
        {
            HTTPD_DEBUG(1, "socket: listen error\n");
            goto ERROR;
        }
    }
#endif
    /* Open socket for IPv6. */
#if RTCSCFG_ENABLE_IP6
    if( ((params->af)&AF_INET6) == AF_INET6)
    {

        if ((server->sock_v6 = socket(AF_INET6, SOCK_STREAM, 0)) == RTCS_ERROR)
        {
            HTTPD_DEBUG(1, "socket_v6: creation error\n");
            goto ERROR_ALLOC;
        }
        option = 500;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));
        option = 5000;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
        option = 1000;
        option = 1000;
        option = HTTPCFG_TIMEWAIT_TIMEOUT;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
    #if HTTPDCFG_POLL_MODE
        option = 500;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));
        option = TRUE;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
        option = TRUE;
    #else
        option = 0;     // 0 - RTCS default
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));
        option = FALSE; // FALSE - RTCS default
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
        option = TRUE;  // TRUE - RTCS default
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_RECEIVE_PUSH, &option, sizeof(option));
    #endif
        option = TRUE;
        option = TRUE;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));
        option = HTTPCFG_TX_WINDOW_SIZE;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
        option = HTTPCFG_RX_WINDOW_SIZE;
        error = setsockopt(server->sock_v6, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
        ((sockaddr_in6 *)&sin_sock_v6)->sin6_port      = server->params->port;
        ((sockaddr_in6 *)&sin_sock_v6)->sin6_family    = AF_INET6;
        /* add here scope_id for our server */
        ((sockaddr_in6 *)&sin_sock_v6)->sin6_scope_id  = server->params->if_scope_id;
        IN6_ADDR_COPY(&(server->params->ipv6_address), &(((sockaddr_in6 *)&sin_sock_v6)->sin6_addr));
        error = (int_32)bind(server->sock_v6, &sin_sock_v6, sizeof(sin_sock_v6));
        if(error != 0)
        {
            HTTPD_DEBUG(1, "bind error\n");
            goto ERROR;
        }
        /* listen*/
        error = (int_32)listen(server->sock_v6, 5);
        if (error != 0)
        {
            HTTPD_DEBUG(1, "socket: listen error\n");
            goto ERROR;
        }
    }
#endif

#if (RTCSCFG_ENABLE_IP4 || RTCSCFG_ENABLE_IP6)
    if (((params->af & AF_INET6) != AF_INET6) && (params->af & AF_INET) != AF_INET)
    {
        /* Wrong IP familly detected. */
        HTTPD_DEBUG(1, "Wrong IP familly detected.\n");
        goto ERROR_ALLOC;
    }


    server->run = 1;

    HTTPD_DEBUG(1, "------------------------------------------\n");
    HTTPD_DEBUG(1, " HTTP 1.0 server has been started.\n");
    HTTPD_DEBUG(1, "------------------------------------------\n\n");

    return server;

    ERROR:
    printf("error 0x%X\n", error);

    shutdown(server->sock_v4, FLAG_ABORT_CONNECTION);
    shutdown(server->sock_v6, FLAG_ABORT_CONNECTION);
#endif

ERROR_ALLOC:

    HTTPD_FREE(server);

    return NULL;
}


_mqx_int httpd_release(HTTPD_STRUCT *server)
{
    _mqx_int res = 0;

    if (server->run == 0 && server)
    {
        /* Shutdown all sockets from server. */
        shutdown(server->sock, FLAG_ABORT_CONNECTION);
        shutdown(server->sock_v4, FLAG_ABORT_CONNECTION);
        shutdown(server->sock_v6, FLAG_ABORT_CONNECTION);

        memset(server->params, 0, sizeof(HTTPD_PARAMS_STRUCT));
    }
    else
        res = -1;

    return res;
}


/** Close session
 * \param session < pointer to session
 */
static void httpd_ses_close(HTTPD_SESSION_STRUCT *session) {
    HTTPD_ASSERT(session);
    HTTPD_DEBUG(1, "session closed (%p)\n", session);

    if (session->response.file) {
        fclose(session->response.file);
    }

    session->valid = HTTPD_SESSION_INVALID;

    shutdown(session->sock, FLAG_CLOSE_TX);
}

/** Read http request.
 * \param session < pointer to session structure
 * \return
 */
static int httpd_readreq(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
#if HTTPDCFG_POLL_MODE
    char *buf;
#else
    char buf[HTTPD_MAX_LEN + 1];
#endif

    char *cp, *cp2;
    int max_len, res;


    HTTPD_ASSERT(server && session);

    // standard response
    strcpy(session->response.header, "Server: MQX HTTP - Freescale Embedded Web Server\n");
    session->response.contenttype = CONTENT_TYPE_HTML;
    session->response.statuscode = 200;
    session->response.hdrsent = 0;
    session->response.file = NULL;
    session->response.data = 0;
    session->response.script_token = 0;

    HTTPD_DEBUG(4, "request BEGIN\n");


#if HTTPDCFG_POLL_MODE
    buf = &session->line[session->line_used];
    max_len = server->params->max_line - session->line_used;
#else
    max_len = HTTPD_MAX_LEN;
#endif

    // read the request
    while (0 < (res = httpd_readln(session, buf, max_len)))
    {
        HTTPD_DEBUG(4, "request (%p): %s\n", session, buf);
        session->req_lines++;

#if HTTPDCFG_POLL_MODE
        buf = session->line;
        res = server->params->max_line;
#endif

        if (1 == session->req_lines) {  // first line = start-line
            if ((cp = strstr(buf, "GET"))!=0)
            {
                session->request.method = HTTPD_REQ_GET;
            }
            else if ((cp = strstr(buf, "POST"))!=0)
            {
                session->request.method = HTTPD_REQ_POST;
            }
            else {  // unknown method
                session->request.method = HTTPD_REQ_UNKNOWN;
                return -1;
            }

            // parse remain part of 1. line
            cp = strchr(cp, ' ') + 1;
            cp2 = strchr(cp, ' ');
            *cp2 = 0;

            strncpy(session->request.path, cp, server->params->max_uri);
            continue;
        }

        if (session->header) {
            if (buf[0] == '\r' && buf[1] == '\n') {        // header end - \r\n\r\n
                session->header = 0;

                res = 0;
                break;
            }
            else if (strncmp(buf, "Connection: ", 12) == 0)
            {
                cp = strchr(buf, ':') + 2;

                if ( (strncmp(cp, "keep-alive", 10) == 0)||(strncmp(cp, "Keep-Alive", 10) == 0) )
                {
                    /* You should remember that HTTP 1.0 do not support keep alive */
                    /* It is only work around */
                    session->keep_alive = 1;
                    HTTPD_DEBUG(4, "Connection: keep-alive = 1\n");
                }
            }
            else if (strncmp(buf, "Content-Length: ", 16) == 0) {
                cp = strchr(buf, ':') + 2;
                if (cp)
                {
                    session->request.content_len = _io_atoi(cp);
                    HTTPD_DEBUG(4, "Content-Length: %u\n", session->request.content_len);
                }
            }
            else if (strncmp(buf, "Authorization: ", 15) == 0) {
                cp = strchr(buf, ':') + 2;

                if (strncmp(cp, "Basic ", 6) == 0) {
                    cp = strchr(cp, ' ') + 1;

                    httpd_decode_base64(session->request.auth, cp, (int)server->params->max_auth);

                    cp = strchr(session->request.auth, ':');
                    if (cp)
                    {
                        *cp = 0;
                        session->request.authPasswd = cp + 1;
                    }

                    HTTPD_DEBUG(4, "Authorization - Basic: %s:%s\n", session->request.auth, session->request.authPasswd);
                }
                else
                {
                    HTTPD_DEBUG(4, "Authorization - unknown method\n");
                }
            }

            continue;
        }
    }

#if HTTPDCFG_POLL_MODE
    session->line_used += res;
#endif

    if (!session->header)
    {
        // process any URL data
        cp = strchr(session->request.path, '?');
        if (0 != cp)
        {
            *cp = 0;
            cp++;
            session->request.urldata = cp;
        }
    }

    HTTPD_DEBUG(4, "request END res = %d\n", res);

    return (0 > res) ? res : 0;
}

/** Http request process.
 * \param server < pointer to server
 * \param session < pointer to session data structure
 */
static void httpd_processreq(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session)
{
    char buf[HTTPD_MAX_LEN + 1];
    char *cp, *cp1;
    int cgi = 0, res = 0, auth = 0; // authorization required flag
    HTTPD_ROOT_DIR_STRUCT *root_dir;

    HTTPD_ASSERT(server && session);

    // check request path
    cp = strrchr(session->request.path, '/');
    if (0 == cp)
    {
        HTTPD_DEBUG(1, "Invalid request path '%s'\n", session->request.path);
        return;
    }

    if (session->request.path[0] == '/' && session->request.path[1] == 0)
        strcpy(session->request.path, server->params->index_page);

    // check if request is script request

    // if request contain data
    res = -1;

    if ((cp1 = strrchr(cp, '.')) != 0)
    {
        if (0 == strcasecmp(cp1, ".cgi"))   // file extension is cgi, maybe cgi script
        {
            cgi = 1;

            // search cgi script
            *cp1 = 0;
            res = httpd_cgi(server, session, cp + 1);   // cp + 1 = cgi script name begin
            *cp1 = '.';
        }
    }

    if (cgi || session->request.content_len)
    {
        if (res < 0)
        {
            if (res != -2)
            	cgi = 0;

            // cgi script not found - read data from socket - discard it
            while (session->request.content_len)
            {
                res = httpd_read(session, buf, (int)((res > sizeof(buf))? sizeof(buf) : session->request.content_len));

                session->request.content_len -= res;

                // TODO add timeout - can be deadlock

                // temporary solution
                if (res == 0)
                    break;
            }

        }
        else {
            // cgi script found - decrease content len
            session->request.content_len -= res;

            if (session->request.content_len == 0)  // whole content procesed
                session->state = HTTPD_SES_END_REQ;
        }
    }

    // send regular file(document) - if is not cgi script
    if (!cgi) {
        // get correct root dir (by alias)

        if (server->params->root_dir[1].alias) {
            // more root dir pressents, find alias
            root_dir = server->params->root_dir;

            cp = strchr(session->request.path, '/');

            if (cp) {
                cp++;
                cp1 = strchr(cp, '/');

                if (cp1) {
                    while (root_dir->alias) {
                        if (strncmp(root_dir->alias, cp, cp1 - cp) == 0) {
                            strcpy(buf, root_dir->path);
                            strcat(buf, cp1);
                            break;
                        }

                        root_dir++;
                    }
                }
            }

            if (!cp || !cp1 || !root_dir->alias) {
                strcpy(buf, server->params->root_dir->path);
                strcat(buf, session->request.path);
            }
        }
        else {
            strcpy(buf, server->params->root_dir[0].path);
            strcat(buf, session->request.path);
        }

        // correct slash
        cp = buf;
        while (*cp) {
            if ('/' == *cp)
                *cp = '\\';

            cp++;
        }

        session->response.file = fopen(buf, "rb");
        session->response.len = 0;

        if (session->response.file) {
        }
        else {
            session->response.statuscode = 404;
        }

        session->state = HTTPD_SES_RESP;
    }

    if (auth && server->params->auth_fn)
    {
        if (*session->request.auth)
        {
            // check authorization
            if (server->params->auth_fn(session))
            {
                session->response.statuscode = 401;
                strncat(session->response.header, "WWW-Authenticate: Basic realm=\"test\"\n", HTTPDCFG_MAX_HEADER_LEN);
            }
        }
        else
        {
            session->response.statuscode = 401;
            strncat(session->response.header, "WWW-Authenticate: Basic realm=\"test\"\n", HTTPDCFG_MAX_HEADER_LEN);
        }
    }
}

/** http response
 * \param server < pointer to server data structure
 * \param session < pointer to session data structure
 */
static void httpd_response(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    HTTPD_ASSERT(server && session);

    switch (session->response.statuscode) {
    case 200:
        httpd_sendfile(server, session);
        break;
    case 401:
        httpd_send401(server, session);
        session->state = HTTPD_SES_END_REQ;
        break;
    default:
        httpd_send404(server, session);
        session->state = HTTPD_SES_END_REQ;
        break;
    }
}

/** HTTPD session state machine
 * \param server < pointer to server
 * \param session < session number
 */
void httpd_ses_process(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    HTTPD_TIME_STRUCT time;
    int res;
    HTTPD_ASSERT(server && session);

    if (HTTPD_SESSION_VALID == session->valid)
    {
        // check timeout
        HTTPD_GET_TIME(&time);
        if (HTTPD_DIFF_TIME_MS(session->time, time) > HTTPDCFG_SES_TO) {
            HTTPD_DEBUG(1, "session %p timeout\n", session);
            session->state = HTTPD_SES_CLOSE;
        }

        switch (session->state) {
        case HTTPD_SES_WAIT_REQ:
            res = httpd_readreq(server, session);
            if (0 > res) {
                session->state = HTTPD_SES_END_REQ;
            }
            else {
                if (session->header == 0) {
                    session->state = HTTPD_SES_PROCESS_REQ;
                }
            }

            break;
        case HTTPD_SES_PROCESS_REQ:
            httpd_processreq(server, session);
            break;

        case HTTPD_SES_RESP:
            httpd_response(server, session);
            break;

        case HTTPD_SES_END_REQ:
            if (!session->keep_alive)
            {
                session->state = HTTPD_SES_CLOSE;
            }
            else
            {
                // init session
                session->state = HTTPD_SES_WAIT_REQ;
                if (session->response.file)
                {
                    fclose(session->response.file);
                    /* To be sure that file handle will not used. */
                    session->response.file = NULL;
                }
                session->header = 1;
                session->req_lines = 0;
                session->remain = 0;

                /*
                  To avoid problem with keep- alive, just added receive time out.
                  If client fast enough, it can use opened connection. If not,
                  server will close connection after timeout.
                */
                ((SOCKET_STRUCT_PTR)session->sock)->RECEIVE_TIMEOUT =HTTPD_RECEIVE_TIMEOUT;

#if HTTPDCFG_POLL_MODE
                session->recv_used = 0;
#endif

                HTTPD_GET_TIME(&session->time);

                session->keep_alive--;
                break;
            }

        case HTTPD_SES_CLOSE:
            httpd_ses_close(session);
            break;

        default:
            // invalid state
            session->valid = HTTPD_SESSION_INVALID;
            break;
        }
    }
}

/** Allocate session.
 * \param server < pointer to server
 * \return pointer to allocated session
 */
HTTPD_SESSION_STRUCT* httpd_ses_alloc(HTTPD_STRUCT *server) {
    HTTPD_SESSION_STRUCT *session = NULL;

    HTTPD_ASSERT(server);

    if (server) {
        session = HTTPD_MALLOC(sizeof(HTTPD_SESSION_STRUCT));
        if (session) {
            HTTPD_MEMTYPE(session,MEM_TYPE_HTTPD_SESSION_STRUCT);

            HTTPD_MEMZERO(session, sizeof(HTTPD_SESSION_STRUCT));

            session->request.path = HTTPD_MALLOC(server->params->max_uri + 1);
            if (NULL == session->request.path)
                goto ERROR;

            HTTPD_MEMTYPE(session->request.path,MEM_TYPE_HTTPD_URI);

            session->request.auth = HTTPD_MALLOC(server->params->max_auth + 1);
            if (NULL == session->request.auth)
                goto ERROR;

            HTTPD_MEMTYPE(session->request.auth,MEM_TYPE_HTTPD_AUTH);

#if HTTPDCFG_POLL_MODE
            session->recv_used = 0;

            session->line = HTTPD_MALLOC(server->params->max_line + 1);;
            HTTPD_MEMTYPE(session->line, MEM_TYPE_HTTPD_LINE);
            session->line_used = 0;
#endif
        }
    }

    return session;

ERROR:
    if (session->request.path)
        HTTPD_FREE(session->request.path);

    HTTPD_FREE(session);

    return NULL;
}

void httpd_ses_free(HTTPD_SESSION_STRUCT *session) {
    HTTPD_ASSERT(session);

    if (session) {
        HTTPD_FREE(session->request.path);
        HTTPD_FREE(session->request.auth);
        HTTPD_FREE(session);
    }
}

void httpd_ses_init(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session, const int sock) {
    HTTPD_ASSERT(server && session && sock != RTCS_ERROR);

    HTTPD_DEBUG(5, "httpd_ses_init: server(%p) session(%p) sock(0x%x)\n", server, session, sock);

    if (server && session) {
        // init session
        session->state = HTTPD_SES_WAIT_REQ;
        session->sock = sock;
        session->keep_alive = 0;
        session->valid = HTTPD_SESSION_VALID;

        session->header = 1;
        session->req_lines = 0;
        session->remain = 0;

        /* The auth data must be initialized when a new session
         * is init. Otherwise a new session may profit from a previous authenticated session. */
        HTTPD_MEMZERO(session->request.auth, server->params->max_auth);

#if HTTPDCFG_POLL_MODE
        session->recv_used = 0;
        session->line_used = 0;
#endif

        HTTPD_GET_TIME(&session->time);
    }
}

void httpd_ses_poll(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session) {
    int s;
    unsigned short len;
    struct sockaddr_in sin;

    HTTPD_ASSERT(server && session);

#if !HTTPDCFG_POLL_MODE
    // loop - when server run in task mode, loop until flag run is nonzero
    do {
#endif
        if (HTTPD_SESSION_VALID != session->valid) {
            len = sizeof(sin);

            s = accept(server->sock,(sockaddr *) &sin, &len);
            if (s && s != RTCS_SOCKET_ERROR) {
                HTTPD_DEBUG(1, "request from (%p): %d.%d.%d.%d; port: %d.\n", session, (sin.sin_addr.s_addr >> 24) & 0xff, (sin.sin_addr.s_addr >> 16) & 0xff, (sin.sin_addr.s_addr >> 8) & 0xff, sin.sin_addr.s_addr & 0xff, sin.sin_port);

                httpd_ses_init(server, session, s);
            }
        }

#if HTTPDCFG_POLL_MODE
        // in poll mode, session is partial processed
        if (HTTPD_SESSION_VALID == session->valid)
#else
        // in task mode, loop until session is valid
        while (HTTPD_SESSION_VALID == session->valid)
#endif
        {
            httpd_ses_process(server, session);
        }
#if !HTTPDCFG_POLL_MODE
        // task mode - loop until server run (run flag is nonzero)
    } while (server->run);
#endif

}

