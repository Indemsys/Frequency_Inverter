#ifndef _nandflash_h_
#define _nandflash_h_
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: nandflash.h$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the NAND flash driver.
*
*END************************************************************************/

#include "ioctl.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*
** NAND Flash command set
*/
#define NANDFLASH_CMD_PAGE_READ_CYCLE1       0x00
#define NANDFLASH_CMD_PAGE_READ_CYCLE2       0x30
#define NANDFLASH_CMD_RESET                  0xFF
#define NANDFLASH_CMD_PAGE_PROGRAM_CYCLE1    0x80
#define NANDFLASH_CMD_PAGE_PROGRAM_CYCLE2    0x10
#define NANDFLASH_CMD_BLOCK_ERASE_CYCLE1     0x60
#define NANDFLASH_CMD_BLOCK_ERASE_CYCLE2     0xD0
#define NANDFLASH_CMD_READ_STATUS            0x70
#define NANDFLASH_CMD_READ_ID                0x90

/*
** NAND Flash status response
*/
#define NANDFLASH_STATUS_PASS                0x00
#define NANDFLASH_STATUS_ERR                 0x01

/*
** NAND Flash IOCTL calls
*/
#define NANDFLASH_IOCTL_GET_PHY_PAGE_SIZE    _IO(IO_TYPE_NANDFLASH,0x01)
#define NANDFLASH_IOCTL_GET_SPARE_AREA_SIZE  _IO(IO_TYPE_NANDFLASH,0x02)
#define NANDFLASH_IOCTL_GET_BLOCK_SIZE       _IO(IO_TYPE_NANDFLASH,0x03)
#define NANDFLASH_IOCTL_GET_NUM_BLOCKS       _IO(IO_TYPE_NANDFLASH,0x04)
#define NANDFLASH_IOCTL_GET_WIDTH            _IO(IO_TYPE_NANDFLASH,0x05)
#define NANDFLASH_IOCTL_GET_NUM_VIRT_PAGES   _IO(IO_TYPE_NANDFLASH,0x06)
#define NANDFLASH_IOCTL_GET_VIRT_PAGE_SIZE   _IO(IO_TYPE_NANDFLASH,0x07)
#define NANDFLASH_IOCTL_ERASE_BLOCK          _IO(IO_TYPE_NANDFLASH,0x08)
#define NANDFLASH_IOCTL_ERASE_CHIP           _IO(IO_TYPE_NANDFLASH,0x09)
#define NANDFLASH_IOCTL_WRITE_PROTECT        _IO(IO_TYPE_NANDFLASH,0x0A)
#define NANDFLASH_IOCTL_GET_WRITE_PROTECT    _IO(IO_TYPE_NANDFLASH,0x0B)
#define NANDFLASH_IOCTL_CHECK_BLOCK          _IO(IO_TYPE_NANDFLASH,0x0C)
#define NANDFLASH_IOCTL_MARK_BLOCK_AS_BAD    _IO(IO_TYPE_NANDFLASH,0x0D)
#define NANDFLASH_IOCTL_GET_BAD_BLOCK_TABLE  _IO(IO_TYPE_NANDFLASH,0x0E)
#define NANDFLASH_IOCTL_GET_ID               _IO(IO_TYPE_NANDFLASH,0x0F)
#define NANDFLASH_IOCTL_ERASE_BLOCK_FORCE    _IO(IO_TYPE_NANDFLASH,0x10)
 
/*
** NAND Flash Error Codes 
*/

#define NANDFLASHERR_NO_ERROR                (0)
#define NANDFLASHERR_ECC_FAILED              (NANDFLASH_ERROR_BASE | 0x01)
#define NANDFLASHERR_ECC_CORRECTED           (NANDFLASH_ERROR_BASE | 0x02)
#define NANDFLASHERR_ERASE_FAILED            (NANDFLASH_ERROR_BASE | 0x03)
#define NANDFLASHERR_WRITE_FAILED            (NANDFLASH_ERROR_BASE | 0x04)
#define NANDFLASHERR_TIMEOUT                 (NANDFLASH_ERROR_BASE | 0x05)
#define NANDFLASHERR_BLOCK_BAD               (NANDFLASH_ERROR_BASE | 0x06)
#define NANDFLASHERR_BLOCK_NOT_BAD           (NANDFLASH_ERROR_BASE | 0x07)
#define NANDFLASHERR_INFO_STRUC_MISSING      (NANDFLASH_ERROR_BASE | 0x08)
#define NANDFLASHERR_IMPROPER_ECC_SIZE       (NANDFLASH_ERROR_BASE | 0x09)

