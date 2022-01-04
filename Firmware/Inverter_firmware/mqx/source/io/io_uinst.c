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
* $FileName: io_uinst.c$
* $Version : 3.5.4.0$
* $Date    : Dec-8-2009$
*
* Comments:
*
*   This file contains the function for un-installing a dynamic device
*   driver.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#ifdef NULL
#undef NULL
#endif
#include <string.h>
#ifndef NULL
#define NULL ((pointer)0)
#endif

#if MQX_USE_IO


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dev_uninstall
* Returned Value   : _mqx_int a task error code or MQX_OK
* Comments         :
*    Un-Install a device dynamically.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_dev_uninstall
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr           identifier
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   _mqx_int               result = IO_OK;

   _GET_KERNEL_DATA(kernel_data);

   /* Find the device */
   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   dev_ptr = (IO_DEVICE_STRUCT_PTR)((pointer)kernel_data->IO_DEVICES.NEXT);
   while (dev_ptr != (pointer)&kernel_data->IO_DEVICES.NEXT) {
      if (!strncmp(identifier, dev_ptr->IDENTIFIER, IO_MAXIMUM_NAME_LENGTH)) {
         /* Found it */
         if (dev_ptr->IO_UNINSTALL != NULL) {
            result = (*dev_ptr->IO_UNINSTALL)(dev_ptr);
            if (result == IO_OK) {
               _QUEUE_REMOVE(&kernel_data->IO_DEVICES, dev_ptr);
               _mem_free(dev_ptr);
            } /* Endif */
         } /* Endif */
         _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
         return(result);
      } /* Endif */
      dev_ptr = (IO_DEVICE_STRUCT_PTR)((pointer)dev_ptr->QUEUE_ELEMENT.NEXT);
   } /* Endwhile */
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);

   return(IO_DEVICE_DOES_NOT_EXIST);

} /* Endbody */

#endif

/* EOF */
