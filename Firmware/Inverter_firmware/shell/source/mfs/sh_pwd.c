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
* $FileName: sh_pwd.c$
* $Version : 3.8.10.0$
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
#include <mfs.h>
#include <shell.h>
#include <sh_prv.h>

#if SHELLCFG_USES_MFS

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_pwd
* Returned Value   :  int_32 error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  pwd <directory>
*
*END*---------------------------------------------------------------------*/

int_32  Shell_pwd(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean                    print_usage, shorthelp = FALSE;
   int_32                     error = 0, return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR               fs_ptr;
   SHELL_CONTEXT_PTR          shell_ptr = Shell_get_context( argv );    

   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc >  1) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else   {
         fs_ptr = Shell_get_current_filesystem(argv);
         if (fs_ptr == NULL)  {
            printf("Error, file system not mounted\n" );
            return_code = SHELL_EXIT_ERROR;
         } else  {
            printf("%s%s\n", shell_ptr->CURRENT_DEVICE_NAME, shell_ptr->CURRENT_DIR);
         }
      }
   }
      
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s \n", argv[0]);
      } else  {
         printf("Usage: %s \n", argv[0]);
      }
   }
   return return_code;
}

#endif //SHELLCFG_USES_MFS
/* EOF*/
