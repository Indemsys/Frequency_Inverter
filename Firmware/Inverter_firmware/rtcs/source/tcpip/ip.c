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
* $FileName: ip.c$
* $Version : 3.8.20.0$
* $Date    : Oct-9-2012$
*
* Comments:
*
*   This file contains the implementation of the Internet
*   Protocol.  For more details, refer to RFC791 and RFC1122.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_init
* Returned Values : RTCS_OK or error code
* Comments        :
*     Initialize the IP layer.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4  

uint_32 IP_init
   (
      void
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;

   IP_cfg_ptr = RTCS_mem_alloc_zero(sizeof(IP_CFG_STRUCT));

   if (IP_cfg_ptr == NULL)  {
      return RTCSERR_OUT_OF_MEMORY;
   }

   _mem_set_type(IP_cfg_ptr, MEM_TYPE_IP_CFG_STRUCT);
   
   RTCS_setcfg(IP, IP_cfg_ptr);

   RTCS_LIST_INIT(IP_cfg_ptr->ICB_HEAD);
   IP_cfg_ptr->NEXT_ID  = 0;
   IP_cfg_ptr->DEFAULT_TTL = IPTTL_DEFAULT;

   IP_cfg_ptr->ROUTE_PARTID  = RTCS_part_create(sizeof(IP_ROUTE_DIRECT),
      IPROUTEALLOC_SIZE, IPROUTEALLOC_SIZE, 0, NULL, NULL);
#if RTCSCFG_ENABLE_GATEWAYS
   IP_cfg_ptr->GATE_PARTID   = RTCS_part_create(sizeof(IP_ROUTE_INDIRECT),
      IPGATEALLOC_SIZE,  IPGATEALLOC_SIZE,  0, NULL, NULL);
#endif
#if RTCSCFG_ENABLE_VIRTUAL_ROUTES
   IP_cfg_ptr->VIRTUAL_PARTID   = RTCS_part_create(sizeof(IP_ROUTE_VIRTUAL),
      IPROUTEALLOC_SIZE,  IPROUTEALLOC_SIZE,  0, NULL, NULL);
#endif
   IP_cfg_ptr->ROUTE_FN      = NULL;
#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   IP_cfg_ptr->MCB_PARTID    = RTCS_part_create(sizeof(MC_MEMBER),
      IPMCBALLOC_SIZE,   IPMCBALLOC_SIZE,   0, NULL, NULL);
#endif
#if RTCSCFG_ENABLE_IPSEC
   IP_cfg_ptr->SEC_SERVICE_FN = NULL;
   IP_cfg_ptr->SEC_SEND_FN    = NULL;
   IP_cfg_ptr->SEC_FN_ARG     = NULL;
#endif
   IP_cfg_ptr->RADIX_PARTID  = RTCS_part_create(sizeof(IP_ROUTE),
      RADIXALLOC_SIZE,   RADIXALLOC_SIZE,   0, NULL, NULL);

   IP_route_init(&IP_cfg_ptr->ROUTE_ROOT);

   return RTCS_OK;
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_send
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP packet generated on the local host.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_send
   (
      RTCSPCB_PTR    rtcs_pcb,
            /* [IN] the packet to send */
      uint_32        protocol,
            /* [IN] the transport layer protocol */
      _ip_address    ipsrc,
            /* [IN] the destination interface (0 = any) */
      _ip_address    ipdest,
            /* [IN] the ultimate destination */
      uint_32        flags
            /* [IN] optional flags */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   IP_CFG_STRUCT_PTR    IP_cfg_ptr;

   IP_cfg_ptr = RTCS_getcfg(IP);

   IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);

   rtcs_pcb->IP_COMPLETE = IP_complete_send;
   rtcs_pcb->IFSRC       = RTCS_IF_LOCALHOST_PRV;

   /*
   ** Validate destination address
   */
   if (IN_ZERONET(ipdest)) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_DISCARDED++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_ADDRESS);
      RTCSPCB_free(rtcs_pcb);
      return RTCSERR_IP_BAD_ADDRESS;
   } /* Endif */

   if ((ipdest == INADDR_BROADCAST)
    || IN_MULTICAST(ipdest)) {
      return IP_route_multi(rtcs_pcb, protocol, ipsrc, ipdest, flags);
   } else {
      return IP_route(rtcs_pcb, protocol, ipsrc, INADDR_ANY, ipdest, flags);
   } /* Endif */

