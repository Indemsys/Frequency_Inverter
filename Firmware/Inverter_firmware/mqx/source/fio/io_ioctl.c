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
* $FileName: io_ioctl.c$
* $Version : 3.6.5.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the function _io_ioctl.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Performs specified io_ctl command.
 * 
 * \param[in] file_ptr  The stream to perform the operation on.
 * \param[in] cmd       The ioctl command.
 * \param[in] param_ptr The ioctl parameters.
 * 
 * \return MQX_OK (Succes.)
 * \return IO_EOF
 * \return MQX error code.  
 */ 
_mqx_int _io_ioctl
   ( 
      MQX_FILE_PTR file_ptr,
      _mqx_uint   cmd,
      pointer     param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   _mqx_uint_ptr          tmp_ptr;
   _mqx_uint              result = MQX_OK;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_EOF);
   } /* Endif */
#endif

   tmp_ptr = (_mqx_uint_ptr)param_ptr;

   switch (cmd) {
      case IO_IOCTL_GET_FLAGS:
         *tmp_ptr = file_ptr->FLAGS;
         break;
      case IO_IOCTL_SET_FLAGS:
         file_ptr->FLAGS = *tmp_ptr;
         break;
      default:
         dev_ptr = file_ptr->DEV_PTR;
         if (dev_ptr->IO_IOCTL != NULL) {
            result = (*dev_ptr->IO_IOCTL)(file_ptr, cmd, param_ptr);
         }
         else
            result = IO_ERROR_INVALID_IOCTL_CMD;
         
         break;
   } /* Endswitch */

   return(result);

} /* Endbody */

/* EOF */
