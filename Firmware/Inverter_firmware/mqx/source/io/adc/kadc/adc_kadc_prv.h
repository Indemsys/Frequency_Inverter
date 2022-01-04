/**************************************************************************
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
* $FileName: adc_kadc_prv.h$
* $Version : 3.8.10.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*   CPU specific ADC driver header file
*
*END************************************************************************/

#ifndef __adc_kadc_prv_h__
#define __adc_kadc_prv_h__

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_MAX_FREQUENCY 5000000 /* 5 MHz for ADC clock source is maximum */

#ifndef ADC_CHANNELS_PER_ADC
    #define ADC_CHANNELS_PER_ADC (2)  /* one ADC can handle 2 channels */
#endif
#define ADC_MAX_HW_CHANNELS  ADC_CHANNELS_PER_ADC /* there is no sense to have more channels allocated */

#ifndef ADC_MAX_MODULES
    #error Define number of ADC peripheral modules on chip in BSP (<MQX>/source/bsp/<board_name>/<board_name>.h)
#endif

#ifndef ADC_MAX_SW_CHANNELS
    #define ADC_MAX_CHANNELS ADC_MAX_HW_CHANNELS
#else
    #if (ADC_MAX_HW_CHANNELS >= ADC_MAX_SW_CHANNELS)
        #define ADC_MAX_CHANNELS ADC_MAX_SW_CHANNELS
    #else
        #error The ADC_MAX_SW_CHANNELS must be less than ADC_MAX_HW_CHANNELS.
    #endif
#endif

#define ADC_CFG2_ADLSTS_20             ADC_CFG2_ADLSTS(0)
#define ADC_CFG2_ADLSTS_12             ADC_CFG2_ADLSTS(1)
#define ADC_CFG2_ADLSTS_6              ADC_CFG2_ADLSTS(2)
#define ADC_CFG2_ADLSTS_2              ADC_CFG2_ADLSTS(3)
#define ADC_CFG2_ADLSTS_DEFAULT       (ADC_CFG2_ADLSTS_20)

#define ADC_SC3_AVGS_4                 ADC_SC3_AVGS(0x00)
#define ADC_SC3_AVGS_8                 ADC_SC3_AVGS(0x01)
#define ADC_SC3_AVGS_16                ADC_SC3_AVGS(0x02)
#define ADC_SC3_AVGS_32                ADC_SC3_AVGS(0x03)
#define ADC_SC3_AVGE                   ADC_SC3_AVGE_MASK
#define ADC_SC3_ADCO                   ADC_SC3_ADCO_MASK
#define ADC_SC3_CALF                   ADC_SC3_CALF_MASK
#define ADC_SC3_CAL                    ADC_SC3_CAL_MASK

#define ADC_SC2_REFSEL_VREF            ADC_SC2_REFSEL(0x00)
#define ADC_SC2_REFSEL_VALT            ADC_SC2_REFSEL(0x01)
#define ADC_SC2_REFSEL_VBG             ADC_SC2_REFSEL(0x02)

#define ADC_SC1_ADCH_DISABLED          ADC_SC1_ADCH(0x1F)

#define ADC_CFG1_ADIV_1                ADC_CFG1_ADIV(0x00)
#define ADC_CFG1_ADIV_2                ADC_CFG1_ADIV(0x01)
#define ADC_CFG1_ADIV_4                ADC_CFG1_ADIV(0x02)
#define ADC_CFG1_ADIV_8                ADC_CFG1_ADIV(0x03)

#define ADC_CFG1_ADICLK_BUSCLK         ADC_CFG1_ADICLK(0x00)
#define ADC_CFG1_ADICLK_BUSCLK2        ADC_CFG1_ADICLK(0x01)
#define ADC_CFG1_ADICLK_ALTCLK         ADC_CFG1_ADICLK(0x02)
#define ADC_CFG1_ADICLK_ADACK          ADC_CFG1_ADICLK(0x03)

#define PDB_SC_TRGSEL_SWTRIG           PDB_SC_TRGSEL(0x1F)
/* Structure holding ADC driver settings */
typedef struct adc {
   ADC_GENERIC           g; /* this must be included as the first member at the beginning of structure */
   /* below, CPU specific members are used */
   void (*old_isr)(pointer);
   pointer old_isr_data;
   uint_8                muxsel; /* holds setting for MUX selection- used for checking whether MUXSEL settings meets channel criteria */
} ADC, _PTR_ ADC_PTR;

/* Structure holding ADC driver settings */
typedef struct adt {
   ADT_GENERIC           g; /* this must be included as the first item at the beginning of structure */
} ADT, _PTR_ ADT_PTR;

/* Structure holding ADC driver channel settings */
typedef struct adc_channel {
   ADC_CHANNEL_GENERIC   g; /* this must be included as the first member at the beginning of structure */
   /* below, CPU specific members are used */
   struct adc_channel_bundle *b;
} ADC_CHANNEL, _PTR_ ADC_CHANNEL_PTR;

/* Structure holding ADT driver channel settings, must be declared */
typedef struct adt_channel {
   ADT_CHANNEL_GENERIC   g; /* this must be included as the first member at the beginning of structure */
   /* below, CPU specific members are used */
   struct adc_channel_bundle *b;
   /* All ADT channels use one common HW trigger settings. Only delay is per - channel set */
} ADT_CHANNEL, _PTR_ ADT_CHANNEL_PTR;

/* ADC converter + trigger channel bundle structure */
typedef struct adc_channel_bundle {
   ADC_CHANNEL_PTR  adc_ch;
   ADT_CHANNEL_PTR  adt_ch;
   /* below, CPU specific members are used */
   _mqx_int         ch_num; /* this channel is A or B (0 or 1) ? */
   _mqx_int         adc_num; /* number of ADC */
   _mqx_int         periods_preset; /* how many dummy PDB periods must pass to readout result data */
   _mqx_int         periods;        /* how many dummy PDB periods elapsed */
    boolean         waste_channel;  /* workaround to waste all channel results */
} ADC_CHANNEL_BUNDLE, _PTR_ ADC_CHANNEL_BUNDLE_PTR;

/* ADC converter + trigger driver bundle structure */
typedef struct adc_driver_bundle {
   ADC_PTR          adc;                      /* ADC driver status information (generic + HW specific) */
   ADT_PTR          adt;                      /* ADT driver status information (generic + HW specific) */
   ADC_CHANNEL_PTR  adc_ch[ADC_MAX_CHANNELS];
   ADT_CHANNEL_PTR  adt_ch[ADC_MAX_CHANNELS];
   uchar_ptr        adc_name[ADC_MAX_CHANNELS]; /* filename of channel(s) */
   /* below, CPU specific members are used */
   struct kadc_install_struct *init;
} ADC_DRIVER_BUNDLE, _PTR_ ADC_DRIVER_BUNDLE_PTR;

typedef struct adc_bundle {
/* Following must be defined, because following values should be written in one uninterruptable block of code, so we cannot
** split IOCTL command to ADC and ADT. There is posibility that ADC module disables interrupts and ADT module enables interrupt,
** but that's not the right solution.
*/
   uint_32 late_process; /* This is mask of late processing channel, constrained to 32 channels. */
} ADC_BUNDLE, ADC_BUNDLE_PTR;

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
_mqx_int _adt_hw_run(void);
_mqx_int _adt_hw_stop(void);
void _adt_hw_set_wait(uint_32);
uint_32 _adt_hw_get_time(void);

void adc_process_all_data(void);

#ifdef __cplusplus
}
#endif

#endif /* __adc_kadc_prv_h__ */
