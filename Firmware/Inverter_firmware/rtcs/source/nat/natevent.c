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
* $FileName: natevent.c$
* $Version : 3.6.3.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the NAT event management functions.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include <rtcs_prv.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_event_add
* Returned Value  : void
* Comments        :
*     Insert a nat event into a nat event queue.
*
*END*-----------------------------------------------------------------*/

void NAT_event_add
   (
      TCPIP_EVENT_PTR         tcpip_event_ptr,         /* [IN] Controller event */
      NAT_EVENT_STRUCT_PTR    nat_event_ptr      /* [IN] NAT event to add */
   )
{ /* Body */
   NAT_EVENT_HEAD_PTR      nat_event_head_ptr = tcpip_event_ptr->PRIVATE;
   
   NAT_event_tick(tcpip_event_ptr, FALSE);
      
   /* We know the event will be placed at the end of the list */
   nat_event_ptr->TIME -= nat_event_head_ptr->TIMEDELTA;       
   nat_event_head_ptr->TIMEDELTA += nat_event_ptr->TIME;
   nat_event_ptr->NEXT = NULL;
   
   if (nat_event_head_ptr->FIRST) {      /* If there are already elements in list .. */
      nat_event_ptr->PREV = nat_event_head_ptr->LAST;
      nat_event_ptr->PREV->NEXT = nat_event_ptr;
      nat_event_head_ptr->LAST = nat_event_ptr;
      
   } else {
      nat_event_head_ptr->FIRST = nat_event_ptr;
      nat_event_head_ptr->LAST = nat_event_ptr;
      nat_event_ptr->PREV = NULL;
      tcpip_event_ptr->TIME = nat_event_ptr->TIME;
      TCPIP_Event_add(tcpip_event_ptr);
   } /* Endif */

   nat_event_ptr->EVENT = tcpip_event_ptr;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_event_del
* Returned Value  : void
* Comments        :
*     Removes a nat event from its event queue.
*
*END*-----------------------------------------------------------------*/

void NAT_event_del
   (
      NAT_EVENT_STRUCT_PTR    nat_event_ptr   /* [IN] NAT event to del */
   )
{ /* Body */
   TCPIP_EVENT_PTR      tcpip_event_ptr = nat_event_ptr->EVENT;
   NAT_EVENT_HEAD_PTR   nat_event_head_ptr;
   
   if (tcpip_event_ptr == NULL) {  /* If the event isn't in any queue, return */
      return;
   } /* Endif */ 
   
   /* If event is already deleted return */
   if ((nat_event_ptr->NEXT == NULL) && (nat_event_ptr->PREV == NULL) && (nat_event_ptr->EVENT == NULL))
   {
	   return;
   }
   NAT_event_tick(tcpip_event_ptr, FALSE);
   nat_event_head_ptr = tcpip_event_ptr->PRIVATE;   
   
   if (nat_event_ptr == nat_event_head_ptr->FIRST) {        /* 1st in the queue */
      TCPIP_Event_cancel(tcpip_event_ptr);
      nat_event_head_ptr->FIRST = nat_event_ptr->NEXT;
      if (nat_event_head_ptr->FIRST != NULL) {          /* If there are other nodes */
         nat_event_head_ptr->FIRST->TIME += nat_event_ptr->TIME;
         tcpip_event_ptr->TIME = nat_event_head_ptr->FIRST->TIME;
         nat_event_head_ptr->FIRST->PREV = NULL;
         TCPIP_Event_add(tcpip_event_ptr);
      } else { 
         nat_event_head_ptr->TIMEDELTA = 0;
         nat_event_head_ptr->LAST = NULL;
      } /* Endif */
   } else {                                  /* Not first in the queue */
      
      if (nat_event_head_ptr->LAST != nat_event_ptr) {                 /* Not last in the queue */
         nat_event_ptr->NEXT->TIME += nat_event_ptr->TIME;
         nat_event_ptr->NEXT->PREV = nat_event_ptr->PREV;
      } else { 
         nat_event_head_ptr->TIMEDELTA -= nat_event_ptr->TIME;
         nat_event_head_ptr->LAST = nat_event_ptr->PREV;
      } /* Endif */
      
      nat_event_ptr->PREV->NEXT = nat_event_ptr->NEXT;
      
   } /* Endif */   
   
   nat_event_ptr->NEXT = NULL;
   nat_event_ptr->PREV = NULL;
   nat_event_ptr->EVENT = NULL;
 
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_expire
* Returned Value  : TRUE if more events are left on the queue, FALSE otherwise
* Comments        :
*        Frees the memory associated with a session.
*
*END*-----------------------------------------------------------------*/

boolean NAT_expire
   (
      TCPIP_EVENT_PTR   tcpip_event_ptr      /* [IN] Points to the event that expired */
   ) 
{ /* Body */
   NAT_EVENT_HEAD_PTR      nat_event_head_ptr = tcpip_event_ptr->PRIVATE;
   NAT_EVENT_STRUCT_PTR    tmp_nat_event_ptr, nat_event_ptr = nat_event_head_ptr->FIRST;
      
   NAT_event_tick(tcpip_event_ptr, TRUE);
   
   while (nat_event_ptr && nat_event_ptr->TIME == 0) {
      tmp_nat_event_ptr = nat_event_ptr->NEXT;
      
      /* De-thread node and delete session corresponding to node */
      if (tmp_nat_event_ptr) { 
         tmp_nat_event_ptr->PREV = NULL; 
      } /* Endbody */
      
      NAT_delete(nat_event_ptr->SESSION_PTR);
      
      nat_event_ptr = tmp_nat_event_ptr;
   } /* Endwhile */
   
   nat_event_head_ptr->FIRST = tmp_nat_event_ptr;
   
   if (tmp_nat_event_ptr) {
      tcpip_event_ptr->TIME = tmp_nat_event_ptr->TIME;
      return TRUE;
   } else {
      nat_event_head_ptr->LAST = NULL;
   } /* Endif */
   
   return FALSE;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_event_tick
* Returned Value  : void
* Comments        :
*     Advances time. 
*
*END*-----------------------------------------------------------------*/

void NAT_event_tick
   (
      TCPIP_EVENT_PTR   tcpip_event_ptr,   /* [IN] Controlling event           */
      boolean           force    /* [IN] Force expiry of first event */
   )
{ /* Body */
   NAT_EVENT_HEAD_PTR   nat_event_head_ptr = tcpip_event_ptr->PRIVATE;
   uint_32              msec, elapsed;

   msec = RTCS_time_get();
   
   if (nat_event_head_ptr->FIRST) {
   
      if (force) {
         elapsed = nat_event_head_ptr->FIRST->TIME;   
      } else {
         elapsed = RTCS_timer_get_interval(nat_event_head_ptr->TIMESTAMP, msec); 
      } /* Endif */

      if (nat_event_head_ptr->FIRST->TIME <= elapsed) {
         nat_event_head_ptr->FIRST->TIME = 0;
      } else { 
         nat_event_head_ptr->FIRST->TIME -= elapsed;
      } /* Endif */
      
      if (nat_event_head_ptr->TIMEDELTA <= elapsed) {
         nat_event_head_ptr->TIMEDELTA = 0;
      } else { 
         nat_event_head_ptr->TIMEDELTA -= elapsed;
      } /* Endif */
      
   } /* Endif */
   
   nat_event_head_ptr->TIMESTAMP = msec;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_tout_max
* Returned Value  : void 
* Comments        :
*     Sets a maximum timeout for the NAT events in the specified queue. 
*     Any timeouts greater than the maximum will be set to the maximum.
*
*END*-----------------------------------------------------------------*/

void NAT_tout_max
   (
      TCPIP_EVENT_PTR   tcpip_event_ptr,    /* [IN] Event queue to update */
      uint_32           max      /* [IN] New maximum timeout for events in queue */
   )
{ /* Body */
   NAT_EVENT_HEAD_PTR   nat_event_head_ptr = tcpip_event_ptr->PRIVATE;
   NAT_EVENT_STRUCT_PTR nat_event_ptr;
   uint_32              total = 0;
   
   nat_event_ptr = nat_event_head_ptr->FIRST;
   
   /* If the queue is empty or does not exceed max, do nothing */
   if ((nat_event_ptr==NULL) || (nat_event_head_ptr->TIMEDELTA <= max)) {  
      return;
   } /* Endif */
   
   /* The time for the whole queue must be max or less */
   nat_event_head_ptr->TIMEDELTA = max;
   
   /* Do not modify events expiring in less than max */
   while (nat_event_ptr && total <= max) {
      total += nat_event_ptr->TIME;
      nat_event_ptr = nat_event_ptr->NEXT;
   } /* Endwhile */

   /* The event's timeout (if it exists) exceeds max. Fix it */
   if (nat_event_ptr) {
      nat_event_ptr->TIME -= (total - max);
      nat_event_ptr = nat_event_ptr->NEXT;
   } /* Endif */

   /* Set the remaining timeouts to 0, which is equal to a timeout of max */
   while (nat_event_ptr) { 
      nat_event_ptr->TIME = 0;
      nat_event_ptr = nat_event_ptr->NEXT;
   } /* Endwhile */
      
} /* Endbody */

#endif

/* EOF */
