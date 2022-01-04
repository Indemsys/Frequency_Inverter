/**HEADER*********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
******************************************************************************
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
******************************************************************************
*
* $FileName: cortex.h$
* $Version : 3.8.15.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This file contains the type definitions for the ARM Cortex processors.
*
*END************************************************************************/

#ifndef __cortex_h__
#define __cortex_h__

/*==========================================================================*/
/*
**                   MQX REQUIRED DEFINITIONS
**
** Other MQX kernel and component functions require these definitions to exist.
*/

/* Indicate which endian this PSP is */
#if defined(__ARM__) || defined(__GHS__)
    #ifdef __BIG_ENDIAN
        #define PSP_ENDIAN          MQX_BIG_ENDIAN
    #else
        #define PSP_ENDIAN          MQX_LITTLE_ENDIAN
    #endif
#elif defined(__MET__)
    #ifdef _BE
        #define PSP_ENDIAN          MQX_BIG_ENDIAN
    #else
        #define PSP_ENDIAN          MQX_LITTLE_ENDIAN
    #endif
#elif defined(__GNUC__)
    #ifdef BYTES_BIG_ENDIAN
        #define PSP_ENDIAN          MQX_BIG_ENDIAN
    #else
        #define PSP_ENDIAN          MQX_LITTLE_ENDIAN
    #endif
#else
    #define PSP_ENDIAN              MQX_LITTLE_ENDIAN
#endif

/*
** Memory alignment requirements.
** The alignment indicates how memory is to be aligned for all memory
** accesses.   This is to avoid mis-aligned transfers of data, thus
** optimizing memory accesses.
*/
#define PSP_MEMORY_ALIGNMENT        (PSP_CACHE_LINE_SIZE-1)
#define PSP_MEMORY_ALIGNMENT_MASK   (~PSP_MEMORY_ALIGNMENT)

/*
** Stack alignment requirements.
** The alignment indicates how the stack is to be initially aligned.
** This is to avoid mis-aligned types on the stack
*/
#define PSP_STACK_ALIGNMENT         PSP_MEMORY_ALIGNMENT
#define PSP_STACK_ALIGNMENT_MASK    PSP_MEMORY_ALIGNMENT_MASK

/*
** Indicate the direction of the stack
*/
#define PSP_STACK_GROWS_TO_LOWER_MEM                (1)

/*
** Indicate addressing capability of the CPU
** This is the memory width. i.e., the number of bits addressed
** by each memory location.
*/
#define PSP_MEMORY_ADDRESSING_CAPABILITY            (8)

/*
** Indicate alignment restrictions on memory accesses
** For an n-bit memory access,
**
** if n <  PSP_MEMORY_ACCESSING_CAPABILITY,
**         n-bit accesses must be n-bit-aligned
**
** if n >= PSP_MEMORY_ACCESSING_CAPABILITY,
**         n-bit accesses must be PSP_MEMORY_ACCESSING_CAPABILITY-bit-aligned
*/
#define PSP_MEMORY_ACCESSING_CAPABILITY             (32)

/*
** Define padding need to make the STOREBLOCK_STRUCT aligned properly
*/
#define PSP_MEM_STOREBLOCK_ALIGNMENT                (2)

/*
**   MINIMUM STACK SIZE FOR A TASK
*/

/*
** Memory overhead on users stack before switching over to the interrupt stack.

*/
#if PSP_MQX_CPU_IS_ARM_CORTEX_M4
    #define PSP_STACK_INTERRUPT_OVERHEAD            (116)
#else /* PSP_MQX_CPU_IS_ARM_CORTEX_M0 is assumed */
    #define PSP_STACK_INTERRUPT_OVERHEAD            (64)
#endif

/* This much extra stack is required for the logging of mqx functions */
#if MQX_KERNEL_LOGGING
    #define PSP_STACK_KLOG_OVERHEAD                 (128)
#else
    #define PSP_STACK_KLOG_OVERHEAD                 (0)
#endif

/* Minimum stack size for all tasks. */
#define PSP_MINSTACKSIZE                      \
            (sizeof(PSP_STACK_START_STRUCT) + \
            PSP_STACK_INTERRUPT_OVERHEAD    + \
            PSP_STACK_KLOG_OVERHEAD)

/* Minimum stack size for the Idle Task
**   Idle task written in C uses at most 8 bytes of stack:
**     1 Link Instruction, 1 local variable (pointer)
*/
#define PSP_IDLE_STACK_SIZE                         (PSP_MINSTACKSIZE + 8)

