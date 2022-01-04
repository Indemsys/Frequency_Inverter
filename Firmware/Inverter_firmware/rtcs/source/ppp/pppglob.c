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
* $FileName: pppglob.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the various global PPP structures
*
*END************************************************************************/

#include <ppp.h>
#include "ppp_prv.h"


/*
** PPP task priority and extra stack size
*/

uint_32  _PPPTASK_priority   = 6;
uint_32  _PPPTASK_stacksize  = 0;

/*
** Variables used by the retransmission timer
*/

uint_32 _PPP_MIN_XMIT_TIMEOUT = 3000;        /* RFC 1661 4.6 */
uint_32 _PPP_MAX_XMIT_TIMEOUT = 10000l;
uint_32 _PPP_MAX_CONF_RETRIES = 10;          /* RFC 1661 4.6 */
uint_32 _PPP_MAX_TERM_RETRIES = 2;           /* RFC 1661 4.6 */
uint_32 _PPP_MAX_CONF_NAKS    = 5;           /* RFC 1661 4.6 */


/*
** The default PPP link options
*/

PPP_OPT PPP_DEFAULT_OPTIONS = {
   /* ACCM     */   0xFFFFFFFF,
   /* MRU      */   DEFAULT_MRU,
   /* AP       */   0,
   /* QP       */   0,
   /* AP_Start */   NULL,
   /* MAGIC    */   0,
   /* PFC      */   FALSE,
   /* ACFC     */   FALSE,
   /* CP       */   NULL
};

PPP_OPT PPP_DEFAULT_OPTIONS_PPPOE = {
   /* ACCM     */   0xFFFFFFFF,
   /* MRU      */   1492,
   /* AP       */   0,
   /* QP       */   0,
   /* AP_Start */   NULL,
   /* MAGIC    */   0,
   /* PFC      */   FALSE,
   /* ACFC     */   FALSE,
   /* CP       */   NULL
};


/*
** The default PPP call table
*/

PPP_CALL PPP_DEFAULT_CALL = {
   /* PARAM          */    0,
   /* PPP_Link_up    */    NULL,
   /* PPP_Link_down  */    NULL,
   /* PPP_Echo_reply */    NULL,
   /* PPP_Auth_fail  */    NULL
};



/*
** The minimal ACCM
*/

uint_32 _PPP_ACCM = 0xFFFFFFFFl;


/*
** The secrets.  These are the secrets used by PAP and CHAP.
** an xSECRET is a pointer to a single secret; an xSECRETS
** points to an array of secrets.
**
** LSECRET(S) points to the secret(s) to use when authenticating
** ourself to the peer (local secret(s)); RSECRET(S) points to
** the secret(s) to use when authenticating the peer (remote
** secret(s)).
**
** If an LSECRET(S) pointer is NULL, LCP will not allow the
** peer to request that authentication protocol.  If all LSECRET(S)
** are NULL, LCP will reject negotiation of the authentication
** protocol.
**
** If any RSECRET(S) are non-NULL, LCP will require that the peer
** authenticate itself.  If the peer rejects negotiation of the
** authentication protocol, LCP will terminate the link immediately
** upon reaching the OPENED state.
*/

PPP_SECRET_PTR _PPP_PAP_LSECRET = NULL;
PPP_SECRET_PTR _PPP_PAP_RSECRETS = NULL;
char_ptr       _PPP_CHAP_LNAME = "";
PPP_SECRET_PTR _PPP_CHAP_LSECRETS = NULL;
PPP_SECRET_PTR _PPP_CHAP_RSECRETS = NULL;


/* EOF */
