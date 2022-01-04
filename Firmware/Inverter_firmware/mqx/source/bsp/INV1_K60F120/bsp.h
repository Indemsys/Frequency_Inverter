/**HEADER***********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: bsp.h$
* $Version : 3.8.2.0$
* $Date    : Aug-20-2012$
*
* Comments:
*
*   This file includes all include files specific to this target system.
*
*END***************************************************************************/

#ifndef __bsp_h__
#define __bsp_h__   1

#include <app_types.h>
#include <psp.h>

/* Processor Expert files */
#include <PE_LDD.h>
#ifdef PE_LDD_VERSION
#include <Events.h>
#endif

#include <bsp_rev.h>
#include <INV1_K60F120.h>

/* Clock manager */
#include <cm_kinetis.h>
#include <bsp_cm.h>
#include <cm.h>
/* Low power manager*/
#include <init_lpm.h>
#include <lpm_kinetis.h>
#include <lpm.h>

#include <fio.h>
#include <io.h>
#include <serial.h>
#include <serl_kuart.h>
#include <spi.h>
#include <spi_dspi.h>
#include <i2c.h>
#include <i2c_ki2c.h>
#include <io_mem.h>
#include <io_null.h>
#include <enet.h>
#include <macnet_mk60.h>
#include <lwgpio_kgpio.h>
#include <lwgpio.h>
#include <io_gpio.h>
#include <esdhc.h>
#include <sdcard.h>
#include <sdcard_spi.h>
#include <sdcard_esdhc.h>
#include <iodebug.h>
#include <apccard.h>
#include <pccardflexbus.h>
#include <pcflash.h>
#include <adc_mk60.h>
#include <adc.h>
#include <adc_kadc.h>
#include <kinetis_fcan.h>
#include <kflexcan.h>
#include <flashx.h>
#include <flash_ftfe.h>
#include <flash_mk60.h>
#include <krtc.h>
#include <krtc_ext.h>
#include <rtc.h>
#include <nandflash.h>
#include <usb_dcd.h>
#include <usb_dcd_kn.h>
#include <usb_dcd_kn_prv.h>
#include <tchres.h>
#include <timer_lpt.h>
#include <timer_qpit.h>
#include <usb_bsp.h>
#include <sai.h>
#include <sai_ksai.h>

#ifdef __cplusplus
extern "C" {
#endif

_mqx_int _bsp_chip_pins_init(void);
_mqx_int _bsp_adc_io_init(_mqx_uint adc_num);
_mqx_int _bsp_adc_channel_io_init(uint_16 source);
_mqx_int _bsp_dspi_io_init(uint_32 dev_num);
_mqx_int _bsp_flexcan_io_init (uint_8);
_mqx_int _bsp_i2c_io_init( uint_32 );
_mqx_int _bsp_gpio_io_init( void );
_mqx_int _bsp_esdhc_io_init (uint_8, uint_16);
_mqx_int _bsp_rtc_io_init( void );
_mqx_int _bsp_serial_io_init(uint_8 dev_num,  uint_8 flags);
_mqx_int _bsp_serial_rts_init( uint_32 );
_mqx_int _bsp_usb_host_io_init(struct usb_host_if_struct *);
_mqx_int _bsp_usb_dev_io_init(struct usb_dev_if_struct *);
_mqx_int _bsp_enet_io_init(_mqx_uint);
boolean  _bsp_get_mac_address(uint_32,uint_32,_enet_address);
_mqx_int _bsp_nandflash_io_init(void);
_mqx_int _bsp_ftfx_io_init(_mqx_uint);
_mqx_int _bsp_sai_io_init(uint_32);

extern const SDCARD_INIT_STRUCT _bsp_sdcard0_init;
extern const LPM_CPU_OPERATION_MODE LPM_CPU_OPERATION_MODES[];

#define _bsp_int_init(num, prior, subprior, enable)     _nvic_int_init(num, prior, enable)
#define _bsp_int_enable(num)                            _nvic_int_enable(num)
#define _bsp_int_disable(num)                           _nvic_int_disable(num)

#ifdef __cplusplus
}
#endif

#endif  /* _bsp_h_ */
/* EOF */
