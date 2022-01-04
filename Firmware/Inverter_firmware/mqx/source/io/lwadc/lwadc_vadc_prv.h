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
* $FileName: lwadc_vadc_prv.h$
* $Version : 3.8.1.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   CPU specific LWADC driver header file
*
*END************************************************************************/

#ifndef __lwadc_vadc_prv_h__
#define __lwadc_vadc_prv_h__

#ifdef __cplusplus
extern "C" {
#endif



#define LWADC_CFG_AVGS_4                ADC_CFG_AVGS(0x00)
#define LWADC_CFG_AVGS_8                ADC_CFG_AVGS(0x01)
#define LWADC_CFG_AVGS_16               ADC_CFG_AVGS(0x02)
#define LWADC_CFG_AVGS_32               ADC_CFG_AVGS(0x03)
#define LWADC_CFG_AVGE                  ADC_CFG_AVGS(0x04)
#define LWADC_CFG_ADCO                  ADC_CFG_AVGS(0x08)
#define LWADC_CFG_CALF                  ADC_CFG_AVGS(0x40)
#define LWADC_CFG_CAL                   ADC_CFG_AVGS(0x80)

#define LWADC_CFG_ADIV_1                ADC_CFG_ADIV(0x00)
#define LWADC_CFG_ADIV_2                ADC_CFG_ADIV(0x01)
#define LWADC_CFG_ADIV_4                ADC_CFG_ADIV(0x02)
#define LWADC_CFG_ADIV_8                ADC_CFG_ADIV(0x03)

#define LWADC_CFG_ADICLK_BUSCLK         ADC_CFG_ADICLK(0x00)
#define LWADC_CFG_ADICLK_BUSCLK2        ADC_CFG_ADICLK(0x01)
#define LWADC_CFG_ADICLK_ALTCLK         ADC_CFG_ADICLK(0x02)
#define LWADC_CFG_ADICLK_ADACK          ADC_CFG_ADICLK(0x03)

extern pointer  _bsp_get_adc_base_address(uint_8);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
