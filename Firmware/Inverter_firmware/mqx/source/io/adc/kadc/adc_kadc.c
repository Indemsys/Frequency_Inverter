/**HEADER***********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
********************************************************************************
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
********************************************************************************
*
* $FileName: adc_kadc.c$
* $Version : 3.8.15.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the ADC driver CPU specific functions
*
*END***************************************************************************/

#include <mqx.h>

#include <bsp.h>
#include "adc_conf.h"
#include "adc_prv.h"
#include "adc_kadc_prv.h"

/* Declarations of IRQ handlers */
static void adc_complete_isr(pointer);

/* static variables for ADC driver */
static struct {
    /* Mask for used channels- each ADC module reserves ADC_CHANNELS_PER_ADC bits
    ** in this 32-bits longword, starting with least significant bits reserved
    ** for ADC0.
    */
    uint_32  used_channel;  /* mask of used channels */
    ADC_CHANNEL_BUNDLE_PTR open_channels[ADC_CHANNELS_PER_ADC][ADC_MAX_MODULES];
} adc_internal = { 0 };

ADC_BUNDLE adc_bundle;

static _mqx_int adc_set_calibration_data(_mqx_int adc_num, KADC_CALIB_STRUCT_PTR calib_ptr);
static _mqx_int adc_get_calibration_data(_mqx_int adc_num, KADC_CALIB_STRUCT_PTR calib_ptr);
static _mqx_int adc_calibrate(_mqx_int adc_num);
static void adc_process_data(_mqx_int adc_num, _mqx_int ch_num);

