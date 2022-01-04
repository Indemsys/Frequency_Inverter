/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_IntegratorTR.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GFLIB_IntegratorTR function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GFLIB_INTEGRATOR_TR_H
#define _GFLIB_INTEGRATOR_TR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Inlines.h"
#include "SWLIBS_Defines.h"
#endif
     
/******************************************************************************
| Defines and macros            (scope: module-local)
-----------------------------------------------------------------------------*/
/*! Function alias for the #GFLIB_IntegratorTRANSIC function.*/
#define GFLIB_IntegratorTR(in, pParam)  GFLIB_IntegratorTRANSIC(in, pParam)

/*! Macro containing default values of the integrator structure.*/
#define GFLIB_INTEGRATOR_TR_DEFAULT   {0,0,0,0}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_INTEGRATOR_TR_T "\GFLIB_IntegratorTR.h"

@brief  Structure containing integrator parameters and coefficients.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32 f32State;       /*!< integrator state value */
    Frac32 f32InK1;        /*!< input value in step k-1 */
    Frac32 f32C1;          /*!< integrator coefficient = \f$\frac{E_{MAX}\cdot T_s}{U_{MAX}\cdot 2} \cdot 2^{-u16NShift} \f$ */
    UWord16    u16NShift;      /*!< Scaling factor for the integrator coefficient f32C1, integer format \f$\left[0,31\right]\f$ */
}GFLIB_INTEGRATOR_TR_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*! 
* @ingroup GFLIB_GROUP 
*/

extern Frac32 GFLIB_IntegratorTRANSIC(Frac32 f32In, GFLIB_INTEGRATOR_TR_T *pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GFLIB_INTEGRATOR_TR_H */

