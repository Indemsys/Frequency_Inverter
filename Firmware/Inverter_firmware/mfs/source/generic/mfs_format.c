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
* $FileName: mfs_format.c$
* $Version : 3.8.14.0$
* $Date    : Jun-27-2012$
*
* Comments:
*
*   This file contains drive specific interface functions related to 
*   formating the drive. 
*
*END*********************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"


#if !MFSCFG_READ_ONLY
    #ifdef MFSCFG_ENABLE_FORMAT


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Format
* Returned Value   : error code
* Comments  :   Perform a high-level DOS format.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Format
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] the MFS device on which to operate */
    MFS_FORMAT_DATA_PTR     format_ptr  /*[IN] information about the disk to format */
    )
{
    MFS_DRIVE_STRUCT_PTR       drive_ptr;
    FILESYSTEM_INFO_DISK_PTR   fsinfo_ptr;
    uchar_ptr                  boot_sector;
    _mfs_error                 error_code;
    uint_32                    fat_type;
    uint_32                    i;
    uint_32                    temp1;
    uint_32                    temp2;
    uint_32                    num_sectors;
    uint_32                    fat_size;
    uint_32                    rootdir_size;
    uint_32                    sys_size;
    int_32                     fat_count;
    uchar                      cluster_size;
    char                       version_str[6];

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error_code = MFS_lock( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

    if ( !_queue_is_empty(&drive_ptr->HANDLE_LIST) )
    {
        MFS_unlock(drive_ptr,FALSE);
        return MFS_SHARING_VIOLATION;
    }

    error_code = MFS_NO_ERROR;

    /*
    ** We are reformatting the disk, so mark it as not accessible. 
    ** ie: it's not a dos disk during the format.
    */
    drive_ptr->DOS_DISK = FALSE;

    MFS_Invalidate_directory_sector(drive_ptr);

    boot_sector = (uchar_ptr) drive_ptr->DIR_SECTOR_PTR;

    if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND1 )
    {
        fat_type = MFS_FAT12;
        rootdir_size = 7;
        cluster_size = 1;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND2 )
    {
        fat_type = MFS_FAT12;
        rootdir_size = 14;
        cluster_size = 1;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND3 )
    {
        fat_type = MFS_FAT12;
        rootdir_size = 32;
        cluster_size = 2;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND4 )
    {
        fat_type = MFS_FAT12;
        rootdir_size = 32;
        cluster_size = 4;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND5 )
    {
        fat_type = MFS_FAT12;
        rootdir_size = 32;
        cluster_size = 8;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND6 )
    {
        fat_type = MFS_FAT16;
        rootdir_size = 32;
        cluster_size = 8;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND7 )
    {
        fat_type = MFS_FAT16;
        rootdir_size = 32;
        cluster_size = 16;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND11 )
    {
        fat_type = MFS_FAT16;
        rootdir_size = 32;
        cluster_size = 32;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND8 )
    {
        fat_type = MFS_FAT32;
        rootdir_size = 0;
        cluster_size = 8;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND9 )
    {
        fat_type = MFS_FAT32;
        rootdir_size = 0;
        cluster_size = 16;
    }
    else if ( format_ptr->NUMBER_OF_SECTORS < SECTOR_BOUND10 )
    {
        fat_type = MFS_FAT32;
        rootdir_size = 0;
        cluster_size = 32;
    }
    else
    {
        fat_type = MFS_FAT32;
        rootdir_size = 0;
        cluster_size = 64;
    }  

    if ( (fat_type == MFS_FAT32) && (format_ptr->RESERVED_SECTORS < 32) )
    {
        MFS_unlock(drive_ptr,FALSE);
        return(MFS_INVALID_PARAMETER);
    }

    /* 
    ** Make sure the format parameters are big enoughh to initialize file system
    */
    if ( format_ptr->NUMBER_OF_SECTORS <= (format_ptr->RESERVED_SECTORS + rootdir_size) )
    {
        MFS_unlock(drive_ptr,FALSE);
        return(MFS_INVALID_PARAMETER);
    }


    drive_ptr->BPB.MEGA_SECTORS = format_ptr->NUMBER_OF_SECTORS;

    num_sectors = format_ptr->NUMBER_OF_SECTORS - format_ptr->RESERVED_SECTORS - rootdir_size;


    /*
    ** The formula is:
    **
    **                  / cluster_size * sector_size     \
    **   num_sectors = (  -------------------------- + 2  ) * fat_size
    **                  \         (1.5 or 2)             /
    **
    */
    if ( fat_type == MFS_FAT12 )
    {
        i = 2*format_ptr->BYTES_PER_SECTOR*cluster_size + 6;
        fat_size = 1 + (3*num_sectors - 1) / i;
    }
    else if ( fat_type == MFS_FAT16 )
    {
        i = format_ptr->BYTES_PER_SECTOR*cluster_size/2 + 2;
        fat_size = 1 + (num_sectors - 1) / i;
    }
    else
    {
        temp1 = format_ptr->NUMBER_OF_SECTORS - format_ptr->RESERVED_SECTORS;
        temp2 = (256 * cluster_size + MFSCFG_NUM_OF_FATS) / 2;
        fat_size = (temp1 + temp2 - 1) /temp2;
    } 

    _mem_zero( boot_sector, format_ptr->BYTES_PER_SECTOR );
    sys_size = rootdir_size+MFSCFG_NUM_OF_FATS*fat_size+format_ptr->RESERVED_SECTORS;
    for ( i=format_ptr->RESERVED_SECTORS+1; i < sys_size; i++ )
    {
        error_code = MFS_Write_device_sector(drive_ptr, i,(char_ptr)boot_sector);
        if ( error_code )
        {
            MFS_unlock(drive_ptr,TRUE);
            return(error_code);
        }
    }  


    /* If we have a fat32 drive, we must clear the root dir cluster */
    if ( fat_type == MFS_FAT32 )
    {
        for ( i = 0 ; i < cluster_size && !error_code; i++ )
        {
            error_code = MFS_Write_device_sector(drive_ptr, sys_size + i, (char_ptr)boot_sector);
        }  
        if ( error_code )
        {
            MFS_unlock(drive_ptr,TRUE);
            return( error_code );
        }
    }


    /* now write the first sector of each FAT */
    boot_sector[0] = format_ptr->MEDIA_DESCRIPTOR;
    boot_sector[1] = 0xFF;
    boot_sector[2] = 0xFF;

    if ( fat_type == MFS_FAT16 )
    {
        boot_sector[3] = 0xFF;
    }

    if ( fat_type == MFS_FAT32 )
    {
        boot_sector[3] = 0x0F;
        boot_sector[4] = 0xFF;
        boot_sector[5] = 0xFF;
        boot_sector[6] = 0xFF;
        boot_sector[7] = 0x0F;

        /* First cluster of root entry */
        boot_sector[8] = 0xFF;
        boot_sector[9] = 0xFF;
        boot_sector[10] = 0xFF;
        boot_sector[11] = 0x0F;
    }

    fat_count = MFSCFG_NUM_OF_FATS;
    while ( --fat_count >= 0 )
    {
        error_code = MFS_Write_device_sector(drive_ptr, format_ptr->RESERVED_SECTORS + (uint_32) fat_count * fat_size, (char_ptr)boot_sector);

        if ( error_code )
        {
            MFS_unlock(drive_ptr,TRUE);
            return(error_code);
        }
    } /* EndWhile */

    boot_sector[0] = 0xEB;    /* jmp boot_sector[62] */
    boot_sector[1] = 0x3C;
    boot_sector[2] = 0x90;    /* nop */

    /* Write MFS and version number as OEM name */
    sprintf(version_str,"%x",MFS_VERSION);
    _mem_copy("MFS", boot_sector+3, 3);
    _mem_copy(version_str, boot_sector+6, 5);

    boot_sector[11] = (uchar) ((format_ptr->BYTES_PER_SECTOR)      & 0xFF);
    boot_sector[12] = (uchar) ((format_ptr->BYTES_PER_SECTOR >> 8) & 0xFF);

    boot_sector[13] = cluster_size;

    boot_sector[14] = (uchar) ((format_ptr->RESERVED_SECTORS)      & 0xFF);
    boot_sector[15] = (uchar) ((format_ptr->RESERVED_SECTORS >> 8) & 0xFF);

    boot_sector[16] = MFSCFG_NUM_OF_FATS;      /* number of FATs */

    rootdir_size *= format_ptr->BYTES_PER_SECTOR >> 5;
    boot_sector[17] = (uchar) ((rootdir_size)      & 0xFF);
    boot_sector[18] = (uchar) ((rootdir_size >> 8) & 0xFF);

    if ( (format_ptr->NUMBER_OF_SECTORS+format_ptr->HIDDEN_SECTORS) <= MAX_UINT_16 )
    {
        boot_sector[19] = (uchar) ((format_ptr->NUMBER_OF_SECTORS)      & 0xFF);
        boot_sector[20] = (uchar) ((format_ptr->NUMBER_OF_SECTORS >> 8) & 0xFF);
    }
    else
    {
        /* boot_sector[19..20] should already be 0 */
        boot_sector[32] = (uchar) ((format_ptr->NUMBER_OF_SECTORS)       & 0xFF);
        boot_sector[33] = (uchar) ((format_ptr->NUMBER_OF_SECTORS >>  8) & 0xFF);
        boot_sector[34] = (uchar) ((format_ptr->NUMBER_OF_SECTORS >> 16) & 0xFF);
        boot_sector[35] = (uchar) ((format_ptr->NUMBER_OF_SECTORS >> 24) & 0xFF);
    }  

    boot_sector[21] = format_ptr->MEDIA_DESCRIPTOR;

    if ( fat_type != MFS_FAT32 )
    {
        boot_sector[22] = (uchar) ((fat_size)       & 0xFF);
        boot_sector[23] = (uchar) ((fat_size >> 8)  & 0xFF);
    }
    else
    {
        boot_sector[36] = (uchar) ((fat_size)       & 0xFF);
        boot_sector[37] = (uchar) ((fat_size >> 8)  & 0xFF);
        boot_sector[38] = (uchar) ((fat_size >> 16) & 0xFF);
        boot_sector[39] = (uchar) ((fat_size >> 24) & 0xFF);      
    }

    boot_sector[24] = (uchar) ((format_ptr->SECTORS_PER_TRACK)      & 0xFF);
    boot_sector[25] = (uchar) ((format_ptr->SECTORS_PER_TRACK >> 8) & 0xFF);

    boot_sector[26] = (uchar) ((format_ptr->NUMBER_OF_HEADS)      & 0xFF);
    boot_sector[27] = (uchar) ((format_ptr->NUMBER_OF_HEADS >> 8) & 0xFF);

    boot_sector[28] = (uchar) ((format_ptr->HIDDEN_SECTORS)       & 0xFF);
    boot_sector[29] = (uchar) ((format_ptr->HIDDEN_SECTORS >>  8) & 0xFF);
    boot_sector[30] = (uchar) ((format_ptr->HIDDEN_SECTORS >> 16) & 0xFF);
    boot_sector[31] = (uchar) ((format_ptr->HIDDEN_SECTORS >> 24) & 0xFF);

    if ( fat_type == MFS_FAT32 )
    {
        /* fields 40 and 41 are for FAT mirroring */
        boot_sector[40] = 0x00;    
        boot_sector[41] = 0x00;   

        boot_sector[42] = (uchar) ((MFS_FAT32_VER) & 0xFF); /* Minor FAT 32 rev */
        boot_sector[43] = (uchar) ((MFS_FAT32_VER >> 8) & 0xFF); /* Major rev  */

        /* Root Cluster for FAT32... cluster 2 is the standard */
        boot_sector[44] = 0x02;    
        boot_sector[45] = 0x00;
        boot_sector[46] = 0x00;
        boot_sector[47] = 0x00;

        /* BPB_FSINFO struct...  The standard is sector 1 */
        boot_sector[48] = (uchar) ((FSINFO_SECTOR)      & 0xFF);
        boot_sector[49] = (uchar) ((FSINFO_SECTOR >> 8) & 0xFF);

        /* Back up boot sector... The standard is sector 6  */
        boot_sector[50] = (uchar) ((BKBOOT_SECTOR)      & 0xFF);
        boot_sector[51] = (uchar) ((BKBOOT_SECTOR >> 8) & 0xFF);

        /* Fields 52 to 63 are reserved. They are already zeroed */

        boot_sector[64] = format_ptr->PHYSICAL_DRIVE;
        boot_sector[66] = 0x29;  /* Extended boot signature */

        /* Volume label and ID */       
        _mem_copy("NO NAME    FAT32   ", boot_sector+71, 19);  
    }
    else
    {

        boot_sector[36] = format_ptr->PHYSICAL_DRIVE;

        boot_sector[38] = 0x29;  /* Extended boot signature */

        if ( fat_type == MFS_FAT12 )
        {
            /* Volume label and ID */
            _mem_copy("NO NAME    FAT12   ", boot_sector+43, 19);  
        }
        else if ( fat_type == MFS_FAT16 )
        {
            /* Volume label and ID */
            _mem_copy("NO NAME    FAT16   ", boot_sector+43, 19);  
        }

        boot_sector[62] = 0x33;  /* xor ax, ax */
        boot_sector[63] = 0xC0;
        boot_sector[64] = 0x8E;  /* mov ss, ax */
        boot_sector[65] = 0xD0;
        boot_sector[66] = 0xBC;  /* mov sp, 7C00 */
        boot_sector[67] = 0x00;
        boot_sector[68] = 0x7C;
        boot_sector[69] = 0xFC;  /* cld */
        boot_sector[70] = 0xE8;  /* call boot_sector[118] */
        boot_sector[71] = 45;
        boot_sector[72] = 0;

        _mem_copy("\r\nNon-System disk\r\nPress any key to reboot\r\n", boot_sector+73, 45);

        boot_sector[118] = 0x5E; /* pop si */
        boot_sector[119] = 0xEB; /* jmp boot_sector[123] */
        boot_sector[120] = 0x02;
        boot_sector[121] = 0xCD; /* int 10 */
        boot_sector[122] = 0x10;
        boot_sector[123] = 0xB4; /* mov ah, 0E */
        boot_sector[124] = 0x0E;
        boot_sector[125] = 0xBB; /* mov bx, 0007 */
        boot_sector[126] = 0x07;
        boot_sector[127] = 0x00;
        boot_sector[128] = 0x2E; /* cs:lodsb */
        boot_sector[129] = 0xAC;
        boot_sector[130] = 0x84; /* test al, al */
        boot_sector[131] = 0xC0;
        boot_sector[132] = 0x75; /* jnz boot_sector[121] */
        boot_sector[133] = 0xF3;
        boot_sector[134] = 0x98; /* cbw */
        boot_sector[135] = 0xCD; /* int 16 */
        boot_sector[136] = 0x16;
        boot_sector[137] = 0xCD; /* int 19 */
        boot_sector[138] = 0x19;
        boot_sector[139] = 0xEB; /* jmp boot_sector[62] */
        boot_sector[140] = 0xB1;
    }
    boot_sector[format_ptr->BYTES_PER_SECTOR-2] = 0x55;
    boot_sector[format_ptr->BYTES_PER_SECTOR-1] = 0xAA;

    error_code = MFS_Write_device_sector(drive_ptr, BOOT_SECTOR, (char_ptr)boot_sector);

    if ( (error_code == MFS_NO_ERROR) && ( fat_type == MFS_FAT32) )
    {
        error_code = MFS_Write_device_sector(drive_ptr, BKBOOT_SECTOR, (char_ptr)boot_sector);

        if ( error_code == MFS_NO_ERROR )
        {
             fsinfo_ptr = (FILESYSTEM_INFO_DISK_PTR) drive_ptr->DIR_SECTOR_PTR;
             htodl(fsinfo_ptr->LEAD_SIG, FSI_LEADSIG);
             htodl(fsinfo_ptr->STRUCT_SIG, FSI_STRUCTSIG);
             htodl(fsinfo_ptr->FREE_COUNT, 0xFFFFFFFF);   /* compute it */
             htodl(fsinfo_ptr->NEXT_FREE, 0xFFFFFFFF);  /* compute it */
             htodl(fsinfo_ptr->TRAIL_SIG, FSI_TRAILSIG);

             error_code = MFS_Write_device_sector(drive_ptr, FSINFO_SECTOR, (char_ptr)fsinfo_ptr);

             if ( error_code == MFS_NO_ERROR )
             {
                 error_code = MFS_Read_device_sector (drive_ptr, 0, drive_ptr->DIR_SECTOR_PTR);
             }             
        }
    }

    drive_ptr->DIR_SECTOR_NUMBER = 0;
    drive_ptr->DIR_SECTOR_DIRTY  = FALSE;

    if ( error_code == MFS_NO_ERROR )
    {
        MFS_Mount_drive_internal( drive_ptr );
    }

    /* Calculate the free space on disk */
    if ( error_code == MFS_NO_ERROR )
    {
        MFS_Get_disk_free_space_internal(drive_ptr,&error_code);
    }
    MFS_unlock(drive_ptr,FALSE);

    return(error_code);
}  

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Default_Format
* Returned Value   : error code
* Comments  :   Perform a high-level DOS format with default values.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Default_Format
    (
    MQX_FILE_PTR            mfs_fd_ptr /*[IN] the MFS device on which to operate */
    )
{
    MFS_DRIVE_STRUCT_PTR   drive_ptr;
    MFS_FORMAT_DATA        format_data;
    uint_32                sector_size, num_sectors, reserved_sectors;
    _mfs_error             error_code;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif
    
    /* lock the drive */
    error_code = MFS_lock( mfs_fd_ptr, &drive_ptr );
    if (error_code != MFS_NO_ERROR) {
        return error_code;
    }
    
    /* get the device sector size */
    error_code  = _io_ioctl(drive_ptr->DEV_FILE_PTR, IO_IOCTL_GET_BLOCK_SIZE, &sector_size);
    /* get the number of sectors */
    if (error_code == MFS_NO_ERROR) {
        error_code  = _io_ioctl(drive_ptr->DEV_FILE_PTR, IO_IOCTL_GET_NUM_SECTORS, &num_sectors);
    }
    /* unlock the drive */
    MFS_unlock(drive_ptr,FALSE);

    if (error_code == MFS_NO_ERROR) {
        /* set reserved sectors. For FAT12, FAT16 should be 1 */
        /* for FAT32 usualy 32 */
        if (num_sectors < SECTOR_BOUND11) {
            reserved_sectors = 1;
        } else {
            reserved_sectors = 32;
        }
        /* fill the MFS_FORMAT_DATA structure. This values are used in format command */
        /* this is the default settings */
        format_data.PHYSICAL_DRIVE    = 0x80;
        format_data.MEDIA_DESCRIPTOR  = 0xf8;
        format_data.BYTES_PER_SECTOR  = sector_size;
        format_data.SECTORS_PER_TRACK = 0x00;
        format_data.NUMBER_OF_HEADS   = 0x00;
        format_data.NUMBER_OF_SECTORS = num_sectors - reserved_sectors;
        format_data.HIDDEN_SECTORS    = 0;
        format_data.RESERVED_SECTORS  = reserved_sectors;
    
        /* format drive */
        error_code = MFS_Format(mfs_fd_ptr, &format_data);
    }
    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Test_unused_clusters
* Returned Value   : error code
* Comments  :   Perform a sector test.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Test_unused_clusters
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] the MFS device on which to operate */
    uint_32_ptr             count_ptr   /*[IN/OUT] number of bad clusters detected */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    uchar_ptr               cluster_buffer;
    uint_32                 cluster_status;
    uint_32                 cluster_num;
    uint_32                 sector_num;
    _mfs_error              error_code;
    uint_32                 io_error;

