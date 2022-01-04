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
 * $FileName: lwlog.c$
 * $Version : 3.8.0.1$
 * $Date    : Feb-22-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Lightweight LOG component.
 *
 *END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_LWLOGS
#include "lwlog.h"
#include "lwlog_prv.h"

#if MQX_LITE_VERSION_NUMBER
LWLOG_COMPONENT_STRUCT lwlog_struct = {0};
#endif /* MQX_LITE_VERSION_NUMBER */

/*!
 * \brief This function creates a kernel component providing a lightweight log
 * service for all user tasks.
 *
 * The lightweight log component provides a maximum of 16 logs, all with the same
 * size of entries. Log number 0 is reserved for kernel log.
 * \n An application subsequently creates lightweight logs with _lwlog_create() or
 * _lwlog_create_at().
 *
 * \return MQX_OK
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 *
 * \warning Cannot be called from an ISR.
 *
 * \see _lwlog_create
 * \see _lwlog_create_at
 * \see _klog_create
 * \see _klog_create_at
 */
_mqx_uint _lwlog_create_component(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        return MQX_CANNOT_CALL_FUNCTION_FROM_ISR;
    } /* Endif */
#endif

    _lwsem_wait((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG] != NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);
        return (MQX_OK);
    } /* Endif */
#endif

#if MQX_LITE_VERSION_NUMBER
    log_component_ptr = &lwlog_struct;
