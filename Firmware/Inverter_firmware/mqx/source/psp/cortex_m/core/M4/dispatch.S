/*HEADER********************************************************************
*
* Copyright (c) 2010-2011 Freescale Semiconductor;
* All Rights Reserved
*
****************************************************************************
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
***************************************************************************
*
* $FileName: dispatch.S$
* $Version : 3.8.38.0$
* $Date    : Oct-5-2012$
*
* Comments:
*
*   This assembler file contains functions for task scheduling
*
*END***********************************************************************/

#include <asm_mac.h>

#include "mqx_cnfg.h"
#include "types.inc"
#include "psp_prv.inc"

#define __ASM__
#include <psp_cpu.h>
#include "mqx_prv.h"
#undef __ASM__


ASM_EQUATE(SVC_RUN_SCHED, 1)
ASM_EQUATE(SVC_TASK_BLOCK, 2)
ASM_EQUATE(SVC_TASK_SWITCH, 3)
ASM_EQUATE(SVC_MQX_FN, 0xaa)

 ASM_EXTERN(_mqx_kernel_data)
 ASM_EXTERN(_klog_isr_start_internal)
 ASM_EXTERN(_klog_isr_end_internal)
 ASM_EXTERN(_mqx_api_call_handler)

 ASM_CODE_SECTION(KERNEL)
 SET_FUNCTION_ALIGNMENT
 ASM_COMP_SPECIFIC_DIRECTIVES

 ASM_PUBLIC(_sched_start_internal)
 ASM_PUBLIC(_sched_run_internal)
 ASM_PUBLIC(_sched_check_scheduler_internal)
 ASM_PUBLIC(_sched_execute_scheduler_internal)

 ASM_PUBLIC(_task_block)
 ASM_PUBLIC(_svc_handler)
 ASM_PUBLIC(_pend_svc)
 ASM_PUBLIC(_int_kernel_isr)

 ASM_PUBLIC(_mqx_api_call)

 ASM_PUBLIC(_mem_test_and_set)
 ASM_PUBLIC(__get_PSP)
 ASM_PUBLIC(__get_MSP)
 ASM_PUBLIC(__get_PSR)

 ASM_PUBLIC(_psp_exception_return)



/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _sched_start_internal
 Returned Value   : none
 Comments         : start MQX scheduler

 END*------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_sched_start_internal)
 ASM_PUBLIC_FUNC(_sched_start_internal)
ASM_LABEL(_sched_start_internal)
                svc SVC_RUN_SCHED
                bx lr
 ASM_PUBLIC_END(_sched_start_internal)

