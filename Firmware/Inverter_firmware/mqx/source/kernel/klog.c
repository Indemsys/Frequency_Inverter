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
 * $FileName: klog.c$
 * $Version : 3.8.0.1$
 * $Date    : Feb-22-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Kernel LOG component.
 *
 *END************************************************************************/

#include "mqx_inc.h"

#if MQX_KERNEL_LOGGING
#include "lwlog.h"
#include "lwlog_prv.h"
#include "klog.h"
#include "fio.h"

extern char _PTR_ _klog_get_function_name_internal(uint_32);
extern char _PTR_ _klog_get_function_name2_internal(uint_32);

/*! A macro to display the name for a KLOG function entry */
#define KLOG_NAME(x) case KLOG ## x : result = #x ; break

/*!
 * \brief Controls logging in kernel log.
 *
 * The application must first create kernel log with _klog_create().
 * \n The function _klog_control() sets or clears bits in the kernel log control
 * variable, which MQX uses to control logging. To select which functions to log,
 * set combinations of bits in the KLOG_FUNCTIONS_ENABLED flag for the bit_mask
 * parameter.
 * \n MQX logs to kernel log only if KLOG_ENABLED is set in bit_mask.
 *
 * <table>
 *  <tr>
 *     <td><b> If this bit is set: </b></td>
 *     <td><b> MQX: </b></td>
 *  </tr>
 *  <tr>
 *     <td> KLOG_ENABLED (log MQX services) </td>
 *     <td> Logs to kernel log. </td>
 *  </tr>
 * </table>
 *
 * <table>
 *  <tr>
 *     <td><b> Use combinations of these bits </b></td>
 *     <td><b> Additional information </b></td>
 *  </tr>
 *  <tr>
 *     <td><b> KLOG_FUNCTION_ENABLED </b></td>
 *     <td> Log calls to specified MQX component APIs: KLOG_TASKING_FUNCTIONS
 *     KLOG_ERROR_FUNCTIONS KLOG_MESSAGE_FUNCTIONS
 *     KLOG_INTERRUPT_FUNCTIONS KLOG_MEMORY_FUNCTIONS KLOG_TIME_FUNCTIONS
 *     KLOG_EVENT_FUNCTIONS KLOG_NAME_FUNCTIONS KLOG_MUTEX_FUNCTIONS
 *     KLOG_SEMAPHORE_FUNCTIONS KLOG_WATCHDOG_FUNCTIONS
 *     KLOG_PARTITION_FUNCTIONS KLOG_IO_FUNCTIONS</td>
 *  </tr>
 *  <tr>
 *     <td><b> KLOG_TASK_QUALIFIED </b></td>
 *     <td> Log specific tasks only. For each task to log, call one of: _klog_disable_logging_task() _klog_enable_logging_task()</td>
 *  </tr>
 *  <tr>
 *     <td><b> KLOG_INTERRUPTS_ENABLED </b></td><td>Log interrupts</td>
 *  </tr>
 *  <tr>
 *     <td><b> KLOG_SYSTEM_CLOCK_INT_ENABLED </b></td><td>Log periodic timer interrupts</td>
 *  </tr>
 *  <tr>
 *     <td><b> KLOG_CONTEXT_ENABLED </b></td><td>Log context switches</td>
 *  </tr>
 * </table>
 *
 * \param[in] bit_mask Which bits of the kernel log control variable to modify.
 * \param[in] set_bits TRUE (Bits set in bit_mask are set in the control
 * variable.), FALSE (Bits set in bit_mask are cleared in the control variable.)
 *
 * \see _klog_create
 * \see _klog_create_at
 * \see _klog_disable_logging_task
 * \see _klog_enable_logging_task
 * \see _lwlog_create_component
 */
void _klog_control
(
    uint_32 bit_mask,
    boolean set_bits
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    _int_disable();
    if (set_bits)
    {
        kernel_data->LOG_CONTROL |= bit_mask;
    }
    else
    {
        kernel_data->LOG_CONTROL &= ~bit_mask;
    } /* Endif */
    _int_enable();

} /* Endbody */

#if !MQX_LITE_VERSION_NUMBER

/*!
 * \brief Creates the kernel log.
 *
 * If the log component is not created, MQX creates it. MQX uses lightweight log
 * number 0 as kernel log.
 * \n Each entry in kernel log contains MQX-specific data, a timestamp (in absolute
 * time), a sequence number, and information specified by _klog_control().
 * \n The MQX Task_aware Debugger plug-in and Performance Tools use kernel log
 * to analyze how the application operates and uses resources.
 *
 * \param[in] max_size Maximum size (in mqx_max_types) of the data to be stored.
 * \param[in] flags    One of the following:
 * \n - LOG_OVERWRITE (When the log is full, oldest entries are overwritten.)
 * \n - 0 (When the log is full, no more entries are written; default.)
 *
 * \warning The max_size parameter specifies the size of kernel log in sizeof(mqx_max_types).
 * To determine byt size of the momory used by the log, calculate as max_size * sizeof(mqx_max_types).
 *
 * \note To use kernel logging, MQX must be configured at compile time with
 * MQX_KERNEL_LOGGING set to 1. For information on configuring MQX, see MQX User's Guide.
 *
 * \return MQX_OK
 * \return LOG_EXISTS (Lightweight log with log number log_number exists.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_INVALID_SIZE (Max_size is 0.)
 * \return MQX_INVALID_COMPONENT_BASE (Invalid data for the lightweight log component.)
 *
 * \see _klog_create_at
 * \see _klog_control
 * \see _klog_disable_logging_task
 * \see _klog_enable_logging_task
 * \see _lwlog_create_component
 * \see _lwlog_create
 * \see _lwlog_create_at
 */
