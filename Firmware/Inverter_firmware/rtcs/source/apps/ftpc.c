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
* $FileName: ftpc.c$
* $Version : 3.0.5.0$
* $Date    : Apr-14-2009$
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
#include <ftpc.h>

#define FTPC_MAX_ARGS 8
extern  const FTPc_COMMAND_STRUCT FTPc_commands[];

static int_32 FTP_parse_command_line( char_ptr command_line_ptr, char_ptr argv[] )
{
   char_ptr             sptr;
   uint_32              i;
   int_32               argc;

   sptr = command_line_ptr;
   argc = 0;
   for (i=0;i<FTPC_MAX_ARGS;i++)  {
      while (*sptr && !isgraph((uchar)*sptr)) sptr++;
      if (!*sptr) {
         argv[i] = NULL;
      } else {
         argc++;
         argv[i] = sptr;
         while (*sptr && isgraph(*sptr)) sptr++;
         if (*sptr) *sptr++ = '\0';
      } 
   }
   return argc;
}

_mqx_int FTP_client(char_ptr dest)
{
   FTPc_CONTEXT_PTR  ftpc_context_ptr;
   uint_32           i;
   _mqx_int          response;

   ftpc_context_ptr = _mem_alloc_zero( sizeof( FTPc_CONTEXT_STRUCT ));
   if (ftpc_context_ptr == NULL)  {
      printf("Error, unable to allocate memory for FTP client.\n");
      return IO_ERROR;
   }

   _mem_set_type(ftpc_context_ptr, MEM_TYPE_FTPc_CONTEXT_STRUCT);

   printf("\nRTCS v%d.%02d.%02d FTP client\n",
          RTCS_VERSION_MAJOR, RTCS_VERSION_MINOR, RTCS_VERSION_REV);

   if (dest) {
      ftpc_context_ptr->ARGC = 2;
      ftpc_context_ptr->ARGV[0] = "open";
      ftpc_context_ptr->ARGV[1] = dest;
   } else  {
      ftpc_context_ptr->ARGC = 0;
   }
   
   while (!ftpc_context_ptr->EXIT) {
      if (ftpc_context_ptr->ARGC > 0) { 
         _io_strtolower(ftpc_context_ptr->ARGV[0]); 
         for (i=0; FTPc_commands[i].COMMAND;i++) { 
            if (strcmp(ftpc_context_ptr->ARGV[0], FTPc_commands[i].COMMAND) == 0)  {
               response = (*FTPc_commands[i].FTPc_FUNC)(ftpc_context_ptr, 
                  ftpc_context_ptr->ARGC, ftpc_context_ptr->ARGV);
               if (response == RTCS_ERROR)  {
                  printf("FTP connection closed.\n");
                  response = 0;
                  ftpc_context_ptr->HOSTADDR = 0;
                  ftpc_context_ptr->HANDLE = NULL;   
               } else  {
                   while ((response >= 100) && (response < 200))   {
                     response = FTP_receive_message(ftpc_context_ptr->HANDLE,stdout);
                   }
               }   
               break;   
            }
         } /* Endwhile */
   
         if (FTPc_commands[i].COMMAND == NULL)  {
           printf("Invalid command.  Type 'help' for a list of commands.\n");
         } /* Endif */
      } /* Endif */

      if( ftpc_context_ptr->EXIT)  {
         break;
      }
      
      if (ftpc_context_ptr->HOSTADDR)  {
         printf("ftp [%s]> ", ftpc_context_ptr->HOSTNAME);
      } else  {
         printf("ftp> ");
      }

      if (!fgets(ftpc_context_ptr->CMD_LINE, sizeof(ftpc_context_ptr->CMD_LINE), stdin)) {
         break;
      } /* Endif */

      if (strcmp(ftpc_context_ptr->CMD_LINE, "!") == 0)  {
         strncpy(ftpc_context_ptr->CMD_LINE,ftpc_context_ptr->HISTORY,sizeof(ftpc_context_ptr->CMD_LINE));
      } else  {
         strncpy(ftpc_context_ptr->HISTORY,ftpc_context_ptr->CMD_LINE,sizeof(ftpc_context_ptr->HISTORY));
      }

      ftpc_context_ptr->ARGC = FTP_parse_command_line(ftpc_context_ptr->CMD_LINE, &ftpc_context_ptr->ARGV[0] );

   } /* Endwhile */
   printf("FTP terminated\n");
   _mem_free(ftpc_context_ptr);

   return MQX_OK;
}
   



 

/* EOF */
