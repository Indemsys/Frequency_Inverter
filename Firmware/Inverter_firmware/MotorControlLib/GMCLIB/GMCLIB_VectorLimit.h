/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file      GMCLIB_VectorLimit.h
* 
* @author    r29302
* 
* @version   1.0.1.0
* 
* @date      Dec-6-2012
* 
* @brief     Header file for the Vector-Limit function.
*
*******************************************************************************
*
* Function impemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GMCLIB_VectorLimit_H_
#define _GMCLIB_VectorLimit_H_

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
 * \def GMCLIB_VectorLimit(f32In, f32Out, pParam)
 * Function alias for the #GMCLIB_VectorLimitANSIC function.
 */

#define GMCLIB_VectorLimit(w32Out, w32In, pParam) \
            GMCLIB_VectorLimitANSIC((w32Out), (w32In), (pParam))
                            

/************************************************************************
| typedefs and structures (scope: module-local)
|-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!
@struct     GMCLIB_VectorLimit_T "\GMCLIB_VectorLimit.h"

@brief      Structure containing the limits.

*//*-------------------------------------------------------------------------*/

typedef struct   
{
  Frac32 f32Lim;    /*!<The maximum magnitude of the input vector.  The
                         defined magnitude must be positive and equal to or
                         greater than \f$2^-15\f$.*/
} GMCLIB_VectorLimit_T;

/************************************************************************
| exported variables
|-----------------------------------------------------------------------*/

/************************************************************************
| exported function prototypes
|-----------------------------------------------------------------------*/
/*! 
* @ingroup GFLIB_GROUP 
*/

extern bool GMCLIB_VectorLimitANSIC(MCLIB_2_COOR_SYST_D_Q_T *const pOut,
                                    const MCLIB_2_COOR_SYST_D_Q_T *const pIn,
                                    const GMCLIB_VectorLimit_T *const pParam);

#ifdef __cplusplus
}
#endif

#endif /* _GMCLIB_VectorLimit_H_ */

/* End of file */