_mqx_uint _klog_create
(
    _mqx_uint max_size,
    _mqx_uint flags
)
{ /* Body */

    /*
      * Each element in the each log entry is of the type _mqx_max_type.
      * The lightweight log takes the number of entries to create so
      * adjust size accordingly.
      */
    max_size = max_size * sizeof(_mqx_max_type) / sizeof(LWLOG_ENTRY_STRUCT);

    return _lwlog_create(LOG_KERNEL_LOG_NUMBER, max_size, flags);

} /* Endbody */
#endif /* MQX_LITE_VERSION_NUMBER */

/*!
 * \brief Creates the kernel log at specified location.
 *
 * If the log component is not created, MQX creates it. MQX uses lightweight log
 * number 0 as kernel log.
 * \n Each entry in kernel log contains MQX-specific data, a timestamp (in absolute
 * time), a sequence number, and information specified by _klog_control().
 * \n The MQX Embedded Performance Tool uses kernel log to analyze how the application
 * operates and uses resources.
 *
 * \param[in] max_size Maximum size (in mqx_max_types) of the data to be stored.
 * \param[in] flags    One of the following:
 * \n - LOG_OVERWRITE (When the log is full, oldest entries are overwritten.)
 * \n - 0 (When the log is full, no more entries are written; default.)
 * \param[in] where    Where in memory is the log to start.
 *
 * \warning The max_size parameter specifies the size of kernel log in sizeof(mqx_max_types).
 * To determine byt size of the momory used by the log, calculate as max_size * sizeof(mqx_max_types).
 *
 * \note To use kernel logging, MQX must be configured at compile time with
 * MQX_KERNEL_LOGGING set to 1. For information on configuring MQX, see MQX User's Guide.
 *
 * \return MQX_OK
 * \return LOG_EXISTS (Lightweight log with log number log_number exists.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_INVALID_SIZE (Max_size is 0.)
 * \return MQX_INVALID_COMPONENT_BASE (Invalid data for the lightweight log component.)
 * \return MQX_INVALID_POINTER (Pointer "where" is NULL.)
 *
 * \see _klog_create
 * \see _klog_control
 * \see _klog_disable_logging_task
 * \see _klog_enable_logging_task
 * \see _lwlog_create_component
 * \see _lwlog_create
 * \see _lwlog_create_at
 */
_mqx_uint _klog_create_at
(
    _mqx_uint max_size,
    _mqx_uint flags,
    pointer where
)
{ /* Body */

    /*
      * The size of the kernel log must be a multiple of the size of a
      * lightweight log entry
      */
    max_size = max_size * sizeof(_mqx_max_type) / sizeof(LWLOG_ENTRY_STRUCT);

    return _lwlog_create_at(LOG_KERNEL_LOG_NUMBER, max_size, flags, where);

} /* Endbody */

/*!
 * \brief Disables kernel logging for the task.
 *
 * The application disables logging by calling _klog_disable_logging_task() for
 * each task which it wants to stop logging. If the application did not first
 * enable logging for the task, MQX ignores the request.
 *
 * \param[in] tid Task ID of the task for which kernel logging is to be disabled.
 *
 * \see _klog_enable_logging_task
 * \see _klog_control
 */
void _klog_disable_logging_task
(
    _task_id tid
)
{ /* Body */
    TD_STRUCT_PTR td_ptr;

    _int_disable();
    td_ptr = (TD_STRUCT_PTR)_task_get_td(tid);
    if (td_ptr != NULL)
    {
        td_ptr->FLAGS &= ~TASK_LOGGING_ENABLED;
    } /* Endif */
    _int_enable();

} /* Endbody */

/*!
 * \brief Enables kernel logging for the task.
 *
 * If the application calls _klog_control() with KLOG_TASK_QUALIFIED, it must also call
 * _klog_enable_logging_task() for each task for which it wants to log information.
 *
 * \param[in] tid Task ID of the task for which kernel logging is to be enabled.
 *
 * \see _klog_disable_logging_task
 * \see _klog_control
 */
void _klog_enable_logging_task
(
    _task_id tid
)
{ /* Body */
    TD_STRUCT_PTR td_ptr;

    _int_disable();
    td_ptr = (TD_STRUCT_PTR)_task_get_td(tid);
    if (td_ptr != NULL)
    {
        td_ptr->FLAGS |= TASK_LOGGING_ENABLED;
    } /* Endif */
    _int_enable();

} /* Endbody */

/*!
 * \brief Displays the oldest entry in kernel log and delete this entry.
 *
 * The function prints the oldest entry in kernel log to the default output stream
 * of the current task and deletes this entry.
 *
 * \return TRUE (Entry is found and displayed.)
 * \return FALSE (Entry is not found.)
 *
 * \warning Depending on the low-level I/O used, the calling task might be blocked
 * and MQX might perform a dispatch operation.
 *
 * \see _klog_control
 * \see _klog_create
 * \see _klog_create_at
 */
