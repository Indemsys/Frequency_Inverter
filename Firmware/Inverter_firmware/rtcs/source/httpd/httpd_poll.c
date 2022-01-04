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
* $FileName: httpd_poll.c$
* $Version : 3.7.1.0$
* $Date    : Mar-9-2011$
*
* Comments:
*
*   HTTPD os specific part implementation.
*
*END************************************************************************/

#include "httpd_mqx.h"
#include "httpd.h"
#include "httpd_prv.h"

#if HTTPDCFG_POLL_MODE

#include <select.h>



/** initialize httpd server with default configuration
 * \param root_dir < root directory structure
 * \param index_page < pointer to index page with absolut path
 * \return pointer to created server structure
 */
HTTPD_STRUCT* httpd_server_init(HTTPD_ROOT_DIR_STRUCT *root_dir, const char *index_page) {
    HTTPD_STRUCT *server = NULL;
    HTTPD_PARAMS_STRUCT *params;
    
    HTTPD_ASSERT(root_dir && index_page);
    
    params = httpd_default_params(NULL);
    if (params) {
        params->root_dir = root_dir;
        params->index_page = (char*)index_page;
        server = httpd_init(params);
    }
    
    return server;
}

/** httpd server poll
 * Main function for HTTPD which provide server functionality. This function is 
 * called from httpd task (in infinity loop) or user can call it periodicaly 
 * from user application.
 * \param server < pointer to server structure
 * \param to < timeout for poll (max time for waiting event)
 */
void httpd_server_poll(HTTPD_STRUCT *server, _mqx_int to) {
    HTTPD_SESSION_STRUCT **session;
    int si, res;
    fd_set read_set, write_set;
    struct timeval tm;
    
    HTTPD_ASSERT(server);

    if (!server)
        return;
    
    session = server->session;

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
        
    for (si = 0; si < server->params->max_ses; si++) {
        if (session[si]->valid == HTTPD_SESSION_VALID) {
            FD_SET(session[si]->sock, &read_set);
            FD_SET(session[si]->sock, &write_set);
        }
    }
    
    if (read_set.COUNT < server->params->max_ses)       // set listen socket if some session is not used
        FD_SET(server->sock, &read_set);

    tm.tv_sec = (to == -1) ? 0 : to;
    tm.tv_usec = 0;
        
    res = select(server->sock + 1, &read_set, &write_set, NULL, (to == 0) ? NULL : &tm);
        
    if (res > 0) {
        if (FD_ISSET(server->sock, &read_set)) {
            // new connection
            
            for (si = 0; si < server->params->max_ses; si++) {
                if (session[si] && session[si]->valid != HTTPD_SESSION_VALID) {
                    httpd_ses_poll(server, session[si]);
                    break;
                }
            }
        }
            
        for (si = 0; si < server->params->max_ses; si++) {
            if ((FD_ISSET(session[si]->sock, &read_set) || FD_ISSET(session[si]->sock, &write_set)) &&
                session[si]->valid == HTTPD_SESSION_VALID) {
                    
                httpd_ses_poll(server, session[si]);
            }
        }
    }
}

/** server task - httpd run in one task and poll each session
 */
static void httpd_server_task(pointer init_ptr, pointer creator) {
    HTTPD_STRUCT *server = (HTTPD_STRUCT*)init_ptr;
    
    HTTPD_ASSERT(server);
    
    if (!server)
        goto ERROR;
        
    RTCS_task_resume_creator(creator, RTCS_OK);

    while (1) {
        httpd_server_poll(server, 1);
    }
    
ERROR:
    RTCS_task_resume_creator(creator, (uint_32)RTCS_ERROR);
}

/** create httpd server task - poll mode
 * \param server < pointer to server structure
 * \return
 */
static uint_32 httpd_server_task_create(HTTPD_STRUCT *server) {
    uint_32 stack;

    HTTPD_ASSERT(server);
    
    return RTCS_task_create("httpd server", server->params->server_prio, server->params->server_stack, httpd_server_task, server);
}

/** run httpd server in poll mode
 * \param server < pointer to server structure
 * \return zero if success
 */
_mqx_int httpd_server_run(HTTPD_STRUCT *server) {
    HTTPD_ASSERT(server);
    
    if (server && httpd_server_task_create(server) == RTCS_OK)
        return 0;
    
    return -1;
}

#endif
