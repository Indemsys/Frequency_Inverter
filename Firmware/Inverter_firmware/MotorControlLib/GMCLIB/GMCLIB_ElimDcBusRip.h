/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GMCLIB_ElimDcBusRip.h
*
* @author   B04459, R29302
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GMCLIB_ElimDcBusRip function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GMCLIB_ELIMDCBUSRIP_H
#define _GMCLIB_ELIMDCBUSRIP_H

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
/*! Function alias for the #GMCLIB_ElimDcBusRipANSIC function.*/
#define GMCLIB_ElimDcBusRip(pOut, pIn, pParams) GMCLIB_ElimDcBusRipANSIC(pOut, pIn, pParams)

/*! Macro containing default values for the parameter structure of the #GMCLIB_ElimDcBusRip
 * function.*/
#define GMCLIB_ELIMDCBUSRIP_DEFAULT   {0,0}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!
@struct GMCLIB_ELIM_DC_BUS_RIP_T "\GMCLIB_ElimDcBusRip.h"

@brief  Structure containing the PWM modulation index and the measured value of
        the DC bus voltage for calculation of the DC bus ripple elimination.

*//*-------------------------------------------------------------------------*/

typedef struct
{
    Frac32 f32ModIndex;    /*!< Inverse Modulation Index, fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$, must be 0 or positive. */
    Frac32 f32ArgDcBusMsr; /*!< Measured DC bus voltage, fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$, must be 0 or positive. */
} GMCLIB_ELIM_DC_BUS_RIP_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*!
* @ingroup GMCLIB_GROUP
*/
extern void GMCLIB_ElimDcBusRipANSIC(MCLIB_2_COOR_SYST_ALPHA_BETA_T *pOut,
                                     const MCLIB_2_COOR_SYST_ALPHA_BETA_T *const pIn,
                                     const GMCLIB_ELIM_DC_BUS_RIP_T *const pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GMCLIB_ELIMDCBUSRIP_H */

