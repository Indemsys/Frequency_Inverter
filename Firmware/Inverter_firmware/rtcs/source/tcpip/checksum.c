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
* $FileName: checksum.c$
* $Version : 3.8.7.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation for a one's
*   complement checksum.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_Sum_immediate
* Returned Value  : one's complement checksum
* Comments        :
*     Returns a one's complement checksum of two numbers.
*
*     Note:  This function returns 0 iff both summands are 0.
*
*END*-----------------------------------------------------------------*/

uint_16 IP_Sum_immediate
   (
      uint_16        sum,
         /* [IN] initial sum */
      uint_16        immediate
         /* [IN] number to add to sum */
   )
{ /* Body */
   uint_32 total = sum & 0xFFFF;

   total += immediate & 0xFFFF;

   sum = (total >> 16) & 0xFFFF;
   total = (total & 0xFFFF) + sum;
   return total;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_Sum_PCB
* Returned Value  : one's complement checksum
* Comments        :
*     Returns a one's complement checksum over all fragments in a PCB.
*
*     Note:  This function returns 0 iff all summands are 0.
*
*END*-----------------------------------------------------------------*/

uint_16 IP_Sum_PCB
   (
      uint_16        sum,
         /* [IN] initial sum */
      RTCSPCB_PTR    pcb
         /* [IN] the PCB */
   )
{ /* Body */
   PCB_FRAGMENT_PTR  frag = &pcb->PCBPTR->FRAG[1];
   uint_32           is_odd = pcb->HEADER_FRAG_USED & 1;

   sum = _mem_sum_ip(sum, pcb->HEADER_FRAG_USED, RTCSPCB_DATA(pcb));

   while (frag->LENGTH) {
      if (is_odd) {
         /* If previous fragment was odd, add in first byte in lower 8 bits */
         sum = IP_Sum_immediate(sum, frag->FRAGMENT[0] & 0xFF);
      } /* Endif */

      sum = _mem_sum_ip(sum, frag->LENGTH - is_odd, frag->FRAGMENT + is_odd);
      is_odd = (frag->LENGTH - is_odd) & 1;
      frag++;
   } /* Endwhile */

   return sum;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IP_Sum_pseudo
* Returned Value  : one's complement checksum
* Comments        :
*     Returns a one's complement checksum of the IP pseudo header
*
*     Note:  This function returns 0 iff all summands are 0.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_16 IP_Sum_pseudo
   (
      uint_16        sum,
         /* [IN] initial sum */
      RTCSPCB_PTR    pcb,
         /* [IN] the PCB */
      int_32         layer
         /* [IN] IP layer, relative to current */
   )
{ /* Body */
   IP_HEADER_PTR iphead = (IP_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb);
   uint_16 protocol;

   protocol = RTCSPCB_TRANSPORT_PROTL(pcb);
   sum = _mem_sum_ip(sum, 8, iphead->SOURCE);     /* source and dest */
   sum = IP_Sum_immediate(sum, protocol);    /* protocol */
   return sum;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
