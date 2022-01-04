/*HEADER*********************************************************************
*
* Copyright (c) 2008-2012 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: mqx.h$
* $Version : 3.8.94.0$
* $Date    : Jul-27-2012$
*
* Comments:
*
*   This file contains the structure definitions and constants for an
*   application which will be using MQX.
*   All compiler provided header files must be included before mqx.h.
*
*END************************************************************************/

#ifndef __mqx_h__
#define __mqx_h__


#if defined(__CWBUILD__)
/* CW 10.1 compilation issue - switch off peephole optimization */
#if  (__CWBUILD__ == 3009)
#pragma peephole off
#pragma no_register_coloring on
#endif

/* CW 10.1 Cortex compiler 2402 build patch */
#if (__arm__)
#if (__CWBE__ < 0x05000200)
#error  You need to install Code Warrior V10.1 latest compiler patch. The code is unstable if patch is not used.
#endif
#endif

#endif

/* In EWL version 15.0 tolower and toupper functionality was swapped */
#if defined(__EWL__)
#if __EWL__ == 0x00015000
#include "ctype.h"
inline int  _mqx_tolower(int c) { return isupper(c) ? c + ('a' - 'A') : c; }
inline int  _mqx_toupper(int c) { return islower(c) ? c - ('a' - 'A') : c; }
#define tolower _mqx_tolower
#define toupper _mqx_toupper
#endif
#endif


/*--------------------------------------------------------------------------*/
/*
 *                         MQX VERSION NUMBER
 */
#define MQX_VERSION (401)
#define __MQX__

/*--------------------------------------------------------------------------*/
/*
 *                   INCLUDE MQX CONFIGURATION CONSTANTS
 */

#include "mqx_cnfg.h"

/*--------------------------------------------------------------------------*/
/*
 *                 INCLUDE MQX STANDARD TYPES FROM THE PSP
 */

#include "psptypes.h"


/*--------------------------------------------------------------------------*/
/*
 *  INCLUDE COMPILER SPECIFIC DEFINITIONS, MACROS AND SUPPORT FUNCTIONS
 */

#include <psp_comp.h>
#include "psp_supp.h"

/*--------------------------------------------------------------------------*/
/*
 *                 INCLUDE MQX IO COMPONENT CONSTANTS
 */

#include "mqx_ioc.h"



/*--------------------------------------------------------------------------*/
/*
 *                 INCLUDE VARIOUS MACROS
 */

#include "mqx_macros.h"


/*--------------------------------------------------------------------------*/
/*
 *                          STANDARD CONSTANTS
 *
 *  Note that if standard 'C' library files are included after mqx.h,
 *  the defines of TRUE, FALSE and NULL may sometimes conflict, as most
 *  standard library files do not check for previous definitions.
 */

#ifdef  FALSE
   #undef  FALSE
#endif
#define FALSE ((boolean)0)

#ifdef  TRUE
   #undef  TRUE
#endif
#define TRUE ((boolean)1)

#ifdef  NULL
   #undef  NULL
#endif
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((pointer)0)
#endif


#ifndef __ASM__
/*--------------------------------------------------------------------------*/
/*
 *                    TASK IDS AND PROCESSOR NUMBERS
 *
 * These definitions are for MQX task IDs and processor numbers.
 */

typedef uint_16 _task_number;       /* what a task_number looks like  */
typedef uint_32 _task_id;           /* what a task_id looks like  */

#define MQX_NULL_TASK_ID        ((_task_id)0)
#define MQX_DEFAULT_TASK_ID     ((_task_id)-1)
#define MQX_IDLE_TASK_NAME      "_mqx_idle_task"
#define MQX_IDLE_TASK_PARAMETER 0
#if (MQX_USE_IDLE_TASK) && (MQX_ENABLE_LOW_POWER) && (MQX_IDLE_TASK_PARAMETER != 0)
   #error MQX_IDLE_TASK_PARAMETER must be 0 when Low Power feature is enabled!
#endif

typedef uint_16 _processor_number;  /* what a processor number is */

/*
 * Valid processor numbers run from 1 to 255 for 16 bit queue IDs or
 * 1 to 65535 for 32 bit queue IDs
 */
#if MQX_USE_32BIT_MESSAGE_QIDS == 0
   #define MQX_MAX_PROCESSOR_NUMBER  ((_processor_number)255)
#else
   #define MQX_MAX_PROCESSOR_NUMBER  ((_processor_number)65535)
#endif

#endif

/*--------------------------------------------------------------------------*/
/*
 *                       MEMORY ENDIAN DEFINITIONS
 *
 *  A PSP will define in its header file PSP_ENDIAN to be one of these
 */
#define MQX_BIG_ENDIAN    (0)
#define MQX_LITTLE_ENDIAN (1)


/*--------------------------------------------------------------------------*/
/*
 *                          ERROR CODE BASES
 *
 *  These are the base values of various MQX component error codes
 */
#define MQX_ERROR_BASE         (0x0000)
#define RTCS_ERROR_BASE        (0x1000)
#define IO_PCB_ERROR_BASE      (0x2000)
#define FS_ERROR_BASE          (0x3000)
#define USB_ERROR_BASE         (0x4000)
#define SHELL_ERROR_BASE       (0x5000)
#define IPC_PCB_ERROR_BASE     (0x9000)
#define DRIVER_ERROR_BASE      (0xA000)
#define IPC_ERROR_BASE         (0xC000)

#define SEM_ERROR_BASE         (MQX_ERROR_BASE | 0x0100)
#define EDS_SERIAL_ERROR_BASE  (MQX_ERROR_BASE | 0x0200)
#define EVENT_ERROR_BASE       (MQX_ERROR_BASE | 0x0300)
#define POSIX_ERROR_BASE       (MQX_ERROR_BASE | 0x0400)
#define LOG_ERROR_BASE         (MQX_ERROR_BASE | 0x0500)
#define WATCHDOG_ERROR_BASE    (MQX_ERROR_BASE | 0x0600)
#define MSG_ERROR_BASE         (MQX_ERROR_BASE | 0x0700)
#define PART_ERROR_BASE        (MQX_ERROR_BASE | 0x0800)
#define RESERVED2_ERROR_BASE   (MQX_ERROR_BASE | 0x0900)
#define IO_ERROR_BASE          (MQX_ERROR_BASE | 0x0A00)
#define RESERVED3_ERROR_BASE   (MQX_ERROR_BASE | 0x0B00)
#define RESERVED4_ERROR_BASE   (MQX_ERROR_BASE | 0x0C00)
#define RESERVED5_ERROR_BASE   (MQX_ERROR_BASE | 0x0D00)
#define RESERVED6_ERROR_BASE   (MQX_ERROR_BASE | 0x0E00)
#define NAME_ERROR_BASE        (MQX_ERROR_BASE | 0x0F00)

#define CAN_ERROR_BASE         (DRIVER_ERROR_BASE | 0x0100)
#define FLEXCAN_ERROR_BASE     (DRIVER_ERROR_BASE | 0x0200)
#define MSCAN_ERROR_BASE       (DRIVER_ERROR_BASE | 0x0300)
#define ENET_ERROR_BASE        (DRIVER_ERROR_BASE | 0x0400)
#define PCFLASH_ERROR_BASE     (DRIVER_ERROR_BASE | 0x0500)
#define QSPI_ERROR_BASE        (DRIVER_ERROR_BASE | 0x0600)
#define SPI_ERROR_BASE         (DRIVER_ERROR_BASE | 0x0700)
#define FLASHX_ERROR_BASE      (DRIVER_ERROR_BASE | 0x0800)
#define I2C_ERROR_BASE         (DRIVER_ERROR_BASE | 0x0900)
#define SERIAL_ERROR_BASE      (DRIVER_ERROR_BASE | 0x0A00)
#define NANDFLASH_ERROR_BASE   (DRIVER_ERROR_BASE | 0x0B00)
#define ESDHC_ERROR_BASE       (DRIVER_ERROR_BASE | 0x0C00)
#define TCHRES_ERROR_BASE      (DRIVER_ERROR_BASE | 0x0D00)
#define ADC_ERROR_BASE         (DRIVER_ERROR_BASE | 0x0E00)
#define DAC_ERROR_BASE         (DRIVER_ERROR_BASE | 0x0F00)
#define RTC_ERROR_BASE         (DRIVER_ERROR_BASE | 0x0F80)
#define COREMUTEX_ERROR_BASE   (DRIVER_ERROR_BASE | 0x0F90)


/*--------------------------------------------------------------------------*/
/*
 *                          FS ERROR CODES
 *
 *  These are the values of the error codes used by file systems
 */
