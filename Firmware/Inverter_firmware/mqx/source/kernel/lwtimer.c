/*HEADER********************************************************************
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
 * $FileName: lwtimer.c$
 * $Version : 3.8.5.1$
 * $Date    : Feb-20-2012$
 *
 * Comments:
 *
 *   This file contains functions of the LWTimer component.
 *
 *END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_LWTIMER
#include "lwtimer.h"
#include "lwtimer_prv.h"

/*!
 * \brief Creates the periodic timer queue.
 *
 * \param[in] period_ptr The location of the data structure to be initialized.
 * \param[in] period     The cycle length of this timer in ticks.
 * \param[in] wait_ticks The number of ticks to wait before starting this queue.
 *
 * \return MQX_OK
 *
 * \see _lwtimer_add_timer_to_queue
 * \see _lwtimer_cancel_period
 * \see _lwtimer_cancel_timer
 * \see _lwtimer_create_periodic_queue
 * \see LWTIMER_PERIOD_STRUCT
 */
_mqx_uint _lwtimer_create_periodic_queue
(
    LWTIMER_PERIOD_STRUCT_PTR period_ptr,
    _mqx_uint                 period,
    _mqx_uint                 wait_ticks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_lwtimer_create_periodic_queue, period_ptr, period, wait_ticks);

    period_ptr->PERIOD = period;
    period_ptr->EXPIRY = 0;
    period_ptr->WAIT   = wait_ticks;
    _QUEUE_INIT(&period_ptr->TIMERS,0);
    period_ptr->TIMER_PTR = (pointer) &period_ptr->TIMERS;

    _int_disable();
    if (kernel_data->LWTIMERS.NEXT == NULL)
    {
        /* Initialize the light weight timer queue */
        _QUEUE_INIT(&kernel_data->LWTIMERS, 0);
        kernel_data->LWTIMER_ISR = _lwtimer_isr_internal;
    } /* Endif */
    period_ptr->VALID = LWTIMER_VALID;
    _QUEUE_ENQUEUE(&kernel_data->LWTIMERS, &period_ptr->LINK);
    _int_enable();

    _KLOGX2(KLOG_lwtimer_create_periodic_queue, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Adds the lightweight timer to the periodic queue.
 *
 * This function inserts the timer in the queue in order of increasing offset
 * from the queue's start time.
 *
 * \param[in] period_ptr Pointer to the periodic queue.
 * \param[in] timer_ptr  Pointer to the lightweight timer to add to the queue,
 * must be smaller than queue.
 * \param[in] ticks      Tick offset from the timers period to expire at.
 * \param[in] func       Function to call when the timer expires.
 * \param[in] parameter  Parameter to pass to the function.
 *
 * \return MQX_OK
 * \return MQX_LWTIMER_INVALID (Period_ptr points to an invalid periodic queue.)
 * \return MQX_INVALID_PARAMETER (Ticks is greater than or equal to the
 * periodic queue's period.)
 *
 * \see _lwtimer_cancel_period
 * \see _lwtimer_cancel_timer
 * \see _lwtimer_create_periodic_queue
 * \see LWTIMER_PERIOD_STRUCT
 * \see LWTIMER_STRUCT
 */
 _mqx_uint _lwtimer_add_timer_to_queue
(
    LWTIMER_PERIOD_STRUCT_PTR period_ptr,
    LWTIMER_STRUCT_PTR        timer_ptr,
    _mqx_uint                 ticks,
    LWTIMER_ISR_FPTR          func,
    pointer                   parameter
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWTIMER_STRUCT_PTR     qe_ptr;
    _mqx_uint              i;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_lwtimer_add_timer_to_queue, period_ptr, timer_ptr, ticks);

#if MQX_CHECK_ERRORS
    if (period_ptr->VALID != LWTIMER_VALID)
    {
        _KLOGX2(KLOG_lwtimer_add_timer_to_queue, MQX_LWTIMER_INVALID);
        return (MQX_LWTIMER_INVALID);
    } /* Endif */
    if (ticks >= period_ptr->PERIOD)
    {
        _KLOGX2(KLOG_lwtimer_add_timer_to_queue, MQX_INVALID_PARAMETER);
        return (MQX_INVALID_PARAMETER);
    } /* Endif */
#endif

    timer_ptr->TIMER_FUNCTION = func;
    timer_ptr->PARAMETER      = parameter;
    timer_ptr->PERIOD_PTR     = period_ptr;
    timer_ptr->RELATIVE_TICKS = ticks;
    _int_disable();
    /* Insert into queue in order of increasing offset from start time */
    qe_ptr = (pointer) &period_ptr->TIMERS.NEXT;
    i = _QUEUE_GET_SIZE(&period_ptr->TIMERS) + 1;
    while (--i)
    {
        qe_ptr = (pointer) qe_ptr->LINK.NEXT;
        if (qe_ptr->RELATIVE_TICKS >= ticks)
        {
            qe_ptr = (pointer) qe_ptr->LINK.PREV;
            break;
        } /* Endif */
    } /* Endwhile */
    timer_ptr->VALID = LWTIMER_VALID;
    _QUEUE_INSERT(&period_ptr->TIMERS, qe_ptr, &timer_ptr->LINK);
    _int_enable();

    _KLOGX2(KLOG_lwtimer_add_timer_to_queue, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \private
 *
 * \brief This function checks the lwtimer queues, and if a timer is ready,
 *  calls an application function.
 */
void _lwtimer_isr_internal(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    LWTIMER_PERIOD_STRUCT_PTR period_ptr;
    LWTIMER_STRUCT_PTR        timer_ptr;

    _GET_KERNEL_DATA(kernel_data);

    period_ptr = (pointer) kernel_data->LWTIMERS.NEXT;
    while ((pointer) period_ptr != (pointer) &kernel_data->LWTIMERS)
    {
        if (period_ptr->WAIT)
        {
            --period_ptr->WAIT;
        }
        else
        {
            timer_ptr = (pointer) period_ptr->TIMER_PTR->LINK.NEXT;
            for (; timer_ptr != (pointer) &period_ptr->TIMERS; timer_ptr = (LWTIMER_STRUCT_PTR) (pointer) timer_ptr->LINK.NEXT)
            {
                if (timer_ptr->RELATIVE_TICKS > period_ptr->EXPIRY)
                {
                    break;
                } /* Endif */
                /* Save the last timer processed */
                period_ptr->TIMER_PTR = timer_ptr;
                (*timer_ptr->TIMER_FUNCTION)(timer_ptr->PARAMETER);
            } /* Endwhile */
            if (++period_ptr->EXPIRY == period_ptr->PERIOD)
            {
                period_ptr->EXPIRY = 0;
                period_ptr->TIMER_PTR = (pointer) &period_ptr->TIMERS;
            } /* Endif */
        } /* Endif */
        period_ptr = (LWTIMER_PERIOD_STRUCT_PTR) (pointer) period_ptr->LINK.NEXT;
    } /* Endwhile */

} /* Endbody */

/*!
 * \brief Cancels an outstanding timer request.
 *
 * \param[in] timer_ptr Pointer to the lightweight timer to cancel.
 *
 * \return MQX_OK
 * \return MQX_LWTIMER_INVALID (Timer_ptr points to either an invalid timer or
 * to a timer with a periodic queue.)
 *
 * \see _lwtimer_add_timer_to_queue
 * \see _lwtimer_cancel_period
 * \see _lwtimer_create_periodic_queue
 * \see LWTIMER_STRUCT
 */
_mqx_uint _lwtimer_cancel_timer
(
    LWTIMER_STRUCT_PTR timer_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    LWTIMER_PERIOD_STRUCT_PTR period_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwtimer_cancel_timer, timer_ptr);

#if MQX_CHECK_VALIDITY
    if (timer_ptr->VALID != LWTIMER_VALID)
    {
        _KLOGX2(KLOG_lwtimer_cancel_timer, MQX_LWTIMER_INVALID);
        return MQX_LWTIMER_INVALID;
    } /* Endif */
#endif

    period_ptr = timer_ptr->PERIOD_PTR;
    _int_disable();
#if MQX_CHECK_VALIDITY
    if (period_ptr->VALID != LWTIMER_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_lwtimer_cancel_timer, MQX_LWTIMER_INVALID);
        return MQX_LWTIMER_INVALID;
    } /* Endif */
#endif
    timer_ptr->VALID = 0;
    if (timer_ptr == period_ptr->TIMER_PTR)
    {
        period_ptr->TIMER_PTR = (pointer) timer_ptr->LINK.PREV;
    } /* Endif */
    _QUEUE_REMOVE(&period_ptr->TIMERS, timer_ptr);
    _int_enable();

    _KLOGX2(KLOG_lwtimer_cancel_timer, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Cancels all the lightweight timers in the periodic queue.
 *
 * \param[in] period_ptr Pointer to the periodic queue to cancel.
 *
 * \return MQX_OK
 * \return MQX_LWTIMER_INVALID (Period_ptr points to an invalid periodic queue.)
 *
 * \see _lwtimer_add_timer_to_queue
 * \see _lwtimer_cancel_timer
 * \see _lwtimer_create_periodic_queue
 * \see LWTIMER_PERIOD_STRUCT
 */
_mqx_uint _lwtimer_cancel_period
(
    LWTIMER_PERIOD_STRUCT_PTR period_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwtimer_cancel_period, period_ptr);

#if MQX_CHECK_VALIDITY
    if (period_ptr->VALID != LWTIMER_VALID)
    {
        _KLOGX2(KLOG_lwtimer_cancel_period, MQX_LWTIMER_INVALID);
        return MQX_LWTIMER_INVALID;
    } /* Endif */
#endif

    _int_disable();
    period_ptr->VALID = 0;
    _QUEUE_REMOVE(&kernel_data->LWTIMERS, period_ptr);
    _int_enable();

    _KLOGX2(KLOG_lwtimer_cancel_period, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Tests all the periodic queues and their lightweight timers for
 * validity and consistency.
 *
 * \param[out] period_error_ptr Pointer to the first periodic queue that has
 * an error (NULL if no error is found).
 * \param[out] timer_error_ptr  Pointer to the first timer that has an error
 * (NULL if no error is found).
 *
 * \return MQX_OK (No periodic queues have been created or no errors found
 * in any periodic queues or timers.)
 * \return MQX_LWTIMER_INVALID (Period_ptr points to an invalid periodic queue.)
 * \return Error from _queue_test() (A periodic queue or its queue was in error.)
 *
 * \see _lwtimer_add_timer_to_queue
 * \see _lwtimer_cancel_period
 * \see _lwtimer_cancel_timer
 * \see _lwtimer_create_periodic_queue
 */
 _mqx_uint _lwtimer_test
(
    pointer _PTR_ period_error_ptr,
    pointer _PTR_ timer_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    LWTIMER_STRUCT_PTR        timer_ptr;
    LWTIMER_PERIOD_STRUCT_PTR period_ptr;
    _mqx_uint                 result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_lwtimer_test, period_error_ptr, timer_error_ptr);

    *period_error_ptr = NULL;
    *timer_error_ptr = NULL;

    /*
     * It is not considered an error if the lwtimer component has not been
     * created yet
     */
    if (kernel_data->LWTIMERS.NEXT == NULL)
    {
        return (MQX_OK);
    } /* Endif */

    result = _queue_test(&kernel_data->LWTIMERS, period_error_ptr);
    if (result != MQX_OK)
    {
        _KLOGX3(KLOG_lwtimer_test, result, *period_error_ptr);
        return (result);
    } /* Endif */

    _int_disable();
    period_ptr = (pointer) kernel_data->LWTIMERS.NEXT;
    while ((pointer) period_ptr != (pointer) &kernel_data->LWTIMERS)
    {
        if (period_ptr->VALID != LWTIMER_VALID)
        {
            _int_enable();
            *period_error_ptr = period_ptr;
            _KLOGX3(KLOG_lwtimer_test, MQX_LWTIMER_INVALID, period_ptr);
            return (MQX_LWTIMER_INVALID);
        } /* Endif */
        result = _queue_test(&period_ptr->TIMERS, timer_error_ptr);
        if (result != MQX_OK)
        {
            _int_enable();
            *period_error_ptr = period_ptr;
            _KLOGX4(KLOG_lwtimer_test, result, *period_error_ptr, *timer_error_ptr);
            return (result);
        } /* Endif */
        timer_ptr = (pointer) period_ptr->TIMERS.NEXT;
        while (timer_ptr != (pointer) &period_ptr->TIMERS)
        {
            if (timer_ptr->VALID != LWTIMER_VALID)
            {
                *period_error_ptr = period_ptr;
                *timer_error_ptr = timer_ptr;
                _KLOGX4(KLOG_lwtimer_test, MQX_LWTIMER_INVALID, period_ptr,
                                timer_ptr);
                return (MQX_LWTIMER_INVALID);
            } /* Endif */
            timer_ptr = (pointer) timer_ptr->LINK.NEXT;
        } /* Endwhile */
        period_ptr = (pointer) period_ptr->LINK.NEXT;
    } /* Endwhile */

    _int_enable();
    _KLOGX2(KLOG_lwtimer_test, MQX_OK);
    return (MQX_OK);

} /* Endbody */

#endif /* MQX_USE_LWTIMER */

/* EOF */
