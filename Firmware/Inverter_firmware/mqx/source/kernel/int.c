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
 * $FileName: int.c$
 * $Version : 3.8.0.1$
 * $Date    : Feb-22-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Interrupt component.
 *
 *END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_INTERRUPTS

/*!
 * \brief Default ISR that MQX calls if an unhandled interrupt or exception occurs.
 *
 * This is the default MQX handler of interrupts or expections not hanlded by
 * a specific handler routine. This function changes the state of the active
 * task to UNHANDLED_INT_BLOCKED and blocks it.
 *
 * \param[in] vector_number Parameter that MQX passes to the ISR.
 *
 * \warning Blocks the active task. Do not call this function, use it as a default
 * handler of unhandled interrupts or exceptions.
 *
 * \see _int_install_default_isr
 * \see _int_install_unexpected_isr
 * \see _int_install_exception_isr
 */
void _int_default_isr
(
    pointer vector_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR          td_ptr;

    _GET_KERNEL_DATA(kernel_data);

    td_ptr = kernel_data->ACTIVE_PTR;
    _KLOGE5(KLOG_int_default_isr, td_ptr, vector_number,
                    &vector_number, vector_number);

    _int_disable();
    if (td_ptr->STATE != UNHANDLED_INT_BLOCKED)
    {
        td_ptr->STATE = UNHANDLED_INT_BLOCKED;
        td_ptr->INFO = (_mqx_uint) vector_number;
        _task_set_error_td_internal(td_ptr, MQX_UNHANDLED_INTERRUPT);
        _QUEUE_UNLINK(td_ptr);
    } /* Endif */
    _int_enable();

} /* Endbody */

/*!
 * \brief This function disables all interrupts for active task.
 *
 * The _int_disable() function disables all hardware interrupts at priorities up
 * to and including the MQX disable-interrupt level. As a result, no task can
 * interrupt the active task while the active task is running until interrupts
 * are re-enabled with _int_enable(). If the active task blocks while interrupts
 * are disabled, the state of the interrupts (disabled or enabled) depends on
 * the interrupt-disabled state of the next task that MQX makes ready.
 * \n Keep minimum code between calls to _int_disable() and its matching _int_enable().
 * If _int_disable() or _int_enable() are nested, MQX re-enables interrupts only
 * after the number of calls to _int_enable() are equal to the number of calls to
 * _int_disable().
 *
 * \see _int_enable
 */
void _int_disable(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    _INT_DISABLE_CODE();

} /* Endbody */

/*!
 * \brief This function enables all interrupts for active task.
 *
 * This function _int_enable() resets the processor priority to the hardware
 * priority that corresponds to the active task's software priority.
 * Keep minimum code between calls to _int_disable() and its matching
 * _int_enable().
 * \n If _int_disable() or _int_enable() are nested, MQX re-enables interrupts only
 * after the number of calls to _int_enable() are equal to the number of calls to
 * _int_disable().
 *
 * \see _int_disable
 */
void _int_enable(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    _INT_ENABLE_CODE();

} /* Endbody */

/*!
 * \brief Gets a pointer to the default ISR that MQX calls when an unexpected interrupt occurs.
 *
 * This function returns the pointer to current default ISR.
 *
 * \return Pointer to the default ISR for unhandled interrupts.
 * \return NULL (Failure.)
 *
 * \see _int_install_default_isr
 */
INT_ISR_FPTR _int_get_default_isr
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return(kernel_data->DEFAULT_ISR);

} /* Endbody */

/*!
 * \brief Gets a pointer to the current ISR exception handler for the vector number.
 *
 * The returned exception handler is either a default ISR or an ISR that the
 * application installed with _int_set_exception_handler().
 *
 * \param[in] vector Number of a vector whose exception handler is to be returned.
 *
 * \return Pointer to the current exception handler.
 * \return NULL (Failure.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code.
 *
 * \see _int_set_exception_handler
 * \see _int_exception_isr
 * \see _task_set_error
 */
#if !MQX_SPARSE_ISR_TABLE

INT_EXCEPTION_FPTR _int_get_exception_handler
(
    _mqx_uint vector
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        return(NULL);
    }/* Endif */
#endif

    vector -= kernel_data->FIRST_USER_ISR_VECTOR;
    return(kernel_data->INTERRUPT_TABLE_PTR[vector].APP_ISR_EXCEPTION_HANDLER);

} /* Endbody */

