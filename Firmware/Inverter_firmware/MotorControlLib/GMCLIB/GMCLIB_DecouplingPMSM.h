/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GMCLIB_DecouplingPMSM.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GMCLIB_DecouplingPMSM function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GMCLIB_DECOUPLINGPMSM_H
#define _GMCLIB_DECOUPLINGPMSM_H

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
/*! Function alias for the #GMCLIB_DecouplingPMSMANSIC function.*/
#define GMCLIB_DecouplingPMSM(pUdqDec,pUdq,pIdq,f32AngularVel,pParam)\
        GMCLIB_DecouplingPMSMANSIC(pUdqDec,pUdq,pIdq,f32AngularVel,pParam)

/*! Macro containing reset values of the parameters for the decoupling algorithm implemented
 * in the #GMCLIB_DecouplingPMSM function.*/
#define GMCLIB_DECOUPLINGPMSM_DEFAULT   {0,0,0,0}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct MCLIB_DECOUPLING_PMSM_PARAM_T "\GMCLIB_DecouplingPMSM.h"

@brief  Structure containing coefficients for calculation of the decoupling algorithm
        implemented in the #GMCLIB_DecouplingPMSM function.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32 f32Kd;          /*!< Coefficient \f$k_{df}\f$, in fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ */
    Word16    w16KdShift;      /*!< Scaling coefficient \f$k_{d\_shift}\f$, integer format \f$\left[-31,31\right]\f$ */
    Frac32 f32Kq;          /*!< Coefficient \f$k_{qf}\f$, in fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ */
    Word16    w16KqShift;      /*!< Scaling coefficient \f$k_{q\_shift}\f$, integer format \f$\left[-31,31\right]\f$ */
}MCLIB_DECOUPLING_PMSM_PARAM_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*! 
* @ingroup GMCLIB_GROUP 
*/ 
extern void GMCLIB_DecouplingPMSMANSIC(MCLIB_2_COOR_SYST_D_Q_T *pUdqDec,
										const MCLIB_2_COOR_SYST_D_Q_T *const pUdq,
										const MCLIB_2_COOR_SYST_D_Q_T *const pIdq,
										Frac32      f32AngularVel,
										const MCLIB_DECOUPLING_PMSM_PARAM_T *const pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GMCLIB_DECOUPLINGPMSM_H */

