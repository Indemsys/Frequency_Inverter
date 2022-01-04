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
 * $FileName: sem.c$
 * $Version : 3.8.2.0$
 * $Date    : Jun-25-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Semaphore component.
 *
 *END************************************************************************/

#include <string.h>
#include "mqx_inc.h"
#if MQX_USE_SEMAPHORES
#include "name.h"
#include "name_prv.h"
#include "sem.h"
#include "sem_prv.h"
#include "mqx_str.h"

/*!
 * \brief Closes the connection to the semaphore.
 * 
 * \param[in] users_sem_ptr Semaphore handle from _sem_open() or _sem_open_fast().
 * 
 * \return MQX_OK 
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return SEM_INVALID_SEMAPHORE_HANDLE (Semaphore is no longer valid or sem_handle 
 * is not a valid semaphore connection)
 * \return Error from _mem_free() (Task is not the one that opened the connection.)
 * 
 * \warning If the semaphore is strict, posts the appropriate number of times to 
 * the semaphore for this connection.
 * \warning Might dispatch tasks that are waiting for the semaphore.
 * \warning Cannot be called from an ISR.
 * 
 * \see _sem_destroy
 * \see _sem_destroy_fast
 * \see _sem_open
 * \see _sem_open_fast 
 */ 
_mqx_uint _sem_close
(
    pointer users_sem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_sem_close, users_sem_ptr);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_sem_close, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)users_sem_ptr;
#if MQX_CHECK_ERRORS
    if (sem_connection_ptr->TD_PTR != kernel_data->ACTIVE_PTR)
    {
        _KLOGX2(KLOG_sem_close, SEM_INVALID_SEMAPHORE_HANDLE);
        return(SEM_INVALID_SEMAPHORE_HANDLE);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (sem_connection_ptr->VALID != SEM_VALID)
    {
        _KLOGX2(KLOG_sem_close, SEM_INVALID_SEMAPHORE_HANDLE);
        return(SEM_INVALID_SEMAPHORE_HANDLE);
    } /* Endif */
#endif

    if (sem_connection_ptr->SEM_PTR->VALID)
    {
        /* Release all held semaphores */
        while (sem_connection_ptr->POST_STATE > 0)
        {
            _sem_post(sem_connection_ptr);
        } /* Endwhile */
    } /* Endif */

    sem_connection_ptr->VALID = 0;

    if (_mem_free(sem_connection_ptr) != MQX_OK)
    {
        _KLOGX2(KLOG_sem_close, kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);
        return(kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);
    }
    else
    {
        _KLOGX2(KLOG_sem_close, MQX_OK);
        return(MQX_OK);
    } /* Endif */

} /* Endbody */

/*!
 * \brief Creates the semaphore component.
 * 
 * If an application previously called the function and maximum_number is greater 
 * that what was specified, MQX changes the maximum number of semaphores to 
 * maximum_number.
 * \n If an application does not explicitly create the semaphore component, MQX 
 * does so with the following default values the first time that a task calls 
 * _sem_create() or _sem_create_fast().
 * \n Parameter default values:
 * \li Initial_number = 8(default)
 * \li Grow_number = 8(default)
 * \li Maximum_number = 0(unlimited)  
 * 
 * \param[in] initial_number Initial number of semaphores that can be created.
 * \param[in] grow_number    Number of semaphores to be added when the initial 
 * number have been created.
 * \param[in] maximum_number If grow_number is not 0, one of:
 * \li Maximum number of semaphores that can be created.
 * \li 0 (Unlimited number.)
 * 
 * \return MQX_OK (success)
 * \return MQX_OUT_OF_MEMORY (Failure: MQX cannot allocate memory for semaphore 
 * component data.) 
 * 
 * \warning On failure, the task error code might be set.
 * 
 * \see _sem_create
 * \see _sem_create_fast
 * \see _sem_open
 * \see _sem_open_fast
 * \see _task_set_error
 */ 
