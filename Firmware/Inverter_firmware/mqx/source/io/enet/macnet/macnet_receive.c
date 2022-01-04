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
* $FileName: macnet_receive.c$
* $Version : 3.8.8.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the MACNET receive
*   support functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "bsp_prv.h"
#include "macnet_prv.h"          

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_rx_free_specific
*  Returned Value : void
*  Comments       :
*        Frees a receive buffer
*
*END*-----------------------------------------------------------------*/

static void MACNET_rx_free_specific
   (
      MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr,
         /* [IN] the PCB to enqueue */
      PCB_PTR     pcb_ptr,
      pointer *   list
   )
{ 
   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   MACNET_int_disable();
   ENET_Enqueue_Buffer(list, pcb_ptr->FRAG[0].FRAGMENT );
   pcb_ptr->FRAG[0].FRAGMENT = NULL;
   QADD(macnet_context_ptr->RxPCBHead, macnet_context_ptr->RxPCBTail, pcb_ptr);
   MACNET_int_enable();
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_rx_free_small
*  Returned Value : void
*  Comments       :
*        Frees a small receive buffer
*
*END*-----------------------------------------------------------------*/

static void MACNET_rx_free_small
   (
         /* [IN] the PCB to enqueue */
      PCB_PTR  pcb_ptr
   )
{ 
   ENET_CONTEXT_STRUCT_PTR     enet_ptr = (ENET_CONTEXT_STRUCT_PTR)pcb_ptr->PRIVATE;
   MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;


   MACNET_rx_free_specific(macnet_context_ptr, pcb_ptr, (pointer *)&macnet_context_ptr->SMALL_BUFFERS);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_rx_free_large
*  Returned Value : void
*  Comments       :
*        Frees a small receive buffer
*
*END*-----------------------------------------------------------------*/

static void MACNET_rx_free_large
   (
         /* [IN] the PCB to enqueue */
      PCB_PTR  pcb_ptr
   )
{ 
   ENET_CONTEXT_STRUCT_PTR     enet_ptr = (ENET_CONTEXT_STRUCT_PTR)pcb_ptr->PRIVATE;
   MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;


   MACNET_rx_free_specific(macnet_context_ptr, pcb_ptr, (pointer *)&macnet_context_ptr->LARGE_BUFFERS);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_rx_free
*  Returned Value : void
*  Comments       :
*        Frees a normal sized receive buffer
*
*END*-----------------------------------------------------------------*/

static void MACNET_rx_free
   (
         /* [IN] the PCB to enqueue */
      PCB_PTR  pcb_ptr
   )
{ 
   ENET_CONTEXT_STRUCT_PTR     enet_ptr = (ENET_CONTEXT_STRUCT_PTR)pcb_ptr->PRIVATE;
   MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   #if ENETCFG_SUPPORT_FRAGMENTED_PCBS
   uint_32                     i;
   #endif

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   MACNET_int_disable();

   #if ENETCFG_SUPPORT_FRAGMENTED_PCBS
      for (i=0;pcb_ptr->FRAG[i].FRAGMENT;i++) {
         ENET_Enqueue_Buffer((pointer*)&macnet_context_ptr->RX_BUFFERS, pcb_ptr->FRAG[i].FRAGMENT );
         pcb_ptr->FRAG[i].FRAGMENT = NULL;
      }
   #else   
      ENET_Enqueue_Buffer((pointer*)&macnet_context_ptr->RX_BUFFERS, pcb_ptr->FRAG[0].FRAGMENT );
      pcb_ptr->FRAG[0].FRAGMENT = NULL;
   #endif
   
   QADD(macnet_context_ptr->RxPCBHead, macnet_context_ptr->RxPCBTail, pcb_ptr);

   MACNET_add_buffers_to_rx_ring(macnet_context_ptr);
   MACNET_int_enable();
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_add_buffers_to_rx_ring
*  Returned Value : void
*  Comments       :
*        Enqueues a PCB onto the receive ring.
*
*END*-----------------------------------------------------------------*/

void MACNET_add_buffers_to_rx_ring(MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr)
{ 
   ENET_MemMapPtr            macnet_ptr = macnet_context_ptr->MACNET_ADDRESS;
   VENET_BD_STRUCT_PTR       bd_ptr;
   uchar_ptr buf;

   if (macnet_ptr == NULL) return;

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   MACNET_int_disable();
   
   while (macnet_context_ptr->ActiveRxBDs < macnet_context_ptr->NumRxBDs) {

      bd_ptr = &macnet_context_ptr->MACNET_RX_RING_PTR[macnet_context_ptr->LastRxBD];

      _DCACHE_INVALIDATE_MBYTES((pointer)bd_ptr, sizeof(ENET_BD_STRUCT));
      
      /* Initialize the descriptor and give it to the MACNET */
      buf = (uchar_ptr)HOST_TO_BE_LONG((uint_32)ENET_Dequeue_Buffer((pointer *) &macnet_context_ptr->RX_BUFFERS));
      if (buf == NULL) {
         MACNET_int_enable();
         return;
      }
      
      bd_ptr->BUFFER = buf;
      bd_ptr->LENGTH = 0;

      /*
      ** The processor doesn't clear control bits when errors don't occur;
      ** it just sets bits when errors do occur.  So we clear all the
      ** control bits before enqueueing the descriptor.
      */

      bd_ptr->CONTROL &= HOST_TO_BE_SHORT_CONST(ENET_BD_ETHER_RX_WRAP);

      /*
      ** Add the PCB to the receive PCB queue (linked via PRIVATE) and
      ** increment the tail to the next descriptor
      */
      BD_INC(macnet_context_ptr->LastRxBD, macnet_context_ptr->NumRxBDs);
      macnet_context_ptr->ActiveRxBDs++;

      bd_ptr->CONTROL |= HOST_TO_BE_SHORT_CONST(ENET_BD_ETHER_RX_EMPTY);

      /* In case Enhanced BD are used allow interrupt generation */
      bd_ptr->CONTROL_EXT0 |= HOST_TO_BE_SHORT_CONST(ENET_BD_EXT0_ETHER_RX_GENERATE_INTR);

      _DCACHE_FLUSH_MBYTES((pointer)bd_ptr, sizeof(ENET_BD_STRUCT));  

      /*
      ** Signals the MACNET that empty buffers are available.
      */
      macnet_ptr->RDAR = ENET_RDAR_RDAR_MASK;
   }
   MACNET_int_enable();
} 



#if !ENETCFG_SUPPORT_FRAGMENTED_PCBS

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_discard_current_rx
*  Returned Value : void
*  Comments       :
 *
*
*END*-----------------------------------------------------------------*/

static void MACNET_discard_current_rx(ENET_CONTEXT_STRUCT_PTR    enet_ptr,MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr, boolean last)
{
   uint_32  i;
   

   if (macnet_context_ptr->CurrentRxECB) {
      for (i=0;i<macnet_context_ptr->CurrentRxFrag;i++) {
         ENET_INC_STATS(COMMON.ST_RX_DISCARDED);
         if (macnet_context_ptr->FRAGS[i].FRAGMENT) {
            ENET_Enqueue_Buffer((pointer*)&macnet_context_ptr->RX_BUFFERS, macnet_context_ptr->FRAGS[i].FRAGMENT);
         }
         macnet_context_ptr->FRAGS[i].FRAGMENT = NULL;
      }
   }
   macnet_context_ptr->CurrentRxECB=NULL;
   
   if (last) 
      macnet_context_ptr->CurrentRxFrag=0;
}      


  
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_process_rx_bds
*  Returned Value : void
*  Comments       :
*        Processes received buffers.
*
* NOTE: For MACNET, a frame consists of one or more buffers.
*       For ENET, a packet consists of one or more fragments
*        Therefore, a frame = a packet, and a buffer = a fragment.
*
*
*END*-----------------------------------------------------------------*/
static void MACNET_process_rx_bds
   (
         /* [IN] the Ethernet state structure */
      ENET_CONTEXT_STRUCT_PTR    enet_ptr
   )
{ /* Body */
   MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   VENET_BD_STRUCT_PTR        bd_ptr;
   PCB_PTR                    pcb_ptr=NULL;
   uint_16                    rx_status;
   uint_32                    i,length;
   int_32                     ilength;
   uchar_ptr                  buffer, small_packet, large_packet;
   boolean                    buffer_is_valid, consumed, first, last;
   
   //_DCACHE_INVALIDATE();    // temporary - I can't find fast actual part of memory for cache invalidation

   /* Dequeue all received buffers (fragments) */
   while (macnet_context_ptr->ActiveRxBDs > 0) {

      bd_ptr = &macnet_context_ptr->MACNET_RX_RING_PTR[macnet_context_ptr->NextRxBD];
      _DCACHE_INVALIDATE_MBYTES((pointer)bd_ptr, sizeof(ENET_BD_STRUCT));  

      if ((bd_ptr->CONTROL & HOST_TO_BE_SHORT_CONST(ENET_BD_ETHER_RX_EMPTY)) != 0) {
         break; // Currently being written to by MII
      }  

      BD_INC(macnet_context_ptr->NextRxBD,macnet_context_ptr->NumRxBDs);
      macnet_context_ptr->ActiveRxBDs--;

      ENET_INC_STATS(COMMON.ST_RX_TOTAL);
      rx_status = SHORT_BE_TO_HOST(bd_ptr->CONTROL);
      length = SHORT_BE_TO_HOST(bd_ptr->LENGTH);
      buffer = (uchar_ptr)LONG_BE_TO_HOST((uint_32)(bd_ptr->BUFFER));
      //buffer = LONG_BE_TO_HOST(bd_ptr->BUFFER);
      
      consumed = FALSE;
      first = macnet_context_ptr->CurrentRxFrag == 0;
      last = TRUE;
      
      // Start by validating the received buffer 
      // a buffer is valid if:
      //   (it is error free) AND ( (it is the first buffer in a frame) OR (it is a subsequent buffer in a frame we want ) )
      // 
      buffer_is_valid = FALSE;
      if (rx_status & ENET_BD_ETHER_RX_TRUNCATED) {
      
         ENET_INC_STATS(ST_RX_RUNT);
     
      } else if (rx_status & ENET_BD_ETHER_RX_LAST_FRAME) {
         
         // Some error bits can only be checked on the last buffer
         
         // Overrun needs to be checked first,  If this bit is 
         // set, the other status bits, M, LG, NO, CR, and CL lose their normal meaning and are zero. This bit 
         // is valid only if the L-bit is set.
         if (rx_status & ENET_BD_ETHER_RX_OVERRUN) {
            ENET_INC_STATS(ST_RX_OVERRUN);
         
         } else if (rx_status & ENET_BD_ETHER_RX_LENGTH_VIOLATION) {
            ENET_INC_STATS(ST_RX_GIANT);
            
         } else if (rx_status & ENET_BD_ETHER_RX_NON_OCTET) {
            ENET_INC_STATS(ST_RX_ALIGN);
            
         } else if (rx_status & ENET_BD_ETHER_RX_CRC_ERROR) {
            ENET_INC_STATS(ST_RX_FCS);
         } else {
            #if BSPCFG_ENABLE_ENET_HISTOGRAM
               uint_32 index = length>> ENET_HISTOGRAM_SHIFT;
               
               if (index < ENET_HISTOGRAM_ENTRIES) {
                  ENET_INC_STATS(RX_HISTOGRAM[index]);
               }
            #endif
         
         
            // Good buffer (with Last bit set). 
            buffer_is_valid = TRUE;
            // Remove CRC and compute buffer length
            ilength = length - (ENET_FRAMESIZE_TAIL + (macnet_context_ptr->CurrentRxFrag * enet_ptr->PARAM_PTR->RX_BUFFER_SIZE));
            
            // The last fragment may contain nothing but part of the CRC. If this is the case, length will be <= 0,
            // and if length < 0, the previous fragment contains part of the CRC, which needs to be removed.
            if (ilength < 0) {
               length = 0;
               if (!first) {
                  macnet_context_ptr->FRAGS[macnet_context_ptr->CurrentRxFrag-1].LENGTH += ilength;
               } else {
                  // Should never get here - would mean we received a really small packet which should have already been
                  // caught above. But if we do, discard it.
                  buffer_is_valid = TRUE;
               }
            } else {
               length = (uint_32) ilength;
            }
                  
         }
         
      }  else {
         // Good buffer (without Last bit set). Can only be processed if we are not currently discarding.
         buffer_is_valid = TRUE;
         last = FALSE;
      }   
      
      
         
      if (!buffer_is_valid) {
         // Was a buffer with an error
         ENET_INC_STATS(COMMON.ST_RX_ERRORS);
         MACNET_discard_current_rx(enet_ptr,macnet_context_ptr,last);
         
      } else {
         // If it is the first buffer in the frame, we have to see if the frame is of interest. 
         if (first) {
            // make sure we don't examine a cached copy of the buffer
            _DCACHE_INVALIDATE_MBYTES(buffer, length);  

            // the frame is of interest if there is a receiver registered for the destination and protocol. We can do this on the first buffer
            // of the frame, since  the ethernet header information always fits in  the first buffer of a frame.
            macnet_context_ptr->CurrentRxECB = ENET_find_receiver(enet_ptr, (ENET_HEADER_PTR) buffer, &length);
         }

         
         // If we have an Rx ECB, it means we want this packet
         if (macnet_context_ptr->CurrentRxECB ) {
            if (!first) {
               // If it is not the first packet, the cache is still valid. We could have put the invalidate before the first "if (first)",
               // but that would mean we may invalidate the cache on buffers that we know we are discarding.
               _DCACHE_INVALIDATE_MBYTES(buffer, length);  
            }            
            
            // Add the packet to the list of fragments in the PCB               
            macnet_context_ptr->FRAGS[macnet_context_ptr->CurrentRxFrag].FRAGMENT = buffer;
            macnet_context_ptr->FRAGS[macnet_context_ptr->CurrentRxFrag].LENGTH = length;
            macnet_context_ptr->CurrentRxFrag++;

            consumed = TRUE;

            // Now, there are three cases:
            //  !l - start or mid frame buffer - store it for later coalecing into a large buffer
            //  !f, l - end of multi-buffer packet - coalece into a large buffer
            //   f, l - single buffer packet - ready to process 
            
            if (!last) { 

               // Not the last frame in the packet. Have to make sure there is room for at least one
               // more fragment. If not, the packet is too large
               if (macnet_context_ptr->CurrentRxFrag >= MACNET_MAX_FRAGS) {
                  MACNET_discard_current_rx(enet_ptr,macnet_context_ptr,last);
                  ENET_INC_STATS(RX_FRAGS_EXCEEDED);
               }                     

            } else {  
               // last buffer (may or may not also be the first buffer) so process it

               // There is a receiver, so need to allocate a receive PCB
               QGET(macnet_context_ptr->RxPCBHead, macnet_context_ptr->RxPCBTail, pcb_ptr);
               if (pcb_ptr) {
                  // allocation successful, initialize new receive PCB
                  pcb_ptr->PRIVATE = (pointer)enet_ptr;
                  pcb_ptr->FREE = MACNET_rx_free;
                  
                  // Check to see if it is a large packet
                  if (!first) {
                     // Coalese
                     large_packet = ENET_Dequeue_Buffer((pointer *)&macnet_context_ptr->LARGE_BUFFERS);
                     
                     if (large_packet) {
                        // need to compute the length of this fragment, because the length on a BD with the L bit set
                        // is not the length of the buffer, it is the length of the frame.
                        pcb_ptr->FRAG[0].FRAGMENT = large_packet;
                        pcb_ptr->FRAG[0].LENGTH = 0;
                        pcb_ptr->FREE = MACNET_rx_free_large;

                        for (i=0;i<macnet_context_ptr->CurrentRxFrag;i++) {
                           if (macnet_context_ptr->FRAGS[i].LENGTH ) {
                              _mem_copy(macnet_context_ptr->FRAGS[i].FRAGMENT, large_packet, macnet_context_ptr->FRAGS[i].LENGTH  );
                              large_packet += macnet_context_ptr->FRAGS[i].LENGTH;
                              pcb_ptr->FRAG[0].LENGTH += macnet_context_ptr->FRAGS[i].LENGTH;
                           }
                           ENET_Enqueue_Buffer((pointer*)&macnet_context_ptr->RX_BUFFERS, macnet_context_ptr->FRAGS[i].FRAGMENT);
                           macnet_context_ptr->FRAGS[i].FRAGMENT = NULL;
                        }
                  
                        ENET_INC_STATS(ST_RX_COPY_LARGE);
                     } else {
                        ENET_INC_STATS(RX_LARGE_BUFFERS_EXHAUSTED);
                     
                        MACNET_discard_current_rx(enet_ptr,macnet_context_ptr,last);
                        QADD(macnet_context_ptr->RxPCBHead, macnet_context_ptr->RxPCBTail, pcb_ptr);
                        pcb_ptr=NULL;
                     }                     
                  
                  // Check to see if it is a small packet
                  } else { 
                     pcb_ptr->FRAG[0].LENGTH = length;
                     pcb_ptr->FRAG[0].FRAGMENT = buffer;
                     if (length < MACNET_SMALL_PACKET_SIZE) {
 
                        small_packet = ENET_Dequeue_Buffer((pointer *)&macnet_context_ptr->SMALL_BUFFERS);
                        
                        if (small_packet) {
                           _mem_copy( macnet_context_ptr->FRAGS[0].FRAGMENT, small_packet, length );

                           pcb_ptr->FRAG[0].FRAGMENT = small_packet;
                           pcb_ptr->FREE = MACNET_rx_free_small;

                           consumed = FALSE;

                           ENET_INC_STATS(ST_RX_COPY_SMALL);
                        }
                     }
                  }
                  
                                    
                  if (pcb_ptr) {  
                     pcb_ptr->FRAG[1].LENGTH = 0;
                     pcb_ptr->FRAG[1].FRAGMENT = NULL;

#if ENETCFG_SUPPORT_PTP
                     if (macnet_context_ptr->PTP_PRIV->PTIMER_PRESENT) 
                     {
                        MACNET_ptp_store_rxstamp(enet_ptr, pcb_ptr, bd_ptr);
                     }
#endif /* ENETCFG_SUPPORT_PTP */
                     // Call the receiver's service function to pass the PCB to the receiver
                     macnet_context_ptr->CurrentRxECB->SERVICE(pcb_ptr, macnet_context_ptr->CurrentRxECB->PRIVATE);

                     // and clear the current ECB and PCB info, as it's gone to the receiver now.
                     macnet_context_ptr->CurrentRxECB = NULL;
                     macnet_context_ptr->CurrentRxFrag = 0;
                  }                  
                  
                  
               } else {

                  // Couldn't get a Receive PCB, so need to discard buffers until last buffer in frame.
                  MACNET_discard_current_rx(enet_ptr,macnet_context_ptr,last);
                  ENET_INC_STATS(RX_PCBS_EXHAUSTED);
               }
            }
         }         
      }
      
      
      if (!consumed) {
         ENET_INC_STATS(COMMON.ST_RX_DISCARDED);
         ENET_Enqueue_Buffer((pointer*)&macnet_context_ptr->RX_BUFFERS, buffer);
      }
   }
   MACNET_add_buffers_to_rx_ring(macnet_context_ptr);
} 

#else

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_discard_current_rx
*  Returned Value : void
*  Comments       :
 *
*
*END*-----------------------------------------------------------------*/

static void MACNET_discard_current_rx(ENET_CONTEXT_STRUCT_PTR    enet_ptr,MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr, boolean last)
{
   uint_32  i;
   PCB_PTR pcb_ptr = (PCB_PTR) macnet_context_ptr->CurrentRxPCB;
   
   macnet_context_ptr->CurrentRxECB=NULL; 

   if (pcb_ptr) {
      for (i=0;i<macnet_context_ptr->CurrentRxFrag;i++) {
         ENET_INC_STATS(COMMON.ST_RX_DISCARDED);
         ENET_Enqueue_Buffer((pointer*)&macnet_context_ptr->RX_BUFFERS, pcb_ptr->FRAG[i].FRAGMENT);
      }
      QADD(macnet_context_ptr->RxPCBHead, macnet_context_ptr->RxPCBTail, pcb_ptr);

      macnet_context_ptr->CurrentRxFrag=0;
      macnet_context_ptr->CurrentRxPCB=NULL;
      MACNET_add_buffers_to_rx_ring(macnet_context_ptr);
   }
   
   macnet_context_ptr->RxDiscarding = !last;
}      


  
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_process_rx_bds
*  Returned Value : void
*  Comments       :
*        Processes received buffers.
*
* NOTE: For MACNET, a frame consists of one or more buffers.
*       For ENET, a packet consists of one or more fragments
*        Therefore, a frame = a packet, and a buffer = a fragment.
*
*
*END*-----------------------------------------------------------------*/

static void MACNET_process_rx_bds
   (
         /* [IN] the Ethernet state structure */
      ENET_CONTEXT_STRUCT_PTR    enet_ptr
   )
{ /* Body */
   MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->DEVICE_CONTEXT_PTR;
   VENET_BD_STRUCT_PTR        bd_ptr;
   PCB_PTR                    pcb_ptr=NULL;
   uint_16                    rx_status;
   uint_32                    length,i;
   uchar_ptr                  buffer, small_packet;
   boolean                    buffer_is_valid, processed, last;

   /* Dequeue all received buffers (fragments) */
   while (macnet_context_ptr->ActiveRxBDs > 0) {

      bd_ptr = &macnet_context_ptr->MACNET_RX_RING_PTR[macnet_context_ptr->NextRxBD];
      _DCACHE_INVALIDATE_MBYTES(bd_ptr, sizeof(MACNET_BD_STRUCT));  

      if ((bd_ptr->CONTROL & HOST_TO_BE_SHORT_CONST(MCF5XXX_MACNET_BD_ETHER_RX_EMPTY)) != 0) {
         break; // Currently being written to by MII
      }  

      BD_INC(macnet_context_ptr->NextRxBD,macnet_context_ptr->NumRxBDs);
      macnet_context_ptr->ActiveRxBDs--;

      ENET_INC_STATS(COMMON.ST_RX_TOTAL);
      rx_status = SHORT_BE_TO_HOST(bd_ptr->CONTROL);
      length = SHORT_BE_TO_HOST(bd_ptr->LENGTH);
      buffer = LONG_BE_TO_HOST(bd_ptr->BUFFER);

      processed = FALSE;
      last = TRUE;
      
      // Start by validating the received buffer 
      buffer_is_valid = FALSE;
      if (rx_status & MCF5XXX_MACNET_BD_ETHER_RX_TRUNCATED) {
      
         ENET_INC_STATS(ST_RX_RUNT);
     
      } else if (rx_status & MCF5XXX_MACNET_BD_ETHER_RX_LAST_FRAME) {
         
         // Some error bits can only be checked on the last buffer
         
         // Overrun needs to be checked first,  If this bit is 
         // set, the other status bits, M, LG, NO, CR, and CL lose their normal meaning and are zero. This bit 
         // is valid only if the L-bit is set.
         if (rx_status & MCF5XXX_MACNET_BD_ETHER_RX_OVERRUN) {
            ENET_INC_STATS(ST_RX_OVERRUN);
         
         } else if (rx_status & MCF5XXX_MACNET_BD_ETHER_RX_LENGTH_VIOLATION) {
            ENET_INC_STATS(ST_RX_GIANT);
            
         } else if (rx_status & MCF5XXX_MACNET_BD_ETHER_RX_NON_OCTET) {
            ENET_INC_STATS(ST_RX_ALIGN);
            
         } else if (rx_status & MCF5XXX_MACNET_BD_ETHER_RX_CRC_ERROR) {
            ENET_INC_STATS(ST_RX_FCS);
         } else {
            // Good buffer (with Last bit set). Can only be processed if we are not currently discarding.
            buffer_is_valid = !macnet_context_ptr->RxDiscarding;
         }
         
      }  else {
         // Good buffer (without Last bit set). Can only be processed if we are not currently discarding.
         buffer_is_valid = !macnet_context_ptr->RxDiscarding;
         last = FALSE;
      }      

      if (buffer_is_valid) {
         // make sure we don't examine a cached copy of the buffer
         _DCACHE_INVALIDATE_MBYTES(buffer, length);  

         // Check to see if there is a receiver - if there is, this is not the first buffer of the frame.
         if (macnet_context_ptr->CurrentRxECB==NULL) {
            
            // There wasn't, so it is the first buffer of a frame. Check to see if there is a receiver registered for this packet's 
            // destination and protocol. We can do this on the first buffer of the frame, since this information always fits in 
            // the first buffer of a frame.
            macnet_context_ptr->CurrentRxECB = ENET_find_receiver(enet_ptr, (ENET_HEADER_PTR) buffer, &length);
   
            if (macnet_context_ptr->CurrentRxECB ) {
               // There is a receiver, so need to allocate a receive PCB
               QGET(macnet_context_ptr->RxPCBHead, macnet_context_ptr->RxPCBTail, pcb_ptr);
               if (pcb_ptr) {
                  // allocation successful, initialize new receive PCB
                  macnet_context_ptr->CurrentRxPCB = pcb_ptr;
                  macnet_context_ptr->CurrentRxFrag = 0;
                  pcb_ptr->FREE = MACNET_rx_free;
                  pcb_ptr->PRIVATE = (pointer)enet_ptr;
                  
               } else {
                  // Couldn't get a Receive PCB, so need to discard buffers until last buffer in frame.
                  macnet_context_ptr->RxDiscarding = !last;
                  macnet_context_ptr->CurrentRxECB = NULL;
               }
            
            } else {
               // There isn't a receiver, so need to discard buffers until last buffer in frame.
               macnet_context_ptr->RxDiscarding = !last;
               pcb_ptr = NULL;
            }
            
         
         } else {
            // we are already have a receiving ECB, and therefore a PCB.
            pcb_ptr = macnet_context_ptr->CurrentRxPCB;
         }

         if (pcb_ptr) {
         
            // We have a receiver (ecb) and a pcb, so the packet can be processed.
            processed = TRUE;                  

            // Add the packet to the list of fragments in the PCB               
            pcb_ptr->FRAG[macnet_context_ptr->CurrentRxFrag].FRAGMENT = buffer;
       
            // Check to see if this is the last buffer in the frame
            if (last) {
               // need to compute the length of this fragment, because the length on a BD with the L bit set
               // is not the length of the buffer, it is the length of the frame.
               for (i=0;i<macnet_context_ptr->CurrentRxFrag;i++) {
                  length -= pcb_ptr->FRAG[i].LENGTH;
               }
               // remove CRC
               pcb_ptr->FRAG[macnet_context_ptr->CurrentRxFrag].LENGTH = length - ENET_FRAMESIZE_TAIL;
                
               // Terminate the fragment list in the PCB.
               pcb_ptr->FRAG[macnet_context_ptr->CurrentRxFrag+1].LENGTH = 0;
               pcb_ptr->FRAG[macnet_context_ptr->CurrentRxFrag+1].FRAGMENT = 0;

               // Call the receiver's service function to pass the PCB to the receiver
               macnet_context_ptr->CurrentRxECB->SERVICE(pcb_ptr, macnet_context_ptr->CurrentRxECB->PRIVATE);

               // and clear the current ECB and PCB info, as it's gone to the receiver now.
               macnet_context_ptr->CurrentRxECB = NULL;
               macnet_context_ptr->CurrentRxPCB = NULL;
               macnet_context_ptr->CurrentRxFrag = 0;
            } else {
               pcb_ptr->FRAG[macnet_context_ptr->CurrentRxFrag].LENGTH = length;
               
               macnet_context_ptr->CurrentRxFrag++;
               
               // Not the last frame in the packet. Have to make sure there is room for at least one
               // more fragment. If not, the packet is too large
               if (macnet_context_ptr->CurrentRxFrag >= MACNET_MAX_FRAGS) {
                  MACNET_discard_current_rx(enet_ptr,macnet_context_ptr,last);
               }                     
            }
         }
      } else {
         // Was a buffer with an error
         ENET_INC_STATS(COMMON.ST_RX_ERRORS);
         MACNET_discard_current_rx(enet_ptr,macnet_context_ptr,last);
      } 
      
     
      if (!processed) {
         ENET_INC_STATS(COMMON.ST_RX_DISCARDED);
         ENET_Enqueue_Buffer((pointer*)&macnet_context_ptr->RX_BUFFERS, buffer);
         MACNET_add_buffers_to_rx_ring(macnet_context_ptr);
      }
   } 
} 

#endif                

/*NOTIFIER*-------------------------------------------------------------
*
*  Function Name  : MACNET_RX_ISR
*  Returned Value : void
*  Comments       :
*        Interrupt service routine for Rx.
*
*END*-----------------------------------------------------------------*/

void MACNET_RX_ISR
   (
         /* [IN] the Ethernet state structure */
      pointer  enet
   )
{ /* Body */

   ENET_CONTEXT_STRUCT_PTR    enet_ptr           = (ENET_CONTEXT_STRUCT_PTR)enet;
   MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   ENET_MemMapPtr             macnet_ptr         = macnet_context_ptr->MACNET_ADDRESS;
   uint_32                    events;

   if (macnet_ptr == NULL) return;

   events = macnet_ptr->EIR;
   while (events & (ENET_EIR_RXF_MASK | ENET_EIR_RXB_MASK)) {

      /* clear the RX interrupt */
      macnet_ptr->EIR = (ENET_EIR_RXF_MASK | ENET_EIR_RXB_MASK); 
      MACNET_process_rx_bds(enet_ptr);

      events = macnet_ptr->EIR;
   }
} 


/* EOF */
