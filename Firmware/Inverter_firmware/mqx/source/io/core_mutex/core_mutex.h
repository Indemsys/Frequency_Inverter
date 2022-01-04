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
* $FileName: core_mutex.h$
* $Version : 3.8.2.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains the type definitions for the core mutex driver.
*
*END************************************************************************/

#ifndef __core_mutex_h__
#define __core_mutex_h__

#define __core_mutex_h__version "$Version:3.8.2.0$"
#define __core_mutex_h__date    "$Date:Sep-24-2012$"

#include <core_mutex_sema4.h>

/* Core Mutex driver error codes */
#define COREMUTEX_OK         (0)
#define COREMUTEX_LOCKED     (COREMUTEX_ERROR_BASE | 0x01) /* CM locked */
#define COREMUTEX_UNLOCKED   (COREMUTEX_ERROR_BASE | 0x02) /* CM unlocked */


#define CORE_MUTEX_VALID  (('c'<<24)|('m'<<24)|('t'<<24)|'x')

typedef struct core_mutex_component {
   CORE_MUTEX_DEVICE  DEVICE[PSP_NUM_CORES];
} CORE_MUTEX_COMPONENT, * CORE_MUTEX_COMPONENT_PTR;


typedef struct core_mutex_init_struct {
   uint_32     INT_PRIORITY;
} CORE_MUTEX_INIT_STRUCT, * CORE_MUTEX_INIT_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif

extern CORE_MUTEX_COMPONENT_PTR _core_mutext_get_component_ptr(void);
extern void                     _core_mutext_set_component_ptr(CORE_MUTEX_COMPONENT_PTR p);
extern uint_32                  _core_mutex_install( const CORE_MUTEX_INIT_STRUCT * init_ptr );
extern CORE_MUTEX_PTR           _core_mutex_create( uint_32 core_num, uint_32 mutex_num, uint_32 policy );
extern uint_32                  _core_mutex_create_at( CORE_MUTEX_PTR mutex_ptr, uint_32 core_num, uint_32 mutex_num, uint_32 policy );
extern uint_32                  _core_mutex_destroy( CORE_MUTEX_PTR mutex_ptr );
extern CORE_MUTEX_PTR           _core_mutex_get( uint_32 core_num, uint_32 mutex_num );
extern uint_32                  _core_mutex_lock( CORE_MUTEX_PTR core_mutex_ptr );
extern uint_32                  _core_mutex_trylock( CORE_MUTEX_PTR core_mutex_ptr );
extern uint_32                  _core_mutex_unlock( CORE_MUTEX_PTR core_mutex_ptr );
extern int_32                   _core_mutex_owner( CORE_MUTEX_PTR core_mutex_ptr );


#ifdef __cplusplus
}
#endif


#endif
