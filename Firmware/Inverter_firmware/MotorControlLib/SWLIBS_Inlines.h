/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     SWLIBS_Inlines.h
*
* @author   Andrzej Lara, Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Basic inline functions definition file.
*
*******************************************************************************
* This file shall be included only after inclusion of SWLIBS_Typedefs.h
******************************************************************************/
#ifndef _SWLIBS_INLINES_H
#define _SWLIBS_INLINES_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "SWLIBS_Defines.h"

#if defined(__CWCC__)
	#include "intrinsic_cw.h"
	#undef __STATIC_INLINE
	#define __STATIC_INLINE static inline	
#elif defined(__IAR_SYSTEMS_ICC__)
	#include <intrinsics.h>
	#undef __STATIC_INLINE
	#define __STATIC_INLINE static inline
#elif defined(__CC_ARM)	
	#include "MK40N512MD100.h"		//has to be here, without this include it does not work - it is device dependent - should be change
#endif	

	
/******************************************************************************
| defines and macros            (scope: module-local)
|----------------------------------------------------------------------------*/


/******************************************************************************
| Typedefs and structures       (scope: module-local)
|----------------------------------------------------------------------------*/

/******************************************************************************
| Exported variables
|----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
|----------------------------------------------------------------------------*/

/******************************************************************************
| Inline functions
|----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!

@brief      Absolute value of 16-bit fractional type.

@return     Absolute value of the input argument.

@note       Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16 F16Abs(register Frac16 x)
{
    return (Frac16) ((x<0) ? -(x):(x));
}

/*------------------------------------------------------------------------*//*!

@brief      Absolute value with overflow control of 16-bit fractional
            number.

@return     Absolute value of the input argument.

@note       Overflow is detected and the function saturates if the result
            does not fit into the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16AbsSat(register Frac16 x)
{
    register Word32 y;

    y = (F16TOINT32(x)<0) ? -F16TOINT32(x):F16TOINT32(x);
    y = (y == -(Word32) INT16_MIN) ? (Word32) INT16_MAX:y;

    return INT32TOF16(y);
}

/*------------------------------------------------------------------------*//*!

@brief       Negative value of 16-bit fraction type.

@return      Negative value of the input argument.

@note        Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Neg(register Frac16 x)
{
    return (Frac16) (-x);
}

/*------------------------------------------------------------------------*//*!

@brief       Negative value with overflow control of 16-bit fraction type.

@return      Negative value of the input argument.

@note        Overflow is detected and the function saturates if the result
             cannot fit in the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16NegSat(register Frac16 x)
{
    register Word32 y;

    y = -F16TOINT32(x);
    y = (y == -(Word32) INT16_MIN) ? (Word32) INT16_MAX:y;

    return INT32TOF16(y);
}

/*------------------------------------------------------------------------*//*!

@brief        Add two fractional 16-bit values.

@return       Sum of the input arguments.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Add(register Frac16 x, register Frac16 y)
{
    return (Frac16) (x + y);
}

/*------------------------------------------------------------------------*//*!

@brief        Add with overflow control two fractional 16-bit values.

@return       Sum of the input arguments.

@note         Overflow is detected. The function saturates the return
              value if it cannot fit in the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16AddSat(register Frac16 x, register Frac16 y)
{
    return __QADD16(x,y);
}

/*------------------------------------------------------------------------*//*!

@brief        Subtraction of two fractional 16-bit values. The second
              argument is subtracted from the first one.

@return       The subtraction of the second argument from the first argument.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Sub(register Frac16 x, register Frac16 y)
{
    return (Frac16) (x - y);
}

/*------------------------------------------------------------------------*//*!

@brief        Subtraction with overflow control of two fractional 16-bit
              numbers. The second argument is subtracted from the first one.

@return       The subtraction of the second argument from the first argument.

@note         Overflow is detected. The function saturates the return
              value if it cannot fit in the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16SubSat(register Frac16 x, register Frac16 y)
{
        return __QSUB16(x,y);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional multiplication of two fractional 16-bit values.

@return       16 most significant bits of fractional multiplication of the
              input arguments. The extra bits beyond the 16-bit boundary
              are discarded.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Mul(register Frac16 x, register Frac16 y)
{
	return INT32TOF16((F16TOINT32(x)*F16TOINT32(y))>>15);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional multiplication of two fractional 16-bit values.

@return       16 most significant bits of fractional multiplication of the
              input arguments. The extra bits beyond the 16-bit boundary
              are discarded.

@note         Overflow is detected. The functions saturates the return
              value if it cannot fit into the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16MulSat(register Frac16 x, register Frac16 y)
{
	register Word32 z;

	z = (Word32) (((Word32) x)*((Word32) y)>>15);

	z = (z > (Word32) INT16_MAX) ? (Word32) INT16_MAX:z;

	return INT32TOF16(z);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional division of two 16-bit numbers. The first argument
              is the numerator and the second one is the denominator.

@return       15 most significant bits of fractional division of the first
              by the second argument. The extra bits beyond the 15-bit boundary
              are discarded.

@note         Overflow is not detected. The first argument must be lower
              in magnitude than the second argument, otherwise the result
              is undefined.

              If the return value cannot be represented by the 16-bit
              fractional format due to finite resolution, the return value is
              rounded toward zero.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Div(register Frac16 x, register Frac16 y)
{
	register Word32 n;
	register Word32 d;
	register Word32 q;

	n = F16TOINT32(x)<<15;
	d = F16TOINT32(y);
	q = n/d;

	return INT32TOF16(q);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional division of a 32-bit by a 16-bit numbers. The first
              32-bit argument is the numerator and the second 16-bit argument
              is the denominator.

@return       15 most significant bits of fractional division of the first
              by the second argument. The extra bits beyond the 15-bit boundary
              are discarded.

@note         Overflow is not detected. The first argument must be lower
              in magnitude than the second argument, otherwise the result
              is undefined.

              If the return value cannot be represented by the 16-bit
              fractional format due to finite resolution, the return value is
              rounded down (so called rounding toward minus infinity). It should
              be noticed that the used rounding mode is different from the
              rounding mode used in the integer division (rounding toward zero).
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16DivF32F16(register Frac32 x, register Frac16 y)
{
	register Word32 n;
	register Word32 d;
	register Word32 q;

	n = F32TOINT32(x);
	d = F16TOINT32(y);
	q = n/d;
	q >>= 1;

	return INT32TOF16(q);
}

/*------------------------------------------------------------------------*//*!

@brief        Right, bidirectional shift of 16-bit fractional value (x)
              by the specified shift amount (s).

@return       16-bit fractional value shifted by the shift amount, right
              is the positive direction. The bits beyond the 16-bit
              boundary of the results are discarded.

@note         Overflow is not detected.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -16...16.
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Shr(register Frac16 x, register Word16 s)
{
    return (Frac16) ((s<0) ? x<<(-s):x>>s);
}

/*------------------------------------------------------------------------*//*!

@brief        Right, bidirectional shift of 16-bit fractional value with
              overflow control.

@return       16-bit fractional value shifted by the shift amount, right is
              the positive direction. The bits beyond the 16-bit boundary
              are discarded.

@note         Overflow is detected and the function saturates if the return
              value cannot fit into the return type.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -16...16.
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16ShrSat(register Frac16 x, register Word16 s)
{
    register Word32 xl;

    xl = F16TOINT32(x);
    if ( s < 0 )
    {
        xl = xl << (-s);
        xl = (xl < (Word32) INT16_MIN) ? (Word32) INT16_MIN:xl;
        xl = (xl > (Word32) INT16_MAX) ? (Word32) INT16_MAX:xl;
    }
    else
    {
        xl = xl >> s;
    }

    return INT32TOF16(xl);
}

/*------------------------------------------------------------------------*//*!

@brief        Left, bidirectional shift of 16-bit fractional value.

@return       16-bit fractional value shifted by the shift amount, left is
              the positive direction. The bits beyond the 16-bit boundary
              are discarded.

@note         Overflow is not detected.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -16...16.
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Shl(register Frac16 x, register Word16 s)
{
    return INT32TOF16((s<0) ? F16TOINT32(x)>>(-s):F16TOINT32(x)<<s);
}

/*------------------------------------------------------------------------*//*!

@brief        Left, bidirectional shift of 16-bit fractional value with
              overflow control.

@return       16-bit fractional value shifted by the shift amount, left is
              the positive direction. The bits beyond the 16-bit boundary
              are discarded.

@note         Overflow is detected. The function returns a saturated
              fractional value if the return value cannot fit into the return
              type.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -16...16.
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16ShlSat(register Frac16 x, register Word16 s)
{
    register Word32 xl;

    xl = F16TOINT32(x);
    if ( s < 0 )
    {
        xl = xl >> (-s);
    }
    else
    {
        xl = xl << s;
        xl = (xl < (Word32) INT16_MIN) ? (Word32) INT16_MIN:xl;
        xl = (xl > (Word32) INT16_MAX) ? (Word32) INT16_MAX:xl;
    }

    return INT32TOF16(xl);
}

/*------------------------------------------------------------------------*//*!

@brief        Right, unidirectional shift of 16-bit fractional value (x) by
              the specified shift amount (s).

@return       16-bit fractional value shifted right by the shift amount.
              The bits beyond the 16-bit boundary of the result are discarded.

@note         This is the unidirectional right shift.

              The shift amount cannot exceed in magnitude the bit-width of
              the shifted value, that means it must be within the range 0...15.
              Otherwise the result of the function is undefined.

@warning      This function is target dependent.  The shift result for the
              negative value to be shifted may be undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Shruni(register Frac16 x, register UWord16 s)
{
    return (Frac16) (x>>s);
}

/*------------------------------------------------------------------------*//*!

@brief        Counts the number of left shift needed for normalization of
              the input 16-bit fractional value

@return       The number of left shift needed to normalize the argument.
              For the input 0 returns 0.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Word16
F16Norm(register Frac16 x)
{
	register Word32 sham;
	register Word32 shdel;
	register Word32 xlsh;
	register Word32 xl;
	register Word32 b;

	if ( x == 0)
	{
		return 0;
	}

	b = (x >= 0) ? 0x00007fff:0x00008000;

	sham = 0;
	shdel = 8;
	xl = (x<0) ? -F16TOINT32(x):F16TOINT32(x);

	do
	{
		sham = sham + shdel;
		xlsh = xl << sham;
		sham = (xlsh > b) ? sham - shdel:sham;
		shdel >>= 1;
	} while ( shdel > 0 );

	return (Word16) sham;
}

/*------------------------------------------------------------------------*//*!

@brief        Rounds the input 32-bit fractional value to 16-bit fractional
              value.

@return       Round to 16-bit fractional value.

@note         Rounding is performed by adding 0x00008000 to the 32-bit
              argument (round to nearest). Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16Roundton(register Frac32 x)
{
return INT32TOF16((F32TOINT32(x) + 0x00008000)>>16);
}

/*------------------------------------------------------------------------*//*!

@brief        Rounds the input 32-bit fractional value to 16-bit fractional
              value.

@return       Round to 16-bit fractional value.

@note         Rounding is performed by adding 0x00008000 to the 32-bit
              argument (round to nearest). Overflow is detected. The function
              saturates. if the return value does not fit into the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac16
F16RoundtonSat(register Frac32 x)
{
	register Word32 y;

	y = F32TOINT32(x) + 0x00008000;
	y = (F32TOINT32(x) >= 0x7fff8000) ? F32TOINT32(x):y;

	return INT32TOF16(y>>16);
}

/*------------------------------------------------------------------------*//*!

@brief      Absolute value of 32-bit fractional type.

@return     Absolute value of the input argument.

@note       Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Abs(register Frac32 x)
{
    return (x<0) ? -x:x;
}

/*------------------------------------------------------------------------*//*!

@brief      Absolute value with overflow control of 32-bit fractional type.

@return     Absolute value of the input argument.

@note       Overflow is detected and the function saturates the result if
            it cannot fit into the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32AbsSat(register Frac32 x)
{
    register Word32 y;

    y = (F32TOINT32(x)<0) ? -F32TOINT32(x):F32TOINT32(x);
    y = (y == INT32_MIN) ? INT32_MAX:y;

    return INT32TOF32(y);
}

/*------------------------------------------------------------------------*//*!

@brief       Negative value of 32-bit fraction type.

@return      Negative value of the input argument.

@note        Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Neg(register Frac32 x)
{
    return -x;
}

/*------------------------------------------------------------------------*//*!

@brief       Negative value with overflow control of 32-bit fraction type.

@return      Negative value of the input argument.

@note        Overflow is detected and the function saturates if the result
             cannot fit in the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32NegSat(register Frac32 x)
{
    register Word32 y;

    y = -F32TOINT32(x);
    y = (y == INT32_MIN) ? INT32_MAX:y;

    return INT32TOF32(y);
}

/*------------------------------------------------------------------------*//*!

@brief        Add two fractional 32-bit values.

@return       Sum of the input arguments.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Add(register Frac32 x, register Frac32 y)
{
 return x + y;
}

/*------------------------------------------------------------------------*//*!

@brief        Add with overflow control two fractional 32-bit values.

@return       Sum of the input arguments.

@note         Overflow is detected. The function saturates the return
              value if it cannot fit in the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32AddSat(register Frac32 x, register Frac32 y)
{
    return __QADD(x,y);
}

/*------------------------------------------------------------------------*//*!

@brief        Subtraction of two fractional 32-bit values. The second
              argument is subtracted from the first one.

@return       The subtraction of the second argument from the first argument.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Sub(register Frac32 x, register Frac32 y)
{
 return x - y;
}

/*------------------------------------------------------------------------*//*!

@brief        Subtraction with overflow control of two fractional 32-bit
              numbers. The second argument is subtracted from the first one.

@return       The subtraction of the second argument from the first argument.

@note         Overflow is detected. The function saturates the return
              value if it cannot fit in the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32SubSat(register Frac32 x, register Frac32 y)
{
    return __QSUB (x,y);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional multiplication of two fractional 16-bit values.

@return       All 32 bits of fractional multiplication of the two 16-bit
              input arguments.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32MulF16F16(register Frac16 x, register Frac16 y)
{
	return INT32TOF32((F16TOINT32(x)*F16TOINT32(y))<<1);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional multiplication of two fractional 16-bit values with
              overflow detection.

@return       All 32 bits of fractional multiplication of the two 16-bit
              input arguments.

@note         Overflow is detected. The functions saturates the return
              value if it cannot fit into the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32MulSatF16F16(register Frac16 x, register Frac16 y)
{
	register Word32 m;

	m = (F16TOINT32(x)*F16TOINT32(y))<<1;
	m = (m == INT32_MIN) ? INT32_MAX:m;

	return INT32TOF32(m);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional multiplication of two fractional 32-bit values.

@return       32 most significant bits of fractional multiplication of the
              input arguments.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Mul(register Frac32 x, register Frac32 y)
{
	return INT32TOF32((Word32) ((F32TOINT64(x)*F32TOINT64(y))>>31));
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional multiplication of two fractional 32-bit values.

@return       32 most significant bits of fractional multiplication of the
              input arguments.

@note         Overflow is detected. The functions saturates the return
              value if it cannot fit into the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32MulSat(register Frac32 x, register Frac32 y)
{
	register Word32 z;

	z = (Word32) ((F32TOINT64(x)*F32TOINT64(y))>>31);
	z = (z == INT32_MIN) ? INT32_MAX:z;

	return INT32TOF32(z);
}

/*------------------------------------------------------------------------*//*!

@brief        Multiply two 16-bit fractional values and accumulate with
              a 32-bit fractional value.

@return       All bits of the multiply-and-accumulate operation.

@note         Overflow is not detected.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32MacF16F16(register Frac32 a, register Frac16 x, register Frac16 y)
{
	return INT32TOF32((F32TOINT32(a) + (Word32) ((F32TOINT16(x)*F32TOINT16(y))<<1)));
}

/*------------------------------------------------------------------------*//*!

@brief        Multiply two 16-bit fractional values and accumulate with
              a 32-bit fractional value. Perform overflow control.

@return       All bits of the multiply-and-accumulate operation.

@note         Overflow is detected. The functions saturates the return
              value if it cannot fit into the return type.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32MacSatF16F16(register Frac32 a, register Frac16 x, register Frac16 y)
{
	register Word32 m;

	m = F32TOINT32(x)*F32TOINT32(y);
	m <<=1;

	return __QADD(a,m);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional division of two 32-bit values. The first argument is
              the numerator and the second one is the denominator.

@return       31 most significant bits of the result of the fractional
              division of the first by the second argument. The extra bits
              beyond the 31-bit boundary are discarded.

@note         Overflow is not detected. The first argument must be lower in
              magnitude than the second argument, otherwise the result is
              undefined.

              If the return value cannot be represented by the 32-bit
              fractional format due to finite resolution, the return value is
              rounded toward zero.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Word32 F32DivNorm(register UWord32 x)
{
    register Word32 b;
    register Word32 m;
    register Word32 mm;
    register Word32 mmp;
    register Word32 mmn;

    m = 15;

    mmp = 8;
    mmn = -8;
    b = 1<<m;
    mm = (x >= b) ? mmp:mmn;
    m += mm;

    mmp = 4;
    mmn = -4;
    b = 1<<m;
    mm = (x >= b) ? mmp:mmn;
    m += mm;

    mmp = 2;
    mmn = -2;
    b = 1<<m;
    mm = (x >= b) ? mmp:mmn;
    m += mm;

    mmp = 1;
    mmn = -1;
    b = 1<<m;
    mm = (x >= b) ? mmp:mmn;
    m += mm;

    mmp = 0;
    b = 1<<m;
    mm = (x >= b) ? mmp:mmn;
    m += mm;

    m = 30 - m;

    return m;
}

__STATIC_INLINE Frac32
F32Div(register Frac32 x, register Frac32 y)
{
    register UWord32 dh;
    register UWord32 dl;
    register Word32 q;
    register UWord32 n;
    register UWord32 nlsb;
    register UWord32 d;
    register UWord32 qh;
    register UWord32 ql;
    register UWord32 r1;
    register UWord32 r2;
    register Word64 r;
    register Word32 sign;
    register Word32 m;

    /* Special case for denominator at minimum.
     */
    if ( y == 0x80000000 )
    {
        return -x;
    }

    /* Record the sign and make argument absolute */
    sign = x^y;
    n = (UWord32) ((F32TOINT32(x) < 0) ? -F32TOINT32(x):F32TOINT32(x));
    d = (UWord32) ((F32TOINT32(y) < 0) ? -F32TOINT32(y):F32TOINT32(y));

    /* Normalize denominator and numerator*/
    m = F32DivNorm(d);
    d <<= m;
    n <<= m;

    /* Record lsb of n*/
    nlsb = (n & 0x00000001)<<15;

    dh = d>>16;
    dl = 0x0000ffff & d;

    /* Compute tentative 15 higher bits of the quotient */
    qh = n/dh;

    /* Limit the quotient to avoid overflow */
    qh = (qh > 0x0000ffff) ? 0x0000ffff:qh;

    /* Shift down to avoid overflow, lsb of n is discarded */
    qh >>= 1;
    n >>= 1;

    /* Compute the first remainder */
    r1 = qh*dh;
    r2 = qh*dl;
    r1 = n-r1;
    r = ((Word64) r1)<<16;
    r = r - r2;

    /* Retrieve lsb of n */
    r += nlsb;

    /* Make appropriate correction */
    if ( r < 0 )
    {
	r = r + d;
	qh = qh - 1;
    }

    if ( r < 0 )
    {
	r = r + d;
	qh = qh - 1;
    }

    /* Compute tentative 16 lower bits of the quotient */
    n = (UWord32) r;
    ql = n/dh;

    /* Limit the quotient to avoid overflow */
    ql = (ql > 0x0000ffff) ? 0x0000ffff:ql;

    /* Compute the second remainder */
    r1 = ql*dh;
    r2 = ql*dl;
    r1 = n-r1;
    r = ((Word64) r1)<<16;
    r = r - r2;

    /* Make appropriate correction */
    if ( r < 0 )
    {
	ql = ql - 1;
	r = r + d;
    }
    if ( r < 0 )
    {
	ql = ql - 1;
	r = r + d;
    }
    if ( r < 0 )
    {
	ql = ql - 1;
	r = r + d;
    }
    if ( r < 0 )
    {
	ql = ql - 1;
	r = r + d;
    }

    /* Combine the higer and lower bits of the quotient */
    q = (Word32) ((qh<<16) | ql);

    /* Correct sign */
    q = (sign < 0) ? -q:q;

    return INT32TOF32(q);
}

