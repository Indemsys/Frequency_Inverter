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
* $FileName: iop_inst.c$
* $Version : 3.8.4.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains the function for installing a dynamic device
*   driver.
*
*END************************************************************************/

/* suppress warning if partition component is not enabled in MQX */
#define MQX_DISABLE_CONFIG_CHECK 1

#include <string.h>
#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "partition.h"
#include "io_pcb.h"
#include "iopcbprv.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_pcb_dev_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a device dynamically, so tasks can fopen to it.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_pcb_dev_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr             identifier,
  
      /* [IN] The I/O open function */
      _mqx_int (_CODE_PTR_ io_open)(FILE_DEVICE_STRUCT_PTR,
         char _PTR_, char _PTR_),

      /* [IN] The I/O close function */
      _mqx_int (_CODE_PTR_ io_close)(FILE_DEVICE_STRUCT_PTR),

      /* [IN] The I/O read function */
      _mqx_int (_CODE_PTR_ io_read)(FILE_DEVICE_STRUCT_PTR,
         IO_PCB_STRUCT _PTR_ _PTR_),

      /* [IN] The I/O write function */
      _mqx_int (_CODE_PTR_ io_write)(FILE_DEVICE_STRUCT_PTR,
         IO_PCB_STRUCT _PTR_),

      /* [IN] The I/O ioctl function */
      _mqx_int (_CODE_PTR_ io_ioctl)(FILE_DEVICE_STRUCT_PTR, _mqx_uint,
         pointer),

      /* [IN] The I/O uninstall function */
      _mqx_int (_CODE_PTR_ io_uninstall)(IO_PCB_DEVICE_STRUCT_PTR),

      /* [IN] The I/O initialization data */
      pointer              io_init_data_ptr
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR   kernel_data;
   IO_PCB_DEVICE_STRUCT_PTR dev_ptr;

   kernel_data = _mqx_get_kernel_data();

#if MQX_CHECK_ERRORS
   if ((io_open == NULL) || (io_close == NULL)){
      return(MQX_INVALID_PARAMETER);
   }/* Endif */

   /* Check to see if device already installed */
   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   /* Start CR 460 */
   if (kernel_data->IO_DEVICES.NEXT == NULL) {
      /* Set up the device driver queue */
      _QUEUE_INIT(&kernel_data->IO_DEVICES, 0);
   } /* Endif */
   /* End CR 460 */
   dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((pointer)kernel_data->IO_DEVICES.NEXT);
   while (dev_ptr != (IO_PCB_DEVICE_STRUCT_PTR)
      ((pointer)&kernel_data->IO_DEVICES.NEXT))
   {
      if (!strncmp(identifier, dev_ptr->IDENTIFIER, IO_MAXIMUM_NAME_LENGTH)) {
         _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
         return(IO_DEVICE_EXISTS);
      } /* Endif */
      dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((pointer)dev_ptr->QUEUE_ELEMENT.NEXT);
   } /* Endwhile */
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
#endif
   
   dev_ptr     = (pointer)_mem_alloc_system_zero(
      (_mem_size)sizeof(IO_PCB_DEVICE_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   }/* Endif */
#endif
   
   dev_ptr->IDENTIFIER      = identifier;
   dev_ptr->IO_OPEN         = io_open;
   dev_ptr->IO_CLOSE        = io_close;
   dev_ptr->IO_READ         = io_read;
   dev_ptr->IO_WRITE        = io_write;
   dev_ptr->IO_IOCTL        = io_ioctl;
   dev_ptr->IO_UNINSTALL    = io_uninstall;
   dev_ptr->DRIVER_INIT_PTR = io_init_data_ptr;

   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   _QUEUE_ENQUEUE(&kernel_data->IO_DEVICES, dev_ptr);
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_pcb_dev_uninstall
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    UnInstall a device dynamically.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_pcb_dev_uninstall
   (
      /* [IN] A string that identifies the device */
      char_ptr           identifier
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR   kernel_data;
   IO_PCB_DEVICE_STRUCT_PTR dev_ptr;

   kernel_data = _mqx_get_kernel_data();

   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   /* Check to see if device already installed */
   dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((pointer)kernel_data->IO_DEVICES.NEXT);
   while (dev_ptr != (IO_PCB_DEVICE_STRUCT_PTR)
      ((pointer)&kernel_data->IO_DEVICES.NEXT))
   {
      if (!strncmp(identifier, dev_ptr->IDENTIFIER, IO_MAXIMUM_NAME_LENGTH)) {
         break;
      } /* Endif */
      dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((pointer)dev_ptr->QUEUE_ELEMENT.NEXT);
   } /* Endwhile */
   if (dev_ptr == (IO_PCB_DEVICE_STRUCT_PTR)
      ((pointer)&kernel_data->IO_DEVICES.NEXT))
   {
      dev_ptr = NULL;
   } /* Endif */
   if (dev_ptr)  {
      if (dev_ptr->IO_UNINSTALL)  {
         (*dev_ptr->IO_UNINSTALL)(dev_ptr);
      } /* Endif */
      _QUEUE_REMOVE(&kernel_data->IO_DEVICES, dev_ptr);
   } /* Endif */
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   if (dev_ptr) {
      _mem_free(dev_ptr);
      return MQX_OK;
   }/* Endif */
   return(IO_PCB_DEVICE_DOES_NOT_EXIST);
   
} /* Endbody */

/* EOF */