/* Motorola addresses do not need to be normalized
** (ie as for example the Intel chips in real mode do require this)
*/
#define _PSP_NORMALIZE_MEMORY(x)                    (x)

/*==========================================================================*/
/*
**                    PSP SPECIFIC DEFINITIONS
**
*/


/*
** The maximum number of hardware interrupt vectors
*/
#if PSP_MQX_CPU_IS_ARM_CORTEX_M4
    #define PSP_MAXIMUM_INTERRUPT_VECTORS           (0x100)
#else /* PSP_MQX_CPU_IS_ARM_CORTEX_M0 is assumed */
    #define PSP_MAXIMUM_INTERRUPT_VECTORS           (0x2F)
#endif


#ifndef __ASM__

/*
** PSP FUNCTION CALL STRUCT
**
** This structure defines what a function call pushes onto the stack
*/
typedef struct psp_function_call_struct
{

   /* Frame pointer (r11) pushed onto the stack for back tracing */
   pointer              FP_REGISTER;

   /* Pushed onto the stack by the JSR instruction */
   void     (_CODE_PTR_ RETURN_ADDRESS)(void);

} PSP_FUNCTION_CALL_STRUCT, _PTR_ PSP_FUNCTION_CALL_STRUCT_PTR;


/*==========================================================================*/
/*                         MQX DATA TYPES                                   */


/*-----------------------------------------------------------------------*/
/*
** PSP BLOCKED STACK STRUCT
**
** This is what a stack looks like for a task that is NOT the active
** task
*/
typedef struct psp_blocked_stack_struct
{
#if PSP_MQX_CPU_IS_ARM_CORTEX_M4
    uint_32 PENDSVPRIOR;
    uint_32 BASEPRI;
#endif
    uint_32 R4;
    uint_32 R5;
    uint_32 R6;
    uint_32 R7;
    uint_32 R8;
    uint_32 R9;
    uint_32 R10;
    uint_32 R11;
#if PSP_MQX_CPU_IS_ARM_CORTEX_M4
    uint_32 LR2;
#endif
    uint_32 R0;
    uint_32 R1;
    uint_32 R2;
    uint_32 R3;

    uint_32 R12;
    uint_32 LR;
    uint_32 PC;
    uint_32 PSR;
} PSP_BLOCKED_STACK_STRUCT, _PTR_ PSP_BLOCKED_STACK_STRUCT_PTR;

#if PSP_HAS_FPU
/*-----------------------------------------------------------------------*/
/*
** PSP BLOCKED FP STRUCT
**
** This structure defines the registers stored by MQX when a floating
** point task is blocked.  When the FLOATING_POINT_REGISTERS_STORED bit
** is set task's FLAGS, then this structure is valid.
*/
typedef struct psp_blocked_fp_struct {
    uint_32  FPCCR;     /* FP Context Control Register                  */
    uint_32  FPCAR;     /* FP Context Address Register                  */
    uint_32  FPDSCR;    /* FP Default Status Control Register           */
    uint_32  FPSCR;     /* FP Status Control Register                   */
    uint_32  TID;       /* Testing for correctness                      */
    float S[32];        /* The floating point computation registers     */
} PSP_BLOCKED_FP_STRUCT, _PTR_ PSP_BLOCKED_FP_STRUCT_PTR;

#endif

/*-----------------------------------------------------------------------*/
/*
** PSP INTERRUPT CONTEXT STRUCT
**
** This structure provides a "context" for mqx primitives to use while executing
** in an interrupt handler.  A link list of these contexts is built on the
** interrupt stack.  The head of this list (the current interrupt context) is
** pointed to by the INTERRUPT_CONTEXT_PTR field of the kernel data structure.
**
*/
typedef struct psp_int_context_struct
{
    /* Address of previous context, NULL if none */
    struct psp_int_context_struct _PTR_ PREV_CONTEXT;

    /* The exception number for this interrupt frame */
    uint_32                             EXCEPTION_NUMBER;

    /* Used by the _int_enable function while in the ISR */
    uint_32                             ENABLE_SR;

    /* The "task" error code for use by mqx functions while in the ISR */
    uint_32                             ERROR_CODE;
} PSP_INT_CONTEXT_STRUCT, _PTR_ PSP_INT_CONTEXT_STRUCT_PTR;