#else

    RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_IS_DISABLED);
    RTCSPCB_free(rtcs_pcb);
    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_send_IF
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP packet through a specified interface.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_send_IF
   (
      RTCSPCB_PTR    rtcs_pcb,
            /* [IN] the packet to send */
      uint_32        protocol,
            /* [IN] the transport layer protocol */
      pointer        interface
            /* [IN] the destination interface */
   )
{ 

#if RTCSCFG_ENABLE_IP4

   IP_CFG_STRUCT_PTR    IP_cfg_ptr;
   IP_IF_PTR            ifdest = interface;

   IP_cfg_ptr = RTCS_getcfg(IP);

   IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);


   rtcs_pcb->IP_COMPLETE = IP_complete_send;
   rtcs_pcb->IFSRC       = RTCS_IF_LOCALHOST_PRV;

   return IP_send_dgram(ifdest, rtcs_pcb, INADDR_ANY, INADDR_BROADCAST,
      INADDR_BROADCAST, protocol, NULL);
      
#else

    RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_IS_DISABLED);
    RTCSPCB_free(rtcs_pcb);
    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */  

} 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_complete_send
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Generates an IP header for outgoing packets.  This function
*        must not consume pcb.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4

uint_32 IP_complete_send
   (
      pointer           ifdest,
            /* [IN] the destination interface */
      RTCSPCB_PTR _PTR_ rtcs_pcb_ptr_ptr,
            /* [IN] the packet to send */
      _ip_address       hopsrc,
            /* [IN] the destination interface */
       _ip_address      ipdest,
            /* [IN] the ultimate dest */
      uint_32           protocol
            /* [IN] the transport layer protocol */
   )
{ /* Body */
   RTCSPCB_PTR            rtcs_pcb = *rtcs_pcb_ptr_ptr;
   IP_CFG_STRUCT_PTR      IP_cfg_ptr;
   IP_HEADER_PTR          packet;
   uchar                  proto, ttl, tos;
   uint_32                error;
   uint_16                chksum;
   uchar                  dfrag;

   IP_cfg_ptr = RTCS_getcfg(IP);

   error = RTCSPCB_insert_header(rtcs_pcb, sizeof(IP_HEADER));
   if (error) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      IF_IP_STATS_ENABLED(RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
      /* The PCB will be freed for us. */
      return error;
   } /* Endif */

   RTCSLOG_PCB_WRITE(rtcs_pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IP), IP_VERSION);
   packet = (IP_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
   RTCSPCB_DATA_NETWORK(rtcs_pcb) = RTCSPCB_DATA(rtcs_pcb);
   proto  = IPPROTO_GET(protocol);
   tos    = IPTOS_GET(protocol);
   if (!tos) 
     tos = rtcs_pcb->LINK_OPTIONS.TX.TOS;
   dfrag  = IPDFRAG_GET(protocol);
   
      
   ttl = rtcs_pcb->LINK_OPTIONS.TX.TTL;
   if (!ttl) ttl = IPTTL_GET(protocol);
   if (!ttl) ttl = IP_cfg_ptr->DEFAULT_TTL;

   htonc(packet->VERSLEN,  (IP_VERSION << 4) | (sizeof(IP_HEADER) >> 2));
   htonc(packet->TOS,      tos);
   htons(packet->LENGTH,   RTCSPCB_SIZE(rtcs_pcb));
   htons(packet->ID,       IP_cfg_ptr->NEXT_ID);
   IP_cfg_ptr->NEXT_ID++;
   if (dfrag) {
      htons(packet->FRAGMENT, IP_FRAG_DF);
   } else {
      htons(packet->FRAGMENT, 0);
   } /* Endif */
   htonc(packet->TTL,      ttl);
   htonc(packet->PROTOCOL, proto);
   htonl(packet->DEST,     ipdest);
   htons(packet->CHECKSUM, 0);
   htonl(packet->SOURCE,   hopsrc);

   /* Set up the transport layer protocol and delta. */
   RTCSPCB_SET_TRANS_PROTL(rtcs_pcb, proto);
   RTCSPCB_SET_TRANS_DELTA(rtcs_pcb, sizeof(IP_HEADER));

   /*
   ** If the transport layer needs a pseudo header checksum,
   ** calculate it now.
   */
   if (rtcs_pcb->IP_SUM_PTR) {
      chksum = IP_Sum_pseudo(rtcs_pcb->IP_SUM, rtcs_pcb, 0);
      chksum = IP_Sum_invert(chksum);
      htons(rtcs_pcb->IP_SUM_PTR, chksum);
   } /* Endif */

#if RTCSCFG_ENABLE_IPSEC
   if (IP_cfg_ptr->SEC_SEND_FN) {
      error = IP_cfg_ptr->SEC_SEND_FN(IP_cfg_ptr->SEC_FN_ARG, ifdest,
         rtcs_pcb_ptr_ptr, hopsrc, ipdest);
      if (error) {
         /* The calling function will discard the packet for us. */
         return error;
      } /* Endif */
   } /* Endif */
#endif

   rtcs_pcb = *rtcs_pcb_ptr_ptr;

   /* We need to refresh the header location, as it may have been shifted.*/
   packet = (IP_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
   chksum = _mem_sum_ip(0, sizeof(IP_HEADER), packet);
   chksum = IP_Sum_invert(chksum);
   htons(packet->CHECKSUM, chksum);

   /* Special case for loopback */
   if (ifdest == RTCS_IF_LOCALHOST_PRV) {
      RTCSPCB_PTR            new_rtcs_pcb;
      uchar_ptr              data;
      PCB_FRAGMENT_PTR       frag;

      data = RTCS_mem_alloc_system(sizeof(PCB) + sizeof(PCB_FRAGMENT) + RTCSPCB_SIZE(rtcs_pcb));

      if (!data)  {
         return RTCSERR_OUT_OF_MEMORY;
      } /* Endif */

      _mem_set_type(data, MEM_TYPE_IP_DATA);

      frag = ((PCB_PTR)data)->FRAG;
      ((PCB_PTR)data)->FREE = (void(_CODE_PTR_)(PCB_PTR))_mem_free;
      ((PCB_PTR)data)->PRIVATE = NULL;
      frag[0].LENGTH = RTCSPCB_SIZE(rtcs_pcb);
      frag[0].FRAGMENT = data + sizeof(PCB) + sizeof(PCB_FRAGMENT);
      frag[1].LENGTH = 0;
      frag[1].FRAGMENT = NULL;

      new_rtcs_pcb = RTCSPCB_alloc_recv((PCB_PTR)data);
      if (!new_rtcs_pcb)  {
         _mem_free(data);
         return RTCSERR_OUT_OF_MEMORY;
      } /* Endif */

      new_rtcs_pcb->TYPE = rtcs_pcb->TYPE;
      new_rtcs_pcb->IFSRC = rtcs_pcb->IFSRC;
      new_rtcs_pcb->LINK_OPTIONS = rtcs_pcb->LINK_OPTIONS;
      new_rtcs_pcb->IP_COMPLETE = IP_complete_recv;

      data += sizeof(PCB) + sizeof(PCB_FRAGMENT);
      RTCSPCB_memcopy(rtcs_pcb, data, 0, RTCSPCB_SIZE(rtcs_pcb));
      RTCSPCB_DATA_NETWORK(new_rtcs_pcb) = RTCSPCB_DATA(new_rtcs_pcb);
      RTCSPCB_free(rtcs_pcb);

      *rtcs_pcb_ptr_ptr = new_rtcs_pcb;

      return IP_complete_recv(ifdest, rtcs_pcb_ptr_ptr, hopsrc, ipdest, protocol);

   } /* Endif */

   return RTCS_OK;
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_service
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Parses a received IP packet.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4

void IP_service
   (
      RTCSPCB_PTR    rtcs_pcb
            /* [IN] the packet to send */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR          IP_cfg_ptr;
   IP_HEADER_PTR              packet = (IP_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
   _ip_address                ipsrc, ipdest;
   uint_16                    hdrlen, pktlen;
   uint_32                    routeopt, error;
   #if RTCSCFG_ENABLE_NAT
   uint_32 (_CODE_PTR_ _PTR_  nat_exec)(RTCSPCB_PTR _PTR_);
   #endif
   
   RTCSLOG_FNE2(RTCSLOG_FN_IP_service, rtcs_pcb);

   IP_cfg_ptr = RTCS_getcfg(IP);

   IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_TOTAL++);
   hdrlen = (ntohc(packet->VERSLEN) & 0x0F) << 2;
   pktlen = ntohs(packet->LENGTH);
   RTCSPCB_DATA_NETWORK(rtcs_pcb) = RTCSPCB_DATA(rtcs_pcb);

   /*
   ** Make sure that
   **    sizeof(IP_HEADER) <= hdrlen <= pktlen <= RTCSPCB_SIZE(pcb)
   */
   if (hdrlen < sizeof(IP_HEADER)) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_SMALL_HDR++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 1);
      return;
   } /* Endif */
   if (pktlen < hdrlen) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_SMALL_DGRAM++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 2);
      return;
   } /* Endif */
   if (RTCSPCB_SIZE(rtcs_pcb) < pktlen) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_SMALL_PKT++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 3);
      return;
   } /* Endif */

   /*
   ** IP version must be 4
   */
   if ((ntohc(packet->VERSLEN) >> 4) != IP_VERSION) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_BAD_VERSION++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_HEADER);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 4);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(rtcs_pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IP), IP_VERSION);

   /*
   ** Validate source address
   */
   ipsrc = ntohl(packet->SOURCE);
   if (IN_LOOPBACK(ipsrc)
    || IN_MULTICAST(ipsrc)
    || IN_EXPERIMENTAL(ipsrc)) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_BAD_SOURCE++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_ADDRESS);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 5);
      return;
   } 

   /*
   ** Validate destination address
   */
   ipdest = ntohl(packet->DEST);
   if (IN_LOOPBACK(ipdest)
    || IN_ZERONET(ipdest)
    || (IN_EXPERIMENTAL(ipdest) && ipdest != INADDR_BROADCAST)) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_ADDR_ERRORS++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_ADDRESS);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 6);
      return;
   } /* Endif */

   /*
   ** Verify the checksum
   */
   if (_mem_sum_ip(0, hdrlen, packet) != 0xFFFF) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_BAD_CHECKSUM++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_CHECKSUM);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 7);
      return;
   } 

   /*
   ** In this function, the PCB always has only one fragment, so it's
   ** always safe to call RTCSPCB_shrink() here.
   */
   error = RTCSPCB_shrink(rtcs_pcb, RTCSPCB_SIZE(rtcs_pcb) - pktlen);
   if (error) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_ERRORS++);
      IF_IP_STATS_ENABLED(RTCS_seterror(&IP_cfg_ptr->STATS.ERR_RX, error, (uint_32)rtcs_pcb));
      RTCSLOG_PCB_FREE(rtcs_pcb, error);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 8);
      return;
   } 


   rtcs_pcb->IP_COMPLETE = IP_complete_recv;