_mqx_uint _sem_create_component
(
    _mqx_uint initial_number,
    _mqx_uint grow_number,
    _mqx_uint maximum_number
)
{ /* Body */
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    _mqx_uint                result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_sem_create_component, initial_number, grow_number, maximum_number);

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    if (sem_component_ptr != NULL)
    {
        if (maximum_number > sem_component_ptr->MAXIMUM_NUMBER)
        {
            sem_component_ptr->MAXIMUM_NUMBER = maximum_number;
        } /* Endif */
        _KLOGX2(KLOG_sem_create_component, MQX_OK);
        return(MQX_OK);
    } /* Endif */

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    _mem_alloc_system_zero((_mem_size)sizeof(SEM_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (sem_component_ptr == NULL)
    {
        _KLOGX2(KLOG_sem_create_component, MQX_OUT_OF_MEMORY);
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(sem_component_ptr, MEM_TYPE_SEMAPHORE_COMPONENT);

    result = _name_create_handle_internal(&sem_component_ptr->NAME_TABLE_HANDLE,
                    initial_number, grow_number, maximum_number, initial_number);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (result != MQX_OK)
    {
        _mem_free(sem_component_ptr);
        _KLOGX2(KLOG_sem_create_component, result);
        return(result);
    } /* Endif */
#endif

    sem_component_ptr->GROW_NUMBER = grow_number;
    if (maximum_number == 0)
    {
        sem_component_ptr->MAXIMUM_NUMBER = MAX_MQX_UINT;
    }
    else if (maximum_number < initial_number)
    {
        sem_component_ptr->MAXIMUM_NUMBER = initial_number;
    }
    else
    {
        sem_component_ptr->MAXIMUM_NUMBER = maximum_number;
    } /* Endif */
    sem_component_ptr->VALID = SEM_VALID;

    _int_disable();
#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES] != NULL)
    {
        SEM_COMPONENT_STRUCT_PTR new_sem_component_ptr;

        new_sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
        if (maximum_number > new_sem_component_ptr->MAXIMUM_NUMBER)
        {
            new_sem_component_ptr->MAXIMUM_NUMBER = maximum_number;
        } /* Endif */
        _int_enable();
        _name_destroy_handle_internal(sem_component_ptr->NAME_TABLE_HANDLE);
        _mem_free(sem_component_ptr);
        _KLOGX2(KLOG_sem_create_component, MQX_OK);
        return(MQX_OK);
    } /* Endif */
#endif
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES] = sem_component_ptr;

#if MQX_COMPONENT_DESTRUCTION
    kernel_data->COMPONENT_CLEANUP[KERNEL_SEMAPHORES] = _sem_cleanup;
#endif

    _int_enable();

    _KLOGX2(KLOG_sem_create_component, MQX_OK);
    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Creates a named semaphore.
 * 
 * After the semaphore is created, tasks open a connection to it with _sem_open() 
 * and close the connection with _sem_close(). A named semaphore is destroyed 
 * with _sem_destroy().
 * <table>
 *   <tr>
 *     <td><b>Bit flag</b></td>
 *     <td><b>Set</b></td>
 *     <td><b>Not set</b></td>  
 *   </tr>
 *   <tr>
 *     <td><b>SEM_PRIORITY_INHERITANCE</b>(<b>SEM_STRICT</b> must also be set)</td>
 *     <td>If a task that waits for the semaphore has a higher priority than a 
 *     task that owns the semaphore, MQX boosts the priority of one of the owning 
 *     tasks to the priority of the waiting task. When the boosted task posts its 
 *     semaphore, MQX returns its priorities to its original values.</td>
 *     <td>MQX does not boost priorities</td>  
 *   </tr>
 *   <tr>
 *     <td><b>SEM_PRIORITY_QUEUEING</b></td>
 *     <td>Task that waits for the semaphore is queued according to the task's 
 *     priority. Within a priority, tasks are in FIFO order.</td>
 *     <td>Task that waits for the semaphore is queued in FIFO order.</td>  
 *   </tr>
 *   <tr>
 *     <td><b>SEM_STRICT</b></td>
 *     <td>Task must wait for the semaphore before it can post the semaphore.</td>
 *     <td>Task need not wait before posting.</td>  
 *   </tr>
 *   <tr>
 *     <td><b>SEM_STRICT</b></td>
 *     <td>Sem_count must be greater than or equal to 1.</td>
 *     <td>Sem_count must be greater than or equal to 0.</td>  
 *   </tr>     
 * </table> 
 * 
 * \param[in] name_ptr      Name by which to identify the semaphore.
 * \param[in] initial_count Number of requests that can concurrently have the 
 * semaphore.
 * \param[in] flags         Bit flags: 0 or as in Description.
 * 
 * \return MQX_OK (success)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Semaphore component was not created and 
 * cannot be created.)
 * \return MQX_INVALID_COMPONENT_BASE (Semaphore component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the semaphore.)  
 * \return SEM_INCORRECT_INITIAL_COUNT (sem_count cannot be 0 if SEM_STRICT is set.)
 * \return SEM_INVALID_POLICY (SEM_STRICT must be set if SEM_PRIORITY_INHERITANCE 
 * is set.)
 * \return SEM_SEMAPHORE_EXISTS (Semaphore with the name exists.)
 * \return SEM_SEMAPHORE_TABLE_FULL (Semaphore names database is full and cannot 
 * be expanded.)
 * 
 * \warning Creates the semaphore component with default values if it was not 
 * previously created.
 * \warning Cannot be called from an ISR.
 * \warning On failure, the task error code might be set.
 * 
 * \see _sem_create_component
 * \see _sem_destroy
 * \see _sem_destroy_fast
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_close
 * \see _task_set_error          
 */ 
_mqx_uint _sem_create
(
    char_ptr  name_ptr,
    _mqx_uint initial_count,
    _mqx_uint flags
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR            kernel_data;
    register SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    register SEM_STRUCT_PTR           sem_ptr;
    _mqx_uint                         result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_sem_create, name_ptr, initial_count, flags);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_sem_create, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (flags & SEM_STRICT)
    {
        if (initial_count == 0)
        {
            _KLOGX2(KLOG_sem_create, SEM_INCORRECT_INITIAL_COUNT);
            return(SEM_INCORRECT_INITIAL_COUNT);
        }/* Endif */
    }
    else
    {
        if (flags & SEM_PRIORITY_INHERITANCE)
        {
            _KLOGX2(KLOG_sem_create, SEM_INVALID_POLICY);
            return(SEM_INVALID_POLICY);
        } /* Endif */
    }/* Endif */
#endif

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    if (sem_component_ptr == NULL)
    {
        result = _sem_create_component(SEM_DEFAULT_INITIAL_NUMBER,
                        SEM_DEFAULT_GROW_NUMBER, SEM_DEFAULT_MAXIMUM_NUMBER);
        sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (sem_component_ptr == NULL)
        {
            _KLOGX2(KLOG_sem_create, result);
            return(result);
        } /* Endif */
#endif
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (sem_component_ptr->VALID != SEM_VALID)
    {
        _KLOGX2(KLOG_sem_create, MQX_INVALID_COMPONENT_BASE);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    sem_ptr = (SEM_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(SEM_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (sem_ptr == NULL)
    {
        _KLOGX2(KLOG_sem_create, MQX_OUT_OF_MEMORY);
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(sem_ptr, MEM_TYPE_SEMAPHORE);

    sem_ptr->POLICY = flags;
    sem_ptr->COUNT = initial_count;
    if (flags & SEM_STRICT)
    {
        sem_ptr->MAX_COUNT = initial_count;
    }
    else
    {
        sem_ptr->MAX_COUNT = MAX_MQX_UINT;
    }/* Endif */
    _QUEUE_INIT(&sem_ptr->WAITING_TASKS, SEM_MAX_WAITING_TASKS);
    _QUEUE_INIT(&sem_ptr->OWNING_TASKS, SEM_MAX_WAITING_TASKS);

    result = _name_add_internal(sem_component_ptr->NAME_TABLE_HANDLE, name_ptr,
                    (_mqx_uint)sem_ptr);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        _mem_free(sem_ptr);
        if (result == NAME_EXISTS)
        {
            _KLOGX2(KLOG_sem_create, SEM_SEMAPHORE_EXISTS);
            return(SEM_SEMAPHORE_EXISTS);
        }
        else if (result == NAME_TABLE_FULL)
        {
            _KLOGX2(KLOG_sem_create, SEM_SEMAPHORE_TABLE_FULL);
            return(SEM_SEMAPHORE_TABLE_FULL);
        } /* Endif */
        _KLOGX2(KLOG_sem_create, result);
        return(result);
    } /* Endif */
#endif

    strncpy(sem_ptr->NAME, name_ptr, (_mqx_uint)NAME_MAX_NAME_SIZE-1);
    sem_ptr->NAME[NAME_MAX_NAME_SIZE-1] = '\0';

    sem_ptr->VALID = SEM_VALID;

    _KLOGX2(KLOG_sem_create, result);
    return(result);

} /* Endbody */

/*!
 * \brief Destroys the named semaphore.
 * 
 * <table>
 *   <tr>
 *     <td><b>force_destroy is TRUE</b></td>
 *     <td><b>force_destroy is FALSE</b></td> 
 *   </tr>
 *   <tr>
 *     <td> \li Tasks that are waiting for the semaphore are made ready.
 *     \li Semaphore is destroyed after all the owners post the semaphore.</td>
 *     <td> \li Semaphore is destroyed after the last waiting task gets and posts 
 *     the semaphore.
 *     \li This is the action if the semaphore is strict.</td> 
 *   </tr>  
 * </table>  
 * 
 * \param[in] name_ptr      Name of the semaphore to destroy, created using 
 * _sem_create().
 * \param[in] force_destroy A boolean flag, see Description.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Semaphore component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Semaphore component data is not valid.)
 * \return SEM_SEMAPHORE_NOT_FOUND (Name_ptr is not in the semaphore names database.)
 * \return SEM_INVALID_SEMAPHORE (Semaphore data that is associated with name or index is not valid.)
 * _name_find_internal
 * \return NAME_TOO_SHORT (Name is 0 lenght string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.) 
 * 
 * \warning Cannot be called from an ISR.
 *    
 * \see _sem_destroy_fast
 * \see _sem_close
 * \see _sem_create
 * \see _sem_create_fast 
 */ 
_mqx_uint _sem_destroy
(
    char_ptr name_ptr,
    boolean  force_destroy
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR            kernel_data;
    register SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR                    sem_ptr;
    SEM_CONNECTION_STRUCT_PTR         sem_connection_ptr;
    QUEUE_STRUCT_PTR                  queue_ptr;
    TD_STRUCT_PTR                     td_ptr;
    _mqx_max_type                     tmp;
    _mqx_uint                         result;
    _mqx_uint                         i;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_sem_destroy, name_ptr, force_destroy);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_sem_destroy, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES] == NULL)
    {
        _KLOGX2(KLOG_sem_destroy, MQX_COMPONENT_DOES_NOT_EXIST);
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
#if MQX_CHECK_VALIDITY
    if (sem_component_ptr->VALID != SEM_VALID)
    {
        _KLOGX2(KLOG_sem_destroy, MQX_INVALID_COMPONENT_BASE);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE,
                    name_ptr, &tmp);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (result == NAME_NOT_FOUND)
        {
            _KLOGX2(KLOG_sem_destroy, SEM_SEMAPHORE_NOT_FOUND);
            return(SEM_SEMAPHORE_NOT_FOUND);
        } /* Endif */
        _KLOGX2(KLOG_sem_destroy, result);
        return(result);
    } /* Endif */
#endif

    sem_ptr = (SEM_STRUCT_PTR)tmp;

    _int_disable();

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != SEM_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_sem_destroy, SEM_INVALID_SEMAPHORE);
        return(SEM_INVALID_SEMAPHORE);
    } /* Endif */
