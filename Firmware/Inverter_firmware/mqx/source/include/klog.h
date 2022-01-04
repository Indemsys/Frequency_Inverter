/*HEADER*********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: klog.h$
* $Version : 3.8.15.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   kernel log.  It defines what entries in the kernel
*   log look like.  It also identifies the meaning of the bits in the
*   LOG_CONTROL field of mqx.h.
*
*END************************************************************************/
#ifndef __klog_h__
#define __klog_h__ 1

#include <mqx_cnfg.h>

#if (! MQX_KERNEL_LOGGING) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error KERNEL LOGGING functionality is currently disabled in MQX kernel. Please set MQX_KERNEL_LOGGING to 1 in user_config.h and recompile kernel.
#endif


/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* What type of entry in the log */
#define KLOG_FUNCTION_ENTRY   (_mqx_max_type)(1)
#define KLOG_FUNCTION_EXIT    (_mqx_max_type)(2)
#define KLOG_INTERRUPT        (_mqx_max_type)(3)
#define KLOG_INTERRUPT_END    (_mqx_max_type)(4)
#define KLOG_CONTEXT_SWITCH   (_mqx_max_type)(5)
#define KLOG_FUNCTION_ADDRESS (_mqx_max_type)(6)
#define KLOG_RTCS_PCB         (_mqx_max_type)(7)
#define KLOG_RTCS_TIMER       (_mqx_max_type)(8)
#define KLOG_RTCS_ERROR       (_mqx_max_type)(9)

#define KLOG_BSP_IO           (_mqx_max_type)(10)

/*
 * The following control bits are kept in the control variable
 * LOG_CONTROL, and can be modified by the application.
 *
 * Task qualification means that the TD_STRUCT FLAGS field is checked.
 * Only if logging has been enabled for this task are logs generated.
 */

/* The following bits control logging details */

/*
 * The _klog_control variable needs to be modified to set one or more
 * of these bits.
 */

/* Overall enable/disable of kernel logging. */
#define KLOG_ENABLED                  (uint_32)(0x00000001)

/* Enable logging of function entry/exit points. */
#define KLOG_FUNCTIONS_ENABLED        (uint_32)(0x00000002)

/* Enable logging of interrupts. */
#define KLOG_INTERRUPTS_ENABLED       (uint_32)(0x00000004)

/* Enable logging of system clock tick interrupt. */
#define KLOG_SYSTEM_CLOCK_INT_ENABLED (uint_32)(0x00000008)

/* Enable logging of context switches. */
#define KLOG_CONTEXT_ENABLED          (uint_32)(0x00000010)

/* Enable task qualification checking of logs. */
#define KLOG_TASK_QUALIFIED           (uint_32)(0x00000020)

/*
 * The following bits control what functions actually log information.
 * _klog_control variable needs to be modified to set one or more
 * of the following bits.
 *
 * If the KLOG_FUNCTIONS_ENABLED bit is set, then when a function entry
 * or exit point is reached, the function TYPE is checked.  Only those
 * functions for whom logging has been enabled for their TYPE are logged.
 *
 * NOTE: the user can add his own functions to be logged into the kernel
 * log, using the upper bits that are available
 */
#define KLOG_TASKING_FUNCTIONS        (uint_32)(0x00000100)
#define KLOG_ERROR_FUNCTIONS          (uint_32)(0x00000200)
#define KLOG_MESSAGE_FUNCTIONS        (uint_32)(0x00000400)
#define KLOG_INTERRUPT_FUNCTIONS      (uint_32)(0x00000800)
#define KLOG_MEMORY_FUNCTIONS         (uint_32)(0x00001000)
#define KLOG_TIME_FUNCTIONS           (uint_32)(0x00002000)
#define KLOG_EVENT_FUNCTIONS          (uint_32)(0x00004000)
#define KLOG_NAME_FUNCTIONS           (uint_32)(0x00008000)
#define KLOG_MUTEX_FUNCTIONS          (uint_32)(0x00010000)
#define KLOG_SEMAPHORE_FUNCTIONS      (uint_32)(0x00020000)
#define KLOG_WATCHDOG_FUNCTIONS       (uint_32)(0x00040000)
#define KLOG_PARTITION_FUNCTIONS      (uint_32)(0x00080000)
#define KLOG_IO_FUNCTIONS             (uint_32)(0x00100000)
#define KLOG_EDS_FUNCTIONS            (uint_32)(0x00200000)
#define KLOG_RTCS_FUNCTIONS           (uint_32)(0x02000000)

