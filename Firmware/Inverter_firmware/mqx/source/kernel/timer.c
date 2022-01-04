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
 * $FileName: timer.c$
 * $Version : 3.8.2.0$
 * $Date    : Jun-25-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Timer component.
 *
 *END************************************************************************/

#include "mqx_inc.h"
#if MQX_USE_TIMER
#include "timer.h"
#include "timer_prv.h"

/*!
 * \brief Cancels an outstanding timer request.
 * 
 * \param[in] id ID of the timer to be cancelled, from calling a function from 
 * the _timer_start family of functions.
 *  
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Timer component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \return MQX_INVALID_PARAMETER (Id is not valid.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM
 * 
 * \warning Cannot be called from an ISR. 
 * 
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 */ 
_mqx_uint _timer_cancel
(
    _timer_id id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TIMER_ENTRY_STRUCT_PTR     timer_entry_ptr;
    QUEUE_STRUCT_PTR           queue_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_timer_cancel, id);

    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
#if MQX_CHECK_ERRORS
    if (timer_component_ptr == NULL)
    {
        _KLOGX2(KLOG_timer_cancel, MQX_COMPONENT_DOES_NOT_EXIST);
        return MQX_COMPONENT_DOES_NOT_EXIST;
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (timer_component_ptr->VALID != TIMER_VALID)
    {
        _KLOGX2(KLOG_timer_cancel, MQX_INVALID_COMPONENT_BASE);
        return MQX_INVALID_COMPONENT_BASE;
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    if (id == 0)
    {
        _KLOGX2(KLOG_timer_cancel, MQX_INVALID_PARAMETER);
        return MQX_INVALID_PARAMETER;
    } /* Endif */

    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_timer_cancel, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return MQX_CANNOT_CALL_FUNCTION_FROM_ISR;
    } /* Endif */
#endif

    /* Gain exclusive access to the timer queues */
    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        if (_lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM) != MQX_OK)
        {
            _KLOGX2(KLOG_timer_cancel, MQX_INVALID_LWSEM);
            return(MQX_INVALID_LWSEM);
        } /* Endif */
    } /*Endif */
    timer_entry_ptr = _timer_find_entry_internal(timer_component_ptr, id);

    if ((timer_entry_ptr == NULL) || (timer_entry_ptr->VALID != TIMER_VALID))
    {
        if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
        {
            _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
        } /* Endif */
        _KLOGX2(KLOG_timer_cancel, MQX_INVALID_PARAMETER);
        return MQX_INVALID_PARAMETER;
    } /* Endif */

    if (timer_entry_ptr->MODE == TIMER_ELAPSED_TIME_MODE)
    {
        queue_ptr = &timer_component_ptr->ELAPSED_TIMER_ENTRIES;
    }
    else
    {
        queue_ptr = &timer_component_ptr->KERNEL_TIMER_ENTRIES;
    } /* Endif */

    timer_entry_ptr->VALID = 0;
    timer_entry_ptr->ID = 0;
    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        _QUEUE_REMOVE(queue_ptr, timer_entry_ptr);
        _mem_free(timer_entry_ptr);
        _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
    } /* Endif */

    _KLOGX2(KLOG_timer_cancel, MQX_OK);
    return(MQX_OK);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Locates a timer via its ID.
 * 
 * \param[in] timer_component_ptr The address of the timer component structure.
 * \param[in] id                  The timer ID.
 * 
 * \return Pointer to a timer entry.
 * \return NULL (failure)  
 */
TIMER_ENTRY_STRUCT_PTR _timer_find_entry_internal
(
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr,
    _timer_id                  id
)
{ /* Body */
    QUEUE_STRUCT_PTR       queue_ptr;
    TIMER_ENTRY_STRUCT_PTR timer_entry_ptr;
    _mqx_uint              i;
    _mqx_uint              size;

    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            queue_ptr = &timer_component_ptr->ELAPSED_TIMER_ENTRIES;
        }
        else
        {
            queue_ptr = &timer_component_ptr->KERNEL_TIMER_ENTRIES;
        } /* Endif */
        timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)((pointer)queue_ptr->NEXT);
        /* SPR P171-0023-01 use pre-decrement on while loop */
        size = queue_ptr->SIZE + 1;
        while (--size)
        {
            /* END SPR */
            if (timer_entry_ptr->ID == id)
            {
                return(timer_entry_ptr);
            } /* Endif */
            timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)
            ((pointer)timer_entry_ptr->QUEUE_ELEMENT.NEXT);
        } /* Endwhile */
    } /* Endfor */

    return(NULL);

} /* Endbody */

/*!
 * \brief Istalls the timer component into the kernel.
 * 
 * If the timer component is not explicitly created, MQX creates it with default 
 * values the first time that a task calls one of the functions from the 
 * _timer_start family.
 * \n The default values are:
 * \li TIMER_DEFAULT_TASK_PRIORITY
 * \li TIMER_DEFAULT_STACK_SIZE
 * 
 * \n The function returns MQX_OK if either:
 * \li Timer component is created.
 * \li Timer component was previously created and the configuration is not changed.
 *  
 * \param[in] timer_task_priority   The task priority of the timer task.
 * \param[in] timer_task_stack_size The stack size for the timer task.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \return task error codes  
 * 
 * \warning Creates Timer Task.
 * \warning Cannot be called from an ISR. 
 * 
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _timer_cancel   
 */ 