#endif

    queue_ptr = &sem_ptr->WAITING_TASKS;
    if (force_destroy)
    {
        /* Get rid of all waiting tasks */
        i = _QUEUE_GET_SIZE(queue_ptr) + 1;
        while (--i)
        {
            sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
            ((pointer)queue_ptr->NEXT);
            td_ptr = sem_connection_ptr->TD_PTR;
            if ((td_ptr->STATE & STATE_MASK)== SEM_BLOCKED)
            {
                _TIME_DEQUEUE(td_ptr,kernel_data);
                _task_set_error_td_internal(td_ptr,SEM_SEMAPHORE_DELETED);
                _TASK_READY(td_ptr,kernel_data);
            } /* Endif */
        } /* Endwhile */
    } /* Endif */

    if (
        ( (sem_ptr->POLICY & SEM_STRICT) &&
          (sem_ptr->COUNT == sem_ptr->MAX_COUNT) &&
          (_QUEUE_GET_SIZE(queue_ptr) == 0))
        ||
        ( (!(sem_ptr->POLICY & SEM_STRICT)) &&
          (_QUEUE_GET_SIZE(queue_ptr) == 0)))
    {
        /* Destroy the semaphore */
        sem_ptr->VALID = 0;

        _int_enable();

        /* May need to let higher priority task run */
        _CHECK_RUN_SCHEDULER();

        result = _name_delete_internal(sem_component_ptr->NAME_TABLE_HANDLE,
                        name_ptr);
        if (result == NAME_NOT_FOUND)
        {
            result = SEM_SEMAPHORE_NOT_FOUND;
        } /* Endif */
        _mem_free(sem_ptr);
    }
    else
    {
        sem_ptr->DELAYED_DESTROY = TRUE;

        _int_enable();

        /* May need to let higher priority task run */
        _CHECK_RUN_SCHEDULER();
    } /* Endif */

    _KLOGX2(KLOG_sem_destroy, result);
    return(result);

} /* Endbody */

/*!
 * \brief Creates the fast semaphore.
 * 
 * After the semaphore is created, tasks open a connection to it with 
 * _sem_open_fast() and close the connection with _sem_close(). A fast semaphore 
 * is destroyed with _sem_destroy_fast().
 * \n _sem_create_fast() access the semaphore via an index number rather than a 
 * name.
 * 
 * \param[in] sem_index     Number by which to identify the semaphore.
 * \param[in] initial_count Number of tasks that can concurrently have the semaphore.
 * \param[in] flags         Bit flags, as described for _sem_create().
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Semaphore component was not created and 
 * cannot be created.)
 * \return MQX_INVALID_COMPONENT_BASE (Semaphore component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the semaphore.)
 * \return SEM_SEMAPHORE_TABLE_FULL (Semaphore names database is full and cannot 
 * be expanded.)
 * \return SEM_SEMAPHORE_EXISTS (Semaphore with the name exists.)
 * \return SEM_INCORRECT_INITIAL_COUNT (Sem_count cannot be 0 if SEM_STRICT is set.)
 * \return SEM_INVALID_POLICY (SEM_STRICT must be set if SEM_PRIORITY_INHERITANCE 
 * is set.)  
 * 
 * \warning Creates the semaphore component with default values if it was not 
 * previously created.
 * \warning Cannot be called from an ISR.
 * \warning On failure, the task error code might be set.
 * 
 * \see _sem_create_component
 * \see _sem_destroy
 * \see _sem_destroy_fast
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_close
 * \see _sem_create 
 *         
 */ 
_mqx_uint _sem_create_fast
(
    _mqx_uint sem_index,
    _mqx_uint initial_count,
    _mqx_uint flags
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR            kernel_data;
    register SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    register SEM_STRUCT_PTR           sem_ptr;
    _mqx_uint                         result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_sem_create_fast, sem_index, initial_count, flags);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_sem_create_fast, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (flags & SEM_STRICT)
    {
        if (initial_count == 0)
        {
            _KLOGX2(KLOG_sem_create_fast, SEM_INCORRECT_INITIAL_COUNT);
            return(SEM_INCORRECT_INITIAL_COUNT);
        }/* Endif */
    }
    else
    {
        if (flags & SEM_PRIORITY_INHERITANCE)
        {
            _KLOGX2(KLOG_sem_create_fast, SEM_INVALID_POLICY);
            return(SEM_INVALID_POLICY);
        } /* Endif */
    }/* Endif */
#endif

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    if (sem_component_ptr == NULL)
    {
        result = _sem_create_component(SEM_DEFAULT_INITIAL_NUMBER,
                        SEM_DEFAULT_GROW_NUMBER, SEM_DEFAULT_MAXIMUM_NUMBER);
        sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (sem_component_ptr == NULL)
        {
            _KLOGX2(KLOG_sem_create_fast, result);
            return(result);
        } /* Endif */
#endif
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (sem_component_ptr->VALID != SEM_VALID)
    {
        _KLOGX2(KLOG_sem_create_fast, MQX_INVALID_COMPONENT_BASE);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    sem_ptr = (SEM_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(SEM_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (sem_ptr == NULL)
    {
        _KLOGX2(KLOG_sem_create_fast, MQX_OUT_OF_MEMORY);
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(sem_ptr, MEM_TYPE_SEMAPHORE);

    sem_ptr->POLICY = flags;
    sem_ptr->COUNT = initial_count;
    if (flags & SEM_STRICT)
    {
        sem_ptr->MAX_COUNT = initial_count;
    }
    else
    {
        sem_ptr->MAX_COUNT = MAX_MQX_UINT;
    }/* Endif */
    _QUEUE_INIT(&sem_ptr->WAITING_TASKS, SEM_MAX_WAITING_TASKS);
    _QUEUE_INIT(&sem_ptr->OWNING_TASKS, SEM_MAX_WAITING_TASKS);

    _str_mqx_uint_to_hex_string(sem_index, sem_ptr->NAME);

    result = _name_add_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE,
                    sem_ptr->NAME, (_mqx_uint)sem_ptr, sem_index);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        _mem_free(sem_ptr);
        if (result == NAME_EXISTS)
        {
            _KLOGX2(KLOG_sem_create_fast, SEM_SEMAPHORE_EXISTS);
            return(SEM_SEMAPHORE_EXISTS);
        }
        else if (result == NAME_TABLE_FULL)
        {
            _KLOGX2(KLOG_sem_create_fast, SEM_SEMAPHORE_TABLE_FULL);
            return(SEM_SEMAPHORE_TABLE_FULL);
        } /* Endif */
        _KLOGX2(KLOG_sem_create_fast, result);
        return(result);
    } /* Endif */
#endif

    sem_ptr->VALID = SEM_VALID;

    _KLOGX2(KLOG_sem_create_fast, result);
    return(result);

} /* Endbody */

/*!
 * \brief Destroys the fast semaphore.
 * 
 * <table>
 *   <tr>
 *     <td><b>force_destroy is TRUE</b></td>
 *     <td><b>force_destroy is FALSE</b></td> 
 *   </tr>
 *   <tr>
 *     <td> \li Tasks that are waiting for the semaphore are made ready.
 *     \li Semaphore is destroyed after all the owners post the semaphore.</td>
 *     <td> \li Semaphore is destroyed after the last waiting task gets and posts 
 *     the semaphore.
 *     \li This is the action if the semaphore is strict.</td> 
 *   </tr>  
 * </table>  
 * 
 * \param[in] sem_index     Number that identifies the semaphore to destroy, 
 * created using _sem_create_fast().
 * \param[in] force_destroy A boolean flag, see Description.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Semaphore component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Semaphore component data is not valid.)
 * \return SEM_SEMAPHORE_NOT_FOUND (Name_ptr is not in the semaphore names database.)
 * \return SEM_INVALID_SEMAPHORE (Semaphore data that is associated with name or index is not valid.)
 * _name_find_internal_by_index
 * \return NAME_TOO_SHORT (Name is 0 lenght string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.) 
 * 
 * \warning Cannot be called from an ISR. 
 * 
 * \see _sem_destroy
 * \see _sem_close
 * \see _sem_create
 * \see _sem_create_fast
 */ 
_mqx_uint _sem_destroy_fast
(
    _mqx_uint sem_index,
    boolean   force_destroy
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR            kernel_data;
    register SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR                    sem_ptr;
    SEM_CONNECTION_STRUCT_PTR         sem_connection_ptr;
    QUEUE_STRUCT_PTR                  queue_ptr;
    TD_STRUCT_PTR                     td_ptr;
    _mqx_max_type                     tmp;
    _mqx_uint                         result;
    _mqx_uint                         i;
#if MQX_CHECK_ERRORS
    char                              name[9];
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_sem_destroy_fast, sem_index, force_destroy);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_sem_destroy_fast, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES] == NULL)
    {
        _KLOGX2(KLOG_sem_destroy_fast, MQX_COMPONENT_DOES_NOT_EXIST);
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
#if MQX_CHECK_VALIDITY
    if (sem_component_ptr->VALID != SEM_VALID)
    {
        _KLOGX2(KLOG_sem_destroy_fast, MQX_INVALID_COMPONENT_BASE);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE,
                    sem_index, &tmp);
    sem_ptr = (SEM_STRUCT_PTR)tmp;

#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (result == NAME_NOT_FOUND)
        {
            _KLOGX2(KLOG_sem_destroy_fast, SEM_SEMAPHORE_NOT_FOUND);
            return(SEM_SEMAPHORE_NOT_FOUND);
        } /* Endif */
        _KLOGX2(KLOG_sem_destroy_fast, result);
        return(result);
    } /* Endif */

    _str_mqx_uint_to_hex_string(sem_index, name);
    if (strncmp(name, sem_ptr->NAME, 8))
    {
        /* Name does not match */
        _KLOGX2(KLOG_sem_destroy_fast, SEM_SEMAPHORE_NOT_FOUND);
        return(SEM_SEMAPHORE_NOT_FOUND);
    } /* Endif */
