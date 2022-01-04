/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved

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
* $FileName: sh_tad.c$
* $Version : 3.0.1.0$
* $Date    : Jun-9-2009$
*
* Comments:
*
*   This file contains the MQX shell command.
*
*END************************************************************************/


#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include <fio.h>
#include <tad.h>
#include "shell.h"


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_tad
*  Returned Value:  none
*  Comments  :  SHELL utility to print memory and stack usage.
*
*END*-----------------------------------------------------------------*/

int_32  Shell_tad (int_32 argc, char_ptr argv[])
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request (argc, argv, &shorthelp);

   if (! print_usage)  
   {
      if (argc == 1)  
      {
         _tad_stack_usage ();
         _tad_lightweight_memory_blocks ();
      } 
      else if (argc == 2) 
      {
         if (! strcmp ("stack", argv[1])) 
         {
            _tad_stack_usage ();
         }
         else if (! strcmp ("lwmemblock", argv[1])) 
         {
            _tad_lightweight_memory_blocks ();
         }
         else
         {
            printf("Error, %s invoked with unknown argument %s\n", argv[0], argv[1]);
            return_code = SHELL_EXIT_ERROR;
            print_usage = TRUE;
         }
      } 
      else 
      {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  
   {
      if (shorthelp)  
      {
         printf("%s [stack | lwmemblock]\n", argv[0]);
      } 
      else  
      {
         printf("Usage: %s [stack | lwmemblock]\n", argv[0]);
         printf("   stack      = prints stack usage\n");
         printf("   lwmemblock = prints lightweight memory blocks\n");
      }
   }
   return return_code;
} /* Endbody */



/* EOF */
