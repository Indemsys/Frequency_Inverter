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
* $FileName: io_fp.c$
* $Version : 3.8.12.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the functions for the formatting of
*   output for floating point numbers
*
*END************************************************************************/

#include <math.h>
#include <ctype.h>
#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "fpio_prv.h"


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_strtod
* Returned Value   : double the number obtained from the string
* Comments         :
*   this function converts a string to a double.
*
*END*-------------------------------------------------------------------------*/

static volatile double tmp_d1, tmp_d2; 

#ifdef NEED_STRTOD
/* The compiler does not provide a good strtod */


static const double _io_strtod_powers_of_ten[] = {
    1.0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6, 1.0e7, 1.0e8, 1.0e9
};
static const double _io_strtod_inverse_powers_of_ten[] = {
    1.0, 1.0e-1, 1.0e-2, 1.0e-3, 1.0e-4, 1.0e-5, 1.0e-6, 1.0e-7, 1.0e-8, 1.0e-9
};
/*!
 * \brief Converts a string to a double.
 * 
 * \param[in]     nptr   The string containing the number.
 * \param[in,out] endptr The string address after the number.
 * 
 * \return Converted string.
 * \return 0.0 (Failure.) 
 */ 
double _io_strtod
   (
      register char _PTR_ nptr, 
      char _PTR_ _PTR_    endptr
   )
{
   register int_32 x;       /* Partial result */
   register int_32 exp;     /* Power of 10 for the result */
   register double result;
   register _mqx_int ne;     /* Number of digits seen so far */
   register _mqx_int nx;     /* Number of didits used so far */
   register int_32 eminus, minus;
   register char _PTR_ p;

   /* skip leading white space */
   p = (char _PTR_)nptr;
   while (isspace(*p)) {
      ++p;
   } /* Endwhile */

   /* consume the leading '+' or '-' */
   minus = 0;
   if (*p == '+') {
      ++p;
   } else if (*p == '-') {
      ++p;
      minus = 1;
   } /* Endif */

   if ((*p != '.') && (!isdigit(*p))) {
      /* no conversion possible: return initial nptr value */
      if (endptr) {
         *endptr = (char _PTR_)nptr;
      } /* Endif */
      return (0.0);
   } /* Endif */

   /* skip leading zeros */
   while (*p == '0') {
      ++p;
   } /* Endwhile */

   result = 0;
   nx = 0;
   ne = 0;
   exp = 0;

   /* read the integer part */
   for (x = 0; isdigit(*p); ++p) {
      if (ne < MAX_PRECISION) {
         if (nx == 9) {
            if (ne == 9) {
               result = (double)x; /* d was still zero */
            } else {
               result = result * _io_strtod_powers_of_ten[9] + (double)x;
            } /* Endif */
            x = nx = 0;
         } /* Endif */
         x = 10 * x + (*p - '0');
         ++ne;
         ++nx;
      } else { /* ignore digits after max precision */
         ++exp;
      } /* Endif */
   } /* Endfor */

   /* read the fractional part */
   if (*p == '.') {
      for (++p; isdigit(*p); ++p) {
         if (ne == 0 && *p == '0') {
            --exp; /* skip leading zeros if no digits yet */
         } else if (ne < MAX_PRECISION) {
            --exp;
            if (nx == 9) {
               if (ne == 9) {
                  result = (double)x; /* d was still zero */
               } else {
                  result = result * _io_strtod_powers_of_ten[9] + (double)x;
               } /* Endif */
               x = 0, nx = 0;
            } /* Endif */
            x = 10 * x + (*p - '0'); 
            ++ne;
            ++nx;
         } /* Endif */
      } /* Endif */
   } /* Endfor */

   /* pick up the pending product in x */
   if (nx != 0) {
      if (x == 0) {
         exp += nx;
      } else {
         if (ne == 9) {
            result = (double)x; /* d was still zero */
         } else {
            result = result * _io_strtod_powers_of_ten[nx] + (double)x;
         } /* Endif */
      } /* Endif */
   } /* Endfor */

   /* read the exponent */
   if (*p == 'e' || *p == 'E') {
      ++p;
      eminus = 0;
      if (*p == '+') {
         ++p;
      } else if (*p == '-') {
         ++p;
         eminus = 1;
      } /* Endif */

      for (x = 0; isdigit(*p); ++p) {
         x = 10 * x + (*p - '0');
         /* watch for overflow and underflow */
         if (eminus) {
            /* check for underflow */
            if ((exp - x) < -324) {
               exp = 0;
               x = 999;
            } /* Endif */
         } else {
            /* check for overflow */
            if ((exp + x) > DBL_MAX_10_EXP) {
               exp = 0;
               x = 999;
            } /* Endif */
         } /* Endif */
      } /* Endfor */
      if (eminus) {
         if (x == 999) { /* underflow */
            _task_errno = MQX_ERANGE;
            if (endptr) {
               *endptr = p;
            } /* Endif */
            return (0.0);
         } /* Endif */
         exp -= x;
      } else {
         if (x == 999) { /* overflow */
            _task_errno = MQX_ERANGE;
            if (endptr) {
               *endptr = p;
            } /* Endif */
            return (minus ? -HUGE_VAL : HUGE_VAL);
         } /* Endif */
         exp += x;
      } /* Endif */
   } /* Endif */

   /* multiply in the exponent */
   if (exp != 0) {
      while (9 <= exp) {
         result *= _io_strtod_powers_of_ten[9];
         exp -= 9;
      } /* Endwhile */
      while (exp <= -9) {
         result *= _io_strtod_inverse_powers_of_ten[9];
         exp += 9;
      } /* Endwhile */
      /* now exp is between -8 and +8 */
      if (0 < exp) {
         result *= _io_strtod_powers_of_ten[exp];
      } else if (exp < 0) {
         result *= _io_strtod_inverse_powers_of_ten[-exp];
      } /* Endif */
   } /* Endif */

   if (endptr) {
      *endptr = (char *)p;
   } /* Endif */
   return (minus ? -result : result);

} /* Endbody */
#else
#ifndef strtod

