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
* $FileName: io_read.c$
* $Version : 3.8.9.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Contains the function read.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Reads form the specified stream.
 * 
 * \param[in] file_ptr The stream to perform the operation on.
 * \param[in] data_ptr The data location to read to.
 * \param[in] num      The number of bytes to read.
 * 
 * \return Number of characters read.
 * \return IO_ERROR   
 */ 
_mqx_int _io_read
   ( 
      MQX_FILE_PTR file_ptr,
      pointer      data_ptr,
      _mqx_int     num      
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   char_ptr               data = (char_ptr) data_ptr;
   _mqx_int               res = 0;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_ERROR);
   } /* Endif */
#endif

   if (!file_ptr->HAVE_UNGOT_CHARACTER || num > 1) {

        dev_ptr = file_ptr->DEV_PTR;
#if MQX_CHECK_ERRORS
        if (dev_ptr->IO_READ == NULL) {
            file_ptr->ERROR = MQX_IO_OPERATION_NOT_AVAILABLE;
            return (IO_ERROR);
        } /* Endif */
#endif

        if (file_ptr->HAVE_UNGOT_CHARACTER) {
            res = (*dev_ptr->IO_READ)(file_ptr, data+1, num-1);
        }
        else {
            res = (*dev_ptr->IO_READ)(file_ptr, data, num);
        }
        if (res<0) return res;
   }
      
   if ( file_ptr->HAVE_UNGOT_CHARACTER ) {
       if (num <= 0) return 0;
       *data = (char)file_ptr->UNGOT_CHARACTER;
       file_ptr->HAVE_UNGOT_CHARACTER = FALSE;
       res++;
   } /* Endif */

   return res;
} /* Endbody */

/* EOF */
