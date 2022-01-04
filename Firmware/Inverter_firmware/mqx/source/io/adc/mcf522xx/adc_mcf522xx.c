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
* $FileName: adc_mcf522xx.c$
* $Version : 3.6.24.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the ADC driver CPU specific functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <fio_prv.h>
#include "io.h"
#include "adc.h"
#include "adc_prv.h"
#include "adc_conf.h"
#include "adc_mcf522xx_prv.h"

#if BSPCFG_ENABLE_ADC

/* Peripherals pointer */
static VMCF522XX_ADC_STRUCT_PTR  mcf522xx_adc_ptr;

/* static variables */
static void (_CODE_PTR_ old_isr_adc1)(pointer);
static pointer          old_isr_adc1_param;
static ADC_DRIVER_BUNDLE_PTR driver_bundle = NULL;

/* Declarations of IRQ handlers */
static void _adc_g_conv_end(uint_16_ptr results, ADC_HW_CHANNEL_MASK mask);
static void adc_complete_isr(pointer);
static uint_32 adc_channels_used = 0;

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_hw_ioctl
* Returned Value   : _mqx_int
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/
_mqx_int _adc_hw_ioctl(MQX_FILE_PTR f, _mqx_uint cmd, pointer param_ptr)
{
    return IO_ERROR_INVALID_IOCTL_CMD;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_hw_install
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific initialization for ADC driver (during install)
*
*END*********************************************************************/
_mqx_int _adc_hw_install(char_ptr a, ADC_DRIVER_BUNDLE_PTR b, pointer c)
{
   return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_hw_init
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific initialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adc_hw_init(MQX_FILE_PTR f, ADC_PTR adc)
{ /* Body */
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;

#if PSP_MQX_CPU_IS_MCF5225X
    VMCF5225_STRUCT_PTR mcf522xx_ptr;
#elif PSP_MQX_CPU_IS_MCF5223X
    VMCF5223_STRUCT_PTR mcf522xx_ptr;
#elif PSP_MQX_CPU_IS_MCF5222X
    VMCF5222_STRUCT_PTR mcf522xx_ptr;
#else
    #error This CPU is not supported here
#endif
    
    _mqx_uint        div_register;
    _mqx_int         i;

    /* Prepare pointers to peripherals */
    mcf522xx_ptr = _PSP_GET_IPSBAR();
    mcf522xx_adc_ptr = _bsp_get_adc_base_address();

    if (adc->g.resolution != ADC_RESOLUTION_12BIT)
        return ADC_ERROR_BAD_PARAM; /* cannot set ADC resolution */

    /* Prepare ADC clocking to be as fast as possible */
    div_register = (BSP_SYSTEM_CLOCK - 1) / 20000000;
    if (div_register > 31)
        return ADC_ERROR_BAD_PARAM; /* cannot set ADC frequency */

    /* store information about exact ADC clock frequency */
    adc->g.adc_frq = (BSP_SYSTEM_CLOCK >> 2) / (div_register + 1);

    /* Prepare AD converter, but do not start it */
    mcf522xx_adc_ptr->CTRL2 = 0x0800 | div_register; /* Channel B can run in parallel with channel A */
    mcf522xx_adc_ptr->CTRL1 = 0x0800; /* Once mode, single conversion, all inputs single-ended */
    mcf522xx_adc_ptr->POWER &= 0xfff8; /* Power up ADC */

/* Install ISR for ADC */
#if PSP_MQX_CPU_IS_MCF5225X
    old_isr_adc1_param = _int_get_isr_data(BSP_ADC_INTERRUPT_VECTOR);
    old_isr_adc1 = _int_install_isr(BSP_ADC_INTERRUPT_VECTOR, adc_complete_isr, NULL);
#elif PSP_MQX_CPU_IS_MCF5223X
    old_isr_adc1_param = _int_get_isr_data(MCF5223_INT_ADC_ADCA);
    old_isr_adc1 = _int_install_isr(MCF5223_INT_ADC_ADCA, adc_complete_isr, NULL);
#elif PSP_MQX_CPU_IS_MCF5222X
    old_isr_adc1_param = _int_get_isr_data(MCF5222_INT_ADC_ADCA);
    old_isr_adc1 = _int_install_isr(MCF5222_INT_ADC_ADCA, adc_complete_isr, NULL);
#else
    #error This CPU is not supported here
#endif

    if (NULL == old_isr_adc1)
        return ADC_ERROR_ISR; /* could not install new interrupt routine */


#if PSP_MQX_CPU_IS_MCF5225X
    /* Setup ISR levels for ADC */
    _mcf5225_int_init(BSP_ADC_INTERRUPT_VECTOR, BSP_ADC_INTERRUPT_LEVEL, BSP_ADC_INTERRUPT_SUBLEVEL, TRUE);
#elif PSP_MQX_CPU_IS_MCF5223X
    /* Setup ISR levels for ADC */
    _mcf5223_int_init(BSP_ADC_INTERRUPT_VECTOR, BSP_ADC_INTERRUPT_LEVEL, BSP_ADC_INTERRUPT_SUBLEVEL, TRUE);
#elif PSP_MQX_CPU_IS_MCF5222X
    /* Setup ISR levels for ADC */
    _mcf5222_int_init(BSP_ADC_INTERRUPT_VECTOR, BSP_ADC_INTERRUPT_LEVEL, BSP_ADC_INTERRUPT_SUBLEVEL, TRUE);
#else
        #error This CPU is not supported here
#endif

    driver_bundle = adc_d_bundle;
    return ADC_OK;
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_hw_deinit
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific deinitialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adc_hw_deinit(MQX_FILE_PTR f, ADC_PTR adc) {
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_hw_channel_init
* Returned Value   : int_32
* Comments         :
*    Returns result of HW channel initialization
*
*END*********************************************************************/
_mqx_int _adc_hw_channel_init(MQX_FILE_PTR f, ADC_CHANNEL_PTR adc_ch)
{ /* Body */
    _mqx_uint result;

    if (adc_ch->g.source > ADC_SOURCE_AN6_AN7)
        return ADC_ERROR_BAD_PARAM; /* ADC_SOURCE_ constant does not match any source signal for this CPU */
    if ((1 << adc_ch->g.source) & adc_channels_used)
        return ADC_ERROR_OPENED; /* channel already used */
    /* configure GPIO and MUX for ADC */
    if (IO_OK == (result = _bsp_adc_io_init(adc_ch->g.source)))
        adc_channels_used |= 1 << adc_ch->g.source;

    return result;        
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_hw_channel_deinit
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific deinitialization for ADC channel
*
*END*********************************************************************/
_mqx_int _adc_hw_channel_deinit(MQX_FILE_PTR f, ADC_CHANNEL_PTR adc_ch) {
    adc_channels_used &= ~(1 << adc_ch->g.source);
    return ADC_OK;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _adc_hw_measure
* Returned Value   : void
* Comments         : Prepares and runs ADC to measure data from channel
*
*END*********************************************************************/
void _adc_hw_measure(ADC_HW_CHANNEL_MASK mask) {
    _mqx_int i, j;

    mcf522xx_adc_ptr->ADSDIS = 0x00ff; /* presume all disabled */
    mcf522xx_adc_ptr->ADLST1 = mcf522xx_adc_ptr->ADLST2 = 0;
    for (i = 0, j = 0; i < ADC_SOURCE_MAX + 1; i++) {
        if ((1 << i) & mask) {
            if (j < 4)
                mcf522xx_adc_ptr->ADLST1 |= i << (4 * j++);
            else
                mcf522xx_adc_ptr->ADLST2 |= i << (4 * (j++ - 4));
            
            mcf522xx_adc_ptr->ADSDIS = mcf522xx_adc_ptr->ADSDIS << 1;
        }
    }
    mcf522xx_adc_ptr->CTRL1 |= 0x2000; /* start AD conversion */ 
    driver_bundle->last_mask = mask;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : adc_complete_isr
* Returned Value   : void
* Comments         : IRQ handler for completed AD conversion(s)
*    In adc_c.channels_to_next_adc we have list of channels that
*    have been sampled and whole we need to handle the results.
*    We will copy results from AD converter to channels buffers.
*    Since AD converter can hold 8 samples a time, we check if any
*    other channel needs to be sampled and if yes, prepare next AD
*    conversion.
*
*END*********************************************************************/
static void adc_complete_isr
    (
        /* [IN] the address of the device specific information */
        pointer parameter
    )
{ /* Body */
    _mqx_int    i, j;
    uint_16_ptr results = (pointer) &mcf522xx_adc_ptr->ADRSLT[0]; /* prepare pointer to peripheral results */
    uint_16     results_internal[ADC_SOURCE_MAX]; /* 8 - number of results ADC can produce in one shot */
    mcf522xx_adc_ptr->ADSTAT |= 0x1800; /* clear interrupt flags */
   
    /* perform copies from ADC */
    for (i = 0; ((mcf522xx_adc_ptr->ADSDIS & 0x0001) == 0x0000) && (i < 8); i++, mcf522xx_adc_ptr->ADSDIS >>= 1) {
        if (i < 4) {
            j = mcf522xx_adc_ptr->ADLST1 & 0x000f;
            mcf522xx_adc_ptr->ADLST1 >>= 4;
        }
        else {
            j = mcf522xx_adc_ptr->ADLST2 & 0x000f;
            mcf522xx_adc_ptr->ADLST2 >>= 4;
        }
        results_internal[j] = results[i] >> 3;
    }
    _adc_g_conv_end(results_internal, driver_bundle->last_mask);
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_g_conv_end
* Returned Value   : 
* Comments         : This is called by low-level driver to inform generic
*                    driver, that some conversion was performed. Function
*                    moves stored data to the channel buffers.
*END*********************************************************************/
static void _adc_g_conv_end(uint_16_ptr results_buf, ADC_HW_CHANNEL_MASK mask)
{
    _mqx_int i;
    ADC_CHANNEL_PTR *adc_ch = driver_bundle->adc_ch;
    ADT_CHANNEL_PTR *adt_ch = driver_bundle->adt_ch;

    for (i = 0; i < ADC_MAX_CHANNELS; i++) {
        if (!adt_ch[i] || !(adt_ch[i]->g.runtime_flags & ADC_CHANNEL_RESUMED) || !(mask & (1 << adc_ch[i]->g.source)))
            continue;
            
        if (adc_ch[i]->g.init_flags & ADC_CHANNEL_ACCUMULATE)
            /* if the channel has ADC_CHANNEL_ACCUMULATE, then accumulate result */
            adc_ch[i]->g.buffer_driver->result += results_buf[adc_ch[i]->g.source];

        else
            /* else copy the result to the channel buffer */
            adc_ch[i]->g.buffer_driver->result = results_buf[adc_ch[i]->g.source];

        if (!(adc_ch[i]->g.init_flags & ADC_CHANNEL_ACCUMULATE) || !(adt_ch[i]->g.samples)) {
            /* move driver pointer to next index, circullarily */
            if (++adc_ch[i]->g.buffer_driver >= adc_ch[i]->g.buffer_end)
                adc_ch[i]->g.buffer_driver = adc_ch[i]->g.buffer_start;

            /* if new value should overwrite old value in ring buffer, move read pointer to next value */
            if (adc_ch[i]->g.buffer_driver == adc_ch[i]->g.buffer_task) {
                if (++adc_ch[i]->g.buffer_task >= adc_ch[i]->g.buffer_end)
                    adc_ch[i]->g.buffer_task = adc_ch[i]->g.buffer_start;
            }
        }
        
        if (adt_ch[i]->g.samples) {
            adt_ch[i]->g.samples--;
        }
        else {
#if MQX_USE_LWEVENTS
            /* if it is the last sample in the batch, then set event */
            if (adt_ch[i]->g.complete_event)
                _lwevent_set(adt_ch[i]->g.complete_event, adt_ch[i]->g.event_mask); /* set completion event */
#endif
                
            if (adc_ch[i]->g.init_flags & ADC_CHANNEL_MEASURE_ONCE)
                adt_ch[i]->g.runtime_flags &= ~ADC_CHANNEL_RUNNING;
            
            adt_ch[i]->g.samples = adt_ch[i]->g.samples_preset;
        }
    }
}

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_adc_base_address
* Returned Value   : pointer to base of ADC registers
* Comments         :
*    This function returns base address of ADC related register space.
*
*END*********************************************************************/
pointer _bsp_get_adc_base_address
(
    void
)
{
#if PSP_MQX_CPU_IS_MCF5225X
    return (pointer)(&((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->ADC);
#elif PSP_MQX_CPU_IS_MCF5222X
    return (pointer)(&((VMCF5222_STRUCT_PTR)_PSP_GET_IPSBAR())->ADC);
#elif PSP_MQX_CPU_IS_MCF5223X
    return (pointer)(&((VMCF5223_STRUCT_PTR)_PSP_GET_IPSBAR())->ADC);
#else
    #error This CPU is not supported here
    return NULL;
#endif
}

#endif // BSPCFG_ENABLE_ADC
/* EOF */
