#ifndef _apcflshpr_h_
#define _apcflshpr_h_
/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: apcflshpr.h$
* $Version : 3.8.8.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure
*   definitions private to the advanced PC Card flash drivers
*
*END************************************************************************/

#include "bsp.h"
#include "ata.h"

/* Default values used unless definition is provided by apccard layer */
#ifndef APCCARD_BUS_WIDTH
#define APCCARD_BUS_WIDTH     1
#endif

#ifndef APCCARD_ADDR_SHIFT
#define APCCARD_ADDR_SHIFT    0
#endif

/* Register offsets taking address bus shift into account */
#define PCFLASH_REG_BASE            (ATA_REG_BASE << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_ERROR           (ATA_ERROR << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_FEATURES        (ATA_FEATURES << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_SECTOR_COUNT    (ATA_SECTOR_COUNT << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_SECTOR          (ATA_SECTOR << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_CYLINDER_LOW    (ATA_CYLINDER_LOW << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_CYLINDER_HIGH   (ATA_CYLINDER_HIGH << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_CARD_DRIVE_HEAD (ATA_CARD_DRIVE_HEAD << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_STATUS          (ATA_STATUS << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_COMMAND         (ATA_COMMAND << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_DUP_RDE_DATA    (ATA_DUP_RDE_DATA << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_DUP_RDO_DATA    (ATA_DUP_RDO_DATA << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_ALT_STATUS_CMD  (ATA_ALT_STATUS_CMD << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_DEV_CNTRL       (ATA_DEV_CNTRL << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG_DATA_BUF        (ATA_DATA_BUF << APCCARD_ADDR_SHIFT)


#define PCFLASH_REG16_SECTOR        (ATA_REG16_SECTOR << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG16_CYLINDER      (ATA_REG16_CYLINDER << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG16_DHC           (ATA_REG16_DHC << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG16_STATUS        (ATA_REG16_STATUS << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG16_DATA          (ATA_REG16_DATA << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG16_ERROR         (ATA_REG16_ERROR << APCCARD_ADDR_SHIFT)
#define PCFLASH_REG16_DEV_CNTRL     (ATA_REG16_DEV_CNTRL << APCCARD_ADDR_SHIFT)


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* Error codes from lower layers */
#define PCFLASH_NO_ERROR             (0)
#define PCFLASH_ERROR_NO_CARD        (PCFLASH_ERROR_BASE | 0x01)
#define PCFLASH_INVALID_SECTOR       (PCFLASH_ERROR_BASE | 0x02)
#define PCFLASH_INVALID_CARD         (PCFLASH_ERROR_BASE | 0x03)
#define PCFLASH_INVALID_DRIVE        (PCFLASH_ERROR_BASE | 0x04)
#define PCFLASH_INVALID_VOLTAGE      (PCFLASH_ERROR_BASE | 0x05)
#define PCFLASH_INVALID_HANDLE       (PCFLASH_ERROR_BASE | 0x06)
#define PCFLASH_HARDWARE_ERROR       (PCFLASH_ERROR_BASE | 0x07)
#define PCFLASH_READ_ERROR           (PCFLASH_ERROR_BASE | 0x08)
#define PCFLASH_WRITE_ERROR          (PCFLASH_ERROR_BASE | 0x09)

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/


/*
** PC_FLASH_INFO_STRUCT
** Run time state information for each PC Card Flash device
*/
typedef struct io_pcflash_info_struct
{
   /* Handle for PCCard calls */
   MQX_FILE_PTR  PCCARD_STREAM;

   /* Drive number to associate with this slot */
   uint_32       DRIVE;

   /* Sector size in bytes */
   uint_32       SECTOR_SIZE;

   /* The total number of sectors in the device */
   uint_32       NUM_SECTORS;

   /* Total size of flash card in bytes */
   uint_32       SIZE;

   /* The number of heads as reported by the ATA ident command */
   uint_32       NUMBER_OF_HEADS;

   /* The number of tracks as reported by the ATA ident command */
   uint_32       NUMBER_OF_TRACKS;

   /* The number of sectos per cylinder as reported by the ATA ident command */
   uint_32       SECTORS_PER_TRACK;

   /* ATA Register location */
   uchar_ptr     ATA_REG_PTR;

   /* ATA Data Register location */
   uint_16_ptr   ATA_DATA_PTR;

   /* Light weight semaphore struct */
   LWSEM_STRUCT  LWSEM;

   /* The address of temp buffer */
   uchar_ptr     TEMP_BUFF_PTR;

   /* The current error code for the device */
   uint_32       ERROR_CODE;

   /* Indicates if the device is running in block mode or character mode */
   boolean       BLOCK_MODE;


} IO_PCFLASH_STRUCT, _PTR_ IO_PCFLASH_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_apcflash_open(FILE_DEVICE_STRUCT_PTR, char _PTR_, char _PTR_);
extern _mqx_int _io_apcflash_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_apcflash_read (FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_apcflash_write(FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_apcflash_ioctl(FILE_DEVICE_STRUCT_PTR, _mqx_uint, pointer);

extern uint_32 _io_apcflash_read_sector(IO_PCFLASH_STRUCT_PTR, uint_32,
   uchar_ptr);
extern int_32  _io_apcflash_read_partial_sector(IO_PCFLASH_STRUCT_PTR, uint_32,
   uint_32, uint_32, uchar_ptr);
extern uint_32 _io_apcflash_write_sector(IO_PCFLASH_STRUCT_PTR, uint_32,
   uchar_ptr);
extern int_32  _io_apcflash_write_partial_sector(IO_PCFLASH_STRUCT_PTR,
   uint_32, uint_32, uint_32, uchar_ptr);
extern uint_32 _io_apcflash_identify_device(IO_PCFLASH_STRUCT_PTR, uchar_ptr);
extern uint_32 _io_apcflash_reset(IO_PCFLASH_STRUCT_PTR);
extern boolean _io_apcflash_status_timeout(IO_PCFLASH_STRUCT_PTR, uint_32, uchar);
/* Start CR 812 */
extern _mqx_int _io_apcflash_read_write_blocks(FILE_DEVICE_STRUCT_PTR,
   IO_PCFLASH_STRUCT_PTR, char_ptr, _mqx_int, boolean);
/* End   CR 812 */

#ifdef __cplusplus
}
#endif

#endif
