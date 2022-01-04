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
* $FileName: flashx.c$
* $Version : 3.8.32.0$
* $Date    : Aug-30-2012$
*
* Comments:
*
*   This file contains generic flash driver functions to deal with odd
*   size flash blocks.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "flashx.h"
#include "flashxprv.h"
#include <string.h>

#define MFS_SECTOR_SIZE 512

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a flash driver.
*
*END*----------------------------------------------------------------------*/
_mqx_uint _io_flashx_install
(
      /* [IN] A string that identifies the device for fopen */
      char_ptr                      identifier,

      /* [IN] BSP specific settings for installing device */
      FLASHX_INIT_STRUCT const _PTR_  init_ptr

)
{ /* Body */
   IO_FLASHX_STRUCT_PTR           dev_ptr;
  
   dev_ptr = (IO_FLASHX_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(IO_FLASHX_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
#endif

    dev_ptr->SECTOR_ERASE   = init_ptr->DEVICE_IF->SECTOR_ERASE;
    dev_ptr->SECTOR_PROGRAM = init_ptr->DEVICE_IF->SECTOR_PROGRAM;
    dev_ptr->CHIP_ERASE     = init_ptr->DEVICE_IF->CHIP_ERASE;
    dev_ptr->READ           = init_ptr->DEVICE_IF->READ;
    dev_ptr->INIT           = init_ptr->DEVICE_IF->INIT;
    dev_ptr->DEINIT         = init_ptr->DEVICE_IF->DEINIT;
    dev_ptr->WRITE_PROTECT  = init_ptr->DEVICE_IF->WRITE_PROTECT;
    dev_ptr->IOCTL          = init_ptr->DEVICE_IF->IOCTL;

    dev_ptr->BASE_ADDR      = init_ptr->BASE_ADDR;
    dev_ptr->HW_BLOCK       = (FLASHX_BLOCK_INFO_STRUCT_PTR) init_ptr->HW_BLOCK;
    dev_ptr->FILE_BLOCK     = (FLASHX_FILE_BLOCK_PTR) init_ptr->FILE_BLOCK;
    dev_ptr->WIDTH          = init_ptr->WIDTH;
    dev_ptr->DEVICES        = init_ptr->DEVICES;
    dev_ptr->WRITE_VERIFY   = init_ptr->WRITE_VERIFY;

    dev_ptr->DEVICE_SPECIFIC_INIT = init_ptr->DEVICE_SPECIFIC_INIT;

   _lwsem_create(&dev_ptr->HW_ACCESS, 1);
    
    return (_io_dev_install_ext(
        identifier,
        _io_flashx_open,
        _io_flashx_close,
        _io_flashx_read,
        _io_flashx_write,
        _io_flashx_ioctl,
        _io_flashx_uninstall,
        (pointer)dev_ptr));

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_uninstall
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Uninstalls a flash driver.
*
*END*----------------------------------------------------------------------*/
_mqx_int _io_flashx_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

    if (!dev_ptr->FILES) {
        _mem_free((pointer)dev_ptr);
        io_dev_ptr->DRIVER_INIT_PTR = NULL;
        return IO_OK;
    } else {
        return IO_ERROR_DEVICE_BUSY;
    } /* Endif */  
   
}  /* Endbody */    


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_open
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         : Opens and initializes flash driver.
*    Must be synchronized by app software with concurrent calls also with
*    close and uninstall function.
* 
*END*----------------------------------------------------------------------*/
_mqx_int _io_flashx_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,
      
      /* [IN] the remaining portion of the name of the device */
      char_ptr   open_name_ptr,

      /* [IN] the flags to be used during operation: */
      char_ptr   flags
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR b;
    char_ptr             file_name_ptr = fd_ptr->DEV_PTR->IDENTIFIER; /* use driver name temporarily */
    boolean              chip_initialized = TRUE;
    _mqx_uint            erase_array_size;
    _mqx_uint            skip_sectors;
    _mqx_uint            skip_size;
    _mem_size            sfa, efa; /* start file address, end file address */
    _mem_size            bsa, bea; /* block start address, block end address */
    _mem_size            size, sec_size; /* file size, maximum sector size */
    _mqx_int             f; /* file number */

    while (*file_name_ptr++ != 0)
        open_name_ptr++; /* move to the file name */
    file_name_ptr = open_name_ptr; /* throw out value of file_name_ptr and assign the correct file name */

    if (!dev_ptr->FILES) {
        /* First time of opening the device. */
        if (dev_ptr->INIT) {
            chip_initialized = (*dev_ptr->INIT)(dev_ptr);
        } /* Endif */
    } /* Endif */

#if MQX_CHECK_ERRORS
    if (!chip_initialized) {
        return IO_ERROR;
    } /* Endif */
#endif //MQX_CHECK_ERRORS

    /* Check if the file name is in the file list */
    if (dev_ptr->FILE_BLOCK) {
        for (f = 0; dev_ptr->FILE_BLOCK[f].FILENAME; f++) {
            if (strcmp(dev_ptr->FILE_BLOCK[f].FILENAME, file_name_ptr) == 0) {
                /* Found a block with correct filename, prepare file driver memory */
                break;
            }
        }
    }

    if (!dev_ptr->FILE_BLOCK || !dev_ptr->FILE_BLOCK[f].FILENAME) {
        /* File name not found in the list */
        if (*file_name_ptr == 0) {
            /* No filename, use the whole HW space (4GiB) */
            sfa = 0;
            efa = MAX_UINT_32;
            f = -1;
        }
        else {
            /* Specified file name not found in the list of files */
            return IO_ERROR;
        }
    }
    else {
        sfa = dev_ptr->FILE_BLOCK[f].START_ADDR;
        efa = dev_ptr->FILE_BLOCK[f].END_ADDR;
    }

    /* Go through all blocks in HW map and find the first one that matches start of file */
    for (b = dev_ptr->HW_BLOCK; b->NUM_SECTORS != 0; b++) {
        bsa = dev_ptr->BASE_ADDR + b->START_ADDR;
        bea = bsa + b->NUM_SECTORS * b->SECTOR_SIZE - 1;
        if (sfa <= bea)
             break; /* start address of the file preceed the block or is inside the block */
    }

    if ((b->NUM_SECTORS == 0) || (bsa > efa)) {
        return IO_ERROR; /* no intersection */
    }

    if (sfa < bsa) {
        sfa = bsa;
    }
#if MQX_CHECK_ERRORS
    else if ((sfa - bsa) % b->SECTOR_SIZE) {
        return IO_ERROR; /* start address of file is not sector aligned */
    }
#endif //MQX_CHECK_ERRORS

    skip_size = sfa - (dev_ptr->BASE_ADDR + b->START_ADDR);
    skip_sectors = skip_size / b->SECTOR_SIZE;

    /* Get the size of the file */
    size = 0;
    erase_array_size = 0;
    sec_size = 0;
    for ( ; b->NUM_SECTORS != 0; b++) {
        _mqx_uint block_size = b->NUM_SECTORS * b->SECTOR_SIZE;
        bsa = dev_ptr->BASE_ADDR + b->START_ADDR;
        bea = bsa + block_size - 1;

        /* Test if we have not overrun after the file area */
        if (bsa > efa) break;
        
        size += block_size;
        erase_array_size += b->NUM_SECTORS;
        /* Compute max. sector size for this file */
        if (sec_size < b->SECTOR_SIZE) {
            sec_size = b->SECTOR_SIZE;
        }

        /* Check if this is the last block in the file */
        if (bea >= efa) {
#if MQX_CHECK_ERRORS
            if ((efa + 1 - bsa) % b->SECTOR_SIZE) {
                return IO_ERROR; /* end address of file is not sector aligned */
            }
#endif //MQX_CHECK_ERRORS
            /* Remove the trailing space */
            size -= bea - efa;
            erase_array_size -= (bea - efa) / b->SECTOR_SIZE;
            break;
        }
    }
    /* Remove the heading space */
    size -= skip_size;
    erase_array_size -= skip_sectors;

    file_ptr = _mem_alloc_system(sizeof(IO_FLASHX_FILE_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (file_ptr == NULL) {
        return IO_ERROR;
    }
#endif //MQX_CHECK_MEMORY_ALLOCATION_ERRORS

    file_ptr->ERASE_ARRAY = _mem_alloc_system((erase_array_size + 7) / 8); /* size in bytes, each bit represents one sector */
    file_ptr->ERASE_ARRAY_MAX = erase_array_size; /* number of valid bits in ERASE_ARRAY */

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (file_ptr->ERASE_ARRAY == NULL) {
        _mem_free(file_ptr);
        return IO_ERROR; /* end address of file is not sector aligned */
    }
#endif //MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    _mem_set_type(file_ptr->ERASE_ARRAY, MEM_TYPE_IO_FLASHX_SECTOR_MAP); 
    /* Assume that the opened file's area in flash was not erased */
    _mem_zero(file_ptr->ERASE_ARRAY, (erase_array_size + 7) / 8);

    /* Store file block */
    if (f != -1)
        file_ptr->FILE_BLOCK = &dev_ptr->FILE_BLOCK[f];
    else
        file_ptr->FILE_BLOCK = NULL; /* the whole flash is used */

    /* Store the flags */
    file_ptr->FLAGS = 0;
    /* Set the cache for this file as not yet allocated */
    file_ptr->CACHE_PTR = 0;
    /* Invalidate cache, mark the data not dirty */
    file_ptr->DIRTY_DATA = 0;
    /* Store max. sector size */
    file_ptr->MAX_SECT_SIZE = sec_size;

    /* File size */
    fd_ptr->SIZE = size;
    /* Store file context structure */
    fd_ptr->DEV_DATA_PTR = (IO_FLASHX_STRUCT_PTR) file_ptr;

    /* Increment usage counter */
    dev_ptr->FILES++;

    return MQX_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_close
* Returned Value   : MQX_OK
* Comments         : Closes flash driver
* 
*END*----------------------------------------------------------------------*/
_mqx_int _io_flashx_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
   
    if (file_ptr->FLAGS & FLASHX_FLASH_BUFFER_ENABLED) {
        _io_flashx_flush_buffer(fd_ptr);
    } /* Endif */

    if(file_ptr->CACHE_PTR) {
        _mem_free(file_ptr->CACHE_PTR);
    }

    _mem_free((pointer)file_ptr->ERASE_ARRAY);

    _mem_free(file_ptr);

    if (--dev_ptr->FILES == 0) {
        if (dev_ptr->DEINIT) {
            (*dev_ptr->DEINIT)(dev_ptr);
        } /* Endif */
    }

    return MQX_OK;  
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _allocate_sector_cache
* Returned Value   : True, False
* Comments         : Allocate the flash sector cache
* 
*END*----------------------------------------------------------------------*/
boolean _allocate_sector_cache(MQX_FILE_PTR fd_ptr) 
{
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;

    /* test for sector cache enable */
    if (!(file_ptr->FLAGS & FLASHX_SECTOR_CACHE_ENABLED))
        return FALSE;
    
    if (file_ptr->CACHE_PTR == NULL) {
        file_ptr->CACHE_PTR  = _mem_alloc_system(file_ptr->MAX_SECT_SIZE);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        /* test for memory allocation */
        if(file_ptr->CACHE_PTR == NULL)
            return FALSE;
#endif
        _mem_set_type(file_ptr->CACHE_PTR, MEM_TYPE_IO_FLASHX_SECTOR_CACHE);
    }
    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_read
* Returned Value   : number of characters read
* Comments         : Reads data from flash driver
*    NOTE: This function should normally call low-level driver read(),
*    but it is not implemented yet. That's why dev_ptr->BASE_ADDR is used
*    here, even if this information is for low level driver only.
*    The dev_ptr->BASE_ADDR is base HW address for external flashes and should
*    equal to zero for internal flashes.
* 
*END*----------------------------------------------------------------------*/
_mqx_int _io_flashx_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,
   
      /* [IN] where the characters are to be stored */
      char_ptr   data_ptr,
   
      /* [IN] the number of characters to input */
      _mqx_int   num
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    _mqx_uint            sb, ss, offset, fs, copy_num, remains;
     
    if (fd_ptr->LOCATION + num > fd_ptr->SIZE) {
        if (fd_ptr->LOCATION >= fd_ptr->SIZE) {
            fd_ptr->ERROR = IO_ERROR_READ_ACCESS;
            return 0;
        }
        num = fd_ptr->SIZE - fd_ptr->LOCATION;
    }

    _lwsem_wait(&dev_ptr->HW_ACCESS);
                             
    if (file_ptr->FLAGS & FLASHX_FLASH_BUFFER_ENABLED) {
        /* We do this in order to simplify algorithm for reading.
        ** In true write-back caching, we check if the requested data to be read have valid
        ** image in the cache and if they do, we will read them from the cache instead.
        */
        _io_flashx_flush_buffer(fd_ptr);
    } /* Endif */

    remains = num;

    /* Find the first block where to read from */
    _io_flashx_find_correct_sectors(fd_ptr, fd_ptr->LOCATION, &sb, &ss, &offset, &fs);

    while (remains)
    {
        copy_num = (dev_ptr->HW_BLOCK[sb].NUM_SECTORS - ss) * dev_ptr->HW_BLOCK[sb].SECTOR_SIZE - offset;
        if (copy_num > remains)
        copy_num = remains;
        _mem_copy((pointer)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[sb].START_ADDR + ss * dev_ptr->HW_BLOCK[sb].SECTOR_SIZE + offset), (pointer) data_ptr, copy_num);
        remains -= copy_num;
        fd_ptr->LOCATION += copy_num; /* move LOCATION after each partial read */
        ss = 0; /* ss is interesting only in first pass when reading from start block */
        offset = 0; /* offset is interesting only in first pass when reading from start block */
        sb++;
    }

    _lwsem_post(&dev_ptr->HW_ACCESS);

    if (fd_ptr->LOCATION >= fd_ptr->SIZE) {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
    }

    /* Return the number of read bytes, not the remaining size */
    return num - remains;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_erase_sector
* Returned Value   : boolean, TRUE upon success
* Comments         : Although it seems counter-intuitive, we actually set the 
*                    DIRTY DATA bit here, or leave it set if it was set 
*                    previously.  This is to reduce code size. This function
*                    is only called before a write occurs, so the sector will
*                    be dirty again after the write finishes.
* 
*END*----------------------------------------------------------------------*/

boolean _io_flashx_erase_sector
   (
      /* [IN] The device handle */
      MQX_FILE_PTR          fd_ptr,
   
      /* Sector number within file */
      _mqx_uint             file_sector,
   
      /* [IN] The block of sectors to write to */
      _mqx_uint             block_number,
   
      /* [IN] The sector number within the block */
      _mqx_uint             sector_number
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    boolean    result = TRUE;
    char_ptr   dest_ptr;
    _mqx_uint  array_offset;
    uint_8     erase_mask;
   
    array_offset = file_sector / 8;
    erase_mask = 0x1 << (file_sector % 8); 
   
    if (dev_ptr->SECTOR_ERASE) {
        /* Check if sector was not erased before */
        if (0 == (file_ptr->ERASE_ARRAY[array_offset] & erase_mask)) {
            dest_ptr = (char_ptr)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[block_number].START_ADDR + sector_number * dev_ptr->HW_BLOCK[block_number].SECTOR_SIZE);
            result = (*dev_ptr->SECTOR_ERASE)(dev_ptr, dest_ptr, dev_ptr->HW_BLOCK[block_number].SECTOR_SIZE);
        }
    }

    return result;

} /* Endbody */  


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_phys_to_sector
* Returned Value   : void
* Comments         : Finds block for given physical address and provides with
*                    sector number, sector physical address and offset in the sector.
* 
*END*----------------------------------------------------------------------*/

FLASHX_BLOCK_INFO_STRUCT_PTR _io_flashx_phys_to_sector
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR dev_ptr,

      /* [IN] Physical address */
      _mem_size            addr,

      /* [IN/OUT] Number of sector sector in the block */
      _mqx_uint _PTR_      sector_number_ptr,

      /* [IN/OUT] Physical address of the sector */
      _mem_size _PTR_      sector_addr_ptr,

      /* [IN/OUT] The offset within the sector */
      _mem_size _PTR_      sector_offset_ptr

   )
{
    FLASHX_BLOCK_INFO_STRUCT_PTR b;

    _mem_size block_start;

    _mqx_uint sector_number;
    _mem_size sector_addr;
    _mem_size sector_offset;

    /* Go through all blocks in HW map and find the first one that matches */
    for (b = dev_ptr->HW_BLOCK; b->NUM_SECTORS != 0; b++) {
        block_start = dev_ptr->BASE_ADDR + b->START_ADDR;
        if ((addr >= block_start) && (addr <= block_start + b->NUM_SECTORS * b->SECTOR_SIZE - 1))
             break; /* start address of the file is inside the block */
    }

    if (b->NUM_SECTORS == 0)
        return NULL;

    sector_number = (addr - block_start) / b->SECTOR_SIZE;
    sector_addr = block_start + sector_number * b->SECTOR_SIZE;
    sector_offset = addr - sector_addr;

    if (sector_number_ptr != NULL)
        *sector_number_ptr = sector_number;

    if (sector_addr_ptr != NULL)
        *sector_addr_ptr = sector_addr;

    if (sector_offset_ptr != NULL)
        *sector_offset_ptr = sector_offset;

    return b;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_find_correct_sectors
* Returned Value   : void
* Comments         : Sets the start and end block, start and end sectors 
*                    relative to these blocks, and sets the absolute start 
*                    sector.
* 
*END*----------------------------------------------------------------------*/

void _io_flashx_find_correct_sectors
   (
      /* [IN] The device handle */
      MQX_FILE_PTR         fd_ptr,

      /* [IN] The linear address within the file */
      _mqx_uint            location,

      /* [IN/OUT] The HW start block for the write */
      _mqx_uint _PTR_      start_block_ptr,

      /* [IN/OUT] The start sector in the start_block */
      _mqx_uint _PTR_      start_sector_ptr,

      /* [IN/OUT] The offset within the first sector */
      _mqx_uint _PTR_      offset_ptr,

      /* [IN/OUT] The relative sector number within the file */ 
      _mqx_uint _PTR_      file_sector_ptr
   )
{  /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    FLASHX_BLOCK_INFO_STRUCT_PTR b;
    _mem_size sfa; /* start file address */
    _mem_size bsa, bea; /* block start address, block end address */
    _mqx_uint dummy_sect;

    *file_sector_ptr = 0;
    *start_block_ptr = 0;

    if (file_ptr->FILE_BLOCK == NULL) {
        sfa = 0;
    }
    else {
        sfa = file_ptr->FILE_BLOCK->START_ADDR;
    }

    /* Go through all blocks in HW map and find the first one that matches */
    for (b = dev_ptr->HW_BLOCK; b->NUM_SECTORS != 0; b++) {
        bsa = dev_ptr->BASE_ADDR + b->START_ADDR;
        bea = bsa + b->NUM_SECTORS * b->SECTOR_SIZE - 1;
        if ((sfa >= bsa) && (sfa <= bea))
             break; /* start address of the file is inside the block */
        *start_block_ptr += 1;
    }

    if (b->NUM_SECTORS != 0) {
        /* Rebase location so that is counts from the beginning of this block */
        location += sfa - bsa;
        /* Store the number of "dummy' sectors caused by rebasing location.
        ** These sectors are taken into computation in the loop.
        ** They must be subtracted at the end.
        */
        dummy_sect = (sfa - bsa) / b->SECTOR_SIZE;

        /* We are at the beginning of the file, seek to the location */
        for ( ; b->NUM_SECTORS != 0; b++) {
            if (b->NUM_SECTORS * b->SECTOR_SIZE > location) {
                break;
            }
            location -= b->SECTOR_SIZE * b->NUM_SECTORS;
            *file_sector_ptr += b->NUM_SECTORS;
            *start_block_ptr += 1;
        }

        *start_sector_ptr = location / b->SECTOR_SIZE;
        *offset_ptr = location % b->SECTOR_SIZE;
        /* Add the rest of sectors to the file sectors */               
        *file_sector_ptr += location / b->SECTOR_SIZE;
        /* At the end do not forget to subtract the dummy sectors */
        *file_sector_ptr -= dummy_sect;
    }
    else {
        /* TODO: the file is located after possible physical memory address space, return error */
    }
} /* Endbody */    


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_flush_buffer
* Returned Value   : number of characters written
* Comments         : Flushes the flash buffer. This is used only privately 
*                    within the flashx.c file. 
*END*----------------------------------------------------------------------*/

_mqx_int _io_flashx_flush_buffer
   (
      /* [IN] The device handle */
      MQX_FILE_PTR   fd_ptr
   ) 
{ /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    _mqx_int   error = MQX_OK;

    /* Check if buffering is enabled */
    if (file_ptr->FLAGS & FLASHX_FLASH_BUFFER_ENABLED) {
        /* Flush sector buffer */
        error = _io_flashx_flush_sector_buffer(fd_ptr);
    }

    /* No other buffering provided in this driver. */

    return error;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_flush_sector_buffer
* Returned Value   : number of characters written
* Comments         : Flushes the flash buffer. This is used only privately 
*                    within the flashx.c file. 
*END*----------------------------------------------------------------------*/

_mqx_int _io_flashx_flush_sector_buffer
   (
      /* [IN] The device handle */
      MQX_FILE_PTR   fd_ptr
   ) 
{ /* Body */
    IO_FLASHX_STRUCT_PTR dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    _mqx_uint  sb, ss, fs;
    _mqx_int   dest_ptr, error = MQX_OK;

    /* If the sector cache is disabled, return error */
    if (!(file_ptr->FLAGS & FLASHX_SECTOR_CACHE_ENABLED))
        return IO_ERROR;

    if (file_ptr->DIRTY_DATA) {
        sb = file_ptr->CACHE_BLOCK;
        ss = file_ptr->CACHE_SECTOR;
        fs = file_ptr->CACHE_FILE_SECTOR;

        if (!_io_flashx_erase_sector(fd_ptr, fs, sb, ss)) 
        {
            return IO_ERROR;
        } /* Endif */

        dest_ptr = (_mqx_int)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[sb].START_ADDR + (dev_ptr->HW_BLOCK[sb].SECTOR_SIZE * ss));
        if ((*dev_ptr->SECTOR_PROGRAM)(dev_ptr, file_ptr->CACHE_PTR, (char_ptr)dest_ptr, dev_ptr->HW_BLOCK[sb].SECTOR_SIZE))
        {
            error = dev_ptr->HW_BLOCK[sb].SECTOR_SIZE;
            /* Sector written to the flash is not erased */
            file_ptr->ERASE_ARRAY[fs / 8] &= ~(0x1 << (fs % 8));
        } else {
            return IO_ERROR;
        } /* Endif */

        file_ptr->DIRTY_DATA = FALSE;
    } /* Endif */

    return error;
}


 
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_write
* Returned Value   : number of characters written
* Comments         : Writes data to the flash device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_flashx_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,
   
      /* [IN] where the characters are */
      char_ptr     data_ptr,
   
      /* [IN] the number of characters to output */
      _mqx_int     num
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR  dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    _mem_size             copy_num, remains;
    _mqx_uint             b, s, offset, fs;
    _mqx_uint             dest_ptr;
    _mqx_int              result;

    if (fd_ptr->LOCATION + num > fd_ptr->SIZE) {
        if (fd_ptr->LOCATION >= fd_ptr->SIZE) {
            fd_ptr->ERROR = IO_ERROR_WRITE_ACCESS;
            return 0;
        }
        num = fd_ptr->SIZE - fd_ptr->LOCATION;
    }

    _lwsem_wait(&dev_ptr->HW_ACCESS);

    remains = num;

    while (remains) {
        /* Get block, sector, offset and file sector */
        _io_flashx_find_correct_sectors(fd_ptr, fd_ptr->LOCATION, &b, &s, &offset, &fs);

        if (offset || (remains < dev_ptr->HW_BLOCK[b].SECTOR_SIZE)) {
            /* Write partial sector.
            ** Partial sector write handles data caching. For this reason we use
            ** partial write also for last sector (even if it is not partial).
            */
            copy_num = remains <= (dev_ptr->HW_BLOCK[b].SECTOR_SIZE - offset) ? remains : dev_ptr->HW_BLOCK[b].SECTOR_SIZE - offset;
            result = _io_flashx_write_partial_sector(fd_ptr, b, s, offset, copy_num, fs, data_ptr);
            if (result == IO_ERROR) {
                fd_ptr->ERROR = IO_ERROR_WRITE;
                _lwsem_post(&dev_ptr->HW_ACCESS);
                return result;
            }
        }
        else {
            /* Write whole sector. Whole sector does not handle sector caching. */
            copy_num = dev_ptr->HW_BLOCK[b].SECTOR_SIZE;
            
            result = _io_flashx_erase_sector(fd_ptr, fs, b, s);
            if (result != IO_ERROR) {
                dest_ptr = dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[b].START_ADDR + s * dev_ptr->HW_BLOCK[b].SECTOR_SIZE + offset;
                if (!(*dev_ptr->SECTOR_PROGRAM)(dev_ptr, data_ptr, (char_ptr) dest_ptr, copy_num))
                    result = IO_ERROR;
            }
            if (result == IO_ERROR) {
                fd_ptr->ERROR = IO_ERROR_WRITE;
                _lwsem_post(&dev_ptr->HW_ACCESS);
                return result;
            }

            if ((file_ptr->FLAGS & FLASHX_SECTOR_CACHE_ENABLED) &&
                (file_ptr->CACHE_BLOCK == b) && 
                (file_ptr->CACHE_SECTOR == s)) 
            {
                file_ptr->DIRTY_DATA = FALSE; /* data in the cache are invalid, because another data were written into flash */
            } /* Endif */
        }

        /* Sector written to the flash is not erased */
        file_ptr->ERASE_ARRAY[fs / 8] &= ~(0x1 << (fs % 8));
        fd_ptr->LOCATION += copy_num;
        data_ptr += copy_num;
        remains -= copy_num;
    }
      
    _lwsem_post(&dev_ptr->HW_ACCESS);

    if (fd_ptr->LOCATION >= fd_ptr->SIZE) {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
    }

    return num - remains;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_write_partial_sector
* Returned Value   : _mem_size - number of bytes written if successfully,
*                    0 otherwise
* Comments         : Writes a partial flash sector while preserving contents of
*                    unused portions of the sector
* 
*END*----------------------------------------------------------------------*/
_mem_size _io_flashx_write_partial_sector
   (
       /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,
   
      /* [IN] The block of sectors to write to */
      _mqx_uint  block_number,
   
      /* [IN] The sector number within the block */
      _mqx_uint  sector_number,
   
      /* [IN] The start offset within the sector */
      _mem_size  offset_in_sector,
   
      /* [IN] The number of bytes to write */
      _mem_size  bytes_to_write,
   
      /* [IN/OUT] The relative sector number within the file */ 
      _mqx_uint  file_sector,
   
      /* [IN] The data to write */
      char_ptr   src_ptr
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR  dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    _mem_size             sector_size;
    char_ptr              sector_dest_ptr;
    char_ptr              data_dest_ptr;
    _mem_size             num_data_before_write;
    _mem_size             num_data_after_write;  
   
    /* Get the sector size */
    sector_size = dev_ptr->HW_BLOCK[block_number].SECTOR_SIZE;
    /* Get the pointer to sector start */
    sector_dest_ptr = (char_ptr)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[block_number].START_ADDR + dev_ptr->HW_BLOCK[block_number].SECTOR_SIZE * sector_number);
    /* Get the pointer to start data write */ 
    data_dest_ptr = sector_dest_ptr + offset_in_sector;     

    /* Is the write area erased? */ 
    if (!_io_flashx_check_free_space(data_dest_ptr, bytes_to_write)) {
        /* Yes, write data */
        if (!(*dev_ptr->SECTOR_PROGRAM)(dev_ptr, src_ptr, data_dest_ptr, bytes_to_write)) { 
            return (_mem_size) IO_ERROR;
        } /* Endif */
    }
    else if ((file_ptr->FLAGS & FLASHX_FLASH_BUFFER_ENABLED) && (file_ptr->FLAGS & FLASHX_SECTOR_CACHE_ENABLED)) {  
        /* Writing using delayed write */
        if (!_allocate_sector_cache(fd_ptr))
            return (_mem_size) IO_ERROR;
        
        /* Buffering= delayed write caching */
        if ((file_ptr->DIRTY_DATA) && (file_ptr->CACHE_BLOCK != block_number || file_ptr->CACHE_SECTOR != sector_number)) {
            _io_flashx_flush_buffer(fd_ptr);
        }
        /* Read the sector for partial write from flash into RAM */
        _mem_copy((pointer)sector_dest_ptr, file_ptr->CACHE_PTR, sector_size);
 
        file_ptr->CACHE_BLOCK  = block_number;
        file_ptr->CACHE_SECTOR = sector_number;
        file_ptr->CACHE_FILE_SECTOR = file_sector;

        /* Write new data into RAM cache sector */
        _mem_copy((pointer)src_ptr, (pointer)((char_ptr) file_ptr->CACHE_PTR + offset_in_sector), bytes_to_write);

        /* Buffering: after an operation, leave cache valid for delayed write operation */
        file_ptr->DIRTY_DATA = TRUE;
    } /* Endif */
      
    else {
        /* Are there any data written before write area ? */
        num_data_before_write = _io_flashx_check_free_space(sector_dest_ptr, offset_in_sector); 
        /* Are there any data written behind write data? */
        num_data_after_write = _io_flashx_check_free_space(data_dest_ptr + bytes_to_write, sector_size - offset_in_sector - bytes_to_write );

        if (num_data_before_write || num_data_after_write) {
            /* If we have some data already in flash, we copy data to RAM and we assemble the sector to be written. */

            /* Allocate sector for partial write operation */
            if (!_allocate_sector_cache(fd_ptr))
                return (_mem_size) IO_ERROR;
        
            /* Flush buffer if data from another sector is valid in the cache.
            ** We do not need to perform this because the data in the sector cache
            ** are valid only when FLASHX_FLASH_BUFFER_ENABLED is set.
            */
            /*
            if ((file_ptr->DIRTY_DATA) && (file_ptr->CACHE_BLOCK != block_number || file_ptr->CACHE_SECTOR != sector_number)) {
                _io_flashx_flush_sector_buffer(fd_ptr);
            }
            */

            /* Read the sector to do the partial write into ram */
            _mem_copy((pointer)sector_dest_ptr, file_ptr->CACHE_PTR, sector_size);
            /* Write new data into ram copy sector */
            _mem_copy((pointer)src_ptr, (pointer)((char_ptr)file_ptr->CACHE_PTR + offset_in_sector), bytes_to_write);
            /* Erase sector */
            if (!_io_flashx_erase_sector(fd_ptr, file_sector, block_number, sector_number))
            {
                return (_mem_size) IO_ERROR;
            }
            /* Write the newly modified RAM sector back into flash, do not perform buffering (delayed write) */
            if (!(*dev_ptr->SECTOR_PROGRAM)(dev_ptr, file_ptr->CACHE_PTR, sector_dest_ptr, sector_size)) {
                return (_mem_size) IO_ERROR;
            } /* Endif */

            /* Sector caching: after the operation, invalidate cache and free memory. */
            file_ptr->DIRTY_DATA = FALSE;
            _mem_free(file_ptr->CACHE_PTR);
            file_ptr->CACHE_PTR = NULL;
        }
        else {
            /* Otherwise we do not need to store old data, we only erase sector and write new data */
            _io_flashx_erase_sector(fd_ptr, file_sector, block_number, sector_number);
            if (!(*dev_ptr->SECTOR_PROGRAM)(dev_ptr, src_ptr, data_dest_ptr, bytes_to_write)) {
                return (_mem_size) IO_ERROR;
            } /* Endif */
        }
    }

    /* Return number of bytes written */
    return bytes_to_write;
}

/*FUNCTION*****************************************************************
*
* Function Name    : _io_flashx_check_free_space
* Returned Value   : _mem_size
* Comments         :
*    Return position of last used byte in tested area.
*
*END*********************************************************************/
_mem_size _io_flashx_check_free_space(
    /* [IN] source pointer */
    char_ptr    src_ptr,
    /* [IN] number of bytes to check */
    _mem_size   num_bytes_to_check
)
{
    #define BLANK_MEM 0xFF
    uint_32 used_mem_position;
    
    /* Is the write area erased ? */
    for(used_mem_position = num_bytes_to_check; used_mem_position > 0; used_mem_position--)
    {
        /* Workaround for bug in CodeWarrior: do not optimize following two lines! */
        char_ptr tested_byte = src_ptr + used_mem_position - 1;
        if((char) *tested_byte != (char) BLANK_MEM)
            /* return last non-free byte possition */
            return used_mem_position;
    }
    /* space is free */
    return 0;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _io_flashx_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_flashx_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,
   
      /* [IN] the ioctl command */
      _mqx_uint    cmd,
   
      /* [IN/OUT] the ioctl parameters */
      pointer      param_ptr
   )
{ /* Body */
    IO_FLASHX_STRUCT_PTR  dev_ptr = (IO_FLASHX_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IO_FLASHX_FILE_STRUCT_PTR file_ptr = (IO_FLASHX_FILE_STRUCT_PTR) fd_ptr->DEV_DATA_PTR;
    _mqx_int                           result = MQX_OK;
    _mqx_uint                          i, j;
    _mqx_uint                          sb, ss, offset, fs;
    char_ptr                           temp_ptr;
    pointer _PTR_                      pparam_ptr;
    _mqx_uint_ptr                      uparam_ptr;
    _mem_size_ptr                      mparam_ptr;
   
    switch (cmd) {
        case FLASH_IOCTL_GET_BASE_ADDRESS:
            mparam_ptr = (_mem_size_ptr)param_ptr;
            *mparam_ptr = dev_ptr->BASE_ADDR;
            break;

        case FLASH_IOCTL_GET_BLOCK_GROUPS:
            for (i=0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++) { /* just count blocks */ }
            uparam_ptr = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = i;
            break;   

        case IO_IOCTL_GET_NUM_SECTORS:
            *(uint_32_ptr)param_ptr = fd_ptr->SIZE / MFS_SECTOR_SIZE;
            break;

        case FLASH_IOCTL_GET_NUM_SECTORS:
            _io_flashx_find_correct_sectors(fd_ptr, fd_ptr->SIZE, &sb, &ss, &offset, &fs);
            uparam_ptr = (_mqx_uint_ptr) param_ptr;
            *uparam_ptr = fs;
            break;

        case FLASH_IOCTL_GET_WIDTH: 
            uparam_ptr = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = dev_ptr->WIDTH;
            break;

        case IO_IOCTL_DEVICE_IDENTIFY:
            /* 
            ** This is to let the upper layer know what kind of device this is.
            ** It's a physical flash device, capable of being erased, read, seeked, 
            ** and written. Flash devices are not interrupt driven, so 
            ** IO_DEV_ATTR_POLL is included.
            */   
            uparam_ptr = (_mqx_uint_ptr)param_ptr;
            uparam_ptr[0] = IO_DEV_TYPE_PHYS_FLASHX;
            uparam_ptr[1] = IO_DEV_TYPE_LOGICAL_MFS;
            uparam_ptr[2] = IO_DEV_ATTR_ERASE | IO_DEV_ATTR_POLL
                             | IO_DEV_ATTR_READ | IO_DEV_ATTR_SEEK | 
                             IO_DEV_ATTR_WRITE;
            break; 
   
        case IO_IOCTL_GET_BLOCK_SIZE:
            /* returns the fixed size for MFS sector size */
            *(uint_32_ptr)param_ptr = MFS_SECTOR_SIZE;
            break;
      
        case FLASH_IOCTL_GET_SECTOR_SIZE:       
            /* 
            ** This returns the size of the sector after a user does an 
            ** fseek to the location he/she wants to know the sector size of.
            */
            _io_flashx_find_correct_sectors(fd_ptr, fd_ptr->LOCATION, &sb, &ss, &offset, &fs);
            mparam_ptr = (_mem_size_ptr) param_ptr;
            *mparam_ptr = dev_ptr->HW_BLOCK[sb].SECTOR_SIZE;
            break;

        case FLASH_IOCTL_GET_SECTOR_BASE:
            /* 
            ** This returns the start address of the sector after a user does an 
            ** fseek to the sector he/she wants know the start of.
            */
            _io_flashx_find_correct_sectors(fd_ptr, fd_ptr->LOCATION, &sb, &ss, &offset, &fs);
            mparam_ptr = (_mem_size_ptr) param_ptr;
            *mparam_ptr = dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[sb].START_ADDR + dev_ptr->HW_BLOCK[sb].SECTOR_SIZE * ss;
            break;

        case FLASH_IOCTL_GET_BLOCK_MAP:
            pparam_ptr = (pointer _PTR_) param_ptr;
            *pparam_ptr = (pointer) dev_ptr->HW_BLOCK;
            break;

        case IO_IOCTL_FLUSH_OUTPUT:
        case FLASH_IOCTL_FLUSH_BUFFER: 
            _lwsem_wait(&dev_ptr->HW_ACCESS);
            result = _io_flashx_flush_buffer(fd_ptr);
            _lwsem_post(&dev_ptr->HW_ACCESS);
            break;   
   
        case FLASH_IOCTL_ENABLE_BUFFERING:
           /* if RAM cache is enabled, enable buffer */
           if(file_ptr->FLAGS & FLASHX_SECTOR_CACHE_ENABLED)
           {
               file_ptr->FLAGS |= FLASHX_FLASH_BUFFER_ENABLED;
               result = MQX_OK;
           }
           /* else return error */
           else
               result = IO_ERROR;
           break;          

        case FLASH_IOCTL_DISABLE_BUFFERING: 
            _lwsem_wait(&dev_ptr->HW_ACCESS);
            result = _io_flashx_flush_buffer(fd_ptr);
            _lwsem_post(&dev_ptr->HW_ACCESS);
            if (result != MQX_OK) {
                break;
            } /* Endif */
            file_ptr->FLAGS &= ~FLASHX_FLASH_BUFFER_ENABLED;
            break;   

        case FLASH_IOCTL_ENABLE_SECTOR_CACHE:
            /* Set RAM cache flag */
            file_ptr->FLAGS |= FLASHX_SECTOR_CACHE_ENABLED;
            break; 

        case FLASH_IOCTL_DISABLE_SECTOR_CACHE:
            /* Disable buffering first */
            if(file_ptr->FLAGS & FLASHX_FLASH_BUFFER_ENABLED){
                 _lwsem_wait(&dev_ptr->HW_ACCESS);
                 result = _io_flashx_flush_sector_buffer(fd_ptr);
                 _lwsem_post(&dev_ptr->HW_ACCESS);
                 if (result != MQX_OK) {
                     break;
                 } /* Endif */
                 file_ptr->FLAGS &= ~FLASHX_FLASH_BUFFER_ENABLED;
            }
            /* clear ram cache flag */
            file_ptr->FLAGS &= ~FLASHX_SECTOR_CACHE_ENABLED;
            break; 

        case FLASH_IOCTL_ERASE_SECTOR:
            /* 
            ** This erases the sector after a user does an 
            ** fseek to the location of the sector he/she wants to erase.
            */
            if (!dev_ptr->SECTOR_ERASE) {
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            } /* Endif */   
            _io_flashx_find_correct_sectors(fd_ptr, fd_ptr->LOCATION, &sb, &ss, &offset, &fs);
            temp_ptr = (char_ptr)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[sb].START_ADDR + dev_ptr->HW_BLOCK[sb].SECTOR_SIZE * ss);

            _lwsem_wait(&dev_ptr->HW_ACCESS);
            if ((*dev_ptr->SECTOR_ERASE)(dev_ptr, temp_ptr, dev_ptr->HW_BLOCK[sb].SECTOR_SIZE)) {
                file_ptr->ERASE_ARRAY[fs / 8] |= (0x1 << (fs % 8));
                if ((sb == file_ptr->CACHE_BLOCK) && (ss == file_ptr->CACHE_SECTOR))
                    file_ptr->DIRTY_DATA = FALSE; /* invalidate cache */
            }
            else {
                result = IO_ERROR; /* could not erase sector */
            }
            _lwsem_post(&dev_ptr->HW_ACCESS);
            break;

        case FLASH_IOCTL_ERASE_CHIP:
            /* Though this command erases the whole flash,
            ** it does not invalidates all the caches
            ** associated with opened files on this driver.
             */ 
            if (dev_ptr->CHIP_ERASE) {
                _lwsem_wait(&dev_ptr->HW_ACCESS);
                if ((*dev_ptr->CHIP_ERASE)(dev_ptr)) {
                    for (i = 0; i < (file_ptr->ERASE_ARRAY_MAX + 7) / 8; i++) {
                        /* The whole chip is cleared, mark sectors as erased */
                        file_ptr->ERASE_ARRAY[i] = 0xFF;
                    }
                }
                else {
                    result = IO_ERROR; /* could not erase chip */
                }
                _lwsem_post(&dev_ptr->HW_ACCESS);
                break;
            } else if (!dev_ptr->SECTOR_ERASE) {
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            } else {
                fs = 0;
                _lwsem_wait(&dev_ptr->HW_ACCESS);
                for (i = 0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++) {
                    temp_ptr = (char_ptr)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[i].START_ADDR);
                    for (j = 0; j < dev_ptr->HW_BLOCK[i].NUM_SECTORS; j++) {
                        if ((*dev_ptr->SECTOR_ERASE)(dev_ptr, temp_ptr, dev_ptr->HW_BLOCK[i].SECTOR_SIZE)) {
                            file_ptr->ERASE_ARRAY[fs / 8] |= (0x1 << (fs % 8));
                            fs++;
                        }
                        else {
                            result = IO_ERROR; /* sector erase was not successfull */
                            break;
                        }
                        temp_ptr += dev_ptr->HW_BLOCK[i].SECTOR_SIZE;      
                    } /* Endfor */
                    if (result != MQX_OK)
                        break;
                } /* Endfor */
                _lwsem_post(&dev_ptr->HW_ACCESS);
            }/* Endif */
            break;
        
        case FLASH_IOCTL_WRITE_PROTECT:
            if (dev_ptr->WRITE_PROTECT != NULL) {
                _lwsem_wait(&dev_ptr->HW_ACCESS);
                result = (*dev_ptr->WRITE_PROTECT)(dev_ptr, *(_mqx_uint_ptr)param_ptr);
                _lwsem_post(&dev_ptr->HW_ACCESS);
            }
            break;
        
        default:
            if(dev_ptr->IOCTL != NULL){
                result = (*dev_ptr->IOCTL)(dev_ptr, cmd, param_ptr);
            }
            else {
                result = IO_ERROR_INVALID_IOCTL_CMD;    
            }         
            break;
    } /* Endswitch */

    return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_flashx_wait_us
* Returned Value   : none
* Comments         : 
*    This function waits at least the given amount of us, but it is possible
*    to wait longer
* 
*END*----------------------------------------------------------------------*/

void _io_flashx_wait_us
   (
      /* [IN] the time to spin wait for */
      _mqx_int wait_us
   ) 
{/* Body */
    _mqx_int total_us = 0;
    _mqx_int start_us;
    _mqx_int end_us;
     
    start_us = (int_32) _time_get_microseconds();
    while (total_us < wait_us) {
        end_us = (int_32) _time_get_microseconds();
        if (end_us > start_us) {
            total_us += (end_us - start_us);
        } /* Endif */
        start_us = end_us;
    } /* Endwhile */

} /* Endbody */

/* EOF */
