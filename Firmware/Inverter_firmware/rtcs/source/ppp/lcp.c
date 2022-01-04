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
* $FileName: lcp.c$
* $Version : 3.8.10.0$
* $Date    : Aug-27-2012$
*
* Comments:
*
*   This file contains the implementation for the
*   Link Control Protocol.
*
*END************************************************************************/

/*
** lcp.c - PPP Link Control Protocol.
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

/*
** Things to do:
**
** We need a way to let the application specify which configuration
** options to request (in order of preference), and which options to
** allow the peer (unordered set).
**
** Currently, the following are hard coded for both sides:
**
**       MRU <= 1500            (1500 preferred, Receive side)
**       MRU >= 68              (Send side)
**       ACCM = _PPP_ACCM
**       PFC = TRUE or FALSE    (TRUE preferred)
**       ACFC = TRUE or FALSE   (TRUE preferred)
**       FCS = CCITT-32, CCITT-16 or NULL
**                              (CCITT-16 preferred)
**
** The choice of authentication protocols depends on the
** existence of secrets.  The following are supported
**
**       AP = none or PAP
**
*/

#include <ppp.h>
#include "ppp_prv.h"

#if RTCSCFG_ENABLE_IP4

static boolean LCP_up(PPPFSM_CFG_PTR);
static void    LCP_down(PPPFSM_CFG_PTR);

static void    LCP_resetconfreq(PPPFSM_CFG_PTR);
static uint_32 LCP_buildconfreq(PPPFSM_CFG_PTR, uchar_ptr, uint_32);

static uint_32 LCP_recvconfreq(PPPFSM_CFG_PTR, boolean);
static boolean LCP_recvconfack(PPPFSM_CFG_PTR);
static boolean LCP_recvconfnak(PPPFSM_CFG_PTR);
static boolean LCP_recvconfrej(PPPFSM_CFG_PTR);
static boolean LCP_recvcoderej(PPPFSM_CFG_PTR);
static boolean LCP_recvextcode(PPPFSM_CFG_PTR);

static PPPFSM_CALL LCP_CALL = {
   /* PROTOCOL    */    PPP_PROT_LCP,
   /* linkup()    */    LCP_up,
   /* linkdown()  */    LCP_down,
   /* resetreq()  */    LCP_resetconfreq,
   /* buildreq()  */    LCP_buildconfreq,
   /* recvreq()   */    LCP_recvconfreq,
   /* recvack()   */    LCP_recvconfack,
   /* recvnak()   */    LCP_recvconfnak,
   /* recvrej()   */    LCP_recvconfrej,
   /* testcode()  */    LCP_recvcoderej,
   /* recvcode()  */    LCP_recvextcode
};

static LCP_NEG LCP_DEFAULT_NEG = {
   /* NEG_* */    0,1,0,0,0,1,1,0,
   /* MRU   */    DEFAULT_MRU,
   /* ACCM  */    0xFFFFFFFFl,
   /* AP    */    0
};

static LCP_NEG LCP_DEFAULT_NEG_PPPOE = {
   /* NEG_* */    1,0,0,0,0,0,0,0,
   /* MRU   */    1492,
   /* ACCM  */    0,
   /* AP    */    0
};

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_init
* Returned Value  :  error code
* Comments        :
*     Called by PPP_init.  Initializes the LCP Finite State Machine.
*
*END*-----------------------------------------------------------------*/

