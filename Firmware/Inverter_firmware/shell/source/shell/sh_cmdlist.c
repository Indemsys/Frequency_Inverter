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
* $FileName: sh_cmdlist.c$
* $Version : 3.8.4.0$
* $Date    : Sep-19-2011$
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
*  Function Name :  Shell_command_list
*  Returned Value:  none
*  Comments  :  SHELL utility to load and execute an executable file
*
*END*-----------------------------------------------------------------*/

int_32  Shell_command_list(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv ); 
   SHELL_COMMAND_PTR    command_ptr;
   uint_32              i; 

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc==1)  {
         for (i=0,command_ptr = shell_ptr->COMMAND_LIST_PTR; 
              command_ptr->COMMAND != NULL; 
              i++,command_ptr++)  
          {
            printf("%-8s ", command_ptr->COMMAND);
            if ((i&7)==7) printf("\n");
         } /* Endwhile */
         if ((i&7)!=0) printf("\n");        
      } else {  
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s\n", argv[0]);
      } else  {
         printf("Usage: %s\n", argv[0]);
      }
   }
   return return_code;
} /* Endbody */
/* EOF*/
