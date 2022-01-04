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
* $FileName: httpd_prv.h$
* $Version : 3.7.1.0$
* $Date    : Mar-9-2011$
*
* Comments:
*
*   Header for HTTPD.
*
*END************************************************************************/

#ifndef HTTPD_PRV_H_
#define HTTPD_PRV_H_

#include "httpd_types.h"
#include "httpd_supp.h"


#define HTTPD_SERVER_STACK_SIZE     (1000)
#define HTTPD_MIN_SES_STACK_SIZE    (2500)


typedef struct {
    HTTPD_STRUCT *server;
    uint_32 sock;
} HTTPD_SES_TASK_PARAM;

void httpd_poll(HTTPD_STRUCT *server);
//_mqx_int httpd_get_varval(HTTPD_SESSION_STRUCT *session, char *var_str, char *var_name, char *var_val, _mqx_int var_val_len);
void httpd_ses_process(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session);


HTTPD_SESSION_STRUCT* httpd_ses_alloc(HTTPD_STRUCT *server);

/** Release session - free memory allocated by session.
 * \param session < pointer to session
 */
void httpd_ses_free(HTTPD_SESSION_STRUCT *session);


void httpd_ses_poll(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session/*, int loop*/);

void httpd_ses_init(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session, const int sock);


#endif /* HTTP_PRV_H_ */