/*FUNCTION**********************************************************************
*
* Function Name    : _adc_hw_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END***************************************************************************/
_mqx_int _adc_hw_ioctl(MQX_FILE_PTR f, _mqx_uint cmd, pointer param_ptr)
{
    ADC_MemMapPtr adc_ptr;
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    _mqx_int adc_num = adc_d_bundle->init->ADC_NUMBER;
    volatile uint32_t *status_reg;

    switch (cmd)
    {
        case ADC_IOCTL_CALIBRATE:
            /* calibrate ADC autonomously */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* cannot run calibration on channel */
            if ((adc_internal.used_channel >> (ADC_MAX_CHANNELS * adc_num)) & ((1 << ADC_MAX_CHANNELS) - 1))
                return ADC_ERROR_NONEMPTY; /* calibration can be run only if no channel of ADC is opened */
            return adc_calibrate(adc_num);

        case ADC_IOCTL_SET_CALIBRATION:
            /* set calibration values to ADC without performing calibration */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* cannot set calibration data on channel */
            return adc_set_calibration_data(adc_num, (KADC_CALIB_STRUCT_PTR) param_ptr);

        case ADC_IOCTL_GET_CALIBRATION:
            /* gets calibrated value */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* cannot get calibration data on channel */
            return adc_get_calibration_data(adc_num, (KADC_CALIB_STRUCT_PTR) param_ptr);

        case ADC_IOCTL_SET_LONG_SAMPLE:
            /* sets sampling time for ADC module for long; param_ptr = (uint_32) cycles - see ADLSMP in Reference Manual */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* long sample must be set on device file, not on channel file */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);

            adc_ptr->CFG2 &= ~ADC_CFG2_ADLSTS_MASK;
            if ((uint_32) param_ptr == 20)
                adc_ptr->CFG2 |= ADC_CFG2_ADLSTS_20;
            else if ((uint_32) param_ptr == 12)
                adc_ptr->CFG2 |= ADC_CFG2_ADLSTS_12;
            else if ((uint_32) param_ptr == 6)
                adc_ptr->CFG2 |= ADC_CFG2_ADLSTS_6;
            else if ((uint_32) param_ptr == 2)
                adc_ptr->CFG2 |= ADC_CFG2_ADLSTS_2;
            else
                return ADC_ERROR_BAD_PARAM; /* invalid parameter for ioctl command */

            adc_ptr->CFG1 |= ADC_CFG1_ADLSMP_MASK;
            break;

        case ADC_IOCTL_SET_SHORT_SAMPLE:
            /* sets sampling time for ADC module for short - see ADLSMP in Reference Manual */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* long sample must be set on device file, not on channel file */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);

            adc_ptr->CFG1 &= ~ADC_CFG1_ADLSMP_MASK;
            break;

        case ADC_IOCTL_SET_HIGH_SPEED:
            /* sets high speed ADC module - see ADHSC bit in Reference Manual */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* high speed must be set on device file, not on channel file */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);

            adc_ptr->CFG2 |= ADC_CFG2_ADHSC_MASK;
            break;

        case ADC_IOCTL_SET_LOW_SPEED:
            /* sets low speed ADC module - see ADHSC bit in Reference Manual */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* low speed must be set on device file, not on channel file */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);

            adc_ptr->CFG2 &= ~ADC_CFG2_ADHSC_MASK;
            break;

        case ADC_IOCTL_SET_HW_AVERAGING:
            /* sets sample averaging by hardware; param_ptr = (uint_32) cycles - see AVGE bit in Reference Manual */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* averaging must be set on device file, not on channel file */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);

            if ((uint_32) param_ptr == 0)
                adc_ptr->SC3 &= ~ADC_SC3_AVGE_MASK; /* if zero cycles, disable averaging */
            else {
                adc_ptr->SC3 &= ~ADC_SC3_AVGS_MASK;
                if ((uint_32) param_ptr == 32)
                    adc_ptr->SC3 |= ADC_SC3_AVGS_32 | ADC_SC3_AVGE_MASK | ADC_SC3_ADCO_MASK;
                else if ((uint_32) param_ptr == 16)
                    adc_ptr->SC3 |= ADC_SC3_AVGS_16 | ADC_SC3_AVGE_MASK | ADC_SC3_ADCO_MASK;
                else if ((uint_32) param_ptr == 8)
                    adc_ptr->SC3 |= ADC_SC3_AVGS_8 | ADC_SC3_AVGE_MASK | ADC_SC3_ADCO_MASK;
                else if ((uint_32) param_ptr == 4)
                    adc_ptr->SC3 |= ADC_SC3_AVGS_4 | ADC_SC3_AVGE_MASK | ADC_SC3_ADCO_MASK;
                else
                    return ADC_ERROR_BAD_PARAM; /* invalid parameter for ioctl command */
            }
            break;

        case ADC_IOCTL_SET_IDELAY_PROCESS:
            /* Postpones channel data processing */
            if (adc_ch_bundle == NULL)
                return ADC_ERROR_ONLY_CHANNEL; /* data processing can be postponed only on channel files */

            _int_disable();
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            status_reg = &adc_ptr->SC1[adc_ch_bundle->ch_num];
            /* Before writing to register to disable COCO channel :) interrupt, we have to check COCO bit;
            ** this is needed as any write to register will clear COCO bit.
            ** Question: what if COCO bit is set after it was checked for? Answer: no other way, we lose one sample...
            */
            while (*status_reg & ADC_SC1_COCO_MASK) {
                /* COCO bit is set, we have to read data before disabling interrupts */
                adc_process_data(adc_num, adc_ch_bundle->ch_num);
            }
            *status_reg &= ~ADC_SC1_AIEN_MASK; /* disable interrupts, this also clears COCO bit */
            adc_bundle.late_process |= (1 << (ADC_MAX_CHANNELS * adc_num + adc_ch_bundle->ch_num));
            _int_enable();
            break;

        case ADC_IOCTL_SET_INT_PROCESS:
            /* Postpones channel data processing */
            if (adc_ch_bundle == NULL)
                return ADC_ERROR_ONLY_CHANNEL; /* data processing can be postponed only on channel files */

            _int_disable();
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            status_reg = &adc_ptr->SC1[adc_ch_bundle->ch_num];
            /* Before writing to register to disable COCO channel :) interrupt, we have to check COCO bit;
            ** this is needed as any write to register will clear COCO bit.
            ** Question: what if COCO bit is set after it was checked for? Answer: no other way, we lose one sample...
            */
            while (*status_reg & ADC_SC1_COCO_MASK) {
                /* COCO bit is set, we have to read data before disabling interrupts */
                adc_process_data(adc_num, adc_ch_bundle->ch_num);
            }
            *status_reg |= ADC_SC1_AIEN_MASK; /* enable interrupts, this also clears COCO bit */
            adc_bundle.late_process &= ~(1 << (ADC_MAX_CHANNELS * adc_num + adc_ch_bundle->ch_num));
            _int_enable();
            break;

        case ADC_IOCTL_SET_OFFSET: {
            uint_32 uparam_32 = (uint_32) param_ptr;
            uint_16 uparam_16 = (uint_16) uparam_32;

            /* Set offset for ADC module */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* Offset cannot be set on channel files */
            if (uparam_32 > MAX_UINT_16)
                return ADC_ERROR_BAD_PARAM; /* Cannot set offset that exceeds 16 bits */

            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            adc_ptr->OFS = uparam_16;
            break;
        }

        case ADC_IOCTL_SET_PLUS_GAIN: {
            uint_32 uparam_32 = (uint_32) param_ptr;
            uint_16 uparam_16 = (uint_16) uparam_32;

            /* Set offset for ADC module */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* Gain cannot be set on channel files */
            if (uparam_32 > MAX_UINT_16)
                return ADC_ERROR_BAD_PARAM; /* Cannot set gain that exceeds 16 bits */

            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            adc_ptr->PG = uparam_16;
            break;
        }
#if ADC_HAS_DIFFERENTIAL_CHANNELS
        case ADC_IOCTL_SET_MINUS_GAIN: {
            uint_32 uparam_32 = (uint_32) param_ptr;
            uint_16 uparam_16 = (uint_16) uparam_32;

            /* Set offset for ADC module */
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* Gain cannot be set on channel files */
            if (uparam_32 > MAX_UINT_16)
                return ADC_ERROR_BAD_PARAM; /* Cannot set gain that exceeds 16 bits */

            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            adc_ptr->MG = uparam_16;
            break;
        }
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
        case ADC_IOCTL_SET_REFERENCE: {
            if (adc_ch_bundle != NULL)
                return ADC_ERROR_ONLY_DEVICE; /* Gain cannot be set on channel files */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            adc_ptr->SC2 &= ~ADC_SC2_REFSEL_MASK;
            if ((uint_32) param_ptr == ADC_REF_VREF)
                adc_ptr->SC2 |= ADC_SC2_REFSEL_VREF;
            else if ((uint_32) param_ptr == ADC_REF_VALT)
                adc_ptr->SC2 |= ADC_SC2_REFSEL_VALT;
            else if ((uint_32) param_ptr == ADC_REF_VBG)
                adc_ptr->SC2 |= ADC_SC2_REFSEL_VBG;
            else
                return ADC_ERROR_BAD_PARAM; /* Cannot set any other reference */
            break;
        }
#if ADC_HAS_PGA
        case ADC_IOCTL_SET_PGA_GAIN:
            if (adc_ch_bundle == NULL)
                return ADC_ERROR_ONLY_CHANNEL; /* Gain cannot be set on device files */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            if (ADC_GET_CHANNEL(adc_ch_bundle->adc_ch->g.source) != ADC_SOURCE_CHANNEL(2))
                return ADC_ERROR_NOT_SUPPORTED;
            adc_ptr->PGA &= ~ADC_PGA_PGAG_MASK; /* zero the PGAG field */
            adc_ptr->PGA |= (uint_32) param_ptr | 0x00100000 | ADC_PGA_PGAEN_MASK; /* ADC_PGA_PGALP_MASK parameter follows register structure, enable PGA */
            break;

        case ADC_IOCTL_GET_PGA_GAIN: {
            uint_32 *param32_ptr = param_ptr;

            if (param32_ptr == NULL)
                return ADC_ERROR_PARAM; /* Missing pointer parameter where to store result */
            if (adc_ch_bundle == NULL)
                return ADC_ERROR_ONLY_CHANNEL; /* Gain cannot be set on device files */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            if (ADC_GET_CHANNEL(adc_ch_bundle->adc_ch->g.source) != ADC_SOURCE_CHANNEL(2))
                return ADC_ERROR_NOT_SUPPORTED;
            *param32_ptr = adc_ptr->PGA & ADC_PGA_PGAG_MASK;
            break;
        }

        case ADC_IOCTL_ENABLE_CHOPPING:
            if (adc_ch_bundle == NULL)
                return ADC_ERROR_ONLY_CHANNEL; /* Gain cannot be set on device files */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            if (ADC_GET_CHANNEL(adc_ch_bundle->adc_ch->g.source) != ADC_SOURCE_CHANNEL(2))
                return ADC_ERROR_NOT_SUPPORTED;
            adc_ptr->PGA |= 0x00200000; //ADC_PGA_PGACHP_MASK; /* enable chopping on PGA */
            break;

        case ADC_IOCTL_DISABLE_CHOPPING:
            if (adc_ch_bundle == NULL)
                return ADC_ERROR_ONLY_CHANNEL; /* Gain cannot be set on device files */
            adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
            if (ADC_GET_CHANNEL(adc_ch_bundle->adc_ch->g.source) != ADC_SOURCE_CHANNEL(2))
                return ADC_ERROR_NOT_SUPPORTED;
            adc_ptr->PGA &= ~0x00200000; //~ADC_PGA_PGACHP_MASK; /* enable chopping on PGA */
            break;
#endif /* ADC_HAS_PGA */

        default:
            return IO_ERROR_INVALID_IOCTL_CMD;
    }

    return ADC_OK;
}

/*FUNCTION**********************************************************************
*
* Function Name    : _adc_hw_install
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific initialization for ADC driver
*
*END***************************************************************************/
_mqx_int _adc_hw_install(char_ptr identifier, ADC_DRIVER_BUNDLE_PTR adc_d_bundle, pointer init)
{
    adc_d_bundle->init = (KADC_INIT_STRUCT_PTR) init;

#if PE_LDD_VERSION
    if (PE_PeripheralUsed((uint_32)_bsp_get_adc_base_address(adc_d_bundle->init->ADC_NUMBER)))
    {
        return IO_ERROR;
    }
#endif

    return ADC_OK;
}

/*FUNCTION**********************************************************************
*
* Function Name    : _adc_hw_init
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific initialization for ADC driver
*
*END***************************************************************************/
_mqx_int _adc_hw_init(MQX_FILE_PTR f, ADC_PTR adc_d)
{
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    _mqx_int adc_num = adc_d_bundle->init->ADC_NUMBER;
    ADC_MemMapPtr adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
    uint_32 adc_max_frq;
    uint_8  adc_max_frq_index;

    /* This table is made from datasheet values for ADC module in Kinetis */
    const static uint_32 adc_max_frq_table[] = {
        2500000,  /* 2.5 MHz  for low power,    normal speed, 16b resolution */
        5000000,  /* 5.0 MHz  for low power,    normal speed, lower resolution */
        5000000,  /* 5.0 MHz  for low power,    high speed,   16b resolution */
        8000000,  /* 8.0 MHz  for low power,    high speed,   lower resolution */
        8000000,  /* 8.0 MHz  for normal power, normal speed, 16b resolution */
        12000000, /* 12.0 MHz for normal power, normal speed, lower resolution */
        12000000, /* 12.0 MHz for normal power, high speed,   16b resolution */
        18000000, /* 18.0 MHz for normal power, high speed,   lower resolution */
    };

    if (adc_ptr == NULL) {
        return IO_ERROR; /* Cannot find specific ADC module */
    }

    /* First, use 3 bit mask to create index to the max. frq. table */
    adc_max_frq_index = 0;
    switch (adc_d->g.resolution) {
        case ADC_RESOLUTION_16BIT:
            break;
        case ADC_RESOLUTION_12BIT:
        case ADC_RESOLUTION_10BIT:
        case ADC_RESOLUTION_8BIT:
            adc_max_frq_index |= 0x01;
            break;
        default:
            return ADC_ERROR_BAD_PARAM; /* bad resolution set */
    }
    if (adc_d_bundle->init->SPEED == ADC_HSC_HIGH)
        adc_max_frq_index |= 0x02;
    if (adc_d_bundle->init->POWER == ADC_LPC_NORMAL)
        adc_max_frq_index |= 0x04;
    adc_max_frq = adc_max_frq_table[adc_max_frq_index];

    (void) _bsp_adc_io_init(adc_num);

    /* set AD clock to be as fast as possible */
    switch (adc_d_bundle->init->CLOCK_SOURCE) {
        case ADC_CLK_BUSCLK_ANY:
            if (adc_d_bundle->init->CLOCK_DIV == ADC_DIV_ANY) {
                if(BSP_BUS_CLOCK <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_1 | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 2) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_2 | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 4) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_4 | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 8) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_8 | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_8 | ADC_CFG1_ADICLK_BUSCLK2;
                else
                    return ADC_ERROR_BAD_PARAM; /* cannot set ADC clock to be less than adc_max_frq */
            }
            else {
                if ((BSP_BUS_CLOCK / (1 << adc_d_bundle->init->CLOCK_DIV)) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV(adc_d_bundle->init->CLOCK_DIV) | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 2 / (1 << adc_d_bundle->init->CLOCK_DIV)) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV(adc_d_bundle->init->CLOCK_DIV) | ADC_CFG1_ADICLK_BUSCLK2;
                else
                    return ADC_ERROR_BAD_PARAM; /* such clock combination is too fast for ADC */
            }
            break;

        case ADC_CLK_BUSCLK:
            if (adc_d_bundle->init->CLOCK_DIV == ADC_DIV_ANY) {
                if (BSP_BUS_CLOCK <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_1 | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 2) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_2 | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 4) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_4 | ADC_CFG1_ADICLK_BUSCLK;
                else if ((BSP_BUS_CLOCK / 8) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_8 | ADC_CFG1_ADICLK_BUSCLK;
                else
                    return ADC_ERROR_BAD_PARAM; /* cannot set ADC clock to be less than ADC max. frequency */
            }
            else if ((BSP_BUS_CLOCK / (1 << adc_d_bundle->init->CLOCK_DIV)) <= adc_max_frq)
                adc_ptr->CFG1 = ADC_CFG1_ADIV(adc_d_bundle->init->CLOCK_DIV) | ADC_CFG1_ADICLK_BUSCLK;
            else
                return ADC_ERROR_BAD_PARAM; /* such clock combination is too fast for ADC */
            break;

        case ADC_CLK_BUSCLK2:
            if (adc_d_bundle->init->CLOCK_DIV == ADC_DIV_ANY) {
                if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                    adc_ptr->CFG1 = ADC_CFG1_ADIV_8 | ADC_CFG1_ADICLK_BUSCLK2;
                if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                    return ADC_ERROR_BAD_PARAM; /* cannot set ADC clock to be less than ADC max. frequency */
            }
            else if ((BSP_BUS_CLOCK / 2 / (1 << adc_d_bundle->init->CLOCK_DIV)) <= adc_max_frq)
                adc_ptr->CFG1 = ADC_CFG1_ADIV(adc_d_bundle->init->CLOCK_DIV) | ADC_CFG1_ADICLK_BUSCLK2;
            else
                return ADC_ERROR_BAD_PARAM; /* such clock combination is too fast for ADC */
            break;

        case ADC_CLK_ALTERNATE:
            if (adc_d_bundle->init->CLOCK_DIV == ADC_DIV_ANY)
                return ADC_ERROR_BAD_PARAM; /* alternate clock + any division is not supported now */
            adc_ptr->CFG1 = ADC_CFG1_ADIV(adc_d_bundle->init->CLOCK_DIV) | ADC_CFG1_ADICLK_ALTCLK;
            break;

        case ADC_CLK_ASYNC:
            if (adc_d_bundle->init->CLOCK_DIV == ADC_DIV_ANY)
                return ADC_ERROR_BAD_PARAM; /* async clock + any division is not supported now */
            adc_ptr->CFG1 = ADC_CFG1_ADIV(adc_d_bundle->init->CLOCK_DIV) | ADC_CFG1_ADICLK_ADACK;
            break;

        default:
            return ADC_ERROR_BAD_PARAM; /* invalid clock source */
    }

    adc_ptr->CFG1 |= adc_d->g.resolution;

    adc_ptr->SC2 |= ADC_SC2_ADTRG_MASK; /* set HW trigger */

    adc_d->old_isr_data = (pointer) _int_get_isr_data(adc_d_bundle->init->ADC_VECTOR);
    adc_d->old_isr = _int_install_isr(adc_d_bundle->init->ADC_VECTOR, adc_complete_isr, f);
    /* Enable interrupt in NVIC */
    _bsp_int_init(adc_d_bundle->init->ADC_VECTOR, adc_d_bundle->init->ADC_PRIORITY, 0, TRUE);

    return ADC_OK;
}

/*FUNCTION**********************************************************************
*
* Function Name    : _adc_hw_deinit
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific deinitialization for ADC driver
*
*END***************************************************************************/
_mqx_int _adc_hw_deinit(MQX_FILE_PTR f, ADC_PTR adc_d)
{
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;

    _int_install_isr(adc_d_bundle->init->ADC_VECTOR, adc_d->old_isr, adc_d->old_isr_data);

    return ADC_OK;
}

/*FUNCTION**********************************************************************
*
* Function Name    : _adc_cpu_channel_init
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END***************************************************************************/
_mqx_int _adc_hw_channel_init(MQX_FILE_PTR f, ADC_CHANNEL_PTR adc_ch)
{
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    _mqx_int adc_num = adc_d_bundle->init->ADC_NUMBER;
    _mqx_int ch_num;
    ADC_MemMapPtr adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);
    _mqx_uint used_ch = adc_internal.used_channel >> (ADC_MAX_CHANNELS * adc_num);
    uint32_t source_reg;

    adc_ch->b = adc_ch_bundle;

    source_reg = (uint32_t) ADC_GET_CHANNEL(adc_ch->g.source);
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    if (ADC_GET_DIFF(adc_ch->g.source))
        source_reg |= ADC_SC1_DIFF_MASK;
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
    source_reg |= ADC_SC1_AIEN_MASK;

    for (ch_num = 0; ch_num < ADC_CHANNELS_PER_ADC; ch_num++)
        if (!(used_ch & (1 << ch_num)))
            /* channel is free, use it */
            break;
    if (ch_num == ADC_CHANNELS_PER_ADC)
        return ADC_ERROR_FULL; /* all channels are used, no more allowed; shouldn't happen */

    adc_ptr->SC1[ch_num] = source_reg;

    adc_ch_bundle->adc_num = adc_num;
    adc_ch_bundle->ch_num = ch_num;
    adc_internal.used_channel |= 0x01 << (ADC_MAX_CHANNELS * adc_num + ch_num);

    adc_internal.open_channels[ch_num][adc_num] = adc_ch_bundle;

    /* Test if needed module for this source is the one opened */
    if ((ADC_SOURCE_MODULE_ANY != (ADC_GET_MODULE(adc_ch->g.source) >> ADC_SOURCE_MODULE_SHIFT)) \
        && ((adc_num + 1) != (ADC_GET_MODULE(adc_ch->g.source) >> ADC_SOURCE_MODULE_SHIFT)))
        return IO_ERROR; /* cannot assign the source to this ADC module */

    /* Test if muxsel was not selected yet or is the same as selected */
    if ((0 == adc_d_bundle->adc->muxsel) \
        || (ADC_GET_MUXSEL(adc_ch->g.source) == ((uint_16) adc_d_bundle->adc->muxsel << ADC_SOURCE_MUXSEL_SHIFT)))
    {
        _mqx_int status = _bsp_adc_channel_io_init(adc_ch->g.source);
        if (IO_OK == status) {
            if (0 == adc_d_bundle->adc->muxsel)
                adc_d_bundle->adc->muxsel = ADC_GET_MUXSEL(adc_ch->g.source) >> ADC_SOURCE_MUXSEL_SHIFT;
#if PSP_MQX_CPU_IS_KINETIS
            if (1 == adc_d_bundle->adc->muxsel) {
                /* channel A selected */
                adc_ptr->CFG2 &= ~ADC_CFG2_MUXSEL_MASK;
            }
            else if (2 == adc_d_bundle->adc->muxsel) {
                /* channel B selected */
                adc_ptr->CFG2 |= ADC_CFG2_MUXSEL_MASK;
            }
#endif /* PSP_CPU_IS_KINETIS */
#if ADC_HAS_PGA
            if (2 == ADC_GET_CHANNEL(adc_ch->g.source))
                adc_ptr->PGA = (uint_32) ADC_PGA_PGAG(0) | 0x00100000 | ADC_PGA_PGAEN_MASK; /* on channel 2, switch on PGA */
#endif /* ADC_HAS_PGA */
        }
        return status;
    }
    else
        return IO_ERROR; /* cannot set this multiplexer as opposite A/B muxsel was already selected on module */
}