/*-----------------------------------------------------------------------*/
/*
** PSP BASIC INT FRAME STRUCT
**
** This structure is what is pushed onto the memory on the current stack
** when an interrupt occurs.
**
** For the first interrupt that interrupts a running task, this structure
** defines what the top of the stack for the task looks like,
** and the STACK_PTR field of the task descriptor will point to this structure.
**
** The rest of the interrupt frame is then built upon the interrupt stack.
**
** For a nested interrupt, this basic frame is also built upon the interrupt
** stack.
**
*/

typedef struct psp_basic_int_frame_struct {
    uint_32 R0;
    uint_32 R1;
    uint_32 R2;
    uint_32 R3;

    uint_32 R12;
    uint_32 LR;
    uint_32 PC;
    uint_32 PSR;
} PSP_BASIC_INT_FRAME_STRUCT, _PTR_ PSP_BASIC_INT_FRAME_STRUCT_PTR;

/*-----------------------------------------------------------------------*/
/*
** PSP STACK START STRUCT
**
** This structure is used during the initialization of a new task.
** It is overlaid on the bottom of the task's stack
**
*/
typedef struct psp_stack_start_struct
{
    /* The start up registers for the task */
    PSP_BLOCKED_STACK_STRUCT    INITIAL_CONTEXT;

    /* The end of INITIAL CONTEXT has to be aligned at 8B boundary. Presuming that the end of this structure is
    ** aligned at PSP_STACK_ALIGN boundary, we must add reserved bytes to achieve alignment.
    ** The reserved space is computed from the used space from the end (5 * 32-bit value).
    */
    uint_8                      RESERVED[(PSP_STACK_ALIGNMENT_MASK - 5 * sizeof(uint_32)) & 0x7];

    /* Previous stack pointer for exit return */
    pointer                     PREVIOUS_STACK_POINTER;

    /* The function to call when the task "returns" */
    void                        (_CODE_PTR_ EXIT_ADDRESS)();

    /* The task's parameter */
    uint_32                     PARAMETER;

    /* The following two fields are used to create a "bottom" of stack
    ** that debuggers will recognize
    */
    /* End stack pointer    */
    pointer                     ZERO_STACK_POINTER;

    /* close the stack frame with a return address of 0 */
    uint_32                     ZERO_RETURN_ADDRESS;

} PSP_STACK_START_STRUCT, _PTR_ PSP_STACK_START_STRUCT_PTR;

#define PSP_STACK_PARAMETER PARAMETER


/*--------------------------------------------------------------------------*/
/*
**                  PROTOTYPES OF PSP FUNCTIONS
*/


/*
** PROCESSOR MEMORY BOUNDS
*/
#define CORTEX_PERIPH_BASE                  (0x40000000ul)    // peripheral base address
#define CORTEX_PRI_PERIPH_IN_BASE           (0xe0000000ul)    // private peripheral internal base address

/* minimal implemented priority required by Cortex core */
#ifndef CORTEX_PRIOR_IMPL
    #if PSP_MQX_CPU_IS_ARM_CORTEX_M0P
        #define CORTEX_PRIOR_IMPL           (1)
    #elif PSP_MQX_CPU_IS_ARM_CORTEX_M4
        #define CORTEX_PRIOR_IMPL           (3)
    #endif
#endif /* CORTEX_PRIOR_IMPL */

#define CORTEX_PRIOR_SHIFT                  (8 - CORTEX_PRIOR_IMPL)
#define CORTEX_PRIOR_MASK                   ((0xff << CORTEX_PRIOR_SHIFT) & 0xff)
#define CORTEX_PRIOR(x)                     (((x) << CORTEX_PRIOR_SHIFT) & CORTEX_PRIOR_MASK)

#define CORTEX_INT_FIRST_INTERNAL       0

#ifndef CORTEX_INT_LAST_INTERNAL
#if PSP_MQX_CPU_IS_ARM_CORTEX_M0P
    #define CORTEX_INT_LAST_INTERNAL        (32)
#elif PSP_MQX_CPU_IS_ARM_CORTEX_M4
    #define CORTEX_INT_LAST_INTERNAL        (250)
#endif
#endif /* CORTEX_INT_LAST_INTERNAL */

#define PSP_INT_FIRST_INT_ROUTER		16

#define PSP_INT_FIRST_INTERNAL              (CORTEX_INT_FIRST_INTERNAL)
#define PSP_INT_LAST_INTERNAL               (CORTEX_INT_LAST_INTERNAL)

