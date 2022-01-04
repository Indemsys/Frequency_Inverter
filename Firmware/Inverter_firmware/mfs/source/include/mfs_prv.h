#ifndef __mfs_prv_h__
#define __mfs_prv_h__
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
* $FileName: mfs_prv.h$
* $Version : 3.8.26.0$
* $Date    : Aug-23-2012$
*
* Comments:
*
*   This file contains the structure definitions and constants
*   that are internal to the Embedded MS-DOS File System (MFS)
*
*END************************************************************************/

#include <partition.h>

/*
** Defines specific to MFS
*/

#define BOOT_SECTOR           0
#define FSINFO_SECTOR         1     /* FAT32 only */
#define BKBOOT_SECTOR         6     /* FAT32 only */

#define MFS_SECTOR_READ       5
#define MFS_SECTOR_WRITE      10

#define MFS_DOS30_JMP         0xEB /* 2 byte jmp 80x86 opcode */
#define MFS_DOS30_B           0xE9 /* 2 byte b 80x86 opcode */
#define MFS_DEL_FILE          0xE5 /* DOS byte used for a deleted file */

/* FAT 32 Filesystem info signatures */
#define FSI_LEADSIG           0x41615252UL
#define FSI_STRUCTSIG         0x61417272UL
#define FSI_TRAILSIG          0xAA550000UL
#define FSI_UNKNOWN           0xFFFFFFFFUL

/* This is the vesion of tfe FAT32 driver */
#define MFS_FAT32_VER         0x0000


#define MFS_LFN_END           0x40

#define ATTR_EXCLUSIVE        0x40
#define MFS_ATTR_ANY          0x80

/* defines used in FAT   */
#define CLUSTER_INVALID       0x00000001L
#define CLUSTER_UNUSED        0x00000000L
#define CLUSTER_MIN_GOOD      0x00000002L
#define CLUSTER_MAX_GOOD      0xFFFFFEFUL
#define CLUSTER_MIN_RESERVED  0xFFFFFF0UL
#define CLUSTER_MAX_RESERVED  0xFFFFFF6UL
#define CLUSTER_BAD           0xFFFFFF7UL
#define CLUSTER_MIN_LAST      0xFFFFFF8UL
#define CLUSTER_EOF           0xFFFFFFFUL
#define CLUSTER_MAX_12        0x0000FF8UL
#define CLUSTER_MAX_16        0x000FFF8UL
#define CLUSTER_MAX_32        0xFFFFFF8UL

#define MFS_VALID             0x4d465356UL

#ifndef min
    #define min(a,b) ((a<b)?a:b)
#endif

#define MFS_set_error_and_return(error_ptr, error, retval) { if (error_ptr != NULL) { *error_ptr = error;} return(retval ); }
#define ROOT_CLUSTER(drive_ptr) (drive_ptr->FAT_TYPE == MFS_FAT32 ? drive_ptr->BPB32.ROOT_CLUSTER : 0)
#define MFS_LOG(x)  // puts(x);


/* structure of a long file name slot */
typedef struct mfs_lname_enty
{
    uchar    ID;
    uchar    NAME0_4[10];
    uchar    ATTR;
    uchar    RESERVE;
    uchar    ALIAS_CHECKSUM;
    uchar    NAME5_10[12];
    uchar    START[2];
    uchar    NAME11_12[4];
} MFS_LNAME_ENTRY, _PTR_ MFS_LNAME_ENTRY_PTR;



/* structure of the BIOS Parameter Block maintained in the
** boot sector as stored in drive.
*/
typedef struct bios_param_struct
{
    char     RESERVED[11];
    uchar    RESERVED2;
    uint_16  SECTOR_SIZE;
    uchar    SECTORS_PER_CLUSTER;
    uchar    RESERVED3;
    uint_16  RESERVED_SECTORS;
    uchar    NUMBER_OF_FAT;
    uchar    RESERVED4;
    uint_16  ROOT_ENTRIES;
    uint_16  NUMBER_SECTORS;
    uchar    MEDIA_TYPE;
    uchar    RESERVED5;
    uint_16  SECTORS_PER_FAT;
    uint_16  SECTORS_PER_TRACK;
    uint_16  NUM_HEADS;
    uint_32  HIDDEN_SECTORS;
    uint_32  MEGA_SECTORS;
} BIOS_PARAM_STRUCT, _PTR_ BIOS_PARAM_STRUCT_PTR;