#if MFSCFG_READ_ONLY_CHECK
    if ( MFS_is_read_only(mfs_fd_ptr, NULL) )
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error_code = MFS_lock_dos_disk(mfs_fd_ptr, &drive_ptr);
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

    cluster_buffer = MFS_mem_alloc_system_zero(drive_ptr->CLUSTER_SIZE_BYTES);

    if ( cluster_buffer == NULL )
    {
        error_code = MFS_INSUFFICIENT_MEMORY;
        MFS_unlock(drive_ptr, FALSE);
        return error_code;
    }
    _mem_set_type(cluster_buffer,MEM_TYPE_MFS_CLUSTER);

    *count_ptr = 0;

    cluster_num = CLUSTER_MIN_GOOD;
    sector_num = drive_ptr->DATA_START_SECTOR;

    while ( cluster_num<=drive_ptr->LAST_CLUSTER && error_code==MFS_NO_ERROR )
    {
        /*
        ** Test all UNUSED clusters (after a format, all clusters are free)
        */

        error_code = MFS_get_cluster_from_fat(drive_ptr, cluster_num, &cluster_status);

        if ( error_code==MFS_NO_ERROR && cluster_status==CLUSTER_UNUSED )
        {
            /* Check the cluster by performing write with no retries */
            io_error = MFS_Write_device_sectors(drive_ptr, sector_num, drive_ptr->BPB.SECTORS_PER_CLUSTER, 0, (char_ptr)cluster_buffer, NULL);

            switch ( io_error )
            {
                case MFS_WRITE_FAULT:
                case MFS_READ_FAULT:
                case MFS_SECTOR_NOT_FOUND:
                    error_code = MFS_Put_fat(drive_ptr, cluster_num, CLUSTER_BAD);
                    (*count_ptr)++;
                    break;
                case MFS_DISK_IS_WRITE_PROTECTED:
                    error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    break;
            }
        }

        cluster_num++;
        sector_num += drive_ptr->BPB.SECTORS_PER_CLUSTER;
    }  

    if ( error_code==MFS_NO_ERROR )
    {
        error_code = MFS_Write_back_fat(drive_ptr);
    }

    _mem_free(cluster_buffer);
    MFS_unlock(drive_ptr,TRUE);

    return error_code;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Format_and_test
* Returned Value   : error code
* Comments  :   Perform a high-level DOS format.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Format_and_test
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] logical number of the storage medium for the file system */
    MFS_FORMAT_DATA_PTR     format_ptr, /*[IN] information about the disk to format */
    uint_32_ptr             count_ptr   /*[IN/OUT] number of bad clusters detected */
    )
{
    _mfs_error        error_code;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error_code = MFS_Format( mfs_fd_ptr, format_ptr );

    if ( !error_code )
    {
        error_code = MFS_Test_unused_clusters( mfs_fd_ptr, count_ptr );
    }

    return(error_code);
}  

    #endif
#endif