extern double strtod(const char _PTR_, char _PTR_ _PTR_);
#endif

/*!
 * \brief Converts a string to a double.
 * 
 * \param[in]     nptr   Pointer to the string containing the number.
 * \param[in,out] endptr Pointer to the string address after the number.
 * 
 * \return Converted string.
 * \return 0.0 (Failure.) 
 */
double _io_strtod
   (
      char _PTR_ nptr, 
      char _PTR_ _PTR_ endptr
   )
{ /* Body */

   return strtod(nptr, endptr);

} /* Endbody */
#endif


#ifdef NEED_MODF
/* The compiler does not provide a good strtod */

static const uchar _io_modf_masktable[9] =
    { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };
/*!
 * \brief Gets the fractional part.
 * 
 * \param[in]  input_dbl   The number to split.
 * \param[out] intpart_ptr Where to write the integral part of the double.
 * 
 * \return Fractional part of number.
 * \return 0 if exponent is larger than mantissa     
 */ 
double _io_modf
   (
      double input_dbl, 
      double _PTR_ intpart_ptr
   )
{ /* Body */
    FP_UNION  num;
    FP_UNION  intpart;
    int_32    i;
    int_32    exponent;
    int_32    numbits;
    int_32    numbytes;
#if defined(PSP_TMS32030) || defined(PSP_TMS32040)
    uint_32   temp32;
#endif

    intpart.DOUBLE = input_dbl;
    num.DOUBLE     = input_dbl;

    /*----------------------------------------------------------------*/
    /* exponent will have number of bits of the mantissa we want to   */
    /* save.  if less than zero save none, greater than total         */
    /* number of bits of mantissa then save it all.                   */
    /*----------------------------------------------------------------*/

   exponent = EXPOF(num.LONG);
   exponent = exponent - EXP_BIAS;
   if (exponent < 0 ) {
      *intpart_ptr = 0;
      return(input_dbl);
   } else if (exponent >= MANT_BITS) {
      *intpart_ptr = input_dbl;
      return(0);
   } /* Endif */
   
   exponent += NON_MANT_BITS;
   numbytes = exponent / 8;
   numbits  = exponent - (numbytes * 8);
#if defined(PSP_TMS32030) || defined(PSP_TMS32040)
   /* Must swap TI longwords */
   temp32 = intpart->LONG[1];
   intpart.LONG[1] = intpart.LONG[0];
   intpart.LONG[0] = temp32;
#endif
#if PSP_ENDIAN == MQX_BIG_ENDIAN
   for (i = numbytes + 1; i < BYTES_PER_DOUBLE; i++) {
      intpart.BYTE[i] = 0;
   } /* Endfor */
   intpart.BYTE[numbytes] = intpart.BYTE[numbytes] & _io_modf_masktable[numbits];
#else
   for (i = 0; i < (BYTES_PER_DOUBLE-1) - numbytes; i++) {
      intpart.BYTE[i] = 0;
   } /* Endfor */
   /* i has the index to the byte with some significant mantissa */
   intpart.BYTE[i] = intpart.BYTE[i] & _io_modf_masktable[numbits];
#endif
#if defined(PSP_TMS32030) || defined(PSP_TMS32040)
   /* Swap back */
   temp32 = intpart.LONG[1];
   intpart.LONG[1] = intpart.LONG[0];
   intpart.LONG[0] = temp32;
#endif

   num.DOUBLE = num.DOUBLE - intpart.DOUBLE;
   *intpart_ptr = intpart.DOUBLE;

   return(num.DOUBLE);

} /* Endbody */
#else
/*!
 * \brief Gets the fractional part.
 * 
 * \param[in]  input_dbl   The number to split.
 * \param[out] intpart_ptr Where to write the integral part of the double.
 * 
 * \return Fractional part of number.
 * \return 0 if exponent is larger than mantissa     
 */