boolean _klog_display(void)
{ /* Body */
    LWLOG_ENTRY_STRUCT     log_entry;
    LWLOG_ENTRY_STRUCT_PTR log_ptr;
    _mqx_uint              result;
    _mqx_int               i;

    log_ptr = &log_entry;
    result = _lwlog_read(LOG_KERNEL_LOG_NUMBER, LOG_READ_OLDEST_AND_DELETE, log_ptr);
    if (result != MQX_OK)
    {
        return FALSE;
    } /* Endif */

#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    /* Normalize the time in the record */
    log_ptr->MILLISECONDS += log_ptr->MICROSECONDS / 1000;
    log_ptr->MICROSECONDS = log_ptr->MICROSECONDS % 1000;
    log_ptr->SECONDS += log_ptr->MILLISECONDS / 1000;
    log_ptr->MILLISECONDS = log_ptr->MILLISECONDS % 1000;

    printf("%ld. %ld:%03ld%03ld -> ",
                    (uint_32)log_ptr->SEQUENCE_NUMBER,
                    log_ptr->SECONDS,
                    log_ptr->MILLISECONDS,
                    log_ptr->MICROSECONDS);
#else

    printf("%ld. ", (uint_32)log_ptr->SEQUENCE_NUMBER);

    PSP_PRINT_TICKS(&log_ptr->TIMESTAMP);

    printf(" -> ");
#endif

    switch (log_ptr->DATA[0])
    {

        case KLOG_FUNCTION_ENTRY:
        case KLOG_FUNCTION_EXIT:
        printf("%s %22.22s ",
                        (log_ptr->DATA[0] == KLOG_FUNCTION_ENTRY) ? "FUN " : "XFUN",
                        _klog_get_function_name_internal((uint_32)log_ptr->DATA[1]));
        /* for (i = 2; i < LWLOG_MAXIMUM_DATA_ENETRIES; ++i) { */
        for (i = 2; i < LWLOG_MAXIMUM_DATA_ENTRIES; ++i)
        {
            printf("0x%lX ", (uint_32)log_ptr->DATA[i]);
        } /* Endfor */
        printf("\n");
        break;

        case KLOG_INTERRUPT:
        printf("INT   0x%lX\n", (uint_32)log_ptr->DATA[1]);
        break;

        case KLOG_INTERRUPT_END:
        printf("INT   0x%lX END\n",(uint_32)log_ptr->DATA[1]);
        break;

        case KLOG_CONTEXT_SWITCH:
        printf("NEW TASK TD 0x%lX ID 0x%lX STATE 0x%lX STACK 0x%lX\n",
                        (uint_32)log_ptr->DATA[1], (uint_32)log_ptr->DATA[2],
                        (uint_32)log_ptr->DATA[3], (uint_32)log_ptr->DATA[4]);
        break;

        default:
        printf("USER ENTRY: 0x%lX:", (uint_32)log_ptr->DATA[0]);
        /* for (i = 1; i < LWLOG_MAXIMUM_DATA_ENETRIES; ++i) { */
        for (i = 1; i < LWLOG_MAXIMUM_DATA_ENTRIES; ++i)
        {
            printf("0x%lX ", (uint_32)log_ptr->DATA[i]);
        } /* Endfor */
        printf("\n");
        break;
    } /* Endswitch */

    return TRUE;

} /* Endbody */

/*!
 * \brief Gets the stack size for the task and the total amount of it that the
 * task has used.
 *
 * The amount used is a highwater mark - the highest amount of stack that the task
 * has used so far. It might not include the amount that the task is currently
 * using. If the amount is 0, the stack is not large enough.
 *
 * \note To use kernel logging, MQX must be configured at compile time with
 * MQX_MONITOR_STACK set to 1. For information on configuring MQX, see MQX User's Guide.
 *
 * \param[in]  task_id        Task ID of task to display.
 * \param[out] stack_size_ptr Where to write the size (in single-addressable units)
 * of the stack.
 * \param[out] stack_used_ptr Where to write the amount (in single-addressable
 * units) of stack used.
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID (Task_id is not valid.)
 * \return MQX_INVALID_CONFIGURATION (Compile-time configuration option
 * MQX_MONITOR_STACK is not set.)
 *
 * \see _klog_get_interrupt_stack_usage
 * \see _klog_show_stack_usage
 */
_mqx_uint _klog_get_task_stack_usage
(
    _task_id task_id,
    _mem_size_ptr stack_size_ptr,
    _mem_size_ptr stack_used_ptr
)
{ /* Body */
#if MQX_MONITOR_STACK
    TD_STRUCT_PTR td_ptr;

    td_ptr = (TD_STRUCT_PTR)_task_get_td(task_id);
#if MQX_CHECK_ERRORS
    if (td_ptr == NULL)
    {
        return(MQX_INVALID_TASK_ID);
    } /* Endif */
#endif

    return _klog_get_task_stack_usage_internal(td_ptr, stack_size_ptr, stack_used_ptr);

#else
    return MQX_INVALID_CONFIGURATION;
#endif

} /* Endbody */

/*!
 * \private
 *
 * \brief Returns the stack size and the total amount of the stack
 * used.
 *
 *  Used by MQX functions that already have TD pointer.
 *
 * \param[in]  td_ptr         Td of task to display.
 * \param[out] stack_size_ptr Address where size of stack to be written to.
 * \param[out] stack_used_ptr Amount of stack used.
 *
 * \return MQX_OK
 * \return MQX_INVALID_TASK_ID (Task_id is not valid.)
 * \return MQX_INVALID_CONFIGURATION (Compile-time configuration option
 * MQX_MONITOR_STACK is not set.)
 *
 * \see _klog_get_task_stack_usage
 * \see TD_STRUCT
 */
_mqx_uint _klog_get_task_stack_usage_internal
(
    TD_STRUCT_PTR td_ptr,
    _mem_size_ptr stack_size_ptr,
    _mem_size_ptr stack_used_ptr
)
{ /* Body */
#if MQX_MONITOR_STACK && MQX_TD_HAS_STACK_LIMIT
    _mqx_uint_ptr stack_ptr;
    _mqx_uint_ptr stack_base;

#if MQX_CHECK_ERRORS
    if (td_ptr == NULL)
    {
        return(MQX_INVALID_TASK_ID);
    } /* Endif */
#endif

#if PSP_STACK_GROWS_TO_LOWER_MEM
    stack_base = (_mqx_uint_ptr)td_ptr->STACK_BASE;
    stack_ptr = (_mqx_uint_ptr)td_ptr->STACK_LIMIT;
    while (stack_ptr < stack_base)
    {
        if (*stack_ptr != MQX_STACK_MONITOR_VALUE)
        {
            break;
        } /* Endif */
        ++stack_ptr;
    } /* Endwhile */
    *stack_used_ptr = (_mqx_uint)((uchar_ptr)td_ptr->STACK_BASE -
                    (uchar_ptr)stack_ptr);
    *stack_size_ptr = (_mqx_uint)((uchar_ptr)td_ptr->STACK_BASE -
                    (uchar_ptr)td_ptr->STACK_LIMIT);
#else
    stack_base = (pointer)td_ptr->STACK_BASE;
    stack_ptr = (pointer)td_ptr->STACK_LIMIT;
    stack_ptr--;
    while (stack_ptr > stack_base)
    {
        if (*stack_ptr != MQX_STACK_MONITOR_VALUE)
        {
            break;
        } /* Endif */
        --stack_ptr;
    } /* Endwhile */
    *stack_used_ptr = (_mem_size)((uchar_ptr)stack_ptr -
                    (uchar_ptr)td_ptr->STACK_BASE);
    *stack_size_ptr = (_mem_size)((uchar_ptr)td_ptr->STACK_LIMIT -
                    (uchar_ptr)td_ptr->STACK_BASE);
#endif
    return(MQX_OK);
#else
    return MQX_INVALID_CONFIGURATION;
#endif

} /* Endbody */

