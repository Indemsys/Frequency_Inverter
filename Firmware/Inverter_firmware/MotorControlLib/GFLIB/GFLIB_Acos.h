/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Acos.h
*
* @author   B04459
* 
* @version  1.0.1.0
* 
* @date     Dec-6-2012
* 
* @brief    Header file for #GFLIB_Acos function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef GFLIB_ACOS_H_
#define GFLIB_ACOS_H_

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif
/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
/*!
  \def GFLIB_Acos
  Function alias.
*/
#define GFLIB_Acos(x) GFLIB_AcosANSIC((x), &gflibAcosCoef)

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/

/*------------------------------------------------------------------------*//*!
@struct GFLIB_ACOS_TAYLOR_COEF_T "\GFLIB_Acos.h"

@brief      Structure containing five polynomial coefficients for one sub-interval.

@details    Output of \f$\arccos\left(f32In\right)\f$ for
            interval \f$\left[0,1\right)\f$ of the input ratio is divided into two
            sub-sectors. Polynomial approximation is done using a 5th order
            polynomial, for each sub-sector respectively. Five coefficients for
            a single sub-interval are stored in this
            (#GFLIB_ACOS_TAYLOR_COEF_T) structure.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    const Frac32  f32a[5];  /*!< Array of five 32-bit elements for storing
                            coefficients of the piece-wise polynomial. */
} GFLIB_ACOS_TAYLOR_COEF_T;

/*-------------------------------------------Sz-----------------------------*//*!
@struct GFLIB_ACOS_TAYLOR_T "\GFLIB_Acos.h"

@brief      Structure containing two sub-structures with polynomial coefficients
            to cover all sub-intervals.

@details    Output of \f$\arccos\left(f32In\right)\f$ for
            interval \f$\left[0,1\right)\f$ of the input ratio is divided
            into two sub-sectors. Polynomial approximation is done using a 5th
            order polynomial, for each sub-sector respectively. Two arrays,
            each including five polynomial coefficients for each sub-interval,
            are stored in this (#GFLIB_ACOS_TAYLOR_T) structure.

            \par
            By calling the function alias #GFLIB_Acos, default values of the
            coefficients are used. Polynomial coefficients can be modified by
            the user and in such a case the full function call shall be used, i.e.
            #GFLIB_AcosANSIC
*//*-------------------------------------------------------------------------*/

typedef struct
{
    const GFLIB_ACOS_TAYLOR_COEF_T  GFLIB_ACOS_SECTOR[2]; /*!< Array of two
                                elements for storing two sub-arrays (each
                                sub-array contains five 32-bit coefficients)
                                for all sub-intervals.*/
} GFLIB_ACOS_TAYLOR_T;

/******************************************************************************
* Exported Variables
******************************************************************************/

extern const GFLIB_ACOS_TAYLOR_T gflibAcosCoef;

/******************************************************************************
* Exported function prototypes
******************************************************************************/
/*! 
* @ingroup GFLIB_GROUP 
*/ 
extern Frac32 GFLIB_AcosANSIC(Frac32 f32In, const GFLIB_ACOS_TAYLOR_T *const pParam);

/******************************************************************************
* Inline functions
******************************************************************************/ 

#endif /*GFLIB_ACOS_H_*/
