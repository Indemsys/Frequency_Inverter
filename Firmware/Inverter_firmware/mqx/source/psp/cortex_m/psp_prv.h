#ifndef __psp_prv_h__
#define __psp_prv_h__
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
* $FileName: psp_prv.h$
* $Version : 3.8.8.0$
* $Date    : Jun-7-2012$
*
* Comments:
*   This file contains psp private declarations for use when compiling
*   the kernel.
*
*END************************************************************************/

#include "mqx_prv.h"

/* This macro modifies the context of a blocked task so that the
** task will execute the provided function when it next runs
*/  
#define _PSP_SET_PC_OF_BLOCKED_TASK(stack_ptr, func)    \
    ((PSP_BLOCKED_STACK_STRUCT_PTR)(stack_ptr))->PC = ((uint_32)(func))

/* This macro modifies the interrupt priority of a blocked task so that the
** task will execute at the correct interrupt priority when it restarts
*/  
#define _PSP_SET_SR_OF_BLOCKED_TASK(stack_ptr, sr_value)

/* This macro modifies the context of a task that has been interrupted 
** so that the task will execute the provided function when the isr returns
*/  
#define _PSP_SET_PC_OF_INTERRUPTED_TASK(stack_ptr, func)        ((PSP_BASIC_INT_FRAME_STRUCT_PTR)(__get_PSP()))->PC = (uint_32)(func)

/* This macro obtains the address of the kernel data structure */
#define _GET_KERNEL_DATA(x)     x = _mqx_kernel_data

#define _SET_KERNEL_DATA(x)     _mqx_kernel_data = (struct kernel_data_struct _PTR_)(x)

#define _PSP_GET_CALLING_PC()   (0)

/*--------------------------------------------------------------------------*/
/*
**                PROTOTYPES OF PRIVATE PSP FUNCTIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

/* ARM PSP specific prototypes */
void _psp_exception_return(pointer);
void _psp_setup_int_mode_stack(pointer,uint_32,uint_32);
uint_32 _psp_get_int_mode_stack(uint_32);
uint_32 _psp_get_int_mode_lr(uint_32);

boolean _psp_build_float_context(TD_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
