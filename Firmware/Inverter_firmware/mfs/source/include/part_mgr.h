#ifndef __part_mgr_h__
#define __part_mgr_h__
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
* $FileName: part_mgr.h$
* $Version : 3.8.10.0$
* $Date    : Aug-23-2012$
*
* Comments:
*
*   This file contains internal data structures and constants
*   used to manage partitions.
*
*END************************************************************************/


/*
**  Defines constant for partiton
*/
#define  PMGR_VALID_STATE              1
#define  PMGR_INVALID_STATE            0

#define  PMGR_PARTITION_TABLE_OFFSET   0x01BE
#define  PMGR_MAX_PARTITIONS           0x04
#define  PMGR_ENTRY_SIZE               0x10

/* The 2 extra bytes store a validation signature */
#define  PMGR_PART_TABLE_SIZE          PMGR_ENTRY_SIZE * PMGR_MAX_PARTITIONS + 2


/*
** Partition types
*/
#define  PMGR_PARTITION_NOT_USED        0x00
#define  PMGR_PARTITION_FAT_12_BIT      0x01
#define  PMGR_PARTITION_FAT_16_BIT      0x04
#define  PMGR_PARTITION_HUGE            0x06
#define  PMGR_PARTITION_FAT32           0x0B
#define  PMGR_PARTITION_FAT32_LBA       0x0C
#define  PMGR_PARTITION_HUGE_LBA        0x0E


/* Structure of a partition entry on the disk */
typedef struct pmgr_part_entry_struct
{
   /* Active flag (0 = not bootable, 0x80 = bootable) */
   uchar     ACTIVE_FLAG;

   /* Starting head */
   uchar     START_HEAD;

   /* Starting sector/cylinder */
   uchar     START_SECT_CYL;

   /* Starting cylinder */
   uchar     START_CYLINDER;

   /* Partition type (0 not used, 1 FAT 12 bit, 4 FAT 16 bit, 5 extended,
   **                 6 huge - DOS 4.0+, 0x0B FAT32, 0x0C FAT32 LBA,
   **                 0x0E FAT16 LBA)
   */
   uchar     TYPE;

   /* Ending head */
   uchar     ENDING_HEAD;

   /* Ending sector/cylinder */
   uchar     ENDING_SECT_CYL;

   /* Ending cylinder */
   uchar     ENDING_CYLINDER;

   /* Starting sector for partition, relative to beginning of disk */
   uchar     START_SECTOR[4];

   /* Partition length in sectors */
   uchar     LENGTH[4];

} PMGR_PART_ENTRY_STRUCT, _PTR_ PMGR_PART_ENTRY_STRUCT_PTR;


/* Structure passed to/from ioctl call IO_IOCTL_GET_PARTITION and IO_IOCTL_SET_PARTITION */
typedef struct pmgr_part_info_struct
{
   /* Partition slot (1 to 4) */
   uchar     SLOT;

   /* Partition type */
   uchar     TYPE;

   /* Active flag (0 = not bootable, 0x80 = bootable) */
   uchar     ACTIVE_FLAG;

   /* Padding */
   uint_8    RESERVED;

   /* Starting sector for partition, relative to beginning of disk */
   uint_32   START_SECTOR;

   /* Partition length in sectors */
   uint_32   LENGTH;

   /* Heads per Cylinder */
   uchar     HEADS;

   /* Sectors per head */
   uchar     SECTORS;

   /* Cylinders on the device */
   uint_16   CYLINDERS;

} PMGR_PART_INFO_STRUCT, _PTR_ PMGR_PART_INFO_STRUCT_PTR;


typedef struct part_mgr_struct
{
    MQX_FILE_PTR                     DEV_FILE_PTR;
    LWSEM_STRUCT                     SEM;
    uint_32                          DEV_SECTOR_SIZE;
    uint_32                          DEV_NUM_SECTORS;
    boolean                          BLOCK_MODE;
    uint_32                          INSTANCES;
} PART_MGR_STRUCT, _PTR_ PART_MGR_STRUCT_PTR;


/* Macros for byte exchange between disk and host */
#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 8)
#define pmgr_htodl(p,x) \
                   ((p)[3] = (uchar) ((x) >> 24) , \
                    (p)[2] = (uchar) ((x) >> 16) , \
                    (p)[1] = (uchar) ((x) >>  8) , \
                    (p)[0] = (uchar) ((x)      )  \
                    )

