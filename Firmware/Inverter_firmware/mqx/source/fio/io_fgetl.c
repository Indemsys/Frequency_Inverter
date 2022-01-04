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
* $FileName: io_fgetl.c$
* $Version : 3.6.5.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the function for reading an input line.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Returns the number of characters read into the input line.
 * 
 * The terminating line feed is stripped.
 * 
 * \param[in]     file_ptr   The stream to read the characters from.
 * \param[in,out] str_ptr    Where to store the input characters.
 * \param[in]     max_length The maximum number of characters to store.
 * 
 * \return Number of characters read.
 * \return IO_EOF    
 */ 
_mqx_int _io_fgetline
   (
      MQX_FILE_PTR file_ptr,
      char _PTR_   str_ptr,
      _mqx_int     max_length
   )
{ /* Body */
   _mqx_int  c;
   _mqx_int  i;
   _mqx_uint flags;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      *str_ptr = '\0';
      return(IO_EOF);
   } /* Endif */
#endif

   if (max_length) {
      max_length--;  /* Need to leave 1 space for the null termination */
   } else {
      max_length = MAX_MQX_INT;  /* Effectively infinite length */
   } /* Endif */

   c = _io_fgetc(file_ptr);
   if (c == IO_EOF) {
      *str_ptr = '\0';
      return(IO_EOF);
   } /* Endif */
   flags = file_ptr->FLAGS;
   i = 0;
   while ( (! ((c == '\n') || (c == '\r'))) && (i < max_length) ) {
      if ((flags & IO_FLAG_TEXT) && (c == '\b')) {
         if ( i ) {
            *--str_ptr = ' ';
            --i;
         } /* Endif */
      } else {
         *str_ptr++ = (char)c;
         ++i;
      } /* Endif */
      c = _io_fgetc(file_ptr);
      if (c == IO_EOF) {
         *str_ptr = '\0'; /* null terminate the string before returning */
         return(IO_EOF);
      } /* Endif */
   } /* Endwhile */

   if (i >= max_length) {
      _io_fungetc((_mqx_int)c, file_ptr);
   } /* Endif */

   *str_ptr = '\0';

   return (i);

} /* Endbody */

/* EOF */
