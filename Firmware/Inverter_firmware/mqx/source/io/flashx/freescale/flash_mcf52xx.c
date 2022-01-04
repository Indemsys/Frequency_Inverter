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
* $FileName: flash_mcf52xx.c$
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
#include "flash_mcf52xx_prv.h"
#include "flash_mcf52xx.h"
   
/* Internal function definitions */
static void mcf52xx_ram_function(volatile char_ptr ); 
static void mcf52xx_ram_function_end(void);
static char_ptr mcf52xx_init_ram_function(void);

const FLASHX_BLOCK_INFO_STRUCT _flashx_mcf52xx_block_map[] =  {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE, (uint_32) BSP_INTERNAL_FLASH_BASE, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { 0, 0, 0 }
};

const FLASHX_DEVICE_IF_STRUCT _flashx_mcf52xx_if = {
    mcf52xx_flash_erase_sector,
    mcf52xx_flash_write_sector,
    NULL,
    NULL,
    mcf52xx_flash_init,
    mcf52xx_flash_deinit,
    NULL,
    NULL
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf52xx_set_cfmclkd
* Returned Value   : none
* Comments         :
*    Set the CFMCLKD register.
*
*END*----------------------------------------------------------------------*/
void _mcf52xx_set_cfmclkd()
{
    #define PRDIV8_LIMIT_FREQ   12800000L // limit freq. for using PRDIV8_PRSC
    #define FLASH_FREQ          200000L   // 200 kHz program frequency
    #define PRDIV8_PRSC         8         // prescaller 8
    
    VMCF52XX_CFM_STRUCT_PTR  cfm_ptr;
    
    cfm_ptr = _bsp_get_cfm_address();
    
    if(BSP_BUS_CLOCK > PRDIV8_LIMIT_FREQ)
    {
        cfm_ptr->CFMCLKD = MCF52XX_CFM_CFMCLKD_PRDIV8 | MCF52XX_CFM_CFMCLKD_DIV((BSP_BUS_CLOCK+PRDIV8_PRSC*FLASH_FREQ-1) / (PRDIV8_PRSC * FLASH_FREQ) -1);
    }
    else
    {
        cfm_ptr->CFMCLKD = MCF52XX_CFM_CFMCLKD_DIV((BSP_BUS_CLOCK+FLASH_FREQ-1) / FLASH_FREQ -1);
    }       
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf_52xx_flash_init
* Returned Value   : 
* Comments         :
*   Erase a flash memory block  
*
*END*----------------------------------------------------------------------*/
boolean mcf52xx_flash_init
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    MCF52XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
#ifdef PSP_HAS_DUAL_FLASH
    uint_8 array;
#endif //PSP_HAS_DUAL_FLASH

    VMCF52XX_CFM_STRUCT_PTR     cfm_ptr;

    dev_spec_ptr = _mem_alloc(sizeof(MCF52XX_FLASH_INTERNAL_STRUCT));
#if MQX_CHECK_FOR_MEMORY_ALLOCATION_ERRORS
    /* test for correct memory allocation */
    if( dev_spec_ptr == NULL){
        return FALSE;
    }
#endif //MQX_CHECK_FOR_MEMORY_ALLOCATION_ERRORS
        
    /* get the pointer to cfm registers structure */
    cfm_ptr = _bsp_get_cfm_address();
    dev_spec_ptr->cfm_ptr = (vchar_ptr)cfm_ptr;
    dev_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;
    
    /* save pointer to function in ram */
    dev_spec_ptr->flash_execute_code_ptr = mcf52xx_init_ram_function();
    
    /* set correct clocking for CFM module */
    _mcf52xx_set_cfmclkd();

    return TRUE;    
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf52xx_flash_deinit
* Returned Value   : 
* Comments         :
*     
*
*END*----------------------------------------------------------------------*/
void mcf52xx_flash_deinit
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    MCF52XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF52XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    
    /* de-allocate the ram function buffer */
    if(dev_spec_ptr->flash_execute_code_ptr){
        _mem_free(dev_spec_ptr->flash_execute_code_ptr);
    }
    dev_spec_ptr->flash_execute_code_ptr = NULL;
    /* de-allocate device specific structure */
    _mem_free(dev_ptr->DEVICE_SPECIFIC_DATA);
    dev_ptr->DEVICE_SPECIFIC_DATA = NULL; 
}   

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf52xx_init_ram_function
* Returned Value   : pointer to allocated RAM function
* Comments         :
*   allocate and copy flash RAM function  
*
*END*----------------------------------------------------------------------*/
static char_ptr mcf52xx_init_ram_function
(
    void
)
{
    char_ptr ram_code_ptr;
    
    /* Allocate space on stack to run flash command out of SRAM */
    ram_code_ptr = _mem_alloc((char_ptr)mcf52xx_ram_function_end  - (char_ptr)mcf52xx_ram_function);
    /* copy code to RAM buffer */
    _mem_copy ((char_ptr)mcf52xx_ram_function, ram_code_ptr, (char_ptr)mcf52xx_ram_function_end  - (char_ptr)mcf52xx_ram_function);

    return ram_code_ptr;               
}
    
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf52xx_erase_sector
* Returned Value   : true if success, or false
* Comments         :
*   Erase a flash memory block  
*
*END*----------------------------------------------------------------------*/
boolean mcf52xx_flash_erase_sector
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr, 

    /* [IN] Erased sector address */
    char_ptr             from_ptr,

    /* [IN] Erased sector size */
    _mem_size            size
)
{
    MCF52XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF52XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    VMCF52XX_CFM_STRUCT_PTR cfm_ptr;
    int (*RunInRAM)(char_ptr);
    uint_32 temp;

    /* select the proper ramcode function */
    RunInRAM = (int(*)(char_ptr))dev_spec_ptr->flash_execute_code_ptr;
    
    /* get the pointer to cfm registers structure */
    cfm_ptr = (VMCF52XX_CFM_STRUCT_PTR)dev_spec_ptr->cfm_ptr;
    
    _int_disable();
    /* prepare flash write operation, clear flags and wait for ready state */
    cfm_ptr->CFMUSTAT = (MCF52XX_CFM_CFMUSTAT_PVIOL | MCF52XX_CFM_CFMUSTAT_ACCERR);
    while (!(cfm_ptr->CFMUSTAT & MCF52XX_CFM_CFMUSTAT_CBEIF)){
        /* wait */
    };

    /* latch address in Flash */
    (*(volatile vuint_32 *)(FLASH_START_ADDRESS + from_ptr)) = (vuint_32)-1;  
    
    /* issue erase command */
    cfm_ptr->CFMCMD = MCF52XX_CFM_CFMCMD_PAGE_ERASE;
    
    /* run command and wait for it to finish (must execute from RAM) */ 
    temp = _psp_get_sr();
    _psp_set_sr(temp | 0x0700);
    RunInRAM((char_ptr) &cfm_ptr->CFMUSTAT);
    _psp_set_sr(temp);  
    
    if (cfm_ptr->CFMUSTAT & (MCF52XX_CFM_CFMUSTAT_ACCERR | MCF52XX_CFM_CFMUSTAT_PVIOL)) {
        _int_enable();
        return FALSE;
    }
    
    _int_enable();
    return(TRUE);   

}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf52xx_write_sector
* Returned Value   : true if success, or false
* Comments         :
*   Performs a write into flash memory  
*
*END*----------------------------------------------------------------------*/
boolean mcf52xx_flash_write_sector
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
    MCF52XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF52XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    VMCF52XX_CFM_STRUCT_PTR  cfm_ptr;
    int (*RunInRAM)(char_ptr);
    uint_32  temp, temp_data = 0xFFFFFFFF;
    char_ptr temp_data_ptr;
    uchar byte_data_counter = 0;

    #define BYTES_IN_WORD   4
    #define WRITED_BYTES    4

    /* get the offset in write word */
    uint_32 offset = (uint_32)to_ptr & 0x00000003;;

    temp_data_ptr = (char_ptr)&temp_data;
    /* select the proper ramcode function */
    RunInRAM = (int (*)(char_ptr)) dev_spec_ptr->flash_execute_code_ptr;

    /* get the pointer to cfm registers structure */
    cfm_ptr = (VMCF52XX_CFM_STRUCT_PTR) dev_spec_ptr->cfm_ptr;
    /* Clear any errors */
    _int_disable();
    cfm_ptr->CFMUSTAT = (MCF52XX_CFM_CFMUSTAT_PVIOL | MCF52XX_CFM_CFMUSTAT_ACCERR); 
    
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
            *(uchar_ptr)(temp_data_ptr+byte_data_counter) = *from_ptr++;
            byte_data_counter++;
            size--;
        }
        /* move write data to register */
        (*(volatile vuint_32 *)(FLASH_START_ADDRESS + (uint_32)to_ptr)) = temp_data;
        /* init variables for next loop */
        to_ptr += WRITED_BYTES;
        byte_data_counter = 0;
        temp_data = 0xFFFFFFFF;
        /* write command to CFMCMD */
        cfm_ptr->CFMCMD = MCF52XX_CFM_CFMCMD_WORD_PROGRAM;
        
        /* run command and wait for it to finish (must execute from RAM) */     
        temp = _psp_get_sr();
        _psp_set_sr(temp | 0x0700);
        RunInRAM((char_ptr)&cfm_ptr->CFMUSTAT);                           
        _psp_set_sr(temp);
        
        /* Check for Errors */
        if (cfm_ptr->CFMUSTAT & (MCF52XX_CFM_CFMUSTAT_PVIOL | MCF52XX_CFM_CFMUSTAT_ACCERR)) {
            _int_enable();
            return (FALSE);
        }
    }
    _int_enable();
    return (TRUE);  
}


/********************************************************************/
/*
 *  Code required to run in SRAM to perform flash commands. 
 *  All else can be run in flash.
 *  In p_reg we need address of CFMUSTAT register.
 *
 ********************************************************************/ 
static void mcf52xx_ram_function
(
    /* [IN] Flash info structure */
    volatile char_ptr cfmustat_ptr
)
{
      /* start flash write */
      *cfmustat_ptr |= MCF52XX_CFM_CFMUSTAT_CBEIF;   
      /* wait until execution complete */
      while (!( *cfmustat_ptr & MCF52XX_CFM_CFMUSTAT_CCIF)){
      };
}
       
          
static void mcf52xx_ram_function_end(void)
{}
