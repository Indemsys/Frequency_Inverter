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
* $FileName: httpd_task.c$
* $Version : 3.8.12.0$
* $Date    : Aug-31-2012$
*
* Comments:
*
*   HTTPD os specific part implementation.
*
*END************************************************************************/

#include "httpd_mqx.h"
#include "httpd.h"
#include "httpd_prv.h"
#include "socket.h"

#if !HTTPDCFG_POLL_MODE

/** initialize httpd server with default configuration
 * \param root_dir < root directory structure
 * \param index_page < pointer to index page with absolut path
 * \return pointer to created server structure
 *
 *  If AF inet family was not defined, it will be started 
 *  in depend on IP4 or IP6 or IP4+IP6 configuration choosed. 
 *  So, IP4 - server for IP4, IP6 - server for IP6, IP4+IP6 two 
 *  servers well be started.
 */
#if RTCSCFG_ENABLE_IP4

HTTPD_STRUCT* httpd_server_init(HTTPD_ROOT_DIR_STRUCT *root_dir, const char *index_page) 
{
    extern HTTPD_PARAMS_STRUCT  *httpd_default_params(HTTPD_PARAMS_STRUCT *params);
    extern HTTPD_STRUCT         *httpd_init(HTTPD_PARAMS_STRUCT *params);
    HTTPD_STRUCT                *server = NULL;
    HTTPD_PARAMS_STRUCT         *params;
    
    params = httpd_default_params(NULL);
    if (params) 
    {
        params->root_dir = root_dir;
        params->index_page = (char*)index_page;
        params->af = AF_INET;
        server = httpd_init(params);
    }
    
    return server;
}

#endif

HTTPD_STRUCT* httpd_server_init_af(HTTPD_ROOT_DIR_STRUCT *root_dir, const char *index_page, uint_32 af_inet_family)
{
    extern HTTPD_PARAMS_STRUCT  *httpd_default_params(HTTPD_PARAMS_STRUCT *params);
    extern HTTPD_STRUCT         *httpd_init(HTTPD_PARAMS_STRUCT *params);
    HTTPD_STRUCT                *server = NULL;
    HTTPD_PARAMS_STRUCT         *params;
    
    params = httpd_default_params(NULL);
    if (params) 
    {
        params->root_dir = root_dir;
        params->index_page = (char*)index_page;
        params->af=af_inet_family;
        server = httpd_init(params);
    }
    return server;
}



#if HTTPDCFG_STATIC_TASKS

/** session task
 */
static void httpd_session_static_task(pointer init_ptr, pointer creator) {
    extern HTTPD_SESSION_STRUCT* httpd_ses_alloc(HTTPD_STRUCT *server);
    extern void httpd_ses_poll(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session);
    HTTPD_SESSION_STRUCT *session;
    
    if (!init_ptr)
        goto ERROR;
    
    session = httpd_ses_alloc((HTTPD_STRUCT*)init_ptr);
    
    if (session) {
        RTCS_task_resume_creator(creator, RTCS_OK);
        httpd_ses_poll((HTTPD_STRUCT*)init_ptr, session);
    }
    else
        goto ERROR;
    
    return;
    
ERROR:
    RTCS_task_resume_creator(creator, RTCS_ERROR);
}

/** create task for session
 * \param server < pointer to server structure
 * \param stack < stack size for session task
 * \return
 */
static uint_32 httpd_session_static_task_create(HTTPD_STRUCT *server) {
    return RTCS_task_create("httpd session", HTTPDCFG_DEF_SERVER_PRIO , server->params->ses_stack, httpd_session_static_task, server);
}

/** run httpd server - each session run in separete task
 * \param server < pointer to server structure
 * \return zero if success
 */
_mqx_int httpd_server_run(HTTPD_STRUCT *server) {
    _mqx_int ses_cnt = server->params->max_ses;
    
    // try create task per session
    while (server && ses_cnt) {
        if (httpd_session_static_task_create(server) != RTCS_OK)
            break;  // task creation failed, stop
        
        ses_cnt--;
    }
    
    return (ses_cnt);
}

#else // HTTPDCFG_STATIC_TASKS

static LWSEM_STRUCT sem_session_counter;