#define pmgr_htods(p,x) \
                   ((p)[1] = (uchar) ((x) >>  8) , \
                    (p)[0] = (uchar) ((x)      )  \
                   )

#define pmgr_htodc(p,x) \
                   ((p)[0] = ((x)      ) , \
                    )

#define pmgr_dtohl(p)   \
                   ((((uint_32)(p)[3] ) << 24) | \
                    (((uint_32)(p)[2] ) << 16) | \
                    (((uint_32)(p)[1] ) <<  8) | \
                    (((uint_32)(p)[0] )      ))

#define pmgr_dtohs(p)  \
                  ((((uint_16)(p)[1] ) <<  8) | \
                   (((uint_16)(p)[0] )      ))

#define pmgr_dtohc(p)   ((((  uchar)(p)[0] )      ))
#else
#define pmgr_htodl(p,x) \
                   ((p)[3] = ((x) >> 24) & 0xFF, \
                    (p)[2] = ((x) >> 16) & 0xFF, \
                    (p)[1] = ((x) >>  8) & 0xFF, \
                    (p)[0] = ((x)      ) & 0xFF \
                    )

#define pmgr_htods(p,x) \
                   ((p)[1] = ((x) >>  8) & 0xFF, \
                    (p)[0] = ((x)      ) & 0xFF \
                   )

#define pmgr_htodc(p,x) \
                   ((p)[0] = ((x)      ) & 0xFF, \
                    )

#define pmgr_dtohl(p)   \
                   ((((uint_32)(p)[3] & 0xFF) << 24) | \
                    (((uint_32)(p)[2] & 0xFF) << 16) | \
                    (((uint_32)(p)[1] & 0xFF) <<  8) | \
                    (((uint_32)(p)[0] & 0xFF)      ))

#define pmgr_dtohs(p)  \
                  ((((uint_16)(p)[1] & 0xFF) <<  8) | \
                   (((uint_16)(p)[0] & 0xFF)      ))

#define pmgr_dtohc(p)   ((((  uchar)(p)[0] & 0xFF)      ))
#endif



#ifdef __cplusplus
extern "C" {
#endif

extern int_32
   _io_part_mgr_install(
      MQX_FILE_PTR                  dev_fd,
      char_ptr                      identifier,
      uint_32                       sector_size);

extern int_32
   _io_part_mgr_uninstall(
      char_ptr                      identifier);

extern int_32
   _io_part_mgr_open(
      MQX_FILE_PTR                  fd_ptr,
      char_ptr                      open_name_ptr,
      char_ptr                      flags_str);

extern int_32
   _io_part_mgr_close(
      MQX_FILE_PTR                  fd_ptr);

extern int_32
   _io_part_mgr_read(
      MQX_FILE_PTR                  file_ptr,
      char_ptr                      data_ptr,
      int_32                        num);

extern int_32
   _io_part_mgr_write(
      MQX_FILE_PTR                  file_ptr,
      char_ptr                      data_ptr,
      int_32                        num);

extern int_32
   _io_part_mgr_ioctl(
      MQX_FILE_PTR                  file_ptr,
      uint_32                       cmd,
      uint_32_ptr                   param_ptr);

extern int_32
    _pmgr_check_part_type(
        uint_32                     part_type);

extern int_32
   _pmgr_validate_mbr(
      char_ptr                      mbr_ptr);

extern int_32
   _pmgr_get_part_info(
      PART_MGR_STRUCT_PTR           part_mgr_ptr,
      PMGR_PART_INFO_STRUCT_PTR     host_entry);

extern int_32
   _pmgr_set_part_info(
      PART_MGR_STRUCT_PTR           part_mgr_ptr,
      PMGR_PART_INFO_STRUCT_PTR     host_entry);

extern int_32
   _pmgr_clear_part_info(
      PART_MGR_STRUCT_PTR           part_mgr_ptr,
      uchar                         part_num);

extern void
   _pmgr_disk_to_host(
      PMGR_PART_ENTRY_STRUCT_PTR    disk_entry,
      PMGR_PART_INFO_STRUCT_PTR     part_entry);

extern void
   _pmgr_host_to_disk(
      PMGR_PART_INFO_STRUCT_PTR     part_entry,
      PMGR_PART_ENTRY_STRUCT_PTR    disk_entry);


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
