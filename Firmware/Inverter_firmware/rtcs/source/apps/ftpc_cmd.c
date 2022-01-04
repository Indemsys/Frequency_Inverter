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
* $FileName: ftpc_cmd.c$
* $Version : 3.8.6.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <rtcs.h>
#include <fio.h>
#include "ftpc.h"

               
               
               
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_help
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_help(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] ) 
{
   uint_32               i;
   
   for (i=0; FTPc_commands[i].COMMAND != NULL; i++)   {
      printf("%-10s ", FTPc_commands[i].COMMAND);
      if ((i%6)==5) printf("\n");        
   } /* Endwhile */        
   if ((i%6)!=0) printf("\n");        

   return 0;
}
                 
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_ascii
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_ascii(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 1) {
      printf("Usage: ascii\n");
   } else {
      response = FTP_command(context_ptr->HANDLE,"TYPE A\r\n",stdout);
   }
   
   return response;
}
              
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_binary
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_binary(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 1) {
      printf("Usage: binary\n");
   } else {
      response = FTP_command(context_ptr->HANDLE,"TYPE I\r\n",stdout);
   }
   
   return response;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_bye
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_bye(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE != NULL)  {
      response = FTP_close(context_ptr->HANDLE,stdout);
      context_ptr->HANDLE = NULL;
      context_ptr->HOSTADDR = 0;
   }
   context_ptr->EXIT=TRUE; 
   
   return response;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_cd
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_cd(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc > 2) {
      printf("Usage: cd <directory>\n");
   } else  {
      sprintf(context_ptr->BUFFER,"CWD %s\r\n", (argc > 1)?argv[1]:"" );
      response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);
   }
   
   return response;
}
         
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_close
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_close(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 1) {
      printf("Usage: close\n");
   } else {
      response = FTP_close(context_ptr->HANDLE, stdout);
      context_ptr->HANDLE = NULL;
      context_ptr->HOSTADDR = 0;
   }
   
   return response;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_delete
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_delete(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
               
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 2) {
      printf("Usage: delete <filename>\n");
   } else {
      sprintf(context_ptr->BUFFER,"DELE %s\r\n", context_ptr->ARGV[1] );
      response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);
   }
   
   return response;
}
            

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_get
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_get(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32      response=0;
   MQX_FILE_PTR fd;
      
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc < 2) {
      printf("Usage: get <remotename> [<localname>]\n");
   } else  {
      if (argc == 3) {
         fd = fopen(argv[2], "w");
      } else  {
         fd = fopen(argv[1], "w");
      }
      if (!fd)  {
         printf("Unable to open local file: %s\n",argv[1]);
      } else {
         sprintf(context_ptr->BUFFER,"RETR %s\r\n", argv[1] );
         response = FTP_command_data(context_ptr->HANDLE, context_ptr->BUFFER, stdout, fd, FTPMODE_PORT | FTPDIR_RECV);
         fclose(fd);
      }
   }
   
   return response;
}
         
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_ls
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_ls(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc > 2) {
      printf("Usage: ls <filespec>\n");
   } else  {
      sprintf(context_ptr->BUFFER,"LIST %s\r\n", (argc > 1)?argv[1]:"" );
      response = FTP_command_data(context_ptr->HANDLE,context_ptr->BUFFER,stdout,stdout, FTPMODE_PORT | FTPDIR_RECV);
   }
   
   return response;
}
         
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_mkdir
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_mkdir(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 2) {
      printf("Usage: mkdir <dir>\n");
   } else  {
      sprintf(context_ptr->BUFFER,"MKD %s\r\n", argv[1] );
      response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);
   }
   
   return response;
}
         
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_open
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_open(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (argc != 2)  {
      printf("Usage: open <host>\n");
   } else if (context_ptr->HANDLE != NULL) {
      printf("Already connected to %s. Use close first.\n",context_ptr->HOSTNAME);
   } else {
               
      RTCS_resolve_ip_address( argv[1], &context_ptr->HOSTADDR, context_ptr->HOSTNAME, RTCS_SNAME_LEN ); 

      if (!context_ptr->HOSTADDR)  {
         printf("Unable to resolve host.\n");
      } else  {
         printf("Connecting to: %s [%ld.%ld.%ld.%ld]\n",
               context_ptr->HOSTNAME, IPBYTES(context_ptr->HOSTADDR));
   
         response = FTP_open(&context_ptr->HANDLE,context_ptr->HOSTADDR,stdout);
         if (response == RTCS_ERROR)  {
            printf("Unable to open %s\n",context_ptr->HOSTNAME);
            response = 0;
            context_ptr->HOSTADDR = 0;
            context_ptr->HANDLE = NULL;   
         }
      }
   }
   
   return response;
}
            
               
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_pass
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_pass(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32   response=0;
   char_ptr param_ptr;
   char     string[64];
   
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc > 2) {
      printf("Usage: pass <password>\n");
   } else  {
      if (argc == 1) {
         printf("Password: ");
         if (!fgets(string, sizeof(string), stdin)) {
            context_ptr->EXIT = TRUE;
            return 0;
         }
         param_ptr = string;
      } else  {
         param_ptr = argv[1];
      }
      sprintf(context_ptr->BUFFER,"PASS %s\r\n", param_ptr );
      response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);
      printf("response = %d\n", response );
   }
   
   return response;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_put
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_put(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32      response=0;
   MQX_FILE_PTR fd;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if ((argc<2) || (argc>3)) {
      printf("Usage: put <local filename> [<remote filename>]\n");
   } else {
      fd = fopen(argv[1],"r");
      if (!fd)  {
         printf("Unable to open local file: %s\n",argv[1]);
      } else  {
         sprintf(context_ptr->BUFFER,"STOR %s\r\n", (argc==2)?argv[1]:argv[2] );
         response = FTP_command_data(context_ptr->HANDLE,context_ptr->BUFFER,stdout, fd, FTPMODE_PORT | FTPDIR_SEND);
         fclose(fd);
      }
   }
   
   return response;
}
         
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_pwd
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_pwd(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 1) {
      printf("Usage: pwd\n");
   } else {
      response = FTP_command(context_ptr->HANDLE,"PWD \r\n",stdout);
   }
   
   return response;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_remotehelp
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_remotehelp(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32      response=0;
   char_ptr    param_ptr;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc > 2 ) {
      printf("Usage: remotehelp <command>\n");
   } else {
      if (argc == 1) {
         param_ptr = "";
      } else  {
         param_ptr = argv[1];
      }

      sprintf(context_ptr->BUFFER,"HELP %s\r\n", param_ptr );
      response = FTP_command_data(context_ptr->HANDLE,context_ptr->BUFFER,
         stdout, stdin, FTPMODE_PORT | FTPDIR_RECV );
   }
   
   return response;
}
         
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_rename
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_rename(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 3) {
      printf("Usage: rename <oldname> <newname>\n");
   } else  {
      sprintf(context_ptr->BUFFER,"RNFR %s\r\n", argv[1] );
      response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);

      if ((response >= 300) && (response < 400))  {
         sprintf(context_ptr->BUFFER,"RNTO %s\r\n", argv[2] );
         response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);
      }
   }
   
   return response;
}
               
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_rmdir
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_rmdir(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32    response=0;
   
   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc != 2) {
      printf("Usage: rmdir <dir>\n");
   } else  {
      sprintf(context_ptr->BUFFER,"RMD %s\r\n", argv[1] );
      response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);
   }
   
   return response;
}
         
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   FTPc_user
* Returned Value   :  int_32 error code
* Comments  : 
*
* Usage:  
*
*END*---------------------------------------------------------------------*/

