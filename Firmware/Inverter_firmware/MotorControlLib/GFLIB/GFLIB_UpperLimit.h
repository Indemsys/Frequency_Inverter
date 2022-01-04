/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file      GFLIB_UpperLimit.h
* 
* @author    r29302 
* 
* @version   1.0.1.0
* 
* @date      Dec-6-2012
* 
* @brief     Header file for the Upper-Limit function
*
*******************************************************************************
*
* Function impemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GFLIB_UPPERLIMIT_H_
#define _GFLIB_UPPERLIMIT_H_

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

/*! Function alias for the #GFLIB_UpperLimitANSIC function.*/
#define GFLIB_UpperLimit(f32In, pParam) GFLIB_UpperLimitANSIC((f32In),(pParam))

/************************************************************************
| typedefs and structures (scope: module-local)
|-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!
@struct		GFLIB_UPPERLIMIT_T "\GFLIB_UpperLimit.h"

@brief		Structure containing the upper limit.
*
*//*-------------------------------------------------------------------------*/

typedef struct  
{    
     Frac32 f32UpperLimit; /*!<Upper Limit. */
} GFLIB_UPPERLIMIT_T;

/************************************************************************
| exported variables
|-----------------------------------------------------------------------*/

/************************************************************************
| exported function prototypes
|-----------------------------------------------------------------------*/
/*! 
* @ingroup GFLIB_GROUP 
*/
extern Frac32 GFLIB_UpperLimitANSIC (Frac32 f32In,
                                      const GFLIB_UPPERLIMIT_T *const pParam);
#ifdef __cplusplus
}
#endif
#endif /* _GFLIB_UPPERLIMIT_H_ */

/* End of file */
