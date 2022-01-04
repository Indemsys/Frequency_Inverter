/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: flash_mcf51em.c$
* $Version : 3.8.7.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions for internal flash read, write, erase 
*
*END************************************************************************/
#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_mcf51xx.h"
#include "flash_mcf51xx_prv.h"
#include "flash_mcf51em.h"

static void mcf51em_swap_flash_ram_end(void);
static void mcf51em_swap_flash_ram(VMCF51EM_RTC_STRUCT_PTR rtc_ptr);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_cfm_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the FTSR
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_cfm_address(uchar fstr_set)
{
    if ( fstr_set == FSTR_SET_1) {    
        return (pointer)(&((VMCF51EM_STRUCT_PTR)BSP_IPSBAR)->FTSR1);
    } 
    /* FTSR_SET_2 */
    else {
        return (pointer)(&((VMCF51EM_STRUCT_PTR)BSP_IPSBAR)->FTSR2);
    }        
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_pmc_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the PMC
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_pmc_address()
{
   return (pointer)(&((VMCF51EM_STRUCT_PTR)BSP_IPSBAR)->PMC);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf51em_get_rtc_cfg_state
* Returned Value   : Value of bit CFG0
* Comments         :
*    This function returns the value of RTC_CFG_DATA_CFG0.
*    There is stored information which flash bank is addressed from 0
*
*END*----------------------------------------------------------------------*/
uint_32 _mcf51_get_rtc_cfg_state()
{
    uint_32 result;
    
    if ( ( ((VMCF51EM_STRUCT_PTR)BSP_IPSBAR)->RTC.RTC_CFG_DATA ) & RTC_CFG_DATA_CFG0 ){    
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf51_swap_flash_and_reset
* Returned Value   : none
* Comments         :
*    Swap the flash adresses and restart device.
*
*END*----------------------------------------------------------------------*/
void _mcf51_swap_flash_and_reset()
{
    
    uchar_ptr ramcode_ptr = NULL;
    int_32 (*RunInRAM)( VMCF51EM_RTC_STRUCT_PTR );
    VMCF51EM_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    
    /* Allocate space on stack to run flash command out of SRAM */
    ramcode_ptr = _mem_alloc((char*)mcf51em_swap_flash_ram_end - (char*)mcf51em_swap_flash_ram); 
    /* copy code to RAM buffer */
    _mem_copy((void*)mcf51em_swap_flash_ram, ramcode_ptr, (char*)mcf51em_swap_flash_ram_end  - (char*)mcf51em_swap_flash_ram);
    *RunInRAM = (int_32(*)( VMCF51EM_RTC_STRUCT_PTR ))ramcode_ptr;   

    /* start the swap flash sequence */
    _rtc_unlock();
    /* Run the code from RAM */
    RunInRAM( rtc );
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : mcf51em_swap_flash_ram
* Returned Value   : none
* Comments         :
*    Finish the swap of flash adresses and restart device. This function is
*    copied to RAM
*
*    WARNING! This code doesn't run correctly from debuger. If debuger is used,
*    instead reset jump to exception occured.
*
*END*----------------------------------------------------------------------*/
static void mcf51em_swap_flash_ram(VMCF51EM_RTC_STRUCT_PTR rtc_ptr) 
{
    void (_CODE_PTR_ jump_to_address)(void);
    uint_32 temp;

    /* disable flash speculation */
    _psp_set_cpucr(0x02000000);
    
    /* disable all interrupts */
    temp = _psp_get_sr();
    _psp_set_sr(temp | 0x0700);

    /* toggle RTC_CFG_DATA[CFG0] */
    rtc_ptr->RTC_CFG_DATA ^= 1;
    
    /* wait after write to RTC_CFG_DATA */
    _ASM_NOP();
    _ASM_NOP();
    _ASM_NOP();

    /* The processor generates a reset if CPUCR[ARD] = 0 */
    _psp_set_cpucr(0);
    
    /* reset using illegal address jump */
    jump_to_address = (void (_CODE_PTR_)(void))0x00804001;
    jump_to_address();

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : mcf51em_swap_flash_ram_end
* Returned Value   : none
* Comments         :
*    End-mark function
*
*END*----------------------------------------------------------------------*/
static void mcf51em_swap_flash_ram_end(void) {}

/* EOF */