_mqx_uint _timer_create_component
(
    _mqx_uint timer_task_priority,
    _mqx_uint timer_task_stack_size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TASK_TEMPLATE_STRUCT       timer_tt;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_timer_create_component, timer_task_priority);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_timer_create_component, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);

    if (kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER] != NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
        _KLOGX2(KLOG_timer_create_component, MQX_OK);
        return(MQX_OK);
    } /* Endif */

    /* Get the timer component data structure */
    timer_component_ptr = _mem_alloc_system_zero((_mem_size)sizeof(TIMER_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (timer_component_ptr == NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
        _KLOGX2(KLOG_timer_create_component, MQX_OUT_OF_MEMORY);
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif   
    _mem_set_type(timer_component_ptr, MEM_TYPE_TIMER_COMPONENT);
    _QUEUE_INIT(&timer_component_ptr->ELAPSED_TIMER_ENTRIES, 0);
    _QUEUE_INIT(&timer_component_ptr->KERNEL_TIMER_ENTRIES, 0);

    timer_tt.TASK_TEMPLATE_INDEX = 0;
    timer_tt.TASK_ADDRESS = _timer_task;
    if (timer_task_stack_size == 0)
    {
        timer_tt.TASK_STACKSIZE = TIMER_DEFAULT_STACK_SIZE;
    }
    else
    {
        timer_tt.TASK_STACKSIZE = timer_task_stack_size;
    } /* Endif */
    if (timer_task_priority == 0)
    {
        timer_tt.TASK_PRIORITY = TIMER_DEFAULT_TASK_PRIORITY;
    }
    else
    {
        timer_tt.TASK_PRIORITY = timer_task_priority;
    } /* Endif */
    timer_tt.TASK_NAME = "Timer Task";
    timer_tt.TASK_ATTRIBUTES = 0;
    timer_tt.DEFAULT_TIME_SLICE = 0;

    kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER] = timer_component_ptr;

    timer_component_ptr->TIMER_TID = _task_create(0, 0, (uint_32)&timer_tt);
#if MQX_CHECK_ERRORS
    if (timer_component_ptr->TIMER_TID == MQX_NULL_TASK_ID)
    {
        _mqx_uint result;

        kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER] = NULL;
        timer_component_ptr->VALID = 0;
        result = _task_get_error();
        if (result == MQX_OK)
        {
            result = MQX_OUT_OF_MEMORY;
        } /* Endif */
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
        _mem_free(timer_component_ptr);
        _KLOGX2(KLOG_timer_create_component, result);
        return(result);
    }/* Endif */
#endif

#if MQX_COMPONENT_DESTRUCTION
    kernel_data->COMPONENT_CLEANUP[KERNEL_EVENTS] = _timer_cleanup;
#endif
    _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);

    _KLOGX2(KLOG_timer_create_component, MQX_OK);
    return(MQX_OK);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Initializes a timer to start once after a delay period expressed in ticks.
 * 
 * \param[in] notification_function_ptr The function to call when the timer expires.
 * \param[in] notification_data_ptr     The data to pass to the function when 
 * the timer expires.
 * \param[in] mode                      Which time to use when calculating time 
 * to start:
 * \li TIMER_ElAPSED_TIME_MODE
 * \li TIMER_KERNEL_TIME_MODE
 * \param[in] tick_ptr                  The number of ticks to wait before calling 
 * the notification function, and canceling the timer.
 * \param[in] uses_ticks                Set to true if called from the tick 
 * interface functions.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component() 
 */           
_timer_id _timer_start_oneshot_after_internal
(
    TIMER_NOTIFICATION_FPTR notification_function_ptr,
    pointer                 notification_data_ptr,
    _mqx_uint               mode,
    MQX_TICK_STRUCT_PTR     tick_ptr,
    boolean                 uses_ticks
)
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TIMER_ENTRY_STRUCT_PTR     timer_entry_ptr;
    MQX_TICK_STRUCT            kernel_time;
    _mqx_uint                  result;

    _GET_KERNEL_DATA(kernel_data);

    timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
    if (timer_component_ptr == NULL)
    {
        result = _timer_create_component(TIMER_DEFAULT_TASK_PRIORITY,
                        TIMER_DEFAULT_STACK_SIZE);
        timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
#if MQX_CHECK_ERRORS
        if (timer_component_ptr == NULL)
        {
            _task_set_error(result);
            return TIMER_NULL_ID;
        } /* Endif */
#endif
    }/* Endif */

#if MQX_CHECK_VALIDITY
    if (timer_component_ptr->VALID != TIMER_VALID)
    {
        _task_set_error(MQX_INVALID_COMPONENT_BASE);
        return TIMER_NULL_ID;
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    if ((notification_function_ptr == NULL) ||
                    ! ((mode == TIMER_ELAPSED_TIME_MODE) || (mode == TIMER_KERNEL_TIME_MODE))
    )
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */

    result = (_mqx_uint)PSP_CMP_TICKS(&_mqx_zero_tick_struct, tick_ptr);
    if (result == 0)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(TIMER_ENTRY_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (timer_entry_ptr == NULL)
    {
        _task_set_error(MQX_OUT_OF_MEMORY);
        return TIMER_NULL_ID;
    } /* Endif */
#endif
    _mem_set_type(timer_entry_ptr, MEM_TYPE_TIMER_ENTRY);

    if (mode == TIMER_ELAPSED_TIME_MODE)
    {
        _time_get_elapsed_ticks(&kernel_time);
    }
    else
    {
        _time_get_ticks(&kernel_time);
    } /* Endif */

    PSP_ADD_TICKS(tick_ptr, &kernel_time, &timer_entry_ptr->EXPIRATION_TIME);

    timer_entry_ptr->NOTIFICATION_FUNCTION = notification_function_ptr;
    timer_entry_ptr->NOTIFICATION_DATA_PTR = notification_data_ptr;
    timer_entry_ptr->MODE = (uint_16)mode;
    timer_entry_ptr->TIMER_TYPE = TIMER_TYPE_ONESHOT_AFTER;
    timer_entry_ptr->TD_PTR = kernel_data->ACTIVE_PTR;
    timer_entry_ptr->USES_TICKS = uses_ticks;

    /* Gain exclusive access to the timer queues */
    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        if (_lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM) != MQX_OK)
        {
            _task_set_error(MQX_INVALID_LWSEM);
            return(TIMER_NULL_ID);
        } /* Endif */
    } /* Endif */

    timer_entry_ptr->VALID = TIMER_VALID;
    timer_entry_ptr->ID = _timer_alloc_id_internal(timer_component_ptr);

    if (mode == TIMER_ELAPSED_TIME_MODE)
    {
        _timer_insert_queue_internal(&timer_component_ptr->ELAPSED_TIMER_ENTRIES,
                        timer_entry_ptr);
    }
    else
    {
        _timer_insert_queue_internal(&timer_component_ptr->KERNEL_TIMER_ENTRIES,
                        timer_entry_ptr);
    } /* Endif */

    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
    } /* Endif */

    return(timer_entry_ptr->ID);

} /* Endbody */

/*!
 * \brief Starts a timer that expires after the specified number of ticks (in tick 
 * time).
 * 
 * The function calculates the expiry time based on tick_ptr and mode.
 * \n You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 * 
 * \param[in] notification_function_ptr Notification function that MQX calls when 
 * the timer expires.
 * \param[in] notification_data_ptr     The data to pass to the notification 
 * function when the timer expires.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] tick_ptr                  The number of ticks to wait before calling 
 * the notification function, and canceling the timer.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component() 
 * 
 * \see _timer_start_oneshot_after
 * \see _task_set_error
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see _timer_cancel
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _timer_create_component 
 */ 
_timer_id _timer_start_oneshot_after_ticks
(
    TIMER_NOTIFICATION_TICK_FPTR notification_function_ptr,
    pointer                      notification_data_ptr,
    _mqx_uint                    mode,
    MQX_TICK_STRUCT_PTR          tick_ptr
)
{
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_oneshot_after_ticks, notification_function_ptr,
                    mode, tick_ptr);

#if MQX_CHECK_ERRORS
    if (tick_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_timer_start_oneshot_after_ticks, TIMER_NULL_ID);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    result = _timer_start_oneshot_after_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, tick_ptr, TRUE);

    _KLOGX2(KLOG_timer_start_oneshot_after_ticks, result);

    return result;

} /* Endbody */

