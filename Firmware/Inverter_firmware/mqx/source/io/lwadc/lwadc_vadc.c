/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2011 Embedded Access Inc
* All Rights Reserved
*
****************************************************************************
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
* $FileName: lwadc_vadc.c$
* $Version : 3.8.1.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This file contains the ADC driver CPU specific functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwadc_vadc_prv.h>

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_restart
 * Returned Value   :
 * Comments         :
 * This function restarts the ADC after a parameter change.
 *
 *END*----------------------------------------------------------------------*/

static ADC_CONTEXT_PTR         context_ptr;

static void _lwadc_restart(LWADC_STRUCT_PTR lwadc_ptr)
{
    ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(context_ptr->init_ptr->ADC_NUMBER);
    uint_32                 adc_status;
    _mqx_int ch_num;
    adc_ptr->HC1 |= ADC_HC1_AIEN1_MASK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_init
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function initializes the ADC device with global parameters
 *    (conversion frequency, resolution, number format). It does  not start
 *    any conversions.
 *
 *END*----------------------------------------------------------------------*/

static ADC_CONTEXT adc_context[ADC_NUM_DEVICES] = {0};
uint_32 adc_max_frq;

uint_32  _lwadc_init( const LWADC_INIT_STRUCT *  init_ptr)
{
    ADC_MemMapPtr adc_ptr;
    uint_8  adc_max_frq_index;

    /* This table is made from datasheet values for ADC module in Kinetis */
    const static uint_32 adc_max_frq_table[] = {
        2500000,  /* 2.5 MHz  for low power,    normal speed, 12b resolution */
        5000000,  /* 5.0 MHz  for low power,    normal speed, lower resolution */
        5000000,  /* 5.0 MHz  for low power,    high speed,   12b resolution */
        8000000,  /* 8.0 MHz  for low power,    high speed,   lower resolution */
        8000000,  /* 8.0 MHz  for normal power, normal speed, 12b resolution */
        12000000, /* 12.0 MHz for normal power, normal speed, lower resolution */
        12000000, /* TBD MHz  for normal power, high speed,   12b resolution */
        22000000, /* TBD MHz  for normal power, high speed,   lower resolution */
    };
    #if PSP_HAS_DEVICE_PROTECTION
    if (!_bsp_adc_enable_access(init_ptr->ADC_NUMBER)) {
        return FALSE;
    }
    #endif

    adc_ptr = _bsp_get_adc_base_address(init_ptr->ADC_NUMBER);
    if (adc_ptr==NULL) {
         return FALSE;
    }

    context_ptr =  &adc_context[init_ptr->ADC_NUMBER];
    context_ptr->channels = 0;
    context_ptr->default_numerator   = init_ptr->reference;
    context_ptr->default_denominator = LWADC_RESOLUTION_DEFAULT;
    
    /* First, use 3 bit mask to create index to the max. frq. table */
    adc_max_frq_index = 0;
    switch (context_ptr->default_denominator) {
        case LWADC_RESOLUTION_12BIT:
            adc_max_frq_index |= 0x01;
            break;
        case LWADC_RESOLUTION_10BIT:
        case LWADC_RESOLUTION_8BIT:
            break;
        default:
            return FALSE; /* bad resolution set */
    }
    if (init_ptr->SPEED == LWADC_HSC_HIGH)
        adc_max_frq_index |= 0x02;
    if (init_ptr->POWER == LWADC_LPC_NORMAL)
        adc_max_frq_index |= 0x04;
    adc_max_frq = adc_max_frq_table[adc_max_frq_index];
    
    /* set AD clock to be as fast as possible */
     switch (init_ptr->CLOCK_SOURCE) {
         case LWADC_CLK_BUSCLK_ANY:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY) {
                 if(BSP_BUS_CLOCK <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_1 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 2) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_2 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 4) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_4 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 8) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK2;
                 else
                     return FALSE; /* cannot set ADC clock to be less than adc_max_frq */
             }
             else {
                 if ((BSP_BUS_CLOCK / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                     adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 2 / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                     adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK2;
                 else
                     return FALSE; /* such clock combination is too fast for ADC */
             }
             break;

         case LWADC_CLK_BUSCLK:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY) {
                 if (BSP_BUS_CLOCK <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_1 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 2) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_2 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 4) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_4 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 8) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK;
                 else
                     return FALSE; /* cannot set ADC clock to be less than ADC max. frequency */
             }
             else if ((BSP_BUS_CLOCK / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                 adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK;
             else
                 return FALSE; /* such clock combination is too fast for ADC */
             break;

         case LWADC_CLK_BUSCLK2:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY) {
                 if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK2;
                 if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                     return FALSE; /* cannot set ADC clock to be less than ADC max. frequency */
             }
             else if ((BSP_BUS_CLOCK / 2 / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                 adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK2;
             else
                 return FALSE; /* such clock combination is too fast for ADC */
             break;

         case LWADC_CLK_ALTERNATE:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY)
                 return FALSE; /* alternate clock + any division is not supported now */
             adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_ALTCLK;
             break;

         case LWADC_CLK_ASYNC:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY)
                 return FALSE; /* async clock + any division is not supported now */
             adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_ADACK;
             break;

         default:
             return FALSE; /* invalid clock source */
             
     }
    adc_ptr->CFG |= ADC_CFG_ADLPC_MASK;
    adc_ptr->CFG |= ADC_CFG_MODE(0x02);
    adc_ptr->CFG |= LWADC_CFG_AVGS_32;
    adc_ptr->CFG &= ~ADC_CFG_ADTRG_MASK; /* set SW trigger */
    adc_ptr->GC |= ADC_GC_AVGE_MASK | ADC_GC_ADCO_MASK;
    context_ptr->init_ptr = init_ptr;

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_init_input
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 * This function prepares and initialize the LWADC_STRUCT
 * with all data it will need later for quick control of the input.
 * The structure initialized here is used in all subsequent calls to any
 * LWADC driver and is uniquely identifying the input.
 * This function calls a BSP supplied function to enable the ADC input pin.
 * It adds the input to the list of channels being converted, and place the
 * ADC in continuous conversion mode if not already in this mode.
 *
 *END*----------------------------------------------------------------------*/

