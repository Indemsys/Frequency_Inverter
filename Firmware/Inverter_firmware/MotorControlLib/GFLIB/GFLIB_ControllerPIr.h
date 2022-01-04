/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_ControllerPIr.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GFLIB_ControllerPIr function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GFLIB_CONTROLLER_PIR_H
#define _GFLIB_CONTROLLER_PIR_H

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
/*! Function alias for the #GFLIB_ControllerPIrANSIC function.*/
#define GFLIB_ControllerPIr(f32InErr,pParam)\
        GFLIB_ControllerPIrANSIC(f32InErr, pParam)

/*! Macro containing default values of the recurrent form PI controller structure.*/
#define GFLIB_CONTROLLER_PI_R_DEFAULT   {0,0,0,0,0}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_CONTROLLER_PI_R_T "\GFLIB_ControllerPIr.h"

@brief  Structure containing parameters and states of the recurrent form PI controller.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32 f32CC1sc;   /*!< CC1 coefficient, fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ */
    Frac32 f32CC2sc;   /*!< CC2 coefficient, fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ */
    Frac32 f32Acc;     /*!< Internal controller accumulator */
    Frac32 f32InErrK1; /*!< Controller input from the previous calculation step */
    UWord16    u16NShift;  /*!< Scaling factor for controller coefficients, integer format \f$\left[0,31\right]\f$ */
}GFLIB_CONTROLLER_PI_R_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*!
* @ingroup GFLIB_GROUP
*/

extern Frac32 GFLIB_ControllerPIrANSIC(Frac32 f32InErr,
                                        GFLIB_CONTROLLER_PI_R_T *pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GFLIB_CONTROLLER_PIR_H */

