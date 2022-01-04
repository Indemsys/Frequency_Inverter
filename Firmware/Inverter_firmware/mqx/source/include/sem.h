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
* $FileName: sem.h$
* $Version : 3.0.5.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   semaphore component.
*
*END************************************************************************/
#ifndef __sem_h__
#define __sem_h__ 1

#include <mqx_cnfg.h>
#if (! MQX_USE_SEMAPHORES) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error SEMAPHORE component is currently disabled in MQX kernel. Please set MQX_USE_SEMAPHORES to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* ERROR messages */

#define SEM_MULTI_PROCESSOR_NOT_AVAILABLE   (SEM_ERROR_BASE|0x00)
#define SEM_SEMAPHORE_DELETED               (SEM_ERROR_BASE|0x01)
#define SEM_SEMAPHORE_NOT_DELETED           (SEM_ERROR_BASE|0x02)
#define SEM_INVALID_SEMAPHORE_HANDLE        (SEM_ERROR_BASE|0x03)
#define SEM_CANNOT_POST                     (SEM_ERROR_BASE|0x04)
#define SEM_CANNOT_GET_SEMAPHORE            (SEM_ERROR_BASE|0x05)
#define SEM_INVALID_SEMAPHORE_COUNT         (SEM_ERROR_BASE|0x06)
#define SEM_WAIT_TIMEOUT                    (SEM_ERROR_BASE|0x07)
#define SEM_SEMAPHORE_EXISTS                (SEM_ERROR_BASE|0x08)
#define SEM_SEMAPHORE_TABLE_FULL            (SEM_ERROR_BASE|0x09)
#define SEM_SEMAPHORE_NOT_FOUND             (SEM_ERROR_BASE|0x0A)
#define SEM_INVALID_POLICY                  (SEM_ERROR_BASE|0x0B)
#define SEM_INVALID_SEMAPHORE               (SEM_ERROR_BASE|0x0C)
#define SEM_INCORRECT_INITIAL_COUNT         (SEM_ERROR_BASE|0x0D)

/* bit flag parameters for _sem_create */

#define SEM_PRIORITY_QUEUEING               (1)
#define SEM_PRIORITY_INHERITANCE            (2)
#define SEM_STRICT                          (4)

/* Default component creation parameters */
#define SEM_DEFAULT_INITIAL_NUMBER          (8)
#define SEM_DEFAULT_GROW_NUMBER             (8)
#define SEM_DEFAULT_MAXIMUM_NUMBER          (0) /* Unlimited */

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _sem_close(pointer);
extern _mqx_uint _sem_create(char _PTR_, _mqx_uint, _mqx_uint);
extern _mqx_uint _sem_create_fast(_mqx_uint, _mqx_uint, _mqx_uint);
extern _mqx_uint _sem_create_component(_mqx_uint, _mqx_uint, _mqx_uint);
extern _mqx_uint _sem_destroy(char _PTR_, boolean);
extern _mqx_uint _sem_destroy_fast(_mqx_uint, boolean);
extern _mqx_uint _sem_get_wait_count(pointer);
extern _mqx_uint _sem_open(char _PTR_, pointer _PTR_);
extern _mqx_uint _sem_open_fast(_mqx_uint, pointer _PTR_);
extern _mqx_uint _sem_post(pointer);
extern _mqx_uint _sem_test(pointer _PTR_);
extern _mqx_uint _sem_get_value(pointer);
extern _mqx_uint _sem_wait(pointer, uint_32);
extern _mqx_uint _sem_wait_for(pointer, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _sem_wait_ticks(pointer, _mqx_uint);
extern _mqx_uint _sem_wait_until(pointer, MQX_TICK_STRUCT_PTR);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
