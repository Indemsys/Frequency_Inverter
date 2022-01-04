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
* $FileName: telnet.c$
* $Version : 3.8.7.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the Telnet client implementation.
*
*END************************************************************************/

#include <rtcs.h>
#include <fio.h>
#include "telnet.h"


/*TASK*-----------------------------------------------------------------
*
* Function Name  : TELNET_connect
* Returned Value : error code
* Comments       : A TCP/IP based Telnet Client
*
*END------------------------------------------------------------------*/

uint_32 TELNET_connect
   (
      _ip_address    ipaddress
   )
{ /* Body */
   MQX_FILE_PTR   sockfd, telnetfd;
   sockaddr_in    addr;
   uint_32        sock;
   uint_32        error;
   boolean        work;
   int_32         c;

   /*
   ** Install device driver for socket and telnet
   */
   _io_socket_install("socket:");
   _io_telnet_install("telnet:");

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      return RTCSERR_OUT_OF_SOCKETS;
   } /* Endif */

   addr.sin_family      = AF_INET;
   addr.sin_port        = 0;
   addr.sin_addr.s_addr = INADDR_ANY;
   error = bind(sock,(const sockaddr *)&addr, sizeof(addr));
   if (error != RTCS_OK) {
      return error;
   } /* Endif */

   addr.sin_port        = IPPORT_TELNET;
   addr.sin_addr.s_addr = ipaddress;

   error = connect(sock, (const sockaddr *)(&addr), sizeof(addr));
   if (error != RTCS_OK) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return error;
   } /* Endif */

   sockfd = fopen("socket:", (char_ptr)sock);
   if (sockfd == NULL) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */

   telnetfd = fopen("telnet:", (char_ptr)sockfd);
   if (telnetfd == NULL) {
      fclose(sockfd);
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */

   /* Set the console stream to the client  */
   ioctl(telnetfd, IO_IOCTL_SET_STREAM, (uint_32_ptr)((void _PTR_)stdin));
   while (TRUE) {

      work = FALSE;
      if (fstatus(stdin)) {
         work = TRUE;
         c = (int_32)fgetc(stdin);
         if (fputc(c & 0x7F, telnetfd) == IO_EOF)  {
            break;   
         }
      } /* Endif */

      if (fstatus(telnetfd)) {
         work = TRUE;
         c = (int_32)fgetc(telnetfd);
         if (c == IO_EOF) {
            break;
         }/* Endif */
         fputc(c & 0x7F, stdout);
      } /* Endif */

      /* Let another task run if there is no I/O */
      if (!work) {
         RTCS_time_delay(1);
      } /* Endif */

   } /* Endwhile */

   fclose(telnetfd);
   fclose(sockfd);
   shutdown(sock, FLAG_CLOSE_TX);

   return RTCS_OK;

} /* Endbody */

/* EOF */
