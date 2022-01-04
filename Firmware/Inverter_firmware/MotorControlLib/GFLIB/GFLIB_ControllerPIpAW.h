/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_ControllerPIpAW.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GFLIB_ControllerPIpAW function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GFLIB_CONTROLLER_PIPAW_H
#define _GFLIB_CONTROLLER_PIPAW_H

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
    /*! Function alias for the #GFLIB_ControllerPIpAWANSIC function.*/
#define GFLIB_ControllerPIpAW(f32InErr,pParam)\
                    GFLIB_ControllerPIpAWANSIC(f32InErr, pParam)

/*! Macro containing default values for the structure of parameters of
 *  the parallel PI controller with anti-windup.*/
#define GFLIB_CONTROLLER_PIAW_P_DEFAULT   {0,0,0,0,INT32_MIN,INT32_MAX,0,0,0}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_CONTROLLER_PIAW_P_T "\GFLIB_ControllerPIpAW.h"

@brief  Structure containing parameters and states of the the parallel form PI controller
        with anti-windup.
*//*-------------------------------------------------------------------------*/
typedef struct {
  Frac32   f32PropGain;        /*!< Proportional Gain, fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ */
  Frac32   f32IntegGain;       /*!< Integral Gain, fractional format normalized to fit into  \f$\left[-2^{31},2^{31}-1\right)\f$ */
  Word16   w16PropGainShift;   /*!< Proportional Gain Shift, integer format \f$\left[-31,31\right]\f$ */
  Word16   w16IntegGainShift;  /*!< Integral Gain Shift, integer format \f$\left[-31,31\right]\f$ */
  Frac32   f32LowerLimit;      /*!< Lower Limit of the controller, fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ */
  Frac32   f32UpperLimit;      /*!< Upper Limit of the controller, fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ */
  Frac32   f32IntegPartK_1;    /*!< State variable integral part at step k-1*/
  Frac32   f32InK_1;           /*!< State variable input error at step k-1*/
  UWord16  u16LimitFlag;       /*!< Limitation flag, if set to 1, the controller output has reached either the UpperLimit or LowerLimit*/
} GFLIB_CONTROLLER_PIAW_P_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*!
* @ingroup GFLIB_GROUP
*/
extern Frac32 GFLIB_ControllerPIpAWANSIC(Frac32 f32InErr,
                                          GFLIB_CONTROLLER_PIAW_P_T *pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GFLIB_CONTROLLER_PIPAW_H */

/* End of file */