/*!
 * \brief Gets the size of the interrupt stack and the total amount of it used.
 *
 * The amount used is a highwater mark - the highest amount of interrupt stack that
 * the application has used so far. It shows only how much of the stack has been
 * written to at this point. If the amount is 0, the interrupt stack is not large
 * enough.
 * \note To use kernel logging, MQX must be configured at compile time with
 * MQX_MONITOR_STACK set to 1. For information on configuring MQX, see MQX User's
 * Guide.
 *
 * \param[out] stack_size_ptr Where to write the size (in single-addressable units)
 * of the stack.
 * \param[out] stack_used_ptr Where to write the amount (in single-addressable
 * units) of stack used.
 *
 * \return MQX_OK
 * \return MQX_INVALID_CONFIGURATION (Failure: compile-time configuration option
 * MQX_MONITOR_STACK is not set.)
 *
 * \see _klog_get_task_stack_usage
 * \see _klog_show_stack_usage
 */
_mqx_uint _klog_get_interrupt_stack_usage
(
    _mem_size_ptr stack_size_ptr,
    _mem_size_ptr stack_used_ptr
)
{ /* Body */
#if MQX_MONITOR_STACK
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _mqx_uint_ptr          stack_ptr;
    _mqx_uint_ptr          stack_base;

    _GET_KERNEL_DATA(kernel_data);

    *stack_size_ptr = kernel_data->INIT.INTERRUPT_STACK_SIZE;

#if PSP_STACK_GROWS_TO_LOWER_MEM
    stack_base = (_mqx_uint_ptr)kernel_data->INTERRUPT_STACK_PTR;
    stack_ptr = (_mqx_uint_ptr)((uchar_ptr)stack_base -
                    kernel_data->INIT.INTERRUPT_STACK_SIZE
#if PSP_MEMORY_ALIGNMENT
                    + PSP_MEMORY_ALIGNMENT + 1
#endif
    );
    while (stack_ptr < stack_base)
    {
        if (*stack_ptr != MQX_STACK_MONITOR_VALUE)
        {
            break;
        } /* Endif */
        ++stack_ptr;
    } /* Endwhile */
    *stack_used_ptr = (_mem_size)((uchar_ptr)stack_base - (uchar_ptr)stack_ptr);
#else
    stack_base = (_mqx_uint_ptr)kernel_data->INTERRUPT_STACK_PTR;
    stack_ptr = (_mqx_uint_ptr)((uchar_ptr)stack_base +
                    kernel_data->INIT.INTERRUPT_STACK_SIZE - 1
#if PSP_MEMORY_ALIGNMENT
                    - PSP_MEMORY_ALIGNMENT
#endif
    );
    stack_ptr--;
    while (stack_ptr > stack_base)
    {
        if (*stack_ptr != MQX_STACK_MONITOR_VALUE)
        {
            break;
        } /* Endif */
        --stack_ptr;
    } /* Endwhile */
    *stack_used_ptr = (_mem_size)((uchar_ptr)stack_ptr - (uchar_ptr)stack_base);
#endif

    return(MQX_OK);
#else
    return MQX_INVALID_CONFIGURATION;
#endif

} /* Endbody */

/*!
 * \brief This function prints out the stack usage for all tasks currently running
 * in the MQX system. It assumes that MQX has been configured with MQX_MONITOR_STACK.
 *
 * The function displays the information on the standard output stream for the calling
 * task.
 *
 * \note To use kernel logging, MQX must be configured at compile time with
 * MQX_MONITOR_STACK set to 1. For information on configuring MQX, see MQX User's Guide.
 *
 * \warning Depending on the low-level I/O used, the calling task might be blocked
 * and MQX might perform a dispatch operation.
 *
 * \see _klog_get_interrupt_stack_usage
 * \see _klog_get_task_stack_usage
 */
