/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Sin.h
*
* @author   B04459
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GFLIB_Sin function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GFLIB_SIN_H
#define _GFLIB_SIN_H

#ifndef  _MATLAB_BAM_CREATE
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
/*! Function alias for the #GFLIB_SinANSIC function.*/
#define GFLIB_Sin(w32In) GFLIB_SinANSIC(w32In, &gflibSinCoef)

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_SINTLR_T "\GFLIB_Sin.h"

@brief      Structure containing one array of five 32-bit elements for storing
            coefficients of a Taylor polynomial.

@details    By calling the function alias #GFLIB_Sin, default values of the coefficients
            are used. Polynomial coefficients can be modified by the user
            and in such a case the full function call shall be used, i.e. #GFLIB_SinANSIC.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32  f32A[5];  /*!< Array of five 32-bit elements for storing coefficients
                            of the Taylor polynomial. */
}GFLIB_SINTLR_T;

/******************************************************************************
* Exported Variables
******************************************************************************/
extern const GFLIB_SINTLR_T gflibSinCoef;

/******************************************************************************
* Exported function prototypes
******************************************************************************/
/*!
* @ingroup GFLIB_GROUP
*/
extern Frac32 GFLIB_SinANSIC(Frac32 f32In, const GFLIB_SINTLR_T * const pParam);

/******************************************************************************
* Inline functions
******************************************************************************/

#endif /* _GFLIB_SIN_H */
