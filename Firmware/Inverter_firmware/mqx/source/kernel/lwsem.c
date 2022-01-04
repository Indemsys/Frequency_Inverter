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
 * $FileName: lwsem.c$
 * $Version : 3.8.0.1$
 * $Date    : Feb-20-2012$
 *
 * Comments:
 *
 *   This file contains functions of the lightweight semaphore component.
 *
 *END************************************************************************/

#include "mqx_inc.h"

/*!
 * \private
 *
 * \brief Creates the lightweight semaphore.
 *
 * \param[in] sem_ptr        Pointer to the lightweight semaphore to create.
 * \param[in] initial_number Inital number of semaphores available.
 * \param[in] hidden         TRUE if semaphore will ne hidden from kernel, FALSE if not.
 * \param[in] user           User mode functionality.
 *
 * \return MQX_OK
 * \return MQX_EINVAL
 * \return MQX_INVALID_LWSEM
 *
 * \see _lwsem_create
 * \see _lwsem_create_hidden
 * \see _usr_lwsem_create
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_create_internal
(
    LWSEM_STRUCT_PTR sem_ptr,
    _mqx_int         initial_number,
    boolean          hidden,
    boolean          user
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWSEM_STRUCT_PTR       sem_chk_ptr;

#if MQX_ENABLE_USER_MODE
    if (user && !_psp_mem_check_access_mask((uint_32)sem_ptr, sizeof(LWSEM_STRUCT), MPU_UM_R, MPU_UM_RW))
    {
        return MQX_INVALID_LWSEM;
    }
#endif /* MQX_ENABLE_USER_MODE */

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_lwsem_create, initial_number);

    sem_ptr->VALUE = initial_number;
    _QUEUE_INIT(&sem_ptr->TD_QUEUE, 0);
    _int_disable();

    if (!hidden)
    {
#if MQX_CHECK_ERRORS
        /* Check if lwsem is already initialized */
#if MQX_ENABLE_USER_MODE
        if (user)
        {
            sem_chk_ptr = (LWSEM_STRUCT_PTR)((pointer)kernel_data->USR_LWSEM.NEXT);
            while (sem_chk_ptr != (LWSEM_STRUCT_PTR)((pointer)&kernel_data->USR_LWSEM))
            {
                if (sem_chk_ptr == sem_ptr)
                {
                    _int_enable();
                    _KLOGX2(KLOG_lwsem_create, MQX_EINVAL);
                    return(MQX_EINVAL);
                }

                sem_chk_ptr = (LWSEM_STRUCT_PTR)((pointer)sem_chk_ptr->NEXT);
            }
        }
        else
#endif /* MQX_ENABLE_USER_MODE */
        {
            sem_chk_ptr = (LWSEM_STRUCT_PTR) ((pointer) kernel_data->LWSEM.NEXT);
            while (sem_chk_ptr != (LWSEM_STRUCT_PTR) ((pointer) &kernel_data->LWSEM))
            {
                if (sem_chk_ptr == sem_ptr)
                {
                    _int_enable();
                    _KLOGX2(KLOG_lwsem_create, MQX_EINVAL);
                    return (MQX_EINVAL);
                }

                sem_chk_ptr = (LWSEM_STRUCT_PTR) ((pointer) sem_chk_ptr->NEXT);
            }
        }
#endif /* MQX_CHECK_ERRORS */

#if MQX_ENABLE_USER_MODE
        if (user)
        {
            _QUEUE_ENQUEUE(&kernel_data->USR_LWSEM, sem_ptr);
        }
        else
#endif /* MQX_ENABLE_USER_MODE */
        {
            _QUEUE_ENQUEUE(&kernel_data->LWSEM, sem_ptr);
        }
    }

    sem_ptr->VALID = LWSEM_VALID;
    _int_enable();

    _KLOGX2(KLOG_lwsem_create, MQX_OK);

    return (MQX_OK);
}