void _klog_show_stack_usage(void)
{ /* Body */
#if MQX_MONITOR_STACK
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;
    _mem_size              stack_size;
    _mem_size              stack_used;
    _mqx_uint              size;

    _GET_KERNEL_DATA(kernel_data);

    /* Now display the gathered information */
    _klog_get_interrupt_stack_usage(&stack_size, &stack_used);
    printf("Stack usage: \n\n");
    printf("Interrupt stack:    size %08ld    used %08ld\n\n",
                    (uint_32)stack_size, (uint_32)stack_used);

    printf("SIZE                  USED                 TASK ID      NAME\n");

    _lwsem_wait((LWSEM_STRUCT_PTR)(&kernel_data->TASK_CREATE_LWSEM));
    td_ptr = (TD_STRUCT_PTR)((uchar_ptr)kernel_data->TD_LIST.NEXT -
                    FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
    size = _QUEUE_GET_SIZE(&kernel_data->TD_LIST);

    while (size && td_ptr)
    {
        _klog_get_task_stack_usage_internal(td_ptr, &stack_size, &stack_used);
        printf("%08ld(%08lX)    %08ld(%08lX)   %08lX     %s\n",
                        (uint_32)stack_size, (uint_32)stack_size, (uint_32)stack_used,
                        (uint_32)stack_used, td_ptr->TASK_ID,
                        (td_ptr->TASK_TEMPLATE_PTR->TASK_NAME != NULL) ?
                        td_ptr->TASK_TEMPLATE_PTR->TASK_NAME : "");
        size--;
        td_ptr = (TD_STRUCT_PTR)((uchar_ptr)(td_ptr->TD_LIST_INFO.NEXT) -
                        FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
    } /* Endwhile */

    _lwsem_post((LWSEM_STRUCT_PTR)(&kernel_data->TASK_CREATE_LWSEM));

#else
    printf("Stack usage: ERROR MQX_MONITOR_STACK not set in mqx_cnfg.h\n\n");
#endif

} /* Endbody */

/*!
 * \brief Logs information into the kernel log.
 *
 * \param[in] type Defines type of log, see klog.h for possible types.
 * \param[in] p1   Parameter 1.
 * \param[in] p2   Parameter 2.
 * \param[in] p3   Parameter 3.
 * \param[in] p4   Parameter 4.
 * \param[in] p5   Parameter 5.
 */
void _klog_log
(
    _mqx_uint type,
    _mqx_max_type p1,
    _mqx_max_type p2,
    _mqx_max_type p3,
    _mqx_max_type p4,
    _mqx_max_type p5
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _mqx_max_type          calling_pc;

    _GET_KERNEL_DATA(kernel_data);
    if ( !(kernel_data->LOG_CONTROL & KLOG_ENABLED) )
    {
        return;
    } /* Endif */
    if ( kernel_data->LOG_CONTROL & KLOG_TASK_QUALIFIED )
    {
        if (! (kernel_data->ACTIVE_PTR->FLAGS & TASK_LOGGING_ENABLED) )
        {
            return;
        } /* Endif */
    } /* Endif */

    if ((type == KLOG_FUNCTION_ENTRY) || (type == KLOG_FUNCTION_EXIT))
    {
        if ( !(kernel_data->LOG_CONTROL & KLOG_FUNCTIONS_ENABLED) )
        {
            return;
        } /* Endif */
        /* Functions enabled, now lets check function group enabled */
        if (((p1 & KLOG_FUNCTION_MASK) & kernel_data->LOG_CONTROL) == 0)
        {
            return;
        } /* Endif */
    } /* Endif */

    if (type == KLOG_FUNCTION_ENTRY)
    {
        calling_pc = (_mqx_max_type)_PSP_GET_CALLING_PC();
    }
    else
    {
        calling_pc = 0;
    } /* Endif */

    _INT_DISABLE();
    _lwlog_write(LOG_KERNEL_LOG_NUMBER, (_mqx_max_type)type, p1, p2, p3, p4, p5,
                    calling_pc);
    _INT_ENABLE();

} /* Endbody */

/*!
 * \brief Logs a function address into the kernel log.
 *
 * This function is used internally by MQX to log the API calls.
 *
 * \param[in] fn Pointer to the function which si to be logged.
 *
 * \see _klog_log
 */
void _klog_log_function(pointer fn)
{
    _klog_log(KLOG_FUNCTION_ADDRESS, (uint_32)fn, 0,0,0,0);
}

/*!
 * \private
 *
 * \brief Is called from _task_block for logging purposes.
 */
void _klog_block_internal(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

    _GET_KERNEL_DATA(kernel_data);
    if ( kernel_data->LOG_CONTROL & KLOG_TASK_QUALIFIED )
    {
        if (! (kernel_data->ACTIVE_PTR->FLAGS & TASK_LOGGING_ENABLED) )
        {
            return;
        } /* Endif */
    } /* Endif */

    if (kernel_data->LOG_CONTROL & KLOG_FUNCTIONS_ENABLED)
    {
        if ((KLOG_task_block & KLOG_FUNCTION_MASK) & kernel_data->LOG_CONTROL)
        {
            if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG])
            {
                td_ptr = kernel_data->ACTIVE_PTR;
                _lwlog_write_internal(LOG_KERNEL_LOG_NUMBER,
                                (_mqx_max_type)KLOG_FUNCTION_ENTRY,
                                (_mqx_max_type)KLOG_task_block, (_mqx_max_type)td_ptr,
                                (_mqx_max_type)td_ptr->TASK_ID, (_mqx_max_type)td_ptr->STATE,
                                (_mqx_max_type)td_ptr->STACK_PTR, (_mqx_max_type)0);
            } /* Endif */
        } /* Endif */
    } /* Endif */

} /* Endbody */

/*!
 * \private
 *
 * \brief Is called from _sched_execute_scheduler_internal for
 * logging purposes.
 */
void _klog_execute_scheduler_internal(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

    _GET_KERNEL_DATA(kernel_data);
    if ( kernel_data->LOG_CONTROL & KLOG_TASK_QUALIFIED )
    {
        if (! (kernel_data->ACTIVE_PTR->FLAGS & TASK_LOGGING_ENABLED) )
        {
            return;
        } /* Endif */
    } /* Endif */

    if (kernel_data->LOG_CONTROL & KLOG_FUNCTIONS_ENABLED)
    {
        if ((KLOG_task_execute_scheduler & KLOG_FUNCTION_MASK) & kernel_data->LOG_CONTROL)
        {
            if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG])
            {
                td_ptr = kernel_data->ACTIVE_PTR;
                _lwlog_write_internal(LOG_KERNEL_LOG_NUMBER,
                                (_mqx_max_type)KLOG_FUNCTION_ENTRY,
                                (_mqx_max_type)KLOG_task_execute_scheduler, (_mqx_max_type)td_ptr,
                                (_mqx_max_type)td_ptr->TASK_ID, (_mqx_max_type)td_ptr->STATE,
                                (_mqx_max_type)td_ptr->STACK_PTR, (_mqx_max_type)0);
            } /* Endif */
        } /* Endif */
    } /* Endif */

} /* Endbody */

