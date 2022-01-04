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
* $FileName: flash_mcf51xx.c$
* $Version : 3.8.21.0$
* $Date    : Jun-6-2012$
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
   
/* Internal function definitions */
static void mcf51xx_ram_function(volatile uchar_ptr); 
static void mcf51xx_ram_function_end (void);

uchar_ptr mcf51xx_init_ram_function(void);

const FLASHX_BLOCK_INFO_STRUCT _flashx_mcf51xx_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE, (uint_32) BSP_INTERNAL_FLASH_BASE, BSP_INTERNAL_FLASH_SECTOR_SIZE, 0},
    { 0, 0, 0, 0 }
};

const FLASHX_DEVICE_IF_STRUCT _flashx_mcf51xx_if = {
    mcf51xx_flash_erase_sector,
    mcf51xx_flash_write_sector,
    NULL,
    NULL,
    mcf51xx_flash_init,
    mcf51xx_flash_deinit,
    NULL,
    _io_mcf51xx_flash_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf51xx_set_cfmclkd
* Returned Value   : none
* Comments         :
*    Set the CFMCLKD register.
*
*END*----------------------------------------------------------------------*/
void _mcf51xx_set_cfmclkd(uint_8 mem_array)
{
#define PRDIV8_LIMIT_FREQ   12800000L // limit freq. for using PRDIV8_PRSC
#define FLASH_FREQ          200000L   // 190 kHz program frequency
#define PRDIV8_PRSC         8         // prescaller 8
    
    VMCF51XX_FTSR_STRUCT_PTR  ftsr_ptr;
    
    ftsr_ptr = _bsp_get_cfm_address(mem_array);
    
    if(BSP_BUS_CLOCK > PRDIV8_LIMIT_FREQ)
    {
        ftsr_ptr->FCDIV = MCF51XX_FTSR_FCDIV_PRDIV8 | MCF51XX_FTSR_FCDIV_FDIV((BSP_BUS_CLOCK + PRDIV8_PRSC * FLASH_FREQ - 1) / (PRDIV8_PRSC * FLASH_FREQ) - 1);
    }
    else
    {
        ftsr_ptr->FCDIV = MCF51XX_FTSR_FCDIV_FDIV((BSP_BUS_CLOCK + FLASH_FREQ - 1) / FLASH_FREQ - 1);
    }
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_flash_init
* Returned Value   : 
* Comments         :
*   Init flash - store the FSTAT register  
*
*END*----------------------------------------------------------------------*/
boolean mcf51xx_flash_init
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    volatile MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    uint_8 array;
    VMCF51XX_FTSR_STRUCT_PTR ftsr_ptr;
    
    dev_spec_ptr = _mem_alloc(sizeof(MCF51XX_FLASH_INTERNAL_STRUCT));
#if MQX_CHECK_FOR_MEMORY_ALLOCATION_ERRORS
    /* test for correct memory allocation */
    if(dev_spec_ptr == NULL){
        return FALSE;
    }
#endif //MQX_CHECK_FOR_MEMORY_ALLOCATION_ERRORS

    /* get the pointer to cfm registers structure */
#ifndef PSP_HAS_DUAL_FLASH
    array = MEMORY_ARRAY_STD;
#else
    /* RTC_CFG_DATA[CFG0] = 0 : FSTR1 => 0x00000000 - see Flash Array Base Address table */
    array = (dev_ptr->BASE_ADDR == 0) ? MEMORY_ARRAY_1 : MEMORY_ARRAY_2;
    if (_mcf51_get_rtc_cfg_state() == 1) {
        /* if flash are already swapped, then swap the pointers */
        array = (array == MEMORY_ARRAY_1) ? MEMORY_ARRAY_2 : MEMORY_ARRAY_1;
    }
#endif //PSP_HAS_DUAL_FLASH
    ftsr_ptr = _bsp_get_cfm_address(array);

    dev_spec_ptr->ftsr_ptr = (vuchar_ptr)ftsr_ptr;
    dev_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;
    
    /* save pointer to function in ram - this is the default settings  */
    dev_spec_ptr->flash_execute_code_ptr = mcf51xx_init_ram_function();
    
    /* set correct clocking for CFM module */
    _mcf51xx_set_cfmclkd(array);

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_flash_deinit
* Returned Value   : 
* Comments         :
*    release used memory      
*
*END*----------------------------------------------------------------------*/
void mcf51xx_flash_deinit
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    
    /* if the flash write,erase code run form RAM, free memory */
    if (dev_ptr->FLAGS & FLASHX_WR_ERASE_FROM_FLASH_ENABLED == 0) {
        /* de-allocate the ram function buffer */
        if(dev_spec_ptr->flash_execute_code_ptr) {
            _mem_free(dev_spec_ptr->flash_execute_code_ptr);
        }
    }
    dev_spec_ptr->flash_execute_code_ptr = NULL;
    /* de-allocate device specific structure */
    _mem_free(dev_ptr->DEVICE_SPECIFIC_DATA);
    dev_ptr->DEVICE_SPECIFIC_DATA = NULL; 
}   

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_init_ram_function
* Returned Value   : pointer to allocated RAM function
* Comments         :
*   allocate and copy flash RAM function  
*
*END*----------------------------------------------------------------------*/
uchar_ptr mcf51xx_init_ram_function()
{
    uchar_ptr ram_code_ptr;
    
    /* Allocate space on stack to run flash command out of SRAM */
    ram_code_ptr = _mem_alloc((char*)mcf51xx_ram_function_end - (char*)mcf51xx_ram_function);   
    /* copy code to RAM buffer */
    _mem_copy((void*)mcf51xx_ram_function, ram_code_ptr, (char*)mcf51xx_ram_function_end  - (char*)mcf51xx_ram_function);

    return ram_code_ptr;               
}
    
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_erase_sector_ext
* Returned Value   : true if success, or false
* Comments         :
*   Erase a flash memory block  
*
*END*----------------------------------------------------------------------*/
boolean mcf51xx_flash_erase_sector
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr, 

    /* [IN] Erased sector address */
    char_ptr             from_ptr,

    /* [IN] Erased sector size */
    _mem_size            size
)
{
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    VMCF51XX_FTSR_STRUCT_PTR ftsr_ptr;
    int (*RunInRAM)(uchar_ptr);
    uint_32 temp;

    /* select the proper ramcode function */
    *RunInRAM = (int(*)(uchar_ptr))dev_spec_ptr->flash_execute_code_ptr;
    
    /* get the pointer to cfm registers structure */
    ftsr_ptr = (VMCF51XX_FTSR_STRUCT_PTR)dev_spec_ptr->ftsr_ptr;   
    
    _int_disable();
    /* prepare flash write operation, clear flags and wait for ready state */
    ftsr_ptr->FSTAT = (MCF51XX_FTSR_FSTAT_FPVIOL | MCF51XX_FTSR_FSTAT_FACCERR);
    while (!(ftsr_ptr->FSTAT & MCF51XX_FTSR_FSTAT_FCBEF)){
        /* wait */
    };

    /* latch address in Flash */
    (*(volatile vuint_32 *)(from_ptr)) = (vuint_32)-1;  
    
    /* issue erase command */
    ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_SECTOR_ERASE;
    
    /* run command and wait for it to finish (must execute from RAM) */ 
    temp = _psp_get_sr();
    _psp_set_sr(temp | 0x0700);
    RunInRAM( (volatile uchar_ptr)&ftsr_ptr->FSTAT );    
    _psp_set_sr(temp);
    
    if (ftsr_ptr->FSTAT & (MCF51XX_FTSR_FSTAT_FACCERR | MCF51XX_FTSR_FSTAT_FPVIOL)) {
        _int_enable();
        return FALSE;
    }
        
    _int_enable();
    return(TRUE);   
         
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_write_sector
* Returned Value   : true if success, or false
* Comments         :
*   Performs a write into flash memory  
*
*END*----------------------------------------------------------------------*/
boolean mcf51xx_flash_write_sector
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr, 

    /* [IN] Source address */
    char_ptr             from_ptr,

    /* [IN] Destination address */
    char_ptr             to_ptr,

    /* [IN] Number of bytes to write */
    _mem_size            size
) 
{
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    VMCF51XX_FTSR_STRUCT_PTR    ftsr_ptr;
    VMCF51XX_PMC_STRUCT_PTR     pmc_ptr; 
    int (*RunInRAM)( uchar_ptr );
    volatile uint_32 temp, temp_data = 0xFFFFFFFF;
    volatile char_ptr temp_data_ptr;
    uchar byte_data_counter = 0;
    #define BYTES_IN_WORD   4
    /* get the offset in write word */
    uint_32 offset = (uint_32)to_ptr & 0x00000003;

    temp_data_ptr = (char_ptr)&temp_data;
    
    /* select the proper ramcode function */
    RunInRAM = (int(*)( uchar_ptr ))dev_spec_ptr->flash_execute_code_ptr;

    /* get the pointer to cfm registers structure */
    ftsr_ptr = (VMCF51XX_FTSR_STRUCT_PTR)dev_spec_ptr->ftsr_ptr;
    
    /* get the pointer to pmc registers structure */
    pmc_ptr = _bsp_get_pmc_address();    
    /* Clear any errors */
    _int_disable();
    ftsr_ptr->FSTAT = (MCF51XX_FTSR_FSTAT_FPVIOL | MCF51XX_FTSR_FSTAT_FACCERR); 
    
    /* if the start address !=  doesn't correspond with hardware, prepare
       variables for 1st word write */
    if(offset){
        /* Align pointer to writable address */
        to_ptr -= offset;
        /* jump over old data */
        byte_data_counter = offset; 
    }
    /* while are some data to write */
    while(size){
        /* move data to write word */       
        while( byte_data_counter < BYTES_IN_WORD && size ){
            *(temp_data_ptr+byte_data_counter) = *from_ptr++;
            byte_data_counter++;
            size--;
        }
        /* test the LVDF flag - if 1, we need to write data in 2 steps */
        if( pmc_ptr->SPMSC1 & MCF51XX_PMC_SPMSC1_LVDF) {
            /* write odd bytes */
            (*(vuint_32 *)(to_ptr)) = temp_data | 0x00FF00FF;
            /* write command to CFMCMD */
            ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_BURST_PROGRAM;
            /* run command and wait for it to finish (must execute from RAM) */     
            temp = _psp_get_sr();
            _psp_set_sr(temp | 0x0700);
            RunInRAM( (volatile uchar_ptr)&ftsr_ptr->FSTAT );
            _psp_set_sr(temp);
            /* write even bytes */              
            (*(vuint_32 *)(to_ptr)) = temp_data | 0xFF00FF00;
            /* write command to CFMCMD */
            ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_BURST_PROGRAM;
            /* run command and wait for it to finish (must execute from RAM) */     
            temp = _psp_get_sr();
            _psp_set_sr(temp | 0x0700);
            RunInRAM( (volatile uchar_ptr)&ftsr_ptr->FSTAT );
            _psp_set_sr(temp);
        } 
        else {  
            /* move write data to register */
            (*(vuint_32 *)(to_ptr)) = temp_data;
            
            /* write command to CFMCMD */
            ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_BURST_PROGRAM;
            /* run command and wait for it to finish (must execute from RAM) */     
            temp = _psp_get_sr();
            _psp_set_sr(temp | 0x0700);
            RunInRAM( (volatile uchar_ptr)&ftsr_ptr->FSTAT );                
            _psp_set_sr(temp);
        }
        /* Check for Errors */
        if (ftsr_ptr->FSTAT & (MCF51XX_FTSR_FSTAT_FPVIOL | MCF51XX_FTSR_FSTAT_FACCERR)) {
            _int_enable();
            return (FALSE);
        }
        
        /* init variables for next loop */
        to_ptr += BYTES_IN_WORD;
        byte_data_counter = 0;
        temp_data = 0xFFFFFFFF;
    }
    _int_enable();
    return (TRUE);  
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_mcf51xx_ioctl
* Returned Value   : TRUE if successful
* Comments         : 
*    flash ioctl function
* 
*END*----------------------------------------------------------------------*/
_mqx_int _io_mcf51xx_flash_ioctl
   (
      /* [IN] the handle returned from _fopen */
      IO_FLASHX_STRUCT_PTR   dev_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      pointer                param_ptr
   )
{ /* Body */
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    _mqx_uint   result = MQX_OK;
    
    switch(cmd) {
        #ifdef PSP_HAS_DUAL_FLASH
        case FLASH_IOCTL_SWAP_FLASH_AND_RESET:
            _mcf51_swap_flash_and_reset();
            break;
        #endif
            
        case FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_ENABLE:
            dev_ptr->FLAGS |= FLASHX_WR_ERASE_FROM_FLASH_ENABLED; 
            if(dev_spec_ptr->flash_execute_code_ptr){
                _mem_free(dev_spec_ptr->flash_execute_code_ptr);
            }
            dev_spec_ptr->flash_execute_code_ptr = (uchar_ptr)mcf51xx_ram_function;
            break;
            
        case FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_DISABLE:
            dev_ptr->FLAGS &= ~FLASHX_WR_ERASE_FROM_FLASH_ENABLED;
            dev_spec_ptr->flash_execute_code_ptr = mcf51xx_init_ram_function();
            break;
            
        default:
            break;
    }
    return result;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_ram_function
* Returned Value   : 
* Comments         : 
*    Launch the program command and wait until execution complete. This code
*    is required to run in SRAM, 
*    unless FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_ENABLE ioctl command is
*    applied to allow the low level flash write and erase routines being run
*    from internal flash memory (works only with the dual flash memory 
*    controllers).
*    
*END*----------------------------------------------------------------------*/
static void mcf51xx_ram_function(volatile uchar_ptr fstat_ptr) 
{
    /* Clear the FCBEF flag in the FSTAT register by writing a 1 */
    /* to FCBEF to launch the program command. */
    *fstat_ptr |= MCF51XX_FTSR_FSTAT_FCBEF;   
    /* wait until execution complete */
    while (!( *fstat_ptr & MCF51XX_FTSR_FSTAT_FCCF)){
    };
}


static void mcf51xx_ram_function_end (void) {}