/* rtcs_pcb->IFSRC has already been set by link layer */

   /*
   ** Now decide what to do with the packet.
   **
   ** Normally, we should do:
   **
   **    if (!FORWARD || !(TYPE & UNICAST))
   **       IP_route_local();
   **    else
   **       IP_route();
   **
   ** i.e. we only forward unicast packets, and only if forwarding is
   ** turned on.  Unfortunately, this isn't sufficient.
   **
   ** If forwarding is turned on, limited broadcasts from PPP links
   ** will be discarded, because on PPP links, (TYPE & UNICAST) is
   ** always true (PPP packets are both unicast and broadcast), and
   ** IP_route() discards limited broadcasts.
   **
   ** Hence, we have to explicitly make sure that limited broadcasts
   ** and multicasts go through IP_route_local().
   **
   ** Once multicast routing is implemented, we'll remove IN_MULTICAST()
   ** from the first condition.
   **
   ** RFC 2131 says:
   ** "In the case of a client using DHCP for initial configuration (before
   ** the client's TCP/IP software has been completely configured), DHCP
   ** requires creative use of the client's TCP/IP software and liberal
   ** interpretation of RFC 1122.  The TCP/IP software SHOULD accept and
   ** forward to the IP layer any IP packets delivered to the client's
   ** hardware address before the IP address is configured."
   ** 
   ** So if the broadcast flag is disabled, we check, and if the packet's 
   ** source interface is not bound, we make sure the packet goes through 
   ** IP_route_local(), where we have the same test to send the packet to 
   ** IF_LOCALHOST.  
   */

   if (!_IP_forward
    || !(rtcs_pcb->TYPE & RTCSPCB_TYPE_UNICAST)
    || (ipdest == INADDR_BROADCAST)
    || IN_MULTICAST(ipdest)
    || (!_DHCP_broadcast && (IP_get_ipif_addr(rtcs_pcb->IFSRC) == INADDR_ANY))) { 

      IP_route_local(rtcs_pcb, ipdest);

   } else {

      if (IN_ZERONET(ipsrc)) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_BAD_SOURCE++);
         RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_IP_BAD_ADDRESS);
         RTCSPCB_free(rtcs_pcb);
         RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 9);
         return;
      } /* Endif */

      /*
      ** If the packet was broadcast, don't send it out the receiving
      ** interface.  This protects mainly against bouncing PPP directed
      ** broadcasts.
      */
      if (rtcs_pcb->TYPE & RTCSPCB_TYPE_BROADCAST) {
         routeopt = IPROUTEOPT_RECVIF;
      } else {
         routeopt = 0;
      } /* Endif */

      #if RTCSCFG_ENABLE_NAT   
      /* Apply NAT if it is installed */
      nat_exec = RTCS_getcfg(NAT);
      if (nat_exec && *nat_exec) {
         error = (*nat_exec)(&rtcs_pcb);
         if (error) {
            if (rtcs_pcb!=NULL) {
               IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
               RTCSLOG_PCB_FREE(rtcs_pcb, error);
               RTCSPCB_free(rtcs_pcb);
            }
            RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 10);
            return;
         } /* Endif */

         // NAT may have consumed the PCB if it was a fragment
         if (rtcs_pcb==NULL) {
            RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 11);
            return;
         }

         /* PCB pointer may have changed */
         rtcs_pcb->IP_COMPLETE = IP_complete_recv;
         packet = (IP_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
         ipdest = ntohl(packet->DEST);
      } /* Endif */
      #endif

      IP_route(rtcs_pcb, 0, INADDR_ANY, ipsrc, ipdest, routeopt);

   } /* Endif */
   
    RTCSLOG_FNX2(RTCSLOG_FN_IP_service, 0);
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_complete_recv
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Decrements the TTL on forwarding packets.  This function
*        must not consume pcb.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 IP_complete_recv
   (
      pointer           ifdest,
            /* [IN] the destination interface */
      RTCSPCB_PTR _PTR_ rtcs_pcb_ptr_ptr,
            /* [IN] the packet to send */
      _ip_address       hopsrc,
            /* [IN] the destination interface */
      _ip_address       ipdest,
      /* [IN] the ultimate dest */
      uint_32           protocol
      /* [IN] the transport layer protocol */
   )
{ /* Body */
   RTCSPCB_PTR          rtcs_pcb = *rtcs_pcb_ptr_ptr;
   IP_CFG_STRUCT_PTR    IP_cfg_ptr;
   IP_HEADER_PTR packet = (IP_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
   uint_16       hdrlen, pktlen;
   uint_16       chksum;
   uchar         ttl;
   uint_32       error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   /* Set up the transport layer protocol and delta. */
   RTCSPCB_SET_TRANS_PROTL(rtcs_pcb, ntohc(packet->PROTOCOL));
   RTCSPCB_SET_TRANS_DELTA(rtcs_pcb, ((ntohc(packet->VERSLEN) & 0x0F) << 2));

#if RTCSCFG_ENABLE_IPSEC
   if (IP_cfg_ptr->SEC_SEND_FN) {
      error = IP_cfg_ptr->SEC_SERVICE_FN(IP_cfg_ptr->SEC_FN_ARG, ifdest,
         rtcs_pcb_ptr_ptr, hopsrc, ipdest);
      if (error) {
         /* The calling function will discard the packet for us. */
         return error;
      } /* Endif */
   } /* Endif */
#endif

   /* Either no security database exists, or the packet has been decoded, or */
   /* the packet has been let through unchanged (bypass). */
   rtcs_pcb = *rtcs_pcb_ptr_ptr;

   if ((IP_IF_PTR)ifdest != RTCS_IF_LOCALHOST_PRV) {
      /*
      ** Check the TTL
      */

      ttl = ntohc(packet->TTL);
      if (ttl <= 1) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_HDR_ERRORS++);
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_TTL_EXCEEDED++);
         #if RTCSCFG_ENABLE_ICMP
         ICMP_send_error(ICMPTYPE_TIMEEXCEED, ICMPCODE_TE_TTL, 0, rtcs_pcb, 0);
         #endif
         return RTCSERR_IP_TTL;
      } /* Endif */

      /*
      ** Fork the PCB and duplicate the IP header.  Also copy up to
      ** 8 bytes of data in case we need to generate an ICMP error.
      */

      hdrlen = (ntohc(packet->VERSLEN) & 0x0F) << 2;
      pktlen = ntohs(packet->LENGTH);
      if (pktlen > hdrlen + 8) {
         pktlen = hdrlen + 8;
      } /* Endif */

      error = RTCSPCB_next(rtcs_pcb, pktlen);
      if (error) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_IP_STATS_ENABLED(RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
         return error;
      } /* Endif */

      error = RTCSPCB_fork(rtcs_pcb);
      if (error) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_IP_STATS_ENABLED(RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
         return error;
      } /* Endif */

      RTCSLOG_PCB_WRITE(rtcs_pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IP), IP_VERSION);

      error = RTCSPCB_insert_header(rtcs_pcb, pktlen);
      if (error) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_IP_STATS_ENABLED(RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
         return error;
      } /* Endif */

      _mem_copy((uchar_ptr)packet, RTCSPCB_DATA(rtcs_pcb), pktlen);
      packet = (IP_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
      RTCSPCB_DATA_NETWORK(rtcs_pcb) = RTCSPCB_DATA(rtcs_pcb);

      /*
      ** Decrement the TTL and adjust the checksum
      */

      ttl--;
      htonc(packet->TTL, ttl);
      chksum = ntohs(packet->CHECKSUM);
      chksum = IP_Sum_immediate(chksum, 0x100);
      htons(packet->CHECKSUM, chksum);

      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_FORWARDED++);

   } /* Endif */

   rtcs_pcb->LINK_OPTIONS.RX.DEST  = ipdest;
   rtcs_pcb->LINK_OPTIONS.RX.TOS   = ntohc(packet->TOS);
   rtcs_pcb->LINK_OPTIONS.RX.TTL   = ntohc(packet->TTL);

   return RTCS_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_send_dgram
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP datagram out an interface, fragmenting it if
*        necessary.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_send_dgram
   (
      IP_IF_PTR      ifdest,     /* [IN] the outgoing interface */
      RTCSPCB_PTR    inrtcs_pcb, /* [IN] the packet to send */
      _ip_address    hopsrc,     /* [IN] the hop src */
      _ip_address    hopdest,    /* [IN] the hop dest */
      _ip_address    ipdest,     /* [IN] the ultimate dest */
      uint_32        protocol,   /* [IN] the transport layer protocol */
      pointer        data        /* [IN] routing entry data */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   IP_CFG_STRUCT_PTR IP_cfg_ptr = RTCS_getcfg(IP);
   IP_HEADER_PTR     srciph;
   uint_32           iphlen;
   uint_32           srcdatalen;
   uint_32           dmtumax, dmtu;
   uint_32           srclen, offset, error;
   uchar_ptr         srcdata;
   PCB_FRAGMENT_PTR  frag_ptr;
   
   RTCSLOG_FNE2(RTCSLOG_FN_IP_send_dgram, ifdest);

   error = inrtcs_pcb->IP_COMPLETE(ifdest, &inrtcs_pcb, hopsrc, ipdest, protocol);
   if (error) {
      RTCSLOG_PCB_FREE(inrtcs_pcb, error);
      RTCSPCB_free(inrtcs_pcb);
      
      RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, error);
      return error;
   } /* Endif */

   /* If the packet doesn't exceed the MTU, send it directly */
   if (RTCSPCB_SIZE(inrtcs_pcb) <= ifdest->MTU) {
      error = ifdest->DEVICE.SEND(ifdest, inrtcs_pcb, hopsrc, hopdest, data);
      
      RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, error);
      return error;
   } /* Endif */

   srciph = (IP_HEADER_PTR)RTCSPCB_DATA(inrtcs_pcb);
   iphlen = (ntohc(srciph->VERSLEN) & 0x0F) << 2;
   srcdatalen = RTCSPCB_SIZE(inrtcs_pcb) - iphlen;

   /* If the datagram exceeds the MTU, but DF is set, respond with ICMP */
   if (ntohs(srciph->FRAGMENT) & IP_FRAG_DF) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_TX_FRAG_DISCARDED++);
      #if RTCSCFG_ENABLE_ICMP
      ICMP_send_error(ICMPTYPE_DESTUNREACH, ICMPCODE_DU_NEEDFRAG, ifdest->MTU, inrtcs_pcb, 0);
      #endif
      RTCSLOG_PCB_FREE(inrtcs_pcb, RTCSERR_IP_CANTFRAG);
      RTCSPCB_free(inrtcs_pcb);
      
      RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, RTCSERR_IP_CANTFRAG);
      return RTCSERR_IP_CANTFRAG;
   } /* Endif */

   /* Sanity check.  Note: it's an error to bind an interface with an MTU < 68 */
   if (iphlen + IP_FRAG_MIN > ifdest->MTU) {
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_TX_FRAG_DISCARDED++);
      RTCSLOG_PCB_FREE(inrtcs_pcb, RTCSERR_IP_SMALLMTU);
      RTCSPCB_free(inrtcs_pcb);
      
      RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, RTCSERR_IP_SMALLMTU);
      return RTCSERR_IP_SMALLMTU;
   } /* Endif */

   IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_TX_FRAG_FRAGD++);
   dmtumax = ifdest->MTU - iphlen;
   dmtu = dmtumax & (((uint_32)(-1))<<IP_FRAG_SHIFT);
   offset = (ntohs(srciph->FRAGMENT) & IP_FRAG_MASK) << IP_FRAG_SHIFT;
   srcdata = RTCSPCB_DATA(inrtcs_pcb) + iphlen;
   srclen = inrtcs_pcb->HEADER_FRAG_USED - iphlen;
   frag_ptr = inrtcs_pcb->PCBPTR->FRAG;

   do {
      IP_HEADER_PTR  dstiph;
      uchar_ptr      dstdata;
      uint_32        dstlen = (srcdatalen > dmtumax) ? dmtu : srcdatalen;
      RTCSPCB_PTR    rtcs_pcb;
      uint_16        chksum, fragment;

      /* Allocate the PCB */
      rtcs_pcb = RTCSPCB_alloc_send();
      if (rtcs_pcb == NULL) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         RTCSLOG_PCB_FREE(inrtcs_pcb, RTCSERR_PCB_ALLOC);
         RTCSPCB_free(inrtcs_pcb);
         
         RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, RTCSERR_PCB_ALLOC);
         return RTCSERR_PCB_ALLOC;
      } /* Endif */

      /* Allocate the memory for the IP data */
      dstdata = RTCS_mem_alloc_system(dstlen);
      if (dstdata == NULL) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_OUT_OF_MEMORY);
         RTCSPCB_free(rtcs_pcb);
         RTCSLOG_PCB_FREE(inrtcs_pcb, RTCSERR_OUT_OF_MEMORY);
         RTCSPCB_free(inrtcs_pcb);
         
         RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, RTCSERR_OUT_OF_MEMORY);
         return RTCSERR_OUT_OF_MEMORY;
      } /* Endif */
      _mem_set_type(dstdata, MEM_TYPE_IP_DATA);

      /* Append the data to the PCB */
      error = RTCSPCB_append_fragment_autofree(rtcs_pcb, dstlen, dstdata);
      if (error) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_IP_STATS_ENABLED(RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
         _mem_free(dstdata);
         RTCSLOG_PCB_FREE(rtcs_pcb, error);
         RTCSPCB_free(rtcs_pcb);
         RTCSLOG_PCB_FREE(inrtcs_pcb, error);
         RTCSPCB_free(inrtcs_pcb);
         
         RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, error);
         return error;
      } /* Endif */

      /* Allocate the IP header */
      error = RTCSPCB_insert_header(rtcs_pcb, iphlen);
      if (error) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_IP_STATS_ENABLED(RTCS_seterror(&IP_cfg_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
         /* don't _mem_free(dstdata) here because RTCSPCB_free(pcb) will do it */
         RTCSLOG_PCB_FREE(rtcs_pcb, error);
         RTCSPCB_free(rtcs_pcb);
         RTCSLOG_PCB_FREE(inrtcs_pcb, error);
         RTCSPCB_free(inrtcs_pcb);
         
         RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, error);
         return error;
      } /* Endif */

      /* Copy the IP header */
      dstiph = (IP_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
      RTCSPCB_DATA_NETWORK(rtcs_pcb) = RTCSPCB_DATA(rtcs_pcb);
      _mem_copy(srciph, dstiph, iphlen);

      /* Update the LENGTH field */
      htons(dstiph->LENGTH, iphlen + dstlen);

      /* Build the FRAGMENT field */
      fragment = offset >> IP_FRAG_SHIFT;
      offset     += dstlen;
      srcdatalen -= dstlen;
      /* An MF bit must be added on all fragments except the last */
      if (srcdatalen > 0 || (ntohs(srciph->FRAGMENT) & IP_FRAG_MF)) {
         fragment |= IP_FRAG_MF;
      } /* Endif */
      htons(dstiph->FRAGMENT, fragment);

      /* Recompute the IP header checksum */
      htons(dstiph->CHECKSUM, 0);
      chksum = _mem_sum_ip(0, iphlen, dstiph);
      chksum = IP_Sum_invert(chksum);
      htons(dstiph->CHECKSUM, chksum);

      /* Copy the IP data */
      for (;;) {
         uint_32 copylen = (srclen > dstlen) ? dstlen : srclen;
         _mem_copy(srcdata, dstdata, copylen);
         srcdata += copylen;
         dstdata += copylen;
         srclen  -= copylen;
         dstlen  -= copylen;
         if (dstlen == 0) break;
         frag_ptr++;
         srcdata = frag_ptr->FRAGMENT;
         srclen = frag_ptr->LENGTH;
      } /* Endfor */

      /* Send the fragment through the interface */
      IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_TX_FRAG_SENT++);
      error = ifdest->DEVICE.SEND(ifdest, rtcs_pcb, hopsrc, hopdest, data);
      if (error) {
         RTCSLOG_PCB_FREE(inrtcs_pcb, error);
         RTCSPCB_free(inrtcs_pcb);
         
         RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, error);
         return error;
      } /* Endif */

   } while (srcdatalen > 0);

   RTCSLOG_PCB_FREE(inrtcs_pcb, RTCS_OK);
   RTCSPCB_free(inrtcs_pcb);
   
   RTCSLOG_FNX2(RTCSLOG_FN_IP_send_dgram, RTCS_OK);
   return RTCS_OK;

