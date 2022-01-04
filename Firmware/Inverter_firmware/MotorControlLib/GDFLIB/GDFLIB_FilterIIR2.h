/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GDFLIB_FilterIIR2.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GDFLIB_FilterIIR2 function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GDFLIB_FILTER_IIR2_H
#define _GDFLIB_FILTER_IIR2_H

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
/*! Function alias for the #GDFLIB_FilterIIR2InitANSIC function.*/
#define GDFLIB_FilterIIR2Init(pParam) \
    GDFLIB_FilterIIR2InitANSIC(pParam)

/*! Function alias for the #GDFLIB_FilterIIR2ANSIC function.*/
#define GDFLIB_FilterIIR2(f32In,pParam)\
        GDFLIB_FilterIIR2ANSIC(f32In,pParam)

/*! Macro containing default values of the second order IIR filter structure.*/
#define GDFLIB_FILTER_IIR2_DEFAULT   {{0,0,0,0,0},{0,0},{0,0}}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct FILTER_IIR2_COEFF_T "\GDFLIB_FilterIIR2.h"

@brief  Sub-structure containing filter coefficients.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32 f32B0;  /*!< b0 coefficient of an IIR2 filter, 32-bit */
    Frac32 f32B1;  /*!< b1 coefficient of an IIR2 filter, 32-bit */
    Frac32 f32B2;  /*!< b2 coefficient of an IIR2 filter, 32-bit */
    Frac32 f32A1;  /*!< a1 coefficient of an IIR2 filter, 32-bit */
    Frac32 f32A2;  /*!< a2 coefficient of an IIR2 filter, 32-bit */
}FILTER_IIR2_COEFF_T;

/*------------------------------------------------------------------------*//*!
@struct GDFLIB_FILTER_IIR2_T "\GDFLIB_FilterIIR2.h"

@brief  Structure containing filter buffer and coefficients.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    FILTER_IIR2_COEFF_T trFiltCoeff;  /*!< filter coefficients substructure */
    Frac32 f32FiltBufferX[3];        /*!< input buffer of an IIR2 filter */
    Frac32 f32FiltBufferY[3];        /*!< internal accumulator buffer */
}GDFLIB_FILTER_IIR2_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
/*! 
* @ingroup GDFLIB_GROUP 
*/

/*! @brief  Prototype of function for clearing buffers of IIR2 filter.*/
extern void GDFLIB_FilterIIR2InitANSIC(GDFLIB_FILTER_IIR2_T *pParam);

/*! @brief  Prototype of direct form II second order IIR filter.*/
extern Frac32 GDFLIB_FilterIIR2ANSIC(Frac32 f32InErr, \
                                      GDFLIB_FILTER_IIR2_T *pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GDFLIB_FILTER_IIR2_H */

