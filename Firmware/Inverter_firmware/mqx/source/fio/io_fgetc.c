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
* $FileName: io_fgetc.c$
* $Version : 3.8.6.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the functions for reading a character.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief This function returns the next character as an unsigned char (converted 
 * to an int) or IO_EOF if end of file or error occurs.
 *  
 * \param[in] file_ptr The stream to read the character from.
 * 
 * \return Converted character.
 * \return IO_EOF  
 */ 
_mqx_int _io_fgetc
   (
      MQX_FILE_PTR file_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   char                   tmp;
   _mqx_int               ungot;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_EOF);
   } /* Endif */
#endif

   if ( file_ptr->HAVE_UNGOT_CHARACTER ) {
      ungot = file_ptr->UNGOT_CHARACTER;
      file_ptr->HAVE_UNGOT_CHARACTER = FALSE;
      return ungot;
   } /* Endif */

   dev_ptr = file_ptr->DEV_PTR;
#if MQX_CHECK_ERRORS
   if (dev_ptr->IO_READ == NULL) {
      file_ptr->ERROR = MQX_IO_OPERATION_NOT_AVAILABLE;
      return(IO_EOF);
   } /* Endif */
#endif

   if ((*dev_ptr->IO_READ)(file_ptr, &tmp, 1) == 1) {
      return (_mqx_int)(uchar)tmp;
   } else {
      return(IO_EOF);
   } /* Endif */

} /* Endbody */

/* EOF */
