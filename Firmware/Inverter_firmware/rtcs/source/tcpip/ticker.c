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
* $FileName: ticker.c$
* $Version : 3.6.5.0$
* $Date    : Dec-2-2010$
*
* Comments:
*
*   Timer server and interface for the TCP layer.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "ticker.h"     /* for timer definitions */
#include "tcpip.h"      /* for error definitions */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Timer_schedule
* Returned Values : None.
* Comments        :
*
*  Insert node `timeq' into schedule queue;
*   if other TimeQ's are scheduled for the same time,
*   `timeq' will be inserted *after* them.
*
*  Fields of `timeq' which must defined prior to call:
*      timeq->delay
*      timeq->reload
*      timeq->donelist
*      timeq->client
*      timeq->count
*
*  Also assumes `timeq' is not in any list (timeq->curlist == 0).
*
*  This may be called by any task, not necessarily by the TCP_Tick_server;
*   however the calling task must have lower priority than the TCP_Tick_server
*   (which is normally the case).
*
*END*-----------------------------------------------------------------*/

void TCP_Timer_schedule
   (
      TimeQ _PTR_       timeq,      /* IN/OUT - The timer to schedule */
      TimeQ _PTR_ _PTR_ qhead_ptr   /* IN/OUT - The head of the timer queue */
   )
{ /* Body */
   TimeQ    _PTR_ scan,
            _PTR_ last;
   int_32   delta;
   
   RTCSLOG_FNE3(RTCSLOG_FN_TCP_Timer_schedule, timeq, qhead_ptr);

   /* Check assumption */
   if ( timeq->curlist != NULL ) {
      RTCS_log_error( ERROR_TCPIP,
                           RTCSERR_TCPIP_TIMER_CORRUPT,
                           (uint_32)timeq,
                           0, 0 );
      timeq->curlist = NULL;
   } /* Endif */

   /*  Find insertion point into 'schedule' queue
    */
   for ( last=NULL, scan=*qhead_ptr, delta=timeq->delay;
       scan != NULL && delta >= scan->delay;
       last=scan, scan=scan->next)
   {
      if ( last == scan ) { /* Corrupt list */
         RTCS_log_error( ERROR_TCPIP,
                              RTCSERR_TCPIP_TIMER_CORRUPT,
                              (uint_32)timeq,
                              1, 0 );
         scan = NULL;
      } else {
         delta -= scan->delay;
      } /* Endif */
   } /* Endfor */

   /*  Queue request (insert in queue)
    */
   timeq->delay = delta;
   timeq->next = scan;
   timeq->prev = last;
   timeq->curlist = qhead_ptr;

   if ( last != NULL ) {
      last->next = timeq;
   } else {
      *qhead_ptr = timeq;
   } /* Endif */

   if ( scan != NULL ) {
      scan->prev = timeq;
      scan->delay -= delta;      /* adjust next delta */
   } /* Endif */
   
   RTCSLOG_FNX1(RTCSLOG_FN_TCP_Timer_schedule);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Timer_expire
* Returned Values : None.
* Comments        :
*
*  For oneshot timers only; assumes `timeq' is not in any list.
*  Counts timeout, generates signal, and puts timeq in its donelist (if any).
*
*END*-----------------------------------------------------------------*/

   
void  TCP_Timer_expire
   (
      TimeQ _PTR_ timeq          /* IN/OUT - The expired timer */
   )
{ /* Body */
   TimeQ _PTR_ _PTR_ dlist;
   RTCSLOG_FNE2(RTCSLOG_FN_TCP_Timer_expire, timeq);
   
   timeq->count++;            /* count timeouts */

   dlist = timeq->donelist;

   /*
   ** Link onto front of donelist
   */
   if ( dlist != NULL ) {
      timeq->next = *dlist;
      if ( timeq->next != NULL ) {
         timeq->next->prev = timeq;
      } /* Endif */
      *dlist = timeq;
   } else {
      timeq->next = NULL;
   } /* Endif */
   timeq->prev = NULL;
   timeq->curlist = dlist;
   
   RTCSLOG_FNX1(RTCSLOG_FN_TCP_Timer_expire);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Timer_remove
* Returned Values : None.
* Comments        :
*
*  Removes a timer from the list which contains it.
*
*  Assumes timer is not active (i.e. assumes timeq->curlist != &qhead),
*   because it doesn't properly remove timer from qhead list
*   (use TCP_Timer_stop() for that).
*
*END*-----------------------------------------------------------------*/

void  TCP_Timer_remove
   (
      TimeQ _PTR_ timeq    /* IN/OUT - The timer to remove */
   )
{ /* Body */
   TimeQ _PTR_ next;
   
   RTCSLOG_FNE2(RTCSLOG_FN_TCP_Timer_remove, timeq);

   if ( timeq->curlist == NULL ) {   /* Not active and not on a donelist */
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_remove, -1);
      return;
   } /* Endif */

   next = timeq->next;
   if ( next != NULL ) {             /* unlink from next */
      next->prev = timeq->prev;
      timeq->next = NULL;
   } /* Endif */

   if ( timeq->prev != NULL ) {      /* unlink from prev */
      timeq->prev->next = next;
      timeq->prev = NULL;

   } else if ( timeq == *timeq->curlist ) { /* remove from list head */
      *timeq->curlist = next;

   } else {
      RTCS_log_error( ERROR_TCPIP,
                           RTCSERR_TCPIP_TIMER_CORRUPT,
                           (uint_32)timeq,
                           2, 0 );
   } /* Endif */

   timeq->curlist = NULL;
   
   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_remove, 0);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Timer_stop
* Returned Values : Positive error number.
* Comments        :
*
*  Stop a oneshot or cyclic timer; cannot be used to stop delays.
*
*  If the timer is active, it is *not* inserted into its `donelist' after
*   being stopped; if the timer is expired/idle, it is removed from its
*   `donelist' if present.
*
*  Returns OK, or RTCSERR_TCP_TIMED_OUT if timer was expired/idle.
*
*END*-----------------------------------------------------------------*/

int_32 TCP_Timer_stop
   (
      TimeQ _PTR_       timeq,      /* The timer to stop */
      TimeQ _PTR_ _PTR_ qhead_ptr   /* The timer queue head */
   )
{ /* Body */
   RTCSLOG_FNE3(RTCSLOG_FN_TCP_Timer_stop, timeq, qhead_ptr);

   /*  check to see if already timed out
    */
   if ( timeq->curlist != qhead_ptr ) { /* not active */
      TCP_Timer_remove(timeq);
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_stop, RTCSERR_TCP_TIMED_OUT);
      return RTCSERR_TCP_TIMED_OUT;
   } /* Endif */

   /*  Stop timer - remove from queue
    */
   if ( timeq->next != NULL ) {   /* adjust next delta */
      timeq->next->delay += timeq->delay;
      timeq->next->prev = timeq->prev;
   } /* Endif */

   if ( timeq->prev != NULL ) {
      timeq->prev->next = timeq->next;

   } else {
      *qhead_ptr = timeq->next;

   } /* Endif */

   timeq->prev = NULL;
   timeq->next = NULL;
   timeq->curlist = NULL;
   
   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_stop, RTCS_OK);
   return RTCS_OK;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Timer_start
* Returned Values : Positive error value.
* Comments        :
*
*  Start a oneshot or cyclic timer (timeq);
*
*  The timer must have been initialized at some point prior to calling this
*   function, with TimerInit() (or cleared will all zeroes, which is what
*   TimerInit() does); it donelist however may be set.
*
*  If the timer is currently active, it is stopped before proceeding
*   (logically; this may be optimized, e.g. incremented directly to requested
*    delay).
*
*  If the timer is a oneshot (reload == 0), and timeq->donelist is non-zero,
*   then upon timer expiry the timer will be inserted at the head of this
*   (*timeq->donelist) list.
*
*  If the timer is cyclic (reload != 0), then the timer will tick after
*   `delay' msecs, and thereafter every `reload' msecs.
*
*  If the timer was in a list (e.g., its donelist) before calling TCP_Timer_start(),
*   it is first removed.
*
*  Maximum delay or reload is 0x7fffffff (2^31-1), or 24 days+20:31:23.647;
*   negative delay is interpreted as zero;
*   a zero delay for a cyclic timer (reload != 0) maps to a delay = reload;
*   else a zero delay immediately expires the timer and puts it in its donelist
*    (if any), in which case TCP_Timer_start() returns RTCSERR_TCP_TIMED_OUT;
*   a negative reload is invalid.
*
*  Initially timeq->count is cleared.
*
*  Upon oneshot timer expiry, and upon every tick of a cyclic timer,
*   timeq->count is incremented.
*
*  Returns OK, RTCSERR_TCPIP_INVALID_ARGUMENT if reload < 0,
*   or RTCSERR_TCP_TIMED_OUT (for zero delays).
*
*END*-----------------------------------------------------------------*/

int_32 TCP_Timer_start
   (
      TimeQ _PTR_       timeq,      /* IN/OUT - The timer */
      int_32            delay,      /* IN     - No. msec until timeout */
      int_32            reload,     /* IN     - No. msec to dly after 1st to */
      TimeQ _PTR_ _PTR_ qhead_ptr   /* IN/OUT - Head of timer queue */
   )
{ /* Body */
   int_32   delta;
   uint_32  newtime;
   int_32 error;
   
   RTCSLOG_FNE5(RTCSLOG_FN_TCP_Timer_start, timeq, delay, reload, qhead_ptr);

   if ( reload < 0 ) {  /* i.e. delay attempted */
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_start, RTCSERR_TCPIP_INVALID_ARGUMENT);
      return RTCSERR_TCPIP_INVALID_ARGUMENT;
   } /* Endif */

   newtime = RTCS_time_get();
   if ( timeq->curlist == qhead_ptr ) { /* currently active timer */

      delta = timeq->abs_to - newtime;
      if ( delay > delta ) {           /* Longer delay requested */
         timeq->reload = reload;
         timeq->count = 0;
         error = TCP_Timer_advance(timeq, delay - timeq->delay, qhead_ptr);
         RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_start, error);
         return error;

      } else if ( delay < delta ) {       /* shorter delay requested */
         TCP_Timer_stop(timeq, qhead_ptr);

      } else {                             /* No change, so return */
         RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_start, RTCS_OK);
         return RTCS_OK;

      } /* Endif */

   } else {
      TCP_Timer_remove(timeq);

   } /* Endif */

   timeq->reload = reload;
   timeq->count = 0;
   if ( delay <= 0 ) {

      delay = reload;
      if ( delay == 0 ) {      /* zero delay, put timeq in donelist */
         TCP_Timer_expire(timeq);
         RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_start, RTCSERR_TCP_TIMED_OUT);
         return RTCSERR_TCP_TIMED_OUT;
      } /* Endif */

   } /* Endif */

   timeq->delay = delay;
   timeq->abs_to = newtime + delay;
   TCP_Timer_schedule(timeq, qhead_ptr);

   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_start, RTCS_OK);
   return RTCS_OK;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Timer_advance
