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
* $FileName: mfs_dir_entry.c$
* $Version : 3.8.12.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the functions that are used to manipulate FAT directory
*   entries.
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
* Function Name    :  MFS_Create_directory_entry
* Returned Value   :  Pointer to the directory entry
* Comments  :
*   Try to create a new file.  Search for an unused directory entry, setup the
*   entry and write back the directory.
*END*---------------------------------------------------------------------*/

MFS_DIR_ENTRY_PTR MFS_Create_directory_entry
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    char_ptr        filename_ptr,       /*[IN] pointer to the file's name */
    char            attribute,          /*[IN] attribute to be assigned to the file */
    uint_32_ptr     dir_cluster_ptr,    /*[IN/OUT] indicates cluster in which the entry was put.*/
    uint_32_ptr     dir_index_ptr,      /*[IN/OUT] index of the location of new file within the directory */
    uint_32_ptr     error_ptr           /*[IN/OUT] error_return_address */
    )
{
    MFS_DIR_ENTRY_PTR     dir_entry_ptr, dir_entry_ptr1;
    MFS_DIR_ENTRY_PTR     temp_entry_ptr;
    TIME_STRUCT           time;
    DATE_STRUCT           clk_time;
    uint_32               dir_index, saved_index, temp_index;
    uint_32               entry_cluster, saved_cluster, temp_cluster;
    uint_32               needed_entries, i;
    int_32                length;
    boolean               found_all = FALSE;
    char                  temp_name[SFILENAME_SIZE+1], short_name[SFILENAME_SIZE+1];  
    uchar                 checksum = 0;
    uint_32               prev_cluster= CLUSTER_INVALID, saved_prev_cluster, temp_prev_cluster; 

    dir_entry_ptr = NULL;
    entry_cluster = *dir_cluster_ptr;
    dir_index = 0;

    /*
    ** Check for duplicate file
    */
    if ( filename_ptr == NULL )
    {
        *error_ptr = MFS_INVALID_PARAMETER;
    }
    else if ( *filename_ptr == '\0' )
    {
        *error_ptr = MFS_FILE_NOT_FOUND;
    }
#if MFSCFG_READ_ONLY_CHECK
    else if (MFS_is_read_only (NULL, drive_ptr))
    {
        *error_ptr = MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif
    else if ( (dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, filename_ptr,
        &entry_cluster, &dir_index, &prev_cluster, MFS_ATTR_ANY, error_ptr)) != NULL )
    {
        *error_ptr = MFS_FILE_EXISTS;
    }
    else
    {
        /*
        ** Search for an empty slot
        */

        dir_index = 0;
        dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, "", &entry_cluster, &dir_index, &prev_cluster, attribute, error_ptr); 


        if ( MFS_Dirname_valid(filename_ptr) )
        {
            found_all = TRUE;
        }

        if ( dir_entry_ptr ==  NULL && !*error_ptr )
        {
            found_all = TRUE;
        }

        /* 
        ** Save it now because we might not go into the while loop
        ** If we don't go into while, we lose original values when these 
        ** variables are restored after the while loop
        */
        saved_cluster = entry_cluster;
        saved_index = dir_index;

        while ( !found_all )
        {
            /* Calculate the amount of extra entries needed for LFN's */
            saved_cluster = entry_cluster;
            saved_index = dir_index;

            for ( needed_entries = (strlen(filename_ptr) + 12) / 13;  needed_entries >= 1 && !found_all; needed_entries-- )
            {
                *error_ptr = MFS_Increment_dir_index(drive_ptr, &entry_cluster, &dir_index, NULL);  
                if ( entry_cluster == CLUSTER_EOF && !*error_ptr )
                {
                    /* This means the LFN will span a cluster. */
                    found_all = TRUE;
                    break;
                }
                temp_index = dir_index;
                temp_cluster = entry_cluster;
                temp_entry_ptr = MFS_Find_directory_entry(drive_ptr, "", &entry_cluster, &dir_index, &prev_cluster, MFS_ATTR_ANY,error_ptr); 
                if ( *error_ptr )
                {
                    return NULL;
                }
                if ( !temp_entry_ptr )
                {
                    found_all = TRUE;
                    dir_entry_ptr = NULL;
                    break;
                }
                else if ( dir_index == temp_index && temp_cluster == entry_cluster )
                {
                    continue;
                }
                else
                {
                    break;
                }        
            }  

            if ( needed_entries == 0 )
            {
                found_all = TRUE;
            }

            if ( found_all )
            {
                dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, "", &saved_cluster, &saved_index, &saved_prev_cluster, MFS_ATTR_ANY,error_ptr); 
            }
        }  

        entry_cluster = saved_cluster;
        dir_index = saved_index;


        if ( dir_entry_ptr ==  NULL && !*error_ptr )
        {
            /*
            ** Empty spot not found... get a new cluster and use the first entry.
            */
            dir_index = 0;
            if ( entry_cluster == 0 )
            {
                *error_ptr = MFS_ROOT_DIR_FULL;
                return(NULL);
            }
            else
            {
                *error_ptr = MFS_Add_cluster_to_chain(drive_ptr, entry_cluster, &entry_cluster);
                if ( *error_ptr == MFS_NO_ERROR )
                {
                    *error_ptr = MFS_Clear_cluster(drive_ptr, entry_cluster);
                    if ( *error_ptr )
                    {
                        return(NULL);
                    }
                    dir_entry_ptr = (pointer) drive_ptr->DIR_SECTOR_PTR;
                    *error_ptr = MFS_Write_back_fat(drive_ptr);
                    if ( *error_ptr )
                    {
                        return(NULL);
                    }
                }
            }  
        }
    }  

    if ( *error_ptr == MFS_NO_ERROR )
    {
        /* At this point we have one of the following cases 
        **
        ** 1. We have a normal 8.3 filename and an empty slot for it
        ** 2. We have a LFN, and a slot which has enough consecutive free slots
        **    to store the LFN, followed by the actual entry
        */

        /* If the file is a normal 8.3 file */
        if ( MFS_Dirname_valid(filename_ptr) )
        {
            _mem_zero(dir_entry_ptr, sizeof (MFS_DIR_ENTRY));            
            htodc(dir_entry_ptr->ATTRIBUTE, attribute);
            MFS_Expand_dotfile(filename_ptr, dir_entry_ptr->NAME);

            _time_get(&time);
            _time_to_date(&time, &clk_time);
            NORMALIZE_DATE(&clk_time);
            htods(dir_entry_ptr->TIME, PACK_TIME(clk_time));
            htods(dir_entry_ptr->DATE, PACK_DATE(clk_time));   
            drive_ptr->DIR_SECTOR_DIRTY = TRUE;
        }
        else
        {
            /* Case where the file is a LFN */   
            length = strlen(filename_ptr);

            /* Get the 8.3 name and calculate the checksum */
            temp_index = 0;
            temp_cluster = *dir_cluster_ptr;
            *error_ptr = MFS_lfn_to_sfn(filename_ptr, temp_name);

            if ( !*error_ptr )
            {
                do
                {
                    dir_entry_ptr1 = MFS_Find_directory_entry(drive_ptr, temp_name, &temp_cluster, &temp_index, &temp_prev_cluster, MFS_ATTR_ANY, error_ptr); 
                    if ( !*error_ptr && dir_entry_ptr1 != NULL )
                    {
                        MFS_increment_lfn(temp_name);
                        temp_index = 0;       
                    }
                } while ( !*error_ptr && dir_entry_ptr1 != NULL );
                dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, "", &entry_cluster, &dir_index, &prev_cluster, attribute, error_ptr); 

                /*
                ** The memory should be cleared after finding the directory entry.
                */
                _mem_zero(dir_entry_ptr, sizeof (MFS_DIR_ENTRY));

                /* Setup the final slot */
                ((MFS_LNAME_ENTRY_PTR) dir_entry_ptr)->ID |= MFS_LFN_END;
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                MFS_Expand_dotfile(temp_name,short_name);
                checksum = MFS_lfn_checksum(short_name);
            }

            while ( length && !*error_ptr )
            {
                i = length - ((length -1) % 13 + 1);
                *error_ptr = MFS_lfn_name_to_entry(filename_ptr + i, (MFS_LNAME_ENTRY_PTR) dir_entry_ptr);

                /* Set the entry number, the checksum value and the attribute */
                ((MFS_LNAME_ENTRY_PTR) dir_entry_ptr)->ID |= (length + 12) / 13;
                ((MFS_LNAME_ENTRY_PTR) dir_entry_ptr)->ALIAS_CHECKSUM= checksum;
                ((MFS_LNAME_ENTRY_PTR) dir_entry_ptr)->ATTR = MFS_ATTR_LFN;
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                length -= (length - i);
                if ( length < 0 )
                {
                    length = 0;
                }

                if ( length && !*error_ptr )
                {

                    dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, "", &entry_cluster, &dir_index, &prev_cluster, attribute, error_ptr); 
                    if ( dir_entry_ptr ==  NULL && !*error_ptr )
                    {
                        /* No empty spots... We need to get a new cluster */
                        dir_index = 0; 
                        if ( entry_cluster == 0 )
                        {
                            *error_ptr = MFS_ROOT_DIR_FULL; 
                            return(NULL); 
                        }
                        else
                        {
                            *error_ptr = MFS_Add_cluster_to_chain( drive_ptr, entry_cluster, &entry_cluster); 
                            if ( *error_ptr == MFS_NO_ERROR )
                            {
                                *error_ptr = MFS_Clear_cluster(drive_ptr, entry_cluster); 
                                if ( *error_ptr )
                                {
                                    return(NULL); 
                                }
                                dir_entry_ptr = (pointer) drive_ptr->DIR_SECTOR_PTR; 
                                *error_ptr = MFS_Write_back_fat(drive_ptr); 
                                if ( *error_ptr )
                                {
                                    return(NULL); 
                                }
                            }
                        }   
                    }
                    _mem_zero(dir_entry_ptr, sizeof (MFS_DIR_ENTRY));
                    drive_ptr->DIR_SECTOR_DIRTY = TRUE;
                }
            }  

            /* LFN is written, so write the actual entry */
            if ( !*error_ptr )
            {
                dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, "", &entry_cluster, &dir_index, &prev_cluster, attribute, error_ptr); 
                if ( dir_entry_ptr == NULL && !*error_ptr )
                {
                    dir_index = 0; 
                    if ( entry_cluster == 0 )
                    {
                        *error_ptr = MFS_ROOT_DIR_FULL; 
                        return(NULL); 
                    }
                    else
                    {
                        *error_ptr = MFS_Add_cluster_to_chain( drive_ptr, entry_cluster, &entry_cluster); 
                        if ( *error_ptr == MFS_NO_ERROR )
                        {
                            *error_ptr =  MFS_Clear_cluster(drive_ptr, entry_cluster); 
                            if ( *error_ptr )
                            {
                                return(NULL); 
                            }
                            dir_entry_ptr = (pointer) drive_ptr->DIR_SECTOR_PTR; 
                            *error_ptr = MFS_Write_back_fat(drive_ptr); 
                            if ( *error_ptr )
                            {
                                return(NULL); 
                            }
                        }
                    }   
                }

                _mem_zero(dir_entry_ptr, sizeof (MFS_DIR_ENTRY));
                htodc(dir_entry_ptr->ATTRIBUTE, attribute);
                MFS_Expand_dotfile(temp_name, dir_entry_ptr->NAME);

                _time_get(&time);
                _time_to_date(&time, &clk_time);
                NORMALIZE_DATE(&clk_time);
                htods(dir_entry_ptr->TIME, PACK_TIME(clk_time));
                htods(dir_entry_ptr->DATE, PACK_DATE(clk_time));   
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;
            }
        }  

        if ( *error_ptr )
        {
            return(NULL);
        }
    }

    *dir_cluster_ptr = entry_cluster;
    *dir_index_ptr = dir_index;
    return(dir_entry_ptr);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Update_entry