#endif

    _int_disable();

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != SEM_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_sem_destroy_fast, SEM_INVALID_SEMAPHORE);
        return(SEM_INVALID_SEMAPHORE);
    } /* Endif */
#endif

    queue_ptr = &sem_ptr->WAITING_TASKS;

    if (force_destroy)
    {
        /* Get rid of all waiting tasks */
        i = _QUEUE_GET_SIZE(queue_ptr) + 1;
        while (--i)
        {
            sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
            ((pointer)queue_ptr->NEXT);
            td_ptr = sem_connection_ptr->TD_PTR;
            if ((td_ptr->STATE & STATE_MASK)== SEM_BLOCKED)
            {
                _TIME_DEQUEUE(td_ptr,kernel_data);
                _task_set_error_td_internal(td_ptr,SEM_SEMAPHORE_DELETED);
                _TASK_READY(td_ptr,kernel_data);
            } /* Endif */
        } /* Endwhile */
    } /* Endif */

    if (
        ( (sem_ptr->POLICY & SEM_STRICT) &&
          (sem_ptr->COUNT == sem_ptr->MAX_COUNT) &&
          (_QUEUE_GET_SIZE(queue_ptr) == 0))
        ||
        ( (!(sem_ptr->POLICY & SEM_STRICT)) &&
          (_QUEUE_GET_SIZE(queue_ptr) == 0)))
    {
        /* Destroy the semaphore */
        sem_ptr->VALID = 0;

        _int_enable();

        /* May need to let higher priority task run */
        _CHECK_RUN_SCHEDULER();

        result = _name_delete_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE,
                        sem_index);
        if (result == NAME_NOT_FOUND)
        {
            result = SEM_SEMAPHORE_NOT_FOUND;
        } /* Endif */
        _mem_free(sem_ptr);
    }
    else
    {
        sem_ptr->DELAYED_DESTROY = TRUE;

        _int_enable();

        /* May need to let higher priority task run */
        _CHECK_RUN_SCHEDULER();

    } /* Endif */

    _KLOGX2(KLOG_sem_destroy_fast, result);
    return(result);

} /* Endbody */

/*!
 * \brief Opens a connection to the fast semaphore.
 * 
 * \param[in]  sem_index        Number that identifies the semaphore that was 
 * created using _sem_create_fast().
 * \param[out] returned_sem_ptr Pointer to the semaphore handle, which is a 
 * connection to the semaphore.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Semaphore component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Semaphore component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the connection.)
 * \return SEM_SEMAPHORE_NOT_FOUND (Name is not in the semaphore names database.)
 * \return SEM_INVALID_SEMAPHORE (Data that is associated with the semaphore is 
 * not valid.)
 * \return SEM_SEMAPHORE_DELETED (Semaphore is in the process of being destroyed.)
 *   
 * \see _sem_open
 * \see _sem_close
 * \see _sem_create
 * \see _sem_post
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until
 */ 
_mqx_uint _sem_open_fast
(
    _mqx_uint     sem_index,
    pointer _PTR_ returned_sem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR             kernel_data;
    register SEM_COMPONENT_STRUCT_PTR  sem_component_ptr;
    SEM_STRUCT_PTR                     sem_ptr;
    register SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    _mqx_uint                          result;
    _mqx_max_type                      tmp;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_sem_open_fast, sem_index, returned_sem_ptr);

    *returned_sem_ptr = NULL;

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
#if MQX_CHECK_ERRORS
    if (sem_component_ptr == NULL)
    {
        _KLOGX3(KLOG_sem_open_fast, MQX_COMPONENT_DOES_NOT_EXIST, 0);
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (sem_component_ptr->VALID != SEM_VALID)
    {
        _KLOGX3(KLOG_sem_open_fast, MQX_INVALID_COMPONENT_BASE, 0);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE,
                    sem_index, &tmp);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (result == NAME_NOT_FOUND)
        {
            _KLOGX3(KLOG_sem_open_fast, SEM_SEMAPHORE_NOT_FOUND, 0);
            return(SEM_SEMAPHORE_NOT_FOUND);
        } /* Endif */
        _KLOGX3(KLOG_sem_open_fast, result, 0);
        return(result);
    } /* Endif */
#endif

    sem_ptr = (SEM_STRUCT_PTR)tmp;

#if MQX_CHECK_ERRORS
    if (sem_ptr->VALID != SEM_VALID)
    {
        /* Semaphore not valid */
        _KLOGX3(KLOG_sem_open_fast, SEM_INVALID_SEMAPHORE, 0);
        return(SEM_INVALID_SEMAPHORE);
    } /* Endif */
#endif

#if MQX_COMPONENT_DESTRUCTION
    if (sem_ptr->DELAYED_DESTROY)
    {
        /* Semaphore in delayed destroy state */
        _KLOGX3(KLOG_sem_open_fast, SEM_SEMAPHORE_DELETED, 0);
        return(SEM_SEMAPHORE_DELETED);
    } /* Endif */