/* structure of the BIOS Parameter Block maintained in the
** boot sector as read off the disk
*/
typedef struct bios_param_struct_disk
{
    char     RESERVED[11];
    uchar    SECTOR_SIZE[2];
    uchar    SECTORS_PER_CLUSTER[1];
    uchar    RESERVED_SECTORS[2];
    uchar    NUMBER_OF_FAT[1];
    uchar    ROOT_ENTRIES[2];
    uchar    NUMBER_SECTORS[2];
    uchar    MEDIA_TYPE[1];
    uchar    SECTORS_PER_FAT[2];
    uchar    SECTORS_PER_TRACK[2];
    uchar    NUM_HEADS[2];
    uchar    HIDDEN_SECTORS[4];
    uchar    MEGA_SECTORS[4];
} BIOS_PARAM_STRUCT_DISK, _PTR_ BIOS_PARAM_STRUCT_DISK_PTR;

/* structure of the BIOS Parameter Block maintained in the
** boot sector as read off the disk (FAT32 extension)
*/
typedef struct bios_param32_struct_disk
{
    uchar    FAT_SIZE[4];
    uchar    EXT_FLAGS[2];
    uchar    FS_VER[2];
    uchar    ROOT_CLUSTER[4];
    uchar    FS_INFO[2];
    uchar    BK_BOOT_SEC[2];
    uchar    RESERVED[12];
} BIOS_PARAM32_STRUCT_DISK, _PTR_ BIOS_PARAM32_STRUCT_DISK_PTR;


typedef struct bios_param32_struct
{
    uint_32  FAT_SIZE;
    uint_16  EXT_FLAGS;
    uint_16  FS_VER;
    uint_32  ROOT_CLUSTER;
    uint_16  FS_INFO;
    uint_16  BK_BOOT_SEC;
    uchar    RESERVED[12];
} BIOS_PARAM32_STRUCT, _PTR_ BIOS_PARAM32_STRUCT_PTR;


/* The file system info struct. For FAT32 only */
typedef struct filesystem_info_disk
{
    uchar    LEAD_SIG[4];
    uchar    RESERVED1[480];
    uchar    STRUCT_SIG[4];
    uchar    FREE_COUNT[4];
    uchar    NEXT_FREE[4];
    uchar    RESERVED2[12];
    uchar    TRAIL_SIG[4];
} FILESYSTEM_INFO_DISK, _PTR_ FILESYSTEM_INFO_DISK_PTR;

typedef struct filesystem_info
{
    uint_32  FREE_COUNT;
    uint_32  NEXT_FREE;
} FILESYSTEM_INFO, FILESYSTEM_INFO_PTR;

/* configuration data for the MFS */
typedef struct mfs_drive_struct
{
    uint_32                    DRV_NUM;
    BIOS_PARAM_STRUCT          BPB;
    BIOS_PARAM32_STRUCT        BPB32;

    _mfs_cache_policy          WRITE_CACHE_POLICY;
    boolean                    DOS_DISK;
    boolean                    BLOCK_MODE;
    boolean                    READ_ONLY;

    uint_16                    SECTOR_POWER;
    uint_32                    CLUSTER_SIZE_BYTES;

    uint_16                    CLUSTER_POWER_BYTES;
    uint_16                    CLUSTER_POWER_SECTORS;

    uint_16                    RESERVED;
    uint_16                    ENTRIES_PER_SECTOR;

    uint_32                    FAT_START_SECTOR;
    uint_32                    ROOT_START_SECTOR;
    uint_32                    DATA_START_SECTOR;
    QUEUE_STRUCT               HANDLE_LIST;
    uint_32                    FAT_TYPE;
    uint_32                    CUR_DIR_CLUSTER;
    uint_32                    LAST_CLUSTER;
    char                       CURRENT_DIR[PATHNAME_SIZE + 1];
    uchar                      RESERVED2[3];
    LWSEM_STRUCT               SEM;
    _partition_id              HANDLE_PARTITION;
    MQX_FILE_PTR               DEV_FILE_PTR;
    MQX_FILE_PTR               MFS_FILE_PTR;

    boolean                    DIR_SECTOR_DIRTY;
    uint_32                    DIR_SECTOR_NUMBER;
    char_ptr                   DIR_SECTOR_PTR;

    _mfs_cache_policy          FAT_CACHE_POLICY;
    uint_32                    FAT_CACHE_START;
    uint_32                    FAT_CACHE_DIRTY;
    char_ptr                   FAT_CACHE_PTR;
    uint_32                    FAT_CACHE_SIZE;

    boolean                    DATA_SECTOR_DIRTY;
    uint_32                    DATA_SECTOR_NUMBER;
    char_ptr                   DATA_SECTOR_PTR;

    /* Count of free clusters. 0xFFFFFFFF means unknown. Must be recalculated */
    uint_32                    FREE_COUNT;
    /* Is set to the last allocated cluster. If = 0xFFFFFFF start search at 2 */
    uint_32                    NEXT_FREE_CLUSTER;

    boolean                    VERIFY_WRITES;
    uchar_ptr                  READBACK_SECTOR_PTR;

} MFS_DRIVE_STRUCT, _PTR_ MFS_DRIVE_STRUCT_PTR;

