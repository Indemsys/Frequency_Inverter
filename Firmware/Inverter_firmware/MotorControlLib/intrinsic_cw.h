/**************************************************************************//**
 * @file     intrinsic_cw.h
 * @brief    CMSIS Cortex-M4 DSP SIMD Header File
 * @version  V1.00.1.0
 * @date     Dec-6-2012
 *
 ******************************************************************************/

#ifndef __CW_CORTEX_M4_DSP_H__
#define __CW_CORTEX_M4_DSP_H__

/** @addtogroup CMSIS_CM4_DSP_intrinsics CM4 DSP Intrinsics
  This file defines all CMSIS CM4 DSP intrinsics
  @{
 */

#ifdef __cplusplus
 extern "C" {
#endif

/* ###################  Compiler specific Intrinsics  ########################### */
/*------ CM4 DSP Intrinsics ------------------------------------------------------*/
#define __SADD8                           __sadd8
#define __QADD8                           __qadd8
#define __SHADD8                          __shadd8
#define __UADD8                           __uadd8
#define __UQADD8                          __uqadd8
#define __UHADD8                          __uhadd8
#define __SSUB8                           __ssub8
#define __QSUB8                           __qsub8
#define __SHSUB8                          __shsub8
#define __USUB8                           __usub8
#define __UQSUB8                          __uqsub8
#define __UHSUB8                          __uhsub8
#define __SADD16                          __sadd16
#define __QADD16                          __qadd16
#define __SHADD16                         __shadd16
#define __UADD16                          __uadd16
#define __UQADD16                         __uqadd16
#define __UHADD16                         __uhadd16
#define __SSUB16                          __ssub16
#define __QSUB16                          __qsub16
#define __SHSUB16                         __shsub16
#define __USUB16                          __usub16
#define __UQSUB16                         __uqsub16
#define __UHSUB16                         __uhsub16
#define __SASX                            __sasx
#define __QASX                            __qasx
#define __SHASX                           __shasx
#define __UASX                            __uasx
#define __UQASX                           __uqasx
#define __UHASX                           __uhasx
#define __SSAX                            __ssax
#define __QSAX                            __qsax
#define __SHSAX                           __shsax
#define __USAX                            __usax
#define __UQSAX                           __uqsax
#define __UHSAX                           __uhsax
#define __USAD8                           __usad8
#define __USADA8                          __usada8
#define __UXTB16                          __uxtb16
#define __UXTAB16                         __uxtab16
#define __SXTB16                          __sxtb16
#define __SXTAB16                         __sxtab16
#define __SSAT16                          __ssat16
#define __USAT16                          __usat16
#define __SMUAD                           __smuad
#define __SMUADX                          __smuadx
#define __SMLAD                           __smlad
#define __SMLADX                          __smladx
#define __SMLALD                           __smlald
#define __SMLALDX                          __smlaldx
#define __SMUSD                           __smusd
#define __SMUSDX                          __smusdx
#define __SMLSD                           __smlsd
#define __SMLSDX                          __smlsdx
#define __SMLSLD                           __smlsld
#define __SMLSLDX                          __smlsldx
#define __SEL                             __sel
#define __QADD                          __qadd 
#define __QSUB                          __qsub 
/*-- End CM4 Intrinsics ----------------------------------------------------------*/

static inline unsigned int __SADD8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SADD8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                sadd8 res,op1,op2;
        }
	return res;
}

static inline unsigned int __QADD8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __QADD8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                qadd8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SHADD8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SHADD8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                shadd8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UADD8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UADD8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uadd8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UQADD8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UQADD8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uqadd8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UHADD8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UHADD8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uhadd8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SSUB8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SSUB8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                ssub8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __QSUB8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __QSUB8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                qsub8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SHSUB8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SHSUB8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                shsub8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __USUB8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __USUB8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                usub8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UQSUB8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UQSUB8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uqsub8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UHSUB8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UHSUB8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uhsub8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SADD16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SADD16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                sadd16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __QADD16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __QADD16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                qadd16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SHADD16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SHADD16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                shadd16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UADD16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UADD16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uadd16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UQADD16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UQADD16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uqadd16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UHADD16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UHADD16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uhadd16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SSUB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SSUB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                ssub16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __QSUB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __QSUB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                qsub16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SHSUB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SHSUB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                ssub16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __USUB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __USUB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                usub16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UQSUB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UQSUB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uqsub16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __UHSUB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UHSUB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uhsub16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SASX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SASX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                sasx res,op1,op2;
        }

        return res;
}

static inline unsigned int __QASX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __QASX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                qasx res,op1,op2;
        }

        return res;
}

static inline unsigned int __SHASX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SHASX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                shasx res,op1,op2;
        }

        return res;
}

static inline unsigned int __UASX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UASX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uasx res,op1,op2;
        }

        return res;
}

static inline unsigned int __UQASX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UQASX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uqasx res,op1,op2;
        }

        return res;
}

