/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file      GFLIB_LowerLimit.h
* 
* @author    R29302
* 
* @version   1.0.1.0
* 
* @date      Dec-6-2012
* 
* @brief     Header file for the lower limit function.
*
*******************************************************************************
*
* Function impemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GFLIB_LOWERLIMIT_H_
#define _GFLIB_LOWERLIMIT_H_

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
 * \def GFLIB_LowerLimit(f32In, pParam)
 * Function alias for the #GFLIB_LowerLimitANSIC function.
 */
#define GFLIB_LowerLimit(f32In, pParam) GFLIB_LowerLimitANSIC((f32In), (pParam))

/************************************************************************
| typedefs and structures (scope: module-local)
|-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!
@struct		GFLIB_LOWERLIMIT_T "\GFLIB_LowerLimitANSIC.h"

@brief		Structure containing the lower limit.

*//*-------------------------------------------------------------------------*/

typedef struct  
{    
  Frac32 f32LowerLimit;  /*!<Lower limit. */
} GFLIB_LOWERLIMIT_T;

/************************************************************************
| exported variables
|-----------------------------------------------------------------------*/

/************************************************************************
| exported function prototypes
|-----------------------------------------------------------------------*/

/*! 
* @ingroup GFLIB_GROUP 
*/
extern Frac32 GFLIB_LowerLimitANSIC(Frac32 f32In,
                                     const GFLIB_LOWERLIMIT_T *const pParam);

#ifdef __cplusplus
}
#endif
#endif /* _GFLIB_LOWERLIMIT_H_ */

/* End of file */