/*!
 * \private
 *
 * \brief Is called from _sched_yield for logging purposes.
 */
void _klog_yield_internal(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

    _GET_KERNEL_DATA(kernel_data);
    if ( kernel_data->LOG_CONTROL & KLOG_TASK_QUALIFIED )
    {
        if (! (kernel_data->ACTIVE_PTR->FLAGS & TASK_LOGGING_ENABLED) )
        {
            return;
        } /* Endif */
    } /* Endif */

    if (kernel_data->LOG_CONTROL & KLOG_FUNCTIONS_ENABLED)
    {
        if ((KLOG_sched_yield & KLOG_FUNCTION_MASK) & kernel_data->LOG_CONTROL)
        {
            if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG])
            {
                td_ptr = kernel_data->ACTIVE_PTR;
                _lwlog_write_internal(LOG_KERNEL_LOG_NUMBER,
                                (_mqx_max_type)KLOG_FUNCTION_ENTRY,
                                (_mqx_max_type)KLOG_sched_yield, (_mqx_max_type)td_ptr,
                                (_mqx_max_type)td_ptr->TASK_ID, (_mqx_max_type)td_ptr->STATE,
                                (_mqx_max_type)td_ptr->STACK_PTR, (_mqx_max_type)0);
            } /* Endif */
        } /* Endif */
    } /* Endif */

} /* Endbody */

/*!
 * \private
 *
 * \brief Is called from _sched_dispatch for logging purposes.
 */
void _klog_context_switch_internal(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

    _GET_KERNEL_DATA(kernel_data);
#if 0
    if ( kernel_data->LOG_CONTROL & KLOG_TASK_QUALIFIED )
    {
        if (! (kernel_data->ACTIVE_PTR->FLAGS & TASK_LOGGING_ENABLED) )
        {
            return;
        } /* Endif */
    } /* Endif */
#endif
    if (kernel_data->LOG_CONTROL & KLOG_CONTEXT_ENABLED)
    {
        if (kernel_data->ACTIVE_PTR != kernel_data->LOG_OLD_TD)
        {
            if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG])
            {
                kernel_data->LOG_OLD_TD = kernel_data->ACTIVE_PTR;
                if ( kernel_data->LOG_CONTROL & KLOG_TASK_QUALIFIED )
                {
                    if (! (kernel_data->ACTIVE_PTR->FLAGS & TASK_LOGGING_ENABLED) )
                    {
                        return;
                    } /* Endif */
                } /* Endif */
                td_ptr = kernel_data->ACTIVE_PTR;
                _lwlog_write_internal(LOG_KERNEL_LOG_NUMBER,
                                (_mqx_max_type)KLOG_CONTEXT_SWITCH,
                                (_mqx_max_type)td_ptr, (_mqx_max_type)td_ptr->TASK_ID,
                                (_mqx_max_type)td_ptr->STATE, (_mqx_max_type)td_ptr->STACK_PTR,
                                (_mqx_max_type)0, (_mqx_max_type)0);
            } /* Endif */
        } /* Endif */
    } /* Endif */

} /* Endbody */

/*!
 * \private
 *
 * \brief Is called from _int_kernel_isr for logging purposes.
 *
 * \param[in] vector_number
 */
void _klog_isr_start_internal
(
    _mqx_uint vector_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG])
    {
        if (kernel_data->LOG_CONTROL & KLOG_INTERRUPTS_ENABLED)
        {
            if (!(kernel_data->LOG_CONTROL & KLOG_SYSTEM_CLOCK_INT_ENABLED))
            {
                /* Check to see if the vector number is to be ignored */
                if (vector_number == kernel_data->SYSTEM_CLOCK_INT_NUMBER)
                {
                    return;
                } /* Endif */
            } /* Endif */
            _lwlog_write_internal(LOG_KERNEL_LOG_NUMBER,
                            (_mqx_max_type)KLOG_INTERRUPT, (_mqx_max_type)vector_number,
                            (_mqx_max_type)_int_get_isr(vector_number),
                            (_mqx_max_type)_int_get_isr_data(vector_number), (_mqx_max_type)0,
                            (_mqx_max_type)0, (_mqx_max_type)0);
        } /* Endif */
    } /* Endif */

} /* Endbody */

/*!
 * \private
 *
 * \brief Is called from _int_kernel_isr for logging purposes.
 *
 * \param[in] vector_number
 */
void _klog_isr_end_internal
(
    _mqx_uint vector_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG])
    {
        if (kernel_data->LOG_CONTROL & KLOG_INTERRUPTS_ENABLED)
        {
            if (!(kernel_data->LOG_CONTROL & KLOG_SYSTEM_CLOCK_INT_ENABLED))
            {
                /* Check to see if the vector number is to be ignored */
                if (vector_number == kernel_data->SYSTEM_CLOCK_INT_NUMBER)
                {
                    return;
                } /* Endif */
            } /* Endif */
            _lwlog_write_internal(LOG_KERNEL_LOG_NUMBER,
                            (_mqx_max_type)KLOG_INTERRUPT_END, (_mqx_max_type)vector_number,
                            (_mqx_max_type)0, (_mqx_max_type)0, (_mqx_max_type)0,
                            (_mqx_max_type)0, (_mqx_max_type)0);
        } /* Endif */
    } /* Endif */

} /* Endbody */

/*!
 * \private
 *
 * \brief Returns the address of a function name string.
 *
 * \param[in] index The function index.
 *
 * \return Pointer to the function name string.
 */