/*
** NAND Flash ID Masks 
*/

#define NANDFLASH_ID_PAGE_SIZE_MASK           0x00000003
#define NANDFLASH_ID_SPARE_BYTES_NUM_MASK     0x0000000C
#define NANDFLASH_ID_BLOCK_SIZE_MASK          0x00000030
#define NANDFLASH_ID_WIDTH_MASK               0x00000040
#define NANDFLASH_ID_DENSITY_MASK             0x000F0000

#define NANDFLASH_DENSITY_1Gb                 0x01
#define NANDFLASH_DENSITY_2Gb                 0x0A
#define NANDFLASH_DENSITY_4Gb                 0x0C
#define NANDFLASH_DENSITY_8Gb                 0x03
#define NANDFLASH_DENSITY_16Gb                0x05

/*
** ECC size to ECC bytes conversion
*/

#define NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(x) ((x * 2) - (x >> 3))


/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

/*
** NAND FLASH INFO STRUCT
**
** This structure is used to describe the organization of a NAND flash chip
*/
typedef struct nandflash_info_struct
{
   /* The size of the NAND Flash physical page in Bytes (without spare bytes) */
   _mem_size        PHY_PAGE_SIZE;

   /* The size of the NAND Flash spare area in Bytes */
   _mem_size        SPARE_AREA_SIZE;

   /* The size of one block in Bytes */
   _mem_size        BLOCK_SIZE;

   /* The number of NAND Flash blocks */
   _mqx_uint        NUM_BLOCKS;
   
   /* the width of the device in Bytes */
   _mqx_uint        WIDTH;

} NANDFLASH_INFO_STRUCT, _PTR_ NANDFLASH_INFO_STRUCT_PTR;

/*
** NANDFLASH STRUCT
**
** The address of this structure is used to maintain NAND flash specific 
** information.
*/

typedef struct io_nandflash_struct
{
   /* The function to call to initialize the NAND flash device, if needed */
   uint_32 (_CODE_PTR_           INIT)(struct io_nandflash_struct _PTR_);

   /* The function to call to disable the NAND flash device, if needed */
   void (_CODE_PTR_              DEINIT)(struct io_nandflash_struct _PTR_);
   
   /* The function to call to erase the entire NAND flash chip */
   uint_32 (_CODE_PTR_           CHIP_ERASE)(struct io_nandflash_struct _PTR_);

   /* The function to call to erase one NAND flash block */
   uint_32 (_CODE_PTR_           BLOCK_ERASE)(struct io_nandflash_struct _PTR_, 
      uint_32, boolean);

   /* The function to call to read one page of the NAND flash */
   uint_32 (_CODE_PTR_           PAGE_READ)(struct io_nandflash_struct _PTR_, 
      uchar_ptr, uint_32, uint_32);

   /* The function to call to program one page of the NAND flash */
   uint_32 (_CODE_PTR_           PAGE_PROGRAM)(struct io_nandflash_struct _PTR_, 
      uchar_ptr, uint_32, uint_32);
   
   /* The function to call to write-enable or write-protect the device */
   uint_32 (_CODE_PTR_           WRITE_PROTECT)(struct io_nandflash_struct _PTR_, 
      boolean);
   
   /* The function to call to check if the defined block is bad */
   uint_32 (_CODE_PTR_           IS_BLOCK_BAD)(struct io_nandflash_struct _PTR_, 
      uint_32);
   
   /* The function to call to mark the defined block as bad */
   uint_32 (_CODE_PTR_           MARK_BLOCK_AS_BAD)(struct io_nandflash_struct _PTR_, 
      uint_32);
   
   /* The ioctl function to call */
   _mqx_int (_CODE_PTR_          IOCTL)(struct io_nandflash_struct _PTR_, 
      _mqx_uint, pointer);

   /* This structure provides an orgaization of the NAND flash device */ 
   NANDFLASH_INFO_STRUCT_PTR     NANDFLASH_INFO_PTR;
   
   /* The size of one virtual page in Bytes.
      One Physical page can be divided into several virtual pages if supported 
      by the NAND Flash Controller. Virtual page is the smallest unit a block
      device can work with. */
   _mem_size                     VIRTUAL_PAGE_SIZE;

   /* The number of NAND Flash virtual pages */
   _mqx_uint                     NUM_VIRTUAL_PAGES;
   
   /* The ratio between the physical page size and the virtual page size */
   _mqx_uint                     PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO;
   
   /* The number of ECC correction bits per one virtual page */
   uint_32                       ECC_SIZE;
   
   /* When finished programming, should a comparison of data be made
   ** to verify that the write worked correctly.
   */
   _mqx_uint                     WRITE_VERIFY;

   /* Light weight semaphore struct */
   LWSEM_STRUCT                  LWSEM;
   
   /* The current error code for the device */
   _mqx_uint                     ERROR_CODE;
   
   /* The number of tasks which have access to the NAND flash device */
   _mqx_uint                     COUNT;
   
   /* Flags */
   _mqx_uint                     FLAGS;
   
   /* The address of device specific structure */ 
   pointer                       DEVICE_SPECIFIC_DATA;

} IO_NANDFLASH_STRUCT, _PTR_ IO_NANDFLASH_STRUCT_PTR;