int_32  FTPc_user(FTPc_CONTEXT_PTR context_ptr, int_32 argc, char_ptr argv[] )
{
   int_32   response = 0;
   char_ptr param_ptr = NULL;
   char     string[64];

   if (context_ptr->HANDLE == NULL)  {
      printf("Not connected.\n");
   } else if (argc > 2) {
      printf("Usage: user <username>\n");
   } else if (argc == 1) {
      printf("User: ");
      if (!fgets(string, sizeof(string), stdin)) {
         context_ptr->EXIT = TRUE;
      } else {
        param_ptr = string;
      }
   } else  {
      param_ptr = argv[1];
   }

   if (param_ptr != NULL) {
      sprintf(context_ptr->BUFFER,"USER %s\r\n", param_ptr );
      response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);

      if ((response >= 300) && (response < 400))  {
         printf("Password: " );
         // call ioctl to echo off
         if (!fgets(string, sizeof(string), stdin)) {
            context_ptr->EXIT = TRUE;
            response = 0;
         } else {
            // call ioctl to echo on
            sprintf(context_ptr->BUFFER,"PASS %s\r\n", string);
            response = FTP_command(context_ptr->HANDLE,context_ptr->BUFFER,stdout);
         }
      }
   }

   return response;
}

/* End of file */
