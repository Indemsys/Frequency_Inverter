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
* $FileName: sh_pause.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
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


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_pause
*  Returned Value:  none
*  Comments  :  SHELL utility to pause the shell for specified time
*
*END*-----------------------------------------------------------------*/

int_32  Shell_pause(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;
   uint_32              pause=10*60*1000;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 1)  {
         printf("Pausing for 5 minutes...\n");
        _time_delay(5*60*1000);
          printf("Done\n");
      } else if (argc==2) {
         if (!Shell_parse_uint_32(argv[1], &pause  )) {
            printf("Error, invalid rule priority\n");
            return_code = SHELL_EXIT_ERROR;
         }
          printf("Pausing for %d minutes...\n",pause);
          _time_delay(pause*60*1000);
          printf("Done\n");
      } else {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [<minutes>]\n", argv[0]);
      } else  {
         printf("Usage: %s <minutes>\n", argv[0]);
         printf("   <minutes>   = minutes to pause for\n");
      }
   }
   return return_code;
} /* Endbody */



/* EOF */
