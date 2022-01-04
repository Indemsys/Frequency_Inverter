/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: flash_ftfe.c$
* $Version : 3.8.7.0$
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
#include "flash_ftfe_prv.h"
#include "flash_ftfe.h"

static void ftfe_ram_function(volatile uint_8 *); 
static void ftfe_ram_function_end(void);
static uint_32 ftfe_flash_command_sequence(FTFE_FLASH_INTERNAL_STRUCT_PTR, uint_8_ptr, uint_8);
static char_ptr ftfe_init_ram_function(void);
static _mqx_int ftfe_deinit_ram_function(char_ptr);

/* Note: bank1-3 reading are made from another addresses than bank1 writing. Therefore we add next parameter as base for writing */
const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisN_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 4, (uint_32) BSP_INTERNAL_FLASH_BASE,              BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 4, (uint_32) BSP_INTERNAL_FLASH_BASE + 1 * BSP_INTERNAL_FLASH_SIZE / 4, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 4, (uint_32) BSP_INTERNAL_FLASH_BASE + 2 * BSP_INTERNAL_FLASH_SIZE / 4, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 4, (uint_32) BSP_INTERNAL_FLASH_BASE + 3 * BSP_INTERNAL_FLASH_SIZE / 4, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { 0, 0, 0 }
};

const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisX_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 2, (uint_32) BSP_INTERNAL_FLASH_BASE,              BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 2, (uint_32) BSP_INTERNAL_FLASH_BASE + 1 * BSP_INTERNAL_FLASH_SIZE / 2, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { 0, 0, 0 }
};

