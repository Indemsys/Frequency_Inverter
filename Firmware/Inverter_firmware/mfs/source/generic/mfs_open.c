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
* $FileName: mfs_open.c$
* $Version : 3.8.13.0$
* $Date    : Jun-6-2012$
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
* Function Name    : MFS_Open_file
* Returned Value   : A pointer to an MFS_HANDLE
* Comments  :   Open a an existing file.
*
*END*---------------------------------------------------------------------*/

pointer  MFS_Open_file
    (
    MQX_FILE_PTR            mfs_fd_ptr,     /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr                pathname,       /*[IN] directory and filename of the file to be opened */
    uchar                   access,         /*[IN] type of access required: read, write or read/write*/
    uint_32_ptr             error_ptr   /*[IN/OUT] error code is written to this address */   
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    MFS_HANDLE_PTR          handle,open_handle;
    uint_32                 dir_cluster, dir_index;
    _mfs_error              error_code;
    uint_32                 prev_cluster= CLUSTER_INVALID; 

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        if ( error_ptr != NULL )
        {
            *error_ptr = MFS_INVALID_PARAMETER;
        }
        return NULL;
    }

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        if ( error_ptr != NULL )
        {
            *error_ptr = error_code;
        }
        return NULL;
    }

    handle = NULL;

    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);  

    if ( error_code == MFS_NO_ERROR )
    {

        if ( dtohc(dir_entry_ptr->ATTRIBUTE) & (MFS_ATTR_DIR_NAME | MFS_ATTR_VOLUME_NAME) )
        {
            error_code = MFS_WRITE_FAULT;
        }
        else if ( (dtohc(dir_entry_ptr->ATTRIBUTE) & MFS_ATTR_READ_ONLY) && ((access == MFS_ACCESS_WRITE_ONLY) || (access == MFS_ACCESS_READ_WRITE)) )
        {
            error_code = MFS_ACCESS_DENIED;
        }
        else
        {
            // Check to see if the file is already opened
            open_handle = MFS_Find_handle(drive_ptr, dir_cluster, dir_index);
            
            if (open_handle) {
                // If we are opening to write, the file can't already be opened.
                if ((access == MFS_ACCESS_WRITE_ONLY) || (access == MFS_ACCESS_READ_WRITE)) {
                    error_code = MFS_SHARING_VIOLATION;
                }

                // And if we the file is already opened, it can't be opened to write.
                // Note that if it is opened for write, it will be the only instance on the list
                if ((handle->ACCESS == MFS_ACCESS_WRITE_ONLY) || (handle->ACCESS == MFS_ACCESS_READ_WRITE)) {
                    error_code = MFS_SHARING_VIOLATION;
                }
            }
        
            if (error_code == MFS_NO_ERROR) {
                handle = MFS_Get_handle(drive_ptr,dir_entry_ptr);
                if ( handle != NULL )
                {
                    handle->ACCESS = access;
                    handle->CURRENT_CLUSTER = 0;
                    handle->PREVIOUS_CLUSTER = 0;
                    handle->DIR_CLUSTER = dir_cluster;
                    handle->DIR_INDEX = dir_index;
                }
                else
                {
                    error_code = MFS_INSUFFICIENT_MEMORY;
                }  
            }
        }  
    }

    MFS_unlock(drive_ptr,FALSE);

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    return((pointer) handle);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Close_file
* Returned Value   : error code
* Comments  :   Close an existing file.
*
*END*---------------------------------------------------------------------*/