static inline unsigned int __UHASX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UHASX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uhasx res,op1,op2;
        }

        return res;
}

static inline unsigned int __SSAX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SSAX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                ssax res,op1,op2;
        }

        return res;
}

static inline unsigned int __QSAX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __QSAX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                qsax res,op1,op2;
        }

        return res;
}

static inline unsigned int __SHSAX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SHSAX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                shsax res,op1,op2;
        }

        return res;
}

static inline unsigned int __USAX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __USAX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                usax res,op1,op2;
        }

        return res;
}

static inline unsigned int __UQSAX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UQSAX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uqsax res,op1,op2;
        }

        return res;
}

static inline unsigned int __UHSAX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UHSAX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uhsax res,op1,op2;
        }

        return res;
}

static inline unsigned int __USAD8(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __USAD8(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                usad8 res,op1,op2;
        }

        return res;
}

static inline unsigned int __USADA8(register unsigned int op1, register unsigned int op2, register unsigned int op3) __attribute__((always_inline));
static inline unsigned int __USADA8(register unsigned int op1, register unsigned int op2, register unsigned int op3)
{
	register unsigned int res;
        asm {
                usada8 res,op1,op2,op3;
        }

        return res;
}

static inline unsigned int __SSAT16(register unsigned int op1, unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SSAT16(register unsigned int op1, unsigned int op2)
{
	register unsigned int res;
        asm ("ssat16 %0, %1, %2" : "=r" (res) :  "I" (op2), "r" (op1));

        return res;
}

static inline unsigned int __USAT16(register unsigned int op1, unsigned int op2) __attribute__((always_inline));
static inline unsigned int __USAT16(register unsigned int op1, unsigned int op2)
{
	register unsigned int res;
        asm ("usat16 %0, %1, %2" : "=r" (res) :  "I" (op2), "r" (op1));
        return res;
}
/*#define __SSAT16(ARG1,ARG2) \
({                          \
  unsigned int __RES, __ARG1 = (ARG1); \
  asm ("ssat16 %0, %1, %2" : "=r" (__RES) :  "I" (ARG2), "r" (__ARG1) ); \
  __RES; \
 })
*/
/*#define __USAT16(ARG1,ARG2) \
(                          \
  	asm {\
  	  	usat16 r0,#ARG2, ARG1\
		}\
 )
*/

static inline unsigned int __UXTB16(register unsigned int op1) __attribute__((always_inline));
static inline unsigned int __UXTB16(register unsigned int op1)
{
	register unsigned int res;
        asm {
                uxtb16 res,op1;
        }

        return res;
}

static inline unsigned int __UXTAB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __UXTAB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                uxtab16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SXTB16(register unsigned int op1) __attribute__((always_inline));
static inline unsigned int __SXTB16(register unsigned int op1)
{
	register unsigned int res;
        asm {
                sxtb16 res,op1;
        }

        return res;
}

static inline unsigned int __SXTAB16(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SXTAB16(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                sxtab16 res,op1,op2;
        }

        return res;
}

static inline unsigned int __SMUAD(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SMUAD(register unsigned int op1, register unsigned int op2)
{
	register signed int res;
        asm {
                smuad res,op1,op2;
        }

        return res;
}

static inline unsigned int __SMUADX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SMUADX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                smuadx res,op1,op2;
        }

        return res;
}

static inline unsigned int __SMLAD(register unsigned int op1, register unsigned int op2, register unsigned int op3) __attribute__((always_inline));
static inline unsigned int __SMLAD(register unsigned int op1, register unsigned int op2, register unsigned int op3)
{
	register unsigned int res;
        asm {
                smlad res,op1,op2,op3;
        }

        return res;
}

static inline unsigned int __SMLADX(register unsigned int op1, register unsigned int op2, register unsigned int op3) __attribute__((always_inline));
static inline unsigned int __SMLADX(register unsigned int op1, register unsigned int op2, register unsigned int op3)
{
	register unsigned int res;
        asm {
                smladx res,op1,op2,op3;
        }

        return res;
}
static inline unsigned long long __SMLALD(register unsigned int op1, register unsigned int op2, register unsigned long long op3) __attribute__((always_inline));
static inline unsigned long long __SMLALD(register unsigned int op1, register unsigned int op2, register unsigned long long op3)
{
	register unsigned int op3_H = (unsigned int)((op3) >> 32), op3_L = (unsigned int)((op3) & 0xFFFFFFFF);
	unsigned long long res ;
	asm {
		smlald op3_L, op3_H, op1, op2;
		}
        res = op3_H;
		return((res << 32) | op3_L);
}