* Returned Value   : error_code
* Comments  :  Put the copy of the entry in the buffer, then update disk
*              image.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Update_entry
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    MFS_HANDLE_PTR          handle      /*[IN] Update this file */
    )
{
    uint_32              dir_index;
    uint_32              dir_cluster;
    _mfs_error           error_code = MFS_NO_ERROR;
    MFS_DIR_ENTRY_PTR    dir_entry_ptr;
    uint_32              prev_cluster; 

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (NULL, drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    if ( handle == NULL )
    {
        error_code = MFS_INVALID_HANDLE;
    }
    else
    {
        dir_cluster =  handle->DIR_CLUSTER;
        dir_index   =  handle->DIR_INDEX;

        dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, NULL, &dir_cluster, &dir_index, &prev_cluster, MFS_ATTR_ANY, &error_code); 
        if ( dir_entry_ptr == NULL && !error_code )
        {
            error_code = MFS_FILE_NOT_FOUND;
        }
        else
        {
            _mem_copy(&handle->DIR_ENTRY, dir_entry_ptr, sizeof(MFS_DIR_ENTRY));
            drive_ptr->DIR_SECTOR_DIRTY = TRUE;
        }  

    }  

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Create_entry_slave
* Returned Value   : DIR_ENTRY_PTR to the buffer on disk
* Comments  :
*      Create a file. If a file exists, and the Overwrite flag is FALSE,
*      NULL is returned.
*      When an entry is found and overwritten, the error is set to
*      MFS_FILE_EXISTS, but the returned PTR points to a valid location.
*      If error is NO_ERROR, a new entry has been created.
*      The attr of the new entry has to be compatible with the old one,
*      or WRITE_FAULT is returned.
*      If the old one is Read only, ACCESS_DENIED is returned.
*      The semaphore is assumed locked and the fat up to date
*
*END*---------------------------------------------------------------------*/

MFS_DIR_ENTRY_PTR MFS_Create_entry_slave
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,      /*[IN] the drive on which to operate */
    uchar                    attr,          /*[IN] attribute to be given to the new file */
    char_ptr                 pathname,      /*[IN] directory and file name to be given to the new file */
    uint_32_ptr              cluster_ptr,   /*[OUT] cluster # in the directory where the entry was created */
    uint_32_ptr              index_ptr,     /*[OUT] index # in the directory where the entry was created*/
    _mfs_error_ptr           error_ptr,     /*[IN/OUT] error code is written to this address */
    boolean                  overwrite      /*[IN]  if TRUE, we will overwrite an existing entry w/the same name */
    )
{
    MFS_DIR_ENTRY_PTR   dir_entry_ptr;
    char_ptr            temp_dirname;
    char_ptr            temp_filename;
    uchar               at;
    uint_32             dir_cluster;
    uint_32             dir_index;
    uint_32             vol_cluster,
        vol_index;
    _mfs_error          error_code;
    uint_32             prev_cluster; 

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (NULL, drive_ptr))
    {
        if ( error_ptr )
        {
            *error_ptr = MFS_DISK_IS_WRITE_PROTECTED;
        }
        return NULL;
    }