#define KLOG_FUNCTION_MASK            (uint_32)(0xFFFFFF00)

/* For a function, the following are the function identifiers: */

/* Tasking functions */
#define KLOG_task_ready                        (KLOG_TASKING_FUNCTIONS+1)
#define KLOG_task_block                        (KLOG_TASKING_FUNCTIONS+2)
#define KLOG_task_execute_scheduler            (KLOG_TASKING_FUNCTIONS+3)
#define KLOG_task_create                       (KLOG_TASKING_FUNCTIONS+4)
#define KLOG_task_create_block                 (KLOG_TASKING_FUNCTIONS+5)
#define KLOG_task_destroy                      (KLOG_TASKING_FUNCTIONS+6)
#define KLOG_time_dequeue                      (KLOG_TASKING_FUNCTIONS+7)
#define KLOG_sched_setscheduler                (KLOG_TASKING_FUNCTIONS+8)
#define KLOG_sched_yield                       (KLOG_TASKING_FUNCTIONS+9)
#define KLOG_sched_setparam                    (KLOG_TASKING_FUNCTIONS+10)
#define KLOG_sched_setparam_td                 (KLOG_TASKING_FUNCTIONS+11)
#define KLOG_sched_boost_priority              (KLOG_TASKING_FUNCTIONS+12)
#define KLOG_sched_unboost_priority            (KLOG_TASKING_FUNCTIONS+13)
#define KLOG_task_exit_function                (KLOG_TASKING_FUNCTIONS+14)
#define KLOG_taskq_create                      (KLOG_TASKING_FUNCTIONS+15)
#define KLOG_taskq_destroy                     (KLOG_TASKING_FUNCTIONS+16)
#define KLOG_taskq_suspend                     (KLOG_TASKING_FUNCTIONS+17)
#define KLOG_taskq_resume                      (KLOG_TASKING_FUNCTIONS+18)
#define KLOG_sched_set_policy                  (KLOG_TASKING_FUNCTIONS+19)
#define KLOG_sched_set_rr_interval             (KLOG_TASKING_FUNCTIONS+20)
#define KLOG_sched_set_rr_interval_ticks       (KLOG_TASKING_FUNCTIONS+21)
#define KLOG_taskq_suspend_task                (KLOG_TASKING_FUNCTIONS+22)
#define KLOG_task_set_priority                 (KLOG_TASKING_FUNCTIONS+23)
#define KLOG_time_dequeue_td                   (KLOG_TASKING_FUNCTIONS+24)
#define KLOG_task_abort                        (KLOG_TASKING_FUNCTIONS+25)
#define KLOG_task_set_environment              (KLOG_TASKING_FUNCTIONS+26)
#define KLOG_task_enable_fp                    (KLOG_TASKING_FUNCTIONS+27)
#define KLOG_task_disable_fp                   (KLOG_TASKING_FUNCTIONS+28)
#define KLOG_task_set_parameter                (KLOG_TASKING_FUNCTIONS+29)
#define KLOG_task_set_parameter_for            (KLOG_TASKING_FUNCTIONS+30)
#define KLOG_task_stop_preemption              (KLOG_TASKING_FUNCTIONS+31)
#define KLOG_task_start_preemption             (KLOG_TASKING_FUNCTIONS+32)
#define KLOG_task_set_exception_handler        (KLOG_TASKING_FUNCTIONS+33)
#define KLOG_task_set_exit_handler             (KLOG_TASKING_FUNCTIONS+34)
#define KLOG_taskq_test                        (KLOG_TASKING_FUNCTIONS+35)
#define KLOG_io_set_handle                     (KLOG_TASKING_FUNCTIONS+36)
#define KLOG_task_restart                      (KLOG_TASKING_FUNCTIONS+37)
#define KLOG_task_create_at                    (KLOG_TASKING_FUNCTIONS+38)

