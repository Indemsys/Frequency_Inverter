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
* $FileName: ftpd_cmd.c$
* $Version : 3.8.28.0$
* $Date    : Aug-7-2012$
*
* Comments:
*
*   This file contains an implementation of an
*   FTP server.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <rtcs.h>
#include <fio.h>
#include "ftpd.h"
#include "ftpd_msg.h"

#if FTPDCFG_USES_MFS
#include <mfs.h>
#endif

#define hexnum(c)  ((c<='9')?(c-'0'):((c<='F')?(c-'A'+10) :(c-'a'+10))) 

// Set externally to define a user file
char_ptr FTPd_userfile = NULL;
#define fs_get_default( ) session_ptr->FS

static uint_16  FTPd_pasv_port = FTPD_PASV_PORT_START;

/*
static char_ptr months[] =  
{
   "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
};
*/

static void FTPd_convert_percents( char_ptr arg_ptr)
{
   char_ptr   sptr = arg_ptr, dptr = arg_ptr;
   
   while (*sptr)  {
      if ((*sptr == '%') && isxdigit(sptr[1]) && isxdigit(sptr[2]))  {
         *dptr++ = (hexnum(sptr[1]) << 4) + hexnum(sptr[2]);
         sptr+=3;
      } else  {
         *dptr++ = *sptr++;
      }
   }
   *dptr = '\0';
}

static char_ptr FTPd_strip_path_delimiters( char_ptr arg_ptr)
{
   char_ptr   ptr;
   uint_32    len;
    
   ptr = arg_ptr;
   len = strlen(ptr);
   if (len > 2)  {
      if ((ptr[0] == '/') &&  (ptr[1] == '\\') && (ptr[2] == '/'))  {
         ptr +=2;
         len -=2;
      }
   }
   if (len > 2)  {
      if (ptr[len-1] == '/') ptr[len-1] = '\0';   
   }
   
   return ptr;
}


#if FTPDCFG_USES_MFS

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_cd
* Returned Value   :  int_32 error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  cd <directory>
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_cd(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   int_32      error;
   char_ptr    ptr;
   char_ptr    abs_path = NULL;
   uint_16     rootdir_len,fsname_len;
   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   

   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   } 

   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);

   if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }
   
   // convert to absolute path
   error = _io_rel2abs(abs_path,
                      session_ptr->CURRENT_FS_DIR,
                      ptr,
                      min(PATHNAME_SIZE, FTPD_PATHLEN),
                      session_ptr->CURRENT_FS_NAME);   
   
   // check if path exist
   if(!error)
   {
     error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_CHECK_DIR_EXIST,(pointer)abs_path );
   }

   // calculate root dir length
   rootdir_len = session_ptr->CURRENT_FTP_DIR - session_ptr->CURRENT_FS_DIR;
   fsname_len = strlen(session_ptr->CURRENT_FS_NAME);
   
   // check if there is an error and  check if we are still uder FTP root   
   if (error || strncmp(session_ptr->CURRENT_FS_DIR,abs_path + fsname_len,rootdir_len)) {
      FTPd_send_msg(session_ptr, ftpmsg_cd_error);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   }

   // change current dir(skip device name)
   strcpy(session_ptr->CURRENT_FS_DIR,abs_path+(strlen(session_ptr->CURRENT_FS_NAME)));

   MFS_free_path(abs_path);
   return FTPd_pwd(session_ptr,"pwd" , "");
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_cdup
* Returned Value   :  int_32 error code
* Comments  :  change to parent directory.
*
* Usage:  cdup
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_cdup(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   char tmp_cmd[] = "..";
   return FTPd_cd(session_ptr,cmd_ptr,tmp_cmd);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_feat
* Returned Value   :  int_32 error code
* Comments  :  Returns supported feature.
*
* Usage:  feat
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_feat(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ 
   FTPd_send_msg(session_ptr, "211-Features:\r\n");
   FTPd_send_msg(session_ptr, " SIZE\r\n");
   FTPd_send_msg(session_ptr, "211 End\r\n");

   return FTPd_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_dele
* Returned Value   :  int_32 error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  dele <file>
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_dele(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   int_32      error;
   char_ptr    ptr,abs_path = NULL;

   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);
 
   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   } 

   if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }

   error = _io_rel2abs(abs_path,
                      session_ptr->CURRENT_FS_DIR,
                      ptr,
                      min(PATHNAME_SIZE, FTPD_PATHLEN),
                      session_ptr->CURRENT_FS_NAME);       
   if(!error)
   {
      error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_DELETE_FILE, (pointer) abs_path);
   }
   
   if(error)  {
      FTPd_send_msg(session_ptr, ftpmsg_delete_error );
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   }

   sprintf(session_ptr->DATA_BUFFER_PTR, "257 \"%s\" deleted.\r\n",arg_ptr);
   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);
   MFS_free_path(abs_path);
   return FTPd_OK;
}

