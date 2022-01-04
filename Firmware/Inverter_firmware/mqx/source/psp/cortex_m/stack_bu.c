/**HEADER********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor
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
* $FileName: stack_bu.c$
* $Version : 3.8.11.0$
* $Date    : Dec-20-2011$
*
* Comments:
*   This file contains the functions for manipulating the user
*	context on the stack.
*
*END************************************************************************/

#include "mqx_inc.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _psp_build_stack_frame
* Returned Value   : none
* Comments         :
*
*   This function sets up the stack frame of a new task descriptor.
*
*END*----------------------------------------------------------------------*/

boolean _psp_build_stack_frame
   (
      /* [IN] the address of the task descriptor */
      TD_STRUCT_PTR    td_ptr,

      /* [IN] the address of the stack memory block */
      pointer          stack_ptr,

      /* [IN] the size of the stack */
      _mem_size        stack_size,

      /* [IN] the task's template */
      TASK_TEMPLATE_STRUCT_PTR template_ptr,

      /* [IN] the status register to use in creating the task */
      _mqx_uint        status_register,

      /* [IN] the task creation parameter */
      uint_32          create_parameter
   )
{
   uchar_ptr stack_base_ptr;
   PSP_STACK_START_STRUCT_PTR stack_start_ptr;
   boolean res = TRUE;

   stack_base_ptr  = (uchar_ptr)_GET_STACK_BASE(stack_ptr, stack_size);
   stack_start_ptr = (PSP_STACK_START_STRUCT_PTR)(stack_base_ptr - sizeof(PSP_STACK_START_STRUCT));

   td_ptr->STACK_BASE  = (pointer)stack_base_ptr;
   td_ptr->STACK_LIMIT = _GET_STACK_LIMIT(stack_ptr, stack_size);
   td_ptr->STACK_PTR   = stack_start_ptr;

   /*
   ** Build the task's initial stack frame. This contains the initialized
   ** registers, and an exception frame which will cause the task to
   ** "return" to the start of the task when it is dispatched.
   */
   _mem_zero(stack_start_ptr, (_mem_size)sizeof(PSP_STACK_START_STRUCT));
   stack_start_ptr->INITIAL_CONTEXT.LR = (uint_32)_task_exit_function_internal;
   stack_start_ptr->INITIAL_CONTEXT.R0 = (uint_32)create_parameter;
   stack_start_ptr->INITIAL_CONTEXT.PC = (uint_32)(template_ptr->TASK_ADDRESS) | 1;
   stack_start_ptr->INITIAL_CONTEXT.PSR = 0x01000000;
   stack_start_ptr->PARAMETER = create_parameter;
#if PSP_MQX_CPU_IS_ARM_CORTEX_M4
   stack_start_ptr->INITIAL_CONTEXT.PENDSVPRIOR = 0;
   stack_start_ptr->INITIAL_CONTEXT.BASEPRI     = status_register;
   stack_start_ptr->INITIAL_CONTEXT.LR2         = 0xfffffffd;
#endif


#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
   if (td_ptr->FLAGS & MQX_FLOATING_POINT_TASK) {
      res = _psp_build_float_context(td_ptr);
   }
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */

   return res;
}

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _psp_build_float_context
* Returned Value   : none
* Comments         :
*   This function sets up the FLOATING POINT context of a
*   new task descriptor.
*
*END*----------------------------------------------------------------------*/
boolean _psp_build_float_context
   (
      /* [IN] the address of the task descriptor */
      TD_STRUCT_PTR    td_ptr
   )
{
    PSP_BLOCKED_FP_STRUCT_PTR fp_ptr;

    /* Allocate space for saving/restoring the DSP registers */
    fp_ptr = (PSP_BLOCKED_FP_STRUCT_PTR)_mem_alloc_zero((_mem_size)sizeof(PSP_BLOCKED_FP_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (!fp_ptr) {
        /* Couldn't allocate memory for the DSP register context */
        _task_set_error_td_internal(td_ptr, MQX_OUT_OF_MEMORY);
        return FALSE;
    }
#endif

    _mem_set_type(fp_ptr, MEM_TYPE_FP_CONTEXT);
    /*
    ** Transfer the block to the task being created. This will ensure the
    ** float context will be freed if the task is destroyed.
    */
    _mem_transfer_internal((pointer)fp_ptr, td_ptr);

    /* This field should never be overwitten */
    fp_ptr->TID = td_ptr->TASK_ID;

    td_ptr->FLOAT_CONTEXT_PTR = (pointer)fp_ptr;

    return TRUE;
}
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */
