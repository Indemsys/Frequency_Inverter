#ifndef __ftpd_h__
#define __ftpd_h__
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
* $FileName: ftpd.h$
* $Version : 3.8.18.0$
* $Date    : Aug-27-2012$
*
* Comments:
*
*   This file contains the FTP server.
*
*END************************************************************************/

#include <fio.h>

#define FTPD_DEVLEN              8
#define FTPD_PATHLEN             260

#define FTPD_LINELEN             128

#define FTPD_PASV_PORT_START     3000
#define FTPD_PASV_PORT_END       3020
#define FTPD_NEED_PASSWORD       1
#define FTPD_LOGGED_IN           2
#define FTPD_LOGGED_OUT          0
#define FTPD_MAX_LOGIN_ATTEMPTS  3

#define FTPD_MAX_USERLEN         32

#define FTPd_OK                  0
#define FTPd_ERROR               (-1)

#ifndef min
    #define min(a,b) ((a<b)?a:b)
#endif

//#define FTPd_send_msg(p,m) send(p->CONTROL_SOCK, m, strlen(m), 0) 
void FTPd_send_msg(pointer p,const char * m);

typedef struct ftpd_session_struct  {
   sockaddr_in    USER_DATA_SOCKADDR;
   sockaddr_in    SERVER_DATA_SOCKADDR;
   sockaddr_in    SERVER_PASV_SOCKADDR;
   boolean        CONNECTED;
   uint_32        CONTROL_SOCK;
   _ip_address    LOCAL_ADDR;
   uint_32        DATA_SOCK;
   uint_32        PASV_SOCK;
   boolean        PASV_MODE;
#if FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
   uint_32        LOGIN_STATE;
   uint_32        LOGIN_ATTEMPTS;
   char           USERNAME[FTPD_MAX_USERLEN];
   char           PASSWORD[FTPD_MAX_USERLEN];
#endif
   char           CURRENT_FS_DIR[FTPD_PATHLEN];
   char_ptr       CURRENT_FTP_DIR;
   char           CURRENT_FS_NAME[FTPD_DEVLEN];
   MQX_FILE_PTR   CURRENT_FS;               
   char           CMD_LINE[FTPD_LINELEN];    // TODO : Don't need both CMD_LINE and CMD_BUFFER
   char           CMD_BUFFER[FTPD_LINELEN];
   char_ptr       DATA_BUFFER_PTR;
   uint_32        DATA_BUFFER_SIZE;
#ifdef FTPDCFG_ENABLE_RENAME
   char_ptr       RENAME_FROM;
   MQX_FILE_PTR   RENAME_FS;
#endif
   
} FTPd_SESSION_STRUCT, _PTR_ FTPd_SESSION_PTR;


typedef struct FTPd_command_struct  {
   char_ptr  COMMAND;
   int_32      (*FTPd_FUNC)(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr);
} FTPd_COMMAND_STRUCT, _PTR_ FTPd_COMMAND_PTR; 


extern char FTPd_rootdir[];
extern const FTPd_COMMAND_STRUCT FTPd_commands[];
extern char_ptr FTPd_userfile;
extern uint_32 FTPd_window_size;
extern uint_32 FTPd_buffer_size;


extern uint_32 FTPd_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   );


extern void FTPd_task(pointer, pointer);
extern uint_32 FTPd_stop( void );
extern uint_32 FTPd_open_active_data_connection(FTPd_SESSION_PTR);
extern uint_32 FTPd_open_passive_data_connection(FTPd_SESSION_PTR);

int_32  FTPd_cd(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_cdup(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_dele(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_feat(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_help(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_list(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_mkdir(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_nlst(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_noop(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_opts(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_pass(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_pasv(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_port(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_pwd(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_quit(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_rmdir(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_retr(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_rnfr(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_rnto(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_site(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_size(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_stor(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_syst(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_type(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );
int_32  FTPd_user(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );

int_32  FTPd_unimplemented(FTPd_SESSION_PTR session_ptr, char_ptr cmd_ptr, char_ptr arg_ptr );

#ifndef min
    #define min(a,b) ((a<b)?a:b)
#endif


#endif

/* EOF*/
