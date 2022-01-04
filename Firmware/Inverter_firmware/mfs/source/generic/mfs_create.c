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
* $FileName: mfs_create.c$
* $Version : 3.6.6.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the file specific interface functions
*   of MFS which need a filename as an input parameter.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"

#if !MFSCFG_READ_ONLY

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Create_file
* Returned Value   : file_handle
* Comments  :   Create a file.  If the file exists then truncate it to 0.
*
*END*---------------------------------------------------------------------*/

pointer  MFS_Create_file
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] the MFS device on which to operate */
    uchar                   attr,       /*[IN] attribute for created file*/
    char_ptr                pathname,   /*[IN] directory and filename of the file to be created */
    _mfs_error_ptr          error_ptr   /*[IN/OUT] error code is written to this address */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_HANDLE_PTR          handle,next_handle;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    TIME_STRUCT             time;
    DATE_STRUCT             clk_time;
    uint_32                 dir_cluster;
    uint_32                 dir_index;
    char                    access;
    _mfs_error              error_code, saved_code = 0;

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        *error_ptr = MFS_INVALID_PARAMETER;
        return( NULL );
    }

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        *error_ptr = MFS_DISK_IS_WRITE_PROTECTED;
        return NULL;
    }
#endif

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        *error_ptr = error_code;
        return( NULL );
    }

    handle = NULL;

    attr &= (MFS_ATTR_READ_ONLY | MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE | MFS_ATTR_ARCHIVE | MFS_ATTR_VOLUME_NAME);
    attr |= MFS_ATTR_ARCHIVE;
    access = (attr & MFS_ATTR_READ_ONLY) ? MFS_ACCESS_READ_ONLY : MFS_ACCESS_READ_WRITE;
    dir_entry_ptr =  MFS_Create_entry_slave(drive_ptr, attr, pathname, &dir_cluster, &dir_index, &error_code, TRUE);

    if ( (dir_entry_ptr != NULL) && !(attr & MFS_ATTR_VOLUME_NAME) )
    {
        handle = MFS_Get_handle(drive_ptr,dir_entry_ptr);
        if ( handle != NULL )
        {
            handle->DIR_CLUSTER = dir_cluster;
            handle->DIR_INDEX = dir_index;
            handle->ACCESS = access;
            handle->CURRENT_CLUSTER = 0;
            handle->PREVIOUS_CLUSTER = 0;

            /*
            ** If file exists, overwrite and set size to 0
            */
            if ( error_code == MFS_FILE_EXISTS )
            {
                _time_get(&time);
                _time_to_date(&time, &clk_time);
                NORMALIZE_DATE(&clk_time);
                saved_code = MFS_Release_chain(drive_ptr, clustoh(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER));
                if ( saved_code == MFS_NO_ERROR || saved_code == MFS_LOST_CHAIN )
                {
                    clustod(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER, 0);
                    htodl(handle->DIR_ENTRY.FILE_SIZE, 0L);
                    htodc(handle->DIR_ENTRY.ATTRIBUTE, attr);
                    htods(handle->DIR_ENTRY.TIME, PACK_TIME(clk_time));
                    htods(handle->DIR_ENTRY.DATE, PACK_DATE(clk_time));
                    error_code = MFS_Update_entry(drive_ptr, handle);

                    /*
                    ** If the same file is already open, mark it as 'freshly
                    ** truncated' so reads and writes don't clobber any data.
                    */
                    if ( error_code == MFS_NO_ERROR )
                    {
                        next_handle =  (MFS_HANDLE_PTR) _queue_head(&drive_ptr->HANDLE_LIST);
                        while ( next_handle )
                        {
                            if ( next_handle->DIR_CLUSTER == dir_cluster && next_handle->DIR_INDEX == dir_index )
                            {
                                next_handle->CURRENT_CLUSTER = 0;
                                next_handle->PREVIOUS_CLUSTER = 0;
                            }
                            next_handle =  (MFS_HANDLE_PTR) _queue_next(&drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) next_handle);
                        }  
                    }
                }
            }

            /*
            ** No need to update the disk image if we didn't change anything.
            */
            if ( (dtohc(handle->DIR_ENTRY.ATTRIBUTE) != attr) && (error_code == MFS_NO_ERROR) )
            {
                htodc(handle->DIR_ENTRY.ATTRIBUTE, attr);
                error_code = MFS_Update_entry(drive_ptr, handle);
            }

        }
        else
        {
            error_code = MFS_INSUFFICIENT_MEMORY;
        }  
    }

    MFS_unlock(drive_ptr,FALSE);

    if ( error_code == MFS_NO_ERROR && saved_code == MFS_LOST_CHAIN )
    {
        *error_ptr = saved_code;
    }
    else
    {
        *error_ptr = error_code; 
    }  

    return((pointer)handle);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Create_new_file
* Returned Value   : file handle
* Comments  :   Create a file. If the file exists, then an error has occurred.
*
*END*---------------------------------------------------------------------*/

pointer  MFS_Create_new_file
    (
    MQX_FILE_PTR            mfs_fd_ptr,     /*[IN] the MFS device on which to operate */
    uchar                   attr,           /*[IN] attribute to be given to the new file */
    char_ptr                pathname,       /*[IN] directory and file name to be given to the new file */
    _mfs_error_ptr             error_ptr    /*[IN/OUT] error code is written to this address */
    )
{

    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_HANDLE_PTR          handle;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    uint_32                 dir_cluster, dir_index;
    _mfs_error              error_code;
    char                    access;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        *error_ptr = MFS_DISK_IS_WRITE_PROTECTED;
        return NULL;
    }