#endif

    dir_index   = 0;
    dir_cluster = drive_ptr->CUR_DIR_CLUSTER;
    dir_entry_ptr = NULL;

    error_code = MFS_alloc_2paths(&temp_dirname,&temp_filename);
    if ( error_code )
    {
        return( NULL );
    }

    error_code = MFS_Parse_pathname(temp_dirname, temp_filename, pathname);
    if ( error_code == MFS_NO_ERROR )
    {
        dir_cluster = MFS_Find_directory(drive_ptr, temp_dirname, dir_cluster);

        if ( dir_cluster == CLUSTER_INVALID )
        {
            error_code = MFS_PATH_NOT_FOUND;
        }
        else
        {
            if ( MFS_is_valid_lfn(temp_filename) )
            {
                if ( attr & MFS_ATTR_VOLUME_NAME )
                {
                    /*
                    ** If creating a volume label, it must be in the root
                    ** directory.
                    */
                    if ( dir_cluster == 0 )
                    {
                        vol_cluster = 0;
                        vol_index   = 0;
                        dir_entry_ptr =  MFS_Find_directory_entry(drive_ptr, NULL, &vol_cluster, 
                            &vol_index, &prev_cluster, MFS_ATTR_VOLUME_NAME, &error_code); 
                        if ( dir_entry_ptr )
                        {
                            error_code = MFS_ALREADY_ASSIGNED;
                        }
                    }
                    else
                    {
                        error_code = MFS_ACCESS_DENIED;
                    }  
                }

                if ( error_code == MFS_NO_ERROR )
                {
                    dir_entry_ptr = MFS_Create_directory_entry(drive_ptr, temp_filename, attr, &dir_cluster, &dir_index, &error_code);

                    /* if dir_entry_ptr == NULL
                    ** we couldn't create a new entry. 
                    ** if not NULL, it may be a brand new entry (MFS_NO_ERROR) or
                    ** an old one (MFS_FILE_EXISTS)
                    */

                    if ( dir_entry_ptr != NULL )
                    {
                        at = dtohc(dir_entry_ptr->ATTRIBUTE);

                        if ( (error_code != MFS_NO_ERROR) && (overwrite == FALSE) )
                        {
                            dir_entry_ptr = NULL;
                        }
                        else if ( (error_code == MFS_FILE_EXISTS) && ((at & MFS_ATTR_DIR_NAME) != (attr & MFS_ATTR_DIR_NAME)) 
                            || ((at & MFS_ATTR_VOLUME_NAME) !=  (attr & MFS_ATTR_VOLUME_NAME)) )
                        {
                            error_code = MFS_WRITE_FAULT;
                            dir_entry_ptr = NULL;
                        }
                        else if ( at & MFS_ATTR_READ_ONLY && error_code != 
                            MFS_NO_ERROR )
                        {
                            /*
                            ** If not freshly created
                            */
                            error_code = MFS_ACCESS_DENIED;
                            dir_entry_ptr = NULL;
                        }
                    }
                    *cluster_ptr = dir_cluster;
                    *index_ptr   = dir_index;
                }
            }
            else
            {
                error_code = MFS_INVALID_PARAMETER;
                dir_entry_ptr = NULL;
            }  
        }  
    }

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    MFS_free_path(temp_filename);
    MFS_free_path(temp_dirname);

    return(dir_entry_ptr);
}  

