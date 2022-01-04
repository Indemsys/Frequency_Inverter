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
* $FileName: mutex.h$
* $Version : 3.8.5.1$
* $Date    : Feb-20-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   mutex component
*
*END************************************************************************/

#ifndef __mutex_h__
#define __mutex_h__ 1

#include <mqx_cnfg.h>

#if (! MQX_USE_MUTEXES) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error MUTEX component is currently disabled in MQX kernel. Please set MQX_USE_MUTEXES to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*
 *                      CONSTANT DECLARATIONS
 */

/*
 * Mutex waiting policies, only ONE may be selected
 *   SPIN means the task does NOT Block
 *   QUEUEING means the task blocks
 */
#define MUTEX_SPIN_ONLY            (0x01)
#define MUTEX_LIMITED_SPIN         (0x02)
#define MUTEX_QUEUEING             (0x04)
#define MUTEX_PRIORITY_QUEUEING    (0x08)

/*
 * Mutex priority policies, only ONE may be selected
 *   INHERITANCE means the task owning the MUTEX runs at the priority of
 *     the task with the highest priority waiting for the mutex
 *   PROTECT means the task runs at the priority specified in the MUTEX
 *     (unless it is of higher priority already) (priority ceiling)
 *
 */
#define MUTEX_NO_PRIO_INHERIT            (0x0000)
#define MUTEX_PRIO_INHERIT               (0x0100)
#define MUTEX_PRIO_PROTECT               (0x0200)

/* What a mutex valid field should be */
#define MUTEX_VALID                      (_mqx_uint)(0x6d757478)   /* "mutx" */

/*
 * The static initializer for a mutex,
 * NOTE: This implies that this mutex will be "anonymous" and
 * the kernel will not know about this mutex
 */
#define MUTEX_INITIALIZER \
{ \
   NULL, NULL, MUTEX_QUEUEING, MUTEX_VALID, 0, 0, 0, 0, 0, \
   {NULL, NULL, 0, -1}, NULL \
}

/*--------------------------------------------------------------------------*/
/*
 *                      DATATYPE DECLARATIONS
 */

/*!
 * \brief Mutex attributes, which are used to initialize a mutex.
 * 
 * \see _mutatr_destroy
 * \see _mutatr_init  
 * \see _mutatr_get_priority_ceiling
 * \see _mutatr_set_priority_ceiling
 * \see _mutatr_get_sched_protocol
 * \see _mutatr_set_sched_protocol
 * \see _mutatr_get_spin_limit
 * \see _mutatr_set_spin_limit
 * \see _mutatr_get_wait_protocol
 * \see _mutatr_set_wait_protocol 
 * \see _mutex_init        
 */
typedef struct mutex_attr_struct
{

   /*! 
    * \brief Scheduling protocol.
    *        
    * A task using this mutex should follow when it owns the mutex.
    * \n One of the following:
    * \n - MUTEX_NO_PRIO_INHERIT
    * \n - MUTEX_PRIO_INHERIT
    * \n - MUTEX_PRIO_PROTECT
    * \n - MUTEX_PRIO_INHERIT | MUTEX_PRIO_PROTECT        
    */
   _mqx_uint SCHED_PROTOCOL;

   /*! 
    * \brief A validation field for mutexes. 
    *     
    * When a task calls _mutatr_init(), MQX sets the field to MUTEX_VALID and does 
    * not change it. If the field changes, MQX considers the attributes invalid.
    * \n The function _mutatr_init() sets the field to TRUE; _mutatr_destroy() 
    * sets it to FALSE.   
    */
   _mqx_uint VALID;

   /*!
    * \brief Priority of the mutex; applicable only if the scheduling protocol is
    * priority protect.
    */
   _mqx_uint PRIORITY_CEILING;

   /*!
    * \brief Number of spins to use if the waiting protocol is limited spin. 
    */
   _mqx_uint COUNT;

   /*!
    * \brief The waiting protocol a task using this mutex should follow when
    * a mutex is not available
    * 
    * One of the following:
    * \n - MUTEX_SPIN_ONLY
    * \n If the mutex is already locked, MQX timeslices the task until another 
    * task unlocks the mutex.  
    * \n - MUTEX_LIMITED_SPIN
    * \n If the mutex is already locked, MQX timeslices the task for a number
    * of times before the lock attempt fails.
    * \n If this is set, the spin limit should be set.
    * \n - MUTEX_QUEUEING
    * \n If the mutex is already locked, MQX blocks the task until another task
    * unlocks the mutex, at which time MQX gives the mutex to the first task that
    * requested it. 
    * \n - MUTEX_PRIORITY_QUEUEING        
    * \n If the mutex is already locked, MQX blocks the task until another task
    * unlocks the mutex, at which time MQX gives the mutex to the  highest-priority
    * task that is waiting for it. 
    */
   _mqx_uint WAIT_PROTOCOL;

} MUTEX_ATTR_STRUCT, _PTR_ MUTEX_ATTR_STRUCT_PTR;