/*!
 * \brief Creates the lightweight semaphore.
 *
 * Because lightweight semaphores are a core component, an application need not to
 * create the component before it creates lightweight semaphores.
 *
 * \param[in] sem_ptr        Pointer to the lightweight semaphore to create.
 * \param[in] initial_number Inital number of semaphores available.
 *
 * \return MQX_OK
 * \return MQX_EINVAL
 * \return MQX_INVALID_LWSEM
 *
 * \see _lwsem_create_hidden
 * \see _lwsem_destroy
 * \see _lwsem_post
 * \see _lwsem_test
 * \see _lwsem_wait
 * \see _lwsem_wait_for
 * \see _lwsem_wait_ticks
 * \see _lwsem_wait_until
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_create
(
    LWSEM_STRUCT_PTR sem_ptr,
    _mqx_int         initial_number
)
{
#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_create(sem_ptr, initial_number);
    }
#endif

    return _lwsem_create_internal(sem_ptr, initial_number, FALSE, FALSE);
}

/*!
 * \brief Creates the lightweight semaphore hidden from kernel.
 *
 * \param[in] sem_ptr        Pointer to the lightweight semaphore to create.
 * \param[in] initial_number Inital number of semaphores available.
 *
 * \return MQX_OK
 * \return MQX_EINVAL
 * \return MQX_INVALID_LWSEM
 *
 * \see _lwsem_create
 * \see _lwsem_destroy
 * \see _lwsem_post
 * \see _lwsem_test
 * \see _lwsem_wait
 * \see _lwsem_wait_for
 * \see _lwsem_wait_ticks
 * \see _lwsem_wait_until
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_create_hidden
(
    LWSEM_STRUCT_PTR sem_ptr,
    _mqx_int         initial_number
)
{
    return _lwsem_create_internal(sem_ptr, initial_number, TRUE, FALSE);
}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Creates the lightweight semaphore hidden form kernel.
 *
 * This function is an equivalent to the _lwsem_create() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr        Read only. Pointer to the lightweight semaphore to create.
 * \param[in] initial_number Inital number of semaphores available.
 *
 * \return MQX_OK
 * \return MQX_EINVAL
 * \return MQX_INVALID_LWSEM
 *
 * \see _lwsem_create
 * \see _usr_lwsem_destroy
 * \see _usr_lwsem_post
 * \see _usr_lwsem_wait
 * \see _usr_lwsem_wait_for
 * \see _usr_lwsem_wait_ticks
 * \see _usr_lwsem_wait_until
 * \see LWSEM_STRUCT
 */