#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_help
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

#define FTPD_HELP_SPACE_NEEDED 16
int_32  FTPd_help(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{
   FTPd_COMMAND_PTR     command_ptr; 
   uint_32              i,len;
   
   FTPd_send_msg(session_ptr, ftpmsg_help_start);
   
   command_ptr = (FTPd_COMMAND_PTR) FTPd_commands; 
   for (i=0,len=0; command_ptr->COMMAND != NULL; i++,command_ptr++)   {
      // make sure there is room.
      if ((len +FTPD_HELP_SPACE_NEEDED) > session_ptr->DATA_BUFFER_SIZE) {
         FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);
         len = 0;
      }
   
      len += sprintf(&session_ptr->DATA_BUFFER_PTR[len],"%-10s ", command_ptr->COMMAND);
      if ((i%6)==5) len+= sprintf(&session_ptr->DATA_BUFFER_PTR[len],"\n");        
   } /* Endwhile */        
   if ((i%6)!=0) len+= sprintf(&session_ptr->DATA_BUFFER_PTR[len],"\n");        
   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);

   FTPd_send_msg(session_ptr, ftpmsg_help_end);

   return FTPd_OK;
}



#if FTPDCFG_USES_MFS

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_list
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_list(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   uint_32              data_sock;
   int_32               len, error;
   char_ptr             path_ptr;
   pointer              dir_ptr;
   

#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   if (*arg_ptr == '\0')  {
      path_ptr = "*.*";
   } else  {
      FTPd_convert_percents(arg_ptr);
      path_ptr = FTPd_strip_path_delimiters(arg_ptr);
   }
   
   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   } 
   
   error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_CHANGE_CURRENT_DIR, session_ptr->CURRENT_FS_DIR);
   if(error)
   {
      FTPd_send_msg(session_ptr, ftpmsg_cd_error);
      return FTPd_ERROR;
   }   
   
   dir_ptr = _io_mfs_dir_open( session_ptr->CURRENT_FS, path_ptr, "u*");
   
   if (dir_ptr == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_file);
      return FTPd_ERROR;
   } 
   
   FTPd_send_msg(session_ptr, ftpmsg_opening_datacon);
   if (session_ptr->PASV_MODE)  {
      data_sock = session_ptr->PASV_SOCK;
   } else  {
      data_sock = FTPd_open_active_data_connection(session_ptr);
   }
   
   if (data_sock == RTCS_SOCKET_ERROR)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_datacon);
      return FTPd_ERROR;
   } 
   
   while ((len = _io_mfs_dir_read(dir_ptr, session_ptr->DATA_BUFFER_PTR, session_ptr->DATA_BUFFER_SIZE))>0)  {
      if (send(data_sock, session_ptr->DATA_BUFFER_PTR, len, 0) != len) { 
         FTPd_send_msg(session_ptr, ftpmsg_writefail);
         break;
      }
   } 
         
   _io_mfs_dir_close(dir_ptr);      
 
   shutdown(data_sock, FTPDCFG_DATA_SHUTDOWN_OPTION );
   session_ptr->PASV_MODE = FALSE;
   session_ptr->PASV_SOCK = RTCS_SOCKET_ERROR;
   FTPd_send_msg(session_ptr, ftpmsg_tx_complete);
            
   return FTPd_OK;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_mkdir
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  mkdir <directory>
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_mkdir(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   int_32      error;
   char_ptr    path_ptr,abs_path = NULL;

   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   FTPd_convert_percents(arg_ptr);
   path_ptr = FTPd_strip_path_delimiters(arg_ptr);
   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   } 

   if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }

   error = _io_rel2abs(abs_path,
                      session_ptr->CURRENT_FS_DIR,
                      path_ptr,
                      min(PATHNAME_SIZE, FTPD_PATHLEN),
                      session_ptr->CURRENT_FS_NAME);       
   if(!error)
   {
      error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_CREATE_SUBDIR, (pointer) abs_path);
   }

   if(error)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   }
      
   sprintf(session_ptr->DATA_BUFFER_PTR, "257 \"%s\" directory created.\r\n",arg_ptr);
   FTPd_send_msg(session_ptr,  session_ptr->DATA_BUFFER_PTR );

   MFS_free_path(abs_path);

   return FTPd_OK;
}

   
   
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_nlst
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_nlst(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   uint_32              data_sock;
   int                  len;
   char_ptr             path_ptr;
   pointer              dir_ptr;
   

#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   if (*arg_ptr == '\0')  {
      path_ptr = "*.*";
   } else  {
      FTPd_convert_percents(arg_ptr);
      path_ptr = FTPd_strip_path_delimiters(arg_ptr);
   }
   
   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   } 

   ioctl(session_ptr->CURRENT_FS, IO_IOCTL_CHANGE_CURRENT_DIR, session_ptr->CURRENT_FS_DIR);
   
   dir_ptr = _io_mfs_dir_open(session_ptr->CURRENT_FS,path_ptr, "f*");
   
   if (dir_ptr == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_file);
      return FTPd_ERROR;
   } 
   
   FTPd_send_msg(session_ptr, ftpmsg_opening_datacon);
   if (session_ptr->PASV_MODE)  {
      data_sock = session_ptr->PASV_SOCK;
   } else  {
      data_sock = FTPd_open_active_data_connection(session_ptr);
   }
   
   if (data_sock == RTCS_SOCKET_ERROR)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_datacon);
      return FTPd_ERROR;
   } 
   
   while ((len = _io_mfs_dir_read(dir_ptr, session_ptr->DATA_BUFFER_PTR, session_ptr->DATA_BUFFER_SIZE))>0)  {
      send(data_sock, session_ptr->DATA_BUFFER_PTR, len, 0);
   } 
         
   _io_mfs_dir_close(dir_ptr);      
 
   shutdown(data_sock, FTPDCFG_DATA_SHUTDOWN_OPTION );
   session_ptr->PASV_MODE = FALSE;
   session_ptr->PASV_SOCK = RTCS_SOCKET_ERROR;
   FTPd_send_msg(session_ptr, ftpmsg_tx_complete);
   
   return FTPd_OK;
} /* Endbody */


