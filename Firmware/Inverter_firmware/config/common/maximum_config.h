/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 20089 Embedded Access Inc.;
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
* $FileName: maximum_config.h$
* $Version : 3.8.5.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   Smallest configuration for MQX components
*
*END************************************************************************/


#ifndef __maximum_config_h__
#define __maximum_config_h__

/* 
** When MQX_CHECK_ERRORS is defined as 1, 
** kernel functions will perform error checking on their parameters.
*/
#ifndef MQX_CHECK_ERRORS
#define MQX_CHECK_ERRORS 1
#endif


/* 
** When MQX_CHECK_MEMORY_ALLOCATION_ERRORS is defined as 1, 
** kernel functions will check all memory allocations for errors. 
** The functions will verify that the memory allocation 
** was successful.
*/
#ifndef MQX_CHECK_MEMORY_ALLOCATION_ERRORS
#define MQX_CHECK_MEMORY_ALLOCATION_ERRORS 1
#endif


/* 
** When MQX_CHECK_VALIDITY is defined as 1, 
** kernel functions will check the the VALIDITY fields of structures
** as they are accessed.
*/
#ifndef MQX_CHECK_VALIDITY
#define MQX_CHECK_VALIDITY 1
#endif


/* 
** When MQX_COMPONENT_DESTRUCTION is defined as 1,
** the kernel will handle the destruction of a kernel component object
** (such as a semaphore or event)
*/
#ifndef MQX_COMPONENT_DESTRUCTION
#define MQX_COMPONENT_DESTRUCTION 1
#endif


/* 
** When MQX_IO_COMPONENT_DESTRUCTION is defined as 1,
** the kernel will handle the destruction of an IO component object
*/
#ifndef MQX_IO_COMPONENT_DESTRUCTION
#define MQX_IO_COMPONENT_DESTRUCTION 1
#endif


/*
** When MQX_DEFAULT_TIME_SLICE_IN_TICKS is define as 1,
** then the default time slice in the task template structure
** is in units of ticks. Changing this to 0 will change the units
** to milliseconds. This also affects the time slice field in the
** task template since this is used to set a task's default time
** slice.
*/
#ifndef MQX_DEFAULT_TIME_SLICE_IN_TICKS
#define MQX_DEFAULT_TIME_SLICE_IN_TICKS  0
#endif


/* 
** When MQX_EXIT_ENABLED is defined as 1,
** code is compiled in to allow the application to return from _mqx().
*/
#ifndef MQX_EXIT_ENABLED
#define MQX_EXIT_ENABLED 1
#endif


/*
** When MQX_EXTRA_TASK_STACK_ENABLE is defined as 1,
** code is compiled in to reserve extra memory at the top of stack 
** in every task, if desired. Both "OS Changer" and the MetaWare C/C++
** runtime want additional per-task variables.
*/
#ifndef MQX_EXTRA_TASK_STACK_ENABLE
#define MQX_EXTRA_TASK_STACK_ENABLE 1
#endif


/* 
** When MQX_HAS_TIME_SLICE is defined as 1,
** then code is compiled in to support time sliced tasks.
*/
#ifndef MQX_HAS_TIME_SLICE
#define MQX_HAS_TIME_SLICE 1
#endif


/* 
** When MQX_INCLUDE_FLOATING_POINT_IO is defined as 1,
** _io_printf and _io_scanf will include floating point I/O code.
*/
#ifndef MQX_INCLUDE_FLOATING_POINT_IO
#define MQX_INCLUDE_FLOATING_POINT_IO 1
#endif


/* 
** When MQX_IS_MULTI_PROCESSOR is defined as 1,
** then code is compiled in to support multiple processor MQX systems.
*/
#ifndef MQX_IS_MULTI_PROCESSOR
#define MQX_IS_MULTI_PROCESSOR 1
#endif


