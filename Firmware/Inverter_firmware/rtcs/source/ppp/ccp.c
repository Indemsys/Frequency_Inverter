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
* $FileName: ccp.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation for the
*   Compression Control Protocol.
*
*END************************************************************************/

#include <ppp.h>

#if RTCSCFG_ENABLE_IP4

#include "ppp_prv.h"


static boolean CCP_up(PPPFSM_CFG_PTR);
static void    CCP_down(PPPFSM_CFG_PTR);

static void    CCP_resetconfreq(PPPFSM_CFG_PTR);
static uint_32 CCP_buildconfreq(PPPFSM_CFG_PTR, uchar_ptr, uint_32);

static uint_32 CCP_recvconfreq(PPPFSM_CFG_PTR, boolean);
static boolean CCP_recvconfack(PPPFSM_CFG_PTR);
static boolean CCP_recvconfnak(PPPFSM_CFG_PTR);
static boolean CCP_recvconfrej(PPPFSM_CFG_PTR);
static boolean CCP_recvcoderej(PPPFSM_CFG_PTR);
static boolean CCP_recvextcode(PPPFSM_CFG_PTR);

static PPPFSM_CALL CCP_CALL = {
   /* PROTOCOL    */    PPP_PROT_CCP,
   /* linkup()    */    CCP_up,
   /* linkdown()  */    CCP_down,
   /* resetreq()  */    CCP_resetconfreq,
   /* buildreq()  */    CCP_buildconfreq,
   /* recvreq()   */    CCP_recvconfreq,
   /* recvack()   */    CCP_recvconfack,
   /* recvnak()   */    CCP_recvconfnak,
   /* recvrej()   */    CCP_recvconfrej,
   /* testcode()  */    CCP_recvcoderej,
   /* recvcode()  */    CCP_recvextcode
};


/*
** The default CCP link options
*/

static CCP_OPT CCP_DEFAULT_OPTIONS = {
   /* CP    */    NULL,
   /* DATA  */    {0}
};