#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_noop
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_noop(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{
   FTPd_send_msg(session_ptr, ftpmsg_ok);
   return FTPd_OK;
}



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_opts
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_opts(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{
   FTPd_send_msg(session_ptr, ftpmsg_bad_option);
   return FTPd_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_pass
* Returned Value   :  int_32 error code
* Comments  :  .
*
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_pass(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */

#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE == FTPD_NEED_PASSWORD)  {
      if (strcmp(arg_ptr, session_ptr->PASSWORD) == 0)  {
         session_ptr->LOGIN_STATE = FTPD_LOGGED_IN;
         session_ptr->LOGIN_ATTEMPTS = 0;
      } else  {
         session_ptr->LOGIN_STATE = FTPD_LOGGED_OUT;
         session_ptr->LOGIN_ATTEMPTS++;
         if (session_ptr->LOGIN_ATTEMPTS >= FTPD_MAX_LOGIN_ATTEMPTS)  {
            session_ptr->CONNECTED = FALSE;   
            FTPd_send_msg(session_ptr, ftpmsg_bye); 
            return FTPd_ERROR;  
         }
      }
   }
   
   if (session_ptr->LOGIN_STATE == FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_logged);   
   } else  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);   
   }
#else
   FTPd_send_msg(session_ptr, ftpmsg_logged);   
