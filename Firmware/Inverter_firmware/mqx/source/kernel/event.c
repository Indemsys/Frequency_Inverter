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
 * $FileName: event.c$
 * $Version : 3.8.2.0$
 * $Date    : Jun-25-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Event component.
 *
 *END************************************************************************/

#include <string.h>
#include "mqx_inc.h"
#if MQX_USE_EVENTS
#include "name.h"
#include "name_prv.h"
#include "event.h"
#include "event_prv.h"
#include "mqx_str.h"
#include "message.h"
#include "msg_prv.h"
#if MQX_USE_IPC
#include "ipc.h"
#include "ipc_prv.h"
#endif
/*!
 * \brief Wait for all the specified event bits to be set in the event group for
 * the specified number of milliseconds.
 * 
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to wait for.
 * \param[in] timeout         The maximum number of milliseconds to wait for the 
 * events to be set. If the value is 0, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 * 
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until                    
 */ 
_mqx_uint _event_wait_all
(
    pointer   users_event_ptr,
    _mqx_uint bit_mask,
    uint_32   timeout
)
{ /* Body */
    /* TIME_STRUCT                 time; */
    MQX_TICK_STRUCT ticks;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_all, users_event_ptr, bit_mask, timeout);

    if (timeout)
    {
        /* Convert milliseconds to ticks */
        /* time.MILLISECONDS = timeout % 1000; */
        /* time.SECONDS      = timeout / 1000; */
        /*                                     */
        /* PSP_TIME_TO_TICKS(&time, &ticks);   */
        PSP_MILLISECONDS_TO_TICKS_QUICK(timeout, &ticks);

        result = _event_wait_internal(users_event_ptr, bit_mask, &ticks, FALSE, TRUE);
    }
    else
    {
        result = _event_wait_internal(users_event_ptr, bit_mask, NULL, FALSE, TRUE);
    } /* Endif */

    _KLOGX2(KLOG_event_wait_all, result);
    return (result);

} /* Endbody */

/*!
 * \brief Wait for all the specified event bits to be set in the event group for
 * the specified number of ticks (in tick time).
 * 
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to wait for.
 * \param[in] tick_ptr        The maximum number of ticks to wait for the events
 * to be set. If the value is NULL, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 * 
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_wait_all
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until                    
 */
_mqx_uint _event_wait_all_for
(
    pointer             users_event_ptr,
    _mqx_uint           bit_mask,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_all_for, users_event_ptr, bit_mask, tick_ptr);

    result = _event_wait_internal(users_event_ptr, bit_mask, tick_ptr, FALSE, TRUE);

    _KLOGX2(KLOG_event_wait_all_for, result);
    return (result);

} /* Endbody */

/*!
 * \brief Wait for all the specified event bits to be set in the event group for
 * the specified number of ticks.
 * 
 * \param[in] users_event_ptr  An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask         Each set bit represents an event bit to wait for.
 * \param[in] timeout_in_ticks The maximum number of ticks to wait for the events
 * to be set. If the value is 0, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 * 
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_until
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until                    
 */
_mqx_uint _event_wait_all_ticks
(
    pointer   users_event_ptr,
    _mqx_uint bit_mask,
    _mqx_uint timeout_in_ticks
)
{ /* Body */
    MQX_TICK_STRUCT ticks;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_all_ticks, users_event_ptr, bit_mask,
                    timeout_in_ticks);

    if (!timeout_in_ticks)
    {
        result = _event_wait_internal(users_event_ptr, bit_mask, NULL, FALSE, TRUE);
    }
    else
    {
        ticks = _mqx_zero_tick_struct;

        PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, timeout_in_ticks, &ticks);
        result = _event_wait_internal(users_event_ptr, bit_mask, &ticks, FALSE, TRUE);
    } /* Endif */

    _KLOGX2(KLOG_event_wait_all_ticks, result);
    return (result);

} /* Endbody */

/*!
 * \brief Wait for all the specified event bits to be set in the event group 
 * until the specified time (in tick time).
 * 
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to wait for.
 * \param[in] tick_ptr        The time to wait until, in ticks, for the events 
 * to be set. If the value is NULL, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 * 
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until                    
 */
_mqx_uint _event_wait_all_until
(
    pointer users_event_ptr,
    _mqx_uint bit_mask,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_all_until, users_event_ptr, bit_mask, tick_ptr);

    result = _event_wait_internal(users_event_ptr, bit_mask, tick_ptr, TRUE, TRUE);

    _KLOGX2(KLOG_event_wait_all_until, result);
    return (result);

} /* Endbody */

/*!
 * \brief Wait for any of the specified event bits to be set in the event group 
 * for the specified number of milliseconds.
 * 
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to wait for.
 * \param[in] timeout         The maximum number of milliseconds to wait for the 
 * events to be set. If the value is 0, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 *  
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR. 
 *  
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until 
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until        
 */  
_mqx_uint _event_wait_any
(
    pointer users_event_ptr,
    _mqx_uint bit_mask,
    uint_32 timeout
)
{ /* Body */
    /* TIME_STRUCT                    time; */
    MQX_TICK_STRUCT ticks;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_any, users_event_ptr, bit_mask, timeout);

    if (timeout)
    {
        /* Convert milliseconds to ticks */
        /* time.MILLISECONDS = timeout % 1000; */
        /* time.SECONDS      = timeout / 1000; */
        /*                                     */
        /* PSP_TIME_TO_TICKS(&time, &ticks);   */
        PSP_MILLISECONDS_TO_TICKS_QUICK(timeout, &ticks);

        result = _event_wait_internal(users_event_ptr, bit_mask, &ticks, FALSE, FALSE);
    }
    else
    {
        result = _event_wait_internal(users_event_ptr, bit_mask, NULL, FALSE, FALSE);
    } /* Endif */

    _KLOGX2(KLOG_event_wait_any, result);
    return (result);

} /* Endbody */

/*!
 * \brief Wait for any of the specified event bits to be set in the event group 
 * for the specified number of milliseconds.
 * 
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to wait for.
 * \param[in] tick_ptr        The maximum number of ticks to wait for the events
 * to be set. If the value is 0, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 *  
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR. 
 *  
 * \see _event_wait_any
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until 
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until        
 */
_mqx_uint _event_wait_any_for
(
    pointer users_event_ptr,
    _mqx_uint bit_mask,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_any_for, users_event_ptr, bit_mask, tick_ptr);

    result = _event_wait_internal(users_event_ptr, bit_mask, tick_ptr, FALSE, FALSE);

    _KLOGX2(KLOG_event_wait_any_for, result);
    return (result);

} /* Endbody */

/*!
 * \brief Wait for any of the specified event bits to be set in the event group 
 * for the number of ticks.
 * 
 * \param[in] users_event_ptr  An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask         Each set bit represents an event bit to wait for.
 * \param[in] timeout_in_ticks The maximum number of ticks to wait for the events 
 * to be set. If the value is 0, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 *  
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR. 
 *  
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_until 
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until        
 */
