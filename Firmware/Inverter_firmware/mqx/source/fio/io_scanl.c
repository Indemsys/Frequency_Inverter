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
* $FileName: io_scanl.c$
* $Version : 3.8.9.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the function for scanning and converting a line
*   of text.
*
*END************************************************************************/

#include "mqx.h"
#include <string.h>
#include <stdlib.h>

#if MQX_INCLUDE_FLOATING_POINT_IO
#include <math.h>
#endif

#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "fpio_prv.h"


extern char _io_scanline_ignore_white_space( char _PTR_ _PTR_ , _mqx_uint _PTR_ , 
   _mqx_uint);
extern char _io_scanline_format_ignore_white_space( char _PTR_ _PTR_ , 
   _mqx_uint _PTR_);
extern int_32 _io_scanline_base_convert( uchar, _mqx_uint );
extern boolean _io_scanline_is_octal_digit( char );
extern boolean _io_scanline_is_hex_digit( char );

/*!
 * \brief Converts an input line of ASCII characters based upon a provided 
 * string format.
 * 
 * \param[in] line_ptr The input line of ASCII data.
 * \param[in] format   Format first points to the format string.
 * \param[in] args_ptr The list of parameters.
 * 
 * \return Number of input items converted and assigned.
 */ 
_mqx_int _io_scanline
   (
      char _PTR_ line_ptr,
      char _PTR_ format, 
      va_list       args_ptr
   )
{ /* Body */
            char       suppress_field;
   register _mqx_int   c;
   register _mqx_uint  n;
            char _PTR_ sptr = NULL;
            _mqx_int   sign;
            uint_32    val;
            _mqx_int   width;
            _mqx_int   numtype;  /* used to indicate bit size of argument */
   register _mqx_int   number_of_chars;
            _mqx_uint  temp;
            _mqx_uint  base;
            pointer    tmp_ptr;
#if MQX_INCLUDE_FLOATING_POINT_IO
            double     dnum;
#endif
   
   n = 0;
   number_of_chars = 0;
   while ((c = *format++) != 0) {

      width = 0;

      /* 
       * skip white space in format string, and any in input line
       */
      if ( (c == ' ') || (c == '\t') ) {
         if ( ! _io_scanline_format_ignore_white_space( 
            (char _PTR_ _PTR_)&format, &temp ) ) {
            /*
             * End of format string encountered, scanning is finished.
             */
            return (_mqx_int)n;
         } /* Endif */

         if ( ! _io_scanline_ignore_white_space( 
            (char _PTR_ _PTR_)&line_ptr , &temp, 0 ) ) {
            /*
             * End of line string encountered, no more input to scan.
             */
            return (_mqx_int)n;
         } /* Endif */
         number_of_chars += temp;
         continue; 
      } /* Endif */

      if ( c != '%' ) {

         /* 
          * if format character is not %, then it must match text
          * in input line 
          */
         if ( c != _io_scanline_ignore_white_space( 
            (char _PTR_ _PTR_)&line_ptr , &temp, 0) ) {
            /*
             * Text not matched, stop scanning..
             */
            return (_mqx_int)n;
         } else {
            ++line_ptr;
         } /* Endif */
         number_of_chars += temp;

      } else {
         /*
          * process % format conversion 
          */
         suppress_field = 0;
         width = 0;
         numtype = SCAN_MLONG;
         sign = 1;
         val = 0;

         /* 
          * Skip whitespace. Check for " %  ", return if found, otherwise
          * get next character.
          */
         if ( ! _io_scanline_format_ignore_white_space( (char _PTR_ _PTR_)&format,
         &temp ) ) {
            return (_mqx_int)n;
         } /* Endif */
         c = *format;

         /*
          * Check for assignment suppression. if "*" given, 
          * suppress assignment 
          */
         if ( c == '*' ) {
            ++suppress_field;   
            c = *(++format); 
         } /* Endif */

         /* 
          * Skip whitespace. Check for " %  *  ", return if found, otherwise
          * get next character.
          */
         if ( ! _io_scanline_format_ignore_white_space( (char _PTR_ _PTR_)&format,
            &temp)) 
         {
            return (_mqx_int)n;
         } /* Endif */
         c = *format;

         /*
          * Now check for a width field 
          */
         width = 0;
         while (  ('0' <= c) && (c <= '9')  ) {
            width = width * 10 + (int_32)(c - '0');
            c = *(++format); 
         } /* Endwhile */

         /* 
          * Skip whitespace. Check for " %  *  23 ", return if found, 
          * otherwise get next character.
          */
         if ( ! _io_scanline_format_ignore_white_space( 
            (char _PTR_ _PTR_)&format,   &temp ) ) {
            return (_mqx_int)n;
         } /* Endif */
         c = *format++;

         /*
          * Check to see if c is lower case, if so convert to upper case
          */
         if (  (c >= 'a') && (c <= 'z')  ) {
            c -= 32; 
         } /* Endif */
                  
         /*
          * Now check to see if c is a type specifier.
          */
         if (c  == 'H') {
            numtype = SCAN_WLONG;
            if ( ! _io_scanline_format_ignore_white_space(
               (char _PTR_ _PTR_)&format, &temp ) )
            {
               return (_mqx_int)n;
            } /* Endif */
            c = *format++;
         } else if (c == 'L') {
            numtype = SCAN_LLONG;
            if ( ! _io_scanline_format_ignore_white_space(
               (char _PTR_ _PTR_)&format, &temp ) )
            {
               return (_mqx_int)n;
            } /* Endif */
            c = *format++;
         } else if (c == 'Z') {               
            numtype = SCAN_BLONG;
            if ( ! _io_scanline_format_ignore_white_space(
               (char _PTR_ _PTR_)&format, &temp ) )
            {
               return (_mqx_int)n;
            } /* Endif */
            c = *format++;
         } else if (c == 'M') {
            numtype = SCAN_MLONG;
            if ( ! _io_scanline_format_ignore_white_space(
               (char _PTR_ _PTR_)&format, &temp ) )
            {
               return (_mqx_int)n;
            } /* Endif */
            c = *format++;
         } /* Endif */

         /*
          * Check to see if c is lower case, if so convert to upper case
          */
         if (  (c >= 'a') && (c <= 'z')  ) {
            c -= 32; 
         } /* Endif */

         /*
          * Now check to see if c is a valid format specifier.
          */
         switch ( c ) {
               
            case 'I':
               c = _io_scanline_ignore_white_space( (char _PTR_ _PTR_)&line_ptr,
                  &temp, 0 );
               if ( *line_ptr == '0' ) {
                  ++number_of_chars;
                  ++line_ptr;
                  if ( width ) {
                     if ( --width <= 0 ) {
                        goto print_val;
                     } /* Endif */
                  } /* Endif */
                  if ( (*line_ptr == 'x') || (*line_ptr == 'X') ) {
                     base = 16;
                     ++line_ptr;
                     ++number_of_chars;
                     if ( width ) {
                        if ( --width <= 0 ) {
                           goto print_val;
                        } /* Endif */
                     } /* Endif */
                  } else if ( (*line_ptr == 'b') || (*line_ptr == 'B') ) {
                     base = 2;
                     ++line_ptr;
                     ++number_of_chars;
                     if ( width ) {
                        if ( --width <= 0 ) {
                           goto print_val;
                        } /* Endif */
                     } /* Endif */
                  } else {
                     base = 8;
                     if ( ! _io_scanline_is_octal_digit(*line_ptr) ) {
                        goto print_val;
                     } /* Endif */
                  } /* Endif */
               } else {
                  goto decimal;
               } /* Endif */
               goto doval;

            case 'P':
            case 'X':
               base = 16;
               c = _io_scanline_ignore_white_space( (char _PTR_ _PTR_)&line_ptr,
                  &temp, 0 );
               if ( *line_ptr == '0' ) {
                  ++line_ptr;
                  ++number_of_chars;
                  if ( width ) {
                     if ( --width <= 0 ) {
                        goto print_val;
                     } /* Endif */
                  } /* Endif */
                  if ( (*line_ptr == 'x') || (*line_ptr == 'X') ) {
                     ++line_ptr;
                     ++number_of_chars;
                     if ( width ) {
                        if ( --width <= 0 ) {
                           goto print_val;
                        } /* Endif */
                     } /* Endif */
                  } else if ( ! _io_scanline_is_hex_digit(*line_ptr) ) {
                     goto print_val;
                  } /* Endif */
               } /* Endif */
               goto doval;

            case 'O':
               base = 8;
               c = _io_scanline_ignore_white_space( (char _PTR_ _PTR_)&line_ptr,
                  &temp, 0 );
               if ( *line_ptr == '0' ) {
                  ++number_of_chars;
                  ++line_ptr;
                  if ( width ) {
                     if ( --width <= 0 ) {
                        goto print_val;
                     } /* Endif */
                  } /* Endif */
                  if ( ! _io_scanline_is_octal_digit(*line_ptr) ) {
                     goto print_val;
                  }
               } /* Endif */
               goto doval;

            case 'B':
               base = 2;
               c = _io_scanline_ignore_white_space( (char _PTR_ _PTR_)&line_ptr,
                  &temp, 0 );
               if ( *line_ptr == '0' ) {
                  ++line_ptr;
                  ++number_of_chars;
                  if ( width ) {
                     if ( --width <= 0 ) {
                        goto print_val;
                     } /* Endif */
                  } /* Endif */
                  if ( (*line_ptr == 'b') || (*line_ptr == 'B') ) {
                     ++line_ptr;
                     ++number_of_chars;
                     if ( width ) {
                        if ( --width <= 0 ) {
                           goto print_val;
                        } /* Endif */
                     } /* Endif */
                  } else if ( ! _io_scanline_is_hex_digit(*line_ptr) ) {
                     goto print_val;
                  } /* Endif */
               } /* Endif */
               goto doval;

            case 'D':
               decimal:
               base = 10;
               temp = 0;
               if (  _io_scanline_ignore_white_space( (char _PTR_ _PTR_)&line_ptr,
                  &temp, 0 ) == '-'  ) 
               {
                  number_of_chars += temp;
                  sign = -1;
                  ++line_ptr;
                  ++number_of_chars;
                  if ( width ) {
                     width -= (int_32)temp;
                     if ( width <= 0 ) {
                        goto print_val;
                     } /* Endif */
                  } /* Endif */
               } else {
                  number_of_chars += temp;
               } /* Endif */

            case 'U':
               base = 10;
               c = _io_scanline_ignore_white_space( (char _PTR_ _PTR_)&line_ptr,
                  &temp, 0 );
doval:  
               val = 0;
               /* remove spaces if any but don't */
               /* parse passed end of line       */
               c = *line_ptr;
               number_of_chars += temp;
               if ( width ) {
                  width -= temp;
                  if ( width <= 0 ) {
                     break;
                  } /* Endif */
               } /* Endif */
               if (  _io_scanline_base_convert( (uchar)c, base ) == SCAN_ERROR  ) {
                  return (_mqx_int)n; 
               } /* Endif */

               while ( (( c = _io_scanline_base_convert( *line_ptr, base ))
               != SCAN_ERROR) ) {
                  ++line_ptr;
                  ++number_of_chars;
                  val = val * base + (uint_32)((uchar)c & 0x7F); 
                  if ( width ) {
                     if ( --width <= 0 ) {
                        break;
                     } /* Endif */
                  } /* Endif */
               } /* Endwhile */

print_val:
               if (  ! suppress_field  ) {
                  /* assign value using appropriate pointer */
                  val *= sign;
                  tmp_ptr = (pointer)va_arg(args_ptr, pointer);
                  switch ( numtype ) {
                     case SCAN_LLONG:
                        *((uint_32 _PTR_)tmp_ptr) = val;
                        break;
                     case SCAN_WLONG:
                        *((uint_16 _PTR_)tmp_ptr) = (uint_16)val;
                        break;
                     case SCAN_BLONG:
                        *((uchar _PTR_)tmp_ptr) = (uchar)val;
                        break;
                     case SCAN_MLONG:
                        *((_mqx_uint _PTR_)tmp_ptr) = (_mqx_uint)val;
                        break;
                     default:
                        break;
                  } /* End Switch */
                  ++n;
               } /* Endif */
               break;
               
            case 'S':
               temp = 0;
               _io_scanline_ignore_white_space( (char _PTR_ _PTR_)&line_ptr,
                  &temp, 0 );
               number_of_chars += temp;
               if ( ! suppress_field ) {
                  sptr = (char _PTR_)va_arg(args_ptr, pointer);
               } /* Endif */
               if ( width ) {
                  width -= (int_32)temp;
                  if ( width <= 0 ) {
                     goto string_done;
                  } /* Endif */
               } /* Endif */

               while (( c = *line_ptr ) != 0) {
                  ++line_ptr;
                  ++number_of_chars;
                  if ( c == *format ) {
                     ++format;
                     break;
                  } /* Endif */

                  if ( ! suppress_field ) {
                     *sptr++ = (char)c; 
                  } /* Endif */
                  if ( width ) {
                     if ( --width <= 0 ) {
                        break;
                     } /* Endif */
                  } /* Endif */

               } /* Endwhile */

string_done:
               if ( ! suppress_field ) {
                  ++n;
                  *sptr = '\0';
               } /* Endif */
               break;
               
            case 'C':
               if ( width == 0 ) {
                  width = 1;
               } /* Endif */

               if ( ! suppress_field ) {
                  sptr = (char _PTR_)va_arg(args_ptr, pointer);
               } /* Endif */
               while ( width-- > 0 ) {
                  if ( ! suppress_field ) {
                     *sptr++ = (uchar)*line_ptr;
                  } /* Endif */

                  ++line_ptr;
                  ++number_of_chars;
               } /* Endwhile */
   
               if ( ! suppress_field ) {
                  ++n;
               } /* Endif */
               break;
               
            case 'N':
               if ( ! suppress_field ) {
                  tmp_ptr = va_arg(args_ptr, pointer);
                  *(_mqx_int _PTR_)(tmp_ptr) = (_mqx_int)number_of_chars;
               } /* Endif */
               break;

#if MQX_INCLUDE_FLOATING_POINT_IO
         case 'G':
         case 'F':
         case 'E':
            dnum = strtod((char _PTR_)line_ptr, (char _PTR_ _PTR_)&tmp_ptr);

            if ((dnum == HUGE_VAL) || (dnum == -HUGE_VAL))
            {
               return (_mqx_int)n;
            }

            line_ptr = tmp_ptr;
            tmp_ptr = (pointer)va_arg(args_ptr, pointer);
            if (SCAN_LLONG == numtype)
            {
               *((double _PTR_)tmp_ptr) = dnum;
            } else {
               *((float _PTR_)tmp_ptr) = (float)dnum;
            }
            ++n;
         break;
#endif

            default:
               return (_mqx_int)n;

         } /* End Switch */
         
      } /* Endif */
#if 0
      if ( ! *line_ptr || ! *format ) {
         return (_mqx_int)n; 
      } /* Endif */
#endif

      /* if end of input string, return */

   } /* Endwhile */
   return (_mqx_int)n;

} /* Endbody */


