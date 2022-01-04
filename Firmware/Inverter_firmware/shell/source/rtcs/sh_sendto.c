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
* $FileName: sh_sendto.c$
* $Version : 3.8.7.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Example using RTCS Library.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h" 
#if SHELLCFG_USES_RTCS
#include <rtcs.h>
#include "sh_rtcs.h" 



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_sendto
* Returned Value   :  int_32 error code
* Comments  :  Send a UDP broadcase packet to the local network.
*
*END*---------------------------------------------------------------------*/

int_32  Shell_sendto(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   uint_32           length = 64, count = 1, port =0;
   _ip_address       hostaddr = 0;
   char              hostname[MAX_HOSTNAMESIZE];

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc<3) || (argc>5)) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         if (argc>4) {
            if (! Shell_parse_number( argv[4], &count ))  {
               printf("Error, invalid count\n");
               return_code = SHELL_EXIT_ERROR;
            }
         }
         if (argc>3) {
            if (! Shell_parse_number( argv[3], &length ))  {
               printf("Error, invalid length\n");
               return_code = SHELL_EXIT_ERROR;
            }
         }

         if (! Shell_parse_number( argv[2], &port ))  {
            printf("Error, invalid port\n");
            return_code = SHELL_EXIT_ERROR;
         }

         RTCS_resolve_ip_address( argv[1], &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (!hostaddr)  {
            printf("Unable to resolve host\n");
            return_code = SHELL_EXIT_ERROR;
         } else  {
            printf("Sending to %s [%ld.%ld.%ld.%ld]:\n", hostname, IPBYTES(hostaddr));
         }

      }
      if (return_code != SHELL_EXIT_ERROR) {
         uchar_ptr   buffer = _mem_alloc(length);
         uint_32     sock = socket(PF_INET, SOCK_DGRAM, 0);
         sockaddr_in laddr, raddr;
         uint_32     result;
         uint_32     i;
         uint_16     rlen = sizeof(raddr);

         if ((buffer!=NULL) && (sock!=RTCS_SOCKET_ERROR)) {
            memset((char *) &laddr, 0, sizeof(laddr));
            laddr.sin_family = AF_INET;
            laddr.sin_port = 0;
            laddr.sin_addr.s_addr = INADDR_ANY;

            raddr.sin_family = AF_INET;
            raddr.sin_port = (uint_16) port;
            raddr.sin_addr.s_addr = hostaddr;


            result = bind(sock, (const struct sockaddr *)&laddr, sizeof (sockaddr_in));
            if (result != RTCS_OK) {   
               printf("Error, unable to bind socket\n");
               return_code = SHELL_EXIT_ERROR;
            } else {

               for (i=0;i<length;i++) {
                  buffer[i] = (uchar) (i & 0xff);
               }
               printf("\n");
               for (i=0;i<count;i++) {
                  printf("Sending  packet # %d\r",i+1);
                  sendto(sock, buffer, length, 0, (sockaddr *)&raddr, rlen);
               }
               printf("\n");
            }
         } else {
            printf("Error, unable to allocate resources for this command\n");
            return_code = SHELL_EXIT_ERROR;
         }
         if (sock!=RTCS_SOCKET_ERROR) {
            shutdown(sock, FLAG_ABORT_CONNECTION);
         }
         if (buffer!=NULL) {
            _mem_free(buffer);
         }
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <host> <port> [<length>] [<count>]\n", argv[0]);
      } else  {
         printf("Usage: %s <host> <port> [<length>] [<count>]\n", argv[0]);
         printf("   <host>   = destination host\n");
         printf("   <port>   = destination port\n");
         printf("   <length> = number of bytes\n");
         printf("   <count>  = number of messages\n");
      }
   }
   return return_code;
} /* Endbody */
#endif /* SHELLCFG_USES_RTCS */
/* EOF*/
