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
* $FileName: io_fputc.c$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the functions for printing a character.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Writes a character (converted to an unsigned char).
 * 
 * \param[in] c        The character to print out.
 * \param[in] file_ptr The stream to print the character upon.
 * 
 * \return Written character.
 * \return IO_EOF (Failure.) 
 */ 
_mqx_int _io_fputc
   (
      _mqx_int     c,
      MQX_FILE_PTR file_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   char                   tmp;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_EOF);
   } /* Endif */
#endif

   dev_ptr = file_ptr->DEV_PTR;
#if MQX_CHECK_ERRORS
   if (dev_ptr->IO_WRITE == NULL) {
      file_ptr->ERROR = MQX_IO_OPERATION_NOT_AVAILABLE;
      return(IO_EOF);
   } /* Endif */
#endif
   tmp = (char)c;

   if ((*dev_ptr->IO_WRITE)(file_ptr, &tmp, 1) != 1) {
      return(IO_EOF);
   } /* Endif */

   return(c);

} /* Endbody */

/* EOF */
