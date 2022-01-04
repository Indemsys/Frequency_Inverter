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
* $FileName: igmp.c$
* $Version : 3.8.12.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the Internet
*   Group Management Protocol.  For more details, refer
*   to RFC1112 for version 1 and RFC2236 for version 2.
*
*END************************************************************************/

/*
** - review the protocol compliance. drop timer and unsolicited counter when i leave.
**   require to send igmp query/report from another computer.
** - the application problem is: RTCS don't listen on a address/port but only
**   on a port, sending a pkt on a specified interface, supporting the classical
**   setsockopts i.e. NOLOOP(do not loopback the outgoing packet), SET_TTL
**   (set the ttl for the outgoing multicast packet), SET_OUTGOING_IF (set the
**   outgoing interface)
** - IGMPV2 is complete except the router alert ip option which must be added for
**   each igmp packet because ip_send doesnt support the ip option. rfc2236.10
**   (security consideration) a report without a router alert must be discarded.
*/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4

#ifdef IGMP_V2
#  define IGMP_DEFAULT_REPORT   IGMPTYPE_V2_REPORT
#else
#  define IGMP_DEFAULT_REPORT   IGMPTYPE_V1_REPORT
#endif

/* local prototypes */
static void IGMP_service(RTCSPCB_PTR pcb, pointer dummy);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_init
* Returned Values : uint_32
* Comments        :
*     Initialize the IGMP layer.
*
*END*-----------------------------------------------------------------*/

uint_32 IGMP_init
   (
      void
   )
{ /* Body */
   uint_32              error;
   IGMP_CFG_STRUCT_PTR  IGMP_cfg_ptr;

   /* alloc and init the configuration block */
   IGMP_cfg_ptr = RTCS_mem_alloc_zero(sizeof(IGMP_CFG_STRUCT));
   if (!IGMP_cfg_ptr) {
      return RTCSERR_IGMP_CFG;
   } /* Endif */

   /* register the configuration block */
   RTCS_setcfg(IGMP, IGMP_cfg_ptr);

   /* register the igmp protocol in the ip layer */
   IP_open(IPPROTO_IGMP, IGMP_service, NULL, &error);
   if (error != RTCS_OK) {
      return RTCSERR_IGMP_CFG;
   } /* Endif */

   /*
   ** When IGMP_init() is called, the only registered interface is the
   ** loopback interface, so we don't need to join the all-hosts group
   ** on any interface here.
   */

   return error;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_send_report
* Returned Values : uint_32
* Comments        :
*     send a report on the network
*
*END*-----------------------------------------------------------------*/

boolean IGMP_send_report
   (
      ip_mreq _PTR_ igrp,
      uint_16  type
   )
{ /* Body */
   IGMP_CFG_STRUCT_PTR  IGMP_cfg_ptr = RTCS_getcfg(IGMP);
   uint_32              error;
   uint_16              checksum;
   RTCSPCB_PTR          pcb;
   IGMP_HEADER_PTR      header;
   IP_IF_PTR            ipif;
   _ip_address          ipdst;

   IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);

   /* never send a report for the local groups */
   if (IN_LOCAL_MULTICAST(igrp->imr_multiaddr.s_addr)) {
      return TRUE;
   } /* Endif */

   /* get the interface with its ip address */
   ipif = IP_find_if(igrp->imr_interface.s_addr);
   if (ipif == NULL) {
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      return FALSE;
   } /* Endif */

   pcb = RTCSPCB_alloc_send();
   if (pcb == NULL) {
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      return FALSE;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb);

   error = RTCSPCB_insert_header(pcb, sizeof(IGMP_HEADER));
   if (error) {
      RTCSLOG_PCB_FREE(pcb, error);
      RTCSPCB_free(pcb);
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      return FALSE;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IGMP), 1);

#ifdef IGMP_V2
   /* check if routers igmpv1 are present or not */
   if (type == IGMPTYPE_V2_REPORT && ipif->IGMP_V1_ROUTER_FLAG) {
      uint_32 curTime = RTCS_time_get();
      /* WORK: handle the overflow (see tcp seq) */
      if (curTime < ipif->IGMP_V1_ROUTER_TIMEOUT) {
         type = IGMPTYPE_V1_REPORT;
      } else {
         /* if the timeout expired, clear the flag */
         ipif->IGMP_V1_ROUTER_FLAG = 0;
      } /* Endif */
   } /* Endif */
#endif   /* IGMP_V2 */

   /* build the igmp packet */
   header = (IGMP_HEADER_PTR)RTCSPCB_DATA(pcb);
   htonc(header->TYPE, type);
   htonc(header->MAX_RESP_TIME, 0);
   htons(header->CHECKSUM, 0);
   htonl(header->GROUP_ADDRESS, igrp->imr_multiaddr.s_addr);
   checksum = _mem_sum_ip(0, sizeof(IGMP_HEADER), header);
   checksum = IP_Sum_invert(checksum);
   htons(header->CHECKSUM, checksum);

   /* WORK: for IGMP_V2, add a router alert option but currently ip layer doesnt support ip options */

   /* send the igmp packet */
   ipdst = igrp->imr_multiaddr.s_addr;
