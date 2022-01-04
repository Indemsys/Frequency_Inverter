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
* $FileName: sh_sh.c$
* $Version : 3.8.9.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the source for an MFS shell function.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>
#include "shell.h"
#include "sh_prv.h"
//#include "fs_if.h"


#if SHELLCFG_USES_MFS
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_sh
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int_32  Shell_sh(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS, error;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv ); 
   MQX_FILE_PTR         fd;
   char                 abs_path[SHELL_MAX_FILELEN];   

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc != 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
        error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[1],sizeof(abs_path),shell_ptr->CURRENT_DEVICE_NAME);
         if (!error)
         {
            fd = fopen(abs_path, "r");
            if (fd)  {
               shell_ptr = Shell_get_context( argv );
               if (shell_ptr->COMMAND_FP != stdin)  {
                 fclose(shell_ptr->COMMAND_FP);
               }
               shell_ptr->COMMAND_FP = fd;
            } else  {
               printf("Error, unable to open file %s.\n", argv[1] );
               return_code = SHELL_EXIT_ERROR;
            }
         } else  {
            printf("Error, unable to open file %s.\n", argv[1] );
            return_code = SHELL_EXIT_ERROR;
         }
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <filename>\n", argv[0]);
      } else  {
         printf("Usage: %s <filename>\n", argv[0]);
         printf("   <filename>   = filename to execute\n");
      }
   }
   return return_code;
   
   
} /* Endbody */
#endif //SHELLCFG_USES_MFS
/* EOF*/
