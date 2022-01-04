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
 * $FileName: taskq.c$
 * $Version : 3.8.1.0$
 * $Date    : Apr-13-2012$
 *
 * Comments:
 *
 *   This file contains the function for creating a task queue.
 *
 *END************************************************************************/

#include "mqx_inc.h"

/*!
 * \brief Creates a task queue for use.
 * 
 * A task can use the task queue to suspend and resume tasks.
 * 
 * \param[in] policy Queuing policy, one of the following:
 * \li MQX_TASK_QUEUE_BY_PRIORITY
 * \li MQX_TASK_QUEUE_FIFO
 * 
 * \return Pointer to the task queue (Success.)
 * \return NULL (Failure.)
 * 
 * \warning Cannot be called from an ISR.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li Error from _mem_alloc_system() (MQX cannot allocate memory for the task 
 * queue.)
 * \li MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \li MQX_INVALID_PARAMETER (Policy is not one of the allowed policies.)
 * 
 * \see _taskq_destroy
 * \see _taskq_resume
 * \see _taskq_suspend
 * \see _task_set_error      
 */ 
pointer _taskq_create
(
    _mqx_uint policy
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TASK_QUEUE_STRUCT_PTR task_queue_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_taskq_create, policy);

#if MQX_CHECK_ERRORS
    if (! ((policy == MQX_TASK_QUEUE_FIFO) ||
                                    (policy == MQX_TASK_QUEUE_BY_PRIORITY)))
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_taskq_create, NULL);
        return (NULL);
    } /* Endif */
    if (kernel_data->IN_ISR) {
        _task_set_error(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        _KLOGX2(KLOG_taskq_create, NULL);
        return(NULL);
    }/* Endif */
#endif

    task_queue_ptr = (TASK_QUEUE_STRUCT_PTR) _mem_alloc_system((_mem_size) sizeof(TASK_QUEUE_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (task_queue_ptr == NULL) {
        _KLOGX2(KLOG_taskq_create, NULL);
        return(NULL);
    } /* Endif */
#endif

    _mem_set_type(task_queue_ptr, MEM_TYPE_TASK_Q);

    task_queue_ptr->POLICY = policy;
    _QUEUE_INIT(&task_queue_ptr->TD_QUEUE, 0);
    task_queue_ptr->VALID = TASK_QUEUE_VALID;
    _int_disable();
    if (kernel_data->KERNEL_TASK_QUEUES.NEXT == NULL) {
        /* Initialize the task queue */
        _QUEUE_INIT(&kernel_data->KERNEL_TASK_QUEUES, 0);

    } /* Endif */
    _QUEUE_ENQUEUE(&kernel_data->KERNEL_TASK_QUEUES, task_queue_ptr);
    _int_enable();

    _KLOGX2(KLOG_taskq_create, task_queue_ptr);

    return (task_queue_ptr);

} /* Endbody */

/*!
 * \brief Destroys the task queue.
 * 
 * The function removes all tasks from the task queue, puts them in their ready 
 * queues, and frees the task queue.
 * 
 * \param[in] users_task_queue_ptr Pointer to the task queue to destroy; 
 * returned by _taskq_create().
 * 
 * \return MQX_OK
 * \return MQX_INVALID_PARAMETER (Users_task_queue_ptr is NULL.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an 
 * ISR.)
 * \return MQX_INVALID_TASK_QUEUE (Task_queue_ptr does not point to a valid task 
 * queue.)
 * 
 * \warning Might put tasks in their ready queues.
 * \warning Cannot be called from an ISR.
 * 
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 * \see _taskq_resume
 * \see _taskq_suspend    
 */ 
_mqx_uint _taskq_destroy
(
    pointer users_task_queue_ptr
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR td_ptr;
    register TASK_QUEUE_STRUCT_PTR task_queue_ptr = (TASK_QUEUE_STRUCT_PTR) users_task_queue_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_taskq_destroy, users_task_queue_ptr);

#if MQX_CHECK_ERRORS
    if (task_queue_ptr == NULL) {
        _int_enable();
        _KLOGX2(KLOG_taskq_destroy, MQX_INVALID_PARAMETER);
        return(MQX_INVALID_PARAMETER);
    } /* Endif */
    if (kernel_data->IN_ISR) {
        _KLOGX2(KLOG_taskq_destroy, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    }/* Endif */
#endif

    _int_disable();
#if MQX_CHECK_VALIDITY
    if (task_queue_ptr->VALID != TASK_QUEUE_VALID) {
        _int_enable();
        _KLOGX2(KLOG_taskq_destroy, MQX_INVALID_TASK_QUEUE);
        return(MQX_INVALID_TASK_QUEUE);
    } /* Endif */
#endif

    task_queue_ptr->VALID = 0;
    while (_QUEUE_GET_SIZE(&task_queue_ptr->TD_QUEUE)) {
        _QUEUE_DEQUEUE(&task_queue_ptr->TD_QUEUE, td_ptr);
        _TASK_READY(td_ptr, kernel_data);
        _int_enable();
        _int_disable();
    } /* Endwhile */

    _QUEUE_REMOVE(&kernel_data->KERNEL_TASK_QUEUES, task_queue_ptr);

    _int_enable();

    _CHECK_RUN_SCHEDULER(); /* Let higher priority task run */

    _mem_free(task_queue_ptr);

    _KLOGX2(KLOG_taskq_destroy, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Suspends the active task and put it in the specified task queue.
 * 
 * The function blocks the calling task and puts the task's task descriptor in 
 * the task queue.
 * 
 * \param[in] users_task_queue_ptr Pointer to the task queue returned by 
 * _taskq_create().
 * 
 * \return MQX_OK
 * \return MQX_INVALID_PARAMETER (Users_task_queue_ptr is NULL.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an 
 * ISR.)
 * \return MQX_INVALID_TASK_QUEUE (Task_queue_ptr does not point to a valid task
 * queue.)
 * 
 * \warning Blocks the calling task.
 * \warning Cannot be called from an ISR. 
 * 
 * \see _taskq_destroy
 * \see _taskq_create
 * \see _taskq_resume
 * \see _taskq_get_value      
 */ 
_mqx_uint _taskq_suspend
(
    pointer users_task_queue_ptr
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR td_ptr;
    register TASK_QUEUE_STRUCT_PTR task_queue_ptr = (TASK_QUEUE_STRUCT_PTR) users_task_queue_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_taskq_suspend, users_task_queue_ptr);

#if MQX_CHECK_ERRORS
    if (task_queue_ptr == NULL) {
        _KLOGX2(KLOG_taskq_suspend, MQX_INVALID_PARAMETER);
        return(MQX_INVALID_PARAMETER);
    } /* Endif */
    if (kernel_data->IN_ISR) {
        _KLOGX2(KLOG_taskq_suspend, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    }/* Endif */
#endif

    td_ptr = kernel_data->ACTIVE_PTR;

    _INT_DISABLE();

#if MQX_CHECK_VALIDITY
    if (task_queue_ptr->VALID != TASK_QUEUE_VALID) {
        _int_enable();
        _KLOGX2(KLOG_taskq_suspend, MQX_INVALID_TASK_QUEUE);
        return(MQX_INVALID_TASK_QUEUE);
    } /* Endif */
#endif

    td_ptr->STATE = TASK_QUEUE_BLOCKED;
    _QUEUE_UNLINK(td_ptr); /* Remove task from ready to run queue */
    td_ptr->INFO = (_mqx_uint) & task_queue_ptr->TD_QUEUE;
    if (task_queue_ptr->POLICY & MQX_TASK_QUEUE_BY_PRIORITY) {
        _sched_insert_priorityq_internal(&task_queue_ptr->TD_QUEUE, td_ptr);
    }
    else {
        _QUEUE_ENQUEUE(&task_queue_ptr->TD_QUEUE, td_ptr);
    } /* Endif */

    _sched_execute_scheduler_internal(); /* Let the other tasks run */

    _INT_ENABLE();

    _KLOGX2(KLOG_taskq_suspend, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Restarts the task that is suspended in the task queue.
 *  
 * The function removes the task or tasks from the task queue and puts them in 
 * their ready queues. MQX schedules the tasks based on their priority, 
 * regardless of the scheduling policy of the task queue.
 * 
 * \param[in] users_task_queue_ptr Pointer to the task queue returned by 
 * _taskq_create().
 * \param[in] all_tasks FALSE (Make the first task ready.), TRUE (Makes all tasks 
 * ready.)
 * 
 * \return MQX_OK
 * \return MQX_INVALID_TASK_QUEUE (Task_queue_ptr is NULL.)
 * \return MQX_TASK_QUEUE_EMPTY (Task queue is empty.)   
 */ 
_mqx_uint _taskq_resume
(
    pointer users_task_queue_ptr,
    boolean all_tasks
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR td_ptr;
    register TASK_QUEUE_STRUCT_PTR task_queue_ptr = (TASK_QUEUE_STRUCT_PTR) users_task_queue_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_taskq_resume, users_task_queue_ptr, all_tasks);

#if MQX_CHECK_ERRORS
    if (task_queue_ptr == NULL) {
        _KLOGX2(KLOG_taskq_resume, MQX_INVALID_TASK_QUEUE);
        return(MQX_INVALID_TASK_QUEUE);
    } /* Endif */
#endif

    _INT_DISABLE();

#if MQX_CHECK_VALIDITY
    if (task_queue_ptr->VALID != TASK_QUEUE_VALID) {
        _int_enable();
        _KLOGX2(KLOG_taskq_resume, MQX_INVALID_TASK_QUEUE);
        return(MQX_INVALID_TASK_QUEUE);
    } /* Endif */
#endif

    if (_QUEUE_GET_SIZE(&task_queue_ptr->TD_QUEUE) == 0) {
        /* Task queue is empty */
        _int_enable();
        _KLOGX2(KLOG_taskq_resume, MQX_TASK_QUEUE_EMPTY);
        return (MQX_TASK_QUEUE_EMPTY);
    } /* Endif */

    if (all_tasks) {
        while (_QUEUE_GET_SIZE(&task_queue_ptr->TD_QUEUE)) {
            _QUEUE_DEQUEUE(&task_queue_ptr->TD_QUEUE, td_ptr);
            _TASK_READY(td_ptr, kernel_data);
        } /* Endwhile */
    }
    else {
        _QUEUE_DEQUEUE(&task_queue_ptr->TD_QUEUE, td_ptr);
        _TASK_READY(td_ptr, kernel_data);
    } /* Endif */

    _INT_ENABLE();

    _CHECK_RUN_SCHEDULER();/* Let higher priority task run */

    _KLOGX2(KLOG_taskq_resume, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Tests all task queues for correctness.
 * 
 * \param[out] task_queue_error_ptr Pointer to the task queue with an error 
 * (NULL if no error is found).
 * \param[out] td_error_ptr         Pointer to the task descriptor with an error 
 * (NULL if no error is found).
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an 
 * ISR.)
 * \return MQX_CORRUPT_QUEUE (A task on a task queue is not valid.) 
 * \return MQX_INVALID_TASK_QUEUE (A task queue is not valid.)
 * 
 * \warning Cannot be called from an ISR.
 * \warning Disables and enables interrupts.
 * 
 * \see _taskq_destroy
 * \see _taskq_create
 * \see _taskq_resume
 * \see _taskq_get_value    
 */ 
_mqx_uint _taskq_test
(
    pointer _PTR_ task_queue_error_ptr,
    pointer _PTR_ td_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TASK_QUEUE_STRUCT_PTR task_queue_ptr;
    _mqx_uint queue_size;
    _mqx_uint result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_taskq_test, task_queue_error_ptr, td_error_ptr);

    *td_error_ptr = NULL;
    *task_queue_error_ptr = NULL;

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR) {
        _KLOGX2(KLOG_taskq_test, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    }/* Endif */
#endif

    task_queue_ptr = (TASK_QUEUE_STRUCT_PTR)
    ((pointer)kernel_data->KERNEL_TASK_QUEUES.NEXT);
    if (_QUEUE_GET_SIZE(&kernel_data->KERNEL_TASK_QUEUES) == 0) {
        _KLOGX2(KLOG_taskq_test, MQX_OK);
        return(MQX_OK);
    } /* Endif */

    _int_disable();

    result = _queue_test((QUEUE_STRUCT_PTR)&kernel_data->KERNEL_TASK_QUEUES,
                    task_queue_error_ptr);
    if (result != MQX_OK) {
        _int_enable();
        _KLOGX3(KLOG_taskq_test, result, *task_queue_error_ptr);
        return(result);
    } /* Endif */

    queue_size = _QUEUE_GET_SIZE(&kernel_data->KERNEL_TASK_QUEUES);
    while (queue_size--) {
        if (task_queue_ptr->VALID != TASK_QUEUE_VALID) {
            result = MQX_INVALID_TASK_QUEUE;
            break;
        } /* Endif */

        result = _queue_test(&task_queue_ptr->TD_QUEUE, td_error_ptr);
        if (result != MQX_OK) {
            break;
        } /* Endif */

        task_queue_ptr = task_queue_ptr->NEXT;
    } /* Endwhile */

    _int_enable();

    if (result != MQX_OK) {
        *task_queue_error_ptr = (pointer)task_queue_ptr;
    } /* Endif */
    _KLOGX4(KLOG_taskq_test, result, *task_queue_error_ptr, *td_error_ptr);

    return(result);

} /* Endbody */

/*!
 * \brief Suspends the specified task in the specified task queue.
 * 
 * The function blocks the specified task and puts the task's task descriptor in 
 * the task queue.
 * 
 * \param[in] task_id              Task ID of the task to suspend.
 * \param[in] users_task_queue_ptr Pointer to the task queue; returned by 
 * _taskq_create().
 * 
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID (Task_id is not a valid task descriptor.) 
 * \return MQX_INVALID_PARAMETER (Users_task_queue_ptr is NULL.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an 
 * ISR.)
 * \return MQX_INVALID_TASK_QUEUE (Users_task_queue_ptr does not point to a 
 * valid task queue.) 
 * \return MQX_INVALID_TASK_STATE (Task is not in the ready state.)
 * 
 * \warning Blocks the specified task.
 * \warning Cannot be called from an ISR.
 * 
 * \see _taskq_destroy
 * \see _taskq_create
 * \see _taskq_resume
 * \see _taskq_get_value   
 */ 
_mqx_uint _taskq_suspend_task
(
    _task_id task_id,
    pointer  users_task_queue_ptr
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TASK_QUEUE_STRUCT_PTR task_queue_ptr = (TASK_QUEUE_STRUCT_PTR) users_task_queue_ptr;
    register TD_STRUCT_PTR td_ptr;
    boolean me;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_taskq_suspend_task, task_id, users_task_queue_ptr);

    td_ptr = (TD_STRUCT_PTR) _task_get_td(task_id);
    me = (td_ptr == kernel_data->ACTIVE_PTR);

#if MQX_CHECK_ERRORS
    if (td_ptr == NULL) {
        _KLOGX2(KLOG_taskq_suspend_task, MQX_INVALID_TASK_ID);
        return(MQX_INVALID_TASK_ID);
    } /* Endif */
    if (task_queue_ptr == NULL) {
        _KLOGX2(KLOG_taskq_suspend_task, MQX_INVALID_PARAMETER);
        return(MQX_INVALID_PARAMETER);
    } /* Endif */
    if (me && kernel_data->IN_ISR) {
        _KLOGX2(KLOG_taskq_suspend_task, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    }/* Endif */
#endif

    _INT_DISABLE();

#if MQX_CHECK_VALIDITY
    if (task_queue_ptr->VALID != TASK_QUEUE_VALID) {
        _int_enable();
        _KLOGX2(KLOG_taskq_suspend_task, MQX_INVALID_TASK_QUEUE);
        return(MQX_INVALID_TASK_QUEUE);
    } /* Endif */
#endif

    if (td_ptr->STATE != READY) {
        _INT_ENABLE();
        _KLOGX2(KLOG_taskq_suspend_task, MQX_INVALID_TASK_STATE);
        return (MQX_INVALID_TASK_STATE);
    } /* Endif */

    td_ptr->STATE = TASK_QUEUE_BLOCKED;
    _QUEUE_UNLINK(td_ptr); /* Remove task from ready to run queue */
    td_ptr->INFO = (_mqx_uint) & task_queue_ptr->TD_QUEUE;
    if (task_queue_ptr->POLICY & MQX_TASK_QUEUE_BY_PRIORITY) {
        _sched_insert_priorityq_internal(&task_queue_ptr->TD_QUEUE, td_ptr);
    }
    else {
        _QUEUE_ENQUEUE(&task_queue_ptr->TD_QUEUE, td_ptr);
    } /* Endif */

    if (me && (kernel_data->IN_ISR == 0)) {
        _sched_execute_scheduler_internal(); /* Let the other tasks run */
    } /* Endif */

    _INT_ENABLE();

    _KLOGX2(KLOG_taskq_suspend_task, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Gets the size of the task queue.
 * 
 * \param[in] users_task_queue_ptr Pointer to the task queue; returned by 
 * _taskq_create().
 * 
 * \return Number of tasks on the task queue (Success.)
 * \return MAX_MQX_UINT (Failure.) 
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_PARAMETER (Users_task_queue_ptr is NULL.)
 * \li MQX_INVALID_TASK_QUEUE (Users_task_queue_ptr does not point to a valid 
 * task queue.)
 * 
 * \see _taskq_create
 * \see _task_set_error  
 */ 
_mqx_uint _taskq_get_value
(
    pointer users_task_queue_ptr
)
{ /* Body */
    register TASK_QUEUE_STRUCT_PTR task_queue_ptr = (TASK_QUEUE_STRUCT_PTR) users_task_queue_ptr;

#if MQX_CHECK_ERRORS
    if (task_queue_ptr == NULL) {
        _task_set_error(MQX_INVALID_PARAMETER);
        return(MAX_MQX_UINT);
    } /* Endif */
#endif

#if MQX_CHECK_VALIDITY   
    if (task_queue_ptr->VALID != TASK_QUEUE_VALID) {
        _task_set_error(MQX_INVALID_TASK_QUEUE);
        return(MAX_MQX_UINT);
    } /* Endif */
#endif

    return (task_queue_ptr->TD_QUEUE.SIZE);

} /* Endbody */

/* EOF */
