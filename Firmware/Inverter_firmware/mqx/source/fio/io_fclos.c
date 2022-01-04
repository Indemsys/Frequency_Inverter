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
* $FileName: io_fclos.c$
* $Version : 3.8.8.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   Contains the function fclose.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

#if MQX_USE_IO

/*!
 * \brief Calls the close function for the device.
 * 
 * \param[in] file_ptr The stream to close.
 * 
 * \return Value returned by close function.
 * \return IO_EOF (Failure.)  
 */ 
_mqx_int _io_fclose
   ( 
      MQX_FILE_PTR file_ptr
   )
{ /* Body */
/*   KERNEL_DATA_STRUCT_PTR kernel_data; */
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   _mqx_uint               result;

/*   _GET_KERNEL_DATA(kernel_data); */

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_EOF);
   } /* Endif */
#endif

   dev_ptr = file_ptr->DEV_PTR;
#if MQX_CHECK_ERRORS
   if (dev_ptr->IO_CLOSE == NULL) {
      _mem_free(file_ptr);
      return(IO_EOF);
   } /* Endif */
#endif
   
   result = (*dev_ptr->IO_CLOSE)(file_ptr);
 
   _mem_free(file_ptr);

   return(result);

} /* Endbody */
#endif
/* EOF */
