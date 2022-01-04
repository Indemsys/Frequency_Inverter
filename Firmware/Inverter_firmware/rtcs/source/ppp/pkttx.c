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
* $FileName: pkttx.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the PPP Tx
*   task.
*
*END************************************************************************/

#include <ppp.h>
#include "ppp_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_send
* Returned Value  :  error code
* Comments        :
*     Sends a PPP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 PPP_send
   (
      _ppp_handle    handle,
            /* [IN] - the PPP state structure */
      uint_16        protocol,
            /* [IN] - protocol for the packet */
      PCB_PTR        pcb
            /* [IN] - the packet to send */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR       ppp_ptr = handle;
   PCB_FRAGMENT_PTR  frag_ptr;
   uint_32           size, mru;
   uint_32           error;

   /* Do some error checking */
   if (ppp_ptr->VALID != PPP_VALID) {
      PCB_free(pcb);
      return RTCSERR_PPP_INVALID_HANDLE;
   } else if (!(protocol&1) || (protocol&0x100)) {
      PCB_free(pcb);
      return RTCSERR_PPP_INVALID_PROTOCOL;
   } /* Endif */

   error = PPP_OK;
   PPP_mutex_lock(&ppp_ptr->MUTEX);
   if (!ppp_ptr->LINK_STATE) {
      error = RTCSERR_PPP_LINK_NOT_OPEN;
   } else {
      /* SPR P122-0371-01 */
      /* PCB includes 2-byte protocol field, but negotiated MRU doesn't */
      mru = ppp_ptr->SEND_OPTIONS->MRU + 2;
      /* End SPR P122-0371-01 */
   } /* Endif */
   PPP_mutex_unlock(&ppp_ptr->MUTEX);
   if (error) {
      PCB_free(pcb);
      return error;
   } /* Endif */

   /*
   ** Make sure the first fragment is long enough for the PPP protocol
   ** field.  This isn't strictly necessary, but it's impractical
   ** to split a 2 byte field over multiple fragments.
   */
   if (pcb->FRAG[0].LENGTH < 2) {
      PCB_free(pcb);
      return RTCSERR_PPP_PACKET_TOO_SHORT;
   } /* Endif */

   /*
   ** Make sure that no fragment exceeds a maximum packet length.
   ** We check every fragment because we want to prevent something
   ** like FRAG[0].LENGTH = 2000, FRAG[1].LENGTH = -1000.  This
   ** situation would not be detected if we only check the total
   ** length.
   */
   size = 0;
   for (frag_ptr = pcb->FRAG; frag_ptr->LENGTH; frag_ptr++) {
      if (frag_ptr->LENGTH > mru) {
         PCB_free(pcb);
         return RTCSERR_PPP_PACKET_TOO_LONG;
      } /* Endif */
      size += frag_ptr->LENGTH;
   } /* Endfor */

   /*
   ** Make sure that the total sum of the fragments doesn't exceed
   ** a maximum packet length.
   */
   if (size > mru) {
      PCB_free(pcb);
      return RTCSERR_PPP_PACKET_TOO_LONG;
   } /* Endif */

   /*
   ** Everything checks out -- send the packet to the Tx task.
   */
   htons(pcb->FRAG[0].FRAGMENT, protocol);
   return PPP_send_one(ppp_ptr, protocol, pcb);

#else

    return RTCSERR_IP_IS_DISABLED;    

#endif /* RTCSCFG_ENABLE_IP4 */     

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_send_one
* Returned Value  :  None
* Comments        :
*     Sends a PPP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 PPP_send_one
   (
      PPP_CFG_PTR    ppp_ptr,
            /* [IN] - PPP handle */
      uint_16        protocol,
            /* [IN] - protocol for the packet */
      PCB_PTR        pcb
            /* [IN] - packet */
   )
{ /* Body */
   PPP_MESSAGE_PTR   message;

   message = RTCS_msg_alloc(ppp_ptr->MSG_POOL);
   if (message == NULL) {
      return RTCSERR_PPP_OUT_OF_BUFFERS;
   } /* Endif */

   message->HEADER.SIZE       = sizeof(*message);
   message->HEADER.TARGET_QID = ppp_ptr->MSG_QUEUE;

   message->COMMAND  = PPPCMD_SEND;
   message->PROTOCOL = protocol & 0xFFFF;
   message->PCB      = pcb;
   message->TIMEOUT  = 0;

   RTCS_msgq_send(message, ppp_ptr->MSG_POOL);
   return PPP_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_send_rexmit
* Returned Value  :  None
* Comments        :
*     Sends a PPP packet, with retransmissions.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 PPP_send_rexmit
   (
      PPP_CFG_PTR       ppp_ptr,
            /* [IN] - PPP handle */
      uint_16           protocol,
            /* [IN] - protocol for the packet */
      PCB_PTR           pcb,
            /* [IN] - packet */
      uint_32           retries,
            /* [IN] - maximum number of retries */
      void (_CODE_PTR_  timeout)(pointer, PCB_PTR, boolean),
            /* [IN] - function to call after max retransmissions */
      pointer           param
            /* [IN] - parameter for timeout function */
   )
{ /* Body */
   PPP_MESSAGE_PTR   message;

   message = RTCS_msg_alloc(ppp_ptr->MSG_POOL);
   if (message == NULL) {
      return RTCSERR_PPP_OUT_OF_BUFFERS;
   } /* Endif */

   message->HEADER.SIZE       = sizeof(*message);
   message->HEADER.TARGET_QID = ppp_ptr->MSG_QUEUE;

   message->COMMAND  = PPPCMD_SEND;
   message->PROTOCOL = protocol & 0xFFFF;
   message->PCB      = pcb;
   message->TIMEOUT  = _PPP_MIN_XMIT_TIMEOUT;
   message->RETRY    = retries;
   message->CALL     = timeout;
   message->PARAM    = param;

   RTCS_msgq_send(message, ppp_ptr->MSG_POOL);
   return PPP_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_send_shutdown
* Returned Value  :  None
* Comments        :
*     send a message to remove tx task.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 PPP_send_shutdown
   (
      PPP_CFG_PTR       ppp_ptr,
            /* [IN] - PPP handle */
      pointer           param
            /* [IN] - parameter for timeout function */
   )
{ /* Body */
   PPP_MESSAGE_PTR   message;

   message = RTCS_msg_alloc(ppp_ptr->MSG_POOL);
   if (message == NULL) {
      return RTCSERR_PPP_OUT_OF_BUFFERS;
   } /* Endif */

   message->HEADER.SIZE       = sizeof(*message);
   message->HEADER.TARGET_QID = ppp_ptr->MSG_QUEUE;

   message->COMMAND  = PPPCMD_SHUTDOWN;
   message->TIMEOUT  = 0;
   message->PARAM    = param;

   RTCS_msgq_send(message, ppp_ptr->MSG_POOL);
   return PPP_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_send_restart
* Returned Value  :  None
* Comments        :
*     Restart the retransmission timer for a particular protocol.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 PPP_send_restart
   (
      PPP_CFG_PTR       ppp_ptr,
            /* [IN] - PPP handle */
      uint_16           protocol
            /* [IN] - protocol for the packet */
   )
{ /* Body */
   PPP_MESSAGE_PTR   message;

   message = RTCS_msg_alloc(ppp_ptr->MSG_POOL);
   if (message == NULL) {
      return RTCSERR_PPP_OUT_OF_BUFFERS;
   } /* Endif */

   message->HEADER.SIZE       = sizeof(*message);
   message->HEADER.TARGET_QID = ppp_ptr->MSG_QUEUE;

   message->COMMAND  = PPPCMD_RESTART;
   message->PROTOCOL = protocol & 0xFFFF;

   RTCS_msgq_send(message, ppp_ptr->MSG_POOL);

   return PPP_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_send_stop
* Returned Value  :  None
* Comments        :
*     Abort the retransmission timer for a particular protocol.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 PPP_send_stop
   (
      PPP_CFG_PTR       ppp_ptr,
            /* [IN] - PPP handle */
      uint_16           protocol
            /* [IN] - protocol for the packet */
   )
{ /* Body */
   PPP_MESSAGE_PTR   message;

   message = RTCS_msg_alloc(ppp_ptr->MSG_POOL);
   if (message == NULL) {
      return RTCSERR_PPP_OUT_OF_BUFFERS;
   } /* Endif */

   message->HEADER.SIZE       = sizeof(*message);
   message->HEADER.TARGET_QID = ppp_ptr->MSG_QUEUE;

   message->COMMAND  = PPPCMD_STOP;
   message->PROTOCOL = protocol & 0xFFFF;

   RTCS_msgq_send(message, ppp_ptr->MSG_POOL);

   return PPP_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_tx_pcbfree
* Returned Value  :  void
* Comments        :
*     Replacement for a PCB's FREE function.
*
*     When the Tx task needs to retransmit a packet, it tries
*     to reuse the PCB sent by the application.  Unfortunately,
*     _iopcb_write() frees the PCB when it's done.
*
*     So, when the Tx task detects that it may need to retransmit
*     a PCB, it replaces its FREE function with this empty
*     function, while preserving the original FREE function in
*     message->ORIG_FREE.
*
*     When the Tx finally does want to free the PCB, (either when
*     message->RETRIES reaches 0, or it receives a PPPCMD_STOP
*     command), it restores the original FREE function and calls
*     PCB_free().
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4

static void PPP_tx_pcbfree(PCB_PTR pcb) {}

#endif /* RTCSCFG_ENABLE_IP4 */

/*TASK*-----------------------------------------------------------------
*
* Task Name       :   PPP_tx_task
* Comments        :
*     This task receives requests from PPP_send().
*
*  When we receive a packet with a nonzero TIMEOUT field, we will
*  resend the packet RETRY times.
*
*  Packets to be retransmitted are put on a linked list (called the
*  retransmission queue) starting at xmithead.  This list is sorted
*  by time as follows:  for each packet 'message', message->DELTA is
*  the amount of time to wait before sending message, after sending
*  previous packet.  That is, xmithead->DELTA is the amount of time
*  to wait before sending xmithead, and xmithead->NEXT->DELTA is the
*  amount of time to wait bewteen sending xmithead and xmithead->NEXT.
*  Thus whenever we measure elapsed time, we only decrement the head
*  of the retransmission queue.
*
*  Every time through the loop, we do the following:
*
*     1) If the head of the retransmission queue timed out
*        (xmithead->DELTA <= 0), we resend it.
*
*     2) If the retransmission queue is nonempty, but the head
*        hasn't timed out, call _msgq_receive() with timeout
*        xmithead->DELTA.
*
*     3) If the retransmission queue is empty, call _msgq_receive()
*        with no timeout.
*
*     4) If _msgq_receive() timed out (case 2 only), resend the
*        head of the retransmission queue.
*
*     5) If _msgq_receive() obtained a control message (case 2 or 3),
*        process it.
*
*     6) If _msgq_receive() obtained a packet (case 2 or 3), send it.
*
*END*-----------------------------------------------------------------*/

void PPP_tx_task
   (
      pointer     handle,
            /* [IN] - the PPP state structure */
      pointer     creator
            /* [IN] - the creation information */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   PPP_CFG_PTR       ppp_ptr = handle;
   uint_32           ctrl;
   boolean           state;
   PPP_OPT           opt;
   PCB_PTR           pcb;
   uint_32           pcbopt;
   PPP_MESSAGE_PTR   message, xmithead, _PTR_ xmitnode;
   uint_32           timebefore, timeafter;
   boolean           wait;
   int_32            timeout;
   uint_16           protocol;
   _queue_id         queue;
   pointer           param;

   /* Obtain a message queue so that PPP_send() can reach us */
   queue = RTCS_msgq_open(0, 0);
   if (queue == 0) {
      RTCS_task_exit(creator, RTCSERR_PPP_OPEN_QUEUE_FAILED);
   } /* Endif */

   RTCS_task_resume_creator(creator, PPP_OK);

   ppp_ptr->MSG_QUEUE = queue;

   xmithead = NULL;
   timebefore = RTCS_time_get();

#define PPPTX_DISCARD(msg) \
            (msg)->PCB->FREE = (msg)->ORIG_FREE;   \
            PCB_free((msg)->PCB);                  \
            RTCS_msg_free(msg)

   for (;;) {

      /**********************************************************
      **
      **    Wait for a packet to send
      **
      */

      wait = TRUE;
      timeout = 0;

      /* Check if a packet is waiting for retransmission */
      if (xmithead) {

         /* Measure elapsed time */
         timeafter = RTCS_time_get();
         timeout = RTCS_timer_get_interval(timebefore, timeafter); 
         timebefore = timeafter;
         xmithead->DELTA -= timeout;

         /* If its timer has expired, send it */
         timeout = xmithead->DELTA;
         if (timeout <= 0) {
            message = NULL;
            wait = FALSE;
         } /* Endif */
      } /* Endif */

      /*
      ** There are three cases at this point:
      **    1) Retransmission queue is empty
      **          (wait == TRUE,  timeout == 0, message == ?)
      **    2) Retransmission queue is nonempty, positive timeout
      **          (wait == TRUE,  timeout > 0,  message == ?)
      **    3) Retransmission queue is nonempty, nonpositive timeout,
      **       i.e., head of retransmission queue timed out
      **          (wait == FALSE, timeout == 0, message == NULL)
      */

      /* If there are no expired messages, block */
      if (wait) {
         message = RTCS_msgq_receive(queue, timeout, ppp_ptr->MSG_POOL);
      } /* Endif */

      /*
      ** There are two cases at this point:
      **    1) We got a message from _msgq_receive
      **          (message != NULL)
      **    2) Head of retransmission queue timed out
      **          (message == NULL)
      */

      /* We got a packet to send */
      if (message) {

         /* Control packets restart or stop retransmission */
         ctrl = message->COMMAND;

         if (ctrl == PPPCMD_SHUTDOWN) {
            param = message->PARAM;
            /* Free the message we just got */
            RTCS_msg_free(message);
            for (xmitnode = &xmithead;;){
               if (*xmitnode == NULL) {
                  break;
               } else {
                 /* unlink */
                 message = *xmitnode;
                 *xmitnode = message->NEXT;
                 /* Free the message from the list */
                 PPPTX_DISCARD(message);
               } /* Endif */
            } /* Endfor */
            RTCS_msgq_close(queue);
            /* Unblock PPP_shutdown() */
            RTCS_sem_post(param);
            /* Kill self */
            return;
         } /* Endif */
         
         if (ctrl != PPPCMD_SEND) {
            protocol = message->PROTOCOL;
            RTCS_msg_free(message);

            /*
            ** Search the retransmission queue for a packet
            ** matching the protocol field of the control message
            */
            for (xmitnode = &xmithead;;
                 xmitnode = &(*xmitnode)->NEXT) {
               if (*xmitnode == NULL) {
                  break;
               } else if ((*xmitnode)->PROTOCOL == protocol) {
                  message = *xmitnode;
                  switch (ctrl) {
                  case PPPCMD_RESTART:
                     message->TIMEOUT = _PPP_MIN_XMIT_TIMEOUT;
                     message->RETRY = _PPP_MAX_CONF_RETRIES;
                     break;
                  case PPPCMD_STOP:
                     if (message->NEXT) {
                        message->NEXT->DELTA += message->DELTA;
                     } /* Endif */
                     *xmitnode = message->NEXT;
                     PPPTX_DISCARD(message);
                     break;
                  } /* Endswitch */
                  break;
               } /* Endif */
            } /* Endfor */

            continue;
         } /* Endif */

         /* Save the PCB's FREE field */
         message->ORIG_FREE = message->PCB->FREE;
         if (message->TIMEOUT) {
            message->PCB->FREE = PPP_tx_pcbfree;
         } /* Endif */

      /* Receive timed out -- get the head of the retransmission queue */
      } else {
         message = xmithead;
         xmithead = message->NEXT;
         if (xmithead) {
            xmithead->DELTA += message->DELTA;
         } /* Endif */

         /* Generate a TO event */
         if (message->CALL) {
            message->CALL(message->PARAM, message->PCB, message->RETRY == 1);
         } /* Endif */

         /* RETRY == 0 means retry forever */
         if (message->RETRY) {

            /* When the retry count reaches zero, discard the packet */
            if (!--message->RETRY) {
               PPPTX_DISCARD(message);
               continue;
            } /* Endif */
         } /* Endif */

         /* Use exponential backoff when retransmitting */
         message->TIMEOUT <<= 1;
         if (message->TIMEOUT > _PPP_MAX_XMIT_TIMEOUT) {
            message->TIMEOUT = _PPP_MAX_XMIT_TIMEOUT;
         } /* Endif */
      } /* Endif */

      /**********************************************************
      **
      **    We have a packet -- validate it
      **
      */

      /* Take a snapshot of the current state and send options */
      PPP_mutex_lock(&ppp_ptr->MUTEX);
      state = ppp_ptr->LINK_STATE;
      opt = *ppp_ptr->SEND_OPTIONS;
      PPP_mutex_unlock(&ppp_ptr->MUTEX);

      /* Send the data packet (unless the link is not open) */
      if (!state && message->PROTOCOL != PPP_PROT_LCP) {
         PPPTX_DISCARD(message);
         ppp_ptr->ST_TX_DISCARDED++;
         continue;
      } /* Endif */

      /**********************************************************
      **
      **    We have a valid packet -- send it
      **
      */

      /* LCP control packets are always sent with default options */
      if (message->PROTOCOL == PPP_PROT_LCP
       && message->PCB->FRAG[0].FRAGMENT[2] >= 1
       && message->PCB->FRAG[0].FRAGMENT[2] <= 7) {
         pcbopt = 1;
      } else {
         pcbopt = 0;
      } /* Endif */

      pcb = message->PCB;

      /* Only data packets are compressed */
      /* Start CR 2296 */
      if (((message->PROTOCOL & 0xC000) == 0) && opt.CP) {
      /* End CR 2296 */
         pcb = opt.CP->CP_comp(&ppp_ptr->CCP_STATE.SEND_DATA, pcb, ppp_ptr, &opt);
         htons(pcb->FRAG[0].FRAGMENT, PPP_PROT_CP);
      } /* Endif */

      _iopcb_write(ppp_ptr->DEVICE, pcb, pcbopt);

      /**********************************************************
      **
      **    Packet is sent -- update retransmission queue
      **
      */

      /* Free the buffer unless it may need to be retransmitted */
      if (message->TIMEOUT) {

         /* Measure elapsed time */
         timeafter = RTCS_time_get();
         timeout = RTCS_timer_get_interval(timebefore, timeafter); 
         timebefore = timeafter;
         if (xmithead) {
            xmithead->DELTA -= timeout;
         } /* Endif */

         /* Insert packet into proper place in retransmission queue */
         for (message->DELTA = message->TIMEOUT,    xmitnode = &xmithead;;
              message->DELTA -= (*xmitnode)->DELTA, xmitnode = &(*xmitnode)->NEXT) {
            if (*xmitnode == NULL) {
               /* Add packet at tail of queue */
               message->NEXT = NULL;
               *xmitnode = message;
               break;
            } else if ((*xmitnode)->DELTA > message->TIMEOUT) {
               /* Insert packet in middle (possibly head) of queue */
               (*xmitnode)->DELTA -= message->DELTA;
               message->NEXT = *xmitnode;
               *xmitnode = message;
               break;
            } /* Endif */
         } /* Endfor */

      } else {
         /* PCB has already been freed by _iopcb_write() */
         RTCS_msg_free(message);
      } /* Endif */

   } /* Endfor */
   
#endif /* RTCSCFG_ENABLE_IP4 */
   
} /* Endbody */


/* EOF */