double _io_modf
   (
      double input_dbl, 
      double _PTR_ intpart_ptr
   )
{ /* Body */
   return modf(input_dbl, intpart_ptr);
}
#endif

/*!
 * \brief Converts argument to ASCII string.
 * 
 * \param[in]  arg     The number to be converted.
 * \param[in]  ndigits The maximal width of the field.
 * \param[out] decpt   Where the decimal place should be.
 * \param[out] sign    The sign of the number.
 * \param[in]  eflag   Is this an E format number.
 * \param[out] buf     The character string of digits for the number.
 * \param[in]  type    Use lower or upper case. 
 * 
 * \return ASCII string. 
 */ 
char _PTR_ _io_cvt
   (
      double     arg,
      int_32    ndigits,
      int_32_ptr decpt,
      char _PTR_ sign,
      char       eflag,
      char _PTR_ buf,
      char       type
   )
{ /* Body */
    register int_32      r2;
             double      intpart;
             double      tmpintpart; 
             double      fracpart;
    register char _PTR_  p;
    register char _PTR_  p1;
             uint_32      i;
             int_32      tmp = 0;

   if (ndigits < 0) {
      ndigits = 0;
   } /* Endif */
   if (ndigits >= NDIG-1) {
      ndigits = NDIG-2;
   } /* Endif */
   r2 = 0;
   p = &buf[0];
   if (SIGNOF((unsigned long _PTR_)((pointer)&arg)))
   {
        *sign = (char)TRUE;
        arg = -arg;
   }
   else
   {
        *sign = (char)FALSE;
   }
   if (ISINFINITY((unsigned long _PTR_)((pointer)&arg)))
   {
      buf[0] = 'i';
      return(buf);
   }
   if (ISNAN((unsigned long _PTR_)((pointer)&arg)))
   {
      *sign = (char)FALSE;
      buf[0] = 'n';
      return(buf);
   }
   if (ISZERO((unsigned long _PTR_)((pointer)&arg)))
   {
      if (( type == 'e') || (type == 'E'))
      {
        *decpt = 1; /* avoid e_or_E type create string 0.000e-1 */
      }
      else
      {
        *decpt = 0;
      }
      for(i = 0; i < ndigits; i++)
      {
        buf[i] = '0';
      }
      buf[ndigits] = '\0';
      return(buf);
   }

   tmp_d1 = arg;
   /* modf return fraction part and set integer part */
   arg = modf(arg, (double _PTR_)(&intpart));

   /*
    * arg has decimal part, intpart has integer part.            
    * take the inparts digits one by one and stick them into the end
    * of the buffer then move them to the beginning of it.         
    */
   if (intpart != 0) {
      p1 = &buf[NDIG];
      while (intpart != 0) {
            /*
             * It's possible that inaccuracy will cause the division  
             * by 10 to come out a little bit low.  In C, conversions 
             * from floating point to integer are done by truncation. 
             * We add in .03 so that temp will be rounded up when it  
             * is converted to the nearest number of tenths, as done  
             * by the call to modf and the assignment to i.           
             */
         tmpintpart = (intpart / 10) + 0.03;
         tmpintpart = modf(tmpintpart, (double _PTR_)(&intpart));
         tmpintpart = tmpintpart * 10.0;
         *--p1 = (char)((uint_32)tmpintpart + '0');
         r2++;
      } /* Endwhile */
      while (p1 < &buf[NDIG]) {
         *p++ = *p1++;
      } /* Endwhile */
   } else {

      /*
       * if the intpart was zero, then get the decimal part so that it
       * does not have any zero directly to the right of the decimal  
       * point.  with each shift left of the decimal number adjust the
       * exponent.                       
       */
      if (arg > 0) {
        double temp;
        while (1 > (temp = arg*10))
        {
            arg = temp;
            r2--;
         } /* Endwhile */
      } /* Endif */

   } /* Endif */

   p1 = &buf[ndigits];
   if (eflag == 0) {
      tmp = r2;
      /*
       * Loop around adjusting the pointer up or down in the buffer.   
       * We must be careful not to move the pointer out of the range   
       * of the buffer.  If the pointer goes outside that range and    
       * the buffer is in very low or very high memory, the pointer    
       * value could "wrap" and cause later pointer comparisons to fail.
       */
      if (tmp > 0) {
         while (tmp && p1 < &buf[NDIG-1]) {
            p1++;
            tmp--;
         } /* Endwhile */
      } else {
         while (tmp && p1 > buf) {
            p1--;
            tmp++;
         } /* Endwhile */
         if (tmp < 0) {      /* pointer would have underflowed buffer */
            for (i = 0; i < ndigits; i++)
            {
                buf[i] = '0';
            }
            *decpt = 0;
            buf[ndigits] = '\0';
               /*
                * This indicates that the number is     
                * smaller than the precision they've    
                * given us.  The result will be 0.0.... 
                */
            return(buf);
         } /* Endif */
      } /* Endif */
   } /* Endif */
   *decpt = r2;

   /*
    * we now stick the fractional part in digit by digit regardless  
    * of whether there was a non-zero integer part            
    */
   while (p <= p1) {
      tmpintpart = arg * 10;
      tmpintpart = modf(tmpintpart, (double _PTR_)(&intpart));
      arg = tmpintpart;
      *p++ = (char)((int_32)intpart + '0');
   } /* Endwhile */
   if (!eflag && tmp) {
      /* precision is too large.   */
      /* return with what we have. */
      buf[NDIG-1] = '0';
      return(buf);
   } /* Endif */

   /*
    * round the number 
    */
   p = p1;
   *p1 += 5;
   while (*p1 > '9') {
      *p1 = '0';
      if (p1 > buf) {
         p1--;
         *p1 += 1;
      } else {
         *p1 = '1';
         (*decpt)++;
         if (eflag == 0) {
            if (p > buf) {
               *p = '0';
            } /* Endif */
            p++;
         } /* Endif eflag == 0 */
      } /* Endif */
   } /* Endwhile */
   *p = '\0';
   return(buf);

} /* Endbody */