#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Increment_dir_index
* Returned Value   :  uint_32 error_code
* Comments  :
*END*---------------------------------------------------------------------*/

_mfs_error  MFS_Increment_dir_index
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    uint_32_ptr    cluster_ptr,         /*[IN/OUT]  the initial/next cluster # */
    uint_32_ptr    index_ptr,           /*[IN/OUT]  the initial/next index */
    uint_32_ptr    prev_cluster_ptr     /*[IN/OUT]  the prev cluster # */
    )
{
    uint_32        index;
    uint_32        cluster;
    _mfs_error     error_code;

    error_code = MFS_NO_ERROR;
    index   = *index_ptr;
    cluster = *cluster_ptr;

    index++;

    if ( !(index & (drive_ptr->ENTRIES_PER_SECTOR-1)) )
    {
        /*
        ** if the index count LSB's wrapped to 0 (new sector)
        */
        if ( cluster != 0 )
        {
            if ( INDEX_TO_SECTOR (index) >= drive_ptr->BPB.SECTORS_PER_CLUSTER )
            {
                /*
                ** If we are over the size of a cluster
                */

                error_code = MFS_get_cluster_from_fat(drive_ptr,cluster, &cluster);
                if ( error_code == MFS_NO_ERROR )
                {
                    index = 0;
                }
            }
        }
        else
        {
            if ( index >= drive_ptr->BPB.ROOT_ENTRIES )
            {
                index = 0;
                cluster = CLUSTER_INVALID;
            }
        }  
    }

    if ( (cluster != *cluster_ptr) && (cluster != CLUSTER_INVALID) )
    {
        if ( prev_cluster_ptr != NULL )
        {
            *prev_cluster_ptr = *cluster_ptr;
        }
    }

    *index_ptr   = index;
    *cluster_ptr = cluster;

    return(error_code);
}
