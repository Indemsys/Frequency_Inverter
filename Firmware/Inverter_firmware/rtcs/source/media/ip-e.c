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
* $FileName: ip-e.c$
* $Version : 3.8.19.0$
* $Date    : Sep-19-2012$
*
* Comments:
*
*   This file contains the interface between IP and
*   the Ethernet packet driver.
*
*END************************************************************************/


#include <string.h>
#include <rtcs.h>
#include <ethernet.h>
#include <enet.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"
#include "ip-e.h"

#if BSP_ENET_DEVICE_COUNT > 0

#define IPE_ADDR_BC(addr)     \
            (addr)[0] = 0xFF, \
            (addr)[1] = 0xFF, \
            (addr)[2] = 0xFF, \
            (addr)[3] = 0xFF, \
            (addr)[4] = 0xFF, \
            (addr)[5] = 0xFF

#define IPE_ADDR_MC(addr,ip)  \
            (addr)[0] = 0x01, \
            (addr)[1] = 0x00, \
            (addr)[2] = 0x5E, \
            (addr)[3] = ((ip) >> 16) & 0x7F, \
            (addr)[4] = ((ip) >>  8) & 0xFF, \
            (addr)[5] =  (ip)        & 0xFF

/*
** Ethernet packet header
*/

/*
** The structure used by the application to bind an IP address
** to an Ethernet packet driver.
*/

static const RTCS_IF_STRUCT rtcs_enet =
{
    IPE_open,
    IPE_close
#if RTCSCFG_ENABLE_IP4     
    ,
    IPE_send_IP
#if RTCSCFG_ENABLE_IGMP && BSPCFG_ENABLE_ENET_MULTICAST
    ,
    IPE_join,
    IPE_leave
#endif
#endif /* RTCSCFG_ENABLE_IP4 */

#if RTCSCFG_ENABLE_IP6
    ,
    IP6E_send,
    IP6E_join,
    IP6E_leave
#endif

};

const RTCS_IF_STRUCT_PTR RTCS_IF_ENET = (RTCS_IF_STRUCT_PTR)&rtcs_enet;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Registers IP and ARP with an Ethernet packet driver.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_open
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */
   uint_32  error;


#if RTCSCFG_ENABLE_IP4
   error = ARP_open(if_ptr);
   if (error) {
      return error;
   } /* Endif */

   error = ENET_open(if_ptr->HANDLE, ENETPROT_IP, IPE_recv_IP, if_ptr);
   if (error) {
      ARP_close(if_ptr);
      return error;
   } /* Endif */

   error = ENET_open(if_ptr->HANDLE, ENETPROT_ARP, IPE_recv_ARP, if_ptr);
   if (error) {
      ENET_close(if_ptr->HANDLE, ENETPROT_IP);
      ARP_close(if_ptr);
      return error;
   } /* Endif */
#endif /* RTCSCFG_ENABLE_IP4 */
 
   
   if_ptr->MTU = ENET_get_MTU(if_ptr->HANDLE);

   if_ptr->DEV_TYPE    = 1;
   if_ptr->DEV_ADDRLEN = 6;
   ENET_get_address(if_ptr->HANDLE, if_ptr->DEV_ADDR);
   
#if RTCSCFG_ENABLE_SNMP && RTCSCFG_ENABLE_IP4
   switch (ENET_get_speed(if_ptr->HANDLE)) {
   case 100:
      if_ptr->SNMP_IF_TYPE = IPIFTYPE_FASTETHERNET;
      break;
   case 1000:
      if_ptr->SNMP_IF_TYPE = IPIFTYPE_GIGABITETHERNET;
      break;
   default:
      if_ptr->SNMP_IF_TYPE = IPIFTYPE_ETHERNET;
      break;
   } /* Endswitch */
#endif

   /* Add some uniqueness to the seed */
   RTCS_rand_seed(if_ptr->DEV_ADDR[0]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[1]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[2]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[3]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[4]);
   RTCS_rand_seed(if_ptr->DEV_ADDR[5]);

#if RTCSCFG_ENABLE_IP6
    error = IP6E_open(if_ptr);
    if (error) 
    {
        ENET_close(if_ptr->HANDLE, ENETPROT_ARP);
        ENET_close(if_ptr->HANDLE, ENETPROT_IP);
    #if RTCSCFG_ENABLE_IP4
        ARP_close(if_ptr);
    #endif /* RTCSCFG_ENABLE_IP4 */
        return error;
    }     
