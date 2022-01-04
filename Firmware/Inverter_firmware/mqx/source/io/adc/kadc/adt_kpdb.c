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
* $FileName: adt_kpdb.c$
* $Version : 3.8.16.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the ADC driver CPU specific functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "adc_conf.h"
#include "adc_prv.h"
#include "adc_kadc_prv.h"

static volatile struct {
    int_8    prescaler;
} pdb_external = {
    -1 /* not configured yet */
};

/* static variables for ADT driver */
static volatile struct {
    uint_32  period;
    PDB_INT_FCN idelay_fcn;
    pointer old_idelay_isr;
    pointer old_idelay_isr_data;
    _mqx_int old_isr_valid; /* which of interrupts handlers above are valid */
    uint_32 used_channel[ADC_MAX_MODULES];  /* mask of used channels */
    uint_32 delayA[ADC_MAX_MODULES]; /* if a channel A is paused, the original value of delay stored here */
} pdb_internal = { 0 };

/* Following remembers last delay value and then writes DLY for ChA behind accessible boundary */
#define DISABLE_CH0_PROCESSING(adc_num, adc_ch_bundle) \
    do { \
    pdb_internal.delayA[adc_num] = PDB0_BASE_PTR->CH[adc_num].DLY[0]; \
        if (PDB0_BASE_PTR->MOD == PDB_MOD_MOD_MASK) { \
            PDB0_BASE_PTR->CH[adc_num].DLY[0] = PDB_MOD_MOD_MASK; \
        } \
        else { \
            PDB0_BASE_PTR->CH[adc_num].DLY[0] = PDB0_BASE_PTR->MOD + 1; \
        } \
    PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; \
        adc_ch_bundle->waste_channel = 1; \
    } while(0);

/* Following reuses last delay value and then stops wasting AD channel */
#define ENABLE_CH0_PROCESSING(adc_num, adc_ch_bundle) \
    do { \
    PDB0_BASE_PTR->CH[adc_num].DLY[0] = pdb_internal.delayA[adc_num]; \
    PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; \
        adc_ch_bundle->waste_channel = 0; \
    } while(0);

#if PSP_MQX_CPU_IS_KINETIS
    #define PDB_CH_EN(adc_num)\
        * (uint_8_ptr) (&PDB0_BASE_PTR->CH[adc_num].C1)
#else
    #define PDB_CH_EN(adc_num)\
        *(uint_8_ptr)((uint_32)&PDB0_BASE_PTR->CH[adc_num].C1 + 3)
#endif
extern ADC_BUNDLE adc_bundle;