/*
** MFS Macro code
*/



/*
** CLUSTER_BOUNDARY ... takes a file offset and resets it to point to the
**                      first byte within the same cluster.
**       All this by masking off the least significant bits.
*/
#define  CLUSTER_BOUNDARY(x)   ((x) & (~(drive_ptr->CLUSTER_SIZE_BYTES - 1)))

/*
** The offset within the cluster is obtained by masking off the MSB's
*/
#define  OFFSET_WITHIN_CLUSTER(x)  ((x) & (drive_ptr->CLUSTER_SIZE_BYTES - 1))
#define  CLUSTER_OFFSET_TO_SECTOR(x)  (x>>drive_ptr->SECTOR_POWER)

#define  OFFSET_WITHIN_SECTOR(x)  ((x) & (drive_ptr->BPB.SECTOR_SIZE - 1))

/*
**  Returns the # of the first sector in the cluster.
*/
#define  CLUSTER_TO_SECTOR(x) ((x - CLUSTER_MIN_GOOD) *\
   drive_ptr->BPB.SECTORS_PER_CLUSTER + drive_ptr->DATA_START_SECTOR)

/*
** INDEX_TO_SECTOR converts an entry_index within the cluster or directory
** to the sector # in which that entry is found.
*/
#define INDEX_TO_SECTOR(x)  ((x) / drive_ptr->ENTRIES_PER_SECTOR)


/* Used to fix bug in date calculation */
#define NORMALIZE_DATE(c_ptr) \
   if ((c_ptr)->YEAR < 1980) (c_ptr)->YEAR = 1980

/* Macros for byte exchanges between host and disk */

/*
** This set of macros will always work on all processors.
** The sets of macros above are just optimizations for
** specific architectures.
*/
#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 8)

    #define htodl(p,x) ((p)[3] = (uchar) ((x) >> 24) , \
                    (p)[2] = (uchar)((x) >> 16) , \
                    (p)[1] = (uchar)((x) >>  8) , \
                    (p)[0] = (uchar)((x)      )  \
                   )

    #define htods(p,x) ((p)[1] = (uchar) ((x) >>  8) , \
                    (p)[0] = (uchar) ((x)      )   \
                   )
    #define htodc(p,x) ((p)[0] = ((x)      )  \
                   )

    #define dtohl(p)   ((((uint_32)(p)[3] ) << 24) | \
                    (((uint_32)(p)[2] ) << 16) | \
                    (((uint_32)(p)[1] ) <<  8) | \
                    (((uint_32)(p)[0] )      ))

    #define dtohs(p)   ((((uint_16)(p)[1] ) <<  8) | \
                    (((uint_16)(p)[0] )      ))

    #define dtohc(p)   ((((  uchar)(p)[0] )      ))



