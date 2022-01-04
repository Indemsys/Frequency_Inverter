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
* $FileName: sh_gate.c$
* $Version : 3.7.8.0$
* $Date    : Feb-24-2011$
*
* Comments:
*
*   Example using RTCS Library.
*
*END************************************************************************/

#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"



/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_gate
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

int_32 Shell_gate(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;

   _ip_address       gate_ipaddr;
   _ip_address       ipaddr = INADDR_ANY;
   _ip_address       ipmask = INADDR_ANY;
   uint_32           error = 0;
  
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc <= 1) || (argc > 4))  {
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      } else  {
         if (! Shell_parse_ip_address( argv[1], &gate_ipaddr  ))  {
            printf("Error, invalid gateway address\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (argc > 2)  {
            if (! Shell_parse_ip_address( argv[2], &ipaddr  ))  {
               printf("Error, invalid ip address\n");
               return_code = SHELL_EXIT_ERROR;
            } else if (argc > 3)  {
               if (! Shell_parse_netmask( argv[3], &ipmask ))  {
                  printf("Error, invalid ip mask\n");
                  return_code = SHELL_EXIT_ERROR;
               }          
            }          
         }          
      }          
         
         
      if (return_code == SHELL_EXIT_SUCCESS)  {
         printf("Adding gateway %d.%d.%d.%d, ip address: %d.%d.%d.%d, netmask: %d.%d.%d.%d\n",
              IPBYTES(gate_ipaddr),IPBYTES(ipaddr),IPBYTES(ipmask));
         error = RTCS_gate_add(gate_ipaddr, ipaddr, ipmask);
         if (error) {
            printf("Error, could not initialize gateway %d.%d.%d.%d\n", gate_ipaddr);
            printf("RCS_gate_add returned: %d\n", error);
            return_code = SHELL_EXIT_ERROR;
         } 
      } 
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <gateway> [<ipaddr>] [<netmask>]\n", argv[0]);
      } else  {
         printf("Usage: %s  <gateway> [<ipaddr>] [<netmask>]\n", argv[0]);
         printf("   <gateway> = IP address of gateway\n");
         printf("   <ipaddr>  = IP address \n");
         printf("   <netmask> = Network mask\n");
      }
   }

   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */
/* EOF */

