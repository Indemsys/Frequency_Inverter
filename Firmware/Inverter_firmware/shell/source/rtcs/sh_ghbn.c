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
* $FileName: sh_ghbn.c$
* $Version : 3.7.7.0$
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
*  Function Name :  Shell_get_host_by_name
*  Returned Value:  none
*  Comments  :  SHELL utility to resolve a host name
*
*END*-----------------------------------------------------------------*/

int_32  Shell_get_host_by_name(int_32 argc, char_ptr argv[] )
{ /* Body */
   _ip_address          hostaddr;
   char                 hostname[MAX_HOSTNAMESIZE];

   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 2)  {
         RTCS_resolve_ip_address( argv[1], &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (hostaddr)  {
            printf("%s...%ld.%ld.%ld.%ld\n", hostname, IPBYTES(hostaddr));
         } else  {
         
#if RTCSCFG_ENABLE_DNS | RTCSCFG_ENABLE_LWDNS
            printf("Unable to resolve host\n");
#else
            printf("Unable to resolve host - dns service disabled\n");
#endif            
            return_code = SHELL_EXIT_ERROR;
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      }
   }
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <host>\n", argv[0]);
      } else  {
         printf("Usage: %s <host>\n", argv[0]);
         printf("   <host>   = host ip address or name\n");
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */
/* EOF */