_mqx_uint _event_wait_any_ticks
(
    pointer users_event_ptr,
    _mqx_uint bit_mask,
    _mqx_uint timeout_in_ticks
)
{ /* Body */
    MQX_TICK_STRUCT ticks;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_any_ticks, users_event_ptr, bit_mask,
                    timeout_in_ticks);

    if (timeout_in_ticks)
    {
        ticks = _mqx_zero_tick_struct;

        PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, timeout_in_ticks, &ticks);
        result = _event_wait_internal(users_event_ptr, bit_mask, &ticks, FALSE, FALSE);
    }
    else
    {
        result = _event_wait_internal(users_event_ptr, bit_mask, NULL, FALSE, FALSE);
    } /* Endif */

    _KLOGX2(KLOG_event_wait_any_ticks, result);
    return (result);

} /* Endbody */

/*!
 * \brief Wait for any of the specified event bits to be set in the event group 
 * until the specified time (in tick time).
 * 
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to wait for.
 * \param[in] tick_ptr        The time in ticks to wait for the events to be set. 
 * If the value is NULL, then the timeout will be infinite.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)
 *  
 * \warning Blocks until the event combination is set or until the timeout expires.
 * \warning Cannot be called from an ISR. 
 *  
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks 
 * \see _event_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until        
 */
_mqx_uint _event_wait_any_until
(
    pointer users_event_ptr,
    _mqx_uint bit_mask,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_event_wait_any_until, users_event_ptr, bit_mask, tick_ptr);

    result = _event_wait_internal(users_event_ptr, bit_mask, tick_ptr, TRUE, FALSE);

    _KLOGX2(KLOG_event_wait_any_until, result);
    return (result);

} /* Endbody */

/*! 
 * \brief Clears the specified event bits in an event group.
 * 
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to clear.
 * 
 * \return MQX_OK
 * \return EVENT_INVALID_EVENT_HANDLE (_event_open() or _event_open_fast() did 
 * not get the event group handle or _event_create() did not create the event 
 * group.)
 * \return EVENT_INVALID_EVENT (Event group is not valid.)
 * 
 * \see _event_create
 * \see _event_create_auto_clear
 * \see _event_open
 * \see _event_open_fast
 * \see _event_set
 * \see _event_get_value
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until 
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until          
 */  
_mqx_uint _event_clear
(
    pointer users_event_ptr,
    _mqx_uint bit_mask
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    register EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    register EVENT_STRUCT_PTR            event_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_event_clear, users_event_ptr, bit_mask);
    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) users_event_ptr;

#if MQX_CHECK_VALIDITY
    if (event_connection_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_clear, EVENT_INVALID_EVENT_HANDLE);
        return (EVENT_INVALID_EVENT_HANDLE);
    } /* Endif */
#endif

    _INT_DISABLE();
    event_ptr = event_connection_ptr->EVENT_PTR;

#if MQX_CHECK_VALIDITY
    if (event_ptr->VALID != EVENT_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_event_clear, EVENT_INVALID_EVENT);
        return (EVENT_INVALID_EVENT);
    } /* Endif */
#endif

    event_ptr->EVENT &= ~bit_mask;
    _INT_ENABLE();

    _KLOGX2(KLOG_event_clear, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Closes connection to the event group.
 * 
 * The function closes the connection to the event group and frees the event 
 * group handle.
 * \n A task that opened an event group on a remote processor can also close the 
 * event group.
 *   
 * \param[in] users_event_ptr An event handle returned from a call to _event_open() 
 * or _event_open_fast().
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Event group connection is not valid.)
 * \return Task error code from _mem_free() (MQX could not free the event group handle.)
 * 
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_destroy
 * \see _event_open
 * \see _event_open_fast         
 */  
_mqx_uint _event_close
(
    pointer users_event_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    register EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_event_close, users_event_ptr);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_event_close, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) users_event_ptr;
#if MQX_CHECK_VALIDITY
    if (event_connection_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_close, EVENT_INVALID_EVENT_HANDLE);
        return (EVENT_INVALID_EVENT_HANDLE);
    } /* Endif */

#endif

    event_connection_ptr->VALID = 0;

    if (_mem_free(event_connection_ptr) != MQX_OK)
    {
        _KLOGX2(KLOG_event_close, kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);
        return (kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);
    }
    else
    {
        _KLOGX2(KLOG_event_close, MQX_OK);
        return (MQX_OK);
    } /* Endif */

} /* Endbody */

 /*!
  * \brief Creates the event component so that other tasks may create and use 
  * events.
  * 
  * If an application previously called the function and maximum_number is now 
  * greater that what was previously specified, MQX changes the maximum number 
  * of event groups to maximum_number.
  * \n If an application does not explicitly create the event component, MQX does 
  * so with the following default values the first time that a task calls a 
  * function in the _event_create family of functions.
  * \n Parameter default values:
  * \li Initial_number = 8(default)
  * \li Grow_number = 8(default)
  * \li Maximum_number = 0(unlimited)
  * 
  * \param[in] initial_number Initial number of event groups that the application 
  * can create.
  * \param[in] grow_number Number of event groups to add if the application 
  * creates all the event groups
  * \param[in] maximum_number If grow_number is non-zero, maximum number of event 
  * groups (0 means an unlimited number)
  * 
  * \return MQX_OK
  * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
  * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event group.)
  * 
  * \see _event_create
  * \see _event_create_auto_clear
  * \see _event_create_fast
  * \see _event_create_fast_auto_clear
  * \see _event_open
  * \see _event_open_fast                                         
  */  
_mqx_uint _event_create_component
(
    _mqx_uint initial_number,
    _mqx_uint grow_number,
    _mqx_uint maximum_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    register EVENT_COMPONENT_STRUCT_PTR event_component_ptr;
    _mqx_uint                           result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_event_create_component, initial_number, grow_number, maximum_number);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_event_create_component, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    _lwsem_wait((LWSEM_STRUCT_PTR) (&kernel_data->COMPONENT_CREATE_LWSEM));

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    if (event_component_ptr != NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR) (&kernel_data->COMPONENT_CREATE_LWSEM));
        _KLOGX2(KLOG_event_create_component, MQX_OK);
        return (MQX_OK);
    } /* Endif */

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) _mem_alloc_system_zero(
                    (_mem_size) sizeof(EVENT_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (event_component_ptr == NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR) (&kernel_data->COMPONENT_CREATE_LWSEM));
        _KLOGX2(KLOG_event_create_component, MQX_OUT_OF_MEMORY);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(event_component_ptr, MEM_TYPE_EVENT_COMPONENT);

    result = _name_create_handle_internal(&event_component_ptr->NAME_TABLE_HANDLE, initial_number, grow_number,
                    maximum_number, initial_number);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        _lwsem_post((LWSEM_STRUCT_PTR) (&kernel_data->COMPONENT_CREATE_LWSEM));
        _mem_free(event_component_ptr);
        _KLOGX2(KLOG_event_create_component, result);
        return (result);
    } /* Endif */
#endif
    event_component_ptr->VALID = EVENT_VALID;
    event_component_ptr->GROW_NUMBER = grow_number;
    if (maximum_number == 0)
    {
        event_component_ptr->MAXIMUM_NUMBER = MAX_MQX_UINT;
    }
    else if (maximum_number < initial_number)
    {
        event_component_ptr->MAXIMUM_NUMBER = initial_number;
    }
    else
    {
        event_component_ptr->MAXIMUM_NUMBER = maximum_number;
    } /* Endif */

    kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS] = event_component_ptr;