/*!
 * \private
 * 
 * \brief Initializes a timer to start once at a specified time.
 * 
 * \param[in] notification_function_ptr The function to call when the timer expires.
 * \param[in] notification_data_ptr     The data to pass to the function when 
 * the timer expires.
 * \param[in] mode                      Which time to use when calculating time 
 * to start:
 * \li TIMER_ElAPSED_TIME_MODE
 * \li TIMER_KERNEL_TIME_MODE
 * \param[in] tick_ptr                  The time at which to call the notification 
 * function, and then cancel the timer.
 * \param[in] uses_ticks                Set to true if notification function 
 * uses ticks.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component()   
 */ 
_timer_id _timer_start_oneshot_at_internal
(
    TIMER_NOTIFICATION_FPTR notification_function_ptr,
    pointer                 notification_data_ptr,
    _mqx_uint               mode,
    MQX_TICK_STRUCT_PTR     tick_ptr,
    boolean                 uses_ticks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TIMER_ENTRY_STRUCT_PTR     timer_entry_ptr;
    _mqx_uint                  error;

    _GET_KERNEL_DATA(kernel_data);

    timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
    if (timer_component_ptr == NULL)
    {
        error = _timer_create_component(TIMER_DEFAULT_TASK_PRIORITY,
                        TIMER_DEFAULT_STACK_SIZE);
        timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
#if MQX_CHECK_ERRORS
        if (timer_component_ptr == NULL)
        {
            _task_set_error(error);
            return TIMER_NULL_ID;
        } /* Endif */
#endif
    }/* Endif */

#if MQX_CHECK_VALIDITY
    if (timer_component_ptr->VALID != TIMER_VALID)
    {
        _task_set_error(MQX_INVALID_COMPONENT_BASE);
        return TIMER_NULL_ID;
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    if ((notification_function_ptr == NULL) ||
                    ! ((mode == TIMER_ELAPSED_TIME_MODE) || (mode == TIMER_KERNEL_TIME_MODE))
    )
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */

    error = (_mqx_uint)PSP_CMP_TICKS(&_mqx_zero_tick_struct, tick_ptr);
    if (error == 0)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(TIMER_ENTRY_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (timer_entry_ptr == NULL)
    {
        _task_set_error(MQX_OUT_OF_MEMORY);
        return TIMER_NULL_ID;
    } /* Endif */
#endif
    _mem_set_type(timer_entry_ptr, MEM_TYPE_TIMER_ENTRY);

    timer_entry_ptr->NOTIFICATION_FUNCTION = notification_function_ptr;
    timer_entry_ptr->NOTIFICATION_DATA_PTR = notification_data_ptr;
    timer_entry_ptr->MODE = (uint_16)mode;
    timer_entry_ptr->TIMER_TYPE = TIMER_TYPE_ONESHOT_AT;
    timer_entry_ptr->TD_PTR = kernel_data->ACTIVE_PTR;
    timer_entry_ptr->USES_TICKS = uses_ticks;

    timer_entry_ptr->EXPIRATION_TIME = *tick_ptr;

    /* Gain exclusive access to the timer queues */
    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        if (_lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM) != MQX_OK)
        {
            _task_set_error(MQX_INVALID_LWSEM);
            return(TIMER_NULL_ID);
        } /* Endif */
    } /* Endif */

    timer_entry_ptr->VALID = TIMER_VALID;

    timer_entry_ptr->ID = _timer_alloc_id_internal(timer_component_ptr);

    if (mode == TIMER_ELAPSED_TIME_MODE)
    {
        _timer_insert_queue_internal(&timer_component_ptr->ELAPSED_TIMER_ENTRIES,
                        timer_entry_ptr);
    }
    else
    {
        _timer_insert_queue_internal(&timer_component_ptr->KERNEL_TIMER_ENTRIES,
                        timer_entry_ptr);
    } /* Endif */
    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
    } /* Endif */

    return(timer_entry_ptr->ID);

} /* Endbody */

/*!
 * \brief Starts a timer that expire once at the specified time in tick time.
 * 
 * When the timer expires, MQX calls notification_function with timer_id, 
 * notification_data_ptr, and the current time.
 * \n You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 *  
 * \param[in] notification_function_ptr Pointer to the notification function that 
 * MQX calls when the timer expire.
 * \param[in] notification_data_ptr     The data to pass to the function when the 
 * timer expires.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] tick_ptr                  Pointer to the tick time at which MQX 
 * calls the notification function and cancels the timer.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component()
 * 
 * \see _timer_start_oneshot_at
 * \see _timer_cancel
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _task_set_error
 * \see _timer_create_component   
 */ 
_timer_id _timer_start_oneshot_at_ticks
(
    TIMER_NOTIFICATION_TICK_FPTR notification_function_ptr,
    pointer                      notification_data_ptr,
    _mqx_uint                    mode,
    MQX_TICK_STRUCT_PTR          tick_ptr
)
{
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_oneshot_at_ticks, notification_function_ptr, mode,
                    tick_ptr);

#if MQX_CHECK_ERRORS
    if (tick_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_timer_start_oneshot_at_ticks, TIMER_NULL_ID);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    result = _timer_start_oneshot_at_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, tick_ptr, TRUE);

    _KLOGX2(KLOG_timer_start_oneshot_at_ticks, result);

    return(result);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Initializes a timer to fire periodically at a specific time.
 * 
 * \param[in] notification_function_ptr The function to call when the timer expires.
 * \param[in] notification_data_ptr     The data to pass to the function when 
 * the timer expires.
 * \param[in] mode                      Which time to use when calculating time 
 * to start:
 * \li TIMER_ElAPSED_TIME_MODE
 * \li TIMER_KERNEL_TIME_MODE
 * \param[in] stick_ptr                 The time at which to call the notification 
 * function, and then cancel the timer.
 * \param[in] wtick_ptr                 The number of ticks to wait between calls 
 * to the notification function.
 * \param[in] uses_ticks                Set to true if notification function 
 * uses ticks.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component()   
 */ 
_timer_id _timer_start_periodic_at_internal
(
    TIMER_NOTIFICATION_FPTR notification_function_ptr,
    pointer                 notification_data_ptr,
    _mqx_uint               mode,
    MQX_TICK_STRUCT_PTR     stick_ptr,
    MQX_TICK_STRUCT_PTR     wtick_ptr,
    boolean                 uses_ticks
)
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TIMER_ENTRY_STRUCT_PTR     timer_entry_ptr;
    _mqx_uint                  result;

    _GET_KERNEL_DATA(kernel_data);

    timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
    if (timer_component_ptr == NULL)
    {
        result = _timer_create_component(TIMER_DEFAULT_TASK_PRIORITY,
                        TIMER_DEFAULT_STACK_SIZE);
        timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
#if MQX_CHECK_ERRORS
        if (timer_component_ptr == NULL)
        {
            _task_set_error(result);
            return TIMER_NULL_ID;
        } /* Endif */
#endif
    }/* Endif */