#else
    log_component_ptr = _mem_alloc_system_zero((_mem_size) sizeof(LWLOG_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (log_component_ptr == NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(log_component_ptr, MEM_TYPE_LWLOG_COMPONENT);
#endif /* MQX_LITE_VERSION_NUMBER */

    kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG] = log_component_ptr;
    log_component_ptr->VALID = LWLOG_VALID;

    _lwsem_post((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Creates the lightweight log at the specified location.
 *
 * Each entry in the log is the same size and contains a sequence number, a timestamp,
 * and a seven-element array of application-defined data.
 * \n Use _lwlog_calculate_size() to determine amount of memory needed to create
 * log with requested number of entries.
 *
 * \param[in] log_number Log number to create ( 1 through 15; 0 is reserved for
 * kernel log).
 * \param[in] max_size   Maximum number of entries in the log.
 * \param[in] flags      LOG_OVERWRITE (when the log is full, write new entries
 * over oldest ones), NULL (when the log is full, do not write entries; the default
 * behavior).
 * \param[in] where      Where to create the lightweight log.
 *
 * \return MQX_OK
 * \return LOG_EXISTS (Lightweight log with log number log_number exists.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_INVALID_SIZE (Max_size is 0.)
 * \return MQX_INVALID_POINTER (Where is NULL.)
 * \return MQX_INVALID_COMPONENT_BASE (Data for the lightweight log component is
 * not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 *
 * \warning Creates the lightweight log component if it was not created.
 *
 * \see _lwlog_create
 * \see _lwlog_create_component
 * \see _klog_create
 * \see _klog_create_at
 */
_mqx_uint _lwlog_create_at
(
    _mqx_uint log_number,
    _mqx_uint max_size,
    _mqx_uint flags,
    pointer   where
)
{ /* Body */
    LWLOG_HEADER_STRUCT_PTR log_header_ptr = (LWLOG_HEADER_STRUCT_PTR) where;
    _mqx_uint               result;

#if MQX_CHECK_ERRORS
    if (max_size == 0)
    {
        return (LOG_INVALID_SIZE);
    } /* Endif */

    if (where == NULL)
    {
        return (MQX_INVALID_POINTER);
    } /* Endif */
#endif

    result = _lwlog_create_internal(log_number, max_size, flags, log_header_ptr);
    if (result == MQX_OK)
    {
        log_header_ptr->TYPE = LWLOG_STATIC;
    } /* Endif */

    return (result);

} /* Endbody */

/*!
 * \private
 *
 * \brief Creates the lightweight log.
 *
 * \param[in] log_number     Log number to create ( 1 through 15; 0 is reserved
 * for kernel log).
 * \param[in] max_size       Maximum number of entries in the log.
 * \param[in] flags          LOG_OVERWRITE (when the log is full, write new entries
 * over oldest ones), NULL (when the log is full, do not write entries; the default
 * behavior).
 * \param[in] log_header_ptr Pointer to lightweight log header.
 *
 * \return MQX_OK
 * \return LOG_INVALID (log_number is out of range.)
 * \return LOG_EXISTS (Lightweight log with log number log_number exists.)
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Data for the lightweight log component is
 * not valid.)
 *
 * \see _lwlog_create
 * \see _lwlog_create_at
 * \see LWLOG_HEADER_STRUCT
 */
_mqx_uint _lwlog_create_internal
(
    _mqx_uint log_number,
    _mqx_uint max_size,
    _mqx_uint flags,
    LWLOG_HEADER_STRUCT_PTR log_header_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_ENTRY_STRUCT_PTR     entry_ptr;
    _mqx_uint                  i;
    _mqx_uint                  result = MQX_OK;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return (LOG_INVALID);
    } /* Endif */
#endif

    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];
    if (log_component_ptr == NULL)
    {
        result = _lwlog_create_component();
        log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (log_component_ptr == NULL)
        {
            return (result);
        } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    }
    else if (log_component_ptr->LOGS[log_number] != NULL)
    {
        return (LOG_EXISTS);
#endif
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif
    _mem_zero((pointer) log_header_ptr, (_mem_size) sizeof(LWLOG_HEADER_STRUCT));

    log_header_ptr->FLAGS = flags;
    log_header_ptr->FLAGS |= LWLOG_ENABLED;
    log_header_ptr->NUMBER = 1;
    log_header_ptr->MAX_ENTRIES = max_size;

    entry_ptr = &log_header_ptr->FIRST_ENTRY;

    log_header_ptr->READ_PTR = entry_ptr;
    log_header_ptr->OLDEST_PTR = entry_ptr;

    max_size--;
    for (i = 0; i < max_size; i++)
    {
        entry_ptr->NEXT_PTR = entry_ptr + 1;
        entry_ptr++;
    } /* Endfor */

    log_header_ptr->WRITE_PTR = entry_ptr;
    entry_ptr->NEXT_PTR = log_header_ptr->READ_PTR;

    _int_disable();
#if MQX_CHECK_ERRORS
    if (log_component_ptr->LOGS[log_number] != NULL)
    {
        _int_enable();
        return (LOG_EXISTS);
    } /* Endif */
#endif
    log_component_ptr->LOGS[log_number] = log_header_ptr;
    _int_enable();

    return (result);

} /* Endbody */


#if !MQX_LITE_VERSION_NUMBER
/*!
 * \brief Creates the lightweight log.
 *
 * Each entry in the log is the same size and contains a sequence number, a timestamp,
 * and a seven-element array of application-defined data.
 *
 * \param[in] log_number Log number to create ( 1 through 15; 0 is reserved for
 * kernel log).
 * \param[in] max_size   Maximum number of entries in the log.
 * \param[in] flags      LOG_OVERWRITE (when the log is full, write new entries
 * over oldest ones), NULL (when the log is full, do not write entries; the default
 * behavior).
 *
 * \return MQX_OK
 * \return LOG_EXISTS (Lightweight log with log number log_number exists.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_INVALID_SIZE (Max_size is 0.)
 * \return MQX_INVALID_COMPONENT_BASE (Data for the lightweight log component is
 * not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 *
 * \warning Creates the lightweight log component if it was not created.
 *
 * \see _lwlog_create_at
 * \see _lwlog_create_component
 * \see _klog_create
 * \see _klog_create_at
 */
_mqx_uint _lwlog_create
(
    _mqx_uint log_number,
    _mqx_uint max_size,
    _mqx_uint flags
)
{ /* Body */
    LWLOG_HEADER_STRUCT_PTR log_header_ptr;
    _mqx_uint               result;

#if MQX_CHECK_ERRORS
    if (max_size == 0)
    {
        return LOG_INVALID_SIZE;
    } /* Endif */
#endif

    log_header_ptr = _mem_alloc_system_zero((_mem_size) sizeof(LWLOG_HEADER_STRUCT) + (_mem_size) (max_size - 1)
                    * (_mem_size) sizeof(LWLOG_ENTRY_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (log_header_ptr == NULL)
    {
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(log_header_ptr, MEM_TYPE_LWLOG);

    result = _lwlog_create_internal(log_number, max_size, flags, log_header_ptr);
    if (result == MQX_OK)
    {
        log_header_ptr->TYPE = LWLOG_DYNAMIC;
    }
    else
    {
        _mem_free(log_header_ptr);
    } /* Endif */

    return (result);

} /* Endbody */
#endif /* MQX_LITE_VERSION_NUMBER */

/*!
 * \brief Destroys an existing lightweight log.
 *
 * \param[in] log_number Log number of a previously created lightweight log (if
 * log_number is 0, kernel log is destroyed).
 *
 * \return MQX_OK
 * \return LOG_DOES_NOT_EXIST (Log_number was not previously created.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Lightweight log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Lightweight log component data is not valid.)
 *
 * \see _lwlog_create
 * \see _lwlog_create_at
 * \see _lwlog_create_component
 */
_mqx_uint _lwlog_destroy
(
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_HEADER_STRUCT_PTR    log_header_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return (LOG_INVALID);
    } /* Endif */
#endif

    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];

#if MQX_CHECK_ERRORS
    if (log_component_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        return (MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _int_disable();

    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return (LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_component_ptr->LOGS[log_number] = NULL;
    _int_enable();

#if !MQX_LITE_VERSION_NUMBER
    if (log_header_ptr->TYPE == LWLOG_DYNAMIC)
    {
        _mem_free(log_header_ptr);
    } /* Endif */
#endif

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Stops logging to the selected lightweight log.
 *
 * \param[in] log_number Log number of a previously created lightweight log (if
 * log_number is 0, kernel log is disabled).
 *
 * \return MQX_OK
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Lightweight log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Lightweight log component data is not valid.)
 *
 * \see _lwlog_enable
 * \see _lwlog_read
 * \see _lwlog_reset
 * \see _lwlog_write
 */
_mqx_uint _lwlog_disable
(
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_HEADER_STRUCT_PTR    log_header_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return (LOG_INVALID);
    } /* Endif */
#endif

    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];

#if MQX_CHECK_ERRORS
    if (log_component_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        return (MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _int_disable();
    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return (LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_header_ptr->FLAGS &= ~LWLOG_ENABLED;
    _int_enable();

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Starts logging to the selected lightweight log.
 *
 * \param[in] log_number Log number of a previously created lightweight log (if
 * log_number is 0, kernel log is enabled).
 *
 * \return MQX_OK
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Lightweight log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Lightweight log component data is not valid.)
 *
 * \see _lwlog_disable
 * \see _lwlog_read
 * \see _lwlog_reset
 * \see _lwlog_write
 */
_mqx_uint _lwlog_enable
(
    /* [IN] the log number to be used */
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_HEADER_STRUCT_PTR    log_header_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return (LOG_INVALID);
    } /* Endif */
#endif

    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];

#if MQX_CHECK_ERRORS
    if (log_component_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        return (MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _int_disable();
    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return (LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_header_ptr->FLAGS |= LWLOG_ENABLED;
    _int_enable();

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Reads the information in the lightweight log.
 *
 * <table>
 *   <tr>
 *     <td><b>read_type</b></td>
 *     <td><b>Returns this entry in the log:</b></td>
 *   </tr>
 *   <tr>
 *     <td>LOG_READ_NEWEST</td>
 *     <td>Newest.</td>
 *   </tr>
 *   <tr>
 *     <td>LOG_READ_NEXT</td>
 *     <td>Next one after the previous one read (must be used with LOG_READ_OLDEST).</td>
 *   </tr>
 *   <tr>
 *     <td>LOG_READ_OLDEST</td>
 *     <td>Oldest.</td>
 *   </tr>
 *   <tr>
 *     <td>LOG_READ_OLDEST_AND_DELETE</td>
 *     <td>Oldest and deletes it.</td>
 *   </tr>
 * </table>
 *
 * \param[in] log_number Log number of a previously created lightweight log (if
 * log_number is 0, kernel log is read).
 * \param[in] read_type  Type of read operation.
 * \param[in] entry_ptr  Pointer to where to write the lightweight log entry.
 *
 * \return MQX_OK
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return LOG_ENTRY_NOT_AVAILABLE (Log entry is not available.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_INVALID_READ_TYPE (Read_type is not valid.)
 * \return MQX_INVALID_POINTER (Entry_ptr is NULL.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Lightweight log component data is not valid.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Lightweight log component is not created.)
 *
 * \see _lwlog_create
 * \see _lwlog_create_at
 * \see _lwlog_write
 * \see _klog_display
 * \see LWLOG_ENTRY_STRUCT
 */
_mqx_uint _lwlog_read
(
    _mqx_uint               log_number,
    _mqx_uint               read_type,
    LWLOG_ENTRY_STRUCT_PTR  entry_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_HEADER_STRUCT_PTR    log_header_ptr;
    LWLOG_ENTRY_STRUCT_PTR     log_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return (LOG_INVALID);
    } /* Endif */
    if (entry_ptr == NULL)
    {
        return (MQX_INVALID_POINTER);
    } /* Endif */
#endif

    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];

#if MQX_CHECK_ERRORS
    if (log_component_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    _int_disable();
#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    log_header_ptr = log_component_ptr->LOGS[log_number];

#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return (LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    if (!log_header_ptr->CURRENT_ENTRIES)
    {
        /* No data available */
        _int_enable();
        return (LOG_ENTRY_NOT_AVAILABLE);
    } /* Endif */

    if (read_type == LOG_READ_OLDEST_AND_DELETE)
    {
        log_header_ptr->CURRENT_ENTRIES--;
        log_ptr = log_header_ptr->OLDEST_PTR;
        log_header_ptr->OLDEST_PTR = log_ptr->NEXT_PTR;
        log_header_ptr->READ_PTR = log_ptr->NEXT_PTR;
        log_header_ptr->READS = 0;
    }
    else if (read_type == LOG_READ_OLDEST)
    {
        log_ptr = log_header_ptr->OLDEST_PTR;
        log_header_ptr->READ_PTR = log_ptr->NEXT_PTR;
        log_header_ptr->READS = 1;
    }
    else if (read_type == LOG_READ_NEXT)
    {
        log_ptr = log_header_ptr->READ_PTR;
        if ((log_ptr == log_header_ptr->WRITE_PTR->NEXT_PTR) && (log_header_ptr->READS
                        >= log_header_ptr->CURRENT_ENTRIES))
        {
            _int_enable();
            return (LOG_ENTRY_NOT_AVAILABLE);
        } /* Endif */
        log_header_ptr->READ_PTR = log_ptr->NEXT_PTR;
        log_header_ptr->READS++;
    }
    else if (read_type == LOG_READ_NEWEST)
    {
        log_header_ptr->READS = log_header_ptr->CURRENT_ENTRIES;
        log_ptr = log_header_ptr->WRITE_PTR;
#if MQX_CHECK_ERRORS
    }
    else
    {
        _int_enable();
        return (LOG_INVALID_READ_TYPE);
#endif
    } /* Endif */

    *entry_ptr = *log_ptr;

    _int_enable();

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Resets the lightweight log to its initial state (remove all entries).
 *
 * \param[in] log_number Log number of a previously created lightweight log (if
 * log_number is 0, kernel log is reseted).
 *
 * \return MQX_OK
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)
 *
 * \see _lwlog_disable
 * \see _lwlog_enable
 */
_mqx_uint _lwlog_reset
(
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_HEADER_STRUCT_PTR    log_header_ptr;
    LWLOG_ENTRY_STRUCT_PTR     entry_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return (LOG_INVALID);
    } /* Endif */
#endif

    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];

    if (log_component_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */

    _int_disable();

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        return (MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return (LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_header_ptr->READS = 0;
    log_header_ptr->CURRENT_ENTRIES = 0;
    log_header_ptr->NUMBER = 1;

    entry_ptr = &log_header_ptr->FIRST_ENTRY;

    log_header_ptr->WRITE_PTR = entry_ptr + (log_header_ptr->MAX_ENTRIES - 1);
    log_header_ptr->READ_PTR = entry_ptr;
    log_header_ptr->OLDEST_PTR = entry_ptr;

    _int_enable();

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Calculates the size required for the lightweight log with given number
 * of entries.
 *
 * The calculation takes into account size of lwlog entries and also the lwlog
 * header structure.
 *
 * \param[in] entries Maximum number of entries in the lightweight log.
 *
 * \return Number of single-addressable units required
 *
 * \see _lwlog_create
 * \see _lwlog_create_at
 * \see _lwlog_create_component
 * \see _klog_create
 * \see _klog_create_at
 */
_mem_size _lwlog_calculate_size
(
    _mqx_uint entries
)
{ /* Body */
    _mem_size result;

#if MQX_CHECK_ERRORS
    if (entries == 0)
    {
        return 0;
    } /* Endif */
#endif

    result = (_mem_size) sizeof(LWLOG_HEADER_STRUCT) + (_mem_size) (entries - 1)
                    * (_mem_size) sizeof(LWLOG_ENTRY_STRUCT);

    return result;

} /* Endbody */

/*!
 * \brief Tests the lightweight log component for consistency.
 *
 * \param[out] log_error_ptr Pointer to the lightweight log if error is found (NULL
 * if no error is found).
 *
 * \return MQX_OK Lightweight log component data is valid (Log_error_ptr is NULL.).
 * \return LOG_INVALID Information for a specific lightweight log is not valid
 * (Log_error_ptr contains log number of the first invalid lightweight log.).
 * \return MQX_INVALID_POINTER Log_error_ptr is NULL.
 * \return MQX_INVALID_COMPONENT_BASE Lightweight log component data is not valid
 * (Log_error_ptr is NULL.).
 *
 * \see _lwlog_create_component
 * \see _lwlog_create
 * \see _lwlog_create_at
 */
_mqx_uint _lwlog_test
(
    _mqx_uint _PTR_ log_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_HEADER_STRUCT_PTR    log_header_ptr;
    LWLOG_ENTRY_STRUCT_PTR     entry_ptr;
    uchar_ptr                  min_ptr;
    uchar_ptr                  max_ptr;
    _mqx_uint                  i, j;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if ((pointer) log_error_ptr == NULL)
    {
        return MQX_INVALID_POINTER;
    } /* Endif */
#endif

    *log_error_ptr = 0;
    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];
    if (log_component_ptr == NULL)
    {
        return (MQX_OK);
    } /* Endif */

    _int_disable();

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    for (i = 0; i < LOG_MAXIMUM_NUMBER; i++)
    {
        log_header_ptr = log_component_ptr->LOGS[i];
        if (log_header_ptr != NULL)
        {
            /* Verify the log pointers */
            min_ptr = (uchar_ptr) log_header_ptr + sizeof(LWLOG_HEADER_STRUCT_PTR);
            max_ptr = min_ptr + sizeof(LWLOG_ENTRY_STRUCT) * log_header_ptr->MAX_ENTRIES;

            if (((uchar_ptr) log_header_ptr->READ_PTR < min_ptr) || ((uchar_ptr) log_header_ptr->READ_PTR >= max_ptr)
                            || ((uchar_ptr) log_header_ptr->WRITE_PTR < min_ptr)
                            || ((uchar_ptr) log_header_ptr->WRITE_PTR >= max_ptr)
                            || ((uchar_ptr) log_header_ptr->OLDEST_PTR < min_ptr)
                            || ((uchar_ptr) log_header_ptr->OLDEST_PTR >= max_ptr))

            {
                _int_enable();
                *log_error_ptr = i;
                return (LOG_INVALID);
            } /* Endif */

            /* Check each entry in the log */
            entry_ptr = &log_header_ptr->FIRST_ENTRY;
            j = log_header_ptr->MAX_ENTRIES;
            while (entry_ptr->NEXT_PTR && j)
            {
                entry_ptr = entry_ptr->NEXT_PTR;
                --j;
                if (((uchar_ptr) entry_ptr < min_ptr) || ((uchar_ptr) entry_ptr >= max_ptr))
                {
                    _int_enable();
                    *log_error_ptr = i;
                    return (LOG_INVALID);
                } /* Endif */
            } /* Endwhile */
        } /* Endif */
    } /* Endfor */

    _int_enable();

    return (MQX_OK);

} /* Endbody */

/*!
 * \private
 *
 * \brief Writes data to the lightweight log.
 *
 * \param[in] log_number Log number of a previously created lightweight log.
 * \param[in] p1         Data to be written into the log entry.
 * \param[in] p2         Data to be written into the log entry.
 * \param[in] p3         Data to be written into the log entry.
 * \param[in] p4         Data to be written into the log entry.
 * \param[in] p5         Data to be written into the log entry.
 * \param[in] p6         Data to be written into the log entry.
 * \param[in] p7         Data to be written into the log entry.
 *
 * \return MQX_OK
 * \return LOG_FULL (Log is full and LOG_OVERWRITE is not set.)
 * \return LOG_DISABLED (Log is disabled.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)
 *
 * \see _lwlog_write
 */
_mqx_uint _lwlog_write_internal
(
    _mqx_uint     log_number,
    _mqx_max_type p1,
    _mqx_max_type p2,
    _mqx_max_type p3,
    _mqx_max_type p4,
    _mqx_max_type p5,
    _mqx_max_type p6,
    _mqx_max_type p7
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    LWLOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LWLOG_HEADER_STRUCT_PTR    log_header_ptr;
    _mqx_max_type _PTR_        data_ptr;
    LWLOG_ENTRY_STRUCT_PTR     log_ptr;
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    TIME_STRUCT                time;
    MQX_TICK_STRUCT ticks;
#endif

    _GET_KERNEL_DATA(kernel_data);

    log_component_ptr = (LWLOG_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG];
    log_header_ptr = log_component_ptr->LOGS[log_number];

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LWLOG_VALID)
    {
        return (MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        return (LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    if (!(log_header_ptr->FLAGS & LWLOG_ENABLED))
    {
        return (LOG_DISABLED);
    } /* Endif */

    log_ptr = log_header_ptr->WRITE_PTR->NEXT_PTR;

    if (log_header_ptr->CURRENT_ENTRIES >= log_header_ptr->MAX_ENTRIES)
    {
        if (log_header_ptr->FLAGS & LOG_OVERWRITE)
        {
            if (log_ptr == log_header_ptr->READ_PTR)
            {
                log_header_ptr->READ_PTR = log_ptr->NEXT_PTR;
            } /* Endif */
            log_header_ptr->OLDEST_PTR = log_ptr->NEXT_PTR;
        }
        else
        {
            return (LOG_FULL);
        } /* Endif */
    }
    else
    {
        log_header_ptr->CURRENT_ENTRIES++;
    } /* Endif */

#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
    log_ptr->MICROSECONDS = (uint_32)_time_get_microseconds();
    PSP_ADD_TICKS(&kernel_data->TIME, &kernel_data->TIME_OFFSET, &ticks);
    PSP_TICKS_TO_TIME(&ticks, &time);
    log_ptr->SECONDS = time.SECONDS;
    log_ptr->MILLISECONDS = time.MILLISECONDS;
#else
    log_ptr->TIMESTAMP = kernel_data->TIME;
    log_ptr->TIMESTAMP.HW_TICKS = _time_get_hwticks();
    PSP_ADD_TICKS(&log_ptr->TIMESTAMP, &kernel_data->TIME_OFFSET,
                    &log_ptr->TIMESTAMP);
#endif

    log_ptr->SEQUENCE_NUMBER = log_header_ptr->NUMBER++;

    data_ptr = &log_ptr->DATA[0];
    *data_ptr++ = p1;
    *data_ptr++ = p2;
    *data_ptr++ = p3;
    *data_ptr++ = p4;
    *data_ptr++ = p5;
    *data_ptr++ = p6;
    *data_ptr = p7;

    log_header_ptr->WRITE_PTR = log_ptr;

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Writes to the lightweight log.
 *
 * This function writes the log entry only if it returns MQX_OK.
 *
 * \param[in] log_number Log number of a previously created lightweight log.
 * \param[in] p1         Data to be written to the log entry. If log_number is 0
 * and p1 is >= 10 (0 through 9 are reserved for MQX), data specified by p2 through
 * p7 is written to kernel log.
 * \param[in] p2         Data to be written into the log entry.
 * \param[in] p3         Data to be written into the log entry.
 * \param[in] p4         Data to be written into the log entry.
 * \param[in] p5         Data to be written into the log entry.
 * \param[in] p6         Data to be written into the log entry.
 * \param[in] p7         Data to be written into the log entry.
 *
 * \return MQX_OK
 * \return LOG_FULL (Log is full and LOG_OVERWRITE is not set.)
 * \return LOG_DISABLED (Log is disabled.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 *
 * \see _lwlog_create
 * \see _lwlog_create_at
 * \see _lwlog_read
 * \see _lwlog_disable
 * \see _lwlog_enable
 */
_mqx_uint _lwlog_write
(
    _mqx_uint     log_number,
    _mqx_max_type p1,
    _mqx_max_type p2,
    _mqx_max_type p3,
    _mqx_max_type p4,
    _mqx_max_type p5,
    _mqx_max_type p6,
    _mqx_max_type p7
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return (LOG_INVALID);
    } /* Endif */

    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG] == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    _INT_DISABLE();

    result = _lwlog_write_internal(log_number, p1, p2, p3, p4, p5, p6, p7);

    _INT_ENABLE();

    return (result);

} /* Endbody */

#endif /* MQX_USE_LWLOGS */

/* EOF */
