#ifndef _flashx_h_
#define _flashx_h_
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
* $FileName: flashx.h$
* $Version : 3.8.16.0$
* $Date    : Oct-11-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the flash drivers
*
*END************************************************************************/

#include "ioctl.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*
** Flash IOCTL calls
*/
#define FLASH_IOCTL_GET_BASE_ADDRESS     _IO(IO_TYPE_FLASH,0x01)
#define FLASH_IOCTL_GET_NUM_SECTORS      _IO(IO_TYPE_FLASH,0x02)
#define FLASH_IOCTL_GET_SECTOR_SIZE      _IO(IO_TYPE_FLASH,0x03)
#define FLASH_IOCTL_GET_WIDTH            _IO(IO_TYPE_FLASH,0x04)
#define FLASH_IOCTL_GET_SECTOR_BASE      _IO(IO_TYPE_FLASH,0x05)
#define FLASH_IOCTL_GET_BLOCK_GROUPS     _IO(IO_TYPE_FLASH,0x06)
#define FLASH_IOCTL_GET_BLOCK_MAP        _IO(IO_TYPE_FLASH,0x07)
#define FLASH_IOCTL_FLUSH_BUFFER         _IO(IO_TYPE_FLASH,0x08)
#define FLASH_IOCTL_ENABLE_BUFFERING     _IO(IO_TYPE_FLASH,0x09)
#define FLASH_IOCTL_DISABLE_BUFFERING    _IO(IO_TYPE_FLASH,0x0A)
#define FLASH_IOCTL_ERASE_SECTOR         _IO(IO_TYPE_FLASH,0x0B)
#define FLASH_IOCTL_ERASE_CHIP           _IO(IO_TYPE_FLASH,0x0C)
#define FLASH_IOCTL_ENABLE_SECTOR_CACHE  _IO(IO_TYPE_FLASH,0x0D)
#define FLASH_IOCTL_DISABLE_SECTOR_CACHE _IO(IO_TYPE_FLASH,0x0E)
#define FLASH_IOCTL_WRITE_PROTECT        _IO(IO_TYPE_FLASH,0x0F)
#define FLASH_IOCTL_GET_WRITE_PROTECT    _IO(IO_TYPE_FLASH,0x10)
#define FLASH_IOCTL_SWAP_FLASH_AND_RESET _IO(IO_TYPE_FLASH,0x11)
#define FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_ENABLE   _IO(IO_TYPE_FLASH,0x12) 
#define FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_DISABLE  _IO(IO_TYPE_FLASH,0x13) 
 
/*
** Flash Error Codes 
*/

#define FLASHXERR_NO_ERROR                (0)
#define FLASHXERR_INVALID_SIZE             (FLASHX_ERROR_BASE | 0x01)

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/
/* The flash buffering is delayed write caching */
#define FLASHX_FLASH_BUFFER_ENABLED      (0x00000001)
/* Sector cache enables the driver to allocate sector-sized cache.
** Without this feature buffering is not possible.
** Without this feature also partial writes to sector are not possible
** (besides the cases when the sector was erased before).
*/
#define FLASHX_SECTOR_CACHE_ENABLED      (0x00000002)
#define FLASHX_WR_ERASE_FROM_FLASH_ENABLED  (0x00000004)

/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

/* Forward declaration of private structure */
struct io_flashx_struct;

/*
** This structure is used to describe HW layout of flashx blocks belonging to specific controller
*/
typedef struct flashx_block_info_struct
{
    /* The number of sectors with the same size in this block */
    _mqx_uint        NUM_SECTORS;
   
    /* Physical start address of the block of same size sectors */
    _mem_size        START_ADDR;

    /* The size of the sectors in this block */
    _mem_size        SECTOR_SIZE;

    /* Any tag that will be handled by corresponding layer (usually HW layer) */
    uint_32          SPECIAL_TAG;

} FLASHX_BLOCK_INFO_STRUCT, _PTR_ FLASHX_BLOCK_INFO_STRUCT_PTR;

/*
** This structure is used to describe file management of flashx
** driver instance. Note that the file can span over several HW blocks.
*/
typedef struct flashx_file_block
{
    /* The name of file associated with the region */
    char_ptr const   FILENAME;
   
    /* Physical start address of the file */
    _mem_size        START_ADDR;

    /* The size of the sectors in this block */
    _mem_size        END_ADDR;

} FLASHX_FILE_BLOCK, _PTR_ FLASHX_FILE_BLOCK_PTR;

/* 
** This structure is used solely by the driver to describe the low level driver.
*/
typedef struct flashx_device_if_struct
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

} FLASHX_DEVICE_IF_STRUCT, _PTR_ FLASHX_DEVICE_IF_STRUCT_PTR;

/* 
** This structure is used to install a flash device. Configurable by BSP.
*/
typedef struct flashx_init_struct
{
    /* Base address of the flash device. Useful for external flash devices
    ** whose address space is determined by BSP.
    */
    _mem_size                             BASE_ADDR;

    /* This array of strutures provides the description of HW flash configuration */ 
    const FLASHX_BLOCK_INFO_STRUCT _PTR_  HW_BLOCK;

    /* This array of strutures provides the description of files */ 
    const FLASHX_FILE_BLOCK _PTR_         FILE_BLOCK;

    /* Interface to device algorithms */
    const FLASHX_DEVICE_IF_STRUCT _PTR_   DEVICE_IF;

    /* The bus width of the device (in bytes, i.e. WIDTH = 4 for 32 bits) */
    _mqx_uint                             WIDTH;

    /* The number of devices in parallel */
    _mqx_uint                             DEVICES;

    /* When finished programming, should a comparison of data be made
    ** to verify that the write worked correctly?
    */
    _mqx_uint                             WRITE_VERIFY;

    /* The address of device specific structure */ 
    pointer                               DEVICE_SPECIFIC_INIT;

} FLASHX_INIT_STRUCT, _PTR_ FLASHX_INIT_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif
  
_mqx_uint _io_flashx_install(char_ptr, FLASHX_INIT_STRUCT const _PTR_);
_mqx_int  _io_flashx_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