#define FS_NO_ERROR                          (0)
#define FS_INVALID_FUNCTION_CODE             (FS_ERROR_BASE|1)
#define FS_FILE_NOT_FOUND                    (FS_ERROR_BASE|2)
#define FS_PATH_NOT_FOUND                    (FS_ERROR_BASE|3)
#define FS_ACCESS_DENIED                     (FS_ERROR_BASE|5)
#define FS_INVALID_HANDLE                    (FS_ERROR_BASE|6)
#define FS_INSUFFICIENT_MEMORY               (FS_ERROR_BASE|8)
#define FS_INVALID_MEMORY_BLOCK_ADDRESS      (FS_ERROR_BASE|9)
#define FS_ATTEMPT_TO_REMOVE_CURRENT_DIR     (FS_ERROR_BASE|16)
#define FS_DISK_IS_WRITE_PROTECTED           (FS_ERROR_BASE|19)
#define FS_BAD_DISK_UNIT                     (FS_ERROR_BASE|20)
#define FS_INVALID_LENGTH_IN_DISK_OPERATION  (FS_ERROR_BASE|24)
#define FS_NOT_A_DOS_DISK                    (FS_ERROR_BASE|26)
#define FS_SECTOR_NOT_FOUND                  (FS_ERROR_BASE|27)
#define FS_WRITE_FAULT                       (FS_ERROR_BASE|29)
#define FS_READ_FAULT                        (FS_ERROR_BASE|30)
#define FS_SHARING_VIOLATION                 (FS_ERROR_BASE|32)
#define FS_FILE_EXISTS                       (FS_ERROR_BASE|80)
#define FS_ALREADY_ASSIGNED                  (FS_ERROR_BASE|85)
#define FS_INVALID_PARAMETER                 (FS_ERROR_BASE|87)
#define FS_DISK_FULL                         (FS_ERROR_BASE|101)
#define FS_ROOT_DIR_FULL                     (FS_ERROR_BASE|102)
#define FS_EOF                               (FS_ERROR_BASE|103)
#define FS_CANNOT_CREATE_DIRECTORY           (FS_ERROR_BASE|104)
#define FS_NOT_INITIALIZED                   (FS_ERROR_BASE|151)
#define FS_OPERATION_NOT_ALLOWED             (FS_ERROR_BASE|152)
#define FS_ERROR_INVALID_DRIVE_HANDLE        (FS_ERROR_BASE|157)
#define FS_ERROR_INVALID_FILE_HANDLE         (FS_ERROR_BASE|158)
#define FS_ERROR_UNKNOWN_FS_VERSION          (FS_ERROR_BASE|159)
#define FS_LOST_CHAIN                        (FS_ERROR_BASE|160)
#define FS_INVALID_DEVICE                    (FS_ERROR_BASE|161)
#define FS_INVALID_CLUSTER_NUMBER            (FS_ERROR_BASE|162)
#define FS_FAILED_TO_DELETE_LFN              (FS_ERROR_BASE|163)
#define FS_BAD_LFN_ENTRY                     (FS_ERROR_BASE|164)
#define FS_PMGR_INVALID_PARTITION            (FS_ERROR_BASE|170)
#define FS_PMGR_INSUF_MEMORY                 (FS_ERROR_BASE|171)
#define FS_PMGR_UNKNOWN_PARTITION            (FS_ERROR_BASE|172)
#define FS_PMGR_INVALID_PARTTABLE            (FS_ERROR_BASE|173)


/*--------------------------------------------------------------------------*/
/*
 *                          TASK ERROR CODES
 *
 *  These are the values of the task error codes used by Kernel
 */
#define MQX_OK                              (0)

#define MQX_INVALID_POINTER                 (MQX_ERROR_BASE|0x01)
#define MQX_INVALID_SIZE                    (MQX_ERROR_BASE|0x02)
#define MQX_NOT_RESOURCE_OWNER              (MQX_ERROR_BASE|0x03)
#define MQX_OUT_OF_MEMORY                   (MQX_ERROR_BASE|0x04)
#define MQX_CORRUPT_MEMORY_SYSTEM           (MQX_ERROR_BASE|0x05)
#define MQX_CORRUPT_STORAGE_POOL            (MQX_ERROR_BASE|0x06)
#define MQX_CORRUPT_STORAGE_POOL_FREE_LIST  (MQX_ERROR_BASE|0x07)
#define MQX_CORRUPT_STORAGE_POOL_POINTERS   (MQX_ERROR_BASE|0x08)
#define MQX_INVALID_CHECKSUM                (MQX_ERROR_BASE|0x09)
#define MQX_OUT_OF_TASK_DESCRIPTORS         (MQX_ERROR_BASE|0x0A)
#define MQX_INVALID_MEMORY_BLOCK            (MQX_ERROR_BASE|0x0B)
#define MQX_INVALID_PARAMETER               (MQX_ERROR_BASE|0x0C)
#define MQX_CANNOT_CALL_FUNCTION_FROM_ISR   (MQX_ERROR_BASE|0x0D)
#define MQX_INVALID_TASK_PRIORITY           (MQX_ERROR_BASE|0x0E)
#define MQX_TASK_QUEUE_EMPTY                (MQX_ERROR_BASE|0x0F)
#define MQX_NO_TASK_TEMPLATE                (MQX_ERROR_BASE|0x10)
#define MQX_INVALID_TASK_STATE              (MQX_ERROR_BASE|0x11)
#define MQX_INVALID_TASK_ID                 (MQX_ERROR_BASE|0x12)
#define MQX_INVALID_PROCESSOR_NUMBER        (MQX_ERROR_BASE|0x13)
#define MQX_INVALID_VECTORED_INTERRUPT      (MQX_ERROR_BASE|0x14)
#define MQX_INVALID_TEMPLATE_INDEX          (MQX_ERROR_BASE|0x15)
#define MQX_INVALID_CONFIGURATION           (MQX_ERROR_BASE|0x16)

/* Kernel component error codes */
#define MQX_COMPONENT_EXISTS                (MQX_ERROR_BASE|0x17)
#define MQX_COMPONENT_DOES_NOT_EXIST        (MQX_ERROR_BASE|0x18)
#define MQX_INVALID_COMPONENT_HANDLE        (MQX_ERROR_BASE|0x19)
#define MQX_INVALID_COMPONENT_BASE          (MQX_ERROR_BASE|0x1A)
#define MQX_INVALID_COMPONENT_NAME          (MQX_ERROR_BASE|0x1B)
#define MQX_INVALID_HANDLE                  (MQX_ERROR_BASE|0x1C)

/* Test error codes */
#define MQX_CORRUPT_QUEUE                   (MQX_ERROR_BASE|0x1D)
#define MQX_INVALID_TASK_QUEUE              (MQX_ERROR_BASE|0x1E)
#define MQX_INVALID_LWSEM                   (MQX_ERROR_BASE|0x1F)
#define MQX_CORRUPT_INTERRUPT_STACK         (MQX_ERROR_BASE|0x20)

/* Initialization error return codes */
#define MQX_KERNEL_MEMORY_TOO_SMALL         (MQX_ERROR_BASE|0x21)
#define MQX_COULD_NOT_CREATE_IPC_TASK       (MQX_ERROR_BASE|0x22)
#define MQX_TOO_MANY_PRIORITY_LEVELS        (MQX_ERROR_BASE|0x23)
#define MQX_TOO_MANY_INTERRUPTS             (MQX_ERROR_BASE|0x24)
#define MQX_DUPLICATE_TASK_TEMPLATE_INDEX   (MQX_ERROR_BASE|0x25)
#define MQX_TIMER_ISR_INSTALL_FAIL          (MQX_ERROR_BASE|0x26)

/* Scheduler error codes */
#define MQX_SCHED_INVALID_POLICY            (MQX_ERROR_BASE|0x27)
#define MQX_SCHED_INVALID_PARAMETER_PTR     (MQX_ERROR_BASE|0x28)
#define MQX_SCHED_INVALID_PARAMETER         (MQX_ERROR_BASE|0x29)
#define MQX_SCHED_INVALID_TASK_ID           (MQX_ERROR_BASE|0x2A)

/* I/O error codes */
#define MQX_INVALID_IO_CHANNEL              (MQX_ERROR_BASE|0x2B)
#define MQX_IO_OPERATION_NOT_AVAILABLE      (MQX_ERROR_BASE|0x2C)

/* IPC error codes */
#define MQX_INTER_PROCESSOR_INIT_FAILED     (MQX_ERROR_BASE|0x2D)
#define MQX_IPC_INVALID_MESSAGE             (MQX_ERROR_BASE|0x2E)
#define MQX_IPC_SERVICE_NOT_AVAILABLE       (MQX_ERROR_BASE|0x2F)
#define MQX_IPC_ROUTE_EXISTS                (MQX_ERROR_BASE|0x30)

/* User memory error codes */
#define MQX_MEM_POOL_TOO_SMALL              (MQX_ERROR_BASE|0x31)
#define MQX_MEM_POOL_INVALID                (MQX_ERROR_BASE|0x32)

/* MMU error codes */
#define MQX_OUT_OF_MMU_PAGE_TABLES          (MQX_ERROR_BASE|0x33)
#define MQX_MMU_CONTEXT_EXISTS              (MQX_ERROR_BASE|0x34)
#define MQX_MMU_CONTEXT_DOES_NOT_EXIST      (MQX_ERROR_BASE|0x35)
#define MQX_MMU_PARENT_TASK_CANNOT_BE_MMU   (MQX_ERROR_BASE|0x36)

/* LWSEM wait timeout error codes */
#define MQX_LWSEM_WAIT_TIMEOUT              (MQX_ERROR_BASE|0x37)

/* LWMEM error codes              */
#define MQX_LWMEM_POOL_INVALID              (MQX_ERROR_BASE|0x38)

/* LWEVENT error codes            */
#define MQX_LWEVENT_INVALID                 (MQX_ERROR_BASE|0x39)

/* LWTIMER error codes            */
#define MQX_LWTIMER_INVALID                 (MQX_ERROR_BASE|0x40)

/* UNHANDLED error codes            */
#define MQX_UNHANDLED_INTERRUPT             (MQX_ERROR_BASE|0x41)

/* RTC error codes */
#define MQX_RTC_UNLOCK_FAILED               (MQX_ERROR_BASE|0x42)

