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
* $FileName: mfs_directory.c$
* $Version : 3.8.11.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   This file contains the functions that are used to create, remove, change,
*   and search through directories.
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
* Function Name    :  MFS_Create_subdir
* Returned Value   :  error_code
* Comments  :  Create a subdirectory.
*
*END*---------------------------------------------------------------------*/

_mfs_error  MFS_Create_subdir
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr                pathname    /*[IN] pathname of the directory to be created */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    uint_32                 dir_cluster;
    uint_32                 parent_cluster;
    uint_32                 free_cluster;
    uint_32                 dir_index;
    _mfs_error              error_code;
    char_ptr                temp_dirname;
    char_ptr                temp_filename;

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error_code = MFS_alloc_2paths(&temp_dirname,&temp_filename);
    if ( error_code != MFS_NO_ERROR )
    {
        return( error_code );
    }
    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        MFS_free_path(temp_dirname);
        MFS_free_path(temp_filename);
        return error_code;
    }

    MFS_Parse_pathname (temp_dirname, temp_filename, pathname);
    dir_cluster = drive_ptr->CUR_DIR_CLUSTER;
    parent_cluster = MFS_Find_directory (drive_ptr, temp_dirname, dir_cluster);

    dir_cluster = parent_cluster;
    if ( MFS_is_valid_lfn(temp_filename) )
    {
        if ( dir_cluster != CLUSTER_INVALID )
        {
            /*
            ** We'll obtain a cluster for the new directory first. If we
            ** cannot create the directory afterwards, it is easier to re-free
            ** the cluster than to remove the new entry.
            */
            free_cluster = MFS_Find_unused_cluster_from(drive_ptr, drive_ptr->NEXT_FREE_CLUSTER);
            if ( free_cluster != CLUSTER_INVALID )
            {
                error_code = MFS_Clear_cluster(drive_ptr, free_cluster);
                if ( error_code )
                {
                    MFS_unlock(drive_ptr,TRUE);
                    MFS_free_path(temp_dirname);
                    MFS_free_path(temp_filename);
                    return( error_code );
                }
                error_code = MFS_Put_fat(drive_ptr, free_cluster, CLUSTER_EOF);
                dir_entry_ptr = MFS_Create_directory_entry(drive_ptr, temp_filename, MFS_ATTR_DIR_NAME, &dir_cluster, &dir_index, &error_code);
                if ( error_code == MFS_NO_ERROR )
                {
                    clustod(dir_entry_ptr->HFIRST_CLUSTER, dir_entry_ptr->LFIRST_CLUSTER, free_cluster);
                    drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                    /*
                    ** We shall now create the "." and ".." entries.
                    */
                    dir_cluster = free_cluster;
                    dir_entry_ptr = MFS_Create_directory_entry(drive_ptr,".",  MFS_ATTR_DIR_NAME, &dir_cluster, &dir_index, &error_code);
                    if ( error_code == MFS_NO_ERROR )
                    {
                        clustod(dir_entry_ptr->HFIRST_CLUSTER,  dir_entry_ptr->LFIRST_CLUSTER, free_cluster);
                        drive_ptr->DIR_SECTOR_DIRTY = TRUE;
                        dir_entry_ptr = MFS_Create_directory_entry(drive_ptr,"..", MFS_ATTR_DIR_NAME, &dir_cluster, &dir_index, &error_code);

                        if ( error_code == MFS_NO_ERROR )
                        {
                            if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
                            {
                                if ( drive_ptr->BPB32.ROOT_CLUSTER == parent_cluster )
                                {
                                    /* 
                                    ** Even though the FAT32 root sector can be 
                                    ** anywhere, it is identified as 0 when referenced 
                                    ** through a directory entry
                                    */          
                                    parent_cluster = 0;
                                }
                            }
                            clustod(dir_entry_ptr->HFIRST_CLUSTER, dir_entry_ptr->LFIRST_CLUSTER, parent_cluster);
                            drive_ptr->DIR_SECTOR_DIRTY = TRUE;
                        }
                    }
                }
                else
                {
                    MFS_Put_fat(drive_ptr, free_cluster, CLUSTER_UNUSED);
                }  
            }
            else
            {
                error_code = MFS_DISK_FULL;
            }  
        }
        else
        {
            error_code = MFS_PATH_NOT_FOUND;
        }  
    }
    else if ( MFS_lfn_dirname_valid(temp_filename) )
    {
        if ( dir_cluster )
        {
            error_code = MFS_FILE_EXISTS;
        }
        else
        {
            error_code = MFS_CANNOT_CREATE_DIRECTORY;
        }  
    }
    else
    {
        error_code = MFS_INVALID_PARAMETER;
    }  

    MFS_free_path(temp_dirname);
    MFS_free_path(temp_filename);
    MFS_unlock(drive_ptr,TRUE);

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Remove_subdir
* Returned Value   :  error_code
* Comments  :  If the specified subdirectory has no entries, then remove
*                    the directory.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Remove_subdir
    (
    MQX_FILE_PTR            mfs_fd_ptr,  /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr                pathname     /*[IN] pathname of the directory to be removed */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    _mfs_error              error_code, saved_error = 0;
    uint_32                 dir_cluster, parent_cluster, dir_index;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    uint_32                 prev_cluster= CLUSTER_INVALID; 
    uint_32                 parent_prev_cluster= CLUSTER_INVALID; 

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

    dir_entry_ptr = MFS_Find_entry_on_disk (drive_ptr, pathname, &error_code, &parent_cluster, &dir_index,  &parent_prev_cluster);  

    if ( dir_entry_ptr )
    {
        if ( dtohc(dir_entry_ptr->ATTRIBUTE) & MFS_ATTR_DIR_NAME )
        {
            dir_cluster = clustoh(dir_entry_ptr->HFIRST_CLUSTER, dir_entry_ptr->LFIRST_CLUSTER);
            if ( dir_cluster != drive_ptr->CUR_DIR_CLUSTER )
            {
                dir_index = 2;  /* Skip over '.' and '..' */
                dir_entry_ptr = MFS_Find_directory_entry (drive_ptr, NULL, &dir_cluster, &dir_index, &prev_cluster, MFS_ATTR_ANY, &error_code);  
                if ( dir_entry_ptr == NULL && !error_code )
                {
                    dir_index = 0;
                    dir_entry_ptr = MFS_Find_entry_on_disk (drive_ptr, pathname, &error_code, &parent_cluster, &dir_index, &parent_prev_cluster);
                    if ( dir_entry_ptr )
                    {
                        *dir_entry_ptr->NAME = MFS_DEL_FILE;
                        drive_ptr->DIR_SECTOR_DIRTY = TRUE;
                        error_code = MFS_remove_lfn_entries(drive_ptr, parent_cluster, dir_index, parent_prev_cluster); 
                        if ( !error_code )
                        {
                            saved_error = MFS_Release_chain(drive_ptr, dir_cluster);
                        }
                    }
                }
                else if ( dir_entry_ptr != NULL && !error_code )
                {
                    error_code = MFS_FILE_EXISTS;
                }
            }
            else
            {
                error_code = MFS_ATTEMPT_TO_REMOVE_CURRENT_DIR;
            }  
        }
        else
        {
            error_code = MFS_WRITE_FAULT;
        }  
    }
    else
    {
        error_code = MFS_PATH_NOT_FOUND;
    }  

    MFS_unlock(drive_ptr,TRUE);

    if ( saved_error == MFS_LOST_CHAIN && error_code == MFS_NO_ERROR )
    {
        error_code = saved_error;
    }

    return(error_code);
}  

