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
* $FileName: adc.h$
* $Version : 3.8.31.1$
* $Date    : Jul-11-2012$
*
* Comments:
*
*   Generic ADC driver header file
*
*END************************************************************************/
#ifndef __adc_h__
#define __adc_h__

#if MQX_USE_LWEVENTS
    #include <lwevent.h>
#endif
/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Initialization struct when opening driver */
typedef struct adc_init_struct {
   uint_32   resolution;       /* resolution of ADC driver, the bigger or equal possible value is used to configure ADC peripheral */
} ADC_INIT_STRUCT, _PTR_ ADC_INIT_STRUCT_PTR;

typedef uint_32 ADT_TRIGGER_MASK;
#define ADC_TRIGGER_1                  (0x0001)
#define ADC_TRIGGER_2                  (0x0002)
#define ADC_TRIGGER_3                  (0x0004)
#define ADC_TRIGGER_4                  (0x0008)
#define ADC_TRIGGER_5                  (0x0010)
#define ADC_TRIGGER_6                  (0x0020)
#define ADC_TRIGGER_7                  (0x0040)
#define ADC_TRIGGER_8                  (0x0080)
#define ADC_TRIGGER_9                  (0x0100)
#define ADC_TRIGGER_10                 (0x0200)
#define ADC_TRIGGER_11                 (0x0400)
#define ADC_TRIGGER_12                 (0x0800)
#define ADC_TRIGGER_13                 (0x1000)
#define ADC_TRIGGER_14                 (0x2000)
#define ADC_TRIGGER_15                 (0x4000)
#define ADC_TRIGGER_16                 (0x8000)

/* flags used to initialize every channel */
#define ADC_CHANNEL_MEASURE_LOOP       (0x00) /* measurement never ends, lwevents are set periodically during measurement */
#define ADC_CHANNEL_MEASURE_ONCE       (0x04) /* measurement ends with set lwevent */
#define ADC_CHANNEL_START_TRIGGERED    (0x02) /* after RUN on channel is set, delay is applied (trigger delay) */
#define ADC_CHANNEL_START_NOW          (0x00) /* after RUN on channel is set, no delay is applied and measurement is immediate */
#define ADC_CHANNEL_ACCUMULATE         (0x10) /* accumulate the samples into one result variable, not to the field of values */

typedef struct adc_init_channel_struct {
   uint_16   source;           /* one of ADC_SOURCE_* definitions defined in ./<module_name>/adc_<module_name>.h - CPU specific header file */
   uint_16   flags;            /* initialization flags, combination of ADC_CHANNEL_<FLAG_NAME> possibilities above */
   uint_32   number_samples;   /* the number of samples needed to trigger event */
   uint_32   time_offset;      /* the time offset applied once after measurement on given channel is started, in microseconds */
   uint_32   time_period;      /* the time slot between two samples in microseconds */
   /* NOTE: the following item is not used in the current ADC driver and is prepared to be used in the future */
   uint_32   range;            /* the range of result sample(s), NOT USED NOW */
                               /* Note #1: using ADC_CHANNEL_ACCUMULATE, you can achieve finer resolution than specified in ADC */
                               /* Note #2: The result of ADC driver always fits to the range specified here */
                               /* Note #3: if range is too low, then you lose LSbits */
                               /* Note #4: if range is too high, then you get LSbits cleared */
                               /* Note #5: range can be only a number 2^N (in C language: 1 << N) */
   _mqx_uint buffer_size;      /* ADC output buffer size, measured samples are stored here */
   ADT_TRIGGER_MASK trigger;   /* trigger mask that fires this channel */
#if MQX_USE_LWEVENTS
   LWEVENT_STRUCT_PTR complete_event; /* pointer to lwevent to be signaled after nuber_samples is measured */
   uint_32            event_mask;     /* event mask to be signalled in lwevent after nuber_samples is measured */
#endif
} ADC_INIT_CHANNEL_STRUCT, _PTR_ ADC_INIT_CHANNEL_STRUCT_PTR;

typedef uint_32 ADC_VALUE_TYPE;
typedef struct adc_result_struct {
   ADC_VALUE_TYPE   result;
} ADC_RESULT_STRUCT, _PTR_ ADC_RESULT_STRUCT_PTR;

