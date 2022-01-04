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
* $FileName: timer_kinetis.c$
* $Version : 3.8.3.0$
* $Date    : Aug-21-2012$
*
* Comments:
*
*   This file contains board-specific LPT initialization functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "timer_qpit.h"

PSP_INTERRUPT_TABLE_INDEX _qpit_vectors[] =
{
	INT_PIT0,  
	INT_PIT1,
	INT_PIT2,
	INT_PIT3
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_qpit_base_address
* Returned Value   : none
* Comments         :
*    Get base address of qpit
*
*END*------------------------------------------------------------------------*/

VQPIT_REG_STRUCT_PTR _bsp_get_qpit_base_address
    (
        /* [IN] PIT index */
    	uint_32 timer  	
    )
{
    if (timer != 0) 
    	return NULL;
    
    return (VQPIT_REG_STRUCT_PTR)PIT_BASE_PTR;	 
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_qpit_vector
* Returned Value   : Vector number upon success, 0 upon failure
* Comments         :
*    This function returns desired interrupt vector number for specified PIT
*
*END*----------------------------------------------------------------------*/

PSP_INTERRUPT_TABLE_INDEX _bsp_get_qpit_vector
    (
        uint_32 timer,
        uint_32 channel
    )
{
	if (timer != 0)
		return INT_Initial_Stack_Pointer; //we return in fact 0 to inform that error occured
	
    return (PSP_INTERRUPT_TABLE_INDEX) _qpit_vectors[channel];  
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_qpit_clk_en
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function enables clock to corresponding MTIM
*
*END*----------------------------------------------------------------------*/
void _bsp_qpit_clk_en (uint_32 timer)
{
    switch (timer) {
        case 0:
        	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable PIT Module Clock
          break;
    }
}

