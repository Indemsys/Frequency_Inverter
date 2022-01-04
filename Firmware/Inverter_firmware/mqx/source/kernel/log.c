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
 * $FileName: log.c$
 * $Version : 3.8.1.0$
 * $Date    : Apr-13-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Log component.
 *
 *END************************************************************************/

#include <stdarg.h>
#include "mqx_inc.h"
#if MQX_USE_LOGS
#include "log.h"
#include "log_prv.h"

/*!
 * \brief Creates a kernel component providing a general log service for all 
 * user tasks.
 * 
 * The log component provides a maximum of 16 separately configurable user logs 
 * (log numbers 0 through 15).
 * \n An application subsequently creates user logs with _log_create().
 * 
 * \return MQX_OK
 * \return MQX_OUT_OF_MEMORY (Failure.)
 * 
 * \warning Disables and enables interrupts.
 * 
 * \see _log_create   
 */ 
_mqx_uint _log_create_component(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    _mem_alloc_system_zero((_mem_size)sizeof(LOG_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (log_component_ptr == NULL)
    {
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(log_component_ptr, MEM_TYPE_LOG_COMPONENT);

    /* We must exclude all ISRs at this point */
    _int_disable();

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] != NULL)
    {
        _int_enable();
        _mem_free(log_component_ptr);
        return(MQX_OK);
    } /* Endif */
#endif

    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] = log_component_ptr;
    log_component_ptr->VALID = LOG_VALID;
    _int_enable();

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Destroys an existing log.
 * 
 * \param[in] log_number The number of a log to be destroyed.
 * 
 * \return MQX_OK  
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not previously created.)  
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)
 * 
 * \see _log_create
 * \see _log_create_component             
 */  
