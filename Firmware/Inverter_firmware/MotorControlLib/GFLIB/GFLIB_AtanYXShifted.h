/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GFLIB_AtanYXShifted.h
*
* @author   Andrzej Lara
* 
* @version  1.0.1.0
* 
* @date     Dec-6-2012
* 
* @brief    Header file for #GFLIB_AtanYXShifted function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GFLIB_ATANYXSHIFTED_H_
#define _GFLIB_ATANYXSHIFTED_H_

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif
/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
/*!
  \def GFLIB_AtanYXShifted
  Function alias for the #GFLIB_AtanYXShiftedANSIC function.
*/
#define GFLIB_AtanYXShifted(y, x, p) GFLIB_AtanYXShiftedANSIC((y), (x), (p))

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/

/*------------------------------------------------------------------------*//*!
@struct		GFLIB_ATANYXSHIFTED_T "\GFLIB_AtanYXShiftedANSIC.h"

@brief		Structure containing the parameter for the AtanYXShifted
            function.

*//*-------------------------------------------------------------------------*/

typedef struct
{    
    Frac32     f32Ky;  /*!<Multiplication coefficient for the y-signal. */
    Frac32     f32Kx;  /*!<Multiplication coefficient for the x-signal. */
    Word32     w32Ny;  /*!<Scaling coefficient for the y-signal. */
    Word32     w32Nx;  /*!<Scaling coefficient for the x-signal. */
    Frac32     f32ThetaAdj; /*!<Adjusting angle. */
    
} GFLIB_ATANYXSHIFTED_T;

/******************************************************************************
* Exported Variables
******************************************************************************/

/******************************************************************************
* Exported function prototypes
******************************************************************************/
/*! 
* @ingroup GFLIB_GROUP 
*/ 
extern Frac32 GFLIB_AtanYXShiftedANSIC(Frac32 f32InY, Frac32 f32InX,
                const GFLIB_ATANYXSHIFTED_T * const pParam);

/******************************************************************************
* Inline functions
******************************************************************************/ 

#endif /* _GFLIB_ATANYXSHIFTED_H_*/