/*
**  IOCTL commands used in ADC driver
*/
#define ADC_IOCTL_RUN_CHANNEL           _IO(IO_TYPE_ADC, 0x01)
#define ADC_IOCTL_RUN_CHANNELS          _IO(IO_TYPE_ADC, 0x02)
#define ADC_IOCTL_FIRE_TRIGGER          ADC_IOCTL_RUN_CHANNELS
#define ADC_IOCTL_STOP_CHANNEL          _IO(IO_TYPE_ADC, 0x03)
#define ADC_IOCTL_STOP_CHANNELS         _IO(IO_TYPE_ADC, 0x04)
#define ADC_IOCTL_PAUSE_CHANNEL         _IO(IO_TYPE_ADC, 0x05)
#define ADC_IOCTL_PAUSE_CHANNELS        _IO(IO_TYPE_ADC, 0x06)
#define ADC_IOCTL_RESUME_CHANNEL        _IO(IO_TYPE_ADC, 0x07)
#define ADC_IOCTL_RESUME_CHANNELS       _IO(IO_TYPE_ADC, 0x08)
#define ADC_IOCTL_HW_SPECIFIC           0x09 /* this is not valid IOCTL in fact */

/* IOCTL obsolete name convention, for backward compatibility */
#define IOCTL_ADC_RUN_CHANNEL           ADC_IOCTL_RUN_CHANNEL
#define IOCTL_ADC_RUN_CHANNELS          ADC_IOCTL_RUN_CHANNELS
#define IOCTL_ADC_FIRE_TRIGGER          ADC_IOCTL_FIRE_TRIGGER
#define IOCTL_ADC_STOP_CHANNEL          ADC_IOCTL_STOP_CHANNEL
#define IOCTL_ADC_STOP_CHANNELS         ADC_IOCTL_STOP_CHANNELS
#define IOCTL_ADC_PAUSE_CHANNEL         ADC_IOCTL_PAUSE_CHANNEL
#define IOCTL_ADC_PAUSE_CHANNELS        ADC_IOCTL_PAUSE_CHANNELS
#define IOCTL_ADC_RESUME_CHANNEL        ADC_IOCTL_RESUME_CHANNEL
#define IOCTL_ADC_RESUME_CHANNELS       ADC_IOCTL_RESUME_CHANNELS
#define IOCTL_ADC_HW_SPECIFIC           ADC_IOCTL_HW_SPECIFIC

/*
**  IOCTL error codes for IOCTLs
*/
#define ADC_OK                                        (IO_OK)
#define ADC_ERROR_ALLOC                               (ADC_ERROR_BASE | 0x01) /* memory allocation error */
#define ADC_ERROR_ISR                                 (ADC_ERROR_BASE | 0x02) /* cannot install ISR */
#define ADC_ERROR_PARAM                               (ADC_ERROR_BASE | 0x03) /* missing parameter */
#define ADC_ERROR_OPENED                              (ADC_ERROR_BASE | 0x04) /* file already opened */
#define ADC_ERROR_MISSING_DEVICE                      (ADC_ERROR_BASE | 0x05) /* device was not opened prior to channel open */
#define ADC_ERROR_BAD_PARAM                           (ADC_ERROR_BASE | 0x06) /* bad parameter */
#define ADC_ERROR_FULL                                (ADC_ERROR_BASE | 0x07) /* cannot open more files */
#define ADC_ERROR_NONEMPTY                            (ADC_ERROR_BASE | 0x08) /* cannot run command if channel is still opened */
#define ADC_ERROR_ONLY_DEVICE                         (ADC_ERROR_BASE | 0x09) /* cannot run such command on channel */
#define ADC_ERROR_ONLY_CHANNEL                        (ADC_ERROR_BASE | 0x0A) /* cannot run such command on device */
#define ADC_ERROR_HW_SPECIFIC                         0x0B /* this is not valid error in fact */


#if PSP_MQX_CPU_IS_MCF51JF || PSP_MQX_CPU_IS_MCF51QM || PSP_MQX_CPU_IS_KINETIS
    #include "adc_kadc.h"
#elif PSP_MQX_CPU_IS_MCF5222X || PSP_MQX_CPU_IS_MCF5223X || PSP_MQX_CPU_IS_MCF5225X
    #include "adc_mcf522xx.h"
#elif PSP_MQX_CPU_IS_MCF51EM
    #include "adc_mcf51em.h"
#elif PSP_MQX_CPU_IS_MCF51MM
    #include "adc_mcf51mm.h"
#elif PSP_MQX_CPU_IS_MCF51JE
    #include "adc_mcf51je.h"
#elif PSP_MQX_CPU_IS_MCF51AG
    #include "adc_mcf51ag.h"
#elif PSP_MQX_CPU_IS_MCF51
    #include "adc_mcf51xx.h"
#elif PSP_MQX_CPU_IS_MCF5441X
    #include "adc_mcf544xx.h"
#else
  #error ADC device driver not supported for processor.
#endif


_mqx_uint _io_adc_install(char_ptr, pointer);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
