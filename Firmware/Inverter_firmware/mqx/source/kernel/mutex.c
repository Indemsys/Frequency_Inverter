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
 * $FileName: mutex.c$
 * $Version : 3.8.0.1$
 * $Date    : Feb-20-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Mutex component.
 *
 *END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_MUTEXES
#include "mutex.h"
#include "mutex_prv.h"

#if MQX_LITE_VERSION_NUMBER
MUTEX_COMPONENT_STRUCT mutex_struct;
#endif /* MQX_LITE_VERSION_NUMBER */

/*!
 * \brief Initializes the mutex attributes structure to default values.
 *
 * This function initializes the mutex attributes structure to default values and
 * validates the structure. It must be called before a task can modify the values
 * of the mutex attributes structure. This function does not affect any mutexes
 * already initialized with this structure.
 *
 * <table>
 *  <tr>
 *     <td><b> Mutex attribute </b></td>
 *     <td><b> Field in MUTEX_ATTR_STRUCT </b></td>
 *     <td><b> Default value </b></td>
 *  </tr>
 *  <tr>
 *     <td> Scheduling protocol </td>
 *     <td> POLICY </td>
 *     <td> MUTEX_NO_PRIO_INHERIT </td>
 *  </tr>
 *  <tr>
 *     <td> Valid </td>
 *     <td> VALID </td>
 *     <td> TRUE </td>
 *  </tr>
 *  <tr>
 *     <td> Priority </td>
 *     <td> PRIORITY </td>
 *     <td> 0 </td>
 *  </tr>
 *  <tr>
 *     <td> Spin limit </td>
 *     <td> COUNT </td>
 *     <td> 0 </td>
 *  </tr>
 *  <tr>
 *     <td> Waiting protocol </td>
 *     <td> WAITING POLICY </td>
 *     <td> MUTEX_QUEUEING </td>
 *  </tr>
 * </table>
 *
 * \param[in] attr_ptr Pointer to the mutex attributes structure to initialize.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL.)
 *
 * \see _mutex_init
 * \see _mutatr_destroy
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_init
(
    register MUTEX_ATTR_STRUCT_PTR attr_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE2(KLOG_mutatr_init, attr_ptr);

#if MQX_CHECK_ERRORS
    if (attr_ptr == NULL)
    {
        _KLOGX2(KLOG_mutatr_init, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    attr_ptr->SCHED_PROTOCOL   = MUTEX_NO_PRIO_INHERIT;
    attr_ptr->PRIORITY_CEILING = 0;
    attr_ptr->VALID            = MUTEX_VALID;
    attr_ptr->COUNT            = 0;
    attr_ptr->WAIT_PROTOCOL    = MUTEX_QUEUEING;

    _KLOGX2(KLOG_mutatr_init, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Installs the mutex component into the kernel.
 *
 * MQX calls this function if the mutex component is not created when a task calls
 * _mutex_init().
 *
 * \return MQX_OK
 * \return MQX_OUT_OF_MEMORY
 *
 * \see _mutex_init
 * \see _mutatr_init
 */
_mqx_uint _mutex_create_component(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    register MUTEX_COMPONENT_STRUCT_PTR mutex_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE1(KLOG_mutex_create_component);

    if (kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] != NULL)
    {
        _KLOGX2(KLOG_mutex_create_component, MQX_OK);
        return (MQX_OK);
    } /* Endif */

#if MQX_LITE_VERSION_NUMBER
    mutex_component_ptr = &mutex_struct;
#else
    mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) _mem_alloc_system_zero(
                    (_mem_size) sizeof(MUTEX_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (mutex_component_ptr == NULL)
    {
        _KLOGX2(KLOG_mutex_create_component, MQX_OUT_OF_MEMORY);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(mutex_component_ptr, MEM_TYPE_MUTEX_COMPONENT);
#endif /* MQX_LITE_VERSION_NUMBER */

    _QUEUE_INIT(&mutex_component_ptr->MUTEXES, 0);
    mutex_component_ptr->VALID = MUTEX_VALID;

    _int_disable();

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] != NULL)
    {
        _int_enable();
#if !MQX_LITE_VERSION_NUMBER
        _mem_free(mutex_component_ptr);
#endif /* MQX_LITE_VERSION_NUMBER */
        _KLOGX2(KLOG_mutex_create_component, MQX_OK);
        return (MQX_OK);
    } /* Endif */
#endif

    kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] = mutex_component_ptr;

#if MQX_COMPONENT_DESTRUCTION
    kernel_data->COMPONENT_CLEANUP[KERNEL_MUTEXES] = _mutex_cleanup;
#endif

    _int_enable();

    _KLOGX2(KLOG_mutex_create_component, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Deinitializes the mutex attributes structure.
 *
 * To reuse the mutex attributes structure, a task must reinitialize the structure.
 *
 * \param[in] attr_ptr Pointer to the mutex attributes structure; initialized
 * with _mutatr_init().
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attributes structure.)
 *
 * \see _mutatr_init
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_destroy
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE2(KLOG_mutatr_destroy, attr_ptr);

