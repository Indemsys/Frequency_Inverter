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
* $FileName: tcp_rcv.c$
* $Version : 3.6.10.0$
* $Date    : Dec-1-2010$
*
* Comments:
*
*   Receive Buffer Space:
*   |----+------------|               (circular)
*   rcvbuf            rcvbuf
*   +rcvpos           +rcvpos
*   :              +rcvlen (w/wrap-around)
*   :                 :
*   :                 :
*   Receive Sequence Space:   :                 :
*   :                 :
*   rcvbufseq      rcvbufseq+rcvlen
*   :                 :
*   : rcvuna          :
*   :  :              :
*   :  :              :
*   |-- - -       - ------+--+-|----------|-+--------
*   rcvirs            1     :     2    :     3
*   rcvnxt   rcvwndedge
*   +----+ 1a       +-+ 3a
*   +-+ 1b
*   1 - old sequence numbers which have been acknowledged
*   1a - seq #s acked but not yet returned to client, because
*   there is no receive client (if there is one, as much data
*   as possible (up to rcvnxt) is put in its buffer);
*   1b - seq #s between rcvuna and rcvnxt have not been ACKed (this is
*   for delayed ACKs), but rcvuna catches up quickly with rcvnxt.
*   2 - sequence numbers allowed for new reception
*   3 - future sequence numbers which are not yet allowed
*   3a - seq #s at the right of the advertised window but within the
*   receive buffer are accepted anyway
*   Note: due to receiver SWS-avoidance algorithm (Silly Window Syndrome),
*   rcvwndedge doesn't strictly follow rcvbufseq+rcvlen but only
*   advances in reasonable increments (a full packet or half the
*   buffer space, whichever is smaller).
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "ticker.h"     /* for timer definitions */
#include "tcpip.h"      /* for error codes */
#include "tcp_prv.h"    /* for TCP internal definitions */
#include "socket.h"     /* for TCP internal definitions */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Must_send_ack
* Returned Values : None.
* Comments        :
*
*  Called when it is determined that an ACK (or anything) must be sent;
*   the ACK isn't necessarily sent immediately...
*
*END*-----------------------------------------------------------------*/

void  TCP_Must_send_ack
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP layer context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN     - TCP layer data */
   )
{ /* Body */
   /*
   ** Ack if two full packets received, or TCP_ACKDELAY ms have elapsed...
   **
   ** Note that we don't transmit immediately since we may be able to
   **    ACK with new data to be sent.
   **
   ** Note that the wrap possibility is taken
   ** care of because we're using uint_32's.
   **
   ** e.g. if tcb->rcvuna   = FFFFFA23
   **     and tcb->rcvnxt   = C00
   **     and tcb->rcv2full = 9FB
   **    then diff          = -11DD
   **    but should be 5DD + C00 = 11DD
   **    but -11DD becomes 11DD as uint_32.
   */
   RTCSLOG_FNE4(RTCSLOG_FN_TCP_Must_send_ack, tcb->rcvuna, tcb->status, tcb->state);
    
   tcb->keepalive = FALSE;

#if 0
   /*DEBUGTCP(*/printf("\nTCP_Must_send_ack: 0x%x ", tcb->status)/*)*/;
   /*DEBUGTCP(*/printf("rcvuna:0x%x acktq:0x%x n:0x%x p:0x%x *curlist:0x%x *donelist:0x%x ", tcb->rcvuna, tcp_cfg->acktq, tcb->sndacktq.t.next, 
                   tcb->sndacktq.t.prev, *tcb->sndacktq.t.curlist, *tcb->sndacktq.t.donelist)/*)*/;
#endif

   if ((uint_32)(tcb->rcvnxt - tcb->rcvuna) >= tcb->rcv2full/* || tcb->status & TCPS_RPUSH*/) {
#if 0
      TCP_Transmit(tcb, tcp_cfg, FALSE, TRUE);
      TCP_Timer_remove(&tcb->sndacktq.t);
#else
      (void)TCP_Timer_oneshot_max(&tcb->sndacktq.t,     /* the timer */
                                  1,                    /* smallest timeout value */
                                  tcp_cfg->qhead );     /* the timer queue head */
#endif
   } else {
        (void)TCP_Timer_oneshot_max(&tcb->sndacktq.t,   /* the timer */
                                    (tcb->state & LAST_ACK) ? 1 : tcb->delayackto,     /* the timeout value */
                                    tcp_cfg->qhead );   /* the timer queue head */
   }

   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Must_send_ack, tcb);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Advance_receive_user
* Returned Values : None.
* Comments        :
*
*  `size' received bytes have been given to the user; advance appropriate
*   receive pointers (rcvbufseq and rcvpos)
*
*END*-----------------------------------------------------------------*/

void TCP_Advance_receive_user
   (
      TCB_STRUCT_PTR tcb,  /* IN/OUT - TCP context */
      int_32         size  /* IN     - no. of bytes given to user */
   )
{ /* Body */

    tcb->rcvpos = (tcb->rcvpos + size) % tcb->rcvlen;
    DEBUGTCP(printf(" Update rcvbufseq by %d from %X to ", size, tcb->rcvbufseq));
    tcb->rcvbufseq += size;
    DEBUGTCP(printf(" %X ", tcb->rcvbufseq));

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Truncate_receive_chunks
* Returned Values : None.
* Comments        :
*
*  Removes `datalen' bytes from the beginning of the receive buffer's chunk
*   list.
*
*  TCP_Advance_receive_user(tcb,datalen) should also be called, unless the connection
*   is being closed and all chunks have been removed.
*
*END*-----------------------------------------------------------------*/

