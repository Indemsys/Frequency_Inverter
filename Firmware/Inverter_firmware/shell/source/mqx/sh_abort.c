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
* $FileName: sh_abort.c$
* $Version : 3.0.4.0$
* $Date    : Jan-7-2009$
*
* Comments:
*
*   This file contains the abort command.
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include "shell.h"



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_abort
*  Returned Value:  none
*  Comments  :  SHELL utility to abort a task
*  Usage:   
*
*END*-----------------------------------------------------------------*/

int_32  Shell_abort(int_32 argc, char_ptr argv[] )
{
   boolean     print_usage, shorthelp = FALSE;
   int_32      return_code = SHELL_EXIT_SUCCESS;
   _task_id    task_id;
   uint_32     result;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      
      if (argc == 2)  {
         task_id = _task_get_id_from_name( argv[1] );
         if (task_id == MQX_NULL_TASK_ID)  {
            printf("No task named %s running.\n",argv[1]);
            return_code = SHELL_EXIT_ERROR;
         } else  {
            result = _task_abort(task_id);
            if (result == MQX_OK)  {  
               printf("Task %s aborted.\n",argv[1]);
            } else  {
               printf("Unable to abort task %s.\n",argv[1]);
               return_code = SHELL_EXIT_ERROR;
            }
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <taskname>\n", argv[0]);
      } else  {
         printf("Usage: %s <taskname>\n", argv[0]);
         printf("   <taskname> = MQX Task name\n");
      }
   }

   return return_code;
} /* Endbody */

/* EOF */
