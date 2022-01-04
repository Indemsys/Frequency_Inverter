/*HEADER*********************************************************************
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
* $FileName: lwtimer.h$
* $Version : 3.8.5.1$
* $Date    : Feb-20-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   light weight timer component.
*
*END************************************************************************/

#ifndef __lwtimer_h__
#define __lwtimer_h__ 1

#include <mqx_cnfg.h>

#if (! MQX_USE_LWTIMER) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error LWTIMER component is currently disabled in MQX kernel. Please set MQX_USE_LWTIMER to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                     DATA STRUCTURE DEFINITIONS                           */

/*!
 * \brief Lightweight timer.
 *
 * This structure defines a light weight timer.
 * These timers implement a system where the specified function will be called 
 * at a periodic interval.
 * 
 * \see _lwtimer_add_timer_to_queue
 * \see _lwtimer_cancel_timer   
 * \see LWTIMER_PERIOD_STRUCT  
 */
typedef struct lwtimer_struct
{

   /*! \brief Queue data structures. */
   QUEUE_ELEMENT_STRUCT       LINK;

   /*! \brief The relative number of ticks until this timer is to expire. */
   _mqx_uint                  RELATIVE_TICKS;

   /*! \brief When the timer is added to the timer queue, MQX initializes the field.
    * When the timer or the timer queue that the timer is in is cancelled, MQX 
    * clears the field.
    */
   _mqx_uint                  VALID;

   /*! \brief Function that is called when the timer expires. */
   LWTIMER_ISR_FPTR       TIMER_FUNCTION;

   /*! \brief Parameter that is passed to the timer function. */
   pointer                    PARAMETER;

   /*! \brief Pointer to the lightweight timer queue to which the timer is attatched. */
   pointer                    PERIOD_PTR;

} LWTIMER_STRUCT, _PTR_ LWTIMER_STRUCT_PTR;


/*!
 * \brief Lightweight timer queue.
 *
 * This structure controls any number of lightweight timers wishing to be executed
 * at the periodic rate defined by this structure.
 * The periodic rate will be a multiple of the BSP_ALARM_RESOLUTION.
 * 
 * \see _lwtimer_add_timer_to_queue
 * \see _lwtimer_cancel_period
 * \see _lwtimer_create_periodic_queue   
 * \see LWTIMER_STRUCT  
 */
typedef struct lwtimer_period_struct
{
   /*! \brief Queue of lightweight timers. */
   QUEUE_ELEMENT_STRUCT       LINK;

   /*! \brief The period of this group of timers (in ticks), a multiple of
    * BSP_ALARM_RESOLUTION. */
   _mqx_uint                  PERIOD;

   /*! \brief Number of ticks that have elapsed in this period. */
   _mqx_uint                  EXPIRY;

   /*! 
    * \brief Number of ticks to wait before starting to process this queue.*/
   _mqx_uint                  WAIT;

   /*! \brief A queue of timers to expire at this periodic rate. */
   QUEUE_STRUCT               TIMERS;

   /*! \brief Pointer to the last timer on the queue that was processed */
   LWTIMER_STRUCT_PTR         TIMER_PTR;

   /*! \brief When the timer queue is created, MQX initializes the field. When
    * the queue is cancelled, MQX clears the field. */
   _mqx_uint                  VALID;

} LWTIMER_PERIOD_STRUCT, _PTR_ LWTIMER_PERIOD_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*                       EXTERNAL DECLARATIONS                              */

#ifdef __cplusplus
extern "C" {
#endif
#ifndef __TAD_COMPILE__

extern _mqx_uint _lwtimer_add_timer_to_queue(LWTIMER_PERIOD_STRUCT_PTR,
   LWTIMER_STRUCT_PTR, _mqx_uint, LWTIMER_ISR_FPTR, pointer);
extern _mqx_uint _lwtimer_cancel_period(LWTIMER_PERIOD_STRUCT_PTR);
extern _mqx_uint _lwtimer_cancel_timer(LWTIMER_STRUCT_PTR);
extern _mqx_uint _lwtimer_create_periodic_queue(LWTIMER_PERIOD_STRUCT_PTR,
   _mqx_uint, _mqx_uint);
extern _mqx_uint _lwtimer_test(pointer _PTR_, pointer _PTR_);

#endif
#ifdef __cplusplus
}
#endif

#endif  /* __lwtimer_h__ */
/* EOF */
