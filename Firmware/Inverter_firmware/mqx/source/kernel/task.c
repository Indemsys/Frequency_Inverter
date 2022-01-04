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
 *****************************************************************************
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
 *****************************************************************************
 *
 * $FileName: task.c$
 * $Version : 3.8.0.2$
 * $Date    : Feb-27-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Task Management component.
 *
 *END************************************************************************/

#include <string.h>
#include "mqx_inc.h"

/*!
 * \private
 *
 * This variable has no use to MQX.
 * Its been created for Task Aware Debug module.
 */
volatile uint_32 _tad_task_at_flag;

/*!
 * \private
 *
 * \brief Makes a task run its task exit handler and then destroys itself.
 *
 * \param[in] task_id The task id of the task to abort.
 * \param[in] user User mode functionality.
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID
 *
 * \see _task_abort
 */
_mqx_uint _task_abort_internal
(
    _task_id    task_id,
    boolean     user
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;
    pointer                stack_ptr;
    _processor_number processor;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_task_abort, task_id);

    if (task_id != MQX_NULL_TASK_ID)
    {
        processor = PROC_NUMBER_FROM_TASKID(task_id);
        if (processor != (_processor_number) kernel_data->INIT.PROCESSOR_NUMBER)
        {
#if MQX_IS_MULTI_PROCESSOR
            if ( kernel_data->IPC != NULL )
            {
                _KLOGX2(KLOG_task_abort, MQX_OK);
                return( (*kernel_data->IPC)(FALSE, processor,
                                                KERNEL_MESSAGES, IPC_TASK_ABORT, 1, (_mqx_uint)task_id) );
            }
            else
            {
#endif
            _KLOGX2(KLOG_task_abort, MQX_INVALID_TASK_ID);
            return (MQX_INVALID_TASK_ID);
#if MQX_IS_MULTI_PROCESSOR
        } /* Endif */
#endif
        }/* Endif */
    }/* Endif */

    td_ptr = (TD_STRUCT_PTR) _task_get_td(task_id);

#if MQX_ENABLE_USER_MODE
    /* check template for correct user task parameters */
    if (user)
    {
        if (!(td_ptr->FLAGS & MQX_USER_TASK))
        {
            return MQX_INVALID_TASK_ID;
        }
    }
#endif

#if MQX_CHECK_ERRORS
    if ((td_ptr == NULL) || (td_ptr == SYSTEM_TD_PTR(kernel_data)))
    {
        _KLOGX2(KLOG_task_abort, MQX_INVALID_TASK_ID);
        return (MQX_INVALID_TASK_ID);
    } /* Endif */
#endif

    if (td_ptr == kernel_data->ACTIVE_PTR)
    {
        if (kernel_data->IN_ISR)
        {
            stack_ptr = (pointer) td_ptr->STACK_PTR;
            _PSP_SET_PC_OF_INTERRUPTED_TASK(stack_ptr,
                            _task_exit_function_internal);
        }
        else
        {
            _task_exit_function_internal();
        }/* Endif */
    }
    else
    {

        _int_disable();
        /* Task is not running */
        stack_ptr = (pointer) td_ptr->STACK_PTR;
        _PSP_SET_PC_OF_BLOCKED_TASK(stack_ptr,
                        _task_exit_function_internal);
        if (td_ptr->STATE & IS_ON_TIMEOUT_Q)
        {
            /* Remove from time queue (uses NEXT, PREV field) */
            _TIME_DEQUEUE(td_ptr, kernel_data);
        }
        else if (td_ptr->STATE & TD_IS_ON_QUEUE)
        {
            _QUEUE_REMOVE(td_ptr->INFO, td_ptr);
        }
        else if ((td_ptr->STATE & BLOCKED_ON_AUX_QUEUE) == BLOCKED_ON_AUX_QUEUE)
        {
            /* We need to remove it here because _task_ready() below will
             change its state to READY */
            _QUEUE_REMOVE(td_ptr->INFO, &td_ptr->AUX_QUEUE);
        } /* Endif */
        if (td_ptr->STATE & IS_BLOCKED)
        {
            _task_ready(td_ptr);
        } /* Endif */
        _int_enable();
    }/* Endif */

    _KLOGX2(KLOG_task_abort, MQX_OK);
    return (MQX_OK);
}

/*!
 * \brief Makes a task run its task exit handler and then destroys itself.
 *
 * While _task_destroy causes the task destroy to happen from the context
 * of the caller and is performed immediately, _task_abort causes the destroy
 * to happen from the context of the victim. _task_abort causes the victim task
 * to be removed from any queues it is blocked on, it's PC is effectively set to
 * the task exit handler and then the victim task is added to the ready to run
 * queue. Normal task scheduling and priority rules apply, so the actual task
 * destruction may be deferred indefinitely (or for a long time). The implication
 * is that there is no guarantee that the victim task is destroyed upon return
 * from _task_abort.

 *
 * \param[in] task_id One of the following:
 * \n - Task ID of the task to be destroyed.
 * \n - MQX_NULL_TASK_ID (Abort the calling task.)
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID (Task_id does not represent a valid task.)
 *
 * \see _task_destroy
 * \see _task_get_exit_handler
 * \see _task_set_exit_handler
 */
_mqx_uint _task_abort
(
    _task_id    task_id
)
{
#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_task_abort(task_id);
    }
#endif

    return _task_abort_internal(task_id, FALSE);
}

#if MQX_ENABLE_USER_MODE

/*!
 * \brief Makes a task run its task exit handler and then destroys itself.
 *
 * This function is an equivalent to the _task_abort() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] task_id One of the following:
 * \n - Task ID of the task to be destroyed.
 * \n - MQX_NULL_TASK_ID (Abort the calling task.)
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID
 *
 * \see _task_abort
 * \see _usr_task_destroy
 */
_mqx_uint _usr_task_abort
(
    _task_id    task_id
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)task_id, 0, 0, 0, 0};
    return _mqx_api_call(MQX_API_TASK_ABORT, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

#if !MQX_LITE_VERSION_NUMBER
/*!
 * \private
 *
 * \brief Creates the task and make it ready.
 *
 * \param[in] processor_number One of the following:
 * \n - Processor number of the processor where the task is to be created.
 * \n - 0 (Create on the local processor.)
 * \param[in] template_index One of the following:
 * \n - Index of the task template in the processor's task template list to use
 * for the child task.
 * \n - 0 (Use the task template that create_parameter defines.)
 * \param[in] parameter Pointer:
 * \n - Template_index is not 0 (Pointer to the parameter that MQX passes to the
 * child task.)
 * \n - Template_index is 0 (Pointer to the task template.)
 * \param[in] user User mode functionality.
 *
 * \return Task ID of the child task (Success.)
 * \return MQX_NULL_TASK_ID
 *
 * \warning If the child is on another processor, blocks the creator until the
 * child is created.
 * \warning On failure, _task_set_error() is called to set the the following task
 * error codes:
 * \n - MQX_INVALID_PROCESSOR_NUMBER (Processor_number is not one of the allowed
 * processor numbers.)
 * \n - MQX_NO_TASK_TEMPLATE (Template_index is not in the task template list.)
 * \n - MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the task data structures.)
 * \warning For _task_create(): If the child is on the same processor, preempts
 * the creator if the
 * child is a higher priority.
 *
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 * \see _usr_task_create
 */
_task_id _task_create_internal
(
    _processor_number   processor_number,
    _mqx_uint           template_index,
    uint_32             parameter,
    boolean             user
)
{
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    TD_STRUCT_PTR            td_ptr;
#if MQX_IS_MULTI_PROCESSOR
    TASK_TEMPLATE_STRUCT_PTR task_template_ptr;
    boolean                  blocking;
#endif
    _task_id                 result = MQX_NULL_TASK_ID;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE4(KLOG_task_create, processor_number, template_index, parameter);

#if MQX_CHECK_ERRORS
    if (template_index & SYSTEM_TASK_FLAG)
    {
        _task_set_error(MQX_INVALID_TEMPLATE_INDEX);
        _KLOGX3(KLOG_task_create, MQX_NULL_TASK_ID, MQX_INVALID_TEMPLATE_INDEX);
        return MQX_NULL_TASK_ID;
    }
#endif

#if MQX_ENABLE_USER_MODE
    if (user)
    {
        if (kernel_data->INIT.MAX_USER_TASK_COUNT && kernel_data->USR_TASK_RUN_COUNT >= kernel_data->INIT.MAX_USER_TASK_COUNT)
        {
            _task_set_error(MQX_TOO_MANY_USER_TASKS);
            _KLOGX3(KLOG_task_create, MQX_NULL_TASK_ID, MQX_TOO_MANY_USER_TASKS);
            return MQX_NULL_TASK_ID;
        }

        kernel_data->USR_TASK_RUN_COUNT++;
    }
#endif /* MQX_ENABLE_USER_MODE */
    if (processor_number == 0)
    {
        processor_number = (_processor_number) kernel_data->INIT.PROCESSOR_NUMBER;
#if MQX_CHECK_ERRORS
    }
    else if (processor_number > MQX_MAX_PROCESSOR_NUMBER)
    {
        _task_set_error(MQX_INVALID_PROCESSOR_NUMBER);
        _KLOGX3(KLOG_task_create, MQX_NULL_TASK_ID, MQX_INVALID_PROCESSOR_NUMBER);
        return MQX_NULL_TASK_ID;
#endif
    }
    else if (processor_number != kernel_data->INIT.PROCESSOR_NUMBER)
    {
#if MQX_ENABLE_USER_MODE
        if (user)
        {
            /* usermode don't support multiprocessor functionality */
            _task_set_error(MQX_INVALID_PROCESSOR_NUMBER);
            _KLOGX3(KLOG_task_create, MQX_NULL_TASK_ID, MQX_INVALID_PROCESSOR_NUMBER);
            return MQX_NULL_TASK_ID;
        }
#endif

#if MQX_IS_MULTI_PROCESSOR
        if ( kernel_data->IPC != NULL )
        {
#if MQX_TASK_CREATION_BLOCKS
            blocking = TRUE;
#else
            blocking = FALSE;
#endif
            if (template_index == 0)
            {
                /* Task template is pointed to by the parameter */
                task_template_ptr = (TASK_TEMPLATE_STRUCT_PTR)parameter;
                (*kernel_data->IPC)(blocking, processor_number,
                                KERNEL_MESSAGES,
                                IPC_TASK_CREATE_WITH_TEMPLATE,
                                8L,
                                task_template_ptr->TASK_TEMPLATE_INDEX,
                                task_template_ptr->TASK_ADDRESS,
                                task_template_ptr->TASK_STACKSIZE,
                                task_template_ptr->TASK_PRIORITY,
                                task_template_ptr->TASK_NAME,
                                task_template_ptr->TASK_ATTRIBUTES,
                                task_template_ptr->CREATION_PARAMETER,
                                task_template_ptr->DEFAULT_TIME_SLICE);
            }
            else
            {
                (*kernel_data->IPC)(blocking, processor_number,
                                KERNEL_MESSAGES, IPC_TASK_CREATE, 3,
                                processor_number, template_index, parameter);
            } /* Endif */
            return(kernel_data->ACTIVE_PTR->INFO);
        }
        else
        {
#endif
        _task_set_error(MQX_INVALID_PROCESSOR_NUMBER);
        _KLOGX3(KLOG_task_create, MQX_NULL_TASK_ID, MQX_INVALID_PROCESSOR_NUMBER);
        return MQX_NULL_TASK_ID;
#if MQX_IS_MULTI_PROCESSOR
    } /* Endif */
#endif
    } /* Endif */

    td_ptr = _task_build_internal(template_index, parameter, NULL, 0, user);

    if (td_ptr != NULL)
    {
        result = td_ptr->TASK_ID;
        _INT_DISABLE();
        _task_ready_internal(td_ptr);
        _INT_ENABLE();
        _CHECK_RUN_SCHEDULER(); /* Let a higher priority task run */
    }

    _KLOGX3(KLOG_task_create, result, kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);

    return (result);
}

/*!
 * \brief Creates the task and make it ready.
 *
 * \param[in] processor_number One of the following:
 * \n - Processor number of the processor where the task is to be created.
 * \n - 0 (Create on the local processor.)
 * \param[in] template_index   One of the following:
 * \n - Index of the task template in the processor's task template list to use
 * for the child task.
 * \n - 0 (Use the task template that create_parameter defines.)
 * \param[in] parameter        Pointer:
 * \n - Template_index is not 0 (Pointer to the parameter that MQX passes to the
 * child task.)
 * \n - Template_index is 0 (Pointer to the task template.)
 *
 * \return Task ID of the child task (Success.)
 * \return MQX_NULL_TASK_ID (Failure.)
 *
 * \warning If the child is on another processor, blocks the creator until the
 * child is created.
 * \warning On failure, _task_set_error() is called to set the the following task
 * error codes:
 * \n - MQX_INVALID_PROCESSOR_NUMBER (Processor_number is not one of the allowed
 * processor numbers.)
 * \n - MQX_NO_TASK_TEMPLATE (Template_index is not in the task template list.)
 * \n - MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the task data structures.)
 * \warning If the child is on the same processor, preempts the creator if the
 * child is a higher priority.
 *
 * \see _task_create_blocked
 * \see _task_create_at
 * \see _task_abort
 * \see _task_block
 * \see _task_destroy
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see _task_ready
 * \see _task_set_error
 */
_task_id _task_create
(
    _processor_number   processor_number,
    _mqx_uint           template_index,
    uint_32             parameter
)
{
#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_task_create(processor_number, template_index, parameter);
    }
#endif
    return _task_create_internal(processor_number, template_index, parameter, FALSE);
}
#endif /* MQX_LITE_VERSION_NUMBER */

#if MQX_ENABLE_USER_MODE

