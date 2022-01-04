#ifndef __rtcs25x_h__
#define __rtcs25x_h__
/**HEADER********************************************************************
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
* $FileName: rtcs25x.h$
* $Version : 3.8.16.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains the definitions required to
*   use RTCS with MQX2.5X.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#define MQX_DISABLE_CONFIG_CHECK 1
#include <message.h>
#include <partition.h>
#include <log.h>
#include <klog.h>
#include <pcb.h>
#undef MQX_DISABLE_CONFIG_CHECK

#include <rtcscfg.h>

#define RTCS_get_data()  _mqx_get_io_component_handle(IO_RTCS_COMPONENT)
#define RTCS_set_data(p) _mqx_set_io_component_handle(IO_RTCS_COMPONENT,p)

#define SNMP_get_data()  _mqx_get_io_component_handle(IO_SNMP_COMPONENT)
#define SNMP_set_data(p) _mqx_set_io_component_handle(IO_SNMP_COMPONENT,p)



/***************************************
**
** Memory management
*/

extern pointer RTCS_mem_alloc(_mem_size size);
extern pointer RTCS_mem_alloc_zero(_mem_size size);
extern pointer RTCS_mem_alloc_system(_mem_size size);
extern pointer RTCS_mem_alloc_system_zero(_mem_size size);



/***************************************
**
** Message passing
*/

extern boolean _msgq_send_blocked_internal(pointer);
#define RTCS_msgq_send_blocked(a,b)    _msgq_send_blocked_internal(a)
#define RTCS_msgq_send(a,b)            _msgq_send(a)
#define RTCS_msgq_get_id               _msgq_get_id
#define RTCS_msg_free                  _msg_free
#define RTCS_msg_alloc                 _msg_alloc
#define RTCS_msgq_receive(a,b,c)       _msgq_receive(a,b)
#define RTCS_msgpool_create            _msgpool_create
#define RTCS_msgpool_destroy           _msgpool_destroy
#define RTCS_msgq_open                 _msgq_open
#define RTCS_msgq_close                _msgq_close


/***************************************
**
** PPP Memory management
*/

#define PPP_memzero(ptr, bsize)        _mem_zero(ptr, bsize)
#define PPP_memcopy(src, dest, bsize)  _mem_copy(src, dest, bsize)
#define PPP_memalloc(bsize)            _mem_alloc_zero(bsize)
#define PPP_memfree(ptr)               _mem_free(ptr)


/***************************************
**
** PPP Mutual exclusion
*/

#define _ppp_mutex            LWSEM_STRUCT

#define PPP_mutex_init(p)     _lwsem_create_hidden(p, 1)
#define PPP_mutex_destroy(p)  _lwsem_destroy(p)
#define PPP_mutex_lock(p)     _lwsem_wait(p)
#define PPP_mutex_unlock(p)   _lwsem_post(p)

/***************************************
**
** Partitions
*/
typedef struct rtcs_partition{
#if RTCSCFG_USE_MQX_PARTITIONS
   _partition_id        PART;
   uint_32              SIZE;
#endif
#if RTCSCFG_USE_INTERNAL_PARTITIONS

   uint_32              BLOCK_SIZE;
   uint_32              NUM_BLOCKS;
   uint_32              GROW_BLOCKS;
   uint_32              MAX_BLOCKS;
   pointer              GROW;
#endif
   pointer              FREE;
   int_32 (_CODE_PTR_   CONSTR)(pointer);
   int_32 (_CODE_PTR_   DESTR)(pointer);
} RTCS_PARTITION, _PTR_ _rtcs_part;


extern _rtcs_part RTCS_part_create(
                     uint_32 size,
                     uint_32 init, uint_32 grow, uint_32 max,
                     int_32 (_CODE_PTR_ cons)(pointer),
                     int_32 (_CODE_PTR_ dest)(pointer)
                  );

