/**HEADER********************************************************************
*
* Copyright (c) 2012 Freescale Semiconductor;
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
* $FileName: cortexa5.h$
* $Version : 3.8.7.0$
* $Date    : Oct-2-2012$
*
* Comments:
* ARM Nested Vectored Interrupt Controller (NVIC)
*
*END************************************************************************/

#include "mqx_inc.h"
#include "nvic.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _nvic_int_init
* Returned Value  : uint_32
*       MQX_OK or error code
* Comments        :
*  	Initialize a specific interrupt in the cortex core nvic
*
*END*---------------------------------------------------------------------*/

_mqx_uint _nvic_int_init
   (
      // [IN} Interrupt number
      _mqx_uint irq,

      // [IN} Interrupt priority
      _mqx_uint prior,

      // [IN] enable the interrupt now?
      boolean enable
   )
{
	VCORTEX_NVIC_STRUCT_PTR nvic = (VCORTEX_NVIC_STRUCT_PTR)&(((CORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->NVIC);
	_mqx_uint ext_irq_no = irq - 16;

	// check priority value, must be below maximal enabled/set value
	if (prior >= (1 << CORTEX_PRIOR_IMPL)) {
		return MQX_INVALID_PARAMETER;
	}

	if (irq >= PSP_INT_FIRST_INTERNAL && irq <= PSP_INT_LAST_INTERNAL) {
		nvic->PRIORITY[ext_irq_no >> 2] = (nvic->PRIORITY[ext_irq_no >> 2] & ~(0xff << ((ext_irq_no & 3) * 8))) | (((prior << CORTEX_PRIOR_SHIFT) & CORTEX_PRIOR_MASK) << ((ext_irq_no & 3) * 8));

		if (enable)
			_nvic_int_enable(irq);
		else
			_nvic_int_disable(irq);

    }
    else
        return MQX_INVALID_PARAMETER;

    return MQX_OK;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _nvic_int_enable
* Returned Value  : uint_32
*       MQX_OK or error code
* Comments        :
* 	Enable interrupt on cortex core NVIC
*
*END*---------------------------------------------------------------------*/
_mqx_uint _nvic_int_enable
   (
      // [IN] Interrupt number
      _mqx_uint  irq
   )
{
    VCORTEX_NVIC_STRUCT_PTR nvic = (VCORTEX_NVIC_STRUCT_PTR)&(((CORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->NVIC);
    uint_32 ext_irq_no = irq - 16;

    if (ext_irq_no >= PSP_INT_FIRST_INTERNAL && ext_irq_no <= PSP_INT_LAST_INTERNAL) {
        nvic->ENABLE[ext_irq_no >> 5] = 1 << (ext_irq_no & 0x1f);
    }
    else
        return MQX_INVALID_PARAMETER;

    return MQX_OK;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _nvic_int_disable
* Returned Value  : uint_32
*       MQX_OK or error code
* Comments        :
* 	Disable interrupt on cortex core NVIC
*
*END*---------------------------------------------------------------------*/

_mqx_uint _nvic_int_disable
   (
      // [IN] Interrupt number
      _mqx_uint  irq
   )
{
    VCORTEX_NVIC_STRUCT_PTR nvic = (VCORTEX_NVIC_STRUCT_PTR)&(((CORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->NVIC);
    uint_32 ext_irq_no = irq - 16;

    if (ext_irq_no >= PSP_INT_FIRST_INTERNAL && ext_irq_no <= PSP_INT_LAST_INTERNAL) {
        nvic->DISABLE[ext_irq_no >> 5] = 1 << (ext_irq_no & 0x1f);
    }
    else
        return MQX_INVALID_PARAMETER;

    return MQX_OK;
}