/*FUNCTION**********************************************************************
*
* Function Name    : _adc_hw_channel_deinit
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific deinitialization for ADC channel
*
*END***************************************************************************/
_mqx_int _adc_hw_channel_deinit(MQX_FILE_PTR f, ADC_CHANNEL_PTR adc_ch)
{
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    _mqx_int adc_num = adc_ch_bundle->adc_num;
    _mqx_int ch_num = adc_ch_bundle->ch_num;
    ADC_MemMapPtr adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);

    /* deinit channel */
    _int_disable();
    adc_ptr->SC1[ch_num] = ADC_SC1_ADCH_DISABLED;
    adc_bundle.late_process &= ~(0x01 << (ADC_MAX_CHANNELS * adc_num + ch_num));
    _int_enable();

    adc_internal.used_channel &= ~(0x01 << (ADC_MAX_CHANNELS * adc_num + ch_num));
    adc_internal.open_channels[ch_num][adc_num] = NULL;

    return ADC_OK;
}

/*FUNCTION**********************************************************************
*
* Function Name    : adc_process_data
* Returned Value   : void
* Comments         : Process data result for specific channel
*
*END***************************************************************************/
static void adc_process_data(_mqx_int adc_num, _mqx_int ch_num)
{
    ADC_CHANNEL_PTR        adc_ch;
    ADT_CHANNEL_PTR        adt_ch;
    ADC_MemMapPtr adc_ptr;
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle;

    adc_ptr = (ADC_MemMapPtr) _bsp_get_adc_base_address(adc_num);

    if (!(adc_ptr->SC1[ch_num] & ADC_SC1_COCO_MASK))
        return; /* conversion was not completed on this channel */

    adc_ch_bundle = adc_internal.open_channels[ch_num][adc_num];
    adc_ch = adc_ch_bundle->adc_ch;

    if (adc_ch_bundle->periods != 0) {
        volatile uint_32 dummy_read;
        /* dummy read to clear interrupt flag */
        dummy_read = adc_ptr->R[ch_num];
        adc_ch_bundle->periods--;
    } else {
        adc_ch_bundle->periods = adc_ch_bundle->periods_preset - 1;

        if (adc_ch->g.init_flags & ADC_CHANNEL_ACCUMULATE)
            /* if the channel has ADC_CHANNEL_ACCUMULATE, then accumulate result */
            adc_ch->g.buffer_driver->result += adc_ptr->R[ch_num];
        else
            /* else copy the result to the channel buffer */
            adc_ch->g.buffer_driver->result = adc_ptr->R[ch_num];

        adt_ch = adc_ch_bundle->adt_ch;
        if (!(adc_ch->g.init_flags & ADC_CHANNEL_ACCUMULATE) || !(adt_ch->g.samples)) {
            /* move driver pointer to next index, circularly */
            if (++adc_ch->g.buffer_driver >= adc_ch->g.buffer_end)
                adc_ch->g.buffer_driver = adc_ch->g.buffer_start;
            /* if new value should overwrite old value in ring buffer, move read pointer to next value */
            if (adc_ch->g.buffer_driver == adc_ch->g.buffer_task) {
                if (++adc_ch->g.buffer_task >= adc_ch->g.buffer_end)
                    adc_ch->g.buffer_task = adc_ch->g.buffer_start;
            }
        }
        _adt_process_data(adc_ch_bundle);
    }
}