_mqx_uint _usr_lwsem_create
(
    LWSEM_STRUCT_PTR sem_ptr,
    _mqx_int         initial_number
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)sem_ptr, (uint_32)initial_number, (uint_32)FALSE, 0, 0};
    return _mqx_api_call(MQX_API_LWSEM_CREATE, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \private
 *
 * \brief Destroys the lightweight semaphore.
 *
 * \param[in] sem_ptr Pointer to the created lightweight semaphore.
 * \param[in] user    User mode functionality.
 *
 * \return MQX_OK
 * \return MQX_INVALID_LWSEM (Lwsem_ptr does not point to a valid lightweight semaphore.)
 *
 * \see _lwsem_destroy
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_destroy_internal
(
    LWSEM_STRUCT_PTR sem_ptr,
    boolean          user
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;
    LWSEM_STRUCT_PTR       sem_chk_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_lwsem_destroy, sem_ptr);

    _int_disable();
#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_lwsem_destroy, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */
#endif

    sem_ptr->VALID = 0; /* Invalidate the semaphore */
    while (_QUEUE_GET_SIZE(&sem_ptr->TD_QUEUE))
    {
        _QUEUE_DEQUEUE(&sem_ptr->TD_QUEUE, td_ptr);
        _BACKUP_POINTER(td_ptr, TD_STRUCT, AUX_QUEUE);
        _TIME_DEQUEUE(td_ptr, kernel_data);
        _TASK_READY(td_ptr, kernel_data);
    } /* Endwhile */

    /* Check if lwsem is in kernel's semaphore list */
#if MQX_ENABLE_USER_MODE
    if (user)
    {
        sem_chk_ptr = (LWSEM_STRUCT_PTR)((pointer)kernel_data->USR_LWSEM.NEXT);
        while (sem_chk_ptr != (LWSEM_STRUCT_PTR)((pointer)&kernel_data->USR_LWSEM))
        {
            if (sem_chk_ptr == sem_ptr)
            {
                /* remove semaphore from kernel LWSEM queue */
                _QUEUE_REMOVE(&kernel_data->USR_LWSEM, sem_ptr);
                break;
            }
            sem_chk_ptr = (LWSEM_STRUCT_PTR)((pointer)sem_chk_ptr->NEXT);
        }
    }
    else
#endif /* MQX_ENABLE_USER_MODE */
    {
        sem_chk_ptr = (LWSEM_STRUCT_PTR) ((pointer) kernel_data->LWSEM.NEXT);
        while (sem_chk_ptr != (LWSEM_STRUCT_PTR) ((pointer) &kernel_data->LWSEM))
        {
            if (sem_chk_ptr == sem_ptr)
            {
                /* remove semaphore from kernel LWSEM queue */
                _QUEUE_REMOVE(&kernel_data->LWSEM, sem_ptr);
                break;
            } /* Endif */
            sem_chk_ptr = (LWSEM_STRUCT_PTR) ((pointer) sem_chk_ptr->NEXT);
        } /* Endwhile */
    }

    _int_enable();

    _CHECK_RUN_SCHEDULER(); /* Allow higher priority tasks to run */


    _KLOGX2(KLOG_lwsem_destroy, MQX_OK);
    return (MQX_OK);

} /* Endbody */


/*!
 * \brief Destroys the lightweight semaphore.
 *
 * \param[in] sem_ptr Pointer to the created lightweight semaphore.
 *
 * \return MQX_OK
 * \return MQX_INVALID_LWSEM (Lwsem_ptr does not point to a valid lightweight semaphore.)
 *
 * \warning Puts all waiting tasks in their ready queues.
 * \warning Cannot be called from an ISR.
 *
 * \see _lwsem_create
 * \see _lwsem_create_hidden
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_destroy
(
    LWSEM_STRUCT_PTR sem_ptr
)
{
#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_destroy(sem_ptr);
    }
#endif

    return _lwsem_destroy_internal(sem_ptr, FALSE);
}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Destroys the lightweight semaphore.
 *
 * This function is an equivalent to the _lwsem_destroy() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr Read only. Pointer to the created lightweight semaphore.
 *
 * \return MQX_OK
 * \return MQX_INVALID_LWSEM (Lwsem_ptr does not point to a valid lightweight semaphore.)
 *
 * \warning Puts all waiting tasks in their ready queues.
 * \warning Cannot be called from an ISR.
 *
 * \see _lwsem_destroy
 * \see _usr_lwsem_create
 * \see LWSEM_STRUCT
 */