/*!
 * \brief Make string Not A Number, and INFinite.
 * 
 * \param[out] bp          Where to put the number.
 * \param[in]  numstr      'i' for Infinite, 'n' for NAN, 'z' for ZERO.
 * \param[in]  printsign   Should the sign be printed.
 * \param[in]  printprefix Should a prefix be printed.
 * \param[in]  sign        The sign of the number.
 * \param[in]  type        Type of number for upper or lower case.  
 */ 
static void _io_inf_or_nan
    (
      char _PTR_ bp,
      char _PTR_ numstr,
      char       printsign,
      char       printprefix,
      char       sign,
      char       type
    )
{
    char shift = 'a' - 'A'; /* for lovercase */
    /* Conversion gave 0, infinity, or NAN, use f or F format for output */
     if (sign) 
     {
        *bp++ =  '-';
     } 
     else if (printsign) 
     {
        *bp++ =  '+';
     } 
     else if (printprefix) 
     {
        *bp++ =  ' ';
     }
    /* for uppercase */
    if (type == 'G' || type == 'E' || type == 'F')
    {
        shift = (char)0;
    }
    /* Infinity */
    if ( *numstr == 'i')
    {
    *bp++ = 'I' + shift;
    *bp++ = 'N' + shift;
    *bp++ = 'F' + shift;
    }
    /* NAN */
    else if (*numstr == 'n')
    {
    *bp++ = 'N' + shift;
    *bp++ = 'A' + shift;
    *bp++ = 'N' + shift;
    }
    *bp = '\0';
}