#define CORTEX_MPU_REC                      (12)

#define MPU_SM_RWX                          (0)
#define MPU_SM_RX                           (1)
#define MPU_SM_RW                           (2)
#define MPU_SM_AS_USER                      (3)

#define MPU_UM_R                            (4)
#define MPU_UM_W                            (2)
#define MPU_UM_X                            (1)

#define MPU_UM_RW                           (MPU_UM_R | MPU_UM_W)

/*
** CORTEX_SCS_STRUCT
** Reset and Clock Control
*/
typedef struct cortex_syst_struct {
    uint_32 CSR;      /* SysTick Control and Status */
    uint_32 RVR;      /* SysTick Reload value       */
    uint_32 CVR;      /* SysTick Current value      */
    uint_32 CALIB;    /* SysTick Calibration value  */
} CORTEX_SYST_STRUCT, _PTR_ CORTEX_SYST_STRUCT_PTR;
typedef volatile struct cortex_syst_struct _PTR_ VCORTEX_SYST_STRUCT_PTR;

/*
** CORTEX_NVIC_STRUCT
** Reset and Clock Control
*/
typedef struct cortex_nvic_struct {
    uint_32 ENABLE[32];
    uint_32 DISABLE[32];
    uint_32 SET[32];
    uint_32 CLR[32];
    uint_32 ACTIVE[32];
    uint_32 rsvd[32];
    uint_32 PRIORITY[32];
} CORTEX_NVIC_STRUCT, _PTR_ CORTEX_NVIC_STRUCT_PTR;
typedef volatile struct cortex_nvic_struct _PTR_ VCORTEX_NVIC_STRUCT_PTR;

/*
** CORTEX_SCB_STRUCT
** system control block
*/
typedef struct cortex_scb_struct {
    uint_32 CPUID;
    uint_32 ICSR;
    uint_32 VTOR;
    uint_32 AIRCR;
    uint_32 SCR;
    uint_32 CCR;
    uint_32 SHPR1;
    uint_32 SHPR2;
    uint_32 SHPR3;
    uint_32 SHCRS;
    uint_32 CFSR;
    uint_32 HFSR;
} CORTEX_SCB_STRUCT, _PTR_ CORTEX_SCB_STRUCT_PTR;
typedef volatile struct cortex_scb_struct _PTR_ VCORTEX_SCB_STRUCT_PTR;

/*
** CORTEX__SCS_STRUCT
** This structure defines the memory/registers provided by the CORTEX core
** 0xE0000000 - 0xE0100000
*/
typedef struct cortex_scs_struct
{
    uchar           filler0[0xe010];
    CORTEX_SYST_STRUCT      SYST;
    uchar           filler1[0xE000E100 - (0xE000E010 + sizeof(CORTEX_SYST_STRUCT))];
    CORTEX_NVIC_STRUCT      NVIC;
    uchar           filler2[0xE000ED00 - (0xE000E100 + sizeof(CORTEX_NVIC_STRUCT))];
    CORTEX_SCB_STRUCT       SCB;
} CORTEX_SCS_STRUCT, _PTR_ CORTEX_SCS_STRUCT_PTR;
typedef volatile struct cortex_scs_struct _PTR_ VCORTEX_SCS_STRUCT_PTR;

#define _PSP_GET_VTOR()     (_mqx_max_type)(((VCORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->SCB.VTOR);
#define _PSP_SET_VTOR(x)    ((VCORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->SCB.VTOR = ((uint_32)(x));



/* Interrupt Acknowledge Level and Priority Register
**
** Interrupt priority can range 0 (lowest) to 7 (highest priority) and
** a special value 8 (mid-point) for fixed level interrupts
*/
typedef uint_8 _int_level;
typedef uint_8 _int_priority;

/*--------------------------------------------------------------------------*/
/*
**                  PROTOTYPES OF PSP FUNCTIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Generic PSP prototypes */
_mqx_uint _psp_int_init(_mqx_uint, _mqx_uint);
void _psp_int_install(void);
void __set_MSP(uint_32);

uint_32 _psp_core_num(void);

/* Enable and disable ALL interrupts on ARM CortexM processors.
  Some compilers export this function as pre-defined macro                    */
#ifndef __enable_interrupt
extern void         __enable_interrupt(void);
#endif
#ifndef __disable_interrupt
extern void         __disable_interrupt(void);
#endif

#endif // __ASM__

#ifdef __cplusplus
}
#endif

#endif