/*FUNCTION**********************************************************************
*
* Function Name    : adc_process_all_data
* Returned Value   : void
* Comments         : Look at all channels and finds
*
*END***************************************************************************/
void adc_process_all_data(void)
{
    _mqx_int adc_num, ch_num;

    /* conversion done, but which channel? */
    for (adc_num = 0; adc_num < ADC_MAX_MODULES; adc_num++) {
        for (ch_num = 0; ch_num < ADC_MAX_CHANNELS; ch_num++) {
            if (adc_internal.open_channels[ch_num][adc_num] == NULL)
                continue;
            adc_process_data(adc_num, ch_num);
        }
    }
}

/*FUNCTION**********************************************************************
*
* Function Name    : adc_complete_isr
* Returned Value   : void
* Comments         : IRQ handler for completed AD conversion(s)
*
*END***************************************************************************/
void adc_complete_isr
(
      /* [IN] the file pointer */
      pointer param
)
{ /* Body */
    MQX_FILE_PTR f = (MQX_FILE_PTR) param;
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    _mqx_int adc_num = adc_d_bundle->init->ADC_NUMBER;
    _mqx_int ch_num;

    for (ch_num = 0; ch_num < ADC_MAX_CHANNELS; ch_num++) {
        adc_process_data(adc_num, ch_num);
    }
}

/*FUNCTION**********************************************************************
*
* Function Name    : adc_calibrate
* Returned Value   : ADC_OK or ADC_ERROR
* Comments         : Calibrates the ADC module
*
*END***************************************************************************/
static _mqx_int adc_calibrate(_mqx_int adc_index)
{
    uint_16 cal_var;
    ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(adc_index);
    struct {
        uint_8  SC2;
        uint_8  SC3;
    } saved_regs;

    if (adc_ptr == NULL)
        return ADC_ERROR_BAD_PARAM; /* no such ADC peripheral exists */

    saved_regs.SC2 = adc_ptr->SC2;
    saved_regs.SC3 = adc_ptr->SC3;

    /* Enable Software Conversion Trigger for Calibration Process */
    adc_ptr->SC2 &= ~ADC_SC2_ADTRG_MASK;

    /* Initiate calibration */
    adc_ptr->SC3 |= ADC_SC3_CAL;

    /* Wait for conversion to complete */
    while (adc_ptr->SC2 & ADC_SC2_ADACT_MASK){}

    /* Check if calibration failed */
    if (adc_ptr->SC3 & ADC_SC3_CALF_MASK)  {
        /* Clear calibration failed flag */
        adc_ptr->SC3 |= ADC_SC3_CALF_MASK;

        /* calibration failed */
        return -1;
    }

    /* Calculate plus-side calibration values according
     * to Calibration function described in reference manual.   */
    /* 1. Initialize (clear) a 16b variable in RAM */
    cal_var  = 0x0000;
    /* 2. Add the following plus-side calibration results CLP0, CLP1,
     *    CLP2, CLP3, CLP4 and CLPS to the variable. */
    cal_var +=  adc_ptr->CLP0;
    cal_var +=  adc_ptr->CLP1;
    cal_var +=  adc_ptr->CLP2;
    cal_var +=  adc_ptr->CLP3;
    cal_var +=  adc_ptr->CLP4;
    cal_var +=  adc_ptr->CLPS;

    /* 3. Divide the variable by two. */
    cal_var = cal_var / 2;

    /* 4. Set the MSB of the variable. */
    cal_var += 0x8000;

    /* 5. Store the value in the plus-side gain calibration registers ADCPGH and ADCPGL. */
    adc_ptr->PG = cal_var;
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    /* Repeat procedure for the minus-side gain calibration value. */
    /* 1. Initialize (clear) a 16b variable in RAM */
    cal_var  = 0x0000;

    /* 2. Add the following plus-side calibration results CLM0, CLM1,
     *    CLM2, CLM3, CLM4 and CLMS to the variable. */
    cal_var  = 0x0000;
    cal_var += adc_ptr->CLM0;
    cal_var += adc_ptr->CLM1;
    cal_var += adc_ptr->CLM2;
    cal_var += adc_ptr->CLM3;
    cal_var += adc_ptr->CLM4;
    cal_var += adc_ptr->CLMS;

    /* 3. Divide the variable by two. */
    cal_var = cal_var / 2;

    /* 4. Set the MSB of the variable. */
    cal_var += 0x8000;

    /* 5. Store the value in the minus-side gain calibration registers ADCMGH and ADCMGL. */
    adc_ptr->MG = cal_var;
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
    /* Clear CAL bit */
    adc_ptr->SC3 = saved_regs.SC3;
    adc_ptr->SC2 = saved_regs.SC2;

    return ADC_OK;
}