/* Special Macros for reading and writing the FAT */
    #define htof0(p,x)    ((p)[0]  = (uchar) (x)        , \
                       (p)[1] &=              0x000000F0L, \
                       (p)[1] |= (uchar)((x) >> 8) & 0x0000000FL  \
                      )

    #define htof1(p,x)    ((p)[0] &=               0x0000000FL, \
                       (p)[0] |= (uchar)((x) <<  4) & 0x000000F0L, \
                       (p)[1]  =(uchar) ((x) >>  4)   \
                      )

    #define ftoh0(p)      (((uint_32)((p)[0]       )       ) | \
                       ((uint_32)((p)[1] & 0x0F) <<  8 ))

    #define ftoh1(p)      (((uint_32)((p)[0] & 0xF0) >>  4 ) | \
                       ((uint_32)((p)[1]       ) <<  4 ))

    #define htof32(p,x)   ((p)[3] = (uchar)(((p)[3] & 0xF0) | (((x) >> 24) & 0x0F)), \
                       (p)[2] = (uchar)((x) >> 16) , \
                       (p)[1] = (uchar)((x) >>  8) , \
                       (p)[0] = (uchar)((x)      ) )
#else
/*
** This set of macros will always work on all processors.
** The sets of macros above are just optimizations for
** specific architectures.
*/
    #define htodl(p,x) ((p)[3] = ((x) >> 24) & 0xFF, \
                    (p)[2] = ((x) >> 16) & 0xFF, \
                    (p)[1] = ((x) >>  8) & 0xFF, \
                    (p)[0] = ((x)      ) & 0xFF  \
                   )

    #define htods(p,x) ((p)[1] = ((x) >>  8) & 0xFF, \
                    (p)[0] = ((x)      ) & 0xFF  \
                   )

    #define htodc(p,x) ((p)[0] = ((x)      ) & 0xFF \
                   )

    #define dtohl(p)   ((((uint_32)(p)[3] & 0xFF) << 24) | \
                    (((uint_32)(p)[2] & 0xFF) << 16) | \
                    (((uint_32)(p)[1] & 0xFF) <<  8) | \
                    (((uint_32)(p)[0] & 0xFF)      ))

    #define dtohs(p)   ((((uint_16)(p)[1] & 0xFF) <<  8) | \
                    (((uint_16)(p)[0] & 0xFF)      ))

    #define dtohc(p)   ((((  uchar)(p)[0] & 0xFF)      ))



/* Special Macros for reading and writing the FAT */

    #define htof0(p,x)    ((p)[0]  = (x)        & 0x000000FFL, \
                       (p)[1] &=              0x000000F0L, \
                       (p)[1] |= ((x) >> 8) & 0x0000000FL  \
                      )

    #define htof1(p,x)    ((p)[0] &=               0x0000000FL, \
                       (p)[0] |= ((x) <<  4) & 0x000000F0L, \
                       (p)[1]  = ((x) >>  4) & 0x000000FFL  \
                      )

    #define ftoh0(p)      (((uint_32)((p)[0]       )       & 0x000000FFL) | \
                       ((uint_32)((p)[1] & 0x0F) <<  8 & 0x0000FF00L))

    #define ftoh1(p)      (((uint_32)((p)[0] & 0xF0) >>  4 & 0x000000FFL) | \
                       ((uint_32)((p)[1]       ) <<  4 & 0x00000FF0L))

    #define htof32(p,x)   ((p)[3] = ((p)[3] & 0xF0) | (((x) >> 24) & 0x0F), \
                       (p)[2] = ((x) >> 16) & 0xFF, \
                       (p)[1] = ((x) >>  8) & 0xFF, \
                       (p)[0] = ((x)      ) & 0xFF)
#endif


/*
** Macros for getting/setting cluster, using a sperate high and low word
*/
#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 8)

    #define clustoh(high,low) \
  (((((uint_32)(low)[0]  )      )  | \
    (((uint_32)(low)[1]  ) <<  8)  | \
    (((uint_32)(high)[0] ) << 16)  | \
    (((uint_32)(high)[1] ) << 24)) & \
   ((drive_ptr->FAT_TYPE == MFS_FAT32) ? 0xFFFFFFFF : 0x0000FFFF))

    #define clustod(high,low,x)   \
   ((low)[0]  = (uchar) ((x)      ) , \
    (low)[1]  = (uchar) ((x) >>  8) , \
    (high)[0] = (uchar) ((x) >> 16) , \
    (high)[1] = (uchar) ((x) >> 24))