/*!
 * \brief Scanline function which ignores white spaces.
 * 
 * \param[in,out] s_ptr The addres of the string pointer to update.
 * \param[out]    count The number of characters skipped.
 * \param[in]     width Maximum number of white spaces to skip.
 * 
 * \return String without white spaces. 
 */ 
char _io_scanline_ignore_white_space
   (  
      register char _PTR_ _PTR_ s_ptr,
      register _mqx_uint _PTR_  count,
      register _mqx_uint        width
   )
{ /* Body */
   register char c;
   
   c = **s_ptr;
   *count = 0;
   while (  (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') ||
   (c == '\v') || (c == '\f') ) {
      c = *(++*s_ptr);
      (*count)++;
      if ( width ) {
         if ( --width == 0 ) {
            return(c);
         } /* Endif */
      } /* Endif */
   } /* Endwhile */
   return (c);

} /* Endbody */

/*!
 * \brief Scanline function which ignores white spaces.
 * 
 * \param[in,out] s_ptr The address of the string pointer.
 * \param[out]    count The number of characters skipped.
 * 
 * \return String without white spaces. 
 */ 
char _io_scanline_format_ignore_white_space
   ( 
      register char _PTR_ _PTR_ s_ptr,
      register _mqx_uint _PTR_  count
   )
{ /* Body */
   register char c;
   
   *count = 0;
   c = **s_ptr;
   while (  (c == ' ') || (c == '\t') ) {
      c = *(++*s_ptr);
      (*count)++;
   } /* Endwhile */
   return (c);

} /* Endbody */

/*!
 * \brief Converts input string.
 * 
 * \param[in] c    The character to convert.
 * \param[in] base The base to convert the character in.
 * 
 * \return Input string.
 * \return SCAN_ERROR (Failure.)  
 */ 
int_32 _io_scanline_base_convert
   (
      register unsigned char c,
      _mqx_uint              base
   )
{ /* Body */

   if (  c >= 'a' && c <= 'z'  ) {
      /* upper case c */
      c -= 32; 
   } /* Endif */
   
   if (  c >= 'A' && c <= 'Z'  ) {
      /* reduce hex digit */
      c -= 55;
   } else if (  (c >= '0') && (c <= '9')  ) {
      /* reduce decimal digit */
      c -= 0x30;
   } else {
      return SCAN_ERROR;
   } /* Endif */
   if ( (_mqx_uint)c  > (base-1) ) {
      return SCAN_ERROR;
   } else {
      return c;
   } /* Endif */

} /* Endbody */

/*!
 * \brief Determines whether input character is an octal number.
 * 
 * \param[in] c The character to check.
 * 
 * \return TRUE (Input is an octal number.), FALSE (Input is not an octal number.) 
 */ 
boolean _io_scanline_is_octal_digit
   (
      char c
   )
{ /* Body */

   if ( (c >= '0') && (c <= '7') ) {   /* An octal digit */
      return TRUE;
   } else {
      return FALSE;
   } /* Endif */

} /* Endbody */

/*!
 * \brief Determines whether input character is a hexadecimal number.
 * 
 * \param[in] c The character to check.
 * 
 * \return TRUE (Input is a hexadecimal number.), FALSE (Input is not a 
 * hexadecimal number.) 
 */  
boolean _io_scanline_is_hex_digit
   (
      char c
   )
{ /* Body */

   if ( ((c >= '0') && (c <= '9')) ||
      ((c >= 'a') && (c <= 'f')) ||
      ((c >= 'A') && (c <= 'F'))
   ) {   /* A hex digit */
      return TRUE;
   } else {
      return FALSE;
   } /* Endif */

} /* Endbody */

/* EOF */
