/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: sh_exec.c$
* $Version : 3.7.5.0$
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


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_exec
*  Returned Value:  none
*  Comments  :  SHELL utility to load and execute an executable file
*
*END*-----------------------------------------------------------------*/

int_32  Shell_exec(int_32 argc, char_ptr argv[] )
{ /* Body */
   _ip_address          hostaddr;
   char                 hostname[MAX_HOSTNAMESIZE];
   char_ptr             host_ptr , file_ptr, format_ptr;

   boolean              print_usage, shorthelp = FALSE;
   int_32               error = 0, return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 4 )  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
         return_code = SHELL_EXIT_ERROR;
      } else  {
         format_ptr = "srec";
         if (argc > 1)  {
            file_ptr = argv[1];
            if (argc > 2)  {
               host_ptr = argv[2];
               if (argc>3)  {
                  format_ptr = argv[3];
               }
            } else  {
                  printf("Error, no TFTP Server specified.\n", argv[0]);
                  print_usage = TRUE;
                  return_code = SHELL_EXIT_ERROR;
            }
         } else  {
               printf("Error, no  bootfile specified.\n", argv[0]);
               print_usage = TRUE;
               return_code = SHELL_EXIT_ERROR;
         }
      }
      
      if (return_code != SHELL_EXIT_ERROR)  {
         RTCS_resolve_ip_address( host_ptr, &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (hostaddr)  {
            printf("Loading %s from %s [%ld.%ld.%ld.%ld]:\n", file_ptr, hostname, IPBYTES(hostaddr));
            if (strcmp(format_ptr, "srec") == 0)  {
               error = RTCS_exec_TFTP_SREC(hostaddr, file_ptr);
            } else if (strcmp(format_ptr, "coff") == 0)  {
               error = RTCS_exec_TFTP_COFF(hostaddr, file_ptr);
            } else {
               printf("Error, invalid format %s\n", format_ptr);
               print_usage = TRUE;
               return_code = SHELL_EXIT_ERROR;
            }
            if (error)  {
               printf("RTCS_exec_TFTP_%s returned with error code = %x\n",format_ptr, error);
               return_code = SHELL_EXIT_ERROR;
            }
         } else  {
            printf("Unable to resolve host\n");
            return_code = SHELL_EXIT_ERROR;
         }
      }         
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <file> <host> <format>\n", argv[0]);
      } else  {
         printf("Usage: %s <file> <host> <format>\n", argv[0]);
         printf("   <file>   = filename\n");
         printf("   <host>   = TFTP Server ip address or name\n");
         printf("   <format> = file format, one of coff or srec\n");
      }
   }
   return return_code;
} /* Endbody */
#endif /* SHELLCFG_USES_RTCS */

/* EOF */