const FLASHX_DEVICE_IF_STRUCT _flashx_ftfe_if = {
    ftfe_flash_erase_sector,
    ftfe_flash_write_sector,
    NULL,
    NULL,
    ftfe_flash_init,
    ftfe_flash_deinit,
    NULL,
    NULL
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfe_flash_command_sequence
* Returned Value   : uint_32 an error code
* Comments         :
*    Run command in FTFE device.
*
*END*----------------------------------------------------------------------*/
static uint_32 ftfe_flash_command_sequence
(                                                
    /* [IN] Flash specific structure */
    FTFE_FLASH_INTERNAL_STRUCT_PTR     dev_spec_ptr,
   
    /* [IN] Command byte array */
    uint_8_ptr                         command_array,
      
    /* [IN] Number of values in the array */
    uint_8                             count
)
{
    uint_8  fstat;
    uint_32 result, temp;
    void (* RunInRAM)(volatile uint_8 *);
    FTFE_MemMapPtr ftfe_ptr;
    
    ftfe_ptr = (FTFE_MemMapPtr)dev_spec_ptr->ftfe_ptr;

    /* get pointer to RunInRAM function */
    RunInRAM = (void(*)(volatile uint_8 *))(dev_spec_ptr->flash_execute_code_ptr);
    
    /* set the default return as FTFE_OK */
    result = FTFE_OK;
    
    /* check CCIF bit of the flash status register */
    while (0 == (ftfe_ptr->FSTAT & FTFE_FSTAT_CCIF_MASK))
        { };
    
    /* clear RDCOLERR & ACCERR & FPVIOL error flags in flash status register */
    if (ftfe_ptr->FSTAT & FTFE_FSTAT_RDCOLERR_MASK) 
    {
        ftfe_ptr->FSTAT |= FTFE_FSTAT_RDCOLERR_MASK;
    }
    if (ftfe_ptr->FSTAT & FTFE_FSTAT_ACCERR_MASK) 
    {
        ftfe_ptr->FSTAT |= FTFE_FSTAT_ACCERR_MASK;
    }
    if (ftfe_ptr->FSTAT & FTFE_FSTAT_FPVIOL_MASK) 
    {
        ftfe_ptr->FSTAT |= FTFE_FSTAT_FPVIOL_MASK;
    }
    
    switch (count)
    {
        case 12: ftfe_ptr->FCCOBB = command_array[--count];
        case 11: ftfe_ptr->FCCOBA = command_array[--count];
        case 10: ftfe_ptr->FCCOB9 = command_array[--count];
        case 9: ftfe_ptr->FCCOB8 = command_array[--count];
        case 8: ftfe_ptr->FCCOB7 = command_array[--count];
        case 7: ftfe_ptr->FCCOB6 = command_array[--count];
        case 6: ftfe_ptr->FCCOB5 = command_array[--count];
        case 5: ftfe_ptr->FCCOB4 = command_array[--count];
        case 4: ftfe_ptr->FCCOB3 = command_array[--count];
        case 3: ftfe_ptr->FCCOB2 = command_array[--count];
        case 2: ftfe_ptr->FCCOB1 = command_array[--count];
        case 1: ftfe_ptr->FCCOB0 = command_array[--count];
        default: break;
    }
    
#if PSP_MQX_CPU_IS_COLDFIRE
    temp = _psp_get_sr();
    _psp_set_sr(temp | 0x0700);
#elif PSP_MQX_CPU_IS_KINETIS
    __disable_interrupt ();
#endif //PSP_MQX_CPU_IS_KINETIS

    /* run command and wait for it to finish (must execute from RAM) */ 
    RunInRAM(&ftfe_ptr->FSTAT);

#if PSP_MQX_CPU_IS_COLDFIRE
    _psp_set_sr(temp);
#elif PSP_MQX_CPU_IS_KINETIS
    __enable_interrupt ();    
#endif //PSP_MQX_CPU_IS_KINETIS
    
    /* get flash status register value */
    fstat = ftfe_ptr->FSTAT;

#if PSP_MQX_CPU_IS_KINETIS
    /* invalidate flash cache to expose flash changes */
    kinetis_flash_invalidate_cache(FLASHX_INVALIDATE_CACHE_ALL);
#endif
   
    /* checking access error */
    if (0 != (fstat & FTFE_FSTAT_ACCERR_MASK))
    {
        /* return an error code FTFE_ERR_ACCERR */
        result = FTFE_ERR_ACCERR;
    }
    /* checking protection error */
    else if (0 != (fstat & FTFE_FSTAT_FPVIOL_MASK))
    {
        /* return an error code FTFE_ERR_PVIOL */
        result = FTFE_ERR_PVIOL;
    }
    /* checking MGSTAT0 non-correctable error */
    else if (0 != (fstat & FTFE_FSTAT_MGSTAT0_MASK))
    {
        /* return an error code FTFE_ERR_MGSTAT0 */
        result = FTFE_ERR_MGSTAT0;
    }
    
    return result;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : ftfe_flash_init
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Initialize flash specific information.
*
*END*----------------------------------------------------------------------*/
boolean ftfe_flash_init 
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    FTFE_FLASH_INTERNAL_STRUCT_PTR    dev_spec_ptr;
    FTFE_MemMapPtr                    ftfe_ptr;

    if (_bsp_ftfx_io_init(0)) {
        /* Cannot initialize FTF module */
        return FALSE;
    }

    /* allocate internal structure */
    dev_spec_ptr = _mem_alloc(sizeof(FTFE_FLASH_INTERNAL_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (dev_spec_ptr == NULL)
    {
        return FALSE;
    }
#endif
    dev_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;
        
    /* get the pointer to ftfe registers structure */
    ftfe_ptr = _bsp_get_ftfe_address();
    dev_spec_ptr->ftfe_ptr = (vchar_ptr)ftfe_ptr;
    
    /* save pointer to function in ram */
    dev_spec_ptr->flash_execute_code_ptr = ftfe_init_ram_function();

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (dev_spec_ptr->flash_execute_code_ptr == NULL)
    {
        return FALSE;
    }
#endif
        
    return TRUE;    
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : ftfe_flash_deinit
* Returned Value   : none
* Comments         :
*   Release flash specific information.  
*
*END*----------------------------------------------------------------------*/
void ftfe_flash_deinit 
( 
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    FTFE_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFE_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    
    /* de-allocate the ram function buffer */
    ftfe_deinit_ram_function(dev_spec_ptr->flash_execute_code_ptr);

    /* de-allocate the device specific structure */
    _mem_free(dev_spec_ptr);

    dev_ptr->DEVICE_SPECIFIC_DATA = NULL;
}   

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : ftfe_init_ram_function
* Returned Value   : pointer to allocated RAM function
* Comments         :
*   Allocate and copy flash RAM function.
*
*END*----------------------------------------------------------------------*/
char_ptr ftfe_init_ram_function
(
    void
)
{
    char_ptr ram_code_ptr;
    _mem_size ftfe_ram_function_start;

    /* remove thumb2 flag from the address and align to word size */
    ftfe_ram_function_start = (_mem_size)ftfe_ram_function & ~3;

    /* allocate space to run flash command out of RAM */
    ram_code_ptr = _mem_alloc_align((char_ptr)ftfe_ram_function_end - (char_ptr)ftfe_ram_function_start, 4);

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (ram_code_ptr == NULL)
    {
        return NULL;
    }
#endif

    /* copy code to RAM buffer */
    _mem_copy ((char_ptr)ftfe_ram_function_start, ram_code_ptr, (char_ptr)ftfe_ram_function_end - (char_ptr)ftfe_ram_function_start);
    
    /* adjust address with respect to the original alignment */
    ram_code_ptr = (char_ptr)((_mem_size)ram_code_ptr | ((_mem_size)ftfe_ram_function & 3));

    return ram_code_ptr;
}
    
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfe_deinit_ram_function
* Returned Value   : MQX_OK or an error code 
* Comments         :
*   Free up flash RAM function, if any.
*
*END*-----------------------------------------------------------------------*/
_mqx_int ftfe_deinit_ram_function
(
    char_ptr ram_code_ptr
)
{
    if (NULL != ram_code_ptr)
    {
        return _mem_free((pointer)((_mem_size)ram_code_ptr & ~3));
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : ftfe_flash_erase_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Erase a flash memory block.
*
*END*----------------------------------------------------------------------*/
boolean ftfe_flash_erase_sector
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] Erased sector address */
    char_ptr             from_ptr,

    /* [IN] Erased sector size */
    _mem_size            size
)
{
    FTFE_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFE_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    FTFE_MemMapPtr                    ftfe_ptr;
    _mqx_uint                         i;
    _mem_size                         write_addr;
    uint_8                            command_array[4];

    /* get the pointer to ftfe registers structure */
    ftfe_ptr = (FTFE_MemMapPtr)dev_spec_ptr->ftfe_ptr;

    for (i = 0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++)
    {
        if (((uint_32)from_ptr >= dev_ptr->HW_BLOCK[i].START_ADDR) &&
            ((uint_32)from_ptr <= dev_ptr->HW_BLOCK[i].START_ADDR + dev_ptr->HW_BLOCK[i].SECTOR_SIZE * dev_ptr->HW_BLOCK[i].NUM_SECTORS)) {
             /* check if the from_ptr is sector aligned or not */
             if ((uint_32)from_ptr % dev_ptr->HW_BLOCK[i].SECTOR_SIZE)
                return FALSE;
             /* check if the size is sector aligned or not */
             if ((uint_32)size % dev_ptr->HW_BLOCK[i].SECTOR_SIZE)
                return FALSE;
             break;
        }
    }
 
    write_addr = (_mem_size) from_ptr;

    /* preparing passing parameter to erase a flash block */
    command_array[0] = FTFE_ERASE_SECTOR;
    command_array[1] = (uint_8)(write_addr >> 16);
    command_array[2] = (uint_8)((write_addr >> 8) & 0xFF);
    command_array[3] = (uint_8)(write_addr & 0xFF);

    /* call flash command sequence function to execute the command */
    if (FTFE_OK != ftfe_flash_command_sequence (dev_spec_ptr, command_array, 4))
    {
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : ftfe_flash_write_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Performs a write into flash memory.
*
*END*----------------------------------------------------------------------*/
boolean ftfe_flash_write_sector
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
    FTFE_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFE_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    FTFE_MemMapPtr                    ftfe_ptr;
    _mqx_uint                         i;
    _mem_size                         write_addr = 0;
    uint_8                            command_array[4 + FTFE_PHRASE_SIZE];
    char                              temp_data[FTFE_PHRASE_SIZE];
    uchar                             byte_data_counter = 0;
    uint_32                           offset = ((uint_32)to_ptr) & 0x00000007;

    dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;

    /* get the pointer to ftfe registers structure */
    ftfe_ptr = (FTFE_MemMapPtr)dev_spec_ptr->ftfe_ptr;    

    /* write to address mod 8 correction */
    if (offset)
    {
        /* align pointer to writable address */
        to_ptr -= offset;
        
        /* jump over old data */
        byte_data_counter = offset; 
    }

    write_addr = (_mem_size) to_ptr;

    /* heading space should be 0xFF */
    for (i = 0; i < offset; i++)
        temp_data[i] = 0xFF;

    while (size)
    {
        /* move data to temporary char array */       
        while ((byte_data_counter < FTFE_PHRASE_SIZE) && size)
        {
            temp_data[byte_data_counter++] = *from_ptr++;
            size--;
        }
        
        /* remaining space should be 0xFF */
        while (byte_data_counter < FTFE_PHRASE_SIZE) {
            temp_data[byte_data_counter++] = 0xFF;
        }

        /* prepare parameters to program the flash block */
        command_array[0] = FTFE_PROGRAM_PHRASE;
        command_array[1] = (uint_8)(write_addr >> 16);
        command_array[2] = (uint_8)((write_addr >> 8) & 0xFF);
        command_array[3] = (uint_8)(write_addr & 0xFF);
        
        command_array[4] = temp_data[3];
        command_array[5] = temp_data[2];
        command_array[6] = temp_data[1];
        command_array[7] = temp_data[0];

        command_array[8] = temp_data[7];
        command_array[9] = temp_data[6];
        command_array[10] = temp_data[5];
        command_array[11] = temp_data[4];
                
        /* call flash command sequence function to execute the command */
        if (FTFE_OK != ftfe_flash_command_sequence (dev_spec_ptr, command_array, 4 + FTFE_PHRASE_SIZE))
        {
            return FALSE;
        }
               
        /* update destination address for next iteration */
        write_addr += FTFE_PHRASE_SIZE;
        /* init variables for next loop */
        byte_data_counter = 0;
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
static void ftfe_ram_function
( 
    /* [IN] Flash info structure */
    volatile uint_8 *ftfe_fstat_ptr
) 
{
    /* start flash write */
    *ftfe_fstat_ptr |= FTFE_FSTAT_CCIF_MASK;
    
    /* wait until execution complete */
    while (0 == ((*ftfe_fstat_ptr) & FTFE_FSTAT_CCIF_MASK))
        { };
}
         
static void ftfe_ram_function_end(void)
{}
