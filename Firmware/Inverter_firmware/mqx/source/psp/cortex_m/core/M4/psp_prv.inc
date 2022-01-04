/*HEADER***********************************************************************
*
* Copyright (c) 2010-2011 Freescale Semiconductor
* All Rights Reserved
*
*******************************************************************************
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
*******************************************************************************
*
* $FileName: psp_prv.inc$
* $Version : 3.8.12.0$
* $Date    : Sep-19-2011$
*
* Comments:
*   This assembler header file contains private declarations for
*   use with the mqx assembler files
*
*END***************************************************************************/

#include "asm_mac.h"
#include "mqx_cnfg.h"

/*****************************************************************************
 *                         EXTERNAL REFERENCES
 *****************************************************************************/

#if MQX_KERNEL_LOGGING
        ASM_EXTERN(_klog_block_internal)
        ASM_EXTERN(_klog_execute_scheduler_internal)
        ASM_EXTERN(_klog_yield_internal)
        ASM_EXTERN(_klog_context_switch_internal)
        ASM_EXTERN(_klog_isr_start_internal)
        ASM_EXTERN(_klog_isr_end_internal)
#endif
        ASM_EXTERN(_mqx_kernel_data)
        ASM_EXTERN(_mqx_system_stack)

        ASM_EXTERN(_psp_irq_sp)
        ASM_EXTERN(_psp_fiq_sp)
        ASM_EXTERN(_psp_undef_sp)
        ASM_EXTERN(_psp_abort_sp)

/*****************************************************************************
 *                       CONSTANT DECLARATIONS
 *
 * The following are the bits in the CONFIG field of the kernel data structure
 * to set for the psp options.
 * NOTE: These must agree with values in mqx_prv.h or mqxlite_prv.h
 *
 *****************************************************************************/

ASM_EQUATE(PSP_CNFG_MQX_KERNEL_LOGGING, 0x8000)
ASM_EQUATE(PSP_CNFG_FP_REGISTERS_EXIST, 0x4000)

#ifndef MQX_FP_REGISTERS_EXIST
#define MQX_FP_REGISTERS_EXIST 0
#endif

#if MQX_KERNEL_LOGGING == 1
#if MQX_FP_REGISTERS_EXIST == 1
ASM_EQUATE(PSP_CNFG, (PSP_CNFG_MQX_KERNEL_LOGGING | PSP_CNFG_FP_REGISTERS_EXIST))
#else
ASM_EQUATE(PSP_CNFG, PSP_CNFG_MQX_KERNEL_LOGGING)
#endif
#else
#if MQX_FP_REGISTERS_EXIST == 1
ASM_EQUATE(PSP_CNFG, PSP_CNFG_FP_REGISTERS_EXIST)
#else
ASM_EQUATE(PSP_CNFG, 0)
#endif
#endif


/*
 * Task FLAGS bits
 * These must match definitions in mqx_prv.h
 */

ASM_EQUATE(FP_TASK_MASK, 0x0002)
ASM_EQUATE(FP_CONTEXT_SAVED_MASK, 0x0200)
ASM_EQUATE(FP_CONTEXT_CLEAR_MASK, 0xfdff)
ASM_EQUATE(PREEMPTION_DISABLED, 0x1000)

/*****************************************************************************
 *                      MACRO DECLARATIONS
 *****************************************************************************/
#ifdef __IASMARM__ /* IAR */

/* This macro returns the address of the kernel data in the specified register */
GET_KERNEL_DATA MACRO reg
        ldr reg, =ASM_PREFIX(_mqx_kernel_data)
        ldr reg, [reg, #0]
        ENDM

SET_FUNCTION_ALIGNMENT MACRO
                ALIGNROM 2
        ENDM

/* This macro calls the kernel logging function, if logging enabled */
KLOG    MACRO KDATA, KLOG_FUNCTION
#if MQX_KERNEL_LOGGING
        LOCAL klog_end
        push {r1}
        ldr r1, [KDATA, #KD_LOG_CONTROL]
        tst r1, #1
        beq klog_end

        push {lr}
        bl KLOG_FUNCTION
        pop {lr}
klog_end:
        pop {r1}
#endif
        ENDM

#elif defined __CC_ARM /* Keil */

/* This macro returns the address of the kernel data in the specified register */
        MACRO
        GET_KERNEL_DATA $reg
        ldr $reg, =ASM_PREFIX(_mqx_kernel_data)
        ldr $reg, [$reg, #0]
        MEND

        MACRO
        SET_FUNCTION_ALIGNMENT
        ALIGN 2
        MEND

        MACRO
        KLOG $KDATA, $KLOG_FUNCTION
#if MQX_KERNEL_LOGGING
        push {r1}
        ldr r1, [$KDATA, #KD_LOG_CONTROL]
        tst r1, #1
        beq %FT1

        push {lr}
        bl $KLOG_FUNCTION
        pop {lr}
1
        pop {r1}
#endif
    MEND

#elif defined(__GNUC__)

/* This macro returns the address of the kernel data in the specified register */
GET_KERNEL_DATA: .macro reg
        ldr \reg, =ASM_PREFIX(_mqx_kernel_data)
        ldr \reg, [\reg, #0]
        .endm
 
SET_FUNCTION_ALIGNMENT: .macro
        .align 2
        .endm

/* This macro calls the kernel logging function, if logging enabled */
KLOG:   .macro KDATA, KLOG_FUNCTION
        .if MQX_KERNEL_LOGGING
        push {r1}
        ldr r1, [\KDATA, #KD_LOG_CONTROL]
        tst r1, #1
        beq klog_end\@

        push {lr}
        bl \KLOG_FUNCTION
        pop {lr}
klog_end\@:
        pop {r1}
        .endif
        .endm

#else  /* CW */

/* This macro returns the address of the kernel data in the specified register */
GET_KERNEL_DATA: .macro reg
        ldr reg, =ASM_PREFIX(_mqx_kernel_data)
        ldr reg, [reg, #0]
        .endm

SET_FUNCTION_ALIGNMENT: .macro
        .align 2
        .endm

/* This macro calls the kernel logging function, if logging enabled */
KLOG:   .macro KDATA,KLOG_FUNCTION
        .if MQX_KERNEL_LOGGING
        push {r1}
        ldr r1, [KDATA, #KD_LOG_CONTROL]
        tst r1, #1
        beq klog_end\@

        push {lr}
        bl KLOG_FUNCTION
        pop {lr}
klog_end\@:
        pop {r1}
        .endif
        .endm

#endif