#if MQX_COMPONENT_DESTRUCTION
    kernel_data->COMPONENT_CLEANUP[KERNEL_EVENTS] = _event_cleanup;
#endif

    _lwsem_post((LWSEM_STRUCT_PTR) (&kernel_data->COMPONENT_CREATE_LWSEM));

    _KLOGX2(KLOG_event_create_component, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Creates the named event group with autoclearing event bits.
 * 
 * After a task creates a named event group, any task that wants to use it must 
 * open a connection to it with _event_open(). When a task no longer needs a 
 * named event group, it can destroy the event group with _event_destroy().
 * \n When a task creates an event group with autoclearing event bits, MQX clears 
 * the event bits as soon as they are set. Tasks that are waiting for the event 
 * bits are made ready, but need not to clear the bits.
 * 
 * \param[in] name_ptr Pointer to the name of the event group.
 * 
 * \return MQX_OK 
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event group.)
 * \return EVENT_EXISTS (Event group was already created.)
 * \return EVENT_TABLE_FULL (Name table is full and cannot be expanded.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component was not created.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * 
 * \warning Creates the event component with default values if it was not 
 * previously created.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_create
 * \see _event_close
 * \see _event_create_component
 * \see _event_destroy
 * \see _event_open           
 */  
_mqx_uint _event_create_auto_clear
(
    char _PTR_ name_ptr
)
{ /* Body */
    EVENT_STRUCT_PTR event_ptr;
    _mqx_uint        result;

    result = _event_create_internal(name_ptr, &event_ptr);

#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        return (result);
    } /* Endif */
#endif

    event_ptr->AUTO_CLEAR = TRUE;

    return (result);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Used internally to create an instance of an event.
 * 
 * \param[in]  name_ptr      Pointer to the name of the event group.
 * \param[out] event_ptr_ptr Where the event pointer is.
 * 
 * \return MQX_OK 
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event group.)
 * \return EVENT_EXISTS (Event group was already created.)
 * \return EVENT_TABLE_FULL (Name table is full and cannot be expanded.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component was not created.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * 
 * \warning Creates the event component with default values if it was not 
 * previously created.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_create
 * \see _event_create_auto_clear          
 */
_mqx_uint _event_create_internal
(
    char _PTR_             name_ptr,
    EVENT_STRUCT_PTR _PTR_ event_ptr_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    register EVENT_COMPONENT_STRUCT_PTR event_component_ptr;
    register EVENT_STRUCT_PTR           event_ptr;
    _mqx_uint                           result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_event_create, name_ptr);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_event_create, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    if (event_component_ptr == NULL)
    {
        result = _event_create_component(EVENT_DEFAULT_INITIAL_NUMBER, EVENT_DEFAULT_GROW_NUMBER,
                        EVENT_DEFAULT_MAXIMUM_NUMBER);
        event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (event_component_ptr == NULL)
        {
            _KLOGX2(KLOG_event_create, result);
            return (result);
        } /* Endif */
#endif
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_create, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    event_ptr = (EVENT_STRUCT_PTR) _mem_alloc_system_zero((_mem_size) sizeof(EVENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (event_ptr == NULL)
    {
        _KLOGX2(KLOG_event_create, MQX_OUT_OF_MEMORY);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(event_ptr, MEM_TYPE_EVENT);

    _QUEUE_INIT(&event_ptr->WAITING_TASKS, EVENT_MAX_WAITING_TASKS);

    strncpy(event_ptr->NAME, name_ptr, (_mqx_uint) NAME_MAX_NAME_SIZE - 1);
    event_ptr->NAME[NAME_MAX_NAME_SIZE - 1] = '\0';
    result = _name_add_internal(event_component_ptr->NAME_TABLE_HANDLE, event_ptr->NAME, (_mqx_uint) event_ptr);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        _mem_free(event_ptr);
        if (result == NAME_EXISTS)
        {
            _KLOGX2(KLOG_event_create, EVENT_EXISTS);
            return (EVENT_EXISTS);
        }
        else if (result == NAME_TABLE_FULL)
        {
            _KLOGX2(KLOG_event_create, EVENT_TABLE_FULL);
            return (EVENT_TABLE_FULL);
        } /* Endif */
        _KLOGX2(KLOG_event_create, result);
        return (result);
    } /* Endif */
#endif
    event_ptr->VALID = EVENT_VALID;

    *event_ptr_ptr = event_ptr;

    _KLOGX2(KLOG_event_create, result);
    return (result);

} /* Endbody */

/*!
 * \brief Creates the named event group.
 * 
 * After a task creates a named event group, any task that wants to use it must 
 * open a connection to it with _event_open(). When a task no longer needs a 
 * named event group, it can destroy the event group with _event_destroy().
 * 
 * \param[in] name_ptr Pointer to the name of the event group.
 * 
 * \return MQX_OK 
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event group.)
 * \return EVENT_EXISTS (Event group was already created.)
 * \return EVENT_TABLE_FULL (Name table is full and cannot be expanded.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component was not created.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * 
 * \warning Creates the event component with default values if it was not 
 * previously created.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_create_auto_clear
 * \see _event_close
 * \see _event_create_component
 * \see _event_destroy
 * \see _event_open      
 */ 
_mqx_uint _event_create
(
    char _PTR_ name_ptr
)
{ /* Body */
    EVENT_STRUCT_PTR event_ptr;
    _mqx_uint        result;

    result = _event_create_internal(name_ptr, &event_ptr);

#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        return (result);
    } /* Endif */
#endif

    event_ptr->AUTO_CLEAR = FALSE;

    return (result);

} /* Endbody */

/*!
 * \brief Used by a task to destroy an instance of an event.
 * 
 * The event group must have been created with _event_create() or 
 * _event_create_auto_clear().
 * \n If tasks are blocked waiting for an event bit in the event group, MQX does 
 * the following:
 * \n - Moves them to their ready queues.
 * \n - Sets their task error code to EVENT_DELETED.
 * \n - Returns EVENT_DELETED for _event_wait_all() and _event_wait_any().
 * 
 * \param[in] name_ptr Pointer to the name of the event group.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component was not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return EVENT_NOT_FOUND (Name is not in the names database.)
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)   
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.) 
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * 
 * \warning Cannot be called from an ISR
 * 
 * \see _event_create
 * \see _event_create_auto_clear
 * \see _event_create_component
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until                  
 */ 
_mqx_uint _event_destroy
(
    char _PTR_ name_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    register EVENT_COMPONENT_STRUCT_PTR event_component_ptr;
    EVENT_STRUCT_PTR                    event_ptr;
    EVENT_CONNECTION_STRUCT_PTR         event_connection_ptr;
    QUEUE_STRUCT_PTR                    queue_ptr;
    TD_STRUCT_PTR                       td_ptr;
    _mqx_uint                           result;
    _mqx_uint                           i;
    _mqx_max_type                       tmp;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_event_destroy, name_ptr);

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_event_destroy, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
    if (event_component_ptr == NULL)
    {
        _KLOGX2(KLOG_event_destroy, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_destroy, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    result = _name_find_internal(event_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);

#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (result == NAME_NOT_FOUND)
        {
            _KLOGX2(KLOG_event_destroy, EVENT_NOT_FOUND);
            return (EVENT_NOT_FOUND);
        } /* Endif */
        _KLOGX2(KLOG_event_destroy, result);
        return (result);
    } /* Endif */
#endif

    event_ptr = (EVENT_STRUCT_PTR) tmp;
    queue_ptr = &event_ptr->WAITING_TASKS;

    _int_disable();
    if (event_ptr->VALID != EVENT_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_event_destroy, EVENT_INVALID_EVENT);
        return (EVENT_INVALID_EVENT);
    } /* Endif */

    /* Effectively stop all access to the event */
    event_ptr->VALID = 0;

    /* Ready all waiting tasks */
    i = _QUEUE_GET_SIZE(queue_ptr) + 1;
    while (--i)
    {
        event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) ((pointer) queue_ptr->NEXT);
        td_ptr = event_connection_ptr->TD_PTR;
        if ((td_ptr->STATE & STATE_MASK) == EVENT_BLOCKED)
        {
            _TIME_DEQUEUE(td_ptr, kernel_data);
            _task_set_error_td_internal(td_ptr, EVENT_DELETED);
            _TASK_READY(td_ptr,kernel_data);
        } /* Endif */
    } /* Endwhile */

    _int_enable();

    result = _name_delete_internal(event_component_ptr->NAME_TABLE_HANDLE, name_ptr);
#if MQX_CHECK_ERRORS
    if (result == NAME_NOT_FOUND)
    {
        result = EVENT_NOT_FOUND;
    } /* Endif */
#endif

    _mem_free(event_ptr);

    /* May need to let higher priority task run */
    _CHECK_RUN_SCHEDULER();

    _KLOGX2(KLOG_event_destroy, result);
    return (result);

} /* Endbody */