#define MQX_NO_USER_TASKS                   (MQX_ERROR_BASE|0x43)
#define MQX_TOO_MANY_USER_TASKS             (MQX_ERROR_BASE|0x44)
#define MQX_ACCESS_ERROR                    (MQX_ERROR_BASE|0x45)

#define MQX_INVALID_DEVICE                  (MQX_ERROR_BASE|0x46)
#define MQX_TASKQ_CREATE_FAILED             (MQX_ERROR_BASE|0x47)

#define MQX_ERROR                           (MQX_ERROR_BASE|0xff)

/*--------------------------------------------------------------------------*/
/*
 *                          POSIX ERROR CODES
 *
 * These are the POSIX error codes used by POSIX compliant functions
 * in MQX.
 */

#define _task_errno  (*_task_get_error_ptr())

#define MQX_EOK             (0x00)
#define MQX_E2BIG           (POSIX_ERROR_BASE|0x01)
#define MQX_EACCES          (POSIX_ERROR_BASE|0x02)
#define MQX_EAGAIN          (POSIX_ERROR_BASE|0x03)
#define MQX_EBADF           (POSIX_ERROR_BASE|0x04)
#define MQX_EBADMSG         (POSIX_ERROR_BASE|0x05)
#define MQX_EBUSY           (POSIX_ERROR_BASE|0x06)
#define MQX_ECANCELED       (POSIX_ERROR_BASE|0x07)
#define MQX_ECHILD          (POSIX_ERROR_BASE|0x08)
#define MQX_EDEADLK         (POSIX_ERROR_BASE|0x09)
#define MQX_EDOM            (POSIX_ERROR_BASE|0x0a)
#define MQX_EEXIST          (POSIX_ERROR_BASE|0x0b)
#define MQX_EFAULT          (POSIX_ERROR_BASE|0x0c)
#define MQX_EFBIG           (POSIX_ERROR_BASE|0x0d)
#define MQX_EINPROGRESS     (POSIX_ERROR_BASE|0x0e)
#define MQX_EINTR           (POSIX_ERROR_BASE|0x0f)
#define MQX_EINVAL          (POSIX_ERROR_BASE|0x10)
#define MQX_EIO             (POSIX_ERROR_BASE|0x11)
#define MQX_EISDIR          (POSIX_ERROR_BASE|0x12)
#define MQX_EMFILE          (POSIX_ERROR_BASE|0x13)
#define MQX_EMLINK          (POSIX_ERROR_BASE|0x14)
#define MQX_EMSGSIZE        (POSIX_ERROR_BASE|0x15)
#define MQX_ENAMETOOLONG    (POSIX_ERROR_BASE|0x16)
#define MQX_ENFILE          (POSIX_ERROR_BASE|0x17)
#define MQX_ENODEV          (POSIX_ERROR_BASE|0x18)
#define MQX_ENOENT          (POSIX_ERROR_BASE|0x19)
#define MQX_ENOEXEC         (POSIX_ERROR_BASE|0x1a)
#define MQX_ENOLCK          (POSIX_ERROR_BASE|0x1b)
#define MQX_ENOMEM          (POSIX_ERROR_BASE|0x1c)
#define MQX_ENOSPC          (POSIX_ERROR_BASE|0x1d)
#define MQX_ENOSYS          (POSIX_ERROR_BASE|0x1e)
#define MQX_ENOTDIR         (POSIX_ERROR_BASE|0x1f)
#define MQX_ENOTEMPTY       (POSIX_ERROR_BASE|0x20)
#define MQX_ENOTSUP         (POSIX_ERROR_BASE|0x21)
#define MQX_ENOTTY          (POSIX_ERROR_BASE|0x22)
#define MQX_ENXIO           (POSIX_ERROR_BASE|0x23)
#define MQX_EPERM           (POSIX_ERROR_BASE|0x24)
#define MQX_EPIPE           (POSIX_ERROR_BASE|0x25)
#define MQX_ERANGE          (POSIX_ERROR_BASE|0x26)
#define MQX_EROFS           (POSIX_ERROR_BASE|0x27)
#define MQX_ESPIPE          (POSIX_ERROR_BASE|0x28)
#define MQX_ESRCH           (POSIX_ERROR_BASE|0x29)
#define MQX_ETIMEDOUT       (POSIX_ERROR_BASE|0x2a)
#define MQX_EXDEV           (POSIX_ERROR_BASE|0x2b)


/*--------------------------------------------------------------------------*/
/*
 *                 TASK TEMPLATE ATTRIBUTES FLAGS
 *
 * These are bit flags or'd together to form the creation attributes for
 * a task. This is combined with INTERNAL TASK FLAGS.
 */

/*
 * Tasks with this attribute will be created automatically at
 * system initialization time.
 */
#define MQX_AUTO_START_TASK            (0x01)

/*
 * Tasks with this attribute will save the floating point co-processor
 * registers during context switching.
 * If the floating point registers are separate from the normal registers,
 * their context is managed independently during task switching. The registers
 * are saved/restored only when a new floating point task is scheduled to run.
 */
#define MQX_FLOATING_POINT_TASK        (0x02)

/*
 * Tasks with this attribute will use the time slice
 * scheduler within the kernel
 */
#if MQX_HAS_TIME_SLICE
#define MQX_TIME_SLICE_TASK            (0x04)
#endif

/*
 * Tasks with this attribute will save the DSP co-processor
 * registers during context switching.
 * If the DSP registers are separate from the normal registers,
 * their context is managed independently during task switching. The registers
 * are saved/restored only when a new DSP task is scheduled to run.
 */
#define MQX_DSP_TASK                   (0x08)

#if MQX_ENABLE_USER_MODE
#define MQX_USER_TASK                   (0x10)
#else
#define MQX_USER_TASK                   (0)
#endif


/*--------------------------------------------------------------------------*/
/*
 *                             TASK ID MACROS
 *
 * These macros manipulate the internals of a TASKID
 */

/* Obtain the processor number from the task ID */
#define PROC_NUMBER_FROM_TASKID(taskid) ((uint_16)((taskid) >> 16))

/* Obtain the task number from the task ID */
#define TASK_NUMBER_FROM_TASKID(taskid) ((uint_16)((taskid) & 0xFFFF))


/*--------------------------------------------------------------------------*/
/*
 *                        MONITOR DEFINITIONS
 *
 * These definitions are used by the MQX BSPs to determine which type of
 * monitor is currently running, and thus to modify their initialization
 * behaviour (Handling of caches, interrupts etc)
 * The variable _mqx_monitor_type is set to one of these by the application
 * program.  The default value is MONITOR_TYPE_NONE
 */

#define MQX_MONITOR_TYPE_NONE      (0)

/* Polled serial I/O monitor debugger */
#define MQX_MONITOR_TYPE_MON       (0x01)

/* Interrupt driven serial I/O monitor debugger */
#define MQX_MONITOR_TYPE_MON_INT   (0x02)

/* Background mode debugger being used */
#define MQX_MONITOR_TYPE_BDM       (0x03)

/* JTAG debugger being used */
#define MQX_MONITOR_TYPE_JTAG      (0x04)

/* Some other monitor in use */
#define MQX_MONITOR_TYPE_OTHER     (0x10)


/*--------------------------------------------------------------------------*/
/*
 *                       KERNEL SCHEDULER CONSTANTS
 */

/* Scheduling policy constants */
#define MQX_SCHED_FIFO                 (1)
#if MQX_HAS_TIME_SLICE
#define MQX_SCHED_RR                   (2)
#endif

/* Explicit scheduler task queueing policy */
#define MQX_TASK_QUEUE_FIFO           (0x00)
#define MQX_TASK_QUEUE_BY_PRIORITY    (0x08)
#define MQX_TASK_QUEUE_RESUME_ONE     FALSE
#define MQX_TASK_QUEUE_RESUME_ALL     TRUE

/*--------------------------------------------------------------------------*/
/*
 *                       TEST AND SET CONSTANTS
 *
 * These are the possible values returned by the MQX test and set function.
 * It is also used by the mutex functions as the value used when locking
 * a mutex.
 */
#define MQX_TEST_AND_SET_VALUE        (0x80)
#define MQX_TEST_AND_SET_NOT_ACQUIRED MQX_TEST_AND_SET_VALUE
#define MQX_TEST_AND_SET_ACQUIRED     (0x00)


/*--------------------------------------------------------------------------*/
/*
 *                       GENERIC CONSTANTS
 *
 */
/*
 * The maximum number of characters in a task's name. Includes terminating
 * /0 character.
 */
#define MQX_MAX_TASK_NAME_SIZE        (50)

/*--------------------------------------------------------------------------*/
/*
 *                       MEMORY CONSTANTS AND TYPE DEFINITIONS
 */
/* what a memory pool ID looks like  */
#ifndef __ASM__
typedef pointer _mem_pool_id;
#endif

#define MQX_NULL_MEM_POOL_ID    (NULL)

/* The minimum amount of memory needed by the memory management routines. */
#define MQX_MIN_MEMORY_POOL_SIZE (256)

#if !MQX_ENABLE_USER_MODE
#ifndef KERNEL_ACCESS
#define KERNEL_ACCESS
#endif
#ifndef USER_RW_ACCESS
#define USER_RW_ACCESS
#endif
#ifndef USER_RO_ACCESS
#define USER_RO_ACCESS
#endif
#ifndef USER_NO_ACCESS
#define USER_NO_ACCESS
#endif
#endif /* MQX_ENABLE_USER_MODE */
/*--------------------------------------------------------------------------*/
/*
 *                          DATA STRUCTURES
 */

