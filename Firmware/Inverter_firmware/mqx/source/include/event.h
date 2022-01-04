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
* $FileName: event.h$
* $Version : 3.0.5.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   event component.
*
*END************************************************************************/
#ifndef __event_h__
#define __event_h__ 1

#include <mqx_cnfg.h>
#if (! MQX_USE_EVENTS) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error EVENT component is currently disabled in MQX kernel. Please set MQX_USE_EVENTS to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* ERROR messages */

#define EVENT_MULTI_PROCESSOR_NOT_AVAILABLE     (EVENT_ERROR_BASE|0x00)
#define EVENT_DELETED                           (EVENT_ERROR_BASE|0x01)
#define EVENT_NOT_DELETED                       (EVENT_ERROR_BASE|0x02)
#define EVENT_INVALID_EVENT_HANDLE              (EVENT_ERROR_BASE|0x03)
#define EVENT_CANNOT_SET                        (EVENT_ERROR_BASE|0x04)
#define EVENT_CANNOT_GET_EVENT                  (EVENT_ERROR_BASE|0x05)
#define EVENT_INVALID_EVENT_COUNT               (EVENT_ERROR_BASE|0x06)
#define EVENT_WAIT_TIMEOUT                      (EVENT_ERROR_BASE|0x07)
#define EVENT_EXISTS                            (EVENT_ERROR_BASE|0x08)
#define EVENT_TABLE_FULL                        (EVENT_ERROR_BASE|0x09)
#define EVENT_NOT_FOUND                         (EVENT_ERROR_BASE|0x0A)
#define EVENT_INVALID_EVENT                     (EVENT_ERROR_BASE|0x0B)
#define EVENT_CANNOT_WAIT_ON_REMOTE_EVENT       (EVENT_ERROR_BASE|0x0C)

/* Default component creation parameters */
#define EVENT_DEFAULT_INITIAL_NUMBER            (8)
#define EVENT_DEFAULT_GROW_NUMBER               (8)
#define EVENT_DEFAULT_MAXIMUM_NUMBER            (0) /* Unlimited */

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _event_clear(pointer, _mqx_uint);
extern _mqx_uint _event_close(pointer);
extern _mqx_uint _event_create_component(_mqx_uint, _mqx_uint, _mqx_uint);
extern _mqx_uint _event_create_fast(_mqx_uint);
extern _mqx_uint _event_create_fast_auto_clear(_mqx_uint);
extern _mqx_uint _event_create(char _PTR_);
extern _mqx_uint _event_create_auto_clear(char _PTR_);
extern _mqx_uint _event_destroy(char _PTR_);
extern _mqx_uint _event_destroy_fast(_mqx_uint);
extern _mqx_uint _event_get_value(pointer, _mqx_uint_ptr);
extern _mqx_uint _event_get_wait_count(pointer);
extern _mqx_uint _event_open(char _PTR_, pointer _PTR_);
extern _mqx_uint _event_open_fast(_mqx_uint, pointer _PTR_);
extern _mqx_uint _event_set(pointer, _mqx_uint);
extern _mqx_uint _event_test(pointer _PTR_);
extern _mqx_uint _event_wait_all(pointer, _mqx_uint, uint_32);
extern _mqx_uint _event_wait_all_for(pointer, _mqx_uint, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _event_wait_all_ticks(pointer, _mqx_uint, _mqx_uint);
extern _mqx_uint _event_wait_all_until(pointer, _mqx_uint, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _event_wait_any(pointer, _mqx_uint, uint_32);
extern _mqx_uint _event_wait_any_for(pointer, _mqx_uint, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _event_wait_any_ticks(pointer, _mqx_uint, _mqx_uint);
extern _mqx_uint _event_wait_any_until(pointer, _mqx_uint, MQX_TICK_STRUCT_PTR);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