/*!
 * \brief This structure defines the mutual exclusion (MUTEX) data structure.
 * 
 * \see _mutex_destroy
 * \see _mutex_get_priority_ceiling
 * \see _mutex_get_wait_count  
 * \see _mutex_init
 * \see _mutex_lock
 * \see _mutex_set_priority_ceiling 
 * \see _mutex_try_lock
 * \see _mutex_unlock   
 * \see MUTEX_ATTR_STRUCT   
 */
typedef struct mutex_struct
{

   /*! \brief Link pointers to maintain a list of mutexes in the kernel. */
   QUEUE_ELEMENT_STRUCT  LINK;

   /*! 
    * \brief Waiting protocol (most significant word) and scheduling protocol
    * (least significant word) for the mutex. 
    */
   _mqx_uint             PROTOCOLS;

   /*! 
    * \brief A validation field for mutexes.
    * 
    * When a task calls _mutex_init(), MQX sets the field to MUTEX_VALID and does 
    * not change it. If the field changes, MQX considers the mutex invalid.
    */
   _mqx_uint             VALID;

   /*!
    * \brief Priority of the mutex.
    * 
    * If the scheduling protocol is priority protect, MQX grants the mutex only
    * to tasks with at least this priority. 
    */
   _mqx_uint             PRIORITY_CEILING;

   /*!
    * \brief Maximum number of spins.
    * 
    * The field is used only if the waiting protocol is limited spin.
    */
   _mqx_uint             COUNT;

   /*! \brief TRUE if the mutex is being destroyed. */
   uint_16               DELAYED_DESTROY;

   /*! \brief Most significant bit is set when the mutex is locked. */
   uchar                 LOCK;

   /*! \brief An alignment filler. */
   uchar                 FILLER;

   /*! 
    * \brief A queue of tasks waiting for the mutex.
    * 
    * If PRIORITY_INHERITANCE is set, the queue is in priority order; otherwise
    * it is in FIFO order.         
    */
   QUEUE_STRUCT          WAITING_TASKS;

   /*! \brief Task descriptor of the task that has locked the mutex. */
   pointer               OWNER_TD;

   /*! 
    * \brief Number of times that MQX has boosted the priority of the task that
    * has locked the mutex. 
    */
   _mqx_uint             BOOSTED;

} MUTEX_STRUCT, _PTR_ MUTEX_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
 *                      'C' FUNCTION PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _mutatr_destroy(MUTEX_ATTR_STRUCT_PTR);
extern _mqx_uint _mutatr_get_priority_ceiling(MUTEX_ATTR_STRUCT_PTR, _mqx_uint_ptr);
extern _mqx_uint _mutatr_get_sched_protocol(MUTEX_ATTR_STRUCT_PTR, _mqx_uint_ptr);
extern _mqx_uint _mutatr_get_spin_limit(MUTEX_ATTR_STRUCT_PTR, _mqx_uint_ptr);
extern _mqx_uint _mutatr_get_wait_protocol(MUTEX_ATTR_STRUCT_PTR, _mqx_uint_ptr);
extern _mqx_uint _mutatr_set_priority_ceiling(MUTEX_ATTR_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _mutatr_set_sched_protocol(MUTEX_ATTR_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _mutatr_set_spin_limit(MUTEX_ATTR_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _mutatr_set_wait_protocol(MUTEX_ATTR_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _mutatr_init(MUTEX_ATTR_STRUCT_PTR);

/* Mutex functions */
extern _mqx_uint _mutex_create_component( void );
extern _mqx_uint _mutex_destroy(MUTEX_STRUCT_PTR);
extern _mqx_uint _mutex_get_priority_ceiling(MUTEX_STRUCT_PTR, _mqx_uint_ptr);
extern _mqx_uint _mutex_get_wait_count(MUTEX_STRUCT_PTR);
extern _mqx_uint _mutex_init(MUTEX_STRUCT_PTR, MUTEX_ATTR_STRUCT_PTR);
extern _mqx_uint _mutex_lock(MUTEX_STRUCT_PTR);
extern _mqx_uint _mutex_set_priority_ceiling(MUTEX_STRUCT_PTR, _mqx_uint, _mqx_uint_ptr);
extern _mqx_uint _mutex_test(pointer _PTR_);
extern _mqx_uint _mutex_try_lock(MUTEX_STRUCT_PTR);
extern _mqx_uint _mutex_unlock(MUTEX_STRUCT_PTR);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __mutex_h__ */
/* EOF */
