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
* $FileName: tcp_buf.c$
* $Version : 3.8.10.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   Buffer management routines for the TCP layer.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"      /* for TCP definitions */
#include "tcp_prv.h"    /* for TCP internal definitions */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Copy_to_ring
* Returned Values : error status
* Comments        :
*
*  General purpose routine to copy a block of memory
*   (`data',`size') to a circular block of memory (`ring',`rsize')
*   starting at offset `ringofs' from beginning of ring (may be negative
*   downto -rsize).
*
*  Note: it is up to the calling function to update the ring buffer
*        data length, and to ensure that enough space exists in the ring.
*
*END*-----------------------------------------------------------------*/

uint_32 TCP_Copy_to_ring
   (
            uchar _PTR_ data,    /* IN - source data buffer */
            uint_32     size,    /* IN - size of data */
   register int_32      ringofs, /* IN - where in ring to put data */
            uchar _PTR_ ring,    /* IN - start of ring buffer */
            uint_32     rsize    /* IN - total size of ring buffer */
   )
{ /* Body */
   register uint_32 atend;   /* nb of bytes available at end of ring */

   if ( rsize == 0 ) {
      RTCS_log_error(ERROR_TCP, RTCSERR_TCP_CTR_ZERO_RSIZE, 0, 0, 0);
      return RTCS_OK;
   } /* Endif */

   /*
   ** Bring `ringofs' within ring
   */
   while ( ringofs < 0 ) {
      ringofs += rsize;
   } /* Endwhile */

   while ( ringofs >= rsize ) {
      ringofs -= rsize;
   } /* Endwhile */

   atend = rsize - ringofs;

   /*
   ** Copy the data.  If the data will over-run the end of
   ** the ring, copy in two parts.
   */
   if ( size > atend ) {
      _mem_copy(data, ring + ringofs, atend);
      _mem_copy(data + atend, ring, size - atend);
   } else {
      _mem_copy(data, ring + ringofs, size);
   } /* Endif */

   return RTCS_OK;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Copy_from_ring
* Returned Values : error status
* Comments        :
*
*  Complement of TCP_Copy_to_ring();
*
*  Copies `size' bytes of data starting at offset `ringofs' of circular
*   buffer `ring' of size `rsize', into `data'.  Note that the ring buffer
*   must be on-board.
*
*END*-----------------------------------------------------------------*/

uint_32 TCP_Copy_from_ring
   (
            uchar _PTR_ data,    /* IN - destination buffer */
            uint_32     size,    /* IN - size of data at ringofs */
   register int_32      ringofs, /* IN - offset within ring buffer */
            uchar _PTR_ ring,    /* IN - source ring buffer */
            uint_32     rsize    /* IN - ring buffer total size */
   )
{ /* Body */
   register uint_32 atend;   /* nb of bytes available at end of ring */

   if ( rsize == 0 ) {
      RTCS_log_error(ERROR_TCP, RTCSERR_TCP_CTR_ZERO_RSIZE, 1, 0, 0);
      return RTCS_OK;
   } /* Endif */

   /*
   ** Bring `ringofs' within ring
   */
   while ( ringofs < 0 ) {
      ringofs += rsize;
   } /* Endwhile */

   while ( ringofs >= rsize ) {
      ringofs -= rsize;
   } /* Endwhile */

   atend = rsize - ringofs;
   if ( size > atend ) {
      _mem_copy(ring + ringofs, data, atend);
      _mem_copy(ring, data + atend, size - atend);
   } else {
      _mem_copy(ring + ringofs, data, size);
   } /* Endif */

   return RTCS_OK;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Copy_from_buffer
* Returned Values : error status
* Comments        :
*
*  Copies `size' bytes, to `data', starting at `offset' bytes from the
*  start of the transmit buffer of the given TCB.
*
*  If the transmit buffer is too short, only the available bytes are
*  copied.
*
*  Note: This function is only called within TCP_Send_packet().  Thus,
*  pcb_ptr is always allocated with RTCSPCB_alloc_send().
*
*END*-----------------------------------------------------------------*/

uint_32 TCP_Copy_from_buffer
   (
      TCB_STRUCT_PTR tcb,     /* IN - TCP context */
      uchar_ptr      data,    /* IN - destination buffer */
      uint_32_ptr    size_ptr,/* IN/OUT - number of bytes to copy */
      uint_32        offset,  /* IN - offset from start of transmit buffer */
      RTCSPCB_PTR    pcb_ptr  /* IN - PCB used to send the packet */
   )
{ /* Body */
   SbufNode _PTR_ buf = tcb->sndbuf;
   uint_32        ring_tail;
   int_32         ring_offs;
   uint_32        size = *size_ptr;

   /*
   ** Skip untouched buffer nodes
   */
   while ( buf != NULL && offset >= buf->size ) {
      offset -= buf->size;
      buf = buf->next;
   } /* Endwhile */

   if ( buf != NULL && size != 0 ) {

      if (size > buf->size - offset) size = buf->size - offset;

      if ( buf->use_cbuf ) {

         /*
         ** User buffer is in the SbufNode.  Use a second fragment
         ** to point to it.
         */
         RTCSPCB_append_fragment(pcb_ptr, size, buf->data + offset);

      } else {

         /*
         ** User buffer is in the send window.  Use one or two fragments
         ** to point to the ring (we need two fragments if the data
         ** wraps at the end of the ring).
         */
         ring_offs = (int_32)buf->data + offset;
         while (ring_offs <  0)               ring_offs += tcb->sndringlen;
         while (ring_offs >= tcb->sndringlen) ring_offs -= tcb->sndringlen;
         ring_tail = tcb->sndringlen - ring_offs;
         if (size > ring_tail) {
            RTCSPCB_append_fragment(pcb_ptr, ring_tail, tcb->sndringbuf + ring_offs);
            RTCSPCB_append_fragment(pcb_ptr, size - ring_tail, tcb->sndringbuf);
         } else {
            RTCSPCB_append_fragment(pcb_ptr, size , tcb->sndringbuf + ring_offs);
         } /* Endif */

      } /* Endif */

   } /* Endif */

   *size_ptr = size;
   return RTCS_OK;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Copy_to_Rbuffer
* Returned Values : error status
* Comments        :
*
*  Copies `data' to `section' of the circular receive buffer (only where not
*   already filled); `section' is an offset from the start of the receive
*   buffer's current position (i.e., rcvbuf+rcvpos), and may wrap around the
*   buffer.
*
*  Assumes specified `size' bytes are properly contained within the
*   receive buffer.
*
*END*-----------------------------------------------------------------*/

uint_32 TCP_Copy_to_Rbuffer
   (
      TCB_STRUCT_PTR       tcb,        /* IN/OUT - TCP context */
      uint_32              section,    /* IN     - destination offset from
                                                    rcvpos in rcvbuf */
      uchar _PTR_          data,       /* IN     - source data */
      uint_32              size,       /* IN     - size of data */
      uchar _PTR_          rcvbuf,     /* IN     - receive buffer */
      uint_32              rcvbufsize, /* IN     - total size of rcv buffer */
      uint_32              rcvpos,     /* IN     - current position in rcvbuf */
      TCP_CFG_STRUCT_PTR   tcp_cfg     /* IN/OUT - TCP layer data */
   )
{ /* Body */
   Rchunk   _PTR_ h;
   Rchunk   _PTR_ last;
   Rchunk   _PTR_ nxt;
   uint_32  status;
   uint_32  mid_status;

   if ( size == 0 ) {
      return RTCS_OK;
   } /* Endif */

   /*
   ** Skip preceding chunk nodes
   */
   for ( last = NULL, h = tcb->rcvchunks;
       h != NULL && (h->used + h->size) < section;
       last = h, h = h->next)
   {
   } /* Endfor */

   /*
   ** See whether the current chunk node merges with the added section
   */
   if ( h != NULL && h->used <= section ) {  /* extend previous chunk */

      /*
      ** Merge, back-extend section to chunk node
      */
      size    += section - h->used;
      data    -= section - h->used;
      section = h->used;

      if ( h->size >= size ) {

         /*
         ** Have already copied these seq #'s to buf
         */
         return RTCS_OK;

      } /* Endif */

      status = RTCS_OK;

   } else if ( h != NULL && h->used <= section + size ) {

      /*
      ** Some duplicate data so extend next chunk backwards
      */
      status = TCP_Copy_to_ring( data,          /* source */
                           h->used - section,   /* size */
                           rcvpos + section,    /* offset */
                           rcvbuf,
                           rcvbufsize );

      /*
      ** Merge, back-extend chunk node to section
      */
      h->size += h->used - section;
      h->used = section;

   } else {

      /*
      ** Nothing merges with added section, create a new chunk node
      */
      status = TCP_Copy_to_ring( data,          /* source */
                           size,                /* size */
                           rcvpos + section,    /* offset */
                           rcvbuf,
                           rcvbufsize );

      if ( tcp_cfg->Rchunkfree != NULL ) {
         h = tcp_cfg->Rchunkfree;
         tcp_cfg->Rchunkfree = h->next;
      } else {
         h = (Rchunk _PTR_)RTCS_mem_alloc(sizeof(Rchunk));
         if (h == NULL)  {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_MISSED++);
            RTCS_log_error( ERROR_TCP, RTCSERR_TCP_PR_OUT_OF_MEM, (uint_32)tcb, 0, 0 );
            return (uint_32) RTCS_ERROR;
         }
         _mem_set_type(h, MEM_TYPE_Rchunk);
         
      } /* Endif */

      h->size = size;
      h->used = section;

      if ( last != NULL ) {
         h->next = last->next;
         last->next = h;
      } else {
         h->next = tcb->rcvchunks;
         tcb->rcvchunks = h;
      } /* Endif */

      return status;

   } /* Endif */

   /*
   ** A chunk node, `h', was merged with the added section of data;
   **   this node now coincides with the beginning of the added `section';
   **   more nodes may merge, merge them now:
   */
   nxt = h->next;
   while ( nxt != NULL && nxt->used <= h->used+size ) {

      /*
      ** Merge node - copy gap between end of current RcvChunk
      **    and start of next.
      ** don't allow further copies if an error occurred,
      **    but keep ring in sync.
      */
      if ( status == RTCS_OK ) {
         status = TCP_Copy_to_ring( data + h->size,            /* source */
                              nxt->used - h->used - h->size,   /* size */
                              rcvpos + section + h->size,      /* offset */
                              rcvbuf,
                              rcvbufsize );
      } /* Endif */

      h->size = nxt->used - h->used + nxt->size;
      h->next = nxt->next;

      mid_status = _mem_free(nxt);
      if ( mid_status != MQX_OK ) {
         RTCS_log_error( ERROR_TCP, RTCSERR_TCP_DEALLOC_FAILED,
                              mid_status, (uint_32)nxt, 0 );
      } /* Endif */

      nxt = h->next;

   } /* Endwhile */

   /*
   ** Copy any remainder:
   */
   if ( h->size < size ) {

      if ( status == RTCS_OK ) {
         status = TCP_Copy_to_ring( data + h->size,
                              size - h->size,
                              rcvpos + section + h->size,
                              rcvbuf,
                              rcvbufsize );
      } /* Endif */

      h->size = size;

   } /* Endif */

   return status;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Use_ringbuffer
* Returned Values : error status
* Comments        :
*
*  Switch from using user's buffer (because it was larger than ring buffer)
*   to using the ring buffer (since it has now more free space?); any
*   out-of-order (non-contiguous) data is copied from user buffer to ring
*   buffer; if there is too much to fit in the ring buffer, it is truncated
*   (this shouldn't normally happen, except when definitively closing a
*    connection (via TCP_Close_TCB())).
*
*  Note that this routine assumes that all contiguous data in the
*     user buffer is captured in the first receive chunk.
*
*END*-----------------------------------------------------------------*/

uint_32  TCP_Use_ringbuffer
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   Rchunk   _PTR_ chunk;
   Rchunk   _PTR_ nextchunk;
   int_32   n;                /* nb of bytes which remain in user buf */

   if ( tcb->rcvbuf != tcb->rcvuserbuf ) {
      return RTCS_OK;
   } /* Endif */

   tcb->rcvbuf = tcb->rcvringbuf;
   tcb->rcvlen = tcb->rcvringlen;
   tcb->rcvpos = 0;  /* ring buffer empty */

   chunk = tcb->rcvchunks;
   if ( chunk != NULL && chunk->used == 0 ) {

      n = chunk->size;

      /*
      ** Remove leading chunk, whose data will remain in user buffer only
      */
      tcb->rcvchunks = chunk->next;
      _mem_free(chunk);
      chunk = tcb->rcvchunks;

   } else {
      n = 0;
   } /* Endif */

   tcb->rcvusernxt = n;
   tcb->rcvbufseq += n;

   /*
   ** Copy all chunks that fit in the receive ring buffer
   */
   while ( chunk != NULL ) {

      chunk->used -= n;
      if ( chunk->used < tcb->rcvlen ) {

         if ( chunk->size > tcb->rcvlen - chunk->used ) {   /* Truncate */
            /*
            ** Throwing away received data!
            */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_DISCARDED++);
            chunk->size = tcb->rcvlen - chunk->used;

         } /* Endif */

            _mem_copy(tcb->rcvuserbuf + n + chunk->used,
                      tcb->rcvringbuf + chunk->used, chunk->size);

      } else {

         /*
         ** Throwing away received data!
         */
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_DISCARDED++);
         break;   /* remaining chunks are extraneous... */

      } /* Endif */

      chunk = chunk->next;

   } /* Endwhile */

   /*
   ** Delete extraneous chunks
   */
   while ( chunk != NULL ) {

      nextchunk = chunk->next;
      if ( chunk == tcb->rcvchunks ) {
         tcb->rcvchunks = nextchunk;
      } /* Endif */

      _mem_free(chunk);
      chunk = nextchunk;

   } /* Endwhile */

   /*
   ** We now have all of the ring buffer space available (but we
   **    may have some out-of-order data in it).
   */
   TCP_Advertise_receive_window(tcb, tcp_cfg);

   return RTCS_OK;

} /* Endbody */

/* EOF */