#endif  


   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_close
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Registers IP and ARP with an Ethernet packet driver.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_close
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */
   uint_32  firsterror, error;

   firsterror = RTCS_OK;

#if RTCSCFG_ENABLE_IP4
   error = ARP_close(if_ptr);
   if (error && !firsterror) {
      firsterror = error;
   } /* Endif */

   error = ENET_close(if_ptr->HANDLE, ENETPROT_IP);
   if (error && !firsterror) {
      firsterror = error;
   } /* Endif */

   error = ENET_close(if_ptr->HANDLE, ENETPROT_ARP);
   if (error && !firsterror) {
      firsterror = error;
   } /* Endif */
#endif /* RTCSCFG_ENABLE_IP4 */

#if RTCSCFG_ENABLE_IP6
   error = IP6E_close (if_ptr);
   if (error && !firsterror)
   { //TBD  error handling.
      firsterror = error;
   } /* Endif */
#endif 

   return firsterror;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an Ethernet packet.
*
*END*-----------------------------------------------------------------*/

/* //AB static*/ void IPE_recv
   (
      PCB_PTR           pcb,
         /* [IN] the received packet */
      pointer           handle,
         /* [IN] the IP interface structure */
      void (_CODE_PTR_  service)(RTCSPCB_PTR)
         /* [IN] IP_service or ARP_service */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = handle;
   RTCSPCB_PTR          rtcs_pcb;
   ENET_HEADER_PTR      enetpkt;
#if RTCSCFG_LINKOPT_8021Q_PRIO || RTCSCFG_LINKOPT_8023
   uchar_ptr            type_ptr;
#endif
   uint_32              hdrlen, error;
   uchar                rladdr[6];
   
   RTCSLOG_FNE4(RTCSLOG_FN_IPE_recv, pcb, handle, service);
   
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_TOTAL++);
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_RX_OCTETS += pcb->FRAG[0].LENGTH);

   rtcs_pcb = RTCSPCB_alloc_recv(pcb);
   if (rtcs_pcb == NULL) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_MISSED++);
      PCB_free(pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IPE_recv, -1);
      return;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(rtcs_pcb);

   enetpkt = (ENET_HEADER_PTR)RTCSPCB_DATA(rtcs_pcb);
   if (enetpkt->DEST[0] & 1) {
      if ((enetpkt->DEST[0] == 0xFF)
       && (enetpkt->DEST[1] == 0xFF)
       && (enetpkt->DEST[2] == 0xFF)
       && (enetpkt->DEST[3] == 0xFF)
       && (enetpkt->DEST[4] == 0xFF)
       && (enetpkt->DEST[5] == 0xFF)) {
         IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_RX_BROADCAST++);
         rtcs_pcb->TYPE = RTCSPCB_TYPE_BROADCAST;
      } else {
         IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_RX_MULTICAST++);
         rtcs_pcb->TYPE = RTCSPCB_TYPE_MULTICAST;
      } /* Endif */
   } else {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_RX_UNICAST++);
      rtcs_pcb->TYPE = RTCSPCB_TYPE_UNICAST;
   } /* Endif */

#if RTCSCFG_LINKOPT_8021Q_PRIO || RTCSCFG_LINKOPT_8023
   type_ptr = enetpkt->TYPE;
#endif
   hdrlen = sizeof(ENET_HEADER);

#if RTCSCFG_LINKOPT_8021Q_PRIO
   if (ntohs(type_ptr) == 0x8100) {
      rtcs_pcb->LINK_OPTIONS.RX.OPT_PRIO = 1;
      rtcs_pcb->LINK_OPTIONS.RX.PRIO = (ntohc(type_ptr+2) >> 5) & 0x7;
      hdrlen += sizeof(ENET_8021QTAG_HEADER);
      type_ptr += sizeof(ENET_8021QTAG_HEADER);
   } /* Endif */
#endif

#if RTCSCFG_LINKOPT_8023
   if (ntohs(type_ptr) < ENET_FRAMESIZE_MAXDATA) {
      rtcs_pcb->LINK_OPTIONS.RX.OPT_8023 = 1;
      hdrlen += sizeof(ENET_8022_HEADER);
      type_ptr += sizeof(ENET_8022_HEADER);
   } /* Endif */
