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
* $FileName: ensend.c$
* $Version : 3.7.12.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the Ethernet send
*   interface function.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_send
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Sends a packet.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_send
   (
      /* [IN] the Ethernet state structure */
      _enet_handle   handle,

      /* [IN] the packet to send */
      PCB_PTR        packet,

      /* [IN] the protocol */
      uint_16        type,

      /* [IN] the destination Ethernet address */
      _enet_address  dest,

      /* [IN] optional flags, zero = default */
      uint_32        flags
   )
{ 
   ENET_CONTEXT_STRUCT_PTR  enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   ENET_HEADER_PTR         packet_ptr;
   uchar_ptr               type_ptr;
   PCB_FRAGMENT_PTR        frag_ptr;
   uint_32                 swhdr, size, frags;
   uint_32                 error;
   _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
   
   _KLOGM(_GET_KERNEL_DATA(kernel_data);)
   _KLOGE6(KLOG_ENET_send, handle, packet, type, dest, flags);

   if (flags & ENET_OPT_8021QTAG) {
       swhdr = ENET_FRAMESIZE_HEAD_VLAN;
   } else {
       swhdr = ENET_FRAMESIZE_HEAD;
   } 

   /*
   ** Make sure the first fragment is long enough for the Ethernet
   ** frame header.  This isn't strictly necessary, but it's impractical
   ** to split a 14-26 byte header over multiple fragments.
   */
#if MQX_CHECK_ERRORS
   if (packet->FRAG[0].LENGTH < swhdr) {
      ENET_INC_STATS(COMMON.ST_TX_DISCARDED);
      error = ENETERR_SEND_SHORT;
      goto ERROR;
   }
#endif

   /*
   ** Make sure that no fragment exceeds a maximum packet length.
   ** We check every fragment because we want to prevent something
   ** like FRAG[0].LENGTH = 2000, FRAG[1].LENGTH = -1000.  This
   ** situation would not be detected if we only check the total
   ** length.
   */
   size = frags = 0;
   for (frag_ptr = packet->FRAG; frag_ptr->LENGTH; frag_ptr++) {
#if MQX_CHECK_ERRORS
      if (frag_ptr->LENGTH > enet_ptr->MaxTxFrameSize) {   
         ENET_INC_STATS(COMMON.ST_TX_DISCARDED);
         error = ENETERR_SEND_LONG;
         goto ERROR;
      } 
#endif
      size += frag_ptr->LENGTH;
      frags++;
   } 

   /*
   ** Make sure that the total sum of the fragments doesn't exceed
   ** a maximum packet length.
   */
#if MQX_CHECK_ERRORS
   if (size > enet_ptr->MaxTxFrameSize) {
      ENET_INC_STATS(COMMON.ST_TX_DISCARDED);
      error = ENETERR_SEND_LONG;
      goto ERROR;
   } 
#endif

   /*
   ** Everything checks out -- fill in the header.
   */
   packet_ptr = (ENET_HEADER_PTR)packet->FRAG[0].FRAGMENT;
   htone(packet_ptr->DEST, dest);
   htone(packet_ptr->SOURCE, enet_ptr->ADDRESS);
   type_ptr = packet_ptr->TYPE;

   if (flags & ENET_OPT_8021QTAG) {
      ENET_8021QTAG_HEADER_PTR tag_ptr = (ENET_8021QTAG_HEADER_PTR)(type_ptr+2);
      uint_16 tag;
      tag = ENET_GETOPT_8021QPRIO(flags) << 13;
      htons(type_ptr, ENETPROT_8021Q);
      htons(tag_ptr->TAG, tag);
      type_ptr = tag_ptr->TYPE;
   } 

   if (flags & ENET_OPT_8023) {
      ENET_8022_HEADER_PTR llc_ptr = (ENET_8022_HEADER_PTR)(type_ptr+2);
      htons(type_ptr, size - swhdr);
      htonc(llc_ptr->DSAP, 0xAA);
      htonc(llc_ptr->SSAP, 0xAA);
      htonc(llc_ptr->COMMAND, 0x03);
      htonc(&llc_ptr->OUI[0], 0x00);
      htonc(&llc_ptr->OUI[1], 0x00);
      htonc(&llc_ptr->OUI[2], 0x00);
      type_ptr = llc_ptr->TYPE;
   } 

   htons(type_ptr, type);

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself, and with ENET_ISR().
   */
   ENET_lock_context(enet_ptr);
   error = (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->SEND)(handle, packet, size, frags, flags);
   ENET_unlock_context(enet_ptr);

   if (error) {
   ERROR:
      PCB_free(packet);
   }
   
   _KLOGX4(KLOG_ENET_send, handle, packet, error);
   
   return error;
}  



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_send_raw
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Sends a packet.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_send_raw
   (
      /* [IN] the Ethernet state structure */
      _enet_handle   handle,

      /* [IN] the packet to send */
      PCB_PTR        packet
   )
{ /* Body */
   ENET_CONTEXT_STRUCT_PTR    enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   PCB_FRAGMENT_PTR           frag_ptr;
   uint_32                    size, frags;
   uint_32                    error;

   /*
   ** Make sure that no fragment exceeds a maximum packet length.
   ** We check every fragment because we want to prevent something
   ** like FRAG[0].LENGTH = 2000, FRAG[1].LENGTH = -1000.  This
   ** situation would not be detected if we only check the total
   ** length.
   */
   size = frags = 0;
   for (frag_ptr = packet->FRAG; frag_ptr->LENGTH; frag_ptr++) {
#if MQX_CHECK_ERRORS
      if (frag_ptr->LENGTH > enet_ptr->MaxTxFrameSize) {   
         ENET_INC_STATS(COMMON.ST_TX_DISCARDED);
         error = ENETERR_SEND_LONG;
         goto ERROR;
      } 
#endif
      size += frag_ptr->LENGTH;
      frags++;
   } 

   /*
   ** Make sure that the total sum of the fragments doesn't exceed
   ** a maximum packet length.
   */
#if MQX_CHECK_ERRORS
   if (size > enet_ptr->MaxTxFrameSize) {
      ENET_INC_STATS(COMMON.ST_TX_DISCARDED);
      error = ENETERR_SEND_LONG;
      goto ERROR;
   } 
#endif

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself, and with ENET_ISR().
   */
   ENET_lock_context(enet_ptr);
   error = (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->SEND)(handle, packet, size, frags, 0);
   ENET_unlock_context(enet_ptr);

   if (error) {
   ERROR:
      PCB_free(packet);
   }  

   return error;

}  

/* EOF */
