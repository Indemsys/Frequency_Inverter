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
* $FileName: bootp.c$
* $Version : 3.8.8.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the BOOTP client
*   for the TCP/IP protocol suite.
*   This implementation does not:
*   - allow the client to specify its own IP address in
*   the BOOTREQUEST.  BOOTREQUESTs are always sent with
*   source = 0.0.0.0 and destination = 255.255.255.255.
*   If the client really does know its IP address, it
*   can bind the address to the interface and then
*   transmit a BOOTREQUEST through a UDP socket.
*   - allow the client to direct the BOOTREQUEST to a
*   specific server.  ARP is not initialized until the
*   BOOTREPLY is received, so the client will not be
*   able to ARP the server.  Thus, BOOTREQUESTs are
*   always sent via limited broadcast.
*   For more information, refer to:
*   RFC 951  (BOOTP)
*   RFC 1542 (BOOTP Extensions)
*   RPC 1533 (BOOTP Options)
*   Limitations:  Although BOOTP can be used over any medium, this
*   implementation will work only over an Ethernet
*   link layer.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4

#include "rtcs_prv.h"
#include "tcpip.h"
#include "bootp.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : BOOTP_open
* Returned Values : uint_32 (error code)
* Comments        :
*     Initialize the BOOTP client.
*
*END*-----------------------------------------------------------------*/

