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
* $FileName: sh_arpdisp.c$
* $Version : 3.8.8.0$
* $Date    : Sep-26-2012$
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
#include <enet.h>
#include <ipcfg.h>
#include "sh_rtcs.h"
#include "sh_enet.h"

#ifdef BSP_ENET_DEVICE_COUNT
#if  (BSP_ENET_DEVICE_COUNT > 0) 


extern void ARP_display_if_table(_rtcs_if_handle   ihandle);
extern uint_32 RTCS_arp_delete (_rtcs_if_handle _PTR_   ihandle,_ip_address paddr );

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_arpdisp
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

int_32 Shell_arpdisp(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   _rtcs_if_handle   ihandle = ipcfg_get_ihandle (BSP_DEFAULT_ENET_DEVICE);
  
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
      if (argc > 1)  {
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      } else  { 
        ARP_display_if_table(ihandle);
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
} /* Endbody */

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_arpdel
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

int_32 Shell_arpdel(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   _ip_address       ipaddr;
   _rtcs_if_handle   ihandle = ipcfg_get_ihandle (BSP_DEFAULT_ENET_DEVICE);
  
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc != 2)  {
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      } else  if (! Shell_parse_ip_address( argv[1], &ipaddr))  {
          printf("Error, invalid ip address\n");
          return_code = SHELL_EXIT_ERROR;
      }
      if (return_code == SHELL_EXIT_SUCCESS) {
          RTCS_arp_delete(ihandle,ipaddr);
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <ip_address>\n", argv[0]);
      } else  {
         printf("Usage: %s <ip_address> \n", argv[0]);
         printf("   <ip_address>  = IP Address\n");
      }
   }

   return return_code;
} /* Endbody */

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_arpadd
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
int_32 Shell_arpadd(int_32 argc, char_ptr argv[] )
{
    boolean         print_usage, shorthelp = FALSE;
    int_32          return_code = SHELL_EXIT_SUCCESS;
    _enet_address   mac;
    _ip_address     ipaddr;    

    _rtcs_if_handle   ihandle = ipcfg_get_ihandle (BSP_DEFAULT_ENET_DEVICE);
  
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)
    {
        if ((argc != 4)&&(argc != 3))
        {
            return_code = SHELL_EXIT_ERROR;
            print_usage = TRUE;
        }
        else  if (! Shell_parse_ip_address( argv[1], &ipaddr))
        {
            printf("Error, invalid ip address\n");
            return_code = SHELL_EXIT_ERROR;
        }
        else if (! Shell_parse_enet_address(argv[2], mac))
        {
            printf("Invalid MAC address!\n");
            return_code = SHELL_EXIT_ERROR;
        }
        else
        {
            RTCS_arp_add(ihandle, ipaddr, (char*)mac);
            ARP_display_if_table(ihandle);
        }
    }
  
   
    if (print_usage)
    {
        if (shorthelp)
        {
            printf("%s <ip_address> <mac>\n", argv[0]);
        }
        else
        {
            printf("Usage: %s <ip_address> <mac>\n", argv[0]);
            printf("   <ip_address>  = IP Address\n");
            printf("   <mac>      = Ethernet MAC address\n"  );         
        }
    }

    return return_code;
} 

#else /* (BSP_ENET_DEVICE_COUNT > 0) */
int_32 Shell_arpadd(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;
}
int_32 Shell_arpdel(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;   
}
int_32 Shell_arpdisp(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;   
}
#endif /* (BSP_ENET_DEVICE_COUNT > 0) */

#else /* BSP_ENET_DEVICE_COUNT */
int_32 Shell_arpadd(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;   
}
int_32 Shell_arpdel(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;   
}
int_32 Shell_arpdisp(int_32 argc, char_ptr argv[] )
{
   printf ("Cannot use this command, no enet device driver available in this BSP.");
   return SHELL_EXIT_ERROR;   
}
#endif /* BSP_ENET_DEVICE_COUNT */
#endif /* SHELLCFG_USES_RTCS */

/* EOF */

