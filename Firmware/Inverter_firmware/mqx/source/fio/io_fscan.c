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
* $FileName: io_fscan.c$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the function fscanf.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief This function performs similarly to the scanf function of 'C'.
 * 
 * \param[in] file_ptr The stream to scan from.
 * \param[in] fmt_ptr  The format string to use when scanning.
 * 
 * \return Number of input items converted and assigned.
 * \return IO_EOF  
 */ 
_mqx_int _io_fscanf
   (
      MQX_FILE_PTR     file_ptr,
      const char _PTR_ fmt_ptr, 
      ...
   )
{ /* Body */
   char    temp_buf[IO_MAXLINE];
   va_list ap;
   _mqx_int result;
   
#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return(IO_EOF);
   } /* Endif */
#endif

   va_start(ap, fmt_ptr);
   /* get a line of input from user */
   if (_io_fgetline( file_ptr, temp_buf, IO_MAXLINE) == IO_EOF) {
      return(IO_EOF);
   } /* Endif */
   result = _io_scanline( temp_buf, (char _PTR_)fmt_ptr, ap );
   va_end(ap);
   return result;

} /* Endbody */

/* EOF */