char _PTR_ _klog_get_function_name_internal
(
    uint_32 index
)
{ /* Body */
    char _PTR_ result;

    switch (index)
    {
        KLOG_NAME(_task_ready);
        KLOG_NAME(_task_block);
        KLOG_NAME(_task_execute_scheduler);
        KLOG_NAME(_task_create);
        KLOG_NAME(_task_create_block);
        KLOG_NAME(_task_destroy);
        KLOG_NAME(_time_dequeue);

        KLOG_NAME(_sched_setscheduler);
        KLOG_NAME(_sched_yield);
        KLOG_NAME(_sched_setparam);
        KLOG_NAME(_sched_setparam_td);
        KLOG_NAME(_sched_boost_priority);
        KLOG_NAME(_sched_unboost_priority);
        KLOG_NAME(_task_exit_function);
        KLOG_NAME(_taskq_create);
        KLOG_NAME(_taskq_destroy);
        KLOG_NAME(_taskq_suspend);
        KLOG_NAME(_taskq_resume);
        KLOG_NAME(_sched_set_policy);
        KLOG_NAME(_sched_set_rr_interval);
        KLOG_NAME(_sched_set_rr_interval_ticks);
        KLOG_NAME(_taskq_suspend_task);
        KLOG_NAME(_task_set_priority);
        KLOG_NAME(_time_dequeue_td);
        KLOG_NAME(_task_abort);
        KLOG_NAME(_task_set_environment);
        KLOG_NAME(_task_enable_fp);
        KLOG_NAME(_task_disable_fp);
        KLOG_NAME(_task_set_parameter);
        KLOG_NAME(_task_stop_preemption);
        KLOG_NAME(_task_start_preemption);
        KLOG_NAME(_task_set_exception_handler);
        KLOG_NAME(_task_set_exit_handler);
        KLOG_NAME(_taskq_test);
        KLOG_NAME(_io_set_handle);
        KLOG_NAME(_task_restart);

        KLOG_NAME(_mqx_exit);
        KLOG_NAME(_int_default_isr);
        KLOG_NAME(_task_set_error);
        KLOG_NAME(_log_test);
        KLOG_NAME(_mqx_set_exit_handler);
        KLOG_NAME(_mqx_fatal_error);

        KLOG_NAME(_msgq_close);
        KLOG_NAME(_msgpool_create_system);
        KLOG_NAME(_msgpool_create);
        KLOG_NAME(_msgpool_destroy);
        KLOG_NAME(_msg_free);
        KLOG_NAME(_msg_alloc_system);
        KLOG_NAME(_msg_alloc);
        KLOG_NAME(_msgq_open);
        KLOG_NAME(_msgq_open_system);
        KLOG_NAME(_msgq_receive);
        KLOG_NAME(_msgq_receive_for);
        KLOG_NAME(_msgq_receive_ticks);
        KLOG_NAME(_msgq_receive_until);
        KLOG_NAME(_msgq_poll);
        KLOG_NAME(_msgq_get_count);
        KLOG_NAME(_msgq_send);
        KLOG_NAME(_msgq_send_blocked);
        KLOG_NAME(_msgq_send_queue);
        KLOG_NAME(_msgq_send_urgent);
        KLOG_NAME(_msgq_send_broadcast);
        KLOG_NAME(_msgq_set_notification_function);
        KLOG_NAME(_msgq_peek);
        KLOG_NAME(_msgq_send_priority);
        KLOG_NAME(_msg_create_component);
        KLOG_NAME(_msgpool_test);
        KLOG_NAME(_msgq_test);
        KLOG_NAME(_lwmsgq_init);
        KLOG_NAME(_lwmsgq_send);
        KLOG_NAME(_lwmsgq_receive);

        KLOG_NAME(_int_install_kernel_isr);
        KLOG_NAME(_int_install_isr);
        KLOG_NAME(_int_install_default_isr);
        KLOG_NAME(_int_set_isr_data);
        KLOG_NAME(_int_set_exception_handler);

        default: result = _klog_get_function_name2_internal(index); break;
    } /* Endswitch */

    return result;

} /* Endbody */

/*!
 * \private
 *
 * \brief Returns the address of a function name string.
 *
 * \param[in] index The function index.
 *
 * \return Pointer to the function name string.
 *
 * \see _klog_get_function_name_internal
 */