/*!
 * \brief Creates the task and make it ready.
 *
 * This function is an equivalent to the _task_create() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] processor_number One of the following:
 * \n - Processor number of the processor where the task is to be created.
 * \n - 0 (Create on the local processor.)
 * \param[in] template_index   One of the following:
 * \n - Index of the task template in the processor's task template list to use
 * for the child task.
 * \n - 0 (Use the task template that create_parameter defines.)
 * \param[in] parameter        Pointer:
 * \n - Template_index is not 0 (Pointer to the parameter that MQX passes to the
 * child task.)
 * \n - Template_index is 0 (Pointer to the task template.)
 *
 * \return Task ID of the child task (Success.)
 * \return MQX_NULL_TASK_ID (Failure.)
 *
 * \warning If the child is on another processor, blocks the creator until the
 * child is created.
 * \warning On failure, _task_set_error() is called to set the the following task
 * error codes:
 * \n - MQX_INVALID_PROCESSOR_NUMBER (Processor_number is not one of the allowed
 * processor numbers.)
 * \n - MQX_NO_TASK_TEMPLATE (Template_index is not in the task template list.)
 * \n - MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the task data structures.)
 * \warning If the child is on the same processor, preempts the creator if the
 * child is a higher priority.
 *
 * \see _task_create
 * \see _usr_task_abort
 * \see _usr_task_destroy
 * \see _usr_task_ready
 * \see _usr_task_set_error
 */
_task_id _usr_task_create
(
    _processor_number   processor_number,
    _mqx_uint           template_index,
    uint_32             parameter
)
{
    MQX_API_CALL_PARAMS params =
        {(uint_32)processor_number, (uint_32)template_index, (uint_32)parameter, 0, 0};

    return _mqx_api_call(MQX_API_TASK_CREATE, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

#if !MQX_LITE_VERSION_NUMBER
/*!
 * \brief Creates the task, but do not make it ready.
 *
 * \param[in] processor_number One of the following:
 * \n - Processor number of the processor where the task is to be created.
 * \n - 0 (Create on the local processor.)
 * \param[in] template_index   One of the following:
 * \n - Index of the task template in the processor's task template list to use
 * for the child task.
 * \n - 0 (Use the task template that create_parameter defines.)
 * \param[in] parameter        Pointer:
 * \n - Template_index is not 0 (Pointer to the parameter that MQX passes to the
 * child task.)
 * \n - Template_index is 0 (Pointer to the task template.)
 *
 * \return Task ID of the child task (Success.)
 * \return MQX_NULL_TASK_ID (Failure.)
 *
 * \warning If the child is on another processor, blocks the creator until the
 * child is created.
 * \warning On failure, _task_set_error() is called to set the the following task
 * error codes:
 * \n - MQX_INVALID_PROCESSOR_NUMBER (Processor_number is not one of the allowed
 * processor numbers.)
 * \n - MQX_NO_TASK_TEMPLATE (Template_index is not in the task template list.)
 * \n - MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the task data structures.)
 * \warning If the child is on the same processor, preempts the creator if the
 * child is a higher priority.
 *
 * \see _task_create
 * \see _task_create_at
 * \see _task_abort
 * \see _task_block
 * \see _task_destroy
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see _task_ready
 * \see _task_set_error
 */
_task_id _task_create_blocked
(
    _processor_number   processor_number,
    _mqx_uint           template_index,
    uint_32             parameter
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    TD_STRUCT_PTR            td_ptr;
#if MQX_IS_MULTI_PROCESSOR
    TASK_TEMPLATE_STRUCT_PTR task_template_ptr;
    boolean                  blocking;
#endif
    _task_id                 result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE4(KLOG_task_create_block, processor_number, template_index, parameter);

#if MQX_CHECK_ERRORS
    if (template_index & SYSTEM_TASK_FLAG)
    {
        _task_set_error(MQX_INVALID_TEMPLATE_INDEX);
        _KLOGX3(KLOG_task_create_block, MQX_NULL_TASK_ID,
                        MQX_INVALID_TEMPLATE_INDEX);
        return MQX_NULL_TASK_ID;
    } /* Endif */
#endif

    if (processor_number == 0)
    {
        processor_number = (_processor_number) kernel_data->INIT.PROCESSOR_NUMBER;
#if MQX_CHECK_ERRORS
    }
    else if (processor_number > MQX_MAX_PROCESSOR_NUMBER)
    {
        _task_set_error(MQX_INVALID_PROCESSOR_NUMBER);
        _KLOGX3(KLOG_task_create_block, MQX_NULL_TASK_ID,
                        MQX_INVALID_PROCESSOR_NUMBER);
        return MQX_NULL_TASK_ID;
#endif
    }
    else if (processor_number != kernel_data->INIT.PROCESSOR_NUMBER)
    {
#if MQX_IS_MULTI_PROCESSOR
        if ( kernel_data->IPC != NULL )
        {
#if MQX_TASK_CREATION_BLOCKS
            blocking = TRUE;
#else
            blocking = FALSE;
#endif
            if (template_index == 0)
            {
                /* Task template is pointed to by the parameter */
                task_template_ptr = (TASK_TEMPLATE_STRUCT_PTR)parameter;
                (*kernel_data->IPC)(blocking, processor_number,
                                KERNEL_MESSAGES, IPC_TASK_CREATE_WITH_TEMPLATE, 8L,
                                task_template_ptr->TASK_TEMPLATE_INDEX,
                                task_template_ptr->TASK_ADDRESS,
                                task_template_ptr->TASK_STACKSIZE,
                                task_template_ptr->TASK_PRIORITY,
                                task_template_ptr->TASK_NAME,
                                task_template_ptr->TASK_ATTRIBUTES,
                                task_template_ptr->CREATION_PARAMETER,
                                task_template_ptr->DEFAULT_TIME_SLICE
                );
            }
            else
            {
                (*kernel_data->IPC)(blocking, processor_number,
                                KERNEL_MESSAGES, IPC_TASK_CREATE, 3,
                                processor_number, template_index, parameter);
            } /* Endif */
            return(kernel_data->ACTIVE_PTR->INFO);
        }
        else
        {
#endif
        _task_set_error(MQX_INVALID_PROCESSOR_NUMBER);
        _KLOGX3(KLOG_task_create_block, MQX_NULL_TASK_ID,
                        MQX_INVALID_PROCESSOR_NUMBER);
        return MQX_NULL_TASK_ID;
#if MQX_IS_MULTI_PROCESSOR
    } /* Endif */
#endif
    } /* Endif */

    td_ptr = _task_build_internal(template_index, parameter, NULL, 0, FALSE);
    if (td_ptr != NULL)
    {
        td_ptr->STATE = BLOCKED;
        result = td_ptr->TASK_ID;
    }
    else
    {
        result = MQX_NULL_TASK_ID;
    }/* Endif */

    _KLOGX3(KLOG_task_create_block, result,
                    kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);

    return (result);

} /* Endbody */
#endif /* MQX_LITE_VERSION_NUMBER */

/*!
 * \brief Creates the task with the stack location specified.
 *
 * \param[in] processor_number One of the following:
 * \n - Processor number of the processor where the task is to be created.
 * \n - 0 (Create on the local processor.)
 * \param[in] template_index   One of the following:
 * \n - Index of the task template in the processor's task template list to use
 * for the child task.
 * \n - 0 (Use the task template that create_parameter defines.)
 * \param[in] parameter        Pointer:
 * \n - Template_index is not 0 (Pointer to the parameter that MQX passes to the
 * child task.)
 * \n - Template_index is 0 (Pointer to the task template.)
 * \param[in] stack_ptr        Pointer to where the stack and TD are to be created.
 * \param[in] stack_size       The size of the stack.
 *
 * \return Task ID of the child task (Success.)
 * \return MQX_NULL_TASK_ID (Failure.)
 *
 * \warning If the child is on another processor, blocks the creator until the
 * child is created.
 * \warning On failure, _task_set_error() is called to set the following task
 * error codes:
 * \n Task Error Codes
 * \n - MQX_INVALID_PROCESSOR_NUMBER (Processor_number is not one of the allowed
 * processor numbers.)
 * \n - MQX_NO_TASK_TEMPLATE (Template_index is not in the task template list.)
 * \n - MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the task data structures.)
 *
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_abort
 * \see _task_block
 * \see _task_destroy
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see _task_ready
 * \see _task_set_error
 */
_task_id _task_create_at
(
    _processor_number   processor_number,
    _mqx_uint           template_index,
    uint_32             parameter,
    pointer             stack_ptr,
    _mem_size           stack_size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;
    _task_id               result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE5(KLOG_task_create_at, processor_number, template_index, parameter,
                    stack_ptr);

#if MQX_CHECK_ERRORS
    if (template_index & SYSTEM_TASK_FLAG)
    {
        _task_set_error(MQX_INVALID_TEMPLATE_INDEX);
        _KLOGX3(KLOG_task_create_at, MQX_NULL_TASK_ID,
                        MQX_INVALID_TEMPLATE_INDEX);
        return MQX_NULL_TASK_ID;
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (processor_number != 0)
    {
        _task_set_error(MQX_INVALID_PROCESSOR_NUMBER);
        _KLOGX3(KLOG_task_create_at, MQX_NULL_TASK_ID,
                        MQX_INVALID_PROCESSOR_NUMBER);
        return MQX_NULL_TASK_ID;
    } /* Endif */
#endif

    td_ptr = _task_build_internal(template_index, parameter, stack_ptr, stack_size, FALSE);
    if (td_ptr != NULL)
    {
        result = td_ptr->TASK_ID;
        _INT_DISABLE();
        _task_ready_internal(td_ptr);
        _INT_ENABLE();
        _CHECK_RUN_SCHEDULER(); /* Let a higher priority task run */
    }
    else
    {
        result = MQX_NULL_TASK_ID;
    }/* Endif */

    _KLOGX3(KLOG_task_create_at, result,
                    kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);

    _tad_task_at_flag++;

    return (result);

} /* Endbody */

/*!
 * \private
 *
 * \brief Destroys the task.
 *
 * \param[in] task_id One of the following:
 * \n - Task ID of the task to be destroyed.
 * \n - MQX_NULL_TASK_ID (Destroy the calling task.)
 * \param[in] user    User mode functionality.
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID
 *
 * \see _task_destroy
 * \see _usr_task_destroy
 */
_mqx_uint _task_destroy_internal
(
    _task_id task_id,
    boolean  user
)
{
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    TD_STRUCT_PTR            victim_ptr;
    TD_STRUCT_PTR            td_ptr;
    _processor_number        processor;
    uint_16                  task_num;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_task_destroy, task_id);

    if (task_id != MQX_NULL_TASK_ID) {
        processor = PROC_NUMBER_FROM_TASKID(task_id);
        if (processor != (_processor_number)kernel_data->INIT.PROCESSOR_NUMBER ) {
#if MQX_IS_MULTI_PROCESSOR
            if ( kernel_data->IPC != NULL ) {
                _KLOGX2(KLOG_task_destroy, MQX_OK);
                return( (*kernel_data->IPC)(FALSE, processor,
                                KERNEL_MESSAGES, IPC_TASK_DESTROY, 1, (_mqx_uint)task_id) );
            } else {
#endif
                _KLOGX2(KLOG_task_destroy, MQX_INVALID_TASK_ID);
                return(MQX_INVALID_TASK_ID);
#if MQX_IS_MULTI_PROCESSOR
            } /* Endif */
#endif
        }/* Endif */
    }/* Endif */

    victim_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);

#if MQX_ENABLE_USER_MODE
    /* check template for correct user task parameters */
    if (user) {
        if (!(victim_ptr->FLAGS & MQX_USER_TASK)) {
            return MQX_INVALID_TASK_ID;
        }
    }
#endif

    /* Make sure we are not trying to destroy the system task */
#if MQX_CHECK_ERRORS
    if ((victim_ptr == NULL) || (victim_ptr == SYSTEM_TD_PTR(kernel_data))) {
        _KLOGX2(KLOG_task_destroy, MQX_INVALID_TASK_ID);
        return MQX_INVALID_TASK_ID;
    } /* Endif */
#endif

    /* First, serialize task creation/destruction */
    _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    _int_disable();

    if (victim_ptr->STATE == DYING) {
        /* Task is already being destroyed */
        /* SPR P171-0021-01 re-enable interrupts */
        _int_enable();
        /* END SPR */
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);
        _KLOGX2(KLOG_task_destroy, MQX_OK);
        return MQX_OK;
    } /* Endif */

    /* remove the victim from any queues it may be in. */
    if (victim_ptr->STATE == READY) {
        if (victim_ptr != kernel_data->ACTIVE_PTR) {
            /* Remove other task from ready queue */
            _QUEUE_UNLINK(victim_ptr);
        } /* Endif */
    } else if (victim_ptr->STATE & IS_ON_TIMEOUT_Q) {
        /* Remove from time queue (uses NEXT, PREV field) */
        _TIME_DEQUEUE(victim_ptr, kernel_data);
    } else if (victim_ptr->STATE & TD_IS_ON_QUEUE) {
        /*
         * Remove from queue, where suspended
         * eg, MUTEX_BLOCKED, IO_BLOCKED, TASKQ_BLOCKED
         * (uses NEXT, PREV field)
         */
        _QUEUE_REMOVE(victim_ptr->INFO, victim_ptr);
    } /* Endif */

    if (victim_ptr->STATE & TD_IS_ON_AUX_QUEUE) {
        /*
         * Remove from queue, where suspended
         * eg, LWSEM_BLOCKED
         * (uses AUX_QUEUE field)
         */
        _QUEUE_REMOVE(victim_ptr->INFO, &victim_ptr->AUX_QUEUE);
    } /* Endif */

    victim_ptr->STATE   = DYING;

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
    /* Stop Floating point context monitoring */
    if (kernel_data->FP_ACTIVE_PTR == victim_ptr) {
        kernel_data->FP_ACTIVE_PTR = NULL;
    }/* Endif */
#endif

#if MQXCFG_ENABLE_DSP && PSP_HAS_DSP
    /* Stop DSP context monitoring */
    if (kernel_data->DSP_ACTIVE_PTR == victim_ptr) {
        kernel_data->DSP_ACTIVE_PTR = NULL;
    }/* Endif */
#endif

    _int_enable();

#if MQX_COMPONENT_DESTRUCTION
    {
        _mqx_uint i;
        for (i = 0; i < MAX_KERNEL_COMPONENTS; ++i) {
            if (kernel_data->COMPONENT_CLEANUP[i] != NULL) {
                (*kernel_data->COMPONENT_CLEANUP[i])(victim_ptr);
            }
        }
    }
#endif


    /* Save the task number */
    task_num = TASK_NUMBER_FROM_TASKID(victim_ptr->TASK_ID);

    td_ptr = kernel_data->ACTIVE_PTR;

#if !MQX_LITE_VERSION_NUMBER
    if ((victim_ptr->FLAGS & TASK_STACK_PREALLOCATED) == 0) {
        pointer block_ptr = _mem_get_next_block_internal(victim_ptr, NULL);
        while (block_ptr != NULL) {
            pointer next_block_ptr = _mem_get_next_block_internal(victim_ptr, block_ptr);
            if (td_ptr != victim_ptr) {
                _mem_transfer_internal(block_ptr, SYSTEM_TD_PTR(kernel_data));
            }

            /* dealloc everything else except td and stack */
            if ((block_ptr != victim_ptr) && (block_ptr != victim_ptr->STACK_ALLOC_BLOCK)) {
                _mem_free(block_ptr);
            }
            block_ptr = next_block_ptr;
        }
    }
#endif /* MQX_LITE_VERSION_NUMBER */

    victim_ptr->TASK_ID = TASK_ID_DYING;

    /* Start SPR P171-0017-01                */
    /* Remove td from linked list of all tds */
    if (_QUEUE_GET_SIZE(&kernel_data->TD_LIST) == 1) {
        /* Destroying the last task */
        kernel_data->TASK_NUMBER   = 1;
        kernel_data->INSERT_TD_PTR = NULL;
    } else if (kernel_data->TASK_NUMBER > task_num) {
        kernel_data->TASK_NUMBER   = task_num;
        kernel_data->INSERT_TD_PTR =
                        (TD_STRUCT_PTR)((uchar_ptr)victim_ptr->TD_LIST_INFO.PREV -
                                        FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
    } /* Endif */
    _QUEUE_REMOVE(&kernel_data->TD_LIST, &victim_ptr->TD_LIST_INFO);
    /* End SPR P171-0017-01                  */

    /* Free up create/destroy for other tasks */
    _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    if (victim_ptr == td_ptr) {
        /* We are destroying the current task */
        _int_disable();
        _QUEUE_UNLINK(victim_ptr);  /* Remove myself from the ready queue */
        _psp_destroy_stack_frame(victim_ptr);
#if !MQX_LITE_VERSION_NUMBER
        if ((victim_ptr->FLAGS & TASK_STACK_PREALLOCATED) == 0) {
            if (victim_ptr->STACK_ALLOC_BLOCK) {
                _mem_transfer_internal(victim_ptr->STACK_ALLOC_BLOCK, SYSTEM_TD_PTR(kernel_data));
                _mem_free(victim_ptr->STACK_ALLOC_BLOCK);   /* Free the task's stack */
            }

            _mem_transfer_internal(victim_ptr, SYSTEM_TD_PTR(kernel_data));
            _mem_free(victim_ptr);   /* Free the task descriptor */
        } /* Endif */
#endif /* MQX_LITE_VERSION_NUMBER */
        /* Now we must jump to the scheduler */
        _sched_run_internal();   /* WILL NEVER RETURN FROM HERE */
    }/* Endif */

    _psp_destroy_stack_frame(victim_ptr);

#if !MQX_LITE_VERSION_NUMBER
    /* Free the task descriptor. */
    if ((victim_ptr->FLAGS & TASK_STACK_PREALLOCATED) == 0) {
        if (victim_ptr->STACK_ALLOC_BLOCK) {
            _mem_free(victim_ptr->STACK_ALLOC_BLOCK);   /* Free the task's stack */
        }
        _mem_free(victim_ptr);  /* Free the task descriptor */
    } /* Endif */
#endif /* MQX_LITE_VERSION_NUMBER */

#if MQX_ENABLE_USER_MODE
    if (victim_ptr->FLAGS & MQX_USER_TASK) {
        _int_disable();
        kernel_data->USR_TASK_RUN_COUNT--;
        _int_enable();
    }
#endif /* MQX_ENABLE_USER_MODE */

    _KLOGX2(KLOG_task_destroy, MQX_OK);
    return(MQX_OK);

}

/*!
 * \brief Destroys the task.
 *
 * This function does the following for the task being destroyed:
 * \n - Frees memory resources that the task allocated with functions from the
 * _mem and _partition families.
 * \n - Closes all queues that the task owns and frees all the queue elements.
 * \n - Frees any other component resources that the task owns.
 *
 * While _task_abort causes the destroy to happen from the context of the victim
 * and may be deferred indefinitely (or for a long time), _task_destroy causes
 * the task destroy to happen from the context of the caller and is performed
 * immediately.
 *
 * \param[in] task_id One of the following:
 * \n - Task ID of the task to be destroyed.
 * \n - MQX_NULL_TASK_ID (Destroy the calling task.)
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID
 *
 * \warning If the task being destroyed is remote, blocks the calling task until
 * the task is destroyed.
 * \warning If the task being destroyed is local, does not block the calling task.
 * \warning If the task being destroyed is the active task, blocks it.
 *
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 * \see _task_get_creator
 * \see _task_get_id
 * \see _task_abort
 */
_mqx_uint _task_destroy
(
    _task_id task_id
)
{
#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_task_destroy(task_id);
    }
#endif

    return _task_destroy_internal(task_id, FALSE);
}