/* 
** When MQX_KERNEL_LOGGING is defined as 1, 
** calls to write to the kernel log will be added to some kernel functions at 
** function entry (with parameters) and function exit (with error codes).
**
** The MQX functions that have logging compiled in will run slower, however
** it is only when the logging is enabled for the function that performance
** will be affected.
**
** The logging of specific functions can be controlled using the 
** _klog_control function.
**
** The default value of this define is dependent upon the PSP. Please see
** source\psp\<psp src dir>\psptypes.h
*/
#ifndef MQX_KERNEL_LOGGING
#define MQX_KERNEL_LOGGING   1
#endif


/*
** When MQX_LWLOG_TIME_STAMP_IN_TICKS is define as 0,
** then the time stamp in the light weight log component
** is in seconds, milliseconds, and microseconds. Changing this
** to 1 will causes the time stamp to be in ticks which is not
** as readable, but has much less overhead.
*/
#ifndef MQX_LWLOG_TIME_STAMP_IN_TICKS
#define MQX_LWLOG_TIME_STAMP_IN_TICKS  0
#endif


/* 
** When MQX_MEMORY_FREE_LIST_SORTED is defined as 1,
** then the free memory blocks are stored in order of address
** to reduce fragmentation. This can increase memory freeing time
** and code size
*/
#ifndef MQX_MEMORY_FREE_LIST_SORTED
#define MQX_MEMORY_FREE_LIST_SORTED   1
#endif


/*
** When MQX_MONITOR_STACK is defined as 1,
** the kernel will fill all stacks with a known value when the stack is 
** initialized.
** This allows utility software and debuggers to calculate how much of the 
** stack has been used.  This will only have an impact at task creation time.
*/
#ifndef MQX_MONITOR_STACK
#define MQX_MONITOR_STACK 1
#endif


/* 
** When MQX_MUTEX_HAS_POLLING is defined as 1,
** then extra code will be compiled in to support the
** mutex options: MUTEX_SPIN_ONLY and MUTEX_LIMITED_SPIN
*/
#ifndef MQX_MUTEX_HAS_POLLING
#define MQX_MUTEX_HAS_POLLING 1
#endif


/* 
** When MQX_PROFILING_ENABLE is defined as 1,
** code to support an external profiling tool will be compiled into the kernel
** Profiling will add to the size of the compiled image, and it will run slower
**
** Note: You can only use profiling if the toolset being used supports it
**
** The setting of this define must be the same as the definition by the same name,
** in source\psp\<psp>\psp_cnfg.<comp>
*/
#ifndef MQX_PROFILING_ENABLE
#define MQX_PROFILING_ENABLE 1
#endif


/* 
** When MQX_RUN_TIME_ERR_CHECK_ENABLE is defined as 1,
** code to support an external run-time error checking tool will be compiled
** into the kernel.  This will add to the size of the compiled image,
** and will cause MQX to run slower
**
** Note: You can only use run time error checking if the toolset being used 
**       supports it
**
** The setting of this define must be the same as the definition by the same name,
** in source\psp\<psp>\psp_cnfg.<comp>
*/
#ifndef MQX_RUN_TIME_ERR_CHECK_ENABLE
#define MQX_RUN_TIME_ERR_CHECK_ENABLE 1
#endif

#ifndef MQX_THREAD_LOCAL_STORAGE_ENABLE
#define MQX_THREAD_LOCAL_STORAGE_ENABLE 1
#endif

#ifndef MQX_TAD_RESERVED_ENABLE
#define MQX_TAD_RESERVED_ENABLE 1
#endif

/* 
** When MQX_TASK_CREATION_BLOCKS is defined as 1, 
** the creating task will be suspended when _task_create is called for a task 
** residing on a different processor.
**
** The creator task will be suspended until the target task is created, 
** and an error code is returned.
*/
#ifndef MQX_TASK_CREATION_BLOCKS
#define MQX_TASK_CREATION_BLOCKS 1
#endif