#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Get_current_dir
* Returned Value   :  error_code
* Comments  :  Return the pathname of the current directory.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Get_current_dir
    (
    MQX_FILE_PTR            mfs_fd_ptr,     /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr                buffer_address  /*[IN/OUT] address of the buffer where the pathname is to be written */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    _mfs_error              error_code;

    if ( buffer_address == NULL )
    {
        return MFS_INVALID_PARAMETER;
    }

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

    strcpy(buffer_address, drive_ptr->CURRENT_DIR);

    MFS_unlock(drive_ptr,FALSE);

    return(error_code);
}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Change_current_dir
* Returned Value   :  error_code
* Comments  :  Change the current directory.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Change_current_dir
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr               pathname     /*[IN] pathname of the directory to become the current dir */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    uint_32                 dir_cluster;
    uint_16                 length;
    _mfs_error              error_code;
    char_ptr                directory;
    char_ptr                temp_cur;
    register char_ptr       src, dest, dest_start;

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

    error_code = MFS_alloc_2paths(&directory,&temp_cur);
    if ( error_code != MFS_NO_ERROR )
    {
        return( error_code );
    }

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        MFS_free_path(directory);
        MFS_free_path(temp_cur);
        return error_code;
    }

    dir_cluster = drive_ptr->CUR_DIR_CLUSTER;
    dir_cluster = MFS_Find_directory(drive_ptr, pathname, dir_cluster);

    if ( dir_cluster != CLUSTER_INVALID )
    {
        src = pathname;
        dest = temp_cur;

        if ( *src == '\\' || *src == '/' )
        {
            temp_cur[0] = '\\';
            temp_cur[1] = '\0';
            src++;
        }
        else
        {
            strcpy (temp_cur, drive_ptr->CURRENT_DIR);
            dest = temp_cur + strlen(temp_cur) - 1;
        }  
        dest_start = temp_cur;

        length = strlen(dest_start);

        while ( src )
        {
            if ( *src )
            {
                src = MFS_Parse_next_filename(src, directory);
                if ( src != NULL )
                {
                    MFS_strupr(directory);
                }
                if ( directory[0] == '.' && directory[1] == '.' )
                {
                    if ( dest != dest_start )
                    {
                        while ( dest != dest_start && *dest != '\\' )
                        {
                            dest--;
                        }  
                        /* Erase the previous backslash, unless it is the
                        ** first (root directory).
                        */
                        if ( dest != dest_start )
                        {
                            *dest-- = '\0';
                        }
                        else
                        {
                            *(dest+1) = '\0';
                        }  
                    }
                    else
                    {
                        /*
                        ** The check for underflow is redundant, because the path
                        ** has been found already.
                        */
                        error_code = MFS_PATH_NOT_FOUND;
                        break;
                    }  
                }
                else if ( directory[0] && directory[0] != '.' )
                {
                    if ( *dest != '\\' )
                    {
                        *(++dest) = '\\';
                        length++;
                    }
                    length += strlen(directory);
                    if ( length < sizeof (drive_ptr->CURRENT_DIR) )
                    {
                        strcpy (++dest, directory);
                        dest = temp_cur + length - 1;
                    }
                    else
                    {
                        error_code = MFS_INVALID_LENGTH_IN_DISK_OPERATION;
                        break;
                    }  
                }
            }
            else
            {
                break;
            }  
        }  
        if ( error_code == MFS_NO_ERROR )
        {
            strcpy (drive_ptr->CURRENT_DIR, dest_start);
            drive_ptr->CUR_DIR_CLUSTER = dir_cluster;
        }
    }
    else
    {
        error_code = MFS_PATH_NOT_FOUND;
    }  

    MFS_free_path(directory);
    MFS_free_path(temp_cur);
    MFS_unlock(drive_ptr,FALSE);

    return(error_code);
}  

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Check_dir_exist
* Returned Value   :  error_code
* Comments  :  Check if directory exist on device
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Check_dir_exist
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr               pathname     /*[IN] pathname of the directory to become the current dir */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    uint_32                 dir_cluster;
    _mfs_error              error_code;
    char_ptr                directory;

    // for empty string return error
    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

    // allocate memory for directory
    error_code = MFS_alloc_path(&directory);
    if ( error_code != MFS_NO_ERROR )
    {
        return( error_code );
    }

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        MFS_free_path(directory);
        return error_code;
    }

    dir_cluster = ROOT_CLUSTER(drive_ptr);
    dir_cluster = MFS_Find_directory(drive_ptr, pathname, dir_cluster);

    if ( dir_cluster == CLUSTER_INVALID )
    {
        error_code = MFS_PATH_NOT_FOUND;
    }  

    MFS_free_path(directory);
    MFS_unlock(drive_ptr,FALSE);

    return(error_code);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Find_first_file