/*!
 * \brief Convert a number (exponent) to ASCII
 * 
 * \param[in] number The number to convert.
 * \param[in] str_ptr Where to store the converted number.
 * 
 * \return ASCII number. 
 */ 
int_32 _io_dtoa
   ( 
      int_32     number,
      char _PTR_ str_ptr
   )
{ /* Body */
   int_32   frac;
   char     i;
   int_32   digits;

   if (number < 0) {
      number = -number;
   }
   if (number > 99) {
      digits = 3;
   } else {
      digits = 2;
   }
   for (i = 1; i <= digits; i++) {
      frac = number % 10;
      number = number / 10;
      str_ptr[digits-i] = (char)(frac + '0');
   }
    return (digits);

} /* Endbody */

/*!
 * \brief Converts a double to E format output.
 * 
 * \param[out] buf         Where to put the number.
 * \param[in]  arg         The number to convert.
 * \param[in]  fillchar    The fill character to use.
 * \param[in]  printsign   Should the sign be printed.
 * \param[in]  printprefix Should a prefix be printed.
 * \param[in]  altformat   Makes sure that output is printed out as floating.
 * \param[in]  precision   What preicision to use.
 * \param[in]  e_or_E      Use lower or uppercase E.
 * 
 * \return Number in E format. 
 */ 
