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
* $FileName: io_write.c$
* $Version : 3.8.7.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   Contains the function _io_write.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Writes data to selected stream.
 * 
 * \param[in] file_ptr The stream to perform the operation on.
 * \param[in] data_ptr The data location to write from.
 * \param[in] num      The number of bytes to write.
 * 
 * \return The number of characters written.
 * \return IO_ERROR  
 */ 
_mqx_int _io_write
   ( 
      MQX_FILE_PTR file_ptr,
      pointer      data_ptr,
      _mqx_int     num      
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   dev_ptr;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_ERROR);
   } /* Endif */
#endif

   dev_ptr = file_ptr->DEV_PTR;
#if MQX_CHECK_ERRORS
   if (dev_ptr->IO_WRITE == NULL) {
      file_ptr->ERROR = MQX_IO_OPERATION_NOT_AVAILABLE;
      return(IO_ERROR);
   } /* Endif */
#endif

   return((*dev_ptr->IO_WRITE)(file_ptr, (char *)data_ptr, num));

} /* Endbody */

/* EOF */