#endif
   return FTPd_OK;
}   

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_pasv
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_pasv(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   session_ptr->SERVER_PASV_SOCKADDR.sin_family      = AF_INET;
   session_ptr->SERVER_PASV_SOCKADDR.sin_addr.s_addr =  session_ptr->LOCAL_ADDR;
   session_ptr->SERVER_PASV_SOCKADDR.sin_port = FTPd_pasv_port++;
   if (FTPd_pasv_port == FTPD_PASV_PORT_END)  {
      FTPd_pasv_port = FTPD_PASV_PORT_START;   
   }
   
   sprintf(session_ptr->DATA_BUFFER_PTR, ftpmsg_pasv_mode,
      ((session_ptr->SERVER_PASV_SOCKADDR.sin_addr.s_addr >> 24) & 0xff),
      ((session_ptr->SERVER_PASV_SOCKADDR.sin_addr.s_addr >> 16) & 0xff),
      ((session_ptr->SERVER_PASV_SOCKADDR.sin_addr.s_addr >> 8) & 0xff),
      (session_ptr->SERVER_PASV_SOCKADDR.sin_addr.s_addr & 0xff),
      ((session_ptr->SERVER_PASV_SOCKADDR.sin_port >> 8) & 0xff),
      (session_ptr->SERVER_PASV_SOCKADDR.sin_port & 0xff));

   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);
       
   if (FTPd_open_passive_data_connection(session_ptr) != RTCS_SOCKET_ERROR) {
      session_ptr->PASV_MODE = TRUE;
   } else  {
      FTPd_send_msg(session_ptr, ftpmsg_no_datacon);
   }
   
   return FTPd_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_port
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_port(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
/* Begin CR 2302 */
   uint_32     i,n;
   uint_16     p;
/* End CR 2302 */
   char_ptr    arg;


#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   n = 0;
   arg = arg_ptr;
   for ( i=0; i<4; i++ ) {
      n = _io_atoi(arg) + (n << 8);
      if ( (arg = strpbrk(arg, ",")) == NULL ) {
         FTPd_send_msg(session_ptr,  ftpmsg_badport );
         return FTPd_ERROR;
      } /* Endif */
      arg++;
   } /* Endfor */
   p = (uint_16) _io_atoi(arg);
   if ( (arg = strpbrk(arg, ",")) == NULL ) {
      FTPd_send_msg(session_ptr, ftpmsg_badport );
      return FTPd_ERROR;
   } /* Endif */
   arg++;
   p = (uint_16) _io_atoi(arg) + (p << 8);
   
   session_ptr->USER_DATA_SOCKADDR.sin_family = AF_INET;
   session_ptr->USER_DATA_SOCKADDR.sin_addr.s_addr = n;
   session_ptr->USER_DATA_SOCKADDR.sin_port = p;
   FTPd_send_msg(session_ptr, ftpmsg_portok );
   
   return FTPd_OK;
} /* Endbody */


#if FTPDCFG_USES_MFS

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_pwd
* Returned Value   :  int_32 error code
* Comments  :  .
*
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_pwd(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   }
   
   sprintf(session_ptr->DATA_BUFFER_PTR, "257 \"%s\" is the current directory\n", session_ptr->CURRENT_FTP_DIR);
   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);
   return FTPd_OK;
}
#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_quit
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_quit(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   session_ptr->LOGIN_STATE = FTPD_LOGGED_OUT;
   session_ptr->PASSWORD[0] = '\0';
#endif
   FTPd_send_msg(session_ptr, ftpmsg_goodbye);
   session_ptr->CONNECTED = FALSE;
   return FTPd_OK;
}


#if FTPDCFG_USES_MFS

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_retr
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_retr(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   MQX_FILE_PTR      fd;
   uint_32           data_sock;
   uint_32           start_time, end_time, bytes_moved;
   int_32            count,error;
   char_ptr          ptr, abs_path = NULL;


#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);

   if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }
   error = _io_rel2abs(abs_path,
                      session_ptr->CURRENT_FS_DIR,
                      ptr,
                      min(PATHNAME_SIZE, FTPD_PATHLEN),
                      session_ptr->CURRENT_FS_NAME);       
   if(error)
   {
      FTPd_send_msg(session_ptr, ftpmsg_no_file);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   }

   fd = fopen(abs_path, "r");
   if (fd == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_file);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   }
   

   FTPd_send_msg(session_ptr, ftpmsg_opening_datacon);
   if (session_ptr->PASV_MODE)  {
      data_sock = session_ptr->PASV_SOCK;
   } else  {
      data_sock = FTPd_open_active_data_connection(session_ptr);
      #if 0
         sprintf(session_ptr->DATA_BUFFER_PTR, ftpmsg_sending,
                 (FTPd_user_data_port.sin_addr.s_addr >> 24) & 0xFF,
                 (FTPd_user_data_port.sin_addr.s_addr >> 16) & 0xFF,
                 (FTPd_user_data_port.sin_addr.s_addr >>  8) & 0xFF,
                  FTPd_user_data_port.sin_addr.s_addr        & 0xFF,
                   FTPd_user_data_port.sin_port);
         FTPd_send_msg(session_ptr,  session_ptr->DATA_BUFFER_PTR );
      #endif
   }
   if (data_sock == RTCS_SOCKET_ERROR)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_datacon);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   } 

   bytes_moved = 0;
   start_time = RTCS_time_get();
   do  {
      count = read(fd, session_ptr->DATA_BUFFER_PTR, session_ptr->DATA_BUFFER_SIZE);
      if (count > 0)  {
         count = send(data_sock, session_ptr->DATA_BUFFER_PTR, count, 0);
      }
      bytes_moved += count;
   } while (count > 0);
   
   end_time = RTCS_time_get();

   shutdown(data_sock, FTPDCFG_DATA_SHUTDOWN_OPTION);

   sprintf(session_ptr->DATA_BUFFER_PTR, ftpmsg_txok, bytes_moved,
      RTCS_timer_get_interval(start_time, end_time) / 1000,
      RTCS_timer_get_interval(start_time, end_time) % 1000);
   FTPd_send_msg(session_ptr,  session_ptr->DATA_BUFFER_PTR );

   fclose(fd);
   MFS_free_path(abs_path);
   return FTPd_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_rmdir
