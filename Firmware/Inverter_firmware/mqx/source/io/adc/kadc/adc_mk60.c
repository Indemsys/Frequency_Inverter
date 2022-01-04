/**HEADER***********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: adc_mk60.c$
* $Version : 3.8.2.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   CPU specific ADC driver header file
*
*END***************************************************************************/

#include "mqx.h"
#include "bsp.h"

static const pointer adc_address[] = {
    (pointer)ADC0_BASE_PTR,
    (pointer)ADC1_BASE_PTR
	#if BSP_TWR_K60F120M
	,
	(pointer)ADC2_BASE_PTR,
    (pointer)ADC3_BASE_PTR
	#endif
};

/*FUNCTION**********************************************************************
*
* Function Name    : _bsp_get_adc_base_address
* Returned Value   : pointer to base of ADC registers
* Comments         :
*    This function returns base address of ADC related register space.
*
*END***************************************************************************/
pointer _bsp_get_adc_base_address(uint_32 device_number)
{
    /* Check if device number is correct */
    if (device_number < ELEMENTS_OF(adc_address)) {
        return (pointer)adc_address[device_number];
    }
    return NULL;
}