/*---------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_sched_run_internal)
 ASM_PUBLIC_FUNC(_sched_run_internal)
ASM_LABEL(_sched_run_internal)
                svc SVC_RUN_SCHED
ASM_LABEL(_sched_run_internal_loop)
                b _sched_run_internal_loop
 ASM_PUBLIC_END(_sched_run_internal)

/*---------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_sched_check_scheduler_internal)
 ASM_PUBLIC_FUNC(_sched_check_scheduler_internal)
ASM_LABEL(_sched_check_scheduler_internal)
                GET_KERNEL_DATA r0

                cpsid.n i
                ldrh r1, [r0, #KD_IN_ISR]
                cbnz r1, _sched_check_scheduler_internal_end

                ldr r1, [r0, #KD_CURRENT_READY_Q]
                ldr r2, [r0, #KD_ACTIVE_PTR]
                ldr r3, [r2, #TD_MY_QUEUE]
                cmp r1, r3

                // current task is still the active task
                beq _sched_check_scheduler_internal_end

#if MQX_ENABLE_USER_MODE
                cpsid.n i
                push {r0}
                mrs r0, IPSR
                cmp r0, #0xb
                pop {r0}
                bne _sched_check_scheduler_internal2
                push {lr}
                bl _set_pend_sv
                pop {lr}
                cpsie.n i
                bx lr
ASM_LABEL(_sched_check_scheduler_internal2)
#endif
                cpsie.n i
                svc SVC_TASK_SWITCH

ASM_LABEL(_sched_check_scheduler_internal_end)
                cpsie.n i
                bx lr
 ASM_PUBLIC_END(_sched_check_scheduler_internal)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _sched_execute_scheduler_internal
 Returned Value   : none
 Comments         :

 END*------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_sched_execute_scheduler_internal)
 ASM_PUBLIC_FUNC(_sched_execute_scheduler_internal)
ASM_LABEL(_sched_execute_scheduler_internal)
#if MQX_ENABLE_USER_MODE
                cpsid.n i
                push {r0}
                mrs r0, IPSR
                cmp r0, #0xb
                pop {r0}
                bne _sched_execute_scheduler_internal2
                push {lr}
                bl _set_pend_sv
                pop {lr}
                cpsie.n i
                bx lr
ASM_LABEL(_sched_execute_scheduler_internal2)
                cpsie.n i
#endif

                svc SVC_TASK_SWITCH
                bx lr
 ASM_PUBLIC_END(_sched_execute_scheduler_internal)


/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _mqx_api_call
 Returned Value   : none
 Comments         :

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_mqx_api_call)
 ASM_PUBLIC_FUNC(_mqx_api_call)
ASM_LABEL(_mqx_api_call)

#if MQX_ENABLE_USER_MODE
                push {lr}
                svc SVC_MQX_FN
#endif
ASM_LABEL(_mqx_api_call_end)
                b _mqx_api_call_end
 ASM_PUBLIC_END(_mqx_api_call)

#if MQX_ENABLE_USER_MODE
ASM_LABEL(_mqx_api_call_handler_epilogue)
                push {r0, r1}

                /* get active task descriptor */
                GET_KERNEL_DATA r0
                ldr r0, [r0, #KD_ACTIVE_PTR]

                /* set task flag to privilege mode */
                ldr r1, [r0, #TD_FLAGS]

                tst r1, #0x10   /* #MQX_USER_TASK */
                beq _mqx_api_call_handler_epilogue_end

                orr r1, r1, #TASK_USER_MODE
                str r1, [r0, #TD_FLAGS]

                /* user mode, proc stack */
                mov r0, #3
                msr CONTROL, r0

ASM_LABEL(_mqx_api_call_handler_epilogue_end)
                pop {r0, r1, pc}

ASM_LABEL(_mqx_api_call_handler_prologue)
                mrs r1, PSP

                /* modify stack - return adress from svc */
                ldr r0, =_mqx_api_call_handler
                /* bic r0, r0, #1 */
                str r0, [r1, #24]                                        /* set stacked PC to requested mqx fn */

                /* modify stack - return address from mqx api */
                ldr r0, =_mqx_api_call_handler_epilogue
                /* bic r0, r0, #1 */
                str r0, [r1, #20]                                        /* set stacked LR to _mqx_fn_handler_epilogue */

                /* get active task descriptor */
                GET_KERNEL_DATA r0
                ldr r0, [r0, #KD_ACTIVE_PTR]

                /* set task flag to privilege mode */
                ldr r1, [r0, #TD_FLAGS]
                bic r1, r1, #TASK_USER_MODE
                str r1, [r0, #TD_FLAGS]

                /* privilege mode */
                mov r0, #0
                msr CONTROL, r0

                cpsie.n i

                bx lr

#endif

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _task_block
 Returned Value   : none
 Comments         : task block function - block actual task - switch to another

END*-------------------------------------------------------------------------*/
 ASM_PUBLIC_BEGIN(_task_block)
 ASM_PUBLIC_FUNC(_task_block)
ASM_LABEL(_task_block)

                svc SVC_TASK_BLOCK
                bx lr
 ASM_PUBLIC_END(_task_block)


/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _svc_handler
 Returned Value   : none
 Comments         : SVC handler - called after SVC instruction
                    call MQX scheduler functions

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_svc_handler)
 ASM_PUBLIC_FUNC(_svc_handler)
ASM_LABEL(_svc_handler)
                cpsid.n i

                tst lr, #0x4                        /* test EXC_RETURN number in LR bit 2 */
                ite eq                              /* if zero (equal) then */
                mrseq r1, MSP                       /* Main Stack was used, put MSP in R1 */
                mrsne r1, PSP                       /* else, Process Stack was used, put PSP in R1 */
/*                ldr r0, [r1, #0]                *//* get stacked R0 from stack */
                ldr r1, [r1, #24]                   /* get stacked PC from stack */
                ldrb r1, [r1, #-2]                  /* get the immediate data from the instruction */

#if MQX_ENABLE_USER_MODE
                cmp r1, #SVC_MQX_FN
                bne svc_task_switch

                /*tst lr, #0x4                    *//* test EXC_RETURN number in LR bit 2 */
                /*ite eq                          *//* if zero (equal) then */
                /*mrseq r0, MSP                   *//* Main Stack was used, put MSP in R1 */
                /*mrsne r0, PSP*/

                b _mqx_api_call_handler_prologue
#endif

ASM_LABEL(svc_task_switch)
                cmp r1, #SVC_TASK_SWITCH
                bne svc_run_sched

                push {lr}
                bl _set_pend_sv
                pop {lr}

                b svc_handler_end

ASM_LABEL(svc_run_sched)
                cmp r1, #SVC_RUN_SCHED
                bne svc_task_block

                GET_KERNEL_DATA r0
                b first_run

ASM_LABEL(svc_task_block)
                cmp r1, #SVC_TASK_BLOCK
                bne svc_handler_end

                GET_KERNEL_DATA r0
                ldr r1, [r0, #KD_ACTIVE_PTR]    /* get active task descriptor */

                /* set task block bit */
                ldr r2, [r1, #TD_STATE]
                orr r2, r2, #1                  /* TODO change from number to define */
                str r2, [r1, #TD_STATE]

                push {lr}

#if MQX_KERNEL_LOGGING
                KLOG r0, ASM_PREFIX(_klog_block_internal)        /* kernel log this function */
#endif // MQX_KERNEL_LOGGING
                /* remove active task from ready que */
                ldr r2, [r1, #TD_TD_PREV]           /* get ptr to ready_q structure */
                ldr r3, [r1, #TD_TD_NEXT]
                str r3, [r2, #RQ_HEAD_READY_Q]
                str r2, [r3, #TD_TD_PREV]

                bl _set_pend_sv
                pop {lr}

ASM_LABEL(svc_handler_end)
                cpsie.n i
                bx lr
 ASM_PUBLIC_END(_svc_handler)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _set_pend_sv
 Returned Value   : none
 Comments         : This functions set pending sv flag - request for PendSV
                    request for task switch

END*-------------------------------------------------------------------------*/

ASM_LABEL(_set_pend_sv)
                push {r0-r2, lr}

                /* get PendSV flag */
                ldr r0, =0xE000ED00
                ldr r1, [r0, #4]                    /* 0xE000ED04 */
                tst r1, #0x10000000
                bne _set_pend_sv_end

                mrs r2, BASEPRI
                sub r2, r2, #0x10

                /* set priority for PendSV */
                strb r2, [r0, #0x22]                /* 0xE000ED22 */

                /* set PendSV flag */
                ldr r1, =0x10000000                 /* PENDSVSET */
                str r1, [r0, #4]                    /* 0xE000ED04 */

ASM_LABEL(_set_pend_sv_end)
                pop {r0-r2, pc}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _pend_svc
 * Returned Value   : none
 * Comments         : PendSV handler - task switch functionality
 *
 *END*----------------------------------------------------------------------*/

/* Pending Service Call */
 ASM_PUBLIC_BEGIN(_pend_svc)
 ASM_PUBLIC_FUNC(_pend_svc)
ASM_LABEL(_pend_svc)
                cpsid.n i

                /* store active task registers */
                mrs r12, PSP                        /* get process stack pointer for active task */

                ldr r1, =0xE000ED20                 /* SHPR3 */
                ldrb r2, [r1, #2]

                mrs r3, BASEPRI
                stmdb r12!, {r2-r11, lr}            /* store remaining registers */

                GET_KERNEL_DATA r0
                ldr r3, [r0, #KD_ACTIVE_PTR]        /* get active task descriptor */
                str r12, [r3, #TD_STACK_PTR]        /* store task SP to task descriptor td */

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
                ldr r1, [r3, #TD_FLAGS]

                /* check for fpu flag in TD */
                tst r1, #FP_TASK_MASK
                beq fpu_store_end

                /* store FPU registers */
                ldr r12, [r3, #TD_FLOAT_CONTEXT_PTR]    /* get task fpu context address */
                ldr r2, =0xE000EF34                     /* FPCCR */
                ldr r1, [r2], #4                        /* FPCCR */
                str r1, [r12], #4

                ldr r1, [r2], #4                        /* FPCAR */
                str r1, [r12], #4

                ldr r1, [r2], #4                        /* FPDSCR */
                str r1, [r12], #4

                vmrs r1, FPSCR
                str r1, [r12], #8                       /* FPSCR */

                vstm r12, {s0-s31}                      /* restore fpu registers */
ASM_LABEL(fpu_store_end)
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */

ASM_LABEL(first_run)
                cpsid i
                ldr r1, [r0, #KD_CURRENT_READY_Q]   /* get current ready q */
ASM_LABEL(find_noempty_que)
                ldr r2, [r1, #0]                    /* address of first td */
                cmp r2, r1                          /* ready_q structure itself? */
                bne switch_task
                ldr r1, [r1, #RQ_NEXT_Q]            /* try next queue */
                movs r1, r1
                bne find_noempty_que

                /* r1 is 0 -> empty */
ASM_LABEL(no_one_to_run)
                /* TODO set system task ??? */

                /* enable all interrupts (r1 = 0) */
                /* TODO maybe (maybe not necessary) restore PendSV priority and BASEPRI after wfi */
                msr BASEPRI, r1

                ldr r1, =0xE000ED20                    /* SHPR3 */
                ldr r2, =0xff
                strb r2, [r1, #2]

                /* wait for interrupt */
                cpsie.n i
                wfi
                cpsid.n i

                /* TODO check r0, must be kernel data */
                ldr r1, [r0, #KD_READY_Q_LIST]      /* get first que from ready list */
                b find_noempty_que

ASM_LABEL(switch_task)
                /* update kernel structures */
                str r1, [r0, #KD_CURRENT_READY_Q]   /* store addr for active que */
                str r2, [r0, #KD_ACTIVE_PTR]        /* active task descriptor */

                ldrh r3, [r2, #TD_TASK_SR]
                strh r3, [r0, #KD_ACTIVE_SR]        /* restore priority mask for enabled interrupt for active task */

#if MQX_ENABLE_USER_MODE || (MQXCFG_ENABLE_FP && PSP_HAS_FPU)
                ldr r1, [r2, #TD_FLAGS]
#endif

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
                /* check for fpu flag in TD */
                tst r1, #FP_TASK_MASK
                beq fpu_restore_end

                /* check if last fpu task is different */
                ldr r3, [r0, #KD_FP_ACTIVE_PTR]
                cmp r2, r3
                beq fpu_restore_end

                str r2, [r0, #KD_FP_ACTIVE_PTR]

                /* restore FPU registers */
                ldr r12, [r2, #TD_FLOAT_CONTEXT_PTR]    /* get task fpu context address */
                ldr r4, =0xE000EF34                     /* FPCCR */
                ldr r3, [r12], #4
                str r3, [r4], #4                        /* FPCCR */

                ldr r3, [r12], #4
                str r3, [r4], #4                        /* FPCAR */

                ldr r3, [r12], #4
                str r3, [r4], #4                        /* FPDSCR */

                ldr r3, [r12], #8                       /* FPSCR */
                vmsr FPSCR, r3

                vldm r12!, {s0-s31}                     /* restore fpu registers */
ASM_LABEL(fpu_restore_end)
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */

                /* activate task, restore registers */
                ldr r12, [r2, #TD_STACK_PTR]        /* get task SP */
                ldmia r12!, {r2-r11, lr}            /* restore registers */
                msr BASEPRI, r3

                msr PSP, r12                        /* restore process stack pointer for task */

#if MQX_KERNEL_LOGGING
                KLOG r0, ASM_PREFIX(_klog_context_switch_internal)  /* do kernel logging */
#endif

#if MQX_ENABLE_USER_MODE
                tst r1, #TASK_USER_MODE             /* r1 still contain TD_FLAGS, check for user mode task */
                ite eq
                moveq r0, #0                        /* privilege mode */
                movne r0, #1                        /* user mode */
                msr CONTROL, r0
#endif
                /* clear PendSV flag - fix for late arriving optimalization issue */
                ldr r0, =0xE000ED00
                ldr r1, =0x08000000                 /* PENDSVCLR */
                str r1, [r0, #4]                    /* 0xE000ED04 */

                cpsie.n i
                bx lr
 ASM_PUBLIC_END(_pend_svc)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _int_kernel_isr
 Returned Value   : none
 Comments         : MQX kernel interrupt handler

END*-------------------------------------------------------------------------*/

/* kernel interrupt handler */
 ASM_PUBLIC_BEGIN(_int_kernel_isr)
 ASM_PUBLIC_FUNC(_int_kernel_isr)
ASM_LABEL(_int_kernel_isr)
                cpsid.n i
                push {lr}

                GET_KERNEL_DATA r3              /* get the kernel data address */

                /* increment in interrupt counter */
                ldrh r1, [r3, #KD_IN_ISR]
                add r1, r1, #1
                strh r1, [r3, #KD_IN_ISR]

                /* create interrupt content */
                ldr r0, =0                      /* error code (set 0) */
                push {r0}                       /* store in stack */
                mrs r2, BASEPRI                 /* actual priority */
                mrs r1, IPSR                    /* exception number */
                ldr r0, [r3, #KD_INTERRUPT_CONTEXT_PTR] /* previous interrupt content */
                push {r0-r2}                    /* store in stack */

                mrs r0, MSP                     /* get address of interrupt content */
                str r0, [r3, #KD_INTERRUPT_CONTEXT_PTR] /* store address of actual interrupt content in kernel data */

                mov r0, r1

#if MQX_KERNEL_LOGGING
                ldr r1, [r3, #KD_LOG_CONTROL]
                tst r1, #0x00000001
                beq _isr_no_logging

                /* prepare parameters for klog function */
                push {r0-r3}
                bl ASM_PREFIX(_klog_isr_start_internal)
                pop {r0-r3}
ASM_LABEL(_isr_no_logging)
#endif // MQX_KERNEL_LOGGING
                /* get C function address from MQX interrupt table */
                /* r0 - interrupt number */

                /* check if isr is in table range */
                ldr r2, [r3, #KD_LAST_USER_ISR_VECTOR]

                /* cbz r2, _isr_run_default       *//* isr not used (cbz not working in CW) */
                cbnz r2, _isr_skip_run_default1   /* isr not used (this is CW workaround) */
                b _isr_run_default
ASM_LABEL(_isr_skip_run_default1)

                cmp r0, r2
                bhi _isr_run_default

                ldr r2, [r3, #KD_FIRST_USER_ISR_VECTOR]
                subs r1, r0, r2                         /* r1 = actual exception number in table */
                blt _isr_run_default

#if MQX_SPARSE_ISR_TABLE

ASM_LABEL(_int_kernel_isr_vect_ok)
                ldr r2, [r3, #KD_INTERRUPT_TABLE_PTR]   /* get the interrupt table pointer */
                lsr r1, r1, #MQX_SPARSE_ISR_SHIFT
                lsl r1, r1, #2

                ldr r1, [r2, r1]                        /* get address of first isr in linked list */

                /* cbz r1, _isr_run_default           *//* isr not used (cbz not working in CW) */
                cbnz r1, _isr_skip_run_default2         /* isr not used (this is CW workaround) */
                b _isr_run_default
ASM_LABEL(_isr_skip_run_default2)

                /* r1 - address of first isr in linked list */
ASM_LABEL(_isr_search)
                ldr r2, [r1, #HASH_ISR_NUM]             /* get isr num */
                cbz r2, _isr_search_fail

                cmp r2, r0                              /* compare isr number in record with actual isr number */
                beq _isr_search_suc

                ldr r1, [r1, #HASH_ISR_NEXT]            /* next vector in list */
                tst r1, r1
                bne _isr_search

ASM_LABEL(_isr_search_fail)
                b _isr_run_default

ASM_LABEL(_isr_search_suc)
                ldr r0, [r1, #HASH_ISR_DATA]            /* move notifier data into r0 = first parameter in C func */
                ldr r2, [r1, #HASH_ISR_ADDR]            /* move interrupt function address to r2 */

#else /* MQX_SPARSE_ISR_TABLE */

ASM_LABEL(_int_kernel_isr_vect_ok)
                /* calculate offset in table */
                /* each table entry is 12 bytes in size */
                mov r2, #12
                mul r1, r2, r1

                ldr r2, [r3, #KD_INTERRUPT_TABLE_PTR]         /* pointer to interrupt table begin */
                add r1, r1, r2                                /* get address of entry in table */
                ldr r2, [r1, #0]
                ldr r0, [r1, #IT_APP_ISR_DATA]          /* move notifier data into r0 = first parameter in C func */
#endif /* MQX_SPARSE_ISR_TABLE */

ASM_LABEL(_isr_execute)
                /* r0 = first parameter in C func */
                /* r2 contain interrupt function address */

                cpsie.n i
                push {r3}

                blx r2

ASM_LABEL(_int_kernel_isr_epilog)

                pop {r3}
                cpsid.n i

ASM_LABEL(_int_kernel_isr_return_internal)

#if MQX_KERNEL_LOGGING
                ldr r1, [r3, #KD_LOG_CONTROL]
                tst r1, #0x00000001
                beq _isr_return_no_logging

                mrs r0, IPSR                    /* exception number */

                /* prepare parameters for klog function */
                push {r0-r3}
                bl ASM_PREFIX(_klog_isr_end_internal)
                pop {r0-r3}
ASM_LABEL(_isr_return_no_logging)
#endif /* MQX_KERNEL_LOGGING */

                /* remove interrupt content */
                pop {r0-r2}
                str r0, [r3, #KD_INTERRUPT_CONTEXT_PTR] /* update pointer to interrupt content */

                pop {r0}                                /* error code */

                /* decrement interrupt counter */
                ldrh r1, [r3, #KD_IN_ISR]
                subs r1, r1, #1
                strh r1, [r3, #KD_IN_ISR]

                /* check for reschedule */
                /* check preemtion */
                ldr r2, [r3, #KD_ACTIVE_PTR]    /* TD pointer */
                ldr r0, [r2, #TD_FLAGS]
                tst r0, #TASK_PREEMPTION_DISABLED
                bne _isr_return_end

                cbnz r1, _isr_return_end        /* waiting another isr, do not reschedulle */

                /* if a different TD at head of current readyq, then we need to run the scheduler */
                /* check for reschedule */
                ldr r1, [r3, #KD_CURRENT_READY_Q]
                ldr r1, [r1]
                cmp r1, r2
#ifdef __CODEWARRIOR__
                it ne
                bl _set_pend_sv
#else
                it ne
                blne _set_pend_sv
#endif

ASM_LABEL(_isr_return_end)
                cpsie.n i
                pop {pc}

ASM_LABEL(_isr_run_default)
                /* r0 - interrupt number */

                ldr r2, [r3, #KD_DEFAULT_ISR]

                b _isr_execute
 ASM_PUBLIC_END(_int_kernel_isr)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _psp_exception_return
 Returned Value   : none
 Comments         : This functions returns us from an isr exception

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_psp_exception_return)
 ASM_PUBLIC_FUNC(_psp_exception_return)
ASM_LABEL(_psp_exception_return)
                cpsid.n i
                push {r0, r4-r7}    /* store parameter and registers which are used for copy and cpu do not store if */

                GET_KERNEL_DATA r3

                /* copy actual int context + 4WORDS -> prev int context - (1 + 8)WORDS */
                ldr r12, [r3, #KD_INTERRUPT_CONTEXT_PTR]    /* get actual int context */
                add r12, r12, #16               /* calculate source address */

                ldr lr, [r12], #4               /* lr - return from exception... (0xfffffff1) */

                /* modify return address (PC) in exception stack frame (kernel isr epilog) */
                ldr r0, =_int_kernel_isr_epilog
                bic r0, r0, #1
                str r0, [r12, #24]

                ldmia r12, {r0-r7}              /* read exception stack frame */

                pop {r12}                       /* get prev int context address (function parameter - r0) */

                /* destination stack address (4B + 32B - exception stack frame) */
                sub r12, r12, #4    /*#36*/
                stmdb r12!, {r0-r7}

                /* restore modified registers which are not restored by cpu */
                pop {r4-r7}

                msr MSP, r12   /* update MSP (main stack pointer) */

                cpsie.n i

                bx lr
 ASM_PUBLIC_END(_psp_exception_return)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _psp_save_fp_context_internal
 Returned Value   : none
 Comments         :
   This function saves the floating point context for the
 current floating point task

END*-------------------------------------------------------------------------*/

        ASM_PUBLIC(_psp_save_fp_context_internal)
        ASM_ALIGN(4)

ASM_LABEL(_psp_save_fp_context_internal)
#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
/*       Stop the floating point unit, and save the internal */
/*       floating point registers on the stack of the FP task */
                GET_KERNEL_DATA r0
                ldr r3, [r0, #KD_ACTIVE_PTR]        /* get active task descriptor */
                ldr r1, [r3, #TD_FLAGS]

                // store FPU registers
                ldr r12, [r3, #TD_FLOAT_CONTEXT_PTR]    /* get task fpu context address */
                ldr r2, =0xE000EF34                     /* FPCCR */
                ldr r1, [r2], #4                        /* FPCCR */
                str r1, [r12], #4

                ldr r1, [r2], #4                        /* FPCAR */
                str r1, [r12], #4

                ldr r1, [r2], #4                        /* FPDSCR */
                str r1, [r12], #4

                vmrs r1, FPSCR
                str r1, [r12], #8                       /* FPSCR */

                vstm r12, {s0-s31}                      /* store fpu registers */
ASM_LABEL(_psp_save_fp_context_done)
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */
                bx lr

/*******************************************************************************
 * test support
 ******************************************************************************/

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _mem_test_and_set
 Returned Value   : previous value of location
 Comments         :
   This function tests a byte location, and if AND 0x80 is 0 (7bit is not set !!!), sets it to 0x80.
   It returns the previous value of the byte (0 or 0x80).

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(_mem_test_and_set)
 ASM_PUBLIC_FUNC(_mem_test_and_set)
ASM_LABEL(_mem_test_and_set)
        mov r2, #0x80
ASM_LABEL(_mem_test_and_set_loop)

#if !defined(__CODEWARRIOR__)
        ldrexb r1, [r0]
#else
 ASM_CONST16(0xe8d0)
 ASM_CONST16(0x1f4f)
#endif
        cmp r1, #0                  /* is it free? */
        ite eq
#if !defined(__CODEWARRIOR__)
        strexbeq r3, r2, [r0]       /* yes, try lock */
#else
/* beloved CW10 dont know strexb */
 ASM_CONST16(0xe8c0)
 ASM_CONST16(0x2f43)
#endif

        bne _mem_test_and_set_end
        cmp r3, #0
        bne _mem_test_and_set_loop     /* store success? */

ASM_LABEL(_mem_test_and_set_end)
        mov r0, r1
        bx lr
 ASM_PUBLIC_END(_mem_test_and_set)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __get_PSP
 Returned Value   : none
 Comments         : This functions returns PSP register value

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(__get_PSP)
 ASM_PUBLIC_FUNC(__get_PSP)
ASM_LABEL(__get_PSP)
                mrs r0, PSP
                bx lr
 ASM_PUBLIC_END(__get_PSP)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __get_MSP
 Returned Value   : none
 Comments         : This functions returns MSP register value

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(__get_MSP)
 ASM_PUBLIC_FUNC(__get_MSP)
ASM_LABEL(__get_MSP)
                mrs r0, MSP
                bx lr
 ASM_PUBLIC_END(__get_MSP)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __set_MSP
 Returned Value   : none
 Comments         : This functions set MSP register value

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(__set_MSP)
 ASM_PUBLIC_FUNC(__set_MSP)
ASM_LABEL(__set_MSP)
                msr MSP, r0
                bx lr
 ASM_PUBLIC_END(__set_MSP)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __get_PSR
 Returned Value   : none
 Comments         : This functions returns PSR (procesor status register) value

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC_BEGIN(__get_PSR)
 ASM_PUBLIC_FUNC(__get_PSR)
ASM_LABEL(__get_PSR)
        #ifdef __CWARM__
                 mrs r0, XPSR
        #else
                 mrs r0, PSR
        #endif
                 bx lr
 ASM_PUBLIC_END(__get_PSR)

/* IAR defines following functions internally */
#ifndef __IAR_SYSTEMS_ASM__

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __enable_interrupt
 Returned Value   : none
 Comments         : This functions enables interrupts

END*-------------------------------------------------------------------------*/

ASM_PUBLIC_BEGIN(__enable_interrupt)
ASM_PUBLIC_FUNC(__enable_interrupt)
ASM_LABEL(__enable_interrupt)
                 cpsie.n i
                 bx lr
ASM_PUBLIC_END(__enable_interrupt)


/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __disable_interrupt
 Returned Value   : none
 Comments         : This functions disables interrupts

END*-------------------------------------------------------------------------*/

ASM_PUBLIC_BEGIN(__disable_interrupt)
ASM_PUBLIC_FUNC(__disable_interrupt)
ASM_LABEL(__disable_interrupt)
                 cpsid.n i
                 bx lr
ASM_PUBLIC_END(__disable_interrupt)


 ASM_PUBLIC(__get_LR)
 ASM_PUBLIC(__get_PC)
 ASM_PUBLIC(__enable_interrupt)
 ASM_PUBLIC(__disable_interrupt)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __get_LR
 Returned Value   : none
 Comments         : This functions returns LR register value

END*-------------------------------------------------------------------------*/

ASM_PUBLIC_BEGIN(__get_LR)
ASM_PUBLIC_FUNC(__get_LR)
ASM_LABEL(__get_LR)
                mov r0, lr
                bx lr
ASM_PUBLIC_END(__get_LR)


/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __get_PC
 Returned Value   : none
 Comments         : This functions returns LR register value

END*-------------------------------------------------------------------------*/

ASM_PUBLIC_BEGIN(__get_PC)
ASM_PUBLIC_FUNC(__get_PC)
ASM_LABEL(__get_PC)
                mov r0, PC
                bx lr
ASM_PUBLIC_END(__get_PC)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : __get_PC
 Returned Value   : none
 Comments         : This functions returns LR register value

END*-------------------------------------------------------------------------*/

 ASM_PUBLIC(__get_CONTROL)

ASM_PUBLIC_BEGIN(__get_CONTROL)
ASM_PUBLIC_FUNC(__get_CONTROL)
ASM_LABEL(__get_CONTROL)
                mrs r0, CONTROL
                bx lr
ASM_PUBLIC_END(__get_CONTROL)

#endif  /*__IAR_SYSTEMS_ASM__*/

 ASM_ALIGN(4)
 ASM_END