static CCP_NEG CCP_DEFAULT_NEG = {
   /* NEG_*     */   0,0
#ifdef PPP_CP_LZS
   /* LZS_HIST  */   ,1
   /* LZS_CHECK */   ,3
#endif
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_init
* Returned Value  :  error code
* Comments        :
*     Called by PPP_initialize.  Initializes the CCP Finite State Machine.
*
*END*-----------------------------------------------------------------*/

uint_32 CCP_init
   (
      _ppp_handle    handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = handle;

      /* Initialize the FSM */
   return PPPFSM_init(&ppp_ptr->CCP_FSM, handle, &CCP_CALL, &ppp_ptr->CCP_STATE);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_destroy
* Returned Value  :  error code
* Comments        :
*     Called by PPP_initialize.  Destroys the CCP Finite State Machine.
*     Assumes the Finite State Machine has been initialized.
*
*END*-----------------------------------------------------------------*/

uint_32 CCP_destroy
   (
      _ppp_handle      handle
            /* [IN] - the PPP state structure */
   )
{ /* Body */

   PPP_CFG_PTR ppp_ptr = handle;

      /* Destroy the FSM */
   return PPPFSM_destroy(&ppp_ptr->CCP_FSM);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_up
* Returned Value  :  TRUE (always)
* Comments        :
*     Called by the FSM when entering the OPENED state.
*
*END*-----------------------------------------------------------------*/

static boolean CCP_up
   (
      PPPFSM_CFG_PTR     fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = fsm->HANDLE;
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;
   CP_CALL_PTR cprecv = ccp_ptr->RECV_OPT.CP;
   CP_CALL_PTR cpsend = ccp_ptr->SEND_OPT.CP;

      /* Initialize compression methods */
   if (cprecv) {
      cprecv->CP_init(&ccp_ptr->RECV_DATA, &ccp_ptr->RECV_OPT);
   } /* Endif */
   if (cpsend) {
      cpsend->CP_init(&ccp_ptr->SEND_DATA, &ccp_ptr->SEND_OPT);
   } /* Endif */

   PPP_mutex_lock(&ppp_ptr->MUTEX);
   ppp_ptr->RECV_OPTIONS->CP = cprecv;
   ppp_ptr->SEND_OPTIONS->CP = cpsend;
   PPP_mutex_unlock(&ppp_ptr->MUTEX);

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_down
* Returned Value  :  void
* Comments        :
*     Called by the FSM when leaving the OPENED state.
*
*END*-----------------------------------------------------------------*/

static void CCP_down
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   PPP_CFG_PTR ppp_ptr = fsm->HANDLE;

      /* Restore default compression methods */
   PPP_mutex_lock(&ppp_ptr->MUTEX);
   ppp_ptr->RECV_OPTIONS->CP = PPP_DEFAULT_OPTIONS.CP;
   ppp_ptr->SEND_OPTIONS->CP = PPP_DEFAULT_OPTIONS.CP;
   PPP_mutex_unlock(&ppp_ptr->MUTEX);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_resetconfreq
* Returned Value  :  void
* Comments        :
*     Called by PPPFSM_sendconfreq when starting negotiation.
*     Initializes the Send Options.
*
*END*-----------------------------------------------------------------*/

static void CCP_resetconfreq
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;

   ccp_ptr->SEND_OPT = CCP_DEFAULT_OPTIONS;
   ccp_ptr->RECV_OPT = CCP_DEFAULT_OPTIONS;
   ccp_ptr->RECV_NEG = CCP_DEFAULT_NEG;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_buildconfreq
* Returned Value  :  number of bytes written
* Comments        :
*     Called by PPPFSM_sendconfreq.  Builds a ConfReq packet.
*
*END*-----------------------------------------------------------------*/

static uint_32 CCP_buildconfreq
   (
      PPPFSM_CFG_PTR    fsm,
            /* [IN] - State Machine */
      uchar_ptr         outp,
            /* [IN] - free packet */
      uint_32           sizeleft
            /* [IN] - size of packet */
   )
{ /* Body */
#if defined(PPP_CP_LZS) || defined(PPP_CP_PRED1)
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;
#endif
/* End SPR P122-0266-38. */
   uint_32     totlen = 0;

      /*
      ** Generate configuration information for each option
      ** we want to negotiate, in order of preference.
      */

#ifdef PPP_CP_LZS
   if (ccp_ptr->RECV_NEG.NEG_LZS) {
      if (sizeleft >= totlen+5) {
         *outp++ = CCP_CI_LZS;
         totlen += *outp++ = 5;
         *outp++ = ccp_ptr->RECV_NEG.LZS_HIST >> 8;
         *outp++ = ccp_ptr->RECV_NEG.LZS_HIST & 0xFF;
         *outp++ = ccp_ptr->RECV_NEG.LZS_CHECK;
      } /* Endif */
   } /* Endif */
#endif

#ifdef PPP_CP_PRED1
   if (ccp_ptr->RECV_NEG.NEG_PRED1) {
      if (sizeleft >= totlen+2) {
         *outp++ = CCP_CI_PRED1;
         len += *outp++ = 2;
      } /* Endif */
   } /* Endif */
#endif

   return totlen;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_recvconfack
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_recvconfack.  Parses a ConfAck packet.
*     This function should not modify any state if the ack is bad.
*
*END*-----------------------------------------------------------------*/

static boolean CCP_recvconfack
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;
#if defined(PPP_CP_LZS) || defined(PPP_CP_PRED1)
   uchar_ptr   inp = fsm->DATA;
#endif
   uint_16     len = fsm->LENGTH;
   CCP_OPT     ack_opt = ccp_ptr->RECV_OPT;
   ack_opt.CP = NULL;

      /*
      ** The ack must be identical to the last ConfReq we sent
      */

#ifdef PPP_CP_LZS
   if (ccp_ptr->RECV_NEG.NEG_LZS) {
      uint_16 hist;
      uchar   check;
      if (len < 5) goto badack;
      if (*inp++ != CCP_CI_LZS) goto badack;
      if (*inp++ != 5) goto badack;
      hist  = (uint_16)*inp++ << 8;
      hist |= (uint_16)*inp++;
      if (hist != ccp_ptr->RECV_NEG.LZS_HIST) goto badack;
      check = *inp++;
      if (check != ccp_ptr->RECV_NEG.LZS_CHECK) goto badack;
      if (!ack_opt.CP) {
         ack_opt.CP = &CP_LZS;
         ack_opt.DATA.LZS.HIST = hist;
         ack_opt.DATA.LZS.CHECK = check;
      } /* Endif */
      len -= 5;
   } /* Endif */
#endif

#ifdef PPP_CP_PRED1
   if (ccp_ptr->RECV_NEG.NEG_PRED1) {
      if (len < 2) goto badack;
      if (*inp++ != CCP_CI_PRED1) goto badack;
      if (*inp++ != 2) goto badack;
      if (!ack_opt.CP) {
         ack_opt.CP = &CP_PRED1;
      } /* Endif */
      len -= 2;
   } /* Endif */
#endif

   if (len) goto badack;

   if (fsm->STATE < PPP_STATE_OPENED) {
      ccp_ptr->RECV_OPT = ack_opt;
   } /* Endif */
   return TRUE;

badack:
   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_recvconfnak
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_recvconfnak.  Parses a ConfNak packet.
*     This function should not modify any state if the nak is bad.
*
*END*-----------------------------------------------------------------*/

static boolean CCP_recvconfnak
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;
   uchar_ptr   inp = fsm->DATA;
   uint_16     len = fsm->LENGTH;
   CCP_NEG     req_neg = ccp_ptr->RECV_NEG;
   uchar       code;
   uchar       codelen;

      /*
      ** Nak'd codes must be in the same order as they were in the ConfReq
      */

   code = *inp++;

#ifdef PPP_CP_LZS
   if (len && req_neg.NEG_LZS && code == CCP_CI_LZS) {
      if (len < 5) goto badnak;
      if (*inp++ != 5) goto badnak;
      len -= 5;

         /* We are prepared to accept a history count of 0 or 1 */
      req_neg.LZS_HIST  = (uint_16)*inp++ << 8;
      req_neg.LZS_HIST |= (uint_16)*inp++;
      if (req_neg.LZS_HIST > 1) {
         req_neg.LZS_HIST = 1;
      } /* Endif */

         /* We are prepared to accept a check mode of 0 to 3 */
      req_neg.LZS_CHECK = *inp++;
      if (req_neg.LZS_CHECK > 3) {
         if (req_neg.LZS_HIST) {
            req_neg.LZS_CHECK = 3;
         } else {
            req_neg.LZS_CHECK = 0;
         } /* Endif */
      } /* Endif */

      code = *inp++;
   } /* Endif */
#endif

      /*
      ** If we advertised Predictor 1 and the peer doesn't want
      ** it, they should send a ConfRej rather than a ConfNak,
      ** so we're not going to check for it here.
      */

      /*
      ** There may be remaining codes if the peer wants us to
      ** negotiate an option we didn't include.
      */

   while (len) {
      switch (code) {
#ifdef PPP_CP_PRED1
      case CCP_CI_PRED1:
         if (req_neg.NEG_PRED1 || len < 2 || *inp++ != 2) goto badnak;
         len -= 2;
         req_neg.NEG_PRED1 = 1;
         break;
#endif
#ifdef PPP_CP_LZS
      case CCP_CI_LZS:
         if (req_neg.NEG_LZS || len < 5 || *inp++ != 5) goto badnak;
         len -= 5;
         req_neg.NEG_LZS = 1;

            /* We are prepared to accept a history count of 0 or 1 */
         req_neg.LZS_HIST  = (uint_16)*inp++ << 8;
         req_neg.LZS_HIST |= (uint_16)*inp++;
         if (req_neg.LZS_HIST > 1) {
            req_neg.LZS_HIST = 1;
         } /* Endif */

            /* We are prepared to accept a check mode of 0 to 3 */
         req_neg.LZS_CHECK = *inp++;
         if (req_neg.LZS_CHECK > 3) {
            if (req_neg.LZS_HIST) {
               req_neg.LZS_CHECK = 3;
            } else {
               req_neg.LZS_CHECK = 0;
            } /* Endif */
         } /* Endif */

         break;
#endif
      default:
         if (len < 2) goto badnak;
         codelen = *inp--;
         if (len < codelen) goto badnak;
         len -= codelen;
         inp += codelen;
      } /* Endswtich */
      code = *inp++;
   } /* Endwhile */

   if (fsm->STATE < PPP_STATE_OPENED) {
      ccp_ptr->RECV_NEG = req_neg;
   } /* Endif */
   return TRUE;

badnak:
   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_recvconfrej
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_recvconfnak.  Parses a ConfRej packet.
*     This function should not modify any state if the rej is bad.
*
*END*-----------------------------------------------------------------*/

static boolean CCP_recvconfrej
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;
   uint_16     len = fsm->LENGTH;
   CCP_NEG     req_neg = ccp_ptr->RECV_NEG;

#if defined(PPP_CP_LZS) || defined(PPP_CP_PRED1)
   uchar_ptr   inp = fsm->DATA;
   uchar       code;

   /*
   ** Rej'd codes must be in the same order as they were in the ConfReq
   */
   code = *inp++;
#endif

#ifdef PPP_CP_LZS
   if (len && req_neg.NEG_LZS && (code == CCP_CI_LZS)) {
      if (len < 5) goto badrej;
      if (*inp++ != 5) goto badrej;
      len -= 5;
      if ((req_neg.LZS_HIST >> 8)   != *inp++) goto badrej;
      if ((req_neg.LZS_HIST & 0xFF) != *inp++) goto badrej;
      if ((req_neg.LZS_CHECK)       != *inp++) goto badrej;
      req_neg.NEG_LZS = 0;
      code = *inp++;
   } /* Endif */
#endif

#ifdef PPP_CP_PRED1
   if (len && req_neg.NEG_PRED1 && (code == CCP_CI_PRED1)) {
      if (len < 2) goto badrej;
      if (*inp++ != 2) goto badrej;
      len -= 2;
      req_neg.NEG_PRED1 = 0;
      code = *inp++;
   } /* Endif */
#endif

   if (len) goto badrej;

   if (fsm->STATE < PPP_STATE_OPENED) {
      ccp_ptr->RECV_NEG = req_neg;
   } /* Endif */
   return TRUE;

badrej:
   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_recvconfreq
* Returned Value  :  number of bytes written
* Comments        :
*     Called by PPPFSM_recvconfreq.  Parses a ConfReq packet.
*     Builds a ConfAck/ConfNak/ConfRej packet out of fsm->PACKET,
*     and sets fsm->CODE appropriately.
*
*END*-----------------------------------------------------------------*/

static uint_32 CCP_recvconfreq
   (
      PPPFSM_CFG_PTR    fsm,
            /* [IN] - State Machine */
      boolean           reject
            /* [IN] - whether to ConfRej if we disagree */
   )
{ /* Body */
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;
   uchar_ptr   inp = fsm->DATA;
#ifdef PPP_CP_LZS
   uchar_ptr nakp;
#endif
   uchar_ptr rejp;

   uint_16     len = fsm->LENGTH;
   uint_16     naklen, rejlen;
   CCP_OPT     req_opt = ccp_ptr->SEND_OPT;
   uchar       code;
   uchar       cicode, cilen;
#ifdef PPP_CP_LZS
   uint_16     lzshist;
   uchar       lzscheck;
#endif

#define CI_REJECT(n)                   \
   inp -= n;                           \
   code = CP_CODE_CONF_REJ;            \
   rejlen += cilen;                    \
   while (cilen--) *rejp++ = *inp++

#ifdef PPP_CP_LZS
#define CI_NAK                               \
   if (code != CP_CODE_CONF_REJ) {           \
      code = CP_CODE_CONF_NAK;               \
      if (reject) {                          \
         inp -= cilen;                       \
         naklen += cilen;                    \
         while (cilen--) *nakp++ = *inp++;   \
      } else
#else
#define CI_NAK                               \
   if (code != CP_CODE_CONF_REJ) {           \
      code = CP_CODE_CONF_NAK;               \
      if (reject) {                          \
         inp -= cilen;                       \
         naklen += cilen;                    \
         while (cilen--) *inp++;             \
      } else
#endif

#define CI_ENDNAK                            \
   } /* Endif */

      /*
      ** Process all requested codes
      */

#ifdef PPP_CP_LZS
   rejp = nakp = inp;
#else
   rejp = inp;
#endif
   rejlen = naklen = 0;
   code = CP_CODE_CONF_ACK;
   req_opt.CP = NULL;

   while (len) {
         /* check remaining length */
      if (len < 2 || inp[1] < 2 || inp[1] > len) {
         code = CP_CODE_CONF_REJ;
         rejlen += len;
         while (len--) *rejp++ = *inp++;
         break;
      } /* Endif */

      cicode = *inp++;
      len -= cilen = *inp++;

      switch (cicode) {

#ifdef PPP_CP_PRED1
      case CCP_CI_PRED1:
         if (cilen != 2) {
            CI_REJECT(2);
            break;
         } /* Endif */

         if (!req_opt.CP) {
            req_opt.CP = &CP_PRED1;
         } /* Endif */
         break;
#endif

#ifdef PPP_CP_LZS
      case CCP_CI_LZS:
         if (cilen != 5) {
            CI_REJECT(2);
            break;
         } /* Endif */

            /* Accept any history count, but only check modes 0 to 3 */
         lzshist  = (uint_16)*inp++ << 8;
         lzshist |= (uint_16)*inp++;
         lzscheck = *inp++;
         if (lzscheck > 3) {
            CI_NAK {
               *nakp++ = CCP_CI_LZS;
               naklen += *nakp++ = 5;
               *nakp++ = lzshist >> 8;
               *nakp++ = lzshist;
               if (lzshist) {
                  *nakp++ = 3;
               } else {
                  *nakp++ = 0;
               } /* Endif */
            } CI_ENDNAK;
         } /* Endif */

         if (!req_opt.CP) {
            req_opt.CP = &CP_LZS;
            req_opt.DATA.LZS.HIST = lzshist;
            req_opt.DATA.LZS.CHECK = lzscheck;
         } /* Endif */
         break;
#endif

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
      ccp_ptr->SEND_OPT = req_opt;
      len = fsm->LENGTH;
   } else if (code == CP_CODE_CONF_REJ) {
      len = rejlen;
   } else if (reject) {
      code = CP_CODE_CONF_REJ;
      len = naklen;
   } else { /* if code == NAK */
      len = naklen;
   } /* Endif */

   fsm->CODE = code;
   return len;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_recvcoderej
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_recvcoderej.  Returns TRUE if the rejected
*     code is catastrophic.
*
*END*-----------------------------------------------------------------*/

static boolean CCP_recvcoderej
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   uchar code = *fsm->DATA;
   return ((code >= 1 && code <= 7) || (code >= 14 && code <= 15));
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  CCP_recvextcode
* Returned Value  :  TRUE or FALSE
* Comments        :
*     Called by PPPFSM_input.  Parses an CCP packet with an extended
*     code.
*
*END*-----------------------------------------------------------------*/

static boolean CCP_recvextcode
   (
      PPPFSM_CFG_PTR    fsm
            /* [IN] - State Machine */
   )
{ /* Body */
   CCP_CFG_PTR ccp_ptr = fsm->PRIVATE;
   CP_CALL_PTR cprecv = ccp_ptr->RECV_OPT.CP;
   CP_CALL_PTR cpsend = ccp_ptr->SEND_OPT.CP;

   switch (fsm->CODE) {

   case CCP_CODE_RESET_REQ:
      ccp_ptr->ST_CCP_RX_RESET++;
      if ((fsm->STATE < PPP_STATE_OPENED) || !cprecv) {
         PCB_free(fsm->PACKET);
      } else {
         cpsend->CP_resetreq(&ccp_ptr->SEND_DATA, fsm->PACKET);
      } /* Endif */
      break;

   case CCP_CODE_RESET_ACK:
      ccp_ptr->ST_CCP_RX_ACK++;
      if ((fsm->STATE < PPP_STATE_OPENED) || !cpsend) {
         PCB_free(fsm->PACKET);
      } else {
         cprecv->CP_resetack(&ccp_ptr->RECV_DATA, fsm->PACKET);
      } /* Endif */
      break;

   default:
      return FALSE;
   } /* Endswitch */

   return TRUE;
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