char _PTR_ _klog_get_function_name2_internal
(
    uint_32 index
)
{ /* Body */
    char _PTR_ result;

    switch (index)
    {
        KLOG_NAME(_mem_free);
        KLOG_NAME(_mem_alloc);
        KLOG_NAME(_mem_alloc_from);
        KLOG_NAME(_mem_test);
        KLOG_NAME(_mem_test_pool);
        KLOG_NAME(_mem_alloc_zero);
        KLOG_NAME(_mem_alloc_system);
        KLOG_NAME(_mem_alloc_system_from);
        KLOG_NAME(_mem_alloc_system_zero);
        KLOG_NAME(_mem_alloc_system_zero_from);
        KLOG_NAME(_mem_extend);
        KLOG_NAME(_mem_extend_pool);
        KLOG_NAME(_mem_transfer);
        KLOG_NAME(_mem_free_part);
        KLOG_NAME(_mem_create_pool);

        KLOG_NAME(_lwmem_alloc);
        KLOG_NAME(_lwmem_alloc_from);
        KLOG_NAME(_lwmem_alloc_zero);
        KLOG_NAME(_lwmem_alloc_zero_from);
        KLOG_NAME(_lwmem_create_pool);
        KLOG_NAME(_lwmem_free);
        KLOG_NAME(_lwmem_alloc_system);
        KLOG_NAME(_lwmem_alloc_system_from);
        KLOG_NAME(_lwmem_transfer);
        KLOG_NAME(_lwmem_alloc_system_zero);
        KLOG_NAME(_lwmem_alloc_system_zero_from);

        KLOG_NAME(_time_set);
        KLOG_NAME(_time_set_ticks);
        KLOG_NAME(_time_delay);
        KLOG_NAME(_time_delay_for);
        KLOG_NAME(_time_delay_ticks);
        KLOG_NAME(_time_delay_until);
        KLOG_NAME(_timer_create_component);
        KLOG_NAME(_timer_cancel);
        KLOG_NAME(_timer_start_oneshot_after);
        KLOG_NAME(_timer_start_oneshot_at);
        KLOG_NAME(_timer_start_periodic_every);
        KLOG_NAME(_timer_start_periodic_at);
        KLOG_NAME(_timer_test);
        KLOG_NAME(_timer_start_oneshot_after_ticks);
        KLOG_NAME(_timer_start_oneshot_at_ticks);
        KLOG_NAME(_timer_start_periodic_every_ticks);
        KLOG_NAME(_timer_start_periodic_at_ticks);

        KLOG_NAME(_lwtimer_create_periodic_queue);
        KLOG_NAME(_lwtimer_add_timer_to_queue);
        KLOG_NAME(_lwtimer_cancel_period);
        KLOG_NAME(_lwtimer_cancel_timer);
        KLOG_NAME(_lwtimer_test);

        KLOG_NAME(_event_create_component);
        KLOG_NAME(_event_create);
        KLOG_NAME(_event_destroy);
        KLOG_NAME(_event_open);
        KLOG_NAME(_event_close);
        KLOG_NAME(_event_wait_all);
        KLOG_NAME(_event_wait_all_ticks);
        KLOG_NAME(_event_wait_all_for);
        KLOG_NAME(_event_wait_all_until);
        KLOG_NAME(_event_wait_any);
        KLOG_NAME(_event_wait_any_ticks);
        KLOG_NAME(_event_wait_any_for);
        KLOG_NAME(_event_wait_any_until);
        KLOG_NAME(_event_set);
        KLOG_NAME(_event_clear);
        KLOG_NAME(_event_create_fast);
        KLOG_NAME(_event_open_fast);
        KLOG_NAME(_event_destroy_fast);
        KLOG_NAME(_event_test);

        KLOG_NAME(_lwevent_clear);
        KLOG_NAME(_lwevent_create);
        KLOG_NAME(_lwevent_destroy);
        KLOG_NAME(_lwevent_set);
        KLOG_NAME(_lwevent_test);
        KLOG_NAME(_lwevent_wait);
        KLOG_NAME(_lwevent_wait_for);
        KLOG_NAME(_lwevent_wait_until);
        KLOG_NAME(_lwevent_wait_ticks);

        KLOG_NAME(_name_create_component);
        KLOG_NAME(_name_add);
        KLOG_NAME(_name_delete);
        KLOG_NAME(_name_find);
        KLOG_NAME(_name_find_name);
        KLOG_NAME(_name_test);

        KLOG_NAME(_mutatr_init);
        KLOG_NAME(_mutatr_destroy);
        KLOG_NAME(_mutatr_set_sched_protocol);
        KLOG_NAME(_mutatr_set_priority_ceiling);

        KLOG_NAME(_mutex_create_component);
        KLOG_NAME(_mutex_init);
        KLOG_NAME(_mutex_set_priority_ceiling);
        KLOG_NAME(_mutex_destroy);
        KLOG_NAME(_mutex_lock);
        KLOG_NAME(_mutex_try_lock);
        KLOG_NAME(_mutex_unlock);

        KLOG_NAME(_mutatr_set_wait_protocol);
        KLOG_NAME(_mutatr_set_spin_limit);
        KLOG_NAME(_mutex_test);

        KLOG_NAME(_sem_create_component);
        KLOG_NAME(_sem_create);
        KLOG_NAME(_sem_destroy);
        KLOG_NAME(_sem_open);
        KLOG_NAME(_sem_close);
        KLOG_NAME(_sem_post);
        KLOG_NAME(_sem_create_fast);
        KLOG_NAME(_sem_open_fast);
        KLOG_NAME(_sem_destroy_fast);
        KLOG_NAME(_sem_test);
        KLOG_NAME(_sem_wait);
        KLOG_NAME(_sem_wait_for);
        KLOG_NAME(_sem_wait_ticks);
        KLOG_NAME(_sem_wait_until);
        KLOG_NAME(_lwsem_create);
        KLOG_NAME(_lwsem_wait);
        KLOG_NAME(_lwsem_post);
        KLOG_NAME(_lwsem_destroy);
        KLOG_NAME(_lwsem_test);
        KLOG_NAME(_lwsem_poll);
        KLOG_NAME(_lwsem_wait_ticks);
        KLOG_NAME(_lwsem_wait_for);
        KLOG_NAME(_lwsem_wait_until);

        KLOG_NAME(_watchdog_create_component);
        KLOG_NAME(_watchdog_stop);
        KLOG_NAME(_watchdog_start);
        KLOG_NAME(_watchdog_start_ticks);
        KLOG_NAME(_watchdog_test);

        KLOG_NAME(_partition_create);
        KLOG_NAME(_partition_create_at);
        KLOG_NAME(_partition_alloc);
        KLOG_NAME(_partition_alloc_zero);
        KLOG_NAME(_partition_alloc_system);
        KLOG_NAME(_partition_alloc_system_zero);
        KLOG_NAME(_partition_extend);
        KLOG_NAME(_partition_free);
        KLOG_NAME(_partition_test);
        KLOG_NAME(_partition_transfer);
        KLOG_NAME(_partition_destroy);
        KLOG_NAME(_partition_create_component);

        KLOG_NAME(_eds_create_component);
        KLOG_NAME(_eds_remove);

        default: result = "Unknown function"; break;
    } /* Endswitch */

    return result;

} /* Endbody */

#endif /* MQX_KERNEL_LOGGING */

/* EOF */
