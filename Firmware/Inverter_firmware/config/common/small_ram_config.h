/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: small_ram_config.h$
* $Version : 3.8.17.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   MQX configuration set: for small-RAM devices
*
*END************************************************************************/

#ifndef __small_ram_config_h__
  #define __small_ram_config_h__

/* Disable heavy weight components */

  #ifndef MQX_USE_IPC
    #define MQX_USE_IPC                         0
  #endif

  #ifndef MQX_USE_LOGS
    #define MQX_USE_LOGS                        0
  #endif

  #ifndef MQX_USE_SEMAPHORES
    #define MQX_USE_SEMAPHORES                  1
  #endif

  #ifndef MQX_USE_SW_WATCHDOGS
    #define MQX_USE_SW_WATCHDOGS                0
  #endif

  #ifndef MQX_USE_TIMER
    #define MQX_USE_TIMER                       1
  #endif

/* Other configuration */

  #ifndef MQX_DEFAULT_TIME_SLICE_IN_TICKS
    #define MQX_DEFAULT_TIME_SLICE_IN_TICKS     1
  #endif

  #ifndef MQX_LWLOG_TIME_STAMP_IN_TICKS
    #define MQX_LWLOG_TIME_STAMP_IN_TICKS       1
  #endif

  #ifndef MQX_TIMER_USES_TICKS_ONLY
    #define MQX_TIMER_USES_TICKS_ONLY           1
  #endif

  #ifndef MQX_EXTRA_TASK_STACK_ENABLE
    #define MQX_EXTRA_TASK_STACK_ENABLE         0
  #endif

  #ifndef MQX_IS_MULTI_PROCESSOR
    #define MQX_IS_MULTI_PROCESSOR              0
  #endif

  #ifndef MQX_MUTEX_HAS_POLLING
    #define MQX_MUTEX_HAS_POLLING               0
  #endif

  #ifndef MQX_USE_INLINE_MACROS
    #define MQX_USE_INLINE_MACROS               0
  #endif

  #ifndef MQX_USE_LWMEM_ALLOCATOR
    #define MQX_USE_LWMEM_ALLOCATOR             1
  #endif

  #ifndef MQX_ROM_VECTORS
    #define MQX_ROM_VECTORS                     0
  #endif

  #ifndef MQX_USE_IDLE_TASK
    #define MQX_USE_IDLE_TASK                   1
  #endif

  #ifndef MQX_HAS_TIME_SLICE
    #define MQX_HAS_TIME_SLICE                  1
  #endif


  #ifndef MQX_KERNEL_LOGGING
    #define MQX_KERNEL_LOGGING                  0
  #endif

  #ifndef MQX_SPARSE_ISR_TABLE
    #define MQX_SPARSE_ISR_TABLE                0

    #ifndef MQX_SPARSE_ISR_SHIFT
      #define MQX_SPARSE_ISR_SHIFT              3
    #endif

  #endif /* MQX_SPARSE_ISR_TABLE */


  #ifndef MQX_TASK_DESTRUCTION
    #define MQX_TASK_DESTRUCTION                0
  #endif

  #ifndef MQX_EXIT_ENABLED
    #define MQX_EXIT_ENABLED                    0
  #endif

/*
** MFS settings
*/

  #ifndef MFSCFG_MINIMUM_FOOTPRINT
    #define MFSCFG_MINIMUM_FOOTPRINT            1
  #endif

/*
** RTCS settings
*/

  #ifndef RTCS_MINIMUM_FOOTPRINT
    #define RTCS_MINIMUM_FOOTPRINT              1
  #endif

  #ifndef RTCSCFG_ENABLE_LWDNS
    #define RTCSCFG_ENABLE_LWDNS                1
  #endif

#endif
/* EOF */
