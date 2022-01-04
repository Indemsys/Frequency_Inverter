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
* $FileName: int_gkis.c$
* $Version : 3.8.4.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   This file contains the functions for returning the kernel isr for an interrupt.
*
*END************************************************************************/

#include "mqx_inc.h"

extern uint_32 __ICFEDIT_region_ROM_start__;
extern uint_32 __ICFEDIT_region_RAM_start__;
/*!
 * \brief Gets a pointer to the kernel ISR for the specified vector number. The 
 * kernel ISR depends on the PSP.
 *  
 * \param[in] vector Vector number whose kernel ISR is requested.
 * 
 * \return Pointer to the kernel ISR (Success.)
 * \return NULL   
 * 
 * \warning On failure, calls _task_set_error() to set the task error code:
 * \li MQX_INVALID_VECTORED_INTERRUPT 
 * 
 * \see _int_kernel_isr
 * \see _int_install_kernel_isr
 */       
INT_KERNEL_ISR_FPTR _int_get_kernel_isr
(
    /* [IN] the vector number whose kernel ISR is wanted */
    uint_32 vector

)
{ /* Body */
   uint_32     vtor = _PSP_GET_VTOR(); //value of Vector Table Offset Register

#if MQX_CHECK_ERRORS
   if ( vector >= PSP_MAXIMUM_INTERRUPT_VECTORS ) {
      _task_set_error(MQX_INVALID_VECTORED_INTERRUPT);
      return NULL;
   } /* Endif */
#endif

   /* Note that VTOR bit indicating SRAM / ROM location is just a SRAM / ROM
   ** base address within Cortex memory map
   */
   return (INT_KERNEL_ISR_FPTR) (vtor + 4 * vector);

} /* Endbody */

/* EOF */