#if MQX_CHECK_VALIDITY
    if (timer_component_ptr->VALID != TIMER_VALID)
    {
        _task_set_error(MQX_INVALID_COMPONENT_BASE);
        return TIMER_NULL_ID;
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    if (!notification_function_ptr ||
                    ! ((mode == TIMER_ELAPSED_TIME_MODE) || (mode == TIMER_KERNEL_TIME_MODE))
    )
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */

    result = (_mqx_uint)PSP_CMP_TICKS(&_mqx_zero_tick_struct, wtick_ptr);
    if (result == 0)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(TIMER_ENTRY_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (timer_entry_ptr == NULL)
    {
        _task_set_error(MQX_OUT_OF_MEMORY);
        return TIMER_NULL_ID;
    } /* Endif */
#endif
    _mem_set_type(timer_entry_ptr, MEM_TYPE_TIMER_ENTRY);

    timer_entry_ptr->NOTIFICATION_FUNCTION = notification_function_ptr;
    timer_entry_ptr->NOTIFICATION_DATA_PTR = notification_data_ptr;
    timer_entry_ptr->MODE = (uint_16)mode;
    timer_entry_ptr->TIMER_TYPE = TIMER_TYPE_PERIODIC_AT;
    timer_entry_ptr->TD_PTR = kernel_data->ACTIVE_PTR;
    timer_entry_ptr->USES_TICKS = uses_ticks;
    timer_entry_ptr->EXPIRATION_TIME = *stick_ptr;
    timer_entry_ptr->CYCLE = *wtick_ptr;

    /* Gain exclusive access to the timer queues */
    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        if (_lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM) != MQX_OK)
        {
            _task_set_error(MQX_INVALID_LWSEM);
            return(TIMER_NULL_ID);
        } /* Endif */
    } /* Endif */

    timer_entry_ptr->VALID = TIMER_VALID;
    timer_entry_ptr->ID = _timer_alloc_id_internal(timer_component_ptr);

    if (mode == TIMER_ELAPSED_TIME_MODE)
    {
        _timer_insert_queue_internal(&timer_component_ptr->ELAPSED_TIMER_ENTRIES,
                        timer_entry_ptr);
    }
    else
    {
        _timer_insert_queue_internal(&timer_component_ptr->KERNEL_TIMER_ENTRIES,
                        timer_entry_ptr);
    } /* Endif */

    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
    } /* Endif */

    return(timer_entry_ptr->ID);

} /* Endbody */

/*!
 * \brief Starts a periodic timer at the specified time in tick time.
 * 
 * You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 *  
 * \param[in] notification_function_ptr Pointer to the notification function that 
 * MQX calls when the timer expire.
 * \param[in] notification_data_ptr     The data to pass to the function when the 
 * timer expires.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] stick_ptr                 Pointer to the tick time at which MQX 
 * starts calling the notification function.
 * \param[in] wtick_ptr                 Ticks (in tick time) that MQX waits 
 * between subsequent calls to the notification function.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component()
 * 
 * \see _timer_start_periodic_at
 * \see _timer_cancel
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _task_set_error
 * \see _timer_create_component   
 */
_timer_id _timer_start_periodic_at_ticks
(
    TIMER_NOTIFICATION_TICK_FPTR notification_function_ptr,
    pointer                      notification_data_ptr,
    _mqx_uint                    mode,
    MQX_TICK_STRUCT_PTR          stick_ptr,
    MQX_TICK_STRUCT_PTR          wtick_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_periodic_at_ticks, notification_function_ptr, mode,
                    stick_ptr);

#if MQX_CHECK_ERRORS
    if ((notification_function_ptr == NULL) ||
                    (stick_ptr == NULL) ||
                    (wtick_ptr == NULL))
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_timer_start_periodic_at_ticks, TIMER_NULL_ID);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    result = _timer_start_periodic_at_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, stick_ptr, wtick_ptr, TRUE);

    _KLOGX2(KLOG_timer_start_periodic_at_ticks, result);

    return(result);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Initializes a timer to fire periodically.
 * 
 * \param[in] notification_function_ptr The function to call when the timer expires.
 * \param[in] notification_data_ptr     The data to pass to the function when 
 * the timer expires.
 * \param[in] mode                      Which time to use when calculating time 
 * to start:
 * \li TIMER_ElAPSED_TIME_MODE
 * \li TIMER_KERNEL_TIME_MODE
 * \param[in] wtick_ptr                 The number of ticks to wait before calling 
 * the notification function.
 * \param[in] uses_ticks                Set to true if called from the tick 
 * interface functions.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component()   
 */ 
