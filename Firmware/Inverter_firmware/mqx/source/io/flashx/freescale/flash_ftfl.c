/**HEADER*********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
******************************************************************************
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
******************************************************************************
*
* $FileName: flash_ftfl.c$
* $Version : 3.8.25.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions for internal flash read, write, erase
*
*END*************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_ftfl_prv.h"
#include "flash_ftfl.h"

/* local function prototypes */
static void     ftfl_ram_function(volatile uint_8 *);
static void     ftfl_ram_function_end(void);
static char_ptr ftfl_init_ram_function(void);
static _mqx_int ftfl_deinit_ram_function(char_ptr);
static uint_32  ftfl_flash_command_sequence(FTFL_FLASH_INTERNAL_STRUCT_PTR, uint_8_ptr, uint_8, boolean);

static boolean  flexnvm_read_resource(IO_FLASHX_STRUCT_PTR, FLEXNVM_READ_RSRC_STRUCT_PTR);
static boolean  flexnvm_prog_part(IO_FLASHX_STRUCT_PTR, FLEXNVM_PROG_PART_STRUCT_PTR);
static boolean  flexnvm_set_flexram(IO_FLASHX_STRUCT_PTR, uchar);


const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisN_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 2,(_mem_size) BSP_INTERNAL_FLASH_BASE                              , BSP_INTERNAL_FLASH_SECTOR_SIZE, 0 },
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 2,(_mem_size) BSP_INTERNAL_FLASH_BASE + BSP_INTERNAL_FLASH_SIZE / 2, BSP_INTERNAL_FLASH_SECTOR_SIZE, 0 },
    { 0, 0, 0 }
};

#ifdef BSP_INTERNAL_FLEXRAM_BASE
const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisX_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE,(_mem_size) BSP_INTERNAL_FLASH_BASE, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { 1, (_mem_size) BSP_INTERNAL_FLEXRAM_BASE, BSP_INTERNAL_FLEXRAM_SIZE },
    { 0, 0, 0 }
};

const FLASHX_BLOCK_INFO_STRUCT _flashx_mcf51xx_plus_block_map[] =  {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE,(_mem_size) BSP_INTERNAL_FLASH_BASE, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { 1, (_mem_size) BSP_INTERNAL_FLEXRAM_BASE, BSP_INTERNAL_FLEXRAM_SIZE },
    { 0, 0, 0 }
};
#endif

