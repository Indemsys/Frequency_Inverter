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
* $FileName: rtcspcb.c$
* $Version : 3.5.9.0$
* $Date    : Nov-24-2009$
*
* Comments:
*
*   This file contains the PCB (Packet Control Block)
*   library.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#define PCBLOG(x) /*printf x*/


/*
** Note:  The following discussion deals with two different types of data
** structures.  A 'PCB' is a very simple structure consisting of a 'free'
** function and a sequence of fragments.  An 'RTCSPCB' is a larger
** structure used by RTCS to hold extra information for 'PCB's.
**
** For PCBs generated on the local host, RTCS calls RTCSPCB_alloc_send(),
** and PCB_PTR points to the PCB field.  The PCB is initialized with only
** one fragment, which is stored in the BUFFER array.
**
** For PCBs arriving from the link layer, RTCS calls RTCSPCB_alloc_recv(),
** and PCB_PTR points the the received PCB.  Here, the PCB, FRAG and BUFFER
** fields of the RTCSPCB are unused.
**
** In both cases, PCB_PTR points to the "actual" PCB.
**
** Some terminology:  RTCSPCBs where PCB_PTR points to the PCB field will
** be called S-PCBs (since they're allocated with RTCSPCB_alloc_send()),
** and RTCSPCBs where PCB_PTR points to an external PCB will be called
** R-PCBs (since they're allocated with RTCSPCB_alloc_recv()).
**
** Now, there exist situations (e.g. IP forwarding) where we want to
** convert an R-PCB to an S-PCB.  In order to do this, we cut the R-PCB's
** first fragment at the current layer, and prepend a fragment for the
** new headers.  This is done by RTCSPCB_fork().  Note that we must prepend
** a fragment:  although there was room in the R-PCB's first fragment for
** headers, we can't use it in case the new headers turn out to be larger
** than the old ones.
**
** Additionally, there are situations where we want to duplicate an RTCSPCB
** (e.g. broadcasting and multicasting).  RTCSPCB_alloc_dup() will allocate
** an S-PCB containing the same data as the original.
**
*/


static void RTCSPCB_free_internal(PCB_PTR);


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_construct
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Construct an RTCSPCB.
*
*END*-----------------------------------------------------------------*/

