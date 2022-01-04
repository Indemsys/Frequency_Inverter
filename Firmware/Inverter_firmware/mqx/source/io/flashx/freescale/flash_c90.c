/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: flash_c90.c$
* $Version : 3.8.4.0$
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
#include "flash_c90_prv.h"
#include "flash_c90.h"


/* Forward declarations */
static void c90_flash_exec(VC90_REG_STRUCT_PTR reg_ptr);
static void c90_flash_exec_end(void);


const FLASHX_DEVICE_IF_STRUCT _flashx_c90_if = {
    c90_flash_sector_erase,
    c90_flash_sector_program,
    c90_flash_chip_erase,
    NULL /* READ */, 
    c90_flash_init,
    c90_flash_deinit,
    c90_flash_write_protect,
    NULL /* IOCTL */
};


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_init
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Initialize flash specific information.
*
*END*----------------------------------------------------------------------*/
boolean c90_flash_init
(
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr
)
{
    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    VC90_REG_STRUCT_PTR reg_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR block_info_ptr;

    pointer ram_code_ptr;
    int     ram_code_size;
    uint_32 aspace;

    reg_ptr = _bsp_get_c90_address(flash_ptr->BASE_ADDR);
    if (reg_ptr == NULL)
    {
        return FALSE;
    }

    /* allocate internal structure */
    dev_spec_ptr = _mem_alloc_system(sizeof (C90_FLASH_INTERNAL_STRUCT));
    if( dev_spec_ptr == NULL)
    {
        return FALSE;
    }
    flash_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;

    dev_spec_ptr->reg_ptr = reg_ptr;

    /* allocate space to execute flashing from RAM, FIXME: this shall be in PSP */
    ram_code_size = (char_ptr)c90_flash_exec_end  - (char_ptr)c90_flash_exec;
    ram_code_ptr = _mem_alloc_system(ram_code_size);
    
    if (ram_code_ptr == NULL)
    {
        _mem_free(dev_spec_ptr);
        return FALSE; 
    }
    dev_spec_ptr->flash_exec = ram_code_ptr;

    /* copy code to RAM */
    _mem_copy ((char_ptr)c90_flash_exec, ram_code_ptr, ram_code_size);

    /* count how many sectors belong to each address space */
    dev_spec_ptr->aspace_sectors[C90_ASPACE_LAS] = 0;
    dev_spec_ptr->aspace_sectors[C90_ASPACE_MAS] = 0;
    dev_spec_ptr->aspace_sectors[C90_ASPACE_HAS] = 0;
    dev_spec_ptr->aspace_sectors[C90_ASPACE_SHADOW] = 0;

    block_info_ptr = flash_ptr->HW_BLOCK;
    
    while (block_info_ptr->NUM_SECTORS) {

        aspace = C90_ASPACE(block_info_ptr->SPECIAL_TAG);
        if (aspace >= 0 && aspace < C90_ASPACE_COUNT)
            dev_spec_ptr->aspace_sectors[aspace] += block_info_ptr->NUM_SECTORS;

        block_info_ptr++;
    }        

    return TRUE;    
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_deinit
* Returned Value   : none
* Comments         :
*   Release flash specific information.  
*
*END*----------------------------------------------------------------------*/
void c90_flash_deinit 
( 
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr 
)
{
    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;

    /* de-allocate the device specific structure */
    dev_spec_ptr = flash_ptr->DEVICE_SPECIFIC_DATA;
    _mem_free(dev_spec_ptr->flash_exec);
    _mem_free(dev_spec_ptr);

    flash_ptr->DEVICE_SPECIFIC_DATA = NULL;
}   


    
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_addr_to_sector
* Returned Value   : pointer to matching FLASHX_BLOCK_INFO_STRUCT  
* Comments         :
*   Finds address space and sector number for given address
*
*END*----------------------------------------------------------------------*/
static FLASHX_BLOCK_INFO_STRUCT_PTR c90_flash_addr_to_sector
(
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr, 
    
    /* [IN] Address */
    uint_32 addr,
    
    /* [OUT] Address space */
    int *aspace,

    /* [OUT] Sector in the address space */
    int *aspace_sec
)  
{
    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR block_info_ptr;

    dev_spec_ptr = flash_ptr->DEVICE_SPECIFIC_DATA;
    block_info_ptr = flash_ptr->HW_BLOCK;

    *aspace = 0;
    *aspace_sec = 0;    

    /* walk through the block info array, assuming it is:
       1) address space boundaries are between the blocks
       2) blocks belonging to different address spaces are not mixed */
    while (block_info_ptr->NUM_SECTORS) {

        if (*aspace != C90_ASPACE(block_info_ptr->SPECIAL_TAG))
        {
            *aspace = C90_ASPACE(block_info_ptr->SPECIAL_TAG);
            *aspace_sec=0;        
        } 
        
        if ((addr >= block_info_ptr->START_ADDR) && (addr < block_info_ptr->START_ADDR + block_info_ptr->NUM_SECTORS * block_info_ptr->SECTOR_SIZE))
            break; /* found matching sector */

        *aspace_sec += block_info_ptr->NUM_SECTORS;
        block_info_ptr++;
    }

    if (block_info_ptr->NUM_SECTORS == 0)
    {
        *aspace = 0;
        *aspace_sec = 0;
        return NULL; /*  no matching sector found */
    }

    *aspace_sec += (addr - block_info_ptr->START_ADDR) / block_info_ptr->SECTOR_SIZE;
    return block_info_ptr;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_protection_check
* Returned Value   : TRUE if write protected, FALSE otherwise
* Comments         :
*   Check for protection of given sector
*
*END*----------------------------------------------------------------------*/
static boolean c90_flash_protection_check
(
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr,
     
    /* [IN] Address space */
    int aspace,

    /* [IN] Sector in the address space */
    int aspace_sec
)
{
    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    VC90_REG_STRUCT_PTR reg_ptr;

    uint_32 aspace_mask;

    dev_spec_ptr = flash_ptr->DEVICE_SPECIFIC_DATA;
    reg_ptr = dev_spec_ptr->reg_ptr;
    
    aspace_mask = 1<<aspace_sec;

    switch (aspace) {
        case C90_ASPACE_LAS:
            if (reg_ptr->LML & C90_LML_LLOCK(aspace_mask)) return TRUE;        
            if (reg_ptr->SLL & C90_SLL_SLLOCK(aspace_mask)) return TRUE;        
            break;
        case C90_ASPACE_MAS:
            if (reg_ptr->LML & C90_LML_MLOCK(aspace_mask)) return TRUE;        
            if (reg_ptr->SLL & C90_SLL_SMLOCK(aspace_mask)) return TRUE;        
            break;
        case C90_ASPACE_HAS:
            if (reg_ptr->HBL & C90_HBL_HLOCK(aspace_mask)) return TRUE;
            break;
        case C90_ASPACE_SHADOW:
            if (reg_ptr->LML & C90_LML_SLOCK) return TRUE;
            if (reg_ptr->SLL & C90_SLL_SSLOC) return TRUE;
            break;
    }

    return FALSE;
}



/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_sector_erase
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Erase a flash memory block.
*
*END*----------------------------------------------------------------------*/
boolean c90_flash_sector_erase
(
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr, 

    /* [IN] Erased sector address */
    char_ptr             erase_ptr,

    /* [IN] Erased sector size */
    _mem_size            size
)
{
    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    VC90_REG_STRUCT_PTR reg_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR block_info_ptr;

    uint_32 erase_addr;

    int aspace; /* address space (LAS, MAS, HAS) */
    int aspace_sec; /* sector counter of current address space */
    uint_32 aspace_mask[4];

    /* -- end of declarations -- */

    dev_spec_ptr = flash_ptr->DEVICE_SPECIFIC_DATA;
    reg_ptr = dev_spec_ptr->reg_ptr;

    erase_addr = (uint_32)erase_ptr - (uint_32)(flash_ptr->BASE_ADDR);

    /* convert address to address space and sector number */
    block_info_ptr = c90_flash_addr_to_sector(flash_ptr, erase_addr, &aspace, &aspace_sec);  

    if (block_info_ptr == NULL)
        return FALSE; /* no matching sector found */
        
    if ((erase_addr - block_info_ptr->START_ADDR) % block_info_ptr->SECTOR_SIZE)
        return FALSE; /* misaligned address */

    if (size != block_info_ptr->SECTOR_SIZE)
        return FALSE; /* only single sector erase is supported */

    /* check for protection */
    if (c90_flash_protection_check(flash_ptr, aspace, aspace_sec))
        return FALSE;

    aspace_mask[0] = aspace_mask[1] = aspace_mask[2] = aspace_mask[3] = 0;
    aspace_mask[aspace] = 1<<aspace_sec;

    /* start erase sequence */
    reg_ptr->MCR |= C90_MCR_ERS;

    reg_ptr->LMS = C90_LMS_LSEL(aspace_mask[C90_ASPACE_LAS]) | C90_LMS_MSEL(aspace_mask[C90_ASPACE_MAS]);
    reg_ptr->HBS = C90_HBS_HSEL(aspace_mask[C90_ASPACE_HAS]);

    *((uint_32 *)erase_ptr) = 0; /* dummy write to flash required */
    dev_spec_ptr->flash_exec(reg_ptr);
    
    if ((reg_ptr->MCR & C90_MCR_PEG) == 0)
    {
        reg_ptr->MCR &= ~C90_MCR_ERS;
        return FALSE;
    }
    
    reg_ptr->MCR &= ~C90_MCR_ERS;
    
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_sector_program
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Performs a write into flash memory.
*
*END*----------------------------------------------------------------------*/
boolean c90_flash_sector_program
(
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr,

    /* [IN] Source address */
    char_ptr             from_ptr,

    /* [IN] Destination address */
    char_ptr             to_ptr,

    /* [IN] Number of bytes to write */
    _mem_size            size
) 
{
    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    VC90_REG_STRUCT_PTR reg_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR block_info_ptr;

    uint_32 *src;
    volatile uint_32 *dst;
    
    int aspace;
    int aspace_sec;

    int align;
    int width;

    uint_32 to_addr;
           

    to_addr = (uint_32)to_ptr - (uint_32)(flash_ptr->BASE_ADDR);

    /* convert address to address space and sector number */
    block_info_ptr = c90_flash_addr_to_sector(flash_ptr, to_addr, &aspace, &aspace_sec);
    
    if (block_info_ptr == NULL)
        return FALSE; /* no matching sector found */

    /* check alignment if applicable, ECC might not be computed correctly for missaligned data */
    align = C90_ALIGN(block_info_ptr->SPECIAL_TAG);
    
    if (align)
    {
        if (to_addr % align)
            return FALSE; /* misaligned address */
        
        if (size % align)
            return FALSE; /* misaligned size */
    }  

    if ((to_addr - block_info_ptr->START_ADDR) % block_info_ptr->SECTOR_SIZE + size > block_info_ptr->SECTOR_SIZE)
        return FALSE; /* write only within a single sector is supported */

    /* check for protection */
    if (c90_flash_protection_check(flash_ptr, aspace, aspace_sec))
        return FALSE;

    dev_spec_ptr = flash_ptr->DEVICE_SPECIFIC_DATA;
    reg_ptr = dev_spec_ptr->reg_ptr;

    src = (uint_32 *)from_ptr;    
    dst = (uint_32 *)to_ptr;    

    width = C90_WIDTH(block_info_ptr->SPECIAL_TAG);

    while (size >= 4)
    {
        /* start programming sequence */
        reg_ptr->MCR |= C90_MCR_PGM;

        /* if there are at least 128 bits to be written and the destination address is aligned */ 
        if ((width >= 128) && (size >= 16 && (((uint_32)dst % 16) == 0)))
        {
            *dst++ = *src++;
            *dst++ = *src++;
            *dst++ = *src++;
            *dst++ = *src++;
            size -= 16;    
        }         
        else if ((width >= 64) && (size >= 8 && (((uint_32)dst % 8) == 0)) ){
            /* write 64 bits */
            *dst++ = *src++;
            *dst++ = *src++;
            size -= 8;
        } else {
            /* write 32 bits */
            *dst++ = *src++;
            size -= 4;
        }            
   
        dev_spec_ptr->flash_exec(reg_ptr);
    
        if ((reg_ptr->MCR & C90_MCR_PEG) == 0)
        {
            reg_ptr->MCR &= ~C90_MCR_PGM;    
            return FALSE;
        }

        reg_ptr->MCR &= ~C90_MCR_PGM;
    }

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_chip_erase
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Erase the whole flash memory.
*
*END*----------------------------------------------------------------------*/
boolean c90_flash_chip_erase 
(
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr
)
{
    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    VC90_REG_STRUCT_PTR reg_ptr;

    uint_32 las_mask;
    uint_32 mas_mask;
    uint_32 has_mask;

    dev_spec_ptr = flash_ptr->DEVICE_SPECIFIC_DATA;
    reg_ptr = dev_spec_ptr->reg_ptr;
     
    las_mask = (1 << dev_spec_ptr->aspace_sectors[C90_ASPACE_LAS]) - 1;
    mas_mask = (1 << dev_spec_ptr->aspace_sectors[C90_ASPACE_MAS]) - 1;
    has_mask = (1 << dev_spec_ptr->aspace_sectors[C90_ASPACE_HAS]) - 1;
    
    /* check for protection */
    if (reg_ptr->LML & (C90_LML_LLOCK(las_mask) | C90_LML_MLOCK(mas_mask)))
        return FALSE;

    if (reg_ptr->SLL & (C90_SLL_SLLOCK(las_mask) | C90_SLL_SMLOCK(mas_mask)))
        return FALSE;

    if (reg_ptr->HBL & C90_HBL_HLOCK(has_mask))
        return FALSE;
        
    /* start erase sequence */
    reg_ptr->MCR |= C90_MCR_ERS;

    reg_ptr->LMS = C90_LMS_LSEL(las_mask) | C90_LMS_MSEL(mas_mask);
    reg_ptr->HBS = C90_HBS_HSEL(has_mask);

    *((uint_32 *)(flash_ptr->BASE_ADDR)) = 0; /* dummy write to flash required */
    dev_spec_ptr->flash_exec(reg_ptr);
    
    if ((reg_ptr->MCR & C90_MCR_PEG) == 0)
    {
        reg_ptr->MCR &= ~C90_MCR_ERS;
        return FALSE;
    }
    
    reg_ptr->MCR &= ~C90_MCR_ERS;
    
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : c90_flash_write_protect
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Sets/clears write protection of the FLASH
*
*END*----------------------------------------------------------------------*/
boolean c90_flash_write_protect
(
    /* [IN] Flash info structure */
    IO_FLASHX_STRUCT_PTR flash_ptr,

    /* [IN] Requested protection state (true = protected) */
    _mqx_uint state
)
{
    uint_32 las_mask;
    uint_32 mas_mask;
    uint_32 has_mask;

    uint_32 lml_mask;
    uint_32 sll_mask;
    uint_32 hbl_mask;
    
    uint_32 lml;
    uint_32 sll;
    uint_32 hbl;

    C90_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    VC90_REG_STRUCT_PTR reg_ptr;

    dev_spec_ptr = flash_ptr->DEVICE_SPECIFIC_DATA;
    reg_ptr = dev_spec_ptr->reg_ptr;

    las_mask = (1 << dev_spec_ptr->aspace_sectors[C90_ASPACE_LAS]) - 1;
    mas_mask = (1 << dev_spec_ptr->aspace_sectors[C90_ASPACE_MAS]) - 1;
    has_mask = (1 << dev_spec_ptr->aspace_sectors[C90_ASPACE_HAS]) - 1;

    lml_mask = C90_LML_SLOCK | C90_LML_MLOCK(mas_mask) | C90_LML_LLOCK(las_mask);
    sll_mask = C90_SLL_SSLOC | C90_SLL_SMLOCK(mas_mask) | C90_SLL_SLLOCK(las_mask);
    hbl_mask = C90_HBL_HLOCK(has_mask);

    if (state)
    {
        lml = lml_mask;
        sll = sll_mask;
        hbl = hbl_mask;
    }
    else
    {
        lml = 0;
        sll = 0;
        hbl = 0;
    }
    
    if ((reg_ptr->LML & lml_mask) != lml)
    {
        if ((reg_ptr->LML & C90_LML_LME) == 0)
            reg_ptr->LML = C90_LML_PASSWORD;
        reg_ptr->LML = lml;
    }

    if ((reg_ptr->SLL & sll_mask) != sll)
    {
        if ((reg_ptr->SLL & C90_SLL_SLE) == 0)
            reg_ptr->SLL = C90_SLL_PASSWORD;
        reg_ptr->SLL = sll;
    }

    if ((reg_ptr->HBL & hbl_mask) != hbl)
    {
        if ((reg_ptr->HBL & C90_HBL_HBE) == 0)
            reg_ptr->HBL = C90_HBL_PASSWORD;
        reg_ptr->HBL = hbl;
    }

    return TRUE;
}


/********************************************************************
*
*  Code required to run in SRAM to perform flash commands. 
*  All else can be run in flash.
*  Parameter is an address of flash status register.
*
********************************************************************/ 
static void c90_flash_exec
( 
    VC90_REG_STRUCT_PTR reg_ptr
) 
{
  
    reg_ptr->MCR |= C90_MCR_EHV;
    while ((reg_ptr->MCR & C90_MCR_DONE) == 0)
    	;
    reg_ptr->MCR &= ~C90_MCR_EHV;
}
         
/* Dummy function to obtain ram_function length. This assumes that linker places objects in the same order as they appear in the source */
static void c90_flash_exec_end
(
    void
)
{
}
