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
* $FileName: sosel0.c$
* $Version : 3.0.4.0$
* $Date    : Jan-13-2009$
*
* Comments:
*
*   This file contains the RTCS select() support functions.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"
#include "tcp_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_activity
* Returned Value  : TRUE or FALSE
* Comments        : Determine whether there is any activity on a socket.
*
*END*-----------------------------------------------------------------*/

boolean SOCK_select_activity
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      int flag                          // direction flag - 0 recv, non zero send
   )
{ /* Body */
   TCB_STRUCT_PTR tcb_ptr;
   boolean        activity = FALSE;

   /* Check closing and connected stream sockets for data */
   if (socket_ptr->TCB_PTR) {
      tcb_ptr = socket_ptr->TCB_PTR;
   
      if (!flag) {
         /*
         ** Check that there is data in the receive ring or
         ** that the socket has been closed by the peer
         */
         if ((tcb_ptr->conn_pending) || (tcb_ptr->state == CLOSED) ||
            /* START CR 2349 */
            //(tcb_ptr->rcvnxt > tcb_ptr->rcvbufseq)) {
            (GT32(tcb_ptr->rcvnxt, tcb_ptr->rcvbufseq))) {
            /* END CR 2349 */
            activity = TRUE;
         }
      }
      else {
         if (!tcb_ptr->sndlen) {    // TODO - remake for partialy empty send buffers
            activity = TRUE;
         }
      }

   /* Check datagram sockets for data */
   } else if (socket_ptr->UCB_PTR) {
      /*
      ** Check that there is queued data
      */
      if (socket_ptr->UCB_PTR->PHEAD) {
         activity = TRUE;
      } /* Endif */

   /* TCB=UCB=NULL is a TCP connection reset by peer */
   } else {

      activity = TRUE;

   } /* Endif */

   return activity;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_block
* Returned Values :
* Comments  :
*     Enqueues a select() call on the select queue.
*
*END*-----------------------------------------------------------------*/

void SOCK_select_block
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);

   parms->NEXT = socket_cfg_ptr->SELECT_HEAD;
   if (parms->NEXT) {
      parms->NEXT->PREV = &parms->NEXT;
   } /* Endif */
   socket_cfg_ptr->SELECT_HEAD = parms;
   parms->PREV = &socket_cfg_ptr->SELECT_HEAD;

   if (parms->timeout) {
      parms->EXPIRE.TIME    = parms->timeout;
      parms->EXPIRE.EVENT   = SOCK_select_expire;
      parms->EXPIRE.PRIVATE = parms;
      TCPIP_Event_add(&parms->EXPIRE);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_expire
* Returned Values :
* Comments  :
*     Called by the Timer.  Expire a select call.
*
*END*-----------------------------------------------------------------*/

boolean SOCK_select_expire
   (
      TCPIP_EVENT_PTR   event
   )
{ /* Body */
   SOCKSELECT_PARM_PTR  parms = event->PRIVATE;

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } /* Endif */
   *parms->PREV = parms->NEXT;

   parms->sock = 0;
   RTCSCMD_complete(parms, RTCS_OK);

   return FALSE;
} /* Endbody */


/* EOF */
