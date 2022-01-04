#ifndef __adc_mcf52xx_prv_h__
#define __adc_mcf52xx_prv_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: adc_mcf522xx_prv.h$
* $Version : 3.6.13.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   CPU specific ADC driver header file
*
*END************************************************************************/
#include "adc.h"
#include "adc_prv.h"
#include "adc_conf.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_MAX_CHANNELS ADC_MAX_SW_CHANNELS

/* Structure holding ADC driver settings */
typedef struct adc {
   ADC_GENERIC           g; /* this must be included as the first item at the beginning of structure */
   /* below, CPU specific items are used */
   ADC_HW_CHANNEL_MASK   last_mask;
} ADC, _PTR_ ADC_PTR;

/* Structure holding ADC driver settings */
typedef struct adt {
   ADT_GENERIC           g; /* this must be included as the first item at the beginning of structure */
   /* below, CPU specific items are used */
   _mqx_int              shift; /* this is the radix we can shift to get from (prescaler+preset) scale to the lowest time */
   uint_32   time_to_next_adc; /* stores time needed to next AD conversion */
   uint_32   time_to_next_irq; /* stores time needed to next timer IRQ */
   uint_32   dispatch_time;    /* stores absolute time for dispatcher */
   boolean   dispatch_next;    /* informs if any data within channels_to_next_adc is valid */
} ADT, _PTR_ ADT_PTR;

/* Structure holding ADC driver channel settings, must be declared */
typedef struct adc_channel {
   ADC_CHANNEL_GENERIC   g; /* this must be included as the first member at the beginning of structure */
   /* below, CPU specific members are used */
} ADC_CHANNEL, _PTR_ ADC_CHANNEL_PTR;

/* Structure holding ADT driver channel settings, must be declared */
typedef struct adt_channel {
   ADT_CHANNEL_GENERIC   g; /* this must be included as the first member at the beginning of structure */
   /* below, CPU specific members are used */
} ADT_CHANNEL, _PTR_ ADT_CHANNEL_PTR;

/* ADC converter + trigger channel bundle structure */
typedef struct adc_channel_bundle {
   ADC_CHANNEL_PTR  adc_ch;
   ADT_CHANNEL_PTR  adt_ch;
   /* below, CPU specific members are used */
} ADC_CHANNEL_BUNDLE, _PTR_ ADC_CHANNEL_BUNDLE_PTR;

/* ADC converter + trigger bundle structure */
typedef struct adc_driver_bundle {
   ADC_PTR          adc;                      /* ADC driver status information (generic + HW specific) */
   ADT_PTR          adt;                      /* ADT driver status information (generic + HW specific) */
   ADC_CHANNEL_PTR  adc_ch[ADC_MAX_CHANNELS];
   ADT_CHANNEL_PTR  adt_ch[ADC_MAX_CHANNELS];
   uchar_ptr        adc_name[ADC_MAX_CHANNELS]; /* filename of channel(s) */
   /* below, CPU specific members are used */
   uint_32          last_mask; /* mask of ADC channels to perform ADC next time */
} ADC_DRIVER_BUNDLE, _PTR_ ADC_DRIVER_BUNDLE_PTR;

/* anything static can be shared within ADC and ADT */
/* typedef struct adc_bundle {
} ADC_BUNDLE, ADC_BUNDLE_PTR; */

_mqx_int _adc_hw_install(char_ptr, ADC_DRIVER_BUNDLE_PTR, pointer);
_mqx_int _adt_hw_install(char_ptr, ADC_DRIVER_BUNDLE_PTR, pointer);
_mqx_int _adc_hw_init(MQX_FILE_PTR, ADC_PTR);
_mqx_int _adt_hw_init(MQX_FILE_PTR, ADT_PTR);
_mqx_int _adc_hw_deinit(MQX_FILE_PTR, ADC_PTR);
_mqx_int _adt_hw_deinit(MQX_FILE_PTR, ADT_PTR);
_mqx_int _adc_hw_channel_init(MQX_FILE_PTR, ADC_CHANNEL_PTR);
_mqx_int _adt_hw_channel_init(MQX_FILE_PTR, ADT_CHANNEL_PTR);
_mqx_int _adc_hw_channel_deinit(MQX_FILE_PTR, ADC_CHANNEL_PTR);
_mqx_int _adt_hw_channel_deinit(MQX_FILE_PTR, ADT_CHANNEL_PTR);
_mqx_int _adc_hw_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
_mqx_int _adt_hw_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);

void _adc_hw_measure(ADC_HW_CHANNEL_MASK);

#ifdef __cplusplus
}
#endif

#endif