_timer_id _timer_start_periodic_every_internal
(
    TIMER_NOTIFICATION_FPTR notification_function_ptr,
    pointer                 notification_data_ptr,
    _mqx_uint               mode,
    MQX_TICK_STRUCT_PTR     wtick_ptr,
    boolean                 uses_ticks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TIMER_ENTRY_STRUCT_PTR     timer_entry_ptr;
    MQX_TICK_STRUCT            kernel_time;
    _mqx_uint                  result;

    _GET_KERNEL_DATA(kernel_data);

    timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
    if (timer_component_ptr == NULL)
    {
        result = _timer_create_component(TIMER_DEFAULT_TASK_PRIORITY,
                        TIMER_DEFAULT_STACK_SIZE);
        timer_component_ptr = (TIMER_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
#if MQX_CHECK_ERRORS
        if (timer_component_ptr == NULL)
        {
            _task_set_error(result);
            return TIMER_NULL_ID;
        } /* Endif */
#endif
    }/* Endif */

#if MQX_CHECK_VALIDITY
    if (timer_component_ptr->VALID != TIMER_VALID)
    {
        _task_set_error(MQX_INVALID_COMPONENT_BASE);
        return TIMER_NULL_ID;
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    if (!notification_function_ptr || !((mode == TIMER_ELAPSED_TIME_MODE) ||
                                    (mode == TIMER_KERNEL_TIME_MODE)) )
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */

    result = (_mqx_uint)PSP_CMP_TICKS(&_mqx_zero_tick_struct, wtick_ptr);
    if (result == 0)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(TIMER_ENTRY_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (timer_entry_ptr == NULL)
    {
        return TIMER_NULL_ID;
    } /* Endif */
#endif
    _mem_set_type(timer_entry_ptr, MEM_TYPE_TIMER_ENTRY);

    timer_entry_ptr->CYCLE = *wtick_ptr;

    if (mode == TIMER_ELAPSED_TIME_MODE)
    {
        _time_get_elapsed_ticks(&kernel_time);
    }
    else
    {
        _time_get_ticks(&kernel_time);
    } /* Endif */

    timer_entry_ptr->NOTIFICATION_FUNCTION = notification_function_ptr;
    timer_entry_ptr->NOTIFICATION_DATA_PTR = notification_data_ptr;
    timer_entry_ptr->MODE = (uint_16)mode;
    timer_entry_ptr->USES_TICKS = uses_ticks;
    timer_entry_ptr->TIMER_TYPE = TIMER_TYPE_PERIODIC_EVERY;

    PSP_ADD_TICKS(wtick_ptr, &kernel_time, &timer_entry_ptr->EXPIRATION_TIME);

    timer_entry_ptr->TD_PTR = kernel_data->ACTIVE_PTR;

    /* Gain exclusive access to the timer queues */
    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        if (_lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM) != MQX_OK)
        {
            _task_set_error(MQX_INVALID_LWSEM);
            return(TIMER_NULL_ID);
        } /* Endif */
    } /* Endif */

    timer_entry_ptr->VALID = TIMER_VALID;
    timer_entry_ptr->ID = _timer_alloc_id_internal(timer_component_ptr);

    if (mode == TIMER_ELAPSED_TIME_MODE)
    {
        _timer_insert_queue_internal(&timer_component_ptr->ELAPSED_TIMER_ENTRIES,
                        timer_entry_ptr);
    }
    else
    {
        _timer_insert_queue_internal(&timer_component_ptr->KERNEL_TIMER_ENTRIES,
                        timer_entry_ptr);
    } /* Endif */

    if (kernel_data->ACTIVE_PTR != timer_component_ptr->TIMER_TD_PTR)
    {
        _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
    } /* Endif */

    return(timer_entry_ptr->ID);

} /* Endbody */

/*!
 * \brief Starts a periodic timer every specified number of ticks (in tick time).
 * 
 * When the timer expires, MQX calls notification_function with timer_id, notifier 
 * data, and the current time.
 * \n You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 *  
 * \param[in] notification_function_ptr Pointer to the notification function that 
 * MQX calls when the timer expire.
 * \param[in] notification_data_ptr     Pointer to the data that MQX passes to 
 * the notification function.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] wtick_ptr                 Ticks (in tick time) that MQX waits before 
 * it first calls the notification function and between subsequent calls to the 
 * notification function.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component() 
 * 
 * \see _timer_start_periodic_every
 * \see _timer_cancel
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _task_set_error
 * \see _timer_create_component 
 */
_timer_id _timer_start_periodic_every_ticks
(
    TIMER_NOTIFICATION_TICK_FPTR notification_function_ptr,
    pointer                      notification_data_ptr,
    _mqx_uint                    mode,
    MQX_TICK_STRUCT_PTR          wtick_ptr
)
{
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_periodic_every_ticks, notification_function_ptr,
                    mode, wtick_ptr);

#if MQX_CHECK_ERRORS
    if ((notification_function_ptr == NULL) || (wtick_ptr == NULL))
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_timer_start_periodic_every_ticks, TIMER_NULL_ID);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    result = _timer_start_periodic_every_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, wtick_ptr, TRUE);

    _KLOGX2(KLOG_timer_start_periodic_every_ticks, result);
    return(result);

} /* Endbody */

/*!
 * \brief Starts a timer that expire after the specified number of milliseconds.
 * 
 * The function calculates the expiry time based on tick_ptr and mode.
 * \n You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 * 
 * \param[in] notification_function_ptr Notification function that MQX calls when 
 * the timer expires.
 * \param[in] notification_data_ptr     The data to pass to the notification 
 * function when the timer expires.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] milliseconds              Milliseconds to wait before MQX calls the 
 * notification function and cancels the timer.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component() 
 * 
 * \see _timer_start_oneshot_after_ticks
 * \see _task_set_error
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see _timer_cancel
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _timer_create_component 
 */ 
_timer_id _timer_start_oneshot_after
(
    TIMER_NOTIFICATION_TIME_FPTR notification_function_ptr,
    pointer                      notification_data_ptr,
    _mqx_uint                    mode,
    uint_32                      milliseconds
)
{
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    MQX_TICK_STRUCT ticks;
    /* TIME_STRUCT                     time; */
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_oneshot_after, notification_function_ptr, mode,
                    milliseconds);

#if MQX_CHECK_ERRORS
    if (milliseconds == 0)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_timer_start_oneshot_after, TIMER_NULL_ID);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    /* time.SECONDS      = 0;            */
    /* time.MILLISECONDS = milliseconds; */
    /*                                   */
    /* PSP_TIME_TO_TICKS(&time, &ticks); */
    PSP_MILLISECONDS_TO_TICKS_QUICK(milliseconds, &ticks);

    result = _timer_start_oneshot_after_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, (MQX_TICK_STRUCT_PTR)&ticks, FALSE);

    _KLOGX2(KLOG_timer_start_oneshot_after, result);

    return result;

} /* Endbody */

/*!
 * \brief Starts a timer that expire once at the specified time in 
 * second/millisecond time.
 * 
 * When the timer expires, MQX calls notification_function with timer_id, 
 * notification_data_ptr, and the current time.
 * \n You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 *  
 * \param[in] notification_function_ptr Pointer to the notification function that 
 * MQX calls when the timer expire.
 * \param[in] notification_data_ptr     The data to pass to the function when the 
 * timer expires.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] time_ptr                  Pointer to the normalized second/millisecond 
 * time at which MQX calls the notification function and cancels the timer.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component()
 * 
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_cancel
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _task_set_error
 * \see _timer_create_component   
 */ 
_timer_id _timer_start_oneshot_at
(
    TIMER_NOTIFICATION_TIME_FPTR notification_function_ptr,
    pointer          notification_data_ptr,
    _mqx_uint        mode,
    TIME_STRUCT_PTR  time_ptr
)
{
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    MQX_TICK_STRUCT ticks;
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_oneshot_at, notification_function_ptr, mode, time_ptr);

#if MQX_CHECK_ERRORS
    if (time_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_timer_start_oneshot_at, TIMER_NULL_ID);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    PSP_TIME_TO_TICKS(time_ptr, &ticks);

    result = _timer_start_oneshot_at_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, (MQX_TICK_STRUCT_PTR)&ticks, FALSE);

    _KLOGX2(KLOG_timer_start_oneshot_at, result);
    return(result);

} /* Endbody */

/*!
 * \brief Starts a periodic timer at the specified time in second/millisecond time.
 * 
 * You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 *  
 * \param[in] notification_function_ptr Pointer to the notification function that 
 * MQX calls when the timer expire.
 * \param[in] notification_data_ptr     The data to pass to the function when the 
 * timer expires.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] time_ptr                  Pointer to the normalized second/millisecond 
 * time at which MQX starts calling the notification function.
 * \param[in] milliseconds              Milliseconds that MQX waits between 
 * subsequent calls to the notification function.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component()
 * 
 * \see _timer_start_periodic_at_ticks
 * \see _timer_cancel
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _task_set_error
 * \see _timer_create_component   
 */
