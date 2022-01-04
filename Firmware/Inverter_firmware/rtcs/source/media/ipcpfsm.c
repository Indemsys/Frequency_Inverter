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
* $FileName: ipcpfsm.c$
* $Version : 3.8.7.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the IPCP
*   automaton.
*
*END************************************************************************/

/*
** ipcp.c - PPP IP Control Protocol.
**
** Copyright (c) 1989 Carnegie Mellon University.
** All rights reserved.
**
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation,
** advertising materials, and other materials related to such
** distribution and use acknowledge that the software was developed
** by Carnegie Mellon University.  The name of the
** University may not be used to endorse or promote products derived
** from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
** WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4 

#include <fio.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "dns.h"

#include <ppp.h>
#include "pppfsm.h"
#include "ipcp.h"


static boolean IPCP_up(PPPFSM_CFG_PTR);
static void    IPCP_down(PPPFSM_CFG_PTR);

static void    IPCP_resetconfreq(PPPFSM_CFG_PTR);
static uint_32 IPCP_buildconfreq(PPPFSM_CFG_PTR, uchar_ptr, uint_32);

static uint_32 IPCP_recvconfreq(PPPFSM_CFG_PTR, boolean);
static boolean IPCP_recvconfack(PPPFSM_CFG_PTR);
static boolean IPCP_recvconfnak(PPPFSM_CFG_PTR);
static boolean IPCP_recvconfrej(PPPFSM_CFG_PTR);
static boolean IPCP_recvcoderej(PPPFSM_CFG_PTR);

PPPFSM_CALL IPCP_FSM_CALL = {
   /* PROTOCOL    */    PPP_PROT_IPCP,
   /* linkup()    */    IPCP_up,
   /* linkdown()  */    IPCP_down,
   /* resetreq()  */    IPCP_resetconfreq,
   /* buildreq()  */    IPCP_buildconfreq,
   /* recvreq()   */    IPCP_recvconfreq,
   /* recvack()   */    IPCP_recvconfack,
   /* recvnak()   */    IPCP_recvconfnak,
   /* recvrej()   */    IPCP_recvconfrej,
   /* testcode()  */    IPCP_recvcoderej,
   /* recvcode()  */    NULL
};