#endif

   error = RTCSPCB_next(rtcs_pcb, hdrlen);
   if (error) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_ERRORS++);
      IF_IPIF_STATS_ENABLED(RTCS_seterror(&if_ptr->STATS.ERR_RX, error, (uint_32)rtcs_pcb));
      RTCSLOG_PCB_FREE(rtcs_pcb, error);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IPE_recv, -2);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(RTCSPCB_DATA(rtcs_pcb), RTCS_LOGCTRL_IFTYPE(IPIFTYPE_ETHERNET), 0);

   rtcs_pcb->LINK_OPTIONS.TX = rtcs_pcb->LINK_OPTIONS.RX;
   rtcs_pcb->IFSRC = handle;

#if RTCSCFG_DISCARD_SELF_BCASTS
   /* discard any broadcast pkts that we sent, as they are likely echoes */ 
   ntohe(enetpkt->SOURCE, rladdr);
   if ((rtcs_pcb->TYPE == RTCSPCB_TYPE_BROADCAST) && 
       (memcmp(rladdr, if_ptr->DEV_ADDR, sizeof(rladdr)) == 0)) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
      RTCSPCB_free(rtcs_pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IPE_recv, -3);
      return;
   }
#endif

   if (!RTCSCMD_service(rtcs_pcb, service)) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_MISSED++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
      RTCSPCB_free(rtcs_pcb);
   }
   
   RTCSLOG_FNX4(RTCSLOG_FN_IPE_recv, 0, ntohl(&(enetpkt->SOURCE[2])), /*ntohs(enetpkt->TYPE), */ntohl(((uint_8*)enetpkt + 0x26)));
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv_IP
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an IP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

void IPE_recv_IP
   (
      PCB_PTR  pcb,
         /* [IN] the received packet */
      pointer  handle
         /* [IN] the IP interface structure */
   )
{ 
   RTCSLOG_FNE3(RTCSLOG_FN_IPE_recv_IP, pcb, handle);
   IPE_recv(pcb, handle, IP_service);
   RTCSLOG_FNX1(RTCSLOG_FN_IPE_recv_IP);
}

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_recv_ARP
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Services an ARP packet.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4