_timer_id _timer_start_periodic_at
(
    TIMER_NOTIFICATION_TIME_FPTR notification_function_ptr,
    pointer          notification_data_ptr,
    _mqx_uint        mode,
    TIME_STRUCT_PTR  time_ptr,
    uint_32          milliseconds
)
{
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    MQX_TICK_STRUCT stick;
    MQX_TICK_STRUCT wtick;
    /* TIME_STRUCT                     wtime; */
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_periodic_at, notification_function_ptr, mode,
                    time_ptr);

#if MQX_CHECK_ERRORS
    if ((notification_function_ptr == NULL) ||
                    (time_ptr == NULL) ||
                    (milliseconds == 0))
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_timer_start_periodic_at, TIMER_NULL_ID);
        return TIMER_NULL_ID;
    } /* Endif */
#endif

    PSP_TIME_TO_TICKS(time_ptr, &stick);

    /* wtime.SECONDS      = 0;            */
    /* wtime.MILLISECONDS = milliseconds; */
    /*                                    */
    /* PSP_TIME_TO_TICKS(&wtime, &wtick); */
    PSP_MILLISECONDS_TO_TICKS_QUICK(milliseconds, &wtick);

    result = _timer_start_periodic_at_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, (MQX_TICK_STRUCT_PTR)&stick,
                    (MQX_TICK_STRUCT_PTR)&wtick, FALSE);

    _KLOGX2(KLOG_timer_start_periodic_at, result);

    return(result);

} /* Endbody */

/*!
 * \brief Starts a periodic timer every specified number of milliseconds.
 * 
 * When the timer expires, MQX calls notification_function with timer_id, notifier 
 * data, and the current time.
 * \n You might need to increase the Timer Task stack size to accommodate the 
 * notification function (see _timer_create_component()).
 *  
 * \param[in] notification_function_ptr Pointer to the notification function that 
 * MQX calls when the timer expire.
 * \param[in] notification_data_ptr     Pointer to the data that MQX passes to 
 * the notification function.
 * \param[in] mode                      Time to use when calculating the time to 
 * expire; one of the following:
 * \li TIMER_ELAPSED_TIME_MODE (Use _time_get_elapsed() or _time_get_elapsed_ticks(), 
 * which are not affected by _time_set() or _time_set_ticks().)
 * \li TIMER_KERNEL_TIME_MODE (Use _time_get() or _time_get_ticks().)
 * \param[in] milliseconds              Milliseconds that MQX waits before it 
 * first calls the notification function and between subsequent calls to the 
 * notification function.
 * 
 * \return Timer ID (success)
 * \return TIMER_NULL_ID (failure) 
 * 
 * \warning Creates the timer component with default values if it was not 
 * previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_COMPONENT_BASE (Timer component data is no longer valid.)
 * \li MQX_INVALID_PARAMETER (One of the following: mode is not one of the 
 * allowed modes, notification_function is NULL, milliseconds is 0 or 
 * tick_time_ptr is NULL.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the timer data.)
 * \li MQX_INVALID_LWSEM
 * \li return code from _timer_create_component() 
 * 
 * \see _timer_start_periodic_every_ticks
 * \see _timer_cancel
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _task_set_error
 * \see _timer_create_component 
 */
_timer_id _timer_start_periodic_every
(
    TIMER_NOTIFICATION_TIME_FPTR notification_function_ptr,
    pointer          notification_data_ptr,
    _mqx_uint        mode,
    uint_32          milliseconds
)
{
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    MQX_TICK_STRUCT wticks;
    /* TIME_STRUCT                    wtime; */
    _timer_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_timer_start_periodic_every, notification_function_ptr, mode,
                    milliseconds);

    /* wtime.SECONDS      = 0;             */
    /* wtime.MILLISECONDS = milliseconds;  */
    /*                                     */
    /* PSP_TIME_TO_TICKS(&wtime, &wticks); */
    PSP_MILLISECONDS_TO_TICKS_QUICK(milliseconds, &wticks);

    result = _timer_start_periodic_every_internal((TIMER_NOTIFICATION_FPTR)notification_function_ptr,
                    notification_data_ptr, mode, (MQX_TICK_STRUCT_PTR)&wticks, FALSE);

    _KLOGX2(KLOG_timer_start_periodic_every, result);
    return(result);

} /* Endbody */

/*!
 * \brief This task runs the timer queues, calling the notification functions as 
 * required.
 * 
 * \param[in] fake_parameter Fake parameter.
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_LWSEM
 */ 
void _timer_task
(
    uint_32 fake_parameter
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TIMER_ENTRY_STRUCT_PTR     timer_entry_ptr;
    QUEUE_STRUCT_PTR           queue_ptrs[2];
    QUEUE_STRUCT_PTR           queue_ptr;
#if MQX_TIMER_USES_TICKS_ONLY==0
    TIME_STRUCT                time;
#endif
    MQX_TICK_STRUCT            ticks;
    _mqx_uint                  i;
    _mqx_int                   result;

    _GET_KERNEL_DATA(kernel_data);

    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
#if MQX_CHECK_ERRORS
    if (timer_component_ptr == NULL)
    {
        _task_block();
    } /* Endif */
#endif

    timer_component_ptr->TIMER_TD_PTR = kernel_data->ACTIVE_PTR;
    _QUEUE_INIT(&timer_component_ptr->ELAPSED_TIMER_ENTRIES, 0);
    _QUEUE_INIT(&timer_component_ptr->KERNEL_TIMER_ENTRIES, 0);
    _lwsem_create(&timer_component_ptr->TIMER_ISR_LWSEM, 0);
    _lwsem_create(&timer_component_ptr->TIMER_ENTRIES_LWSEM, 1);
    timer_component_ptr->VALID = TIMER_VALID;

    kernel_data->TIMER_COMPONENT_ISR = _timer_isr;
    queue_ptrs[0] = &timer_component_ptr->ELAPSED_TIMER_ENTRIES;
    queue_ptrs[1] = &timer_component_ptr->KERNEL_TIMER_ENTRIES;

    while (TRUE)
    {

        for (i = 0; i < 2; ++i)
        {

            queue_ptr = queue_ptrs[i];

            /* Stop tasks from accessing the timer entries table */
            if (_lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM) != MQX_OK)
            {
                _task_set_error(MQX_INVALID_LWSEM);
                _task_block();
            } /* Endif */

            while (queue_ptr->SIZE > 0)
            {
                /* 
                 * Check the first entry on the queue to see if it has expired 
                 * Note that entries are always put into the list in sorted order.
                 */
                timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)
                ((pointer)queue_ptr->NEXT);
                if (i == 0)
                {
                    _time_get_elapsed_ticks(&ticks);
                }
                else
                {
                    _time_get_ticks(&ticks);
                } /* Endif */

                result = PSP_CMP_TICKS(&timer_entry_ptr->EXPIRATION_TIME, &ticks);

                if (result <= 0)
                {
                    /* This entry has expired, remove it from the queue */
                    _QUEUE_REMOVE(queue_ptr, timer_entry_ptr);

                    timer_component_ptr->ENTRY_PTR = timer_entry_ptr;

                    /* fire its notification function */

                    if (timer_entry_ptr->VALID)
                    {

#if MQX_TIMER_USES_TICKS_ONLY==0
                        if (!timer_entry_ptr->USES_TICKS)
                        {
                            PSP_TICKS_TO_TIME(&ticks, &time);
                            (*(TIMER_NOTIFICATION_TIME_FPTR)timer_entry_ptr->NOTIFICATION_FUNCTION)(
                                            timer_entry_ptr->ID,
                                            timer_entry_ptr->NOTIFICATION_DATA_PTR, time.SECONDS,
                                            time.MILLISECONDS);
                        }
                        else
                        {
#endif
                            (*(TIMER_NOTIFICATION_TICK_FPTR)timer_entry_ptr->NOTIFICATION_FUNCTION)(
                                            timer_entry_ptr->ID,
                                            timer_entry_ptr->NOTIFICATION_DATA_PTR,
                                            (pointer)&ticks);
#if MQX_TIMER_USES_TICKS_ONLY==0
                        } /* Endif */
#endif
                    }

                    /* This entry may be a one-shot, in which case it is to be deleted */
                    if (IS_ONESHOT(timer_entry_ptr->TIMER_TYPE) ||
                                    (timer_entry_ptr->VALID==0))
                    {
                        timer_entry_ptr->VALID = 0;
                        _mem_free(timer_entry_ptr);
                    }
                    else
                    {
                        /* Calculate next firing period */
                        PSP_ADD_TICKS(&timer_entry_ptr->EXPIRATION_TIME,
                                        &timer_entry_ptr->CYCLE,
                                        &timer_entry_ptr->EXPIRATION_TIME);
                        _timer_insert_queue_internal(queue_ptr, timer_entry_ptr);
                    }/* Endif */

                }
                else
                {
                    /* This queue does not require work */
                    break;
                }/* Endif */

            } /* Endwhile */

            if (_lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM) !=
                            MQX_OK)
            {
                _task_set_error(MQX_INVALID_LWSEM);
                _task_block();
            } /* Endif */

        } /* Endfor */

        /* wait until activation from the kernel timer isr */
        if (_lwsem_wait(&timer_component_ptr->TIMER_ISR_LWSEM) != MQX_OK)
        {
            _task_set_error(MQX_INVALID_LWSEM);
            _task_block();
        } /* Endif */

    } /* Endwhile */

} /* Endbody */