/*!
 * \brief Creates an instance of a numbered (NOT named) event.
 * 
 * After a task creates a named event group, any task that wants to use it must 
 * open a connection to it with _event_open_fast(). When a task no longer needs a 
 * named event group, it can destroy the event group with _event_destroy_fast().
 * 
 * \param[in] event_index The event group number.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event group.)
 * \return EVENT_EXISTS (Event group was already created.)
 * \return EVENT_TABLE_FULL (Name table is full and cannot be expanded.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 *  
 * \warning Creates the event component with default values if they were not 
 * previously created.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_create_fast_auto_clear
 * \see _event_close
 * \see _event_create
 * \see _event_create_auto_clear
 * \see _event_create_component
 * \see _event_destroy_fast
 * \see _event_open_fast           
 */  
_mqx_uint _event_create_fast
(
    _mqx_uint event_index
)
{ /* Body */
    EVENT_STRUCT_PTR event_ptr;
    _mqx_uint        result;

    result = _event_create_fast_internal(event_index, &event_ptr);

#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        return (result);
    } /* Endif */
#endif

    event_ptr->AUTO_CLEAR = FALSE;

    return (result);

} /* Endbody */

/*!
 * \brief Creates an instance of a numbered (NOT named) event with autoclearing
 * event bits.
 * 
 * After a task creates a named event group, any task that wants to use it must 
 * open a connection to it with _event_open_fast(). When a task no longer needs 
 * a named event group, it can destroy the event group with _event_destroy_fast().
 * \n If a task creates an event group with autoclearing event bits, MQX clears 
 * the event bits as soon as they are set. Task that are waiting for the event 
 * bits are made ready, but need not clear the bits.
 * 
 * \param[in] event_index The event group number.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event group.)
 * \return EVENT_EXISTS (Event group was already created.)
 * \return EVENT_TABLE_FULL (Name table is full and cannot be expanded.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * 
 * \warning Creates the event component with default values if they were not 
 * previously created.
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_create_fast
 * \see _event_close
 * \see _event_create
 * \see _event_create_auto_clear
 * \see _event_create_component
 * \see _event_destroy_fast
 * \see _event_open_fast           
 */ 
_mqx_uint _event_create_fast_auto_clear
(
    _mqx_uint event_index
)
{ /* Body */
    EVENT_STRUCT_PTR event_ptr;
    _mqx_uint        result;

    result = _event_create_fast_internal(event_index, &event_ptr);

#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        return (result);
    } /* Endif */