/* System error functions */
#define KLOG_mqx_exit                          (KLOG_ERROR_FUNCTIONS+1)
#define KLOG_int_default_isr                   (KLOG_ERROR_FUNCTIONS+2)
#define KLOG_task_set_error                    (KLOG_ERROR_FUNCTIONS+3)
#define KLOG_log_test                          (KLOG_ERROR_FUNCTIONS+4)
#define KLOG_mqx_set_exit_handler              (KLOG_ERROR_FUNCTIONS+5)
#define KLOG_mqx_fatal_error                   (KLOG_ERROR_FUNCTIONS+6)

/* Message queue functions */
#define KLOG_msgq_close                        (KLOG_MESSAGE_FUNCTIONS+1)
#define KLOG_msgpool_create_system             (KLOG_MESSAGE_FUNCTIONS+2)
#define KLOG_msgpool_create                    (KLOG_MESSAGE_FUNCTIONS+3)
#define KLOG_msgpool_destroy                   (KLOG_MESSAGE_FUNCTIONS+4)
#define KLOG_msg_free                          (KLOG_MESSAGE_FUNCTIONS+5)
#define KLOG_msg_alloc_system                  (KLOG_MESSAGE_FUNCTIONS+6)
#define KLOG_msg_alloc                         (KLOG_MESSAGE_FUNCTIONS+7)
#define KLOG_msgq_get_owner                    (KLOG_MESSAGE_FUNCTIONS+8)
#define KLOG_msgq_open                         (KLOG_MESSAGE_FUNCTIONS+9)
#define KLOG_msgq_open_system                  (KLOG_MESSAGE_FUNCTIONS+10)
#define KLOG_msgq_receive                      (KLOG_MESSAGE_FUNCTIONS+11)
#define KLOG_msgq_receive_for                  (KLOG_MESSAGE_FUNCTIONS+12)
#define KLOG_msgq_receive_ticks                (KLOG_MESSAGE_FUNCTIONS+13)
#define KLOG_msgq_receive_until                (KLOG_MESSAGE_FUNCTIONS+14)
#define KLOG_msgq_poll                         (KLOG_MESSAGE_FUNCTIONS+15)
#define KLOG_msgq_get_count                    (KLOG_MESSAGE_FUNCTIONS+16)
#define KLOG_msgq_send                         (KLOG_MESSAGE_FUNCTIONS+17)
#define KLOG_msgq_send_blocked                 (KLOG_MESSAGE_FUNCTIONS+18)
#define KLOG_msgq_send_queue                   (KLOG_MESSAGE_FUNCTIONS+19)
#define KLOG_msgq_send_urgent                  (KLOG_MESSAGE_FUNCTIONS+20)
#define KLOG_msgq_send_broadcast               (KLOG_MESSAGE_FUNCTIONS+21)
#define KLOG_msgq_set_notification_function    (KLOG_MESSAGE_FUNCTIONS+22)
#define KLOG_msgq_peek                         (KLOG_MESSAGE_FUNCTIONS+23)
#define KLOG_msgq_send_priority                (KLOG_MESSAGE_FUNCTIONS+24)
#define KLOG_msg_create_component              (KLOG_MESSAGE_FUNCTIONS+25)
#define KLOG_msgpool_test                      (KLOG_MESSAGE_FUNCTIONS+26)
#define KLOG_msgq_test                         (KLOG_MESSAGE_FUNCTIONS+27)
#define KLOG_lwmsgq_init                       (KLOG_MESSAGE_FUNCTIONS+28)
#define KLOG_lwmsgq_send                       (KLOG_MESSAGE_FUNCTIONS+29)
#define KLOG_lwmsgq_receive                    (KLOG_MESSAGE_FUNCTIONS+30)

