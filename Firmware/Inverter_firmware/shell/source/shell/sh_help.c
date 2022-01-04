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
* $FileName: sh_help.c$
* $Version : 3.6.5.0$
* $Date    : Oct-26-2010$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>
#include "shell.h"
#include "sh_prv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_help
*  Returned Value:  none
*  Comments  :  
*
*END*-----------------------------------------------------------------*/

int_32  Shell_help(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   SHELL_COMMAND_PTR    command_ptr; 

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc==1)  {
         shell_ptr->ARGV[1] = "help";
         shell_ptr->ARGV[2] = "short"; 
         shell_ptr->ARGV[3] = NULL; 
         printf("Available commands:\n");
         command_ptr = shell_ptr->COMMAND_LIST_PTR; 
         while (command_ptr->COMMAND != NULL)  {
            shell_ptr->ARGV[0] = command_ptr->COMMAND;
            printf("   ");
            return_code = (*command_ptr->SHELL_FUNC)(3, shell_ptr->ARGV);
            command_ptr++;        
         } /* Endwhile */        
      } else {
         /* Help on a specific command */
         if (argc==2)  {
            shell_ptr->ARGV[0] = argv[1];
            shell_ptr->ARGV[1] = "help";
            shell_ptr->ARGV[2] = "usage"; 
            shell_ptr->ARGV[3] = NULL; 
            command_ptr = shell_ptr->COMMAND_LIST_PTR; 
            while (command_ptr->COMMAND != NULL)  {
               if (strcmp(argv[0], command_ptr->COMMAND) == 0)  {
                  return_code = (*command_ptr->SHELL_FUNC)(3, shell_ptr->ARGV);
                  break;   
               } 
               command_ptr++;        
            } /* Endwhile */
            if (command_ptr->COMMAND == NULL)  {
               printf("Error, command \"%s\" not registered.\n", argv[1] );
            }
         } else {
            print_usage = TRUE;
         }    
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [<command>]\n", argv[0]);
      } else  {
         printf("Usage: %s [<command>]\n", argv[0]);
         printf("   <command> = command to get help on\n");
      }
   }
   return return_code;
} /* Endbody */