#endif

    handle = NULL;
    error_code = MFS_NO_ERROR;

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        *error_ptr = MFS_INVALID_PARAMETER;
        return( NULL );

    }

    *error_ptr = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( *error_ptr != MFS_NO_ERROR )
    {
        return NULL;
    }

    attr &= (MFS_ATTR_READ_ONLY | MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE | MFS_ATTR_ARCHIVE);
    attr |= MFS_ATTR_ARCHIVE;   

    access = (attr & MFS_ATTR_READ_ONLY) ? MFS_ACCESS_READ_ONLY : MFS_ACCESS_READ_WRITE;

    dir_entry_ptr =  MFS_Create_entry_slave(drive_ptr, attr, pathname, &dir_cluster, &dir_index, &error_code, FALSE);

    if ( error_code == MFS_NO_ERROR && dir_entry_ptr != NULL )
    {
        handle = MFS_Get_handle(drive_ptr,dir_entry_ptr);

        if ( handle )
        {
            handle->ACCESS = access;
            handle->DIR_CLUSTER = dir_cluster;
            handle->DIR_INDEX = dir_index;
            handle->CURRENT_CLUSTER = 0;
            handle->PREVIOUS_CLUSTER = 0;
        }
        else
        {
            error_code = MFS_INSUFFICIENT_MEMORY;
        }  
    }

    MFS_unlock(drive_ptr,FALSE);

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    return((pointer)handle);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Create_temp_file
* Returned Value   : MFS_HANDLE_PTR
* Comments  :   Create a temporary file with an internally generated name.
*
*END*---------------------------------------------------------------------*/

pointer  MFS_Create_temp_file
    (
    MQX_FILE_PTR            mfs_fd_ptr,     /*[IN] the MFS device on which to operate */
    uchar                   attr,           /*[IN] attribute to be given to the file when it is created */
    char_ptr                pathname,       /*[IN] provides the directory path where the file is to be created,
                                            **[OUT] the file name is appended to the directory name */
    _mfs_error_ptr             error_ptr    /*[OUT] Pointer for the MFS ERROR CODE location */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_HANDLE_PTR          handle;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    _mfs_error              error_code;
    uint_32                 dir_cluster, dir_index;
    uint_16                 trial;
    char_ptr                last_char;
    uint_16                 pathlen;
    uchar                   access;
    char_ptr                temp_file;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        *error_ptr = MFS_DISK_IS_WRITE_PROTECTED;
        return NULL;
    }
#endif

    trial = 0;
    error_code = MFS_FILE_NOT_FOUND;
    handle = NULL;

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        *error_ptr = MFS_INVALID_PARAMETER;
        return( NULL );
    }

    *error_ptr  = MFS_alloc_path(&temp_file);
    if ( *error_ptr )
    {
        return( NULL );
    }

    *error_ptr = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( *error_ptr != MFS_NO_ERROR )
    {
        MFS_free_path(temp_file);
        return NULL;
    }

    attr &= (MFS_ATTR_READ_ONLY | MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE | MFS_ATTR_ARCHIVE | MFS_ATTR_VOLUME_NAME);
    attr |= MFS_ATTR_ARCHIVE;
    access = (attr & MFS_ATTR_READ_ONLY) ? MFS_ACCESS_READ_ONLY : MFS_ACCESS_READ_WRITE;

    dir_cluster = drive_ptr->CUR_DIR_CLUSTER;
    dir_cluster = MFS_Find_directory(drive_ptr, pathname, dir_cluster);

    if ( dir_cluster == CLUSTER_INVALID )
    {
        error_code = MFS_PATH_NOT_FOUND;
    }
    else
    {
        pathlen = strlen (pathname);
        last_char = pathname + pathlen;
        if ( pathlen )
        {
            last_char--;
        }

        do
        {
            sprintf(temp_file, (*last_char == '\\' || *last_char == '/' || pathlen == 0) ?
                "%sTMP%05.5lu.@@@" : "%s\\TMP%05.5lu.@@@", pathname, (uint_32) trial);
            dir_entry_ptr = MFS_Create_entry_slave(drive_ptr, attr, temp_file, &dir_cluster, &dir_index, &error_code, FALSE);
            trial++;
        } while ( (error_code == MFS_FILE_EXISTS) && (trial < MFSCFG_FIND_TEMP_TRIALS) );

        if ( error_code == MFS_NO_ERROR )
        {
            handle = MFS_Get_handle(drive_ptr,dir_entry_ptr);
            if ( handle )
            {
                _mem_copy (temp_file, pathname, strlen(temp_file) + 1);
                handle->DIR_CLUSTER = dir_cluster;
                handle->DIR_INDEX = dir_index;
                handle->ACCESS = access;
                handle->CURRENT_CLUSTER = 0;
                handle->PREVIOUS_CLUSTER = 0;
            }
            else
            {
                error_code = MFS_INSUFFICIENT_MEMORY;
            }  
        }
    }  

    MFS_free_path(temp_file);
    MFS_unlock(drive_ptr,FALSE);

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    return((pointer) handle);
}  

#endif

