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
* $FileName: httpd_supp.h$
* $Version : 3.7.8.0$
* $Date    : Mar-9-2011$
*
* Comments:
*
*   HTTPD support functions header.
*
*END************************************************************************/

#ifndef HTTP_SUPP_H_
#define HTTP_SUPP_H_

#include "httpd_types.h"

/** Send string to socket.
 * \param s < socket
 * \param str < pointer to string
 * \return length of sent data
 */
_mqx_int httpd_sendstr(_mqx_int s, char *str);

/** Read data from tcp/ip stack.
 * \param session < pointer to session
 * \param dest < pointer to destination buffer
 * \param len < requested read length
 * \return real read length
 */
_mqx_int httpd_read(HTTPD_SESSION_STRUCT *session, char *dst, _mqx_int len);

/** Read line from tcp/ip stack.
 * \param session < pointer to session
 * \param dest < pointer to destination buffer
 * \param len < maximal read length
 * \return nonzero - read success
 */
_mqx_int httpd_readln(HTTPD_SESSION_STRUCT *session, char *dst, _mqx_int max_len);

/** send header
 * \param session < pointer to evaluated session
 * \param content_len < content len
 * \param mod_time < modification time
 */
void httpd_sendhdr(HTTPD_SESSION_STRUCT *session, _mqx_int content_len, _mqx_int mod_time);

/** send file
 * \param server < pointer to server configuration structure
 * \param session < pointer to evaluated session
 */
void httpd_sendfile(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session);

/** HTTP find cgi and call it.
 * \param session < pointer to session
 * \param cgi_name < cgi script name
 * \return
 *      < 0 - not found or cgi script request flush stack
 *      other - readed length from stack
 */
_mqx_int httpd_cgi(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session, char *cgi_name);

/** send 401 error page. Find page file or generate standard page.
 * \param server < pointer to server
 * \param session < pointer to session
 */
void httpd_send401(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session);

/** send 403 error page. Find page file or generate standard page.
 * \param server < pointer to server
 * \param session < pointer to session
 */
void httpd_send403(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session);

/** send 404 error page. Find page file or generate standard page.
 * \param server < pointer to server
 * \param session < pointer to session
 */
void httpd_send404(HTTPD_STRUCT *server, HTTPD_SESSION_STRUCT *session);

/** decode base64 block
 * \param dst < destination string buffer
 * \param src < source string buffer
 * \param dst_size < maximal destination string length
 * \return
 */
_mqx_int httpd_decode_base64(char *dst, char *src, _mqx_int dst_size);

/** Get variable from URL link - GET method.
 * \param session < pointer to session data
 * \param var_str < variables string - string for parsing
 * \param var_name < variable name
 * \param var_val < pointer to buffer for variable value (string)
 * \param var_val_len < maximal length for variable value
 * \return
 */
_mqx_int httpd_get_varval(HTTPD_SESSION_STRUCT *session, char *var_str, char *var_name, char *var_val, _mqx_int var_val_len);

#endif /* HTTP_SUPP_H_ */