static IPCP_NEG IPCP_DEFAULT_NEG = {
   /* NEG_* */    0,0,1,0,
   /* ADDR  */    INADDR_ANY,
   /* DNS   */    INADDR_ANY,
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_up
* Returned Value  :  TRUE if negotiated options are acceptable
* Comments        :
*     Called by the FSM when entering the OPENED state.
*
*END*-----------------------------------------------------------------*/

static boolean IPCP_up
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = fsm->PRIVATE;
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;

   /* Bind the negotiated IP address to this interface */
   ipcp_ptr->BIND_PARMS.ihandle = if_ptr;
   ipcp_ptr->BIND_PARMS.address = ipcp_ptr->LOPT.ADDR;
   ipcp_ptr->BIND_PARMS.network = ipcp_ptr->POPT.ADDR;
   
   RTCSCMD_smartissue(ipcp_ptr->BIND_PARMS, IPIF_bind_ppp);

   /* Install a default gateway */
   if (ipcp_ptr->INIT.DEFAULT_ROUTE) {
      ipcp_ptr->GATE_PARMS.address = ipcp_ptr->POPT.ADDR;
      ipcp_ptr->GATE_PARMS.network = INADDR_ANY;
      ipcp_ptr->GATE_PARMS.netmask = INADDR_ANY;
      /* Start CR 1133 */
      ipcp_ptr->GATE_PARMS.locmask = 0;
      /* End CR 1133 */
      RTCSCMD_smartissue(ipcp_ptr->GATE_PARMS, IPIF_gate_add);
   } /* Endif */

#if RTCSCFG_ENABLE_DNS
   /* Set up the primary DNS server */
   if (ipcp_ptr->NEG.NEG_DNS) {
      /* Start CR 2257 */
      DNS_SLIST_STRUCT slist_entry;
      DNS_init_slist_entry(&slist_entry);
      slist_entry.IPADDR = ipcp_ptr->LOPT.DNS;
      if (DNS_add_slist_entry(&slist_entry) != DNS_OK)
         return FALSE;
      /* End CR 2257 */
   } /* Endif */
#endif

   if (ipcp_ptr->IP_UP) {
      ipcp_ptr->IP_UP(ipcp_ptr->IP_PARAM);
   } /* Endif */

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_down
* Returned Value  :  void
* Comments        :
*     Called by the FSM when leaving the OPENED state.
*
*END*-----------------------------------------------------------------*/

static void IPCP_down
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   IP_IF_PTR            if_ptr = fsm->PRIVATE;
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = if_ptr->HANDLE;

   /* Remove the default gateway */
   if (ipcp_ptr->INIT.DEFAULT_ROUTE) {
      RTCSCMD_smartissue(ipcp_ptr->GATE_PARMS, IPIF_gate_remove);
   } /* Endif */

   /* Unbind the interface */
   RTCSCMD_smartissue(ipcp_ptr->BIND_PARMS, IPIF_unbind_ppp);

   if (ipcp_ptr->IP_DOWN) {
      ipcp_ptr->IP_DOWN(ipcp_ptr->IP_PARAM);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_resetconfreq
* Returned Value  :  void
* Comments        :
*     Called by PPPFSM_Sendconfreq when starting negotiation.
*     Initializes the Send Options.
*
*END*-----------------------------------------------------------------*/

static void IPCP_resetconfreq
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   IPCP_CFG_STRUCT_PTR ipcp_ptr = ((IP_IF_PTR)fsm->PRIVATE)->HANDLE;

   ipcp_ptr->NEG = IPCP_DEFAULT_NEG;
   ipcp_ptr->NEG.NEG_DNS = ipcp_ptr->INIT.NEG_LOCAL_DNS;
   ipcp_ptr->NEG.ADDR = ipcp_ptr->INIT.LOCAL_ADDR;
   ipcp_ptr->NEG.DNS  = ipcp_ptr->INIT.LOCAL_DNS;

   ipcp_ptr->LOPT.ADDR = ipcp_ptr->INIT.LOCAL_ADDR;
   ipcp_ptr->LOPT.DNS  = ipcp_ptr->INIT.LOCAL_DNS;
   ipcp_ptr->POPT.ADDR = ipcp_ptr->INIT.REMOTE_ADDR;
   ipcp_ptr->POPT.DNS  = ipcp_ptr->INIT.REMOTE_DNS;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_buildconfreq
* Returned Value  :  number of bytes written
* Comments        :
*     Called by PPPFSM_Sendconfreq.  Builds a ConfReq packet.
*
*END*-----------------------------------------------------------------*/

static uint_32 IPCP_buildconfreq
   (
      PPPFSM_CFG_PTR    fsm,
            /* [IN] - State Machine */
      uchar_ptr         outp,
            /* [IN] - free packet */
      uint_32           sizeleft
            /* [IN] - size of packet */
   )
{ /* Body */
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = ((IP_IF_PTR)fsm->PRIVATE)->HANDLE;
   uint_32              totlen = 0;

#define IPCP_BREQ(ci,len) \
         *outp++ = IPCP_CI_ ## ci; \
         *outp++ = len;            \
         totlen += len

   if (ipcp_ptr->NEG.NEG_ADDR) {
      if (sizeleft >= totlen+6) {
         IPCP_BREQ(ADDR,6);
         htonl(outp, ipcp_ptr->NEG.ADDR); outp += 4;
      } /* Endif */
   } /* Endif */

   if (ipcp_ptr->NEG.NEG_DNS) {
      if (sizeleft >= totlen+6) {
         IPCP_BREQ(DNS,6);
         htonl(outp, ipcp_ptr->NEG.DNS); outp += 4;
      } /* Endif */
   } /* Endif */

   return totlen;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_recvconfack
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_Recvconfack.  Parses a ConfAck packet.
*     This function should not modify any state if the ack is bad.
*
*END*-----------------------------------------------------------------*/

static boolean IPCP_recvconfack
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = ((IP_IF_PTR)fsm->PRIVATE)->HANDLE;
   uchar_ptr            inp = fsm->DATA;
   uint_32              sizeleft = fsm->LENGTH;
   IPCP_OPT             ack_opt = ipcp_ptr->LOPT;

#define IPCP_RACK(ci,len) \
      if (sizeleft < len) goto badack;           \
      if (*inp++ != IPCP_CI_ ## ci) goto badack; \
      if (*inp++ != len)            goto badack; \
      sizeleft -= len

   /*
   ** The ack must be identical to the last ConfReq we sent
   */

   if (ipcp_ptr->NEG.NEG_ADDR) {
      IPCP_RACK(ADDR,6);
      ack_opt.ADDR = ntohl(inp); inp += 4;
      if (ack_opt.ADDR != ipcp_ptr->NEG.ADDR) goto badack;
   } /* Endif */

   if (ipcp_ptr->NEG.NEG_DNS) {
      IPCP_RACK(DNS,6);
      ack_opt.DNS = ntohl(inp); inp += 4;
      if (ack_opt.DNS != ipcp_ptr->NEG.DNS) goto badack;
   } /* Endif */

   if (sizeleft) goto badack;

   if (fsm->STATE < PPP_STATE_OPENED) {
      ipcp_ptr->LOPT = ack_opt;
   } /* Endif */
   return TRUE;

badack:
   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_recvconfnak
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_Recvconfnak.  Parses a ConfNak packet.
*     This function should not modify any state if the nak is bad.
*
*END*-----------------------------------------------------------------*/

static boolean IPCP_recvconfnak
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = ((IP_IF_PTR)fsm->PRIVATE)->HANDLE;
   uchar_ptr            inp = fsm->DATA;
   uint_32              sizeleft = fsm->LENGTH;
   IPCP_NEG             req_neg = ipcp_ptr->NEG;
   uchar                code;

#define IPCP_RNAK(len) \
      if (sizeleft < len) goto badnak; \
      if (*inp++ != len)  goto badnak; \
      sizeleft -= len

   /*
   ** Nak'd codes must be in the same order as they were in the ConfReq
   */

   code = *inp++;

   if (sizeleft && req_neg.NEG_ADDR && code == IPCP_CI_ADDR) {
      IPCP_RNAK(6);

      /* Accept any address they want if we don't know who we are */
      if (ipcp_ptr->INIT.ACCEPT_LOCAL_ADDR) {
         req_neg.ADDR = ntohl(inp);
      } /* Endif */
      inp += 4;

      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_DNS && code == IPCP_CI_DNS) {
      IPCP_RNAK(6);

      /* Accept any address they want if we don't know who we are */
      if (ipcp_ptr->INIT.ACCEPT_LOCAL_DNS) {
         req_neg.DNS = ntohl(inp);
      } /* Endif */
      inp += 4;

      code = *inp++;
   } /* Endif */

   /*
   ** There may be remaining codes if the peer wants us to
   ** negotiate an option we didn't include.
   */

#define IPCP_RNAK_ADD(ci,len) \
         if (req_neg.NEG_ ## ci || sizeleft < len || *inp++ != len) goto badnak; \
         sizeleft -= len

   while (sizeleft) {
      switch (code) {
      case IPCP_CI_ADDR:
         IPCP_RNAK_ADD(ADDR,6);
         req_neg.NEG_ADDR = 1;
         if (ipcp_ptr->INIT.ACCEPT_LOCAL_ADDR) {
            req_neg.ADDR = ntohl(inp);
         } /* Endif */
         inp += 4;
         break;
      case IPCP_CI_DNS:
         IPCP_RNAK_ADD(DNS,6);
         if (ipcp_ptr->INIT.NEG_LOCAL_DNS) {
            req_neg.NEG_DNS = 1;
            if (ipcp_ptr->INIT.ACCEPT_LOCAL_DNS) {
               req_neg.DNS = ntohl(inp);
            } /* Endif */
         } /* Endif */
         inp += 4;
         break;
      default:
         if (sizeleft < *inp || *inp < 2) goto badnak;
         sizeleft -= *inp;
         inp += *inp - 1;
      } /* Endswtich */
      code = *inp++;
   } /* Endwhile */

   if (fsm->STATE < PPP_STATE_OPENED) {
      ipcp_ptr->NEG = req_neg;
   } /* Endif */
   return TRUE;

badnak:
   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_recvconfrej
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_Recvconfnak.  Parses a ConfRej packet.
*     This function should not modify any state if the rej is bad.
*
*END*-----------------------------------------------------------------*/

static boolean IPCP_recvconfrej
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = ((IP_IF_PTR)fsm->PRIVATE)->HANDLE;
   uchar_ptr            inp = fsm->DATA;
   uint_32              sizeleft = fsm->LENGTH;
   IPCP_OPT             req_opt = ipcp_ptr->LOPT;
   IPCP_NEG             req_neg = ipcp_ptr->NEG;
   uchar                code;

#define IPCP_RREJ(ci,len) \
      if (sizeleft < len) goto badrej; \
      if (*inp++ != len)  goto badrej; \
      sizeleft -= len;                 \
      req_neg.NEG_ ## ci = 0

      /*
      ** Rej'd codes must be in the same order as they were in the ConfReq
      */

   code = *inp++;

   if (sizeleft && req_neg.NEG_ADDR && code == IPCP_CI_ADDR) {
      IPCP_RREJ(ADDR,6);
      if (req_neg.ADDR != ntohl(inp)) goto badrej; inp += 4;
      req_opt.ADDR = ipcp_ptr->INIT.LOCAL_ADDR;
      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_DNS && code == IPCP_CI_DNS) {
      IPCP_RREJ(DNS,6);
      if (req_neg.DNS != ntohl(inp)) goto badrej; inp += 4;
      req_opt.DNS = ipcp_ptr->INIT.LOCAL_DNS;
      code = *inp++;
   } /* Endif */

   if (sizeleft) goto badrej;

   if (fsm->STATE < PPP_STATE_OPENED) {
      ipcp_ptr->LOPT = req_opt;
      ipcp_ptr->NEG  = req_neg;
   } /* Endif */
   return TRUE;

badrej:
   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_recvconfreq
* Returned Value  :  number of bytes written
* Comments        :
*     Called by PPPFSM_Recvconfreq.  Parses a ConfReq packet.
*     Builds a ConfAck/ConfNak/ConfRej packet out of fsm->PACKET,
*     and sets fsm->CODE appropriately.
*
*END*-----------------------------------------------------------------*/

static uint_32 IPCP_recvconfreq
   (
      PPPFSM_CFG_PTR    fsm,
            /* [IN] - State Machine */
      boolean           reject
            /* [IN] - whether to ConfRej if we disagree */
   )
{ /* Body */
   IPCP_CFG_STRUCT_PTR  ipcp_ptr = ((IP_IF_PTR)fsm->PRIVATE)->HANDLE;
   uchar_ptr            inp = fsm->DATA;
   uchar_ptr            nakp, rejp;
   uint_32              sizeleft = fsm->LENGTH;
   uint_32              naklen, rejlen;
   IPCP_OPT             req_opt = ipcp_ptr->POPT;
   uchar                code;
   uchar                cicode, cilen;

#define CI_REJECT(n)                   \
   inp -= n;                           \
   code = CP_CODE_CONF_REJ;            \
   rejlen += cilen;                    \
   while (cilen--) *rejp++ = *inp++

#define CI_NAK                               \
   if (code != CP_CODE_CONF_REJ) {           \
      code = CP_CODE_CONF_NAK;               \
      if (reject) {                          \
         inp -= cilen;                       \
         naklen += cilen;                    \
         while (cilen--) *nakp++ = *inp++;   \
      } else

#define CI_ENDNAK                            \
   } /* Endif */

      /*
      ** Process all requested codes
      */

   rejp = nakp = inp;
   rejlen = naklen = 0;
   code = CP_CODE_CONF_ACK;

   while (sizeleft) {
         /* check remaining length */
      if (sizeleft < inp[1] || inp[1] < 2) {
         code = CP_CODE_CONF_REJ;
         rejlen += sizeleft;
         while (sizeleft--) *rejp++ = *inp++;
         break;
      } /* Endif */

      cicode = *inp++;
      sizeleft -= cilen = *inp++;

      switch (cicode) {
      case IPCP_CI_ADDR:
         if (cilen != 6) {
            CI_REJECT(2);
            break;
         } /* Endif */

         /* Accept their address if we don't know who they are */
         req_opt.ADDR = ntohl(inp); inp += 4;
         if (!ipcp_ptr->INIT.ACCEPT_REMOTE_ADDR &&
              ipcp_ptr->INIT.REMOTE_ADDR != req_opt.ADDR) {
            CI_NAK {
               *nakp++ = IPCP_CI_ADDR;
               naklen += *nakp++ = 6;
               htonl(nakp, ipcp_ptr->INIT.REMOTE_ADDR); nakp += 4;
            } CI_ENDNAK
         } /* Endif */
         break;

      case IPCP_CI_DNS:
         if (cilen != 6) {
            CI_REJECT(2);
            break;
         } /* Endif */

         /* Accept their address if we don't know who they are */
         req_opt.DNS = ntohl(inp); inp += 4;
         if (ipcp_ptr->INIT.NEG_REMOTE_DNS) {
            if (!ipcp_ptr->INIT.ACCEPT_REMOTE_DNS &&
                 ipcp_ptr->INIT.REMOTE_ADDR != req_opt.DNS) {
               CI_NAK {
                  *nakp++ = IPCP_CI_DNS;
                  naklen += *nakp++ = 6;
                  htonl(nakp, ipcp_ptr->INIT.REMOTE_DNS); nakp += 4;
               } CI_ENDNAK
            } /* Endif */
         } else {
            CI_REJECT(6);
         } /* Endif */
         break;

      default:
         CI_REJECT(2);
         break;
      } /* Endswitch */
   } /* Endwhile */

   /*
   ** If we wanted to send additional naks for unsent codes, they
   ** would go here, at *nakp.
   */

   if (code == CP_CODE_CONF_ACK) {
      ipcp_ptr->POPT = req_opt;
      sizeleft = fsm->LENGTH;
   } else if (code == CP_CODE_CONF_REJ) {
      sizeleft = rejlen;
   } else if (reject) {
      code = CP_CODE_CONF_REJ;
      sizeleft = naklen;
   } else { /* if code == NAK */
      sizeleft = naklen;
   } /* Endif */

   fsm->CODE = code;
   return sizeleft;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  IPCP_recvcoderej
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_Recvcoderej.  Returns TRUE if the rejected
*     code is catastrophic.
*
*END*-----------------------------------------------------------------*/

static boolean IPCP_recvcoderej
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   uchar code = ntohc(fsm->DATA);
   return (code >= 1 && code <= 7);
} /* Endbody */

#endif  /* RTCSCFG_ENABLE_IP4 */

/* EOF */