#endif

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)_mem_alloc_zero(
                    (_mem_size)sizeof(SEM_CONNECTION_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (sem_connection_ptr == NULL)
    {
        _KLOGX3(KLOG_sem_open_fast, MQX_OUT_OF_MEMORY, 0);
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(sem_connection_ptr, MEM_TYPE_SEMAPHORE_CONNECTION);

    sem_connection_ptr->SEM_PTR = sem_ptr;
    sem_connection_ptr->VALID = SEM_VALID;
    sem_connection_ptr->TD_PTR = kernel_data->ACTIVE_PTR;

    *returned_sem_ptr = (pointer)sem_connection_ptr;

    _KLOGX3(KLOG_sem_open_fast, MQX_OK, sem_connection_ptr);
    return(MQX_OK);

} /* Endbody */

/*!
 * \private
 * 
 * \brief This function inserts a connection pointer by priority onto a queue.
 * 
 * \param[in] queue_ptr          The queue to insert the member into.
 * \param[in] sem_connection_ptr The connection pointer to insert into the queue. 
 */ 
void _sem_insert_priority_internal
(
    QUEUE_STRUCT_PTR          queue_ptr,
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr
)
{ /* Body */
    SEM_CONNECTION_STRUCT_PTR conn2_ptr;
    SEM_CONNECTION_STRUCT_PTR conn_prev_ptr;
    _mqx_uint                 count;
    _mqx_uint                 priority;

    conn_prev_ptr = (SEM_CONNECTION_STRUCT_PTR)((pointer)queue_ptr);
    conn2_ptr = (SEM_CONNECTION_STRUCT_PTR)((pointer)queue_ptr->NEXT);
    count = _QUEUE_GET_SIZE(queue_ptr) + 1;
    priority = sem_connection_ptr->TD_PTR->MY_QUEUE->PRIORITY;
    while (--count)
    {
        if (conn2_ptr->TD_PTR->MY_QUEUE->PRIORITY > priority)
        {
            break;
        } /* Endif */
        conn_prev_ptr = conn2_ptr;
        conn2_ptr = (SEM_CONNECTION_STRUCT_PTR)conn2_ptr->NEXT;
    } /* Endwhile */
    _QUEUE_INSERT(queue_ptr, conn_prev_ptr, sem_connection_ptr);

} /* Endbody */

/*!
 * \brief Opens a connection to the named semaphore.
 * 
 * \param[in]  name_ptr         Name that identifies the semaphore that was created 
 * using _sem_create().
 * \param[out] returned_sem_ptr Pointer to the semaphore handle, which is a 
 * connection to the semaphore.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Semaphore component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Semaphore component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the connection.)
 * \return SEM_SEMAPHORE_NOT_FOUND (Name is not in the semaphore names database.)
 * \return SEM_INVALID_SEMAPHORE (Data that is associated with the semaphore is 
 * not valid.)
 * \return SEM_SEMAPHORE_DELETED (Semaphore is in the process of being destroyed.)
 *   
 * \see _sem_open_fast
 * \see _sem_close
 * \see _sem_create
 * \see _sem_post
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until
 */
_mqx_uint _sem_open
(
    char_ptr      name_ptr,
    pointer _PTR_ returned_sem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR             kernel_data;
    register SEM_COMPONENT_STRUCT_PTR  sem_component_ptr;
    SEM_STRUCT_PTR                     sem_ptr;
    register SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    _mqx_max_type                      tmp;
    _mqx_uint                          result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_sem_open, name_ptr, returned_sem_ptr);

    *returned_sem_ptr = NULL;

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
#if MQX_CHECK_ERRORS
    if (sem_component_ptr == NULL)
    {
        _KLOGX3(KLOG_sem_open, MQX_COMPONENT_DOES_NOT_EXIST, 0);
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (sem_component_ptr->VALID != SEM_VALID)
    {
        _KLOGX3(KLOG_sem_open, MQX_INVALID_COMPONENT_BASE, 0);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE,
                    name_ptr, &tmp);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (result == NAME_NOT_FOUND)
        {
            _KLOGX3(KLOG_sem_open, SEM_SEMAPHORE_NOT_FOUND, NULL);
            return(SEM_SEMAPHORE_NOT_FOUND);
        } /* Endif */
        _KLOGX3(KLOG_sem_open, result, 0);
        return(result);
    } /* Endif */
#endif

    sem_ptr = (SEM_STRUCT_PTR)tmp;

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != SEM_VALID)
    {
        /* Semaphore not valid */
        _KLOGX2(KLOG_sem_open, SEM_INVALID_SEMAPHORE);
        return(SEM_INVALID_SEMAPHORE);
    } /* Endif */
#endif

#if MQX_COMPONENT_DESTRUCTION
    if (sem_ptr->DELAYED_DESTROY)
    {
        /* Semaphore in delayed destroy state */
        _KLOGX3(KLOG_sem_open, SEM_SEMAPHORE_DELETED, 0);
        return(SEM_SEMAPHORE_DELETED);
    } /* Endif */
#endif

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
    _mem_alloc_zero((_mem_size)sizeof(SEM_CONNECTION_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (sem_connection_ptr == NULL)
    {
        _KLOGX3(KLOG_sem_open, MQX_OUT_OF_MEMORY, 0);
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(sem_connection_ptr, MEM_TYPE_SEMAPHORE_CONNECTION);

    sem_connection_ptr->SEM_PTR = sem_ptr;
    sem_connection_ptr->VALID = SEM_VALID;
    sem_connection_ptr->TD_PTR = kernel_data->ACTIVE_PTR;

    *returned_sem_ptr = (pointer)sem_connection_ptr;

    _KLOGX3(KLOG_sem_open, MQX_OK, sem_connection_ptr);
    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Returns a semaphore to the semaphore, so another task may use it.
 * 
 * MQX gives the semaphore to the first waiting task and puts the task in the 
 * task's ready queue.
 * 
 * \param[in] users_sem_ptr Semaphore handle from _sem_open() or _sem_open_fast().
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return SEM_INVALID_SEMAPHORE_HANDLE (One of the following:
 * \li Sem_handle is not a valid semaphore handle.
 * \li Semaphore is strict and sem_handle was obtained byanother task.)
 * \return SEM_CANNOT_POST (Semaphore is strict and the task has not first waited 
 * for the semaphore.)
 * \return SEM_INVALID_SEMAPHORE (Sem_handle represents a semaphore that is no 
 * longer valid.)
 * \return SEM_INVALID_SEMAPHORE_COUNT (Semaphore data are corrupted.)
 * 
 * \warning Might put a task in its ready queue.
 * \warning For a strict semaphore, cannot be called from an ISR (ISR can call 
 * the function for a non-strict semaphore).
 * 
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_get_wait_count
 * \see _sem_get_value
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until      
 */ 
_mqx_uint _sem_post
(
    pointer users_sem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    SEM_STRUCT_PTR            sem_ptr;
#if MQX_COMPONENT_DESTRUCTION
    SEM_COMPONENT_STRUCT_PTR  sem_component_ptr;
#endif
    SEM_CONNECTION_STRUCT_PTR new_sem_connection_ptr;
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    TD_STRUCT_PTR             new_td_ptr;
    boolean                   task_added = FALSE;
    boolean                   destroying_semaphore = FALSE;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_sem_post, users_sem_ptr);

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)users_sem_ptr;

#if MQX_CHECK_VALIDITY
    if (sem_connection_ptr->VALID != SEM_VALID)
    {
        _KLOGX2(KLOG_sem_post, SEM_INVALID_SEMAPHORE_HANDLE);
        return(SEM_INVALID_SEMAPHORE_HANDLE);
    } /* Endif */
#endif

    sem_ptr = sem_connection_ptr->SEM_PTR;

#if MQX_CHECK_ERRORS
    if (sem_ptr->POLICY & SEM_STRICT)
    {
        if (kernel_data->IN_ISR)
        {
            _KLOGX2(KLOG_sem_post, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
            return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        } /* Endif */

        if (sem_connection_ptr->TD_PTR != kernel_data->ACTIVE_PTR)
        {
            /* ONLY OPENING task can use the semaphore */
            _KLOGX2(KLOG_sem_post, SEM_INVALID_SEMAPHORE_HANDLE);
            return(SEM_INVALID_SEMAPHORE_HANDLE);
        } /* Endif */

        if (sem_connection_ptr->POST_STATE == 0)
        {
            _KLOGX2(KLOG_sem_post, SEM_CANNOT_POST);
            return(SEM_CANNOT_POST);
        } /* Endif */
    } /* Endif */
#endif

    _INT_DISABLE();

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != SEM_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_sem_post, SEM_INVALID_SEMAPHORE);
        return(SEM_INVALID_SEMAPHORE);
    } /* Endif */
#endif

    if (sem_ptr->POLICY & SEM_STRICT)
    {
#if MQX_CHECK_ERRORS
        if (sem_ptr->COUNT > sem_ptr->MAX_COUNT)
        {
            /* Corruption somewhere */
            _int_enable();
            _KLOGX2(KLOG_sem_post, SEM_INVALID_SEMAPHORE_COUNT);
            return(SEM_INVALID_SEMAPHORE_COUNT);
        } /* Endif */
#endif

        --sem_connection_ptr->POST_STATE;
        if (sem_connection_ptr->POST_STATE == 0)
        {
            _QUEUE_REMOVE(&sem_ptr->OWNING_TASKS, sem_connection_ptr);
        } /* Endif */
    } /* Endif */

    if (_QUEUE_GET_SIZE(&sem_ptr->WAITING_TASKS))
    {
        /* Schedule a waiting task to run */

        new_sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
        ((pointer)sem_ptr->WAITING_TASKS.NEXT);
        while (new_sem_connection_ptr != (pointer)&sem_ptr->WAITING_TASKS)
        {
            new_td_ptr = new_sem_connection_ptr->TD_PTR;
            if ((new_td_ptr->STATE & STATE_MASK) == SEM_BLOCKED)
            {
                _TIME_DEQUEUE(new_td_ptr, kernel_data);
                _TASK_READY(new_td_ptr, kernel_data);
                new_td_ptr->INFO = SEM_AVAILABLE;
                task_added = TRUE;
                break;
            } /* Endif */
            new_sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
            new_sem_connection_ptr->NEXT;
        } /* Endwhile */

    } /* Endif */

    if (!task_added)
    {

        ++sem_ptr->COUNT;

#if MQX_COMPONENT_DESTRUCTION
        if ( sem_ptr->DELAYED_DESTROY )
        {
            if ( ( (sem_ptr->POLICY & SEM_STRICT) &&
                   (sem_ptr->COUNT == sem_ptr->MAX_COUNT) ) ||
                 ( !(sem_ptr->POLICY & SEM_STRICT) ) )
            {
                /* Destroy the semaphore name */
                sem_ptr->VALID = 0;
                destroying_semaphore = TRUE;
            } /* Endif */
        } /* Endif */
#endif
    } /* Endif */

    if (sem_connection_ptr->BOOSTED && (sem_connection_ptr->POST_STATE == 0))
    {
        /* This task was boosted by a waiting task */
        _sched_unboost_priority_internal(kernel_data->ACTIVE_PTR,
                        sem_connection_ptr->BOOSTED);
        sem_connection_ptr->BOOSTED = 0;
    } /* Endif */

    _INT_ENABLE();

    /* Let higher priority task run */
    _CHECK_RUN_SCHEDULER();

#if MQX_COMPONENT_DESTRUCTION
    if (destroying_semaphore)
    {
        sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
        if (sem_component_ptr != NULL)
        {
            _name_delete_internal(sem_component_ptr->NAME_TABLE_HANDLE,
                                  sem_ptr->NAME);
        } /* Endif */
        _mem_free(sem_ptr);
    } /* Endif */
#endif

    _KLOGX2(KLOG_sem_post, MQX_OK);
    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Used during task destruction to free up any semaphore resources owned 
 * by this task.
 * 
 * \param[in] td_ptr The task being destroyed. 
 */ 
void _sem_cleanup
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    SEM_CONNECTION_STRUCT_PTR connection_ptr;
    SEM_STRUCT_PTR            sem_ptr;

    connection_ptr = _mem_get_next_block_internal(td_ptr, NULL);
    while (connection_ptr)
    {
        if ((connection_ptr->VALID == SEM_VALID) &&
                        (connection_ptr->TD_PTR == td_ptr) )
        {
            sem_ptr = connection_ptr->SEM_PTR;
            if (sem_ptr->VALID == SEM_VALID)
            {
                /* Check if the connection is on the queue */
                _int_disable();
                sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
                ((pointer)sem_ptr->WAITING_TASKS.NEXT);
                while (sem_connection_ptr != (pointer)&sem_ptr->WAITING_TASKS.NEXT)
                {
                    if (sem_connection_ptr == connection_ptr)
                    {
                        /* Connection is queued, so dequeue it */
                        _QUEUE_REMOVE(&sem_ptr->WAITING_TASKS, connection_ptr);
                        break;
                    }/* Endif */
                    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
                    sem_connection_ptr->NEXT;
                } /* Endwhile */
                if (sem_ptr->POLICY & SEM_STRICT)
                {
                    while (connection_ptr->POST_STATE)
                    {
                        _sem_post(connection_ptr);
                    } /* Endwhile */
                }/* Endif */
                _int_enable();
            }/* Endif */
        } /* Endif */
        connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
        _mem_get_next_block_internal(td_ptr, connection_ptr);
    } /* Endwhile */

} /* Endbody */

/*!
 * \brief Tests the semaphore component for validity and consistency.
 * 
 * The function does the following:
 * \li Verifies semaphore component data.
 * \li Verifies the integrity of the entries in the semaphore names database.
 * \li For each semaphore, checks: validity of data (VALID field), integrity of 
 * the queue of waiting tasks and integrity of the queue of tasks that have the 
 * semaphore.
 * 
 * \param[out] sem_error_ptr Pointer to the semaphore that has an error (NULL if 
 * no errors are found).
 * 
 * \return MQX_OK (No errors were found.)
 * \return MQX_CORRUPT_QUEUE (Error was found in a semaphore queue.) 
 * \return MQX_INVALID_COMPONENT_BASE (Error was found in semaphore component data.)
 * \return SEM_INVALID_SEMAPHORE (Error was found in semaphore data.)
 * 
 * \warning Disables and enables interrupts.
 * 
 * \see _sem_close
 * \see _sem_create
 * \see _sem_create_fast
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_post
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until 
 */ 
_mqx_uint _sem_test
(
    pointer _PTR_ sem_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR           sem_ptr;
    pointer                  table_ptr;
    _mqx_max_type            data;
    _mqx_uint                result;
    _mqx_uint                i;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_sem_test, sem_error_ptr);

    *sem_error_ptr = NULL;

    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    if (sem_component_ptr == NULL)
    {
        _KLOGX3(KLOG_sem_test, MQX_OK, 0);
        return(MQX_OK);
    } /* Endif */

    if (sem_component_ptr->VALID != SEM_VALID)
    {
        _KLOGX3(KLOG_sem_test, MQX_INVALID_COMPONENT_BASE, 0);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    _int_disable();
    /* Test the semaphore name table */
    result = _name_test_internal((NAME_COMPONENT_STRUCT_PTR)
                    sem_component_ptr->NAME_TABLE_HANDLE, &table_ptr, sem_error_ptr);
    _int_enable();
    if (result != MQX_OK)
    {
        _KLOGX3(KLOG_sem_test, MQX_INVALID_COMPONENT_BASE, *sem_error_ptr);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    i = 0;
    while (TRUE)
    {
        _int_disable();
        result = _name_find_internal_by_index(
                        sem_component_ptr->NAME_TABLE_HANDLE, i++, &data);
        if (result != MQX_OK)
        {
            result = MQX_OK;
            break;
        } /* Endif */

        sem_ptr = (SEM_STRUCT_PTR)data;
        if (sem_ptr->VALID != SEM_VALID)
        {
            result = SEM_INVALID_SEMAPHORE;
            break;
        } /* Endif */
        result = _queue_test(&sem_ptr->WAITING_TASKS, sem_error_ptr);
        if (result != MQX_OK)
        {
            break;
        } /* Endif */
        result = _queue_test(&sem_ptr->OWNING_TASKS, sem_error_ptr);
        if (result != MQX_OK)
        {
            break;
        } /* Endif */
        _int_enable();

    } /* Endwhile */

    _int_enable();

    _KLOGX3(KLOG_sem_test, result, *sem_error_ptr);
    return(result);

} /* Endbody */

/*!
 * \brief Gets the value of the semaphore counter; that is, the number of 
 * subsequent requests that can get the semaphore without waiting.
 * 
 * \param[in] users_sem_ptr Semaphore handle from _sem_open() or _sem_open_fast().
 * 
 * \return Current value of the semaphore counter (success).
 * \return MAX_MQX_UINT (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li SEM_INVALID_SEMAPHORE_HANDLE (Sem_ptr is not a valid semaphore handle.)
 * \li SEM_INVALID_SEMAPHORE (Sem_ptr does not point to a valid semaphore.)
 * 
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_post
 * \see _sem_get_wait_count
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until 
 * \see _task_set_error
 */ 
_mqx_uint _sem_get_value
(
    pointer users_sem_ptr
)
{ /* Body */
    register SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    register SEM_STRUCT_PTR            sem_ptr;

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)users_sem_ptr;
#if MQX_CHECK_VALIDITY
    if (sem_connection_ptr->VALID != SEM_VALID)
    {
        _task_set_error(SEM_INVALID_SEMAPHORE_HANDLE);
        return(MAX_MQX_UINT);
    } /* Endif */
#endif

    sem_ptr = sem_connection_ptr->SEM_PTR;
#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != SEM_VALID)
    {
        _task_set_error(SEM_INVALID_SEMAPHORE);
        return(MAX_MQX_UINT);
    } /* Endif */
#endif

    return(sem_ptr->COUNT);

} /* Endbody */

/*!
 * \brief Gets the number of tasks that are waiting for the semaphore.
 * 
 * \param[in] users_sem_ptr Semaphore handle from _sem_open() or _sem_open_fast().
 * 
 * \return Number of tasks waiting for the semaphore (success).
 * \return MAX_MQX_UINT (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the task error codes 
 * described for _sem_get_value().
 *               
 * \see _sem_get_value 
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_post
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until 
 * \see _task_set_error  
 */ 
_mqx_uint _sem_get_wait_count
(
    pointer users_sem_ptr
)
{ /* Body */
    register SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    register SEM_STRUCT_PTR            sem_ptr;

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)users_sem_ptr;
#if MQX_CHECK_VALIDITY
    if (sem_connection_ptr->VALID != SEM_VALID)
    {
        _task_set_error(SEM_INVALID_SEMAPHORE_HANDLE);
        return(MAX_MQX_UINT);
    } /* Endif */
#endif

    sem_ptr = sem_connection_ptr->SEM_PTR;
#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != SEM_VALID)
    {
        _task_set_error(SEM_INVALID_SEMAPHORE);
        return(MAX_MQX_UINT);
    } /* Endif */
#endif

    return(_QUEUE_GET_SIZE(&(sem_ptr->WAITING_TASKS)));

} /* Endbody */