/* Interrupt management functions */
#define KLOG_int_install_kernel_isr            (KLOG_INTERRUPT_FUNCTIONS+1)
#define KLOG_int_install_isr                   (KLOG_INTERRUPT_FUNCTIONS+2)
#define KLOG_int_install_default_isr           (KLOG_INTERRUPT_FUNCTIONS+3)
#define KLOG_int_set_isr_data                  (KLOG_INTERRUPT_FUNCTIONS+4)
#define KLOG_int_set_exception_handler         (KLOG_INTERRUPT_FUNCTIONS+5)

/* Memory management functions */
#define KLOG_mem_free                          (KLOG_MEMORY_FUNCTIONS+1)
#define KLOG_mem_alloc                         (KLOG_MEMORY_FUNCTIONS+2)
#define KLOG_mem_alloc_from                    (KLOG_MEMORY_FUNCTIONS+3)
#define KLOG_mem_test                          (KLOG_MEMORY_FUNCTIONS+4)
#define KLOG_mem_test_pool                     (KLOG_MEMORY_FUNCTIONS+5)
#define KLOG_mem_alloc_zero                    (KLOG_MEMORY_FUNCTIONS+6)
#define KLOG_mem_alloc_system                  (KLOG_MEMORY_FUNCTIONS+7)
#define KLOG_mem_alloc_system_from             (KLOG_MEMORY_FUNCTIONS+8)
#define KLOG_mem_alloc_system_zero             (KLOG_MEMORY_FUNCTIONS+9)
#define KLOG_mem_alloc_system_zero_from        (KLOG_MEMORY_FUNCTIONS+10)
#define KLOG_mem_extend                        (KLOG_MEMORY_FUNCTIONS+11)
#define KLOG_mem_extend_pool                   (KLOG_MEMORY_FUNCTIONS+12)
#define KLOG_mem_transfer                      (KLOG_MEMORY_FUNCTIONS+13)
#define KLOG_mem_free_part                     (KLOG_MEMORY_FUNCTIONS+14)
#define KLOG_mem_create_pool                   (KLOG_MEMORY_FUNCTIONS+15)

#define KLOG_lwmem_alloc                       (KLOG_MEMORY_FUNCTIONS+16)
#define KLOG_lwmem_alloc_from                  (KLOG_MEMORY_FUNCTIONS+17)
#define KLOG_lwmem_alloc_zero                  (KLOG_MEMORY_FUNCTIONS+18)
#define KLOG_lwmem_alloc_zero_from             (KLOG_MEMORY_FUNCTIONS+19)
#define KLOG_lwmem_create_pool                 (KLOG_MEMORY_FUNCTIONS+20)
#define KLOG_lwmem_free                        (KLOG_MEMORY_FUNCTIONS+21)
#define KLOG_lwmem_alloc_system                (KLOG_MEMORY_FUNCTIONS+22)
#define KLOG_lwmem_alloc_system_from           (KLOG_MEMORY_FUNCTIONS+23)
#define KLOG_lwmem_transfer                    (KLOG_MEMORY_FUNCTIONS+24)
#define KLOG_lwmem_alloc_system_zero           (KLOG_MEMORY_FUNCTIONS+25)
#define KLOG_lwmem_alloc_system_zero_from      (KLOG_MEMORY_FUNCTIONS+26)
#define KLOG_lwmem_get_free                    (KLOG_MEMORY_FUNCTIONS+27)
#define KLOG_lwmem_get_free_from               (KLOG_MEMORY_FUNCTIONS+28)

#define KLOG_mem_alloc_align                   (KLOG_MEMORY_FUNCTIONS+29)
#define KLOG_mem_alloc_at                      (KLOG_MEMORY_FUNCTIONS+30)

#define KLOG_lwmem_alloc_at                    (KLOG_MEMORY_FUNCTIONS+31)
#define KLOG_lwmem_alloc_align                 (KLOG_MEMORY_FUNCTIONS+32)
#define KLOG_lwmem_alloc_align_from            (KLOG_MEMORY_FUNCTIONS+33)

