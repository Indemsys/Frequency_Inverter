/**HEADER********************************************************************
*
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: tchres.h$
* $Version : 3.8.10.0$
* $Date    : Oct-13-2011$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the TCHRES driver
*
*END************************************************************************/
#ifndef __tchres_h__
#define __tchres_h__

#ifdef __cplusplus
extern "C" {
#endif

#define IO_IOCTL_TCHSCR_GET_POSITION     _IO(IO_TYPE_TCHRES,0x01)
#define IO_IOCTL_TCHSCR_GET_POSITION_RAW _IO(IO_TYPE_TCHRES,0x02)
#define IO_IOCTL_TCHSCR_SET_CALIBRATION  _IO(IO_TYPE_TCHRES,0x03)
#define IO_IOCTL_TCHSCR_GET_CALIBRATION  _IO(IO_TYPE_TCHRES,0x04)
#define IO_IOCTL_TCHSCR_GET_RAW_LIMITS   _IO(IO_TYPE_TCHRES,0x05)

#define TCHRES_OK                      (0x00)
#define TCHRES_ERROR_INVALID_PARAMETER (TCHRES_ERROR_BASE | 0x01)
#define TCHRES_ERROR_NO_TOUCH          (TCHRES_ERROR_BASE | 0x02)
#define TCHRES_ERROR_TIMEOUT           (TCHRES_ERROR_BASE | 0x03)

/* adc event macros */
#define TCH_ADC_X_COMPLETE  0x01
#define TCH_ADC_Y_COMPLETE  0x02

/* ADC channels settings macros - change, if needed */
#define TCHRES_ADC_FLAGS         (ADC_CHANNEL_MEASURE_ONCE | ADC_CHANNEL_START_TRIGGERED)
#define TCHRES_ADC_NUM_SAMPLES   (1)
#define TCHRES_ADC_TIME_PERIOD   (250)
#define TCHRES_ADC_RANGE         (0x10000)
#define TCHRES_ADC_BUFFER_SIZE   (5)

typedef enum
{
   TWO_POINT_CALIB
}  TCHRES_CALIB_METHOD;

/* pin function structure */
typedef struct tchres_pin_funct_struct
{
   uint_32        X_PLUS_GPIO_FUNCTION;
   uint_32        X_PLUS_ADC_FUNCTION;
   uint_32        Y_PLUS_GPIO_FUNCTION;
   uint_32        Y_PLUS_ADC_FUNCTION;
   uint_32        X_MINUS_GPIO_FUNCTION;
   uint_32        Y_MINUS_GPIO_FUNCTION;
} TCHRES_PIN_FUNCT_STRUCT, _PTR_ TCHRES_PIN_FUNCT_STRUCT_PTR;

/* pin connection structure, defines the gpio pin for touch screen interface */
typedef struct tchres_pin_config_struct
{
   /* BSP_X_PLUS… must be defined in device.h in gpio driver naming standard */
   /* pin configuration */
   LWGPIO_PIN_ID       X_PLUS;    /* x + gpio pin definition */
   LWGPIO_PIN_ID       X_MINUS;   /* x - gpio pin definition */
   LWGPIO_PIN_ID       Y_PLUS;    /* y + gpio pin definition */
   LWGPIO_PIN_ID       Y_MINUS;   /* y - gpio pin definition */
   /* gpio and adc pin mux masks */
   TCHRES_PIN_FUNCT_STRUCT PIN_FUNCT;

} TCHRES_PIN_CONFIG_STRUCT, _PTR_ TCHRES_PIN_CONFIG_STRUCT_PTR;

/* install parameters - adc devices used for measuring on X+ and Y+ electrodes */
typedef struct tchres_install_param_struct
{
	char_ptr ADC_XPLUS_DEVICE; /* ADC device for X+ electrode */
	char_ptr ADC_YPLUS_DEVICE; /* ADC device for Y+ electrode */
} TCHRES_INSTALL_PARAM_STRUCT, _PTR_ TCHRES_INSTALL_PARAM_STRUCT_PTR;

/* result structure */
typedef struct tchres_position
{
    int_16  X;  /* X position*/
    int_16  Y; /* Y position*/
} TCHRES_POSITION_STRUCT, _PTR_ TCHRES_POSITION_STRUCT_PTR;

/* Adc limits struct */
typedef struct tchres_adc_limits
{
    uint_16   FULL_SCALE;
    int_16    X_TOUCH_MIN;
    int_16    Y_TOUCH_MIN;
    int_16    X_TOUCH_MAX;
    int_16    Y_TOUCH_MAX;
} TCHRES_ADC_LIMITS_STRUCT, _PTR_ TCHRES_ADC_LIMITS_STRUCT_PTR;

/* Init structure */
typedef struct tchres_init_struct
{
   /* structure with pins connected to touch_screen detection */
   TCHRES_PIN_CONFIG_STRUCT   PIN_CONFIG;

   /* Touch screen adc limits structure */
   TCHRES_ADC_LIMITS_STRUCT   ADC_LIMITS;

   /* Touch screen adc channel X setting structure */
   LWGPIO_PIN_ID              ADC_CHANNEL_X_SOURCE;
   ADT_TRIGGER_MASK           ADC_CHANNEL_X_TRIGGER;

   /* Touch screen adc channel Y setting structure */
   LWGPIO_PIN_ID              ADC_CHANNEL_Y_SOURCE;
   ADT_TRIGGER_MASK           ADC_CHANNEL_Y_TRIGGER;

#if 0 /* for later use */
   /* Touch Screen calibration information */
   TCHRES_CALIB_METHOD CALIBRATION_METHOD;
#endif
} TCHRES_INIT_STRUCT, _PTR_ TCHRES_INIT_STRUCT_PTR;


/* functions */
_mqx_uint _io_tchres_install( char_ptr identifier, TCHRES_INIT_STRUCT_PTR  init_ptr, TCHRES_INSTALL_PARAM_STRUCT_PTR install_params);

#if 0 /* for later use */
/* Calibration struct :
**  this structure defines coordinates for two calibration points upper left
** corner and lower right corner in pixels. Resistive internally recalculate
** this coordinates to ADC values after Ioctl_Get_calibrate. */
typedef struct tchres_callibration_struct
{
   /* [X0, Y0] calibration coordinates in pixels */
   int_16   X_CALIB_POS;
   int_16   Y_CALIB_POS;
   int_16   X_CALIB_RAW;
   int_16   Y_CALIB_RAW;
} TCHRES_CALLIBRATION_STRUCT, _PTR_ TCHRES_CALLIBRATION_STRUCT_PTR;
#endif

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
