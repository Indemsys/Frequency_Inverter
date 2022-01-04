/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GDFLIB_FilterIIR1.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GDFLIB_FilterIIR1 function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GDFLIB_FILTER_IIR1_H
#define _GDFLIB_FILTER_IIR1_H

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
/*! Function alias for the #GDFLIB_FilterIIR1InitANSIC function.*/
#define GDFLIB_FilterIIR1Init(pParam) \
    GDFLIB_FilterIIR1InitANSIC(pParam)

/*! Function alias for the #GDFLIB_FilterIIR1ANSIC function.*/
#define GDFLIB_FilterIIR1(f32In,pParam)\
        GDFLIB_FilterIIR1ANSIC(f32In,pParam)

/*! Macro containing default values of the first order IIR filter structure.*/
#define GDFLIB_FILTER_IIR1_DEFAULT   {{0,0,0},{0},{0}}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct FILTER_IIR1_COEFF_T "\GDFLIB_FilterIIR1.h"

@brief  Sub-structure containing filter coefficients.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32 f32B0;  /*!< b0 coefficient of an IIR1 filter, 32-bit */
    Frac32 f32B1;  /*!< b1 coefficient of an IIR1 filter, 32-bit */
    Frac32 f32A1;  /*!< a1 coefficient of an IIR1 filter, 32-bit */
}FILTER_IIR1_COEFF_T;

/*------------------------------------------------------------------------*//*!
@struct GDFLIB_FILTER_IIR1_T "\GDFLIB_FilterIIR1.h"

@brief  Structure containing filter buffer and coefficients.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    FILTER_IIR1_COEFF_T trFiltCoeff; /*!< filter coefficients substructure */
    Frac32 f32FiltBufferX[2];  /*!< input buffer of an IIR1 filter */
    Frac32 f32FiltBufferY[2];  /*!< internal accumulator buffer */
}GDFLIB_FILTER_IIR1_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
// Prototype of function for clearing buffers of IIR1 filter.
extern void GDFLIB_FilterIIR1InitANSIC(GDFLIB_FILTER_IIR1_T *pParam);

// Prototype of direct form II first order IIR filter.
extern Frac32 GDFLIB_FilterIIR1ANSIC(Frac32 f32InErr, \
                                      GDFLIB_FILTER_IIR1_T *pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GDFLIB_FILTER_IIR1_H */

