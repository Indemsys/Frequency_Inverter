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
* $FileName: bsp_prv.h$
* $Version : 3.8.3.0$
* $Date    : Oct-1-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for initialization of the card.
*
*END************************************************************************/
#ifndef _bsp_prv_h
#define _bsp_prv_h 1
#ifdef __cplusplus
extern "C" {
#endif

/*
**  FUNCTION PROTOTYPES
*/

extern void     __init(void);
extern uint_32  _bsp_get_hwticks(pointer);
extern void     _bsp_exit_handler(void);
extern void     _bsp_timer_isr(pointer);
extern uint_32  _rtc_int_install(INT_ISR_FPTR); 
extern void     _bsp_ddr2_setup (void);
extern void     _bsp_flexbus_pccard_setup (const uint_32 base_address);
extern void     systick_config(uint32_t bsp_system_clock, uint32_t bsp_alarm_frequency);


/*
**  STRUCTURE DEFINITIONS
*/

/* I/O initialization controlled by initialization structures for each channel */
extern const KUART_INIT_STRUCT _bsp_sci0_init;
extern const KUART_INIT_STRUCT _bsp_sci1_init;
extern const KUART_INIT_STRUCT _bsp_sci2_init;
extern const KUART_INIT_STRUCT _bsp_sci3_init;
extern const KUART_INIT_STRUCT _bsp_sci4_init;
extern const KUART_INIT_STRUCT _bsp_sci5_init;

extern const SPI_INIT_STRUCT _bsp_spi0_init;
extern const SPI_INIT_STRUCT _bsp_spi1_init;
extern const SPI_INIT_STRUCT _bsp_spi2_init;

extern const KI2C_INIT_STRUCT _bsp_i2c0_init;
extern const KI2C_INIT_STRUCT _bsp_i2c1_init;

extern const KADC_INIT_STRUCT _bsp_adc0_init;
extern const KADC_INIT_STRUCT _bsp_adc1_init;
extern const KADC_INIT_STRUCT _bsp_adc2_init;
extern const KADC_INIT_STRUCT _bsp_adc3_init;

extern const TCHRES_INIT_STRUCT _bsp_tchscr_resisitve_init;
extern const PCCARDFLEXBUS_INIT_STRUCT  _bsp_cfcard_init;
extern const IODEBUG_INIT_STRUCT _bsp_iodebug_init;

extern const KUSB_DCD_INIT_STRUCT _bsp_usb_dcd_init;
extern const ESDHC_INIT_STRUCT _bsp_esdhc0_init;
extern const FLASHX_FILE_BLOCK _bsp_flashx_file_blocks[];
extern const FLASHX_INIT_STRUCT _bsp_flashx_init;
extern const NANDFLASH_INIT_STRUCT _bsp_nandflash_init;
extern const SAI_INIT_STRUCT _bsp_sai_init;

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