* Returned Values : Positive error number.
* Comments        :
*
*  Reschedule timer 'timeq' delta milliseconds into the future;
*   this is implemented mainly for TCP, since its retransmit timers must
*   be rescheduled each time new data is ACKed, and rescheduling by delta
*   should be faster than stopping and restarting the timer.
*
*  Assumes delta >= 0 !
*
*  This may also be called by (almost, see above) any task...
*
*END*-----------------------------------------------------------------*/

int_32 TCP_Timer_advance
   (
      TimeQ _PTR_       timeq,      /* IN/OUT - timer to reschedule */
      int_32            delta,      /* IN     - no. msec to increment timeout */
      TimeQ _PTR_ _PTR_ qhead_ptr   /* IN/OUT - timer queue head */
   )
{ /* Body */
   TimeQ _PTR_ next,
         _PTR_ prev;
   int_32 error;

   RTCSLOG_FNE4(RTCSLOG_FN_TCP_Timer_advance, timeq, delta, qhead_ptr);
   
   /*
   ** If timer has already expired, simply restart it with delay 'delta'
   */
   if ( timeq->curlist != qhead_ptr ) {
      error = TCP_Timer_start(timeq, delta, 0, qhead_ptr);
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_advance, error);
      return error;
   } /* Endif */

   /*
   ** Simple cases...
   */
   next = timeq->next;
   if ( next == NULL ) {             /* We are last timer in list */
      timeq->delay += delta;
      timeq->abs_to += delta;
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_advance, RTCS_OK);
      return RTCS_OK;
   } /* Endif */

   if ( delta <= next->delay ) {     /* New t/o is still less than the next */
      next->delay -= delta;
      timeq->delay += delta;
      timeq->abs_to += delta;
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_advance, RTCS_OK);
      return RTCS_OK;
   } /* Endif */

   /*
   ** Remove timer from its position in the queue, so we can advance it...
   */
   delta -= next->delay;
   next->delay += timeq->delay;
   next->prev = timeq->prev;
   if ( timeq->prev != NULL ) {
      timeq->prev->next = next;
   } else {
      *qhead_ptr = next;
   } /* Endif */

   /*
   ** Advance to proper insertion point into 'schedule' queue
   */
   while (1) {

      prev = next;
      next = next->next;
      if ( next == NULL ) {
         goto nonext;
      } /* Endif */

      if ( delta < next->delay ) {
         break;
      } /* Endif */
      delta -= next->delay;

   } /* Endwhile */

   /*
   ** If loop breaks here, next != 0
   */
   next->prev = timeq;
   next->delay -= delta;      /* adjust next delta */

   nonext:
      /*
      ** Reinsert timer in queue
      */
      timeq->delay = delta;
      timeq->abs_to = prev->abs_to + delta;
      timeq->next = next;
      timeq->prev = prev;
      prev->next = timeq;

      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_advance, RTCS_OK);
      return RTCS_OK;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Timer_oneshot_max
