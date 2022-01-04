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
* $FileName: init_tchres.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definition for the baud rate for the TCHRES
*   channel
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

#define FULL_SCALE  ((1 << 12) - 1) //12-bit ADC
#define IO_PIN_FUNC  1
#define ADC_FUNC     0

/* Initialization structure */
const TCHRES_INIT_STRUCT _bsp_tchscr_resisitve_init = {
    /* structure with pins connected to touch_screen detection */
    {
      BSP_LCD_TCHRES_X_PLUS,       /* x + gpio pin definition */
      BSP_LCD_TCHRES_X_MINUS,      /* x - gpio pin definition */
      BSP_LCD_TCHRES_Y_PLUS,       /* y + gpio pin definition */
      BSP_LCD_TCHRES_Y_MINUS,      /* y - gpio pin definition */
      {
         IO_PIN_FUNC,                 /* x + gpio mux index */
         ADC_FUNC,                    /* x + adc mux index */
         IO_PIN_FUNC,                 /* y + gpio mux index */
         ADC_FUNC,                    /* y + adc mux index */
         IO_PIN_FUNC,                 /* x - gpio mux index */
         IO_PIN_FUNC,                 /* y - gpio mux index */
      },
   },

   /* Adc limits struct */
   {
        FULL_SCALE,             /* FULL_SCALE */
        FULL_SCALE / 8,         /* X_TOUCH_MIN */
        FULL_SCALE / 8,         /* Y_TOUCH_MIN  */
       (FULL_SCALE / 8) * 7,    /* X_TOUCH_OFFSET_MAX */
       (FULL_SCALE / 8) * 7,    /* Y_TOUCH_OFFSET_MAX */
   },

   /* Touch screen adc channel X setting structure */
    BSP_LCD_X_PLUS_ADC_CHANNEL,  /* physical ADC channel */
    BSP_TCHRES_X_TRIGGER,        /* logical trigger ID that starts this ADC channel */

   /* Touch screen adc channel Y setting structure */
    BSP_LCD_Y_PLUS_ADC_CHANNEL,  /* physical ADC channel */
    BSP_TCHRES_Y_TRIGGER,        /* logical trigger ID that starts this ADC channel */
};
/* EOF */