#endif

    event_ptr->AUTO_CLEAR = TRUE;

    return (result);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Used internally to create an instance of a numbered (NOT named) event.
 * 
 * \param[in]  event_index   The event number to initialize.
 * \param[out] event_ptr_ptr Pointer to event.
 *     
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event group.)
 * \return EVENT_EXISTS (Event group was already created.)
 * \return EVENT_TABLE_FULL (Name table is full and cannot be expanded.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * 
 * \see _event_create_fast
 * \see _event_create_fast_auto_clear          
 */ 
_mqx_uint _event_create_fast_internal
(
    _mqx_uint event_index,
    EVENT_STRUCT_PTR _PTR_ event_ptr_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    register EVENT_COMPONENT_STRUCT_PTR event_component_ptr;
    register EVENT_STRUCT_PTR           event_ptr;
    _mqx_uint                           result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_event_create_fast, event_index);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_event_create_fast, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    if (event_component_ptr == NULL)
    {
        result = _event_create_component(EVENT_DEFAULT_INITIAL_NUMBER, EVENT_DEFAULT_GROW_NUMBER,
                        EVENT_DEFAULT_MAXIMUM_NUMBER);
        event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (event_component_ptr == NULL)
        {
            _KLOGX2(KLOG_event_create_fast, result);
            return (result);
        } /* Endif */
#endif
    } /* Endif */

#if MQX_CHECK_ERRORS
    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_create_fast, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    event_ptr = (EVENT_STRUCT_PTR) _mem_alloc_system_zero((_mem_size) sizeof(EVENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (event_ptr == NULL)
    {
        _KLOGX2(KLOG_event_create_fast, MQX_OUT_OF_MEMORY);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(event_ptr, MEM_TYPE_EVENT);

    _QUEUE_INIT(&event_ptr->WAITING_TASKS, EVENT_MAX_WAITING_TASKS);

    _str_mqx_uint_to_hex_string(event_index, event_ptr->NAME);

    result = _name_add_internal_by_index(event_component_ptr->NAME_TABLE_HANDLE, event_ptr->NAME,
                    (_mqx_uint) event_ptr, event_index);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        _mem_free(event_ptr);
        if (result == NAME_EXISTS)
        {
            _KLOGX2(KLOG_event_create_fast, EVENT_EXISTS);
            return (EVENT_EXISTS);
        }
        else if (result == NAME_TABLE_FULL)
        {
            _KLOGX2(KLOG_event_create_fast, EVENT_TABLE_FULL);
            return (EVENT_TABLE_FULL);
        } /* Endif */
        _KLOGX2(KLOG_event_create_fast, result);
        return (result);
    } /* Endif */
#endif

    event_ptr->VALID = EVENT_VALID;

    *event_ptr_ptr = event_ptr;

    _KLOGX2(KLOG_event_create_fast, result);
    return (result);

} /* Endbody */

/*!
 * \brief Used by a task to destroy instance of an event using an index.
 * 
 * The event group must have been created with _event_create_fast() or 
 * _event_create_fast_auto_clear().
 * \n If tasks are blocked waiting for an event bit in the event group, MQX does 
 * the following:
 * \n - Moves them to their ready queues.
 * \n - Sets their task error code to EVENT_DELETED.
 * \n - Returns EVENT_DELETED for _event_wait_all() and _event_wait_any().
 * 
 * \param[in] event_index Index of an event to destroy.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component was not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return EVENT_NOT_FOUND (Name is not in the names database.)
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)   
 * 
 * \warning Cannot be called from an ISR.
 * 
 * \see _event_create_component
 * \see _event_create_fast
 * \see _event_create_fast_auto_clear          
 */ 
_mqx_uint _event_destroy_fast
(
    _mqx_uint event_index
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    register EVENT_COMPONENT_STRUCT_PTR event_component_ptr;
    EVENT_STRUCT_PTR                    event_ptr;
    EVENT_CONNECTION_STRUCT_PTR         event_connection_ptr;
    QUEUE_STRUCT_PTR                    queue_ptr;
    TD_STRUCT_PTR                       td_ptr;
    _mqx_uint                           result;
    _mqx_uint                           i;
    _mqx_max_type                       tmp;
#if MQX_CHECK_ERRORS
    char name[9];
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_event_destroy_fast, event_index);

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_event_destroy_fast, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
    if (event_component_ptr == NULL)
    {
        _KLOGX2(KLOG_event_destroy_fast, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_destroy_fast, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    result = _name_find_internal_by_index(event_component_ptr->NAME_TABLE_HANDLE, event_index, &tmp);
    event_ptr = (EVENT_STRUCT_PTR) tmp;
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (result == NAME_NOT_FOUND)
        {
            _KLOGX2(KLOG_event_destroy_fast, EVENT_NOT_FOUND);
            return (EVENT_NOT_FOUND);
        } /* Endif */
        _KLOGX2(KLOG_event_destroy_fast, result);
        return (result);
    } /* Endif */

    /* event_ptr = (EVENT_STRUCT_PTR)tmp; */
    _str_mqx_uint_to_hex_string(event_index, name);
    if (strncmp(name, event_ptr->NAME, 8))
    {
        /* Name does not match */
        _KLOGX2(KLOG_event_destroy_fast, EVENT_NOT_FOUND);
        return (EVENT_NOT_FOUND);
    } /* Endif */
#endif

    queue_ptr = &event_ptr->WAITING_TASKS;

    _int_disable();
    if (event_ptr->VALID != EVENT_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_event_destroy_fast, EVENT_INVALID_EVENT);
        return (EVENT_INVALID_EVENT);
    } /* Endif */

    /* Effectively stop all access to the event */
    event_ptr->VALID = 0;

    /* Ready all waiting tasks */
    i = _QUEUE_GET_SIZE(queue_ptr) + 1;
    while (--i)
    {
        event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) ((pointer) queue_ptr->NEXT);
        td_ptr = event_connection_ptr->TD_PTR;
        if ((td_ptr->STATE & STATE_MASK) == EVENT_BLOCKED)
        {
            _TIME_DEQUEUE(td_ptr, kernel_data);
            _task_set_error_td_internal(td_ptr, EVENT_DELETED);
            _TASK_READY(td_ptr,kernel_data);
        } /* Endif */
    } /* Endwhile */

    _int_enable();

    result = _name_delete_internal_by_index(event_component_ptr->NAME_TABLE_HANDLE, event_index);
#if MQX_CHECK_ERRORS
    if (result == NAME_NOT_FOUND)
    {
        result = EVENT_NOT_FOUND;
    } /* Endif */
#endif

    _mem_free(event_ptr);

    /* May need to let higher priority task run */
    _CHECK_RUN_SCHEDULER();

    _KLOGX2(KLOG_event_destroy_fast, result);
    return (result);

} /* Endbody */

/*!
 * \brief Opens a connection to the indexed event group.
 * 
 * The indexed event group must have been created with _event_create_fast() or 
 * _event_create_fast_auto_clear(). Each task that needs access to the indexed 
 * event group must first open a connection to it.
 * 
 * \param[in]  event_index        Index of the event group to gain access to.
 * \param[out] returned_event_ptr Where to write the event group handle (NULL is 
 * written if an error occurred).
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event 
 * connection data.)
 * \return EVENT_INVALID_EVENT (Event group data is no longer valid.)
 * \return EVENT_NOT_FOUND (Named event group is not in the name table.)
 * 
 * \see _event_close
 * \see _event_create_fast
 * \see _event_create_fast_auto_clear
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until                    
 */ 