void IPE_recv_ARP
   (
      PCB_PTR  pcb,
         /* [IN] the received packet */
      pointer  handle
         /* [IN] the IP interface structure */
   )
{ 
   IPE_recv(pcb, handle, ARP_service);
}

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a packet.
*
*END*-----------------------------------------------------------------*/
uint_32 IPE_send
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] the packet to send */
      uint_16        protocol,
         /* [IN] ENETPROT_IP or ENETPROT_ARP */
      uchar          dest[6]
         /* [IN] the destination Ethernet address */
   )
{ /* Body */
   uint_32        hdrlen, flags, error;
   
   RTCSLOG_FNE5(RTCSLOG_FN_IPE_send, if_ptr, rtcs_pcb, protocol, *(uint_32*)dest);

   hdrlen = sizeof(ENET_HEADER);
   flags = 0;

#if RTCSCFG_LINKOPT_8021Q_PRIO
   if (rtcs_pcb->LINK_OPTIONS.TX.OPT_PRIO) {
      hdrlen += sizeof(ENET_8021QTAG_HEADER);
      flags |= ENET_SETOPT_8021QPRIO(rtcs_pcb->LINK_OPTIONS.TX.PRIO);
   } /* Endif */
#endif

#if RTCSCFG_LINKOPT_8023
   if (rtcs_pcb->LINK_OPTIONS.TX.OPT_8023) {
      hdrlen += sizeof(ENET_8022_HEADER);
      flags |= ENET_OPT_8023;
   } /* Endif */
#endif

   RTCSLOG_PCB_WRITE(rtcs_pcb, RTCS_LOGCTRL_IFTYPE(IPIFTYPE_ETHERNET), 0);

   error = RTCSPCB_insert_header(rtcs_pcb, hdrlen);
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_OCTETS += RTCSPCB_SIZE(rtcs_pcb));
   if (error) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_ERRORS++);
      IF_IPIF_STATS_ENABLED(RTCS_seterror(&if_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
      RTCSLOG_PCB_FREE(rtcs_pcb, error);
      RTCSPCB_free(rtcs_pcb);
      
      RTCSLOG_FNX2(RTCSLOG_FN_IPE_send, error);
      return error;
   } /* Endif */

   rtcs_pcb->PCBPTR->FRAG[0].LENGTH   = rtcs_pcb->HEADER_FRAG_USED;
   rtcs_pcb->PCBPTR->FRAG[0].FRAGMENT = RTCSPCB_DATA(rtcs_pcb);

   error = ENET_send(if_ptr->HANDLE, rtcs_pcb->PCBPTR, protocol, dest, flags);
   RTCSLOG_PCB_FREE(rtcs_pcb, error);
   if (error) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_ERRORS++);
      IF_IPIF_STATS_ENABLED(RTCS_seterror(&if_ptr->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
   } /* Endif */
   
   RTCSLOG_FNX2(RTCSLOG_FN_IPE_send, error);
   return error;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_IP
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends an IP packet.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
uint_32 IPE_send_IP
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] the packet to send */
      _ip_address    src,
         /* [IN] the next-hop source address */
      _ip_address    dest,
         /* [IN] the next-hop destination address */
      pointer        data
         /* [IN] unused */
   )
{ /* Body */
   uchar    enetdest[6];
   uint_32 error;
   
   RTCSLOG_FNE5(RTCSLOG_FN_IPE_send_IP, if_ptr, rtcs_pcb, src, dest);

   IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_TOTAL++);

   if (dest == INADDR_BROADCAST) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_BROADCAST++);
      IPE_ADDR_BC(enetdest);

   } else if (IN_MULTICAST(dest)) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_MULTICAST++);
      IPE_ADDR_MC(enetdest, dest);

   } else {
      /* ARP_resolve() will record an error if one occurs */
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_UNICAST++);
      error = ARP_resolve(if_ptr, rtcs_pcb, src, dest);
      RTCSLOG_FNX2(RTCSLOG_FN_IPE_send_IP, error);
      return error;

   } /* Endif */

   error = IPE_send(if_ptr, rtcs_pcb, ENETPROT_IP, enetdest);
   RTCSLOG_FNX2(RTCSLOG_FN_IPE_send_IP, error);
   return error;
} /* Endbody */
#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_IP_ucast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Called from ARP.  Sends a resolved IP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 IPE_send_IP_ucast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] the packet to send */
      uchar          dest[6]
         /* [IN] the destination Ethernet address */
   )
{ /* Body */

   return IPE_send(if_ptr, rtcs_pcb, ENETPROT_IP, dest);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_ARP_bcast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a broadcast ARP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 IPE_send_ARP_bcast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    rtcs_pcb
         /* [IN] the packet to send */
   )
{ /* Body */
   uchar    enetdest[6];

   IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_TOTAL++);
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_BROADCAST++);
   IPE_ADDR_BC(enetdest);
   return IPE_send(if_ptr, rtcs_pcb, ENETPROT_ARP, enetdest);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_send_ARP_ucast
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Sends a unicast ARP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 IPE_send_ARP_ucast
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] the packet to send */
      uchar          dest[6]
         /* [IN] the next-hop destination address */
   )
{ /* Body */

   IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_TOTAL++);
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_UNICAST++);
   return IPE_send(if_ptr, rtcs_pcb, ENETPROT_ARP, dest);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

#if BSPCFG_ENABLE_ENET_MULTICAST && RTCSCFG_ENABLE_IP4
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_join
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Joins a multicast group.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_join
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      _ip_address    group
         /* [IN] the multicast group */
   )
{ /* Body */
   uchar    enetgroup[6];

   IPE_ADDR_MC(enetgroup, group);
   return ENET_join(if_ptr->HANDLE, ENETPROT_IP, enetgroup);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPE_leave
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Leaves a multicast group.
*
*END*-----------------------------------------------------------------*/

uint_32 IPE_leave
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      _ip_address    group
         /* [IN] the multicast group */
   )
{ /* Body */
   uchar    enetgroup[6];

   IPE_ADDR_MC(enetgroup, group);
   return ENET_leave(if_ptr->HANDLE, ENETPROT_IP, enetgroup);

} /* Endbody */
#endif /* BSPCFG_ENABLE_ENET_MULTICAST && RTCSCFG_ENABLE_IP4 */

#endif /* BSP_ENET_DEVICE_COUNT > 0 */
/* EOF */