_mqx_uint _usr_lwsem_destroy
(
    LWSEM_STRUCT_PTR sem_ptr
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)sem_ptr, 0, 0, 0, 0};
    return _mqx_api_call(MQX_API_LWSEM_DESTROY, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Poll for the lightweight semaphore.
 *
 * This function is the nonblocking alternative to the _lwsem_wait family of functions.
 *
 * \param[in] sem_ptr Pointer to the created lightweight semaphore.
 *
 * \return TRUE (Task got the lightweight semaphore.)
 * \return FALSE (Lightweight semaphore was not available.)
 *
 * \see _lwsem_create
 * \see _lwsem_create_hidden
 * \see _lwsem_wait
 * \see _lwsem_wait_for
 * \see _lwsem_wait_ticks
 * \see _lwsem_wait_until
 * \see LWSEM_STRUCT
 */
boolean _lwsem_poll
(
    LWSEM_STRUCT_PTR sem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    boolean                result;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_poll(sem_ptr);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwsem_poll, sem_ptr);

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _KLOGX2(KLOG_lwsem_poll, FALSE);
        return (FALSE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    _INT_DISABLE();
    if (sem_ptr->VALUE <= 0)
    {
        result = FALSE;
    }
    else
    {
        --sem_ptr->VALUE;
        result = TRUE;
    } /* Endif */
    _INT_ENABLE();

    _KLOGX2(KLOG_lwsem_poll, result);

    return (result);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Poll for the lightweight semaphore.
 *
 * This function is an equivalent to the _lwsem_poll() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr Read only. Pointer to the created lightweight semaphore.
 *
 * \return TRUE (Task got the lightweight semaphore.)
 * \return FALSE (Lightweight semaphore was not available.)
 *
 * \see _lwsem_poll
 * \see _usr_lwsem_create
 * \see _usr_lwsem_wait
 * \see _usr_lwsem_wait_for
 * \see _usr_lwsem_wait_ticks
 * \see _usr_lwsem_wait_until
 * \see LWSEM_STRUCT
 */
boolean _usr_lwsem_poll
(
    LWSEM_STRUCT_PTR sem_ptr
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)sem_ptr, 0, 0, 0, 0};
    
    return (boolean)_mqx_api_call(MQX_API_LWSEM_POLL, &params);
}
#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Posts the lightweight semaphore.
 *
 * If tasks are waiting for the lightweight semaphore, MQX removes the first one
 * from the queue and puts it in the task's ready queue.
 *
 * \param[in] sem_ptr Pointer to the created lightweight semaphore.
 *
 * \return MQX_OK
 * \return MQX_INVALID_LWSEM (Lwsem_ptr does not point to a valid lightweight semaphore.)
 *
 * \warning Might put a waiting task in the task's ready queue.
 *
 * \see _lwsem_create
 * \see _lwsem_wait
 * \see _lwsem_wait_for
 * \see _lwsem_wait_ticks
 * \see _lwsem_wait_until
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_post
(
    LWSEM_STRUCT_PTR sem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_post(sem_ptr);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_lwsem_post, sem_ptr);

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _KLOGX2(KLOG_lwsem_post, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    _INT_DISABLE();
    if ((sem_ptr->VALUE >= 0) && (_QUEUE_GET_SIZE(&sem_ptr->TD_QUEUE)))
    {
        _QUEUE_DEQUEUE(&sem_ptr->TD_QUEUE, td_ptr);
        _BACKUP_POINTER(td_ptr, TD_STRUCT, AUX_QUEUE);
        _TIME_DEQUEUE(td_ptr, kernel_data);
        td_ptr->INFO = 0; /* Signal that post is activating the task */
        _TASK_READY(td_ptr, kernel_data);
        _CHECK_RUN_SCHEDULER(); /* Let higher priority task run */
    }
    else
    {
        ++sem_ptr->VALUE;
    } /* Endif */
    _INT_ENABLE();

    _KLOGX2(KLOG_lwsem_post, MQX_OK);

    return (MQX_OK);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Posts the lightweight semaphore.
 *
 * This function is an equivalent to the _lwsem_post() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr Read only. Pointer to the created lightweight semaphore.
 *
 * \return MQX_OK
 * \return MQX_INVALID_LWSEM (Lwsem_ptr does not point to a valid lightweight semaphore.)
 *
 * \warning Might put a waiting task in the task's ready queue.
 *
 * \see _lwsem_post
 * \see _usr_lwsem_create
 * \see _usr_lwsem_wait
 * \see _usr_lwsem_wait_for
 * \see _usr_lwsem_wait_ticks
 * \see _usr_lwsem_wait_until
 * \see LWSEM_STRUCT
 */
_mqx_uint _usr_lwsem_post
(
    LWSEM_STRUCT_PTR sem_ptr
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)sem_ptr, 0, 0, 0, 0};
    return _mqx_api_call(MQX_API_LWSEM_POST, &params);
}
#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Tests the data structures (including queues) of the lightweight semaphores
 * component for consistency and validity.
 *
 * \param[out] lwsem_error_ptr Pointer to the lightweight semaphore in error (NULL
 * if no error is found)
 * \param[out] td_error_ptr    Pointer to the task descriptor of waiting task that
 * has an error (NULL if no error is found).
 *
 * \return MQX_OK
 * \return MQX_INVALID_LWSEM (Results of _queue_test().)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 *
 * \warning Cannot be called from an ISR.
 * Disables and enables interrupts.
 *
 * \see _lwsem_create
 * \see _lwsem_destroy
 * \see _queue_test
 */