#define KLOG_usr_lwmem_alloc                   (KLOG_MEMORY_FUNCTIONS+34)

/* Partition functions */
#define KLOG_partition_create                  (KLOG_PARTITION_FUNCTIONS+1)
#define KLOG_partition_create_at               (KLOG_PARTITION_FUNCTIONS+2)
#define KLOG_partition_alloc                   (KLOG_PARTITION_FUNCTIONS+3)
#define KLOG_partition_alloc_zero              (KLOG_PARTITION_FUNCTIONS+4)
#define KLOG_partition_alloc_system            (KLOG_PARTITION_FUNCTIONS+5)
#define KLOG_partition_alloc_system_zero       (KLOG_PARTITION_FUNCTIONS+6)
#define KLOG_partition_extend                  (KLOG_PARTITION_FUNCTIONS+7)
#define KLOG_partition_free                    (KLOG_PARTITION_FUNCTIONS+8)
#define KLOG_partition_test                    (KLOG_PARTITION_FUNCTIONS+9)
#define KLOG_partition_transfer                (KLOG_PARTITION_FUNCTIONS+10)
#define KLOG_partition_destroy                 (KLOG_PARTITION_FUNCTIONS+11)
#define KLOG_partition_create_component        (KLOG_PARTITION_FUNCTIONS+12)

/* Time functions */
#define KLOG_time_set                          (KLOG_TIME_FUNCTIONS+1)
#define KLOG_time_set_ticks                    (KLOG_TIME_FUNCTIONS+2)
#define KLOG_time_delay                        (KLOG_TIME_FUNCTIONS+3)
#define KLOG_time_delay_for                    (KLOG_TIME_FUNCTIONS+4)
#define KLOG_time_delay_ticks                  (KLOG_TIME_FUNCTIONS+5)
#define KLOG_time_delay_until                  (KLOG_TIME_FUNCTIONS+6)
#define KLOG_timer_create_component            (KLOG_TIME_FUNCTIONS+7)
#define KLOG_timer_cancel                      (KLOG_TIME_FUNCTIONS+8)
#define KLOG_timer_start_oneshot_after         (KLOG_TIME_FUNCTIONS+9)
#define KLOG_timer_start_oneshot_at            (KLOG_TIME_FUNCTIONS+10)
#define KLOG_timer_start_periodic_every        (KLOG_TIME_FUNCTIONS+11)
#define KLOG_timer_start_periodic_at           (KLOG_TIME_FUNCTIONS+12)
#define KLOG_timer_test                        (KLOG_TIME_FUNCTIONS+13)
#define KLOG_timer_start_oneshot_after_ticks   (KLOG_TIME_FUNCTIONS+14)
#define KLOG_timer_start_oneshot_at_ticks      (KLOG_TIME_FUNCTIONS+15)
#define KLOG_timer_start_periodic_every_ticks  (KLOG_TIME_FUNCTIONS+16)
#define KLOG_timer_start_periodic_at_ticks     (KLOG_TIME_FUNCTIONS+17)
#define KLOG_lwtimer_create_periodic_queue     (KLOG_TIME_FUNCTIONS+18)
#define KLOG_lwtimer_add_timer_to_queue        (KLOG_TIME_FUNCTIONS+19)
#define KLOG_lwtimer_cancel_period             (KLOG_TIME_FUNCTIONS+20)
#define KLOG_lwtimer_cancel_timer              (KLOG_TIME_FUNCTIONS+21)
#define KLOG_lwtimer_test                      (KLOG_TIME_FUNCTIONS+22)