uint_32 LCP_init
   (
      _ppp_handle    handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR ppp_ptr = handle;

   ppp_ptr->LCP_STATE.DEVICE = ppp_ptr->DEVICE;
/* Start CR 2207 */
#if PPP_SECRETS_NOT_SHARED
   ppp_ptr->LCP_STATE.HANDLE = handle;
#endif
/* End CR 2207 */
      /* Initialize the FSM */
   return PPPFSM_init(&ppp_ptr->LCP_FSM, handle, &LCP_CALL, &ppp_ptr->LCP_STATE);

#else
   
   return RTCSERR_IP_IS_DISABLED;
   
#endif /* RTCSCFG_ENABLE_IP4 */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_destroy
* Returned Value  :  error code
* Comments        :
*     Called by PPP_init.  Destroys the LCP Finite State Machine.
*     Assumes the Finite State Machine has been initialized.
*
*END*-----------------------------------------------------------------*/

uint_32 LCP_destroy
   (
      _ppp_handle    handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR ppp_ptr = handle;

      /* Destroy the FSM */
   return PPPFSM_destroy(&ppp_ptr->LCP_FSM);

#else
   
   return RTCSERR_IP_IS_DISABLED;
   
#endif /* RTCSCFG_ENABLE_IP4 */   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_lowerup
* Returned Value  :  void
* Comments        :
*     Called by application.  Lower layer is up.
*
*END*-----------------------------------------------------------------*/

void PPP_lowerup
   (
      _ppp_handle    handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR ppp_ptr = handle;

   PPPFSM_lowerup(&ppp_ptr->LCP_FSM);

#endif /* RTCSCFG_ENABLE_IP4 */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_lowerdown
* Returned Value  :  void
* Comments        :
*     Called by application.  Lower layer is down.
*
*END*-----------------------------------------------------------------*/

void PPP_lowerdown
   (
      _ppp_handle    handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR ppp_ptr = handle;

   PPPFSM_lowerdown(&ppp_ptr->LCP_FSM);

#endif /* RTCSCFG_ENABLE_IP4 */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_open
* Returned Value  :  error code
* Comments        :
*     Called by application.  Begin negotiating PPP.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_open
   (
      _ppp_handle    handle,
            /* [IN] - the PPP state structure */
      uint_32        options
            /* [IN] - Startup options */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR ppp_ptr = handle;

      /* Do some error checking */
   if (ppp_ptr->VALID != PPP_VALID) {
      return RTCSERR_PPP_INVALID_HANDLE;
   } /* Endif */

   PPPFSM_open(&ppp_ptr->LCP_FSM, options);
   return PPP_OK;

#else

    return RTCSERR_IP_IS_DISABLED;    

#endif /* RTCSCFG_ENABLE_IP4 */  

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_close
* Returned Value  :  error code
* Comments        :
*     Called by application.  Begin closing PPP.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_close
   (
      _ppp_handle    handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR ppp_ptr = handle;

      /* Do some error checking */
   if (ppp_ptr->VALID != PPP_VALID) {
      return RTCSERR_PPP_INVALID_HANDLE;
   } /* Endif */

   PPPFSM_close(&ppp_ptr->LCP_FSM);
   return PPP_OK;

#else

    return RTCSERR_IP_IS_DISABLED;    

#endif /* RTCSCFG_ENABLE_IP4 */  

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_recvextcode
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_Input.  Parses an LCP packet with an extended
*     code.  Consumes fsm->PACKET if TRUE is returned.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean LCP_recvextcode
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = fsm->HANDLE;
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   PPP_CALL_INTERNAL_PTR call_ptr = &ppp_ptr->LCP_CALL[PPP_CALL_ECHO_REPLY];

   switch (fsm->CODE) {
   case LCP_CODE_PROT_REJ:
      lcp_ptr->ST_LCP_RX_REJECT++;

      /*
      ** We should notify the rejected protocol, but there
      ** currently isn't any way to do this.  What we'll do
      ** instead is to send a STOP message to the Tx task on
      ** behalf of the protocol.
      **
      ** This will usually be sufficient, since the rejected
      ** protocol isn't likely to send another packet until
      ** either it receives a packet (in which case it's OK to
      ** reply) or the retransmission timer kicks in (which
      ** we're about to disable).
      **
      ** Thus, this will effectively stop the rejected protocol.
      */

      if (fsm->LENGTH >= 2) {
         uint_16 protocol = ntohs(fsm->DATA);
         PPP_send_stop(ppp_ptr, protocol);
      } /* Endif */
      PCB_free(fsm->PACKET);
      break;

   case LCP_CODE_ECHO_REQ:
      lcp_ptr->ST_LCP_RX_ECHO++;
      if (fsm->STATE < PPP_STATE_OPENED) {
         PCB_free(fsm->PACKET);
      } else {
         htonc(fsm->DATA - CP_HDR_LEN, LCP_CODE_ECHO_REP);
         htonl(fsm->DATA, 0);                      /* Set magic field to 0 */
         lcp_ptr->ST_LCP_TX_REPLY++;
         PPP_send_one(ppp_ptr, PPP_PROT_LCP, fsm->PACKET);
      } /* Endif */
      break;

   case LCP_CODE_ECHO_REP:
      lcp_ptr->ST_LCP_RX_REPLY++;
      if (call_ptr->CALLBACK) {
         call_ptr->CALLBACK(call_ptr->PARAM, fsm->ID, fsm->PACKET);
      } /* Endif */
      PCB_free(fsm->PACKET);
      break;

   case LCP_CODE_DISC_REQ:
      lcp_ptr->ST_LCP_RX_DISCARD++;
      PCB_free(fsm->PACKET);
      break;

   default:
      return FALSE;
   } /* Endswitch */

   return TRUE;
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_sendprotrej
* Returned Value  :  void
* Comments        :
*     Called by PPP_rx_task.  Sends a ProtRej packet for some protocol.
*     Consumes (sends) 'pcb'.
*
*END*-----------------------------------------------------------------*/

void LCP_sendprotrej
   (
      PCB_PTR           pcb,
            /* [IN] - packet to reject */
      PPPFSM_CFG_PTR    fsm
            /* [IN/OUT] - State Machine */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR ppp_ptr = fsm->HANDLE;
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   uchar_ptr   packet, src, dst;
   uint_16     length;
   uint_16     size = ppp_ptr->SEND_OPTIONS->MRU - CP_HDR_LEN;

   length = pcb->FRAG[0].LENGTH;
   packet = pcb->FRAG[0].FRAGMENT;

   /* Truncate the packet if necessary */
   if (length > size) {
      length = size;
   } /* Endif */

   /* Make room for a Code-Reject header */
   size = length + CP_HDR_LEN;

   /* Copy the received packet (including protocol field) to the */
   /* data portion of the Protocol-Reject                        */
   src = packet + length;
   dst = src + 2 + CP_HDR_LEN;
   while (length--) *--dst = *--src;

   /* Build a Code-Reject packet */
   htons(packet, PPP_PROT_LCP); packet += 2;
   *packet++ = LCP_CODE_PROT_REJ;
   *packet++ = fsm->CURID;
   fsm->CURID = (fsm->CURID + 1) & 0xFF;
   htons(packet, size); packet += 2;
   pcb->FRAG[0].LENGTH = size + 2;

   lcp_ptr->ST_LCP_TX_REJECT++;
   PPP_send_one(ppp_ptr, PPP_PROT_LCP, pcb);

#else

    PCB_free(pcb);

#endif /* RTCSCFG_ENABLE_IP4  */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_resetconfreq
* Returned Value  :  void
* Comments        :
*     Called by PPPFSM_Sendconfreq when starting negotiation.
*     Initializes the Send Options.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void LCP_resetconfreq
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   uint_32     if_type;
/* Start CR 2207 */
#if PPP_SECRETS_NOT_SHARED
   PPP_CFG_PTR    ppp_ptr = lcp_ptr->HANDLE;
#endif
/* End CR 2207 */

   _iopcb_ioctl(lcp_ptr->DEVICE, IOPCB_IOCTL_GET_IFTYPE, &if_type);
   if (if_type == IPIFTYPE_ETHERNET) {
      lcp_ptr->SEND_OPT = PPP_DEFAULT_OPTIONS_PPPOE;
      lcp_ptr->RECV_OPT = PPP_DEFAULT_OPTIONS_PPPOE;
      lcp_ptr->RECV_NEG = LCP_DEFAULT_NEG_PPPOE;
   } else {
      lcp_ptr->SEND_OPT = PPP_DEFAULT_OPTIONS;
      lcp_ptr->RECV_OPT = PPP_DEFAULT_OPTIONS;
      lcp_ptr->RECV_NEG = LCP_DEFAULT_NEG;
      lcp_ptr->RECV_NEG.ACCM = _PPP_ACCM;
   } /* Endif */

/* Start CR 2207 */
   if (PPP_SECRET(ppp_ptr,_PPP_CHAP_RSECRETS)) {
/* End CR 2207 */
      lcp_ptr->RECV_NEG.NEG_AP = 1;
      lcp_ptr->RECV_NEG.AP = PPP_PROT_CHAP;
/* Start CR 2207 */
   } else if (PPP_SECRET(ppp_ptr,_PPP_PAP_RSECRETS)) {
/* End CR 2207 */
      lcp_ptr->RECV_NEG.NEG_AP = 1;
      lcp_ptr->RECV_NEG.AP = PPP_PROT_PAP;
   } else {
      lcp_ptr->RECV_NEG.NEG_AP = 0;
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_buildconfreq
* Returned Value  :  number of bytes written
* Comments        :
*     Called by PPPFSM_Sendconfreq.  Builds a ConfReq packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 LCP_buildconfreq
   (
      PPPFSM_CFG_PTR    fsm,
            /* [IN] - State Machine */
      uchar_ptr         outp,
            /* [IN] - free packet */
      uint_32           sizeleft
            /* [IN] - size of packet */
   )
{ /* Body */
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   uint_32     totlen = 0;
   uint_32     aplen;

#define LCP_BREQ(ci,len) \
         *outp++ = LCP_CI_ ## ci; \
         *outp++ = len;           \
         totlen += len

   /*
   ** Generate configuration information for each option
   ** we want to negotiate.
   */

   if (lcp_ptr->RECV_NEG.NEG_MRU) {
      if (sizeleft >= totlen+4) {
         LCP_BREQ(MRU,4);
         htons(outp, lcp_ptr->RECV_NEG.MRU); outp += 2;
      } /* Endif */
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_ACCM) {
      if (sizeleft >= totlen+6) {
         LCP_BREQ(ACCM,6);
         htonl(outp, lcp_ptr->RECV_NEG.ACCM); outp += 4;
      } /* Endif */
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_AP) {
      switch (lcp_ptr->RECV_NEG.AP) {
      case PPP_PROT_CHAP: aplen = 5; break;
      default:            aplen = 4; break;
      } /* Endswitch */
      if (sizeleft >= totlen+aplen) {
         LCP_BREQ(AP,aplen);
         htons(outp, lcp_ptr->RECV_NEG.AP); outp += 2;
         switch (lcp_ptr->RECV_NEG.AP) {
         case PPP_PROT_CHAP:
            htonc(outp, 5); outp++;          /* Only MD5 supported */
            break;
         } /* Endswitch */
      } /* Endif */
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_PFC) {
      if (sizeleft >= totlen+2) {
         LCP_BREQ(PFC,2);
      } /* Endif */
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_ACFC) {
      if (sizeleft >= totlen+2) {
         LCP_BREQ(ACFC,2);
      } /* Endif */
   } /* Endif */

   return totlen;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_recvconfack
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_Recvconfack.  Parses a ConfAck packet.
*     This function should not modify any state if the ack is bad.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean LCP_recvconfack
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   uchar_ptr   inp = fsm->DATA;
   uint_32     sizeleft = fsm->LENGTH;
   uint_32     aplen;
   PPP_OPT     ack_opt = lcp_ptr->RECV_OPT;

#define LCP_RACK(ci,len) \
      if (sizeleft < len) goto badack;          \
      if (*inp++ != LCP_CI_ ## ci) goto badack; \
      if (*inp++ != len)           goto badack; \
      sizeleft -= len


   /*
   ** The ack must be identical to the last ConfReq we sent
   */
    /*
    ** This situation can happend when size of data in our request packet is "0"
    ** So, size of received data in "ack" packet will be "0" too.
    ** As example: We send "REQ" packet with field PFC an ACFC "ON", modem send "NAK" 
    ** for that field. We turn of that field, but we have not what negotiate else,
    ** so we send "REQ" packed,it looks like "7E FF 03 C0 21 01 6D 00 04 3F 15 7E"
    ** As you see size of data field is "0". But modem agree and send us "ACK" looks like
    ** "7E FF 03 C0 21 02 6D 00 04 F2 30 7E" and will be ready to start next stage of communication.
    ** In received packet from modem you can se size of data field is "0" too.
    */
    if(sizeleft ==0)
    {
        return TRUE; 
    }
    

   if (lcp_ptr->RECV_NEG.NEG_MRU) {
      LCP_RACK(MRU,4);
      ack_opt.MRU = ntohs(inp); inp += 2;
      if (ack_opt.MRU != lcp_ptr->RECV_NEG.MRU) goto badack;
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_ACCM) {
      LCP_RACK(ACCM,6);
      ack_opt.ACCM = ntohl(inp); inp += 4;
      if (ack_opt.ACCM != lcp_ptr->RECV_NEG.ACCM) goto badack;
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_AP) {
      switch (lcp_ptr->RECV_NEG.AP) {
      case PPP_PROT_CHAP: aplen = 5; break;
      default:            aplen = 4; break;
      } /* Endswitch */
      LCP_RACK(AP,aplen);
      ack_opt.AP = ntohs(inp); inp += 2;
      if (ack_opt.AP != lcp_ptr->RECV_NEG.AP) goto badack;
      switch (ack_opt.AP) {
      case PPP_PROT_CHAP:
         if (ntohc(inp) != 5) goto badack;   /* Only MD5 supported */
         inp++;
         ack_opt.AP_Start = CHAP_challenge;
         break;
      case PPP_PROT_PAP:
         ack_opt.AP_Start = PAP_open;
         break;
      default:
         ack_opt.AP = 0;
         break;
      } /* Endswitch */
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_PFC) {
      LCP_RACK(PFC,2);
      ack_opt.PFC = TRUE;
   } /* Endif */

   if (lcp_ptr->RECV_NEG.NEG_ACFC) {
      LCP_RACK(ACFC,2);
      ack_opt.ACFC = TRUE;
   } /* Endif */

   if (sizeleft) goto badack;

   if (fsm->STATE < PPP_STATE_OPENED) {
      lcp_ptr->RECV_OPT = ack_opt;
   } /* Endif */
   return TRUE;

badack:
   return FALSE;

} /* Endbody */


#endif /* RTCSCFG_ENABLE_IP4 */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_recvconfnak
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_recvconfnak.  Parses a ConfNak packet.
*     This function should not modify any state if the nak is bad.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean LCP_recvconfnak
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   uchar_ptr   inp = fsm->DATA;
   uint_32     sizeleft = fsm->LENGTH;
   LCP_NEG     req_neg = lcp_ptr->RECV_NEG;
   uchar       code;
/* Start CR 2207 */
#if PPP_SECRETS_NOT_SHARED
   PPP_CFG_PTR    ppp_ptr = lcp_ptr->HANDLE;
#endif
/* End CR 2207 */

#define LCP_RNAK(len) \
      if (sizeleft < len) goto badnak; \
      if (*inp++ != len)  goto badnak; \
      sizeleft -= len

   /*
   ** Nak'd codes must be in the same order as they were in the ConfReq
   */

   code = *inp++;

   if (sizeleft && req_neg.NEG_MRU && code == LCP_CI_MRU) {
      LCP_RNAK(4);

         /* We are prepared to accept maximum MRU of 1500 */
      req_neg.MRU = ntohs(inp); inp += 2;
      if (req_neg.MRU > DEFAULT_MRU) {
         req_neg.MRU = DEFAULT_MRU;
      } /* Endif */

      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_ACCM && code == LCP_CI_ACCM) {
      LCP_RNAK(6);

         /* Add any characters they want to our ACCM */
      req_neg.ACCM |= ntohl(inp); inp += 4;

      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_AP && code == LCP_CI_AP) {
      if (sizeleft < *inp || *inp < 4) goto badnak;
      sizeleft -= *inp;

         /*
         ** If AP is nak'd, we don't care what they want --
         ** we choose the next most desirable protocol.  If there
         ** aren't any more to choose, we keep the last one we tried.
         */
      inp += *inp - 1;
      
      if (PPP_SECRET(ppp_ptr,_PPP_CHAP_RSECRETS)) {
          req_neg.AP = PPP_PROT_CHAP;
      } else {
        switch (req_neg.AP) {
        case PPP_PROT_CHAP:
             /* CHAP was NAKed, try PAP */ 
             if (PPP_SECRET(ppp_ptr,_PPP_PAP_RSECRETS)) {
                req_neg.AP = PPP_PROT_PAP;            
             } /* Endif */
             break;
        
        case PPP_PROT_PAP:
           /* no other authentication protocols known */
           break;
           
        default:
            /* Unknown protocol NAKed, try CHAP */
            if (PPP_SECRET(ppp_ptr,_PPP_CHAP_RSECRETS)) {
                req_neg.AP = PPP_PROT_CHAP;
            }
            break;

        } /* Endswitch */
      }
      code = *inp++;
   } /* Endif */

   /*
   ** If we advertised PFC and/or ACFC and the peer doesn't
   ** want it, they should send a ConfRej rather than a ConfNak,
   ** so we're not going to check for them here (if they are
   ** here, the switch statement below will catch it).
   */


#define LCP_RNAK_ADD(ci,len) \
         if (req_neg.NEG_ ## ci || sizeleft < len || *inp++ != len) goto badnak; \
         sizeleft -= len;                                                        \
         req_neg.NEG_ ## ci = 1

   /*
   ** There may be remaining codes if the peer wants us to
   ** negotiate an option we didn't include.
   */

   while (sizeleft) {
      switch (code) {
      case LCP_CI_MRU:
         LCP_RNAK_ADD(MRU,4);
         req_neg.MRU = ntohs(inp); inp += 2;
         if (req_neg.MRU > DEFAULT_MRU) {
            req_neg.MRU = DEFAULT_MRU;
         } /* Endif */
         break;
      case LCP_CI_ACCM:
         LCP_RNAK_ADD(ACCM,6);
         req_neg.ACCM |= ntohl(inp); inp += 4;
         break;
      case LCP_CI_AP:
         if (req_neg.NEG_AP || sizeleft < *inp || *inp < 4) goto badnak;
         sizeleft -= *inp;
            /*
            ** If AP is nak'd, we don't care what they want --
            ** we still don't negotiate
            */
         inp += *inp - 1;
         break;
      case LCP_CI_PFC:
         LCP_RNAK_ADD(PFC,2);
         break;
      case LCP_CI_ACFC:
         LCP_RNAK_ADD(ACFC,2);
         break;
      default:
         if (sizeleft < *inp || *inp < 2) goto badnak;
         sizeleft -= *inp;
         inp += *inp - 1;
      } /* Endswtich */
      code = *inp++;
   } /* Endwhile */

   if (fsm->STATE < PPP_STATE_OPENED) {
      lcp_ptr->RECV_NEG = req_neg;
   } /* Endif */
   return TRUE;

badnak:
   return FALSE;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_recvconfrej
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_recvconfnak.  Parses a ConfRej packet.
*     This function should not modify any state if the rej is bad.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean LCP_recvconfrej
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   uchar_ptr   inp = fsm->DATA;
   uint_32     sizeleft = fsm->LENGTH;
   uint_32     aplen;
   PPP_OPT     req_opt = lcp_ptr->RECV_OPT;
   LCP_NEG     req_neg = lcp_ptr->RECV_NEG;
   uchar       code;

#define LCP_RREJ(ci,len) \
      if (sizeleft < len) goto badrej; \
      if (*inp++ != len)  goto badrej; \
      sizeleft -= len;                 \
      req_neg.NEG_ ## ci = 0;          \
      req_opt.ci = PPP_DEFAULT_OPTIONS.ci

   /*
   ** Rej'd codes must be in the same order as they were in the ConfReq
   */

   code = *inp++;

   if (sizeleft && req_neg.NEG_MRU && (code == LCP_CI_MRU)) {
      LCP_RREJ(MRU,4);
      if (req_neg.MRU != ntohs(inp)) goto badrej; inp += 2;
      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_ACCM && (code == LCP_CI_ACCM)) {
      LCP_RREJ(ACCM,6);
      if (req_neg.ACCM != ntohl(inp)) goto badrej; inp += 4;
      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_AP && (code == LCP_CI_AP)) {
      switch (req_neg.AP) {
      case PPP_PROT_CHAP: aplen = 5; break;
      default:            aplen = 4; break;
      } /* Endswitch */
      LCP_RREJ(AP,aplen);
      if (req_neg.AP != ntohs(inp)) goto badrej; inp += 2;
      switch (req_neg.AP) {
      case PPP_PROT_CHAP:
         if (5 != ntohc(inp)) goto badrej;   /* Only MD5 supported */
         inp++;
         break;
      } /* Endswitch */
      req_opt.AP_Start = PPP_DEFAULT_OPTIONS.AP_Start;
      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_PFC && (code == LCP_CI_PFC)) {
      LCP_RREJ(PFC,2);
      code = *inp++;
   } /* Endif */

   if (sizeleft && req_neg.NEG_ACFC && (code == LCP_CI_ACFC)) {
      LCP_RREJ(ACFC,2);
      code = *inp++;
   } /* Endif */

   if (sizeleft) goto badrej;

   if (fsm->STATE < PPP_STATE_OPENED) {
      lcp_ptr->RECV_OPT = req_opt;
      lcp_ptr->RECV_NEG = req_neg;
   } /* Endif */
   return TRUE;

badrej:
   return FALSE;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_recvconfreq
* Returned Value  :  number of bytes written
* Comments        :
*     Called by PPPFSM_recvconfreq.  Parses a ConfReq packet.
*     Builds a ConfAck/ConfNak/ConfRej packet out of fsm->PACKET,
*     and sets fsm->CODE appropriately.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 LCP_recvconfreq
   (
      PPPFSM_CFG_PTR    fsm,
            /* [IN] - State Machine */
      boolean           reject
            /* [IN] - whether to ConfRej if we disagree */
   )
{ /* Body */
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   uchar_ptr   inp = fsm->DATA;
   uchar_ptr   nakp, rejp;
   uint_32     sizeleft = fsm->LENGTH;
   uint_32     naklen, rejlen;
   boolean     apneg = FALSE;
   PPP_OPT     req_opt = lcp_ptr->SEND_OPT;
   uchar       code;
   uchar       cicode, cilen;
/* Start CR 2207 */
#if PPP_SECRETS_NOT_SHARED
   PPP_CFG_PTR    ppp_ptr = lcp_ptr->HANDLE;
#endif
/* End CR 2207 */

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
      } else {                               \
         apneg = FALSE;

#define CI_ENDNAK                            \
      } /* Endif */                          \
   } /* Endif */

#define CI_NAKAP(n)                          \
   if (code != CP_CODE_CONF_REJ) {           \
      code = CP_CODE_CONF_NAK;               \
      if (reject) {                          \
         inp -= n;                           \
         naklen += cilen;                    \
         while (cilen--) *nakp++ = *inp++;   \
      } else {                               \
         apneg = TRUE;                       \
         inp += cilen - n;                   \
      } /* Endif */                          \
   } else {                                  \
      inp += cilen - n;                      \
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
      case LCP_CI_MRU:
         if (cilen != 4) {
            CI_REJECT(2);
            break;
         } /* Endif */

            /* Accept any MRU no smaller than 68 bytes */
         req_opt.MRU = ntohs(inp); inp += 2;
         if (req_opt.MRU > DEFAULT_MRU) {
            req_opt.MRU = DEFAULT_MRU;
         } else if (req_opt.MRU < MINIMUM_MRU) {
            CI_NAK {
               *nakp++ = LCP_CI_MRU;
               naklen += *nakp++ = 4;
               req_opt.MRU = MINIMUM_MRU;
               htons(nakp, req_opt.MRU); nakp += 2;
            } CI_ENDNAK;
         } /* Endif */
         break;

      case LCP_CI_ACCM:
         if (cilen != 6) {
            CI_REJECT(2);
            break;
         } /* Endif */

            /* If we want any characters not in their ACCM, nak it */
         req_opt.ACCM = ntohl(inp); inp += 4;
         if ((req_opt.ACCM & _PPP_ACCM) != _PPP_ACCM) {
            CI_NAK {
               *nakp++ = LCP_CI_ACCM;
               naklen += *nakp++ = 6;
               req_opt.ACCM |= _PPP_ACCM;
               htonl(nakp, req_opt.ACCM); nakp += 4;
            } CI_ENDNAK;
         } /* Endif */
         break;

         /*
         ** AP is unusual in that it is a variable length
         ** option.  Thus it is possible that we may want
         ** to nak AP and make a suggestion longer than
         ** the peer's request.
         **
         ** This is bad.
         **
         ** It's bad because this function wants to reuse
         ** the incoming ConfReq to build the reply, and
         ** in this case, it is possible for nakp to
         ** overtake inp, thus overwriting an option not
         ** yet parsed.
         **
         ** So we do the following:  If we decide to nak
         ** the requested AP, we just set apneg=TRUE and
         ** append the nak'd AP at the end of our ConfNak
         ** after we've completed parsing the ConfReq.
         **
         ** Note that we won't nak AP if we generate
         ** nak's for other options afterward, because
         ** the RFC states that nak'd options MUST be in
         ** the same order that they were in the ConfReq.
         */

      case LCP_CI_AP:
         if (cilen < 4) {
            CI_REJECT(2);
            break;
         } /* Endif */

            /* If we don't have any secrets, reject AP */
/* Start CR 2207 */
         if (!(PPP_SECRET(ppp_ptr,_PPP_PAP_LSECRET) || PPP_SECRET(ppp_ptr,_PPP_CHAP_LSECRETS))) {
/* End CR 2207 */
            CI_REJECT(2);
            break;
         } /* Endif */

            /* Check the desired authentication protocol */
         req_opt.AP = ntohs(inp); inp += 2;
         switch (req_opt.AP) {

            /* Accept PAP only if we have a secret */
         case PPP_PROT_PAP:
            if (cilen != 4) {
               CI_REJECT(4);
               break;
/* Start CR 2207 */
            } else if (PPP_SECRET(ppp_ptr,_PPP_PAP_LSECRET) == NULL) {
/* End CR 2207 */
               CI_NAKAP(4);
               break;
            } /* Endif */
            req_opt.AP_Start = PAP_send;
            break;

            /* Accept CHAP only if we have a secrets table */
         case PPP_PROT_CHAP:
            if (cilen != 5) {
               CI_REJECT(4);
               break;
/* Start CR 2207 */
            } else if (PPP_SECRET(ppp_ptr,_PPP_CHAP_LSECRETS) == NULL) {
/* End CR 2207 */
               CI_NAKAP(4);
               break;
            } else if (*inp++ != 5) {     /* Only MD5 supported */
               CI_NAKAP(5);
               break;
            } /* Endif */
            req_opt.AP_Start = CHAP_open;
            break;

         default:
            CI_NAKAP(4);
            break;
         } /* Endswitch */
         break;

      case LCP_CI_MAGIC:
         if (cilen != 6) {
            CI_REJECT(2);
            break;
         } /* Endif */

         inp += 4;
         break;

      case LCP_CI_PFC:
         if (cilen != 2) {
            CI_REJECT(2);
            break;
         } /* Endif */

         req_opt.PFC = TRUE;
         break;

      case LCP_CI_ACFC:
         if (cilen != 2) {
            CI_REJECT(2);
            break;
         } /* Endif */

         req_opt.ACFC = TRUE;
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
      lcp_ptr->SEND_OPT = req_opt;
      sizeleft = fsm->LENGTH;
   } else if (code == CP_CODE_CONF_REJ) {
      sizeleft = rejlen;
   } else if (reject) {
      code = CP_CODE_CONF_REJ;
      sizeleft = naklen;
   } else { /* if code == NAK */

         /* We want to nak AP, and we didn't nak anything else after it */
      if (apneg) {
/* Start CR 2207 */
         if (PPP_SECRET(ppp_ptr,_PPP_CHAP_LSECRETS)) {
/* End CR 2207 */
            *nakp++ = LCP_CI_AP;
            naklen += *nakp++ = 5;
            htons(nakp, PPP_PROT_CHAP); nakp += 2;
            *nakp++ = 5;                  /* Only MD5 supported */
/* Start CR 2207 */
         } else if (PPP_SECRET(ppp_ptr,_PPP_PAP_LSECRET)) {
/* End CR 2207 */
            *nakp++ = LCP_CI_AP;
            naklen += *nakp++ = 4;
            htons(nakp, PPP_PROT_PAP); nakp += 2;
         } /* Endif */
      } /* Endif */

      sizeleft = naklen;
   } /* Endif */

   fsm->CODE = code;
   return sizeleft;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_recvcoderej
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_Recvcoderej.  Returns TRUE if the rejected
*     code is catastrophic.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean LCP_recvcoderej
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   uchar code = *fsm->DATA;
   return (code >= 1 && code <= 7);
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_up
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by the FSM when entering the OPENED state.  Returns
*     TRUE if the link can be allowed to remain up.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean LCP_up
   (
      PPPFSM_CFG_PTR          fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = fsm->HANDLE;
   LCP_CFG_PTR lcp_ptr = fsm->PRIVATE;
   PPP_CALL_INTERNAL_PTR callauth_ptr = &ppp_ptr->LCP_CALL[PPP_CALL_AUTH_FAIL];

/* Start CR 2207 */
   if ((PPP_SECRET(ppp_ptr,_PPP_PAP_RSECRETS) || PPP_SECRET(ppp_ptr,_PPP_CHAP_RSECRETS))
/* End CR 2207 */
       && !lcp_ptr->RECV_OPT.AP) {
      if (callauth_ptr->CALLBACK) {
         (*callauth_ptr->CALLBACK)(callauth_ptr->PARAM);
      } /* Endif */
      return FALSE;
   } /* Endif */

      /* Initialize the authentication protocols */
   PAP_init(fsm->HANDLE);
   CHAP_init(fsm->HANDLE);

   PPP_mutex_lock(&ppp_ptr->MUTEX);
   ppp_ptr->LINK_STATE = TRUE;
   ppp_ptr->LINK_AUTH = (lcp_ptr->RECV_OPT.AP == 0);
   ppp_ptr->RECV_OPTIONS = &lcp_ptr->RECV_OPT;
   ppp_ptr->SEND_OPTIONS = &lcp_ptr->SEND_OPT;
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_S_ACCM, &lcp_ptr->SEND_OPT.ACCM);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_R_ACCM, &lcp_ptr->RECV_OPT.ACCM);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_S_PFC,  &lcp_ptr->SEND_OPT.PFC);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_R_PFC,  &lcp_ptr->RECV_OPT.PFC);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_S_ACFC, &lcp_ptr->SEND_OPT.ACFC);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_R_ACFC, &lcp_ptr->RECV_OPT.ACFC);
   PPP_mutex_unlock(&ppp_ptr->MUTEX);

   /* Start CR 2158 */
   if ((lcp_ptr->SEND_OPT.AP != 0) || (lcp_ptr->RECV_OPT.AP != 0)) {
      /* The link requires authentication */
         
      /* If the peer requested authentication, send it */
      if (lcp_ptr->SEND_OPT.AP != 0) {
         (*lcp_ptr->SEND_OPT.AP_Start)(fsm->HANDLE);
      } /* Endif */

      /*
      ** Note: we really should call RECV_OPT.AP_Start() before
      ** setting LINK_STATE (but still call PPP_LINK_UP() after),
      ** but, LCP_up() is always called from the context of the
      ** PPP rx task, so there isn't a problem.
      **
      ** (There would be a problem if an AP packet could be processed
      ** after setting LINK_STATE=TRUE, LINK_AUTH=FALSE and before
      ** AP_Start() could initialize the AP's state.  But there isn't
      ** a problem because we're always in the context of the only task
      ** that does this processing.)
      */

      /* If we requested authentication, wait for it */
      if (lcp_ptr->RECV_OPT.AP != 0) {
         (*lcp_ptr->RECV_OPT.AP_Start)(fsm->HANDLE);
      }

   } else {
   /* Otherwise, call the application's PPP_LINK_UP callback */
      PPP_up(ppp_ptr);
   /* End CR 2158 */
   } /* Endif */

   return TRUE;
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_down
* Returned Value  :  void
* Comments        :
*     Called by the FSM when leaving the OPENED state.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void LCP_down
   (
      PPPFSM_CFG_PTR          fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = fsm->HANDLE;
   PPP_CALL_INTERNAL_PTR calldown_ptr = &ppp_ptr->LCP_CALL[PPP_CALL_LINK_DOWN];
   boolean linkauth;

      /* Stop compression */
   CCP_lowerdown(fsm->HANDLE);

   PPP_mutex_lock(&ppp_ptr->MUTEX);
   ppp_ptr->LINK_STATE = FALSE;
   linkauth = ppp_ptr->LINK_AUTH;
   ppp_ptr->RECV_OPTIONS = &PPP_DEFAULT_OPTIONS;
   ppp_ptr->SEND_OPTIONS = &PPP_DEFAULT_OPTIONS;
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_S_ACCM, &PPP_DEFAULT_OPTIONS.ACCM);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_R_ACCM, &PPP_DEFAULT_OPTIONS.ACCM);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_S_PFC,  &PPP_DEFAULT_OPTIONS.PFC);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_R_PFC,  &PPP_DEFAULT_OPTIONS.PFC);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_S_ACFC, &PPP_DEFAULT_OPTIONS.ACFC);
   _iopcb_ioctl(ppp_ptr->DEVICE, IOPCB_IOCTL_R_ACFC, &PPP_DEFAULT_OPTIONS.ACFC);
   PPP_mutex_unlock(&ppp_ptr->MUTEX);

      /* Call the application's PPP_LINK_DOWN callback */
   if (calldown_ptr->CALLBACK && linkauth) {
      (*calldown_ptr->CALLBACK)(calldown_ptr->PARAM);
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_up
* Returned Value  :  void
* Comments        :
*     Called when the PPP link goes up.  Called either by LCP
*     when entering the OPENED state, or by an authentication
*     protocol once the peer has authenticated itself.
*
*END*-----------------------------------------------------------------*/

void PPP_up
   (
      _ppp_handle   handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR           ppp_ptr    = handle;
   PPP_CALL_INTERNAL_PTR callup_ptr = &ppp_ptr->LCP_CALL[PPP_CALL_LINK_UP];

   if (callup_ptr->CALLBACK) {
      (*callup_ptr->CALLBACK)(callup_ptr->PARAM);
   } /* Endif */

   /* Try to negotiate compression */
   CCP_lowerup(handle);

#endif /* RTCSCFG_ENABLE_IP4 */

} /* Endbody */


/* EOF */
