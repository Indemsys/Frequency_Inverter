/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: sh_getroute.c$
* $Version : 3.8.11.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   Shell RTCS_get_route implementation.
*
*END************************************************************************/

#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"



/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_getroute
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

int_32 Shell_getroute(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;

   _ip_address       gate_ipaddr = 0;
   _ip_address       ipaddr = 0;
   _ip_address       ipmask = 0;
  
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc < 2) || (argc > 3)) {
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      } else  {
         if (! Shell_parse_ip_address( argv[1], &ipaddr  ))  {
            printf("Error, invalid ip address\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (argc == 3)  {
            if (! Shell_parse_ip_address( argv[2], &ipmask  ))  {
               printf("Error, invalid ip mask\n");
               return_code = SHELL_EXIT_ERROR;
            }
         }          
      }          
         
         
      if (return_code == SHELL_EXIT_SUCCESS)  {
         gate_ipaddr = RTCS_get_route(ipaddr, ipmask);
         printf("The route to ip address: %d.%d.%d.%d, netmask: %d.%d.%d.%d is gateway %d.%d.%d.%d\n",
            IPBYTES(ipaddr),IPBYTES(ipmask),IPBYTES(gate_ipaddr));
      } 
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <ipaddr> <netmask>\n", argv[0]);
      } else  {
         printf("Usage: %s <ipaddr> <netmask>\n", argv[0]);
         printf("   <ipaddr>  = IP address \n");
         printf("   <netmask> = Network mask\n");
      }
   }

   return return_code;
} /* Endbody */

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_getroute
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

int_32 Shell_walkroute(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc >1)  {
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      }          
         
         
      if (return_code == SHELL_EXIT_SUCCESS)  {
         RTCS_walk_route();
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

#endif /* SHELLCFG_USES_RTCS */

/* EOF */

