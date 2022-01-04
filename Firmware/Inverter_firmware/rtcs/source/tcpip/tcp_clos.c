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
* $FileName: tcp_clos.c$
* $Version : 3.6.8.0$
* $Date    : Dec-1-2010$
*
* Comments:
*
*   A TCB can be released (via TCP_Process_release) only after the upper layer
*   has provided a tcpCLOSE (or tcpABORT) command or if a tcpOPEN
*   command has failed.  In all other cases the TCB may be moved to
*   CLOSED state (via TCP_Close_TCB), but the TCB cannot be released.
*   The cases where the upper layer has closed are not easy to see (because
*   in the normal case the connection stays up until the other side is
*   also ready to close), but this occurs while in states:
*   FINWAIT_1, FINWAIT_2, CLOSING, TIME_WAIT, and LAST_ACK.
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
* Function Name   : TCP_Free_TCB
* Returned Values : None.
* Comments        :
*
*  Deallocate TCB; must only be done when TCB has been closed and released.
*
*END*-----------------------------------------------------------------*/

void TCP_Free_TCB
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   TCB_STRUCT_PTR    tmp;

   /*
   ** Free everything which wasn't freed in TCP_Close_TCB() ...
   */
   TCP_Truncate_receive_chunks(tcb, tcp_cfg, tcb->rcvlen); /* delete all chunk nodes */
   if ( tcb->rcvringbuf != NULL ) {

      _mem_free(tcb->rcvringbuf);
      tcb->rcvringbuf = NULL;

   } /* Endif */

   /*
   ** Deallocate TCB itself
   */
   if ( tcb != NULL ) {

      if ( tcb == tcp_cfg->TCBhead ) {
         tcp_cfg->TCBhead = tcb->next;
      } else {
         tmp = tcp_cfg->TCBhead;
         while ( tmp != NULL && tmp->next != tcb ) {
            tmp = tmp->next;
         } /* Endwhile */
         if ( tmp != NULL ) {
            tmp->next = tcb->next;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   if (tcb->SOCKET) {
      ((SOCKET_STRUCT_PTR)tcb->SOCKET)->TCB_PTR = NULL;
   } /* Endif */
   tcb->VALID = TCB_INVALID_ID;

   tcp_cfg->CONN_COUNT--;
#if RTCSCFG_TCP_MAX_HALF_OPEN
    TCB_HALF_OPEN_DROP
#endif

   _mem_free(tcb);

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_release
* Returned Values : None.
* Comments        :
*
*  Release the TCB so that it can no longer be used.  Called only
*     when in CLOSED state with no more data to receive.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_release
   (
      TCB_STRUCT_PTR          tcb,     /* IN     - TCP context */
      TCP_CFG_STRUCT_PTR      tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   TCB_STRUCT_PTR _PTR_ search_ptr = &tcp_cfg->NOSOCK_TCB;
   TCB_STRUCT_PTR       temp_tcb;

   /*
   ** 1. Check if the TCB was spawned by a listening TCB, and if so remove it from
   **    the list of spawned listening TCBs.
   ** 2. If the TCB that is begin released is a listening TCB and it spawned
   **    other TCBs, they must also be freed.
   */
   while (*search_ptr) {
      if (*search_ptr == tcb) {
         /* The TCB we are releasing is a child of a listening TCB */
         *search_ptr = (*search_ptr)->NOSOCK_NEXT;
         break;
      } /* Endif */

      if (tcb && (*search_ptr)->LISTEN_TCB == tcb) {
         /*
         ** The TCB is a listening TCB and we have found one of its children.
         ** Delete the child. Recursion is only one level deep, because the
         ** child is not a listening TCB and has not spawned any TCBs of its own
         */
         temp_tcb = *search_ptr;
         *search_ptr = temp_tcb->NOSOCK_NEXT;
         TCP_Close_TCB(temp_tcb, RTCS_OK, tcp_cfg);
         TCP_Process_release(temp_tcb, tcp_cfg);
         continue;
      } /* Endif */

      search_ptr = &(*search_ptr)->NOSOCK_NEXT;
   } /* Endwhile */

   TCP_Free_TCB(tcb, tcp_cfg);

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Close_TCB
* Returned Values : None.
* Comments        :
*
*  Enter CLOSED state and terminate all requests with error code errnum.
*
*  Will be followed by a TCP_Process_release at some time, when there
*     is no more data to receive.  Note that this call will prevent
*     and further data from being queued (and so should immediately
*     be followed by a check for ability to release).
*
*  This is the only routine which puts a TCB in the CLOSED state.
*
*END*-----------------------------------------------------------------*/

void TCP_Close_TCB
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      int_32               errnum,  /* IN     - error code to return */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   TCP_PARM_PTR      req;
   SbufNode _PTR_    buf;
   boolean           first = TRUE;  /* for returning Sends */
   boolean           not_syn_recv = (tcb->state != SYN_RECEIVED);

   /*
   ** This prevents new packets from being queued on this TCB;
   **   however there might still be some queued (but only if
   **   there are no outstanding Receive requests).
   */
   tcb->state = CLOSED;

   /*
   ** Cancel timers...
   */
   (void)TCP_Timer_stop(&tcb->sndxmittq.t, tcp_cfg->qhead);
   (void)TCP_Timer_stop(&tcb->sndacktq.t, tcp_cfg->qhead);
   (void)TCP_Timer_stop(&tcb->sndtq.t, tcp_cfg->qhead);
   (void)TCP_Timer_stop(&tcb->rcvtq.t, tcp_cfg->qhead);

   /*
   ** Return open request if there is one.  This should
   ** only happen if we are closing on error.
   */
   if ( errnum != RTCS_OK && not_syn_recv) {
      TCP_Return_open(tcb, errnum, tcp_cfg);
   } /* Endif */

   /*
   ** Terminate any Receive requests.  We can't receive any more
   **    packets in the CLOSED state, and presence of any Receives
   **    means that all data that could be (contiguously) in the
   **    first Receive is already there (see TCP_Process_data).
   **
   ** In this case the TCB could actually be released, but wait for
   **    user to close it.  Don't have to delete all of the RcvChunks
   **    or indicate no data left in ring since a TCP_Process_close or
   **    TCP_Process_receive will do this.
   */
   while ( tcb->rcvHead != NULL ) {

      TCP_Reply_receive(tcb, tcp_cfg, errnum);
      if ( tcb->rcvHead != NULL ) {
         TCP_Setup_receive(tcb->rcvHead, tcb, tcp_cfg);
      } /* Endif */

   } /* Endwhile */

   /*
   ** Terminate any Send requests
   */
   buf = tcb->sndbuf;
   while ( buf != NULL ) {

      tcb->sndbuf = buf->next;
      if ( tcb->sndbuf == NULL ) {
         tcb->sndbuftail = NULL;
      } /* Endif */

      req = buf->req;
      if ( req != NULL ) {

         /* report actual amount sent */
         if ( first ) {
            req->BUFF_LENG  = tcb->snduna - tcb->sndbufseq;
         } else {
            req->BUFF_LENG = 0;
         } /* Endif */
         RTCSCMD_complete(req, errnum);

      } /* Endif */
      first = FALSE;

      _mem_free(buf);
      buf = tcb->sndbuf;

   } /* Endwhile */


   /*  Terminate any wait request
    */
#if 0
   req = tcb->oclient;
   if ( req != NULL ) {

      req->STATUS     = tcb->status;
      req->SOFTERROR  = tcb->lasterror;
      RTCSCMD_complete(req, errnum);
      tcb->oclient    = NULL;

   } /* Endif */
#endif

   /*
   ** Notify socket layer in case some task is blocked on select()
   ** waiting for data on this socket.
   */
   SOCK_select_signal(tcb->SOCKET, tcb->state);   

   /*  Delete send buffers (keep receive buffer in case there is
    *  pending data)
    */
   if ( tcb->sndringbuf != NULL ) {
      _mem_free(tcb->sndringbuf);
   } /* Endif */
   tcb->sndringbuf = NULL;

   if ( tcb->sndclk != NULL ) {
      _mem_free(tcb->sndclk);     /* xmit timestamps buffer */
   } /* Endif */
   tcb->sndclk = NULL;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_effective_close
* Returned Values : None.
* Comments        :
*
*  Called when the connection is actually closed, i.e., when all data
*   to send has been sent and FIN can at last be sent.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_effective_close
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN     - TCP layer data */
   )
{ /* Body */
   RTCSLOG_FNE2(RTCSLOG_FN_TCP_Process_effective_close, tcb->state);
    
   switch ( tcb->state ) {

      case SYN_RECEIVED:
         /* no data has been sent  */

      case ESTABLISHED:
         tcb->state = FINWAIT_1;
         TCP_Must_send_ack(tcb, tcp_cfg);
         break;

      case CLOSE_WAIT:
         tcb->state = LAST_ACK;
         TCP_Must_send_ack(tcb, tcp_cfg);
         break;

      default:
         RTCS_log_error(ERROR_TCP,
                             RTCSERR_TCP_BAD_STATE_FOR_CLOSE,
                             (uint_32)tcb->state,
                             (uint_32)tcb, 0);

         break;

   } /* End Switch */

    SOCK_select_signal(tcb->SOCKET, tcb->state);
   
    RTCSLOG_FNX1(RTCSLOG_FN_TCP_Process_effective_close);
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_close
* Returned Values : None.
* Comments        :
*
*  Process TcpClose() requests, within TCP server.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_close
   (
      TCP_PARM_PTR   req_ptr     /* IN/OUT - the open request */
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;
   TCB_STRUCT_PTR       tcb;
   int_32               reply = RTCS_OK;

   tcp_cfg = RTCS_getcfg(TCP);
   tcb = req_ptr->TCB_PTR;

   if ( tcb == NULL ) { 
      RTCSCMD_complete(req_ptr, RTCSERR_TCP_CONN_RLSD);
      return;
   } 

   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_CONN_CLOSED++);

   if ( tcb->VALID != TCB_VALID_ID ) {
      reply = RTCSERR_TCP_CONN_RLSD;
      RTCS_log_error( ERROR_TCP,
                           RTCSERR_TCP_BAD_STATE_FOR_CLOSE,
                           (uint_32)tcb,
                           (uint_32)tcb->state,
                           1 );

   } else if ( tcb->state == CLOSED ) {

      /* This should be the standard case if the other side aborted. */
      TCP_Process_release(tcb, tcp_cfg);

   } else if ( (tcb->status & TCPS_FINTOSEND) != 0 ) {

      /* Upper layer must have already given us a tcpCLOSE, so we
       *    can generate an error here. */
      reply = RTCSERR_TCP_CONN_CLOSING;

   } else {

      switch ( tcb->state ) {

         case LISTEN:
         case SYN_SENT:
            TCP_Close_TCB(tcb, RTCS_OK, tcp_cfg);
          case BOUND:
            TCP_Process_release(tcb, tcp_cfg);
            break;

         case SYN_RECEIVED:
            /* no data has been sent */

         case ESTABLISHED:
         case CLOSE_WAIT:

            TCP_Event(tcb,TCPS_FINTOSEND);

            /*  We can only effectively close if all data has been sent,
             *   allowing us to send a FIN immediately; otherwise closing
             *   is delayed (TCPS_FINTOSEND indicates that connection should
             *   be closed as soon as all data is sent)
             */
            if ( tcb->sndnxt == (tcb->sndbufseq + tcb->sndlen) ) {
               TCP_Process_effective_close(tcb, tcp_cfg);
            } else {
               /*
               **  Have to kick ourselves into sending the outstanding
               **  data or we'll sit here forever if the other side is
               **  just waiting.  This would happen if we send w/o PUSH
               **  just before closing.
               */
               TCP_Must_send_ack(tcb, tcp_cfg);
            } /* Endif */

            break;

         default:
            /*  we shouldn't reach here, but jic...  */
            RTCS_log_error( ERROR_TCP,
                                 RTCSERR_TCP_BAD_STATE_FOR_CLOSE,
                                 (uint_32)tcb,
                                 (uint_32)tcb->state,
                                 2);

            reply = RTCSERR_TCP_NOT_CONN;
            break;

      } /* End Switch */

   } /* Endif */

   /* The socket will be freed as soon as we return to shutdown() */
   if (tcb->VALID == TCB_VALID_ID) {
       tcb->SOCKET = 0;
   } /* Endif */

   RTCSCMD_complete(req_ptr, reply);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_abort
* Returned Values : None.
* Comments        :
*
*  Process TcpAbort() requests, within TCP server.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_abort
   (
      TCP_PARM_PTR   req_ptr     /* IN/OUT - the open request */
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;
   TCB_STRUCT_PTR       tcb;
   int_32               reply = RTCS_OK;

   tcp_cfg = RTCS_getcfg(TCP);
   tcb = req_ptr->TCB_PTR;

   if ( tcb == NULL ) { 
      RTCSCMD_complete(req_ptr, RTCSERR_TCP_CONN_RLSD);
      return;
   } 

   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_CONN_ABORTS++);

   if ( tcb->VALID != TCB_VALID_ID ) {
      reply = RTCSERR_TCP_CONN_RLSD;

   } else {

      switch ( tcb->state ) {

         case LISTEN:
            /*
            ** allows for shutdown of listening socket with ABORT option set.
            */
            TCP_Close_TCB(tcb, RTCS_OK, tcp_cfg);
            TCP_Process_release(tcb, tcp_cfg);
            break;

         case SYN_RECEIVED:
         case ESTABLISHED:
         case FINWAIT_1:
         case FINWAIT_2:
         case CLOSE_WAIT:
            TCP_Send_reset(tcb, tcp_cfg);      /* send reset packet */
            /*FALLTHROUGH*/

         case SYN_SENT:
         case CLOSING:
         case LAST_ACK:
         case TIME_WAIT:
            TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_ABORTED, tcp_cfg);
            /*FALLTHROUGH*/

         case BOUND:
         case CLOSED:
            TCP_Process_release(tcb, tcp_cfg);
            break;

         default:
            reply = RTCSERR_TCP_NOT_CONN;
            break;

      } /* End Switch */

   } /* Endif */

   /* The socket will be freed as soon as we return to shutdown() */
   if (tcb->VALID == TCB_VALID_ID) {
      /* Following code no longer needed, as the option is propagated to the TCB when set */ 
      tcb->SOCKET = 0;
   } /* Endif */

   RTCSCMD_complete(req_ptr, reply);

} /* Endbody */

#if RTCSCFG_TCP_MAX_HALF_OPEN
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_half_open_TCB_close
* Returned Values : void
* Comments        :
* This function can be called to recover from SYN attack.
*
*END*-----------------------------------------------------------------*/

void TCP_half_open_TCB_close
   (
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer data */
   )
{ /* Body */
   TCB_STRUCT_PTR       tcb;
   uint_32              curr_time;
   uint_32              removed_tcb_count = 0;
   int                  i;

   curr_time = RTCS_time_get();
   /*
   ** Delete all the TCBs older than the default retransmission
   ** timeout (3 seconds).
   */
   for (i = 0; i < tcp_cfg->HALF_OPEN_TCB_COUNT; i++) {
      tcb = tcp_cfg->HALF_OPEN_TCB_LIST[i];
      if(RTCS_timer_get_interval(tcb->tcb_spawn_time, curr_time) > TCP_INITIAL_RTO_DEFAULT) {
         TCP_Send_reset(tcb, tcp_cfg);      /* send reset packet */
         TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_ABORTED, tcp_cfg);
         TCP_Process_release(tcb, tcp_cfg);
         i--;   /* we removed a TCB  and replaced another TCB
                   from the end, so we need to check same spot again */
         removed_tcb_count++;
      } /* Endif */
   } /* Endfor */

   /*
   ** Remove one TCB from the half open list randomly. Remove the TCB
   ** with a reset. If this is not an attack the client will resend the SYN.
   */
   if (!removed_tcb_count && tcp_cfg->HALF_OPEN_TCB_COUNT) {
      i = RTCS_rand() % tcp_cfg->HALF_OPEN_TCB_COUNT;
      tcb = tcp_cfg->HALF_OPEN_TCB_LIST[i];
      TCP_Send_reset(tcb, tcp_cfg);      /* send reset packet */
      TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_ABORTED, tcp_cfg);
      TCP_Process_release(tcb, tcp_cfg);
   } /* Endif */
} /* Endbody */
#endif

/* EOF */