boolean  _lwadc_init_input( LWADC_STRUCT_PTR lwadc_ptr, uint_32 input )
{    
    ADC_MemMapPtr adc_ptr;
    uint_32 device, channel;
    uint32_t source_reg;
    device  = ADC_DEVICE(input);
    channel = ADC_CHANNEL(input);
    
    adc_ptr = _bsp_get_adc_base_address(device);
    source_reg = channel;

    /* Used as a validity check in other _lwadc functions */
    lwadc_ptr->context_ptr = NULL;
    
    /* Make sure channel & device is in range */
    if ((device > ADC_NUM_DEVICES ) ||(channel > ADC_HW_CHANNELS )) {
        return FALSE;
    } 
    adc_ptr->PCTL |=(1<<channel);
    /* Enable ADC I/O Pin */
    if ( _bsp_adc_io_init(input) != IO_OK) {
        return FALSE;
    }

    adc_ptr->HC1 = source_reg;

    /* set channel specific parameters */
    lwadc_ptr->input   = input;
    lwadc_ptr->context_ptr = &adc_context[device];
    lwadc_ptr->numerator   = context_ptr->init_ptr->reference;
    lwadc_ptr->denominator = LWADC_RESOLUTION_DEFAULT;
    /* Set channel to convert */
    _int_disable();
    lwadc_ptr->context_ptr->channels = channel;
    _lwadc_restart(lwadc_ptr);
    _int_enable();
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_read_raw
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Read the current value of the ADC input and return
 *    the result without applying any scaling.
 *
 *END*----------------------------------------------------------------------*/


boolean _lwadc_read_raw( LWADC_STRUCT_PTR lwadc_ptr, LWADC_VALUE_PTR outSample)
{
    ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(context_ptr->init_ptr->ADC_NUMBER);
    LWADC_VALUE sample;
    _mqx_int ch_num;
    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }
    if ((adc_ptr->GC & ADC_GC_ADCO_MASK) == 0) {
        /* ADC is not in continuous mode. */
        adc_ptr->GC |= ADC_GC_ADCO_MASK;
    }
    sample = adc_ptr->R1;

    *outSample = sample & ADC_CDR_CDATA_MASK;

    /* Clear conversion complete */
    adc_ptr->HC1 &= ~ADC_HC1_AIEN1_MASK; 
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_read
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Read the current value of the ADC input, applies any
 *    scaling, and return the result.
 *
 *END*----------------------------------------------------------------------*/

boolean _lwadc_read( LWADC_STRUCT_PTR lwadc_ptr, LWADC_VALUE_PTR outSample)
{
    LWADC_VALUE rawSample;

    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }
    if (_lwadc_read_raw(lwadc_ptr,&rawSample)) {
        *outSample = ((rawSample*lwadc_ptr->numerator) + (lwadc_ptr->denominator>>1))/ lwadc_ptr->denominator;
        return TRUE;
    }

    *outSample = 0;
    return FALSE;
}



/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_wait_next
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Waits for a new value to be available on the specified ADC input.
 *
 *END*----------------------------------------------------------------------*/

boolean _lwadc_wait_next( LWADC_STRUCT_PTR lwadc_ptr )
{
    ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(context_ptr->init_ptr->ADC_NUMBER);
    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }

    if ((adc_ptr->GC & ADC_GC_ADCO_MASK) == 0) {
        /* ADC is not in continuous mode. */
        adc_ptr->GC |= ADC_GC_ADCO_MASK;
    }
    while((adc_ptr->HS & ADC_HS_COCO1_MASK) == 0){}
    return TRUE;
}