#ifdef IGMP_V2
   if (type == IGMPTYPE_LEAVE) {
      ipdst = INADDR_ALLROUTERS_GROUP;
   } /* Endif */
#endif

   IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.ST_TX_REPORT++);    /* WORK: not always true for IGMP_V2 */
   error = IP_send(pcb, IPPROTO_IGMP | IPTTL(1), igrp->imr_interface.s_addr, ipdst, RTCS_MSG_NOLOOP);

   return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_expire
* Returned Values :
* Comments        :
*     Called by the Timer.  Expire a IGMP report datagram.
*
*END*-----------------------------------------------------------------*/

static boolean IGMP_expire
   (
      TCPIP_EVENT_PTR   event          /* [IN/OUT] the expiration event */
   )
{ /* Body */
   MC_MEMBER_PTR  member = event->PRIVATE;
   uint_32        error;

   member->RUNNING_TIMER = FALSE;

   error = IGMP_send_report(&member->IGRP, IGMP_DEFAULT_REPORT);

   /* an error occurs, retry later */
   if (!error) {
      member->TIMER.TIME = RTCS_rand()%IGMP_UNSOLICITED_REPORT_INTERVAL;
      member->RUNNING_TIMER = TRUE;
      return TRUE;
   } /* Endif */

   /* handle the unsolicited_report repetition */
   if (member->UNSOLICITED_REPORT_COUNTER) {
      member->UNSOLICITED_REPORT_COUNTER--;
      member->TIMER.TIME = IGMP_UNSOLICITED_REPORT_INTERVAL;
      member->RUNNING_TIMER = TRUE;
      return TRUE;
   } /* Endif */
#ifdef IGMP_V2
   member->LAST_REPORTER = TRUE;
#endif

   return FALSE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_init_timer
* Returned Values : void
* Comments        :
*     initialize the timer for one group
*
*END*-----------------------------------------------------------------*/

void IGMP_init_timer
   (
      MC_MEMBER_PTR  group
   )
{ /* Body */

   group->TIMER.EVENT = IGMP_expire;
   group->TIMER.PRIVATE = group;
   group->RUNNING_TIMER = FALSE;
#ifdef IGMP_V2
   group->LAST_REPORTER = FALSE;
#endif
   group->UNSOLICITED_REPORT_COUNTER = IGMP_UNSOLICITED_REPORT_COUNT;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_launch_timer
* Returned Values : void
* Comments        :
*     launch a timer for one group
*
*END*-----------------------------------------------------------------*/

void IGMP_launch_timer
   (
      MC_MEMBER_PTR  member,
      uint_32        delay
   )
{ /* Body */

   if (member->RUNNING_TIMER) {
      return;
   } /* Endif */

   member->TIMER.TIME = delay;
   member->RUNNING_TIMER = TRUE;
   TCPIP_Event_add(&member->TIMER);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_stop_timer
* Returned Values : void
* Comments        :
*     stop a timer for one group
*
*END*-----------------------------------------------------------------*/

void IGMP_stop_timer
   (
      MC_MEMBER_PTR  member
   )
{ /* Body */

   if (!member->RUNNING_TIMER) {
      return;
   } /* Endif */

   TCPIP_Event_cancel(&member->TIMER);
   member->RUNNING_TIMER = FALSE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_rcv_report
* Returned Values :
* Comments        :
*     called when a report is received (from another computer)
*
*END*-----------------------------------------------------------------*/

static void IGMP_rcv_report
   (
      IP_IF_PTR   ipif,    /* [IN] the interface which receive the report */
      _ip_address group_ip /* [IN] the ip address of the multicast group */
   )
{ /* Body */
   MC_MEMBER_PTR  member;

   /* sanity check */
   if (IN_LOCAL_MULTICAST(group_ip) || !IN_MULTICAST(group_ip)) {
      return;
   } /* Endif */

   /* stop the running timer of this group */
   for (member = ipif->IGMP_MEMBER ; member ; member = member->NEXT) {
      if (member->IGRP.imr_multiaddr.s_addr == group_ip) {
         IGMP_stop_timer(member);
#ifdef IGMP_V2
         member->LAST_REPORTER = 0;
#endif
         member->UNSOLICITED_REPORT_COUNTER = 0;
         /* continue to scan the list in case of the same group_ip is joined
         ** with a different interface address. an given interface can have
         ** several ip address.
         */
      } /* Endif */
   } /* Endfor */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_rcv_query
* Returned Values :
* Comments        :
*     called when a query is received (from a router)
*
*END*-----------------------------------------------------------------*/

static void IGMP_rcv_query
   (
      IP_IF_PTR   ipif,          /* [IN] the interface which receive the report */
      _ip_address group_ip,      /* [IN] the ip address of the multicast group */
      uint_32     max_resp_time  /* [IN] the maximum response time of the query */
   )
{ /* Body */
   MC_MEMBER_PTR  member;


   /* sanity check */
   if (IN_LOCAL_MULTICAST(group_ip)) {
      return;
   } /* Endif */

#ifndef IGMP_V2
   max_resp_time = 0;   /* force a IGMP_V1 report */
#endif

   /* test if it is a IGMPv1 query */
   if (max_resp_time == 0) {
      max_resp_time = IGMP_V1_QUERY_RESPONSE_INTERVAL;
#ifdef IGMP_V2
      /* set the time out */
      ipif->IGMP_V1_ROUTER_FLAG = TRUE;
      ipif->IGMP_V1_ROUTER_TIMEOUT = RTCS_time_get()
                                     + IGMP_V1_ROUTER_TIMEOUT_VALUE;
#endif /* IGMP_V2 */
      group_ip = 0;  /* with IGMPv1, an query is always general (never group-specific) */
   } else  {
      /* convert IGMP_UNIT (1/10 sec) into the local unit (1/1000 sec) */
      max_resp_time = max_resp_time * (1000/10);
   } /* Endif */

   /* test if the group is already joined */
   for (member = ipif->IGMP_MEMBER ; member ; member = member->NEXT) {
      /* filter the suitable member(s) */
      if (group_ip && group_ip != member->IGRP.imr_multiaddr.s_addr) {
         continue;
      } /* Endif */

      /* never report a local group */
      if (IN_LOCAL_MULTICAST(member->IGRP.imr_multiaddr.s_addr)) {
         continue;
      } /* Endif */
      if (member->RUNNING_TIMER) {
         /* if a timer is running, check if the timer must be relaunched */
         if (TCPIP_Event_expire(&member->TIMER) > max_resp_time) {
            IGMP_stop_timer(member);
            IGMP_launch_timer(member, RTCS_rand() % max_resp_time);
         } /* Endif */
      } else {
         IGMP_init_timer(member);
         IGMP_launch_timer(member, RTCS_rand() % max_resp_time);
      } /* Endif */
   } /* Endfor */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IGMP_service
* Returned Values : void
* Comments        :
*     Process incoming IGMP packets.  Called from IP_service.
*
*END*-----------------------------------------------------------------*/

static void IGMP_service
   (
      RTCSPCB_PTR    pcb,        /* [IN/OUT] incoming packet */
      pointer        dummy       /* [IN]     not used        */
   )
{ /* Body */
   IGMP_HEADER_PTR      header;
   IGMP_CFG_STRUCT_PTR  IGMP_cfg_ptr;

   IGMP_cfg_ptr = RTCS_getcfg(IGMP);
   IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.COMMON.ST_RX_TOTAL++);

   header = (IGMP_HEADER_PTR)RTCSPCB_DATA(pcb);

   /* check if length >= sizeof(IGMP_HEADER) */
   if (RTCSPCB_SIZE(pcb) < sizeof(IGMP_HEADER)) {
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.ST_RX_SMALL_DGRAM++);
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IGMP_BAD_HEADER);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */

   /* Verify the checksum */
   if (IP_Sum_PCB(0, pcb) != 0xFFFF) {
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.ST_RX_BAD_CHECKSUM++);
      RTCSLOG_PCB_FREE(pcb, RTCSERR_IGMP_BAD_CHECKSUM);
      RTCSPCB_free(pcb);
      return;
   } /* Endif */

   if (ntohc(header->TYPE) == IGMPTYPE_V2_REPORT) {
      RTCSLOG_PCB_READ(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IGMP), 2);
   } else {
      RTCSLOG_PCB_READ(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_IGMP), 1);
   } /* Endif */

   switch (ntohc(header->TYPE)) {
   case IGMPTYPE_QUERY:
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.ST_RX_QUERY++);
      IGMP_rcv_query(pcb->IFSRC, ntohl(header->GROUP_ADDRESS), ntohc(header->MAX_RESP_TIME));
      break;
   case IGMPTYPE_V1_REPORT:
   case IGMPTYPE_V2_REPORT:
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.ST_RX_REPORT++);
      IGMP_rcv_report(pcb->IFSRC, ntohl(header->GROUP_ADDRESS));
      break;
   default:
      IF_IGMP_STATS_ENABLED(IGMP_cfg_ptr->STATS.ST_RX_BAD_TYPE++);
   } /* Endswitch */

   RTCSLOG_PCB_FREE(pcb, RTCS_OK);
   RTCSPCB_free(pcb);

} /* Endbody */

#endif

/* EOF */
