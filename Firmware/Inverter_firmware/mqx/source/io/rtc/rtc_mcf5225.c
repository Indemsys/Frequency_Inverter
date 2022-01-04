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
* $FileName: rtc_mcf5225.c$
* $Version : 3.6.7.0$
* $Date    : Nov-2-2010$
*
* Comments:
*
*   Processor family specific file needed for RTC module.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <rtc.h>

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_rtc_base_address
* Returned Value   : pointer to base of RTC registers
* Comments         :
*    This function returns base address of RTC related register space.
*
*END*********************************************************************/
pointer _bsp_get_rtc_base_address 
(
    void
)
{
    return (pointer)(&((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->RTC);
}

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_rtc_vector
* Returned Value   : MQX vector number for specified interrupt
* Comments         :
*    This function returns index into MQX interrupt vector table for
*    RTC interrupt.
*
*END*********************************************************************/
PSP_INTERRUPT_TABLE_INDEX _bsp_get_rtc_vector 
(
    void
)
{
    return MCF5225_INT_RTC;
}