/*!
 * \brief Check the timer queues and if a timer is pending schedule the timer 
 * task to run. 
 * 
 * This function is called from the kernel timer ISR.
 * 
 *  
 */ 
void _timer_isr(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    TIMER_ENTRY_STRUCT_PTR     timer_entry_ptr;
    MQX_TICK_STRUCT            kernel_time;
    _mqx_int                   result;

    _GET_KERNEL_DATA(kernel_data);

    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
#if MQX_CHECK_ERRORS
    if (timer_component_ptr == NULL)
    {
        return;
    } /* Endif */
#endif

    if (timer_component_ptr->TIMER_TD_PTR->STATE == READY)
    {
        return;
    }/* Endif */

    _time_get_elapsed_ticks(&kernel_time);
    if (_QUEUE_GET_SIZE(&timer_component_ptr->ELAPSED_TIMER_ENTRIES))
    {
        timer_entry_ptr = (pointer)timer_component_ptr->ELAPSED_TIMER_ENTRIES.NEXT;
        result = PSP_CMP_TICKS(&kernel_time, &timer_entry_ptr->EXPIRATION_TIME);
        if (result >= 0)
        {
            _lwsem_post(&timer_component_ptr->TIMER_ISR_LWSEM);
            return;
        }/* Endif */
    }/* Endif */

    if (_QUEUE_GET_SIZE(&timer_component_ptr->KERNEL_TIMER_ENTRIES))
    {
        PSP_ADD_TICKS(&kernel_time, &kernel_data->TIME_OFFSET, &kernel_time);
        timer_entry_ptr = (pointer)timer_component_ptr->KERNEL_TIMER_ENTRIES.NEXT;
        result = PSP_CMP_TICKS(&kernel_time, &timer_entry_ptr->EXPIRATION_TIME);
        if (result >= 0)
        {
            _lwsem_post(&timer_component_ptr->TIMER_ISR_LWSEM);
            return;
        }/* Endif */
    }/* Endif */

} /* Endbody */

/*!
 * \private
 * 
 * \brief Insert timer_entry_ptr into the queue by time with lowest time first.
 * 
 * \param[in] queue_ptr       The queue to insert onto.
 * \param[in] timer_entry_ptr The entry to insert into the queue.
 */ 
void _timer_insert_queue_internal
(
    register QUEUE_STRUCT_PTR       queue_ptr,
    register TIMER_ENTRY_STRUCT_PTR timer_entry_ptr
)
{ /* Body */
    /* The element on the queue to start inserting from */
    register TIMER_ENTRY_STRUCT_PTR element_ptr;
    register TIMER_ENTRY_STRUCT_PTR prev_ptr;
    register _mqx_int               result;

    prev_ptr = NULL;
    element_ptr = (pointer)queue_ptr->NEXT;
    while (element_ptr != (pointer)queue_ptr)
    {
        result = PSP_CMP_TICKS(&element_ptr->EXPIRATION_TIME,
                        &timer_entry_ptr->EXPIRATION_TIME);
        if (MQX_DELAY_ENQUEUE_POLICY(result))
        {
            /* Found the spot to insert it */
            break;
        }/* Endif */
        prev_ptr = element_ptr;
        element_ptr = (pointer)element_ptr->QUEUE_ELEMENT.NEXT;
    } /* Endwhile */

    _queue_insert(queue_ptr, (QUEUE_ELEMENT_STRUCT_PTR)((pointer)prev_ptr),
                    (QUEUE_ELEMENT_STRUCT_PTR)((pointer)timer_entry_ptr));

} /* Endbody */

/*!
 * \private
 * 
 * \brief Create a new timer ID
 * 
 * \param[in] timer_component_ptr The address of the timer component structure.
 * 
 * \return New timer ID. 
 */ 
