#ifndef _flashxprv_h_
#define _flashxprv_h_
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
* $FileName: flashxprv.h$
* $Version : 3.8.10.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the flash drivers
*
*END************************************************************************/


/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

/*
** The address of this structure is used to maintain flash specific 
** information.
*/
typedef struct io_flashx_struct
{
    /* The function to call to erase a sector on the device */
    boolean (_CODE_PTR_           SECTOR_ERASE)(struct io_flashx_struct *, char_ptr, _mem_size);

    /* The function to call to program a sector on the device */
    boolean (_CODE_PTR_           SECTOR_PROGRAM)(struct io_flashx_struct *, char_ptr, char_ptr, _mem_size);
   
    /* The function to call to erase the entire device */
    boolean (_CODE_PTR_           CHIP_ERASE)(struct io_flashx_struct *);
   
    /* The function to call to read data from flash */
    _mqx_int (_CODE_PTR_          READ)(struct io_flashx_struct *, char_ptr, char_ptr, _mem_size);

    /* The function to call to initialize the device */
    boolean (_CODE_PTR_           INIT)(struct io_flashx_struct *);
   
    /* The function to call to disable the device */
    void (_CODE_PTR_              DEINIT)(struct io_flashx_struct *);

    /* The function to call to write enable or protect the device */
    boolean (_CODE_PTR_           WRITE_PROTECT)(struct io_flashx_struct *, _mqx_uint);
   
    /* The ioctl function to call */
    _mqx_int (_CODE_PTR_          IOCTL)(struct io_flashx_struct *, _mqx_uint, pointer);

    /* Base address of the flash device. Useful for external flash devices
    ** whose address space is determined by BSP.
    */
    _mem_size                     BASE_ADDR;

    /* This array of strutures provides the description of HW flash configuration */ 
    FLASHX_BLOCK_INFO_STRUCT_PTR  HW_BLOCK;
   
    /* This array of strutures provides the description of HW flash configuration */ 
    FLASHX_FILE_BLOCK_PTR         FILE_BLOCK;
   
    /* The bus width of the device (in bytes, i.e. WIDTH = 4 for 32 bits) */
    _mqx_uint                     WIDTH;
   
    /* The number of parallel devices */
    _mqx_uint                     DEVICES;
   
    /* Light weight semaphore for HW access exclusion */
    LWSEM_STRUCT                  HW_ACCESS;

    /* Number of files opened */
    _mqx_uint                     FILES;
   
    /* Flags */
    _mqx_uint                     FLAGS;
   
    /* When finished programming, should a comparison of data be made
    ** to verify that the write worked correctly.
    */
    _mqx_uint                     WRITE_VERIFY;

    /* The address of device specific structure got in init phase */ 
    pointer                       DEVICE_SPECIFIC_INIT;

    /* The address of device specific structure */ 
    pointer                       DEVICE_SPECIFIC_DATA;

} IO_FLASHX_STRUCT, _PTR_ IO_FLASHX_STRUCT_PTR;

typedef struct io_flashx_file_struct
{
    /* HW block reference */
    FLASHX_FILE_BLOCK const _PTR_ FILE_BLOCK;

    /* Next four are needed for caching data */
    _mqx_uint                     DIRTY_DATA;
    _mqx_int                      CACHE_BLOCK;
    _mqx_int                      CACHE_SECTOR;
    _mqx_int                      CACHE_FILE_SECTOR;
    /* One sector cache memory of size MAX_SECT_SIZE */
    char_ptr                      CACHE_PTR;

    /* The maximum sector size of this file: useful for allocating cache space */
    _mem_size                     MAX_SECT_SIZE;

    /* The size of the erase array in bytes */
    _mqx_uint                     ERASE_ARRAY_MAX;

    /* The address of erase check array */
    uint_8 _PTR_                  ERASE_ARRAY;

    /* Flags */
    _mqx_uint                     FLAGS;
   
} IO_FLASHX_FILE_STRUCT, _PTR_ IO_FLASHX_FILE_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

_mqx_int  _io_flashx_flush_buffer(MQX_FILE_PTR); 
_mqx_int  _io_flashx_flush_sector_buffer(MQX_FILE_PTR); 
 boolean  _io_flashx_erase_sector(MQX_FILE_PTR, _mqx_uint, _mqx_uint, _mqx_uint);
FLASHX_BLOCK_INFO_STRUCT_PTR _io_flashx_phys_to_sector(IO_FLASHX_STRUCT_PTR, _mem_size, _mqx_uint _PTR_, _mem_size _PTR_, _mem_size _PTR_);
 void     _io_flashx_find_correct_sectors(MQX_FILE_PTR, _mqx_uint, _mqx_uint _PTR_, _mqx_uint _PTR_, _mqx_uint _PTR_, _mqx_uint _PTR_);
_mqx_int  _io_flashx_open(MQX_FILE_PTR, char_ptr, char_ptr);
_mqx_int  _io_flashx_close(MQX_FILE_PTR);
_mqx_int  _io_flashx_write(MQX_FILE_PTR, char_ptr, _mqx_int);
_mqx_int  _io_flashx_read(MQX_FILE_PTR, char_ptr, _mqx_int);
_mqx_int  _io_flashx_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
_mem_size _io_flashx_write_partial_sector(MQX_FILE_PTR, _mqx_uint, _mqx_uint, _mem_size, _mem_size, _mqx_uint, char_ptr);
_mem_size _io_flashx_check_free_space(char_ptr, _mem_size);
void      _io_flashx_wait_us(_mqx_int);

boolean _allocate_sector_cache(MQX_FILE_PTR);
   
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