/*--------------------------------------------------------------------------*/
/*
 *                            TIME CONSTANTS
 */
#define PICOSECS_IN_NANOSECOND   (1000)
#define NANOSECS_IN_MICROSECOND  (1000)
#define MICROSECS_IN_MILLISECOND (1000)
#define MILLISECS_IN_SECOND      (1000)
#define MICROSECS_IN_SECOND      (MILLISECS_IN_SECOND*MICROSECS_IN_MILLISECOND)
#define NANOSECS_IN_SECOND       (MICROSECS_IN_SECOND*NANOSECS_IN_MICROSECOND)
#define SECS_IN_MINUTE           (60UL)
#define MINUTES_IN_HOUR          (60UL)
#define HOURS_IN_DAY             (24UL)
#define MAX_DAYS_IN_MONTH        (31UL)
#define DAYS_IN_YEAR             (365UL)
#define MAX_DAYS_IN_YEAR         (366UL)
#define MONTHS_IN_YEAR           (12UL)
#define SECS_IN_HOUR             (SECS_IN_MINUTE*MINUTES_IN_HOUR)
#define SECS_IN_DAY              (SECS_IN_HOUR*HOURS_IN_DAY)
#define SECS_IN_YEAR             (SECS_IN_DAY*DAYS_IN_YEAR)
#define CLK_FIRST_YEAR           (1970UL)
#define XCLK_FIRST_YEAR          (1970)
#define CLK_LAST_YEAR            (2239UL)
#define XCLK_LAST_YEAR           (2481)
#define XCLK_ENTRIES             (512)
#define MAXIMUM_SECONDS_IN_TIME  (4102444800UL)
#define MAXIMUM_MILLISECONDS     (999UL)
#define MAXIMUM_DAYS_IN_XDATE    (187004UL)

/* This defines the mininum number of bits in the tick structure */
#define MQX_MIN_BITS_IN_TICK_STRUCT (64)

/* This defines the number of _mqx_uints in the tick structure */
#define MQX_NUM_TICK_FIELDS  ((MQX_MIN_BITS_IN_TICK_STRUCT+ \
   MQX_INT_SIZE_IN_BITS-1) / MQX_INT_SIZE_IN_BITS)

/*
 * Alarm frequency. This is for pre-2.50 BSPs
 * that do not have the define. New BSPs overide this define
 *
 *#ifndef BSP_ALARM_FREQUENCY
 *#define BSP_ALARM_FREQUENCY     (MILLISECS_IN_SECOND / BSP_ALARM_RESOLUTION)
 *#endif
 */

/*--------------------------------------------------------------------------*/
/*
 *                         MQX MEMORY TYPES
 */
#define MEM_TYPE_SYSTEM_STACK             1
#define MEM_TYPE_INTERRUPT_STACK          2
#define MEM_TYPE_INTERRUPT_TABLE          3
#define MEM_TYPE_TASK                     4
#define MEM_TYPE_READYQ                   5
#define MEM_TYPE_PARTITION_COMPONENT      6
#define MEM_TYPE_PARTITION                7
#define MEM_TYPE_PARTITION_BLOCKS         8
#define MEM_TYPE_MESSAGE_COMPONENT        9
#define MEM_TYPE_MESSAGE_QUEUES           10
#define MEM_TYPE_MESSAGE_POOLS            11
#define MEM_TYPE_MESSAGE_POOL_BLOCKS      12
#define MEM_TYPE_LWLOG_COMPONENT          13
#define MEM_TYPE_LWLOG                    14
#define MEM_TYPE_IO_DEVICE                15
#define MEM_TYPE_FILE_PTR                 16
#define MEM_TYPE_TASK_Q                   17
#define MEM_TYPE_EDS_BUFFER               18
#define MEM_TYPE_EVENT_COMPONENT          19
#define MEM_TYPE_EVENT_CONNECTION         20
#define MEM_TYPE_EVENT                    21
#define MEM_TYPE_LOG_COMPONENT            22
#define MEM_TYPE_LOG                      23
#define MEM_TYPE_MUTEX_COMPONENT          24
#define MEM_TYPE_NAME_COMPONENT           25
#define MEM_TYPE_SEMAPHORE_COMPONENT      26
#define MEM_TYPE_SEMAPHORE_CONNECTION     27
#define MEM_TYPE_SEMAPHORE                28
#define MEM_TYPE_TIMER_COMPONENT          29
#define MEM_TYPE_TIMER_ENTRY              30
#define MEM_TYPE_WATCHDOG_COMPONENT       31
#define MEM_TYPE_WATCHDOG_ALARM           32
#define MEM_TYPE_PSP_SUPPORT_STRUCT       33
#define MEM_TYPE_FP_CONTEXT               34
#define MEM_TYPE_INTERRUPT_VECTOR         35
#define MEM_TYPE_TD                       36
#define MEM_TYPE_TASK_STACK               37

#define MEM_TYPE_COMPONENT_SHIFT          12

#define MEM_TYPE_IO_PCFLASH               1001
#define MEM_TYPE_IO_SECTOR_BUFFER         1002
#define MEM_TYPE_IO_CHAN_STRUCT           1003
#define MEM_TYPE_IO_ENET_CONTEXT_STRUCT   1004
#define MEM_TYPE_IO_ENET_BUFFERS          1005
#define MEM_TYPE_IO_ENET_MCB              1006
#define MEM_TYPE_IO_ENET_ECB              1007
#define MEM_TYPE_IO_DUN                   1010
#define MEM_TYPE_IO_DUN_DEV               1011
#define MEM_TYPE_IO_GPIO_DEV_DATA         1012
#define MEM_TYPE_IO_GPIO_PIN_MAP          1013
#define MEM_TYPE_IO_MEM_STRUCT            1014
#define MEM_TYPE_IO_MEM_DATA              1015
#define MEM_TYPE_IO_SERIAL_INT_DEVICE_STRUCT    1016
#define MEM_TYPE_IO_SERIAL_POLLED_DEVICE_STRUCT 1017
#define MEM_TYPE_IO_SERIAL_IN_QUEUE       1018
#define MEM_TYPE_IO_SERIAL_OUT_QUEUE      1019
#define MEM_TYPE_IO_SERIAL_CHARQ          1020
#define MEM_TYPE_IO_SERIAL_INFO_STRUCT    1021
#define MEM_TYPE_IO_TFS_DRIVE_STRUCT      1022
#define MEM_TYPE_IO_SPI_INT_DEVICE_STRUCT    1023
#define MEM_TYPE_IO_SPI_POLLED_DEVICE_STRUCT 1024
#define MEM_TYPE_IO_SPI_IN_BUFFER            1025
#define MEM_TYPE_IO_SPI_OUT_BUFFER           1026
#define MEM_TYPE_IO_SPI_INFO_STRUCT          1027
#define MEM_TYPE_IO_FLASHX_RAM_FUNCT         1028
#define MEM_TYPE_IO_FLASHX_SECTOR_CACHE      1029
#define MEM_TYPE_IO_FLASHX_SECTOR_MAP        1030
#define MEM_TYPE_IO_I2C_INT_DEVICE_STRUCT    1031
#define MEM_TYPE_IO_I2C_POLLED_DEVICE_STRUCT 1032
#define MEM_TYPE_IO_I2C_IN_BUFFER            1033
#define MEM_TYPE_IO_I2C_OUT_BUFFER           1034
#define MEM_TYPE_IO_I2C_INFO_STRUCT          1035
#define MEM_TYPE_IO_PCB_PTR               1036
#define MEM_TYPE_IO_PCB_STRUCT            1037
#define MEM_TYPE_IO_BD_STRUCT                1038
#define MEM_TYPE_IO_ENET_MAC_CONTEXT_STRUCT  1039
#define MEM_TYPE_IO_SDCARD_STRUCT            1040
#define MEM_TYPE_IO_ESDHC_DEVICE_STRUCT      1041
#define MEM_TYPE_IO_ESDHC_INFO_STRUCT        1042
#define MEM_TYPE_IO_TCHRES_STRUCT            1043
#define MEM_TYPE_IO_TCHRES_XY_PIN_STRUCT     1044
#define MEM_TYPE_IO_USB_DCD_INFO_STRUCT          1045
#define MEM_TYPE_IO_USB_DCD_POLLED_DEVICE_STRUCT 1046
#define MEM_TYPE_IO_USB_DCD_INT_DEVICE_STRUCT    1047
#define MEM_TYPE_IO_SDIO_INT_DEVICE_STRUCT       1048
#define MEM_TYPE_IO_SDIO_IN_BUFFER               1049
#define MEM_TYPE_IO_SDIO_OUT_BUFFER              1050
#define MEM_TYPE_IO_LPM_DRIVER_ENTRY         1051
#ifndef __ASM__