/* 
** When MQX_TASK_DESTRUCTION is defined as 1,
** the kernel will allow for the destruction of tasks,
** releasing all resources owned by the task when it is destroyed.
** (memory, events, semaphores, mutexes etc.)
*/
#ifndef MQX_TASK_DESTRUCTION
#define MQX_TASK_DESTRUCTION 1
#endif


/* 
** When MQX_TIMER_USES_TICKS_ONLY, the timer task will *only* process
** timer periodic and one-shot requests using ticks for timeout
** reporting, rather than the MQX2.40 milliseconds/seconds.
*/
#ifndef MQX_TIMER_USES_TICKS_ONLY
#define MQX_TIMER_USES_TICKS_ONLY 1
#endif


/* 
** When MQX_USE_32BIT_MESSAGE_QIDS is defined as 1,
** the message component datatypes (_queue_numbe & _queue_id) will be uint_32s 
** wide instead of uint_16s.
** This will allow for more than 256 message queues on a cpu, and
** more than 256 processors in a multi-processor network.
*/
#ifndef MQX_USE_32BIT_MESSAGE_QIDS
#define MQX_USE_32BIT_MESSAGE_QIDS 0
#endif


/* 
** When MQX_USE_32BIT_TYPES is defined as 1, the kernel will be forced
** to 32 bit mode regardless of the natural size of the processor. This will
** reduce the number of warning generated by the compiler when linking 2.4x
** applications against versio 2.5x of MQX.
*/
#ifndef MQX_USE_32BIT_TYPES
#define MQX_USE_32BIT_TYPES 0
#endif


/* 
** When MQX_USE_EVENTS is defined as 1,
** then mqx will compile in the support code for event component.
*/
#ifndef MQX_USE_EVENTS
#define MQX_USE_EVENTS 1
#endif


/* 
** When MQX_USE_IDLE_TASK is defined as 1,
** the kernel will create the idle task otherwise, no actual
** task will run when one is not ready to run.
*/
#ifndef MQX_USE_IDLE_TASK
#define MQX_USE_IDLE_TASK 1
#endif


/* 
** When MQX_USE_IPC is defined as 1,
** then mqx will compile in the support code for Interprocessor communication.
*/
#ifndef MQX_USE_IPC
#define MQX_USE_IPC 1
#endif


/* 
** When MQX_USE_INLINE_MACROS is defined as 1,
** certain internal utility functions called by MQX will
** be made "inline" rather than by function calls.  This allows the user
** to optimize for time or code space.  Inlining optimizes for time,
** not inlining optimizes for space.
*/
#ifndef MQX_USE_INLINE_MACROS
#define MQX_USE_INLINE_MACROS 1
#endif


/* 
** When MQX_USE_LWEVENTS is defined as 1,
** then mqx will compile in the support code for light weight event component.
*/
#ifndef MQX_USE_LWEVENTS
#define MQX_USE_LWEVENTS 1
#endif


/* 
** When MQX_USE_LOGS is defined as 1,
** then mqx will compile in the support code for log component.
*/
#ifndef MQX_USE_LOGS
#define MQX_USE_LOGS 1
#endif


/* 
** When MQX_USE_LWLOGS is defined as 1,
** then mqx will compile in the support code for light weight log component.
*/
#ifndef MQX_USE_LWLOGS
#define MQX_USE_LWLOGS 1
#endif

/* 
** When MQX_USE_LWMEM_ALLOCATOR is defined as 1,
** then mqx will use the light weight memory functions
** to allocate and free memory 
*/
#ifndef MQX_USE_LWMEM_ALLOCATOR
#define MQX_USE_LWMEM_ALLOCATOR 0
#endif

/* 
** When MQX_USE_LWMEM is defined as 1,
** then mqx will compile in the support code for light weight memory component.
*/
#ifndef MQX_USE_LWMEM
#define MQX_USE_LWMEM 1
#endif