_mqx_uint _lwsem_test
(
    pointer _PTR_ lwsem_error_ptr,
    pointer _PTR_ td_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWSEM_STRUCT_PTR       sem_ptr;
    _mqx_uint              queue_size;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_lwsem_test, lwsem_error_ptr, td_error_ptr);

    *td_error_ptr = NULL;
    *lwsem_error_ptr = NULL;

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_lwsem_test, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    }/* Endif */
#endif

    _int_disable();

    result = _queue_test((QUEUE_STRUCT_PTR) &kernel_data->LWSEM, lwsem_error_ptr);
    if (result != MQX_OK)
    {
        _KLOGX3(KLOG_lwsem_test, result, *lwsem_error_ptr);
        return (result);
    } /* Endif */

    sem_ptr = (LWSEM_STRUCT_PTR) ((pointer) kernel_data->LWSEM.NEXT);
    queue_size = _QUEUE_GET_SIZE(&kernel_data->LWSEM);
    while (queue_size--)
    {
        if (sem_ptr->VALID != LWSEM_VALID)
        {
            result = MQX_INVALID_LWSEM;
            break;
        } /* Endif */

        result = _queue_test(&sem_ptr->TD_QUEUE, td_error_ptr);
        if (result != MQX_OK)
        {
            break;
        } /* Endif */

        sem_ptr = sem_ptr->NEXT;
    } /* Endwhile */

    _int_enable();

    if (result != MQX_OK)
    {
        *lwsem_error_ptr = (pointer) sem_ptr;
    } /* Endif */
    _KLOGX4(KLOG_lwsem_test, result, *lwsem_error_ptr, *td_error_ptr);

    return (result);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Checks whether the specified pointer corresponds with a valid lightweight
 * semaphore.
 *
 * \param[in] tst_sem_ptr Pointer to the lightweight semaphore.
 *
 * \return MQX_OK (Valid lightweight semaphore.)
 * \return MQX_INVALID_LWSEM (Specified lightweight semaphore is not valid.)
 *
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_usr_check
(
    LWSEM_STRUCT_PTR tst_sem_ptr
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWSEM_STRUCT_PTR       sem_ptr;
    _mqx_uint              result = MQX_INVALID_LWSEM;
    _mqx_uint              queue_size;

    _GET_KERNEL_DATA(kernel_data);

    sem_ptr = (LWSEM_STRUCT_PTR)((pointer)kernel_data->USR_LWSEM.NEXT);
    queue_size = _QUEUE_GET_SIZE(&kernel_data->USR_LWSEM);

    while (queue_size--)
    {
        if (sem_ptr->VALID != LWSEM_VALID)
        {
            break;
        }

        if (tst_sem_ptr == sem_ptr)
        {
            result = MQX_OK;
            break;
        }

        sem_ptr = (LWSEM_STRUCT_PTR)(pointer)sem_ptr->NEXT;
    }

    return result;
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore until it is available.
 *
 * \note Because priority inversion might occur if tasks with different priorities
 * access the same lightweight semaphore, we recommend under these circumstances
 * that you use the semaphore component.
 *
 * \param[in] sem_ptr Pointer to the lightweight semaphore.
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_create
 * \see _lwsem_post
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_wait
(
    LWSEM_STRUCT_PTR sem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR td_ptr;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_wait(sem_ptr);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_lwsem_wait, sem_ptr);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_lwsem_wait, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    }