* Returned Value   : error_code
* Comments  :   Find the first file that matches the input criteria.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Find_first_file
    (
    MQX_FILE_PTR            mfs_fd_ptr,     /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    uchar                   attribute,      /*[IN] type of file to find, Search attributes */
    char_ptr                pathname,       /*[IN] optionally the directory and filename to search for */
    MFS_SEARCH_DATA_PTR     transfer_ptr    /*[IN] address of search data block into which the results of the search are put */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_INTERNAL_SEARCH_PTR internal_search_ptr;
    char_ptr                temp_dirname;
    char_ptr                temp_filename;
    uint_32                 current_cluster;
    _mfs_error              error_code;
    uint_32                 i;
    char                    c;

    if ( (pathname==NULL) || (*pathname=='\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

    if ( transfer_ptr == NULL )
    {
        return MFS_INVALID_MEMORY_BLOCK_ADDRESS;
    }

    error_code = MFS_alloc_2paths(&temp_dirname,&temp_filename);
    if ( error_code != MFS_NO_ERROR )
    {
        return( error_code );
    }

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        MFS_free_path(temp_dirname);
        MFS_free_path(temp_filename);
        return error_code;
    }

    _mem_zero(transfer_ptr, sizeof (MFS_SEARCH_DATA));
    transfer_ptr->DRIVE_PTR = drive_ptr;

    MFS_Parse_pathname(temp_dirname, temp_filename, pathname);

    current_cluster = drive_ptr->CUR_DIR_CLUSTER;
    current_cluster = MFS_Find_directory(drive_ptr, temp_dirname, current_cluster);

    if ( current_cluster == CLUSTER_INVALID )
    {
        error_code = MFS_PATH_NOT_FOUND;
    }
    else
    {
        /*
        ** The internal search is only initialised if the directory exists.
        */
        internal_search_ptr = &transfer_ptr->INTERNAL_SEARCH_DATA;
        internal_search_ptr->CURRENT_CLUSTER = current_cluster;
        internal_search_ptr->PREV_CLUSTER = CLUSTER_INVALID;  
        internal_search_ptr->DIR_ENTRY_INDEX = 0;

        MFS_Expand_wildcard(temp_filename, internal_search_ptr->FILENAME);   
        internal_search_ptr->FULLNAME = temp_filename;

        i = _strnlen(pathname, PATHNAME_SIZE + FILENAME_SIZE + 1);
        c = pathname[--i];
        while ( (c != '\\') && (c != '/') && (c != ':') && i )
        {
            i--;         
            c = pathname[i];
        }  
        if ( i || c == '\\' || c == '/' )
        {
            i++;
        }

        internal_search_ptr->SRC_PTR = pathname + i;
        internal_search_ptr->ATTRIBUTE = attribute;
        error_code = MFS_Find_next_slave(drive_ptr, transfer_ptr);
    }  

    MFS_free_path(temp_dirname);
    MFS_free_path(temp_filename);
    MFS_unlock(drive_ptr,FALSE);

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Find_next_file
* Returned Value   : error_code
* Comments  :   Find the next file that matches the input criteria. Must
*    follow a Find_first_file and the search data block from the search
*    must be supplied.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Find_next_file
    (
    MQX_FILE_PTR            mfs_fd_ptr,     /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    MFS_SEARCH_DATA_PTR     transfer_ptr    /* [IN/OUT] address of search data block indicating the current criteria and the results of 
                                            ** the last search results of this search are placed in this data block */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    _mfs_error              error_code;

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );

    if ( error_code == MFS_NO_ERROR )
    {
        error_code = MFS_Find_next_slave(drive_ptr, transfer_ptr);
        MFS_unlock(drive_ptr,FALSE);
    }
    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Set_volume
* Returned Value   : error_code
* Comments  :   Sets the drive volume. Assumes the drive is NOT locked.
*
*END*---------------------------------------------------------------------*/

#if !MFSCFG_READ_ONLY
_mfs_error MFS_Set_volume
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr                volume_name /*[IN] name to use for the volune */
    )
{
    MFS_DRIVE_STRUCT_PTR       drive_ptr;
    MFS_DIR_ENTRY_PTR          dir_entry_ptr;
    _mfs_error                 error_code;
    uint_32                    vol_cluster, vol_index;
    uint_32                    prev_cluster= CLUSTER_INVALID; 

    if ( volume_name == NULL )
    {
        return( MFS_INVALID_PARAMETER );
    }

    if ( *volume_name == '\0' )
    {
        return( MFS_INVALID_PARAMETER );
    }

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (mfs_fd_ptr, NULL))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error_code = MFS_lock_dos_disk(mfs_fd_ptr, &drive_ptr);

    if ( !error_code )
    {
        vol_cluster = ROOT_CLUSTER(drive_ptr);
        vol_index = 0;
        dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, NULL, &vol_cluster, &vol_index, &prev_cluster, MFS_ATTR_VOLUME_NAME, &error_code); 

        if ( dir_entry_ptr )
        {
            MFS_Expand_dotfile(volume_name, dir_entry_ptr->NAME);
            *dir_entry_ptr->ATTRIBUTE = MFS_ATTR_VOLUME_NAME;
            drive_ptr->DIR_SECTOR_DIRTY = TRUE;

        }
        else if ( !error_code )
        {
            vol_cluster = ROOT_CLUSTER(drive_ptr);
            vol_index = 0;
            dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, "", &vol_cluster, &vol_index, &prev_cluster, MFS_ATTR_ANY, &error_code); 
            if ( dir_entry_ptr )
            {
                MFS_Expand_dotfile(volume_name, dir_entry_ptr->NAME);
                *dir_entry_ptr->ATTRIBUTE = MFS_ATTR_VOLUME_NAME;
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;
            }
        }
    }

    MFS_unlock(drive_ptr,TRUE);
    return( error_code );
}  