_mqx_uint _log_destroy
(
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_header_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return(LOG_INVALID);
    } /* Endif */
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] == NULL)
    {
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LOG_VALID)
    {
        return(MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _int_disable();
    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return(LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_component_ptr->LOGS[log_number] = NULL;
    _int_enable();

    _mem_free(log_header_ptr);

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Stops logging to the specified log.
 * 
 * A task can enable a log that has been disabled.
 * 
 * \param[in] log_number Log number of a previously created log.
 * 
 * \return MQX_OK
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)     
 * 
 * \see _log_enable
 * \see _log_read
 * \see _log_reset
 * \see _log_write
 */
_mqx_uint _log_disable
(
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_header_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return(LOG_INVALID);
    } /* Endif */
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] == NULL)
    {
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LOG_VALID)
    {
        return(MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _int_disable();
    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return(LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_header_ptr->FLAGS &= ~LOG_ENABLED;
    _int_enable();

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Starts logging to the specified log.
 * 
 * A task can enable a log that has been disabled.
 * 
 * \param[in] log_number Log number of a previously created log.
 * 
 * \return MQX_OK
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)     
 * 
 * \see _log_disable
 * \see _log_read
 * \see _log_reset
 * \see _log_write
 */
_mqx_uint _log_enable
(
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_header_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return(LOG_INVALID);
    } /* Endif */
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] == NULL)
    {
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LOG_VALID)
    {
        return(MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _int_disable();
    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return(LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_header_ptr->FLAGS |= LOG_ENABLED;
    _int_enable();

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Creates a new log.
 * 
 * Each entry in the log contains application-specified data, a timestamp (in 
 * absolute time), and a sequence number.
 * 
 * \param[in] log_number Log number to create (0 through 15).
 * \param[in] max_size   Maximum size of the data to be stored in _mqx_uints 
 * (includes LOG_ENTRY_STRUCT headers).
 * \param[in] flags      One of the following:
 * \li LOG_OVERWRITE (When the log is full, write new entries over oldest ones.)
 * \li 0 (When the log is full, do not write entries.)
 * 
 * \return MQX_OK
 * \return LOG_INVALID (Log_number exceeds 15)
 * \return MQX_INVALID_COMPONENT_BASE (Component is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)
 * \return LOG_EXISTS (Log log_number has already been created.)
 * 
 * \warning Creates the log component if it was not created.
 * 
 * \see _log_create_component
 * \see _log_destroy
 * \see _log_read
 * \see _log_write
 * \see LOG_ENTRY_STRUCT 
 */ 
_mqx_uint _log_create
(
    _mqx_uint log_number,
    _mqx_uint max_size,
    uint_32   flags
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_header_ptr;
    _mqx_uint                result;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return(LOG_INVALID);
    } /* Endif */
#endif

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];
    if (log_component_ptr == NULL)
    {
        result = _log_create_component();
        log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (log_component_ptr == NULL)
        {
            return(result);
        } /* Endif */
#endif
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LOG_VALID)
    {
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif

    log_header_ptr = (LOG_HEADER_STRUCT_PTR)
    _mem_alloc_system((_mem_size)(sizeof(LOG_HEADER_STRUCT) + max_size *
                                    sizeof(_mqx_uint)));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (log_header_ptr == NULL)
    {
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(log_header_ptr, MEM_TYPE_LOG);

    log_header_ptr->FLAGS = flags;
    log_header_ptr->FLAGS |= LOG_ENABLED;
    log_header_ptr->NUMBER = 1;
    log_header_ptr->MAX = max_size;
    log_header_ptr->SIZE = 0;
    log_header_ptr->LOG_WRITE = &log_header_ptr->DATA[0];
    log_header_ptr->LOG_READ = log_header_ptr->LOG_WRITE;
    log_header_ptr->LAST_LOG = log_header_ptr->LOG_WRITE;
    log_header_ptr->LOG_START = log_header_ptr->LOG_WRITE;
    log_header_ptr->LOG_END = &log_header_ptr->DATA[max_size];
    log_header_ptr->LOG_NEXT = NULL;

    _int_disable();
#if MQX_CHECK_ERRORS
    if (log_component_ptr->LOGS[log_number] != NULL)
    {
        _int_enable();
        _mem_free(log_header_ptr);
        return(LOG_EXISTS);
    } /* Endif */
#endif
    log_component_ptr->LOGS[log_number] = log_header_ptr;
    _int_enable();

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Reads the data from the log.
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
 * \param[in] log_number   Log number of a previously created log.
 * \param[in] read_type    Type of read operation (see Description).
 * \param[in] size_to_read Maximum number of _mqx_uints (not including the entry 
 * header) to be read from an entry.
 * \param[in] entry_ptr    Where to write the log entry (any structure that 
 * starts with LOG_STRUCT or LOG_ENTRY_STRUCT).
 * 
 * \return MQX_OK
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return LOG_ENTRY_NOT_AVAILABLE (Log entry is not available.)
 * \return LOG_INVALID_READ_TYPE (Read_type is not valid.) 
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 * \return MQX_INVALID_POINTER (Entry_ptr is NULL.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)
 * 
 * \see _log_create
 * \see _log_write
 * \see LOG_STRUCT
 * \see LOG_ENTRY_STRUCT        
 */ 
_mqx_uint _log_read
(
    _mqx_uint            log_number,
    _mqx_uint            read_type,
    _mqx_uint            size_to_read,
    LOG_ENTRY_STRUCT_PTR entry_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_header_ptr;
    LOG_ENTRY_STRUCT_PTR     log_ptr;
    _mqx_uint_ptr            data_ptr;
    _mqx_uint_ptr            next_ptr;
    _mqx_uint_ptr            user_ptr;
    _mqx_uint                size;
    uint_16                  milliseconds;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return(LOG_INVALID);
    } /* Endif */
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] == NULL)
    {
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
    if (entry_ptr == NULL)
    {
        return(MQX_INVALID_POINTER);
    } /* Endif */
#endif

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LOG_VALID)
    {
        return(MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _INT_DISABLE();
    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return(LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    if (log_header_ptr->SIZE == 0)
    {
        /* No data available */
        _INT_ENABLE();
        return(LOG_ENTRY_NOT_AVAILABLE);
    } /* Endif */

    if (read_type == LOG_READ_OLDEST_AND_DELETE)
    {
        data_ptr = log_header_ptr->LOG_READ;
    }
    else if (read_type == LOG_READ_NEXT)
    {
        data_ptr = log_header_ptr->LOG_NEXT;
        if (data_ptr == NULL)
        {
            _INT_ENABLE();
            return(LOG_ENTRY_NOT_AVAILABLE);
        } /* Endif */
    }
    else if (read_type == LOG_READ_OLDEST)
    {
        data_ptr = log_header_ptr->LOG_READ;
    }
    else if (read_type == LOG_READ_NEWEST)
    {
        data_ptr = log_header_ptr->LAST_LOG;
#if MQX_CHECK_ERRORS
    }
    else
    {
        _int_enable();
        return(LOG_INVALID_READ_TYPE);
#endif
    } /* Endif */

    size = *data_ptr;
    next_ptr = data_ptr + size;

#if 0
    if (next_ptr >= log_header_ptr->LOG_END)
    {
        next_ptr = log_header_ptr->LOG_START +
        (next_ptr - log_header_ptr->LOG_END);
    } /* Endif */
#else
    if (log_header_ptr->LOG_READ < log_header_ptr->LOG_WRITE)
    {
        /* R < W */
        if (next_ptr >= log_header_ptr->LOG_WRITE)
        {
            next_ptr = log_header_ptr->LOG_READ;
        } /* Endif */
    }
    else
    {
        /* R > W */
        if (next_ptr >= log_header_ptr->LOG_END)
        {
            next_ptr = log_header_ptr->LOG_START +
            (next_ptr - log_header_ptr->LOG_END);
        } /* Endif */
        if (next_ptr == log_header_ptr->LOG_WRITE)
        {
            next_ptr = log_header_ptr->LOG_READ;
        } /* Endif */
    } /* Endif */
#endif

    if (read_type == LOG_READ_OLDEST_AND_DELETE)
    {
        log_header_ptr->LOG_READ += size;
        log_header_ptr->SIZE -= size;
        if (log_header_ptr->LOG_READ >= log_header_ptr->LOG_END)
        {
            log_header_ptr->LOG_READ = log_header_ptr->LOG_START +
            (log_header_ptr->LOG_READ - log_header_ptr->LOG_END);
        } /* Endif */
    } /* Endif */

    if (size > (size_to_read + (_mqx_uint)sizeof(LOG_ENTRY_STRUCT)/sizeof(_mqx_uint)))
    {
        size = (size_to_read + (_mqx_uint)sizeof(LOG_ENTRY_STRUCT)/sizeof(_mqx_uint));
    } /* Endif */

    ++size;
    user_ptr = (_mqx_uint_ptr)((pointer)entry_ptr);
    while (--size)
    {
        *user_ptr++ = *data_ptr++;
        if (data_ptr == log_header_ptr->LOG_END)
        {
            data_ptr = log_header_ptr->LOG_START;
        } /* Endif */
    } /* Endwhile */
    log_ptr = entry_ptr;
    if (log_ptr->MICROSECONDS > MICROSECS_IN_MILLISECOND)
    {
        milliseconds = log_ptr->MICROSECONDS / (uint_16)MICROSECS_IN_MILLISECOND;
        log_ptr->MILLISECONDS += milliseconds;
        log_ptr->MICROSECONDS -= (milliseconds * (uint_16)MICROSECS_IN_MILLISECOND);
    } /* Endif */

    if (read_type == LOG_READ_NEXT)
    {
        log_header_ptr->LOG_NEXT = next_ptr;
    }
    else if (read_type == LOG_READ_OLDEST)
    {
        log_header_ptr->LOG_NEXT = next_ptr;
    }
    else
    {
        log_header_ptr->LOG_NEXT = NULL;
    } /* Endif */
    _INT_ENABLE();

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Resets the log to its initial state (remove all entries).
 * 
 * \param[in] log_number Log number of a previously created log.
 * 
 * \return MQX_OK
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.) 
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)
 * 
 * \see _log_disable
 * \see _log_enable   
 */  
_mqx_uint _log_reset
(
    _mqx_uint log_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_header_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return(LOG_INVALID);
    } /* Endif */
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] == NULL)
    {
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LOG_VALID)
    {
        return(MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

    _int_disable();
    log_header_ptr = log_component_ptr->LOGS[log_number];
#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        _int_enable();
        return(LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_header_ptr->SIZE = 0;
    log_header_ptr->NUMBER = 1;
    log_header_ptr->LOG_WRITE = &log_header_ptr->DATA[0];
    log_header_ptr->LOG_READ = log_header_ptr->LOG_WRITE;
    log_header_ptr->LAST_LOG = log_header_ptr->LOG_WRITE;
    _int_enable();

    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Tests the log component for consistency.
 * 
 * \param[out] log_error_ptr Pointer to the log in error (NULL if no error is 
 * found).
 * 
 * \return MQX_OK Log component data is valid (*Log_error_ptr is 0.).
 * \return LOG_INVALID Information for a specific log is not valid 
 * (*log_error_ptr contains a log number of the first invalid log.). 
 * \return MQX_INVALID_COMPONENT_BASE Log component data is not valid 
 * (*log_error_ptr is NULL.). 
 * 
 * \warning Disables and enables interrupts
 * 
 * \see _log_create_component
 * \see _log_create
 */ 
_mqx_uint _log_test
(
    _mqx_uint _PTR_ log_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_ptr;
    _mqx_uint                i;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_log_test, log_error_ptr);

    *log_error_ptr = 0;
    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];
    if (log_component_ptr == NULL)
    {
        _KLOGX2(KLOG_log_test, MQX_OK);
        return(MQX_OK);
    } /* Endif */

    if (log_component_ptr->VALID != LOG_VALID)
    {
        _KLOGX2(KLOG_log_test, MQX_INVALID_COMPONENT_BASE);
        return(MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    _int_disable();
    for (i = 0; i < LOG_MAXIMUM_NUMBER; i++)
    {
        log_ptr = log_component_ptr->LOGS[i];
        if (log_ptr != NULL)
        {
            /* Verify the log pointers */
            if ((log_ptr->LOG_END != &log_ptr->DATA[log_ptr->MAX]) ||
                            (log_ptr->LOG_START != &log_ptr->DATA[0]))
            {
                break;
            } /* Endif */

            if ((log_ptr->LOG_WRITE > log_ptr->LOG_END) ||
                            (log_ptr->LOG_NEXT > log_ptr->LOG_END) ||
                            (log_ptr->LOG_READ > log_ptr->LOG_END) ||
                            (log_ptr->LAST_LOG > log_ptr->LOG_END))
            {
                break;
            } /* Endif */
            if ((log_ptr->LOG_WRITE < log_ptr->LOG_START) ||
                            (log_ptr->LOG_READ < log_ptr->LOG_START) ||
                            (log_ptr->LAST_LOG < log_ptr->LOG_START))
            {
                break;
            } /* Endif */
            if ((log_ptr->LOG_NEXT != NULL) &&
                            (log_ptr->LOG_NEXT < log_ptr->LOG_START))
            {
                break;
            } /* Endif */
        } /* Endif */
    } /* Endfor */
    _int_enable();

    if (i == LOG_MAXIMUM_NUMBER)
    {
        _KLOGX2(KLOG_log_test, MQX_OK);
        return(MQX_OK);
    } /* Endif */

    *log_error_ptr = i;
    _KLOGX3(KLOG_log_test, LOG_INVALID, i);
    return(LOG_INVALID);

} /* Endbody */

/*!
 * \brief Writes data to the log.
 * 
 * The function writes the log entry only if it returns MQX_OK.
 * 
 * \param[in] log_number           Log number of a previously created log.
 * \param[in] number_of_parameters Number of parameters to write.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Log component is not created.)
 * \return MQX_INVALID_COMPONENT_HANDLE (Log component data is not valid.)
 * \return LOG_INVALID (Log_number is out of range.)
 * \return LOG_DOES_NOT_EXIST (Log_number was not created.)
 * \return LOG_DISABLED (Log is disabled.)
 * \return LOG_FULL (Log is full and LOG_OVERWRITE is not set.)      
 */ 
_mqx_uint _log_write
(
    _mqx_uint log_number,
    _mqx_uint number_of_parameters,

    ...
)
{ /* Body */
    MQX_TICK_STRUCT          ticks;
    TIME_STRUCT              time;
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    LOG_COMPONENT_STRUCT_PTR log_component_ptr;
    LOG_HEADER_STRUCT_PTR    log_header_ptr;
    _mqx_uint_ptr            data_ptr;
    va_list                  param_ptr;
    _mqx_uint                us;
    _mqx_uint                i;
    _mqx_uint                size;
    _mqx_uint                old_size;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_LOG] == NULL)
    {
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    log_component_ptr = (LOG_COMPONENT_STRUCT_PTR)
    kernel_data->KERNEL_COMPONENTS[KERNEL_LOG];

#if MQX_CHECK_VALIDITY
    if (log_component_ptr->VALID != LOG_VALID)
    {
        return(MQX_INVALID_COMPONENT_HANDLE);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (log_number >= LOG_MAXIMUM_NUMBER)
    {
        return(LOG_INVALID);
    } /* Endif */
#endif

    log_header_ptr = log_component_ptr->LOGS[log_number];

#if MQX_CHECK_ERRORS
    if (log_header_ptr == NULL)
    {
        return(LOG_DOES_NOT_EXIST);
    } /* Endif */
#endif

    if (! (log_header_ptr->FLAGS & LOG_ENABLED))
    {
        return(LOG_DISABLED);
    } /* Endif */

    size = sizeof(LOG_ENTRY_STRUCT)/sizeof(_mqx_uint) + number_of_parameters;
#if MQX_CHECK_ERRORS
    if (size > log_header_ptr->MAX)
    {
        /* 
         * The request is to log an entry larger than the
         * entire log. Reject this.
         */
        return(LOG_FULL);
    } /* Endif */
#endif

    va_start(param_ptr, number_of_parameters);

    i = number_of_parameters + 1;

    _INT_DISABLE();
    if ((log_header_ptr->SIZE + size) > log_header_ptr->MAX)
    {
        if (log_header_ptr->FLAGS & LOG_OVERWRITE)
        {
            /* Remove leading elements until it can fit */
            while ((log_header_ptr->SIZE + size) > log_header_ptr->MAX)
            {
                data_ptr = log_header_ptr->LOG_READ;
                old_size = *data_ptr;
                log_header_ptr->SIZE -= old_size;
                data_ptr += old_size;
                if (data_ptr >= log_header_ptr->LOG_END)
                {
                    data_ptr = log_header_ptr->LOG_START +
                    (data_ptr - log_header_ptr->LOG_END);
                } /* Endif */
                if (log_header_ptr->LOG_NEXT == log_header_ptr->LOG_READ)
                {
                    /* Move up the next read pointer */
                    log_header_ptr->LOG_NEXT = data_ptr;
                } /* Endif */
                log_header_ptr->LOG_READ = data_ptr;
            } /* Endwhile */
        }
        else
        {
            _INT_ENABLE();
            va_end(param_ptr);
            return(LOG_FULL);
        } /* Endif */
    } /* Endif */

    log_header_ptr->SIZE += size;

    data_ptr = log_header_ptr->LOG_WRITE;
    log_header_ptr->LAST_LOG = data_ptr;

    *data_ptr++ = size;
    if (data_ptr == log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */

    *data_ptr++ = log_header_ptr->NUMBER++;
    if (data_ptr == log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */

    us = (_mqx_uint)_time_get_microseconds();
    PSP_ADD_TICKS(&kernel_data->TIME, &kernel_data->TIME_OFFSET, &ticks);
    PSP_TICKS_TO_TIME(&ticks, &time);

#if MQX_INT_SIZE_IN_BITS >= 32
    *data_ptr++ = (_mqx_uint)time.SECONDS;
#else
#if PSP_ENDIAN == MQX_BIG_ENDIAN
    *data_ptr++ = (_mqx_uint)((time.SECONDS >> MQX_INT_SIZE_IN_BITS) &
                    MAX_MQX_UINT);
    if (data_ptr == log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */
    *data_ptr++ = (_mqx_uint)(time.SECONDS & MAX_MQX_UINT);
#else
    *data_ptr++ = (_mqx_uint)(time.SECONDS & MAX_MQX_UINT);
    if (data_ptr == log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */
    *data_ptr++ = (_mqx_uint)((time.SECONDS >> MQX_INT_SIZE_IN_BITS) &
                    MAX_MQX_UINT);
#endif
#endif

    if (data_ptr >= log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */

#if PSP_MEMORY_ADDRESSING_CAPABILITY >= 32
    *data_ptr++ = time.MILLISECONDS;
    if (data_ptr == log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */
    *data_ptr++ = us;
#elif MQX_INT_SIZE_IN_BITS == 32
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
    *data_ptr++ = time.MILLISECONDS | (us << 16);
#else
    *data_ptr++ = us | (time.MILLISECONDS << 16);
#endif
#else
    *data_ptr++ = (_mqx_uint)time.MILLISECONDS;
    if (data_ptr == log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */
    *data_ptr++ = us;
#endif

    if (data_ptr >= log_header_ptr->LOG_END)
    {
        data_ptr = log_header_ptr->LOG_START;
    } /* Endif */

    while (--i)
    {
        *data_ptr++ = (_mqx_uint)va_arg(param_ptr, _mqx_uint);
        if (data_ptr == log_header_ptr->LOG_END)
        {
            data_ptr = log_header_ptr->LOG_START;
        } /* Endif */
    } /* Endwhile */

    log_header_ptr->LOG_WRITE = data_ptr;
    _INT_ENABLE();

    va_end(param_ptr);

    return(MQX_OK);

} /* Endbody */

#endif /* MQX_USE_LOGS */

/* EOF */