#endif

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _KLOGX2(KLOG_lwsem_wait, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    }
#endif

    _INT_DISABLE();
    if (sem_ptr->VALUE <= 0)
    {
        td_ptr = kernel_data->ACTIVE_PTR;
        td_ptr->STATE = LWSEM_BLOCKED;
        td_ptr->INFO = (_mqx_uint) &sem_ptr->TD_QUEUE;
        _QUEUE_UNLINK(td_ptr);
        _QUEUE_ENQUEUE(&sem_ptr->TD_QUEUE, &td_ptr->AUX_QUEUE);
        _sched_execute_scheduler_internal(); /* Let the other tasks run */
        /* Another task has posted a semaphore, and it has been tranfered to this
         ** task.
         */
    }
    else
    {
        --sem_ptr->VALUE;
    }

    /* We must check for component destruction */
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _int_enable();
        /* The semaphore has been deleted */
        _KLOGX2(KLOG_lwsem_wait, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */

    _INT_ENABLE();

    _KLOGX2(KLOG_lwsem_wait, MQX_OK);

    return (MQX_OK);
}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore until it is available.
 *
 * This function is an equivalent to the _lwsem_wait() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr Read only. Pointer to the lightweight semaphore.
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_wait
 * \see _usr_lwsem_create
 * \see _usr_lwsem_post
 * \see LWSEM_STRUCT
 */
