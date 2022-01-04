/**HEADER********************************************************************
*
* Copyright (c) 2008-2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2011 Embedded Access Inc.;
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
* $FileName: core_mutex_sema4.c$
* $Version : 3.8.3.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains the source functions for functions required to
*   control the PBRIDGE.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <core_mutex.h>
#include <core_mutex_sema4.h>

#define _sema4_int_disable() _int_disable()
#define _sema4_int_enable()  _int_enable()


/*FUNCTION****************************************************************
*
* Function Name    : _sema4_isr
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    This function sets the appropriate access control for the specified
*    register number.
*
*END*********************************************************************/
#if PSP_MQX_CPU_IS_VYBRID
static const int idx[16] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12};
#else
static const int idx[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
#endif

static void _sema4_isr(pointer p)
{
    CORE_MUTEX_DEVICE_PTR   device_ptr = (CORE_MUTEX_DEVICE_PTR) p;
    uint_32                 i;
    vuchar_ptr              gate_ptr;
    uchar                   lock = _psp_core_num()+1;
    uint_16                 mask = 0x8000;
    boolean                 waiting;

    for (i=0;i<SEMA4_NUM_GATES;i++) {
        if (*device_ptr->CPNTF_PTR & mask ) {
            // An interrupt is pending on this mutex, the only way to clear it is to lock it (either
            // by this core or another)
            gate_ptr = device_ptr->MUTEX_PTR[idx[i]]->GATE_PTR;
            *gate_ptr = lock;
            if (*gate_ptr == lock) {
                // Now, check to see if any task is waiting for it
                waiting = FALSE;
                if (device_ptr->MUTEX_PTR[idx[i]]) {
                    if (_taskq_get_value(device_ptr->MUTEX_PTR[idx[i]]->WAIT_Q)) {
                        _taskq_resume(device_ptr->MUTEX_PTR[idx[i]]->WAIT_Q, FALSE);
                        waiting = TRUE;
                    }
                }
                if (!waiting) {
                    // No task was waiting, give it back - this can occur due to a failed trylock
                    *gate_ptr = SEMA4_UNLOCK;
                }
            }
        }
        mask >>= 1;
   }
}

/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_install
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    This function sets the appropriate access control for the specified
*    register number.
*
*END*********************************************************************/

uint_32 _core_mutex_install( const CORE_MUTEX_INIT_STRUCT *init_ptr )
{
   CORE_MUTEX_COMPONENT_PTR   component_ptr = _core_mutext_get_component_ptr();
   PSP_INTERRUPT_TABLE_INDEX  vector;
   uint_32                    i;

#if MQX_CHECK_ERRORS
   if (component_ptr!=NULL) {
      return MQX_COMPONENT_EXISTS;
   }
#endif

   component_ptr = _mem_alloc_system_zero(sizeof(*component_ptr));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (component_ptr==NULL) {
       return MQX_OUT_OF_MEMORY;
   }
#endif

   for (i=0;i<SEMA4_NUM_DEVICES;i++) {
       vector = _bsp_get_sema4_vector(i);
       component_ptr->DEVICE[i].SEMA4_PTR = _bsp_get_sema4_base_address(i);
       if (_psp_core_num()==0) {
           component_ptr->DEVICE[i].CPNTF_PTR = &(component_ptr->DEVICE[i].SEMA4_PTR->CP0NTF);
       } else {
           component_ptr->DEVICE[i].CPNTF_PTR = &(component_ptr->DEVICE[i].SEMA4_PTR->CP1NTF);
       }
#if MQX_CHECK_ERRORS
       if ((component_ptr->DEVICE[i].SEMA4_PTR == NULL) || (vector==0)) {
           _mem_free(component_ptr);
           return MQX_INVALID_DEVICE;
       }
#endif
   }

   _int_disable();
#if MQX_CHECK_ERRORS
   if (_core_mutext_get_component_ptr()) {
       _int_enable();
       _mem_free(component_ptr);
       return MQX_COMPONENT_EXISTS;
   }
#endif

   _core_mutext_set_component_ptr(component_ptr);
   _int_enable();


   for (i=0;i<SEMA4_NUM_DEVICES;i++) {
       vector = _bsp_get_sema4_vector(i);
       _int_install_isr(vector, _sema4_isr, &component_ptr->DEVICE[i]);
       _bsp_int_init(vector, init_ptr->INT_PRIORITY, 0, TRUE);
   }

   return COREMUTEX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_create_internal
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    This function creates a core mutex at the specified location.
*
*END*********************************************************************/

uint_32 _core_mutex_create_internal(CORE_MUTEX_PTR mutex_ptr, uint_32 core_num, uint_32 mutex_num, uint_32 policy, boolean allocated )
{
    CORE_MUTEX_COMPONENT_PTR component_ptr = _core_mutext_get_component_ptr();

#if MQX_CHECK_ERRORS
    if (component_ptr == NULL) {
        return MQX_COMPONENT_DOES_NOT_EXIST;
    }

    // range check mutex_id
    if (mutex_num>=SEMA4_NUM_GATES) {
        return (MQX_INVALID_PARAMETER);
    }

    if (core_num>=PSP_NUM_CORES) {
        return (MQX_INVALID_PARAMETER);
    }

    if (mutex_ptr==NULL) {
        return MQX_INVALID_PARAMETER;
    }
#endif

    mutex_ptr->VALID    = CORE_MUTEX_VALID;
    mutex_ptr->GATE_NUM = mutex_num;
    mutex_ptr->GATE_PTR = &(component_ptr->DEVICE[core_num].SEMA4_PTR->GATE[mutex_ptr->GATE_NUM]);
    mutex_ptr->WAIT_Q   = _taskq_create(policy);
#if MQX_CHECK_ERRORS
    if (mutex_ptr->WAIT_Q==NULL) {
        return MQX_TASKQ_CREATE_FAILED;
    }
#endif

    _int_disable();
#if MQX_CHECK_ERRORS
    if (component_ptr->DEVICE[core_num].MUTEX_PTR[mutex_num] !=NULL) {
        _int_enable();
        _taskq_destroy(mutex_ptr->WAIT_Q);
        return MQX_COMPONENT_EXISTS;
    }
#endif
    component_ptr->DEVICE[core_num].MUTEX_PTR[mutex_num]=mutex_ptr;

    // Need to remember if we allocated this mutex, or user did
    if (allocated) {
       component_ptr->DEVICE[core_num].ALLOCED |= (1<<mutex_num);
    }

    // Enable 'unlocked' interrupt for this core
    if (_psp_core_num()==0) {
        component_ptr->DEVICE[core_num].SEMA4_PTR->CP0INE |= 1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]);	//1 << (SEMA4_NUM_GATES-1-mutex_num);
    } else {
        component_ptr->DEVICE[core_num].SEMA4_PTR->CP1INE |= 1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]);	//1 << (SEMA4_NUM_GATES-1-mutex_num);
    }
    _int_enable();

    return COREMUTEX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_create
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    This function allocates and creates a core mutex.
*
*END*********************************************************************/

