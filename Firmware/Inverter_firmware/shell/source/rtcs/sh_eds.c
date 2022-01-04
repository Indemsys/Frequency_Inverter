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
* $FileName: sh_eds.c$
* $Version : 3.7.4.0$
* $Date    : Feb-24-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h" 

#define SHELL_EDS_PRIO        7
#define SHELL_EDS_STACK       1500

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_EDS
*  Returned Value:  none
*  Comments  :  SHELL utility to TFTP to or from a host
*  Usage:  tftp host get source [destination] [mode] 
*
*END*-----------------------------------------------------------------*/

int_32  Shell_EDS(int_32 argc, char_ptr argv[] )
{
   uint_32  result;
   boolean  print_usage, shorthelp = FALSE;
   int_32   return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 2)  {
         if (strcmp(argv[1], "start") == 0)  {
         
            result = EDS_init("EDS", SHELL_EDS_PRIO, SHELL_EDS_STACK );
            if (result ==  0)  {
               printf("EDS Started.\n");
            } else  {
               printf("Unable to start EDS, error = 0x%x\n",result);
               return_code = SHELL_EXIT_ERROR;
            }
         } else if (strcmp(argv[1], "stop") == 0)  {
            result = EDS_stop();
            if (result ==  0)  {
               printf("EDS Stopped.\n");
            } else  {
               printf("Unable to stop EDS, error = 0x%x\n",result);
               return_code = SHELL_EXIT_ERROR;
            }
         } else  {
         printf("Error, %s invoked with incorrect option\n", argv[0]);
            print_usage = TRUE;
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [start|stop]\n", argv[0]);
      } else  {
         printf("Usage: %s [start|stop]\n",argv[0]);
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */

/* EOF */