/*FUNCTION**********************************************************************
*
* Function Name    : adc_get_calibration_data
* Returned Value   : ADC_OK or ADC_ERROR
* Comments         : Calibrates the ADC.
*    Gets (copies) calibrated data from ADC registers to memory
*
*END***************************************************************************/
static _mqx_int adc_get_calibration_data
(    /* ADC module index */
    _mqx_int                       adc_index,
    /* pointer to structure to read the calibration data */
    KADC_CALIB_STRUCT_PTR          calib_ptr
)
{
    ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(adc_index);

    if (calib_ptr == NULL)
        return ADC_ERROR_PARAM; /* pointer to store calibration data to is NULL */
    if (adc_ptr == NULL)
        return ADC_ERROR_BAD_PARAM; /* no such ADC peripheral exists */

    calib_ptr->OFS  = adc_ptr->OFS;
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    calib_ptr->PG   = adc_ptr->PG;
    calib_ptr->MG   = adc_ptr->MG;
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
    calib_ptr->CLPD = adc_ptr->CLPD;
    calib_ptr->CLPS = adc_ptr->CLPS;
    calib_ptr->CLP4 = adc_ptr->CLP4;
    calib_ptr->CLP3 = adc_ptr->CLP3;
    calib_ptr->CLP2 = adc_ptr->CLP2;
    calib_ptr->CLP1 = adc_ptr->CLP1;
    calib_ptr->CLP0 = adc_ptr->CLP0;
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    calib_ptr->CLMD = adc_ptr->CLMD;
    calib_ptr->CLMS = adc_ptr->CLMS;
    calib_ptr->CLM4 = adc_ptr->CLM4;
    calib_ptr->CLM3 = adc_ptr->CLM3;
    calib_ptr->CLM2 = adc_ptr->CLM2;
    calib_ptr->CLM1 = adc_ptr->CLM1;
    calib_ptr->CLM0 = adc_ptr->CLM0;
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
    return ADC_OK;
}

/*FUNCTION**********************************************************************
*
* Function Name    : adc_set_calibration_data
* Returned Value   : ADC_OK or ADC_ERROR
* Comments         : Calibrates the ADC.
*    Sets directly calibration data to calibration registers
*
*END***************************************************************************/
static _mqx_int adc_set_calibration_data
(    /* ADC module index */
    _mqx_int                       adc_index,
    /* pointer to structure to read the calibration data */
    KADC_CALIB_STRUCT_PTR          calib_ptr
)
{
    ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(adc_index);

    if (calib_ptr == NULL)
        return ADC_ERROR_PARAM; /* pointer to store calibration data to is NULL */
    if (adc_ptr == NULL)
        return ADC_ERROR_BAD_PARAM; /* no such ADC peripheral exists */

    adc_ptr->OFS  = calib_ptr->OFS;
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    adc_ptr->PG   = calib_ptr->PG;
    adc_ptr->MG   = calib_ptr->MG;
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
    adc_ptr->CLPD = calib_ptr->CLPD;
    adc_ptr->CLPS = calib_ptr->CLPS;
    adc_ptr->CLP4 = calib_ptr->CLP4;
    adc_ptr->CLP3 = calib_ptr->CLP3;
    adc_ptr->CLP2 = calib_ptr->CLP2;
    adc_ptr->CLP1 = calib_ptr->CLP1;
    adc_ptr->CLP0 = calib_ptr->CLP0;
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    adc_ptr->CLMD = calib_ptr->CLMD;
    adc_ptr->CLMS = calib_ptr->CLMS;
    adc_ptr->CLM4 = calib_ptr->CLM4;
    adc_ptr->CLM3 = calib_ptr->CLM3;
    adc_ptr->CLM2 = calib_ptr->CLM2;
    adc_ptr->CLM1 = calib_ptr->CLM1;
    adc_ptr->CLM0 = calib_ptr->CLM0;
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */

    return ADC_OK;
}

