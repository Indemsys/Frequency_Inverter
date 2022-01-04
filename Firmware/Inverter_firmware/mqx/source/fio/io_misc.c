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
* $FileName: io_misc.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the functions that normally would not appear
*   since the fio.h file generates macros that replace them.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Returns the next character as an unsigned char (converted to an int). 
 * 
 * \return Character in char (int).
 * \return IO_EOF (End of file of error.)  
 */ 
_mqx_int _io_getchar
   (
      void
   )
{ /* Body */

   return(_io_fgetc(stdin));

} /* Endbody */

/*!
 * \brief Returns the number of characters read into the input line.
 * 
 * Terminating '\n' is stripped.
 * 
 * \param[in,out] str_ptr   Where to store the input characters.
 * \param[in]     max_count Maximum number of characters to store.
 * 
 * \return Number of characters. 
 */ 
_mqx_int _io_getline
   (
      char _PTR_ str_ptr,
      _mqx_int   max_count
   )
{ /* Body */

   return(_io_fgetline(stdin, str_ptr, max_count));

} /* Endbody */

/*!
 * \brief Reads the next input line into the character array.
 * 
 * The terminating newline is replaced with '\0'.
 * 
 * \param[in,out] tty_line_ptr Where to store the input string.
 * 
 * \return Input string.
 * \return NULL (End of file or error.)   
 */ 
char _PTR_ _io_gets
   (
      char _PTR_ tty_line_ptr
   )
{ /* Body */

   return(_io_fgets(tty_line_ptr, 0, stdin));

} /* Endbody */

/*!
 * \brief Writes the character (converted to an unsigned char).
 * 
 * \param[in] c Character to print out.
 * 
 * \return Written character.
 * |return IO_EOF (Failure.)  
 */ 
_mqx_int _io_putchar
   (
      _mqx_int c
   )
{ /* Body */

   return(_io_fputc(c, stdout));

} /* Endbody */

/*!
 * \brief Writes the sting.
 * 
 * \param[in] string_ptr The string to print out.
 * 
 * \return 0 (Succes.)
 * \return MQX_EOF (Failure.)  
 */ 
_mqx_int _io_puts
   (
      char _PTR_ string_ptr
   )
{ /* Body */

   return(_io_fputs(string_ptr, stdout));

} /* Endbody */

/*!
 * \brief Determines whether a character is available.
 * 
 * \return TRUE (Character is available.), FALSE (Character is not available.) 
 */ 
boolean _io_status
   (
      void
   )
{ /* Body */

   return(_io_fstatus(stdin));

} /* Endbody */

/*!
 * \brief Pushes back a character.
 *  
 * Pushed character will be returned on next read. Only 1 pushback character is
 * allowed.
 * 
 * \param[in] character The character to return to the current input stream.
 * 
 * \return Pushed back character.
 * \return IO_EOF (Failure.)  
 */
_mqx_int _io_ungetc
   (
      _mqx_int character
   )
{ /* Body */

   return(_io_fungetc(character, stdin));

} /* Endbody */

/* EOF */
