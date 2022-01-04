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
* $FileName: io_fopen.c$
* $Version : 3.6.7.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   Contains the function fopen.
*
*END************************************************************************/

#include <string.h>
#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io_prv.h"

#if MQX_USE_IO

/*!
 * \brief The returned value is a pointer to an I/O model.
 * 
 * \param[in] open_type_ptr The name of the device to open.
 * \param[in] open_mode_ptr I/O initialization parameter to pass to the device 
 * initialization.
 * 
 * \return Pointer to MQX_FILE structure
 * \return NULL (Failure.)  
 */ 
MQX_FILE_PTR _io_fopen
   ( 
      const char _PTR_ open_type_ptr,
      const char _PTR_ open_mode_ptr
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   MQX_FILE_PTR           file_ptr;
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   char _PTR_             dev_name_ptr;
   char _PTR_             tmp_ptr;
   _mqx_int                result;

   _GET_KERNEL_DATA(kernel_data);

   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   dev_ptr = (IO_DEVICE_STRUCT_PTR)((pointer)kernel_data->IO_DEVICES.NEXT);
   while (dev_ptr != (pointer)&kernel_data->IO_DEVICES.NEXT) {
      dev_name_ptr = dev_ptr->IDENTIFIER;
      tmp_ptr      = (char _PTR_)open_type_ptr;
      while (*tmp_ptr && *dev_name_ptr &&
         (*tmp_ptr == *dev_name_ptr))
      {
         ++tmp_ptr;
         ++dev_name_ptr;
      } /* Endwhile */
      if (*dev_name_ptr == '\0') {
         /* Match */
         break;
      } /* Endif */
      dev_ptr = (IO_DEVICE_STRUCT_PTR)((pointer)dev_ptr->QUEUE_ELEMENT.NEXT);
   } /* Endwhile */
   
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   
   if (dev_ptr == (pointer)&kernel_data->IO_DEVICES.NEXT) {
      return(NULL);
   } /* Endif */

   file_ptr = (MQX_FILE_PTR)_mem_alloc_system_zero((_mem_size)sizeof(MQX_FILE));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (file_ptr == NULL) {
      return(NULL);
   } /* Endif */
#endif
   _mem_set_type(file_ptr, MEM_TYPE_FILE_PTR);
   
   file_ptr->DEV_PTR = dev_ptr;
   if (dev_ptr->IO_OPEN != NULL) {
      result = (*dev_ptr->IO_OPEN)(file_ptr, (char _PTR_)open_type_ptr, (char _PTR_)open_mode_ptr);
      if (result != MQX_OK) {
         _task_set_error(result);
         _mem_free(file_ptr);
         return(NULL);
      } /* Endif */
   } /* Endif */

   return(file_ptr);

} /* Endbody */
#endif
/* EOF */
