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
* $FileName: io_spr.c$
* $Version : 3.8.5.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the function for sprintf and snprintf.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Performs similarly to the sprintf function found in 'C'.
 * 
 * The returned number of characters does not include the terminating '\0'
 * 
 * \param[in] str_ptr The string to print into.
 * \param[in] fmt_ptr The format specifier.
 * 
 * \return Number of characters
 * \return IO_EOF (End Of File found.)   
 */ 
_mqx_int _io_sprintf
   (
      char        _PTR_ str_ptr,
      const char  _PTR_ fmt_ptr,
      ...
   )
{ /* Body */
   _mqx_int result;
   va_list ap;
   
   va_start(ap, fmt_ptr);
   result = _io_doprint((MQX_FILE_PTR)((pointer)&str_ptr), _io_sputc, -1, (char _PTR_)fmt_ptr, ap);
   *str_ptr = '\0';
   va_end(ap);
   return result;

} /* Endbody */

/*!
 * \brief This function performs similarly to the sprintf function found in 'C'.
 * 
 * The returned number of characters does not include the terminating '\0'.
 * 
 * \param[in] str_ptr   The string to print into.
 * \param[in] max_count The maximal size of string.
 * \param[in] fmt_ptr   The format specifier.
 * 
 * \return Number of characters
 * \return IO_EOF (End Of File found.) 
 */ 
_mqx_int _io_snprintf
   (
      char          _PTR_ str_ptr,
      _mqx_int      max_count,
      const char    _PTR_ fmt_ptr,
      ...
   )
{
    _mqx_int result;

    va_list ap;
    va_start(ap, fmt_ptr);
    result = _io_doprint((MQX_FILE_PTR)((pointer)&str_ptr), _io_sputc, max_count, (char _PTR_)fmt_ptr, ap);
    va_end(ap);
    if (0 != max_count)
    {
        *str_ptr = '\0';
    }
   
    return result;

}

/*!
 * \brief Erites the character into the string located by the string pointer and
 * updates the string pointer.
 * 
 * \param[in]      c                The character to put into the string.
 * \param[in, out] input_string_ptr This is an updated pointer to a string pointer.
 * 
 * \return Character written into string. 
 */ 
_mqx_int _io_sputc
   (
      _mqx_int     c,
      MQX_FILE_PTR input_string_ptr
   )
{ /* Body */
   char _PTR_ _PTR_ string_ptr = (char _PTR_ _PTR_)((pointer)input_string_ptr);

   *(*string_ptr)++ = (char)c;
   return c;

} /* Endbody */

/* EOF */
