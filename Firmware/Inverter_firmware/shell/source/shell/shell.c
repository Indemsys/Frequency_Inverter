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
* $FileName: shell.c$
* $Version : 3.8.16.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include <fio.h>
#include "shell.h"
#include "sh_prv.h"

   
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell
*  Returned Value:  none
*  Comments  :  The RTCS shell
*
*END*-----------------------------------------------------------------*/

int_32 Shell
   (
      const SHELL_COMMAND_STRUCT   shell_commands[],
      char_ptr            start_file 
   )
{ /* Body */
   SHELL_CONTEXT_PTR    shell_ptr;
   // int_32               return_code;
   uint_32              i;

   printf("\nShell (build: %s)\n", __DATE__);
   printf("Copyright (c) 2013 Freescale Semiconductor;\n");
   
   
   shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
   if (shell_ptr == NULL)  {
      return SHELL_EXIT_ERROR;
   }
   _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
   
   shell_ptr->COMMAND_LIST_PTR=(SHELL_COMMAND_PTR)shell_commands; 

   if (start_file != NULL)  {
      if (start_file[0] == '\'') {
         strncpy(  shell_ptr->CMD_LINE, &start_file[1],sizeof(shell_ptr->CMD_LINE  ));    
      } else {
         shell_ptr->COMMAND_FP = fopen(start_file, "r");
      }
   }
   
   if (!shell_ptr->COMMAND_FP)  {
      shell_ptr->COMMAND_FP = stdin;
   }

#if SHELLCFG_USES_MFS
   // initialize current fs ptr, fs name and path
   Shell_set_current_filesystem((pointer)shell_ptr,NULL);
#endif //SHELLCFG_USES_MFS
   
   if (shell_ptr->CMD_LINE){
      printf("shell> %s\n",shell_ptr->CMD_LINE);
   } else {
      printf("shell> ");
   }
   fflush(stdout);
   
   while (!shell_ptr->EXIT) {
      
      if ((!shell_ptr->EXIT) && (shell_ptr->CMD_LINE[0] != '\0'))  {
         
         if (shell_ptr->COMMAND_FP != stdin)  {
            printf("%s\n", shell_ptr->CMD_LINE);
         }
      
         if (shell_ptr->CMD_LINE[0] != '#') {

            if (strcmp(shell_ptr->CMD_LINE, "!") == 0)  {
               strncpy(shell_ptr->CMD_LINE,shell_ptr->HISTORY,sizeof(shell_ptr->CMD_LINE));
            } else if (strcmp(shell_ptr->CMD_LINE, "\340H") == 0)  {
               strncpy(shell_ptr->CMD_LINE,shell_ptr->HISTORY,sizeof(shell_ptr->CMD_LINE));
            } else  {
               strncpy(shell_ptr->HISTORY,shell_ptr->CMD_LINE,sizeof(shell_ptr->HISTORY));
            }

            shell_ptr->ARGC = Shell_parse_command_line(shell_ptr->CMD_LINE, shell_ptr->ARGV );
         
            if (shell_ptr->ARGC > 0) { 
            
               _io_strtolower(shell_ptr->ARGV[0]); 
               for (i=0;shell_commands[i].COMMAND != NULL;i++)  {
                  if (strcmp(shell_ptr->ARGV[0], shell_commands[i].COMMAND) == 0)  {
                     /* return_code = */ (*shell_commands[i].SHELL_FUNC)(shell_ptr->ARGC, shell_ptr->ARGV);
                     break;   
                  }
               } 
            
               if (shell_commands[i].COMMAND == NULL)  {
                 printf("Invalid command.  Type 'help' for a list of commands.\n");
               } 
            }
         }
      }

      if (!shell_ptr->EXIT) { 
         printf("shell> ");
         fflush(stdout);

         do {
            if (!fgets(shell_ptr->CMD_LINE, sizeof(shell_ptr->CMD_LINE  ), shell_ptr->COMMAND_FP)) {
               if (shell_ptr->COMMAND_FP != stdin)  {
                  fclose(shell_ptr->COMMAND_FP);
                  shell_ptr->COMMAND_FP = stdin;
                  shell_ptr->HISTORY[0]=0;
                  shell_ptr->CMD_LINE[0]=0;
                  printf("\n");
               } else  {
                  shell_ptr->EXIT=TRUE;
                  break;
               }
            }
         } while ((shell_ptr->CMD_LINE[0] == '\0') && (shell_ptr->COMMAND_FP != stdin)) ; 
      }
   } 

   printf("Terminating shell.\n");
   _mem_free(shell_ptr);
   return SHELL_EXIT_SUCCESS;
} /* Endbody */


/* EOF */
