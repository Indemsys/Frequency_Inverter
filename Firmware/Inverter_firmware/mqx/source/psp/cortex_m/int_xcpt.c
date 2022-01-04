/*HEADER*********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: int_xcpt.c$
* $Version : 3.8.5.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   This file contains the isr that handles exceptions.
*
*END************************************************************************/

#include "mqx_inc.h"


#if MQX_USE_INTERRUPTS
/*!
 * \brief To provide support for exception handlers, applications can use this ISR
 * to replace the default ISR. The ISR is specific to the PSP.
 * 
 * An application calls _int_install_exception_isr() to install _int_exception_isr().
 * \n The function _int_exception_isr() does the following:
 * \li If an exception occurs when a task is running and a task exception ISR 
 * exists, MQX runs the ISR; if a task exception ISR does not exist, MQX aborts 
 * the task by calling _task_abort().
 * \li If an exception occurs when an ISR is running and an ISR exception ISR 
 * exists, MQX aborts the running ISR and runs the ISR’s exception ISR.
 * \li The function walks the interrupt stack looking for information about the 
 * ISR or task that was running before the exception occurred. If the function 
 * determines that the interrupt stack contains incorrect information, it calls 
 * _mqx_fatal_error() with error code MQX_CORRUPT_INTERRUPT_STACK.
 * 
 * \param[in] parameter Parameter passed to the default ISR (the vector number).
 * 
 * \warning See description.
 * 
 * \see _int_install_exception_isr
 * \see _mqx_fatal_error
 * \see _task_abort     
 */   
void _int_exception_isr
   (
      pointer parameter
   )
{ /* Body */
    KERNEL_DATA_STRUCT_PTR         kernel_data;
    TD_STRUCT_PTR                  td_ptr;
    PSP_INT_CONTEXT_STRUCT_PTR     exception_frame_ptr;
    PSP_INT_CONTEXT_STRUCT_PTR     isr_frame_ptr;
    INTERRUPT_TABLE_STRUCT_PTR     table_ptr;
    INT_EXCEPTION_FPTR             exception_handler;
    uint_32                        isr_vector;
/*    uint_32                        exception_vector; */

    _GET_KERNEL_DATA(kernel_data);
    td_ptr = kernel_data->ACTIVE_PTR;

    /* Stop all interrupts */
    _PSP_SET_DISABLE_SR(kernel_data->DISABLE_SR);
    /*_int_disable(); */

    if ( kernel_data->IN_ISR > 1 ) {
        /* We have entered this function from an exception that happened
         * while an isr was running.
         */
    
        /* Get our current exception frame */
        exception_frame_ptr = kernel_data->INTERRUPT_CONTEXT_PTR;

        /* the current context contains a pointer to the next one */
        isr_frame_ptr = (PSP_INT_CONTEXT_STRUCT_PTR)exception_frame_ptr->PREV_CONTEXT;
        if (isr_frame_ptr == NULL) {
            /* This is not allowable */
            _mqx_fatal_error(MQX_CORRUPT_INTERRUPT_STACK);
        }
        
        isr_vector =  isr_frame_ptr->EXCEPTION_NUMBER;

        /* Call the isr exception handler for the ISR that WAS running */
        table_ptr = kernel_data->INTERRUPT_TABLE_PTR;
#if MQX_CHECK_ERRORS
        if ((table_ptr != NULL) &&
            (isr_vector >= kernel_data->FIRST_USER_ISR_VECTOR) &&
            (isr_vector <= kernel_data->LAST_USER_ISR_VECTOR))
        {
#endif
        /* Call the exception handler for the isr on isr_vector,
         * passing the isr_vector, the exception_vector, the isr_data and
         * the basic frame pointer for the exception
         */
        exception_handler = _int_get_exception_handler(isr_vector);
   
        if (exception_handler) {
            (*exception_handler)(isr_vector, (_mqx_uint)parameter, _int_get_isr_data(isr_vector)/*table_ptr->APP_ISR_DATA*/, exception_frame_ptr);
        }
    
#if MQX_CHECK_ERRORS
        } else {
            /* In this case, the exception occured in this handler */
            _mqx_fatal_error(MQX_INVALID_VECTORED_INTERRUPT);
        }
#endif

        /* Indicate we have popped 1 interrupt stack frame (the exception frame) */
        --kernel_data->IN_ISR;

        /* Reset the stack to point to the interrupt frame */
        /* And off we go. Will never return */
        _psp_exception_return( (pointer)isr_frame_ptr );

    } else {
        /* We have entered this function from an exception that happened
         * while a task was running.
         */

        if (td_ptr->EXCEPTION_HANDLER_PTR != NULL ) {
            (*td_ptr->EXCEPTION_HANDLER_PTR)((_mqx_uint)parameter, 
            td_ptr->STACK_PTR);
        } else {
            /* Abort the current task */
            _task_abort(MQX_NULL_TASK_ID);
        }
   }
}

#endif