#else

    #define clustoh(high,low) \
  (((((uint_32)(low)[0]  & 0xFF)      )  | \
    (((uint_32)(low)[1]  & 0xFF) <<  8)  | \
    (((uint_32)(high)[0] & 0xFF) << 16)  | \
    (((uint_32)(high)[1] & 0xFF) << 24)) & \
   ((drive_ptr->FAT_TYPE == MFS_FAT32) ? 0xFFFFFFFF : 0x0000FFFF))

    #define clustod(high,low,x)   \
   ((low)[0]  = ((x)      ) & 0xFF, \
    (low)[1]  = ((x) >>  8) & 0xFF, \
    (high)[0] = ((x) >> 16) & 0xFF, \
    (high)[1] = ((x) >> 24) & 0xFF)
#endif


/*
** extern statements for MFS
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _mqx_str_h_
    extern uint_32 _strnlen(char_ptr, uint_32);
#endif

extern uint_32                _mfs_partition_destroy(_partition_id);
extern _mfs_error             _mfs_validate_device(MQX_FILE_PTR, uint_32_ptr, boolean _PTR_);
extern _mfs_error             MFS_Add_cluster_to_chain(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32_ptr);
extern boolean                MFS_Attribute_match(uchar, uchar);
extern uint_32                MFS_Bad_clusters(MQX_FILE_PTR);
extern uint_32                MFS_Change_current_dir(MQX_FILE_PTR, char_ptr);
extern uint_32                MFS_Check_dir_exist(MQX_FILE_PTR, char_ptr);
extern boolean                MFS_Check_search_string_for_8dot3(char_ptr);
extern boolean                MFS_Check_search_string_for_all(char_ptr);
extern uint_32                MFS_Clear_cluster(MFS_DRIVE_STRUCT_PTR, uint_32);
extern int_32                 MFS_Close_Device(MQX_FILE_PTR);
extern int_32                 MFS_Close_file(MQX_FILE_PTR);
extern void                   MFS_Compress_nondotfile(char_ptr, char_ptr);
extern MFS_DIR_ENTRY_PTR      MFS_Create_directory_entry(MFS_DRIVE_STRUCT_PTR, char_ptr, char, uint_32_ptr, uint_32_ptr, uint_32_ptr);
extern MFS_DIR_ENTRY_PTR      MFS_Create_entry_slave(MFS_DRIVE_STRUCT_PTR, uchar, char_ptr, uint_32_ptr, uint_32_ptr, uint_32_ptr, boolean);
extern pointer                MFS_Create_file(MQX_FILE_PTR, uchar, char_ptr, uint_32_ptr);
extern pointer                MFS_Create_new_file(MQX_FILE_PTR, uchar, char_ptr, uint_32_ptr);
extern uint_32                MFS_Create_subdir(MQX_FILE_PTR, char_ptr);
extern pointer                MFS_Create_temp_file(MQX_FILE_PTR, uchar, char_ptr, uint_32_ptr);
extern void                   MFS_Decrement_free_clusters(MFS_DRIVE_STRUCT_PTR);
extern _mfs_error             MFS_Default_Format(MQX_FILE_PTR);
extern uint_32                MFS_Delete_file(MQX_FILE_PTR, char_ptr);
extern int_32                 MFS_device_write_internal(MFS_DRIVE_STRUCT_PTR, uint_32, char_ptr, int_32_ptr, char_ptr _PTR_, int_32);
extern boolean                MFS_Dirname_valid(char_ptr);
extern void                   MFS_Expand_dotfile(char_ptr, char_ptr);
extern void                   MFS_Expand_wildcard(char_ptr, char_ptr);
extern _mfs_error             MFS_Extend_chain(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32, uint_32_ptr);
extern boolean                MFS_Filename_valid(char_ptr);
extern uint_32                MFS_Find_directory(MFS_DRIVE_STRUCT_PTR, char_ptr, uint_32);
extern MFS_DIR_ENTRY_PTR      MFS_Find_directory_entry(MFS_DRIVE_STRUCT_PTR, char_ptr, uint_32_ptr, uint_32_ptr, uint_32_ptr, uchar, uint_32_ptr);
extern MFS_DIR_ENTRY_PTR      MFS_Find_entry_on_disk(MFS_DRIVE_STRUCT_PTR, char_ptr, uint_32_ptr, uint_32_ptr, uint_32_ptr, uint_32_ptr);
extern uint_32                MFS_Find_first_file(MQX_FILE_PTR, uchar, char_ptr, MFS_SEARCH_DATA_PTR);
extern MFS_HANDLE_PTR         MFS_Find_handle(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32);
extern uint_32                MFS_Find_next_file(MQX_FILE_PTR, MFS_SEARCH_DATA_PTR);
extern uint_32                MFS_Find_next_slave(MFS_DRIVE_STRUCT_PTR, pointer);
extern uint_32                MFS_Find_unused_cluster_from(MFS_DRIVE_STRUCT_PTR, uint_32);
extern _mfs_error             MFS_Flush_caches(MFS_DRIVE_STRUCT_PTR);
extern uint_32                MFS_Flush_data_sector_buffer(MFS_DRIVE_STRUCT_PTR);
extern uint_32                MFS_Flush_directory_sector_buffer(MFS_DRIVE_STRUCT_PTR);
extern uint_32                MFS_Flush_fat_cache(MFS_DRIVE_STRUCT_PTR);
extern uint_32                MFS_Format(MQX_FILE_PTR, MFS_FORMAT_DATA_PTR);
extern uint_32                MFS_Format_and_test(MQX_FILE_PTR, MFS_FORMAT_DATA_PTR, uint_32_ptr);
extern void                   MFS_Free_handle(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR);
extern _mfs_error             MFS_get_cluster_from_fat(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32_ptr);
extern uint_32                MFS_Get_current_dir(MQX_FILE_PTR, char_ptr);
extern uint_32                MFS_Get_date_time(MQX_FILE_PTR, uint_16_ptr, uint_16_ptr);
extern uint_64                MFS_Get_disk_free_space(MQX_FILE_PTR);
extern uint_32                MFS_Get_disk_free_space_internal(MFS_DRIVE_STRUCT_PTR, uint_32_ptr);
extern uint_32                MFS_Get_file_attributes(MQX_FILE_PTR, char_ptr, uchar_ptr);
extern MFS_HANDLE_PTR         MFS_Get_handle(MFS_DRIVE_STRUCT_PTR, MFS_DIR_ENTRY_PTR);
extern uint_32                MFS_get_lfn(MFS_DRIVE_STRUCT_PTR, char_ptr, char_ptr);
extern uint_32                MFS_get_lfn_dir_cluster(MFS_DRIVE_STRUCT_PTR, pointer, char_ptr, char_ptr);
extern _mfs_error             MFS_get_lfn_of_entry(MFS_DRIVE_STRUCT_PTR, MFS_DIR_ENTRY_PTR, uint_32, uint_32, uint_32, char_ptr);
extern uint_32                MFS_get_prev_cluster(MFS_DRIVE_STRUCT_PTR, uint_32_ptr, uint_32);
extern uint_32                MFS_Get_volume(MQX_FILE_PTR, char_ptr);
extern uint_32                MFS_Increment_dir_index(MFS_DRIVE_STRUCT_PTR, uint_32_ptr, uint_32_ptr, uint_32_ptr);
extern void                   MFS_Increment_free_clusters(MFS_DRIVE_STRUCT_PTR);
extern void                   MFS_increment_lfn(char_ptr);
extern _mfs_error             MFS_Invalidate_data_sector(MFS_DRIVE_STRUCT_PTR);
extern _mfs_error             MFS_Invalidate_directory_sector(MFS_DRIVE_STRUCT_PTR);
extern uint_16                MFS_Is_dot_directory(char_ptr);
extern boolean                MFS_is_valid_lfn(char_ptr);
extern uint_32                MFS_Last_cluster(MQX_FILE_PTR);
extern uchar                  MFS_lfn_checksum(char_ptr);
extern boolean                MFS_lfn_dirname_valid(char_ptr);
extern void                   MFS_lfn_extract(MFS_LNAME_ENTRY_PTR, char_ptr);
extern boolean                MFS_lfn_match(char_ptr, char_ptr, uint_32) ;
extern uint_32                MFS_lfn_name_to_entry(char_ptr, MFS_LNAME_ENTRY_PTR);
extern uint_32                MFS_lfn_to_sfn(char_ptr, char_ptr);
extern uint_32                MFS_lock(MQX_FILE_PTR, MFS_DRIVE_STRUCT_PTR _PTR_);
extern uint_32                MFS_lock_dos_disk(MQX_FILE_PTR, MFS_DRIVE_STRUCT_PTR _PTR_);
extern uint_32                MFS_lock_handle(MQX_FILE_PTR, MFS_DRIVE_STRUCT_PTR _PTR_, MFS_HANDLE_PTR _PTR_);
extern uint_32                MFS_Mount_drive_internal(MFS_DRIVE_STRUCT_PTR);
extern uint_32                MFS_Move_file_pointer(MQX_FILE_PTR, uint_32_ptr);
extern _mfs_error             MFS_next_data_sector(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, uint_32_ptr, uint_32_ptr);
extern int_32                 MFS_Open_Device(MQX_FILE_PTR, MFS_DRIVE_STRUCT_PTR);
extern pointer                MFS_Open_file(MQX_FILE_PTR, char_ptr, uchar, uint_32_ptr);
extern char_ptr               MFS_Parse_next_filename(char_ptr, char_ptr);
extern char_ptr               MFS_Parse_Out_Device_Name(char_ptr);
extern uint_32                MFS_Parse_pathname(char_ptr, char_ptr, char_ptr);
extern uint_32                MFS_Put_fat(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32);
extern uint_32                MFS_Read(MQX_FILE_PTR, uint_32, char_ptr, uint_32_ptr);
extern _mfs_error             MFS_Read_data_sector(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, uint_32, boolean);
extern _mfs_error             MFS_Read_device_sectors(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32, uint_32, char_ptr, uint_32_ptr);
extern _mfs_error             MFS_Read_device_sector(MFS_DRIVE_STRUCT_PTR, uint_32, char_ptr);
extern pointer                MFS_Read_directory_sector(MFS_DRIVE_STRUCT_PTR, uint_32, uint_16, uint_32_ptr);
extern uint_32                MFS_Read_fat(MFS_DRIVE_STRUCT_PTR, uint_32);
extern _mfs_error             MFS_Release_chain(MFS_DRIVE_STRUCT_PTR, uint_32);
extern uint_32                MFS_remove_lfn_entries(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32, uint_32);
extern uint_32                MFS_Remove_subdir(MQX_FILE_PTR, char_ptr);
extern uint_32                MFS_Rename_file(MQX_FILE_PTR, char_ptr, char_ptr);
extern uint_32                MFS_Set_date_time(MQX_FILE_PTR, uint_16_ptr, uint_16_ptr);
extern uint_32                MFS_Set_file_attributes(MQX_FILE_PTR, char_ptr, uchar_ptr);
extern uint_32                MFS_Set_volume(MQX_FILE_PTR, char_ptr);
extern boolean                MFS_strcmp(char_ptr, char_ptr, uint_32);
extern void                   MFS_strupr(char_ptr);
extern uint_32                MFS_Test_unused_clusters(MQX_FILE_PTR, uint_32_ptr);

_mfs_error MFS_unlock(MFS_DRIVE_STRUCT_PTR, boolean);

extern uint_32                MFS_Update_entry(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR);
extern boolean                MFS_Wildcard_match(char_ptr, char_ptr);
extern uint_32                MFS_Write(MQX_FILE_PTR, uint_32, char_ptr, uint_32_ptr);
extern _mfs_error             MFS_Write_back_fat(MFS_DRIVE_STRUCT_PTR);
extern uint_32                MFS_Write_back_fat(MFS_DRIVE_STRUCT_PTR);
extern _mfs_error             MFS_Write_device_sectors(MFS_DRIVE_STRUCT_PTR, uint_32, uint_32, uint_32, char_ptr, uint_32_ptr);
extern _mfs_error             MFS_Write_device_sector(MFS_DRIVE_STRUCT_PTR, uint_32, char_ptr);
extern boolean                MFS_is_read_only(MQX_FILE_PTR, MFS_DRIVE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif
/* EOF */
