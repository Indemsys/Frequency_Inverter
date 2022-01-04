/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file      GFLIB_Limit.h
*
* @author    R29302
* 
* @version   1.0.1.0
* 
* @date      Dec-6-2012
* 
* @brief     Header file for two side limit function.
*
*******************************************************************************
*
* Function impemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GFLIB_LIMIT_H_
#define _GFLIB_LIMIT_H_

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
| defines and macros (scope: module-local)
|-----------------------------------------------------------------------*/

/*! 
 * \def GFLIB_Limit(f32In, pParam)
 * Function alias for the #GFLIB_LimitANSIC function.
 */

#define GFLIB_Limit(f32In, pParam) GFLIB_LimitANSIC((f32In), (pParam))

/************************************************************************
| typedefs and structures (scope: module-local)
|-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!
@struct     GFLIB_LIMIT_T "\GFLIB_Limit.h"

@brief      Structure containing the limits.

*//*-------------------------------------------------------------------------*/

typedef struct {
    Frac32 f32LowerLimit; /*!< Lower limit. */
    Frac32 f32UpperLimit; /*!< Upper limit. */
} GFLIB_LIMIT_T;

/************************************************************************
| exported variables
|-----------------------------------------------------------------------*/

/************************************************************************
| exported function prototypes
|-----------------------------------------------------------------------*/

/*! 
* @ingroup GFLIB_GROUP 
*/ 
extern Frac32 GFLIB_LimitANSIC (Frac32 f32In,
                                 const GFLIB_LIMIT_T *const pParam);

#ifdef __cplusplus
}
#endif

#endif /* _GFLIB_LIMIT_H_ */

/* End of file */