CORE_MUTEX_PTR _core_mutex_create(uint_32 core_num, uint_32 mutex_num, uint_32 policy )
{
    CORE_MUTEX_PTR mutex_ptr;
    uint_32        result;

    mutex_ptr = _mem_alloc_system_zero(sizeof(*mutex_ptr));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (mutex_ptr==NULL) {
        _task_set_error(MQX_OUT_OF_MEMORY);
        return NULL;
    }
#endif

    result = _core_mutex_create_internal(mutex_ptr, core_num, mutex_num, policy, TRUE );
#if MQX_CHECK_ERRORS
    if (result != COREMUTEX_OK) {
        _mem_free(mutex_ptr);
        _task_set_error(result);
        mutex_ptr = NULL;
    }
#endif
    return mutex_ptr;
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_create_at
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    This function creates a core mutex at the specified location.
*
*END*********************************************************************/


uint_32 _core_mutex_create_at( CORE_MUTEX_PTR mutex_ptr, uint_32 core_num, uint_32 mutex_num, uint_32 policy )
{
#if MQX_CHECK_ERRORS
    if (mutex_ptr==NULL) {
        return MQX_INVALID_PARAMETER;
    }
#endif

    return _core_mutex_create_internal(mutex_ptr, core_num, mutex_num, policy, FALSE );
}

/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_destroy
* Returned Value   : MQX error code
* Comments         :
*    This function creates a core mutex at the specified location.
*
*END*********************************************************************/


uint_32 _core_mutex_destroy( CORE_MUTEX_PTR mutex_ptr )
{
    CORE_MUTEX_COMPONENT_PTR    component_ptr = _core_mutext_get_component_ptr();
    SEMA4_MemMapPtr sema4_ptr;
    pointer                     tq;
    uint_32                     core_num, mutex_num,i;

#if MQX_CHECK_ERRORS
    if (component_ptr == NULL) {
       return MQX_COMPONENT_DOES_NOT_EXIST;
    }

    if (mutex_ptr==NULL) {
        return MQX_INVALID_POINTER;
    }

    if (mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    mutex_num = mutex_ptr->GATE_NUM;

    // figure out which device this mutex is associated with
    for (i=0;i<SEMA4_NUM_DEVICES;i++) {
        sema4_ptr = _bsp_get_sema4_base_address(i);
        if (&sema4_ptr->GATE[mutex_num] == mutex_ptr->GATE_PTR) {
            core_num = i;
            break;
        }
    }

    _int_disable();
#if MQX_CHECK_ERRORS
    if (component_ptr->DEVICE[core_num].MUTEX_PTR[mutex_num] == NULL) {
       _int_enable();
       return MQX_COMPONENT_DOES_NOT_EXIST;
    }
#endif

    component_ptr->DEVICE[core_num].MUTEX_PTR[mutex_num] = NULL;

    if (_psp_core_num()==0) {
        component_ptr->DEVICE[core_num].SEMA4_PTR->CP0INE &= ~(1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]));	//~(1 << (SEMA4_NUM_GATES-1-mutex_num));
    } else {
        component_ptr->DEVICE[core_num].SEMA4_PTR->CP1INE &= ~(1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]));	//~(1 << (SEMA4_NUM_GATES-1-mutex_num));
    }
    tq = mutex_ptr->WAIT_Q;
    mutex_ptr->VALID=0;
    _int_enable();

    if (component_ptr->DEVICE[core_num].ALLOCED & (1<<mutex_num)) {
        component_ptr->DEVICE[core_num].ALLOCED &=  ~(1<<mutex_num);
    } else {
        mutex_ptr=NULL;
    }
    _taskq_destroy(tq);
    if (mutex_ptr) {
        _mem_free(mutex_ptr);
    }

    return COREMUTEX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_get
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    This function sets the appropriate access control for the specified
*    register number.
*
*END*********************************************************************/