#else
    
    RTCSLOG_PCB_FREE(inrtcs_pcb, RTCSERR_IP_IS_DISABLED);
    RTCSPCB_free(inrtcs_pcb);
    return RTCSERR_IP_IS_DISABLED; 

#endif /* RTCSCFG_ENABLE_IP4 */
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_source
* Returned Value  : source IP address
* Comments        :
*     Return source IP address of packet.
*
*END*-----------------------------------------------------------------*/


_ip_address IP_source
   (
      RTCSPCB_PTR    rtcs_pcb      /* [IN] packet to find source of */
   )
{
#if RTCSCFG_ENABLE_IP4

   uchar_ptr srcptr = ((IP_HEADER_PTR)RTCSPCB_DATA_NETWORK(rtcs_pcb))->SOURCE;

   return ntohl(srcptr);
   
#else
    
    return 0;

#endif /* RTCSCFG_ENABLE_IP4 */   

} 


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_dest
* Returned Value  : destination IP address
* Comments        :
*     Return destination IP address of packet.
*
*END*-----------------------------------------------------------------*/

_ip_address IP_dest
   (
      RTCSPCB_PTR    rtcs_pcb      /* [IN] packet to find dest of */
   )
{ 
#if RTCSCFG_ENABLE_IP4

   uchar_ptr dstptr = ((IP_HEADER_PTR)RTCSPCB_DATA_NETWORK(rtcs_pcb))->DEST;

   return ntohl(dstptr);
   
#else
    
    return 0;

#endif /* RTCSCFG_ENABLE_IP4 */
}




/* EOF */
