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
* $FileName: ipcp.c$
* $Version : 3.8.14.0$
* $Date    : Sep-19-2012$
*
* Comments:
*
*   This file contains the interface between IP and
*   the PPP packet driver.
*
*END************************************************************************/

#include <rtcs.h>
#include <fio.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"

#include <ppp.h>
#include "pppfsm.h"
#include "ipcp.h"


static void IPCP_lowerup   (pointer);
static void IPCP_lowerdown (pointer);

/*
** The structure used by the application to bind an IP address
** to a PPP channel.
*/

static RTCS_IF_STRUCT rtcs_ppp =
{
    IPCP_init,
    IPCP_destroy
#if RTCSCFG_ENABLE_IP4 
    ,  
    IPCP_send
#if RTCSCFG_ENABLE_IGMP && BSPCFG_ENABLE_ENET_MULTICAST 
    ,
    NULL,
    NULL
#endif
#endif /* RTCSCFG_ENABLE_IP4 */
};

const RTCS_IF_STRUCT_PTR RTCS_IF_PPP = &rtcs_ppp;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_init
* Returned Value  :  error code
* Comments        :
*     Called by TCP/IP task.  Initializes a PPP link.
*
*END*-----------------------------------------------------------------*/

uint_32 IPCP_init
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4  

   _ppp_handle handle = if_ptr->HANDLE;
   IPCP_CFG_STRUCT_PTR ipcp_ptr;
   PPPFSM_CFG_PTR fsm;
   uint_32 error;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   } /* Endif */

   /* Allocate state for this interface */
   ipcp_ptr = RTCS_mem_alloc(sizeof(IPCP_CFG_STRUCT));
   if (!ipcp_ptr) {
      return RTCSERR_IPCP_CFG;
   } /* Endif */

   /* Initialize the IPCP FSM */
   fsm = &ipcp_ptr->FSM;
   error = PPPFSM_init(fsm, handle, &IPCP_FSM_CALL, if_ptr);
   if (error) {
      _mem_free(ipcp_ptr);
      return PPP_TO_RTCS_ERROR(error);
   } /* Endif */

   /* Register IPCP */
   error = PPP_register(handle, PPP_PROT_IPCP, PPPFSM_input, fsm);
   if (error) {
      PPPFSM_destroy(fsm);
      _mem_free(ipcp_ptr);
      return PPP_TO_RTCS_ERROR(error);
   } /* Endif */

   /* Register IP */
   error = PPP_register(handle, PPP_PROT_IP, IPCP_recv, if_ptr);
   if (error) {
      PPP_unregister(handle, PPP_PROT_IPCP);
      PPPFSM_destroy(fsm);
      _mem_free(ipcp_ptr);
      return PPP_TO_RTCS_ERROR(error);
   } /* Endif */

   ipcp_ptr->HANDLE = handle;
   if_ptr->HANDLE = ipcp_ptr;
   if_ptr->MTU = IP_DEFAULT_MTU;
   if_ptr->ARP = NULL;

   if_ptr->DEV_TYPE    = 20;
   if_ptr->DEV_ADDRLEN = 1;
   if_ptr->DEV_ADDR[0] = 0xFF;
#if RTCSCFG_ENABLE_SNMP && RTCSCFG_ENABLE_IP4
   if_ptr->SNMP_IF_TYPE = IPIFTYPE_PPP;
#endif
   return RTCS_OK;

#else
    
    return RTCSERR_IP_IS_DISABLED;    
    
#endif /* RTCSCFG_ENABLE_IP4 */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_destroy
* Returned Value  :  error code
* Comments        :
*     Called by TCP/IP task.  Closes a PPP link.
*
*END*-----------------------------------------------------------------*/