* Returned Value   :  int_32 error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  rmdir <directory>
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_rmdir(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   int_32      error;
   char_ptr    ptr,abs_path = NULL;
   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);

   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   } 

   if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }

   error = _io_rel2abs(abs_path,
                      session_ptr->CURRENT_FS_DIR,
                      ptr,
                      min(PATHNAME_SIZE, FTPD_PATHLEN),
                      session_ptr->CURRENT_FS_NAME);       
   // change mfs internal current dir
   ioctl(session_ptr->CURRENT_FS, IO_IOCTL_CHANGE_CURRENT_DIR, session_ptr->CURRENT_FS_DIR);
   
   if(!error)
   {
      error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_REMOVE_SUBDIR, (pointer) abs_path);
   }

   if(error)  {
      FTPd_send_msg(session_ptr, ftpmsg_rmdir_error );
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   } 
   
   sprintf(session_ptr->DATA_BUFFER_PTR, "257 \"%s\" directory removed.\r\n",arg_ptr);
   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);

   MFS_free_path(abs_path);
   return FTPd_OK;
}

#if FTPDCFG_ENABLE_RENAME

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_rnfr
* Returned Value   :  int_32 error code
* Comments  :  specify rename from file.
*
* Usage:  rnfr <file>
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_rnfr(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   int_32      error;
   char_ptr    ptr;
   MFS_FILE_ATTR_PARAM  param;
   uchar attr;


   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   }
    
   if (session_ptr->RENAME_FROM) {
      MFS_free_path(session_ptr->RENAME_FROM);
      session_ptr->RENAME_FROM=NULL;
   }
   
   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   
   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);


   if (MFS_alloc_path(&session_ptr->RENAME_FROM) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }

   error = _io_rel2abs(session_ptr->RENAME_FROM,
                     session_ptr->CURRENT_FS_DIR,
                     ptr,
                     min(PATHNAME_SIZE, FTPD_PATHLEN),
                     session_ptr->CURRENT_FS_NAME); 
   param.PATHNAME = session_ptr->RENAME_FROM;
   param.ATTRIBUTE_PTR = &attr;
   error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_GET_FILE_ATTR, (pointer) &param);
   if(error)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_file );
      return FTPd_ERROR;
   } 



   sprintf(session_ptr->DATA_BUFFER_PTR, "350 File exists, ready for destination name.\r\n");
   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);
   return FTPd_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_rnto
* Returned Value   :  int_32 error code
* Comments  :  specify rename to file.
*
* Usage:  rnto <file>
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_rnto(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   int_32            error;
   char_ptr          ptr, abs_path ;
   MFS_RENAME_PARAM  param;

   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   

   if (session_ptr->RENAME_FROM==NULL) {
      FTPd_send_msg(session_ptr, "550 Rename from file not specified.\r\n");
      return FTPd_ERROR;
   }
   
   
   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);

  if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      MFS_free_path(session_ptr->RENAME_FROM);
      session_ptr->RENAME_FROM = NULL;      
      return FTPd_ERROR;
   }

   error = _io_rel2abs(abs_path,
                     session_ptr->CURRENT_FS_DIR,
                     ptr,
                     min(PATHNAME_SIZE, FTPD_PATHLEN),
                     session_ptr->CURRENT_FS_NAME); 

   param.OLD_PATHNAME = session_ptr->RENAME_FROM;
   param.NEW_PATHNAME = abs_path;
   error = ioctl(session_ptr->CURRENT_FS, IO_IOCTL_RENAME_FILE, (pointer) &param);
   
   if(error)  {
      sprintf(session_ptr->DATA_BUFFER_PTR, "550 unable to rename \"%s\" to %s.\r\n",session_ptr->RENAME_FROM,arg_ptr);
      error = FTPd_ERROR;
   } else {
      sprintf(session_ptr->DATA_BUFFER_PTR, "250 \"%s\" renamed to \"%s\".\r\n",session_ptr->RENAME_FROM,arg_ptr);
      error = FTPd_OK;
   } 

   MFS_free_path(session_ptr->RENAME_FROM);
   session_ptr->RENAME_FROM = NULL;
   MFS_free_path(abs_path);   

   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);
   return error;
}