#else /* MQX_SPARSE_ISR_TABLE */

INT_EXCEPTION_FPTR _int_get_exception_handler
(
    _mqx_uint vector
)
{
    return NULL;
}

#endif /* MQX_SPARSE_ISR_TABLE */

/*!
 * \brief Gets the current ISR for the specified vector.
 *
 * The returned ISR is either a default ISR or an ISR that the application
 * installed with _int_install_isr().
 *
 * \param[in] vector Number of the vector whose ISR is to be returned.
 *
 * \return Pointer to the ISR. (Success.)
 * \return NULL (Failure.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code.
 *
 * \see _int_install_isr
 * \see _int_get_isr_data
 * \see _int_set_isr_data
 * \see _task_set_error
 */
#if !MQX_SPARSE_ISR_TABLE

INT_ISR_FPTR _int_get_isr
(
    _mqx_uint vector
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        return NULL;
    } /* Endif */
#endif

    vector -= kernel_data->FIRST_USER_ISR_VECTOR;

    return(kernel_data->INTERRUPT_TABLE_PTR[vector].APP_ISR);

} /* Endbody */

#else /* MQX_SPARSE_ISR_TABLE */
INT_ISR_FPTR _int_get_isr
(
    _mqx_uint vector
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR          kernel_data;
    INTERRUPT_SPARSE_REC_STRUCT_PTR int_ptr;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        return NULL;
    } /* Endif */
#endif

    _int_disable();

    int_ptr = kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT];

    while (int_ptr && int_ptr->VEC_NUM != vector)
    {
        int_ptr = int_ptr->NEXT;
    }

    _int_enable();

    return (int_ptr) ? int_ptr->APP_ISR : _int_default_isr;

}

#endif /* MQX_SPARSE_ISR_TABLE */

/*!
 * \brief Retrieves a pointer of the interrupt handler data for the specified
 * vector.
 *
 * When MQX calls _int_kernel_isr() or an application ISR, it passes the data
 * as the first parameter to the ISR.
 * \nISR data can be modified with _int_set_isr_data().
 *
 * \param[in] vector Number of the vector whose ISR data are to be returned.
 *
 * \return Pointer to the ISR data.
 * \return NULL (Failure.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code.
 *
 * \see _int_get_isr
 * \see _int_install_isr
 * \see _int_set_isr_data
 */
#if !MQX_SPARSE_ISR_TABLE

pointer _int_get_isr_data
(
    _mqx_uint vector
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        return(NULL);
    }/* Endif */
#endif

    vector -= kernel_data->FIRST_USER_ISR_VECTOR;
    return(kernel_data->INTERRUPT_TABLE_PTR[vector].APP_ISR_DATA);

} /* Endbody */

#else /* MQX_SPARSE_ISR_TABLE */

pointer _int_get_isr_data
(
    _mqx_uint vector
)
{
    KERNEL_DATA_STRUCT_PTR          kernel_data;
    INTERRUPT_SPARSE_REC_STRUCT_PTR int_ptr;

    _GET_KERNEL_DATA(kernel_data);
#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        return(NULL);
    }/* Endif */
#endif

    _int_disable();

    int_ptr = kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT];

    while (int_ptr && int_ptr->VEC_NUM != vector)
    {
        int_ptr = int_ptr->NEXT;
    }

    _int_enable();

    return (int_ptr) ? int_ptr->APP_ISR_DATA : NULL;
}

#endif /* MQX_SPARSE_ISR_TABLE */

/*!
 * \brief Gets the depth of nesting of the current interrupt stack.
 *
 * \return 0 (An interrupt is not being serviced.)
 * \return 1 (A non-nested interrupt is being serviced.)
 * \return >=2 (A nested interrupt is being serviced.)
 *
 * \see _int_install_isr
 */
_mqx_uint _int_get_isr_depth(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return( kernel_data->IN_ISR );
} /* Endbody */

/*!
 * \brief Installs the provided function as the default ISR, called whenever an
 * unhandled interrupt occurs.
 *
 * MQX uses the application-provided default ISR for all interrupts for which
 * the application has not installed an application ISR. The routine handles all
 * unhandled and unexpected interrupts.
 *
 * \param[in] default_isr The new default ISR function.
 *
 * \return Pointer to previous default ISR which was installed before this function was called.
 *
 * \see _int_get_default_isr
 * \see _int_install_isr
 * \see _int_default_isr
 * \see _int_install_exception_isr
 * \see _int_install_unexpected_isr
 */
