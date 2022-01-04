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
* $FileName: sem_prv.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   semaphore component.
*
*END************************************************************************/
#ifndef __sem_prv_h__
#define __sem_prv_h__ 1

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* Used to mark a block of memory as belonging to the semaphore component */
#define SEM_VALID                     ((_mqx_uint)(0x73656d20))   /* "sem " */

#define SEM_MAX_WAITING_TASKS         ((uint_16)0)  /* Unlimited */

/* Flag bits for the connection */
#define SEM_WANT_SEMAPHORE            (0x1001)
#define SEM_AVAILABLE                 (0x1002)


/*--------------------------------------------------------------------------*/
/*                      DATA STRUCTURE DEFINITIONS                          */

/* SEM COMPONENT STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This is the base semaphore structure pointed to by the kernel data
 * structure COMPONENT field for semaphores
 */
typedef struct sem_component_struct
{

   /*! \brief The maximum number of semaphores allowed. */
   _mqx_uint        MAXIMUM_NUMBER;

   /*! \brief The number of semaphores to grow by when table full. */
   _mqx_uint        GROW_NUMBER;

   /*! \brief A validation stamp allowing for checking of memory overwrite. */
   _mqx_uint        VALID;

   /*! \brief The handle to the name table for semaphores. */
   pointer          NAME_TABLE_HANDLE;

} SEM_COMPONENT_STRUCT, _PTR_ SEM_COMPONENT_STRUCT_PTR;
/*! \endcond */

/* SEM STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This is the structure of an instance of a semaphore. 
 * 
 * The address is kept in the semaphore name table, associated with the name.
 */
typedef struct sem_struct
{

   /*! 
    * \brief This semaphore will be destroyed when all waiting tasks relinquish
    * their semaphore. Other tasks may not wait any more.
    */
   boolean         DELAYED_DESTROY;

   /*! 
    * \brief Policy settings. Specifies whether this semaphore has 
    * PRIORITY_QUEUEING, and/or PRIORITY_INHERITANCE.
    */
   _mqx_uint       POLICY;

   /*! 
    * \brief This is the queue of waiting tasks.  
    * 
    * This queue may be sorted in order of task priority it INHERITANCE is 
    * enabled. What is queued is the address of the handle provided to the user 
    * (SEM_COMPONENT_STRUCT).
    */
   QUEUE_STRUCT    WAITING_TASKS;

   /*! \brief The queue of tasks; each of which owns a semaphore. */
   QUEUE_STRUCT    OWNING_TASKS;

   /*! \brief The current count of the semaphore. */
   _mqx_uint       COUNT;

   /*! 
    * \brief The maximum count allowed for this semaphore.
    *     
    * Only used when SEM_STRICT is TRUE.
    */
   _mqx_uint       MAX_COUNT;

   /*! \brief A validation stamp for the semaphore. */
   _mqx_uint       VALID;
   
   /*! \brief The string name of the semaphore, includes NULL. */
   char            NAME[NAME_MAX_NAME_SIZE];

} SEM_STRUCT, _PTR_ SEM_STRUCT_PTR;
/*! \endcond */

/* SEM CONNECTION STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE   
 * 
 * \brief This is the struture whose address is returned to the user as a 
 * semaphore handle.
 */
typedef struct sem_connection_struct
{

   /*! \brief Pointer to the next item in WAITING TASK queue of the semaphore. */ 
   pointer         NEXT;
   /*! \brief Pointer to the previous item in WAITING TASK queue of the semaphore. */
   pointer         PREV;

   /*! \brief A validation stamp for the data structure. */
   _mqx_uint       VALID;

   /*! \brief The number of times this task has been priority boosted. */
   _mqx_uint       BOOSTED;

   /* A reserved field */
   /* Start SPR P171-0009-01     */
   /* _mqx_uint        RESERVED; */
   /* End SPR P171-0009-01       */

   /*! 
    * \brief This field is only used for STRICT semaphores.
    *     
    * A count of the number of semaphores owned by this task.
    * \n This is incremented by _sem_wait, and decremented by _sem_post.
    * \n A semaphore may be posted only if a wait has previously succeeded.
    */
   /* Start SPR P171-0010-01 */
   /* int        POST_STATE; */
   _mqx_int        POST_STATE;
   /* End SPR P171-0010-01   */
   
   /*! \brief The address of the task descriptor that owns this connection. */
   TD_STRUCT_PTR   TD_PTR;

   /*! 
    * \brief The address of the semaphore structure associated with this
    * connection. 
    */
   SEM_STRUCT_PTR  SEM_PTR;

    
} SEM_CONNECTION_STRUCT, _PTR_ SEM_CONNECTION_STRUCT_PTR;
/*! \endcond */

/* ANSI C prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern void      _sem_insert_priority_internal(QUEUE_STRUCT_PTR, 
   SEM_CONNECTION_STRUCT_PTR);
extern void      _sem_cleanup(TD_STRUCT_PTR);
extern _mqx_uint _sem_wait_internal(pointer, MQX_TICK_STRUCT_PTR, boolean);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