#endif
#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_site
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_site(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{

   FTPd_send_msg(session_ptr, ftpmsg_site_info);
   return FTPd_OK;
}


#if FTPDCFG_USES_MFS

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_size
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_size(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   MQX_FILE_PTR         fp;
   char_ptr             ptr,abs_path;
   int_32               size=0,error;
   
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);
   
   if (session_ptr->CURRENT_FS == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_fs );
      return FTPd_ERROR;
   } 


   if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }

   error = _io_rel2abs(abs_path,
                      session_ptr->CURRENT_FS_DIR,
                      ptr,
                      min(PATHNAME_SIZE, FTPD_PATHLEN),
                      session_ptr->CURRENT_FS_NAME);       
   if(error)
   {
      FTPd_send_msg(session_ptr, ftpmsg_no_file );
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   } 

   fp = fopen(abs_path,"r");
   if (fp==NULL) {
      FTPd_send_msg(session_ptr, ftpmsg_no_file );
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   } 

   fseek(fp,0,IO_SEEK_END);
   size = ftell(fp);
   fclose(fp);

   if (size == IO_ERROR)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_file);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   } 
   
   sprintf (session_ptr->DATA_BUFFER_PTR, ftpmsg_size,  size );
   FTPd_send_msg(session_ptr, session_ptr->DATA_BUFFER_PTR);
   
   MFS_free_path(abs_path);
   return FTPd_OK;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_retr
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_stor(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{ /* Body */
   MQX_FILE_PTR fd;
   uint_32     data_sock,error;
   uint_32     start_time, end_time, bytes_moved;
   int_32      rx_count, write_count=0;
   char_ptr    ptr,abs_path;

#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   FTPd_convert_percents(arg_ptr);
   ptr = FTPd_strip_path_delimiters(arg_ptr);

   if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
      FTPd_send_msg(session_ptr, ftpmsg_no_memory );
      return FTPd_ERROR;
   }
   error = _io_rel2abs(abs_path,
                      session_ptr->CURRENT_FS_DIR,
                      ptr,
                      min(PATHNAME_SIZE, FTPD_PATHLEN),
                      session_ptr->CURRENT_FS_NAME);       
   if(error)
   {
      FTPd_send_msg(session_ptr, ftpmsg_no_file);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   }


   fd = fopen(abs_path, "w");
   if (fd == NULL)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_file);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   }
   
   FTPd_send_msg(session_ptr, ftpmsg_opening_datacon);
   if (session_ptr->PASV_MODE)  {
      data_sock = session_ptr->PASV_SOCK;
   } else  {
      data_sock = FTPd_open_active_data_connection(session_ptr);
   }
   if (data_sock == RTCS_SOCKET_ERROR)  {
      FTPd_send_msg(session_ptr, ftpmsg_no_datacon);
      MFS_free_path(abs_path);
      return FTPd_ERROR;
   } 
   MFS_free_path(abs_path);
   
   bytes_moved = 0;
   start_time = RTCS_time_get();
   do {
         
      rx_count = recv(data_sock, session_ptr->DATA_BUFFER_PTR, session_ptr->DATA_BUFFER_SIZE, 0);
      
      if (rx_count > 0)  {
         write_count = write(fd, session_ptr->DATA_BUFFER_PTR, rx_count );
         if (write_count != rx_count)  {
            shutdown(data_sock, FLAG_ABORT_CONNECTION);
            data_sock = RTCS_SOCKET_ERROR;
            if (write_count == IO_ERROR ) {
               FTPd_send_msg(session_ptr, ftpmsg_writefail);
            } else {
               FTPd_send_msg(session_ptr, ftpmsg_writeeof);
            }
            break;
         }
         bytes_moved += write_count;
      }
   } while ((rx_count > 0) && (write_count == rx_count));
   
   if (data_sock != RTCS_SOCKET_ERROR) {
      end_time = RTCS_time_get();
      shutdown(data_sock, FLAG_CLOSE_TX);
      sprintf(session_ptr->DATA_BUFFER_PTR, ftpmsg_txok, bytes_moved, RTCS_timer_get_interval(start_time, end_time) / 1000, RTCS_timer_get_interval(start_time, end_time) % 1000);
      FTPd_send_msg(session_ptr,  session_ptr->DATA_BUFFER_PTR );
   }

   fclose(fd);

   return FTPd_OK;
} /* Endbody */

