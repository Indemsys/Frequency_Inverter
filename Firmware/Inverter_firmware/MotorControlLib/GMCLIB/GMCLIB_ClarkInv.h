/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file      GMCLIB_ClarkInv.h
*
* @author    Roman Filka
*
* @version   1.0.1.0
*
* @date      Dec-6-2012
*
* @brief     Header file for #GMCLIB_ClarkInv transformation algorithm.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GMCLIB_CLARK_INV_H_
#define _GMCLIB_CLARK_INV_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  _MATLAB_BAM_CREATE
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Inlines.h"
#include "SWLIBS_Defines.h"
#endif

/************************************************************************
| defines and macros (scope: module-local)
|-----------------------------------------------------------------------*/
/*! Function alias for the #GMCLIB_ClarkInvANSIC function.*/
#define GMCLIB_ClarkInv(pOut, pIn)    GMCLIB_ClarkInvANSIC(pOut, pIn)

/************************************************************************
| typedefs and structures (scope: module-local)
|-----------------------------------------------------------------------*/

/************************************************************************
| exported variables
|-----------------------------------------------------------------------*/

/************************************************************************
| exported function prototypes
|-----------------------------------------------------------------------*/

/*!
* @ingroup GMCLIB_GROUP
*/
extern void GMCLIB_ClarkInvANSIC (MCLIB_3_COOR_SYST_T *pOut,
                                  const MCLIB_2_COOR_SYST_ALPHA_BETA_T *const pIn);

#ifdef __cplusplus
}
#endif
#endif /* _GMCLIB_CLARK_INV_H_ */

/* End of file */