#if MQX_ENABLE_USER_MODE
/*!
 * \brief Destroys the task.
 *
 * This function is an equivalent to the _task_destroy() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] task_id One of the following:
 * \n - Task ID of the task to be destroyed.
 * \n - MQX_NULL_TASK_ID (Destroy the calling task.)
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID
 *
 * \warning If the task being destroyed is remote, blocks the calling task until
 * the task is destroyed.
 * \warning If the task being destroyed is local, does not block the calling task.
 * \warning If the task being destroyed is the active task, blocks it.
 *
 * \see _task_destroy
 * \see _usr_task_create
 * \see _usr_task_abort
 */
_mqx_uint _usr_task_destroy
(
    _task_id task_id
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)task_id, 0, 0, 0, 0};
    return _mqx_api_call(MQX_API_TASK_DESTROY, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
#if !MQX_LITE_VERSION_NUMBER

/*!
 * \brief Enables floating-point context switching for the active task.
 *
 * When MQX performs a context switch (due to _task_enable_fp call), floating-point
 * registers are saved and restored. Otherwise ( _task_disable_fp call) floating-point
 * registers are not saved and restored.
 *
 * \warning Changes context information that MQX stores.
 *
 * \see _task_disable_fp
 */
boolean _task_enable_fp(void)
{
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR td_ptr;
    boolean res = TRUE;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE1(KLOG_task_enable_fp);

    td_ptr = kernel_data->ACTIVE_PTR;
    _int_disable();

    if (NULL == td_ptr->FLOAT_CONTEXT_PTR)
    {
        /* we dont have allocated space for fp context... try allocate it */
        _psp_build_float_context(td_ptr);
    }

    if (NULL != td_ptr->FLOAT_CONTEXT_PTR)
    {
        /* we have allocated space for fp context */
        td_ptr->FLAGS |= MQX_FLOATING_POINT_TASK;
        if (kernel_data->FP_ACTIVE_PTR != NULL)
        {
            if (kernel_data->FP_ACTIVE_PTR != kernel_data->ACTIVE_PTR)
            {
                _psp_save_fp_context_internal();
            }
        }
        kernel_data->FP_ACTIVE_PTR = kernel_data->ACTIVE_PTR;
    }
    else
    {
        /* some problem - TD dont have allocated space for fp context */
        res = FALSE;
    }

    _int_enable();
    _KLOGX1(KLOG_task_enable_fp);

    return res;
}

/*!
 * \brief Disables floating-point context switching for the active task if the
 * task is a floating-point task.
 *
 * When MQX performs a context switch (due to _task_enable_fp call), floating-point
 * registers are saved and restored. Otherwise ( _task_disable_fp call) floating-point
 * registers are not saved and restored.
 *
 * \warning Changes context information that MQX stores.
 *
 * \see _task_enable_fp
 */
void _task_disable_fp(void)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR                   td_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE1(KLOG_task_disable_fp);

    td_ptr = kernel_data->ACTIVE_PTR;
    _int_disable();
    if (td_ptr->FLAGS & MQX_FLOATING_POINT_TASK)
    {
        td_ptr->FLAGS &= ~MQX_FLOATING_POINT_TASK;
        kernel_data->FP_ACTIVE_PTR = NULL;
    }

    if (NULL != td_ptr->FLOAT_CONTEXT_PTR)
    {
        /* deallocate fp context area */
        _mem_free(td_ptr->FLOAT_CONTEXT_PTR);
        td_ptr->FLOAT_CONTEXT_PTR = NULL;
    }

    _int_enable();
    _KLOGX1(KLOG_task_disable_fp);

} /* Endbody */
#endif /* MQX_LITE_VERSION_NUMBER */
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */

#if MQX_HAS_TASK_ENVIRONMENT

/*!
 * \brief Sets the address of the application-specific environment data for the task.
 *
 * \param[in] task_id         Task ID of the task whose environment is to be set.
 * \param[in] environment_ptr Pointer to the environment data.
 *
 * \return Previous environment data (Success.)
 * \return NULL (Failure.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code to
 * MQX_INVALID_TASK_ID.
 *
 * \see _task_get_environment
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see _task_set_error
 */
pointer _task_set_environment
(
    _task_id    task_id,
    pointer     environment_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    TD_STRUCT_PTR td_ptr;
    pointer       old_env_ptr;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE3(KLOG_task_set_environment, task_id, environment_ptr);

    td_ptr = (TD_STRUCT_PTR) _task_get_td(task_id);
    if (td_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_TASK_ID);
        _KLOGX2(KLOG_task_set_environment, NULL);
        return (NULL);
    }/* Endif */

    old_env_ptr = td_ptr->ENVIRONMENT_PTR;
    td_ptr->ENVIRONMENT_PTR = environment_ptr;

    _KLOGX2(KLOG_task_set_environment, old_env_ptr);
    return (old_env_ptr);

} /* Endbody */

/*!
 * \brief Gets a pointer to the application-specific environment data for the task.
 *
 * \param[in] task_id Task ID of the task whose environment is to be obtained.
 *
 * \return Environment data (Success.)
 * \return NULL (Failure.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code to
 * MQX_INVALID_TASK_ID.
 *
 * \see _task_set_environment
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see _task_set_error
 */
pointer _task_get_environment
(
    _task_id task_id
)
{ /* Body */
    TD_STRUCT_PTR td_ptr;

    td_ptr = (TD_STRUCT_PTR) _task_get_td(task_id);
    if (td_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_TASK_ID);
        return (NULL);
    }/* Endif */

    return (td_ptr->ENVIRONMENT_PTR);

} /* Endbody */

#endif /* MQX_HAS_TASK_ENVIRONMENT */

#if MQX_HAS_EXCEPTION_HANDLER

/*!
 * \brief Gets a pointer to the task exception handler.
 *
 * \param[in] task_id Task ID of the task whose exception handler is to be obtained.
 *
 * \return Pointer to the task exception handler for the task (might be NULL).
 * \return NULL (Task ID is not valid.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code to
 * MQX_INVALID_TASK_ID.
 *
 * \see _task_set_exception_handler
 * \see _task_get_exit_handler
 * \see _task_set_exit_handler
 * \see _int_exception_isr
 * \see _task_set_error
 */
TASK_EXCEPTION_FPTR _task_get_exception_handler
(
    _task_id        task_id
)
{ /* Body */
    TD_STRUCT_PTR       td_ptr;
    TASK_EXCEPTION_FPTR excpt_function;

    td_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);
    if ( td_ptr == NULL )
    {
        _task_set_error(MQX_INVALID_TASK_ID);
        return( NULL );
    }/* Endif */

    excpt_function   = td_ptr->EXCEPTION_HANDLER_PTR;

    return(excpt_function);

} /* Endbody */

/*!
 * \brief Sets the address of the task exception handler.
 *
 * \param[in] task_id         Task ID of the task whose exception handler is to be set.
 * \param[in] handler_address Pointer to the task exception handler.
 *
 * \return Pointer to the previous task exception handler (might be NULL).
 * \return NULL (Task ID is not valid.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code to
 * MQX_INVALID_TASK_ID.
 *
 * \see _task_get_exception_handler
 * \see _task_get_exit_handler
 * \see _task_set_exit_handler
 * \see _int_exception_isr
 * \see _task_set_error
 */
TASK_EXCEPTION_FPTR _task_set_exception_handler
(
    _task_id            task_id,
    TASK_EXCEPTION_FPTR handler_address
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    TD_STRUCT_PTR       td_ptr;
    TASK_EXCEPTION_FPTR old_function;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE3(KLOG_task_set_exception_handler, task_id, handler_address);

    td_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);
    if ( td_ptr == NULL )
    {
        _task_set_error(MQX_INVALID_TASK_ID);
        _KLOGX2(KLOG_task_set_exception_handler, NULL);
        return( NULL );
    } /* Endif */

    old_function = td_ptr->EXCEPTION_HANDLER_PTR;
    td_ptr->EXCEPTION_HANDLER_PTR = handler_address;

    _KLOGX2(KLOG_task_set_exception_handler, old_function);
    return(old_function);

} /* Endbody */