uint_32 IPCP_destroy
   (
      IP_IF_PTR   if_ptr
         /* [IN] the IP interface structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;

   PPP_unregister(ipcp_ptr->HANDLE, PPP_PROT_IP);
   PPP_unregister(ipcp_ptr->HANDLE, PPP_PROT_IPCP);
   PPPFSM_lowerdown(&ipcp_ptr->FSM);
   PPPFSM_destroy(&ipcp_ptr->FSM);
   _mem_free(ipcp_ptr);
   if_ptr->HANDLE = NULL;

   return RTCS_OK;

#else
    
    return RTCSERR_IP_IS_DISABLED;    
    
#endif /* RTCSCFG_ENABLE_IP4 */   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_bind
* Returned Value  :  error code
* Comments        :
*     Called by TCP/IP task.  Initializes and opens a PPP link.
*
*END*-----------------------------------------------------------------*/

void IPCP_bind
   (
      TCPIP_PARM_IPCP _PTR_ parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   IP_IF_PTR            if_ptr = parms->handle;
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;
   _ppp_handle          handle = ipcp_ptr->HANDLE;
   uint_32              error;

   ipcp_ptr->CALLUP  = IPCP_lowerup;
   ipcp_ptr->PARAMUP = &ipcp_ptr->FSM;
   ipcp_ptr->CALLDOWN  = IPCP_lowerdown;
   ipcp_ptr->PARAMDOWN = &ipcp_ptr->FSM;

   error = PPP_setcall(handle, PPP_CALL_LINK_UP, &ipcp_ptr->CALLUP, &ipcp_ptr->PARAMUP);
   if (!error) {
      error = PPP_setcall(handle, PPP_CALL_LINK_DOWN, &ipcp_ptr->CALLDOWN, &ipcp_ptr->PARAMDOWN);
      if (error) {
         PPP_setcall(handle, PPP_CALL_LINK_UP, &ipcp_ptr->CALLUP, &ipcp_ptr->PARAMUP);
      } /* Endif */
   } /* Endif */

   if (!error) {
      ipcp_ptr->IP_UP    = parms->data->IP_UP;
      ipcp_ptr->IP_DOWN  = parms->data->IP_DOWN;
      ipcp_ptr->IP_PARAM = parms->data->IP_PARAM;

      ipcp_ptr->INIT.ACCEPT_LOCAL_ADDR  = parms->data->ACCEPT_LOCAL_ADDR;
      ipcp_ptr->INIT.LOCAL_ADDR         = parms->data->LOCAL_ADDR;
      ipcp_ptr->INIT.ACCEPT_REMOTE_ADDR = parms->data->ACCEPT_REMOTE_ADDR;
      ipcp_ptr->INIT.REMOTE_ADDR        = parms->data->REMOTE_ADDR;
      ipcp_ptr->INIT.DEFAULT_NETMASK    = parms->data->DEFAULT_NETMASK;
      ipcp_ptr->INIT.NETMASK            = parms->data->NETMASK;
      ipcp_ptr->INIT.DEFAULT_ROUTE      = parms->data->DEFAULT_ROUTE;
      ipcp_ptr->INIT.NEG_LOCAL_DNS      = parms->data->NEG_LOCAL_DNS;
      ipcp_ptr->INIT.ACCEPT_LOCAL_DNS   = parms->data->ACCEPT_LOCAL_DNS;
      ipcp_ptr->INIT.LOCAL_DNS          = parms->data->LOCAL_DNS;
      ipcp_ptr->INIT.NEG_REMOTE_DNS     = parms->data->NEG_REMOTE_DNS;
      ipcp_ptr->INIT.ACCEPT_REMOTE_DNS  = parms->data->ACCEPT_REMOTE_DNS;
      ipcp_ptr->INIT.REMOTE_DNS         = parms->data->REMOTE_DNS;

      /* Start IPCP */
      PPPFSM_open(&ipcp_ptr->FSM, 0);

      /* Open the link */
      error = PPP_open(handle, 0);

   } /* Endif */

   RTCSCMD_complete(parms, PPP_TO_RTCS_ERROR(error));

#else
    
    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);
    
#endif /* RTCSCFG_ENABLE_IP4 */     

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_lowerup
* Returned Value  :  void
* Comments        :
*     Called by PPP.  Begin negotiating IPCP once PPP is up.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4 

static void IPCP_lowerup
   (
      pointer fsm
            /* [IN] - IPCP automaton */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = ((PPPFSM_CFG_PTR)fsm)->PRIVATE;
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;

   /* Set the MTU */
   if_ptr->MTU = PPP_getmtu(ipcp_ptr->HANDLE);

   /* Call the previous PPP_LINK_UP callback */
   if (ipcp_ptr->CALLUP) {
      ipcp_ptr->CALLUP(ipcp_ptr->PARAMUP);
   } /* Endif */

   /* Start IPCP */
   PPPFSM_lowerup(fsm);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_lowerdown
* Returned Value  :  void
* Comments        :
*     Called by PPP.  Terminate IPCP once PPP is down.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4 

static void IPCP_lowerdown
   (
      pointer fsm
            /* [IN] - IPCP automaton */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = ((PPPFSM_CFG_PTR)fsm)->PRIVATE;
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;

   /* Call the previous PPP_LINK_DOWN callback */
   if (ipcp_ptr->CALLDOWN) {
      ipcp_ptr->CALLDOWN(ipcp_ptr->PARAMDOWN);
   } /* Endif */

   /* Start IPCP */
   PPPFSM_lowerdown(fsm);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_recv
* Returned Value  :  nothing
* Comments        :
*     Called by PPP.  Service an incoming IP datagram.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4 

void IPCP_recv
   (
      PCB_PTR     pcb,
            /* [IN] received packet */
      pointer     handle
            /* [IN] IPCP configuration */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = handle;
   RTCSPCB_PTR          packet;
   uint_32              error;

   IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_TOTAL++);
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_RX_OCTETS += pcb->FRAG[0].LENGTH);
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_RX_UNICAST++);

   packet = RTCSPCB_alloc_recv(pcb);
   if (packet == NULL) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_MISSED++);
      PCB_free(pcb);
      return;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(packet);

   error = RTCSPCB_next(packet, 2);
   if (error) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_ERRORS++);
      IF_IPIF_STATS_ENABLED(RTCS_seterror(&if_ptr->STATS.ERR_RX, error, (uint_32)packet));
      RTCSLOG_PCB_FREE(packet, error);
      RTCSPCB_free(packet);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(packet, RTCS_LOGCTRL_IFTYPE(IPIFTYPE_PPP), 0);

   packet->TYPE = RTCSPCB_TYPE_ONLYCAST;
   packet->IFSRC = if_ptr;

   if (!RTCSCMD_service(packet, IP_service)) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_RX_MISSED++);
      RTCSLOG_PCB_FREE(packet, RTCS_OK);
      RTCSPCB_free(packet);
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_send
* Returned Value  :  nothing
* Comments        :
*     Called by IP_Send.  Send an IP datagram over a PPP link.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4 