const FLASHX_DEVICE_IF_STRUCT _flashx_ftfl_if = {
    ftfl_flash_erase_sector,
    ftfl_flash_write_sector,
    NULL,
    NULL,
    ftfl_flash_init,
    ftfl_flash_deinit,
    NULL,
    flexnvm_flash_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_command_sequence
* Returned Value   : uint_32 an error code
* Comments         :
*    Run command in FTFL device.
*
*END*-----------------------------------------------------------------------*/
static uint_32 ftfl_flash_command_sequence
(
    /* [IN] Flash specific structure */
    FTFL_FLASH_INTERNAL_STRUCT_PTR     dev_spec_ptr,

    /* [IN] Command byte array */
    uint_8_ptr                         command_array,

    /* [IN] Number of values in the array */
    uint_8                             count,

    /* [IN] Read the result back? */
    boolean                            read

)
{
    uint_8  fstat;
    uint_32 result, temp;
    void (* RunInRAM)(volatile uint_8 *);
    FTFL_MemMapPtr ftfl_ptr;
    uint_8 cnt;

    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

    /* get pointer to RunInRAM function */
    RunInRAM = (void(*)(volatile uint_8 *))(dev_spec_ptr->flash_execute_code_ptr);

    /* set the default return as FTFL_OK */
    result = FTFL_OK;

    /* check CCIF bit of the flash status register */
    while (0 == (ftfl_ptr->FSTAT & FTFL_FSTAT_CCIF_MASK))
    { /* void */ }

    /* clear RDCOLERR & ACCERR & FPVIOL error flags in flash status register */
    if (ftfl_ptr->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
    {
        ftfl_ptr->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;
    }
    if (ftfl_ptr->FSTAT & FTFL_FSTAT_ACCERR_MASK)
    {
        ftfl_ptr->FSTAT |= FTFL_FSTAT_ACCERR_MASK;
    }
    if (ftfl_ptr->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
    {
        ftfl_ptr->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;
    }

    cnt = count;
    switch (cnt)
    {

        case 12: ftfl_ptr->FCCOBB = command_array[--cnt];
        case 11: ftfl_ptr->FCCOBA = command_array[--cnt];
        case 10: ftfl_ptr->FCCOB9 = command_array[--cnt];
        case  9: ftfl_ptr->FCCOB8 = command_array[--cnt];
        case  8: ftfl_ptr->FCCOB7 = command_array[--cnt];
        case  7: ftfl_ptr->FCCOB6 = command_array[--cnt];
        case  6: ftfl_ptr->FCCOB5 = command_array[--cnt];
        case  5: ftfl_ptr->FCCOB4 = command_array[--cnt];
        case  4: ftfl_ptr->FCCOB3 = command_array[--cnt];
        case  3: ftfl_ptr->FCCOB2 = command_array[--cnt];
        case  2: ftfl_ptr->FCCOB1 = command_array[--cnt];
        case  1: ftfl_ptr->FCCOB0 = command_array[--cnt];
        default: break;
    }

#if PSP_MQX_CPU_IS_COLDFIRE
    temp = _psp_get_sr();
    _psp_set_sr(temp | 0x0700);
#elif PSP_MQX_CPU_IS_KINETIS
    __disable_interrupt ();
#endif /* PSP_MQX_CPU_IS_KINETIS */

    /* run command and wait for it to finish (must execute from RAM) */
    RunInRAM(&ftfl_ptr->FSTAT);

#if PSP_MQX_CPU_IS_COLDFIRE
    _psp_set_sr(temp);
#elif PSP_MQX_CPU_IS_KINETIS
    __enable_interrupt ();
#endif /* PSP_MQX_CPU_IS_KINETIS */

    cnt = count;
    if (read) {
        switch (cnt)
        {
            case 12: command_array[--cnt] = ftfl_ptr->FCCOBB;
            case 11: command_array[--cnt] = ftfl_ptr->FCCOBA;
            case 10: command_array[--cnt] = ftfl_ptr->FCCOB9;
            case  9: command_array[--cnt] = ftfl_ptr->FCCOB8;
            case  8: command_array[--cnt] = ftfl_ptr->FCCOB7;
            case  7: command_array[--cnt] = ftfl_ptr->FCCOB6;
            case  6: command_array[--cnt] = ftfl_ptr->FCCOB5;
            case  5: command_array[--cnt] = ftfl_ptr->FCCOB4;
            case  4: command_array[--cnt] = ftfl_ptr->FCCOB3;
            case  3: command_array[--cnt] = ftfl_ptr->FCCOB2;
            case  2: command_array[--cnt] = ftfl_ptr->FCCOB1;
            case  1: command_array[--cnt] = ftfl_ptr->FCCOB0;
            default: break;
        }
    }
    /* get flash status register value */
    fstat = ftfl_ptr->FSTAT;

#if PSP_MQX_CPU_IS_KINETIS
    /* invalidate flash cache to expose flash changes */
    kinetis_flash_invalidate_cache(FLASHX_INVALIDATE_CACHE_ALL);
#endif

    /* checking access error */
    if (0 != (fstat & FTFL_FSTAT_ACCERR_MASK))
    {
        /* return an error code FTFL_ERR_ACCERR */
        result = FTFL_ERR_ACCERR;
    }
    /* checking protection error */
    else if (0 != (fstat & FTFL_FSTAT_FPVIOL_MASK))
    {
        /* return an error code FTFL_ERR_PVIOL */
        result = FTFL_ERR_PVIOL;
    }
    /* checking MGSTAT0 non-correctable error */
    else if (0 != (fstat & FTFL_FSTAT_MGSTAT0_MASK))
    {
        /* return an error code FTFL_ERR_MGSTAT0 */
        result = FTFL_ERR_MGSTAT0;
    }

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_init
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Initialize flash specific information.
*
*END*-----------------------------------------------------------------------*/
boolean ftfl_flash_init
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR    dev_spec_ptr;
    FTFL_MemMapPtr                    ftfl_ptr;

    if (_bsp_ftfx_io_init(0)) {
        /* Cannot initialize FTF module */
        return FALSE;
    }

    /* allocate internal structure */
    dev_spec_ptr = _mem_alloc(sizeof(FTFL_FLASH_INTERNAL_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (dev_spec_ptr == NULL)
    {
        return FALSE;
    }
#endif
    dev_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;

    /* get the pointer to ftfl registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)_bsp_get_ftfl_address();
    dev_spec_ptr->ftfl_ptr = (vchar_ptr)ftfl_ptr;

    /* save pointer to function in ram */
    dev_spec_ptr->flash_execute_code_ptr = ftfl_init_ram_function();

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
* Function Name    : ftfl_flash_deinit
* Returned Value   : none
* Comments         :
*   Release flash specific information.
*
*END*-----------------------------------------------------------------------*/
void ftfl_flash_deinit
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;

    /* de-allocate the ram function buffer */
    ftfl_deinit_ram_function(dev_spec_ptr->flash_execute_code_ptr);

    /* de-allocate the device specific structure */
    _mem_free(dev_spec_ptr);

    dev_ptr->DEVICE_SPECIFIC_DATA = NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_init_ram_function
* Returned Value   : pointer to allocated RAM function
* Comments         :
*   Allocate and copy flash RAM function.
*
*END*-----------------------------------------------------------------------*/
char_ptr ftfl_init_ram_function
(
    void
)
{
    char_ptr ram_code_ptr;
    _mem_size ftfl_ram_function_start;

    /* remove thumb2 flag from the address and align to word size */
    ftfl_ram_function_start = (_mem_size)ftfl_ram_function & ~3;

    /* allocate space to run flash command out of RAM */
    ram_code_ptr = _mem_alloc_align((char_ptr)ftfl_ram_function_end - (char_ptr)ftfl_ram_function_start, 4);

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (ram_code_ptr == NULL)
    {
        return NULL;
    }
#endif

    /* aopy code to RAM buffer */
    _mem_copy ((char_ptr)ftfl_ram_function_start, ram_code_ptr, (char_ptr)ftfl_ram_function_end - (char_ptr)ftfl_ram_function_start);

    /* adjust address with respect to the original alignment */
    ram_code_ptr = (char_ptr)((_mem_size)ram_code_ptr | ((_mem_size)ftfl_ram_function & 3));

    return ram_code_ptr;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_deinit_ram_function
* Returned Value   : MQX_OK or an error code 
* Comments         :
*   Free up flash RAM function, if any.
*
*END*-----------------------------------------------------------------------*/
_mqx_int ftfl_deinit_ram_function
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
* Function Name    : ftfl_flash_erase_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Erase a flash memory block.
*
*END*-----------------------------------------------------------------------*/
boolean ftfl_flash_erase_sector
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] Erased sector address */
    char_ptr             from_ptr,

    /* [IN] Erased sector size */
    _mem_size            size
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    FTFL_MemMapPtr                    ftfl_ptr;
    _mqx_uint                         i;
    _mem_size                         write_addr;
    uint_8                            command_array[4];

    /* get the pointer to ftfl registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

    for (i = 0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++)
    {
        if (((uint_32)from_ptr >= dev_ptr->HW_BLOCK[i].START_ADDR) &&
            ((uint_32)from_ptr <= dev_ptr->HW_BLOCK[i].START_ADDR + dev_ptr->HW_BLOCK[i].SECTOR_SIZE * dev_ptr->HW_BLOCK[i].NUM_SECTORS))
        {
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
    command_array[0] = FTFL_ERASE_SECTOR;
    command_array[1] = (uint_8)(write_addr >> 16);
    command_array[2] = (uint_8)((write_addr >> 8) & 0xFF);
    command_array[3] = (uint_8)(write_addr & 0xFF);

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence (dev_spec_ptr, command_array, 4, FALSE))
    {
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_write_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Performs a write into flash memory.
*
*END*-----------------------------------------------------------------------*/
boolean ftfl_flash_write_sector
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] Source address */
    char_ptr             from_ptr,

    /* [IN] Destination address */
    char_ptr             to_ptr,

    /* [IN] Number of bytes to write */
    _mem_size            size
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    FTFL_MemMapPtr                    ftfl_ptr;
    _mqx_uint                         i;
    _mem_size                         write_addr;
    uint_8                            command_array[4 + FTFL_LONGWORD_SIZE];
    char                              temp_data[FTFL_LONGWORD_SIZE];
    uchar                             byte_data_counter = 0;
    uint_32                           offset = ((uint_32)to_ptr) & 0x00000003;

    dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;

    /* get the pointer to ftfl registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

    /* write to address mod 4 correction */
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
        while ((byte_data_counter < FTFL_LONGWORD_SIZE) && size)
        {
            temp_data[byte_data_counter++] = *from_ptr++;
            size--;
        }

        /* remaining space should be 0xFF */
        while (byte_data_counter < FTFL_LONGWORD_SIZE) {
            temp_data[byte_data_counter++] = 0xFF;
        }

        /* prepare parameters to program the flash block */
        command_array[0] = FTFL_PROGRAM_LONGWORD;
        command_array[1] = (uint_8)(write_addr >> 16);
        command_array[2] = (uint_8)((write_addr >> 8) & 0xFF);
        command_array[3] = (uint_8)(write_addr & 0xFF);

#if PSP_ENDIAN == MQX_BIG_ENDIAN
        command_array[4] = temp_data[0];
        command_array[5] = temp_data[1];
        command_array[6] = temp_data[2];
        command_array[7] = temp_data[3];
#else
        command_array[4] = temp_data[3];
        command_array[5] = temp_data[2];
        command_array[6] = temp_data[1];
        command_array[7] = temp_data[0];
#endif /* PSP_ENDIAN */

        /* call flash command sequence function to execute the command */
        if (FTFL_OK != ftfl_flash_command_sequence (dev_spec_ptr, command_array, 4 + FTFL_LONGWORD_SIZE, FALSE))
        {
            return FALSE;
        }

        /* update destination address for next iteration */
        write_addr += FTFL_LONGWORD_SIZE;
        /* init variables for next loop */
        byte_data_counter = 0;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_ram_function
* Returned Value   : void
* Comments         :
*   Code required to run in SRAM to perform flash commands.
*   All else can be run in flash.
*   Parameter is an address of flash status register.
*
*END*-----------------------------------------------------------------------*/
static void ftfl_ram_function
(
    /* [IN] Flash info structure */
    volatile uint_8 *ftfl_fstat_ptr
)
{
    /* start flash write */
    *ftfl_fstat_ptr |= FTFL_FSTAT_CCIF_MASK;

    /* wait until execution complete */
    while (0 == ((*ftfl_fstat_ptr) & FTFL_FSTAT_CCIF_MASK))
    { /* void */ }
}

static void ftfl_ram_function_end(void)
{}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_read_resource
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*-----------------------------------------------------------------------*/
static boolean flexnvm_read_resource
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR         dev_ptr,

    /* [IN] Pointer to resource to be read */
    FLEXNVM_READ_RSRC_STRUCT_PTR param_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    FTFL_MemMapPtr                 ftfl_ptr;
    uint_8 command_array[9];

    /* get the pointer to ftfl registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

    /* preparing passing parameter to erase a flash block */
    command_array[0] = FTFL_READ_RESOURCE;
    command_array[1] = (uint_8)((param_ptr->ADDR >> 16) & 0xFF);
    command_array[2] = (uint_8)((param_ptr->ADDR >> 8) & 0xFF);
    command_array[3] = (uint_8)(param_ptr->ADDR & 0xFF);

    command_array[8] = (uint_8)(param_ptr->RSRC_CODE);

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence(dev_spec_ptr, command_array, 9, TRUE)) {
        return FALSE;
    }

    param_ptr->RD_DATA = *(uint_32*)(&command_array[4]);

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_prog_part
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*-----------------------------------------------------------------------*/
static boolean flexnvm_prog_part
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR         dev_ptr,

    /* [IN] Pointer to the structure containing partitioning info */
    FLEXNVM_PROG_PART_STRUCT_PTR param_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    FTFL_MemMapPtr                 ftfl_ptr;
    uint_8 command_array[6];

    /* get the pointer to ftfl registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

    /* preparing passing parameter to erase a flash block */
    command_array[0] = FTFL_PROGRAM_PARTITION;
    command_array[1] = 0;
    command_array[2] = 0;
    command_array[3] = 0;
    command_array[4] = param_ptr->EE_DATA_SIZE_CODE;
    command_array[5] = param_ptr->FLEXNVM_PART_CODE;

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence(dev_spec_ptr, command_array, 6, FALSE)) {
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_set_flexram
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*----------------------------------------------------------------------*/
static boolean flexnvm_set_flexram
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] Command code */
    uchar ctrl_code
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    FTFL_MemMapPtr ftfl_ptr;
    uint_8 command_array[2];

    /* get the pointer to ftfl registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

    /* prepare to pass the parameter to set FlexRAM */
    command_array[0] = FTFL_SET_EERAM;
    command_array[1] = ctrl_code;

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence (dev_spec_ptr, command_array, 2, FALSE)) {
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_flash_ioctl
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*----------------------------------------------------------------------*/
_mqx_int flexnvm_flash_ioctl
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] the ioctl command */
    _mqx_uint cmd,

    /* [IN] the ioctl command parameter */
    pointer param_ptr
)
{
    _mqx_int result = IO_OK;

    switch (cmd) {
        case FLEXNVM_IOCTL_READ_RESOURCE:
            flexnvm_read_resource(dev_ptr, (FLEXNVM_READ_RSRC_STRUCT_PTR)param_ptr);
            break;
        case FLEXNVM_IOCTL_GET_PARTITION_CODE: {
                FLEXNVM_READ_RSRC_STRUCT rsrc_param;

                rsrc_param.ADDR = 0x8000fc;
                rsrc_param.RSRC_CODE = 0;

                if (flexnvm_read_resource(dev_ptr, &rsrc_param) && NULL != param_ptr) {
                    ((FLEXNVM_PROG_PART_STRUCT_PTR)param_ptr)->EE_DATA_SIZE_CODE = (rsrc_param.RD_DATA >> 8) & (FLEXNVM_EE_SPLIT_MASK | FLEXNVM_EE_SIZE_MASK);
                    ((FLEXNVM_PROG_PART_STRUCT_PTR)param_ptr)->FLEXNVM_PART_CODE = rsrc_param.RD_DATA & FLEXNVM_PART_CODE_MASK;
                }
                else {
                    result = IO_ERROR_READ;
                }

                break;
            }
        case FLEXNVM_IOCTL_SET_PARTITION_CODE:
            flexnvm_prog_part(dev_ptr, (FLEXNVM_PROG_PART_STRUCT_PTR)param_ptr);
            break;
        case FLEXNVM_IOCTL_SET_FLEXRAM_FN:
            flexnvm_set_flexram(dev_ptr, *(uchar*)param_ptr);
            break;
        case FLEXNVM_IOCTL_WAIT_EERDY: {
                FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
                FTFL_MemMapPtr ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

                while (!(ftfl_ptr->FCNFG & FTFL_FCNFG_EEERDY_MASK)){}

                break;
            }
        case FLEXNVM_IOCTL_GET_EERDY: {
                FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
                FTFL_MemMapPtr ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

                if (NULL != param_ptr)
                    *(uint_32*)param_ptr = (ftfl_ptr->FCNFG & FTFL_FCNFG_EEERDY_MASK);

                break;
            }
        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }

    return result;
}
