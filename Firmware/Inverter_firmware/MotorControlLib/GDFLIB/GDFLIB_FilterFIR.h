/*****************************************************************************
*
* (c) Copyright 2009, Freescale & STMicroelectronics
*
***************************************************************************//*!
*
* @file      GDFLIB_FilterFIR.h
* 
* @author    r29302 
* 
* @version   1.0.1.0
* 
* @date      Dec-6-2012
* 
* @brief     Header file for the filter FIR function.
*
*******************************************************************************
*
* Function implemented as ANSIC ISO/IEC 9899:1990, C90.
*
******************************************************************************/
#ifndef _GDFLIB_FILTERFIR_H_
#define _GDFLIB_FILTERFIR_H_

#ifndef  _MATLAB_BAM_CREATE    
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_Inlines.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
| defines and macros (scope: module-local)
|-----------------------------------------------------------------------*/

/*! Function alias for the #GDFLIB_FilterFIRInitANSIC function.*/
#define GDFLIB_FilterFIRInit(pParam, pState, pInBuf) \
    GDFLIB_FilterFIRInitANSIC((pParam), (pState), (pInBuf))

/*! Function alias for the #GDFLIB_FilterFIRANSIC function.*/
#define GDFLIB_FilterFIR(f32In, pParam, pState) \
    GDFLIB_FilterFIRANSIC((f32In), (pParam), (pState))

/************************************************************************
| typedefs and structures (scope: module-local)
|-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!
@struct     GDFLIB_FILTERFIR_PARAM_T "\GDFLIB_FilterFIR.h"

@brief      Structure containing parameters of the filter.
*
*//*-------------------------------------------------------------------------*/
typedef struct  
{    
     UWord32    u32Order;      /*!<FIR filter order, must be 1 or more. */
     const Frac32 *pf32CoefBuf;  /*!<FIR filter coefficients buffer. */
} GDFLIB_FILTERFIR_PARAM_T;

/*------------------------------------------------------------------------*//*!
@struct     GDFLIB_FILTERFIR_STATE_T "\GDFLIB_FilterFIR.h"

@brief      Structure containing the current state of the filter.
*
*//*-------------------------------------------------------------------------*/
typedef struct
{    
     UWord32 u32Idx;          /*!<Input buffer index. */
     Frac32 *pf32InBuf;   /*!<Pointer to the input buffer. */
} GDFLIB_FILTERFIR_STATE_T;

/************************************************************************
| exported variables
|-----------------------------------------------------------------------*/

/************************************************************************
| exported function prototypes
|-----------------------------------------------------------------------*/
/*! 
* @ingroup GDFLIB_GROUP 
*/

void GDFLIB_FilterFIRInitANSIC(const GDFLIB_FILTERFIR_PARAM_T *const pParam,
                               GDFLIB_FILTERFIR_STATE_T *const pState,
                               Frac32 *pf32InBuf);

/*! 
* @ingroup GDFLIB_GROUP 
*/
Frac32 GDFLIB_FilterFIRANSIC(Frac32 f32In,
                              const GDFLIB_FILTERFIR_PARAM_T *const pParam,
                              GDFLIB_FILTERFIR_STATE_T *const pState);

#ifdef __cplusplus
}
#endif
#endif /* _GDFLIB_FILTERFIR_H_ */

/* End of file */