/*!
 * \private
 * 
 * \brief This function waits for a semaphore to become available. If one is not
 * the task is queued according to the queueing policy for this semaphore.
 *   
 * \param[in] users_sem_ptr      Semaphore handle from _sem_open() or 
 * _sem_open_fast().
 * \param[in] tick_ptr           One of the following:
 * \li Pointer to the maximum number of ticks to wait.
 * \li NULL (Unlimited wait.)
 * \param[in] ticks_are_absolute Specifies whether the wait time specified in 
 * ticks represents absolute or relative time.
 *  
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_EDEADLK (Function was aborted to prevent deadlock: the task has 
 * all the semaphore locks and, since the semaphore is strict, the task cannot 
 * post to "wake" itself.)
 * \return SEM_INVALID_SEMAPHORE_HANDLE (One of the following:
 * \li Sem_handle is not a valid semaphore handle.
 * \li Sem_handle was obtained by another task.)
 * \return SEM_INVALID_SEMAPHORE (Sem_handle is for a semaphore that is no longer 
 * valid.)
 * \return SEM_SEMAPHORE_DELETED (MQX is in the process of destroying the semaphore.)
 * \return SEM_WAIT_TIMEOUT (Timeout expired before the task can get the semaphore.)
 */ 
_mqx_uint _sem_wait_internal
(
    pointer             users_sem_ptr,
    MQX_TICK_STRUCT_PTR tick_ptr,
    boolean             ticks_are_absolute

)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR             kernel_data;
    TD_STRUCT_PTR                      td_ptr;
    SEM_STRUCT_PTR                     sem_ptr;
    register SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    register SEM_CONNECTION_STRUCT_PTR list_connection_ptr;
    SEM_CONNECTION_STRUCT_PTR          saved_connection_ptr;
    _mqx_uint                          priority;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        return(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)users_sem_ptr;