typedef uint_16 _mem_type;

 /*--------------------------------------------------------------------------*/
 /*
  *                    TYPEDEFS FOR _CODE_PTR_ FUNCTIONS
  *
  */
 /* int.c */
 typedef void (_CODE_PTR_ INT_EXCEPTION_FPTR)(_mqx_uint, _mqx_uint, pointer, pointer);
 typedef void (_CODE_PTR_ INT_ISR_FPTR)(pointer);
 typedef void (_CODE_PTR_ INT_KERNEL_ISR_FPTR)(void);

 /* lwtimer.c/.h */
 typedef void (_CODE_PTR_ LWTIMER_ISR_FPTR)(pointer);

 /* mqx_utils.c */
 typedef void (_CODE_PTR_ MQX_EXIT_FPTR)(void);

 /* task_template_struct */
 typedef void (_CODE_PTR_ TASK_FPTR)(uint_32);

 /* kernel_data_struct */
 typedef uint_32 (_CODE_PTR_ MQX_GET_HWTICKS_FPTR)(pointer);
 typedef void (_CODE_PTR_ TIMER_COMPONENT_ISR_FPTR)(void);
 typedef _mqx_uint (_CODE_PTR_ MQX_IPC_FPTR)( boolean, _processor_number, _mqx_uint, _mqx_uint, _mqx_uint, ...);

 /* task.c */
 typedef void (_CODE_PTR_ TASK_EXCEPTION_FPTR)(_mqx_uint, pointer);
 typedef void (_CODE_PTR_ TASK_EXIT_FPTR)(void);



/*--------------------------------------------------------------------------*/
/* IDLE LOOP STRUCTURE */
/*!
 * \brief This structure defines idle loop counters.
 */
typedef struct idle_loop_struct
{
    /*! brief Idle loop. */
    _mqx_uint                   IDLE_LOOP1;
    /*! brief Idle loop. */
    _mqx_uint                   IDLE_LOOP2;
    /*! brief Idle loop. */
    _mqx_uint                   IDLE_LOOP3;
    /*! brief Idle loop. */
    _mqx_uint                   IDLE_LOOP4;

} IDLE_LOOP_STRUCT, _PTR_ IDLE_LOOP_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/* TIME STRUCTURE */
/*!
 * \brief A structure to represent time in seconds and milliseconds.
 *
 * \see _time_from_date
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see _time_to_date
 * \see DATE_STRUCT
 */
typedef struct time_struct
{

   /*! \brief The number of seconds in the time. */
   uint_32     SECONDS;

   /*! \brief The number of milliseconds in the time. */
   uint_32     MILLISECONDS;

} TIME_STRUCT, _PTR_ TIME_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/* DATE STRUCT */
/*!
 * \brief This structure is used to hold the time in a simple "date" format.
 *
 * Conversion functions _time_to_date, _time_from_date convert between this date
 * struct and the time struct.
 *
 * \warning If you violate the ranges, undefined behavior results.
 *
 * \see _time_from_date
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see _time_to_date
 * \see MQX_XDATE_STRUCT
 * \see TIME_STRUCT
 */
typedef struct date_struct
{
   /*! \brief Range from 1970 to 2099. */
   uint_16 YEAR;

   /*! \brief Range from 1 to 12. */
   uint_16 MONTH;

   /*! \brief Range from 1 to 31 (depending on month). */
   uint_16 DAY;

   /*! \brief Range from 0 to 23. */
   uint_16 HOUR;

   /*! \brief Range from 0 to 59. */
   uint_16 MINUTE;

   /*! \brief Range from 0 to 59. */
   uint_16 SECOND;

   /*! \brief Range from 0 to 999. */
   uint_16 MILLISEC;

} DATE_STRUCT, _PTR_ DATE_STRUCT_PTR;

/*!
 * \brief This structure defines how time is maintained in the system.
 *
 * Time is kept internally in the form of ticks. This is a 64 bit
 * field which is maintained in an array whose size is
 * dependent upon the PSP. HW_TICKS is used to track time between
 * ticks (timer interrupts).
 */
typedef struct mqx_tick_struct
{
    /*!
     * \brief Ticks since MQX started.
     *
     * The field is a minimum of 64 bits; the exact size depends on the PSP.
     */
    _mqx_uint TICKS[MQX_NUM_TICK_FIELDS];

    /*!
     * \brief Hardware ticks (timer counter increments) between ticks.
     *
     * The field increases the accuracy over counting the time simply in ticks.
     */
    uint_32   HW_TICKS;
} MQX_TICK_STRUCT, _PTR_ MQX_TICK_STRUCT_PTR;

/*--------------------------------------------------------------------------*/

/* EXTENDED DATE STRUCT */
/*!
 * \brief This structure is used to hold the time in a "date" format more detailed
 * than the MQX_DATE_STRUCT.
 *
 * Conversion functions _time_ticks_to_xdate, _time_xdate_to_ticks
 * convert between this extended date struct and the tick struct.
 *
 * \see _time_ticks_to_xdate
 * \see _time_xdate_to_ticks
 * \see DATE_STRUCT
 * \see MQX_TICK_STRUCT
 */
typedef struct mqx_xdate_struct
{

   /*! \brief Year since 1970. Range is from 1970 to 2481. */
   uint_16 YEAR;
   /*! \brief Month since January. Range is from 1 to 12. */
   uint_16 MONTH;
   /*! \brief Day of the month. Range is from 1 to 31. */
   uint_16 MDAY;
   /*! \brief Hour since midnight. Range is from 0 to 23. */
   uint_16 HOUR;
   /*! \brief Minute since the last hour. Range is from 0 to 59. */
   uint_16 MIN;
   /*! \brief Second since the last minute. Range is from 0 to 59. */
   uint_16 SEC;
   /*! \brief Milliseconds. Range is from 0 to 999. */
   uint_16 MSEC;
   /*! \brief Microseconds. Range is from 0 to 999. */
   uint_16 USEC;
   /*! \brief Nanoseconds. Range is from 0 to 999. */
   uint_16 NSEC;
   /*! \brief Picoseconds. Range is from 0 to 999. */
   uint_16 PSEC;
#if 0
   /*! \brief Day of the week. Sunday is day 1. Range is from 1 to 7. */
   uint_16 WDAY;
#endif
   /*! \brief Day of the week. Sunday is day 0. Range is from 0 to 6. */
   uint_16 WDAY;
   /*! \brief Day of the year. Range is from 0 to 365. */
   uint_16 YDAY;
} MQX_XDATE_STRUCT, _PTR_ MQX_XDATE_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/* TASK TEMPLATE STRUCTURE */
/*!
 * \brief Task template that MQX uses to create instances of a task.
 *
 * The task template list is an array of these structures, terminated by a
 * zero-filled element. The MQX initialization structure contains a pointer to
 * the list.
 *
 * \see _mqx
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 * \see MQX_INITIALIZATION_STRUCT
 */

typedef struct   task_template_struct
{
   /*!
    * \brief Application-unique number that identifies the task template.
    *
    * The minimum value is 1, maximum is MAX_MQX_UINT. The field is ignored if you
    * call _task_create() or _task_create_blocked() or _task_create_at() with a
    * template index equal to 0 and a creation parameter set to a pointer to a task
    * template.
    */
   _mqx_uint            TASK_TEMPLATE_INDEX;

   /*!
    * \brief Pointer to the root function for the task.
    *
    * This function will be called when a task is created with the task template
    * index above. The task is deleted when this function returns.
    */
   TASK_FPTR            TASK_ADDRESS;

   /*! \brief The amount of stack space required by this task. */
   _mem_size            TASK_STACKSIZE;


   /*!
    * \brief Software priority of the task.
    *
    * Priorities start at 0, which is the highest priority. 1, 2, 3, and so on
    * are progressively lower priorities.
    */
   _mqx_uint            TASK_PRIORITY;

   /*! \brief Pointer to a string name for tasks that MQX creates from the template. */
   char           _PTR_ TASK_NAME;

   /*!
    * \brief Possible attributes for the task.
    *
    * Possible bit values are:
    * \li MQX_AUTO_START_TASK (When MQX starts, it creates one instance of the task.)
    * \li MQX_FLOATING_POINT_TASK (Task uses the floating point co-processor. MQX
    * also saves floating-point registers as part of the task's context.)
    * \li MQX_TIME_SLICE_TASK (MQX uses round robin scheduling for the task (the
    * default is FIFO scheduling).)
    */
   _mqx_uint             TASK_ATTRIBUTES;

   /*! \brief Parameter passed to tasks that MQX creates from the template. */
   uint_32               CREATION_PARAMETER;

   /*!
    * \brief This field is the default time slice to use for the task.
	  *
    * Time slice in ticks or milliseconds depending on configuration bit in
    * mqx_cnfg.h
    */
   _mqx_uint             DEFAULT_TIME_SLICE;

} TASK_TEMPLATE_STRUCT, _PTR_ TASK_TEMPLATE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/* MQX INITIALIZATION STRUCTURE */
/*!
 * \brief MQX initialization structure for each processor.
 *
 * When an application starts MQX on each processor, it calls _mqx() with the
 * MQX initialization structure.
 *
 * \see _mqx
 * \see _task_create
 * \see _task_create_blocked
 * \see _task_create_at
 * \see _task_get_processor
 * \see TASK_TEMPLATE_STRUCT
 */
