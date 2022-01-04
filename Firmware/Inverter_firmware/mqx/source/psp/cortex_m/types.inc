/*HEADER********************************************************************
* 
* Copyright (c) 2010-2011 Freescale Semiconductor
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
* $FileName: types.inc$
* $Version : 3.8.10.0$
* $Date    : Sep-24-2012$
*
* Comments:
*   This file contains the assembler offsets calculated by the
*   program KRNL_OFF.C.  These offsets are to be included in
*   any assembler program that wishes to access kernel data
*   structures.
*
*END***********************************************************************/

#include <asm_mac.h>

/*
* 'PSP_BLOCKED_STACK_STRUCT' size = 64 , 0x40
*/

ASM_EQUATE(PSPB_ST_PC,                          60)  /*  0x3C */


/* 'PSP_BLOCKED_FP_STRUCT' */
ASM_EQUATE(FP_FPCR_OFFSET,                       0)     /* 0x00 */
ASM_EQUATE(FP_FPSR_OFFSET,                       4)     /* 0x04 */
ASM_EQUATE(FP_FPIAR_OFFSET,                      8)     /* 0x08 */
ASM_EQUATE(FP_TID_OFFSET,                        12)    /* 0x0C */
ASM_EQUATE(FP_FPR0_OFFSET,                       16)    /* 0x10 */
ASM_EQUATE(FP_FPR1_OFFSET,                       24)    /* 0x18 */
ASM_EQUATE(FP_FPR2_OFFSET,                       32)    /* 0x20 */
ASM_EQUATE(FP_FPR3_OFFSET,                       40)    /* 0x28 */
ASM_EQUATE(FP_FPR4_OFFSET,                       48)    /* 0x30 */
ASM_EQUATE(FP_FPR5_OFFSET,                       56)    /* 0x38 */
ASM_EQUATE(FP_FPR6_OFFSET,                       64)    /* 0x40 */
ASM_EQUATE(FP_FPR7_OFFSET,                       72)    /* 0x48 */
 



/*
* 'TD_STRUCT' size = 216 , 0xd8
*/
ASM_EQUATE(TD_TD_NEXT,                              0)
ASM_EQUATE(TD_TD_PREV,                              4)
ASM_EQUATE(TD_STATE,                                8)
ASM_EQUATE(TD_TASK_ID,                             12)
ASM_EQUATE(TD_STACK_BASE,                          16)
ASM_EQUATE(TD_STACK_PTR,                           20)
ASM_EQUATE(TD_STACK_LIMIT,                         24)
ASM_EQUATE(TD_MY_QUEUE,                            28)
ASM_EQUATE(TD_HOME_QUEUE,                          32)
ASM_EQUATE(TD_TASK_SR,                             36)
ASM_EQUATE(TD_RESERVED_1,                          38)
ASM_EQUATE(TD_FLAGS,                               40)
ASM_EQUATE(TD_FLOAT_CONTEXT_PTR,                   52)

/*
* 'READY_Q_STRUCT' size = 16 , 0x10
*/
ASM_EQUATE(RQ_STRUCT_SIZE,                         16)
ASM_EQUATE(RQ_HEAD_READY_Q,                         0) 
ASM_EQUATE(RQ_TAIL_READY_Q,                         4) 
ASM_EQUATE(RQ_NEXT_Q,                               8)
ASM_EQUATE(RQ_ENABLE_SR,                           12)  
ASM_EQUATE(RQ_PRIORITY,                            14) 

/*
* 'PSP_INT_CONTEXT_STRUCT' size = 24 , 0x18
*/
/*ASM_EQUATE(IC_STRUCT_SIZE,			24)  //  0x18
ASM_EQUATE(IC_CPSR,                             0)  //  0x0
ASM_EQUATE(IC_EXCEPTION_NUMBER,                 4)  //  0x4
ASM_EQUATE(IC_ERROR_CODE,                       8)  //  0x8
ASM_EQUATE(IC_ENABLE_SR,                        12)  //  0xc
ASM_EQUATE(IC_ENABLE_LR,                        16)  //  0x10
ASM_EQUATE(IC_PREV_CONTEXT,                     20)  //  0x14
*/
/*
* 'INTERRUPT_TABLE_STRUCT' size = 12 , 0xc
*/
ASM_EQUATE(IT_STRUCT_SIZE,                         12)
ASM_EQUATE(IT_APP_ISR,                              0)
ASM_EQUATE(IT_APP_ISR_EXCEPTION_HANDLER,            4)
ASM_EQUATE(IT_APP_ISR_DATA,                         8)

ASM_EQUATE(HASH_ISR_ITM_SIZE,                      20)
ASM_EQUATE(HASH_ISR_NUM,                           0)
ASM_EQUATE(HASH_ISR_ADDR,                          4)
ASM_EQUATE(HASH_ISR_EXCEPT_HNDL,                   8)
ASM_EQUATE(HASH_ISR_DATA,                          12)
ASM_EQUATE(HASH_ISR_NEXT,                          16)