#endif /* MQX_HAS_EXCEPTION_HANDLER */

#if MQX_HAS_EXIT_HANDLER
/*!
 * \brief Gets a pointer to the task exit handler for the task.
 *
 * MQX calls a task's task exit handler if either of these conditions is true:
 * \n - Task is terminated with _task_abort().
 * \n - Task returns from its function body (for example, if it calls _mqx_exit()).
 *
 * \param[in] task_id Task ID of the task whose exit handler is to be obtained.
 *
 * \return Pointer to the exit handler (might be NULL).
 * \return NULL (Task_id is not valid.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code to
 * MQX_INVALID_TASK_ID.
 *
 * \see _task_set_exit_handler
 * \see _mqx_exit
 * \see _task_get_exception_handler
 * \see _task_set_exception_handler
 * \see _task_abort
 * \see _task_set_error
 */
TASK_EXIT_FPTR _task_get_exit_handler
(
    _task_id        task_id
)
{ /* Body */
    TD_STRUCT_PTR  td_ptr;
    TASK_EXIT_FPTR exit_function;

    td_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);
    if ( td_ptr == NULL )
    {
        _task_set_error(MQX_INVALID_TASK_ID);
        return( NULL );
    }/* Endif */

    exit_function = td_ptr->EXIT_HANDLER_PTR;

    return(exit_function);
} /* Endbody */

/*!
 * \brief Sets the address of the task exit handler for the task.
 *
 * MQX calls a task's task exit handler if either of these conditions is true:
 * \n - Task is terminated with _task_abort().
 * \n - Task returns from its function body (for example, if it calls _mqx_exit()).
 *
 * \param[in] task_id              Task ID of the task whose exit handler is to be set.
 * \param[in] exit_handler_address Pointer to the exit handler for the task.
 *
 * \return Pointer to the previous exit handler (might be NULL).
 * \return NULL (Task_id is not valid.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code to
 * MQX_INVALID_TASK_ID.
 *
 * \see _task_get_exit_handler
 * \see _mqx_exit
 * \see _task_get_exception_handler
 * \see _task_set_exception_handler
 * \see _task_abort
 * \see _task_set_error
 */
TASK_EXIT_FPTR _task_set_exit_handler
(
    _task_id       task_id,
    TASK_EXIT_FPTR exit_handler_address
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    TD_STRUCT_PTR  td_ptr;
    TASK_EXIT_FPTR old_function;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE3(KLOG_task_set_exit_handler, task_id, exit_handler_address);

    td_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);
    if ( td_ptr == NULL )
    {
        _task_set_error(MQX_INVALID_TASK_ID);
        _KLOGX2(KLOG_task_set_exit_handler, NULL);
        return( NULL );
    }/* Endif */

    old_function = td_ptr->EXIT_HANDLER_PTR;
    td_ptr->EXIT_HANDLER_PTR = exit_handler_address;

    _KLOGX2(KLOG_task_set_exit_handler, old_function);
    return(old_function);

} /* Endbody */

#endif /* MQX_HAS_EXIT_HANDLER */

#if MQX_TD_HAS_TASK_TEMPLATE_PTR
/*!
 * \brief Gets the task ID that is associated with the task name.
 *
 * This function uses a task name (from its task template) to find a task id. Only
 * the first task found with the provided name is found.
 *
 * \param[in] name_ptr Pointer to the name to find in the task template list.
 *
 * \return Task ID that is associated with the first match of name_ptr.
 * \return MQX_NULL_TASK_ID (Name is not in the task template list.)
 *
 * \see _task_get_creator
 * \see _task_get_processor
 * \see _task_get_id
 */
_task_id _task_get_id_from_name
(
    char_ptr name_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    TD_STRUCT_PTR           td_ptr;
    _mqx_uint               size;

    _GET_KERNEL_DATA(kernel_data);

    _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    td_ptr = (TD_STRUCT_PTR)((uchar_ptr)kernel_data->TD_LIST.NEXT -
             FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
    size   = _QUEUE_GET_SIZE(&kernel_data->TD_LIST);

    while (size && td_ptr)
    {
        if (strncmp( td_ptr->TASK_TEMPLATE_PTR->TASK_NAME, name_ptr,
                        MQX_MAX_TASK_NAME_SIZE) == 0)
        {
            _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);
            return td_ptr->TASK_ID;
        } /* Endif */
        size--;
        td_ptr = (TD_STRUCT_PTR)((uchar_ptr)(td_ptr->TD_LIST_INFO.NEXT) -
                 FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
    } /* Endwhile */

    _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    return MQX_NULL_TASK_ID;

} /* Endbody */

/*!
 * \brief Gets the pointer to the task template for the task ID.
 *
 * \param[in] task_id Task ID for the task for which to get pointer.
 *
 * \return Pointer to the task's task template. NULL if an invalid task_id is
 * presented.
 *
 * \see _task_get_template_index
 * \see _task_get_index_from_id
 * \see TASK_TEMPLATE_STRUCT
 */
TASK_TEMPLATE_STRUCT_PTR  _task_get_template_ptr
(
    _task_id  task_id
)
{ /* Body */
    TD_STRUCT_PTR  td_ptr;

    td_ptr = _task_get_td(task_id);

    return (td_ptr ? td_ptr->TASK_TEMPLATE_PTR : NULL);

} /* Endbody */

#endif /* MQX_TD_HAS_TASK_TEMPLATE_PTR */

/*!
 * \brief Gets the task template index that is associated with the task name.
 *
 * \param[in] name_ptr Pointer to the name to find in the task template list.
 *
 * \return Task template index that is associated with the first match of name_ptr.
 * \return MQX_NULL_TASK_ID (Name is not in the task template list.)
 *
 * \see _task_get_id_from_name
 * \see _task_get_index_from_id
 */
_mqx_uint  _task_get_template_index
(
    char_ptr name_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    TASK_TEMPLATE_STRUCT_PTR task_template_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    /* Search all the local task templates for one whose index matches */
    task_template_ptr = kernel_data->INIT.TASK_TEMPLATE_LIST;

    while (task_template_ptr->TASK_TEMPLATE_INDEX)
    {
        if (task_template_ptr->TASK_NAME != NULL)
        {
            if (strncmp( task_template_ptr->TASK_NAME, name_ptr,
                            MQX_MAX_TASK_NAME_SIZE) == 0)
            {
                break;
            } /* Endif */
        } /* Endif */
        ++task_template_ptr;
    } /* Endwhile */

    _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    return task_template_ptr->TASK_TEMPLATE_INDEX;

} /* Endwhile */

/*!
 * \brief Gets the task ID out of the task descriptor.
 *
 * \param[in] td_ptr Pointer to the task descriptor.
 *
 * \return TASK_ID Task ID
 * \return MQX_NULL_TASK_ID
 */
_task_id _task_get_id_from_td
(
    pointer td_ptr
)
{
    if (NULL == td_ptr)
    {
        return MQX_NULL_TASK_ID;
    }
    return ((TD_STRUCT_PTR)td_ptr)->TASK_ID;
}

#if MQX_TD_HAS_TASK_TEMPLATE_PTR || MQX_TD_HAS_TEMPLATE_INDEX

/*!
 * \brief Gets the task template index for the task ID.
 *
 * \param[in] task_id Task ID to look up.
 *
 * \return Task template index.
 * \return 0 (Task ID was not found.)
 *
 * \see _task_get_template_index
 */
_mqx_uint _task_get_index_from_id
(
    _task_id task_id
)
{ /* Body */
    TD_STRUCT_PTR           td_ptr;

    td_ptr = _task_get_td(task_id);

    if (td_ptr == NULL)
    {
        return 0;
    } /* Endif */

#if MQX_TD_HAS_TASK_TEMPLATE_PTR
    return td_ptr->TASK_TEMPLATE_PTR->TASK_TEMPLATE_INDEX;
#elif MQX_TD_HAS_TEMPLATE_INDEX
    return td_ptr->TEMPLATE_INDEX;
#else
    return 0;
#endif

} /* Endbody */
#endif /* MQX_TD_HAS_TASK_TEMPLATE_PTR || MQX_TD_HAS_TEMPLATE_INDEX */

/*!
 * \brief Gets the task creation parameter of the active task.
 *
 * If a deeply nested function needs the task creation parameter, it can get the
 * parameter with _task_get_parameter() or _task_get_parameter_for() rather than
 * have the task's main body pass the parameter to it.
 *
 * \return Creation parameter (might be NULL).
 *
 * \see _task_get_parameter_for
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 */
uint_32 _task_get_parameter(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return _task_get_parameter_internal(kernel_data->ACTIVE_PTR);

} /* Endbody */

/*!
 * \brief Gets the task creation parameter of the specified task.
 *
 * If a deeply nested function needs the task creation parameter, it can get the
 * parameter with _task_get_parameter() or _task_get_parameter_for() rather than
 * have the task's main body pass the parameter to it.
 *
 * \param[in] tid Task ID of the task to get creation parameter from.
 *
 * \return Creation parameter (might be NULL).
 *
 * \see _task_get_parameter
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 */
uint_32 _task_get_parameter_for
(
    _task_id  tid
)
{ /* Body */
    TD_STRUCT_PTR td_ptr;

    td_ptr = (TD_STRUCT_PTR)_task_get_td(tid);

    return _task_get_parameter_internal(td_ptr);

} /* Endbody */

/*!
 * \private
 *
 * \brief Gets the task creation parameter of the specified task.
 *
 * \param[in] td_ptr Task ID of the task to get creation parameter from.
 *
 * \return Creation parameter (might be NULL).
 *
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 *
 * \see TD_STRUCT
 */
uint_32 _task_get_parameter_internal
(
    TD_STRUCT_PTR td_ptr
)
{
    PSP_STACK_START_STRUCT_PTR stack_start_ptr;
    stack_start_ptr = _psp_get_stack_start(td_ptr);
    return PSP_TASK_PARAM(stack_start_ptr);
}

/*!
 * \brief Sets the task creation parameter of the active task.
 *
 * \param[in] new_value Value to set the task parameter to.
 *
 * \return Previous creation parameter (might be NULL).
 *
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 * \see _task_set_parameter_for
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 */
uint_32 _task_set_parameter
(
    uint_32 new_value
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    uint_32                    old_value;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_task_set_parameter, new_value);

    old_value = _task_set_parameter_internal(new_value, kernel_data->ACTIVE_PTR);

    _KLOGX2(KLOG_task_set_parameter, old_value);
    return old_value;

} /* Endbody */

/*!
 * \brief Sets the task creation parameter of the specified task.
 *
 * \param[in] new_value Value to set the task parameter to.
 * \param[in] tid       Task ID of the task to set.
 *
 * \return Previous creation parameter (might be NULL).
 *
 * \see _task_get_parameter
 * \see _task_get_parameter_for
 * \see _task_set_parameter
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 */
uint_32 _task_set_parameter_for
(
    uint_32   new_value,
    _task_id  tid
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    TD_STRUCT_PTR           td_ptr;
    uint_32                 old_value;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_task_set_parameter_for, tid, new_value);

    td_ptr = (TD_STRUCT_PTR)_task_get_td(tid);
#if MQX_CHECK_ERRORS
    if (td_ptr == NULL)
    {
        _KLOGX3(KLOG_task_set_parameter_for, tid, MQX_INVALID_PARAMETER);
        return 0;
    } /* Endif */
#endif

    old_value = _task_set_parameter_internal(new_value, td_ptr);

    _KLOGX2(KLOG_task_set_parameter_for, old_value);
    return old_value;

} /* Endbody */

/*!
 * \private
 *
 * \brief Sets the task creation parameter of the specified task.
 *
 * \param[in] new_value Value to set the task parameter to.
 * \param[in] td_ptr    Task ID of the task to set.
 *
 * \return Previous creation parameter (might be NULL).
 *
 * \see _task_set_parameter
 * \see _task_set_parameter_for
 * \see TD_STRUCT
 */
uint_32 _task_set_parameter_internal
(
    uint_32       new_value,
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    PSP_STACK_START_STRUCT_PTR stack_start_ptr;
    uint_32                    old_value;

    stack_start_ptr = _psp_get_stack_start(td_ptr);

    old_value = PSP_TASK_PARAM(stack_start_ptr);
    PSP_TASK_PARAM(stack_start_ptr) = new_value;

    return old_value;

} /* Endbody */

#if MQX_HAS_DYNAMIC_PRIORITIES

/*!
 * \brief Gets the priority of the task.
 *
 * \param[in]  task_id      One of the following:
 * \n - Task ID of the task for which to set or get info.
 * \n - MQX_NULL_TASK_ID (Use the calling task.)
 * \param[out] priority_ptr Pointer to the priority.
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID (Task_id does not represent a currently valid task.)
 *
 * \warning Might dispatch a task.
 *
 * \see _task_set_priority
 * \see _task_get_creator
 * \see _task_get_processor
 * \see _sem_create
 * \see _sem_create_fast
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until
 * \see _mutatr_get_sched_protocol
 * \see _mutatr_set_sched_protocol
 * \see _mutex_lock
 */
_mqx_uint _task_get_priority
(
    _task_id      task_id,
    _mqx_uint_ptr priority_ptr
)
{ /* Body */
    TD_STRUCT_PTR           td_ptr;

    td_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);
    if (td_ptr == NULL)
    {
        return(MQX_INVALID_TASK_ID);
    } /* Endif */
    *priority_ptr = td_ptr->HOME_QUEUE->PRIORITY;
    return MQX_OK;

} /* Endbody */

