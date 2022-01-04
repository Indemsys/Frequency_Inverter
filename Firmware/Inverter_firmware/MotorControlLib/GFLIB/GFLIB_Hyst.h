/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Hyst.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GFLIB_Hyst function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GFLIB_HYST_H
#define _GFLIB_HYST_H

#ifndef  _MATLAB_BAM_CREATE
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
/*! Function alias for the #GFLIB_HystANSIC function.*/
#define GFLIB_Hyst(f32In, pParam) GFLIB_HystANSIC(f32In, pParam)

/*! Macro containing default values of the hysteresis function structure.*/
#define GFLIB_HYST_DEFAULT   {0,0,0,0,0}

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_HYST_T "\GFLIB_Hyst.h"

@brief      Structure containing parameters and states for the hysteresis function
            implemented in #GFLIB_Hyst.

@details
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32  f32HystOn;     /*!< Value determining the upper threshold; fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ . */
    Frac32  f32HystOff;    /*!< Value determining the lower threshold; fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ . */
    Frac32  f32OutValOn;   /*!< Value of the output when input is higher than the upper threshold; fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ . */
    Frac32  f32OutValOff;  /*!< Value of the output when input is the lower than lower threshold; fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ . */
    Frac32  f32OutState;   /*!< Actual state of the output; fractional format normalized to fit into \f$\left[-2^{31},2^{31}-1\right)\f$ . */
}GFLIB_HYST_T;

/******************************************************************************
* Exported Variables
******************************************************************************/

/******************************************************************************
* Exported function prototypes
******************************************************************************/
/*!
* @ingroup GFLIB_GROUP
*/
extern Frac32 GFLIB_HystANSIC(Frac32 f32In, GFLIB_HYST_T *pParam);

/******************************************************************************
* Inline functions
******************************************************************************/

#endif /* _GFLIB_HYST_H */
