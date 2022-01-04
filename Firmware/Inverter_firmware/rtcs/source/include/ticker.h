#ifndef __ticker_h__
#define __ticker_h__
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
* $FileName: ticker.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   Timer interface definitions for the TCP layer.
*
*END************************************************************************/


/************************************************************************/
/* Constants                                                            */
/************************************************************************/

#if 0
#ifdef _MSDOS_
#define TICK_LENGTH   55   /* in milliseconds */
#else
#define TICK_LENGTH   5    /* in milliseconds */
#endif
#endif
#define TICK_LENGTH _time_get_resolution()


/************************************************************************/
/* Macros                                                               */
/************************************************************************/

#define TimerInit(timeq_ptr)  (_Zeromem((void _PTR_)(timeq_ptr), sizeof(TimeQ)))

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/*  The following structure is used for delays (where client blocks),
 *   oneshot timers, and cyclic timers, i.e. all items scheduled by
 *   the TickerServer.
 */
typedef struct TimeQ {

    struct TimeQ  _PTR_ next;
    struct TimeQ  _PTR_ prev;

    struct TimeQ  _PTR_ _PTR_ donelist;
                              /* where to put completed oneshots */

    struct TimeQ  _PTR_ _PTR_ curlist;
                              /* the list which currently holds the
                              ** timer (usually &qhead or donelist),
                              ** 0 if none */

    uint_16       RESERVED;
    uint_16       count;      /* nb of timeouts (ignored for delay) */
    int_32        delay;      /* time delta before this node done */
    uint_32       abs_to;     /* time at which this node should timeout */
    int_32        reload;     /* <0==delay, 0==oneshot, >0==cyclic
                              ** timer's repeated delay */

/*  task_id       client; */  /* task to Signal() (timers) or
                                 Reply() to (delays); cleared when
                                 oneshot or delay has expired;
                                 if set, this means this timer is
                                 active, and in the schedule list
                                 (`qhead', see ticker.c) */

} TimeQ;

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

void TCP_Timer_schedule
(
   TimeQ _PTR_       ,        /* IN/OUT - The timer to schedule */
   TimeQ _PTR_ _PTR_          /* IN/OUT - The head of the timer queue */
);

void  TCP_Timer_expire
(
   TimeQ _PTR_                /* IN/OUT - The expired timer */
);

void  TCP_Timer_remove
(
   TimeQ _PTR_                /* IN/OUT - The timer to remove */
);

int_32 TCP_Timer_stop
(
   TimeQ _PTR_       ,        /* The timer to stop */
   TimeQ _PTR_ _PTR_          /* The timer queue head */
);

int_32 TCP_Timer_start
(
   TimeQ _PTR_       ,        /* IN/OUT - The timer */
   int_32            ,        /* IN     - No. msec until timeout */
   int_32            ,        /* IN     - No. msec to dly after 1st to */
   TimeQ _PTR_ _PTR_          /* IN/OUT - Head of timer queue */
);

int_32 TCP_Timer_advance
(
   TimeQ _PTR_       ,        /* IN/OUT - timer to reschedule */
   int_32            ,        /* IN     - no. msec to increment timeout */
   TimeQ _PTR_ _PTR_          /* IN/OUT - timer queue head */
);

int_32 TCP_Timer_oneshot_max
(
   TimeQ _PTR_       ,        /* IN/OUT - timer to start */
   int_32            ,        /* IN     - timeout */
   TimeQ _PTR_ _PTR_          /* IN/OUT - timer queue head */
);

uint_32 TCP_Tick_server
(
   void
);

#endif
/* EOF */