/*!
 * \brief Sets the priority of the task.
 *
 * MQX might boost the priority of a task that waits for a semaphore or locks a
 * mutex. If MQX has boosted the priority of the task that is specified by task_id,
 * _task_set_priority() will raise but not lower the task's priority.
 * \n If the task is in the blocked state, priority change takes place when task
 * is ready.
 * \n When the task is in the ready state, priority change takes place immediately.
 *
 * \param[in]  task_id      One of the following:
 * \n - Task ID of the task for which to set or get info.
 * \n - MQX_NULL_TASK_ID (Use the calling task.)
 * \param[in]  new_priority New task priority.
 * \param[out] priority_ptr Pointer to the previous task priority.
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID (Task_id does not represent a currently valid task.)
 * \return MQX_INVALID_PARAMETER (New_priority is numerically greater than the
 * lowest-allowable priority of an application task.)
 *
 * \warning Might dispatch a task.
 *
 * \see _task_get_priority
 * \see _task_get_creator
 * \see _task_get_processor
 * \see _sem_create
 * \see _sem_create_fast
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until
 * \see _mutatr_get_sched_protocol
 * \see _mutatr_set_sched_protocol
 * \see _mutex_lock
 */
_mqx_uint _task_set_priority
(
    _task_id     task_id,
    _mqx_uint     new_priority,
    _mqx_uint_ptr priority_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    READY_Q_STRUCT_PTR      ready_q_ptr;
    TD_STRUCT_PTR           td_ptr;
    TASK_QUEUE_STRUCT_PTR   task_queue_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_task_set_priority, task_id, new_priority);

#if MQX_CHECK_ERRORS
    if (new_priority > kernel_data->LOWEST_TASK_PRIORITY)
    {
        _KLOGX2(KLOG_task_set_priority, MQX_INVALID_PARAMETER);
        return(MQX_INVALID_PARAMETER);
    }/* Endif */
#endif

    td_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);
    if (td_ptr == NULL)
    {
        _KLOGX2(KLOG_task_set_priority, MQX_INVALID_TASK_ID);
        return(MQX_INVALID_TASK_ID);
    } /* Endif */

    _int_disable();
    /* Return old priority */
    *priority_ptr = td_ptr->HOME_QUEUE->PRIORITY;

    /* Make the change permanent */
    ready_q_ptr = kernel_data->READY_Q_LIST;
    td_ptr->HOME_QUEUE = ready_q_ptr - new_priority;

    if (td_ptr->BOOSTED)
    {
        /* Can only change priority to a higher (lower value) */
        if (new_priority < td_ptr->MY_QUEUE->PRIORITY)
        {
            /* Move the task to the correct priority level */
            _sched_set_priority_internal(td_ptr, new_priority);
        } /* Endif */
    }
    else
    {
        /* Move the task to the correct priority level */
        _sched_set_priority_internal(td_ptr, new_priority);
    } /* Endif */

    if (td_ptr->STATE == TASK_QUEUE_BLOCKED)
    {
        task_queue_ptr = (TASK_QUEUE_STRUCT_PTR)
                         ((uchar_ptr)td_ptr->INFO - FIELD_OFFSET(TASK_QUEUE_STRUCT, TD_QUEUE));
        if (task_queue_ptr->POLICY & MQX_TASK_QUEUE_BY_PRIORITY)
        {
            /* Requeue the td by priority */
            _QUEUE_REMOVE(&task_queue_ptr->TD_QUEUE, td_ptr);
            _sched_insert_priorityq_internal(&task_queue_ptr->TD_QUEUE, td_ptr);
        }/* Endif */
    }/* Endif */

    /* Allow higher priority tasks to run */
    _CHECK_RUN_SCHEDULER();

    _int_enable();

    _KLOGX2(KLOG_task_set_priority, MQX_OK);
    return MQX_OK;

} /* Endbody */

#endif /* MQX_HAS_DYNAMIC_PRIORITIES */

/*!
 * \brief Makes the task ready to run by putting it in its ready queue.
 *
 * This function is the only way to make ready a task that called _task_block().
 *
 * \param[in] td Pointer to the task descriptor of the task (on this processor)
 * to be made ready.
 *
 * \warning If the new ready task has higher priority than the calling task,
 * MQX makes the new ready task active.
 * \warning Might set one of the following task error codes:
 * \n - MQX_INVALID_TASK_ID (Task_id is not valid for this processor.)
 * \n - MQX_INVALID_TASK_STATE (Task is already in its ready queue.)
 *
 * \see _task_block
 * \see _time_dequeue
 * \see _taskq_resume
 */
void _task_ready
(
    pointer td
)
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    TD_STRUCT_PTR           td_ptr = (TD_STRUCT_PTR)td;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        _usr_task_ready(td);
        return;
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    /* Perform a validity check on the td */
#if MQX_CHECK_ERRORS
    if (PROC_NUMBER_FROM_TASKID(td_ptr->TASK_ID) != kernel_data->INIT.PROCESSOR_NUMBER)
    {
        _KLOGE2(KLOG_task_ready, td_ptr);
        _task_set_error(MQX_INVALID_TASK_ID);
        _KLOGX1(KLOG_task_ready);
        return;
    }
#endif

    _INT_DISABLE();

    if (td_ptr->STATE == READY)
    {
        /* The task is already on the ready to run Queue! */
        _int_enable();
        _KLOGE2(KLOG_task_ready, td_ptr);
        _task_set_error(MQX_INVALID_TASK_STATE);
        _KLOGX1(KLOG_task_ready);
        return;
    }/* Endif */

    _task_ready_internal(td_ptr);

    _INT_ENABLE();

    _CHECK_RUN_SCHEDULER(); /* Let higher priority task run */
}

#if MQX_ENABLE_USER_MODE

/*!
 * \brief Makes the task ready to run by putting it in its ready queue.
 *
 * This function is an equivalent to the _task_ready() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] td Pointer to the task descriptor of the task (on this processor)
 * to be made ready.
 *
 * \warning If the new ready task has higher priority than the calling task,
 * MQX makes the new ready task active.
 * \warning Might set one of the following task error codes:
 * \n - MQX_INVALID_TASK_ID (Task_id is not valid for this processor.)
 * \n - MQX_INVALID_TASK_STATE (Task is already in its ready queue.)
 *
 * \see _task_ready
 */
void _usr_task_ready
(
    pointer td
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)td, 0, 0, 0, 0};
    _mqx_api_call(MQX_API_TASK_READY, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

#if MQX_TD_HAS_TASK_TEMPLATE_PTR

/*!
 * \brief Restart the specified task.
 *
 * Restart the task specified by the given task_id (the victim).
 * \n All of the victim's resources are released, specifically all queues are
 * closed and all memory is freed.
 * \n Component cleanup functions are called to free any component resources
 * owned by this task.
 *
 * \param[in] task_id   Task ID of the task to restart.
 * \param[in] param_ptr One of the following:
 * \n - Pointer to a new task creation parameter.
 * \n - NULL (victim's creation parameter used).
 * \param[in] blocked   Whether the task should be restarted in the blocked state or not.
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_TASK_ID (Task_id is invalid.)
 * \return MQX_OUT_OF_MEMORY (Not enough memory to rebuild stack.)
 *
 * \warning Cannot be called from an ISR.
 *
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 */
static _mqx_uint _task_restart_func
(
    _task_id    task_id,
    uint_32_ptr param_ptr,
    boolean     blocked
)
{ /* Body */
    PSP_STACK_START_STRUCT_PTR  stack_start_ptr;
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    TASK_TEMPLATE_STRUCT_PTR    template_ptr;
    TD_STRUCT_PTR               victim_ptr;
    TD_STRUCT_PTR               td_ptr;
    READY_Q_STRUCT_PTR          ready_q_ptr;
    char_ptr                    stack_ptr;
    uint_32                     create_param;
    _processor_number           processor;
    _mem_size                   stack_size;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_task_restart, task_id);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        /* Cannot be called from an ISR */
        _KLOGX2(KLOG_task_restart, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    if (task_id != MQX_NULL_TASK_ID)
    {
        processor = PROC_NUMBER_FROM_TASKID(task_id);
        if (processor != (_processor_number)kernel_data->INIT.PROCESSOR_NUMBER )
        {
#if MQX_IS_MULTI_PROCESSOR
            if ( kernel_data->IPC != NULL )
            {
                _KLOGX2(KLOG_task_restart, MQX_OK);
                return( (*kernel_data->IPC)(FALSE, processor,
                                KERNEL_MESSAGES, IPC_TASK_RESTART, 3, (_mqx_uint)task_id,
                                (_mqx_uint)param_ptr, (_mqx_uint)blocked));
            }
            else
            {
#endif
                _KLOGX2(KLOG_task_restart, MQX_INVALID_TASK_ID);
                return(MQX_INVALID_TASK_ID);
#if MQX_IS_MULTI_PROCESSOR
            } /* Endif */
#endif
        }/* Endif */
    }/* Endif */

    victim_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);

#if MQX_CHECK_ERRORS
    if (victim_ptr == NULL)
    {
        _KLOGX2(KLOG_task_restart, MQX_INVALID_TASK_ID);
        return(MQX_INVALID_TASK_ID);
    } /* Endif */

    if (victim_ptr == SYSTEM_TD_PTR(kernel_data))
    {
        _KLOGX2(KLOG_task_restart, MQX_INVALID_TASK_ID);
        return(MQX_INVALID_TASK_ID);
    } /* Endif */
#endif

    /* First, serialize task creation/destruction/restart */
    _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    _int_disable();
    /* remove the victim from any queues it may be in. */
    if (victim_ptr->STATE == READY)
    {
        if (victim_ptr != kernel_data->ACTIVE_PTR)
        {
            /* Remove other task from ready queue */
            _QUEUE_UNLINK(victim_ptr);
        } /* Endif */
    }
    else if (victim_ptr->STATE & IS_ON_TIMEOUT_Q)
    {
        /* Remove from time queue */
        _TIME_DEQUEUE(victim_ptr, kernel_data);
    }
    else if (victim_ptr->STATE & TD_IS_ON_QUEUE)
    {
        /* Remove from queue, where suspended
         * eg, MUTEX_BLOCKED, IO_BLOCKED, TASKQ_BLOCKED
         */
        _QUEUE_REMOVE(victim_ptr->INFO, victim_ptr);
    } /* Endif */

    if (victim_ptr->STATE & TD_IS_ON_AUX_QUEUE)
    {
        /*
         * Remove from queue, where suspended
         * eg, LWSEM_BLOCKED
         * (uses AUX_QUEUE field)
         */
        _QUEUE_REMOVE(victim_ptr->INFO, &victim_ptr->AUX_QUEUE);
    } /* Endif */

    victim_ptr->STATE   = DYING;

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
    /* Stop Floating point context monitoring */
    if (kernel_data->FP_ACTIVE_PTR == victim_ptr)
    {
        kernel_data->FP_ACTIVE_PTR = NULL;
    }/* Endif */
#endif

    _int_enable();

#if MQX_COMPONENT_DESTRUCTION
    {
        _mqx_uint   i;
        for (i = 0; i < MAX_KERNEL_COMPONENTS; ++i)
        {
            if (kernel_data->COMPONENT_CLEANUP[i] != NULL)
            {
                (*kernel_data->COMPONENT_CLEANUP[i])(victim_ptr);
            } /* Endif */
        } /* Endfor */
    }
#endif

    td_ptr = kernel_data->ACTIVE_PTR;
#if !MQX_LITE_VERSION_NUMBER
    {
        pointer                     block_ptr;
        block_ptr = _mem_get_next_block_internal(victim_ptr, NULL);
        while (block_ptr != NULL)
        {
            if (block_ptr           == victim_ptr ||
                MEM_TYPE_TD         == GET_MEMBLOCK_TYPE(block_ptr) ||
                MEM_TYPE_TASK_STACK == GET_MEMBLOCK_TYPE(block_ptr))
            {

                block_ptr = _mem_get_next_block_internal (victim_ptr, block_ptr);
                if (block_ptr == victim_ptr)
                    break;
            }
            else
            {
                _mem_transfer_td_internal(block_ptr, victim_ptr, td_ptr);
                _mem_free(block_ptr);
                block_ptr = _mem_get_next_block_internal (victim_ptr, NULL);
            }
        }
    }
#endif /* MQX_LITE_VERSION_NUMBER */

    /* Free up create/destroy/restart for other tasks */
    _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    template_ptr    = victim_ptr->TASK_TEMPLATE_PTR;
    stack_start_ptr = _psp_get_stack_start(victim_ptr);
    if (param_ptr == NULL)
    {
        create_param = stack_start_ptr->PARAMETER;
    }
    else
    {
        create_param = *param_ptr;
    } /* Endif */

    stack_ptr = victim_ptr->STACK_ALLOC_BLOCK; /* get mem allocated block for stack */

    if (stack_ptr == (pointer)template_ptr)
    {
        /* task has been created with temporary template, copy of template is before stack in allocated block for stack */
            stack_ptr +=  sizeof(TASK_TEMPLATE_STRUCT);
    }

    stack_size = template_ptr->TASK_STACKSIZE;
    if ( stack_size <= PSP_MINSTACKSIZE  )
    {
        stack_size = PSP_MINSTACKSIZE;
    } /* Endif */

    /* Make the size a multiple of the memory alignment */
    _MEMORY_ALIGN_VAL_LARGER(stack_size);

    ready_q_ptr = kernel_data->READY_Q_LIST - template_ptr->TASK_PRIORITY;
    victim_ptr->MY_QUEUE = ready_q_ptr;
#if MQX_HAS_DYNAMIC_PRIORITIES
    victim_ptr->HOME_QUEUE  = ready_q_ptr;
#endif
    victim_ptr->TASK_SR     = ready_q_ptr->ENABLE_SR;
    victim_ptr->FLAGS       = template_ptr->TASK_ATTRIBUTES;

#if MQX_ENABLE_USER_MODE
    if (template_ptr->TASK_ATTRIBUTES & MQX_USER_TASK)
    {
        victim_ptr->FLAGS |= TASK_USER_MODE;
    }
#endif /* MQX_ENABLE_USER_MODE */

#if MQX_HAS_TIME_SLICE
    /* Reset the time slice back to the default */
    if (template_ptr->DEFAULT_TIME_SLICE)
    {
#if (MQX_DEFAULT_TIME_SLICE_IN_TICKS == 0)
        uint_32 ticks;

        ticks = ((template_ptr->DEFAULT_TIME_SLICE * 2 *
                        kernel_data->TICKS_PER_SECOND) / 1000) / 2  /* Rounding.. */;
        PSP_ADD_TICKS_TO_TICK_STRUCT(&td_ptr->TIME_SLICE,
            ticks, &td_ptr->TIME_SLICE);
#else
        PSP_ADD_TICKS_TO_TICK_STRUCT(&victim_ptr->TIME_SLICE,
            template_ptr->DEFAULT_TIME_SLICE, &victim_ptr->TIME_SLICE);
#endif
    }
    else
    {
        victim_ptr->TIME_SLICE = kernel_data->SCHED_TIME_SLICE;
    } /* Endif */
    /* Reset scheduling policy for task */
    if (kernel_data->SCHED_POLICY == MQX_SCHED_RR)
    {
        victim_ptr->FLAGS |= MQX_TIME_SLICE_TASK;
    } /* Endif */
#endif

#if MQX_USE_IO
    /* Reset the io streams back to the default */
    victim_ptr->STDIN_STREAM  = kernel_data->PROCESSOR_STDIN;
    victim_ptr->STDOUT_STREAM = kernel_data->PROCESSOR_STDOUT;
    victim_ptr->STDERR_STREAM = kernel_data->PROCESSOR_STDERR;
#endif

    _int_disable();

    if (victim_ptr == td_ptr)
    {
        _QUEUE_UNLINK(victim_ptr);  /* Remove victim from the queue */
    } /* Endif */

    /* Rebuild the task's initial context */
    if (FALSE == _psp_build_stack_frame(victim_ptr, stack_ptr, stack_size, template_ptr, victim_ptr->TASK_SR, create_param))
    {
        /* build stack failed */
        return MQX_OUT_OF_MEMORY;
    }

    if (blocked)
    {
        victim_ptr->STATE = BLOCKED;
    }
    else
    {
        _task_ready_internal(victim_ptr);
    } /* Endif */

    victim_ptr->DISABLED_LEVEL = 0;

    if (victim_ptr == td_ptr)
    {
        /*
         * The active task is to be restarted.
         * Will not return from here
         */
        _sched_run_internal();
    } /* Endif */

    /* The victim has been added to the ready Q. Preemption check must be made */
    if (kernel_data->CURRENT_READY_Q != td_ptr->MY_QUEUE)
    {
        _sched_execute_scheduler_internal();
    } /* Endif */

    _int_enable();

    _KLOGX2(KLOG_task_restart, MQX_OK);
    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Restart the specified task.
 *
 * This function closes all queues that the task has open, releases all the task's
 * resources, and frees all memory that is associated with the task's resources.
 * \n This function restarts the task with the same task descriptor, task ID, and
 * task stack (Reserve stack for Stack Start Structure and call the
 * _task_restart_func().).
 *
 * \param[in] task_id   Task ID of the task to restart.
 * \param[in] param_ptr One of the following:
 * \n - Pointer to a new task creation parameter.
 * \n - NULL
 * \param[in] blocked   Whether the task should be restarted in the blocked state or not.
 *
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_TASK_ID (Task_id is invalid.)
 * \return MQX_OUT_OF_MEMORY (Not enough memory to restart function.)
 *
 * \warning Cannot be called from an ISR.
 *
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 */
_mqx_uint _task_restart
(
    _task_id    task_id,
    uint_32_ptr param_ptr,
    boolean     blocked
)
{ /* Body */
    /* reserve memory in stack */
    volatile char dummy_stack_start_struct[sizeof(PSP_STACK_START_STRUCT)];
    dummy_stack_start_struct[0] = 0x00;

    /* call the right _task_restart function */
    return(_task_restart_func(task_id, param_ptr, blocked));
}

#endif /* MQX_TD_HAS_TASK_TEMPLATE_PTR */

/*!
 * \brief Disables preemption of the current task.
 *
 * The _task_stop_preemption() function disables preemption of the active task
 * unless the task blocks explicitly (_task_block()) or calls _task_start_preemption().
 *
 * \warning Changes the preemption ability of tasks.
 * \warning Interrupts are still handled.
 *
 * \see _task_start_preemption
 * \see _task_ready
 * \see _task_block
 */
void _task_stop_preemption(void)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR                   td_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE1(KLOG_task_stop_preemption);

    td_ptr      = kernel_data->ACTIVE_PTR;
    _int_disable();
    td_ptr->FLAGS |= TASK_PREEMPTION_DISABLED;
    _int_enable();
    _KLOGX1(KLOG_task_stop_preemption);

} /* Endbody */