typedef  struct mqx_initialization_struct
{
   /*!
    * \brief Application-unique processor number of the processor.
    *
    * Minimum is 1, maximum is 255. (Processor number 0 is reserved and is used
    * by tasks to indicate their local processor.)
    */
   _mqx_uint       PROCESSOR_NUMBER;

   /*! \brief Lowest address from which MQX allocates dynamic memory and task stacks. */
   pointer        START_OF_KERNEL_MEMORY;

   /*!
    * \brief Highest address from which MQX allocates dynamic memory and task stacks.
    *
    * It is the application's responsibility to allocate enough memory for all
    * tasks.
    */
   pointer        END_OF_KERNEL_MEMORY;

   /*!
    * \brief The size of the interrupt stack.
    *
    * This is the maximum amount of stack space used by all interrupt handlers.
    *
    */
   _mqx_uint       INTERRUPT_STACK_SIZE;

   /*! \brief Pointer to the task template list for the processor.
    *
    * The default name for the list is MQX_template_list[].
    */
   TASK_TEMPLATE_STRUCT_PTR TASK_TEMPLATE_LIST;

   /*!
    * \brief The maximum hardware interrupt priority level of MQX.
    *
    * All tasks and interrupts run at lower priority (Applicable to CPUs with
    * multiple interrupt levels only).
    */
   _mqx_uint       MQX_HARDWARE_INTERRUPT_LEVEL_MAX;

   /*! \brief Maximum number of message pools. */
   _mqx_uint       MAX_MSGPOOLS;

   /*!
    * \brief Maximum number of message queues. Minimum is MSGQ_FIRST_USER_QUEUE,
    * maximum is 255.
    */
   _mqx_uint       MAX_MSGQS;

   /*!
    * \brief A string indicating which device is to be used as
    * the default (console) I/O for the processor.
    */
   char     _PTR_ IO_CHANNEL;

   /*! \brief The open flags to pass to the default I/O device */
   char     _PTR_ IO_OPEN_MODE;

   /* reserved fields for later use */
/*   _mqx_uint       RESERVED[2];*/

   /*! \brief The location of the interrupt stack (if not NULL). */
   pointer        INTERRUPT_STACK_LOCATION;
   /*! \brief Reserved field.
    *
    * Reserved for future enhancements to MQX; each element of the array must be
    * initialized to 0.
    */
   _mqx_uint       RESERVED[1];

#if MQX_ENABLE_USER_MODE
   /*! \brief Lowest adress of kernel area. */
   pointer          START_OF_KERNEL_AREA;
   /*! \brief Highest adress of kernel area. */
   pointer          END_OF_KERNEL_AREA;

   /*! \brief Lowest adress of default data section (.data .bss). */
   pointer          START_OF_USER_DEFAULT_MEMORY;
   /*! \brief Highest adress of default data section (.data .bss). */
   pointer          END_OF_USER_DEFAULT_MEMORY;

   /*! \brief Lowest adress of user heap - mem alloc. */
   pointer          START_OF_USER_HEAP;
   /*! \brief Highest adress of user heap - mem alloc. */
   pointer          END_OF_USER_HEAP;

   /*! \brief Lowest adress of user read/write memory. */
   pointer          START_OF_USER_RW_MEMORY;
   /*! \brief Highest adress of user read/write memory. */
   pointer          END_OF_USER_RW_MEMORY;

   /*! \brief Lowest adress of user read-only memory. */
   pointer          START_OF_USER_RO_MEMORY;
   /*! \brief Highest adress of user read-only memory. */
   pointer          END_OF_USER_RO_MEMORY;

   /*! \brief Lowest adress of user no access memory. */
   pointer          START_OF_USER_NO_MEMORY;
   /*! \brief Highest adress of user no access memory. */
   pointer          END_OF_USER_NO_MEMORY;
   /*! \brief Maximal priority of user's tasks. */
   _mqx_uint        MAX_USER_TASK_PRIORITY;
   /*! \brief Maximal number of user's tasks. */
   _mqx_uint        MAX_USER_TASK_COUNT;
#endif

} MQX_INITIALIZATION_STRUCT, _PTR_ MQX_INITIALIZATION_STRUCT_PTR;


/* Include queue structures and function prototypes */
#include "queue.h"

#include "lwsem.h"


#endif /* __ASM__ */

/*--------------------------------------------------------------------------*/
/*
 *                 INCLUDE MQX PROCESSOR CONFIGURATION
 */
#include <psp_cpu.h>


/*--------------------------------------------------------------------------*/
/*
 *                  PROTOTYPES OF KERNEL FUNCTIONS
 */

#ifndef __ASM__
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __TAD_COMPILE__
extern void             _int_default_isr(pointer);
extern void             _int_disable(void);
extern void             _int_enable(void);
extern void             _int_exception_isr(pointer);
extern INT_ISR_FPTR     _int_get_default_isr(void);
extern INT_EXCEPTION_FPTR _int_get_exception_handler(_mqx_uint);
extern INT_ISR_FPTR     _int_get_isr(_mqx_uint);
extern pointer          _int_get_isr_data(_mqx_uint);
extern _mqx_uint        _int_get_isr_depth(void);
extern INT_KERNEL_ISR_FPTR _int_get_kernel_isr(_mqx_uint);
extern _mqx_max_type    _int_get_previous_vector_table(void);
extern _mqx_max_type    _int_get_vector_table(void);
extern _mqx_uint        _int_init(_mqx_uint, _mqx_uint);
extern INT_ISR_FPTR     _int_install_default_isr(INT_ISR_FPTR);
extern INT_KERNEL_ISR_FPTR _int_install_kernel_isr(_mqx_uint,INT_KERNEL_ISR_FPTR);
extern INT_ISR_FPTR     _int_install_isr(_mqx_uint,INT_ISR_FPTR, pointer);
extern INT_ISR_FPTR     _int_install_exception_isr(void);
extern INT_ISR_FPTR     _int_install_unexpected_isr(void);
extern void             _int_kernel_isr(void);
extern INT_EXCEPTION_FPTR _int_set_exception_handler(_mqx_uint,INT_EXCEPTION_FPTR);
extern pointer          _int_set_isr_data(_mqx_uint, pointer);
extern _mqx_max_type    _int_set_vector_table(_mqx_max_type);
extern void             _int_unexpected_isr(pointer);

extern _mqx_uint        _klog_create(_mqx_uint, _mqx_uint);
extern _mqx_uint        _klog_create_at(_mqx_uint, _mqx_uint, pointer);
extern _mqx_uint        _klog_get_interrupt_stack_usage(_mem_size_ptr, _mem_size_ptr);
extern _mqx_uint        _klog_get_task_stack_usage(_task_id, _mem_size_ptr,
   _mem_size_ptr);
extern void             _klog_control(uint_32, boolean);
extern void             _klog_disable_logging_task(_task_id);
extern boolean          _klog_display(void);
extern void             _klog_enable_logging_task(_task_id);
extern void             _klog_show_stack_usage(void);
extern void             _klog_log(_mqx_uint,_mqx_max_type,_mqx_max_type,_mqx_max_type,_mqx_max_type,_mqx_max_type);
extern void             _klog_log_function(pointer fn);

extern void _PTR_       _mem_alloc(_mem_size);
extern void _PTR_       _mem_alloc_at(_mem_size, pointer);
extern void _PTR_       _mem_alloc_align(_mem_size, _mem_size);
extern void _PTR_       _mem_alloc_from(_mem_pool_id, _mem_size);
extern void _PTR_       _mem_alloc_align_from(_mem_pool_id, _mem_size, _mem_size);
extern void _PTR_       _mem_alloc_zero(_mem_size);
extern void _PTR_       _mem_alloc_zero_from(_mem_pool_id, _mem_size);
extern pointer          _mem_alloc_system(_mem_size);
extern pointer          _mem_alloc_system_from(_mem_pool_id, _mem_size);
extern pointer          _mem_alloc_system_zero(_mem_size);
extern pointer          _mem_alloc_system_zero_from(_mem_pool_id, _mem_size);

#if MQX_USE_UNCACHED_MEM && PSP_HAS_DATA_CACHE
void _PTR_ _mem_alloc_uncached(_mem_size);
void _PTR_ _mem_alloc_align_uncached(_mem_size, _mem_size);
pointer _mem_alloc_system_uncached(_mem_size);
pointer _mem_alloc_system_zero_uncached(_mem_size);
#else /* MQX_USE_UNCACHED_MEM && PSP_HAS_DATA_CACHE */
#define _mem_alloc_uncached _mem_alloc
#define _mem_alloc_align_uncached _mem_alloc_align
#define _mem_alloc_system_uncached _mem_alloc_system
#define _mem_alloc_system_zero_uncached _mem_alloc_system_zero
#endif /* MQX_USE_UNCACHED_MEM && PSP_HAS_DATA_CACHE */

#if MQXCFG_MEM_COPY || MQXCFG_MEM_COPY_NEON
extern void             _mem_copy(pointer, pointer, _mem_size);
#else
# include <string.h>
#define _mem_copy(s,d,l) memcpy(d,s,l)
#endif

#if MQX_USE_MEM_ZERO
extern void             _mem_zero(pointer, _mem_size);
#else
# include <string.h>
#define _mem_zero(p,n)  memset(p,0,n)
#endif

extern _mem_pool_id     _mem_create_pool(pointer, _mem_size);
extern _mqx_uint        _mem_extend(pointer, _mem_size);
extern _mqx_uint        _mem_extend_pool(_mem_pool_id, pointer, _mem_size);
extern _mqx_uint        _mem_free(pointer);
extern _mqx_uint        _mem_free_part(pointer, _mem_size);
extern pointer          _mem_get_error(void);
extern pointer          _mem_get_error_pool(_mem_pool_id);
extern void _PTR_       _mem_get_highwater(void);
extern void _PTR_       _mem_get_highwater_pool(_mem_pool_id);
extern _mem_size        _mem_get_size(pointer);
extern _mem_pool_id     _mem_get_system_pool_id(void);
extern uint_32          _mem_sum_ip(uint_32, _mem_size, pointer);
extern void             _mem_swap_endian(uchar _PTR_, pointer);
extern void             _mem_swap_endian_len(uchar _PTR_, pointer, _mqx_uint);
extern _mqx_uint        _mem_test(void);
extern _mqx_uint        _mem_test_all(_mem_pool_id _PTR_);
extern _mqx_uint        _mem_test_pool(_mem_pool_id);
extern _mqx_uint        _mem_test_and_set(uchar_ptr);
extern _mqx_uint        _mem_transfer(pointer, _task_id, _task_id);

