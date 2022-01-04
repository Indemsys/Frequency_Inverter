/******************************************************************************
 *
 * (c) Copyright 2009, Freescale & STMicroelectronics
 *
 ***************************************************************************//*!
 *
 * @file     SWLIBS_Typedefs.h
 *
 * @author   r63172
 * 
 * @version  1.0.1.0
 * 
 * @date     Dec-6-2012
 * 
 * @brief    Basic types definition file.
 *
 *******************************************************************************
 *
 ******************************************************************************/
#ifndef _SWLIBS_TYPEDEFS_H
#define _SWLIBS_TYPEDEFS_H

/******************************************************************************
 * Defines and macros            (scope: module-local)
 ******************************************************************************/
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#define NULL ((void*)0)
#endif
#endif

/******************************************************************************
 * Typedefs and structures       (scope: module-local)
 ******************************************************************************/

//typedef unsigned char bool; /*!< basic boolean type */


#ifndef FALSE
#define FALSE ((bool)0)                    /*!< Boolean type FALSE constant */
#endif

#ifndef TRUE
#define TRUE ((bool)1)                     /*!< Boolean type TRUE constant */
#endif

typedef unsigned char UWord8; /*!< unsigned 8-bit integer type */
typedef signed char Word8; /*!< signed 8-bit integer type */
typedef unsigned short UWord16; /*!< unsigned 16-bit integer type */
typedef signed short Word16; /*!< signed 16-bit integer type */
typedef unsigned int UWord32; /*!< unsigned 32-bit integer type */
typedef signed int Word32; /*!< signed 32-bit integer type */
typedef unsigned long long UWord64; /*!< unsigned 64-bit integer type */
typedef signed long long Word64; /*!< signed 64-bit integer type */

typedef unsigned char UInt8; /*!< unsigned 8-bit integer type */
typedef signed char Int8; /*!< signed 8-bit integer type */
typedef unsigned short UInt16; /*!< unsigned 16-bit integer type */
typedef signed short Int16; /*!< signed 16-bit integer type */
typedef unsigned int UInt32; /*!< unsigned 32-bit integer type */
typedef signed int Int32; /*!< signed 32-bit integer type */
typedef unsigned long long UInt64; /*!< unsigned 64-bit integer type */
typedef signed long long Int64; /*!< signed 64-bit integer type */

typedef Word16 Frac16; /*!< 16-bit signed fractional 
 Q1.15 type */
typedef Word32 Frac32; /*!< 32-bit signed fractional 
 Q1.31 type */

/*------------------------------------------------------------------------*//*!
 @struct MCLIB_3_COOR_SYST_T "\SWLIBS_Typedefs.h"

 @brief  Structure data type for three phase system with A,B and C components - input/output variables.
 *//*-------------------------------------------------------------------------*/
typedef struct {
	Frac32 f32A; /*!< A component, type signed 32-bit fractional */
	Frac32 f32B; /*!< B component, type signed 32-bit fractional */
	Frac32 f32C; /*!< C component, type signed 32-bit fractional */
} MCLIB_3_COOR_SYST_T;

/*------------------------------------------------------------------------*//*!
 @struct MCLIB_2_COOR_SYST_ALPHA_BETA_T "\SWLIBS_Typedefs.h"

 @brief  Structure data type for two phase system with alpha and beta components - input/output variables.
 *//*-------------------------------------------------------------------------*/
typedef struct {
	Frac32 f32Alpha; /*!< Alpha component, type signed 32-bit fractional */
	Frac32 f32Beta;  /*!< Beta component, type signed 32-bit fractional */
} MCLIB_2_COOR_SYST_ALPHA_BETA_T;

/*------------------------------------------------------------------------*//*!
 @struct MCLIB_2_COOR_SYST_D_Q_T "\SWLIBS_Typedefs.h"

 @brief  Structure data type for two phase system with direct and quadrature components - input/output variables.
 *//*-------------------------------------------------------------------------*/
typedef struct {
	Frac32 f32D;     /*!< Direct axis component, type signed 32-bit fractional */
	Frac32 f32Q;     /*!< Quadrature axis component, type signed 32-bit fractional */
} MCLIB_2_COOR_SYST_D_Q_T;

/*------------------------------------------------------------------------*//*!
 @struct MCLIB_ANGLE_T "\SWLIBS_Typedefs.h"

 @brief  Structure data type for two axis of two phase system with sine and cosine components - input/output variables.
 *//*-------------------------------------------------------------------------*/
typedef struct {
	Frac32 f32Sin;   /*!< Sine component, type signed 32-bit fractional */
	Frac32 f32Cos;   /*!< Cosine component, type signed 32-bit fractional */
} MCLIB_ANGLE_T;

/******************************************************************************
 * Exported variables
 ******************************************************************************/

/******************************************************************************
 * Exported function prototypes
 ******************************************************************************/

/******************************************************************************
 * Inline functions
 ******************************************************************************/

#endif /* _SWLIBS_TYPEDEFS_H */