/* Event functions */
#define KLOG_event_create_component            (KLOG_EVENT_FUNCTIONS+1)
#define KLOG_event_create                      (KLOG_EVENT_FUNCTIONS+2)
#define KLOG_event_destroy                     (KLOG_EVENT_FUNCTIONS+3)
#define KLOG_event_open                        (KLOG_EVENT_FUNCTIONS+4)
#define KLOG_event_close                       (KLOG_EVENT_FUNCTIONS+5)
#define KLOG_event_wait_all                    (KLOG_EVENT_FUNCTIONS+6)
#define KLOG_event_wait_all_ticks              (KLOG_EVENT_FUNCTIONS+7)
#define KLOG_event_wait_all_for                (KLOG_EVENT_FUNCTIONS+8)
#define KLOG_event_wait_all_until              (KLOG_EVENT_FUNCTIONS+9)
#define KLOG_event_wait_any                    (KLOG_EVENT_FUNCTIONS+10)
#define KLOG_event_wait_any_ticks              (KLOG_EVENT_FUNCTIONS+11)
#define KLOG_event_wait_any_for                (KLOG_EVENT_FUNCTIONS+12)
#define KLOG_event_wait_any_until              (KLOG_EVENT_FUNCTIONS+13)
#define KLOG_event_set                         (KLOG_EVENT_FUNCTIONS+14)
#define KLOG_event_clear                       (KLOG_EVENT_FUNCTIONS+15)
#define KLOG_event_create_fast                 (KLOG_EVENT_FUNCTIONS+16)
#define KLOG_event_open_fast                   (KLOG_EVENT_FUNCTIONS+17)
#define KLOG_event_destroy_fast                (KLOG_EVENT_FUNCTIONS+18)
#define KLOG_event_test                        (KLOG_EVENT_FUNCTIONS+19)
#define KLOG_lwevent_clear                     (KLOG_EVENT_FUNCTIONS+20)
#define KLOG_lwevent_create                    (KLOG_EVENT_FUNCTIONS+21)
#define KLOG_lwevent_destroy                   (KLOG_EVENT_FUNCTIONS+22)
#define KLOG_lwevent_set                       (KLOG_EVENT_FUNCTIONS+23)
#define KLOG_lwevent_test                      (KLOG_EVENT_FUNCTIONS+24)
#define KLOG_lwevent_wait                      (KLOG_EVENT_FUNCTIONS+25)
#define KLOG_lwevent_wait_for                  (KLOG_EVENT_FUNCTIONS+26)
#define KLOG_lwevent_wait_until                (KLOG_EVENT_FUNCTIONS+27)
#define KLOG_lwevent_wait_ticks                (KLOG_EVENT_FUNCTIONS+28)
#define KLOG_lwevent_set_auto_clear            (KLOG_EVENT_FUNCTIONS+29)
#define KLOG_lwevent_get_signalled             (KLOG_EVENT_FUNCTIONS+30)

/* Naming functions */
#define KLOG_name_create_component             (KLOG_NAME_FUNCTIONS+1)
#define KLOG_name_add                          (KLOG_NAME_FUNCTIONS+2)
#define KLOG_name_delete                       (KLOG_NAME_FUNCTIONS+3)
#define KLOG_name_find                         (KLOG_NAME_FUNCTIONS+4)
#define KLOG_name_find_name                    (KLOG_NAME_FUNCTIONS+5)
#define KLOG_name_test                         (KLOG_NAME_FUNCTIONS+6)

/* Mutex functions */
#define KLOG_mutatr_init                       (KLOG_MUTEX_FUNCTIONS+1)
#define KLOG_mutatr_destroy                    (KLOG_MUTEX_FUNCTIONS+2)
#define KLOG_mutatr_set_sched_protocol         (KLOG_MUTEX_FUNCTIONS+3)
#define KLOG_mutatr_set_priority_ceiling       (KLOG_MUTEX_FUNCTIONS+4)

#define KLOG_mutex_create_component            (KLOG_MUTEX_FUNCTIONS+5)
#define KLOG_mutex_init                        (KLOG_MUTEX_FUNCTIONS+6)
#define KLOG_mutex_set_priority_ceiling        (KLOG_MUTEX_FUNCTIONS+7)
#define KLOG_mutex_destroy                     (KLOG_MUTEX_FUNCTIONS+8)
#define KLOG_mutex_lock                        (KLOG_MUTEX_FUNCTIONS+9)
#define KLOG_mutex_try_lock                    (KLOG_MUTEX_FUNCTIONS+10)
#define KLOG_mutex_unlock                      (KLOG_MUTEX_FUNCTIONS+11)