#if MQX_CHECK_VALIDITY
    if (sem_connection_ptr->VALID != SEM_VALID)
    {
        return(SEM_INVALID_SEMAPHORE_HANDLE);
    } /* Endif */
#endif

    sem_ptr = sem_connection_ptr->SEM_PTR;
    td_ptr = kernel_data->ACTIVE_PTR;

#if MQX_CHECK_ERRORS
    if (sem_connection_ptr->TD_PTR != td_ptr)
    {
        /* Only OPENING task can wait on the semaphore */
        return(SEM_INVALID_SEMAPHORE_HANDLE);
    } /* Endif */
#endif

    _INT_DISABLE();

#if MQX_CHECK_VALIDITY
    if (sem_ptr->VALID != SEM_VALID)
    {
        _int_enable();
        return(SEM_INVALID_SEMAPHORE);
    } /* Endif */
#endif

    if (sem_ptr->COUNT == 0)
    {

        /* Must wait for a semaphore to become available */
        if (sem_ptr->POLICY & SEM_STRICT)
        {

            /* 
             * First check for deadlock... See if this task already
             * owns all the semaphores, thus cannot post to wake itself up
             */
            if (sem_connection_ptr->POST_STATE == sem_ptr->MAX_COUNT)
            {
                _int_enable();
                return(MQX_EDEADLK);
            } /* Endif */

            /* 
             * May need to raise priorities of one task with semaphore,
             * If the current task has a higher priority.
             */
            if ((sem_ptr->POLICY & SEM_PRIORITY_INHERITANCE) &&
                            (_QUEUE_GET_SIZE(&sem_ptr->OWNING_TASKS) > 0))
            {

                /* Find owning task with highest priority */
                list_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
                ((pointer)sem_ptr->OWNING_TASKS.NEXT);
                saved_connection_ptr = list_connection_ptr;
                priority = kernel_data->LOWEST_TASK_PRIORITY;
                while ((pointer)list_connection_ptr !=
                                (pointer)&sem_ptr->OWNING_TASKS)
                {
                    if (list_connection_ptr->TD_PTR->MY_QUEUE->PRIORITY < priority)
                    {
                        priority = list_connection_ptr->TD_PTR->MY_QUEUE->PRIORITY;
                        saved_connection_ptr = list_connection_ptr;
                    } /* Endif */
                    list_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)
                    list_connection_ptr->NEXT;
                } /* Endwhile */

                if (priority > td_ptr->HOME_QUEUE->PRIORITY)
                {
                    if (priority > td_ptr->MY_QUEUE->PRIORITY)
                    {
                        /* Not high enough, boost the tasks priority */
                        _sched_boost_priority_internal(saved_connection_ptr->TD_PTR,
                                        (_mqx_uint)td_ptr->MY_QUEUE->PRIORITY);
                    } /* Endif */
                    saved_connection_ptr->BOOSTED++;
                } /* Endif */
            } /* Endif */
        } /* Endif */

        if (sem_ptr->POLICY & SEM_PRIORITY_QUEUEING)
        {
            /* 
             * We must insert the waiting task into the waiting list in order
             * of the priority of the waiting task.
             */
            _sem_insert_priority_internal(&sem_ptr->WAITING_TASKS, sem_connection_ptr);
        }
        else
        {
            /* Enqueue at end */
            _QUEUE_ENQUEUE(&sem_ptr->WAITING_TASKS, sem_connection_ptr);
        } /* Endif */

        /* Now put the task to sleep */
        td_ptr->STATE = SEM_BLOCKED;
        td_ptr->INFO = SEM_WANT_SEMAPHORE;
        /* Indicate wanting semaphore */

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

#if MQX_COMPONENT_DESTRUCTION
            if (sem_ptr->VALID != SEM_VALID)
            {
                _int_enable();
                return(SEM_SEMAPHORE_DELETED);
            } /* Endif */
#endif

            _QUEUE_REMOVE(&sem_ptr->WAITING_TASKS, sem_connection_ptr);

            if (td_ptr->INFO != SEM_AVAILABLE)
            {
                td_ptr->INFO = 0;
                _INT_ENABLE();
                return(SEM_WAIT_TIMEOUT);
            } /* Endif */

        }
        else
        {

            _task_block();

#if MQX_COMPONENT_DESTRUCTION
            if (sem_ptr->VALID != SEM_VALID)
            {
                _int_enable();
                return(SEM_SEMAPHORE_DELETED);
            } /* Endif */
#endif
            _QUEUE_REMOVE(&sem_ptr->WAITING_TASKS, sem_connection_ptr);

#if MQX_COMPONENT_DESTRUCTION
            if (td_ptr->INFO != SEM_AVAILABLE)
            {
                td_ptr->INFO = 0;
                _INT_ENABLE();
                return(SEM_SEMAPHORE_DELETED);
            } /* Endif */
#endif

        } /* Endif */

    }
    else
    {
        --sem_ptr->COUNT;
    } /* Endif */

    if (sem_ptr->POLICY & SEM_STRICT)
    {
        if (sem_connection_ptr->POST_STATE == 0)
        {
            _QUEUE_ENQUEUE(&sem_ptr->OWNING_TASKS, sem_connection_ptr);
        } /* Endif */
        sem_connection_ptr->POST_STATE++; /* # of posts allowed to do */
    } /* Endif */

    _INT_ENABLE();

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Wait for the semaphore for the specified number of milliseconds.
 * 
 * If the task cannot get the semaphore, MQX queues the task according to the 
 * semaphore's queuing policy, which is set when the semaphore is created.
 * 
 * \param[in] users_sem_ptr Semaphore handle from _sem_open() or _sem_open_fast().
 * \param[in] timeout       One of the following:
 * \li Maximum number of milliseconds to wait.
 * \li 0 (Unlimited wait.)
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_EDEADLK (Function was aborted to prevent deadlock: the task has 
 * all the semaphore locks and, since the semaphore is strict, the task cannot 
 * post to  "wake" itself.)
 * \return SEM_INVALID_SEMAPHORE_HANDLE (One of the following:
 * \li Sem_handle is not a valid semaphore handle.
 * \li Sem_handle was obtained by another task.)
 * \return SEM_INVALID_SEMAPHORE (Sem_handle is for a semaphore that is no longer 
 * valid.)
 * \return SEM_SEMAPHORE_DELETED (MQX is in the process of destroying the semaphore.)
 * \return SEM_WAIT_TIMEOUT (Timeout expired before the task can get the semaphore.)
 * 
 * \warning Might block the calling task.
 * \warning Cannot be called from an ISR.
 * 
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_wait_until
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_post
 * \see _sem_get_wait_count
 * \see _sem_get_value
 * \see _sem_create
 * \see _sem_create_fast
 * \see MQX_TICK_STRUCT
 */ 
