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
* $FileName: mfs_write.c$
* $Version : 3.8.13.0$
* $Date    : Jun-27-2012$
*
* Comments:
*
*   This file contains the file specific interface functions
*   of the MFS.
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"



#if !MFSCFG_READ_ONLY

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Write
* Returned Value   : number of bytes writen
* Comments  :   Write a specific number of bytes to an open file.
*
*END*---------------------------------------------------------------------*/

uint_32  MFS_Write
    (
    MQX_FILE_PTR            fd_ptr,         /*[IN] handle to the open file */
    uint_32                 num_bytes,      /*[IN] number of bytes to be written */
    char_ptr                buffer_address, /*[IN/OUT] bytes are written from this buffer */
    _mfs_error_ptr          error_ptr       /*[IN/OUT] error code is written to this address */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_HANDLE_PTR          handle_ptr;
    uint_32                 bytes_written;
    uint_32                 copy_size;
    uint_32                 cluster_offset;
    uint_32                 sector_number, sector_index;
    uint_32                 sector_offset;
    uint_32                 whole_sectors;
    uint_32                 cont_sectors;
    uint_32                 proc_sectors;
    _mfs_error              error, temp_error;
    uint_32                 file_size;
    uint_32                 next_cluster;
    boolean                 need_hwread;
    uint_32                 location;
    uint_32                 num_zeros;
    uint_32                 zeros_written;
    uint_32                 zero_size;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (fd_ptr, NULL))
    {
        MFS_set_error_and_return(error_ptr,MFS_DISK_IS_WRITE_PROTECTED,0);
    }
