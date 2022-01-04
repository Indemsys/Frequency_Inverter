/*HEADER********************************************************************
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
* $FileName: timer.h$
* $Version : 3.0.5.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   timer component.
*
*END************************************************************************/
#ifndef __timer_h__
#define __timer_h__ 1

#include <mqx_cnfg.h>
#if (! MQX_USE_TIMER) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error TIMER component is currently disabled in MQX kernel. Please set MQX_USE_TIMER to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* 
 * This mode tells the timer to use the elapsed time when calculating time 
 * (_time_get_elapsed)
 */
#define TIMER_ELAPSED_TIME_MODE       (1)

/* 
 * This mode tells the timer to use the actual time when calculating time
 * (_time_get)  Note that the time returned by _time_get can be modified
 * by _time_set.
 */
#define TIMER_KERNEL_TIME_MODE        (2)

/* The error return from the timer start functions */
#define TIMER_NULL_ID                 ((_timer_id)0)

/* The default parameters for the timer_create_component function */
#define TIMER_DEFAULT_TASK_PRIORITY   (1)
#define TIMER_DEFAULT_STACK_SIZE      (800)

/*--------------------------------------------------------------------------*/
/*                     DATA STRUCTURE DEFINITIONS                           */

typedef _mqx_uint  _timer_id;

/*--------------------------------------------------------------------------*/
/*
 *                    TYPEDEFS FOR _CODE_PTR_ FUNCTIONS
 */
typedef void (_CODE_PTR_  TIMER_NOTIFICATION_TIME_FPTR)( _timer_id, pointer, uint_32, uint_32);
typedef void (_CODE_PTR_  TIMER_NOTIFICATION_TICK_FPTR)( _timer_id, pointer, MQX_TICK_STRUCT_PTR);

/*--------------------------------------------------------------------------*/
/*                       EXTERNAL DECLARATIONS                              */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint  _timer_cancel(_timer_id);
extern _mqx_uint  _timer_create_component(_mqx_uint, _mqx_uint);
extern _timer_id  _timer_start_oneshot_after(
   TIMER_NOTIFICATION_TIME_FPTR, pointer, _mqx_uint, uint_32);
extern _timer_id  _timer_start_oneshot_at( 
   TIMER_NOTIFICATION_TIME_FPTR, pointer, _mqx_uint, TIME_STRUCT_PTR);        
extern _timer_id  _timer_start_periodic_every( 
   TIMER_NOTIFICATION_TIME_FPTR, pointer, _mqx_uint, uint_32);        
extern _timer_id  _timer_start_periodic_at( 
   TIMER_NOTIFICATION_TIME_FPTR, pointer, _mqx_uint, TIME_STRUCT_PTR, uint_32);
extern _timer_id  _timer_start_oneshot_after_ticks(
   TIMER_NOTIFICATION_TICK_FPTR, pointer, _mqx_uint, MQX_TICK_STRUCT_PTR);
extern _timer_id  _timer_start_oneshot_at_ticks( 
   TIMER_NOTIFICATION_TICK_FPTR, pointer, _mqx_uint, MQX_TICK_STRUCT_PTR);        
extern _timer_id  _timer_start_periodic_every_ticks( 
   TIMER_NOTIFICATION_TICK_FPTR, pointer, _mqx_uint, MQX_TICK_STRUCT_PTR);        
extern _timer_id  _timer_start_periodic_at_ticks( 
   TIMER_NOTIFICATION_TICK_FPTR, pointer, _mqx_uint,
   MQX_TICK_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
extern _mqx_uint  _timer_test(pointer _PTR_);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