extern void    RTCS_part_destroy    (_rtcs_part);
extern pointer RTCS_part_alloc      (_rtcs_part);
extern pointer RTCS_part_alloc_zero (_rtcs_part);
extern void    RTCS_part_free       (pointer);


/***************************************
**
** Time
*/

#define RTCS_time_delay       _time_delay
#define RTCS_get_milliseconds RTCS_time_get

extern uint_32 RTCS_time_get     (void);
extern uint_32 RTCS_time_get_sec (void);

/* This function calculates an interval between two moments in time
 * This function takes into account also a possible counter overrun @c (start>end).*/
extern uint_32 RTCS_timer_get_interval( uint_32, uint_32);


/***************************************
**
** Date
*/

extern void RTCS_date_get (uint_32_ptr, uint_32_ptr);
extern void RTCS_date_set (uint_32, uint_32);


/***************************************
**
** Synchronization
*/

#define _rtcs_sem             LWSEM_STRUCT

#define RTCS_sem_init(p)      _lwsem_create_hidden(p, 0)
#define RTCS_sem_destroy(p)   _lwsem_destroy(p)
#define RTCS_sem_post(p)      _lwsem_post(p)
#define RTCS_sem_wait(p)      _lwsem_wait(p)
#define RTCS_sem_trywait(p)   (_lwsem_poll(p) ? RTCS_OK : RTCS_ERROR)


/***************************************
**
** Mutual exclusion
*/

#define _rtcs_mutex           LWSEM_STRUCT

#define RTCS_mutex_init(p)    _lwsem_create_hidden(p, 1)
#define RTCS_mutex_destroy(p) _lwsem_destroy(p)
#define RTCS_mutex_lock(p)    _lwsem_wait(p)
#define RTCS_mutex_unlock(p)  _lwsem_post(p)


/***************************************
**
** Task management
*/

typedef pointer   _rtcs_taskid;

#define RTCS_task_getid()  _task_get_td(0)

extern uint_32 RTCS_task_create
(
   char_ptr          name,
   uint_32           priority,
   uint_32           stacksize,
   void (_CODE_PTR_  start)(pointer, pointer),
   pointer           arg
);

extern void RTCS_task_resume_creator (pointer, uint_32);
extern void RTCS_task_exit           (pointer, uint_32);

extern void RTCS_task_destroy (_rtcs_taskid);
extern void RTCS_task_abort (_rtcs_taskid);
extern _task_id RTCS_task_id_from_td (_rtcs_taskid);


/***************************************
**
** I/O
*/

#define RTCS_EACCES MQX_EACCES
#define RTCS_ENOENT MQX_ENOENT
#define RTCS_EEXIST MQX_EEXIST

extern pointer RTCS_io_open(char_ptr, char_ptr, uint_32 _PTR_);

#define RTCS_io_read(f,p,l)   ((int_32)read(f,(char_ptr)(p),l))
#define RTCS_io_write(f,p,l)  ((int_32)write(f,(char_ptr)(p),l))
#define RTCS_io_close(f)      fclose(f)

extern int_32 _io_socket_install(char_ptr);
extern int_32 _io_telnet_install(char_ptr);


/***************************************
**
** Logging
*/

#define RTCSLOG_TYPE_FNENTRY  KLOG_FUNCTION_ENTRY
#define RTCSLOG_TYPE_FNEXIT   KLOG_FUNCTION_EXIT
#define RTCSLOG_TYPE_PCB      KLOG_RTCS_PCB
#define RTCSLOG_TYPE_TIMER    KLOG_RTCS_TIMER
#define RTCSLOG_TYPE_ERROR    KLOG_RTCS_ERROR


#if MQX_KERNEL_LOGGING
   extern void _klog_log(uint_32, uint_32, uint_32, uint_32, uint_32, uint_32);
   #define RTCS_log_internal(type,p1,p2,p3,p4,p5)  _klog_log(type,p1,p2,p3,p4,p5)
#else
   #define RTCS_log_internal(type,p1,p2,p3,p4,p5) 
#endif


#endif
/* EOF */
