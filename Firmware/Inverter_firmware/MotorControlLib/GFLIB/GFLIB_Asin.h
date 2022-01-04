/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Asin.h
*
* @author   B04459
* 
* @version  1.0.1.0
* 
* @date     Dec-6-2012
* 
* @brief    Header file for #GFLIB_Asin function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef GFLIB_ASIN_H_
#define GFLIB_ASIN_H_

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif
/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
/*!
  \def GFLIB_Asin
  Function alias.
*/
#define GFLIB_Asin(x) GFLIB_AsinANSIC((x), &gflibAsinCoef)

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/

/*------------------------------------------------------------------------*//*!
@struct GFLIB_ASIN_TAYLOR_COEF_T "\GFLIB_Asin.h"

@brief      Structure containing five polynomial coefficients for one sub-interval.

@details    Output of \f$\arcsin\left(f32In\right)\f$ for
            interval \f$\left[0,1\right)\f$ of the input ratio is divided
            into two sub-sectors. Polynomial approximation is done using a 5th
            order polynomial, for each sub-sector respectively. Five coefficients
            for a single sub-interval are stored in this (#GFLIB_ASIN_TAYLOR_COEF_T)
            structure.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    const Frac32  f32a[5];  /*!< Array of five 32-bit elements for storing
                                  coefficients of the piece-wise polynomial. */
}GFLIB_ASIN_TAYLOR_COEF_T;

/*------------------------------------------------------------------------*//*!
@struct GFLIB_ASIN_TAYLOR_T "\GFLIB_Asin.h"

@brief      Structure containing two sub-structures with polynomial coefficients
            to cover all sub-intervals.

@details    Output of \f$\arcsin\left(f32In\right)\f$ for
            interval \f$\left[0,1\right)\f$ of the input ratio is divided
            into two sub-sectors. Polynomial approximation is done using a 5th
            order polynomial, for each sub-sector respectively. Two arrays,
            each including five polynomial coefficients for each sub-interval,
            are stored in this (#GFLIB_ASIN_TAYLOR_T) structure.

            \par
            By calling the function alias #GFLIB_Asin, default values of
            the coefficients are used. Polynomial coefficients can be modified by
            the user and in such a case the full function call shall be used, i.e.
            #GFLIB_AsinANSIC
*//*-------------------------------------------------------------------------*/

typedef struct
{
    const GFLIB_ASIN_TAYLOR_COEF_T  GFLIB_ASIN_SECTOR[2]; /*!< Array of two
                                    elements for storing eight sub-arrays (each
                                    sub-array contains four 32-bit
                                    coefficients) for all sub-intervals.*/
} GFLIB_ASIN_TAYLOR_T;

/******************************************************************************
* Exported Variables
******************************************************************************/

extern GFLIB_ASIN_TAYLOR_T gflibAsinCoef;

/******************************************************************************
* Exported function prototypes
******************************************************************************/
/*! 
* @ingroup GFLIB_GROUP 
*/ 
extern Frac32 GFLIB_AsinANSIC(Frac32 f32In, const GFLIB_ASIN_TAYLOR_T *const pParam);

/******************************************************************************
* Inline functions
******************************************************************************/ 

#endif /*GFLIB_ASIN_H_*/
