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
* $FileName: io_pcb2.c$
* $Version : 3.8.7.0$
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
* Function Name    : _io_pcb_destroy_pool
* Returned Value   : MQX_OK or an error code
* Comments         :
*   Destroys a pool of pcbs.
*END*-------------------------------------------------------------------------*/

_mqx_uint _io_pcb_destroy_pool
   (
      /* The PCB pool to destroy */
      _io_pcb_pool_id pool
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   IO_PCB_POOL_STRUCT_PTR pool_ptr = IO_PCB_GET_POOL_PTR(pool);
   _mqx_uint              result;
   
#if MQX_CHECK_VALIDITY
   if (pool_ptr->VALID != IO_PCB_VALID) {
      return(IO_PCB_POOL_INVALID);
   }/* Endif */
#endif

   pool_ptr->VALID = 0;
  
   result = _partition_destroy(pool_ptr->PARTITION);
   if (result != MQX_OK ) {
      return(result);
   }/* Endif */

  _lwsem_destroy(&pool_ptr->PCB_LWSEM);
   _GET_KERNEL_DATA(kernel_data);
  _queue_unlink((QUEUE_STRUCT_PTR)(&kernel_data->IO_PCB_POOLS), 
     &pool_ptr->QUEUE);
  return(_mem_free(pool_ptr));

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pcb_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   test all pcb pools
*END*-------------------------------------------------------------------------*/

_mqx_uint _io_pcb_test
   (
      /* The PCB pool in error */
      pointer _PTR_ pool_in_error,

      /* The PCB in error */
      pointer _PTR_ pcb_in_error
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   IO_PCB_POOL_STRUCT_PTR pool_ptr;
   _mqx_uint              i;
   _mqx_uint              result;
   
   _GET_KERNEL_DATA(kernel_data);

   *pool_in_error = NULL;
   *pcb_in_error = NULL;
   _INT_DISABLE();
   result = _queue_test((QUEUE_STRUCT_PTR)(&kernel_data->IO_PCB_POOLS), 
      pool_in_error);
   if (result != MQX_OK) {
      _int_enable();
      return(result);
   }/* Endif */
   pool_ptr = (pointer)kernel_data->IO_PCB_POOLS.NEXT;
   for (i = 0; i < 
      _queue_get_size((QUEUE_STRUCT_PTR)(&kernel_data->IO_PCB_POOLS)); i++) 
   {
      if (pool_ptr->VALID != IO_PCB_VALID) {
         /* START CR 2062 */
         _int_enable();
         /* END CR 2062 */
         *pool_in_error = (pointer)pool_ptr;
         return(IO_PCB_POOL_INVALID);
      }/* Endif */
      pool_ptr = (pointer)pool_ptr->QUEUE.NEXT;
   } /* Endfor */

   /* START CR 2062 */
   _int_enable();
   /* END CR 2062 */

   return(MQX_OK);

} /* Endbody */
#endif
/* EOF */