_mqx_uint _usr_lwsem_wait
(
    LWSEM_STRUCT_PTR sem_ptr
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)sem_ptr, 0, 0, 0, 0};
    return _mqx_api_call(MQX_API_LWSEM_WAIT, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \private
 *
 * \brief This function is an internal funciton, it waits for a light weight
 * semaphore with a pre-calcualted timeout.
 *
 * \param[in] sem_ptr Pointer to the lightweight semaphore.
 * \param[in] td_ptr  Pointer to the task descriptor waiting.
 *
 * \return MQX_OK
 * \return MQX_LWSEM_WAIT_TIMEOUT
 *
 * \see _lwsem_wait
 * \see _lwsem_wait_for
 * \see _lwsem_wait_ticks
 * \see _lwsem_wait_until
 * \see _usr_lwsem_wait
 * \see _usr_lwsem_wait_for
 * \see _usr_lwsem_wait_ticks
 * \see _usr_lwsem_wait_until
 * \see LWSEM_STRUCT
 * \see TD_STRUCT
 */
_mqx_uint _lwsem_wait_timed_internal
(
    LWSEM_STRUCT_PTR sem_ptr,
    TD_STRUCT_PTR    td_ptr
)
{ /* Body */

    td_ptr->STATE = LWSEM_BLOCKED;
    td_ptr->INFO  = (_mqx_uint) &sem_ptr->TD_QUEUE;
    _QUEUE_UNLINK(td_ptr);
    _QUEUE_ENQUEUE(&sem_ptr->TD_QUEUE, &td_ptr->AUX_QUEUE);
    _time_delay_internal(td_ptr);
    if (td_ptr->INFO != 0)
    {
        /*_QUEUE_REMOVE(&sem_ptr->TD_QUEUE, &td_ptr->AUX_QUEUE);*/
        return (MQX_LWSEM_WAIT_TIMEOUT);
    } /* Endif */

    return (MQX_OK);

} /* Endbody */

#if MQX_HAS_TICK

/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore for the number of
 * ticks (in tick time).
 *
 * \note Because priority inversion might occur if tasks with different priorities access
 * the same lightweight semaphore, we recommend under these circumstances that you
 * use the semaphore component.
 *
 * \param[in] sem_ptr Pointer to the lightweight semaphore.
 * \param[in] ticks   Pointer to the maximum number of ticks to wait or NULL
 * (unlimited wait).
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_create
 * \see _lwsem_post
 * \see LWSEM_STRUCT
 * \see MQX_TICK_STRUCT
 */
_mqx_uint _lwsem_wait_for
(
    LWSEM_STRUCT_PTR    sem_ptr,
    MQX_TICK_STRUCT_PTR ticks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;
    _mqx_uint              result;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_wait_for(sem_ptr, ticks);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_lwsem_wait_for, sem_ptr, ticks);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_lwsem_wait_for, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _KLOGX2(KLOG_lwsem_wait_for, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */
#endif

    _INT_DISABLE();
    if (sem_ptr->VALUE <= 0)
    {
        td_ptr = kernel_data->ACTIVE_PTR;
        /* Calculate time to wake up the task */
        PSP_ADD_TICKS(ticks, &kernel_data->TIME, &td_ptr->TIMEOUT);
        result = _lwsem_wait_timed_internal(sem_ptr, td_ptr);
    }
    else
    {
        --sem_ptr->VALUE;
        result = MQX_OK;
    } /* Endif */

    /* We must check for component destruction */
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _int_enable();
        /* The semaphore has been deleted */
        _KLOGX2(KLOG_lwsem_wait_for, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */

    _INT_ENABLE();

    _KLOGX2(KLOG_lwsem_wait_for, result);

    return (result);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore for the number of
 * ticks (in tick time).
 *
 * This function is an equivalent to the _lwsem_wait_for() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr Read only. Pointer to the lightweight semaphore.
 * \param[in] ticks   Read/write. Pointer to the maximum number of ticks to wait
 * or NULL (unlimited wait).
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_wait_for
 * \see _usr_lwsem_create
 * \see _usr_lwsem_post
 * \see LWSEM_STRUCT
 * \see MQX_TICK_STRUCT
 */
_mqx_uint _usr_lwsem_wait_for
(
    LWSEM_STRUCT_PTR    sem_ptr,
    MQX_TICK_STRUCT_PTR ticks
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)sem_ptr, (uint_32)ticks, 0, 0, 0};
    return _mqx_api_call(MQX_API_LWSEM_WAIT_FOR, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore for the number of ticks.
 *
 * \note Because priority inversion might occur if tasks with different priorities
 * access the same lightweight semaphore, we recommend under these circumstances
 * that you use the semaphore component.
 *
 * \param[in] sem_ptr       Pointer to the lightweight semaphore.
 * \param[in] time_in_ticks Maximum number of ticks to wait or 0 (unlimited wait).
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_create
 * \see _lwsem_post
 * \see LWSEM_STRUCT
 */
_mqx_uint _lwsem_wait_ticks
(
    LWSEM_STRUCT_PTR sem_ptr,
    _mqx_uint        time_in_ticks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;
    _mqx_uint              result;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_wait_ticks(sem_ptr, time_in_ticks);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_lwsem_wait_ticks, sem_ptr, time_in_ticks);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_lwsem_wait_ticks, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _KLOGX2(KLOG_lwsem_wait_ticks, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */
#endif

    _INT_DISABLE();
    if (sem_ptr->VALUE <= 0)
    {
        td_ptr = kernel_data->ACTIVE_PTR;
        if (time_in_ticks == 0)
        {
            td_ptr->STATE = LWSEM_BLOCKED;
            td_ptr->INFO = (_mqx_uint) &sem_ptr->TD_QUEUE;
            _QUEUE_UNLINK(td_ptr);
            _QUEUE_ENQUEUE(&sem_ptr->TD_QUEUE, &td_ptr->AUX_QUEUE);
            _sched_execute_scheduler_internal(); /* Let the other tasks run */
            /* Another task has posted a semaphore, and it has been tranfered to this
             ** task.
             */
            result = MQX_OK;
        }
        else
        {
            PSP_ADD_TICKS_TO_TICK_STRUCT(&kernel_data->TIME, time_in_ticks,
                            &td_ptr->TIMEOUT);
            result = _lwsem_wait_timed_internal(sem_ptr, td_ptr);
        } /* Endif */
    }
    else
    {
        --sem_ptr->VALUE;
        result = MQX_OK;
    } /* Endif */

    /* We must check for component destruction */
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _int_enable();
        /* The semaphore has been deleted */
        _KLOGX2(KLOG_lwsem_wait_ticks, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */

    _INT_ENABLE();

    _KLOGX2(KLOG_lwsem_wait_ticks, result);

    return (result);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore for the number of ticks.
 *
 * This function is an equivalent to the _lwsem_wait_ticks() API call but it can
 * be executed from within the User task or other code running in the CPU User
 * mode. Parameters passed to this function by pointer are required to meet the
 * memory protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr       Rread only. Pointer to the lightweight semaphore.
 * \param[in] time_in_ticks Maximum number of ticks to wait or 0 (unlimited wait).
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_wait_ticks
 * \see _usr_lwsem_create
 * \see _usr_lwsem_post
 * \see LWSEM_STRUCT
 */
_mqx_uint _usr_lwsem_wait_ticks
(
    LWSEM_STRUCT_PTR sem_ptr,
    _mqx_uint        time_in_ticks
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)sem_ptr, (uint_32)time_in_ticks, 0, 0, 0};
    return _mqx_api_call(MQX_API_LWSEM_WAIT_TICKS, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore until the specified
 * time (in tick time).
 *
 * \note Because priority inversion might occur if tasks with different priorities
 * access the same lightweight semaphore, we recommend under these circumstances
 * that you use the semaphore component.
 *
 * \param[in] sem_ptr Pointer to the lightweight semaphore.
 * \param[in] ticks   Pointer to the time (in tick time) until which to wait or
 * NULL (unlimited wait).
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_create
 * \see _lwsem_post
 * \see LWSEM_STRUCT
 * \see MQX_TICK_STRUCT
 */
_mqx_uint _lwsem_wait_until
(
    LWSEM_STRUCT_PTR    sem_ptr,
    MQX_TICK_STRUCT_PTR ticks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;
    _mqx_uint              result;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwsem_wait_until(sem_ptr, ticks);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_lwsem_wait_until, sem_ptr, ticks);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_lwsem_wait_until, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _KLOGX2(KLOG_lwsem_wait_until, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */
#endif

    _INT_DISABLE();
    if (sem_ptr->VALUE <= 0)
    {
        td_ptr = kernel_data->ACTIVE_PTR;
        td_ptr->TIMEOUT = *ticks;
        result = _lwsem_wait_timed_internal(sem_ptr, td_ptr);
    }
    else
    {
        --sem_ptr->VALUE;
        result = MQX_OK;
    } /* Endif */

    /* We must check for component destruction */
    if (sem_ptr->VALID != LWSEM_VALID)
    {
        _int_enable();
        /* The semaphore has been deleted */
        _KLOGX2(KLOG_lwsem_wait_until, MQX_INVALID_LWSEM);
        return (MQX_INVALID_LWSEM);
    } /* Endif */

    _INT_ENABLE();

    _KLOGX2(KLOG_lwsem_wait_until, result);

    return (result);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Waits (in FIFO order) for the lightweight semaphore until the specified
 * time (in tick time).
 *
 * This function is an equivalent to the _lwsem_wait_until() API call but it can
 * be executed from within the User task or other code running in the CPU User
 * mode. Parameters passed to this function by pointer are required to meet the
 * memory protection requirements as described in the parameter list below.
 *
 * \param[in] sem_ptr Read only. Pointer to the lightweight semaphore.
 * \param[in] ticks   Read/write. Pointer to the time (in tick time) until which
 * to wait or NULL (unlimited wait).
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the
 * lightweight semaphore.)
 *
 * \warning Might block the calling task.
 * Cannot be called from an ISR.
 *
 * \see _lwsem_wait_until
 * \see _usr_lwsem_create
 * \see _usr_lwsem_post
 * \see LWSEM_STRUCT
 * \see MQX_TICK_STRUCT
 */
_mqx_uint _usr_lwsem_wait_until
(
    LWSEM_STRUCT_PTR    sem_ptr,
    MQX_TICK_STRUCT_PTR ticks
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)sem_ptr, (uint_32)ticks, 0, 0, 0};
    return _mqx_api_call(MQX_API_LWSEM_WAIT_UNTIL, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

#endif /* MQX_HAS_TICK */
