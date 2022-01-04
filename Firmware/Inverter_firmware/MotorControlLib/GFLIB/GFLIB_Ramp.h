/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Ramp.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GFLIB_Ramp function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GFLIB_RAMP_H
#define _GFLIB_RAMP_H

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
/*! Function alias for the #GFLIB_RampANSIC function.*/
#define GFLIB_Ramp(in, pParam)  GFLIB_RampANSIC(in, pParam)

/*! Macro containing default values of the Ramp structure.*/
#define GFLIB_RAMP_DEFAULT   {0,0,0}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_RAMP_T "\GFLIB_Ramp.h"

@brief  Structure containing controller parameters and coefficients.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32 f32State;       /*!< ramp state value */
    Frac32 f32RampUp;      /*!< ramp up increment coefficient */
    Frac32 f32RampDown;    /*!< ramp down increment(decrement) coefficient */
}GFLIB_RAMP_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*! 
* @ingroup GFLIB_GROUP 
*/

extern Frac32 GFLIB_RampANSIC(Frac32 f32In, GFLIB_RAMP_T *pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GFLIB_RAMP_H */