_mqx_uint _event_open_fast
(
    _mqx_uint event_index,
    pointer _PTR_ returned_event_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    register EVENT_COMPONENT_STRUCT_PTR  event_component_ptr;
    EVENT_STRUCT_PTR                     event_ptr;
    register EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    _mqx_uint                            result;
    _mqx_max_type                        tmp;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_event_open_fast, event_index, returned_event_ptr);

    *returned_event_ptr = NULL;

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
#if MQX_CHECK_ERRORS
    if (event_component_ptr == NULL)
    {
        _KLOGX2(KLOG_event_open_fast, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_open_fast, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    result = _name_find_internal_by_index(event_component_ptr->NAME_TABLE_HANDLE, event_index, &tmp);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (result == NAME_NOT_FOUND)
        {
            _KLOGX2(KLOG_event_open_fast, EVENT_NOT_FOUND);
            return (EVENT_NOT_FOUND);
        } /* Endif */
        _KLOGX2(KLOG_event_open_fast, result);
        return (result);
    } /* Endif */
#endif

    event_ptr = (EVENT_STRUCT_PTR) tmp;

#if MQX_CHECK_VALIDITY
    if (event_ptr->VALID != EVENT_VALID)
    {
        /* Event not valid */
        _KLOGX2(KLOG_event_open_fast, EVENT_INVALID_EVENT);
        return (EVENT_INVALID_EVENT);
    } /* Endif */
#endif

    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) _mem_alloc_zero((_mem_size) sizeof(EVENT_CONNECTION_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (event_connection_ptr == NULL)
    {
        _KLOGX2(KLOG_event_open_fast, MQX_OUT_OF_MEMORY);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(event_connection_ptr, MEM_TYPE_EVENT_CONNECTION);

    event_connection_ptr->EVENT_PTR = event_ptr;
    event_connection_ptr->VALID = EVENT_VALID;
    event_connection_ptr->TD_PTR = kernel_data->ACTIVE_PTR;

    *returned_event_ptr = (pointer) event_connection_ptr;

    _KLOGX3(KLOG_event_open_fast, MQX_OK, event_connection_ptr);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Opens a connection to the named event group.
 * 
 * The named event group must have been created with _event_create() or 
 * _event_create_auto_clear(). Each task that needs access to the named event 
 * group must first open a connection to it.
 * \n To open an event group on a remote processor, prepend the event-group name 
 * with the remote processor number as follows.
 * <table>
 *   <tr>
 *     <td><b>This string:</b></td>
 *     <td><b>Opens an event group with this name:</b></td>
 *     <td><b>On this processor:</b></td>   
 *   </tr>
 *   <tr>
 *     <td>"2:Fred"</td>
 *     <td>"Fred"</td>
 *     <td>2</td>   
 *   </tr>
 *   <tr>
 *     <td>"0:Sue"</td>
 *     <td>"Sue"</td>
 *     <td>Local processor</td>   
 *   </tr>   
 * </table> 
 * \n The other allowed event operations on remote processors are:
 * \n - _event_set()
 * \n - _event_close()
 * \n The task closes the connection with _event_close(). 
 * 
 * \param[in]  name_ptr           String name of the event.
 * \param[out] returned_event_ptr Where to write the event group handle (NULL 
 * is written if an error occurred).
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX could not allocate memory for the event 
 * connection data.)  
 * \return EVENT_INVALID_EVENT (Event group data is no longer valid.)
 * \return EVENT_NOT_FOUND (Named event group is not in the name table.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.) 
 * 
 * \see _event_close
 * \see _event_create
 * \see _event_create_auto_clear
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all 
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until          
 */ 
_mqx_uint _event_open
(
    char _PTR_    name_ptr,
    pointer _PTR_ returned_event_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    register EVENT_COMPONENT_STRUCT_PTR  event_component_ptr;
    EVENT_STRUCT_PTR                     event_ptr;
    register EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    _mqx_uint                            result;
    char _PTR_                           parse_ptr;
    uint_32                              processor;
    _mqx_max_type                        tmp;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_event_open, name_ptr, returned_event_ptr);

    *returned_event_ptr = NULL;

    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
#if MQX_CHECK_ERRORS
    if (event_component_ptr == NULL)
    {
        _KLOGX2(KLOG_event_open, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_open, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    /* If lead by a number, this is a processor number */
    processor = 0;
    parse_ptr = name_ptr;
    while (*parse_ptr && (*parse_ptr >= '0') && (*parse_ptr <= '9'))
    {
        processor = (processor * 10) + *parse_ptr - (char) '0';
        parse_ptr++;
    } /* Endwhile */
    if (processor)
    {
        /* Processor number must be followed by ':' */
        if (*parse_ptr == ':')
        {
            parse_ptr++;
        }
        else
        {
            processor = 0;
        } /* Endif */
    } /* Endif */

    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) _mem_alloc_zero((_mem_size) sizeof(EVENT_CONNECTION_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (event_connection_ptr == NULL)
    {
        _KLOGX2(KLOG_event_open, MQX_OUT_OF_MEMORY);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(event_connection_ptr, MEM_TYPE_EVENT_CONNECTION);

    if (processor != 0)
    {

#if MQX_IS_MULTI_PROCESSOR
        if (kernel_data->IPC)
        {
            /* This open is for a remote processor */
            result = (*kernel_data->IPC)(TRUE, processor, KERNEL_EVENTS,
                            IPC_EVENT_OPEN | IPC_STRING_PARAMETER, 1, parse_ptr);
            if (result != MQX_OK)
            {
                _mem_free(event_connection_ptr);
                _KLOGX2(KLOG_event_open, result);
                return(result);
            } /* Endif */
            event_connection_ptr->EVENT_PTR = (pointer)
            kernel_data->ACTIVE_PTR->INFO;
            event_connection_ptr->REMOTE_CPU = processor;
        }
        else
        {
#endif
        _mem_free(event_connection_ptr);
        _KLOGX2(KLOG_event_open, EVENT_NOT_FOUND);
        return (EVENT_NOT_FOUND);
#if MQX_IS_MULTI_PROCESSOR
    }/* Endif */
#endif

    }
    else
    {

        result = _name_find_internal(event_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);

#if MQX_CHECK_ERRORS
        if (result != MQX_OK)
        {
            _mem_free(event_connection_ptr);
            if (result == NAME_NOT_FOUND)
            {
                _KLOGX2(KLOG_event_open, EVENT_NOT_FOUND);
                return (EVENT_NOT_FOUND);
            } /* Endif */
            _KLOGX2(KLOG_event_open, result);
            return (result);
        } /* Endif */
#endif

        event_ptr = (EVENT_STRUCT_PTR) tmp;

#if MQX_CHECK_VALIDITY
        if (event_ptr->VALID != EVENT_VALID)
        {
            /* Event not valid */
            _mem_free(event_connection_ptr);
            _KLOGX2(KLOG_event_open, EVENT_INVALID_EVENT);
            return (EVENT_INVALID_EVENT);
        } /* Endif */
#endif

        event_connection_ptr->EVENT_PTR = event_ptr;

    }/* Endif */

    event_connection_ptr->VALID = EVENT_VALID;
    event_connection_ptr->TD_PTR = kernel_data->ACTIVE_PTR;
    *returned_event_ptr = (pointer) event_connection_ptr;

    _KLOGX3(KLOG_event_open, MQX_OK, event_connection_ptr);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Sets the specified event bits in the event group.
 * 
 * Before a task can set an event bit in an event group, the event group must be 
 * created and the task must open an connection to the event group.
 * \n A task can set or clear one event bit or any combination of event bits in 
 * the event group.
 * \n A task that opened an event group on a remote processor can set bits in 
 * the event group.
 * 
 * \param[in] users_event_ptr Event group handle returned by _event_open() or 
 * _event_open_fast().
 * \param[in] bit_mask        Each set bit represents an event bit to be set.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Event component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is no longer valid.)
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_INVALID_EVENT_HANDLE (Event group handle is not a valid event
 * connection.) 
 * \return EVENT_NOT_FOUND
 * \return Task error code for ?   
 * 
 * \warning Tasks waiting for the event bits might be dispatched.
 * 
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all 
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until             
 */ 
_mqx_uint _event_set
(
    pointer   users_event_ptr,
    _mqx_uint bit_mask
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    EVENT_STRUCT_PTR            event_ptr;
    EVENT_COMPONENT_STRUCT_PTR  event_component_ptr;
    EVENT_CONNECTION_STRUCT_PTR conn_ptr;
    EVENT_CONNECTION_STRUCT_PTR next_conn_ptr;
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    TD_STRUCT_PTR               new_td_ptr;
    _mqx_uint                   set_bits;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_event_set, users_event_ptr, bit_mask);

    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) users_event_ptr;

#if MQX_CHECK_VALIDITY
    if (event_connection_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_set, EVENT_INVALID_EVENT_HANDLE);
        return (EVENT_INVALID_EVENT_HANDLE);
    } /* Endif */
#endif

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];

#if MQX_CHECK_ERRORS
    if (event_component_ptr == NULL)
    {
        _KLOGX2(KLOG_event_set, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_set, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

#if MQX_IS_MULTI_PROCESSOR
    if (event_connection_ptr->REMOTE_CPU)
    {
        if (kernel_data->IPC)
        {
            /* This open is for a remote processor */
            (*kernel_data->IPC)(TRUE, event_connection_ptr->REMOTE_CPU,
                            KERNEL_EVENTS, IPC_EVENT_SET, 2, event_connection_ptr->EVENT_PTR,
                            bit_mask);
            _KLOGX2(KLOG_event_set, kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);
            return(kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);
        }
        else
        {
            _KLOGX2(KLOG_event_set, EVENT_NOT_FOUND);
            return(EVENT_NOT_FOUND);
        }/* Endif */
    }/* Endif */
#endif

    _INT_DISABLE();
    event_ptr = event_connection_ptr->EVENT_PTR;

#if MQX_CHECK_VALIDITY
    if (event_ptr->VALID != EVENT_VALID)
    {
        _INT_ENABLE();
        _KLOGX2(KLOG_event_set, EVENT_INVALID_EVENT);
        return (EVENT_INVALID_EVENT);
    } /* Endif */
#endif

    set_bits = event_ptr->EVENT | bit_mask;

    if (_QUEUE_GET_SIZE(&event_ptr->WAITING_TASKS))
    {
        /* Schedule waiting task(s) to run if bits ok */

        conn_ptr = (EVENT_CONNECTION_STRUCT_PTR) ((pointer) event_ptr->WAITING_TASKS.NEXT);
        while (conn_ptr != (EVENT_CONNECTION_STRUCT_PTR) ((pointer) &event_ptr->WAITING_TASKS))
        {
            next_conn_ptr = (EVENT_CONNECTION_STRUCT_PTR) conn_ptr->NEXT;
            if (((conn_ptr->FLAGS & EVENT_WANTS_ALL) && ((conn_ptr->MASK & set_bits) == conn_ptr->MASK))
                            || ((!(conn_ptr->FLAGS & EVENT_WANTS_ALL)) && (conn_ptr->MASK & set_bits)))
            {
                new_td_ptr = conn_ptr->TD_PTR;
                if ((new_td_ptr->STATE & STATE_MASK) == EVENT_BLOCKED)
                {
                    /* He may have timed out */
                    conn_ptr->FLAGS |= EVENT_OCCURRED;
                    _TIME_DEQUEUE(new_td_ptr, kernel_data);
                    _TASK_READY(new_td_ptr, kernel_data);
                    /* Only ready one task if event is an auto clear event */
                    if (event_ptr->AUTO_CLEAR)
                    {
                        set_bits &= ~conn_ptr->MASK;
                        break;
                    } /* Endif */
                } /* Endif */
            } /* Endif */
            conn_ptr = next_conn_ptr;
        } /* Endwhile */
    } /* Endif */

    event_ptr->EVENT = set_bits;

    _INT_ENABLE();

    /* May need to let higher priority task run */
    _CHECK_RUN_SCHEDULER();

    _KLOGX2(KLOG_event_set, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Used during task destruction to free up any events resources owned by 
 * this task.
 * 
 * The function checks the resource list of the task and look for an event 
 * connection struct. It can detect one by looking for EVENT_VALID in the 
 * correct location.
 * 
 * \param[in] td_ptr Pointer to the task descriptor of a task to destroy.   
 */ 
void _event_cleanup
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    EVENT_CONNECTION_STRUCT_PTR connection_ptr;
    EVENT_STRUCT_PTR            event_ptr;

    connection_ptr = _mem_get_next_block_internal(td_ptr, NULL);
    while (connection_ptr)
    {
        if ((connection_ptr->VALID == EVENT_VALID) && (connection_ptr->TD_PTR == td_ptr))
        {
            event_ptr = connection_ptr->EVENT_PTR;
            if (event_ptr->VALID == EVENT_VALID)
            {
                /* Check if the connection is on the queue */
                _int_disable();
                event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) ((pointer) event_ptr->WAITING_TASKS.NEXT);
                while (event_connection_ptr != (pointer) &event_ptr->WAITING_TASKS.NEXT)
                {
                    if (event_connection_ptr == connection_ptr)
                    {
                        /* Connection is queued, so dequeue it */
                        _QUEUE_REMOVE(&event_ptr->WAITING_TASKS, connection_ptr);
                        break;
                    }/* Endif */
                    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) event_connection_ptr->NEXT;
                } /* Endwhile */
                _int_enable();
            }/* Endif */
        } /* Endif */
        connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) _mem_get_next_block_internal(td_ptr, connection_ptr);
    } /* Endwhile */

} /* Endbody */

/*!
 * \brief Tests the event component for validity and consistency.
 * 
 * \param[out] event_error_ptr Handle for the event group that has an error if 
 * MQX found an error in the event component (NULL if no error is found).
 * 
 * \return MQX_OK
 * \return EVENT_INVALID_EVENT (Data for an event group is not valid.)
 * \return MQX_INVALID_COMPONENT_BASE (Event component data is not valid.)
 * \return Return code from _queue_test() (Waiting queue for an event group has 
 * an error.)
 * 
 * \see _event_close
 * \see _event_open
 * \see _event_set
 * \see _event_get_wait_count
 * \see _event_get_value
 * \see _event_wait_all 
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until            
 */ 
_mqx_uint _event_test
(
    pointer _PTR_ event_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    EVENT_COMPONENT_STRUCT_PTR event_component_ptr;
    EVENT_STRUCT_PTR           event_ptr;
    pointer                    table_ptr;
    _mqx_uint                  result;
    _mqx_uint                  i;
    _mqx_max_type              data;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_event_test, event_error_ptr);

    *event_error_ptr = NULL;

    event_component_ptr = (EVENT_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    if (event_component_ptr == NULL)
    {
        _KLOGX2(KLOG_event_test, MQX_OK);
        return (MQX_OK);
    } /* Endif */

    if (event_component_ptr->VALID != EVENT_VALID)
    {
        _KLOGX2(KLOG_event_test, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    _int_disable();
    /* Test the event name table */
    result = _name_test_internal((NAME_COMPONENT_STRUCT_PTR) event_component_ptr->NAME_TABLE_HANDLE,
                    (pointer _PTR_) &table_ptr, (pointer _PTR_) event_error_ptr);
    _int_enable();
    if (result != MQX_OK)
    {
        _KLOGX3(KLOG_event_test, MQX_INVALID_COMPONENT_BASE, *event_error_ptr);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    i = 0;
    while (TRUE)
    {
        _int_disable();
        result = _name_find_internal_by_index(event_component_ptr->NAME_TABLE_HANDLE, i++, &data);
        if (result != MQX_OK)
        {
            result = MQX_OK;
            break;
        } /* Endif */

        event_ptr = (EVENT_STRUCT_PTR) data;
        if (event_ptr->VALID != EVENT_VALID)
        {
            result = EVENT_INVALID_EVENT;
            break;
        } /* Endif */
        result = _queue_test(&event_ptr->WAITING_TASKS, event_error_ptr);
        if (result != MQX_OK)
        {
            break;
        } /* Endif */
        _int_enable();

    } /* Endwhile */

    _int_enable();

    _KLOGX3(KLOG_event_test, result, *event_error_ptr);
    return (result);

} /* Endbody */

/*!
 * \brief Gets the current event bits that have been set.
 * 
 * \param[in] users_event_ptr Event group handle returned by _event_open() or 
 * _event_open_fast().
 * \param[out] event_value_ptr Where to write the value of the event bits (on error, 
 * 0 is written). 
 * 
 * \return MQX_OK
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_INVALID_EVENT_HANDLE (Event group handle is not valid.)
 * 
 * \see _event_clear
 * \see _event_set
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until       
 */ 
_mqx_uint _event_get_value
(
    pointer users_event_ptr,
    _mqx_uint_ptr event_value_ptr
)
{ /* Body */
    register EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    register EVENT_STRUCT_PTR            event_ptr;

    *event_value_ptr = 0;
    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) users_event_ptr;
#if MQX_CHECK_VALIDITY
    if (event_connection_ptr->VALID != EVENT_VALID)
    {
        return (EVENT_INVALID_EVENT_HANDLE);
    } /* Endif */
#endif

    event_ptr = event_connection_ptr->EVENT_PTR;
#if MQX_CHECK_VALIDITY
    if (event_ptr->VALID != EVENT_VALID)
    {
        return (EVENT_INVALID_EVENT);
    } /* Endif */
#endif

    *event_value_ptr = event_ptr->EVENT;

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Gets the number of tasks that are waiting for event bits in the event 
 * group.
 * 
 * Tasks can be waiting for different combinations of event bits.
 * 
 * \param[in] users_event_ptr Event group handle returned by _event_open() or 
 * _event_open_fast().
 * 
 * \return Number of waiting tasks. (Success.)
 * \return MAX_MQX_UINT (Failure.)
 * \return MAX_UINT_32 (Failure.)
 * 
 * \warning On failure, calls _task_set_error() to set the task error code to 
 * EVENT_INVALID_EVENT_HANDLE.
 * 
 * \see _event_open
 * \see _event_open_fast
 * \see _event_wait_all_for
 * \see _event_wait_all_ticks
 * \see _event_wait_all_until
 * \see _event_wait_any
 * \see _event_wait_any_for
 * \see _event_wait_any_ticks
 * \see _event_wait_any_until 
 * \see _task_set_error        
 */ 
_mqx_uint _event_get_wait_count
(
    pointer users_event_ptr
)
{ /* Body */
    register EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    register EVENT_STRUCT_PTR            event_ptr;

    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) users_event_ptr;
#if MQX_CHECK_VALIDITY
    if (event_connection_ptr->VALID != EVENT_VALID)
    {
        _task_set_error(EVENT_INVALID_EVENT_HANDLE);
        return ((_mqx_uint) MAX_UINT_32);
    } /* Endif */
#endif

    event_ptr = event_connection_ptr->EVENT_PTR;
#if MQX_CHECK_VALIDITY
    if (event_ptr->VALID != EVENT_VALID)
    {
        _task_set_error(EVENT_INVALID_EVENT_HANDLE);
        return (MAX_MQX_UINT);
    } /* Endif */
#endif

    return (_QUEUE_GET_SIZE(&(event_ptr->WAITING_TASKS)));

} /* Endbody */

/*!
 * \private
 * 
 * \brief Used internally by a task to wait for all or any specified events
 * according to bit mask
 * 
 * \param[in] users_event_ptr    Event group handle returned by _event_open() or 
 * _event_open_fast().
 * \param[in] bit_mask           Each set bit represents an event bit to wait for.
 * \param[in] tick_ptr           The maximum number of ticks to wait for the 
 * events to be set. If the value is NULL, then the timeout will be infinite.
 * \param[in] ticks_are_absolute TRUE (ticks represents absolute time), FALSE 
 * (ticks represents relative time).
 * \param[in] all                Specifies whether wait for all bits.
 *  
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return EVENT_INVALID_EVENT_HANDLE (Handle is not a valid event group handle.)
 * \return EVENT_CANNOT_WAIT_ON_REMOTE_EVENT 
 * \return EVENT_INVALID_EVENT (Event group is no longer valid.)
 * \return EVENT_DELETED (Event group was destroyed while the task waited.)
 * \return EVENT_WAIT_TIMEOUT (Timeout expired before the event bits were set.)   
 */ 
_mqx_uint _event_wait_internal
(
    pointer             users_event_ptr,
    _mqx_uint           bit_mask,
    MQX_TICK_STRUCT_PTR tick_ptr,
    boolean             ticks_are_absolute,
    boolean             all
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    TD_STRUCT_PTR                        td_ptr;
    register EVENT_STRUCT_PTR            event_ptr;
    register EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR) users_event_ptr;

#if MQX_CHECK_VALIDITY
    if (event_connection_ptr->VALID != EVENT_VALID)
    {
        return (EVENT_INVALID_EVENT_HANDLE);
    } /* Endif */
#endif

    if (event_connection_ptr->REMOTE_CPU)
    {
        _KLOGX2(KLOG_event_wait_all, EVENT_CANNOT_WAIT_ON_REMOTE_EVENT);
        return (EVENT_CANNOT_WAIT_ON_REMOTE_EVENT);
    }/* Endif */

    td_ptr = kernel_data->ACTIVE_PTR;

#if MQX_CHECK_ERRORS
    if (event_connection_ptr->TD_PTR != td_ptr)
    {
        return (EVENT_INVALID_EVENT_HANDLE);
    } /* Endif */
#endif

    _INT_DISABLE();
    event_ptr = event_connection_ptr->EVENT_PTR;

#if MQX_CHECK_VALIDITY
    if (event_ptr->VALID != EVENT_VALID)
    {
        _int_enable();
        return (EVENT_INVALID_EVENT);
    } /* Endif */
#endif

    if ((all && ((event_ptr->EVENT & bit_mask) != bit_mask)) || (!all && ((event_ptr->EVENT & bit_mask) == 0)))
    {
        /* Must wait for a event to become available */
        if (all)
        {
            event_connection_ptr->FLAGS = EVENT_WANTS_ALL;
        }
        else
        {
            event_connection_ptr->FLAGS = 0;
        } /* Endif */

        event_connection_ptr->MASK = bit_mask;

        /* Enqueue at end */
        _QUEUE_ENQUEUE(&event_ptr->WAITING_TASKS, event_connection_ptr);

        /* Now put the task to sleep */
        td_ptr->STATE = EVENT_BLOCKED;

        if (tick_ptr)
        {
            if (ticks_are_absolute)
            {
                _time_delay_until(tick_ptr);
            }
            else
            {
                _time_delay_for(tick_ptr);
            } /* Endif */
        }
        else
        {
            _task_block();
        } /* Endif */

#if MQX_COMPONENT_DESTRUCTION
        if (event_ptr->VALID == 0)
        { /* We've been deleted */
            _int_enable();
            return (EVENT_DELETED);
        } /* Endif */
#endif

        _QUEUE_REMOVE(&event_ptr->WAITING_TASKS, event_connection_ptr);

        if (tick_ptr && !(event_connection_ptr->FLAGS & EVENT_OCCURRED))
        {
            /* MUST have timed out */
            _INT_ENABLE();
            return (EVENT_WAIT_TIMEOUT);
        } /* Endif */
    } /* Endif */

    if (event_ptr->AUTO_CLEAR)
    {
        event_ptr->EVENT = event_ptr->EVENT & ~bit_mask;
    } /* Endif */

    /* GOT IT */
    _INT_ENABLE();

    return (MQX_OK);

} /* Endbody */

#endif /* MQX_USE_EVENTS */

/* EOF */