int_32  MFS_Close_file
    (
    MQX_FILE_PTR            fd_ptr  /*[IN] file that is to be closed */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_HANDLE_PTR          handle;
    TIME_STRUCT             time;
    DATE_STRUCT             clk_time;
    _mfs_error              error_code;

    error_code = MFS_lock_handle( fd_ptr, &drive_ptr, &handle );
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

#if !MFSCFG_READ_ONLY
#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (NULL, drive_ptr))
    {
        error_code = MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif    
    if ((handle->TOUCHED) && (error_code == MFS_NO_ERROR))
    {
        _time_get(&time);
        _time_to_date(&time, &clk_time);
        NORMALIZE_DATE(&clk_time);
        htods(handle->DIR_ENTRY.TIME, PACK_TIME(clk_time));
        htods(handle->DIR_ENTRY.DATE, PACK_DATE(clk_time));
        error_code = MFS_Update_entry(drive_ptr, handle);
    }
#endif

    MFS_Free_handle(drive_ptr, handle);
    MFS_unlock(drive_ptr,TRUE);

    return(int_32) error_code;
}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Move_file_pointer
* Returned Value   : current file pointer position
* Comments  :   Move the current file pointer position.
*               If filelength = 0, it returns MFS_EOF
*END*---------------------------------------------------------------------*/

uint_32 MFS_Move_file_pointer
    (
    MQX_FILE_PTR            fd_ptr,         /*[IN] file handle upon which the action is to be taken */
    _mfs_error_ptr             error_ptr    /*[IN/OUT] resulting error code is written to this address*/
    )
{

    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_HANDLE_PTR          handle;
    uint_32                 position_after_seek, position_before_seek;
    uint_32                 current_cluster,
                            previous_cluster,
                            first_cluster,
                            skip_clusters,
                            k;
    _mfs_error              error_code;


    error_code = MFS_lock_handle( fd_ptr, &drive_ptr, &handle );
    if ( error_code != MFS_NO_ERROR )
    {
        if ( error_ptr != NULL )
        {
            *error_ptr = error_code;
        }
        return 0;
    }

    position_after_seek = fd_ptr->LOCATION;

    MFS_LOG(printf("seek to %d\n",position_after_seek));

    /*
    ** Cannot move ahead of the beginning of the file; force beginning.
    */
    if ( error_code == MFS_NO_ERROR )
    {

        /*
        ** There are four cases
        **
        ** 1) Seeking to beginning of file (position_after_seek=0, cluster=0)
        ** 2) No change in position (position_after_seek==position_before_seek)
        ** 2) Seeking ahead of current file pointer
        ** 3) Seeking behind current file pointer
        */
        /*
        ** Cannot move beyond the end of file !
        */
        if ( position_after_seek> dtohl(handle->DIR_ENTRY.FILE_SIZE) )     // + 1
        {
            position_after_seek = dtohl(handle->DIR_ENTRY.FILE_SIZE);  //  - 1
            error_code = MFS_EOF;
        }

        current_cluster = 0;
        previous_cluster = 0;
        first_cluster = clustoh(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER);
        MFS_LOG(printf("first_cluster =  %d\n",first_cluster));

        /*
        ** Set the current_cluster correctly.
        ** If we're moving ahead, don't start from the beginning.
        */
        position_before_seek = CLUSTER_BOUNDARY(((MFS_HANDLE_PTR) fd_ptr->DEV_DATA_PTR)->SAVED_POSITION);

        if ( handle->CURRENT_CLUSTER==0 )
        {
            handle->CURRENT_CLUSTER = first_cluster;
            handle->PREVIOUS_CLUSTER = 0;
            position_before_seek = 0;
        }

        if ( position_after_seek == 0 )
        {
            current_cluster = first_cluster;
            previous_cluster = 0;
        }
        else if ( position_after_seek == position_before_seek )
        {
            current_cluster  = handle->CURRENT_CLUSTER;
            previous_cluster = handle->PREVIOUS_CLUSTER;
        }
        else
        {
            if ( position_after_seek < position_before_seek )
            {
                position_before_seek = 0;
                current_cluster = first_cluster;
                previous_cluster = 0;
            }
            else
            {
                current_cluster  = handle->CURRENT_CLUSTER;
                previous_cluster = handle->PREVIOUS_CLUSTER;
            }  

            MFS_LOG(printf("current cluster =  %d\n",current_cluster));

            if ( current_cluster && (current_cluster!=CLUSTER_EOF) )
            {
                /*
                ** How many clusters do we need to skip?
                */
                skip_clusters = (position_after_seek - position_before_seek) >> drive_ptr->CLUSTER_POWER_BYTES;
                for ( k = 0; k < skip_clusters; k++ )
                {
                    previous_cluster = current_cluster;

                    error_code = MFS_get_cluster_from_fat(drive_ptr, previous_cluster, &current_cluster);
                    if ( error_code != MFS_NO_ERROR )
                    {
                        break;
                    }
                    if ( current_cluster==CLUSTER_EOF )
                    {
                        error_code = MFS_EOF;
                        break;
                    }
                    else if ( (current_cluster < CLUSTER_MIN_GOOD) || (current_cluster > drive_ptr->LAST_CLUSTER) )
                    {
                        error_code = MFS_BAD_DISK_UNIT;
                        break;
                    }
                    MFS_LOG(printf("skip, current cluster =  %d\n",current_cluster));
                }  
            }
            else
            {
                error_code = MFS_EOF;
            }
        }  

        if ( (error_code == MFS_NO_ERROR) || (error_code == MFS_EOF) )
        {
            ((MFS_HANDLE_PTR) fd_ptr->DEV_DATA_PTR)->SAVED_POSITION = position_after_seek;

            handle->PREVIOUS_CLUSTER  = previous_cluster;
            handle->CURRENT_CLUSTER   = current_cluster;
        }
    }
    MFS_unlock(drive_ptr,FALSE);

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    return(fd_ptr->LOCATION);
}