#if MQX_CHECK_ERRORS
    if (attr_ptr == NULL)
    {
        _KLOGX2(KLOG_mutatr_destroy, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutatr_destroy, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    attr_ptr->VALID = 0;

    _KLOGX2(KLOG_mutatr_destroy, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Deinitializes the mutex.
 *
 * To reuse the mutex after it is destroyed, a task must reinitialize it with _mutex_create().
 *
 * \param[in] mutex_ptr Pointer to the mutex to be deinitialized.
 *
 * \return MQX_EOK
 * \return MQX_COMPONENT_DOES_NOT_EXIST
 * \return MQX_INVALID_COMPONENT_BASE (Mutex component data is not valid.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (From _mutex_lock: function cannot
 * be called from an ISR.)
 * \return MQX_EINVAL (From _mutex_lock: mutex_ptr was destroyed or is NULL.)
 * \return MQX_EDEADLK (From _mutex_lock: task already has the mutex locked.)
 * \return MQX_EBUSY (From _mutex_lock: mutex is already locked.)
 *
 * \warning Puts in their ready queues all tasks that are waiting for the mutex;
 * their call to _mutex_lock() returns MQX_EINVAL.
 *
 * \see _mutex_init
 * \see MUTEX_STRUCT
 */
_mqx_uint _mutex_destroy
(
    register MUTEX_STRUCT_PTR mutex_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    MUTEX_COMPONENT_STRUCT_PTR mutex_component_ptr;
    TD_STRUCT_PTR              td_ptr;
    _mqx_uint                  result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_mutex_destroy, mutex_ptr);

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] == NULL)
    {
        _KLOGX2(KLOG_mutex_destroy, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];

#if MQX_CHECK_VALIDITY
    if (mutex_component_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutex_destroy, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    td_ptr = kernel_data->ACTIVE_PTR;

    if (mutex_ptr->OWNER_TD != td_ptr)
    {
        result = _mutex_lock(mutex_ptr); /* Get the mutex */
        if (result != MQX_EOK)
        {
            _KLOGX2(KLOG_mutex_destroy, result);
            return (result);
        } /* Endif */
    } /* Endif */

    mutex_ptr->VALID = 0;

    if (mutex_ptr->LINK.NEXT != NULL)
    {
        /* Remove mutex from the queue of mutexes in the kernel */
        _int_disable();
        _QUEUE_REMOVE(&mutex_component_ptr->MUTEXES, mutex_ptr);
        _int_enable();
    } /* Endif */

    /* Get rid of all waiting tasks */
    while (_QUEUE_GET_SIZE(&mutex_ptr->WAITING_TASKS))
    {
        _QUEUE_DEQUEUE(&mutex_ptr->WAITING_TASKS, td_ptr);
        _task_set_error_td_internal(td_ptr, MQX_EINVAL);
        _int_disable();
        _TASK_READY(td_ptr,kernel_data);
        _int_enable();
    } /* Endwhile */

    td_ptr = kernel_data->ACTIVE_PTR;

    _int_disable();
    if (mutex_ptr->BOOSTED)
    {
        _sched_unboost_priority_internal(td_ptr, mutex_ptr->BOOSTED);
    } /* Endif */

    _int_enable();

    _CHECK_RUN_SCHEDULER(); /* Let higher priority task run */

    _KLOGX2(KLOG_mutex_destroy, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Gets the waiting policy of the mutex attributes structure.
 *
 * \param[in]  attr_ptr             Pointer to an initialized mutex attributes
 * structure.
 * \param[out] waiting_protocol_ptr Pointer to the current waiting protocol.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attribute structure.)
 *
 * \see _mutatr_set_wait_protocol
 * \see _mutatr_init
 * \see _mutatr_get_spin_limit
 * \see _mutatr_set_spin_limit
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_get_wait_protocol
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint_ptr         waiting_protocol_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if ((attr_ptr == NULL) || (waiting_protocol_ptr == NULL))
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif

    *waiting_protocol_ptr = attr_ptr->WAIT_PROTOCOL;
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Gets the priority ceiling from a mutex attributes structure.
 *
 * Priority applies only to mutexes whose scheduling protocol is priority protect.
 *
 * \param[in]  attr_ptr    Pointer to an initialized mutex attributes structure.
 * \param[out] ceiling_ptr Pointer to the current priority.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attributes structure.)
 *
 * \see _mutatr_set_priority_ceiling
 * \see _mutatr_init
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_get_priority_ceiling
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint_ptr         ceiling_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if ((attr_ptr == NULL) || (ceiling_ptr == NULL))
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif

    *ceiling_ptr = attr_ptr->PRIORITY_CEILING;
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Gets the scheduling protocol of the mutex attributes structure.
 *
 * \param[in]  attr_ptr     Pointer to an initialized mutex attributes structure.
 * \param[out] protocol_ptr Pointer to the current scheduling protocol.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attributes structure.)
 *
 * \see _mutatr_set_sched_protocol
 * \see _mutatr_init
 * \see _mutatr_get_priority_ceiling
 * \see _mutatr_set_priority_ceiling
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_get_sched_protocol
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint_ptr         protocol_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if ((attr_ptr == NULL) || (protocol_ptr == NULL))
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif

    *protocol_ptr = attr_ptr->SCHED_PROTOCOL;
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Gets the spin limit of the mutex attributes structure.
 *
 * Spin limit applies only to mutexes whose waiting policy is limited spin. Spin
 * limit is the number of times that a task spins (is rescheduled) while it waits
 * for the mutex.
 *
 * \param[in]  attr_ptr       Pointer to an initialized mutex attributes structure.
 * \param[out] spin_count_ptr Pointer to the current spin limit.
 *
 * \return MQX_OK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attributes structure.)
 *
 * \see _mutatr_set_spin_limit
 * \see _mutatr_init
 * \see _mutatr_get_wait_protocol
 * \see _mutatr_set_wait_protocol
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_get_spin_limit
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint_ptr         spin_count_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if ((attr_ptr == NULL) || (spin_count_ptr == NULL))
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif

    *spin_count_ptr = attr_ptr->COUNT;
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Initializes the mutex.
 *
 * \param[in] mutex_ptr Pointer to where the mutex is to be initialized.
 * \param[in] attr_ptr  Pointer to an initialized mutex attributes structure or
 * NULL (use default attributes as defined for _mutatr_init()).
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (Mutex_ptr is NULL, attr_ptr is not initialized or a value
 * in attr_ptr is not correct.)
 * \return MQX_INVALID_COMPONENT_BASE (Mutex component data is not valid.)
 * \return MQX_OUT_OF_MEMORY
 *
 * \warning Creates the mutex component if it was not previously created.
 *
 * \see _mutex_destroy
 * \see _mutatr_init
 * \see MUTEX_STRUCT
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutex_init
(
    register MUTEX_STRUCT_PTR      mutex_ptr,
    register MUTEX_ATTR_STRUCT_PTR attr_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    MUTEX_COMPONENT_STRUCT_PTR mutex_component_ptr;
    MUTEX_ATTR_STRUCT          default_attr;
#if MQX_CHECK_ERRORS
    MUTEX_STRUCT_PTR           mutex_chk_ptr;
#endif
    _mqx_uint                  result;

    _GET_KERNEL_DATA(kernel_data);
    if (attr_ptr == NULL)
    {
        attr_ptr = &default_attr;
        _mutatr_init(attr_ptr);
        _KLOGE3(KLOG_mutex_init, mutex_ptr, NULL);
    }
    else
    {
        _KLOGE3(KLOG_mutex_init, mutex_ptr, attr_ptr);
    } /* Endif */

#if MQX_CHECK_ERRORS
    if (mutex_ptr == NULL)
    {
        _KLOGX2(KLOG_mutex_init, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutex_init, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
    if (mutex_component_ptr == NULL)
    {
        result = _mutex_create_component();
        mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (mutex_component_ptr == NULL)
        {
            _KLOGX2(KLOG_mutex_init, result);
            return (result);
        } /* Endif */
#endif
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (mutex_component_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutex_init, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    _int_disable();
#if MQX_CHECK_ERRORS
    /* Check if mutex is already initialized */
    mutex_chk_ptr = (MUTEX_STRUCT_PTR) ((pointer) mutex_component_ptr->MUTEXES.NEXT);
    while (mutex_chk_ptr != (MUTEX_STRUCT_PTR) ((pointer) &mutex_component_ptr->MUTEXES))
    {
        if (mutex_chk_ptr == mutex_ptr)
        {
            _int_enable();
            _KLOGX2(KLOG_mutex_init, MQX_EINVAL);
            return (MQX_EINVAL);
        } /* Endif */
        mutex_chk_ptr = (MUTEX_STRUCT_PTR) ((pointer) mutex_chk_ptr->LINK.NEXT);
    } /* Endif */
#endif

    mutex_ptr->PROTOCOLS = attr_ptr->SCHED_PROTOCOL | attr_ptr->WAIT_PROTOCOL;
    mutex_ptr->VALID            = MUTEX_VALID;
    mutex_ptr->COUNT            = attr_ptr->COUNT;
    mutex_ptr->PRIORITY_CEILING = attr_ptr->PRIORITY_CEILING;
    mutex_ptr->LOCK             = 0;
    mutex_ptr->BOOSTED          = 0;
    mutex_ptr->OWNER_TD         = NULL;
    _QUEUE_INIT(&mutex_ptr->WAITING_TASKS, 0);

    _QUEUE_ENQUEUE(&mutex_component_ptr->MUTEXES, mutex_ptr);
    _int_enable();

    _KLOGX2(KLOG_mutex_init, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Locks the mutex.
 *
 * If the mutex is already locked, the task waits according to the waiting protocol
 * of the mutex.
 *
 * \param[in] mutex_ptr Pointer to the mutex to be locked.
 *
 * \return MQX_EOK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_EINVAL (Mutex_ptr is NULL or mutex was destroyed.)
 * \return MQX_EDEADLK (Task already has the mutex locked.)
 * \return MQX_EBUSY (Mutex is already locked.)
 *
 * \warning Might block the calling task.
 * \warning Cannot be called from an ISR.
 *
 * \see _mutex_init
 * \see _mutex_try_lock
 * \see _mutex_unlock
 * \see _mutatr_init
 * \see _mutatr_get_wait_protocol
 * \see _mutatr_set_wait_protocol
 * \see _mutex_destroy
 * \see MUTEX_STRUCT
 */
_mqx_uint _mutex_lock
(
    register MUTEX_STRUCT_PTR mutex_ptr
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR          td_ptr;
    volatile uchar _PTR_            lock_ptr;
    _mqx_uint                       priority;
    _mqx_uint                       count;
#if MQX_MUTEX_HAS_POLLING
    _mqx_uint                       limited;
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_mutex_lock, mutex_ptr);

    td_ptr = kernel_data->ACTIVE_PTR;

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_mutex_lock, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
    if (mutex_ptr == NULL)
    {
        _KLOGX2(KLOG_mutex_lock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (mutex_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutex_lock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    if (mutex_ptr->OWNER_TD == (pointer) td_ptr)
    {
        _KLOGX2(KLOG_mutex_lock, MQX_EDEADLK);
        return (MQX_EDEADLK);
    } /* Endif */

    priority = td_ptr->MY_QUEUE->PRIORITY;
    lock_ptr = &mutex_ptr->LOCK;

#if MQX_MUTEX_HAS_POLLING
    if ((mutex_ptr->PROTOCOLS & MUTEX_SPIN_ONLY) ||
        (mutex_ptr->PROTOCOLS & MUTEX_LIMITED_SPIN))
    {

        if (mutex_ptr->PROTOCOLS & MUTEX_SPIN_ONLY)
        {
            count = 1;
            limited = 0;
        }
        else
        {
            count = mutex_ptr->COUNT;
            limited = 1;
        } /* Endif */
        if (mutex_ptr->PROTOCOLS & MUTEX_PRIO_INHERIT)
        {

            _INT_DISABLE();
            while (count && *lock_ptr)
            {
                count -= limited;
                if (((TD_STRUCT_PTR)(mutex_ptr->OWNER_TD))->HOME_QUEUE->PRIORITY >
                    priority)
                {
                    if (((TD_STRUCT_PTR)(mutex_ptr->OWNER_TD))->MY_QUEUE->PRIORITY >
                        priority)
                    {
                        _sched_boost_priority_internal(
                                        (TD_STRUCT_PTR)mutex_ptr->OWNER_TD, priority);
                    } /* Endif */
                    mutex_ptr->BOOSTED++;
                } /* Endif */
                _INT_ENABLE(); /* Allow interruption and tasks of same prio a kick*/
                _sched_yield();
                _INT_DISABLE();
#if MQX_COMPONENT_DESTRUCTION
                if (mutex_ptr->VALID != MUTEX_VALID)
                {
                    _int_enable();
                    _KLOGX2(KLOG_mutex_lock, MQX_EINVAL);
                    return(MQX_EINVAL);
                } /* Endif */
#endif
            } /* Endwhile */
            if (count)
            {
                /* Test and set sets the high bit. */
                mutex_ptr->LOCK = MQX_TEST_AND_SET_VALUE;
            } /* Endif */
            _INT_ENABLE();

        }
        else
        {

            while (count && _mem_test_and_set((uchar_ptr)lock_ptr))
            {
                count -= limited;
                _sched_yield();
#if MQX_COMPONENT_DESTRUCTION
                if (mutex_ptr->VALID != MUTEX_VALID)
                {
                    _KLOGX2(KLOG_mutex_lock, MQX_EINVAL);
                    return(MQX_EINVAL);
                } /* Endif */
#endif
            } /* Endwhile */

        } /* Endif */

        if (count == 0)
        {
            _KLOGX2(KLOG_mutex_lock, MQX_EBUSY);
            return(MQX_EBUSY);
        } /* Endif */
        mutex_ptr->OWNER_TD = td_ptr;

    }
    else
    {

#endif

    _INT_DISABLE();
    if (*lock_ptr)
    {
        /* Must wait for a mutex to become available */

        /* Boost the priority of the owner task if necessary */
        if (mutex_ptr->PROTOCOLS & MUTEX_PRIO_INHERIT) {
            if (((TD_STRUCT_PTR) (mutex_ptr->OWNER_TD))->HOME_QUEUE->PRIORITY > priority)
            {
                if (((TD_STRUCT_PTR) (mutex_ptr->OWNER_TD))->MY_QUEUE->PRIORITY > priority)
                {
                    _sched_boost_priority_internal((TD_STRUCT_PTR) mutex_ptr->OWNER_TD, priority);
                } /* Endif */
                mutex_ptr->BOOSTED++;
            } /* Endif */
        } /* Endif */

        _QUEUE_UNLINK(td_ptr); /* Remove from ready to run queue */
        td_ptr->INFO = (_mqx_uint) &mutex_ptr->WAITING_TASKS;
        if (mutex_ptr->PROTOCOLS & MUTEX_PRIORITY_QUEUEING)
        {
            /*
             ** The waiting task must be inserted into the waiting list in order
             ** of the priority of the waiting task.
             */
            _sched_insert_priorityq_internal(&mutex_ptr->WAITING_TASKS, td_ptr);
        }
        else
        {
            /* Enqueue at end */
            _QUEUE_ENQUEUE(&mutex_ptr->WAITING_TASKS, td_ptr);
        } /* Endif */

        /* Now put the task to sleep */
        td_ptr->STATE = MUTEX_BLOCKED;
        _sched_execute_scheduler_internal();

        /* If mutex alive, then active task owns it now, (done by unlock) */
#if MQX_COMPONENT_DESTRUCTION
        if (mutex_ptr->VALID != MUTEX_VALID)
        {
            _int_enable();
            _KLOGX2(KLOG_mutex_lock, MQX_EINVAL);
            return (MQX_EINVAL);
        } /* Endif */
#endif

    }
    else
    {
        /* Test and set sets the high bit. */
        mutex_ptr->LOCK = MQX_TEST_AND_SET_VALUE;
        mutex_ptr->OWNER_TD = td_ptr;
    } /* Endif */

    /*
     * This task now owns the mutex, if priority inheritance is in effect
     * the queue of waiting tasks must be checked for any task which may
     * have a higher priority than mine ONLY if priority queueing is NOT
     * in effect
     */
    if ((mutex_ptr->PROTOCOLS & MUTEX_PRIO_INHERIT) && !(mutex_ptr->PROTOCOLS & MUTEX_PRIORITY_QUEUEING))
    {
        count = _sched_get_max_priority_on_q_internal(&mutex_ptr->WAITING_TASKS);
        if (count < td_ptr->HOME_QUEUE->PRIORITY)
        {
            if (count < td_ptr->MY_QUEUE->PRIORITY)
            {
                _sched_boost_priority_internal(td_ptr, count);
            } /* Endif */
            mutex_ptr->BOOSTED++;
        } /* Endif */
    } /* Endif */
    _INT_ENABLE();

#if MQX_MUTEX_HAS_POLLING
} /* Endif */
#endif

    if (mutex_ptr->PROTOCOLS & MUTEX_PRIO_PROTECT)
    {
        /* Switch my priority to the higher one if necessary */
        _INT_DISABLE();
        if (mutex_ptr->PRIORITY_CEILING < td_ptr->HOME_QUEUE->PRIORITY)
        {
            if (mutex_ptr->PRIORITY_CEILING < td_ptr->MY_QUEUE->PRIORITY)
            {
                _sched_boost_priority_internal(td_ptr, mutex_ptr->PRIORITY_CEILING);
            } /* Endif */
            mutex_ptr->BOOSTED++;
        } /* Endif */
        _INT_ENABLE();
    } /* Endif */

    _KLOGX2(KLOG_mutex_lock, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Sets the waiting policy of the mutex attributes structure.
 *
 * \param[in] attr_ptr         Pointer to an initialized mutex attributes structure.
 * \param[in] waiting_protocol New waiting protocol (see waiting protocols).
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attribute structure.)
 *
 * \see _mutatr_get_wait_protocol
 * \see _mutatr_init
 * \see _mutatr_get_spin_limit
 * \see _mutatr_set_spin_limit
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_set_wait_protocol
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint             waiting_protocol
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE3(KLOG_mutatr_set_wait_protocol, attr_ptr, waiting_protocol);

#if MQX_CHECK_ERRORS
    if (attr_ptr == NULL)
    {
        _KLOGX2(KLOG_mutatr_set_wait_protocol, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutatr_set_wait_protocol, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    /* Validate input value */
    if (!(
#if MQX_MUTEX_HAS_POLLING
    (waiting_protocol == MUTEX_SPIN_ONLY) ||
    (waiting_protocol == MUTEX_LIMITED_SPIN) ||
#endif
    (waiting_protocol == MUTEX_QUEUEING) || (waiting_protocol == MUTEX_PRIORITY_QUEUEING)))
    {
        _KLOGX2(KLOG_mutatr_set_wait_protocol, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    attr_ptr->WAIT_PROTOCOL = waiting_protocol;

    _KLOGX2(KLOG_mutatr_set_wait_protocol, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Sets the priority ceiling of a mutex attributes structure.
 *
 * Priority applies only to mutexes whose scheduling protocol is priority protect.
 *
 * \param[in] attr_ptr Pointer to an initialized mutex attributes structure.
 * \param[in] ceiling New priority ceiling to use.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attribute structure.)
 *
 * \see _mutatr_get_priority_ceiling
 * \see _mutatr_init
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_set_priority_ceiling
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint             ceiling
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_mutatr_set_priority_ceiling, attr_ptr, ceiling);

#if MQX_CHECK_ERRORS
    if (attr_ptr == NULL)
    {
        _KLOGX2(KLOG_mutatr_set_priority_ceiling, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutatr_set_priority_ceiling, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    if (ceiling > kernel_data->LOWEST_TASK_PRIORITY)
    {
        _KLOGX2(KLOG_mutatr_set_priority_ceiling, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    attr_ptr->PRIORITY_CEILING = ceiling;

    _KLOGX2(KLOG_mutatr_set_priority_ceiling, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Sets the scheduling protocol of the mutex attributes structure.
 *
 * \param[in] attr_ptr Pointer to an initialized mutex attributes structure.
 * \param[in] protocol New scheduling protocol (see scheduling protocols).
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attributes structure.)
 *
 * \see _mutatr_get_sched_protocol
 * \see _mutatr_init
 * \see _mutatr_get_priority_ceiling
 * \see _mutatr_set_priority_ceiling
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_set_sched_protocol
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint             protocol
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE3(KLOG_mutatr_set_sched_protocol, attr_ptr, protocol);

#if MQX_CHECK_ERRORS
    if (attr_ptr == NULL)
    {
        _KLOGX2(KLOG_mutatr_set_sched_protocol, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutatr_set_sched_protocol, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    /* Validate protocol value */
    if (!((protocol == MUTEX_PRIO_INHERIT) || (protocol == MUTEX_PRIO_PROTECT) || (protocol == MUTEX_NO_PRIO_INHERIT)))
    {
        _KLOGX2(KLOG_mutatr_set_sched_protocol, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    attr_ptr->SCHED_PROTOCOL = protocol;

    _KLOGX2(KLOG_mutatr_set_sched_protocol, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Sets the spin limit of the mutex attributes structure.
 *
 * Spin limit applies only to mutexes whose waiting policy is limited spin. Spin
 * limit is the number of times that a task spins (is rescheduled) while it waits
 * for the mutex.
 *
 * \param[in] attr_ptr Pointer to an initialized mutex attributes structure.
 * \param[in] spin_count New spin limit.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (attr_ptr is NULL or points to an invalid attributes structure.)
 *
 * \see _mutatr_get_spin_limit
 * \see _mutatr_init
 * \see _mutatr_get_wait_protocol
 * \see _mutatr_set_wait_protocol
 * \see MUTEX_ATTR_STRUCT
 */
_mqx_uint _mutatr_set_spin_limit
(
    MUTEX_ATTR_STRUCT_PTR attr_ptr,
    _mqx_uint             spin_count
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE3(KLOG_mutatr_set_spin_limit, attr_ptr, spin_count);

#if MQX_MUTEX_HAS_POLLING
#if MQX_CHECK_ERRORS
    if (attr_ptr == NULL)
    {
        _KLOGX2(KLOG_mutatr_set_spin_limit, MQX_EINVAL);
        return(MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (attr_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutatr_set_spin_limit, MQX_EINVAL);
        return(MQX_EINVAL);
    } /* Endif */
#endif

    attr_ptr->COUNT = spin_count;

    _KLOGX2(KLOG_mutatr_set_spin_limit, MQX_EOK);
    return(MQX_EOK);
#else
    _KLOGX2(KLOG_mutatr_set_spin_limit, MQX_EINVAL);
    return (MQX_EINVAL);
#endif

} /* Endbody */

/*!
 * \brief Used during task destruction to free up any mutex owned by this task.
 *
 * \param[in] td_ptr Pointer to the task descriptor of the task to be destroyed.
 *
 * \see mutex_destroy
 */
void _mutex_cleanup
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    MUTEX_COMPONENT_STRUCT_PTR mutex_component_ptr;
    MUTEX_STRUCT_PTR           mutex_ptr;

    _GET_KERNEL_DATA(kernel_data);

    mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
    if (mutex_component_ptr == NULL)
    {
        return; /* No work to do! */
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (mutex_component_ptr->VALID != MUTEX_VALID)
    {
        return;
    } /* Endif */
#endif

    _int_disable();
    mutex_ptr = (MUTEX_STRUCT_PTR) ((pointer) mutex_component_ptr->MUTEXES.NEXT);
    while (mutex_ptr != (MUTEX_STRUCT_PTR) ((pointer) &mutex_component_ptr->MUTEXES))
    {
        if ((mutex_ptr->LOCK) && (mutex_ptr->OWNER_TD == td_ptr))
        {
            mutex_ptr->OWNER_TD = kernel_data->ACTIVE_PTR;
            _mutex_unlock(mutex_ptr);
            mutex_ptr = (MUTEX_STRUCT_PTR) ((pointer) mutex_component_ptr->MUTEXES.NEXT);
        }
        else
        {
            mutex_ptr = (MUTEX_STRUCT_PTR) ((pointer) mutex_ptr->LINK.NEXT);
        } /* Endif */
    } /* Endwhile */
    _int_enable();

} /* Endbody */

/*!
 * \brief Tests the mutex component.
 *
 * This function tests:
 * \n - mutex component data
 * \n - MQX queue of mutexes
 * \n - each mutex
 * \n - waiting queue of each mutex
 *
 * \param[out] mutex_error_ptr Pointer to the invalid queue or to the mutex with the
 * error (see return).
 *
 * \return MQX_OK No errors were found (mutex_error_ptr = NULL).
 * \return MQX_INVALID_COMPONENT_BASE Mutex component data is not valid (mutex_error_ptr
 * = NULL).
 * \return MQX_EINVAL A mutex is not valid or a mutex queue is not valid (mutex_error_ptr
 * = pointer to the mutex with the error).
 * \return MQX_CORRUPT_QUEUE Queue of mutexes is not valid (mutex_error_ptr = pointer
 * to the invalid queue).
 *
 * \see _mutex_create_component
 * \see _mutex_init
 */
_mqx_uint _mutex_test
(
    pointer _PTR_ mutex_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    MUTEX_COMPONENT_STRUCT_PTR mutex_component_ptr;
    MUTEX_STRUCT_PTR           mutex_ptr;
    _mqx_uint                  result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_mutex_test, mutex_error_ptr);

    *mutex_error_ptr = NULL;

    mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
    if (mutex_component_ptr == NULL)
    {
        _KLOGX2(KLOG_mutex_test, MQX_OK);
        return (MQX_OK);
    } /* Endif */

    if (mutex_component_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutex_test, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    _int_disable();

    /* Make sure that the queue of mutexes is ok */
    result = _queue_test(&mutex_component_ptr->MUTEXES, mutex_error_ptr);
    if (result != MQX_OK)
    {
        _int_enable();
        _KLOGX3(KLOG_mutex_test, result, *mutex_error_ptr);
        return (result);
    } /* Endif */

    mutex_ptr = (MUTEX_STRUCT_PTR) ((pointer) mutex_component_ptr->MUTEXES.NEXT);
    while (mutex_ptr != (MUTEX_STRUCT_PTR) ((pointer) &mutex_component_ptr->MUTEXES))
    {
        if (mutex_ptr->VALID != MUTEX_VALID)
        {
            _int_enable();
            *mutex_error_ptr = mutex_ptr;
            _KLOGX3(KLOG_mutex_test, MQX_EINVAL, mutex_ptr);
            return (MQX_EINVAL);
        } /* Endif */
        result = _queue_test(&mutex_ptr->WAITING_TASKS, mutex_error_ptr);
        if (result != MQX_OK)
        {
            _int_enable();
            *mutex_error_ptr = mutex_ptr;
            _KLOGX3(KLOG_mutex_test, result, mutex_ptr);
            return (result);
        } /* Endif */
        mutex_ptr = (MUTEX_STRUCT_PTR) ((pointer) mutex_ptr->LINK.NEXT);
    } /* Endif */

    _int_enable();

    _KLOGX2(KLOG_mutex_test, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Tries to lock the mutex.
 *
 * If the mutex is not currently locked, the task locks it. If the mutex is currently
 * locked, the task continues to run; it does not block.
 *
 * \param[in] mutex_ptr Pointer to the mutex.
 *
 * \return MQX_EOK
 * \return MQX_EBUSY (Mutex is currently locked.)
 * \return MQX_EDEADLK (Task already has the mutex locked.)
 * \return MQX_EINVAL (Mutex_ptr is NULL or mutex has been destroyed.)
 *
 * \see _mutex_create_component
 * \see _mutex_init
 * \see _mutex_lock
 * \see _mutex_unlock
 * \see _mutatr_init
 * \see MUTEX_STRUCT
 */
_mqx_uint _mutex_try_lock
(
    register MUTEX_STRUCT_PTR mutex_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_mutex_try_lock, mutex_ptr);

#if MQX_CHECK_ERRORS
    if (mutex_ptr == NULL)
    {
        _KLOGX2(KLOG_mutex_try_lock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (mutex_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutex_try_lock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    _INT_DISABLE();
    if (_mem_test_and_set(&mutex_ptr->LOCK))
    {
        _INT_ENABLE();
        /* Lock value was set, can't have it */
        if (mutex_ptr->OWNER_TD == (pointer) kernel_data->ACTIVE_PTR)
        {
            _KLOGX2(KLOG_mutex_try_lock, MQX_EDEADLK);
            return (MQX_EDEADLK);
        }
        else
        {
            _KLOGX2(KLOG_mutex_try_lock, MQX_EBUSY);
            return (MQX_EBUSY);
        } /* Endif */
    }
    else
    {
        /* Lock value was not set, now it is, I own it */
        mutex_ptr->OWNER_TD = kernel_data->ACTIVE_PTR;
        _INT_ENABLE();

        if (mutex_ptr->PROTOCOLS & MUTEX_PRIO_PROTECT)
        {
            /* Switch my priority to the higher one if necessary */
            td_ptr = kernel_data->ACTIVE_PTR;
            _INT_DISABLE();
            if (mutex_ptr->PRIORITY_CEILING < td_ptr->HOME_QUEUE->PRIORITY)
            {
                if (mutex_ptr->PRIORITY_CEILING < td_ptr->MY_QUEUE->PRIORITY)
                {
                    _sched_boost_priority_internal(td_ptr, mutex_ptr->PRIORITY_CEILING);
                } /* Endif */
                mutex_ptr->BOOSTED++;
            } /* Endif */
            _INT_ENABLE();
        } /* Endif */

        _KLOGX2(KLOG_mutex_try_lock, MQX_EOK);
        return (MQX_EOK);
    } /* Endif */

} /* Endbody */

/*!
 * \brief Unlocks the mutex.
 *
 * If tasks are waiting for the mutex, MQX removes the first one from the mutex
 * queue and puts the task in the task's ready queue.
 *
 * \param[in] mutex_ptr Pointer to the mutex.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL
 *
 * \warning Might put a task in the task's ready queue.
 *
 * \see _mutex_create_component
 * \see _mutex_init
 * \see _mutex_lock
 * \see _mutex_try_lock
 * \see _mutatr_init
 * \see MUTEX_STRUCT
 */
_mqx_uint _mutex_unlock
(
    register MUTEX_STRUCT_PTR mutex_ptr
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR          td_ptr;
    _mqx_uint                       boosted;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_mutex_unlock, mutex_ptr);

    td_ptr = kernel_data->ACTIVE_PTR;

#if MQX_CHECK_ERRORS
    if (mutex_ptr == NULL)
    {
        _KLOGX2(KLOG_mutex_unlock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (mutex_ptr->VALID != MUTEX_VALID)
    {
        _KLOGX2(KLOG_mutex_unlock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (mutex_ptr->LOCK != MQX_TEST_AND_SET_VALUE)
    {
        _KLOGX2(KLOG_mutex_unlock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
    if (mutex_ptr->OWNER_TD != (pointer) td_ptr)
    {
        _KLOGX2(KLOG_mutex_unlock, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    _INT_DISABLE();
    boosted = mutex_ptr->BOOSTED;
    mutex_ptr->BOOSTED = 0;

    if (mutex_ptr->PROTOCOLS & (MUTEX_SPIN_ONLY | MUTEX_LIMITED_SPIN))
    {
        mutex_ptr->OWNER_TD = NULL;
        mutex_ptr->LOCK = 0;
    }
    else
    {

        if (_QUEUE_GET_SIZE(&mutex_ptr->WAITING_TASKS))
        {

            /* Schedule a waiting task to run */
            _QUEUE_DEQUEUE(&mutex_ptr->WAITING_TASKS, td_ptr);
            mutex_ptr->OWNER_TD = td_ptr;
            _TASK_READY(td_ptr, kernel_data);
            if (mutex_ptr->PROTOCOLS & MUTEX_PRIO_PROTECT)
            {
                /* Switch priority to the higher one if necessary */
                if (mutex_ptr->PRIORITY_CEILING < td_ptr->HOME_QUEUE->PRIORITY)
                {
                    if (mutex_ptr->PRIORITY_CEILING < td_ptr->MY_QUEUE->PRIORITY)
                    {
                        _sched_boost_priority_internal(td_ptr, mutex_ptr->PRIORITY_CEILING);
                    } /* Endif */
                    mutex_ptr->BOOSTED++;
                } /* Endif */
            } /* Endif */

            _CHECK_RUN_SCHEDULER();/* Let higher priority task run */

        }
        else
        {
            mutex_ptr->LOCK = 0;
            mutex_ptr->OWNER_TD = NULL;
        } /* Endif */

    } /* Endif */

    if (boosted)
    {
        _sched_unboost_priority_internal(kernel_data->ACTIVE_PTR, boosted);
    } /* Endif */
    _INT_ENABLE();

    _KLOGX2(KLOG_mutex_unlock, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Sets the priority of the mutex.
 *
 * This function operate on an initialized mutex; whereas, _mutatr_get_priority_ceiling()
 * and _mutatr_set_priority_ceiling() operate on an initialized mutex attributes
 * structure.
 *
 * \param[in]  mutex_ptr       Pointer to the mutex.
 * \param[in]  ceiling         New priority ceiling.
 * \param[out] old_ceiling_ptr Pointer to the previous priority ceiling.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (Mutex_ptr does not point to a valid mutex structure or
 * priority_ptr is NULL)
 *
 * \see _mutex_get_priority_ceiling
 * \see _mutex_init
 * \see MUTEX_STRUCT
 */
_mqx_uint _mutex_set_priority_ceiling
(
    MUTEX_STRUCT_PTR mutex_ptr,
    _mqx_uint        ceiling,
    _mqx_uint_ptr    old_ceiling_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE4(KLOG_mutex_set_priority_ceiling, mutex_ptr, ceiling, old_ceiling_ptr);

#if MQX_CHECK_ERRORS
    if (ceiling > kernel_data->LOWEST_TASK_PRIORITY)
    {
        _KLOGX2(KLOG_mutex_set_priority_ceiling, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    _int_disable();

#if MQX_CHECK_ERRORS
    if ((mutex_ptr == NULL) || (old_ceiling_ptr == NULL))
    {
        _int_enable();
        _KLOGX2(KLOG_mutex_set_priority_ceiling, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (mutex_ptr->VALID != MUTEX_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_mutex_set_priority_ceiling, MQX_EINVAL);
        return (MQX_EINVAL);
    } /* Endif */
#endif

    *old_ceiling_ptr = mutex_ptr->PRIORITY_CEILING;
    mutex_ptr->PRIORITY_CEILING = ceiling;
    _int_enable();

    _KLOGX2(KLOG_mutex_set_priority_ceiling, MQX_EOK);
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Gets the priority of the mutex.
 *
 * This function operate on an initialized mutex; whereas, _mutatr_get_priority_ceiling()
 * and _mutatr_set_priority_ceiling() operate on an initialized mutex attributes
 * structure.
 *
 * \param[in]  mutex_ptr   Pointer to the mutex.
 * \param[out] ceiling_ptr Pointer to the previous priority ceiling.
 *
 * \return MQX_EOK
 * \return MQX_EINVAL (Mutex_ptr does not point to a valid mutex structure or
 * priority_ptr is NULL)
 *
 * \see _mutex_set_priority_ceiling
 * \see _mutex_init
 * \see MUTEX_STRUCT
 */
_mqx_uint _mutex_get_priority_ceiling
(
    MUTEX_STRUCT_PTR mutex_ptr,
    _mqx_uint_ptr    ceiling_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if ((mutex_ptr == NULL) || (ceiling_ptr == NULL))
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (mutex_ptr->VALID != MUTEX_VALID)
    {
        return (MQX_EINVAL);
    } /* Endif */
#endif

    *ceiling_ptr = mutex_ptr->PRIORITY_CEILING;
    return (MQX_EOK);

} /* Endbody */

/*!
 * \brief Gets the number of tasks waiting for the specified mutex.
 *
 * \param[in] mutex_ptr Pointer to the mutex.
 *
 * \return Number of tasks that are waiting for the mutex.
 * \return MAX_MQX_UINT (Failure.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code to
 * MQX_EINVAL.
 *
 * \see _mutex_lock
 * \see _task_set_error
 * \see MUTEX_STRUCT
 */
_mqx_uint _mutex_get_wait_count
(
    register MUTEX_STRUCT_PTR mutex_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if (mutex_ptr == NULL)
    {
        _task_set_error(MQX_EINVAL);
        return (MAX_MQX_UINT);
    } /* Endif */
#endif
#if MQX_CHECK_VALIDITY
    if (mutex_ptr->VALID != MUTEX_VALID)
    {
        _task_set_error(MQX_EINVAL);
        return (MAX_MQX_UINT);
    } /* Endif */
#endif

    return (_QUEUE_GET_SIZE(&mutex_ptr->WAITING_TASKS));

} /* Endbody */

#endif /* MQX_USE_MUTEXES */

/* EOF */