static _mqx_int adt_period_prescaler(uint_32 period);
static void adt_idelay_isr(pointer param);

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_hw_ioctl
* Returned Value   : _mqx_int
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/
_mqx_int _adt_hw_ioctl
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR f,

    /* [IN] the ioctl command */
    _mqx_uint  cmd,

    /* [IN] the ioctl parameters */
    pointer    param_ptr
)
{
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    _mqx_uint adc_num = 0;
    uint_32 d, pdb_period;

    if (adc_ch_bundle != NULL)
        adc_num = adc_ch_bundle->adc_num;

    switch (cmd)
    {
        case ADC_IOCTL_SET_IDELAY:
            /* sets offset for interrupt in PDB module - see IDELAY register in Reference Manual */
            pdb_period = pdb_internal.period;
            if (pdb_period == 0)
                return ADC_ERROR_PERIOD; /* cannot set idelay if period was not set before */
            d = ((uint_64) param_ptr * PDB0_BASE_PTR->MOD) / pdb_period;
            if (d > MAX_UINT_16)
                return ADC_ERROR_BAD_PARAM; /* cannot set such offset */
            PDB0_BASE_PTR->IDLY = (uint32_t) d;
            break;

        case ADC_IOCTL_SET_IDELAYREG:
            /* sets offset for interrupt in PDB module - see IDELAY register in Reference Manual */
            if ((uint_32) param_ptr > MAX_UINT_16)
                return ADC_ERROR_BAD_PARAM; /* cannot set such idelay */
            PDB0_BASE_PTR->IDLY = (uint32_t) param_ptr;
            break;

        case ADC_IOCTL_SET_IDELAY_FCN:
            /* sets callback for interrupt in PDB module */
            pdb_internal.idelay_fcn = (PDB_INT_FCN) param_ptr;
            break;

        case ADC_IOCTL_SET_BASE_PERIOD:
            /* Correction of PDB period, delays are not checked against shortened period value */
            pdb_period = pdb_internal.period;
            if (pdb_period == 0) { /* period was not set yet? */
                return adt_period_prescaler((uint_32) param_ptr);
            }
            else { /* trim period if possible */
                d = (uint_32) ((uint_64) param_ptr * PDB0_BASE_PTR->MOD) / pdb_period - 1;
                if (d > MAX_UINT_16)
                    return ADC_ERROR_BAD_PARAM; /* cannot set such period */
                PDB0_BASE_PTR->MOD = (uint_16) d;
            }
            if ((adc_d_bundle->adt_ch[0] == NULL) || adc_d_bundle->adt_ch[0]->b->waste_channel)
                PDB0_BASE_PTR->CH[adc_num].DLY[0] = (PDB0_BASE_PTR->MOD == PDB_MOD_MOD_MASK ? PDB_MOD_MOD_MASK : PDB0_BASE_PTR->MOD + 1); /* move ChA behind accessible boundary */
            PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; /* write DLY value */
            break;

        case ADC_IOCTL_TRIM_BASE_PERIOD:
            /* Correction of PDB period, delays are not checked against shortened period value */
            if ((uint_32) param_ptr > MAX_UINT_16)
                return ADC_ERROR_BAD_PARAM; /* cannot set such period */
            PDB0_BASE_PTR->MOD = (uint32_t) param_ptr;
            if ((adc_d_bundle->adt_ch[0] == NULL) || adc_d_bundle->adt_ch[0]->b->waste_channel)
                PDB0_BASE_PTR->CH[adc_num].DLY[0] = (PDB0_BASE_PTR->MOD == PDB_MOD_MOD_MASK ? PDB_MOD_MOD_MASK : PDB0_BASE_PTR->MOD + 1); /* move ChA behind accessible boundary */
            PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; /* write MOD value */
            break;

        case ADC_IOCTL_SET_DELAYREG:
            /* Correction of delay registers */
            if (adc_ch_bundle == NULL)
                return ADC_ERROR_ONLY_CHANNEL; /* delays can be set only on channel files */
            if ((uint_32) param_ptr > MAX_UINT_16)
                return ADC_ERROR_PARAM; /* cannot set such delay */
            PDB0_BASE_PTR->CH[adc_num].DLY[adc_ch_bundle->ch_num] = (uint32_t) param_ptr;

            if (adc_ch_bundle->ch_num == 0)
                pdb_internal.delayA[adc_num] = PDB0_BASE_PTR->CH[adc_num].DLY[0]; /* remember last delay value */

            PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; /* write DLY value */
            break;

        case ADC_IOCTL_SET_TRIGGER:
            /* Set HW triggering options */
            PDB0_BASE_PTR->SC = (PDB0_BASE_PTR->SC & ~PDB_SC_TRGSEL_MASK) | PDB_SC_TRGSEL(param_ptr);
            break;

        default:
            return IO_ERROR_INVALID_IOCTL_CMD;
    }

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_hw_install
* Returned Value   : IO_OK
* Comments         :
*    Performs CPU specific initialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adt_hw_install(char_ptr adc_name, ADC_DRIVER_BUNDLE_PTR adc_d_bundle, pointer p)
{
    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_hw_init
* Returned Value   : IO_OK
* Comments         :
*    Performs CPU specific initialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adt_hw_init(MQX_FILE_PTR f, ADT_PTR adt_d)
{ /* Body */
    SIM_MemMapPtr sim_ptr = SIM_BASE_PTR;
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    _mqx_int adc_num = adc_d_bundle->init->ADC_NUMBER;
#if PSP_MQX_CPU_IS_KINETIS
    sim_ptr->SCGC6 |= SIM_SCGC6_PDB_MASK; /* enable PDB module clocking */
#else
    sim_ptr->SCGC3 |= SIM_SCGC3_PDB_MASK; /* enable PDB module clocking */
#endif /* PSP_MQX_CPU_IS_KINETIS*/

    PDB0_BASE_PTR->SC |= PDB_SC_PDBEN_MASK; /* enable PDB module, so we can write o registers with LDOK */

    if (!(pdb_internal.old_isr_valid & 0x01)) {
        pdb_internal.old_idelay_isr_data = (pointer) _int_get_isr_data(adc_d_bundle->init->PDB_INIT->IDELAY_VECTOR);
        pdb_internal.old_idelay_isr = (pointer) _int_install_isr(adc_d_bundle->init->PDB_INIT->IDELAY_VECTOR, adt_idelay_isr, f);
        /* Enable interrupt in NVIC */
        _bsp_int_init(adc_d_bundle->init->PDB_INIT->IDELAY_VECTOR, adc_d_bundle->init->PDB_INIT->IDELAY_PRIORITY, 0, TRUE);

        pdb_internal.old_isr_valid |= 0x01;
    }

    PDB0_BASE_PTR->SC &= ~PDB_SC_PDBIF_MASK; /* clear PDB interrupts */

    PDB0_BASE_PTR->SC &= ~PDB_SC_LDMOD_MASK;
    PDB0_BASE_PTR->SC |= PDB_SC_LDMOD(0x00); /* write to MOD and DLY registers immediately */

    PDB_CH_EN(adc_num) = 0; /* no channel enabled yet */
    PDB0_BASE_PTR->CH[adc_num].C1 |= PDB_C1_TOS_MASK; /* select pre-trigger as a function of delay for all channels */
    PDB0_BASE_PTR->CH[adc_num].C1 &= ~PDB_C1_BB_MASK; /* don't use back-to-back mode, use delayed channel conversions instead */

#if PSP_MQX_CPU_IS_KINETIS
    if (adc_num == 0) {
        sim_ptr->SOPT7 &= ~SIM_SOPT7_ADC0ALTTRGEN_MASK; /* set PDB to be source for ADC conversion start */
    }
#if defined(SIM_SOPT7_ADC1ALTTRGEN_MASK)
    else {
        sim_ptr->SOPT7 &= ~SIM_SOPT7_ADC1ALTTRGEN_MASK; /* set PDB to be source for ADC conversion start */
    }
#endif
#else
    if (adc_num == 0) {
        sim_ptr->SOPT7 &= ~SIM_SOPT7_ADTRGS_MASK; /* set PDB to be source for ADC conversion start */
    }
#endif /* PSP_MQX_CPU_IS_KINETIS*/

    return ADC_OK;
} /* Endbody */

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_hw_deinit
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific deinitialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adt_hw_deinit(MQX_FILE_PTR f, ADT_PTR adt) {
    pdb_external.prescaler = -1;
    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : adt_period_prescaler
* Returned Value   : IO_OK or IO_ERROR
* Comments         :
*    Finds the best prescaler for given period
*
*END*********************************************************************/
static _mqx_int adt_period_prescaler(uint_32 period)
{
    volatile uint_32 pdb_prescaler1, pdb_prescaler, pdb_period1, pdb_period = 0, pdb_period_test;
    uint_8 mult = 0; /* assume that we will not use MULT field */

    /* In the first step, we want to find for given a prescaler, which meets following criteria:
    ** considering MOD field = 65535 and, find a combination of (found_prescaler, found_period), in which found_period
    ** is minimal (so the found_prescaler is also lowest possible one), but found_period must be still higher
    ** than given period.
    ** We compute for 2 cases: MULT field is 0 and MULT field is not 0 and after that we compare those 2 results and we
    ** choose the better one (the one with valid lower found_period).
    ** At the end, we reduce MOD, to set given period for PDB. With this approach, MOD field gets the highest possible value.
    */
#define PDB_MAX_PERIOD1 (uint_32) ((uint_64) 8388608000000L / BSP_BUS_CLOCK)   /*  1 * 128 * 65536 * 1000000 = 8388608000000 */
#define PDB_MAX_PERIOD  (uint_32) ((uint_64) 335544320000000L / BSP_BUS_CLOCK) /* 40 * 128 * 65536 * 1000000 = 335544320000000 */

    /* PDB was not run till yet... so setup period */
    if (pdb_external.prescaler == -1) {
        /* PDB prescaler was not set yet */

        pdb_period_test = PDB_MAX_PERIOD1;
        pdb_prescaler1 = 7;
        while ((pdb_prescaler1 != (uint_32)-1) && (period < pdb_period_test)) {
            pdb_prescaler1--;
            pdb_period1 = pdb_period_test;
            pdb_period_test >>= 1;
        }
        pdb_prescaler1++;

        /* now, in pdb_prescaler1, we have prescaler as if there was
        ** no additional division factor (MULT field equivalent to 1) and in
        ** pdb_period1 we have related period in microseconds
        */

        pdb_period_test = PDB_MAX_PERIOD;
        pdb_prescaler = 9; /* we got 2 next bits from MULT field, because MULT can have values of 10, 20, 40 */
        while ((pdb_prescaler != (uint_32)-1) && (period < pdb_period_test)) {
            pdb_prescaler--;
            pdb_period = pdb_period_test;
            pdb_period_test >>= 1;
        }
        pdb_prescaler++;

        /* now, in pdb_prescaler, we have prescaler as if there was
        ** additional division factor (MULT field equivalent to 10) and in
        ** pdb_period we have related period in microseconds
        */

        if ((pdb_prescaler1 == 8) && (pdb_prescaler == 10)) {
                /* cannot set period with pdb_prescaler */
                return ADC_ERROR_BAD_PARAM;
        }
        /* Compare both values of pdb_period1 and pdb_period - the smaller one wins */
        if ((pdb_period1 < pdb_period) && (pdb_prescaler1 != 8)) {
            /* use values of pdb_period1 => copy them to the result; MULT field remains zero */
            pdb_period = pdb_period1;
            pdb_prescaler = pdb_prescaler1;
        }
        else {
            /* compute pdb_period and pdb_prescaler plus compute MULT field */
            if (pdb_prescaler > 7) {
                /* move additional bits from pdb_prescaler to MULT field */
                mult = pdb_prescaler - 6;
                /* and saturate pdb_prescaler to maximum */
                pdb_prescaler = 7;
            }
            else
                mult = 1;
        }

        pdb_external.prescaler = (int_8) pdb_prescaler;
    }
    else {
        /* compute PDB period with prescaler already set */
        switch (PDB0_BASE_PTR->SC & PDB_SC_MULT_MASK) {
            case PDB_SC_MULT(0): /* 0 in the mask means that divider is 1 */
                pdb_period = PDB_MAX_PERIOD1 >> (7 - pdb_external.prescaler);
                mult = 0;
                break;
            case PDB_SC_MULT(1): /* 1 in the mask means that divider is 10 */
                pdb_period = PDB_MAX_PERIOD >> (7 - pdb_external.prescaler + 2);
                mult = 1;
                break;
            case PDB_SC_MULT(2): /* 2 in the mask means that divider is 20 */
                pdb_period = PDB_MAX_PERIOD >> (7 - pdb_external.prescaler + 1);
                mult = 2;
                break;
            case PDB_SC_MULT(3): /* 3 in the mask means that divider is 40 */
                pdb_period = PDB_MAX_PERIOD >> (7 - pdb_external.prescaler + 0);
                mult = 3;
                break;
        }

        if (period > pdb_period)
            return ADC_ERROR_BAD_PARAM; /* cannot set period */
    }


    pdb_period1 = ((uint_64) period << 16) / pdb_period;
    if (pdb_period1 > 0xFFFF)
        return ADC_ERROR_BAD_PARAM; /* cannot set period */
    PDB0_BASE_PTR->MOD =  pdb_period1 - 1;
    PDB0_BASE_PTR->SC &= ~PDB_SC_MULT_MASK;
    PDB0_BASE_PTR->SC |= PDB_SC_MULT(mult);
    PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK;
    pdb_internal.period = period;

    /* set prescaler and enable module with interrupts */
    PDB0_BASE_PTR->SC &= ~PDB_SC_PRESCALER_MASK;
    PDB0_BASE_PTR->SC |= (pdb_prescaler << PDB_SC_PRESCALER_SHIFT);
    PDB0_BASE_PTR->SC |= PDB_SC_PDBIE_MASK | PDB_SC_CONT_MASK | PDB_SC_LDOK_MASK;

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_hw_channel_init
* Returned Value   : IO_OK or IO_ERROR
* Comments         :
*    Returns result of HW channel initialization
*
*END*********************************************************************/
_mqx_int _adt_hw_channel_init(MQX_FILE_PTR f, ADT_CHANNEL_PTR adt_ch)
{
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    uint_32  pdb_period;
    _mqx_uint adc_num = adc_ch_bundle->adc_num;
    _mqx_int ch_num;
    _mqx_int status;

    if ((adt_ch->g.trigger & ADC_PDB_TRIGGER) != ADC_PDB_TRIGGER)
        return ADC_ERROR_HWTRIGGER; /* the channel can be run only from HW trigger */

    if (pdb_internal.period != 0) {
        _mqx_int N;
        N = adt_ch->g.period / pdb_internal.period;
        if (N * pdb_internal.period != adt_ch->g.period)
            return ADC_ERROR_BAD_PARAM; /* it's only allowed to have periods as multiple of basic period */
        adc_ch_bundle->periods_preset = N;
        adc_ch_bundle->periods = 0;
    }
    else {
        adc_ch_bundle->periods_preset = 1;
        adc_ch_bundle->periods = 0;

        status = adt_period_prescaler(adt_ch->g.period);
        if (status != ADC_OK)
            return status;
    }
    PDB0_BASE_PTR->SC |= PDB_SC_TRGSEL_SWTRIG; /* set to software trigger by default */

    ch_num = adc_ch_bundle->ch_num;

    pdb_period = pdb_internal.period; /* <err001>: DON'T REMOVE THIS LINE! CW 6.2 COMPILER ERROR WITH DIVIDING OCCURS OTHERWISE */

    PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; /* write new MOD value */
    if (ch_num == 0) { /* temporary, we will make running channel A not actively, first trigger will move it to area within accesible boundary */
        pdb_internal.delayA[adc_num] = ((uint_64) adt_ch->g.offset * PDB0_BASE_PTR->MOD) / pdb_period;/* <err001>: we must use pdb_period */
        PDB0_BASE_PTR->CH[adc_num].DLY[0] = (PDB0_BASE_PTR->MOD == PDB_MOD_MOD_MASK ? PDB_MOD_MOD_MASK : PDB0_BASE_PTR->MOD + 1); /* move ChA behind accessible boundary */
        adc_ch_bundle->waste_channel = 1;
    }
    else
        PDB0_BASE_PTR->CH[adc_num].DLY[ch_num] = ((uint_64) adt_ch->g.offset * PDB0_BASE_PTR->MOD) / pdb_period;/* <err001>: we must use pdb_period */
    PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; /* write new DLY value */

    pdb_internal.used_channel[adc_num] |= 0x01 << ch_num;

    adt_ch->g.samples = adt_ch->g.samples_preset;
    adt_ch->b = adc_ch_bundle;

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_hw_channel_deinit
* Returned Value   : IO_OK or IO_ERROR
* Comments         :
*    Performs CPU specific deinitialization for ADC channel
*
*END*********************************************************************/
_mqx_int _adt_hw_channel_deinit(MQX_FILE_PTR f, ADT_CHANNEL_PTR adt_ch)
{
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    _mqx_uint adc_num = adc_ch_bundle->adc_num;
    _mqx_int ch_num;
    _mqx_uint i;

    ch_num = adc_ch_bundle->ch_num;

    if (ch_num == 0) {
        if (PDB0_BASE_PTR->MOD == PDB_MOD_MOD_MASK)
        {
            PDB0_BASE_PTR->CH[adc_num].DLY[0] = PDB_MOD_MOD_MASK;
        }
        else
        {
            PDB0_BASE_PTR->CH[adc_num].DLY[0] = PDB0_BASE_PTR->MOD + 1;
        }
        PDB0_BASE_PTR->SC |= PDB_SC_LDOK_MASK; /* write DLY value */
        /* Following line is not usable anymore as adc_ch_bundle is about to be mem_freed */
        // adc_ch_bundle->waste_channel = 1; /* in the case of interrupt, waste channel */
    }
    else
        PDB_CH_EN(adc_num) &= ~(1 << ch_num);

    pdb_internal.used_channel[adc_num] &= ~(0x01 << ch_num);

    if (pdb_internal.used_channel[adc_num] == 0) {
        PDB_CH_EN(adc_num) = 0; /* disable also channel A */
    }
    for (i = 0; i < ADC_MAX_MODULES; i++) {
        if (pdb_internal.used_channel[adc_num] != 0)
            goto _adt_hw_channel_deinit_ok;
    }

    /* If we reached this code, it means, that no PDB channels are running anymore */
    PDB0_BASE_PTR->SC &= ~PDB_SC_PDBEN_MASK;
    pdb_internal.period = 0;
    pdb_external.prescaler = -1;

_adt_hw_channel_deinit_ok:
    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_trigger
* Returned Value   : IO_OK or IO_ERROR
* Comments         : Starts channel measurement or more channels defined
*                    by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_trigger(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    _mqx_uint adc_num = adc_d_bundle->init->ADC_NUMBER;

    if (mask & ~ADC_PDB_TRIGGER)
        return ADC_ERROR_HWTRIGGER; /* cannot trigger SW-triggers */

    if (adc_ch_bundle == NULL) {
        PDB_CH_EN(adc_num) = (pdb_internal.used_channel[adc_num] | 0x01); /* enable all used channels */
        if (pdb_internal.used_channel[adc_num] & 0x01) {
            ENABLE_CH0_PROCESSING(adc_num, adc_d_bundle->adc_ch[0]->b);
        }
    }
    else {
        if (adc_ch_bundle->ch_num == 0)
            ENABLE_CH0_PROCESSING(adc_num, adc_ch_bundle);
        PDB_CH_EN(adc_num) |= (1 << adc_ch_bundle->ch_num) | 0x01; /* ChA must run all the time */
    }

    PDB0_BASE_PTR->SC |= PDB_SC_PDBEN_MASK | PDB_SC_SWTRIG_MASK; /* enable PDB module and trigger it */

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_pause
* Returned Value   : IO_OK
* Comments         : Pauses measurement of single channel, or more channels
*                    defined by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_pause(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    _mqx_uint adc_num = adc_d_bundle->init->ADC_NUMBER;

    if (mask & ~ADC_PDB_TRIGGER)
        return ADC_ERROR_HWTRIGGER; /* cannot pause SW-triggers */

    if (adc_ch_bundle == NULL)
        PDB0_BASE_PTR->CH[adc_num].C1 &= ~PDB_C1_EN_MASK;
    else if (adc_ch_bundle->ch_num > 0)
        PDB0_BASE_PTR->CH[adc_num].C1 &= ~PDB_C1_EN(1 << adc_ch_bundle->ch_num);
    else {
        DISABLE_CH0_PROCESSING(adc_num, adc_ch_bundle);
    }

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_resume
* Returned Value   : IO_OK
* Comments         : Resumes measurement of single paused channel, or more
*                    channels defined by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_resume(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    _mqx_uint adc_num = adc_d_bundle->init->ADC_NUMBER;

    if (mask & ~ADC_PDB_TRIGGER)
        return ADC_ERROR_HWTRIGGER; /* cannot resume SW-triggers */

    if (adc_ch_bundle == NULL) {
        PDB_CH_EN(adc_num) = (pdb_internal.used_channel[adc_num] | 0x01); /* enable all used channels */
        if (pdb_internal.used_channel[adc_num] & 0x01) {
            ENABLE_CH0_PROCESSING(adc_num, adc_d_bundle->adc_ch[0]->b);
        }
    }
    else if (adc_ch_bundle->ch_num > 0)
        PDB_CH_EN(adc_num) |= (1 << adc_ch_bundle->ch_num);
    else {
        ENABLE_CH0_PROCESSING(adc_num, adc_ch_bundle);
    }

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _adt_stop
* Returned Value   : IO_OK or IO_ERROR
* Comments         : Stops (resets) measurement of single channel, or more
*                    channels defined by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_stop(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    _mqx_uint adc_num = adc_d_bundle->init->ADC_NUMBER;

    if (mask & ~ADC_PDB_TRIGGER)
        return ADC_ERROR_HWTRIGGER; /* cannot stop SW-triggers */

    if (adc_ch_bundle == NULL)
        PDB_CH_EN(adc_num) = 0;
    else if (adc_ch_bundle->ch_num > 0)
        PDB_CH_EN(adc_num) |= (1 << adc_ch_bundle->ch_num);
    else {
        DISABLE_CH0_PROCESSING(adc_num, adc_ch_bundle);
    }

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : adt_process_data
* Returned Value   : void
* Comments         : called from adc_process_all_data
*
*END*********************************************************************/
_mqx_int _adt_process_data(ADC_CHANNEL_BUNDLE_PTR adc_ch_bundle)
{ /* Body */
    ADT_CHANNEL_PTR adt_ch = adc_ch_bundle->adt_ch;
    _mqx_uint adc_num = adc_ch_bundle->adc_num;

    if (adt_ch->g.samples)
        adt_ch->g.samples--;
    else {
        if (adt_ch->g.init_flags & ADC_CHANNEL_MEASURE_ONCE) {
            if (adc_ch_bundle->ch_num > 0)
                PDB_CH_EN(adc_num) &= ~(1 << adc_ch_bundle->ch_num);
            else {
                DISABLE_CH0_PROCESSING(adc_num, adc_ch_bundle);
            }
        }
#if MQX_USE_LWEVENTS
        if (adt_ch->g.complete_event != NULL)
            _lwevent_set(adt_ch->g.complete_event, adt_ch->g.event_mask); /* set completion event */
#endif
        adt_ch->g.samples = adt_ch->g.samples_preset;
    }

    return ADC_OK;
}

/*FUNCTION*****************************************************************
*
* Function Name    : adt_idelay_isr
* Returned Value   : IO_OK
* Comments         : PDB's delay isr
*
*END*********************************************************************/

static void adt_idelay_isr(pointer param)
{
#if PSP_MQX_CPU_IS_KINETIS
    _mqx_uint i;
    /* clear ERR flags */
    for (i = 0; i < ADC_MAX_MODULES; i++) {
        PDB0_BASE_PTR->CH[i].S = 0x0F;
    }
#endif /* PSP_MQX_CPU_IS_KINETIS */
    /* we should perform data readout for channels, that were supposed to perform late process data processing */
    if (adc_bundle.late_process)
         /* if at least one channel is late processed, then do late processing */
        adc_process_all_data();

    if (pdb_internal.idelay_fcn != NULL)
        pdb_internal.idelay_fcn();
    PDB0_BASE_PTR->SC &= ~PDB_SC_PDBIF_MASK; /* clear interrupt flag */
}


/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_pdb_base_address
* Returned Value   : pointer to base of ADC registers
* Comments         :
*    This function returns base address of ADC related register space.
*
*END*********************************************************************/
pointer _bsp_get_pdb_base_address()
{
    return (pointer) PDB0_BASE_PTR;
}

/*FUNCTION*****************************************************************
*
* Function Name    : pdb_get_prescaler
* Returned Value   : none
* Comments         : Gets PDB raw value or -1 if prescaler was not set yet
*
*END*********************************************************************/
int_16 _pdb_get_prescaler()
{
    if (pdb_external.prescaler == 0)
        return -1;
    else
        return pdb_external.prescaler;
}

/*FUNCTION*****************************************************************
*
* Function Name    : pdb_set_prescaler
* Returned Value   : none
* Comments         : Sets PDB prescaler value
*
*END*********************************************************************/
_mqx_int _pdb_set_prescaler(int_16 prescaler, int_16 multiplier)
{
    if ((prescaler < 0) || (prescaler > (PDB_SC_PRESCALER_MASK >> PDB_SC_PRESCALER_SHIFT)))
        return IO_ERROR; /* cannot set such prescaler */

    /* set prescaler */
    PDB0_BASE_PTR->SC &= ~PDB_SC_PRESCALER_MASK;
    PDB0_BASE_PTR->SC = prescaler << PDB_SC_PRESCALER_SHIFT;

    pdb_external.prescaler = prescaler;

    return IO_OK;
}

/* EOF */
