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
* $FileName: rtcssh.c$
* $Version : 3.6.9.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <rtcs.h>
#include <serial.h>
#include <fio.h>


/*TASK*-----------------------------------------------------------------
*
* Function Name    : RTCS_netstat_task
* Returned Value   : none
* Comments  :  Start the RTCS shell
*
*END*-----------------------------------------------------------------*/

void RTCS_netstat_task
   (
      uint_32 console
   )
{ /* Body */
   MQX_FILE_PTR console_fd;

   if ((char_ptr)console != NULL) {
      console_fd = fopen((char_ptr)console, (char_ptr)(IO_SERIAL_XON_XOFF | IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO));
      if (console_fd == NULL) {
         return;
      } /* Endif */
      _io_set_handle(IO_STDIN,  console_fd);
      _io_set_handle(IO_STDOUT, console_fd);
   } /* Endif */

   RTCS_shell();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  RTCS_shell
*  Returned Value:  none
*  Comments  :  The RTCS shell
*
*END*-----------------------------------------------------------------*/

void RTCS_shell
   (
      void
   )
{ /* Body */
   char                 string[64];
   char_ptr             arg0, arg1, arg2;

#if RTCSCFG_ENABLE_DNS   
   HOSTENT_STRUCT_PTR   host_ptr;
   _ip_address          hostaddr;
   char                 addrstr[16];
   uint_16              pingid = 0; 
   int_32               error;  
   uint_32              i, time;   
#endif   
#if RTCSCFG_ENABLE_IP_STATS
   IP_STATS_PTR         ip   = IP_stats();
#endif
#if RTCSCFG_ENABLE_ICMP_STATS
   ICMP_STATS_PTR       icmp = ICMP_stats();
#endif
#if RTCSCFG_ENABLE_UDP_STATS
   UDP_STATS_PTR        udp  = UDP_stats();
#endif
#if RTCSCFG_ENABLE_TCP_STATS
   TCP_STATS_PTR        tcp  = TCP_stats();
#endif

   printf("\nRTCS v%d.%02d.%02d shell\n",
          RTCS_VERSION_MAJOR, RTCS_VERSION_MINOR, RTCS_VERSION_REV);

   while (TRUE) {

      printf("RTCS> ");

      if (!fgets(string, sizeof(string), stdin)) {
         break;
      } /* Endif */

      arg0 = string;
      while (*arg0 && !isgraph(*arg0)) arg0++;
      if (!*arg0) continue;

      arg1 = arg0;
      while (isgraph(*arg1)) arg1++;
      if (*arg1) *arg1++ = '\0';
      while (*arg1 && !isgraph(*arg1)) arg1++;

      if (!*arg1) {
         arg1 = NULL;
      } else {
         arg2 = arg1;
         while (isgraph(*arg2)) arg2++;
         *arg2 = '\0';
      } /* Endif */

      if (strcmp(arg0, "netstat") == 0) {

#if RTCSCFG_ENABLE_IP_STATS
       printf("\nIP:\n");
         printf("\t%ld packets received\n",                  ip->COMMON.ST_RX_TOTAL);
         printf("\t%ld packets delivered\n",                 ip->ST_RX_DELIVERED);
         printf("\t%ld packets forwarded\n",                 ip->ST_RX_FORWARDED);
         printf("\t%ld discarded for lack of resources\n",   ip->COMMON.ST_RX_MISSED);
         printf("\t%ld discarded due to internal errors\n",  ip->COMMON.ST_RX_ERRORS);
         printf("\t%ld discarded for other reasons:\n",      ip->COMMON.ST_RX_DISCARDED);
         printf("\t\t%ld with header errors\n",              ip->ST_RX_HDR_ERRORS);
         printf("\t\t%ld with an illegal destination\n",     ip->ST_RX_ADDR_ERRORS);
         printf("\t\t%ld with unknown protocols\n",          ip->ST_RX_NO_PROTO);
         printf("\t%ld fragments received\n",                ip->ST_RX_FRAG_RECVD);
         printf("\t%ld fragments reassembled\n",             ip->ST_RX_FRAG_REASMD);
         printf("\t%ld fragments discarded\n",               ip->ST_RX_FRAG_DISCARDED);
         printf("\t%ld packets sent\n",                      ip->COMMON.ST_TX_TOTAL);
         printf("\t%ld unsent for lack of resources\n",      ip->COMMON.ST_TX_MISSED);
         printf("\t%ld unsent due to internal errors\n",     ip->COMMON.ST_TX_ERRORS);
         printf("\t%ld destinations found unreachable\n",    ip->COMMON.ST_TX_DISCARDED);
         printf("\t%ld packets fragmented\n",                ip->ST_TX_FRAG_FRAGD);
         printf("\t%ld fragments sent\n",                    ip->ST_TX_FRAG_SENT);
         printf("\t%ld fragmentation failures\n",            ip->ST_TX_FRAG_DISCARDED);
         printf("-- More --"); getchar();
#endif

#if RTCSCFG_ENABLE_ICMP_STATS
         printf("\nICMP:\n");
         printf("\t%ld packets received\n",                  icmp->COMMON.ST_RX_TOTAL);
         printf("\t%ld discarded for lack of resources\n",   icmp->COMMON.ST_RX_MISSED);
         printf("\t%ld discarded due to internal errors\n",  icmp->COMMON.ST_RX_ERRORS);
         printf("\t%ld discarded for other reasons:\n",      icmp->COMMON.ST_RX_DISCARDED);
         printf("\t\t%ld with header errors\n",              icmp->ST_RX_BAD_CHECKSUM + icmp->ST_RX_SMALL_DGRAM);
         printf("\t\t%ld unrecognized codes\n",              icmp->ST_RX_BAD_CODE);
         printf("\t\t%ld redirects from non-gateways\n",     icmp->ST_RX_RD_NOTGATE);
         printf("\t%ld packets sent\n",                      icmp->COMMON.ST_TX_TOTAL);
         printf("\t%ld discarded for lack of resources\n",   icmp->COMMON.ST_TX_MISSED);
         printf("\t%ld discarded due to internal errors\n",  icmp->COMMON.ST_TX_ERRORS);
         printf("\t%ld with illegal type or code\n",         icmp->COMMON.ST_TX_DISCARDED);
         printf("\tInput histogram:\n");
         if (icmp->ST_RX_DESTUNREACH) printf("\t\t%ld Destination Unreachables\n", icmp->ST_RX_DESTUNREACH);
         if (icmp->ST_RX_TIMEEXCEED)  printf("\t\t%ld Time Exceededs\n",           icmp->ST_RX_TIMEEXCEED);
         if (icmp->ST_RX_PARMPROB)    printf("\t\t%ld Parameter Problems\n",       icmp->ST_RX_PARMPROB);
         if (icmp->ST_RX_SRCQUENCH)   printf("\t\t%ld Source Quenches\n",          icmp->ST_RX_SRCQUENCH);
         if (icmp->ST_RX_REDIRECT)    printf("\t\t%ld Redirects\n",                icmp->ST_RX_REDIRECT);
         if (icmp->ST_RX_ECHO_REQ)    printf("\t\t%ld Echo Requests\n",            icmp->ST_RX_ECHO_REQ);
         if (icmp->ST_RX_ECHO_REPLY)  printf("\t\t%ld Echo Replies\n",             icmp->ST_RX_ECHO_REPLY);
         if (icmp->ST_RX_TIME_REQ)    printf("\t\t%ld Timestamp Requests\n",       icmp->ST_RX_TIME_REQ);
         if (icmp->ST_RX_TIME_REPLY)  printf("\t\t%ld Timestamp Replies\n",        icmp->ST_RX_TIME_REPLY);
         if (icmp->ST_RX_INFO_REQ)    printf("\t\t%ld Information Requests\n",     icmp->ST_RX_INFO_REQ);
         if (icmp->ST_RX_INFO_REPLY)  printf("\t\t%ld Information Replies\n",      icmp->ST_RX_INFO_REPLY);
         if (icmp->ST_RX_OTHER)       printf("\t\t%ld Unknown\n",                  icmp->ST_RX_OTHER);
         printf("\tOutput histogram:\n");
         if (icmp->ST_TX_DESTUNREACH) printf("\t\t%ld Destination Unreachables\n", icmp->ST_TX_DESTUNREACH);
         if (icmp->ST_TX_TIMEEXCEED)  printf("\t\t%ld Time Exceededs\n",           icmp->ST_TX_TIMEEXCEED);
         if (icmp->ST_TX_PARMPROB)    printf("\t\t%ld Parameter Problems\n",       icmp->ST_TX_PARMPROB);
         if (icmp->ST_TX_SRCQUENCH)   printf("\t\t%ld Source Quenches\n",          icmp->ST_TX_SRCQUENCH);
         if (icmp->ST_TX_REDIRECT)    printf("\t\t%ld Redirects\n",                icmp->ST_TX_REDIRECT);
         if (icmp->ST_TX_ECHO_REQ)    printf("\t\t%ld Echo Requests\n",            icmp->ST_TX_ECHO_REQ);
         if (icmp->ST_TX_ECHO_REPLY)  printf("\t\t%ld Echo Replies\n",             icmp->ST_TX_ECHO_REPLY);
         if (icmp->ST_TX_TIME_REQ)    printf("\t\t%ld Timestamp Requests\n",       icmp->ST_TX_TIME_REQ);
         if (icmp->ST_TX_TIME_REPLY)  printf("\t\t%ld Timestamp Replies\n",        icmp->ST_TX_TIME_REPLY);
         if (icmp->ST_TX_INFO_REQ)    printf("\t\t%ld Information Requests\n",     icmp->ST_TX_INFO_REQ);
         if (icmp->ST_TX_INFO_REPLY)  printf("\t\t%ld Information Replies\n",      icmp->ST_TX_INFO_REPLY);
         if (icmp->ST_TX_OTHER)       printf("\t\t%ld Unknown\n",                  icmp->ST_TX_OTHER);
         printf("-- More --"); getchar();
#endif

#if RTCSCFG_ENABLE_UDP_STATS
         printf("\nUDP:\n");
         printf("\t%ld packets received\n",                  udp->COMMON.ST_RX_TOTAL);
         printf("\t%ld discarded for lack of resources\n",   udp->COMMON.ST_RX_MISSED);
         printf("\t%ld discarded due to internal errors\n",  udp->COMMON.ST_RX_ERRORS);
         printf("\t%ld discarded for other reasons:\n",      udp->COMMON.ST_RX_DISCARDED);
         printf("\t\t%ld with header errors\n",              udp->ST_RX_BAD_PORT + udp->ST_RX_BAD_CHECKSUM + udp->ST_RX_SMALL_DGRAM + udp->ST_RX_SMALL_PKT);
         printf("\t\t%ld with unknown ports\n",              udp->ST_RX_NO_PORT);
         printf("\t%ld packets sent\n",                      udp->COMMON.ST_TX_TOTAL);
         printf("\t%ld unsent for lack of resources\n",      udp->COMMON.ST_TX_MISSED);
         printf("\t%ld unsent due to internal errors\n",     udp->COMMON.ST_TX_ERRORS);
         printf("\t%ld with illegal destination port\n",     udp->COMMON.ST_TX_DISCARDED);
         printf("-- More --"); getchar();
#endif

#if RTCSCFG_ENABLE_TCP_STATS
         printf("\nTCP:\n");
         printf("\t%ld packets received\n",                  tcp->COMMON.ST_RX_TOTAL);
         printf("\t%ld discarded for lack of resources\n",   tcp->COMMON.ST_RX_MISSED);
         printf("\t%ld discarded due to internal errors\n",  tcp->COMMON.ST_RX_ERRORS);
         printf("\t%ld discarded for other reasons:\n",      tcp->COMMON.ST_RX_DISCARDED);
         printf("\t\t%ld with header errors\n",              tcp->ST_RX_BAD_PORT + tcp->ST_RX_BAD_CHECKSUM + tcp->ST_RX_SMALL_HDR + tcp->ST_RX_SMALL_DGRAM + tcp->ST_RX_SMALL_PKT);
         printf("\t\t%ld acks for unsent data\n",            tcp->ST_RX_BAD_ACK);
         printf("\t\t%ld with data outside window\n",        tcp->ST_RX_BAD_DATA);
         printf("\t\t%ld with data after close\n",           tcp->ST_RX_LATE_DATA);
         printf("\t%ld segments with data\n",                tcp->ST_RX_DATA);
         printf("\t%ld segments with duplicate data\n",      tcp->ST_RX_DATA_DUP);
         printf("\t%ld segments with only an ACK\n",         tcp->ST_RX_ACK);
         printf("\t%ld segments with a duplicate ACK\n",     tcp->ST_RX_ACK_DUP);
         printf("\t%ld segments with a RST\n",               tcp->ST_RX_RESET);
         printf("\t%ld window probes\n",                     tcp->ST_RX_PROBE);
         printf("\t%ld window updates\n",                    tcp->ST_RX_WINDOW);
         printf("-- More --"); getchar();
         printf("\t%ld packets sent\n",                      tcp->COMMON.ST_TX_TOTAL);
         printf("\t%ld discarded for lack of resources\n",   tcp->COMMON.ST_TX_MISSED);
         printf("\t%ld discarded due to internal errors\n",  tcp->COMMON.ST_TX_ERRORS);
         printf("\t%ld with illegal destination port\n",     tcp->COMMON.ST_TX_DISCARDED);
         printf("\t%ld segments with data\n",                tcp->ST_TX_DATA);
         printf("\t%ld segments with retransmitted data\n",  tcp->ST_TX_DATA_DUP);
         printf("\t%ld segments with only an ACK\n",         tcp->ST_TX_ACK);
         printf("\t%ld segments with a delayed ACK\n",       tcp->ST_TX_ACK_DELAYED);
         printf("\t%ld segments with a RST\n",               tcp->ST_TX_RESET);
         printf("\t%ld window probes\n",                     tcp->ST_TX_PROBE);
         printf("\t%ld window updates\n",                    tcp->ST_TX_WINDOW);
         printf("\t%ld active opens\n",                      tcp->ST_CONN_ACTIVE);
         printf("\t%ld passive opens\n",                     tcp->ST_CONN_PASSIVE);
         printf("\t%ld connections currently established\n", tcp->ST_CONN_OPEN);
         printf("\t%ld connections gracefully closed\n",     tcp->ST_CONN_CLOSED);
         printf("\t%ld connections aborted\n",               tcp->ST_CONN_RESET);
         printf("\t%ld failed connection attempts\n",        tcp->ST_CONN_FAILED);
#endif

#if RTCSCFG_ENABLE_DNS
      } else if (strcmp(arg0, "ping") == 0) {

         host_ptr = gethostbyname(arg1);
         if ((host_ptr == NULL)
          || (*host_ptr->h_addr_list == NULL)) {
            printf("Unable to resolve IP address for host: %s\n", arg1);
            if (host_ptr) DNS_free_HOSTENT_STRUCT(host_ptr);
            continue;
         } /* Endif */

         hostaddr = *(_ip_address _PTR_)*host_ptr->h_addr_list;

         sprintf(addrstr, "%ld.%ld.%ld.%ld", ((hostaddr>>24)&0xFF),
                                             ((hostaddr>>16)&0xFF),
                                             ((hostaddr>> 8)&0xFF),
                                             ( hostaddr     &0xFF));

         printf("Pinging %s [%s]:\n", host_ptr->h_name, addrstr);
         DNS_free_HOSTENT_STRUCT(host_ptr);

         for (i=0; i<4; i++) {
            time = 5000; /* 5 seconds */
            error = RTCS_ping(hostaddr, &time, ++pingid, 0, 0, 0);
            if (error == RTCSERR_ICMP_ECHO_TIMEOUT) {
               printf("Request timed out\n");
            } else if (error) {
               printf("Error 0x%04lX\n", error);
            } else {
               printf("Reply from %s: time=%ldms\n", addrstr, time);
               if ((time<1000) && (i<3)) {
                  RTCS_time_delay(1000-time);
               } /* Endif */
            } /* Endif */
         } /* Endfor */

      } else if (strcmp(arg0, "telnet") == 0) {

         host_ptr = gethostbyname(arg1);
         if ((host_ptr == NULL)
          || (*host_ptr->h_addr_list == NULL)) {
            printf("Unable to resolve IP address for host: %s\n", arg1);
            if (host_ptr) DNS_free_HOSTENT_STRUCT(host_ptr);
            continue;
         } /* Endif */

         hostaddr = *(_ip_address _PTR_)*host_ptr->h_addr_list;

         sprintf(addrstr, "%ld.%ld.%ld.%ld", ((hostaddr>>24)&0xFF),
                                             ((hostaddr>>16)&0xFF),
                                             ((hostaddr>> 8)&0xFF),
                                             ( hostaddr     &0xFF));

         printf("Trying %s [%s]:\n", host_ptr->h_name, addrstr);
         DNS_free_HOSTENT_STRUCT(host_ptr);

         error = TELNET_connect(hostaddr);
         if (error) {
            printf("Connection failed, error 0x%04lX\n", error);
         } /* Endif */
         printf("Connection closed\n", error);
#endif
      } else if (strcmp(arg0, "help") == 0) {

         printf("Available commands:\n");
         printf("\tnetstat       \tdisplay RTCS statistics\n");
         printf("\tping <host>   \tping a host\n");
         printf("\ttelnet <host> \ttelnet to a host\n");
         printf("\thelp          \tdisplay this message\n");
         printf("\texit          \texit the shell\n");

      } else if (strcmp(arg0, "exit") == 0) {

         break;

      } else {

         printf("Invalid command.  Type 'help' for a list of commands.\n");

      } /* Endif */

   } /* Endwhile */

} /* Endbody */


/* EOF */