#endif

   
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_syst
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_syst(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{
   FTPd_send_msg(session_ptr, ftpmsg_syst);

   return FTPd_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_type
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_type(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{

#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   if (session_ptr->LOGIN_STATE != FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      return FTPd_ERROR;
   }
#endif   
   
   switch ( *arg_ptr ) {
      case 'a':   /* Ascii */
      case 'A':   /* Ascii */
         FTPd_send_msg(session_ptr, ftpmsg_type_ascii );
         break;
      case 'b':   /* Binary */
      case 'B':   /* Binary */
      case 'i':   /* Image */
      case 'I':   /* Image */
         FTPd_send_msg(session_ptr, ftpmsg_type_image );
         break;
      default:   /* Invalid */
         FTPd_send_msg(session_ptr, ftpmsg_type_unknown );
         return FTPd_ERROR;
   } /* End Switch */

   return FTPd_OK;
}




/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_user
* Returned Value   :  int_32 error code
* Comments  :  
*
* Usage:  
*
*END*---------------------------------------------------------------------*/
   
int_32  FTPd_user(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{

#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD && FTPDCFG_USES_MFS
/* Begin CR 2296 */
   uint_32      i=0,j,error;
/* End CR 2296 */
   boolean      found = FALSE;
   MQX_FILE_PTR fd;
   char_ptr     ptr;
   char_ptr     abs_path = NULL;
   
   ptr = session_ptr->DATA_BUFFER_PTR;

   // Any attempt to log in logs previous user out.
   session_ptr->LOGIN_STATE = FTPD_LOGGED_OUT;
   session_ptr->PASSWORD[0] = '\0';

   if (FTPd_userfile != NULL)  {
  
      if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
         FTPd_send_msg(session_ptr, ftpmsg_no_memory );
         FTPd_send_msg(session_ptr, ftpmsg_bye); 
         return FTPd_ERROR;
      }
      error = _io_rel2abs(abs_path,
                         session_ptr->CURRENT_FS_DIR,
                         FTPd_userfile,
                         min(PATHNAME_SIZE, FTPD_PATHLEN),
                         session_ptr->CURRENT_FS_NAME);       
      if(error)
      {
         FTPd_send_msg(session_ptr, ftpmsg_bye);
         MFS_free_path(abs_path);
         return FTPd_ERROR;  
      }
      
      fd=fopen(abs_path, "r");
      if (fd)  {
         while (fgets(ptr, FTPD_LINELEN, fd))  {
         
            if (*ptr == '\0') continue;
            for (i=0;(i<FTPD_LINELEN) && (ptr[i] != ':') && (ptr[i] != '\0'); i++)  {
            }
            if ((arg_ptr[i]=='\0') && (strncmp(ptr, arg_ptr, i)==0))  {
               found = TRUE;
               break;
            }
         }
         if (found)  {
            strncpy(session_ptr->USERNAME,&ptr[0], i); 
            session_ptr->USERNAME[i]='\0';
            //check if there is only username or also password (:)
            if (session_ptr->DATA_BUFFER_PTR[i] == '\0')  {
               session_ptr->LOGIN_STATE = FTPD_LOGGED_IN;
            } else  {
               //there is : delimiter - check if there is valid password
               for (j=i+1;(j<FTPD_LINELEN) && (ptr[j] != ':') && (ptr[j] != '\0'); j++)  {
               }
               if (j==(i+1))  {
                  session_ptr->LOGIN_STATE = FTPD_LOGGED_IN;
               } else  {
                  session_ptr->LOGIN_STATE = FTPD_NEED_PASSWORD;
                  strncpy(session_ptr->PASSWORD,&ptr[i+1], j-i-1); 
                  session_ptr->PASSWORD[j-i-1]='\0';
               }
               // todo - here add code for FTP ROOTDIR
            }
         } else  {
            session_ptr->LOGIN_ATTEMPTS++;
            if (session_ptr->LOGIN_ATTEMPTS >= FTPD_MAX_LOGIN_ATTEMPTS)  {
               session_ptr->CONNECTED = FALSE;   
               FTPd_send_msg(session_ptr, ftpmsg_bye);
               MFS_free_path(abs_path);
               return FTPd_ERROR;  
            }
         } 
         fclose(fd);     
      }
   } else  {
      session_ptr->LOGIN_STATE = FTPD_LOGGED_IN;
   }
   
   if (session_ptr->LOGIN_STATE == FTPD_LOGGED_IN)  {
      FTPd_send_msg(session_ptr, ftpmsg_logged);
   } else  {
      if (session_ptr->LOGIN_STATE == FTPD_NEED_PASSWORD)  {
         FTPd_send_msg(session_ptr, ftpmsg_need_password);
      } else  {
         FTPd_send_msg(session_ptr, ftpmsg_not_logged);
      }
   }
   if (abs_path != NULL)
   {
      MFS_free_path(abs_path);
   }  
#else
   FTPd_send_msg(session_ptr, ftpmsg_logged);
#endif // FTPDCFG_ENABLE_USERNAME_AND_PASSWORD || FTPDCFG_USES_MFS

      return FTPd_OK;
}



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPd_unimplemented
* Returned Value   :  int_32 error code
* Comments  :  .
*
*
*END*---------------------------------------------------------------------*/

int_32  FTPd_unimplemented(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr )
{
   FTPd_send_msg(session_ptr, ftpmsg_unimp);
   return FTPd_ERROR;
}

/*FUNCTION*--------------------------------------------------------------
*
*Function Name  :FTPd_open_active_data_connection
*Returned Value :Boolean Operation Successful or not
*Comments       :
*   This function opens the DataConnection to send the list Information
*
*END*---------------------------------------------------------------------*/

uint_32 FTPd_open_active_data_connection(FTPd_SESSION_PTR session_ptr)
{
   uint_32      data_connection;
   uint_32      error_code; 
   uint_32      option;
  
   data_connection = socket(PF_INET, SOCK_STREAM, 0);

   if (data_connection != RTCS_SOCKET_ERROR) {

      option = FTPd_window_size;   
      error_code = setsockopt(data_connection, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
 
      option = FTPd_window_size;   
      error_code = setsockopt(data_connection, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));

      option = FTPDCFG_SEND_TIMEOUT;   
      error_code = setsockopt(data_connection, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));

      option = FTPDCFG_CONNECT_TIMEOUT;   
      error_code = setsockopt(data_connection, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));

      option = FTPDCFG_TIMEWAIT_TIMEOUT;   
      error_code = setsockopt(data_connection, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));

      option = TRUE;   
      error_code = setsockopt(data_connection, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));


      error_code = bind(data_connection, (const sockaddr *)(&session_ptr->SERVER_DATA_SOCKADDR), sizeof(session_ptr->SERVER_DATA_SOCKADDR));
      if (error_code == RTCS_OK) {
         error_code = connect(data_connection, (const sockaddr *)(&session_ptr->USER_DATA_SOCKADDR), sizeof(session_ptr->USER_DATA_SOCKADDR));
      } 

      if (error_code != RTCS_OK) {
         shutdown(data_connection, FTPDCFG_SHUTDOWN_OPTION);
         data_connection = RTCS_SOCKET_ERROR; 
      } 
   } 
      
   return data_connection;

} /* Endbody */