void BOOTP_open
   (
      TCPIP_PARM_BOOTP _PTR_ parms
   )
{ /* Body */
   IP_IF_PTR      if_ptr = (IP_IF_PTR)parms->handle;
   BOOTP_CFG_PTR  bootp = (BOOTP_CFG_PTR) &parms->config;
   uint_32        error;

   error = BOOT_open(BOOT_service);
   if (error) {
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */

   if_ptr->BOOTFN = BOOTP_service;

   /* Pick a random transaction ID */
   bootp->XID = RTCS_rand();

   /* Set initial timeout */
   bootp->TIMEOUT = BOOTP_TIMEOUT_MIN;
   bootp->SECS = 0;

   /* Build a BOOTREQUEST packet */
   htonc(bootp->PACKET.OP,    BOOTPOP_BOOTREQUEST);
   htonc(bootp->PACKET.HTYPE, if_ptr->DEV_TYPE);
   htonc(bootp->PACKET.HLEN,  if_ptr->DEV_ADDRLEN);
   htonc(bootp->PACKET.HOPS,  0);
   htonl(bootp->PACKET.XID,   bootp->XID);
   htons(bootp->PACKET.FLAGS, 0x8000);
   htonl(bootp->PACKET.CIADDR, INADDR_ANY);
   htonl(bootp->PACKET.YIADDR, INADDR_ANY);
   htonl(bootp->PACKET.SIADDR, INADDR_ANY);
   htonl(bootp->PACKET.GIADDR, INADDR_ANY);

   _mem_zero(bootp->PACKET.CHADDR, sizeof(bootp->PACKET.CHADDR));
   _mem_copy(if_ptr->DEV_ADDR, bootp->PACKET.CHADDR, if_ptr->DEV_ADDRLEN);

   /* Start the retransmission timer to start sending immediately */
   bootp->RESEND.TIME    = 0;
   bootp->RESEND.EVENT   = BOOTP_send;
   bootp->RESEND.PRIVATE = if_ptr;
   TCPIP_Event_add(&bootp->RESEND);

   if_ptr->BOOT = (pointer)parms;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : BOOTP_close
* Returned Values : uint_32 (error code)
* Comments        :
*     Terminate the BOOTP client.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOTP_close
   (
      IP_IF_PTR   if_ptr
         /* [IN] IP interface structure */
   )
{ /* Body */
   BOOTP_CFG_PTR  bootp = (BOOTP_CFG_PTR) &((TCPIP_PARM_BOOTP _PTR_)if_ptr->BOOT)->config;

   TCPIP_Event_cancel(&bootp->RESEND);
   if_ptr->BOOTFN = NULL;
   if_ptr->BOOT   = NULL;

   return BOOT_close();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : BOOTP_send
* Returned Values :
* Comments        :
*     Called by the Timer.  Send a BOOTP BOOTREQUEST packet.
*
*END*-----------------------------------------------------------------*/

boolean BOOTP_send
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the resend event */
   )
{ /* Body */
   IP_IF_PTR               if_ptr = (IP_IF_PTR)event->PRIVATE;
   TCPIP_PARM_BOOTP _PTR_  parms = (TCPIP_PARM_BOOTP _PTR_)if_ptr->BOOT;
   BOOTP_CFG_PTR           bootp = (BOOTP_CFG_PTR) &parms->config;
   RTCSPCB_PTR             pcb_ptr;

   /* Set the event to trigger for the next retransmission (+/- 1 sec) */
   bootp->RESEND.TIME = bootp->TIMEOUT + (RTCS_rand() & 0x7FF) - 0x400;

   /* Allocate a PCB */
   pcb_ptr = RTCSPCB_alloc_send();
   if (pcb_ptr == NULL) {
      return TRUE;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb_ptr);

   /* The only field that changes in BOOTREQUEST packets is 'secs' */
   htons(bootp->PACKET.SECS, bootp->SECS);
   bootp->SECS += bootp->RESEND.TIME >> 10;     /* approx. divide by 1000 */

   /* Double the timeout */
   bootp->TIMEOUT <<= 1;
   if (bootp->TIMEOUT > BOOTP_TIMEOUT_MAX) {
      bootp->TIMEOUT = BOOTP_TIMEOUT_MAX;
   } /* Endif */

   /* Put the BOOTREQUEST in the PCB */
   RTCSPCB_append_fragment(pcb_ptr, sizeof(BOOTP_HEADER), (uchar_ptr)&bootp->PACKET);
   RTCSPCB_append_fragment(pcb_ptr, sizeof(BOOTP_DATA),   parms->data->SNAME);

   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_PORT(IPPORT_BOOTPC), 0);

   /* Send the datagram */
   BOOT_send(pcb_ptr, if_ptr);

   /* Always retransmit */
   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : BOOTP_service
* Returned Values : void
* Comments        :
*     Services a BOOTREPLY packet.
*
*END*-----------------------------------------------------------------*/

void BOOTP_service
   (
      RTCSPCB_PTR    rtcs_pcb     /* [IN] BOOTREPLY packet */
   )
{ /* Body */
   IP_IF_PTR               if_ptr = (IP_IF_PTR)rtcs_pcb->IFSRC;
   TCPIP_PARM_BOOTP _PTR_  parms = (TCPIP_PARM_BOOTP _PTR_)if_ptr->BOOT;
   BOOTP_CFG_PTR           bootp = (BOOTP_CFG_PTR) &parms->config;
   BOOTP_PACKET_PTR        bootreply;
   IPIF_PARM               parms_bind;
   uint_32                 error;

   uchar_ptr   opt;
   uchar       len, optval, optlen;

   /* Make sure the datagram is large enough */
   bootreply = (BOOTP_PACKET_PTR)RTCSPCB_DATA(rtcs_pcb);
   if (RTCSPCB_SIZE(rtcs_pcb) < sizeof(BOOTP_PACKET)) {
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
      RTCSPCB_free(rtcs_pcb);
      return;
   } /* Endif */

   /* Make sure the XID matches */
   if (ntohl(bootreply->HEAD.XID) != bootp->XID) {
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
      RTCSPCB_free(rtcs_pcb);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(rtcs_pcb, RTCS_LOGCTRL_PORT(IPPORT_BOOTPC), 0);

   /* OK, assume this reply is for us */
   BOOTP_close(if_ptr);

   /* Get our IP address, and pick the default netmask */
   parms_bind.ihandle = if_ptr;
   parms_bind.address = ntohl(bootreply->HEAD.YIADDR);
   parms_bind.locmask = 0xFFFFFFFFL;
   parms_bind.netmask = IN_DEFAULT_NET(parms_bind.address);
   parms_bind.probe = FALSE;

   parms->data->SADDR = ntohl(bootreply->HEAD.SIADDR);
#if RTCSCFG_BOOTP_RETURN_YIADDR
   parms->data->CLIENTADDR = ntohl(bootreply->HEAD.YIADDR);
#endif
   _mem_copy(bootreply->DATA.SNAME, parms->data->SNAME, sizeof(BOOTP_DATA));
   RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
   RTCSPCB_free(rtcs_pcb);

   /* Parse the vend field for recognized options */
   opt = parms->data->OPTIONS;
   len = sizeof(parms->data->OPTIONS);
   if (ntohl(opt) == BOOTP_MAGIC) {
      opt += 4;
      len -= 4;

#define BOOTP_NEXTOPT   opt += optlen; \
                        break

      while (len) {

         /* Get the next option code */
         optval = ntohc(opt);
         opt++;
         len--;

         /* Interpret the pad and end options */
         if (optval == BOOTPOPT_END) break;
         if (optval == BOOTPOPT_PAD) continue;

         /* All other codes have a length byte */
         if (len == 0) break;
         optlen = ntohc(opt);
         opt++;
         len--;
         if (len < optlen) break;
         len -= optlen;

         switch (optval) {

         case BOOTPOPT_MASK:
            if (optlen != 4) {BOOTP_NEXTOPT;}
            parms_bind.netmask = ntohl(opt);
            opt += 4;
            break;

         default:
            BOOTP_NEXTOPT;
         } /* Endswitch */

      } /* Endwhile */
   } /* Endif */

   /* Bind the received IP address to this interface */
   error = RTCSCMD_internal(parms_bind, IPIF_bind);

   /* Done -- unblock the application */
   RTCSCMD_complete(parms, error);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