/* 
** When MQX_USE_MEM is defined as 1,
** then mqx will compile in the support code for memory component.
*/
#ifndef MQX_USE_MEM
    #if MQX_USE_LWMEM_ALLOCATOR
        #define MQX_USE_MEM         0
    #else
        #define MQX_USE_MEM         1
    #endif
#endif

/* 
** When MQX_USE_UNCACHED_MEM is defined as 1,
** then mqx will compile with support for uncached memory allocation.
*/
#ifndef MQX_USE_UNCACHED_MEM
#define MQX_USE_UNCACHED_MEM         1
#endif 

/* 
** When MQX_USE_LWMSGQ is defined as 1,
** then mqx will compile in the support code for light weight message queues.
*/
#ifndef MQX_USE_LWMSGQ
#define MQX_USE_LWMSGQ 1
#endif


/* 
** When MQX_USE_LWTIMER is defined as 1,
** then mqx will compile in the support code for light weight timers.
*/
#ifndef MQX_USE_LWTIMER
#define MQX_USE_LWTIMER 1
#endif


/* 
** When MQX_USE_MESSAGES is defined as 1,
** then mqx will compile in the support code for messages.
*/
#ifndef MQX_USE_MESSAGES
#define MQX_USE_MESSAGES 1
#endif


/* 
** When MQX_USE_MUTEXES is defined as 1,
** then mqx will compile in the support code for mutex component.
*/
#ifndef MQX_USE_MUTEXES
#define MQX_USE_MUTEXES 1
#endif


/* 
** When MQX_USE_NAME is defined as 1,
** then mqx will compile in the support code for name component.
*/
#ifndef MQX_USE_NAME
#define MQX_USE_NAME 1
#endif


/* 
** When MQX_USE_PARTITIONS is defined as 1,
** then mqx will compile in the support code for memory with fixed-size blocks.
*/
#ifndef MQX_USE_PARTITIONS
#define MQX_USE_PARTITIONS 1
#endif


/* 
** When MQX_USE_SEMAPHORES is defined as 1,
** then mqx will compile in the support code for Semaphores
*/
#ifndef MQX_USE_SEMAPHORES
#define MQX_USE_SEMAPHORES 1
#endif


/* 
** When MQX_USE_SW_WATCHDOGS is defined as 1,
** then mqx will compile in the support code for software watchdog timers.
*/
#ifndef MQX_USE_SW_WATCHDOGS
#define MQX_USE_SW_WATCHDOGS 1
#endif


/* 
** When MQX_USE_TIMER is defined as 1,
** then mqx will compile in the support code for timer component.
*/
#ifndef MQX_USE_TIMER
#define MQX_USE_TIMER 1
#endif


/* 
** When MQX_VERIFY_KERNEL_DATA is defined as 1,
** then mqx will perform a memory check at startup to
** verify it can correcly read and write kernel memory.
*/
#ifndef MQX_VERIFY_KERNEL_DATA
#define MQX_VERIFY_KERNEL_DATA 1
#endif


/* 
** When MQX_ALLOW_TYPED_MEMORY is defined as 1,
** then mqx will include code that allows types memory blocks
*/
#ifndef MQX_ALLOW_TYPED_MEMORY
#define MQX_ALLOW_TYPED_MEMORY 1
#endif

/* Set _mem_alloc_align mode - default is 0.
** 0 create free mem block before allocated block for alignment.
** 1 resize previous used block for alignment.
*/
#ifndef MQX_ALLOC_ALLIGN_RESIZE
#define MQX_ALLOC_ALLIGN_RESIZE 0
#endif

#ifndef MQX_ROM_VECTORS
#define MQX_ROM_VECTORS 1
#endif

#ifndef MQX_SPARSE_ISR_TABLE
#define MQX_SPARSE_ISR_TABLE 0
#endif

#ifndef MQX_GUERRILLA_INTERRUPTS_EXIST
#define MQX_GUERRILLA_INTERRUPTS_EXIST  0
#endif

#endif
/* EOF */
