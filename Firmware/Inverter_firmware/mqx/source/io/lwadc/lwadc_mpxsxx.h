/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2011 Embedded Access Inc
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
* $FileName: lwadc_mpxsxx.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   CPU specific ADC driver header file
*
*END************************************************************************/

#ifndef __lwadc_mpxsxx_h__
#define __lwadc_mpxsxx_h__

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_RESOLUTION_12BIT    0x1000
#define ADC_RESOLUTION_DEFAULT  ADC_RESOLUTION_12BIT

#define ADC_DEV_SHIFT           (4)
#define ADC_DEV_MASK            (0xf0)
#define ADC_CHANNEL_MASK        (0xf)

#define ADC_INPUT(dev,channel)  (dev<<ADC_DEV_SHIFT | channel)
#define ADC_DEVICE(input)       (input>>ADC_DEV_SHIFT)
#define ADC_CHANNEL(input)      (input&ADC_CHANNEL_MASK)
#define ADC_SOURCE_AN0          0x00
#define ADC_SOURCE_AN1          0x01
#define ADC_SOURCE_AN2          0x02
#define ADC_SOURCE_AN3          0x03
#define ADC_SOURCE_AN4          0x04
#define ADC_SOURCE_AN5          0x05
#define ADC_SOURCE_AN6          0x06
#define ADC_SOURCE_AN7          0x07
#define ADC_SOURCE_AN8          0x08
#define ADC_SOURCE_AN9          0x09
#define ADC_SOURCE_AN10         0x0a
#define ADC_SOURCE_AN11         0x0b
#define ADC_SOURCE_AN12         0x0c
#define ADC_SOURCE_AN13         0x0d
#define ADC_SOURCE_AN14         0x0e
#define ADC_SOURCE_AN15         0x0f
#define ADC_SOURCE_MAX          0x0f

#define ADC_AN0_MASK            (1<<ADC_SOURCE_AN0)
#define ADC_AN1_MASK            (1<<ADC_SOURCE_AN1)
#define ADC_AN2_MASK            (1<<ADC_SOURCE_AN2)
#define ADC_AN3_MASK            (1<<ADC_SOURCE_AN3)
#define ADC_AN4_MASK            (1<<ADC_SOURCE_AN4)
#define ADC_AN5_MASK            (1<<ADC_SOURCE_AN5)
#define ADC_AN6_MASK            (1<<ADC_SOURCE_AN6)
#define ADC_AN7_MASK            (1<<ADC_SOURCE_AN7)
#define ADC_AN8_MASK            (1<<ADC_SOURCE_AN8)
#define ADC_AN9_MASK            (1<<ADC_SOURCE_AN9)
#define ADC_AN10_MASK           (1<<ADC_SOURCE_AN10)
#define ADC_AN11_MASK           (1<<ADC_SOURCE_AN11)
#define ADC_AN12_MASK           (1<<ADC_SOURCE_AN12)
#define ADC_AN13_MASK           (1<<ADC_SOURCE_AN13)
#define ADC_AN14_MASK           (1<<ADC_SOURCE_AN14)
#define ADC_AN15_MASK           (1<<ADC_SOURCE_AN15)

#define ADC_ALL_CHANNEL_MASK    (0x0000ffff)


typedef struct lwadc_init_struct {
    uint_32                     device;
    uint_32                     format;
    uint_32                     clock;
    uint_32                     reference;
}LWADC_INIT_STRUCT, * LWADC_INIT_STRUCT_PTR;


typedef struct adc_context {
    VPXADC_REG_STRUCT_PTR       adc_ptr;
    const LWADC_INIT_STRUCT *   init_ptr;
    uint_32                     mcr;
    uint_32                     channels;
}ADC_CONTEXT, * ADC_CONTEXT_PTR;

typedef struct lwadc_struct {
    ADC_CONTEXT_PTR             context_ptr;
    uint_32                     input;
    uint_32                     mask;
    uint_32                     numerator;
    uint_32                     denominator;
}LWADC_STRUCT, * LWADC_STRUCT_PTR;



#ifdef __cplusplus
}
#endif

#endif
