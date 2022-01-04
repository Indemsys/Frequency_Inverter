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
* $FileName: io_vpr.c$
* $Version : 3.8.5.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the functions for vprintf, vfprintf and vsprintf.
*   These functions are equivalent to the corresponding printf functions,
*   except that the variable argument list is replaced by one argument,
*   which has been initialized by the va_start macro.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief This function is equivalent to the corresponding printf function, except 
 * that the variable argument list is replaced by one argument, which has been 
 * initialized by the va_start macro.
 * 
 * \param[in] fmt_ptr The format string.
 * \param[in] arg     The arguments.
 * 
 * \return Number of characters
 * \return IO_EOF (End Of File found.)   
 */ 
_mqx_int _io_vprintf
   (
      const char _PTR_ fmt_ptr, 
      va_list          arg
   )
{ /* Body */
   _mqx_int result;
   
   result = _io_doprint(stdout, _io_fputc, -1, (char _PTR_)fmt_ptr, arg);

   return result;

} /* Endbody */

/*!
 * \brief This function is equivalent to the corresponding printf function, except 
 * that the variable argument list is replaced by one argument, which has been 
 * initialized by the va_start macro.
 * 
 * \param[in] file_ptr The stream to print upon.
 * \param[in] fmt_ptr  The format string to use for printing.
 * \param[in] arg      The argument list to print.
 * 
 * \return Number of characters
 * \return IO_EOF (End Of File found.)   
 */ 
_mqx_int _io_vfprintf
   (
      MQX_FILE_PTR     file_ptr,
      const char _PTR_ fmt_ptr,
      va_list          arg
   )
{ /* Body */
   _mqx_int result;
   
   result = 0;
   if ( file_ptr ) {
      result = _io_doprint(file_ptr, _io_fputc, -1, (char _PTR_)fmt_ptr, arg);
   } /* Endif */
   return result;

} /* Endbody */

/*!
 * \brief This function is quivalent to the corresponding printf function, except 
 * that the variable argument list is replaced by one argument, which has been 
 * initialized by the va_start macro.
 * 
 * \param[in] str_ptr The string to print into.
 * \param[in] fmt_ptr The format string.
 * \param[in] arg     The arguments.
 * 
 * \return Number of characters
 * \return IO_EOF (End Of File found.)   
 */ 
_mqx_int _io_vsprintf
   ( 
      char        _PTR_ str_ptr,
      const char  _PTR_ fmt_ptr,
      va_list           arg
   )
{ /* Body */
   _mqx_int result;
   
   result = _io_doprint((MQX_FILE_PTR)((pointer)&str_ptr), _io_sputc, -1, (char _PTR_)fmt_ptr, arg);
   *str_ptr = '\0';
   return result;

} /* Endbody */

/*!
 * \brief This function is equivalent to the corresponding printf function, except 
 * that the variable argument list is replaced by one argument, which has been 
 * initialized by the va_start macro.
 * 
 * \param[in] str_ptr The string to print into.
 * \param[in] max_count The maximal size of string.
 * \param[in] fmt_ptr   The format specifier.
 * \param[in] arg       The arguments.
 * 
 * \return Number of characters
 * \return IO_EOF (End Of File found.)  
 */ 
_mqx_int _io_vsnprintf
   (
      char          _PTR_ str_ptr,
      _mqx_int            max_count,
      const char    _PTR_ fmt_ptr,
      va_list             arg
   )
{
    _mqx_int result;

    result = _io_doprint((MQX_FILE_PTR)((pointer)&str_ptr), _io_sputc, max_count, (char _PTR_)fmt_ptr, arg);
    
    if (0 != max_count)
    {
        *str_ptr = '\0';
    }
    
    return result;

}
/* EOF */
