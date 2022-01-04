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
* $FileName: tchres_prv.h$
* $Version : 3.8.7.0$
* $Date    : Sep-22-2011$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the SPI driver
*
*END************************************************************************/
#ifndef __tchres_prv_h__
#define __tchres_prv_h__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
/* for later use */
typedef struct tchres_calib_struct
{
  uint_16   X_RESOLUTION;
  uint_16   Y_RESOLUTION;
  int_16    K_X;
  int_16    Q_X;
  int_16    K_Y;
  int_16    Q_Y;
} TCHRES_CALIB_STRUCT, _PTR_ TCHRES_CALIB_STRUCT_PTR ;
#endif

typedef struct tchres_struct
{
    /* pin information */
    LWGPIO_STRUCT X_PLUS_PIN;
    LWGPIO_STRUCT Y_PLUS_PIN;
    LWGPIO_STRUCT X_MINUS_PIN;
    LWGPIO_STRUCT Y_MINUS_PIN;

    /* every pin can have another mux mask */
    TCHRES_PIN_FUNCT_STRUCT PIN_FUNCT;

   /* adc limits structure */
   TCHRES_ADC_LIMITS_STRUCT   ADC_LIMITS;
   /* open file counter */
   uchar    COUNT;

   /* ADC device info */
   char_ptr   ADC_XPLUS_DEVICE;
   char_ptr   ADC_YPLUS_DEVICE;
   MQX_FILE_PTR   ADC_CHANNEL_X;
   MQX_FILE_PTR   ADC_CHANNEL_Y;
   ADC_INIT_CHANNEL_STRUCT_PTR   ADC_CHANNEL_X_STRUCT;
   ADC_INIT_CHANNEL_STRUCT_PTR   ADC_CHANNEL_Y_STRUCT;
   LWEVENT_STRUCT TCH_EVENT;

#if 0
    /* callibration data */
    uchar    NUMBER_OF_CALIB_POINTS;
    /* pointer to TCHRES_CALLIBRATION_STRUCT array */
   TCHRES_CALLIBRATION_STRUCT_PTR  CALLIB_POINTS_STRUCT_PTR;
   /* calibration data */
   TCHRES_CALIB_METHOD CALIBRATION_METHOD;
   TCHRES_CALIB_STRUCT        CALIB_DATA;
#endif

} TCHRES_DEVICE_STRUCT, _PTR_ TCHRES_DEVICE_STRUCT_PTR;

typedef enum
{
   X_PLUS,
   X_MINUS,
   Y_PLUS,
   Y_MINUS
} TCHRES_PIN_SELECT_ENUM, _PTR_ TCHRES_PIN_SELECT_ENUM_PTR ;

typedef enum
{
    FULL_SCALE,
    X_TOUCH_MIN,
    Y_TOUCH_MIN,
    X_TOUCH_MAX,
    Y_TOUCH_MAX
} TCHRES_LIMITS_ENUM, _PTR_ TCHRES_LIMITS_ENUM_PTR;

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