/*FUNCTION*--------------------------------------------------------------
*
*Function Name  :FTPd_open_passive_data_connection
*Returned Value :Boolean Operation Successful or not
*Comments       :
*   This function opens the DataConnection to send the list Information
*
*END*---------------------------------------------------------------------*/

uint_32 FTPd_open_passive_data_connection(FTPd_SESSION_PTR session_ptr)
{
   uint_32      listen_connection, result=RTCS_SOCKET_ERROR;
   uint_32      error_code;
   sockaddr_in  remote_addr;
   uint_16      remote_addr_len;
   uint_32      option;
    
   
   listen_connection = socket(PF_INET, SOCK_STREAM, 0);
   if (listen_connection != RTCS_SOCKET_ERROR) {

      option = FTPd_window_size;   
      error_code = setsockopt(listen_connection, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));

      option = FTPd_window_size;   
      error_code = setsockopt(listen_connection, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));

      option = FTPDCFG_SEND_TIMEOUT;   
      error_code = setsockopt(listen_connection, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));

      option = FTPDCFG_CONNECT_TIMEOUT;   
      error_code = setsockopt(listen_connection, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));

      option = FTPDCFG_TIMEWAIT_TIMEOUT;   
      error_code = setsockopt(listen_connection, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));

      option = TRUE;   
      error_code = setsockopt(listen_connection, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));      

      error_code = bind(listen_connection,(const sockaddr *)(&session_ptr->SERVER_PASV_SOCKADDR), sizeof(session_ptr->SERVER_PASV_SOCKADDR));
      if (error_code == RTCS_OK) {
         error_code = listen(listen_connection, 0);
         if (error_code == RTCS_OK) {
            remote_addr_len = sizeof(remote_addr);
            session_ptr->PASV_SOCK = accept(listen_connection,(sockaddr *) &remote_addr, &remote_addr_len);
            if (session_ptr->PASV_SOCK)  {
               result = session_ptr->PASV_SOCK;
            }  
         }  
      }  
      shutdown(listen_connection, FTPDCFG_SHUTDOWN_OPTION);
   }  
      
   return result;
}  

/*
void FTPd_send_msg(FTPd_SESSION_PTR p,char* m)  
{
    send(p->CONTROL_SOCK, m, strlen(m), 0); 
}*/

/* EOF */
