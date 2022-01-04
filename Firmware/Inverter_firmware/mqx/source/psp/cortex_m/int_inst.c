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
* $FileName: int_inst.c$
* $Version : 3.7.6.0$
* $Date    : Mar-17-2011$
*
* Comments:
*
*   This file contains the function for initializing the handling of
*   interrupts.
*
*END************************************************************************/

#include "mqx_inc.h"

/*!
 * \brief This function initializes kernel interrupt tables.
 */
#if MQX_USE_INTERRUPTS

void _psp_int_install(void) {
    KERNEL_DATA_STRUCT_PTR kernel_data;
#if !MQX_ROM_VECTORS
   uint_32                    i;
   uint_32_ptr                ptr;
   extern uint_32 ram_vector[];
#endif

    _GET_KERNEL_DATA(kernel_data);
    __set_MSP((uint_32)kernel_data->INTERRUPT_STACK_PTR);

#if !MQX_ROM_VECTORS
    ptr = (uint_32_ptr)ram_vector;

    /* Initialize the hardware interrupt vector table */
    // 0: Initial stack pointer
    // 1: Initial program counter
    for (i = 16; i < PSP_MAXIMUM_INTERRUPT_VECTORS; i++) {
        ptr[i] = (uint_32)_int_kernel_isr;
    }
#endif
}

#endif /* MQX_USE_INTERRUPTS */