#if !MQX_SPARSE_ISR_TABLE

INT_ISR_FPTR _int_install_default_isr
(
    INT_ISR_FPTR default_isr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    INT_ISR_FPTR                old_default_isr;
    INTERRUPT_TABLE_STRUCT_PTR  int_table_ptr;
    _mqx_uint                   number;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_int_install_default_isr, default_isr);

    old_default_isr = kernel_data->DEFAULT_ISR;
    kernel_data->DEFAULT_ISR = default_isr;

    int_table_ptr = kernel_data->INTERRUPT_TABLE_PTR;
    if (int_table_ptr != NULL)
    {
        number = (kernel_data->LAST_USER_ISR_VECTOR -
                        kernel_data->FIRST_USER_ISR_VECTOR) + 1 + 1;

        while (--number)
        {
            if (int_table_ptr->APP_ISR == old_default_isr)
            {
                int_table_ptr->APP_ISR = default_isr;
            } /* Endif */
            ++int_table_ptr;
        } /* Endwhile */
    } /* Endif */

    _KLOGX2(KLOG_int_install_default_isr, old_default_isr);

    return(old_default_isr);

} /* Endbody */

#else /* MQX_SPARSE_ISR_TABLE */

INT_ISR_FPTR _int_install_default_isr
(
    INT_ISR_FPTR default_isr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    INT_ISR_FPTR           old_default_isr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_int_install_default_isr, default_isr);

    old_default_isr = kernel_data->DEFAULT_ISR;
    kernel_data->DEFAULT_ISR = default_isr;

    _KLOGX2(KLOG_int_install_default_isr, old_default_isr);

    return(old_default_isr);

} /* Endbody */

#endif /* MQX_SPARSE_ISR_TABLE */

/*!
 * \brief Installs the MQX-provided _int_exception_isr() as the default ISR for
 * unhandled interrupts and exceptions.
 *
 * The exception ISR handler performs the following service when unhandled interrupt occurs:
 * \n a) A task is running
 * \n - If the task has installed an exception handler, this handler is called
 * \n - Otherwise, the task is aborted (_task_abort)
 * \n b) An ISR is running
 * \n - If the ISR has an exception handler installed, then the exception handler
 * is called. Finally, both exception and ISR interrupt frames are removed.
 *
 * \return Pointer to the default exception handler before this function was called.
 *
 * \see _int_get_default_isr
 */
INT_ISR_FPTR _int_install_exception_isr
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    kernel_data->FLAGS |= MQX_FLAGS_EXCEPTION_HANDLER_INSTALLED;
    return(_int_install_default_isr(_int_exception_isr));

} /* Endbody */

/*!
 * \brief Installs the ISR.
 *
 * MQX catches all hardware interrupts in the range specified in _int_init and saves
 * the context of the active task. For these interrupts, the MQX calls the ISR that is
 * stored in the interrupt vector table at the location identified by its interrupt
 * vector number.
 * \n The application defines the ISR data, which can be any constant or pointer value.
 *
 * \param[in] vector   Vector number (not the offset) of the interrupt.
 * \param[in] isr_ptr  Pointer to the ISR
 * \param[in] isr_data Pointer to the data to be passed as the first parameter to
 * the ISR when an interrupt occurs and the ISR runs
 *
 * \return Pointer to the previous ISR installed for the vector before calling this function.
 * \return NULL (Failure.)
 *
 * \see _int_get_default_isr
 * \see _int_install_default_isr
 * \see _int_get_isr_data
 * \see _int_set_isr_data
 * \see _int_get_isr
 * \see _task_set_error
 */
#if !MQX_SPARSE_ISR_TABLE

INT_ISR_FPTR _int_install_isr
(
    _mqx_uint        vector,
    INT_ISR_FPTR     isr_ptr,
    pointer          isr_data
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    INTERRUPT_TABLE_STRUCT_PTR table_ptr;
    INT_ISR_FPTR               old_isr_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_int_install_isr, vector, isr_ptr, isr_data);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        _KLOGX2(KLOG_int_install_isr, NULL);
        return(NULL);
    }/* Endif */