#if MQXCFG_MEM_COPY_NEON
_mem_size neon_copy_offset_0(pointer dst, pointer src, _mem_size len);
_mem_size neon_copy_offset_1(pointer dst, pointer src, _mem_size len);
_mem_size neon_copy_offset_2(pointer dst, pointer src, _mem_size len);
_mem_size neon_copy_offset_3(pointer dst, pointer src, _mem_size len);
#endif // MQXCFG_MEM_COPY_NEON

#if MQX_ALLOW_TYPED_MEMORY
extern boolean          _mem_set_type(pointer, _mem_type);
extern _mem_type        _mem_get_type(pointer);
#else
#define _mem_get_type(p)   0
#define _mem_set_type(p,u)
#endif



extern _mqx_uint         _mem_verify(pointer base, pointer extent);

extern _mqx_uint         _mqx(MQX_INITIALIZATION_STRUCT_PTR);
extern const MQX_TICK_STRUCT  _mqx_zero_tick_struct;
extern const uint_32     _mqx_version_number;
extern const char _PTR_  const _mqx_vendor;
extern const char _PTR_  const _mqx_version;
extern const char _PTR_  const _mqx_copyright;
extern const char _PTR_  const _mqx_date;
extern const char _PTR_  const _mqx_generic_revision;
extern const char _PTR_  const _mqx_psp_revision;
extern const char _PTR_  _mqx_bsp_revision;
extern const char _PTR_  _mqx_io_revision;
extern void              _mqx_exit(_mqx_uint);
extern void              _mqx_fatal_error(_mqx_uint);
extern _mqx_uint         _mqx_get_counter(void);
extern _mqx_uint         _mqx_get_cpu_type(void);
extern MQX_INITIALIZATION_STRUCT_PTR _mqx_get_initialization(void);
extern pointer           _mqx_get_kernel_data(void);
extern MQX_EXIT_FPTR     _mqx_get_exit_handler(void);
extern _task_id          _mqx_get_system_task_id(void);
extern pointer           _mqx_get_tad_data(pointer);
extern void              _mqx_idle_task(uint_32);
extern volatile _mqx_uint _mqx_monitor_type;
extern void              _mqx_set_cpu_type(_mqx_uint);
extern void              _mqx_set_exit_handler(MQX_EXIT_FPTR);
extern void              _mqx_set_tad_data(pointer, pointer);
extern _mqx_uint         _mqx_get_idle_loop_count(IDLE_LOOP_STRUCT_PTR);


extern _mqx_uint         _sched_get_max_priority(_mqx_uint);
extern _mqx_uint         _sched_get_min_priority(_mqx_uint);
extern uint_32           _sched_get_rr_interval(_task_id, uint_32_ptr);
extern _mqx_uint         _sched_get_rr_interval_ticks(_task_id, MQX_TICK_STRUCT_PTR);
extern _mqx_uint         _sched_get_policy(_task_id, _mqx_uint_ptr);
extern _mqx_uint         _sched_set_policy(_task_id, _mqx_uint);
extern uint_32           _sched_set_rr_interval(_task_id, uint_32);
extern _mqx_uint         _sched_set_rr_interval_ticks(_task_id,
   MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
extern void              _sched_yield(void);


extern _mqx_uint         _task_abort(_task_id);
extern void              _task_block(void);
extern _mqx_uint         _task_check_stack(void);
extern _task_id          _task_create(_processor_number, _mqx_uint, uint_32);
extern _task_id          _task_create_blocked(_processor_number, _mqx_uint, uint_32);
extern _task_id          _task_create_at(_processor_number, _mqx_uint,
   uint_32, pointer, _mem_size);
extern _mqx_uint         _task_destroy(_task_id);
extern void              _task_disable_fp(void);
extern boolean           _task_enable_fp(void);
extern _task_id          _task_get_creator(void);
extern pointer           _task_get_environment(_task_id);
extern TASK_EXCEPTION_FPTR _task_get_exception_handler(_task_id);
extern _mqx_uint         _task_get_error(void);
extern _mqx_uint   _PTR_ _task_get_error_ptr(void);
extern TASK_EXIT_FPTR    _task_get_exit_handler(_task_id);
extern _task_id          _task_get_id(void);
extern _task_id          _task_get_id_from_name(char_ptr);
extern _task_id          _task_get_id_from_td(pointer);
extern _mqx_uint         _task_get_index_from_id(_task_id);
extern uint_32           _task_get_parameter(void);
extern uint_32           _task_get_parameter_for(_task_id);
extern _processor_number _task_get_processor(_task_id);
extern _mqx_uint         _task_get_priority(_task_id, _mqx_uint_ptr);
extern pointer           _task_get_td(_task_id);
extern _mqx_uint         _task_get_template_index(char_ptr);
extern TASK_TEMPLATE_STRUCT_PTR _task_get_template_ptr(_task_id);
extern void              _task_ready(pointer);
extern _mqx_uint         _task_restart(_task_id, uint_32_ptr, boolean);
extern pointer           _task_set_environment(_task_id, pointer);
extern _mqx_uint         _task_set_error(_mqx_uint);
extern TASK_EXCEPTION_FPTR _task_set_exception_handler(_task_id,TASK_EXCEPTION_FPTR);
extern TASK_EXIT_FPTR    _task_set_exit_handler(_task_id,TASK_EXIT_FPTR);
extern uint_32           _task_set_parameter(uint_32);
extern uint_32           _task_set_parameter_for(uint_32, _task_id);
extern _mqx_uint         _task_set_priority(_task_id, _mqx_uint, _mqx_uint_ptr);
extern void              _task_stop_preemption(void);
extern void              _task_start_preemption(void);

extern _mqx_uint         _task_reserve_space(_mqx_uint);
extern pointer           _task_get_reserved_base(void);

#if MQX_ENABLE_USER_MODE
extern _task_id          _usr_task_create(_processor_number, _mqx_uint, uint_32);
extern _mqx_uint         _usr_task_destroy(_task_id);
extern _mqx_uint         _usr_task_abort(_task_id task_id);
extern _mqx_uint         _usr_task_set_error(_mqx_uint);
extern pointer           _usr_task_get_td(_task_id);
extern void              _usr_task_ready(pointer);
#endif

extern pointer           _taskq_create(_mqx_uint);
extern _mqx_uint         _taskq_destroy(pointer);
extern _mqx_uint         _taskq_resume(pointer, boolean);
extern _mqx_uint         _taskq_suspend(pointer);
extern _mqx_uint         _taskq_suspend_task(_task_id, pointer);
extern _mqx_uint         _taskq_test(pointer _PTR_, pointer _PTR_);
extern _mqx_uint         _taskq_get_value(pointer);

extern MQX_TICK_STRUCT_PTR  _time_add_day_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern MQX_TICK_STRUCT_PTR  _time_add_hour_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern MQX_TICK_STRUCT_PTR  _time_add_min_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern MQX_TICK_STRUCT_PTR  _time_add_msec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern MQX_TICK_STRUCT_PTR  _time_add_nsec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern MQX_TICK_STRUCT_PTR  _time_add_sec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern MQX_TICK_STRUCT_PTR  _time_add_usec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern MQX_TICK_STRUCT_PTR  _time_add_psec_to_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);

