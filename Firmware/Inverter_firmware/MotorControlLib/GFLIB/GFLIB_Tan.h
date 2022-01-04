/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Tan.h
*
* @author   B04459
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GFLIB_Tan function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef GFLIB_TAN_H_
#define GFLIB_TAN_H_

#ifndef  _MATLAB_BAM_CREATE
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
/*! Function alias for the #GFLIB_TanANSIC function.*/
#define GFLIB_Tan(f32In) GFLIB_TanANSIC(f32In, &gflibTanCoef)

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_TAN_TAYLOR_COEF_T "\GFLIB_Tan.h"

@brief      Structure containing four polynomial coefficients for one sub-interval.

@details    Output of \f$\tan\left(\pi\cdot \mbox{f32In}\right)\f$ for
            interval \f$\left[0,\frac{\pi}{4}\right)\f$ of the input angles is divided
            into eight sub-sectors. Polynomial approximation is done using a 4th
            order polynomial, for each sub-sector respectively. Four coefficients
            for a single sub-interval are stored in this (#GFLIB_TAN_TAYLOR_COEF_T)
            structure.
*//*-------------------------------------------------------------------------*/
typedef struct
{
	Frac32  f32A[4];          /*!< Array of four 32-bit elements for storing the polynomial
                                    coefficients for one sub-interval. */
}GFLIB_TAN_TAYLOR_COEF_T;

/*------------------------------------------------------------------------*//*!
@struct GFLIB_TANTLR_T "\GFLIB_Tan.h"

@brief      Structure containing eight sub-structures with polynomial coefficients
            to cover all sub-intervals.

@details    Output of \f$\tan\left(\pi\cdot \mbox{f32In}\right)\f$ for
            interval \f$\left[0,\frac{\pi}{4}\right)\f$ of the input angles is divided
            into eight sub-sectors. Polynomial approximation is done using a 4th
            order polynomial, for each sub-sector respectively. Eight arrays,
            each including four polynomial coefficients for each sub-interval,
            are stored in this (#GFLIB_TANTLR_T) structure.

            \par
            By calling the function alias #GFLIB_Tan, default values of the coefficients
            are used. Polynomial coefficients can be modified by the user
            and in such a case the full function call shall be used, i.e. #GFLIB_TanANSIC.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    GFLIB_TAN_TAYLOR_COEF_T  GFLIB_TAN_SECTOR[8]; /*!< Array of eight elements for
                                                       storing eight sub-arrays
                                                       (each sub-array contains four
                                                       32-bit coefficients)
                                                       for all sub-intervals.*/
}GFLIB_TANTLR_T;

/******************************************************************************
* Exported Variables
******************************************************************************/
extern const GFLIB_TANTLR_T gflibTanCoef;

/******************************************************************************
* Exported function prototypes
******************************************************************************/
/*!
* @ingroup GFLIB_GROUP
*/
extern Frac32 GFLIB_TanANSIC(Frac32 f32In, const GFLIB_TANTLR_T * const pParam);

/******************************************************************************
* Inline functions
******************************************************************************/

#endif /*GFLIB_TAN_H_*/
