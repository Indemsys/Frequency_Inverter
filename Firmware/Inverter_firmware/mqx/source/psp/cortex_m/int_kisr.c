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
* $FileName: int_kisr.c$
* $Version : 3.7.4.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the function for installing a kernel level isr.
*
*END************************************************************************/

#include "mqx_inc.h"

/*!
 * \brief Installs the kernel ISR handler. The kernel ISR depends on the PSP.
 *
 * Some real-time applications need special event handling to occur outside the
 * scope of MQX. The need might arise that the latency in servicing an interrupt
 * be less than the MQX interrupt latency. If this is the case, an application can
 * use _int_install_kernel_isr() to bypass MQX and let the interrupt be serviced
 * immediately.
 * \n Because the function returns the previous kernel ISR, applications can
 * temporarily install an ISR or chain ISRs so that each new one calls the one
 * installed before it.
 * \n A kernel ISR must save the registers that it needs and must service the
 * hardware interrupt. When the kernel ISR is finished, it must restore the
 * registers and perform a return-from-interrupt instruction.
 * \n A kernel ISR cannot call MQX functions. However, it can put data in global
 * data, which a task can access.
 *
 * \note The function is not available for all PSPs.
 *
 * \param[in] vector  Vector where the ISR is to be installed.
 * \param[in] isr_ptr Pointer to the ISR to install into the vector table.
 *
 * \return Pointer to the previous kernel ISR for the vector (Success.).
 * \return NULL
 *
 * \see _int_kernel_isr
 * \see _int_get_kernel_isr
 */
INT_KERNEL_ISR_FPTR _int_install_kernel_isr
(
    uint_32             vector,
    INT_KERNEL_ISR_FPTR isr_ptr
)
{
#if !MQX_ROM_VECTORS

#if MQX_KERNEL_LOGGING
   KERNEL_DATA_STRUCT_PTR kernel_data;
#endif
   INT_KERNEL_ISR_FPTR    old_isr_ptr;
   uint_32                result_code;
   uint_32_ptr            loc_ptr;

#if MQX_KERNEL_LOGGING
   _GET_KERNEL_DATA(kernel_data);
#endif

   _KLOGE3(KLOG_int_install_kernel_isr, vector, isr_ptr);

#if MQX_CHECK_ERRORS
   result_code = MQX_OK;
   old_isr_ptr = NULL;

   if ( vector >= PSP_MAXIMUM_INTERRUPT_VECTORS ) {
      result_code = MQX_INVALID_VECTORED_INTERRUPT;
   } else {
#endif

   loc_ptr = (uint_32_ptr)_int_get_vector_table();
   old_isr_ptr = (INT_KERNEL_ISR_FPTR)loc_ptr[vector];
   loc_ptr[vector] = (uint_32)isr_ptr;

#if MQX_CHECK_ERRORS
   } /* Endif */

   /* Set result code and return result. */
   _task_set_error(result_code);
#endif

   _KLOGX3(KLOG_int_install_kernel_isr, old_isr_ptr, result_code);
   return (old_isr_ptr);
#else

#if MQX_CHECK_ERRORS
   /* Set result code and return result. */
   _task_set_error(MQX_INVALID_CONFIGURATION);
#endif

   return NULL;
#endif
}