int_32 _io_dtoe
   ( 
      char _PTR_ buf,
      double     arg,
      char _PTR_ fillchar,
      char       printsign,
      char       printprefix,
      char       altformat,
      int_32     precision,
      char       e_or_E
   )
{ /* Body */
   char       sign;
   int_32     decpt;
   char _PTR_ numstr;
   char _PTR_ bp;
   char       cvtbuf[NDIG];

   numstr = _io_cvt(arg, precision + 1, &decpt, &sign, 1, cvtbuf, e_or_E);
   bp = buf;
   if ((*numstr == 'i') || (*numstr == 'n'))
   {
      /* infinity, or NAN */
        _io_inf_or_nan(bp, numstr, printsign, printprefix, sign, e_or_E);
        *fillchar = ' ';
   } else {

      if (sign) {
         *bp++ =  '-';
      } else if (printsign) {
         *bp++ =  '+';
      }
      else if (printprefix) {
         *bp++ =  ' ';
      } /* Endif */
      *bp++ = *numstr++;
      if ((*numstr != '\0') || altformat) {
         *bp++ = '.';
      } /* Endif */
      while (*numstr != '\0') {
        
         *bp++ = *numstr++;
      } /* Endwhile */
      *bp++ = e_or_E;
      if (decpt -1 >= 0) {
         *bp++ = '+';
      } else {
         *bp++ = '-';
      } /* Endif */
      /* update bp after putting in exp */
      bp += _io_dtoa(decpt - 1, bp);
      *bp = '\0';
   } /* Endif */

   return (int)(bp - buf);

} /* Endbody */


/*!
 * \brief Converts a double to F format output
 *
 * \param[out] buf         Where to put the number.
 * \param[in]  arg         The number to convert.
 * \param[in]  fillchar    The fill character to use.
 * \param[in]  printsign   Should the sign be printed.
 * \param[in]  printprefix Should a prefix be printed.
 * \param[in]  altformat   Makes sure that output is printed out as floating.
 * \param[in]  precision   What preicision to use.
 * \param[in]  f_or_F      Use lower or uppercase F. 
 * 
 * \return Number in F format.   
 */ 
int_32 _io_dtof
   (
      char _PTR_  buf,
      double      arg,
      char _PTR_  fillchar,
      char        printsign,
      char        printprefix,
      char        altformat,
      int_32      precision,
      char       f_or_F
   )
{ /* Body */
   char       sign;
   int_32     decpt;
   char _PTR_ numstr;
   char _PTR_ bp;
   char       cvtbuf[NDIG];

   bp = buf;
   if (precision > NDIG) {
      precision = NDIG;
   }

   numstr = _io_cvt(arg, precision, &decpt, &sign, 0, cvtbuf, f_or_F);

   if ((*numstr == 'i') || (*numstr == 'n'))
   {
        _io_inf_or_nan(bp, numstr,printsign , printprefix, sign, f_or_F);
        *fillchar = ' ';
   } else {
      if (sign) {
         *bp++ =  '-';
      } else if (printsign) {
         *bp++ =  '+';
      }
      else if (printprefix) {
         *bp++ =  ' ';
      } /* Endif */
      if (decpt <= 0) {
         *bp++ = '0';
         *bp++ = '.';
         while (decpt++ < 0) {
            *bp++ = '0';
         } /* Endwhile */
         while ((*bp++ = *numstr++) != 0) {
         } /* Endwhile */
         --bp; /* bump bp back to point to the null */
      } else {
         while ((*numstr != '\0') && (decpt-- != 0)) {
            *bp++ = *numstr++;
         } /* Endwhile */
         if ((*numstr != '\0') || altformat) {
            *bp++ = '.';
         } /* Endif */
         if (*numstr != '\0') {
            while ((*bp++ = *numstr++) != 0) { ;
            } /* Endwhile */
            --bp; /* bump bp back to point to the null */
         } else {
            *bp = '\0';
         } /* Endif */
      } /* Endif */
   } /* Endif */

   return (int)(bp - buf); /* same as strlen(buf) */

} /* Endbody */

/*!
 * \brief Converts a double to G format output
 *  
 * \param[out] buf         Where to put the number.
 * \param[in]  arg         The number to convert.
 * \param[in]  fillchar    The fill character to use.
 * \param[in]  printsign   Should the sign be printed.
 * \param[in]  printprefix Should a prefix be printed.
 * \param[in]  altformat   Makes sure that output is printed out as floating.
 * \param[in]  precision   What preicision to use.
 * \param[in]  g_or_G      Use lower or uppercase G. 
 * 
 * \return Number in G format.
 */ 