_mqx_uint _sem_wait
(
    pointer users_sem_ptr,
    uint_32 timeout
)
{ /* Body */
    /* TIME_STRUCT                 time; */
    MQX_TICK_STRUCT ticks;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE3(KLOG_sem_wait, users_sem_ptr, timeout);

    if (timeout)
    {
        /* Convert milliseconds to ticks */
        /* time.MILLISECONDS = timeout % 1000; */
        /* time.SECONDS      = timeout / 1000; */
        /*                                     */
        /* PSP_TIME_TO_TICKS(&time, &ticks);   */
        PSP_MILLISECONDS_TO_TICKS_QUICK(timeout, &ticks);

        result = _sem_wait_internal(users_sem_ptr, &ticks, FALSE);
    }
    else
    {
        result = _sem_wait_internal(users_sem_ptr, NULL, FALSE);
    } /* Endif */

    _KLOGX2(KLOG_sem_wait, result);
    return(result);

} /* Endbody */

/*!
 * \brief Wait for the semaphore for the specified number of ticks (in tick time).
 * 
 * If the task cannot get the semaphore, MQX queues the task according to the 
 * semaphore's queuing policy, which is set when the semaphore is created.
 * 
 * \param[in] users_sem_ptr Semaphore handle from _sem_open() or _sem_open_fast().
 * \param[in] tick_ptr      One of the following:
 * \li Pointer to the maximum number of ticks to wait.
 * \li NULL (Unlimited wait.)
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_EDEADLK (Function was aborted to prevent deadlock: the task has 
 * all the semaphore locks and, since the semaphore is strict, the task cannot 
 * post to  "wake" itself.)
 * \return SEM_INVALID_SEMAPHORE_HANDLE (One of the following:
 * \li Sem_handle is not a valid semaphore handle.
 * \li Sem_handle was obtained by another task.)
 * \return SEM_INVALID_SEMAPHORE (Sem_handle is for a semaphore that is no longer 
 * valid.)
 * \return SEM_SEMAPHORE_DELETED (MQX is in the process of destroying the semaphore.)
 * \return SEM_WAIT_TIMEOUT (Timeout expired before the task can get the semaphore.)
 * 
 * \warning Might block the calling task.
 * \warning Cannot be called from an ISR.
 * 
 * \see _sem_wait
 * \see _sem_wait_ticks
 * \see _sem_wait_until
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_post
 * \see _sem_get_wait_count
 * \see _sem_get_value
 * \see _sem_create
 * \see _sem_create_fast
 * \see MQX_TICK_STRUCT
 */ 
_mqx_uint _sem_wait_for
(
    pointer             users_sem_ptr,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE3(KLOG_sem_wait_for, users_sem_ptr, tick_ptr);

    result = _sem_wait_internal(users_sem_ptr, tick_ptr, FALSE);

    _KLOGX2(KLOG_sem_wait_for, result);
    return(result);

} /* Endbody */

/*!
 * \brief Wait for the semaphore for the specified number of ticks.
 * 
 * If the task cannot get the semaphore, MQX queues the task according to the 
 * semaphore's queuing policy, which is set when the semaphore is created.
 * 
 * \param[in] users_sem_ptr    Semaphore handle from _sem_open() or _sem_open_fast().
 * \param[in] timeout_in_ticks One of the following:
 * \li Maximum number of ticks to wait.
 * \li 0 (Unlimited wait.)
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_EDEADLK (Function was aborted to prevent deadlock: the task has 
 * all the semaphore locks and, since the semaphore is strict, the task cannot 
 * post to  "wake" itself.)
 * \return SEM_INVALID_SEMAPHORE_HANDLE (One of the following:
 * \li Sem_handle is not a valid semaphore handle.
 * \li Sem_handle was obtained by another task.)
 * \return SEM_INVALID_SEMAPHORE (Sem_handle is for a semaphore that is no longer 
 * valid.)
 * \return SEM_SEMAPHORE_DELETED (MQX is in the process of destroying the semaphore.)
 * \return SEM_WAIT_TIMEOUT (Timeout expired before the task can get the semaphore.)
 * 
 * \warning Might block the calling task.
 * \warning Cannot be called from an ISR.
 * 
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_until
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_post
 * \see _sem_get_wait_count
 * \see _sem_get_value
 * \see _sem_create
 * \see _sem_create_fast
 * \see MQX_TICK_STRUCT
 */
_mqx_uint _sem_wait_ticks
(
    pointer   users_sem_ptr,
    _mqx_uint timeout_in_ticks
)
{ /* Body */
    MQX_TICK_STRUCT ticks;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE3(KLOG_sem_wait_ticks, users_sem_ptr, timeout_in_ticks);

    if (timeout_in_ticks)
    {
        ticks = _mqx_zero_tick_struct;

        PSP_GET_ELEMENT_FROM_TICK_STRUCT(&ticks, 0) = timeout_in_ticks;
        result = _sem_wait_internal(users_sem_ptr, &ticks, FALSE);
    }
    else
    {
        result = _sem_wait_internal(users_sem_ptr, NULL, FALSE);
    } /* Endif */

    _KLOGX2(KLOG_sem_wait_ticks, result);
    return(result);

} /* Endbody */

/*!
 * \brief Wait for the semaphore until the specified time (in tick time).
 * 
 * If the task cannot get the semaphore, MQX queues the task according to the 
 * semaphore's queuing policy, which is set when the semaphore is created.
 * 
 * \param[in] users_sem_ptr Semaphore handle from _sem_open() or _sem_open_fast().
 * \param[in] tick_ptr      One of the following:
 * \li Pointer to the time (in tick time) until which to wait.
 * \li NULL (Unlimited wait.)
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_EDEADLK (Function was aborted to prevent deadlock: the task has 
 * all the semaphore locks and, since the semaphore is strict, the task cannot 
 * post to  "wake" itself.)
 * \return SEM_INVALID_SEMAPHORE_HANDLE (One of the following:
 * \li Sem_handle is not a valid semaphore handle.
 * \li Sem_handle was obtained by another task.)
 * \return SEM_INVALID_SEMAPHORE (Sem_handle is for a semaphore that is no longer 
 * valid.)
 * \return SEM_SEMAPHORE_DELETED (MQX is in the process of destroying the semaphore.)
 * \return SEM_WAIT_TIMEOUT (Timeout expired before the task can get the semaphore.)
 * 
 * \warning Might block the calling task.
 * \warning Cannot be called from an ISR.
 * 
 * \see _sem_wait
 * \see _sem_wait_for
 * \see _sem_wait_ticks
 * \see _sem_open
 * \see _sem_open_fast
 * \see _sem_post
 * \see _sem_get_wait_count
 * \see _sem_get_value
 * \see _sem_create
 * \see _sem_create_fast
 * \see MQX_TICK_STRUCT
 */ 
_mqx_uint _sem_wait_until
(
    pointer             users_sem_ptr,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _mqx_uint result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE3(KLOG_sem_wait_until, users_sem_ptr, tick_ptr);

    result = _sem_wait_internal(users_sem_ptr, tick_ptr, TRUE);

    _KLOGX2(KLOG_sem_wait_until, result);
    return(result);

} /* Endbody */

#endif /* MQX_USE_SEMAPHORES */

/* EOF */
