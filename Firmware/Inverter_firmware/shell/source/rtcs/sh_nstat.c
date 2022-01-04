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
* $FileName: sh_nstat.c$
* $Version : 3.8.7.0$
* $Date    : Jun-7-2012$
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
#include <enet.h>
#include "sh_rtcs.h"



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_netstat
*  Returned Value:  none
*  Comments  :  The shell function to print RTCS netstats
*
*END*-----------------------------------------------------------------*/

int_32 Shell_netstat(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;

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
#if  BSPCFG_ENABLE_ENET_STATS
   _enet_handle         handle;
   ENET_STATS_PTR       enet = NULL;
#endif
   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );
   
   if (!print_usage)  {
#if BSPCFG_ENABLE_ENET_STATS
       handle = _mqx_get_io_component_handle(IO_ENET_COMPONENT);
       while (handle) {
           #if BSP_ENET_DEVICE_COUNT > 0
           enet = ENET_get_stats(handle);
           printf("\nENET %x:\n",handle );
           printf("\t%ld packets received\n",               enet->COMMON.ST_RX_TOTAL);
           printf("\t%ld rx packets missed\n",              enet->COMMON.ST_RX_MISSED);
           printf("\t%ld rx packets discarded\n",           enet->COMMON.ST_RX_DISCARDED);
           printf("\t%ld rx packets with error\n",          enet->COMMON.ST_RX_ERRORS);
           
           printf("\t%ld packets received\n",               enet->COMMON.ST_TX_TOTAL);
           printf("\t%ld tx packets missed\n",              enet->COMMON.ST_TX_MISSED);
           printf("\t%ld tx packets discarded\n",           enet->COMMON.ST_TX_DISCARDED);
           printf("\t%ld tx packets received with error\n", enet->COMMON.ST_TX_ERRORS);

        /* Following stats are physical errors/conditions */
           printf("\t%ld rx align error\n",        enet->ST_RX_ALIGN);
           printf("\t%ld rx fcs error\n",          enet->ST_RX_FCS);
           printf("\t%ld rx runt error\n",         enet->ST_RX_RUNT);
           printf("\t%ld rx giant error\n",        enet->ST_RX_GIANT);
           printf("\t%ld rx latecoll error\n",     enet->ST_RX_LATECOLL);
           printf("\t%ld rx overrun error\n",      enet->ST_RX_OVERRUN);

           printf("\t%ld tx SQE\n",                enet->ST_TX_SQE);
           printf("\t%ld tx deferred\n",           enet->ST_TX_DEFERRED);
           printf("\t%ld tx late collision\n",     enet->ST_TX_LATECOLL);
           printf("\t%ld tx excessive collision\n",enet->ST_TX_EXCESSCOLL);
           printf("\t%ld tx carrier\n",            enet->ST_TX_CARRIER);
           printf("\t%ld tx unerrun error\n",      enet->ST_TX_UNDERRUN);

          handle = ENET_get_next_device_handle(handle);
          #endif /* BSP_ENET_DEVICE_COUNT > 0 */
       }
       
#endif

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
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s\n", argv[0]);
      } else  {
         printf("Usage: %s\n", argv[0]);
      }
   }
   return SHELL_EXIT_SUCCESS;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */
/* EOF */