void TCP_Truncate_receive_chunks
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg, /* IN/OUT - TCP layer data */
      int_32               datalen  /* IN     - length of data to remove */
   )
{ /* Body */
   Rchunk _PTR_   chunk;

   /*
   ** Remove 'datalen' bytes from receive buffer (rcvchunks...)
   */
   chunk = tcb->rcvchunks;
   while ( chunk != NULL ) {

      /*
      ** Shift chunk by datalen bytes, towards beginning of rcvbuf
      */
      chunk->used -= datalen;
      if ( chunk->used < 0 ) {

         /*
         ** This chunk started somewhere within the data section removed
         **    from the ring buffer, so remove it if the entire chunk
         **    was within the data section removed, or truncate it if not.
         **
         ** Note the chunk was necessarily at the head of the chunk list.
         */
         chunk->size -= -chunk->used;
         if ( chunk->size <= 0 ) {

            /*  Remove chunk */
            tcb->rcvchunks = chunk->next;
            _mem_free(chunk);
            chunk = tcb->rcvchunks;

         } else {
            chunk->used = 0;
            chunk = chunk->next;

         } /* Endif */

      } else {
         chunk = chunk->next;

      } /* Endif */

   } /* Endwhile */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Advertise_receive_window
* Returned Values : None.
* Comments        :
*
*  This is called when the real receive window (bytes available in receive
*   buffer) may have changed; update the advertised receive window if
*   necessary, taking into account the SWS-avoidance algorithm
*   (RFC1122 4.2.3.3 page 97).
*
*END*-----------------------------------------------------------------*/

void TCP_Advertise_receive_window
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP layer context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   uint_32  newedge;
   uint_32  maxedge;
   uint_32  increment;
   
   RTCSLOG_FNE2(RTCSLOG_FN_TCP_Advertise_receive_window, tcb);

   if ( (tcb->status & TCPS_FINRECV) != 0 ) {  /* window should be zero; do nothing */
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Advertise_receive_window, tcb);
      return;
   } /* Endif */

   increment = (tcb->rcvlen >> 1);
   if ( increment > (uint_32)tcb->sndmax ) {
      increment = (uint_32)tcb->sndmax;
   } /* Endif */

   /*
   ** We are prepared to accept data in the range from
   **   rcvbufseq to rcvbufseq+rcvlen.
   */
   newedge = tcb->rcvbufseq + tcb->rcvlen;

   maxedge = tcb->rcvbufseq + tcb->rcvwndmax;

   if ( GT32(newedge,maxedge) ) {
      newedge = maxedge;
   } /* Endif */

   /*
   ** If two increments or more are possible, go directly to maximum edge,
   **   else advance only by one increment.
   **
   ** From RFC 793 3.7 (page 43):
   **
   **   One suggestion for avoiding small windows is for the receiver to
   **   defer updating a window until the additional allocation is at
   **   least X percent of the maximum allocation possible for the
   **   connection (where X might be 20 to 40).
   **
   ** Note that the wrap possibility is taken
   ** care of because we're using uint_32's.
   **
   ** e.g. if tcb->rcvbufseq           = FFFFFA23
   **     and tcb->rcvwndedge          = FFFFFC00
   **     and tcb->rcvlen              = D800
   **    then newedge                  = D223
   **    and  newedge-tcb->rcvwndedge  = -D623
   **    but should be 400 + D223      = D623
   **    but -D623 becomes D623 as uint_32.
   */
   if ( (uint_32)(newedge - tcb->rcvwndedge) >= (increment << 1) ) {
      tcb->rcvwndedge = newedge;
      TCP_Must_send_ack(tcb, tcp_cfg);

   } else if ( (uint_32)(newedge - tcb->rcvwndedge) >= increment ) {
      tcb->rcvwndedge += increment;
      TCP_Must_send_ack(tcb, tcp_cfg);

   } /* Endif */
   
   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Advertise_receive_window, tcb);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Start_receiving
* Returned Values : None.
* Comments        :
*
*  Called when we start receiving anything on a connection...
*
*  Changes TCB state to SYN_RECEIVED.
*
*END*-----------------------------------------------------------------*/

void TCP_Start_receiving
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP layer context */
      uint_32              irs,     /* IN     - initial receive sequence */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
    RTCSLOG_FNE2(RTCSLOG_FN_TCP_Start_receiving, tcb);

   /*  The IRS (Initial Receive Sequence) is given by the remote side
    *   of the connection.
    */
   tcb->rcvirs = irs;
   tcb->rcvnxt = irs + 1;
   tcb->rcvuna = irs + 1;
   tcb->rcvbufseq  = irs + 1;
   tcb->rcvwndedge = irs + 1;
   DEBUGTCP(printf("\nTCP_StrtRcv: Call to AdRcvwnd: rcvwndedge = %X, rcvnxt = %X ",
                     tcb->rcvwndedge, tcb->rcvnxt));
   TCP_Advertise_receive_window(tcb, tcp_cfg);
   tcb->sndwl1 = irs;
   tcb->state  = SYN_RECEIVED;
   
   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Start_receiving, tcb);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Reply_receive
