/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved                       
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: ftpd.c$
* $Version : 3.8.21.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains an implementation of an FTP server.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <rtcs.h>
#include <fio.h>

#include "ftpd.h"
#include "ftpd_msg.h"

#if FTPDCFG_USES_MFS
#include <mfs.h>
#endif

_rtcs_taskid FTPd_task_id = 0;
uint_32 FTPd_window_size = FTPDCFG_WINDOW_SIZE;
uint_32 FTPd_buffer_size = FTPDCFG_BUFFER_SIZE;

typedef struct ftpd_context  {
   uint_32     LISTENSOCK;
} FTPd_CONTEXT, _PTR_ FTPd_CONTEXT_PTR;


#ifdef __MQX__ 
void FTPd_Exit_handler(void);
#endif

extern void FTPd_child(pointer, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPd_init
* Returned Value   : error code
* Comments  :  Start the FTP server.
*
*END*-----------------------------------------------------------------*/

uint_32 FTPd_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   if (FTPd_task_id == 0)  {
      return RTCS_task_create(name, priority, stacksize, FTPd_task, NULL);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : FTPd_task
* Returned Value   : none
* Comments  :  FTP server.
*
*END*-----------------------------------------------------------------*/

void FTPd_task
   (
      pointer  init_ptr,
      pointer creator
   )
{ /* Body */
   FTPd_CONTEXT               ftpd_context =  { 0 };
   sockaddr_in                laddr;
   uint_32                    sock, listensock;
   uint_32                    error=RTCS_OK;
   uint_16                    remote_addr_len;
   sockaddr_in                remote_addr = {0};
#if FTPDCFG_ENABLE_MULTIPLE_CLIENTS   
   TASK_TEMPLATE_STRUCT_PTR   t_ptr;
#endif   
   FTPd_SESSION_PTR           session_ptr;
   uint_32                    option;
   boolean                    dev_in_path = FALSE;
   int_16                     devlen = 0, rootdirlen = 0;

   FTPd_task_id = RTCS_task_getid();
#ifdef __MQX__ 
   /* Set up exit handler and context so that we can clean up if the FTP Server is terminated */
   _task_set_environment( _task_get_id(), (pointer) &ftpd_context );
   _task_set_exit_handler( _task_get_id(), FTPd_Exit_handler );
#endif 

   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_FTP;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Listen on TCP port */
   ftpd_context.LISTENSOCK = socket(PF_INET, SOCK_STREAM, 0);
   listensock = ftpd_context.LISTENSOCK;
   if (listensock == RTCS_SOCKET_ERROR) {
      error = RTCSERR_OUT_OF_SOCKETS;
   } 
   
   if (!error) {
      option = FTPd_window_size;   
      error = setsockopt(listensock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
   }
   
   if (!error) {
      option = FTPd_window_size;   
      error = setsockopt(listensock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
   }      

   if (!error) {
      option = FTPDCFG_SEND_TIMEOUT;   
      error = setsockopt(listensock, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));
   }   

   if (!error) {
      option = FTPDCFG_CONNECT_TIMEOUT;   
      error = setsockopt(listensock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
   }   

   if (!error) {
      option = FTPDCFG_TIMEWAIT_TIMEOUT;   
      error = setsockopt(listensock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
   }   

   if (!error) {
      error = bind(listensock, (const sockaddr *)&laddr, sizeof(laddr));
   } 

   if (!error) {
      error = listen(listensock, 0);
   } 

   if (error) {
      RTCS_task_exit(creator, error);
   } 
   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {

      /* Connection requested; accept it */
      remote_addr_len = sizeof(remote_addr);
      sock = accept(listensock,(sockaddr *)&remote_addr, &remote_addr_len);
      
      if ((sock != 0) && (sock!=RTCS_SOCKET_ERROR)) {
         session_ptr = (FTPd_SESSION_PTR) RTCS_mem_alloc_zero(sizeof (FTPd_SESSION_STRUCT));

         if ( session_ptr ) {
            _mem_set_type(session_ptr, MEM_TYPE_FTPd_SESSION_PTR);
 
            session_ptr->DATA_BUFFER_SIZE = FTPd_buffer_size;
            session_ptr->DATA_BUFFER_PTR = RTCS_mem_alloc_zero(session_ptr->DATA_BUFFER_SIZE);
            if (session_ptr->DATA_BUFFER_PTR == NULL) {
               _mem_free(session_ptr);
               session_ptr = NULL;
            } else {
               _mem_set_type(session_ptr->DATA_BUFFER_PTR, MEM_TYPE_FTPd_DATA_BUFFER);
            }
         }
            
            
         if (session_ptr == NULL) {
            shutdown((uint_32)sock, FTPDCFG_SHUTDOWN_OPTION);
         } else  {
      

            session_ptr->CONTROL_SOCK = (uint_32) sock;
            session_ptr->CONNECTED = TRUE;
            /* set default data ports */
            session_ptr->SERVER_DATA_SOCKADDR.sin_family      = AF_INET;
            session_ptr->SERVER_DATA_SOCKADDR.sin_port        = IPPORT_FTPDATA;
            session_ptr->SERVER_DATA_SOCKADDR.sin_addr.s_addr = INADDR_ANY;

            session_ptr->USER_DATA_SOCKADDR.sin_family      = remote_addr.sin_family;
            session_ptr->USER_DATA_SOCKADDR.sin_port        = remote_addr.sin_port;
            session_ptr->USER_DATA_SOCKADDR.sin_addr.s_addr = remote_addr.sin_addr.s_addr;

#if FTPDCFG_USES_MFS
            //initialize current directory and current filesystem
            devlen = _io_get_dev_for_path(session_ptr->CURRENT_FS_NAME,
                        &dev_in_path,
                        FTPD_DEVLEN,
                        (char *)FTPd_rootdir,
                        NULL);
                        
            session_ptr->CURRENT_FS = _io_get_fs_by_name(session_ptr->CURRENT_FS_NAME);
            
            error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_CHECK_DIR_EXIST,(pointer)FTPd_rootdir );  
            
            if (error)  {
#endif            
               session_ptr->CURRENT_FS = NULL;
               session_ptr->CURRENT_FTP_DIR = NULL;
               session_ptr->CURRENT_FS_DIR[0] = '\0';
#if FTPDCFG_USES_MFS               
            } else {
               // set current fs dir (including root dir)
               strcpy(session_ptr->CURRENT_FS_DIR,FTPd_rootdir+devlen);
               
               rootdirlen = strlen(session_ptr->CURRENT_FS_DIR);

               // set current FTP dir 
               session_ptr->CURRENT_FTP_DIR = session_ptr->CURRENT_FS_DIR + rootdirlen - 1; 
               
               //check if there is / at the end of root dir name
               if(*(session_ptr->CURRENT_FTP_DIR) != '\\' &&
                  *(session_ptr->CURRENT_FTP_DIR) != '/') 
               {
                  session_ptr->CURRENT_FTP_DIR++;
               } 
             session_ptr->CURRENT_FTP_DIR[0] = '\\';
             session_ptr->CURRENT_FTP_DIR[1] = '\0';
            } 
#endif                     
            #if FTPDCFG_ENABLE_MULTIPLE_CLIENTS
               /* Create a task to look after it */
               RTCS_detachsock(sock);
               t_ptr = _task_get_template_ptr(MQX_NULL_TASK_ID); 
               if (RTCS_task_create("FTPd_child", t_ptr->TASK_PRIORITY, t_ptr->TASK_STACKSIZE, FTPd_child, (pointer) session_ptr) != RTCS_OK ) {
                  RTCS_attachsock(sock);
                  shutdown(sock, FLAG_ABORT_CONNECTION);
                  _mem_free(session_ptr->DATA_BUFFER_PTR);
                  _mem_free(session_ptr);
               }
            #else
               FTPd_child((pointer) session_ptr,0);
            #endif

         }         
      }
   } 
} 


/*TASK*-----------------------------------------------------------------
*
* Function Name    : FTPd_child
* Returned Value   : none
* Comments  :  FTP server.
*
*END*-----------------------------------------------------------------*/

void FTPd_child
   (
      pointer  session,
      pointer  creator
   )
{ /* Body */
   FTPd_SESSION_PTR           session_ptr = (FTPd_SESSION_PTR) session;
   FTPd_COMMAND_PTR           command_ptr;
   char_ptr                   cmd_ptr, arg_ptr, ptr;
   int_32                     len;
   uint_32                    i,bi,ci,arglen;
   boolean                    got_a_line;
   uint_32                    iplen;


   #if FTPDCFG_ENABLE_MULTIPLE_CLIENTS
   _mem_transfer( session, _task_get_creator(), _task_get_id() ); 
   _mem_transfer( session_ptr->DATA_BUFFER_PTR, _task_get_creator(), _task_get_id() );
   RTCS_task_resume_creator(creator, RTCS_OK);
   #endif  
   
   iplen = sizeof(_ip_address);
   getsockopt(session_ptr->CONTROL_SOCK, SOL_IP, RTCS_SO_IP_LOCAL_ADDR, &session_ptr->LOCAL_ADDR, &iplen);
   

   FTPd_send_msg(session_ptr, ftpmsg_banner);

   bi=0;
   len=0;   
   while (session_ptr->CONNECTED) {
      got_a_line = FALSE;
      ci=0;
      while (session_ptr->CONNECTED && !got_a_line) {
         if (bi>=(uint_32)len)  {
            len = recv(session_ptr->CONTROL_SOCK, session_ptr->CMD_BUFFER, 
               sizeof(session_ptr->CMD_BUFFER), 0);    
            if (len == RTCS_ERROR) {
               session_ptr->CONNECTED = FALSE;
            } else  {
               bi=0;
            }
         } else  {
            session_ptr->CMD_LINE[ci++] = session_ptr->CMD_BUFFER[bi++];
            if (session_ptr->CMD_BUFFER[bi] == '\n')  {
               session_ptr->CMD_LINE[ci++] = 0; 
               got_a_line=TRUE;  
            }
         }
         if (ci >= FTPD_LINELEN) {
               got_a_line=TRUE;  
         }
      }
      
      if (session_ptr->CONNECTED)  {
         if (*session_ptr->CMD_LINE == '\0') continue;
   
         ptr = session_ptr->CMD_LINE;
         while (*ptr && ! isgraph(*ptr)) ptr++;
         cmd_ptr = ptr;
         while (*ptr && isgraph(*ptr)) ptr++;
         if (*ptr)  {
            *ptr++ = '\0';
         
            while (*ptr && ! isgraph(*ptr)) ptr++;
            arg_ptr = ptr;
          //  while (*ptr && isgraph(*ptr)) ptr++;
          //  *ptr++ = '\0';
         } else  {
            arg_ptr = ptr;
         }
         if (arg_ptr) {
            arglen = strlen(arg_ptr);
            while (arglen > 0) {
               if ((arg_ptr[arglen-1] == '\r') || (arg_ptr[arglen-1] == '\n')) {
                  arg_ptr[arglen-1] = 0;
                  arglen--;
               } else {
                  break;
               }
            }
         }
         
         if (*cmd_ptr != '\0') { 
      
            i=0;
            while (cmd_ptr[i])  {
               cmd_ptr[i] = (char)tolower(cmd_ptr[i]);
               i++;
            }
      
            command_ptr = (FTPd_COMMAND_PTR) FTPd_commands; 
            while (command_ptr->COMMAND != NULL)  {
               if (strcmp(cmd_ptr, command_ptr->COMMAND) == 0)  {
                  (void)(*command_ptr->FTPd_FUNC)(session_ptr, cmd_ptr, arg_ptr);
                  break;   
               }
               command_ptr++;        
            } /* Endwhile */
            if (command_ptr->COMMAND == NULL)  {
               FTPd_send_msg(session_ptr, ftpmsg_badcmd);
            } /* Endif */
         } /* Endif */
      }
   } /* Endwhile */

   shutdown(session_ptr->CONTROL_SOCK, FTPDCFG_SHUTDOWN_OPTION );
   _mem_free(session_ptr->DATA_BUFFER_PTR);
   _mem_free(session_ptr); 
} /* Endbody */


#ifdef __MQX__ 
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPd_stop
* Returned Value   : error code
* Comments  :  Start the FTPd task.
*
*END*-----------------------------------------------------------------*/

uint_32 FTPd_stop( void )
{ /* Body */
   if (FTPd_task_id == 0)  {
      return RTCSERR_SERVER_NOT_RUNNING;
   }
   RTCS_task_abort(FTPd_task_id);
   FTPd_task_id = 0;
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTPd_Exit_handler
* Returned Value   : error code
* Comments  :  Stop the FTPd task.
*
*END*-----------------------------------------------------------------*/

void FTPd_Exit_handler( void )
{ /* Body */
   FTPd_CONTEXT_PTR    ftpd_context_ptr;
   
   ftpd_context_ptr = (FTPd_CONTEXT_PTR) _task_get_environment( _task_get_id() );
   if (ftpd_context_ptr != NULL)  {
      if (ftpd_context_ptr->LISTENSOCK)  {
         shutdown(ftpd_context_ptr->LISTENSOCK, FLAG_ABORT_CONNECTION);   
         ftpd_context_ptr->LISTENSOCK = 0;   
      }   
   }
   FTPd_task_id = 0;
} /* Endbody */
#endif

void FTPd_send_msg(pointer p, const char * m)
{
   send((((FTPd_SESSION_PTR)p)->CONTROL_SOCK), (pointer)m, strlen(m), 0);
}
/* EOF */
