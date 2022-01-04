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
* $FileName: httpd.h$
* $Version : 3.8.21.0$
* $Date    : Aug-31-2012$
*
* Comments:
*
*   Header for HTTPD.
*
*END************************************************************************/

#ifndef HTTPD_H_
#define HTTPD_H_

#include <rtcs.h>
#include "httpd_types.h"
#include "httpd_supp.h"


#define HTTPD_SET_PARAM_PORT(server, val)           HTTPD_SET_PARAM(server, port, val)
#define HTTPD_SET_PARAM_ROOT_DIR(server, val)       HTTPD_SET_PARAM(server, root_dir, val)
#define HTTPD_SET_PARAM_INDEX_PAGE(server, val)     HTTPD_SET_PARAM(server, index_page, val)
#define HTTPD_SET_PARAM_FN_TBL(server, val)         HTTPD_SET_PARAM(server, fn_lnk_tbl, val)
#define HTTPD_SET_PARAM_CGI_TBL(server, val)        HTTPD_SET_PARAM(server, cgi_lnk_tbl, val)
#define HTTPD_SET_PARAM_AUTH_FN(server, val)        HTTPD_SET_PARAM(server, auth_fn, val)

#define HTTPD_SET_PARAM(server, pname, val)             \
{                                                       \
    HTTPD_ASSERT(server && server->params);             \
    if (server && server->params) {                     \
        server->params->pname = val;                    \
    }                                                   \
}
/* Waiting time in miliseconds for RTCS_selectall() function for waiting input connection.*/ 
#define HTTPD_SELLECTALL_WAITING_TIME 3000 

/* It is timeout (in milliseconds) for keep-alive mode */
#ifndef HTTPD_RECEIVE_TIMEOUT
#define HTTPD_RECEIVE_TIMEOUT 50
#endif


/** Set default parameters for server. Function initialize parameters structure to default value.
 * If calling parameter params is NULL, function allocate parameters in memory - don't forget release it
 * when close/realease HTTPD.
 * \param params < pointer to parameters structure which will be set to default values
 * \return pointer to parameters structure.
 */
HTTPD_PARAMS_STRUCT* httpd_default_params(HTTPD_PARAMS_STRUCT *params);

/** Set index page for httpd.
 * \param server < pointer to server
 * \param index_page < pointer to index page filename (can be with relative path to root_dir)
 * \return zero when success
 */
_mqx_int httpd_set_index_page(HTTPD_STRUCT *server, char *index_page);

/** Initialize HTTP server.
 * \param server < pointer to server data structure
 * \return zero when success
 */
HTTPD_STRUCT* httpd_init(HTTPD_PARAMS_STRUCT *params);

/** Release HTTP server.
 * \param server < pointer to server data structure
 * \return 0 when success
 */
_mqx_int httpd_release(HTTPD_STRUCT *server);

/** Initialize HTTP server.
 * \param root_dir < pointer to root directory structure
 * \param index_page < index page relative path
 * \return pointer to server
 */
#if RTCSCFG_ENABLE_IP4 
    HTTPD_STRUCT* httpd_server_init(HTTPD_ROOT_DIR_STRUCT *root_dir, const char *index_page);
#endif
/** Initialize HTTP server.
 * \param root_dir < pointer to root directory structure
 * \param index_page < index page relative path
 * \return pointer to server
 *  parameter af_inet-family define IP4 or IP6 mode of server will be used
 */
HTTPD_STRUCT* httpd_server_init_af(HTTPD_ROOT_DIR_STRUCT *root_dir, const char *index_page, uint_32 af_inet_family);


/** Run HTTP server.
 * \param server < pointer to server structure
 * \return non zero when success
 */
_mqx_int httpd_server_run(HTTPD_STRUCT *server);

/** HTTP server polling fn
 * \param server < pointer to server structure
 * \param to < timeout for poll fn
 */
void httpd_server_poll(HTTPD_STRUCT *server, _mqx_int to);

/** stop httpd server
 * \param server < pointer to server structure
 * \return zero if success
 */
_mqx_int httpd_server_stop(HTTPD_STRUCT *server);

#endif /* HTTP_H_ */
