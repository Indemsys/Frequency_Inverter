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
* $FileName: mfs_read.c$
* $Version : 3.8.10.0$
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

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_next_data_sector
* Returned Value   :
* Comments  :  
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_next_data_sector(
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    MFS_HANDLE_PTR          handle,
    uint_32_ptr             sector_index_ptr,
    uint_32_ptr             sector_number_ptr
    )
{
    _mfs_error  error = MFS_NO_ERROR;
    uint_32     next_cluster;

    if ( handle->CURRENT_CLUSTER != CLUSTER_EOF )
    {
        (*sector_index_ptr)++;
        if ( *sector_index_ptr >= drive_ptr->BPB.SECTORS_PER_CLUSTER )
        {
            // New cluster
            error = MFS_get_cluster_from_fat(drive_ptr, handle->CURRENT_CLUSTER, &next_cluster);
            if ( error == MFS_NO_ERROR )
            {
                handle->PREVIOUS_CLUSTER = handle->CURRENT_CLUSTER;
                handle->CURRENT_CLUSTER = next_cluster;
                *sector_index_ptr = 0;

                if ( next_cluster == CLUSTER_EOF )
                {
                    error = MFS_EOF;
                }
            }
        }

        if ( error == MFS_NO_ERROR )
        {
            *sector_number_ptr = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER) + *sector_index_ptr;
        }
    }
    else
    {
        error = MFS_EOF;
    }
    return error;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Read
* Returned Value   : number of bytes read
* Comments  :   Read a specific number of bytes from an open file.
*
*END*---------------------------------------------------------------------*/


uint_32  MFS_Read
    (
    MQX_FILE_PTR            fd_ptr,         /*[IN] handle to the open file */
    uint_32                 num_bytes,      /*[IN] number of bytes to be read */
    char_ptr                buffer_address, /*[IN/OUT] bytes are read into this buffer */
    _mfs_error_ptr          error_ptr       /*[IN/OUT] error code is written to this address */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_HANDLE_PTR          handle;
    uint_32                 location;
    uint_32                 bytes_read;
    uint_32                 bytes_left_in_file;
    uint_32                 copy_size;
    uint_32                 cluster_offset;
    uint_32                 sector_number, sector_index;
    uint_32                 sector_offset;
    uint_32                 whole_sectors;
    uint_32                 cont_sectors;
    uint_32                 proc_sectors;
    _mfs_error              error, temp_error;
    boolean                 eof_reached;
    uint_32                 file_size;

    if ( num_bytes == 0 )
    {
        MFS_set_error_and_return(error_ptr,MFS_NO_ERROR,0);
    }

    if ( buffer_address == NULL )
    {
        MFS_set_error_and_return(error_ptr,MFS_INVALID_PARAMETER,0);
    }

    error = MFS_lock_handle( fd_ptr, &drive_ptr, &handle );
    if ( error != MFS_NO_ERROR )
    {
        MFS_set_error_and_return(error_ptr,error,0);
    }

    if ( handle->ACCESS != MFS_ACCESS_READ_ONLY &&
        handle->ACCESS != MFS_ACCESS_READ_WRITE )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_ACCESS_DENIED,0);
    }


    if ( handle->CURRENT_CLUSTER == 0 )
    {
        handle->PREVIOUS_CLUSTER = 0;
        handle->CURRENT_CLUSTER = clustoh(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER);
    }
    else if ( handle->CURRENT_CLUSTER == CLUSTER_EOF )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_EOF,0);
    }


    bytes_read = 0;
    eof_reached = FALSE;
    location = fd_ptr->LOCATION;

    /*
    ** Can't read past file size
    */
    file_size = dtohl(handle->DIR_ENTRY.FILE_SIZE);
    if ( location > file_size )
    {
        location = file_size;
    }
    bytes_left_in_file = file_size - location;
    if ( num_bytes > bytes_left_in_file )
    {
        eof_reached = TRUE;
        num_bytes = bytes_left_in_file;
    }

    if ( bytes_left_in_file )
    {
        /*   
        ** Read the number of bytes from the current file   
        ** position to the end of the current cluster   
        */   
        cluster_offset = OFFSET_WITHIN_CLUSTER(location);
        sector_index = CLUSTER_OFFSET_TO_SECTOR(cluster_offset);
        sector_number = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER) + sector_index;
        sector_offset = OFFSET_WITHIN_SECTOR(location);

        /* Read partial sector if sector_offet is non-zero */
        if (sector_offset != 0)
        {
            error = MFS_Read_data_sector(drive_ptr, handle, sector_number, TRUE);
            if ( error == MFS_NO_ERROR )
            {
                /* The requested lenght of data may span the sector to it's end  */
                copy_size = min(num_bytes, drive_ptr->BPB.SECTOR_SIZE-sector_offset);

                _mem_copy(&drive_ptr->DATA_SECTOR_PTR[sector_offset], buffer_address, copy_size);
                bytes_read=copy_size;

                /*
                ** Check to see if we need to advance to the next sector, which has
                ** the side effect of increasing the cluster number if required.
                */
                if ( (sector_offset+bytes_read) >= drive_ptr->BPB.SECTOR_SIZE )
                {
                    temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
                    /* Only an error if we are not done reading */
                    if ( bytes_read<num_bytes )
                    {
                        error = temp_error;
                    }
                }
            }
        }

        whole_sectors = (num_bytes - bytes_read) >> drive_ptr->SECTOR_POWER;
        while ((whole_sectors > 0) && (error == MFS_NO_ERROR))
        {
            cont_sectors = drive_ptr->BPB.SECTORS_PER_CLUSTER - sector_index;
            if (cont_sectors > whole_sectors)
                cont_sectors = whole_sectors;

            error = MFS_Read_device_sectors(drive_ptr, sector_number, cont_sectors, MFSCFG_MAX_READ_RETRIES, buffer_address+bytes_read, &proc_sectors);

            if ( proc_sectors > 0 )
            {
                bytes_read += proc_sectors * drive_ptr->BPB.SECTOR_SIZE;
                whole_sectors -= proc_sectors;
                sector_index += proc_sectors - 1;
                temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
                /* Only an error if we are not done reading */
                if ( (error==MFS_NO_ERROR) && (bytes_read<num_bytes) )
                {
                    error = temp_error;
                }
            }
        }
           
        if ( (bytes_read < num_bytes) && (error == MFS_NO_ERROR) )
        {
            error = MFS_Read_data_sector(drive_ptr, handle, sector_number, TRUE);
            if ( error == MQX_OK )
            {
                _mem_copy(&drive_ptr->DATA_SECTOR_PTR[0], buffer_address+bytes_read, num_bytes-bytes_read);
                bytes_read = num_bytes;
            }
        }

        fd_ptr->LOCATION = location + bytes_read;
        ((MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR)->SAVED_POSITION = fd_ptr->LOCATION;
    }

    error = MFS_unlock(drive_ptr, FALSE);

    if ((error == MFS_NO_ERROR) && eof_reached)
    {
        error = MFS_EOF;
    }

    MFS_set_error_and_return(error_ptr,error,bytes_read);
}