* Returned Values : None.
* Comments        :
*
*  Remove 'datalen' bytes from the receive buffer, and returns them
*   to the  client, with error condition 'errnum'.
*
*  Assumes there is a Receive() request pending, and that 'datalen' bytes
*   are indeed available at the head of the receive buffer (i.e.,
*   tcb->rcvnxt - tcb->rcvbufseq >= datalen).
*
*END*-----------------------------------------------------------------*/

void TCP_Reply_receive
   (
      TCB_STRUCT_PTR          tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR      tcp_cfg, /* IN/OUT - TCP layer data */
      int_32                  errnum   /* IN     - error code to return */
   )
{ /* Body */
   register TCP_PARM_PTR   req;     /* the Receive request */
            uint_32        status = RTCS_OK;

   /*
   ** If we were using user buffer directly, copy any extraneous receive
   **   data from user buffer to ring buffer, and truncate chunk list if
   **   necessary:
   */
   if ( tcb->rcvbuf == tcb->rcvuserbuf ) {
      status = TCP_Use_ringbuffer(tcb, tcp_cfg);
   } /* Endif */

   req = tcb->rcvHead;

   if ( errnum == RTCS_OK && status != RTCS_OK ) {
      req->ERROR_CODE = status;
   } else {
      req->ERROR_CODE = errnum;
   } /* Endif */
   req->BUFF_LENG  = tcb->rcvusernxt;
   req->STATUS     = tcb->status;
   req->URGENT     = 0;

   tcb->rcvuserbuf = NULL; /* buffer is returned to user */
/* tcb->rcvuserlen = 0; */

   /*
   ** Is there urgent data?
   */
   if ( (tcb->status & TCPS_RURGENT) != 0 ) {

      /*
      ** (rcvbufseq - rcvusernxt) is seq # of 1st data byte returned to user
      */
      req->URGENT = tcb->rcvup - (tcb->rcvbufseq - tcb->rcvusernxt);

      if ( GT32(req->URGENT, tcb->rcvup )) {  /* shouldn't happen, but check anyway */
         RTCS_log_error( ERROR_TCP,
                              RTCSERR_TCP_URGENT_DATA_PROB,
                              (uint_32)tcb,
                              (uint_32)req,
                              req->URGENT );
         req->URGENT = 0;
      } /* Endif */

      if ( GE32(tcb->rcvbufseq, tcb->rcvup) ) {
         tcb->status &= ~TCPS_RURGENT;
      } /* Endif */

   } /* Endif */

   if ( (tcb->status & TCPS_RPUSH) != 0 &&
      GE32(tcb->rcvbufseq,tcb->rcvpush))
   {
      tcb->status &= ~TCPS_RPUSH;
   } /* Endif */

   /*
   ** Cancel receive timeout if one is active
   */
   (void)TCP_Timer_stop(&tcb->rcvtq.t, tcp_cfg->qhead);

   /* Unlink the current request */
   tcb->rcvHead = req->NEXT;
   if ( tcb->rcvHead == NULL ) {
      tcb->rcvTail = NULL;
   } /* Endif */
   req->NEXT = NULL;

   /*
   ** Return the request
   */
   RTCSCMD_complete(req, req->ERROR_CODE);

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Test_done_receive
* Returned Values : None.
* Comments        :
*
*  Test to see if a receive request has been completed, and if so,
*   complete it.
*
*  Returns TRUE if request was left on rcvHead queue.
*
*END*-----------------------------------------------------------------*/

boolean TCP_Test_done_receive
   (
      TCB_STRUCT_PTR          tcb,     /* IN/OUT - TCP context */
      uint_32                 status,  /* IN     - error code  */
      TCP_CFG_STRUCT_PTR      tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   boolean  ret_state = TRUE; /* Default return status to TRUE */

   if ( tcb->rcvHead != NULL ) {

      /*
      ** Reply if buffer full, or (if return on PSH option specified)
      **   a PSH was received and data is ready to return.
      */
      if ( tcb->rcvusernxt == tcb->rcvuserlen  ||    /* user buffer full */
         ((tcb->rcvevents & TCPO_PUSH) != 0  &&   /* must push data */
          tcb->rcvusernxt > 0                &&   /* at least one byte */
           GE32(tcb->rcvnxt, tcb->rcvpush )   &&   /* rcvd all pushed data */
          (tcb->status & TCPS_RPUSH) != 0))       /* received a push */
      {

         TCP_Reply_receive( tcb, tcp_cfg, status );
         ret_state = FALSE;

      } /* Endif */

   } else {
      ret_state = FALSE;

   } /* Endif */

   return ret_state;

} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Setup_receive
* Returned Values : boolean
* Comments        :
*
*  Process next receive request.  Sets up the TCB receive section
*  for the next request on the rcvHead queue.
*
*  Assumes that the receive request has been linked into rcvHead
*  at the start (i.e. rcvHead EQ this request).  The request may
*  be immediately returned.  If so, it is unlinked from the rcvHead
*  queue.
*
*  Returns FALSE if receive was unlinked (i.e. the request was returned).
*
*END*-----------------------------------------------------------------*/

boolean  TCP_Setup_receive
   (
      TCP_PARM_PTR         req,     /* IN/OUT - the Receive request */
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   int_32   n;                /* no. of received bytes available */
   boolean  ret_state = TRUE; /* Default return status to TRUE */
   int_32   result = RTCS_OK;

   n = tcb->rcvnxt - tcb->rcvbufseq;   /* Seq #'s acked but not yet
                                          given to upper layer */
   switch ( tcb->state ) {

      default:

         n--;     /* Don't count the FIN */

         if ( n == 0 ) {

            /*
            ** All data given to upper layer
            */

            req->ERROR_CODE = (tcb->state == CLOSED) ?
                                 RTCSERR_TCP_NOT_CONN : RTCSERR_TCP_CONN_CLOSING;
            req->BUFF_LENG = 0;

            /* Unlink the request */
            tcb->rcvHead = req->NEXT;
            if ( tcb->rcvHead == NULL ) {
               tcb->rcvTail = NULL;
            } /* Endif */

            RTCSCMD_complete(req, req->ERROR_CODE);

            ret_state = FALSE;
            break;

         } /* Endif */

         /*
         ** Can't wait, because we received a FIN:
         */
         req->OPTIONS |= TCPO_NOWAIT;

         /*FALLTHROUGH*/

      case LISTEN:
      case SYN_SENT:
      case SYN_RECEIVED:

         /*FALLTHROUGH*/

      case ESTABLISHED:
      case FINWAIT_1:
      case FINWAIT_2:

         tcb->rcvuserbuf = req->BUFFER_PTR;
         tcb->rcvuserlen = req->BUFF_LENG;
         tcb->rcvevents  = req->OPTIONS;
         DEBUGTCP(printf("\nTCP_SetpRcv: Got RECEIVE of size %d ", req->BUFF_LENG));

         /*
         ** Copy any available (contiguous) data to the client's buffer
         */
         if ( n != 0 ) {

            if ( n > tcb->rcvuserlen ) {
               n = tcb->rcvuserlen;
            } /* Endif */
            result = TCP_Copy_from_ring(  tcb->rcvuserbuf,   /* destination */
                                    n,                 /* size */
                                    tcb->rcvpos,       /* offset in ringbuf */
                                    tcb->rcvringbuf,
                                    tcb->rcvringlen);

            TCP_Truncate_receive_chunks(tcb, tcp_cfg, n);

            /*
            ** Move both rcvpos and rcvbufseq forward by the amount of data
            **    copied from the ring buffer to the user buffer.  Note
            **    we should also call AdvertiseRcvWnd...
            */
            DEBUGTCP(printf("\nTCP_SetpRcv: Calls AdvRcvU: "));
            TCP_Advance_receive_user(tcb, n);

         } /* Endif */
         tcb->rcvusernxt = n;

         /*
         ** If client is polling, return after updating receive window.
         **  also return if an error occurred.
         */
         if ( (result != RTCS_OK) ||
            (req->OPTIONS & TCPO_NOWAIT) != 0)
         {
            TCP_Reply_receive(tcb, tcp_cfg, result);
            ret_state = FALSE;
            if (result != RTCS_OK) {
               break;
            } /* Endif */
         } else {

            /*
            ** Otherwise we might return anyway
            **   (will go to next Receive if request completed)
            */
            ret_state = TCP_Test_done_receive(tcb, result, tcp_cfg);
         } /* Endif */

         if ( ret_state ) { /* we still have the receive request */

            /*
            ** If user buffer is bigger than the ring buffer,
            **  use the user buffer instead, so we can give out a bigger
            **  receive window to the peer TCP;
            **
            ** Note that we can't use the user's buffer if we are to
            **  return upon PSH or timeout, because on receipt of either
            **  we'll have to fall back on the ring buffer and possibly
            **  *reduce* our receive window, which is very
            **  improper (translate: unacceptable) TCP behaviour.
            **
            ** We should only switch if the user buffer is significantly
            **  bigger than the ring (by TCP_SWITCH_THRESHOLD), to avoid
            **  needless switching;
            **  the 'switch' feature is really to support things like
            **  downloading 1Meg images in one block, so that we can offer
            **  a full 64k window without having to allocate such buffers...
            */

            DEBUGTCP(printf("\nTCP_SetpRcv: tcb->options = %X ", tcb->options));
            DEBUGTCP(printf("\n           : req->TIMEOUT = %d ", req->TIMEOUT));
            DEBUGTCP(printf("\n           : tcb->rcvuserlen (%d) - n (%d) = %d, rcvringlen + THRESH = %d ",
                     tcb->rcvuserlen, n, tcb->rcvuserlen-n, tcb->rcvringlen+TCP_SWITCH_THRESHOLD));
            DEBUGTCP(printf("\n           : tcb->rcvevents = %X ", tcb->rcvevents));

            if ( (tcb->options & TCPO_NOSWITCH) == 0       &&
               req->TIMEOUT == 0                         &&
               (tcb->rcvuserlen - n) >
                  (tcb->rcvringlen + TCP_SWITCH_THRESHOLD) &&
               (tcb->rcvevents & TCPO_PUSH) == 0)
            {

               Rchunk _PTR_   chunk;

               /*
               ** Copy from ring to user buffer (out-of-order chunks).
               */
               if ( result == RTCS_OK ) {

                  for ( chunk = tcb->rcvchunks;
                      chunk != NULL;
                      chunk = chunk->next)
                  {
                     result =
                        TCP_Copy_from_ring(  tcb->rcvuserbuf + n + chunk->used, /* dest */
                                       chunk->size,                       /* size */
                                       tcb->rcvpos + chunk->used,         /* offset */
                                       tcb->rcvringbuf,
                                       tcb->rcvringlen);

                     if ( result != RTCS_OK ) {
                        break;
                     } /* Endif */
                  } /* Endfor */

               } /* Endif */

               /*
               ** Add leading chunk, which we know is not contiguous with
               **    any other chunks
               */

               if ( tcp_cfg->Rchunkfree != NULL ) {
                  chunk = tcp_cfg->Rchunkfree;
                  tcp_cfg->Rchunkfree = chunk->next;
               } else {
                  chunk = (Rchunk _PTR_)RTCS_mem_alloc_zero(sizeof(Rchunk));
               } /* Endif */

               if ( chunk == NULL ) {
                  IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_MISSED++);
                  RTCS_log_error( ERROR_TCP, RTCSERR_TCP_PR_OUT_OF_MEM, (uint_32)tcb, 0, 0 );
                  break;

               } else {
                  _mem_set_type(chunk, MEM_TYPE_Rchunk);

                  chunk->size = n;
                  chunk->used = 0;
                  chunk->next = tcb->rcvchunks;
                  tcb->rcvchunks = chunk;

                  /*
                  ** Now use user buffer instead of ring
                  */
                  DEBUGTCP(printf("\nTCP_SetpRcv: Switching to User Buffer "));
                  tcb->rcvpos = 0;
                  tcb->rcvbuf = tcb->rcvuserbuf;
                  tcb->rcvlen = tcb->rcvuserlen;

                  /*
                  ** Note that the data that was sitting at the start
                  ** of the ring-buffer (n bytes, to be precise) got
                  ** copied to the user-buffer; in doing so, the start
                  ** of the ring-buffer was incremented by n, since these
                  ** n bytes are now in the user-buffer (hence,
                  ** tcb->rcvbufseq was incremented by n in the call to
                  ** TCP_Advance_receive_user(tcb,n)).  Since the main receive
                  ** buffer is now being set to the user-buffer instead of
                  ** the ring-buffer, these n bytes return to the main
                  ** buffer, and so rcvbufseq must be set back by n.
                  ** The net effect in this particular call to
                  ** TCP_Setup_receive() is that tcb->rcvbufseq hasn't changed.
                  */
                  tcb->rcvbufseq -= n;


               } /* Endif */   /* chunk EQ NULL */

            } else {
               DEBUGTCP(printf("\nTCP_SetpRcv: Couldn't switch "));
            } /* Endif */

            /*
            ** Return the request if we had an error.
            */
            if ( result != RTCS_OK ) {

               TCP_Reply_receive(tcb, tcp_cfg, result);
               ret_state = FALSE;
               break;

            } else {

               /*
               ** Initiate a receive timeout if requested
               */
               if ( req->TIMEOUT != 0 ) {
                  result =
                     TCP_Timer_start( &tcb->rcvtq.t,    /* the timer */
                                 req->TIMEOUT,     /* the timeout value */
                                 0,                /* no reload */
                                 tcp_cfg->qhead ); /* the timer queue head */
                  if ( result != RTCS_OK ) {
                     RTCS_log_error( ERROR_TCP,
                                          RTCSERR_TCP_TIMER_FAILURE,
                                          (uint_32)result,
                                          (uint_32)tcb,
                                          12 );
                  } /* Endif */
               } /* Endif */

            } /* Endif */

         } /* Endif */   /* ret_state */

         /*
         ** Update receive window if rcvbufseq and/or rcvlen has changed
         */
         DEBUGTCP(printf("\nTCP_SetpRcv: Call to AdRcvwnd: rcvwndedge = %X, rcvnxt = %X, wind = %d, rbs = %X ",
            tcb->rcvwndedge, tcb->rcvnxt,
            tcb->rcvwndedge-tcb->rcvnxt, tcb->rcvbufseq));
         TCP_Advertise_receive_window(tcb, tcp_cfg);

         break;

   } /* End Switch */

   return ret_state;

} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_receive
* Returned Values : None.
* Comments        :
*
*  Process TcpReceive() requests, within TCP server.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_receive
   (
      TCP_PARM_PTR      tcp_req      /* IN/OUT - upper layer Send request */
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;    /* TCP layer data */
   TCB_STRUCT_PTR       tcb;

   tcp_cfg = RTCS_getcfg(TCP);
   tcb = tcp_req->TCB_PTR;

   if ( tcb == NULL ) { 
      tcp_req->BUFF_LENG = 0; 
      RTCSCMD_complete(tcp_req, RTCSERR_TCP_CONN_RLSD);
      return; 
   } 
   
   if ( tcb->VALID != TCB_VALID_ID ) {
      tcp_req->BUFF_LENG  = 0;
      RTCSCMD_complete(tcp_req, RTCSERR_TCP_NOT_CONN);

   } else if ( tcb->state == CLOSED ) {
      tcp_req->BUFF_LENG  = 0;
      RTCSCMD_complete(tcp_req, RTCSERR_TCP_CONN_ABORTED);

   } else {

      /*
      ** Ensure the upper layer did not request both wait and
      **    no-wait.
      */
      if ( (tcp_req->OPTIONS & (TCPO_WAIT | TCPO_NOWAIT)) ==
         (TCPO_WAIT | TCPO_NOWAIT))
      {
         tcp_req->BUFF_LENG  = 0;
         RTCSCMD_complete(tcp_req, RTCSERR_TCPIP_INVALID_ARGUMENT);

      /*
      ** If there is already a receive active, just
      ** add this one to the end of the list.
      */
      } else if ( tcb->rcvHead != NULL ) {

         /*
         ** If no wait was specified, then this is a weird
         ** request, so just return the request.
         */
         if ( (tcp_req->OPTIONS & TCPO_NOWAIT) != 0 ) {
            tcp_req->BUFF_LENG  = 0;
            RTCSCMD_complete(tcp_req, RTCSERR_TCP_IN_PROGRESS);
         } else {
            tcp_req->NEXT      = NULL;
            tcb->rcvTail->NEXT = tcp_req;
            tcb->rcvTail       = tcp_req;
         } /* Endif */

      } else {
         /*
         ** Set up this receive
         */
         tcb->rcvHead = tcb->rcvTail = tcp_req;
         tcp_req->NEXT    = NULL;
         TCP_Setup_receive(tcp_req, tcb, tcp_cfg);

      } /* Endif */ /* request ok */

   } /* Endif */ /* tcb valid */

   return;

} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_data
* Returned Values : None.
* Comments        :
*
*  Process incoming text (data) segment.  At this point the current
*     buffer may be a user (Receive) buffer or the ring buffer.  In
*     any case it should be able to handle any data we receive because
*     we've advertised our receive window based on it.
*
*  Assumes that PcbData(pcb) and pcb->size are start and size of data,
*   respectively.
*
*  Returns after all data that could be received was buffered.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_data
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg, /* IN/OUT - TCP layer data */
      TCP_HEADER _PTR_     seg,     /* IN     - TCP packet */
      RTCSPCB_PTR          pcb      /* IN     - the packet */
   )
{ /* Body */
   uint_32     dataseq,       /* seq # of first byte of good data */
               bufoffset;     /* offset in buffer to put good data */
   int_32      sizeallowed,   /* maximum size available in current buffer */
               size,          /* size left to copy */
               truncate,      /* no. of rexmtted bytes in data */
               n;             /* no. of bytes avail at head of buffer */
   uchar_ptr   data;          /* pointer to good data */
   uint_32     status     = RTCS_OK;
   uint_32     mid_status = RTCS_OK;
   uint_32     use_status = RTCS_OK;
   PCB_FRAGMENT _PTR_ pcb_frag_ptr;
   
   RTCSLOG_FNE2(RTCSLOG_FN_TCP_Process_data, tcb);
       
   /*
   ** Get seq # of 1st byte of data
   */
   dataseq = ntohl(seg->seq);
   if ( ntohs(seg->flags) & SYN ) {       /* first byte is the SYN */
      dataseq++;
   } /* Endif */

   /*
   ** Discard unused beginning of data (i.e. retransmitted data)
   */
   size = RTCSPCB_SIZE(pcb);
   truncate = tcb->rcvbufseq - dataseq;   /* bytes to remove at begin. */

   if ( truncate < 0 ) {                  /* dataseq GT rcvbufseq so all */
      truncate = 0;                       /*   data is new */
   } /* Endif */
   if ( truncate >= size ) {              /* no data to process, quit */
      if (size > 0) {
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_DATA_DUP++);
      } /* Endif */
      
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Process_data, tcb);

      return;
   } /* Endif */

   /*
   ** Keep track of maximum sized packets received, so we know when we get
   **   full-sized packets (because we send an ACK upon two full packets,
   **   or rather, after 1.75 packets to allow for unexpected variations...)
   */
   if ( size > tcb->rcvmax ) {
      tcb->rcvmax = size;
      tcb->rcv2full = size + (size >> 1) + (size >> 2);  /* 1.75 * size */
   } /* Endif */

   /*
   ** Step over retransmitted data
   */
   size     -= truncate;
   dataseq  += truncate;
   if (pcb->HEADER_FRAG_USED) {
      data   = RTCSPCB_DATA(pcb) + truncate;   /* pointer to new data */
   } else {
      pcb_frag_ptr = pcb->PCBPTR->FRAG;
      pcb_frag_ptr++;
      /* pointer to new data */
      data  = pcb_frag_ptr->FRAGMENT + truncate;
   } /* Endif */
   DEBUGTCP(printf("\nTCP_ProcDta: Got %d bytes of new data ", size));

   /*
   ** Discard unused end of data (i.e. stuff we can't buffer).
   **    Any discarded here means that the remote host has sent
   **    beyond the window that we advertised.
   */
   bufoffset = dataseq - tcb->rcvbufseq;     /* position within receive buf
                                                of 1st byte of new data */
   sizeallowed = tcb->rcvlen - bufoffset;

   if ( sizeallowed <= 0 ) {                  /* data too far ahead, quit */
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_DATA++);
      TCP_Must_send_ack(tcb, tcp_cfg);
      
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Process_data, tcb);

      return;
   } /* Endif */

   if ( size > sizeallowed ) {               /* we have to truncate some */
      size = sizeallowed;                    /*   of the data */
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_DISCARDED++);
      DEBUGTCP(printf("\nTCP_ProcDta: Truncated new data to %d bytes (sizeallowed) ", size));
   } /* Endif */

   if ( ntohs(seg->flags) & (PSH | FIN) ) {
      if ( (tcb->status & TCPS_RPUSH) == 0 ||
         GT32(dataseq + size, tcb->rcvpush))
      {
         tcb->rcvpush = dataseq + size;
         TCP_Event(tcb, TCPS_RPUSH);
      } /* Endif */
   } /* Endif */

   /*
   ** If we weren't already using the user buffer, can we put the data
   **    (or some of it) directly into the user buffer?  Note we
   **    enforce that user buffer only contains contiguous data
   **    in this case where we are normally using the ring buffer.
   */
   if ( bufoffset == 0          &&     /* data goes at start of current buffer */
      tcb->rcvuserbuf != NULL &&       /* we have a user buffer */
      tcb->rcvuserbuf != tcb->rcvbuf)  /* not already using user buffer */
   {

      /*
      ** Yes; how much can we put?
      */
      sizeallowed = tcb->rcvuserlen - tcb->rcvusernxt;
      if ( sizeallowed > size ) {      /* can move all data from packet */
         sizeallowed = size;
      } /* Endif */

      _mem_copy(data, tcb->rcvuserbuf+tcb->rcvusernxt, sizeallowed);

      /*
      ** Update the number of bytes in the user buffer.
      */
      tcb->rcvusernxt += sizeallowed;
      size            -= sizeallowed;
      data            += sizeallowed;

      /*
      ** Have given 'sizeallowed' bytes to the user, so the ring
      **    buffer pointers should be moved forward the equivalent
      **    amount (i.e. ring buffer bufseq should always be
      **    beyond the last byte given to the user).
      **
      ** Move both rcvpos and rcvbufseq forward by the amount of data
      **    copied from the ring buffer to the user buffer.
      */
      DEBUGTCP(printf("\nTCP_ProcDta: Calls AdvRcvU: "));
      TCP_Advance_receive_user(tcb, sizeallowed);

      /*
      ** Now fix the RcvChunk list to match the data 'removed'
      **    from the ring buffer.
      */
      TCP_Truncate_receive_chunks(tcb, tcp_cfg, sizeallowed);

      /*
      ** See if we can move some data from later in the stream
      **    (that we received too early) from the ring buffer
      **    to the user buffer.  We can move only enough to
      **    fill up the user buffer (i.e. rcvuserlen - rcvusernxt).
      **
      ** Note that we only have to check the first RcvChunk
      **    because if there are more than one they cannot be
      **    contiguous.
      */

      if ( size <= 0 ) {

         { /* Scope */

            Rchunk _PTR_   chunk;

            /*
            ** We were able to copy the entire packet to user buffer.
            **    Note that size should never be 'less than' here, it
            **    should always equal 0.
            */
            chunk = tcb->rcvchunks;
            if ( chunk != NULL ) {

               if ( chunk->used == 0 ) {   /* Can only move if contiguous */
                  n = chunk->size;

                  /*
                  ** Copy as much of the (contiguous) data to the user buffer
                  **    as possible.
                  */
                  if ( n > (tcb->rcvuserlen - tcb->rcvusernxt) ) {
                     n = tcb->rcvuserlen - tcb->rcvusernxt;
                  } /* Endif */

                  mid_status =
                     TCP_Copy_from_ring(  tcb->rcvuserbuf + tcb->rcvusernxt,
                                                      /* destination */
                                    n,                /* size */
                                    tcb->rcvpos,      /* offset in ringbuf */
                                    tcb->rcvringbuf,
                                    tcb->rcvringlen);

                  /*
                  ** Update the number of bytes in the user buffer.
                  */
                  tcb->rcvusernxt += n;

                  /*
                  ** Now fix the RcvChunk list to match the data removed.
                  */
                  TCP_Truncate_receive_chunks(tcb, tcp_cfg, n);

                  /*
                  ** Move both rcvpos and rcvbufseq (ring ptrs) forward by the
                  **    amount of data copied from the ring buffer to the user
                  **    buffer.
                  */
                  DEBUGTCP(printf("\nTCP_ProcDta: Calls AdvRcvU: "));
                  TCP_Advance_receive_user(tcb, n);

               } /* Endif */

            } /* Endif */

         } /* Endscope */

      } /* Endif */

   } /* Endif */

   /*
   ** If there any valid data left, we must copy it to the current
   **    buffer, which may be either the ring buffer or the user
   **    buffer.
   **
   ** Note that if the user buffer was used above, then the current
   **    buffer must be the ring buffer.  If the current buffer is
   **    the user buffer, then we're just using it as if it was
   **    the ring buffer.  The eventual call to TCP_Use_ringbuffer will
   **    reset everything properly (e.g. rcvbufseq).
   */
   if ( size != 0 ) { /* data still available */

      /*
      ** Note that we know that 'size' bytes are available in the
      ** ring (or user) buffer at this point because we truncated
      ** un-bufferable data at the start of the procedure.
      */
      status = TCP_Copy_to_Rbuffer( tcb,  /* TCP context */
                           bufoffset,     /* dest offset from rcvpos in rcv buf */
                           data,          /* source buffer */
                           size,          /* size to copy */
                           tcb->rcvbuf,   /* start of rcv buf */
                           tcb->rcvlen,   /* total len of rcv buf */
                           tcb->rcvpos,   /* current offset in rcv buf */
                           tcp_cfg );     /* TCP layer data */

   } /* Endif */

   /*
   ** Compute nb of bytes of good data available at head of receive buffer.
   **    i.e. 'n' will be the number of contiguous data bytes from the
   **    start of the current buffer.
   */
   n = 0;
   if ( tcb->rcvchunks != NULL && tcb->rcvchunks->used == 0 ) {
      n = tcb->rcvchunks->size;
   } /* Endif */

   /*
   ** Switch from user to ring buffer if there is now more space available
   **    in the ring buffer.
   */
   if ( tcb->rcvuserbuf == tcb->rcvbuf &&     /* using user buffer  */
      tcb->rcvuserlen - n < tcb->rcvringlen) /* ring has more room */
   {

      DEBUGTCP(printf("\nTCP_ProcDta: Switching to Ring Buffer "));
      use_status = TCP_Use_ringbuffer(tcb, tcp_cfg);

      /* Now either ring is empty or it has only non-contiguous data. */
      n = 0;

   } /* Endif */

   /*
   ** Advance rcvnxt and update receive window if necessary.
   */
   if ( GT32(tcb->rcvbufseq + n, tcb->rcvnxt) ) { /* rcvd new contiguous data */

      DEBUGTCP(printf("\nTCP_ProcDta: Update rcvnxt by n=%d from %X ",n,tcb->rcvnxt));
      tcb->rcvnxt = tcb->rcvbufseq + n;
      DEBUGTCP(printf("to %X ", tcb->rcvnxt));

      DEBUGTCP(printf("\nTCP_ProcDta: Call to AdRcvwnd: rcvwndedge = %X, rcvnxt = %X, wind = %d, rbs = %X ",
                     tcb->rcvwndedge, tcb->rcvnxt,
                     tcb->rcvwndedge - tcb->rcvnxt, tcb->rcvbufseq));
      TCP_Advertise_receive_window(tcb, tcp_cfg);
      TCP_Must_send_ack(tcb, tcp_cfg);

   }
   else if ( LT32(dataseq, tcb->rcvnxt) ) { /* first byte past rcvnxt */

      /*
      ** Just acknowledge data received.  If first byte was past rcvnxt,
      **    (i.e. received out of order) then the data is
      **    unacknowledgeable.
      */
      TCP_Must_send_ack(tcb, tcp_cfg);

   } /* Endif */

   /*
   ** Check if we can return a Receive request.
   */
   if ( tcb->rcvHead != NULL ) {

      boolean need_rcv = FALSE;

      if ( status != RTCS_OK ) {
         TCP_Reply_receive( tcb, tcp_cfg, status );
         need_rcv = TRUE;

      } else if ( mid_status != RTCS_OK ) {
         TCP_Reply_receive( tcb, tcp_cfg, mid_status );
         need_rcv = TRUE;

      } else if ( use_status != RTCS_OK ) {
         TCP_Reply_receive( tcb, tcp_cfg, use_status );
         need_rcv = TRUE;

      } else {

         /*
         ** Check to see if the user buffer is full.  Note that TCP_Test_done_receive
         **    will handle switching back to the ring buffer.
         */
         if ( ! TCP_Test_done_receive(tcb, status, tcp_cfg) ) { /* receive was returned */
            need_rcv = TRUE;
         } /* Endif */

      } /* Endif */

      if ( need_rcv ) {

         /*
         ** Setup the next Receive.  Note that TCP_Setup_receive will switch back
         **   to the user buffer if possible.
         */
         while ( tcb->rcvHead != NULL &&
               ! TCP_Setup_receive(tcb->rcvHead, tcb, tcp_cfg)
         ) {
         } /* Endwhile */

      } /* Endif */

   } /* Endif */

   /*
   ** Notify socket layer in case some task is blocked on select()
   ** waiting for this data.
   */
   
   if (GT32(tcb->rcvnxt, tcb->rcvbufseq)) {
      SOCK_select_signal(tcb->SOCKET, tcb->state);
   } /* Endif */
   
    RTCSLOG_FNX2(RTCSLOG_FN_TCP_Process_data, tcb);
} /* Endbody */

/* EOF */