static void httpd_session_dynamic_task(pointer init_ptr, pointer creator) 
{

    
    
    HTTPD_SES_TASK_PARAM *param = (HTTPD_SES_TASK_PARAM*)init_ptr;
    HTTPD_SESSION_STRUCT *session;
    
    HTTPD_ASSERT(init_ptr);
    
    HTTPD_DEBUG(1, "session task start\n");
    
    session = httpd_ses_alloc(param->server);
    
    if (session) 
    {
        RTCS_task_resume_creator(creator, RTCS_OK);
        
        HTTPD_DEBUG(1, "session task run...\n");
        
        httpd_ses_init(param->server, session, param->sock);
    
        while (HTTPD_SESSION_VALID == session->valid) 
        {
            httpd_ses_process(param->server, session);
        }
    
        httpd_ses_free(session);
    }
    else 
    {
        RTCS_task_resume_creator(creator, (uint_32) RTCS_ERROR);
    }
    
    HTTPD_FREE(init_ptr);
    
    _lwsem_post(&sem_session_counter);
    
    HTTPD_DEBUG(1, "session task stop\n");
}

/** server task - httpd main task which create new task for each new request
 */
static void httpd_server_task(pointer init_ptr, pointer creator) 
{
    HTTPD_STRUCT *server = (HTTPD_STRUCT*)init_ptr;
    int s;
    unsigned short len;
    uint_32 connsock; //socket with connection
    struct sockaddr sin;
    
    HTTPD_SES_TASK_PARAM *param;
    _mqx_uint res;
    uint_32 stack;
 
    HTTPD_DEBUG(1, "server task start\n");
      
    res = _lwsem_create(&sem_session_counter, server->params->max_ses);

    if (!server && res != MQX_OK)
        goto ERROR;
        
    RTCS_task_resume_creator(creator, RTCS_OK);
    
    HTTPD_DEBUG(1, "server task run...\n");

    while (server->run) 
    {
        // limit maximal opened sessions
        _lwsem_wait(&sem_session_counter);
        
        // allocate session task parameter structure
        param = _mem_alloc_system(sizeof(HTTPD_SES_TASK_PARAM));
        
        if (param) 
        {
            param->server = server;
            connsock = RTCS_selectset(&(server->sock_v4), 2, 0);
            param->sock = accept(connsock,(sockaddr *) &sin, &len);
            /* Check if accept returned valid socket */
            if (param->sock != RTCS_SOCKET_ERROR)
            {
                // try create task for session
                res = RTCS_task_create("httpd session", server->params->ses_prio, server->params->ses_stack, httpd_session_dynamic_task, param);

                if (MQX_OK != res)
                {
                    // session task creation failed, clean and wait...
                    shutdown(param->sock, FLAG_ABORT_CONNECTION);   // abort opened connection
                    _mem_free(param);
                    _lwsem_post(&sem_session_counter);
                    _time_delay(1);
                }
            }
            else
            {
                // accept return error, clean and wait, then try again...
                _mem_free(param);
                _lwsem_post(&sem_session_counter);
                _time_delay(1);
            }
        }
        else
        {
            // allocation failed ?!?!? wait some time
            _time_delay(100);
        }
    }
    
    HTTPD_DEBUG(1, "server task stop\n");
    
ERROR:
    RTCS_task_resume_creator(creator, (uint_32)RTCS_ERROR);
}

/** create httpd server task - 
 * \param server < pointer to server structure
 * \return
 */
static uint_32 httpd_server_task_create(HTTPD_STRUCT *server) {
    return RTCS_task_create("httpd server", server->params->server_prio, server->params->server_stack, httpd_server_task, server);
}

/** run httpd server in 
 * \param server < pointer to server structure
 * \return zero if success
 */
_mqx_int httpd_server_run(HTTPD_STRUCT *server) {
    HTTPD_ASSERT(server);
    
    if (server && httpd_server_task_create(server) == RTCS_OK)
        return 0;
    
    return -1;
}

#endif // HTTPDCFG_STATIC_TASKS

/** stop httpd server
 * \param server < pointer to server structure
 * \return zero if success
 */
_mqx_int httpd_server_stop(HTTPD_STRUCT *server) {
    HTTPD_ASSERT(server);

    server->run = 0;
    
    return httpd_release(server);   
}

#endif // !HTTPDCFG_POLL_MODE
