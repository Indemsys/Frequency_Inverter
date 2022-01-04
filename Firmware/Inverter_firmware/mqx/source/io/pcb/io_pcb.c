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
* $FileName: io_pcb.c$
* $Version : 3.8.6.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains the functions for use with IO PCB device drivers.
*
*END************************************************************************/

#include "mqx_inc.h"
#if MQX_USE_IPC

#include "partition.h"
#include "fio.h"
#include "io.h"
#include "io_prv.h"
#include "io_pcb.h"
#include "iopcbprv.h"


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_create_pool
* Returned Value   : MQX_OK or a MQX error code
* Comments         :
*   Create a pool of pcbs.
*END*-------------------------------------------------------------------------*/

_io_pcb_pool_id _io_pcb_create_pool
   (
      /* The maximum number of fragments in the PCB */
      _mqx_uint                     max_number_of_fragments,

      /* 
      ** The number of bytes of extra data to append to the PCB 
      ** (following the fragments) 
      */
      _mem_size                     size_extra_data,

      /* The initial number of PCBS to acquire */
      _mqx_uint                     initial_number_of_pcbs, 

      /* The number of PCBS to add when the pool is exhausted */
      _mqx_uint                     grow_number_of_pcbs, 

      /* The maximum number of PCBS allowed in the pool (0 implies unlimited) */
      _mqx_uint                     max_number_of_pcbs,
      
      /*
      ** The function to call when a pcb is allocated from this pool.  If this
      ** function returns NULL, then the pcb should not be allocated
      */
      IO_PCB_STRUCT_PTR (_CODE_PTR_ pcb_alloc_function)
         (IO_PCB_STRUCT_PTR, pointer),
      
      /* Data to pass to the allocation function */
      pointer  alloc_function_data_ptr,

      /* The function to call when a pcb is freed to this pool */
      IO_PCB_STRUCT_PTR (_CODE_PTR_ pcb_free_function)
         (IO_PCB_STRUCT_PTR, pointer),

      /* Data to pass to the free function */
      pointer                       free_function_data_ptr
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   IO_PCB_POOL_STRUCT_PTR pool_ptr;
   _partition_id          pid;
   
   if (max_number_of_fragments < 1) {
      _task_set_error(MQX_INVALID_PARAMETER);
      return(IO_PCB_NULL_POOL_ID);
   }/* Endif */
   pool_ptr = _mem_alloc_system_zero((_mem_size)sizeof(IO_PCB_POOL_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (pool_ptr == NULL) {
      return(IO_PCB_NULL_POOL_ID);
   }/* Endif */
#endif

   pid = _partition_create( (_mem_size)(sizeof(IO_PCB_STRUCT) + 
      (sizeof(IO_PCB_FRAGMENT_STRUCT) * (max_number_of_fragments - 1))) + 
      size_extra_data, initial_number_of_pcbs, grow_number_of_pcbs, 
      max_number_of_pcbs);
#if MQX_CHECK_ERRORS
   if (pid == PARTITION_NULL_ID) {
      _mem_free(pool_ptr);
      return(IO_PCB_NULL_POOL_ID);
   }/* Endif */
#endif
   pool_ptr->VALID                   = IO_PCB_VALID;
   pool_ptr->PARTITION               = pid;
   pool_ptr->NUM_FRAGS               = max_number_of_fragments;
   pool_ptr->EXTRA_DATA_SIZE         = size_extra_data;
   pool_ptr->PCB_ALLOC_FUNCTION_PTR  = pcb_alloc_function;
   pool_ptr->ALLOC_FUNCTION_DATA_PTR = alloc_function_data_ptr;
   pool_ptr->PCB_FREE_FUNCTION_PTR   = pcb_free_function;
   pool_ptr->FREE_FUNCTION_DATA_PTR  = free_function_data_ptr;

   _lwsem_create(&pool_ptr->PCB_LWSEM, initial_number_of_pcbs);
   _GET_KERNEL_DATA(kernel_data);

   if (kernel_data->IO_PCB_POOLS.NEXT == NULL) {
      _queue_init((QUEUE_STRUCT_PTR)(&kernel_data->IO_PCB_POOLS), 0);
   } /* Endif */
   _queue_enqueue((QUEUE_STRUCT_PTR)(&kernel_data->IO_PCB_POOLS), &pool_ptr->QUEUE);

   return(IO_PCB_CREATE_POOL_ID(pool_ptr));

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_alloc
* Returned Value   : a PCB pointer or NULL upon error, setting task error code
* Comments         :
*   This function obtains a pcb from the specified pcb pool
*END*-------------------------------------------------------------------------*/

IO_PCB_STRUCT_PTR _io_pcb_alloc
   (
      /* [IN] The PCB pool to obtain a pcb from */
      _io_pcb_pool_id pool,
      
      /* [IN] A boolean flag, if TRUE indicates block waiting for a PCB */
      boolean         blocking
   )
{ /* Body */
   IO_PCB_POOL_STRUCT_PTR pool_ptr = IO_PCB_GET_POOL_PTR(pool);
   IO_PCB_STRUCT_PTR      pcb_ptr;
   IO_PCB_STRUCT_PTR      new_pcb_ptr;
   
#if MQX_CHECK_VALIDITY
   if (pool_ptr->VALID != IO_PCB_VALID) {
      _task_set_error(IO_PCB_POOL_INVALID);
      return(NULL );
   }/* Endif */
#endif
   if (blocking) {
      _lwsem_wait(&pool_ptr->PCB_LWSEM);
   } else {
      _lwsem_poll(&pool_ptr->PCB_LWSEM);
   } /* Endif */

   pcb_ptr = (IO_PCB_STRUCT_PTR)_partition_alloc_system_zero(
      pool_ptr->PARTITION);

   if (pcb_ptr != NULL) {
      pcb_ptr->FREE_PCB_FUNCTION_PTR = _io_pcb_free_internal;
      pcb_ptr->VALID               = IO_PCB_VALID;
      pcb_ptr->POOL_ID             = pool;
      pcb_ptr->NUMBER_OF_FRAGMENTS = pool_ptr->NUM_FRAGS;
      if (pool_ptr->EXTRA_DATA_SIZE) {
         pcb_ptr->FRAGMENTS[0].FRAGMENT = (uchar_ptr)pcb_ptr + 
            sizeof(IO_PCB_STRUCT) + (sizeof(IO_PCB_FRAGMENT_STRUCT) *
            (pool_ptr->NUM_FRAGS - 1));
         /* Start CR 397 */
         pcb_ptr->FRAGMENTS[0].LENGTH = pool_ptr->EXTRA_DATA_SIZE;
         /* End CR 397 */
      }/* Endif */
      if (pool_ptr->PCB_ALLOC_FUNCTION_PTR) {
         new_pcb_ptr = (*pool_ptr->PCB_ALLOC_FUNCTION_PTR)(pcb_ptr, 
            pool_ptr->ALLOC_FUNCTION_DATA_PTR);
         if (new_pcb_ptr == NULL) {
            _task_set_error(IO_PCB_ALLOC_CALLBACK_FAILED);
            _partition_free(pcb_ptr);
            _lwsem_post(&pool_ptr->PCB_LWSEM);
            return(NULL);
         } /* Endif */
         pcb_ptr = new_pcb_ptr;
      /* Start CR 321 */
      /*
      } else {
         _task_set_error(IO_PCB_ALLOC_CALLBACK_FAILED);
      */
      /* End CR 321 */
      } /* Endif */
   }/* Endif */
   return(pcb_ptr);

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_read
* Returned Value   : MQX_OK or an error code
* Comments         :
*   returns a PCB that has been read from the device.
*END*-------------------------------------------------------------------------*/

_mqx_int _io_pcb_read
   (
      /* The device to read from */
      MQX_FILE_PTR            fd_ptr,

      /* The PCB to return from the read */
      IO_PCB_STRUCT_PTR _PTR_ pcb_ptr_ptr
   )
{ /* Body */
   IO_PCB_DEVICE_STRUCT_PTR dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   
#if MQX_CHECK_ERRORS
   if (!(fd_ptr->FLAGS & IO_FLAG_IS_PCB_DEVICE)) {
      fd_ptr->ERROR = IO_PCB_NOT_A_PCB_DEVICE;
      return(IO_ERROR);
   }/* Endif */
#endif
   if (dev_ptr->IO_READ) {
      return((*dev_ptr->IO_READ)(fd_ptr,pcb_ptr_ptr));
   }/* Endif */
   fd_ptr->ERROR = IO_PCB_READ_NOT_AVAILABLE;
   *pcb_ptr_ptr = NULL;
   return(IO_ERROR);
   
} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_write
* Returned Value   : MQX_OK or an error code
* Comments         :
*   uses the PCB to write the data to the device
*END*-------------------------------------------------------------------------*/

_mqx_int _io_pcb_write
   (
      /* The device to read from */
      MQX_FILE_PTR         fd_ptr,

      /* The PCB to use for writing */
      IO_PCB_STRUCT _PTR_  pcb_ptr
   )
{ /* Body */
   IO_PCB_DEVICE_STRUCT_PTR dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   
#if MQX_CHECK_VALIDITY
   if (pcb_ptr->VALID != IO_PCB_VALID) {
      fd_ptr->ERROR = IO_PCB_NOT_A_PCB;
      return(IO_ERROR);
   }/* Endif */
#endif
#if MQX_CHECK_ERRORS
   if (!(fd_ptr->FLAGS & IO_FLAG_IS_PCB_DEVICE)) {
      fd_ptr->ERROR = IO_PCB_NOT_A_PCB_DEVICE;
      return(IO_ERROR);
   }/* Endif */
#endif
   if (dev_ptr->IO_WRITE) {
      return((*dev_ptr->IO_WRITE)(fd_ptr, pcb_ptr));
   }/* Endif */
   IO_PCB_FREE(pcb_ptr);
   fd_ptr->ERROR = IO_PCB_WRITE_NOT_AVAILABLE;
   return(IO_ERROR);

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_free
* Returned Value   : MQX_OK or an error code
* Comments         :
*   Calls the pcb free function of the pcb
*END*-------------------------------------------------------------------------*/

_mqx_uint _io_pcb_free
   (
      /* The pcb to free */
      IO_PCB_STRUCT _PTR_ pcb_ptr
   )
{ /* Body */

   return(*(pcb_ptr)->FREE_PCB_FUNCTION_PTR)(pcb_ptr);

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_free_internal
* Returned Value   : MQX_OK or an error code
* Comments         :
*   returns the PCB to the PCB pool it came from
*END*-------------------------------------------------------------------------*/

_mqx_uint _io_pcb_free_internal
   (
      /* The pcb to free */
      IO_PCB_STRUCT _PTR_ pcb_ptr
   )
{ /* Body */
   IO_PCB_POOL_STRUCT_PTR pool_ptr;
   _mqx_uint              result;

#if MQX_CHECK_VALIDITY
   if (pcb_ptr->VALID != IO_PCB_VALID) {
      return(IO_PCB_INVALID);
   }/* Endif */
#endif

   pool_ptr = IO_PCB_GET_POOL_PTR(pcb_ptr->POOL_ID);
#if MQX_CHECK_VALIDITY
   if (pool_ptr->VALID != IO_PCB_VALID) {
      return(IO_PCB_POOL_INVALID);
   }/* Endif */
#endif

   if (pool_ptr->PCB_FREE_FUNCTION_PTR) {
      pcb_ptr = (*pool_ptr->PCB_FREE_FUNCTION_PTR)(pcb_ptr, 
         pool_ptr->FREE_FUNCTION_DATA_PTR);
   }/* Endif */

   result = _partition_free((pointer)pcb_ptr);

   _lwsem_post(&pool_ptr->PCB_LWSEM);

   return result;

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_start
* Returned Value   : MQX_OK or an error code
* Comments         :
*   Starts a PCB device
*END*-------------------------------------------------------------------------*/

_mqx_int _io_pcb_start
   (
      /* The device to start */
      MQX_FILE_PTR         fd_ptr
   )
{ /* Body */

   return ioctl(fd_ptr, IO_PCB_IOCTL_START, NULL);

} /* Endbody */

#endif

/* EOF */