/*!
 * \brief Enables preemption of the current task.
 *
 * The _task_start_preemption() function enables preemption of the active task
 * after _task_stop_preemption() was called.
 *
 * \warning Changes the preemption ability of tasks.
 * \warning Interrupts are still handled.
 *
 * \see _task_stop_preemption
 * \see _task_ready
 * \see _task_block
 */
void _task_start_preemption(void)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR                   td_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE1(KLOG_task_start_preemption);

    td_ptr      = kernel_data->ACTIVE_PTR;
    _int_disable();
    td_ptr->FLAGS &= ~TASK_PREEMPTION_DISABLED;
    _CHECK_RUN_SCHEDULER(); /* Allow higher priority tasks to run */
    _int_enable();

    _KLOGX1(KLOG_task_start_preemption);

} /* Endbody */

/*!
 * \brief Determines whether the active task's stack is currently overflowed.
 *
 * This function indicates whether the stack is currently past its limit. The
 * function does not indicate whether the stack previously passed its limit.
 *
 * \return TRUE (Stack is out of bounds.)
 * \return FALSE (Stack is not out of bounds.)
 *
 * \see _task_set_error
 */
boolean _task_check_stack(void)
{ /* Body */
#if MQX_TD_HAS_STACK_LIMIT
    register  KERNEL_DATA_STRUCT_PTR kernel_data;
    volatile  char                   loc = (char)0;

    _GET_KERNEL_DATA(kernel_data);

#if PSP_STACK_GROWS_TO_LOWER_MEM
    if ( &loc < ((char_ptr)kernel_data->ACTIVE_PTR->STACK_LIMIT) )
    {
#else
        if ( &loc > ((char_ptr)kernel_data->ACTIVE_PTR->STACK_LIMIT) )
        {
#endif
            return (TRUE);
        }
        else
        {
            return (FALSE);
        } /* Endif */
#else
        return (FALSE);
#endif
} /* Endbody */

#if !MQX_LITE_VERSION_NUMBER

/*!
 * \brief Gets the processor number of the task's home processor.
 *
 * This function returns the processor-number portion of task_id. It cannot check
 * the validity of task_id because MQX on one processor is unaware of which tasks
 * might reside on another processor.
 *
 * \param[in] task_id Task ID of the task for which to get info.
 *
 * \return Processor number of the processor where the task resides.
 *
 * \see _task_get_id
 */
_processor_number _task_get_processor
(
    _task_id task_id
)
{ /* Body */

    return PROC_NUMBER_FROM_TASKID(task_id);

} /* Endbody */

#endif /* MQX_LITE_VERSION_NUMBER */
#if MQX_TD_HAS_PARENT

/*!
 * \brief Gets parent's task ID to the calling task.
 *
 * \return Task ID of the parent task.
 *
 * \see _task_get_processor
 * \see _task_get_id
 */
_task_id _task_get_creator(void)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return( kernel_data->ACTIVE_PTR->PARENT );

} /* Endbody */
#endif /* MQX_TD_HAS_PARENT */

#if MQX_TD_HAS_ERROR_CODE

/*!
 * \brief Gets the task error code.
 *
 * \return Task error code for the active task.
 *
 * \see _task_get_error_ptr
 * \see _task_set_error
 * \see _task_errno
 */
_mqx_uint _task_get_error(void)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    if (kernel_data->IN_ISR)
    {
        return( kernel_data->INTERRUPT_CONTEXT_PTR->ERROR_CODE );
    }
    else
    {
        return( kernel_data->ACTIVE_PTR->TASK_ERROR_CODE );
    } /* Endif */
} /* Endbody */

/*!
 * \brief Gets a pointer to the task error code.
 *
 * \return Pointer to the task error code.
 *
 * \warning If a task writes to the pointer that _task_get_error_ptr() returns,
 * the task error code is changed to the value, overwriting any previous error code. To
 * avoid overwriting a previous error code, a task should use _task_set_error().
 *
 * \see _task_get_error
 * \see _task_set_error
 * \see _task_errno
 */
_mqx_uint _PTR_ _task_get_error_ptr(void)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    if (kernel_data->IN_ISR)
    {
        return( &kernel_data->INTERRUPT_CONTEXT_PTR->ERROR_CODE );
    }
    else
    {
        return( &kernel_data->ACTIVE_PTR->TASK_ERROR_CODE );
    } /* Endif */
} /* Endbody */
#endif /* MQX_TD_HAS_ERROR_CODE */

/*!
 * \brief Gets the task ID of the active task.
 *
 * \return Task ID of the active task.
 *
 * \see _task_get_creator
 * \see _task_get_processor
 * \see _task_get_id_from_name
 */
_task_id _task_get_id(void)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return( kernel_data->ACTIVE_PTR->TASK_ID );
} /* Endbody */

/*!
 * \brief Gets a pointer to the task descriptor for the task ID.
 *
 * \param[in] task_id One of:
 * \n - Task ID for a task on this processor.
 * \n - MQX_NULL_TASK_ID (Use the current task.)
 *
 * \return Pointer to the task descriptor for task_id.
 * \return NULL (Task_id is not valid for this processor.)
 *
 * \see _task_ready
 */
pointer _task_get_td
(
    _task_id task_id
)
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    TD_STRUCT_PTR           td_ptr;
    _mqx_uint               size;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_task_get_td(task_id);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);
    if (task_id == MQX_NULL_TASK_ID)
    {
        return((pointer)kernel_data->ACTIVE_PTR);
    }

    /* check task_id for SYSTEM_TASK_ID */
    if (task_id == kernel_data->SYSTEM_TD.TASK_ID)
    {
        return((pointer)&(kernel_data->SYSTEM_TD));
    }

    /* SPR P171-0022-01 Use int disable, not a semaphore */
    _INT_DISABLE();
    /* END SPR */

    td_ptr = (TD_STRUCT_PTR)((uchar_ptr)kernel_data->TD_LIST.NEXT - FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));

    size   = _QUEUE_GET_SIZE(&kernel_data->TD_LIST);
    while (size--)
    {
        /* END SPR */
        if (td_ptr->TASK_ID == task_id)
        {
            /* SPR P171-0022-01 Use int disable, not a semaphore */
            _INT_ENABLE();
            /* END SPR */
            return (pointer)td_ptr;
        } /* Endif */
        td_ptr = (TD_STRUCT_PTR)((uchar_ptr)td_ptr->TD_LIST_INFO.NEXT - FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
    } /* Endwhile */

    /* SPR P171-0022-01 Use int disable, not a semaphore */
    _int_enable();
    /* END SPR */

    return NULL;
}

#if MQX_ENABLE_USER_MODE

/*!
 * \brief Gets a pointer to the task descriptor for the task ID.
 *
 * This function is an equivalent to the _task_get_td() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] task_id One of:
 * \n - Task ID for a task on this processor.
 * \n - MQX_NULL_TASK_ID (Use the current task.)
 *
 * \return Pointer to the task descriptor for task_id.
 * \return NULL (Task_id is not valid for this processor.)
 *
 * \see _task_get_td
 * \see _usr_task_ready
 */
pointer _usr_task_get_td
(
    _task_id task_id
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)task_id, 0, 0, 0, 0};
    return (pointer)_mqx_api_call(MQX_API_TASK_GET_TD, &params);
}

#endif /* MQX_ENABLE_USER_MODE */
/*!
 * \brief Sets the task error code.
 *
 * MQX uses this function to indicate an error. MQX never sets the task error code
 * to MQX_OK; that is, MQX does not reset the task error code. It is the responsibility
 * of the application to reset the task error code. As a result, when an application
 * calls _task_get_error(), it gets the first error that MQX detected since the
 * last time the application reset the task error code.
 *
 * <table>
 *  <tr>
 *      <td><b> If the current task error code is: </b></td>
 *      <td><b> Function changes the task error code: </b></td>
 *  </tr>
 *  <tr>
 *      <td> MQX_OK </td>
 *      <td> To new_error_code. </td>
 *  </tr>
 *  <tr>
 *      <td> Not MQX_OK </td>
 *      <td> To new_error_code if new_error_code is MQX_OK. </td>
 *  </tr>
 * </table>
 *
 * \n If this function is called from an ISR, the function sets the interrupt error code.
 *
 * \param[in] new_error_code New task error code.
 *
 * \return Previous task error code.
 *
 * \see _task_check_stack
 * \see _task_get_error
 * \see _task_get_error_ptr
 * \see _task_errno
 */