static int_32 RTCSPCB_construct
   (
      pointer block
   )
{ 
   RTCSPCB_PTR rtcs_pcb = block;

   rtcs_pcb->PCB.FREE    = RTCSPCB_free_internal;
   rtcs_pcb->PCB.PRIVATE = rtcs_pcb;

   rtcs_pcb->DUPPTR = NULL;
   RTCS_sem_init(&rtcs_pcb->DUPCOUNT);

   rtcs_pcb->PCB_ORIG = NULL;

   rtcs_pcb->FREE_FRAG_BITFIELD = 0;

   rtcs_pcb->UDP_REQUEST = NULL;

   return RTCS_OK;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_destroy
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Destroy an RTCSPCB.
*
*END*-----------------------------------------------------------------*/

static int_32 RTCSPCB_destroy
   (
      pointer block
   )
{ 
   RTCSPCB_PTR rtcs_pcb = block;

   RTCS_sem_destroy(&rtcs_pcb->DUPCOUNT);

   return RTCS_OK;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_init
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the PCB module.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_init
   (
      void
   )
{
   RTCS_DATA_PTR    RTCS_data_ptr;

   RTCS_data_ptr = RTCS_get_data();

   RTCS_data_ptr->RTCS_PCB_partition =
      RTCS_part_create(sizeof(RTCSPCB),_RTCSPCB_init, _RTCSPCB_grow, _RTCSPCB_max,
         RTCSPCB_construct, RTCSPCB_destroy);
   if (RTCS_data_ptr->RTCS_PCB_partition == 0) {
      return RTCSERR_PCB_ALLOC;
   } /* Endif */

   return RTCS_OK;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc
*  Returned Value : pcb_ptr or NULL
*  Comments       :
*         Allocate an RTCSPCB
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc
   (
      void
   )
{ 
   RTCS_DATA_PTR     RTCS_data_ptr;
   RTCSPCB_PTR       rtcs_pcb;

   RTCS_data_ptr = RTCS_get_data();

   rtcs_pcb = RTCS_part_alloc(RTCS_data_ptr->RTCS_PCB_partition);

   if (rtcs_pcb != NULL) {
      rtcs_pcb->IP_SUM_PTR = NULL;
      _mem_zero(&rtcs_pcb->LINK_OPTIONS, sizeof(rtcs_pcb->LINK_OPTIONS));
   } /* Endif */

   RTCSLOG_PCB_ALLOC(rtcs_pcb);

   return rtcs_pcb;

}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc_send
*  Returned Value : pointer to PCB
*  Comments       :
*        Allocates a PCB for sending.
*        Caller must still fill in FRAG[n] and SIZE.
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc_send
   (
      void
   )
{ 
   RTCSPCB_PTR          rtcs_pcb;
   PCB_FRAGMENT _PTR_   pcb_frag_ptr;

   rtcs_pcb = RTCSPCB_alloc();

   if (rtcs_pcb != NULL) {

      rtcs_pcb->PCBPTR = &rtcs_pcb->PCB;

      pcb_frag_ptr = rtcs_pcb->PCB.FRAG;
      pcb_frag_ptr->LENGTH   = 0;

      // grows down from end of buffer 
      pcb_frag_ptr->FRAGMENT = &rtcs_pcb->HEADER_FRAG_BUFFER[RTCSPCB_DATA_MAX];
      
      // The following code assumed that PCB immediately followed BUFFER in RTCS_PCB data structure
      /* grows down into BUFFER */
      // pcb_frag_ptr->FRAGMENT = (uchar_ptr)&pcb->PCB;  
      
      pcb_frag_ptr++;
      pcb_frag_ptr->LENGTH   = 0;
      pcb_frag_ptr->FRAGMENT = NULL;

      rtcs_pcb->HEADER_FRAG_USED = 0;
      rtcs_pcb->HEADER_FRAG_FREE = RTCSPCB_DATA_MAX;
      rtcs_pcb->NUM_FRAGS = 1;
      
      RTCSPCB_SIZE(rtcs_pcb) = 0;
      RTCSPCB_DATA(rtcs_pcb) = rtcs_pcb->PCB.FRAG[0].FRAGMENT;

      rtcs_pcb->TYPE = RTCSPCB_TYPE_ONLYCAST;

      rtcs_pcb->PCB_FREE = NULL;
   }

   return rtcs_pcb;

} 



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc_recv
*  Returned Value : pointer to PCB
*  Comments       :
*        Allocates a PCB for receiving.
*
* 
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc_recv
   (
      PCB_PTR  inpcb
         /* [IN] received packet */
   )
{ 
   RTCSPCB_PTR      rtcs_pcb;
   PCB_FRAGMENT_PTR frag;

   rtcs_pcb = RTCSPCB_alloc();

   if (rtcs_pcb != NULL) {

      rtcs_pcb->PCBPTR  = inpcb;

      rtcs_pcb->HEADER_FRAG_USED = inpcb->FRAG[0].LENGTH;
      rtcs_pcb->HEADER_FRAG_FREE = 0;
      
      // We are not using the PCB contained in the RTCS_PCB, so
      // NUM_FRAGS is set to 0 to indicate this.
      rtcs_pcb->NUM_FRAGS = 0;   
      RTCSPCB_SIZE(rtcs_pcb) = 0;
      RTCSPCB_DATA(rtcs_pcb) = inpcb->FRAG[0].FRAGMENT;

      for (frag = inpcb->FRAG; frag->LENGTH; frag++) {
         RTCSPCB_SIZE(rtcs_pcb) += frag->LENGTH;
      } 
      
      rtcs_pcb->PCB_FREE    = inpcb->FREE;
      rtcs_pcb->PCB_PRIVATE = inpcb->PRIVATE;
      rtcs_pcb->PCB_BUFFER  = inpcb->FRAG[0];
      inpcb->FREE      = RTCSPCB_free_internal;
      inpcb->PRIVATE   = rtcs_pcb;
   } 
   
   return rtcs_pcb;

} 



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_alloc_dup
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Duplicate a PCB.
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR RTCSPCB_alloc_dup
   (
      RTCSPCB_PTR    orig_rtcs_pcb
         /* [IN] packet to duplicate */
   )
{ 
   RTCSPCB_PTR rtcs_pcb;

   rtcs_pcb = RTCSPCB_alloc();

   if (rtcs_pcb != NULL) {
      RTCS_sem_destroy(&rtcs_pcb->DUPCOUNT);

      _mem_copy((uchar_ptr)orig_rtcs_pcb, (uchar_ptr)rtcs_pcb, sizeof(RTCSPCB));
      rtcs_pcb->PCB.PRIVATE = rtcs_pcb;

      rtcs_pcb->PCBPTR = &rtcs_pcb->PCB;

      if (orig_rtcs_pcb->PCBPTR == &orig_rtcs_pcb->PCB) {

         /* Packet originated on this host */

         RTCSPCB_DATA(rtcs_pcb) = rtcs_pcb->HEADER_FRAG_BUFFER + (RTCSPCB_DATA(orig_rtcs_pcb) - orig_rtcs_pcb->HEADER_FRAG_BUFFER);

         if (rtcs_pcb->IP_SUM_PTR) {
            rtcs_pcb->IP_SUM_PTR = rtcs_pcb->HEADER_FRAG_BUFFER + (orig_rtcs_pcb->IP_SUM_PTR - orig_rtcs_pcb->HEADER_FRAG_BUFFER);
         } 

      } else {
         _mem_copy(orig_rtcs_pcb->PCBPTR->FRAG, rtcs_pcb->PCB.FRAG, sizeof(PCB_FRAGMENT)*2);

      } 

      rtcs_pcb->PCB_FREE = NULL;
      rtcs_pcb->PCB_ORIG = NULL;

      RTCS_sem_init(&rtcs_pcb->DUPCOUNT);
      RTCSPCB_depend(rtcs_pcb, orig_rtcs_pcb);  /* can't fail because we allocated pcb */

      rtcs_pcb->UDP_REQUEST = NULL;
      rtcs_pcb->FREE_FRAG_BITFIELD = 0;

      PCBLOG(("\nPCB: Alloc_dup: returning %p, duplicate of %p", rtcs_pcb, orig_rtcs_pcb));
   } 
   return rtcs_pcb;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_depend
*  Returned Value : uint_32
*  Comments       :
*        Adds a dependency to another PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_depend
   (
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] pcb that depends on another */
      RTCSPCB_PTR    orig_rtcs_pcb
         /* [IN] pcb that is given a dependency */
   )
{
   
   if (rtcs_pcb->DUPPTR) {
      return RTCSERR_PCB_DEPEND;
   } 

   rtcs_pcb->DUPPTR   = orig_rtcs_pcb;
   RTCS_sem_post(&orig_rtcs_pcb->DUPCOUNT);

   return RTCS_OK;
} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_fork
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Prepends a fragment to a received PCB to allow it to be sent.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_fork
   (
      RTCSPCB_PTR    rtcs_pcb
         /* [IN] packet to fork */
   )
{ 
   PCB_PTR            rx_pcb_ptr;
   uint_32           frag;

   if (rtcs_pcb->NUM_FRAGS != 0) {
      PCBLOG(("\nPCB: Fork: can't fork %p", rtcs_pcb));
      return RTCSERR_PCB_FORK;
   } 

   rx_pcb_ptr = rtcs_pcb->PCBPTR; 
   if (rx_pcb_ptr == &rtcs_pcb->PCB) {
      PCBLOG(("\nPCB: Fork: can't fork %p", rtcs_pcb));
      return RTCSERR_PCB_FORK;
   } 
   

   if (!rtcs_pcb->DUPPTR) {
      rx_pcb_ptr->FREE    = rtcs_pcb->PCB_FREE;
      rx_pcb_ptr->PRIVATE = rtcs_pcb->PCB_PRIVATE;
      rx_pcb_ptr->FRAG[0] = rtcs_pcb->PCB_BUFFER;  // TBD - check this
      rtcs_pcb->PCB_ORIG = rx_pcb_ptr;
   } 
   rtcs_pcb->PCB_FREE = NULL;

   rtcs_pcb->PCBPTR = &rtcs_pcb->PCB;

   rtcs_pcb->PCB.FRAG[0].LENGTH   = 0;
   // grows down from end of buffer 
   rtcs_pcb->PCB.FRAG[0].FRAGMENT = &rtcs_pcb->HEADER_FRAG_BUFFER[RTCSPCB_DATA_MAX];

   rtcs_pcb->PCB.FRAG[1].LENGTH   = rtcs_pcb->HEADER_FRAG_USED;
   rtcs_pcb->PCB.FRAG[1].FRAGMENT = RTCSPCB_DATA(rtcs_pcb);
 
   frag = 0;
   do { 
      frag++;
      rtcs_pcb->PCB.FRAG[frag+1].LENGTH   = rx_pcb_ptr->FRAG[frag].LENGTH;
      rtcs_pcb->PCB.FRAG[frag+1].FRAGMENT = rx_pcb_ptr->FRAG[frag].FRAGMENT;
   } while (rx_pcb_ptr->FRAG[frag].FRAGMENT); 
   
   rtcs_pcb->HEADER_FRAG_USED = 0;
   rtcs_pcb->HEADER_FRAG_FREE = RTCSPCB_DATA_MAX;
   rtcs_pcb->NUM_FRAGS = frag+1;
   RTCSPCB_DATA(rtcs_pcb) = rtcs_pcb->PCB.FRAG[0].FRAGMENT;

   PCBLOG(("\nPCB: Fork: forked %p", rtcs_pcb));
   return RTCS_OK;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_free_internal
*  Returned Value : void
*  Comments       :
*        Frees an allocated PCB.
*
*END*-----------------------------------------------------------------*/

static void RTCSPCB_free_internal
   (
      PCB_PTR   inpcb
         /* [IN] packet to free */
   )
{ 
   RTCSPCB_PTR rtcs_pcb = (RTCSPCB_PTR)inpcb->PRIVATE;

   /* Can't free this PCB until all duplicates are freed */
   if (RTCS_sem_trywait(&rtcs_pcb->DUPCOUNT) == RTCS_OK) {
      PCBLOG(("\nPCB: Free_internal: delaying %p", rtcs_pcb));
      return;
   } 

   /* If this PCB is a duplicate, decrement the original's refcount */
   if (rtcs_pcb->DUPPTR) {
      PCBLOG(("\nPCB: Free_internal: about to free %p", rtcs_pcb->DUPPTR));
      RTCSPCB_free(rtcs_pcb->DUPPTR);
      rtcs_pcb->DUPPTR = NULL;
   } 

   /* If this PCB has been forked, don't forget the link layer's PCB */
   if (rtcs_pcb->PCB_ORIG) {
      PCB_free(rtcs_pcb->PCB_ORIG);
      rtcs_pcb->PCB_ORIG = NULL;
   } 

   /* handle the FREE_FRAG_FIELD */
   if (rtcs_pcb->FREE_FRAG_BITFIELD) {
      uint_32  i;
      for (i = 0; i < RTCSPCB_FRAG_MAX; i++) {
         if (rtcs_pcb->FREE_FRAG_BITFIELD & (1 << i)) {
            _mem_free(rtcs_pcb->PCBPTR->FRAG[i].FRAGMENT);
         } 
      } 
      rtcs_pcb->FREE_FRAG_BITFIELD = 0;
   } 

   if (rtcs_pcb->PCB_FREE) {
      inpcb->FREE    = rtcs_pcb->PCB_FREE;
      inpcb->PRIVATE = rtcs_pcb->PCB_PRIVATE;
      inpcb->FRAG[0] = rtcs_pcb->PCB_BUFFER;
      PCBLOG(("\nPCB: Free_internal: PCB is %p", inpcb));
      PCB_free(inpcb);
   } 

   if (rtcs_pcb->UDP_REQUEST) {
      RTCS_cmd_complete(rtcs_pcb->UDP_REQUEST, RTCS_OK);
      rtcs_pcb->UDP_REQUEST = NULL;
   } 

   RTCS_part_free(rtcs_pcb);

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_insert_header
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Prepend a layer header in a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_insert_header
   (
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] packet to change */
      uint_32        size
         /* [IN] size of new header */
   )
{ 
   uchar_ptr data_ptr;

   if (size > rtcs_pcb->HEADER_FRAG_FREE) {
      PCBLOG(("\nPCB: Insert: out of space in %p (%d/%d)", rtcs_pcb, size, rtcs_pcb->FRAGFREE));
      return RTCSERR_PCB_BUFFER;
   } 

   rtcs_pcb->HEADER_FRAG_FREE     -= size;
   rtcs_pcb->HEADER_FRAG_USED     += size;
   RTCSPCB_SIZE(rtcs_pcb) += size;

   data_ptr = RTCSPCB_DATA(rtcs_pcb);
   data_ptr -= size;
   RTCSPCB_DATA(rtcs_pcb) = data_ptr;

   PCBLOG(("\nPCB: Insert: inserted %d bytes in %p", size, rtcs_pcb));
   return RTCS_OK;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_next
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Advance a layer header in a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_next
   (
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] packet to change */
      uint_32        size
         /* [IN] size of last header */
   )
{ 
   uchar_ptr data_ptr;

   if (size > rtcs_pcb->HEADER_FRAG_USED) {
      PCBLOG(("\nPCB: Next: out of space in %p (%d/%d)", rtcs_pcb, size, rtcs_pcb->FRAGUSED));
      return RTCSERR_PCB_BUFFER;
   } 

   rtcs_pcb->HEADER_FRAG_FREE     += size;
   rtcs_pcb->HEADER_FRAG_USED     -= size;
   RTCSPCB_SIZE(rtcs_pcb) -= size;

   data_ptr = RTCSPCB_DATA(rtcs_pcb);
   data_ptr += size;
   RTCSPCB_DATA(rtcs_pcb) = data_ptr;

   PCBLOG(("\nPCB: Next: skipped %d bytes in %p", size, rtcs_pcb));
   return RTCS_OK;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_shrink
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Shrinks the first fragment of a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_shrink
   (
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] packet to change */
      uint_32        size
         /* [IN] size to remove */
   )
{ 

   if (size > rtcs_pcb->HEADER_FRAG_USED) {
      PCBLOG(("\nPCB: Shrink: out of space in %p (%d/%d)", rtcs_pcb, size, rtcs_pcb->HEADER_FRAG_USED));
      return RTCSERR_PCB_BUFFER;
   } 

   rtcs_pcb->HEADER_FRAG_USED     -= size;
   RTCSPCB_SIZE(rtcs_pcb) -= size;

   PCBLOG(("\nPCB: Shrink: shrunk %d bytes in %p", size, rtcs_pcb));
   return RTCS_OK;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_append_fragment_autofree
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Adds a fragment to a PCB, and get RTCSPCB_free to _mem_free()
*        it when called.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_append_fragment_autofree
   (
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] packet to change */
      uint_32        size,
         /* [IN] size of new fragment */
      uchar_ptr      frag
   )
{ 
   uint_32 lastfrag = rtcs_pcb->NUM_FRAGS;
   uint_32 error = RTCSPCB_append_fragment(rtcs_pcb, size, frag);

   if (error == RTCS_OK) {
      rtcs_pcb->FREE_FRAG_BITFIELD |= 1 << lastfrag;
   } 
   return error;

} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_append_fragment
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Adds a fragment to a PCB.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCSPCB_append_fragment
   (
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] packet to change */
      uint_32        size,
         /* [IN] size of new fragment */
      uchar_ptr      frag
         /* [IN] pointer to new fragment */
   )
{ 

   if (rtcs_pcb->NUM_FRAGS == 0) {
      PCBLOG(("\nPCB: Frag: must fork %p", rtcs_pcb));
      return RTCSERR_PCB_NOFRAG;
   } 

   if (rtcs_pcb->NUM_FRAGS >= RTCSPCB_FRAG_MAX) {
      PCBLOG(("\nPCB: Frag: out of fragments in %p", rtcs_pcb));
      return RTCSERR_PCB_FRAG;
   } 

   rtcs_pcb->PCBPTR->FRAG[rtcs_pcb->NUM_FRAGS].LENGTH   = size;
   rtcs_pcb->PCBPTR->FRAG[rtcs_pcb->NUM_FRAGS].FRAGMENT = frag;
   RTCSPCB_SIZE(rtcs_pcb) += size;
   rtcs_pcb->NUM_FRAGS++;
   rtcs_pcb->PCBPTR->FRAG[rtcs_pcb->NUM_FRAGS].LENGTH   = 0;
   rtcs_pcb->PCBPTR->FRAG[rtcs_pcb->NUM_FRAGS].FRAGMENT = NULL;

   PCBLOG(("\nPCB: Frag: added %d bytes to %p", size, rtcs_pcb));
   return RTCS_OK;

}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCSPCB_memcopy
*  Returned Value : void
*  Comments       :
*        Copies data out from a PCB.
*
*END*-----------------------------------------------------------------*/

void RTCSPCB_memcopy
   (
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] packet to copy */
      pointer        dest,
         /* [IN] destination */
      uint_32        offset,
         /* [IN] offset into PCB */
      uint_32        dest_size
         /* [IN] number of bytes to copy */
   )
{ 
   PCB_FRAGMENT_PTR  frag_ptr = rtcs_pcb->PCBPTR->FRAG;
   uchar_ptr         src_ptr  = RTCSPCB_DATA(rtcs_pcb);
   uchar_ptr         dest_ptr = dest;
   uint_32           src_size = rtcs_pcb->HEADER_FRAG_USED;

   while (src_size < offset) {

      offset -= src_size;
      frag_ptr++;
      src_ptr  = frag_ptr->FRAGMENT;
      src_size = frag_ptr->LENGTH;

   }

   src_ptr  += offset;
   src_size -= offset;

   while (src_size < dest_size) {

      _mem_copy(src_ptr, dest_ptr, src_size);
      dest_ptr  += src_size;
      dest_size -= src_size;
      frag_ptr++;
      src_ptr  = frag_ptr->FRAGMENT;
      src_size = frag_ptr->LENGTH;

   } 

   _mem_copy(src_ptr, dest_ptr, dest_size);

} 


/* EOF */
