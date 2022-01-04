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
* $FileName: io_fflsh.c$
* $Version : 3.8.6.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the function io_fflush.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief This function causes any buffered but unwritten data to be written.
 * 
 * \param[in] file_ptr The stream whose status is desired.
 * 
 * \return MQX_OK
 * \return IO_EOF (Failure.) 
 */ 
_mqx_int _io_fflush
   (
      MQX_FILE_PTR file_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   _mqx_int               res = MQX_OK;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_EOF);
   } /* Endif */
#endif

   dev_ptr = file_ptr->DEV_PTR;
   if (dev_ptr->IO_IOCTL != NULL) {   
      res = ((*dev_ptr->IO_IOCTL)(file_ptr,
         IO_IOCTL_FLUSH_OUTPUT, NULL));
   } /* Endif */

   /* Handle ungetc buffer in the same way as fseek */ 
   if (file_ptr->HAVE_UNGOT_CHARACTER) {
       _io_fseek(file_ptr, 0, IO_SEEK_CUR);
   }
   return res;

} /* Endbody */

/* EOF */
