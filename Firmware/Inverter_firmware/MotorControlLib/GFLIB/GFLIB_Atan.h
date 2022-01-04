/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Atan.h
*
* @author   B04459
* 
* @version  1.0.1.0
* 
* @date     Dec-6-2012
* 
* @brief    Header file for #GFLIB_Atan function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef GFLIB_ATAN_H_
#define GFLIB_ATAN_H_

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif
/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
/*!
  \def GFLIB_Atan
  Function alias.
*/
#define GFLIB_Atan(x) GFLIB_AtanANSIC((x), &gflibAtanCoef)

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/

/*------------------------------------------------------------------------*//*!
@struct GFLIB_ATAN_TAYLOR_COEF_T "\GFLIB_Atan.h"

@brief      Structure containing four polynomial coefficients for one sub-interval.

@details    Output of \f$\arctan\left(f32In\right)\f$ for
            interval \f$\left[0,1\right)\f$ of the input ratio is divided
            into eight sub-sectors. Polynomial approximation is done using a 4th
            order polynomial, for each sub-sector respectively. Four coefficients
            for a single sub-interval are stored in this (#GFLIB_ATAN_TAYLOR_COEF_T)
            structure.
*//*-------------------------------------------------------------------------*/
typedef struct
{
	const Frac32  f32a[4];  /*!< Array of five 32-bit elements for storing
                            coefficients of the piece-wise polynomial. */
}GFLIB_ATAN_TAYLOR_COEF_T;

/*------------------------------------------------------------------------*//*!
@struct GFLIB_ATAN_TAYLOR_T "\GFLIB_Atan.h"

@brief      Structure containing eight sub-structures with polynomial
            coefficients to cover all sub-intervals.

@details    Output of \f$\arctan\left(f32In\right)\f$ for
            interval \f$\left[0,1\right)\f$ of the input ratio is divided
            into eight sub-sectors. Polynomial approximation is done using a 4th
            order polynomial, for each sub-sector respectively. Eight arrays,
            each including four polynomial coefficients for each sub-interval,
            are stored in this (#GFLIB_ATAN_TAYLOR_COEF_T) structure.

            \par
            By the calling function alias #GFLIB_Atan, default values of the
            coefficients are used. Polynomial coefficients can be modified by
            the user and in such a case the full function call shall be used, i.e.
            #GFLIB_AtanANSIC
*//*-------------------------------------------------------------------------*/

typedef struct
{
    const GFLIB_ATAN_TAYLOR_COEF_T  GFLIB_ATAN_SECTOR[8]; /*!< Array of eight
                                elements for storing eight sub-arrays (each
                                sub-array contains four 32-bit coefficients)
                                for all sub-intervals.*/
} GFLIB_ATAN_TAYLOR_T;

/******************************************************************************
* Exported Variables
******************************************************************************/

extern const GFLIB_ATAN_TAYLOR_T gflibAtanCoef;

/******************************************************************************
* Exported function prototypes
******************************************************************************/
/*! 
* @ingroup GFLIB_GROUP 
*/ 
extern Frac32 GFLIB_AtanANSIC(Frac32 f32In, const GFLIB_ATAN_TAYLOR_T *const pParam);

/******************************************************************************
* Inline functions
******************************************************************************/ 

#endif /*GFLIB_ATAN_H_*/