uint_32 IPCP_send
   (
      IP_IF_PTR      if_ptr,
         /* [IN] the IP interface structure */
      RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
      _ip_address    src,
         /* [IN] the next-hop source address */
      _ip_address    dest,
         /* [IN] the next-hop destination address */
      pointer        data
         /* [IN] unused */
   )
{ /* Body */
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;
   _ppp_handle          handle = ipcp_ptr->HANDLE;
   uint_32              error;

   IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_TOTAL++);
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_OCTETS += RTCSPCB_SIZE(pcb));
   IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_UNICAST++);

   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_IFTYPE(IPIFTYPE_PPP), 0);

   error = RTCSPCB_insert_header(pcb, 2);
   if (error) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_ERRORS++);
      IF_IPIF_STATS_ENABLED(RTCS_seterror(&if_ptr->STATS.ERR_TX, error, (uint_32)pcb));
      RTCSLOG_PCB_FREE(pcb, error);
      RTCSPCB_free(pcb);
      return error;
   } /* Endif */

   pcb->PCBPTR->FRAG[0].LENGTH   = pcb->HEADER_FRAG_USED;
   pcb->PCBPTR->FRAG[0].FRAGMENT = RTCSPCB_DATA(pcb);

   error = PPP_send(handle, PPP_PROT_IP, pcb->PCBPTR);
   RTCSLOG_PCB_FREE(pcb, error);
   if (error) {
      IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_ERRORS++);
      IF_IPIF_STATS_ENABLED(RTCS_seterror(&if_ptr->STATS.ERR_TX, error, (uint_32)pcb));
   } /* Endif */

   return error;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_get_local_addr
* Returned Value  :  void
* Comments        :
*     Called by the application to get local address.
*
*END*-----------------------------------------------------------------*/

_ip_address  IPCP_get_local_addr
   (
      _rtcs_if_handle   ihandle
            /* [IN] - interface handle */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   IP_IF_PTR            if_ptr = ihandle;
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;
   return ipcp_ptr->LOPT.ADDR;

#else

    return 0;   

#endif /* RTCSCFG_ENABLE_IP4 */
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_get_peer_addr
* Returned Value  :  void
* Comments        :
*     Called by the application to get local address.
*
*END*-----------------------------------------------------------------*/

_ip_address  IPCP_get_peer_addr
   (
      _rtcs_if_handle   ihandle
            /* [IN] - interface handle */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   IP_IF_PTR            if_ptr = ihandle;
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;
   return ipcp_ptr->POPT.ADDR;

#else

    return 0;   

#endif /* RTCSCFG_ENABLE_IP4 */
   
} /* Endbody */


/* EOF */