#endif

    if ( buffer_address == NULL )
    {
        MFS_set_error_and_return(error_ptr,MFS_INVALID_PARAMETER,0);
    }

    if ( num_bytes == 0 )
    {
        MFS_set_error_and_return(error_ptr,MFS_NO_ERROR,0);
    }

    error = MFS_lock_handle( fd_ptr, &drive_ptr, &handle_ptr );
    if ( error != MFS_NO_ERROR )
    {
        MFS_set_error_and_return(error_ptr,error,0);
    }

    if ( handle_ptr->ACCESS == MFS_ACCESS_READ_ONLY )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_ACCESS_DENIED,0);
    }

    /*
    ** Setup the current cluster. If this is the first time writing to the file, a cluster needs to be added.
    */
    if ( handle_ptr->CURRENT_CLUSTER == 0 )
    {
        handle_ptr->PREVIOUS_CLUSTER = 0;
        handle_ptr->CURRENT_CLUSTER = clustoh(handle_ptr->DIR_ENTRY.HFIRST_CLUSTER, handle_ptr->DIR_ENTRY.LFIRST_CLUSTER);
        if ( handle_ptr->CURRENT_CLUSTER==0 )
        {
            next_cluster = MFS_Find_unused_cluster_from(drive_ptr,drive_ptr->NEXT_FREE_CLUSTER);
            if ( next_cluster != CLUSTER_INVALID )
            {
                clustod(handle_ptr->DIR_ENTRY.HFIRST_CLUSTER, handle_ptr->DIR_ENTRY.LFIRST_CLUSTER, next_cluster);
                handle_ptr->TOUCHED = 1;

                error = MFS_Put_fat(drive_ptr, next_cluster, CLUSTER_EOF);
                if ( error == MFS_NO_ERROR )
                {
                    handle_ptr->CURRENT_CLUSTER = next_cluster;
                }
                else
                {
                    MFS_unlock(drive_ptr,FALSE);
                    MFS_set_error_and_return(error_ptr,error,0);
                }
            }
            else
            {
                MFS_unlock(drive_ptr,FALSE);
                MFS_set_error_and_return(error_ptr,MFS_DISK_FULL,0);
            }
        }
    }
    else if ( handle_ptr->CURRENT_CLUSTER == CLUSTER_EOF )
    {
        error = MFS_Add_cluster_to_chain(drive_ptr, handle_ptr->PREVIOUS_CLUSTER, &handle_ptr->CURRENT_CLUSTER);
        if ( MFS_NO_ERROR != error )
        {
            MFS_unlock(drive_ptr,FALSE);
            MFS_set_error_and_return(error_ptr,error,0);
        }
    }
    else if ( handle_ptr->CURRENT_CLUSTER > drive_ptr->LAST_CLUSTER )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_DISK_FULL,0);
    }

    /* Make sure location (local variable) never points behind the end of file */
    file_size = dtohl(handle_ptr->DIR_ENTRY.FILE_SIZE);
    location = (fd_ptr->LOCATION > file_size) ? file_size : fd_ptr->LOCATION;

    /* Calculate sector number and offsets within cluster and sector */
    cluster_offset = OFFSET_WITHIN_CLUSTER(location);
    sector_index = CLUSTER_OFFSET_TO_SECTOR(cluster_offset);
    sector_number = CLUSTER_TO_SECTOR(handle_ptr->CURRENT_CLUSTER) + sector_index;
    sector_offset = OFFSET_WITHIN_SECTOR(location);

    /* Calculate possible gap to fill in by zeros if writing behind the end of file */
    num_zeros = fd_ptr->LOCATION - location;

    zeros_written = 0;
    bytes_written = 0;

    /* Write zeros to fill in gap if LOCATION points behind the end of file */
    while (zeros_written < num_zeros)
    {
        /* If offset is non-zero, then reading the data is required */
        error = MFS_Read_data_sector(drive_ptr, handle_ptr, sector_number, sector_offset != 0);
        if ( error != MFS_NO_ERROR )
            break;

        /* Zero the buffer  */
        zero_size = min(num_zeros-zeros_written, drive_ptr->BPB.SECTOR_SIZE-sector_offset);
        _mem_zero(&drive_ptr->DATA_SECTOR_PTR[sector_offset], zero_size);
        drive_ptr->DATA_SECTOR_DIRTY = TRUE;

        if ( drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE )
        {
            error = MFS_Flush_data_sector_buffer(drive_ptr);
            if ( error != MFS_NO_ERROR )
                break;
        }

        zeros_written += zero_size;
        sector_offset += zero_size;

        /*
        ** Check to see if we need to advance to the next sector, which has
        ** the side effect of increasing the cluster number if required.
        */
        if ( sector_offset >= drive_ptr->BPB.SECTOR_SIZE )
        {
            temp_error = MFS_next_data_sector(drive_ptr, handle_ptr, &sector_index, &sector_number);
            if (temp_error == MFS_EOF)
            {
                /* Always allocate new cluster, there is something to be written for sure */
                error = MFS_Add_cluster_to_chain(drive_ptr, handle_ptr->PREVIOUS_CLUSTER, &handle_ptr->CURRENT_CLUSTER);
                if ( MFS_NO_ERROR == error )
                {
                    sector_number = CLUSTER_TO_SECTOR(handle_ptr->CURRENT_CLUSTER);
                    sector_index = 0;
                }

            }
            else
            {
                error = temp_error;
            }
            sector_offset = 0;
        }
    }

    /* Write partial sector if sector_offset is non-zero */
    if ( (sector_offset != 0) && (error == MFS_NO_ERROR) )
    {
        /* Offset is non-zero, reading the data is required */
        error = MFS_Read_data_sector(drive_ptr, handle_ptr, sector_number, TRUE);

        if ( error == MFS_NO_ERROR )
        {
            /* The requested lenght of data may span the sector to it's end  */
            copy_size = min(num_bytes, drive_ptr->BPB.SECTOR_SIZE-sector_offset);

            _mem_copy(buffer_address, &drive_ptr->DATA_SECTOR_PTR[sector_offset], copy_size);
            drive_ptr->DATA_SECTOR_DIRTY = TRUE;

            if ( drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE )
            {
                error = MFS_Flush_data_sector_buffer(drive_ptr);
            }

            if ( error == MFS_NO_ERROR )
            {
                bytes_written = copy_size;

                /*
                ** Check to see if we need to advance to the next sector, which has
                ** the side effect of increasing the cluster number if required.
                */
                if ( (sector_offset + bytes_written) >= drive_ptr->BPB.SECTOR_SIZE )
                {
                    temp_error = MFS_next_data_sector(drive_ptr, handle_ptr, &sector_index, &sector_number);
                    /* Only an error if we are not done writing and can't extend the chain */
                    if (bytes_written < num_bytes)
                    {
                        if (temp_error == MFS_EOF)
                        {
                            /* Allocate new cluster */
                            error = MFS_Add_cluster_to_chain(drive_ptr, handle_ptr->PREVIOUS_CLUSTER, &handle_ptr->CURRENT_CLUSTER);
                            if ( MFS_NO_ERROR == error )
                            {
                                sector_number = CLUSTER_TO_SECTOR(handle_ptr->CURRENT_CLUSTER);
                                sector_index = 0;
                            }
                        }
                        else
                        {
                            error = temp_error;
                        }
                    }
                }
            }
        }
    }

    whole_sectors = (num_bytes - bytes_written) >> drive_ptr->SECTOR_POWER;
    while ( (whole_sectors > 0) && (error == MFS_NO_ERROR) )
    {
        cont_sectors = drive_ptr->BPB.SECTORS_PER_CLUSTER - sector_index;
        if (cont_sectors > whole_sectors)
            cont_sectors = whole_sectors;

        error = MFS_Write_device_sectors(drive_ptr, sector_number, cont_sectors, MFSCFG_MAX_WRITE_RETRIES, buffer_address+bytes_written, &proc_sectors);

        if ( proc_sectors > 0 )
        {
            bytes_written += proc_sectors * drive_ptr->BPB.SECTOR_SIZE;
            whole_sectors -= proc_sectors;
            sector_index += proc_sectors - 1;
            temp_error = MFS_next_data_sector(drive_ptr, handle_ptr, &sector_index, &sector_number);
            /* Go on only if we are not done writing */
            if ( (error == MFS_NO_ERROR) && (bytes_written < num_bytes) )
            {
                if ( temp_error == MFS_EOF )
                {
                    /* Allocate new cluster */
                    error = MFS_Add_cluster_to_chain(drive_ptr, handle_ptr->PREVIOUS_CLUSTER, &handle_ptr->CURRENT_CLUSTER);
                    if ( MFS_NO_ERROR == error )
                    {
                        sector_number = CLUSTER_TO_SECTOR(handle_ptr->CURRENT_CLUSTER);
                        sector_index = 0;
                    }
                }
                else
                {
                    error = temp_error;
                }
            }
        }
    }

    /* write last missing data */
    if ( (bytes_written < num_bytes) && (error == MFS_NO_ERROR) )
    {
        /* reading of data required only if we are not going to overwrite the file to the end */
        need_hwread = (fd_ptr->LOCATION + num_bytes) < file_size;
        error = MFS_Read_data_sector(drive_ptr, handle_ptr, sector_number, need_hwread);

        if ( error == MFS_NO_ERROR )
        {
            _mem_copy(buffer_address+bytes_written, &drive_ptr->DATA_SECTOR_PTR[0], num_bytes-bytes_written);
            bytes_written = num_bytes;
            drive_ptr->DATA_SECTOR_DIRTY = TRUE;
            if ( drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE )
            {
                error = MFS_Flush_data_sector_buffer(drive_ptr);
            }
        }
    }

    if ( zeros_written || bytes_written )
    {
        handle_ptr->TOUCHED = 1;
    }

    location += zeros_written + bytes_written;
    ((MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR)->SAVED_POSITION = location;

    if ( location > file_size )
    {
        fd_ptr->SIZE = location;
        htodl(handle_ptr->DIR_ENTRY.FILE_SIZE, fd_ptr->SIZE);
        if ( (drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE) && (error == MFS_NO_ERROR) )
        {
            error = MFS_Update_entry(drive_ptr, handle_ptr);
        }
    }

    /* LOCATION moves forward only by actual data written, the zero fill does not count */
    fd_ptr->LOCATION += bytes_written;

    temp_error = MFS_unlock(drive_ptr, FALSE);
    if (error == MFS_NO_ERROR)
        error = temp_error;

    MFS_set_error_and_return(error_ptr, error, bytes_written);
}

#endif

