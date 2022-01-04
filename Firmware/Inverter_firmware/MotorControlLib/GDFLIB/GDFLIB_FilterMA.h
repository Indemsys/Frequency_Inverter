/******************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file     GDFLIB_FilterMA.h
*
* @author   Roman Filka
*
* @version  1.0.1.0
*
* @date     Dec-6-2012
*
* @brief    Header file for #GDFLIB_FilterMA function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90. 
*
******************************************************************************/
#ifndef _GDFLIB_FILTER_MA_H
#define _GDFLIB_FILTER_MA_H

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
/*! Function alias for the #GDFLIB_FilterMAInitANSIC function.*/
#define GDFLIB_FilterMAInit(pParam)\
        GDFLIB_FilterMAInitANSIC(pParam)

/*! Function alias for the #GDFLIB_FilterMAANSIC function.*/
#define GDFLIB_FilterMA(f32In, pParam)\
        GDFLIB_FilterMAANSIC(f32In, pParam)

/*! Macro containing default values of the first order MA filter structure.*/
#define GDFLIB_FILTER_MA_DEFAULT   {0,0}

/******************************************************************************
| Typedefs and structures       (scope: module-local)
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*//*!
@struct GDFLIB_FILTER_MA_T "\GDFLIB_FilterMA.h"

@brief  Structure containing filter buffer and coefficients.
*//*-------------------------------------------------------------------------*/
typedef struct
{
    Frac32 f32Acc;             /*!< filter accumulator */
    UWord16    u16NSamples;        /*!< number of samples for averaging, filter sample window [0,31]*/
}GDFLIB_FILTER_MA_T;

/******************************************************************************
| Exported Variables
-----------------------------------------------------------------------------*/

/******************************************************************************
| Exported function prototypes
-----------------------------------------------------------------------------*/
// Prototype of function for clearing buffers of IIR1 filter.
extern void GDFLIB_FilterMAInitANSIC(GDFLIB_FILTER_MA_T *pParam);

// Prototype of direct form II first order IIR filter.
extern Frac32 GDFLIB_FilterMAANSIC(Frac32 f32In, \
                                    GDFLIB_FILTER_MA_T *pParam);

/******************************************************************************
| Inline functions
-----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _GDFLIB_FILTER_IIR1_H */