/*
* 'KERNEL_DATA_STRUCT' size = 1156 , 0x484
*/
ASM_EQUATE(KD_ADDRESSING_CAPABILITY,                0)  /*  0x0 */
ASM_EQUATE(KD_ENDIANESS,                            4)  /*  0x4 */
ASM_EQUATE(KD_CPU_TYPE,                             8)  /*  0x8 */
ASM_EQUATE(KD_PSP_CFG_MEMORY_ALIGNMENT,            10)  /*  0xa */
ASM_EQUATE(KD_PSP_CFG_STACK_ALIGNMENT,             12)  /*  0xc */
ASM_EQUATE(KD_PSP_CFG_MEM_STOREBLOCK_ALIGNMENT,    14)  /*  0xe */
ASM_EQUATE(KD_CONFIG1,                             16)  /*  0x10 */
ASM_EQUATE(KD_CONFIG2,                             18)  /*  0x12 */
ASM_EQUATE(KD_FLAGS,                               20)  /*  0x14 */
ASM_EQUATE(KD_DISABLE_SR,                          22)  /*  0x16 */
ASM_EQUATE(KD_IN_ISR,                              24)  /*  0x18 */
ASM_EQUATE(KD_ACTIVE_SR,                           26)  /*  0x1a */
ASM_EQUATE(KD_ACTIVE_PTR,                          28)  /*  0x1c */
ASM_EQUATE(KD_READY_Q_LIST,                        32)  /*  0x20 */
ASM_EQUATE(KD_CURRENT_READY_Q,                     36)  /*  0x24 */
ASM_EQUATE(KD_DEFAULT_ISR,                         40)  /*  0x28 */
ASM_EQUATE(KD_FIRST_USER_ISR_VECTOR,               44)  /*  0x2c */
ASM_EQUATE(KD_LAST_USER_ISR_VECTOR,                48)  /*  0x30 */
ASM_EQUATE(KD_INTERRUPT_CONTEXT_PTR,               52)  /*  0x34 */
ASM_EQUATE(KD_INTERRUPT_TABLE_PTR,                 56)  /*  0x38 */
ASM_EQUATE(KD_INTERRUPT_STACK_PTR,                 60)  /*  0x3c */
 
ASM_EQUATE(KD_LOG_CONTROL,                         64)  /*  0x40 */
ASM_EQUATE(LOG_OLD_TD,                             68)  /*  0x44 */
ASM_EQUATE(KD_FP_ACTIVE_PTR,                       72)  /*  0x48 */
ASM_EQUATE(KD_DSP_ACTIVE_PTR,                      76)  /*  0x4c */
ASM_EQUATE(KD_SYSTEM_TD,                           80)  /*  0x50 */

 #ifdef __ACF__
/*
*  Control Registers
*/
ASM_EQUATE(CACR, 0x002) /* Cache control register */
ASM_EQUATE(ASID, 0x003) /* Address space identifier register */
ASM_EQUATE(acr0, 0x004) /* Access control registers 0 */
ASM_EQUATE(acr1, 0x005) /* Access control registers 1 */
ASM_EQUATE(ACR2, 0x006) /* Access control registers 2 */
ASM_EQUATE(ACR3, 0x007) /* Access control registers 3 */
ASM_EQUATE(mbar, 0x008) /* MMU base address register */
ASM_EQUATE(VBR, 0x801)  /* Vector base register */
ASM_EQUATE(PC, 0x80F)   /* Program counter */

ASM_EQUATE(ROMBAR0, 0xC00) /* ROM base address register 0 */
ASM_EQUATE(ROMBAR1, 0xC01) /* ROM base address register 1 */
ASM_EQUATE(RAMBAR0, 0xC04) /* RAM base address register 0 */
ASM_EQUATE(RAMBAR1, 0xC05) /* RAM base address register 1 */
ASM_EQUATE(MPCR, 0xC0C)    /* Multiprocessor control register 1 */
ASM_EQUATE(EDRAMBAR, 0xC0D) /* Embedded DRAM base address register 1 */
ASM_EQUATE(SECMBAR, 0xC0E) /* Secondary module base address register 1 */
ASM_EQUATE(MBAR, 0xC0F)    /* Primary module base address register */

ASM_EQUATE(PCR1U0, 0xD02) /* 32 msbs of RAM 0 permutation control register 1 */
ASM_EQUATE(PCR1L0, 0xD03) /* 32 lsbs of RAM 0 permutation control register 1 */
ASM_EQUATE(PCR2U0, 0xD04) /* 32 msbs of RAM 0 permutation control register 2 */
ASM_EQUATE(PCR2L0, 0xD05) /* 32 lsbs of RAM 0 permutation control register 2 */
ASM_EQUATE(PCR3U0, 0xD06) /* 32 msbs of RAM 0 permutation control register 3 */
ASM_EQUATE(PCR3L0, 0xD07) /* 32 lsbs of RAM 0 permutation control register 3 */
ASM_EQUATE(PCR1U1, 0xD0A) /* 32 msbs of RAM 1 permutation control register 1 */
ASM_EQUATE(PCR1L1, 0xD0B) /* 32 lsbs of RAM 1 permutation control register 1 */
ASM_EQUATE(PCR2U1, 0xD0C) /* 32 msbs of RAM 1 permutation control register 2 */
ASM_EQUATE(PCR2L1, 0xD0D) /* 32 lsbs of RAM 1 permutation control register 2 */
ASM_EQUATE(PCR3U1, 0xD0E) /* 32 msbs of RAM 1 permutation control register 3 */
ASM_EQUATE(PCR3L1, 0xD0F) /* 32 lsbs of RAM 1 permutation control register 3 */

#endif
