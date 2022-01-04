/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GMCLIB_SvmStd.h
*
* @author   B04459
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GMCLIB_SvmStd function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GMCLIB_SVMSTD_H
#define _GMCLIB_SVMSTD_H

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
| Defines and macros            (scope: module-local)
-----------------------------------------------------------------------------*/
/*! Function alias for the #GMCLIB_SvmStdANSIC function.*/
#define GMCLIB_SvmStd(pOutput,pInput)     GMCLIB_SvmStdANSIC(pOutput,pInput)


/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*! 
* @ingroup GMCLIB_GROUP 
*/ 
extern UWord32 GMCLIB_SvmStdANSIC(MCLIB_3_COOR_SYST_T *pOut,
							   const MCLIB_2_COOR_SYST_ALPHA_BETA_T *const pIn);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GMCLIB_SVMSTD_H */