/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_read_average
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Read the current value of the ADC input, applies any
 *    scaling, and return the result.
 *
 *END*----------------------------------------------------------------------*/

boolean _lwadc_read_average( LWADC_STRUCT_PTR lwadc_ptr, uint_32 num_samples, LWADC_VALUE_PTR outSample)
{
    LWADC_VALUE rawSample,sum = 0;
    uint_32     i;

    if (num_samples > (MAX_UINT_32/LWADC_RESOLUTION_DEFAULT)) {
        return FALSE;
    }

    for (i=0;i<num_samples;i++) {
        if (!_lwadc_wait_next(lwadc_ptr)) {
            return FALSE;
        }

        if (!_lwadc_read_raw(lwadc_ptr,&rawSample)) {
            return FALSE;
        }
        sum += rawSample;
    }
 
    *outSample = (((sum/num_samples)*lwadc_ptr->numerator) + (lwadc_ptr->denominator>>1))/ lwadc_ptr->denominator;
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_set_attribute
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 * This function sets attributes for the specified ADC input. Attributes
 * could include single/differential mode, reference, scaling numerator or
 * denominator, etc.
 *
 *END*----------------------------------------------------------------------*/

boolean _lwadc_set_attribute( LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute,  uint_32 value)
{
    boolean result = TRUE;
	ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(context_ptr->init_ptr->ADC_NUMBER);

    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }

    switch (attribute) {
        case LWADC_NUMERATOR:
            lwadc_ptr->numerator = value;
            break;

        case LWADC_DENOMINATOR:
            lwadc_ptr->denominator = value;
            break;

        case LWADC_DEFAULT_NUMERATOR:
            lwadc_ptr->context_ptr->default_numerator = value;
            break;

        case LWADC_DEFAULT_DENOMINATOR:
            lwadc_ptr->context_ptr->default_denominator = value;
            break;

        case LWADC_POWER_DOWN:
            if (value) {
                adc_ptr->CFG |= ADC_CFG_ADLPC_MASK;
            } else {
                _int_disable();
                _lwadc_restart(lwadc_ptr);
                _int_enable();
            }
            break;

        case LWADC_FORMAT:
            break;

        case LWADC_FREQUENCY:
            break;

        case LWADC_DIVIDER:
            /* We support a divider, as long as it is 1. */
            result = (value == 1)?TRUE:FALSE;
            break;

        case LWADC_DIFFERENTIAL:
            /* We don't support differential, so return FALSE if asked for it, TRUE otherwise */
            result = !value;
            break;

        case LWADC_INPUT_CONVERSION_ENABLE:
            break;

        default:
            return FALSE;
    }
    return result;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_get_attribute
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function This function gets attributes for the specified ADC input,
 *    or for the ADC module as a whole. Attributes could include
 *    single/differential mode, reference, scaling numerator or denominator, etc.
 *
 *END*----------------------------------------------------------------------*/

boolean _lwadc_get_attribute( LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute,  uint_32_ptr value_ptr)
{
    ADC_MemMapPtr adc_ptr = _bsp_get_adc_base_address(context_ptr->init_ptr->ADC_NUMBER);
    if ((lwadc_ptr->context_ptr == NULL) || (value_ptr == NULL)) {
        return FALSE;
    }

    switch (attribute) {
        case LWADC_NUMERATOR:
            *value_ptr = lwadc_ptr->numerator;
            break;

        case LWADC_DENOMINATOR:
            *value_ptr = lwadc_ptr->denominator;
            break;

        case LWADC_DEFAULT_NUMERATOR:
            *value_ptr = lwadc_ptr->context_ptr->default_numerator;
            break;

        case LWADC_DEFAULT_DENOMINATOR:
            *value_ptr = lwadc_ptr->context_ptr->default_denominator;
            break;

        case LWADC_POWER_DOWN:
            *value_ptr = (adc_ptr->CFG&ADC_CFG_ADLPC_MASK)?TRUE:FALSE;
            break;

        case LWADC_RESOLUTION:
            *value_ptr = LWADC_RESOLUTION_DEFAULT;
            break;

        case LWADC_REFERENCE:
            *value_ptr = context_ptr->init_ptr->reference;
            break;

        case LWADC_FREQUENCY:
            *value_ptr = adc_max_frq;
            break;

        case LWADC_DIVIDER:
            *value_ptr = 1;
            break;

        case LWADC_DIFFERENTIAL:
            *value_ptr = FALSE;
            break;

        case LWADC_FORMAT:
            *value_ptr = FALSE;
            break;

        case LWADC_INPUT_CONVERSION_ENABLE:
            *value_ptr = FALSE;
            break;

        default:
            return FALSE;
    }
    return TRUE;
}



/* EOF */