_timer_id _timer_alloc_id_internal
(
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr
)
{ /* Body */
    QUEUE_STRUCT_PTR       queue_ptr;
    TIMER_ENTRY_STRUCT_PTR timer_entry_ptr;
    _timer_id              id;
    boolean                ok;
    _mqx_uint              i;
    _mqx_uint              size;

    if (timer_component_ptr->ID == 0)
    {
        timer_component_ptr->ID = 1;
    } /* Endif */
    id = timer_component_ptr->ID;

    /* Make sure ID not in use */
    ok = FALSE;
    while (!ok)
    {
        ok = TRUE;
        for (i = 0; (i < 2) && ok; i++)
        {
            if (i == 0)
            {
                queue_ptr = &timer_component_ptr->ELAPSED_TIMER_ENTRIES;
            }
            else
            {
                queue_ptr = &timer_component_ptr->KERNEL_TIMER_ENTRIES;
            } /* Endif */
            timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)((pointer)queue_ptr->NEXT);
            /* SPR P171-0023-01 use pre-decrement on while loop */
            size = queue_ptr->SIZE + 1;
            while (--size)
            {
                /* END SPR */
                if (timer_entry_ptr->ID == id)
                {
                    ok = FALSE;
                    id++;
                    break;
                } /* Endif */
                timer_entry_ptr = (TIMER_ENTRY_STRUCT_PTR)
                ((pointer)timer_entry_ptr->QUEUE_ELEMENT.NEXT);
            } /* Endwhile */
        } /* Endfor */
    } /* Endwhile */

    timer_component_ptr->ID = id + 1;

    return(id);

} /* Endbody */

/*!
 * \brief This is the function that is called by a task during task destruction, 
 * in order to cleanup any timers.
 * 
 * \param[in] td_ptr The task being destroyed. 
 */ 
void _timer_cleanup
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    QUEUE_STRUCT_PTR           queue_ptr;
    TIMER_ENTRY_STRUCT_PTR     element_ptr;
    TIMER_ENTRY_STRUCT_PTR     next_element_ptr;

    _GET_KERNEL_DATA(kernel_data);

    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
    if (timer_component_ptr == NULL)
    {
        return;
    } /* Endif */

    /* Gain exclusive access to the timer queues */
    _lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM);

    queue_ptr = (QUEUE_STRUCT_PTR)&timer_component_ptr->ELAPSED_TIMER_ENTRIES;
    element_ptr = (TIMER_ENTRY_STRUCT_PTR)((pointer)queue_ptr->NEXT);
    while ((pointer)element_ptr != (pointer)queue_ptr)
    {
        next_element_ptr = (TIMER_ENTRY_STRUCT_PTR)
        ((pointer)element_ptr->QUEUE_ELEMENT.NEXT);
        if (element_ptr->TD_PTR == td_ptr)
        {
            _QUEUE_REMOVE(queue_ptr, element_ptr);
            element_ptr->VALID = 0;
            _mem_free(element_ptr);
        } /* Endif */
        element_ptr = next_element_ptr;
    } /* Endwhile */

    queue_ptr = (QUEUE_STRUCT_PTR)
    ((pointer)&timer_component_ptr->KERNEL_TIMER_ENTRIES);
    element_ptr = (TIMER_ENTRY_STRUCT_PTR)((pointer)queue_ptr->NEXT);
    while ((pointer)element_ptr != (pointer)queue_ptr)
    {
        next_element_ptr = (TIMER_ENTRY_STRUCT_PTR)
        ((pointer)element_ptr->QUEUE_ELEMENT.NEXT);
        if (element_ptr->TD_PTR == td_ptr)
        {
            _QUEUE_REMOVE(queue_ptr, element_ptr);
            element_ptr->VALID = 0;
            _mem_free(element_ptr);
        } /* Endif */
        element_ptr = next_element_ptr;
    } /* Endwhile */

    _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);

} /* Endbody */

/*!
 * \brief Tests the timer component for validity and consistency.
 * 
 * \param[out] timer_error_ptr Pointer to the first timer entry that has an error.
 * 
 * \return MQX_OK
 * \return MQX_INVALID_COMPONENT_HANDLE (One of the timer entries in the timer 
 * queue is not valid (timer_error_ptr).)
 * \return MQX_CORRUPT_QUEUE (Queue of timers is not valid.)  
 * 
 * \see _timer_start_oneshot_after
 * \see _timer_start_oneshot_after_ticks
 * \see _timer_start_oneshot_at
 * \see _timer_start_oneshot_at_ticks
 * \see _timer_start_periodic_at
 * \see _timer_start_periodic_at_ticks
 * \see _timer_start_periodic_every
 * \see _timer_start_periodic_every_ticks
 * \see _timer_cancel
 */ 
_mqx_uint _timer_test
(
    pointer _PTR_ timer_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;
    QUEUE_STRUCT_PTR           queue_ptr;
    TIMER_ENTRY_STRUCT_PTR     element_ptr;
    _mqx_uint                  result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_timer_test, timer_error_ptr);

    *timer_error_ptr = NULL;

    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
    if (timer_component_ptr == NULL)
    {
        _KLOGX2(KLOG_timer_test, MQX_OK);
        return(MQX_OK);
    } /* Endif */

    /* Gain exclusive access to the timer queues */
    _lwsem_wait(&timer_component_ptr->TIMER_ENTRIES_LWSEM);

    result = _queue_test(&timer_component_ptr->ELAPSED_TIMER_ENTRIES,
                    timer_error_ptr);
    if (result != MQX_OK)
    {
        _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
        _KLOGX3(KLOG_timer_test, result, *timer_error_ptr);
        return(result);
    } /* Endif */

    result = _queue_test(&timer_component_ptr->KERNEL_TIMER_ENTRIES,
                    timer_error_ptr);
    if (result != MQX_OK)
    {
        _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
        _KLOGX3(KLOG_timer_test, result, *timer_error_ptr);
        return(result);
    } /* Endif */

    queue_ptr = (pointer)&timer_component_ptr->ELAPSED_TIMER_ENTRIES;
    element_ptr = (pointer)queue_ptr->NEXT;
    while (element_ptr != (pointer)queue_ptr)
    {
#if MQX_CHECK_VALIDITY
        if (element_ptr->VALID != TIMER_VALID)
        {
            *timer_error_ptr = element_ptr;
            _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
            _KLOGX3(KLOG_timer_test, MQX_INVALID_COMPONENT_HANDLE,
                            *timer_error_ptr);
            return(MQX_INVALID_COMPONENT_HANDLE);
        } /* Endif */
#endif
        element_ptr = (pointer)element_ptr->QUEUE_ELEMENT.NEXT;
    } /* Endwhile */

    queue_ptr = (pointer)&timer_component_ptr->KERNEL_TIMER_ENTRIES;
    element_ptr = (pointer)queue_ptr->NEXT;
    while (element_ptr != (pointer)queue_ptr)
    {
#if MQX_CHECK_VALIDITY
        if (element_ptr->VALID != TIMER_VALID)
        {
            *timer_error_ptr = element_ptr;
            _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
            _KLOGX3(KLOG_timer_test, MQX_INVALID_COMPONENT_HANDLE,
                            *timer_error_ptr);
            return(MQX_INVALID_COMPONENT_HANDLE);
        } /* Endif */
#endif
        element_ptr = (pointer)element_ptr->QUEUE_ELEMENT.NEXT;
    } /* Endwhile */

    _lwsem_post(&timer_component_ptr->TIMER_ENTRIES_LWSEM);
    _KLOGX2(KLOG_timer_test, MQX_OK);
    return(MQX_OK);

} /* Endbody */

#endif /* MQX_USE_TIMER */

/* EOF */