int_32 _io_dtog
   ( 
      char _PTR_ buf,
      double     arg,
      char _PTR_ fillchar,
      char       printsign,
      char       printprefix,
      char       altformat,
      int_32     precision,
      char       g_or_G
   )
{
   char          sign;
   int_32        decpt, i;
   char _PTR_    numstr;
   char _PTR_    bp;
   char          cvtbuf[NDIG];

   /* First get e or E format */
   g_or_G -= 2;
   numstr = _io_cvt(arg, precision + 1, &decpt, &sign, 1, cvtbuf, g_or_G);
   bp = buf;
   
   if ((*numstr == 'i') || (*numstr == 'n'))
   {
        _io_inf_or_nan(bp, numstr, printsign , printprefix, sign, g_or_G);
        *fillchar = ' ';
   } 
   else 
   {
      /* Check whether f format is more suitable */
      if ((decpt - 1 >= -4) && (decpt - 1 < precision))
      {
         if (precision > NDIG) 
         {
            precision = NDIG;
         }
         numstr = _io_cvt(arg, precision, &decpt, &sign, 0, cvtbuf, (g_or_G - 2));
         g_or_G += 1;
      }
      
      /* Remove trailing zeros */
      if (! altformat)
      {
         for (i = 0; numstr[i] != 0; i++) 
         { ; 
         }
         for (--i; i >= 0; i--)
         {
            if ((numstr[i] != '0') || (i == decpt)) break;
         }
         if ((i >= 0) && (numstr[i] == '0')) 
         {
            i--;
         }
         if (i == -1) 
         {
            if ((g_or_G == 'e') || (g_or_G == 'E'))
            {
               numstr[0] = '0';
               i = 0;
            }
         }
         else if ((numstr[i] < '0') || (numstr[i] > '9'))
         {
            numstr[++i] = '0';
         }
         numstr[i + 1] = '\0';
      }
      
      if ((g_or_G == 'e') || (g_or_G == 'E'))
      {
         /* Output in e, E format */
         if (sign) 
         {
            *bp++ =  '-';
         } 
         else if (printsign) 
         {
            *bp++ =  '+';
         } 
         else if (printprefix) 
         {
            *bp++ =  ' ';
         }
         *bp++ = *numstr++;
         if ((*numstr != '\0') || altformat) 
         {
            *bp++ = '.';
         }
         while (*numstr != '\0') 
         {
            *bp++ = *numstr++;
         }
         *bp++ = g_or_G;
         if (decpt -1 >= 0) 
         {
            *bp++ = '+';
         } 
         else 
         {
            *bp++ = '-';
         }
         /* update bp after putting in exp */
         bp += _io_dtoa(decpt - 1, bp);
         *bp = '\0';
      }
      else
      {
         /* Output in f format */
         if (sign) 
         {
            *bp++ =  '-';
         } 
         else if (printsign) 
         {
            *bp++ =  '+';
         } 
         else if (printprefix) 
         {
            *bp++ =  ' ';
         }
         if (decpt <= 0) 
         {
            *bp++ = '0';
            if(*numstr != 0)
            {
                *bp++ = '.';
            }
            while (decpt++ < 0) 
            {
               *bp++ = '0';
            }
            while ((*bp++ = *numstr++) != 0) 
            {
            }
            --bp; /* bump bp back to point to the null */
         } 
         else 
         {
            while ((*numstr != '\0') && (decpt-- != 0)) 
            {
               *bp++ = *numstr++;
            }
            if ((*numstr != '\0') || altformat) 
            {
               *bp++ = '.';
            }
            if (*numstr != '\0') 
            {
               while ((*bp++ = *numstr++) != 0) 
               { ;
               }
               --bp; /* bump bp back to point to the null */
            } 
            else 
            {
               *bp = '\0';
            }
         }
      }
   }

   return (int)(bp - buf);

} /* Endbody */

/* EOF */