#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Get_volume
* Returned Value   : error_code
* Comments  :   Gets the drive volume. Assumes the drive is NOT locked.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Get_volume
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] pointer to the file struct returned by fopen("MFS...:",..); */
    char_ptr                volume_name /*[OUT] name of the volune */
    )
{
    MFS_DRIVE_STRUCT_PTR       drive_ptr;
    MFS_DIR_ENTRY_PTR          dir_entry_ptr;
    _mfs_error                 error_code;
    uint_32                    vol_cluster, vol_index, i;
    char_ptr                   disk_vol_name;
    uint_32                    prev_cluster= CLUSTER_INVALID; 

    if ( volume_name == NULL )
    {
        return( MFS_INVALID_PARAMETER );
    }

    error_code = MFS_lock_dos_disk(mfs_fd_ptr, &drive_ptr);

    if ( !error_code )
    {
        vol_cluster = ROOT_CLUSTER(drive_ptr);
        vol_index = 0;
        dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, NULL, &vol_cluster, &vol_index, &prev_cluster, MFS_ATTR_VOLUME_NAME, &error_code); 

        if ( dir_entry_ptr )
        {
            disk_vol_name = dir_entry_ptr->NAME;
            for ( i = 0 ; i < SFILENAME_SIZE - 1; i++ )
            {
                *volume_name++ = *disk_vol_name++;
            }  
        }
        *volume_name = '\0'; 
    }

    MFS_unlock(drive_ptr,FALSE);   
    return( error_code );
}  

/* EOF */