CORE_MUTEX_PTR _core_mutex_get(uint_32 core_num, uint_32 mutex_num )
{
   CORE_MUTEX_COMPONENT_PTR component_ptr = _core_mutext_get_component_ptr();
#if MQX_CHECK_ERRORS
   if (component_ptr == NULL) {
      _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
      return NULL;
   }

   if (mutex_num>=SEMA4_NUM_GATES) {
      _task_set_error(MQX_INVALID_PARAMETER);
      return NULL;
   }

   if (core_num>=PSP_NUM_CORES) {
      _task_set_error(MQX_INVALID_PARAMETER);
      return NULL;
   }
#endif
   return component_ptr->DEVICE[core_num].MUTEX_PTR[mutex_num];
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_lock
* Returned Value   : COREMUTEX_OK if successful, error code otherwise
* Comments         :
*    This function locks the specified core mutex.
*
*END*********************************************************************/

uint_32 _core_mutex_lock( CORE_MUTEX_PTR core_mutex_ptr )
{
    uchar lock = _psp_core_num()+1;
#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    _sema4_int_disable();
#if BSPCFG_CORE_MUTEX_STATS
    core_mutex_ptr->LOCKS++;
#endif

    // Check to see if this core already own it
    if (*core_mutex_ptr->GATE_PTR == lock) {
        // Yes, then we have to wait for owning task to release it
        #if BSPCFG_CORE_MUTEX_STATS
            core_mutex_ptr->WAITS++;
        #endif
        _taskq_suspend(core_mutex_ptr->WAIT_Q);

    } else {
        // can only try to lock the mutex if another task is not waiting  otherwise we need to get in line
        if (_taskq_get_value(core_mutex_ptr->WAIT_Q)==0) {
            *core_mutex_ptr->GATE_PTR = lock;
        }
        if (*core_mutex_ptr->GATE_PTR != lock) {
            #if BSPCFG_CORE_MUTEX_STATS
                core_mutex_ptr->WAITS++;
            #endif
            _taskq_suspend(core_mutex_ptr->WAIT_Q);
            // Our turn now
        }
    }
    _sema4_int_enable();

    return COREMUTEX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_trylock
* Returned Value   : COREMUTEX_LOCKED if locked, COREMUTEX_UNLOCKED if unlocked
* Comments         :
*    This function tries to lock the specified core mutex.
*
*END*********************************************************************/

uint_32 _core_mutex_trylock( CORE_MUTEX_PTR core_mutex_ptr )
{
    vuchar_ptr  gate_ptr;
    uchar       lock = _psp_core_num()+1;
    boolean     locked = FALSE;

#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    gate_ptr = core_mutex_ptr->GATE_PTR;

    _sema4_int_disable();
    // If any other task is pending on the semaphore, then it's already locked
    if (_taskq_get_value(core_mutex_ptr->WAIT_Q)==0) {
        // Or if it's not unlocked...
        if (*gate_ptr == SEMA4_UNLOCK) {
            *gate_ptr = lock;
            // double check to ensure another core didn't lock it before we could
            locked = (*gate_ptr == lock);
        }
    }
    _sema4_int_enable();

    if(locked) {
        return COREMUTEX_LOCKED;
    }
    else {
        return COREMUTEX_UNLOCKED;
    }
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_unlock
* Returned Value   : COREMUTEX_OK if successful, error code otherwise
* Comments         :
*    This function unlocks a core mutex.
*
*END*********************************************************************/

uint_32 _core_mutex_unlock( CORE_MUTEX_PTR core_mutex_ptr )
{
    uchar lock = _psp_core_num()+1;
    vuchar_ptr gate_ptr;

#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    gate_ptr = core_mutex_ptr->GATE_PTR;

    // Make sure it is locked by this core
    if ( *gate_ptr != lock) {
        return MQX_NOT_RESOURCE_OWNER;
    }

#if BSPCFG_CORE_MUTEX_STATS
    core_mutex_ptr->UNLOCKS++;
#endif

    // Unlock if
    *gate_ptr = SEMA4_UNLOCK;


    // See if this core has any other tasks waiting for this lock
    if (_taskq_get_value(core_mutex_ptr->WAIT_Q)) {

        // if so, have to queue the next request
        _sema4_int_disable();
        *gate_ptr = lock;
        if (*gate_ptr == lock) {
            // if we got it, wake up the next task
            _taskq_resume(core_mutex_ptr->WAIT_Q, FALSE);
        }
        _sema4_int_enable();
    }

    return COREMUTEX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutex_owner
* Returned Value   : core id of mutex owner, -1 if no owner
* Comments         :
*
*END*********************************************************************/

int_32 _core_mutex_owner( CORE_MUTEX_PTR core_mutex_ptr )
{
#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    return *core_mutex_ptr->GATE_PTR - 1;
}
