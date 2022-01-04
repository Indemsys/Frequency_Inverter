/**HEADER********************************************************************
*
* Copyright (c) 2010-2011 Freescale Semiconductor
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: psptypes.h$
* $Version : 3.8.10.0$
* $Date    : Oct-3-2012$
*
* Comments:
*   This file contains the definitions of the basic MQX types.
*
*END************************************************************************/
#ifndef __psptypes_h__
#define __psptypes_h__


/*--------------------------------------------------------------------------*/
/*
**                            STANDARD TYPES
*/

/*
**  The following typedefs allow us to minimize portability problems
**  due to the various C compilers (even for the same processor) not
**  agreeing on the sizes of "int"s and "short int"s and "longs".
*/

#define _PTR_      *
#define _CODE_PTR_ *

typedef char _PTR_                    char_ptr;    /* signed character       */
typedef unsigned char  uchar, _PTR_   uchar_ptr;   /* unsigned character     */
typedef volatile char _PTR_                    vchar_ptr;    /* signed character       */
typedef volatile unsigned char  vuchar, _PTR_   vuchar_ptr;   /* unsigned character     */

typedef signed   char   int_8, _PTR_   int_8_ptr;   /* 8-bit signed integer   */
typedef unsigned char  uint_8, _PTR_  uint_8_ptr;  /* 8-bit signed integer   */
typedef volatile signed   char   vint_8, _PTR_   vint_8_ptr;   /* 8-bit volatile signed integer   */
typedef volatile unsigned char  vuint_8, _PTR_  vuint_8_ptr;  /* 8-bit volatile signed integer   */

typedef          short int_16, _PTR_  int_16_ptr;  /* 16-bit signed integer  */
typedef unsigned short uint_16, _PTR_ uint_16_ptr; /* 16-bit unsigned integer*/
typedef volatile          short vint_16, _PTR_  vint_16_ptr;  /* 16-bit volatile signed integer  */
typedef volatile unsigned short vuint_16, _PTR_ vuint_16_ptr; /* 16-bit volatile unsigned integer*/

typedef          long  int_32, _PTR_  int_32_ptr;  /* 32-bit signed integer  */

#ifdef __IAR_IDE__
typedef unsigned long  mqx_uint_32, _PTR_ uint_32_ptr; /* 32-bit unsigned integer*/
#define uint_32 mqx_uint_32
#else
typedef unsigned long  uint_32, _PTR_ uint_32_ptr; /* 32-bit unsigned integer*/
#endif

typedef volatile          long  vint_32, _PTR_  vint_32_ptr;  /* 32-bit signed integer  */
typedef volatile unsigned long  vuint_32, _PTR_ vuint_32_ptr; /* 32-bit unsigned integer*/

typedef    long  long  int_64, _PTR_  int_64_ptr;       /* 64-bit signed   */
typedef unsigned long long  uint_64, _PTR_ uint_64_ptr; /* 64-bit unsigned */
typedef volatile   long  long  vint_64, _PTR_  vint_64_ptr;       /* 64-bit signed   */
typedef volatile unsigned long long  vuint_64, _PTR_ vuint_64_ptr; /* 64-bit unsigned */

#ifdef __IAR_IDE__
typedef unsigned long  _mqx_boolean;  /* Machine representation of a boolean */
#else
typedef unsigned long  boolean;  /* Machine representation of a boolean */
#endif

typedef void _PTR_     pointer;  /* Machine representation of a pointer */

/* IEEE single precision floating point number (32 bits, 8 exponent bits) */
typedef float          ieee_single;

/* IEEE double precision floating point number (64 bits, 11 exponent bits) */
typedef double         ieee_double;

#ifndef USE_32BIT_TYPES
/* Type for the CPU's natural size */
typedef uint_32  _mqx_uint, _PTR_ _mqx_uint_ptr;
typedef int_32   _mqx_int,  _PTR_ _mqx_int_ptr;

/* How big a data pointer is on this processor */
typedef uint_32  _psp_data_addr, _PTR_ _psp_data_addr_ptr;

/* How big a code pointer is on this processor */
typedef uint_32  _psp_code_addr, _PTR_ _psp_code_addr_ptr;

/* Maximum type */
typedef uint_32  _mqx_max_type, _PTR_ _mqx_max_type_ptr;

/* _mem_size is equated to the a type that can hold the maximum data address */
typedef uint_32 _mem_size, _PTR_ _mem_size_ptr;

/* Used for file sizes. */
typedef uint_32       _file_size;
typedef int_32        _file_offset;

#else
#define _mqx_uint      uint_32
#define _mqx_uint_ptr  uint_32_ptr
#define _mqx_int       int_32
#define _mqx_int_ptr   int_32_ptr
#define _psp_data_addr uint_32
#define _psp_code_addr uint_32
#define _mqx_max_type  uint_32
#define _mem_size      uint_32
#endif

/*--------------------------------------------------------------------------*/
/*
**                         DATATYPE VALUE RANGES
*/

#define MAX_CHAR      (0x7F)
#define MAX_UCHAR     (0xFF)
#define MAX_INT_8     (0x7F)
#define MAX_UINT_8    (0xFF)
#define MAX_INT_16    (0x7FFF)
#define MAX_UINT_16   (0xFFFF)
#define MAX_INT_32    (0x7FFFFFFFL)
#define MAX_UINT_32   (0xFFFFFFFFUL)
#define MAX_INT_64    (0x7FFFFFFFFFFFFFFFLL)
#define MAX_UINT_64   (0xFFFFFFFFFFFFFFFFULL)

#define MIN_FLOAT     (8.43E-37)
#define MAX_FLOAT     (3.37E+38)

#define MIN_DOUBLE    (2.225074E-308)
#define MAX_DOUBLE    (1.797693E+308)

#define MAX_MQX_UINT         (MAX_UINT_32)
#define MAX_MQX_INT          (MAX_INT_32)
#define MAX_FILE_SIZE        (MAX_UINT_32)
#define MAX_MEM_SIZE         (MAX_UINT_32)
#define MAX_MQX_MAX_TYPE     (MAX_UINT_32)
#define MQX_INT_SIZE_IN_BITS (32)


#endif /* __psptypes_h__ */
/* EOF */