static inline unsigned long long __SMLALDX(register unsigned int op1, register unsigned int op2, register unsigned long long op3) __attribute__((always_inline));
static inline unsigned long long __SMLALDX(register unsigned int op1, register unsigned int op2, register unsigned long long op3)
{
	register unsigned int op3_H = (unsigned int)((op3) >> 32), op3_L = (unsigned int)((op3) & 0xFFFFFFFF);
		unsigned long long res ;
		asm {
			smlaldx op3_L, op3_H, op1, op2;
		}

        res = op3_H;
		return((res << 32) | op3_L);
}

/*#define __SMLALD(ARG1,ARG2,ARG3) \
({\
	unsigned int __ARG1 = ARG1, __ARG2 = ARG2, __ARG3_H = (unsigned int)((ARG3) >> 32), __ARG3_L = (unsigned int)((ARG3) & 0xFFFFFFFF); \
	asm {	smlald __ARG3_L, __ARG3_H, __ARG1, __ARG2}\
	})

#define __SMLALDX(ARG1,ARG2,ARG3) \
({\
	unsigned int __ARG1 = ARG1, __ARG2 = ARG2, __ARG3_H = (unsigned int)((ARG3) >> 32), __ARG3_L = (unsigned int)((ARG3) & 0xFFFFFFFF); \
	asm {	smlaldx __ARG3_L, __ARG3_H, __ARG1, __ARG2}\
	})
*/
static inline unsigned int __SMUSD(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SMUSD(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                smusd res,op1,op2;
        }

        return res;
}

static inline unsigned int __SMUSDX(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __SMUSDX(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                smusdx res,op1,op2;
        }

        return res;
}

static inline unsigned int __SMLSD(register unsigned int op1, register unsigned int op2, register unsigned int op3) __attribute__((always_inline));
static inline unsigned int __SMLSD(register unsigned int op1, register unsigned int op2, register unsigned int op3)
{
	register unsigned int res;
        asm {
                smlsd res,op1,op2, op3;
        }

        return res;
}

static inline unsigned int __SMLSDX(register unsigned int op1, register unsigned int op2, register unsigned int op3) __attribute__((always_inline));
static inline unsigned int __SMLSDX(register unsigned int op1, register unsigned int op2, register unsigned int op3)
{
	register unsigned int res;
        asm {
                smlsdx res,op1,op2,op3;
        }

        return res;
}

static inline unsigned long long __SMLSLD(register unsigned int op1, register unsigned int op2, register unsigned long long op3) __attribute__((always_inline));
static inline unsigned long long __SMLSLD(register unsigned int op1, register unsigned int op2, register unsigned long long op3)
{
	register unsigned int op3_H = (unsigned int)((op3) >> 32), op3_L = (unsigned int)((op3) & 0xFFFFFFFF); \
	unsigned long long res ;
	asm {
		smlsld op3_L, op3_H, op1, op2;
		}
        res = op3_H;
		return((res << 32) | op3_L);
}

static inline unsigned long long __SMLSLDX(register unsigned int op1, register unsigned int op2, register unsigned long long op3) __attribute__((always_inline));
static inline unsigned long long __SMLSLDX(register unsigned int op1, register unsigned int op2, register unsigned long long op3)
{
	register unsigned int op3_H = (unsigned int)((op3) >> 32), op3_L = (unsigned int)((op3) & 0xFFFFFFFF); \
		unsigned long long res;
		asm {
			smlsldx op3_L, op3_H, op1, op2;
		}
		res = op3_H;
		return((res << 32) | op3_L);
}

/*#define __SMLSLD(ARG1,ARG2,ARG3) \
({\
	unsigned int __ARG1 = ARG1, __ARG2 = ARG2, __ARG3_H = (unsigned int)((ARG3) >> 32), __ARG3_L = (unsigned int)((ARG3) & 0xFFFFFFFF); \
	asm {	smlsld __ARG3_L, __ARG3_H, __ARG1, __ARG2}\
	})

#define __SMLSLDX(ARG1,ARG2,ARG3) \
({\
	unsigned int __ARG1 = ARG1, __ARG2 = ARG2, __ARG3_H = (unsigned int)((ARG3) >> 32), __ARG3_L = (unsigned int)((ARG3) & 0xFFFFFFFF); \
	asm {	smlsldx __ARG3_L, __ARG3_H, __ARG1, __ARG2}\
	})
*/
static inline unsigned int __SEL(register unsigned int op1, register unsigned int op2) __attribute__((always_inline));
static inline unsigned int __sel(register unsigned int op1, register unsigned int op2)
{
	register unsigned int res;
        asm {
                sel res,op1,op2;
        }

        return res;
}
/* ###################  adding functions QADD QSUB ########################### */

static inline long __QADD(register long op1, register long op2) __attribute__((always_inline));
static inline long __qadd(register long op1, register long op2)
{
	register long res;
        asm {
                QADD res,op1,op2;
        }

        return res;
}

static inline long __QSUB(register long op1, register long op2) __attribute__((always_inline));
static inline long __qsub(register long op1, register long op2)
{
	register long res;
        asm {
                QSUB res,op1,op2;
        }

        return res;
}

#endif
