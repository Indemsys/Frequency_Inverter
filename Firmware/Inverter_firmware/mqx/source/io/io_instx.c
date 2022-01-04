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
* $FileName: io_instx.c$
* $Version : 3.6.7.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the function for installing a dynamic device
*   driver.
*
*END************************************************************************/

#include <string.h>
#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

#if MQX_USE_IO

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dev_install_ext
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a device dynamically, so tasks can fopen to it. Different from
* _io_dev_install since this function also installs an unstall function.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_dev_install_ext
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr          identifier,
  
      /* [IN] The I/O open function */
      IO_OPEN_FPTR      io_open,

      /* [IN] The I/O close function */
      IO_CLOSE_FPTR     io_close,

      /* [IN] The I/O read function */
      IO_READ_FPTR      io_read,

      /* [IN] The I/O write function */
      IO_WRITE_FPTR     io_write,

      /* [IN] The I/O ioctl function */
      IO_IOCTL_FPTR     io_ioctl,

      /* [IN] The I/O un-install function */
      IO_UNINSTALL_FPTR io_uninstall,

      /* [IN] The I/O initialization data */
      pointer           io_init_data_ptr
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   IO_DEVICE_STRUCT_PTR   dev_ptr;
#if MQX_CHECK_ERRORS
   _mqx_uint              i;
   _mqx_uint              found = 0;
#endif

   _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
   if ((io_open == NULL) || (io_close == NULL)){
      return(MQX_INVALID_PARAMETER);
   } /* Endif */

   /* Search for delimiter */
   for (i = 0; i < IO_MAXIMUM_NAME_LENGTH; i++) {
      if (identifier[i] == IO_DEV_DELIMITER) {
         found++;
      } else if (identifier[i] == '\0') {
         break;
      } /* Endif */
   } /* Endfor */
      
   /* 
   ** Return an error if more than 1 delimiter found, no delimiter was found
   ** or the identifier was composed of a single delimiter only.
   */
   if ((found != 1) || (i == 1)) {
      return(MQX_INVALID_PARAMETER);
   } /* Endif */
#endif

   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);

#if MQX_CHECK_ERRORS
   /* Check to see if device already installed */
   dev_ptr = (IO_DEVICE_STRUCT_PTR)((pointer)kernel_data->IO_DEVICES.NEXT);
   while (dev_ptr != (pointer)&kernel_data->IO_DEVICES.NEXT) {
      if (!strncmp(identifier, dev_ptr->IDENTIFIER, IO_MAXIMUM_NAME_LENGTH)) {
         _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
         return(IO_DEVICE_EXISTS);
      } /* Endif */
      dev_ptr = (IO_DEVICE_STRUCT_PTR)((pointer)dev_ptr->QUEUE_ELEMENT.NEXT);
   } /* Endwhile */
#endif
   
   dev_ptr = (IO_DEVICE_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(IO_DEVICE_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
      return(MQX_OUT_OF_MEMORY);
   }/* Endif */
#endif
   _mem_set_type(dev_ptr, MEM_TYPE_IO_DEVICE);
      
   dev_ptr->IDENTIFIER      = identifier;
   dev_ptr->IO_OPEN         = io_open;
   dev_ptr->IO_CLOSE        = io_close;
   dev_ptr->IO_READ         = io_read;
   dev_ptr->IO_WRITE        = io_write;
   dev_ptr->IO_IOCTL        = io_ioctl;
   dev_ptr->IO_UNINSTALL    = io_uninstall;
   dev_ptr->DRIVER_INIT_PTR = io_init_data_ptr;

   _QUEUE_ENQUEUE(&kernel_data->IO_DEVICES, dev_ptr);

   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   return MQX_OK;

} /* Endbody */

#endif

/* EOF */
