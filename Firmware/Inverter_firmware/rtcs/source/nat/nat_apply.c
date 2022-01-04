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
* $FileName: nat_apply.c$
* $Version : 3.8.5.0$
* $Date    : Oct-9-2012$
*
* Comments:
*
*   This file contains the NAT implementation.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include <rtcs_prv.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_apply
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Modifies a packet according to NAT if applicable. If an error
*     code is returned, the packet should be dropped. 
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_apply
   (
      RTCSPCB_PTR _PTR_    pcb_ptr  /* [IN/OUT] PCB containing the packet */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   NAT_ALG_CFG_STRUCT_PTR  nat_alg_cfg_ptr;
   NAT_SESSION_STRUCT_PTR  nat_session_ptr = NULL;
   IP_HEADER_PTR           ip_header_ptr = (IP_HEADER_PTR)(pointer)RTCSPCB_DATA(*pcb_ptr);
   _ip_address             ipsrc, ipdest;
   uint_32                 error, ip_protocol;
   uint_16                 packet_len, icmp_type, tcp_flags;
   TRANSPORT_UNION         transport;
   boolean                 dest_is_local, icmperr = FALSE, reset = FALSE;
   boolean                 source_is_private, dest_is_private;
   uint_16                 destination_port;

   nat_cfg_ptr->STATS.ST_PACKETS_TOTAL++;

   // Start CR TBD
   ipsrc = ntohl(ip_header_ptr->SOURCE);
   ipdest = ntohl(ip_header_ptr->DEST);
   source_is_private = NAT_is_private_addr(&nat_cfg_ptr->PRIVATE_NETWORKS, ipsrc);
   dest_is_private = NAT_is_private_addr(&nat_cfg_ptr->PRIVATE_NETWORKS, ipdest);
   dest_is_local = IP_is_local(NULL, ipdest);      
   if (!source_is_private && !dest_is_local) {
      /*
      ** Case #1:public(external) or public(local) to public(external)
      */
      nat_cfg_ptr->STATS.ST_PACKETS_BYPASS++;
      return RTCS_OK;
   }
   // End CR TBD

   if (ntohs(ip_header_ptr->FRAGMENT) & (IP_FRAG_MF | IP_FRAG_MASK)) {
#if RTCSCFG_ENABLE_IP_REASSEMBLY
      #if RTCS_VERSION <= 0x00029600L
      #ifndef __MSVC__
      #warning "This version of NAT requires a modified version of RTCS, see Release notes"
      #endif
      #endif
      nat_cfg_ptr->STATS.ST_PACKETS_FRAGMENTS++;

      error = IP_reasm(*pcb_ptr,pcb_ptr); 
      if (error != RTCS_OK) {
         nat_cfg_ptr->STATS.ST_PACKETS_REASSSEMBLY_ERR++;
         return error;
      }
      if (*pcb_ptr == NULL) {
         // Reassembly queued the PCB - there is nothing to do yet.
         return RTCS_OK;
      }
      // We now have a reassembled packet - however the PCB is new, so 
      // update packet address
      ip_header_ptr = (IP_HEADER_PTR)(pointer)RTCSPCB_DATA(*pcb_ptr);
      nat_cfg_ptr->STATS.ST_PACKETS_REASSEMBLED++;
#else
    nat_cfg_ptr->STATS.ST_PACKETS_REASSSEMBLY_ERR++;
    return RTCSERR_FEATURE_NOT_ENABLED;
#endif /* RTCSCFG_ENABLE_IP_REASSEMBLY */
   } /* Endif */

   
   /* Get the frequently accessed fields from the IP Header */
   packet_len = ntohs(ip_header_ptr->LENGTH);
   ipsrc = ntohl(ip_header_ptr->SOURCE);
   ipdest = ntohl(ip_header_ptr->DEST);
   ip_protocol = ntohc(ip_header_ptr->PROTOCOL);
   transport.PTR = TRANSPORT_PTR(ip_header_ptr);
   
   /* determine if source and/or destination is for a private network */
   source_is_private = NAT_is_private_addr(&nat_cfg_ptr->PRIVATE_NETWORKS, ipsrc);
   dest_is_private = NAT_is_private_addr(&nat_cfg_ptr->PRIVATE_NETWORKS, ipdest);
   dest_is_local = IP_is_local(NULL, ipdest);      


   /* 
   ** There are four possible cases:
   **    1. Packet is staying in the same network (public->public or private->private) no translation
   **    2. Move packet from private network to public network (will involve translation)
   **    3. Packet is for an unreachable destination (explicit public->private). 
   **    4. Move packet from public network to private network after translation
   **
   */
   if (source_is_private) {
      /*
      ** Source is private (either local or external)
      ** It can send to other hosts on the private network, which includes the 
      ** private network interface of the NAT router.
      */
      if (dest_is_private /* || dest_is_local */ ) {  
         /*
         ** Case #1: private(external) or private(local) to private(external), private(local) or public(local)
         ** No processing required, Not going onto the public network
         */
         nat_cfg_ptr->STATS.ST_PACKETS_BYPASS++;
         return RTCS_OK;                      
      } 

      /*
      ** Case #2: private(external) or private(local) to public(external)
      */

   } else {

      /* Source is public (either local or external) */
      if (dest_is_private) {     
         /*
         ** Case #3: public(external) or public(local) to private(external) or private(local)
         ** The source is a host on the public network, the destination is private.
         **  This packet should not be forwarded into the private network because the public host shouldn't 
         ** know about such an address. 
         */
         nat_cfg_ptr->STATS.ST_PACKETS_PUB_PRV_ERR++;      
         return RTCSERR_IP_UNREACH;                
      
      } else if (!dest_is_local)  {   
         /*
         ** Case #1:public(external) or public(local) to public(external)
         */
         nat_cfg_ptr->STATS.ST_PACKETS_BYPASS++;
         return RTCS_OK;
      } /* Endif */

      /* 
      ** Case #4: public(external) or public(local) to public(local)               
      ** The source is a host on the public network. The destination is not private.
      ** This packet might be for the private network or the NAT router. 
      */
      
   } /* Endif */

   /* NAT only supports ICMP, UDP and TCP transport layer protocols */
   switch (ip_protocol) {
      case IPPROTO_TCP:
         if (packet_len < sizeof(TCP_HEADER))  {
            NAT_UPDATE_ERR_STATS(nat_cfg_ptr,source_is_private);
            return RTCSERR_TCP_BAD_HEADER;
         } /* Endif */
         destination_port = ntohs(transport.TCP_PTR->dest_port);
         break;

      case IPPROTO_UDP:
         if (packet_len < sizeof(UDP_HEADER))  {
            NAT_UPDATE_ERR_STATS(nat_cfg_ptr,source_is_private);
            return RTCSERR_UDP_BAD_HEADER;
         } /* Endif */
         destination_port = ntohs(transport.UDP_PTR->DEST_PORT);
         break;

      case IPPROTO_ICMP:
         if (packet_len < sizeof(ICMP_ECHO_HEADER)) {
            NAT_UPDATE_ERR_STATS(nat_cfg_ptr,source_is_private);
            return RTCSERR_ICMP_BAD_HEADER;
         } /* Endif */
          
         /* Get the icmp message type */
         icmp_type = ntohc(transport.ICMP_PTR->TYPE);
         if (icmp_type==ICMPTYPE_REDIRECT) {
            /* Never modify REDIRECTs */
            nat_cfg_ptr->STATS.ST_PACKETS_BYPASS++;
            return RTCS_OK;
         } 
                  
         icmperr = !ICMPTYPE_ISQUERY(icmp_type);
         break;        
          
      default:
         if (source_is_private) {
            nat_cfg_ptr->STATS.ST_PACKETS_PRV_PUB_ERR++;
            return RTCSERR_NAT_UNSUPPORTED_PROTOCOL;
         } else {
            nat_cfg_ptr->STATS.ST_PACKETS_BYPASS++;
            return RTCS_OK;
         } /* Endif */
   } /* Endswitch */


   /* Apply NAT ALGs and then do lookup */
   if (!icmperr) {
      nat_session_ptr = NAT_lookup(ip_header_ptr, FALSE, FALSE, &error);
      for (nat_alg_cfg_ptr = nat_cfg_ptr->ALG_INFO_PTR; nat_alg_cfg_ptr; nat_alg_cfg_ptr = nat_alg_cfg_ptr->NEXT) {
         error = nat_alg_cfg_ptr->ALG_EXEC(
            pcb_ptr, 
            !source_is_private, 
            nat_alg_cfg_ptr,
            (pointer _PTR_)&nat_session_ptr);

         /* If the ALG returns an error, we discard the packet */
         if (error) {
            NAT_UPDATE_ERR_STATS(nat_cfg_ptr,source_is_private);
            return error;
         } /* Endif */
      } /* Endfor */

      /* 
      ** Update pointers in case the PCB was modifed. 
      */
      ip_header_ptr = (IP_HEADER_PTR)(pointer)RTCSPCB_DATA(*pcb_ptr);
      transport.PTR = TRANSPORT_PTR(ip_header_ptr);
   } /* Endif */
   

   if ((nat_session_ptr == NULL) && (!icmperr)) {

      /* If a public host sent the message, don't create a new session - 
      ** Send the packet on to the  local public interface */
      if (!source_is_private) { 
         nat_cfg_ptr->STATS.ST_PACKETS_BYPASS++;      
         return RTCS_OK;
      } /* Endif */
       
      /*
      ** Normally we would only create a new TCP session if the first packet 
      ** of the session is a SYN packet. We will create a session and forward 
      ** the packet anyway, and the receiver will send a RST back, which we 
      ** will forward to the originator. 
      */
      tcp_flags = ntohs(transport.TCP_PTR->flags);
      reset = ((ip_protocol == IPPROTO_TCP) && (!(tcp_flags & SYN) || (tcp_flags & ACK)));

      /* Create a new session if necessary and save information */
      nat_session_ptr = NAT_insert(ip_header_ptr, &error);
      if (nat_session_ptr == NULL) {
         nat_cfg_ptr->STATS.ST_PACKETS_PRV_PUB_ERR++;        
         return error;
      } /* Endif */

   } /* Endif */
   
   /* Modify the transport level header */   
   switch (ip_protocol) {
      case IPPROTO_TCP:
      case IPPROTO_UDP:
         NAT_mod_tcpudp(transport, nat_session_ptr, !source_is_private, reset);
         break;
         
      case IPPROTO_ICMP:
         dest_is_local = FALSE;
         error = NAT_mod_icmp(ip_header_ptr, &nat_session_ptr, !source_is_private, &dest_is_local);
         
         if (dest_is_local) {  /* ICMP Error meant for NAT router */
            nat_cfg_ptr->STATS.ST_PACKETS_BYPASS++;
            return RTCS_OK;
         } /* Endif */
         
         if (error) {   /* Can only happen with ICMP error packets */
            nat_cfg_ptr->STATS.ST_PACKETS_PRV_PUB_ERR++;
            return error;
         } /* Endif */
         break;
         
      default:    /* Can never get here */
         break;
   } /* Endswitch */
   
   /* Modify the IP header */
   NAT_mod_ip(ip_header_ptr, nat_session_ptr, !source_is_private);
      
   if (source_is_private) {
      nat_cfg_ptr->STATS.ST_PACKETS_PRV_PUB++;
   } else {
      nat_cfg_ptr->STATS.ST_PACKETS_PUB_PRV++;
   }

   return RTCS_OK;   /* Continue routing as normal */
   
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_chksum_mod
* Returned Value  : New chksum (uint_16)
* Comments        :
*        Takes current chksum and calculates a new one based the data being
*        replaced and on the new data.
*
*END*-----------------------------------------------------------------*/

uint_16 NAT_chksum_mod
   (
      uint_16  oldchk,     /* [IN] The old checksum */
      uint_16  old16,      /* [IN] The old data being replaced */
      uint_16  new16       /* [IN] The new replacement data */
   )
{ /* Body */
  
   uint_32 newchk;

   newchk = oldchk + (0xFFFF - new16) + old16;
   newchk = (newchk >> 16) + (newchk & 0xFFFF);
   newchk = (newchk >> 16) + (newchk & 0xFFFF);
   
   return (uint_16)newchk;
   
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_mod_ip
* Returned Value  : void
* Comments        :
*     Modifies the IP header 
*
*END*-----------------------------------------------------------------*/

void NAT_mod_ip
   (
      IP_HEADER_PTR              ip_header_ptr,       /* [IN/OUT] IP datagram */
      NAT_SESSION_STRUCT_PTR     nat_session_ptr,     /* [IN]     NAT session */
      boolean                    pub_to_prv           /* [IN]     Direction   */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   uint_16                 checksum_mod;
   _ip_address             new_ip_addr, old_ip_addr;
   uchar_ptr               replacement_ptr;
   
   if (pub_to_prv) { /* reply */
      new_ip_addr = nat_session_ptr->PRV_HST;
      old_ip_addr = nat_session_ptr->NAT_HST;
      replacement_ptr = ip_header_ptr->DEST;
   } else { /* request */
      new_ip_addr = nat_session_ptr->NAT_HST; 
      old_ip_addr = nat_session_ptr->PRV_HST; 
      replacement_ptr = ip_header_ptr->SOURCE; 
   } /* Endif */
   
   checksum_mod = ntohs(ip_header_ptr->CHECKSUM);
   
   checksum_mod = NAT_chksum_mod(checksum_mod,   /* old chksum */
      (uint_16)(old_ip_addr & 0xFFFF),           /* old data */
      (uint_16)(new_ip_addr & 0xFFFF));          /* new data */

   checksum_mod = NAT_chksum_mod(checksum_mod,   /* old chksum */
      (uint_16)((old_ip_addr >> 16) & 0xFFFF),   /* old data */
      (uint_16)((new_ip_addr >> 16) & 0xFFFF));  /* new data */
  
   htonl(replacement_ptr, new_ip_addr);    
   htons(ip_header_ptr->CHECKSUM, checksum_mod);
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_mod_tcpudp
* Returned Value  : void
* Comments        :
*     Modifies the TCP/UDP header 
*
*END*-----------------------------------------------------------------*/

void NAT_mod_tcpudp
   (
      TRANSPORT_UNION         transport,    /* [IN/OUT] Transport protocol header */ 
      NAT_SESSION_STRUCT_PTR  nat_session_ptr,  /* [IN] The session struct            */
      boolean                 pub_prv,  /* [IN] Direction of packet           */
      boolean                 finrst    /* [IN] Will it go on the FINRST Q?   */
   )
{ /* Body */
   TCPIP_EVENT_PTR      event_ptr;
   NAT_CFG_STRUCT_PTR   nat_cfg_ptr = RTCS_getcfg(NAT);
   uchar_ptr            chksum_ptr, port_ptr;
   _ip_address          new_ip_addr, old_ip_addr;
   uint_32              timeout;
   uint_16              tcp_flags, new_port, old_port, orig;
   uint_16              checksum_mod;
 
   

   /* Read checksum and prepare timeouts */
   if (nat_session_ptr->IP_PROT == IPPROTO_TCP) {

      chksum_ptr = transport.TCP_PTR->checksum;
      /* Check if the packet is a FIN or RST packet */
      tcp_flags = ntohs(transport.TCP_PTR->flags);
      if (finrst || ((tcp_flags & RST) || (tcp_flags & FIN))) {
         /* Use the FIN timeout queue because a FIN or RST was detected */
         event_ptr = &nat_cfg_ptr->FIN_TOUT;
         timeout = nat_cfg_ptr->TIMEOUT_FIN;
      } else { 
         event_ptr = &nat_cfg_ptr->TCP_TOUT;
         timeout = nat_cfg_ptr->TIMEOUT_TCP;
      } /* Endif */
   } else { 
      chksum_ptr = transport.UDP_PTR->CHECKSUM;
      event_ptr = &nat_cfg_ptr->UDP_TOUT;
      timeout = nat_cfg_ptr->TIMEOUT_UDP;
   } /* Endif */
       
   if (pub_prv) { /* reply */
      new_ip_addr = nat_session_ptr->PRV_HST;
      old_ip_addr = nat_session_ptr->NAT_HST;
      new_port = nat_session_ptr->PRV_PORT;
      old_port = nat_session_ptr->NAT_PORT;
      port_ptr = transport.TCP_PTR->dest_port;   
   } else { /* request */
      new_ip_addr = nat_session_ptr->NAT_HST;
      old_ip_addr = nat_session_ptr->PRV_HST;
      new_port = nat_session_ptr->NAT_PORT;
      old_port = nat_session_ptr->PRV_PORT;
      nat_cfg_ptr->GLOBAL_PORT = old_port;
      port_ptr = transport.TCP_PTR->source_port;
   } /* Endif */
   
   /* Modify header and adjust checksum */
   orig = ntohs(chksum_ptr);
   checksum_mod = NAT_chksum_mod(orig,          /* old chksum */
      (uint_16)(old_ip_addr & 0xFFFF),           /* old data */
      (uint_16)(new_ip_addr & 0xFFFF));          /* new data */

   checksum_mod = NAT_chksum_mod(checksum_mod,         /* old chksum */
      (uint_16)((old_ip_addr >> 16) & 0xFFFF),   /* old data */
      (uint_16)((new_ip_addr >> 16) & 0xFFFF));  /* new data */
      
   checksum_mod = NAT_chksum_mod(checksum_mod, old_port, new_port);
   htons(port_ptr, new_port);
      
   
   /* Write final checksum */
   if (orig || nat_session_ptr->IP_PROT != IPPROTO_UDP) {
      htons(chksum_ptr, checksum_mod);
   } /* Endif */
   
   
   /* Reset timeouts */
   NAT_event_del(&nat_session_ptr->TIMEOUT); 
   nat_session_ptr->TIMEOUT.TIME = timeout;
   NAT_event_add(event_ptr, &nat_session_ptr->TIMEOUT);
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_mod_icmp
* Returned Value  : RTCS_OK or uint_32 error code 
* Comments        :
*     Modifies the ICMP header 
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_mod_icmp
   (
      IP_HEADER_PTR                 ip_header_ptr,          /* [IN/OUT] IP datagram       */
      NAT_SESSION_STRUCT_PTR _PTR_  nat_session_ptr_ptr,    /* [IN]     NAT session       */
      boolean                       pub_to_prv,             /* [IN]     Direction         */
      boolean _PTR_                 islocal                 /* [OUT] DEST is NAT router?  */
   )
{ /* Body */
   NAT_SESSION_STRUCT_PTR  nat_session_ptr = *nat_session_ptr_ptr;
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   IP_HEADER_PTR           embedded_ip_header_ptr;
   TRANSPORT_UNION         transport, embedded_transport;
   uchar_ptr               embedded_checksum_ptr = NULL, port_ptr, replacement_ptr;
   _ip_address             new_ip, old_ip;
   boolean                 icmpicmp = FALSE;
   uint_16                 checksum, embedded_checksum, data_len, new_port, old_port;
   uint_16                 checksum_mod;
   uchar                   embedded_ip_protocol;
   uint_32                 error;

   transport.PTR = TRANSPORT_PTR(ip_header_ptr);

   checksum = ntohs(transport.ICMP_PTR->CHECKSUM);
   
   switch (ntohc(transport.ICMP_PTR->TYPE)) {
      case ICMPTYPE_DESTUNREACH:    
      case ICMPTYPE_SRCQUENCH:      /* In all of these cases there is  */
      case ICMPTYPE_TIMEEXCEED:     /* an embedded IP header to modify */
      case ICMPTYPE_PARMPROB:       
         
         /* Calcute data length after embedded IP header */
         embedded_ip_header_ptr = &transport.ERR_PTR->IP;
         embedded_transport.PTR = TRANSPORT_PTR(embedded_ip_header_ptr);

         data_len = ntohs(ip_header_ptr->LENGTH);
         data_len -= (uint_16)((uchar_ptr)embedded_ip_header_ptr - (uchar_ptr)ip_header_ptr);
         data_len -= (uint_16)IPH_LEN(embedded_ip_header_ptr);
         
         embedded_ip_protocol = ntohc(embedded_ip_header_ptr->PROTOCOL);
         
         /* Make sure size is long enough for a lookup */ 
         if ((data_len < 4) || ((data_len < 8) && (embedded_ip_protocol == IPPROTO_ICMP))) {
            return RTCSERR_NAT_NO_SESSION;         
         } /* Endif */        

         /* Try to find the corresponding session struct */
         nat_session_ptr = NAT_lookup(embedded_ip_header_ptr, TRUE, FALSE, &error);
         *nat_session_ptr_ptr=nat_session_ptr;
         
         if (nat_session_ptr == NULL) {
            if (pub_to_prv) {    /* Destination might be NAT router */
               *islocal = TRUE;
               return RTCS_OK;
            } else {             /* Cant find a session, silently discard */
               return RTCSERR_NAT_NO_SESSION;
            } /* Endif */
         } /* Endif */
         
         switch(embedded_ip_protocol) { 
            case IPPROTO_TCP:
               /* ICMP errors don't always contain the whole transport header */
               if (data_len >= 18) {
                  embedded_checksum_ptr = embedded_transport.TCP_PTR->checksum;
               } /* Endif */
               break;
               
            case IPPROTO_UDP:
               if (data_len >= 8 && ntohs(embedded_transport.UDP_PTR->CHECKSUM)) {
                  embedded_checksum_ptr = embedded_transport.UDP_PTR->CHECKSUM;
               } /* Endif */
               break;
               
            case IPPROTO_ICMP:
               embedded_checksum_ptr = embedded_transport.ICMP_PTR->CHECKSUM;
               icmpicmp = TRUE;
               break;
               
            default:
               /* Should never get here */
               return RTCSERR_NAT_UNSUPPORTED_PROTOCOL;
         } /* Endswitch */

         /* The error is in response to a packet going in opposite direction */
         if (pub_to_prv) {
            new_ip = nat_session_ptr->PRV_HST;
            old_ip = nat_session_ptr->NAT_HST;
            replacement_ptr = embedded_ip_header_ptr->SOURCE;
            new_port = nat_session_ptr->PRV_PORT;
            old_port = nat_session_ptr->NAT_PORT;
            port_ptr = embedded_transport.TCP_PTR->source_port;
         } else { 
            new_ip = nat_session_ptr->NAT_HST;
            old_ip = nat_session_ptr->PRV_HST;
            replacement_ptr = embedded_ip_header_ptr->DEST;
            new_port = nat_session_ptr->NAT_PORT;
            old_port = nat_session_ptr->PRV_PORT;
            port_ptr = embedded_transport.TCP_PTR->dest_port;   
         } /* Endif */ 
         
         if (icmpicmp) {
            port_ptr = embedded_transport.ECHO_PTR->SEQ;
         } /* Endif */
         
         if (embedded_checksum_ptr) {
            embedded_checksum = ntohs(embedded_checksum_ptr);

            if (!icmpicmp) {
               /* Modify header and adjust checksum */
               embedded_checksum = NAT_chksum_mod(embedded_checksum,     /* old chksum */
                  (uint_16)(old_ip & 0xFFFF),           /* old data */
                  (uint_16)(new_ip & 0xFFFF));          /* new data */

               embedded_checksum = NAT_chksum_mod(embedded_checksum,     /* old chksum */
                  (uint_16)((old_ip >> 16) & 0xFFFF),   /* old data */
                  (uint_16)((new_ip >> 16) & 0xFFFF));  /* new data */
            } /* Endif */

            embedded_checksum = NAT_chksum_mod(embedded_checksum, old_port, new_port);
            checksum = NAT_chksum_mod(checksum, ntohs(embedded_checksum_ptr), embedded_checksum);
            htons(embedded_checksum_ptr, embedded_checksum);
         } else {
            embedded_checksum = 0;
         } /* Endif */
         
         
         checksum = NAT_chksum_mod(checksum, old_port, new_port);         
         
         /* Write new port number */
         htons(port_ptr, new_port);
            
         checksum_mod = ntohs(embedded_ip_header_ptr->CHECKSUM);
         
         checksum_mod = NAT_chksum_mod(checksum_mod,           /* old chksum */
            (uint_16)(old_ip & 0xFFFF),           /* old data */
            (uint_16)(new_ip & 0xFFFF));          /* new data */
         
         checksum_mod = NAT_chksum_mod(checksum_mod,           /* old chksum */
            (uint_16)((old_ip >> 16) & 0xFFFF),   /* old data */
            (uint_16)((new_ip >> 16) & 0xFFFF));  /* new data */
        
         htonl(replacement_ptr, new_ip);    
         htons(embedded_ip_header_ptr->CHECKSUM, checksum_mod);

         break;
         
      default:       
         /* Modify Query SEQ */
         if (pub_to_prv) {
            checksum = NAT_chksum_mod(checksum, nat_session_ptr->NAT_PORT, 
               nat_session_ptr->PRV_PORT);
            htons(transport.ECHO_PTR->SEQ, nat_session_ptr->PRV_PORT);
         } else { 
            checksum = NAT_chksum_mod(checksum, nat_session_ptr->PRV_PORT, 
               nat_session_ptr->NAT_PORT);
            htons(transport.ECHO_PTR->SEQ, nat_session_ptr->NAT_PORT);
         } /* Endif */
         
         /* Reset timeouts */
         NAT_event_del(&nat_session_ptr->TIMEOUT); 
         nat_cfg_ptr = RTCS_getcfg(NAT);
         
            nat_session_ptr->TIMEOUT.TIME = nat_cfg_ptr->TIMEOUT_ICMP;
            NAT_event_add(&nat_cfg_ptr->ICMP_TOUT, &nat_session_ptr->TIMEOUT);
         
         break;
   } /* Endswitch */
   
   /* Rewrite the checksum */
   htons(transport.ICMP_PTR->CHECKSUM, checksum);
   
   return RTCS_OK;
} /* Endbody */     

#endif
 
/* EOF */