_mqx_uint _task_set_error
(
    _mqx_uint new_error_code
)
{ /* Body */
#if MQX_TD_HAS_ERROR_CODE
    register KERNEL_DATA_STRUCT_PTR  kernel_data;
    register _mqx_uint               old_error_code;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_task_set_error, new_error_code);

    /* get the old value */
    if (kernel_data->IN_ISR)
    {
        old_error_code = kernel_data->INTERRUPT_CONTEXT_PTR->ERROR_CODE;
    }
    else
    {
        old_error_code = kernel_data->ACTIVE_PTR->TASK_ERROR_CODE;
    } /* Endif */

    if (( new_error_code == MQX_OK ) || ( old_error_code == MQX_OK ))
    {
        if (kernel_data->IN_ISR)
        {
            kernel_data->INTERRUPT_CONTEXT_PTR->ERROR_CODE = new_error_code;
        }
        else
        {
            kernel_data->ACTIVE_PTR->TASK_ERROR_CODE = new_error_code;
        } /* Endif */
    } /* Endif */

    _KLOGX2(KLOG_task_set_error, old_error_code);
    return( old_error_code );
#else
    return MQX_OK;
#endif

}

#if MQX_ENABLE_USER_MODE

/*!
 * \brief Sets the task error code.
 *
 * This function is an equivalent to the _task_set_error() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] new_error_code New task error code.
 *
 * \return Previous task error code.
 *
 * \see _task_set_error
 */
_task_id _usr_task_set_error
(
    _mqx_uint new_error_code
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)new_error_code, 0, 0, 0, 0};
    return _mqx_api_call(MQX_API_TASK_SET_ERROR, &params);
}

#endif /* MQX_ENABLE_USER_MODE */
/*!
 * \private
 *
 * \brief Creates a new task of the type specified by the task template number,
 * but not make it ready to run.
 *
 * \param[in] template_index Task template index number for this task.
 * \param[in] parameter      The parameter to pass to the newly created task.
 * \param[in] stack_ptr      Pointer to the stack (if not NULL).
 * \param[in] stack_size     Stack size if provided by the application.
 * \param[in] user           User mode functionality.
 *
 * \return Pointer to the task descriptor for created task.
 * \return NULL
 *
 * \see TD_STRUCT
 */
TD_STRUCT_PTR _task_build_internal
(
    _mqx_uint     template_index,
    uint_32       parameter,
    pointer       stack_ptr,
    _mqx_uint     stack_size,
    boolean       user
)
{
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    TD_STRUCT_PTR            td_ptr;
    TASK_TEMPLATE_STRUCT_PTR task_template_ptr;
    boolean                  dynamic;
    uint_32                  create_parameter;

    _GET_KERNEL_DATA(kernel_data);

    if ( template_index )
    {
        dynamic = FALSE;
        create_parameter = parameter;

        /* Search all the local task templates for one whose index matches */
        task_template_ptr = kernel_data->INIT.TASK_TEMPLATE_LIST;

        while ( task_template_ptr->TASK_TEMPLATE_INDEX &&
                        (task_template_ptr->TASK_TEMPLATE_INDEX != template_index) )
        {
            ++task_template_ptr;
        } /* Endwhile */
        if (task_template_ptr->TASK_TEMPLATE_INDEX == 0)
        {
            /* Task not found */
            task_template_ptr = NULL;
        } /* Endif */
    }
    else
    {
        task_template_ptr = (TASK_TEMPLATE_STRUCT_PTR)parameter;
        create_parameter  = task_template_ptr->CREATION_PARAMETER;
        dynamic = TRUE;
    } /* Endif */

#if MQX_CHECK_ERRORS
    if (task_template_ptr == NULL)
    {
        _task_set_error(MQX_NO_TASK_TEMPLATE);
        return NULL;
    } /* Endif */
#endif


#if MQX_ENABLE_USER_MODE
    /* check template for correct user task parameters */
    if (user)
    {
        if (!(task_template_ptr->TASK_ATTRIBUTES & MQX_USER_TASK))
        {
            _task_set_error(MQX_INVALID_PARAMETER);
            return NULL;
        }

        if (kernel_data->INIT.MAX_USER_TASK_PRIORITY && kernel_data->INIT.MAX_USER_TASK_PRIORITY > task_template_ptr->TASK_PRIORITY)
        {
            _task_set_error(MQX_INVALID_TASK_PRIORITY);
            return NULL;
        }
    }
#endif

    /* serialize task creation/destruction */
    _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    /* Create the task, but do not ready it */
    td_ptr = _task_init_internal(task_template_ptr, kernel_data->ACTIVE_PTR->TASK_ID, \
                    create_parameter, dynamic, stack_ptr, stack_size);

    /* Allow other tasks to create */
    _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->TASK_CREATE_LWSEM);

    return(td_ptr);

} /* Endbody */


/*! \private */
extern void _task_exiting_function_internal(void);  /* Dummy prototype */
/*!
 * \private
 *
 * \brief Dummy function so debuggers will display stack correctly.
 */
void _task_exiting_function_internal(void){}

/*!
 * \private
 *
 * \brief Executes the exit handler for the current task, then destroys the current task.
 */
void _task_exit_function_internal(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_task_exit_function, kernel_data->ACTIVE_PTR->TASK_ID);

    td_ptr = kernel_data->ACTIVE_PTR;
#if MQX_TASK_DESTRUCTION

#if MQX_HAS_EXIT_HANDLER
    if (td_ptr->EXIT_HANDLER_PTR != NULL)
    {
        (*td_ptr->EXIT_HANDLER_PTR)();
    }
#endif
    (void)_task_destroy(MQX_NULL_TASK_ID); /* Never returns */
#else
    td_ptr->STATE = DYING;
    while(TRUE)
    {
        _task_block();
    }
#endif /* MQX_TASK_DESTRUCTION */
}

#if MQX_EXTRA_TASK_STACK_ENABLE
#if !MQX_LITE_VERSION_NUMBER
/*!
 * \brief Gets the base address of the space reserved for the active task.
 *
 * \return TD_STRUCT.TOS_RESERVED
 * \return 0 (MQX is currently in ISR.)
 *
 * \see _task_reserve_space
 */
pointer _task_get_reserved_base(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _GET_KERNEL_DATA(kernel_data);

    if (kernel_data->IN_ISR)
    {
        return 0;
    } /* Endif */

    return kernel_data->ACTIVE_PTR->TOS_RESERVED;

} /* Endbody */
#endif /* MQX_LITE_VERSION_NUMBER */
#endif /* MQX_EXTRA_TASK_STACK_ENABLE */

/*!
 * \private
 *
 * \brief Creates task for given task template.
 *
 * \param[in] template_ptr     Task template for the new task.
 * \param[in] creator_task_id  Task ID of the creating task.
 * \param[in] create_parameter Creation parameter for the task.
 * \param[in] dynamic          TRUE (Template_ptr is a dynamic template.), FALSE
 * (Template_ptr is not a dynamic template.)
 * \param[in] input_stack_ptr  If not NULL, the location of the stack if provided.
 * \param[in] input_stack_size Input stack size if provided by the application.
 *
 * \return Pointer to the created task's descriptor.
 * \return NULL (Failure)
 *
 * \see TASK_TEMPLATE_STRUCT
 * \see TD_STRUCT
 */
TD_STRUCT_PTR _task_init_internal
(
    TASK_TEMPLATE_STRUCT_PTR template_ptr,
    _task_id                 creator_task_id,
    uint_32                  create_parameter,
    boolean                  dynamic,
    pointer                  input_stack_ptr,
    _mem_size                input_stack_size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    TD_STRUCT_PTR            td_ptr;
    READY_Q_STRUCT_PTR       ready_q_ptr;
    TASK_TEMPLATE_STRUCT_PTR new_template_ptr;
#if MQX_USE_LWMEM_ALLOCATOR == 0 && !MQX_LITE_VERSION_NUMBER
    STOREBLOCK_STRUCT_PTR    block_ptr;
#endif

    char_ptr                 stack_ptr;
    _mqx_uint                stack_size;
    _mqx_uint                td_size;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (template_ptr->TASK_PRIORITY > kernel_data->LOWEST_TASK_PRIORITY)
    {
#if MQX_USE_IDLE_TASK
        if (template_ptr != &kernel_data->IDLE_TASK_TEMPLATE)
        {
#endif /* MQX_USE_IDLE_TASK */

            _task_set_error(MQX_INVALID_TASK_PRIORITY);
            return (NULL);

#if MQX_USE_IDLE_TASK
        }
#endif /* MQX_USE_IDLE_TASK */
    }
#endif /* MQX_CHECK_ERRORS */


    /*
     * Calculate space for the stack. Note that the stack size
     * must be no smaller than a specified minimum, and it
     * is made to be a multiple of longwords. The space allocated
     * for the stack includes the specified stacksize and the
     * overhead required by the kernel.
     */
    if (input_stack_ptr)
    {
        /* force stack size, ignore stack size from template */
        stack_size = input_stack_size;
#if MQX_CHECK_ERRORS
        if (stack_size <= (PSP_MINSTACKSIZE+sizeof(TD_STRUCT)))
        {
            _task_set_error(MQX_INVALID_SIZE);
            return(NULL);
        } /* Endif */
#endif
        td_ptr = _task_alloc_td_internal(stack_size, &td_size, input_stack_ptr, template_ptr->TASK_ATTRIBUTES & MQX_USER_TASK);
        stack_size -= td_size;

        if (dynamic)
        {
            stack_size -=  sizeof(TASK_TEMPLATE_STRUCT);
        }

    }
    else
    {
        /* take stack size from template */
        stack_size = template_ptr->TASK_STACKSIZE;
#if MQX_CHECK_ERRORS
        if ( stack_size <= PSP_MINSTACKSIZE  )
        {
            stack_size = PSP_MINSTACKSIZE;
        } /* Endif */
#endif
        /* Make the size a multiple of the memory alignment */
        _STACK_ALIGN_VAL_LARGER(stack_size);

#if 0 /* we dont need this, because we using _mem_alloc_align function in _task_alloc_td_internal */
#if PSP_MEMORY_ALIGNMENT
        /* But we need to add size to allow for alignment of stack base */
        stack_size += PSP_STACK_ALIGNMENT + 1;
#endif
#endif

        /*
         * Allocate space for a task descriptor and stack.
         * If there is none available,
         * then indicate that the create failed.
         */
        /* function allocate task descriptor, stack and optionally increase stack block size by size required for task template (dynamic = nonzero) */
        td_ptr = _task_alloc_td_internal(dynamic ? (stack_size +  sizeof(TASK_TEMPLATE_STRUCT)) : stack_size,
                                                 &td_size, NULL, template_ptr->TASK_ATTRIBUTES & MQX_USER_TASK);
    }/* Endif */


#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (td_ptr == NULL)
    {
        _task_set_error(MQX_OUT_OF_TASK_DESCRIPTORS);
        return (NULL);
    } /* Endif */
#endif
    td_ptr->STATE = BLOCKED;

    /* get begin of new stack */
    if (td_ptr->STACK_ALLOC_BLOCK)
    {
        /* (stack has been allocated) */
        /* stack start in new allocated block */
        stack_ptr = td_ptr->STACK_ALLOC_BLOCK;
    }
    else
    {
        /* (this will be executed if user provide input_stack_size) */
        /* stack start in provided block behind TD */
        stack_ptr = (char_ptr)td_ptr + td_size;
    }

    if (dynamic)
    {
        /* task is creating from temporary template, store template before stack (in stack allocated memory) */
        new_template_ptr = (pointer)stack_ptr;
        stack_ptr +=  sizeof(TASK_TEMPLATE_STRUCT);
        *new_template_ptr = *template_ptr;
        template_ptr = new_template_ptr;
        template_ptr->TASK_TEMPLATE_INDEX |= SYSTEM_TASK_FLAG;
        /* stack_ptr is moved behind template */
    }

#if MQX_MONITOR_STACK
    /* fill stack with mark for TAD stack overflow functionality */
    _task_fill_stack_internal((_mqx_uint_ptr)stack_ptr, stack_size);
#endif

    /*  Find the ready_q that the new task belongs to.  */
    /* The ready queues are stored as a array as follows:
     *     lowest priority - eg 9
     *                          8
     *                        ...
     *     highest priority     0
     * With the READY_Q_LIST pointer pointing to the highest priority queue
     */
    ready_q_ptr = kernel_data->READY_Q_LIST - template_ptr->TASK_PRIORITY;
    td_ptr->MY_QUEUE = ready_q_ptr;
#if MQX_HAS_DYNAMIC_PRIORITIES
    td_ptr->HOME_QUEUE  = ready_q_ptr;
#endif
    td_ptr->TASK_SR     = ready_q_ptr->ENABLE_SR;
    td_ptr->FLAGS       = template_ptr->TASK_ATTRIBUTES;

#if MQX_ENABLE_USER_MODE
    if (template_ptr->TASK_ATTRIBUTES & MQX_USER_TASK)
    {
        td_ptr->FLAGS |= TASK_USER_MODE;
    }
#endif /* MQX_ENABLE_USER_MODE */

    if (input_stack_ptr)
    {
        td_ptr->FLAGS |= TASK_STACK_PREALLOCATED;
    } /* Endif */

    /* create stack frame - initialize stack parameters in TD and stack itself (registers required by scheduller, etc.) */
    if (FALSE == _psp_build_stack_frame(td_ptr, stack_ptr, stack_size, template_ptr, (_mqx_uint)ready_q_ptr->ENABLE_SR, create_parameter))
    {
        /* build stack failed */
        return NULL;
    }


    /* Initialize the task's task descriptor. */
#if MQX_TD_HAS_TASK_TEMPLATE_PTR
    td_ptr->TASK_TEMPLATE_PTR = template_ptr;
#endif
#if MQX_TD_HAS_TEMPLATE_INDEX
    td_ptr->TEMPLATE_INDEX    = template_ptr->TASK_TEMPLATE_INDEX;
#endif

#if MQX_HAS_TIME_SLICE
    /* Use the provided default time slice */
    if (template_ptr->DEFAULT_TIME_SLICE)
    {
#if (MQX_DEFAULT_TIME_SLICE_IN_TICKS == 0)
        uint_32 ticks;

        ticks = ((template_ptr->DEFAULT_TIME_SLICE * 2 *
                        kernel_data->TICKS_PER_SECOND) / 1000) / 2  /* Rounding.. */;
        PSP_ADD_TICKS_TO_TICK_STRUCT(&td_ptr->TIME_SLICE,
            ticks, &td_ptr->TIME_SLICE);
#else
        PSP_ADD_TICKS_TO_TICK_STRUCT(&td_ptr->TIME_SLICE,
            template_ptr->DEFAULT_TIME_SLICE, &td_ptr->TIME_SLICE);
#endif
    }
    else
    {
        td_ptr->TIME_SLICE = kernel_data->SCHED_TIME_SLICE;
    } /* Endif */
    if (kernel_data->SCHED_POLICY == MQX_SCHED_RR)
    {
        td_ptr->FLAGS |= MQX_TIME_SLICE_TASK;
    } /* Endif */
#endif

#if MQX_USE_IO
    td_ptr->STDIN_STREAM  = kernel_data->PROCESSOR_STDIN;
    td_ptr->STDOUT_STREAM = kernel_data->PROCESSOR_STDOUT;
    td_ptr->STDERR_STREAM = kernel_data->PROCESSOR_STDERR;
#endif

#if MQX_TD_HAS_PARENT
    td_ptr->PARENT = creator_task_id;
#endif

#if !MQX_LITE_VERSION_NUMBER
    /*
     * Move ownership of the td from the creating task, to the
     * newly created task.
     */
#if MQX_USE_LWMEM_ALLOCATOR==0
    block_ptr = GET_MEMBLOCK_PTR(td_ptr);
    kernel_data->ACTIVE_PTR->MEMORY_RESOURCE_LIST = block_ptr->NEXTBLOCK;
#endif

    if (input_stack_ptr == 0)
    {
        _mem_transfer_internal(td_ptr, td_ptr);
        _mem_transfer_internal(td_ptr->STACK_ALLOC_BLOCK, td_ptr);
    } /* Endif */
#endif /* MQX_LITE_VERSION_NUMBER */

    return (td_ptr);

} /* Endbody */