#endif

    table_ptr = &kernel_data->INTERRUPT_TABLE_PTR[vector -
                                                  kernel_data->FIRST_USER_ISR_VECTOR];

    _int_disable();
    old_isr_ptr = table_ptr->APP_ISR;
    table_ptr->APP_ISR  = isr_ptr;
    table_ptr->APP_ISR_DATA = isr_data;
    _int_enable();

    _KLOGX2(KLOG_int_install_isr, old_isr_ptr);

    return (old_isr_ptr);

} /* Endbody */

#else /* MQX_SPARSE_ISR_TABLE */

INT_ISR_FPTR _int_install_isr
(
    _mqx_uint        vector,
    INT_ISR_FPTR     isr_ptr,
    pointer          isr_data
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR          kernel_data;
    INTERRUPT_SPARSE_REC_STRUCT_PTR int_ptr;
    INT_ISR_FPTR                    old_isr_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_int_install_isr, vector, isr_ptr, isr_data);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        _KLOGX2(KLOG_int_install_isr, NULL);
        return(NULL);
    }/* Endif */
#endif

    _int_disable();

    int_ptr = kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT];

    while (int_ptr && int_ptr->VEC_NUM != vector)
    {
        int_ptr = int_ptr->NEXT;
    }

    if (!int_ptr)
    {
#if MQX_LITE_VERSION_NUMBER
        int_ptr = &sparse_struct;
#else
        int_ptr = _mem_alloc_system(sizeof(INTERRUPT_SPARSE_REC_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (int_ptr == NULL)
        {
            _int_enable();
            _task_set_error(MQX_OUT_OF_MEMORY);
            _KLOGX2(KLOG_int_install_isr, NULL);
            return(NULL);
        }
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */
        _mem_set_type(int_ptr, MEM_TYPE_INTERRUPT_VECTOR);
#endif /* MQX_LITE_VERSION_NUMBER */

        int_ptr->NEXT = kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT];
        kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT] = int_ptr;

        old_isr_ptr = _int_default_isr;
    }
    else
        old_isr_ptr = int_ptr->APP_ISR;

    int_ptr->VEC_NUM = vector;
    int_ptr->APP_ISR  = isr_ptr;
    int_ptr->APP_ISR_DATA = isr_data;

    _int_enable();

    _KLOGX2(KLOG_int_install_isr, old_isr_ptr);

    return (old_isr_ptr);

} /* Endbody */

#endif /* MQX_SPARSE_ISR_TABLE */

/*!
 * \brief Installs the MQX-provided unexpected ISR, _int_unexpected_isr(), for all
 * interrupts that do not have an application-installed ISR.
 *
 * The unexpected ISR handler writes the cause of the unexpected interrupt to the standard
 * I/O stream.
 *
 * \return Pointer to the previous unexpected interrupt ISR before this function was called.
 *
 * \see _int_install_exception_isr
 * \see _int_unexpected_isr
 */
INT_ISR_FPTR _int_install_unexpected_isr
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    kernel_data->FLAGS &= ~MQX_FLAGS_EXCEPTION_HANDLER_INSTALLED;

    return(_int_install_default_isr(_int_unexpected_isr));

} /* Endbody */

/*!
 * \brief Sets the ISR exception handler for the interrupt vector.
 *
 * This function sets the exception handler for an ISR. When an exception (unhandled
 * interrupt) occurs while the ISR is running, MQX calls the exception handler
 * and terminates the ISR.
 * \n In order to make use of exceptions, an application should install
 *  _int_exception_isr() as the MQX default ISR.
 * \n The returned exception handler is either the default handler or one that the
 * application previously installed with _int_set_exception_handler().
 *
 * \param[in] vector                Interrupt vector that this exception handler is for.
 * \param[in] error_handler_address Pointer to the exception handler.
 *
 * \return Pointer to the previous exception handler installed for the vector before
 *  this function was called.
 * \return NULL (Failure.)
 *
 * \warning On failure, the exception handler is not installed and _task_set_error()
 * is called to set the task error code.
 *
 * \see _int_get_exception_handler
 * \see _int_exception_isr
 * \see _task_set_error
 */
#if !MQX_SPARSE_ISR_TABLE

INT_EXCEPTION_FPTR _int_set_exception_handler
(
    _mqx_uint              vector,
    INT_EXCEPTION_FPTR error_handler_address
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    INT_EXCEPTION_FPTR old_handler = NULL;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_int_set_exception_handler, vector, error_handler_address);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX2(KLOG_int_set_exception_handler, NULL);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        _KLOGX2(KLOG_int_set_exception_handler, NULL);
        return(NULL);
    } /* Endif */
