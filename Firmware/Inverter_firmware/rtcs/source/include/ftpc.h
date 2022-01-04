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
* $FileName: ftpc.h$
* $Version : 3.8.9.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/
#define FTPC_BUF_SIZE      1460
#define FTPC_LINELEN       128
#define FTPC_MAX_ARGS      8

//#define FTPd_send_msg(p,m) send(p->CONTROL_SOCK, m, strlen(m), 0)
void FTPd_send_msg(pointer p, const char * m);


typedef struct ftpc_context_struct  {
   char_ptr             ARGV[FTPC_MAX_ARGS];
   int_32               ARGC;
   char                 CMD_LINE[FTPC_LINELEN];
   char                 HISTORY[FTPC_LINELEN];
   char                 BUFFER[FTPC_BUF_SIZE];
   
   _ip_address          HOSTADDR;
   char                 HOSTNAME[RTCS_SNAME_LEN];
   pointer              HANDLE;
   
   boolean              EXIT;
} FTPc_CONTEXT_STRUCT, _PTR_ FTPc_CONTEXT_PTR;

typedef struct FTPc_command_struct  {
   char_ptr  COMMAND;
   int_32      (*FTPc_FUNC)(FTPc_CONTEXT_PTR context_ptr,int_32 argc, char_ptr argv[]);
} FTPc_COMMAND_STRUCT, _PTR_ FTPc_COMMAND_PTR; 




extern const FTPc_COMMAND_STRUCT FTPc_commands[];


extern int_32  FTPc_help(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_ascii(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_binary(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_bye(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_cd(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_close(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_delete(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_get(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_ls(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_mkdir(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_open(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_pass(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_put(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_pwd(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_remotehelp(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_rename(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_rmdir(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );
extern int_32  FTPc_user(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] );

extern _mqx_int FTP_client(char_ptr dest);

extern int_32  FTP_open         (pointer _PTR_, _ip_address, MQX_FILE_PTR);
extern int_32  FTP_command      (pointer, char_ptr, MQX_FILE_PTR);
extern int_32  FTP_command_data (pointer, char_ptr, MQX_FILE_PTR, MQX_FILE_PTR, uint_32);
extern int_32  FTP_close        (pointer, MQX_FILE_PTR);
extern int_32  FTP_receive_message(pointer , MQX_FILE_PTR);

/*EOF*/
