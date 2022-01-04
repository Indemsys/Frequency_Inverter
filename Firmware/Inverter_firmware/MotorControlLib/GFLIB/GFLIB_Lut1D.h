/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_Lut1D.h
*
* @author   r29302
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
* 
* @brief    Header file for the #GFLIB_Lut1D function. 
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GFLIB_LUT1D_H
#define _GFLIB_LUT1D_H

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/

/*! Function alias for the #GFLIB_Lut1DANSIC function.*/
#define GFLIB_Lut1D(x, pParam) GFLIB_Lut1DANSIC((x), (pParam))

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/
/*------------------------------------------------------------------------*//*!
@struct GFLIB_LUT1D_T "\GFLIB_Lut1D.h"

@brief  Structure containing  look-up table parameters.
*//*-------------------------------------------------------------------------*/
typedef struct {
	Word32 w32ShamOffset;   /*!<Shift amount for extracting the fractional offset
                              within an interpolated interval. */
	Word32 w32ShamIntvl;    /*!<Shift amount for extracting the interval index
                              of an interpolated interval. */
    const Frac32 *pf32Table;   /*!<Table holding ordinate values of
                              interpolating intervals. */
} GFLIB_LUT1D_T;

/******************************************************************************
* Exported Variables
******************************************************************************/

/******************************************************************************
* Exported function prototypes
******************************************************************************/

/*! 
* @ingroup GFLIB_GROUP 
*/ 

extern Frac32 GFLIB_Lut1DANSIC(Frac32 f32In,
                        const GFLIB_LUT1D_T *const pParam);

/******************************************************************************
* Inline functions
******************************************************************************/

#endif /* _GFLIB_LUT1D_H */