#endif

    vector -= kernel_data->FIRST_USER_ISR_VECTOR;

    old_handler = kernel_data->INTERRUPT_TABLE_PTR[vector].APP_ISR_EXCEPTION_HANDLER;
    kernel_data->INTERRUPT_TABLE_PTR[vector].APP_ISR_EXCEPTION_HANDLER = error_handler_address;

    _KLOGX2(KLOG_int_set_exception_handler, old_handler);
    return(old_handler);
} /* Endbody */

#else /* MQX_SPARSE_ISR_TABLE */

INT_EXCEPTION_FPTR _int_set_exception_handler
(
    _mqx_uint              vector,
    INT_EXCEPTION_FPTR error_handler_address
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    INT_EXCEPTION_FPTR old_handler = NULL;
    INTERRUPT_SPARSE_REC_STRUCT_PTR int_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_int_set_exception_handler, vector, error_handler_address);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX2(KLOG_int_set_exception_handler, NULL);
        return (NULL);
    }

    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        _KLOGX2(KLOG_int_set_exception_handler, NULL);
        return (NULL);
    }
#endif

    _int_disable();

    int_ptr = kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT];

    while (int_ptr && int_ptr->VEC_NUM != vector)
    {
        int_ptr = int_ptr->NEXT;
    }

    if (!int_ptr)
    {
#if MQX_LITE_VERSION_NUMBER
        int_ptr = &sparse_struct;
#else
        int_ptr = _mem_alloc_system(sizeof(INTERRUPT_SPARSE_REC_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (int_ptr == NULL)
        {
            _int_enable();
            _task_set_error(MQX_OUT_OF_MEMORY);
            _KLOGX2(KLOG_int_install_isr, NULL);
            return(NULL);
        }
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */
        _mem_set_type(int_ptr, MEM_TYPE_INTERRUPT_VECTOR);
#endif /* MQX_LITE_VERSION_NUMBER */

        int_ptr->NEXT = kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT];
        kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT] = int_ptr;

        old_handler = NULL;
    }
    else
        old_handler = int_ptr->APP_ISR_EXCEPTION_HANDLER;


    int_ptr->VEC_NUM = vector;
    int_ptr->APP_ISR_EXCEPTION_HANDLER  = error_handler_address;

    _int_enable();

    _KLOGX2(KLOG_int_set_exception_handler, old_handler);
    return (old_handler);
}

#endif /* MQX_SPARSE_ISR_TABLE */

/*!
 * \brief Sets the address of the interrupt handler data for the specified vector,
 * and returns the old value.
 *
 * \param[in] vector The interrupt vector that this data is for.
 * \param[in] data   Data that MQX passes to the ISR as its first parameter.
 *
 * \return Previous ISR data registred before this function was called.
 * \return NULL (Failure.)
 *
 * \warning On failure, calls _task_set_error() to set the task error code.
 *
 * \see _int_get_isr
 * \see _int_get_isr_data
 */
#if !MQX_SPARSE_ISR_TABLE

pointer _int_set_isr_data
(
    _mqx_uint vector,
    pointer data
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                old_data;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_int_set_isr_data,vector,data);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX2(KLOG_int_set_isr_data,NULL);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        _KLOGX2(KLOG_int_set_isr_data,NULL);
        return(NULL);
    } /* Endif */
#endif

    vector -= (kernel_data->FIRST_USER_ISR_VECTOR);

    old_data = kernel_data->INTERRUPT_TABLE_PTR[vector].APP_ISR_DATA;
    kernel_data->INTERRUPT_TABLE_PTR[vector].APP_ISR_DATA = data;

    _KLOGX2(KLOG_int_set_isr_data,old_data);
    return old_data;

} /* Endbody */

#else /* MQX_SPARSE_ISR_TABLE */

pointer _int_set_isr_data
(
    _mqx_uint vector,
    pointer data
)
{
    KERNEL_DATA_STRUCT_PTR          kernel_data;
    pointer                         old_data = NULL;
    INTERRUPT_SPARSE_REC_STRUCT_PTR int_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_int_set_isr_data,vector,data);