#define KLOG_mutatr_set_wait_protocol          (KLOG_MUTEX_FUNCTIONS+12)
#define KLOG_mutatr_set_spin_limit             (KLOG_MUTEX_FUNCTIONS+13)
#define KLOG_mutex_test                        (KLOG_MUTEX_FUNCTIONS+14)

/* Semaphore functions */
#define KLOG_sem_create_component              (KLOG_SEMAPHORE_FUNCTIONS+1)
#define KLOG_sem_create                        (KLOG_SEMAPHORE_FUNCTIONS+2)
#define KLOG_sem_destroy                       (KLOG_SEMAPHORE_FUNCTIONS+3)
#define KLOG_sem_open                          (KLOG_SEMAPHORE_FUNCTIONS+4)
#define KLOG_sem_close                         (KLOG_SEMAPHORE_FUNCTIONS+5)
#define KLOG_sem_post                          (KLOG_SEMAPHORE_FUNCTIONS+6)
#define KLOG_sem_create_fast                   (KLOG_SEMAPHORE_FUNCTIONS+7)
#define KLOG_sem_open_fast                     (KLOG_SEMAPHORE_FUNCTIONS+8)
#define KLOG_sem_destroy_fast                  (KLOG_SEMAPHORE_FUNCTIONS+9)
#define KLOG_sem_test                          (KLOG_SEMAPHORE_FUNCTIONS+10)
#define KLOG_sem_wait                          (KLOG_SEMAPHORE_FUNCTIONS+11)
#define KLOG_sem_wait_for                      (KLOG_SEMAPHORE_FUNCTIONS+12)
#define KLOG_sem_wait_ticks                    (KLOG_SEMAPHORE_FUNCTIONS+13)
#define KLOG_sem_wait_until                    (KLOG_SEMAPHORE_FUNCTIONS+14)
#define KLOG_lwsem_create                      (KLOG_SEMAPHORE_FUNCTIONS+15)
#define KLOG_lwsem_wait                        (KLOG_SEMAPHORE_FUNCTIONS+16)
#define KLOG_lwsem_post                        (KLOG_SEMAPHORE_FUNCTIONS+17)
#define KLOG_lwsem_destroy                     (KLOG_SEMAPHORE_FUNCTIONS+18)
#define KLOG_lwsem_test                        (KLOG_SEMAPHORE_FUNCTIONS+19)
#define KLOG_lwsem_poll                        (KLOG_SEMAPHORE_FUNCTIONS+20)
#define KLOG_lwsem_wait_for                    (KLOG_SEMAPHORE_FUNCTIONS+21)
#define KLOG_lwsem_wait_ticks                  (KLOG_SEMAPHORE_FUNCTIONS+22)
#define KLOG_lwsem_wait_until                  (KLOG_SEMAPHORE_FUNCTIONS+23)

/* Watchdog functions */
#define KLOG_watchdog_create_component         (KLOG_WATCHDOG_FUNCTIONS+1)
#define KLOG_watchdog_stop                     (KLOG_WATCHDOG_FUNCTIONS+2)
#define KLOG_watchdog_start                    (KLOG_WATCHDOG_FUNCTIONS+3)
#define KLOG_watchdog_start_ticks              (KLOG_WATCHDOG_FUNCTIONS+4)
#define KLOG_watchdog_test                     (KLOG_WATCHDOG_FUNCTIONS+5)

/* EDS functions */
#define KLOG_eds_create_component              (KLOG_EDS_FUNCTIONS+1)
#define KLOG_eds_remove                        (KLOG_EDS_FUNCTIONS+2)


#define KLOG_ENET_send                         (KLOG_IO_FUNCTIONS + 1)


#endif
/* EOF */