/*------------------------------------------------------------------------*//*!

@brief        Fractional division of a 32-bit by a 16-bit numbers. The first
              32-bit argument is the numerator and the second 16-bit argument
              is the denominator.

@return       31 most significant bits of the fractional division of the first
              by the second argument. The extra bits beyond the 31-bit boundary
              are discarded.

@note         Overflow is not detected. The first argument must be lower
              in magnitude than the second argument, otherwise the result
              is undefined.

              If the return value cannot be represented by the 32-bit
              fractional format due to finite resolution, the return value
              is rounded toward zero.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32DivF32F16(register Frac32 x, register Frac16 y)
{
    register UWord32 n;
    register UWord32 d;
    register Word32 r;
    register Word32 q;
    register UWord32 qh;
    register UWord32 ql;
    register Word32 sign;

    n = (UWord32) ((x < 0) ? -F32TOINT32(x):F32TOINT32(x));
    d = (UWord32) ((y < 0) ? -F16TOINT32(y):F16TOINT32(y));
    sign = x^(y<<16);
    qh = n/d;
    r = (Word32) (n - qh*d);
    r <<= 16;
    ql = r/d;
    ql >>= 1;
    qh <<=15;
    q = (Word32) (qh | ql);
    q = (sign < 0) ? -q:q;

    return INT32TOF32(q);
}

/*------------------------------------------------------------------------*//*!

@brief        Right, bidirectional shift of 32-bit fractional value (x) by
              the specified shift amount (s).

@return       32-bit fractional value shifted by the shift amount, right is
              the positive direction. The bits beyond the 32-bit boundary
              of the results are discarded.

@note         Overflow is not detected.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -32...32.
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Shr(register Frac32 x, register Word32 s)
{
    return (s<0) ? x<<(-s):x>>s;
}

/*------------------------------------------------------------------------*//*!

@brief        Right, bidirectional shift of 32-bit fractional value with
              overflow control.

@return       32-bit fractional value shifted by the shift amount, right is
              the positive direction. The bits beyond the 32-bit boundary
              are discarded.

@note         Overflow is detected and the function saturates if the return
              value cannot fit into the return type.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -32...32
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32ShrSat(register Frac32 x, register Word32 s)
{
    register Word32 y;
    register Word32 xmax;
    register Word32 xmin;

    if(s < 0)
    {
        s = -s;
        xmax = INT32_MAX >> s;
        xmin = INT32_MIN >> s;

        y = x << s;
        y = (x > xmax) ? INT32_MAX:y;
        y = (x <= xmin) ? INT32_MIN:y;
    }
    else
    {
        y = x >> s;
    }

    return INT32TOF32(y);
}

/*------------------------------------------------------------------------*//*!

@brief        Left, bidirectional shift of 32-bit fractional value.

@return       32-bit fractional value shifted by the shift amount, left is
              the positive direction. The bits beyond the 32-bit boundary
              are discarded.

@note         Overflow is not detected.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -32...32.
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Shl(register Frac32 x, register Word32 s)
{
    return (s<0) ? x>>(-s):x<<s;
}

/*------------------------------------------------------------------------*//*!

@brief        Left, bidirectional shift of 32-bit fractional value with
              overflow control.

@return       32-bit fractional value shifted by the shift amount, left is
              the positive direction. The bits beyond the 32-bit boundary
              are discarded.

@note         Overflow is detected. The function returns a saturated
              fractional value if the return value cannot fit into the return
              type.

              The shift amount cannot exceed in magnitude the bit-width of
              the shift value, that means must be within the range -32...32.
              Otherwise the result of the function is undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32ShlSat(register Frac32 x, register Word32 s)
{
    register Word32 y;
    register Word32 xmax;
    register Word32 xmin;

    if(s > 0)
    {
        xmax = INT32_MAX >> s;
        xmin = INT32_MIN >> s;

        y = x << s;
        y = (x > xmax) ? INT32_MAX:y;
        y = (x <= xmin) ? INT32_MIN:y;
    }
    else
    {
        y = x >> (-s);
    }

    return INT32TOF32(y);
}

/*------------------------------------------------------------------------*//*!

@brief        Right, unidirectional shift of 32-bit fractional value (x) by
              the specified shift amount (s).

@return       32-bit fractional value shifted right by the shift amount.
              The bits beyond the 32-bit boundary of the result are discarded.

@note         This is the unidirectional right shift.

              The shift amount cannot exceed in magnitude the bit-width of
              the shifted value, that means it must be within the range 0...31.
              Otherwise the result of the function is undefined.

@warning      This function is target dependent.  The shift result for the
              negative value to be shifted may be undefined.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Frac32
F32Shruni(register Frac32 x, register UWord32 s)
{
    return x>>s;
}

/*------------------------------------------------------------------------*//*!

@brief        Counts the number of left shift needed for normalization of
              the input 32-bit fractional value

@return       The number of left shift needed to normalize the argument.
              For the input 0 returns 0.
-----------------------------------------------------------------------------*/
__STATIC_INLINE Word32
F32Norm(register Frac32 x)
{
    register Word32 sham;
    register Word32 shamd;
    register Word32 shamp;
    register Word32 shamn;
    register Word32 shcorr;
    register Word32 b;
    register Word32 one;

    if(x == 0)
    {
        return 0;
    }

    if(x == 0x80000000)
    {
        return 0;
    }

    shcorr = (x<0) ? 1:0;

    x = (x<0) ? (-x):x;

    one = 1;
    sham = 15;
    shamp = 8;
    shamn = -8;
    do
    {
         b = one<<sham;
         shamd = (x>= b) ? shamp:shamn;
         sham += shamd;
         shamp >>= 1;
         shamn >>= 1;
    } while(shamp > 0);

    shamn = -1;
    b = one<<sham;
    shamd = ( x>= b) ? shamp:shamn;
    sham += shamd;

    sham = 30 - sham + shcorr;

    return sham;
}

/* NOTE:
 *   The next two functions are commented out because lcc (matlab compiler
 *   is unable to compile them).  See also the note at the start of the
 *   file.
 */

#endif /* _SWLIBS_INLINES_H_ */