#if MQX_CHECK_ERRORS
    if ( kernel_data->INTERRUPT_TABLE_PTR == NULL )
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX2(KLOG_int_set_isr_data,NULL);
        return(NULL);
    } /* Endif */
    if ((vector < kernel_data->FIRST_USER_ISR_VECTOR) ||
        (vector > kernel_data->LAST_USER_ISR_VECTOR))
    {
        _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
        _KLOGX2(KLOG_int_set_isr_data,NULL);
        return(NULL);
    } /* Endif */
#endif

    _int_disable();

    int_ptr = kernel_data->INTERRUPT_TABLE_PTR[(vector - kernel_data->FIRST_USER_ISR_VECTOR) >> MQX_SPARSE_ISR_SHIFT];

    while (int_ptr && int_ptr->VEC_NUM != vector)
    {
        int_ptr = int_ptr->NEXT;
    }

    if (int_ptr)
    {
        old_data = int_ptr->APP_ISR_DATA;
        int_ptr->APP_ISR_DATA = data;
    }

    _int_enable();

    _KLOGX2(KLOG_int_set_isr_data,old_data);
    return old_data;
}

#endif /* MQX_SPARSE_ISR_TABLE */

/*!
 * \brief This function initializes the kernel interrupt table.
 *
 * This function initializes the kernel interrupt table and install default interrupt
 * handler to all interrupt sources. This function is typically called very early
 * during system startup.
 *
 * \param[in] first_user_isr_vector_number The first (lower) user ISR vector number.
 * \param[in] last_user_isr_vector_number  The last user ISR vector number.
 *
 * \return MQX_OK (Success.)
 * \return MQX_INVALID_PARAMETER (first_user_isr_vector_number is greater than
 * last_user_isr_vector_number.)
 * \return MQX_OUT_OF_MEMORY (Not enough free memory for the interrupt table.)
 */
_mqx_uint _int_init
(
    _mqx_uint          first_user_isr_vector_number,
    _mqx_uint          last_user_isr_vector_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    INTERRUPT_TABLE_STRUCT_PTR int_table_ptr;
    _mqx_uint                  number;

#if MQX_CHECK_ERRORS
    if (last_user_isr_vector_number < first_user_isr_vector_number)
    {
        return MQX_INVALID_PARAMETER;
    } /* Endif */
#endif

    _GET_KERNEL_DATA(kernel_data);
    kernel_data->INT_KERNEL_ISR_ADDR = _int_kernel_isr;

    /* Set the current default ISR for MQX that is called whenever an
     * unhandled interrupt occurs
     */
    kernel_data->DEFAULT_ISR = _int_default_isr;

#if !MQX_SPARSE_ISR_TABLE
    number = last_user_isr_vector_number - first_user_isr_vector_number + 1;
#else
    number = ((last_user_isr_vector_number - first_user_isr_vector_number + 1) >> MQX_SPARSE_ISR_SHIFT) + 1;
#endif

#ifdef MQX_LITE_VERSION_NUMBER
    int_table_ptr = mqx_static_isr_table;
#else
    int_table_ptr = _mem_alloc_system_zero((_mem_size)(sizeof(INTERRUPT_TABLE_STRUCT) * number));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if ( int_table_ptr == NULL )
    {
        return(MQX_OUT_OF_MEMORY);
    }/* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */

    _mem_set_type(int_table_ptr, MEM_TYPE_INTERRUPT_TABLE);
#endif /* MQX_LITE_VERSION_NUMBER */

    kernel_data->INTERRUPT_TABLE_PTR   = int_table_ptr;
    kernel_data->FIRST_USER_ISR_VECTOR = first_user_isr_vector_number;
    kernel_data->LAST_USER_ISR_VECTOR  = last_user_isr_vector_number;

#if !MQX_SPARSE_ISR_TABLE
    while (number--)
    {
        int_table_ptr->APP_ISR      = _int_default_isr;
        int_table_ptr->APP_ISR_DATA = (pointer)(first_user_isr_vector_number++);
        ++int_table_ptr;
    } /* Endwhile */
#else /* MQX_SPARSE_ISR_TABLE */

    kernel_data->SPARSE_ISR_COUNT = number;
    kernel_data->SPARSE_ISR_SHIFT = MQX_SPARSE_ISR_SHIFT;

    while (number--)
    {
        int_table_ptr[number] = NULL;
    }

#endif /* MQX_SPARSE_ISR_TABLE */

    return MQX_OK;

} /* Endbody */

#endif /* MQX_USE_INTERRUPTS */

/* EOF */
