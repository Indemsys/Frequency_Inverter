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
* $FileName: lcpif.c$
* $Version : 3.8.6.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the LCP
*   interface routines.
*
*END************************************************************************/

#include <ppp.h>
#include "ppp_prv.h"


static uint_32 LCP_send(_ppp_handle, uchar, uchar, PCB_PTR);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_echo
* Returned Value  :  error code
* Comments        :
*     Sends an LCP Echo-Request packet.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_echo
   (
      _ppp_handle       handle,
            /* [IN] - the PPP state structure */
      uchar             id,
            /* [IN] - packet ID */
      PCB_PTR           packet
            /* [IN] - the packet to send */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   return LCP_send(handle, LCP_CODE_ECHO_REQ, id, packet);

#else

    return RTCSERR_IP_IS_DISABLED;    

#endif /* RTCSCFG_ENABLE_IP4 */  
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_discard
* Returned Value  :  error code
* Comments        :
*     Sends an LCP Discard-Request packet.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_discard
   (
      _ppp_handle       handle,
            /* [IN] - the PPP state structure */
      uchar             id,
            /* [IN] - packet ID */
      PCB_PTR           packet
            /* [IN] - the packet to send */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   return LCP_send(handle, LCP_CODE_DISC_REQ, id, packet);

#else

    return RTCSERR_IP_IS_DISABLED;    

#endif /* RTCSCFG_ENABLE_IP4 */  
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  LCP_send
* Returned Value  :  error code
* Comments        :
*     Sends an LCP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 LCP_send
   (
      _ppp_handle       handle,
            /* [IN] - the PPP state structure */
      uchar             code,
            /* [IN] - LCP code */
      uchar             id,
            /* [IN] - packet ID */
      PCB_PTR           pcb
            /* [IN] - the packet to send */
   )
{ /* Body */
   PCB_FRAGMENT_PTR  frag_ptr;
   uchar_ptr         outp;
   uint_32           size;

   /*
   ** Make sure the first fragment is long enough for the PPP protocol
   ** field and the LCP header.  This isn't strictly necessary, but
   ** it's impractical to split a 10 byte field over multiple fragments.
   */
   if (pcb->FRAG[0].LENGTH < 10) {
      PCB_free(pcb);
      return RTCSERR_PPP_PACKET_TOO_SHORT;
   } /* Endif */

   size = 0;
   for (frag_ptr = pcb->FRAG; frag_ptr->LENGTH; frag_ptr++) {
      size += frag_ptr->LENGTH;
   } /* Endfor */
   size -= 2;

   outp = pcb->FRAG[0].FRAGMENT + 2;
   htonc(outp, code); outp++;
   htonc(outp, id);   outp++;
   htons(outp, size); outp += 2;
   htonl(outp, 0);    outp += 4;

   return PPP_send(handle, PPP_PROT_LCP, pcb);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */


/* EOF */