/* 
** NANDFLASH INIT STRUCT
**
** This structure is used to initialize a NAND flash device.
*/
typedef struct nandflash_init_struct
{

   /* A pointer to a string that identifies the device for fopen */
   char_ptr                      ID_PTR;
   
   /* The function to call to initialize the NAND flash device, if needed */
   uint_32 (_CODE_PTR_           INIT)(struct io_nandflash_struct _PTR_);

   /* The function to call to disable the NAND flash device, if needed */
   void (_CODE_PTR_              DEINIT)(struct io_nandflash_struct _PTR_);
   
   /* The function to call to erase the entire NAND flash chip */
   uint_32 (_CODE_PTR_           CHIP_ERASE)(struct io_nandflash_struct _PTR_);

   /* The function to call to erase one NAND flash block */
   uint_32 (_CODE_PTR_           BLOCK_ERASE)(struct io_nandflash_struct _PTR_, 
      uint_32, boolean);

   /* The function to call to read one page of the NAND flash */
   uint_32 (_CODE_PTR_           PAGE_READ)(struct io_nandflash_struct _PTR_, 
      uchar_ptr, uint_32, uint_32);

   /* The function to call to program one page of the NAND flash */
   uint_32 (_CODE_PTR_           PAGE_PROGRAM)(struct io_nandflash_struct _PTR_, 
      uchar_ptr, uint_32, uint_32);
   
   /* The function to call to write-enable or write-protect the device */
   uint_32 (_CODE_PTR_           WRITE_PROTECT)(struct io_nandflash_struct _PTR_, 
      boolean);
   
   /* The function to call to check if the defined block is bad */
   uint_32 (_CODE_PTR_           IS_BLOCK_BAD)(struct io_nandflash_struct _PTR_, 
      uint_32);
   
   /* The function to call to mark the defined block as bad */
   uint_32 (_CODE_PTR_           MARK_BLOCK_AS_BAD)(struct io_nandflash_struct _PTR_, 
      uint_32);
   
   /* The ioctl function to call */
   _mqx_int (_CODE_PTR_          IOCTL)(IO_NANDFLASH_STRUCT_PTR, 
      _mqx_uint, pointer);

   /* This structure provides an orgaization of the NAND flash device */ 
   NANDFLASH_INFO_STRUCT_PTR     NANDFLASH_INFO_PTR;
      
   /* The size of one virtual page in Bytes.
      One Physical page can be divided into several virtual pages if supported 
      by the NAND Flash Controller. Virtual page is the smallest unit a block
      device can work with. */
   _mem_size                     VIRTUAL_PAGE_SIZE;

   /* The number of NAND Flash virtual pages */
   _mqx_uint                     NUM_VIRTUAL_PAGES;
   
   /* The ratio between the physical page size and the virtual page size */
   _mqx_uint                     PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO;
   
   /* The number of ECC correction bits per one virtual page */
   uint_32                       ECC_SIZE;
   
   /* When finished programming, should a comparison of data be made
   ** to verify that the write worked correctly.
   */
   _mqx_uint                     WRITE_VERIFY;

   /* The address of device specific structure */ 
   pointer                       DEVICE_SPECIFIC_DATA;
   

} NANDFLASH_INIT_STRUCT, _PTR_ NANDFLASH_INIT_STRUCT_PTR;

typedef const NANDFLASH_INIT_STRUCT _PTR_ NANDFLASH_INIT_STRUCT_CPTR;
/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif
  
extern _mqx_uint _io_nandflash_install(NANDFLASH_INIT_STRUCT_CPTR);
extern _mqx_int  _io_nandflash_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
