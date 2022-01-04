#ifndef __adc_mcf52xx_h__
#define __adc_mcf52xx_h__
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
* $FileName: adc_mcf522xx.h$
* $Version : 3.6.6.0$
* $Date    : Jun-1-2010$
*
* Comments:
*
*   CPU specific ADC driver header file
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_HW_CHANNELS         8

#define ADC_RESOLUTION_12BIT   0x1000
#define ADC_RESOLUTION_DEFAULT ADC_RESOLUTION_12BIT

#define ADC_SOURCE_AN0      0x00
#define ADC_SOURCE_AN1      0x01
#define ADC_SOURCE_AN2      0x02
#define ADC_SOURCE_AN3      0x03
#define ADC_SOURCE_AN4      0x04
#define ADC_SOURCE_AN5      0x05
#define ADC_SOURCE_AN6      0x06
#define ADC_SOURCE_AN7      0x07

#define ADC_SOURCE_AN0_AN1  0x08
#define ADC_SOURCE_AN2_AN3  0x09
#define ADC_SOURCE_AN4_AN5  0x10
#define ADC_SOURCE_AN6_AN7  0x11
#define ADC_SOURCE_MAX      0x11

pointer _bsp_get_adc_base_address(void);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
