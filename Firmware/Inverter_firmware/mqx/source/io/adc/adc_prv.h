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
* $FileName: adc_prv.h$
* $Version : 3.8.28.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure
*   definitions private to the fdv Ramdisk drivers
*
*END************************************************************************/
#ifndef __adc_prv_h__
#define __adc_prv_h__

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

#define IO_ERROR_INCOMPLETE_IOCTL       (IO_ERROR_INVALID_IOCTL_CMD) /* this is internal ADC IOCTL and means that IOCTL must be passed from ADC part to trigger part */

/* information stored here concerns only to AD converter */
typedef struct adc_generic {
   uint_32            resolution;
   uint_32            adc_frq;       /* frequency of ADC hardware, usually 1 / Tad */
} ADC_GENERIC, _PTR_ ADC_GENERIC_PTR;

/* information stored here concerns only to AD trigger */
typedef struct adt_generic {
   ADT_TRIGGER_MASK   triggers_fire; /* specifies triggers, which were set and there is still attached channel that did not complete conversion */
   boolean            run;           /* specifies if ADC is running (this is OR operation of all channel's ADC_CHANNEL_RUNNING flag) */
} ADT_GENERIC, _PTR_ ADT_GENERIC_PTR;

typedef struct adc_channel_generic {
   _mqx_uint number;     /* number of channel */
   uint_16               init_flags;        /* flags copied from initialization structure */
   uint_16               source;            /* source copied from initialization structure */
   uint_32               range;             /* output range */
   ADC_RESULT_STRUCT_PTR buffer_start;      /* circular result buffer start pointer */
   ADC_RESULT_STRUCT_PTR buffer_end;        /* circular result buffer end pointer */
   ADC_RESULT_STRUCT_PTR buffer_task;       /* circular result buffer - next read pointer for result */
   ADC_RESULT_STRUCT_PTR buffer_driver;     /* circular result buffer - next write pointer for result */
} ADC_CHANNEL_GENERIC, _PTR_ ADC_CHANNEL_GENERIC_PTR;

typedef struct adt_channel_generic {
   _mqx_uint number;     /* number of channel */
   uint_16               init_flags;        /* flags copied from initialization structure */
#define ADC_CHANNEL_RUNNING          (0x01) /* when ADC channel is running, this flag is set */
#define ADC_CHANNEL_RESUMED          (0x02) /* when ADC channel is not paused, this flag is set */
   uint_16               runtime_flags;     /* generic flags for ADC driver */
   int_32                offset;            /* offset of time, counter: first this needs to be decremented to zero to start conversion process */
   _mqx_uint             period;            /* time period between 2 samples taken */
   _mqx_uint             time;              /* time to next ADC */
   int_32                samples;           /* samples, counter: this needs to be decremented to zero to invoke any event */
   int_32                samples_preset;    /* constant: specifies predefined number of samples */
   ADT_TRIGGER_MASK      trigger;           /* trigger, that triggers this ADC channel */
#if MQX_USE_LWEVENTS
   LWEVENT_STRUCT_PTR    complete_event;    /* pointer to lwevent, triggered after counter_samples reaches zero */
   uint_32               event_mask;        /* mask of events in complete_event */
#endif
} ADT_CHANNEL_GENERIC, _PTR_ ADT_CHANNEL_GENERIC_PTR;

typedef uint_32 ADC_HW_CHANNEL_MASK; /* there can be only 32 channels per converter */

/* NOTE: DO NOT MOVE THESE INCLUDES, THEY MUST BE HERE. THEY DEFINES NEW MANDATORY TYPES (E.G. ADC_CHANNEL_BUNDLE) BASED ON GENERIC TYPES */
#if (PSP_MQX_CPU_IS_MCF51QM || PSP_MQX_CPU_IS_MCF51JF || PSP_MQX_CPU_IS_KINETIS)
    #include "adc_kadc_prv.h"
#elif PSP_MQX_CPU_IS_MCF5222X || PSP_MQX_CPU_IS_MCF5223X || PSP_MQX_CPU_IS_MCF5225X
    #include "adc_mcf522xx_prv.h"
#elif PSP_MQX_CPU_IS_MCF51MM
    #include "adc_mcf51mm_prv.h"
#elif PSP_MQX_CPU_IS_MCF51JE
    #include "adc_mcf51je_prv.h"
#elif PSP_MQX_CPU_IS_MCF51AG
    #include "adc_mcf51ag_prv.h"
#elif PSP_MQX_CPU_IS_MCF51EM
    #include "adc_mcf51em_prv.h"
#elif PSP_MQX_CPU_IS_MCF51
    #include "adc_mcf51xx_prv.h"
#elif PSP_MQX_CPU_IS_MCF5441X
    #include "adc_mcf544xx_prv.h"
#else
    #error ADC device driver not supported for processor.
#endif
/* AND NOW, WE HAVE ALL DEFINITIONS FOR DRIVER */

_mqx_int _adc_open(MQX_FILE_PTR, char_ptr, char_ptr);
_mqx_int _adc_close(MQX_FILE_PTR);
_mqx_int _adc_read (MQX_FILE_PTR, char_ptr, _mqx_int);
_mqx_int _adc_write(MQX_FILE_PTR, char_ptr, _mqx_int);
_mqx_int _adc_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);

/* these functions must be present for trigger */
_mqx_int _adt_trigger(MQX_FILE_PTR, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask);
_mqx_int _adt_pause(MQX_FILE_PTR, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask);
_mqx_int _adt_resume(MQX_FILE_PTR, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask);
_mqx_int _adt_stop(MQX_FILE_PTR, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask);
_mqx_int _adt_process_data(ADC_CHANNEL_BUNDLE_PTR channel);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
