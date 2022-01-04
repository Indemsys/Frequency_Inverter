/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: tchres.c$
* $Version : 3.8.9.0$
* $Date    : Sep-22-2011$
*
* Comments:
*
*   This file contains the source for the hello example program.
*
*END************************************************************************/
#include "mqx.h"
#include "bsp.h"
#include "tchres_prv.h"
#include <string.h>

#if !MQX_USE_LWEVENTS
#error use MQX_USE_LWEVENTS
#endif

#define ADC_XPLUS_CHANNEL "adc_X"
#define ADC_YPLUS_CHANNEL "adc_Y"

#if 0
const ADC_INIT_STRUCT adc_init = {
    ADC_RESOLUTION_DEFAULT,     /* resolution */
};
#endif
/* static function prototypes */
static _mqx_int _tchres_uninstall( IO_DEVICE_STRUCT_PTR );
static _mqx_int _tchres_open( MQX_FILE_PTR, char_ptr, char_ptr );
static _mqx_int _tchres_close( MQX_FILE_PTR );
static _mqx_int _tchres_ioctl( MQX_FILE_PTR, _mqx_uint, pointer );
static _mqx_int _tchres_get_position_raw( int_16 *, int_16 *, TCHRES_DEVICE_STRUCT_PTR );
static uint_16 _tchres_read_touch_axis( TCHRES_PIN_SELECT_ENUM, TCHRES_DEVICE_STRUCT_PTR );
static int_16 _tchres_get_raw_limits( TCHRES_LIMITS_ENUM, TCHRES_DEVICE_STRUCT_PTR );
static uchar _tchres_get_filtered_axis(TCHRES_DEVICE_STRUCT_PTR, TCHRES_PIN_SELECT_ENUM, uint_16 *);
static void _tchres_set_pins_to_default( TCHRES_DEVICE_STRUCT_PTR );
static _mqx_int _tchres_measure_position(TCHRES_PIN_SELECT_ENUM, int_16 *, TCHRES_DEVICE_STRUCT_PTR);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_tchres_install
* Returned Value   : none
* Comments         :
*    Return filtered value from adc
*
*END*----------------------------------------------------------------------*/
_mqx_uint _io_tchres_install
(
   /* [IN] A string that identifies the device for fopen */
   char_ptr             identifier,

   /* [IN] The I/O init data pointer */
   TCHRES_INIT_STRUCT_PTR  init_ptr,

   /* [IN] adc devices used for measuring X+ and Y+ */
   TCHRES_INSTALL_PARAM_STRUCT_PTR install_params
)
{
   TCHRES_DEVICE_STRUCT_PTR    dev_ptr;
   ADC_INIT_CHANNEL_STRUCT_PTR adc_x_ptr, adc_y_ptr;
   _mqx_int                    result = MQX_OK;

   if ((install_params->ADC_XPLUS_DEVICE == NULL) || (install_params->ADC_YPLUS_DEVICE == NULL))
   {
      result = MQX_INVALID_IO_CHANNEL;
      goto tchres_alloc_err;
   }

   /* allocate memory */
   dev_ptr = _mem_alloc_system_zero((_mem_size)sizeof( TCHRES_DEVICE_STRUCT ));
   #if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL)
   {
      result = MQX_OUT_OF_MEMORY;
      goto tchres_alloc_err;
   } /* Endif */
   #endif

   adc_x_ptr = _mem_alloc_system_zero((_mem_size)sizeof( ADC_INIT_CHANNEL_STRUCT ));
   #if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (adc_x_ptr == NULL)
   {
      result = MQX_OUT_OF_MEMORY;
      goto adcx_alloc_err;
   } /* Endif */
   #endif

   adc_y_ptr = _mem_alloc_system_zero((_mem_size)sizeof( ADC_INIT_CHANNEL_STRUCT ));
   #if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (adc_y_ptr == NULL)
   {
      result = MQX_OUT_OF_MEMORY;
      goto adcy_alloc_err;
   } /* Endif */
   #endif

   _mem_set_type( dev_ptr, MEM_TYPE_IO_TCHRES_STRUCT );
   _mem_set_type( adc_x_ptr, MEM_TYPE_IO_TCHRES_STRUCT );
   _mem_set_type( adc_y_ptr, MEM_TYPE_IO_TCHRES_STRUCT );

   if (_lwevent_create(&dev_ptr->TCH_EVENT, LWEVENT_AUTO_CLEAR) != MQX_OK)
   {
      result = MQX_IO_OPERATION_NOT_AVAILABLE;
      goto event_err;
   }

   /* Channel X adc settings */
   adc_x_ptr->source = init_ptr->ADC_CHANNEL_X_SOURCE;
   adc_x_ptr->flags = TCHRES_ADC_FLAGS;
   adc_x_ptr->number_samples = TCHRES_ADC_NUM_SAMPLES;
   adc_x_ptr->time_offset = 0;
   adc_x_ptr->time_period = TCHRES_ADC_TIME_PERIOD;
   adc_x_ptr->range = TCHRES_ADC_RANGE;
   adc_x_ptr->buffer_size = TCHRES_ADC_BUFFER_SIZE;
   adc_x_ptr->trigger = init_ptr->ADC_CHANNEL_X_TRIGGER;
   adc_x_ptr->complete_event = &dev_ptr->TCH_EVENT;
   adc_x_ptr->event_mask = TCH_ADC_X_COMPLETE;

   /* Channel Y adc settings */
   adc_y_ptr->source = init_ptr->ADC_CHANNEL_Y_SOURCE;
   adc_y_ptr->flags = TCHRES_ADC_FLAGS;
   adc_y_ptr->number_samples = TCHRES_ADC_NUM_SAMPLES;
   adc_y_ptr->time_offset = TCHRES_ADC_TIME_PERIOD / 2;
   adc_y_ptr->time_period = TCHRES_ADC_TIME_PERIOD;
   adc_y_ptr->range = TCHRES_ADC_RANGE;
   adc_y_ptr->buffer_size = TCHRES_ADC_BUFFER_SIZE;
   adc_y_ptr->trigger = init_ptr->ADC_CHANNEL_Y_TRIGGER;
   adc_y_ptr->complete_event = &dev_ptr->TCH_EVENT;
   adc_y_ptr->event_mask = TCH_ADC_Y_COMPLETE;

   /* copy the adc channel information */
   dev_ptr->ADC_LIMITS = init_ptr->ADC_LIMITS;
   dev_ptr->ADC_CHANNEL_X_STRUCT = adc_x_ptr;
   dev_ptr->ADC_CHANNEL_Y_STRUCT = adc_y_ptr;
   dev_ptr->ADC_XPLUS_DEVICE = install_params->ADC_XPLUS_DEVICE;
   dev_ptr->ADC_YPLUS_DEVICE = install_params->ADC_YPLUS_DEVICE;

   /* initialize lwgpio */
   if (!lwgpio_init(&dev_ptr->X_PLUS_PIN, init_ptr->PIN_CONFIG.X_PLUS, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
   {
      result = MQX_IO_OPERATION_NOT_AVAILABLE;
      goto lw_err;
   }
   if (!lwgpio_init(&dev_ptr->Y_PLUS_PIN, init_ptr->PIN_CONFIG.Y_PLUS, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
   {
      result = MQX_IO_OPERATION_NOT_AVAILABLE;
      goto lw_err;
   }
   if (!lwgpio_init(&dev_ptr->X_MINUS_PIN, init_ptr->PIN_CONFIG.X_MINUS, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
   {
      result = MQX_IO_OPERATION_NOT_AVAILABLE;
      goto lw_err;
   }
   if (!lwgpio_init(&dev_ptr->Y_MINUS_PIN, init_ptr->PIN_CONFIG.Y_MINUS, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
   {
      result = MQX_IO_OPERATION_NOT_AVAILABLE;
      goto lw_err;
   }

   /* set adc and gpio functionality mux indexes */
   dev_ptr->PIN_FUNCT = init_ptr->PIN_CONFIG.PIN_FUNCT;

   /* install device */
   return ( _io_dev_install_ext ( identifier,
      _tchres_open,
      _tchres_close,
      NULL,
      NULL,
      _tchres_ioctl,
      _tchres_uninstall,
      (pointer)dev_ptr) );

    /* exceptions, clear memory */
lw_err:
    _lwevent_destroy(&dev_ptr->TCH_EVENT);
event_err:
   _mem_free(adc_y_ptr);
adcy_alloc_err:
   _mem_free(adc_x_ptr);
adcx_alloc_err:
   _mem_free(dev_ptr);
tchres_alloc_err:
   return result;

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_uninstall
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Uninstall the touch screen driver.
*
*END*----------------------------------------------------------------------*/
static _mqx_int _tchres_uninstall
(
    IO_DEVICE_STRUCT_PTR    io_dev_ptr
)
{
   TCHRES_DEVICE_STRUCT_PTR tchscr_dev_ptr = io_dev_ptr->DRIVER_INIT_PTR;

   /* free adc channel infos */
   if( tchscr_dev_ptr->ADC_CHANNEL_X_STRUCT )
   {
      _mem_free(tchscr_dev_ptr->ADC_CHANNEL_X_STRUCT);
   }
   if( tchscr_dev_ptr->ADC_CHANNEL_Y_STRUCT )
   {
      _mem_free(tchscr_dev_ptr->ADC_CHANNEL_Y_STRUCT);
   }
   if( &tchscr_dev_ptr->TCH_EVENT )
   {
      _lwevent_destroy( &tchscr_dev_ptr->TCH_EVENT );
   }
   if( tchscr_dev_ptr )
   {
      _mem_free (tchscr_dev_ptr);
   }
   io_dev_ptr->DRIVER_INIT_PTR = NULL;
   return IO_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_open
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Open the touch screen driver and initialize the gpio pins to default
*    states. Register  used pins in gpio driver as used. .
*
*END*----------------------------------------------------------------------*/
static _mqx_int _tchres_open
(
   /* [IN] The address of the device specific information */
   MQX_FILE_PTR     fd_ptr,

   /* [IN] The rest of the name of the device opened */
   char_ptr open_name_ptr,

   /* [IN] the flags to be used during operation: */
   char_ptr   flags
)
{
   IO_DEVICE_STRUCT_PTR       io_dev_ptr;
   TCHRES_DEVICE_STRUCT_PTR   tchscr_dev_ptr;
   _mqx_int                   result = MQX_OK;
   char_ptr					  ch_xplus, ch_yplus;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   tchscr_dev_ptr = (TCHRES_DEVICE_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

   /* open this driver only once */
   if( tchscr_dev_ptr->COUNT == 0 )
   {
      tchscr_dev_ptr->COUNT = 1;
      
      
      ch_xplus = _mem_alloc_system_zero((_mem_size) strlen(tchscr_dev_ptr->ADC_XPLUS_DEVICE) + strlen(ADC_XPLUS_CHANNEL) + 1);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
      if (ch_xplus == NULL) {
    	  result = MQX_OUT_OF_MEMORY;
    	  goto _channels_alloc_err;
      }
#endif
      strcpy(ch_xplus, tchscr_dev_ptr->ADC_XPLUS_DEVICE);
      strcpy(ch_xplus + strlen(ch_xplus), ADC_XPLUS_CHANNEL);
      
      
      ch_yplus = _mem_alloc_system_zero((_mem_size) strlen(tchscr_dev_ptr->ADC_YPLUS_DEVICE) + strlen(ADC_YPLUS_CHANNEL) + 1);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
      if (ch_yplus == NULL) {
    	  result = MQX_OUT_OF_MEMORY;
    	  goto ch_yplus_alloc_err;
      }
#endif
      strcpy(ch_yplus, tchscr_dev_ptr->ADC_YPLUS_DEVICE);
      strcpy(ch_yplus + strlen(ch_yplus), ADC_YPLUS_CHANNEL);
      
      /* open X adc channel */
      tchscr_dev_ptr->ADC_CHANNEL_X = fopen(ch_xplus, (const char*)tchscr_dev_ptr->ADC_CHANNEL_X_STRUCT);
      if( tchscr_dev_ptr->ADC_CHANNEL_X == NULL )
      {
         result = MQX_IO_OPERATION_NOT_AVAILABLE;
      }
      _mem_free(ch_xplus);
      
      /* open Y adc channel */
      tchscr_dev_ptr->ADC_CHANNEL_Y = fopen(ch_yplus, (const char*)tchscr_dev_ptr->ADC_CHANNEL_Y_STRUCT);
      if( tchscr_dev_ptr->ADC_CHANNEL_Y == NULL )
      {
         result = MQX_IO_OPERATION_NOT_AVAILABLE;
      }
      _mem_free(ch_yplus);

      /* set pisns to default state */
      lwgpio_set_direction( &tchscr_dev_ptr->X_PLUS_PIN, LWGPIO_DIR_OUTPUT );
      lwgpio_set_direction( &tchscr_dev_ptr->Y_PLUS_PIN, LWGPIO_DIR_OUTPUT );
      lwgpio_set_direction( &tchscr_dev_ptr->X_MINUS_PIN, LWGPIO_DIR_OUTPUT );
      lwgpio_set_direction( &tchscr_dev_ptr->Y_MINUS_PIN, LWGPIO_DIR_OUTPUT );

      lwgpio_set_value( &tchscr_dev_ptr->X_PLUS_PIN, LWGPIO_VALUE_LOW );
      lwgpio_set_value( &tchscr_dev_ptr->X_MINUS_PIN, LWGPIO_VALUE_LOW );
      lwgpio_set_value( &tchscr_dev_ptr->Y_PLUS_PIN, LWGPIO_VALUE_LOW );
      lwgpio_set_value( &tchscr_dev_ptr->Y_MINUS_PIN, LWGPIO_VALUE_LOW );

#if PSP_MQX_CPU_IS_MCF51JE || PSP_MQX_CPU_IS_MCF51MM
      ioctl(tchscr_dev_ptr->ADC_CHANNEL_X, ADC_IOCTL_PIN_DISABLE, (pointer) tchscr_dev_ptr->ADC_CHANNEL_X_STRUCT->source);
      ioctl(tchscr_dev_ptr->ADC_CHANNEL_Y, ADC_IOCTL_PIN_DISABLE, (pointer) tchscr_dev_ptr->ADC_CHANNEL_Y_STRUCT->source);
#else
      lwgpio_set_functionality( &tchscr_dev_ptr->X_PLUS_PIN, tchscr_dev_ptr->PIN_FUNCT.X_PLUS_GPIO_FUNCTION );
      lwgpio_set_functionality( &tchscr_dev_ptr->Y_PLUS_PIN, tchscr_dev_ptr->PIN_FUNCT.Y_PLUS_GPIO_FUNCTION );
#endif

      lwgpio_set_functionality( &tchscr_dev_ptr->X_MINUS_PIN, tchscr_dev_ptr->PIN_FUNCT.X_MINUS_GPIO_FUNCTION );
      lwgpio_set_functionality( &tchscr_dev_ptr->Y_MINUS_PIN, tchscr_dev_ptr->PIN_FUNCT.Y_MINUS_GPIO_FUNCTION );
   }
   else
   {
      result = MQX_IO_OPERATION_NOT_AVAILABLE;
   }

   return result;
   
ch_yplus_alloc_err:
   _mem_free(ch_xplus);
_channels_alloc_err:
   return MQX_OUT_OF_MEMORY;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_close
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Close the touch screen driver.
*
*END*----------------------------------------------------------------------*/
static _mqx_int _tchres_close
(
   /* [IN] The address of the device specific information */
   MQX_FILE_PTR     fd_ptr
)
{
   IO_DEVICE_STRUCT_PTR       io_dev_ptr;
   TCHRES_DEVICE_STRUCT_PTR   tchscr_dev_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   tchscr_dev_ptr = (TCHRES_DEVICE_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

   if( tchscr_dev_ptr )
   {
      fclose( tchscr_dev_ptr->ADC_CHANNEL_X );
      fclose( tchscr_dev_ptr->ADC_CHANNEL_Y );

      /* open this driver only once */
      tchscr_dev_ptr->COUNT = 0;
   }

   return( MQX_OK );
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_ioctl
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    This driver function performs various commands upon the opened TCHSCR
*    device.
*
*END*----------------------------------------------------------------------*/
static _mqx_int _tchres_ioctl
(
   /* [IN] the handle returned from _fopen */
   MQX_FILE_PTR     handle_ptr,

   /* [IN] the ioctl command */
   _mqx_uint   cmd,

   /* [IN] the ioctl parameters */
   pointer     param_ptr
)
{
   IO_DEVICE_STRUCT_PTR       io_dev_ptr;
   TCHRES_DEVICE_STRUCT_PTR   tchscr_dev_ptr;
   _mqx_int                   result = MQX_OK;
   int_16                        x_pos, y_pos;
   TCHRES_POSITION_STRUCT_PTR position_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)handle_ptr->DEV_PTR;
   tchscr_dev_ptr = (TCHRES_DEVICE_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

   switch (cmd) {
      /* not supported yet
        case IO_IOCTL_TCHSCR_GET_POSITION :
            break;
      case IO_IOCTL_TCHSCR_SET_CALIBRATION :
         break;
      case  IO_IOCTL_TCHSCR_GET_CALIBRATION :
         break;
        */
      case IO_IOCTL_TCHSCR_GET_POSITION_RAW :
         if( param_ptr == NULL )
         {
            result = TCHRES_ERROR_INVALID_PARAMETER;
            break;
         }
         /* Call the generic get_possition_raw function */
         result = _tchres_get_position_raw( &x_pos, &y_pos, tchscr_dev_ptr );
         if( result == TCHRES_OK )
         {
            position_ptr = (TCHRES_POSITION_STRUCT_PTR)param_ptr;
            position_ptr->X = x_pos;
            position_ptr->Y = y_pos;
         }

         break;

      case IO_IOCTL_TCHSCR_GET_RAW_LIMITS:
         if( param_ptr != NULL )
         {
            *(TCHRES_ADC_LIMITS_STRUCT_PTR)param_ptr = tchscr_dev_ptr->ADC_LIMITS;
            result = MQX_OK;
         }
         else
         {
            result = TCHRES_ERROR_INVALID_PARAMETER;
         }
         break;

        default:
           /* return unsupported command */
           result = IO_ERROR_INVALID_IOCTL_CMD;
         break;

    } /* end switch */

   return( result );
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_read_touch_axis
* Returned Value   : adc value or 0
* Comments         :
*    Returns the value from ADC from selected channel
*
*END*----------------------------------------------------------------------*/
static uint_16 _tchres_read_touch_axis
(
   TCHRES_PIN_SELECT_ENUM pin_select,
   TCHRES_DEVICE_STRUCT_PTR tchscr_dev_ptr
)
{
   ADC_RESULT_STRUCT data;

   switch( pin_select )
   {
      case X_PLUS:
         ioctl(tchscr_dev_ptr->ADC_CHANNEL_X, ADC_IOCTL_FIRE_TRIGGER, (pointer) tchscr_dev_ptr->ADC_CHANNEL_X_STRUCT->trigger);
         _lwevent_wait_ticks(&tchscr_dev_ptr->TCH_EVENT, tchscr_dev_ptr->ADC_CHANNEL_X_STRUCT->event_mask, TRUE, 0);
         if (read(tchscr_dev_ptr->ADC_CHANNEL_X, &data, sizeof(data)) != 2) {
             return 0;
         }
         break;

      case Y_PLUS:
         ioctl(tchscr_dev_ptr->ADC_CHANNEL_Y, ADC_IOCTL_FIRE_TRIGGER, (pointer) tchscr_dev_ptr->ADC_CHANNEL_Y_STRUCT->trigger);
         _lwevent_wait_ticks(&tchscr_dev_ptr->TCH_EVENT, tchscr_dev_ptr->ADC_CHANNEL_Y_STRUCT->event_mask, TRUE, 0);
         if (read(tchscr_dev_ptr->ADC_CHANNEL_Y, &data, sizeof(data)) != 2) {
             return 0;
         }
         break;
      default:
         return  0;
   }

   return(data.result);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_get_filtered_axis
* Returned Value   : 1 on success
*                    0 when measurement failed
* Comments         :
*    Return filtered value from adc
*
*END*----------------------------------------------------------------------*/
static uchar _tchres_get_filtered_axis
(
        TCHRES_DEVICE_STRUCT_PTR tchscr_dev_ptr,
        TCHRES_PIN_SELECT_ENUM pinId,
        uint_16 *Res
)
{
    #define ABS(a) (((a) < 0) ? -(a) : (a))
    #define NBR_SAMPLES 8
    #define NBR_VALID_SAMPLES 6

    uint_16 wSample[NBR_SAMPLES];
    uint_16 wADCReading, adcMin, adcMax, wRes = 0, wSum = 0;
    uint_8 tmp, tmp_res, deviation, cnt = 0;

    TCHRES_ADC_LIMITS_STRUCT_PTR p_limits = &tchscr_dev_ptr->ADC_LIMITS;
    deviation = (uchar) (p_limits->FULL_SCALE / 256);

    switch (pinId) {
    case X_PLUS:
        adcMin = p_limits->X_TOUCH_MIN;
        adcMax = p_limits->X_TOUCH_MAX;
        break;
    case Y_PLUS:
        adcMin = p_limits->Y_TOUCH_MIN;
        adcMax = p_limits->Y_TOUCH_MAX;
        break;
    default:
        adcMin = 0;
        adcMax = p_limits->FULL_SCALE;
    }

    for (tmp = 0; tmp < NBR_SAMPLES; tmp++) {
        //reading sample from ADC
        wADCReading = _tchres_read_touch_axis(pinId, tchscr_dev_ptr);

        /* check if input value is from valid range */
        if((wADCReading < adcMin) || (wADCReading > adcMax)) {
            break;
        }

        wSample[tmp] = wADCReading;
        wSum += wADCReading;
    }

    //measuring failed due to exceeded sample range
    if (tmp < NBR_SAMPLES) {
        return 0;
    }

    wRes = wSum / NBR_SAMPLES;

    for (tmp = 0, tmp_res = 0; tmp < NBR_SAMPLES; tmp++) {
        if (ABS(wSample[tmp] - wRes) < deviation) {
            tmp_res++;
        }
    }

    if (tmp_res >= NBR_VALID_SAMPLES) {
        *Res = wRes;
        return 1;
    } else {
        return 0;
    }
}

/*FUNCTION*-------------------------------------------------------------------------------
*
* Function Name    : _tchres_set_pins_to_default
* Returned Value   : none
* Comments         : set X, Y pins to default state - ADC off, pins low, output direction
*
*
*END*-----------------------------------------------------------------------------------*/
static void _tchres_set_pins_to_default
(
   TCHRES_DEVICE_STRUCT_PTR tchscr_dev_ptr
)
{
   lwgpio_set_direction( &tchscr_dev_ptr->X_PLUS_PIN, LWGPIO_DIR_OUTPUT );
   lwgpio_set_direction( &tchscr_dev_ptr->X_MINUS_PIN, LWGPIO_DIR_OUTPUT );
   lwgpio_set_direction( &tchscr_dev_ptr->Y_PLUS_PIN, LWGPIO_DIR_OUTPUT );
   lwgpio_set_direction( &tchscr_dev_ptr->Y_MINUS_PIN, LWGPIO_DIR_OUTPUT );

   /* Put on touch screen pins into default mode */
   lwgpio_set_value( &tchscr_dev_ptr->X_PLUS_PIN, LWGPIO_VALUE_LOW );
   lwgpio_set_value( &tchscr_dev_ptr->X_MINUS_PIN, LWGPIO_VALUE_LOW );
   lwgpio_set_value( &tchscr_dev_ptr->Y_PLUS_PIN, LWGPIO_VALUE_LOW );
   lwgpio_set_value( &tchscr_dev_ptr->Y_MINUS_PIN, LWGPIO_VALUE_LOW );

   /* Disable ADC function on X+ and Y+ */
#if PSP_MQX_CPU_IS_MCF51JE || PSP_MQX_CPU_IS_MCF51MM
  ioctl(tchscr_dev_ptr->ADC_CHANNEL_X, ADC_IOCTL_PIN_DISABLE, (pointer) 0);
  ioctl(tchscr_dev_ptr->ADC_CHANNEL_Y, ADC_IOCTL_PIN_DISABLE, (pointer) 0);
#else
  lwgpio_set_functionality(&tchscr_dev_ptr->X_PLUS_PIN, tchscr_dev_ptr->PIN_FUNCT.X_PLUS_GPIO_FUNCTION);
  lwgpio_set_functionality(&tchscr_dev_ptr->Y_PLUS_PIN, tchscr_dev_ptr->PIN_FUNCT.Y_PLUS_GPIO_FUNCTION);
#endif

}

/*FUNCTION*------------------------------------------------------------------------
*
* Function Name    : _tchres_measure_position
* Returned Value   : touchPosition with raw value from ADC valid only on TCHRES_OK
*                    TCHRES_OK on success
*                    TCHRES_ERROR_XXX on failure where XXX describes error
* Comments         : Convenient function for measuring either X or Y coordinate.
*                    Measuring is performed on either X+ or Y+ pin, no other pin
*                    is accepted in parameter pin_select.
*
*
*END*----------------------------------------------------------------------------*/
static _mqx_int _tchres_measure_position
(
    /* [IN] Pin ID to use for measurement either X (X+) or Y (Y+) coordinate */
    TCHRES_PIN_SELECT_ENUM pin_select,
    /* [OUT] Output parameter for result */
    int_16 *touchPosition,
    /* [IN] Touch screen device structure pointer */
    TCHRES_DEVICE_STRUCT_PTR tchscr_dev_ptr
)
{
    #define RAW_VALID_VALUE_TIMEOUT 254
    /* Declare and initialize local variables */
    uint_16 tmpCnt = 0, tmpRes = 0, min_voltage = 0;
    uint_32 adc_func;
    MQX_FILE_PTR adc_channel;
    LWGPIO_STRUCT pin_adc, pin_high, pin_z;

    switch (pin_select) {
    case X_PLUS:
        adc_channel = tchscr_dev_ptr->ADC_CHANNEL_X;
        adc_func = tchscr_dev_ptr->PIN_FUNCT.X_PLUS_ADC_FUNCTION;
        pin_adc = tchscr_dev_ptr->X_PLUS_PIN;
        pin_high = tchscr_dev_ptr->Y_PLUS_PIN;
        pin_z = tchscr_dev_ptr->X_MINUS_PIN;
        min_voltage = _tchres_get_raw_limits(X_TOUCH_MIN, tchscr_dev_ptr);
        break;
    case Y_PLUS:
        adc_channel = tchscr_dev_ptr->ADC_CHANNEL_Y;
        adc_func = tchscr_dev_ptr->PIN_FUNCT.Y_PLUS_ADC_FUNCTION;
        pin_adc = tchscr_dev_ptr->Y_PLUS_PIN;
        pin_high = tchscr_dev_ptr->X_PLUS_PIN;
        pin_z = tchscr_dev_ptr->Y_MINUS_PIN;
        min_voltage = _tchres_get_raw_limits(Y_TOUCH_MIN, tchscr_dev_ptr);
        break;
    default:
        return TCHRES_ERROR_INVALID_PARAMETER;
    }

    /* Set pins to default values */
    _tchres_set_pins_to_default(tchscr_dev_ptr);

    /* Switch to ADC */
 #if PSP_MQX_CPU_IS_MCF51JE || PSP_MQX_CPU_IS_MCF51MM
    ioctl(adc_channel, ADC_IOCTL_PIN_ENABLE, (pointer) 0);
 #else
    lwgpio_set_functionality(&pin_adc, adc_func);
 #endif

    /* Check that the touch screen surface is without any voltage and prepared */
    while (_tchres_read_touch_axis(pin_select, tchscr_dev_ptr) > min_voltage)
    {
        if (++tmpCnt >= RAW_VALID_VALUE_TIMEOUT) break;
    }

    /* test for tmpCnt overflow */
    if(tmpCnt >= RAW_VALID_VALUE_TIMEOUT) {
        return (TCHRES_ERROR_TIMEOUT);
    }

    /* When the touch screen surface is prepared */
    /* Switch on pin_high wire into high level */
    lwgpio_set_value(&pin_high, LWGPIO_VALUE_HIGH);

    /* Switch on pin_z wire into high Z mode to keep measurement non affected */
    lwgpio_set_direction(&pin_z, LWGPIO_DIR_INPUT);

    if(_tchres_get_filtered_axis(tchscr_dev_ptr, pin_select, &tmpRes)) {
        *touchPosition = tmpRes;
        return TCHRES_OK;
    } else {
        return TCHRES_ERROR_NO_TOUCH;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_get_position_raw
* Returned Value   : unsigned int *TouchPositionX    Pointer to X coordinate
*                    unsigned int *TouchPositionY    Pointer to Y coordinate
*                    TCHRES_OK or some kind of TCHRES_ERROR
* Comments         : returns the filtered coordinate values from adc
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _tchres_get_position_raw(
        int_16 *TouchPositionX,
        int_16 *TouchPositionY,
        TCHRES_DEVICE_STRUCT_PTR tchscr_dev_ptr)
{
    _mqx_int result;
    result = _tchres_measure_position(Y_PLUS, TouchPositionY, tchscr_dev_ptr);
    if (result == TCHRES_OK) {
        result = _tchres_measure_position(X_PLUS, TouchPositionX, tchscr_dev_ptr);
    }

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _tchres_get_raw_limits
* Returned Value   : none
* Comments         :
*    Return selected limit for adc
*
*END*----------------------------------------------------------------------*/
static int_16 _tchres_get_raw_limits
(
   TCHRES_LIMITS_ENUM limit,
   TCHRES_DEVICE_STRUCT_PTR tchscr_dev_ptr
)
{
   int_16 result = 0;

   switch( limit )
   {
      case FULL_SCALE:
         result = tchscr_dev_ptr->ADC_LIMITS.FULL_SCALE;
         break;
      case X_TOUCH_MIN:
         result = tchscr_dev_ptr->ADC_LIMITS.X_TOUCH_MIN;
         break;
      case Y_TOUCH_MIN:
         result = tchscr_dev_ptr->ADC_LIMITS.Y_TOUCH_MIN;
         break;
      case X_TOUCH_MAX:
         result = tchscr_dev_ptr->ADC_LIMITS.X_TOUCH_MAX;
         break;
      case Y_TOUCH_MAX:
         result = tchscr_dev_ptr->ADC_LIMITS.Y_TOUCH_MAX;
         break;
      default:
      break;
   }
   return result;
}
/* EOF */

