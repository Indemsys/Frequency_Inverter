/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file      GMCLIB_Park.h
*
* @author    Roman Filka
*
* @version   1.0.1.0
*
* @date      Dec-6-2012
*
* @brief     Header file for Park Transformation.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GMCLIB_PARK_H_
#define _GMCLIB_PARK_H_

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
/*! Function alias for the #GMCLIB_ParkANSIC function.*/
#define GMCLIB_Park(pOut,pInAngle,pIn) GMCLIB_ParkANSIC(pOut,pInAngle,pIn)

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
extern void GMCLIB_ParkANSIC(MCLIB_2_COOR_SYST_D_Q_T *pOut,
                      const MCLIB_ANGLE_T *const pInAngle,
                      const MCLIB_2_COOR_SYST_ALPHA_BETA_T *const pIn);

#ifdef __cplusplus
}
#endif
#endif /* _GMCLIB_PARK_H_ */

/* End of file */
