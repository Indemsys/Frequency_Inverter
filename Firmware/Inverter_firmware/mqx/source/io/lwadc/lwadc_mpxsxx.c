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
* $FileName: lwadc_mpxsxx.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the ADC driver CPU specific functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "pxadc.h"
#include "lwadc_mpxsxx.h"
#include "lwadc_mpxsxx_prv.h"
#include "lwadc.h"


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_restart
 * Returned Value   :
 * Comments         :
 * This function restarts the ADC after a parameter change.
 *
 *END*----------------------------------------------------------------------*/

static void _lwadc_restart(ADC_CONTEXT_PTR context_ptr)
{
    VPXADC_REG_STRUCT_PTR   adc_ptr = context_ptr->adc_ptr;
    uint_32                 adc_status;

    // Stop conversion
    adc_ptr->MCR &= ADC_MCR_NSTART;

    while (adc_ptr->MSR & ADC_MSR_NSTART) {};

    do {
        adc_status = adc_ptr->MSR & ADC_MSR_ADCSTATUS_MASK;
    } while ((adc_status == ADC_MSR_ADCSTATUS_SAMPLE) || (adc_status == ADC_MSR_ADCSTATUS_CONVERSION));

    if (context_ptr->mcr & ADC_MCR_ADCLKSEL) {
        // Power down the ADC
        adc_ptr->MCR = ADC_MCR_PWDN;
        while ((adc_ptr->MSR & ADC_MSR_ADCSTATUS_MASK) !=  ADC_MSR_ADCSTATUS_POWER_DOWN) {};
        _PSP_SYNC();

        // Need to set the clock in power down mode
        adc_ptr->MCR = ADC_MCR_PWDN | ADC_MCR_ADCLKSEL;
        _PSP_SYNC();

        // Now go to IDLE mode
        adc_ptr->MCR = ADC_MCR_ADCLKSEL;
    } else {
        adc_ptr->MCR = 0;
    }

    _PSP_SYNC();
     while ((adc_ptr->MSR & ADC_MSR_ADCSTATUS_MASK) !=  ADC_MSR_ADCSTATUS_IDLE) {};

    adc_ptr->CEOCFR0 = context_ptr->channels;
    _PSP_SYNC();

    // Set up continuous conversion on selected channels
    adc_ptr->NCMR0 =  context_ptr->channels;

    // Start conversions
    adc_ptr->MCR  = context_ptr->mcr; // ADC_MCR_OWREN|ADC_MCR_MODE_SCAN|ADC_MCR_NSTART; //

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

static ADC_CONTEXT adc_context[MPXSXX_ADC_NUM_DEVICES] = {0};

boolean  _lwadc_init( const LWADC_INIT_STRUCT *  init_ptr)
{
    VPXADC_REG_STRUCT_PTR   adc_ptr;
    uint_32                 mcr=ADC_MCR_OWREN|ADC_MCR_MODE_SCAN|ADC_MCR_NSTART;
    ADC_CONTEXT_PTR         context_ptr;

    #if PSP_HAS_DEVICE_PROTECTION
    if (!_bsp_adc_enable_access(init_ptr->device)) {
        return FALSE;
    }
    #endif

    adc_ptr = _bsp_get_adc_base_address(init_ptr->device);

    if (adc_ptr==NULL) {
         return FALSE;
    }

    context_ptr =  &adc_context[init_ptr->device];
    context_ptr->adc_ptr   = adc_ptr;
    context_ptr->channels = 0;

    if (init_ptr->format == LWADC_FORMAT_LEFT_JUSTIFIED) {
        mcr |= ADC_MCR_WLSIDE;
    } else if (init_ptr->format != LWADC_FORMAT_RIGHT_JUSTIFIED) {
        return FALSE;
    }

    if (init_ptr->clock == BSP_MOTC_CLOCK) {
        mcr |= ADC_MCR_ADCLKSEL;
    } else if (init_ptr->clock != BSP_MOTC_CLOCK/2) {
        return FALSE;
    }
    adc_ptr->MCR = ADC_MCR_PWDN;

    context_ptr->init_ptr = init_ptr;
    context_ptr->mcr = mcr;
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
    uint_32 device, channel;

    device  = ADC_DEVICE(input);
    channel = ADC_CHANNEL(input);

    // Used as a validity check in other _lwadc functions
    lwadc_ptr->context_ptr = NULL;

    // Make sure channel & device is in range
    if ((device >= MPXSXX_ADC_NUM_DEVICES ) ||(channel >= ADC_HW_CHANNELS )) {
        return FALSE;
    }

    lwadc_ptr->mask = (1<<channel);

    // And that the ADC can convert this channel
    if ( (_bsp_get_adc_channel_mask(device) & lwadc_ptr->mask) == 0) {
        return FALSE;
    }

    // Enable ADC I/O Pin
    if ( _bsp_adc_io_init(input) != MQX_OK) {
        return FALSE;
    }

    // set channel specific parameters
    lwadc_ptr->input   = input;
    lwadc_ptr->context_ptr = &adc_context[device];
    lwadc_ptr->numerator   = lwadc_ptr->context_ptr->init_ptr->reference;
    lwadc_ptr->denominator = ADC_RESOLUTION_DEFAULT;

    // Set channel to convert
    _int_disable();
    lwadc_ptr->context_ptr->channels |= lwadc_ptr->mask;

    // start conversion
    _lwadc_restart(lwadc_ptr->context_ptr);
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
    LWADC_VALUE sample;

    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }

    // Make sure this channel has not been disabled
    if ((lwadc_ptr->context_ptr->adc_ptr->NCMR0 & lwadc_ptr->mask) ==0) {
        return FALSE;
    }

    if ((lwadc_ptr->context_ptr->adc_ptr->MCR & ADC_MCR_MODE_SCAN) == 0) {
        // ADC is not in scan mode.
        _lwadc_restart(lwadc_ptr->context_ptr);
    }

    sample = lwadc_ptr->context_ptr->adc_ptr->CDR[ADC_CHANNEL(lwadc_ptr->input)];

    *outSample = sample & ADC_CDR_CDATA_MASK;

    // Clear conversion complete
    lwadc_ptr->context_ptr->adc_ptr->CEOCFR0 = lwadc_ptr->mask;
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
    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }

    if ((lwadc_ptr->context_ptr->adc_ptr->MCR & ADC_MCR_MODE_SCAN) == 0) {
        // ADC is not in scan mode.
        _lwadc_restart(lwadc_ptr->context_ptr);
    }

    while ((lwadc_ptr->context_ptr->adc_ptr->CEOCFR0 & lwadc_ptr->mask) == 0) {};

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

    if (num_samples > (MAX_UINT_32/ADC_RESOLUTION_DEFAULT)) {
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
    VPXADC_REG_STRUCT_PTR adc_ptr;

    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }

    adc_ptr = lwadc_ptr->context_ptr->adc_ptr;

    switch (attribute) {
        case LWADC_NUMERATOR:
            lwadc_ptr->numerator = value;
            break;

        case LWADC_DENOMINATOR:
            lwadc_ptr->denominator = value;
            break;

        case LWADC_POWER_DOWN:
            if (value) {
                adc_ptr->MCR = ADC_MCR_PWDN;
            } else {
                _int_disable();
                _lwadc_restart(lwadc_ptr->context_ptr);
                _int_enable();
            }
            break;

        case LWADC_FORMAT:
            _int_disable();
            if (value == LWADC_FORMAT_LEFT_JUSTIFIED) {
                lwadc_ptr->context_ptr->mcr |= ADC_MCR_WLSIDE;
            } else if (value == LWADC_FORMAT_RIGHT_JUSTIFIED) {
                lwadc_ptr->context_ptr->mcr  &= ~ADC_MCR_WLSIDE;
            } else {
                return FALSE;
            }
            if ((adc_ptr->MCR & ADC_MCR_PWDN) == 0) {
                _lwadc_restart(lwadc_ptr->context_ptr);
            }
            _int_enable();
           break;

        case LWADC_FREQUENCY:
            _int_disable();
            if (value == BSP_MOTC_CLOCK) {
                lwadc_ptr->context_ptr->mcr &= ~ADC_MCR_ADCLKSEL;
            } else if (value == BSP_MOTC_CLOCK/2) {
                lwadc_ptr->context_ptr->mcr |= ADC_MCR_ADCLKSEL;
            } else {
                _int_enable();
                return FALSE;
            }

            // If we are NOT powered down, AND we are switching clock frequencies,
            // then we need to power down to switch frequencies (otherwise we will do it when we power back up)
            if ( ((adc_ptr->MCR & ADC_MCR_PWDN) == 0)  &&
                 ( (adc_ptr->MCR & ADC_MCR_ADCLKSEL) != ( adc_ptr->MCR & ADC_MCR_ADCLKSEL)) )
            {
                _lwadc_restart(lwadc_ptr->context_ptr);
            }
            _int_enable();
            break;

        case LWADC_DIVIDER:
            // We support a divider, as long as it is 1.
            return (value == 1)?TRUE:FALSE;
            break;

        case LWADC_DIFFERENTIAL:
            // We don't support differential, so return FALSE if asked for it, TRUE otherwise
            return !value;
            break;

        case LWADC_INPUT_CONVERSION_ENABLE:
            _int_disable();
            if (value) {
                lwadc_ptr->context_ptr->channels |= lwadc_ptr->mask;
            } else {
                lwadc_ptr->context_ptr->channels &= ~lwadc_ptr->mask;
            }
            _lwadc_restart(lwadc_ptr->context_ptr);
            _int_enable();
            break;

        default:
            return FALSE;
    }
    return TRUE;
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

        case LWADC_POWER_DOWN:
            *value_ptr = (lwadc_ptr->context_ptr->adc_ptr->MCR & ADC_MCR_PWDN)?TRUE:FALSE;
            break;

        case LWADC_RESOLUTION:
            *value_ptr = ADC_RESOLUTION_DEFAULT;
            break;

        case LWADC_REFERENCE:
            *value_ptr = lwadc_ptr->context_ptr->init_ptr->reference;
            break;

        case LWADC_FREQUENCY:
            if (lwadc_ptr->context_ptr->adc_ptr->MCR & ADC_MCR_ADCLKSEL) {
                *value_ptr = BSP_MOTC_CLOCK;
            } else {
                *value_ptr = BSP_MOTC_CLOCK/2;
            }
            break;

        case LWADC_DIVIDER:
            *value_ptr = 1;
            break;

        case LWADC_DIFFERENTIAL:
            *value_ptr = FALSE;
            break;

        case LWADC_FORMAT:
            if (lwadc_ptr->context_ptr->adc_ptr->MCR & ADC_MCR_WLSIDE) {
                *value_ptr = LWADC_FORMAT_LEFT_JUSTIFIED;
            } else {
                *value_ptr = LWADC_FORMAT_RIGHT_JUSTIFIED;
            }
            break;

        case LWADC_INPUT_CONVERSION_ENABLE:
            *value_ptr = (lwadc_ptr->context_ptr->adc_ptr->NCMR0 & lwadc_ptr->mask)?TRUE:FALSE;
            break;

        default:
            return FALSE;
    }
    return TRUE;
}