* Returned Values : Positive error number.
* Comments        :
*
*  Make timer `timeq' expire in at most `delay' msecs:
*
*   if already active at more than `delay' ms, reschedule to `delay' ms;
*
*   if already active at less than `delay', leave as is;
*
*   if expired (idle) but is in its donelist, i.e. if
*    (timeq->curlist EQ timeq->donelist AND timeq->curlist NE NULL),
*    then leave timer as is (usually it was not "processed"...);
*
*   if expired (idle) and not in its donelist, then simply start the timer
*    for `delay' msecs.
*
*  In the special case where `delay' is zero or negative, and the timer is
*   active, the timer is stopped and removed from its donelist (and remains
*   "read"), and OK is returned.
*
*  Before calling TCP_Timer_oneshot_max() for the first time, the timer should be
*   cleared to all zeroes (and its donelist possibly set; if it is, the timer
*   is automatically inserted into the donelist when it expires, and removed
*   from it (if present) when restarted).
*
*  This function is used, for example, in TCP's delayed ack timer
*
*  Return values:
*   0:                  timer started or was already in progress
*   RTCSERR_TCP_TIMED_OUT:  timer expired and in its donelist; not restarted.
*
*END*-----------------------------------------------------------------*/

int_32 TCP_Timer_oneshot_max
   (
      TimeQ _PTR_       timeq,      /* IN/OUT - timer to start */
      int_32            delay,      /* IN     - timeout */
      TimeQ _PTR_ _PTR_ qhead_ptr   /* IN/OUT - timer queue head */
   )
{ /* Body */
   int_32 error;
    
   RTCSLOG_FNE4(RTCSLOG_FN_TCP_Timer_oneshot_max, timeq, delay, qhead_ptr);

   if ( timeq->curlist != NULL ) {            /* may be active */

      if ( timeq->curlist == timeq->donelist ) { /* in its donelist */
         RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_oneshot_max, RTCSERR_TCP_TIMED_OUT);
         return RTCSERR_TCP_TIMED_OUT;
      } /* Endif */

      if ( timeq->curlist == qhead_ptr ) {    /* active... */
         if ( timeq->abs_to <= (RTCS_time_get()+delay) ) {
            /* Will expire earlier, so leave as is */
            RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_oneshot_max, RTCS_OK);
            return RTCS_OK;
         } /* Endif */
         TCP_Timer_stop(timeq, qhead_ptr);        /* too far, stop bfr rstrting */

      } else {
         TCP_Timer_remove(timeq);
      } /* Endif */

   } /* Endif */
   
   error = TCP_Timer_start(timeq, delay, 0, qhead_ptr);
   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Timer_oneshot_max, error);
   return error;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Tick_server
* Returned Value  : time delay until next event
* Comments        :
*
*  This was the Tick Server task, now just a function.
*
*END*-----------------------------------------------------------------*/

uint_32 TCP_Tick_server
   (
      void
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;
   TimeQ _PTR_ _PTR_    qhead_ptr;
   register int_32      reload;
   uint_32              elapsed;
   TimeQ _PTR_          expired;
   boolean              reply;
   uint_32              newtime;

   RTCSLOG_FNE1(RTCSLOG_FN_TCP_Tick_server);
       
   tcp_cfg = RTCS_getcfg(TCP);
   qhead_ptr = tcp_cfg->qhead;
   reply = FALSE; /* assume no timeout */

   newtime = RTCS_time_get();
   elapsed = RTCS_timer_get_interval(tcp_cfg->lasttime, newtime); /* get elapsed msecs... */
   tcp_cfg->lasttime = newtime;

   /*  Update schedule queue
    */
   while ( *qhead_ptr != NULL ) {

      if (elapsed < (*qhead_ptr)->delay) {
         (*qhead_ptr)->delay -= elapsed;
         break;
      } else {
         reply = TRUE;  /* At least *qhead has expired */

      } /* Endif */
      elapsed -= (*qhead_ptr)->delay;

      /*  Timeout/delay expired - process and remove
       *   (reinsert afterwards if cyclic)
       *
       *  Here we assume we are running at higher priority than any
       *   task using timers, so we don't disable interrupts while
       *   processing expiration.
       */

      /* remove from front of queue... */
      expired = *qhead_ptr;
      *qhead_ptr = (*qhead_ptr)->next;
      if ( *qhead_ptr != NULL ) {
         (*qhead_ptr)->prev = NULL;
      } /* Endif */

      if ( expired->prev != NULL ) {
         RTCS_log_error( ERROR_TCPIP,
                              RTCSERR_TCPIP_TIMER_CORRUPT,
                              (uint_32)expired,
                              3,
                              (uint_32)expired->prev );
         expired->prev = NULL;
      } /* Endif */
      expired->next = NULL;
      expired->curlist = NULL;
      reload = expired->reload;

      if ( reload < 0 ) {           /* delay expired */

         RTCS_log_error( ERROR_TCPIP,
                              RTCSERR_TCPIP_DELAY_REQUESTED,
                              1,
                              0, 0 );


      } else if ( reload > 0 ) {       /* cyclic timeout */

         expired->count++;          /* count timeouts */

         /* Signal(expired->client); */

         expired->delay = reload;   /* reschedule... */
         expired->abs_to = newtime + reload;
         TCP_Timer_schedule(expired, qhead_ptr);

      } else {
         TCP_Timer_expire(expired);      /* oneshot */

      } /* Endif */

   } /* Endwhile */

   if (reply) {
      TCP_Process_signal();
   } /* Endif */

   if (!*qhead_ptr) {
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Tick_server, 0);
      return 0;
   } else if ((signed)((*qhead_ptr)->abs_to - newtime) <= 0) {
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Tick_server, 1);
      return 1;
   } else {
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Tick_server, (*qhead_ptr)->abs_to - newtime);
      return (*qhead_ptr)->abs_to - newtime;
   } /* Endif */

} /* Endbody */

/* EOF */