#if MQX_MONITOR_STACK
/*!
 * \private
 *
 * \brief Initializes a stack to be filled with the MQX_MONITOR_STACK_VALUE: "stak".
 *
 * \param[in] addr_ptr Pointer to the start of the stack which is to be filled.
 * \param[in] size     The number of bytes to be filled.
 */
void _task_fill_stack_internal
(
    _mqx_uint_ptr addr_ptr,
    _mqx_uint     size
)
{ /* Body */

    size = (size/sizeof(_mqx_uint)) + 1;  /* Calculate # _mqx_uints */
    while (--size)
    {
        *addr_ptr++ = MQX_STACK_MONITOR_VALUE;
    } /* Endwhile */

} /* Endbody */
#endif

/*!
 * \private
 *
 * \brief Adds a task to the ready to run queue.
 *
 * This function MUST BE CALLED DISABLED.
 *
 * \param[in] td_ptr Pointer to the task descriptor to add.
 *
 * \see TD_STRUCT
 */
void _task_ready_internal
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR       kernel_data;
    register READY_Q_STRUCT_PTR  ready_q_ptr;
    register TD_STRUCT_PTR       old_tail_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_task_ready, td_ptr);

    ready_q_ptr   = td_ptr->MY_QUEUE;
    old_tail_ptr  = ready_q_ptr->TAIL_READY_Q;
    /*
     * Ready queues are ALWAYS allocated with higher priority queues in higher
     * memory, thus we need to update the CURRENT_READY_Q field (which
     * always points to the highest priority ready queue with a task
     * on it.
     */
    if ( (uchar_ptr)ready_q_ptr > (uchar_ptr)(kernel_data->CURRENT_READY_Q) )
    {
        kernel_data->CURRENT_READY_Q = ready_q_ptr;
    } /* Endif */

    td_ptr->STATE              = READY;
    td_ptr->TD_PREV            = old_tail_ptr;
    td_ptr->TD_NEXT            = old_tail_ptr->TD_NEXT;
    old_tail_ptr->TD_NEXT      = td_ptr;
    ready_q_ptr->TAIL_READY_Q  = td_ptr;
    /* SPR P171-0020-01 add macro to zero tick structure */
#if MQX_HAS_TIME_SLICE
    MQX_ZERO_TICK_STRUCT(&td_ptr->CURRENT_TIME_SLICE);
#endif
    /* END SPR */
    _KLOGX1(KLOG_task_ready);

} /* Endbody */

#if MQX_EXTRA_TASK_STACK_ENABLE
#if !MQX_LITE_VERSION_NUMBER
/*!
 * \brief Reserves space to add to every stack.
 *
 * This function reserves 'size' bytes that will be added to the size of every
 * task's stack. It can be thought of as a way of allocating "task local storage"
 * or perhaps as a way of dynamically adding to the TD_STRUCT. Both "OS Changer"
 * and the MetaWare C/C++ runtime want additional per-task variables. However,
 * this is a general feature that can be used for many more purposes.
 *
 * \note
 * The space added to each task must be known before ANY tasks are created.
 *
 * \param[in] size Amount of space to reserve.
 *
 * \return Byte offset from reserved area base address.
 *
 * \see _task_get_reserved_base
 * \see _task_alloc_td_internal
 */
_mqx_uint _task_reserve_space
(
    _mqx_uint size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _mqx_uint              off, algn;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    /* Can only be called prior to creating any tasks! */
    if (_QUEUE_GET_SIZE(&kernel_data->TD_LIST) != 0)
    {
        _mqx_fatal_error(MQX_EINVAL);
    } /* Endif */
#endif

    if      (size > 7) algn = 7;
    else if (size > 3) algn = 3;
    else if (size > 1) algn = 1;
    else               algn = 0;

    if (algn > kernel_data->TOS_RESERVED_ALIGN_MASK)
    {
        kernel_data->TOS_RESERVED_ALIGN_MASK = algn;
    } /* Endif */

    off = (kernel_data->TOS_RESERVED_SIZE + algn) & ~algn;
    kernel_data->TOS_RESERVED_SIZE = off + size;

    return off;

} /* Endbody */
#endif /* MQX_LITE_VERSION_NUMBER */
#endif /* MQX_EXTRA_TASK_STACK_ENABLE */

/*!
 * \private
 *
 * \brief Changes task's priority level.
 *
 * This function is called when a quiescent task's priority level has changed, and
 * the hardware disable/enable bits may need to be patched to the correct level.
 *
 * \param[in] td_ptr Task descriptor whose stack is to be modified.
 *
 * \see TD_STRUCT
 */
void _task_sync_priority_internal
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    pointer stack_ptr = td_ptr->STACK_PTR;

    _PSP_SET_SR_OF_BLOCKED_TASK(stack_ptr, td_ptr->MY_QUEUE->ENABLE_SR);

} /* Endbody */

/* Start SPR P171-0016-01 */

/*!
 * \private
 *
 * \brief Gets a pointer to an initialized task descriptor.
 *
 * Note that the task descriptor is initialized to zero.
 *
 * \param[in]  stack_size      Size of the stack for the newly created task.
 * \param[out] overhead        Size of TD_STRUCT and other overhead.
 * \param[in]  input_stack_ptr If not NULL, the location of the stack is provided.
 * \param[in]  user            User mode functionality.
 *
 * \return Pointer to an initialized task descriptor.
 * \return NULL
 * \return 0 (Not enough resources to create a task.)
 *
 * \see TD_STRUCT
 */

TD_STRUCT_PTR _task_alloc_td_internal
(
    _mem_size     stack_size,
    _mem_size_ptr overhead,
    pointer    input_stack_ptr,
    _mqx_uint user
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    TD_STRUCT_PTR               insert_td_ptr;
    TD_STRUCT_PTR               next_td_ptr;
    TD_STRUCT_PTR               new_td_ptr;
    TD_STRUCT_PTR               prev_td_ptr;
    QUEUE_ELEMENT_STRUCT_PTR    q_ptr;
    _mem_size                   extra = sizeof(TD_STRUCT);
#if MQX_EXTRA_TASK_STACK_ENABLE
    _mem_size                tos, tos_offset = 0;
#endif
    uint_16                  task_num;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (_QUEUE_GET_SIZE(&kernel_data->TD_LIST) >= (MAX_UINT_16-1))
    {
        /* We already have maximum number of tasks in system */
        return NULL;
    } /* Endif */
#endif

#if MQX_EXTRA_TASK_STACK_ENABLE
    tos = kernel_data->TOS_RESERVED_SIZE;
    if (tos != 0)
    {
        uint_32 align_mask = kernel_data->TOS_RESERVED_ALIGN_MASK;
        extra = (extra + align_mask) & ~align_mask;
        tos_offset = extra;
        /* must be word multiple */
        extra = (extra + tos + 3) & ~3;
    } /* Endif */
#endif
    *overhead = extra;


    if (input_stack_ptr)
    {
        new_td_ptr = (TD_STRUCT_PTR)input_stack_ptr;
        _mem_zero((pointer)new_td_ptr, extra);
    }
#if !MQX_LITE_VERSION_NUMBER
    else
    {
        new_td_ptr = (TD_STRUCT_PTR)_mem_alloc_zero(extra);

        if (new_td_ptr)
        {
#if MQX_ENABLE_USER_MODE
            if (user)
            {
                new_td_ptr->STACK_ALLOC_BLOCK = (TD_STRUCT_PTR)_mem_alloc_from(kernel_data->KD_USER_POOL, stack_size);
            }
            else
            {
                new_td_ptr->STACK_ALLOC_BLOCK = (TD_STRUCT_PTR)_mem_alloc(stack_size);
            }
#else
            new_td_ptr->STACK_ALLOC_BLOCK = (TD_STRUCT_PTR)_mem_alloc(stack_size);
#endif /* MQX_ENABLE_USER_MODE */
        }

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if ( new_td_ptr == NULL )
        {
            return ((TD_STRUCT_PTR)new_td_ptr);
        } /* Endif */

        if (new_td_ptr->STACK_ALLOC_BLOCK == NULL )
        {
            /* dealloc TD, because we do not have enough resources to create task */
            _mem_free(new_td_ptr);
            return 0;
        }

        _mem_set_type(new_td_ptr, MEM_TYPE_TD);
        _mem_set_type(new_td_ptr->STACK_ALLOC_BLOCK, MEM_TYPE_TASK_STACK);
#endif
    } /* Endif */
#endif /* MQX_LITE_VERSION_NUMBER */


#if MQX_EXTRA_TASK_STACK_ENABLE
    if (tos != 0)
    {
        new_td_ptr->TOS_RESERVED = ((uchar_ptr) new_td_ptr) + tos_offset;
    } /* Endif */
#endif

    /* Add new task to list */
    task_num = kernel_data->TASK_NUMBER;
    insert_td_ptr = kernel_data->INSERT_TD_PTR;
    new_td_ptr->TASK_ID = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, task_num);

    if (!insert_td_ptr)
    {
        q_ptr = (QUEUE_ELEMENT_STRUCT_PTR)((pointer)&kernel_data->TD_LIST);
    }
    else
    {
        q_ptr = &insert_td_ptr->TD_LIST_INFO;
    } /* Endif */

    _QUEUE_INSERT(&kernel_data->TD_LIST, q_ptr, &new_td_ptr->TD_LIST_INFO);

    /*
     * New task may not have been inserted at the end of the list. Search for a
     * spot to insert the next td and also calculate its task number
     */
    prev_td_ptr = new_td_ptr;
    do
    {
        insert_td_ptr = prev_td_ptr;
        next_td_ptr   = (TD_STRUCT_PTR)((uchar_ptr)insert_td_ptr->TD_LIST_INFO.NEXT - FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
        prev_td_ptr   = next_td_ptr;
        INC_TASK_NUMBER(task_num);
    } while( ((pointer)next_td_ptr != ((pointer)&kernel_data->TD_LIST)) &&
                    (TASK_NUMBER_FROM_TASKID(next_td_ptr->TASK_ID) == task_num) );

    kernel_data->INSERT_TD_PTR = insert_td_ptr;
    kernel_data->TASK_NUMBER   = task_num;

    return ((TD_STRUCT_PTR)new_td_ptr);

} /* Endbody */

/* End SPR P171-0016-01 */

/*!
 * \private
 *
 * \brief Sets the task error code of the specified task.
 *
 * If the task error code does not equal MQX_OK, then the value is not changed.
 * However, the value of the task error code may be reset to MQX_OK by setting the
 * task error code to MQX_OK.  The old value of the task error code is returned.
 *
 * \param[in] td_ptr         Pointer to the task descriptor whose error is to be set.
 * \param[in] new_error_code The new error code.
 *
 * \return MQX_OK
 * \return Old task error code.
 *
 * \see TD_STRUCT
 */
_mqx_uint _task_set_error_td_internal
(
    TD_STRUCT_PTR td_ptr,
    _mqx_uint new_error_code
)
{ /* Body */
#if MQX_TD_HAS_ERROR_CODE
    _KLOGM(KERNEL_DATA_STRUCT_PTR  kernel_data;)
    _mqx_uint                      old_error_code;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE2(KLOG_task_set_error, new_error_code);

    /* get the old value */
    old_error_code = td_ptr->TASK_ERROR_CODE;
    if (( new_error_code == MQX_OK ) || ( old_error_code == MQX_OK ))
    {
        td_ptr->TASK_ERROR_CODE = new_error_code;
    } /* Endif */

    _KLOGX2(KLOG_task_set_error, old_error_code);
    return( old_error_code );
#else
    return MQX_OK;
#endif

} /* Endbody */