extern void                 _time_delay(uint_32);
extern void                 _time_delay_ticks(_mqx_uint);
extern void                 _time_delay_for(MQX_TICK_STRUCT_PTR);
extern void                 _time_delay_until(MQX_TICK_STRUCT_PTR);
extern void                 _time_dequeue(_task_id);
extern void                 _time_dequeue_td(pointer);
extern void                 _time_diff(TIME_STRUCT_PTR, TIME_STRUCT_PTR, TIME_STRUCT_PTR);
extern int_32               _time_diff_days(MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern int_32               _time_diff_hours(MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern int_32               _time_diff_minutes(MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern int_32               _time_diff_milliseconds(MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern int_32               _time_diff_nanoseconds(MQX_TICK_STRUCT_PTR,
   MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern int_32               _time_diff_picoseconds(MQX_TICK_STRUCT_PTR,
   MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern int_32               _time_diff_seconds(MQX_TICK_STRUCT_PTR,
   MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern int_32               _time_diff_microseconds(MQX_TICK_STRUCT_PTR,
   MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern _mqx_uint            _time_diff_ticks(MQX_TICK_STRUCT_PTR,
   MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
extern int_32               _time_diff_ticks_int32(MQX_TICK_STRUCT_PTR,
   MQX_TICK_STRUCT_PTR, boolean _PTR_);
extern boolean              _time_from_date(DATE_STRUCT_PTR, TIME_STRUCT_PTR);
extern void                 _time_get(TIME_STRUCT_PTR);
extern void                 _time_get_elapsed(TIME_STRUCT_PTR);
extern void                 _time_get_elapsed_ticks(MQX_TICK_STRUCT_PTR);
extern void                 _time_get_elapsed_ticks_fast(MQX_TICK_STRUCT_PTR);
extern uint_32              _time_get_hwticks(void);
extern uint_32              _time_get_hwticks_per_tick(void);
extern uint_16              _time_get_microseconds(void);
extern uint_32              _time_get_nanoseconds(void);
extern _mqx_uint            _time_get_resolution(void);
extern void                 _time_get_ticks(MQX_TICK_STRUCT_PTR);
extern _mqx_uint            _time_get_ticks_per_sec(void);
extern _mqx_uint            _time_init_ticks(MQX_TICK_STRUCT_PTR, _mqx_uint);
extern boolean              _time_normalize_xdate(MQX_XDATE_STRUCT_PTR);
extern void                 _time_notify_kernel(void);
extern void                 _time_set(TIME_STRUCT_PTR);
extern void                 _time_set_hwtick_function(MQX_GET_HWTICKS_FPTR, pointer);
extern void                 _time_set_hwticks_per_tick(uint_32);
extern _mqx_uint            _time_set_resolution(_mqx_uint);
extern void                 _time_set_ticks(MQX_TICK_STRUCT_PTR);
extern void                 _time_set_ticks_per_sec(_mqx_uint);
extern void                 _time_set_timer_vector(_mqx_uint);
extern boolean              _time_ticks_to_xdate(MQX_TICK_STRUCT_PTR,
   MQX_XDATE_STRUCT_PTR);
extern boolean              _time_to_date(TIME_STRUCT_PTR, DATE_STRUCT_PTR);
extern boolean              _time_xdate_to_ticks(MQX_XDATE_STRUCT_PTR,
   MQX_TICK_STRUCT_PTR);
extern boolean              _time_to_ticks(TIME_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
extern boolean              _ticks_to_time(MQX_TICK_STRUCT_PTR, TIME_STRUCT_PTR);

#if MQX_ENABLE_USER_MODE
extern void                 _usr_time_delay(uint_32);
extern void                 _usr_time_delay_ticks(_mqx_uint);
extern void                 _usr_time_get_elapsed_ticks(MQX_TICK_STRUCT_PTR);
#endif /* MQX_ENABLE_USER_MODE */
#define MQX_TLS_SUPPORTED   1 /* Thread local storage supported */
extern pointer              _crt_tls_reference(void);

/*--------------------------------------------------------------------------*/
/*
 *                  ENDIAN SWAP MACROS AND PROTOTYPES
 */

#define _PSP_SWAP2BYTE_CONST(n) ((((n) & 0x00FF) << 8) | (((n) & 0xFF00) >> 8))
#define _PSP_SWAP4BYTE_CONST(n) ((((n) & 0x000000FF) << 24) | (((n) & 0x0000FF00) << 8) | (((n) & 0x00FF0000) >> 8) | (((n) & 0xFF000000) >> 24))

#if (PSP_ENDIAN == MQX_BIG_ENDIAN)
    #define HOST_TO_BE_SHORT(n)           (n)
    #define HOST_TO_BE_SHORT_CONST(n)     (n)
    #define HOST_TO_LE_SHORT(n)           _PSP_SWAP2BYTE(n)
    #define HOST_TO_LE_SHORT_CONST(n)     _PSP_SWAP2BYTE_CONST(n)
    #define SHORT_BE_TO_HOST(n)           (n)
    #define SHORT_BE_TO_HOST_CONST(n)     (n)
    #define SHORT_LE_TO_HOST(n)           _PSP_SWAP2BYTE(n)
    #define SHORT_LE_TO_HOST_CONST(n)     _PSP_SWAP2BYTE_CONST(n)
    #define SHORT_UNALIGNED_LE_TO_HOST(n) ((n[1]<<8)|n[0])

    #define HOST_TO_BE_LONG(n)            (n)
    #define HOST_TO_BE_LONG_CONST(n)      (n)
	#define HOST_TO_BE_UNALIGNED_LONG(n, m)  \
    { \
       m[0]=((n>>24) & 0xFF); \
       m[1]=((n>>16) & 0xFF); \
       m[2]=((n>>8) & 0xFF); \
       m[3]=(n & 0xFF); \
    }
    #define HOST_TO_LE_LONG(n)            _PSP_SWAP4BYTE(n)
    #define HOST_TO_LE_LONG_CONST(n)      _PSP_SWAP4BYTE_CONST(n)
	#define HOST_TO_LE_UNALIGNED_LONG(n, m)  \
	{ \
       m[0]=(n & 0xFF); \
       m[1]=((n>>8) & 0xFF); \
       m[2]=((n>>16) & 0xFF); \
       m[3]=((n>>24) & 0xFF); \
    }
    #define LONG_BE_TO_HOST(n)            (n)
    #define LONG_BE_TO_HOST_CONST(n)      (n)
    #define LONG_LE_TO_HOST(n)            _PSP_SWAP4BYTE(n)
    #define LONG_LE_TO_HOST_CONST(n)      _PSP_SWAP4BYTE_CONST(n)
    #define LONG_UNALIGNED_LE_TO_HOST(n)  ((n[3]<<24)|(n[2]<<16)|(n[1]<<8)|n[0])

#else /* (PSP_ENDIAN == MQX_BIG_ENDIAN) */

    #define HOST_TO_BE_SHORT(n)           _PSP_SWAP2BYTE(n)
    #define HOST_TO_BE_SHORT_CONST(n)     _PSP_SWAP2BYTE_CONST(n)
    #define HOST_TO_LE_SHORT(n)           (n)
    #define HOST_TO_LE_SHORT_CONST(n)     (n)
    #define SHORT_BE_TO_HOST(n)           _PSP_SWAP2BYTE(n)
    #define SHORT_BE_TO_HOST_CONST(n)     _PSP_SWAP2BYTE_CONST(n)
    #define SHORT_LE_TO_HOST(n)           (n)
    #define SHORT_LE_TO_HOST_CONST(n)     (n)
    #define SHORT_UNALIGNED_LE_TO_HOST(n) ((n[1]<<8)|n[0])

    #define HOST_TO_BE_LONG(n)            _PSP_SWAP4BYTE(n)
    #define HOST_TO_BE_LONG_CONST(n)      _PSP_SWAP4BYTE_CONST(n)
    #define HOST_TO_BE_UNALIGNED_LONG(n, m)  \
    { \
       m[0]=((n>>24) & 0xFF); \
       m[1]=((n>>16) & 0xFF); \
       m[2]=((n>>8) & 0xFF); \
       m[3]=(n & 0xFF); \
    }
    #define HOST_TO_LE_LONG(n)            (n)
    #define HOST_TO_LE_LONG_CONST(n)      (n)
    #define HOST_TO_LE_UNALIGNED_LONG(n, m)  \
    { \
       m[0]=(n & 0xFF); \
       m[1]=((n>>8) & 0xFF); \
       m[2]=((n>>16) & 0xFF); \
       m[3]=((n>>24) & 0xFF); \
    }

    #define LONG_BE_TO_HOST(n)            _PSP_SWAP4BYTE(n)
    #define LONG_BE_TO_HOST_CONST(n)      _PSP_SWAP4BYTE_CONST(n)
    #define LONG_LE_TO_HOST(n)            (n)
    #define LONG_LE_TO_HOST_CONST(n)      (n)
    #define LONG_UNALIGNED_LE_TO_HOST(n)  ((n[3]<<24)|(n[2]<<16)|(n[1]<<8)|n[0])


#endif /* (PSP_ENDIAN == MQX_BIG_ENDIAN) */

#endif /* __TAD_COMPILE__ */

#ifdef __cplusplus
}
#endif

#endif /* __ASM__ */

/*--------------------------------------------------------------------------*/
/*
 *                  SPECIAL MEMORY ALLOCATOR OPTION
 */

#if MQX_USE_LWMEM_ALLOCATOR
#ifndef __MEMORY_MANAGER_COMPILE__
#include "lwmem.h"
#define _mem_pool_id _lwmem_pool_id
#define _mem_alloc _lwmem_alloc
#define _mem_alloc_at _lwmem_alloc_at
#define _mem_alloc_align _lwmem_alloc_align
#define _mem_alloc_align_from _lwmem_alloc_align_from
#define _mem_alloc_from _lwmem_alloc_from
#define _mem_alloc_zero _lwmem_alloc_zero
#define _mem_alloc_zero_from _lwmem_alloc_zero_from
#define _mem_alloc_system _lwmem_alloc_system
#define _mem_alloc_system_from _lwmem_alloc_system_from
#define _mem_alloc_system_zero _lwmem_alloc_system_zero
#define _mem_alloc_system_zero_from _lwmem_alloc_system_zero_from
#define _mem_free _lwmem_free
#define _mem_transfer _lwmem_transfer
#define _mem_get_system_pool_id _lwmem_get_system_pool_id
#define _mem_create_pool _lwmem_create_pool_mapped
#define _mem_get_highwater _lwmem_get_highwater
#define _mem_get_free _lwmem_get_free
#define _mem_get_free_from _lwmem_get_free_from
#if MQX_ALLOW_TYPED_MEMORY
#define _mem_get_type _lwmem_get_type
#define _mem_set_type _lwmem_set_type
#endif
#if MQX_ENABLE_USER_MODE
#define _usr_mem_alloc                  _usr_lwmem_alloc
#define _usr_mem_alloc_from             _usr_lwmem_alloc_from
#define _usr_mem_free                   _usr_lwmem_free
#endif /* MQX_ENABLE_USER_MODE */
#endif
#endif

#endif
/* EOF */
