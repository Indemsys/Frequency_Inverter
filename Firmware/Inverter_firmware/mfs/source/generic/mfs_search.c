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
* $FileName: mfs_search.c$
* $Version : 3.8.9.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains functions that search for specific directory entries
*   on the disk.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Find_directory_entry
* Returned Value   :  pointer to a directory entry
* Comments  :  Search the directory starting at start_cluster for an entry
*    with the name of filename satisfying the attributes specified.
*   IF file_ptr = NULL, it returns the first entry at clust/index.
*   If file_ptr = "", it returns the next empty or deleted entry.
*END*---------------------------------------------------------------------*/

MFS_DIR_ENTRY_PTR MFS_Find_directory_entry
    (
    MFS_DRIVE_STRUCT_PTR  drive_ptr,            /*[IN] drive context */
    char_ptr              file_ptr,             /*[IN] specific name to search for, if NULL then first dir entry */
    uint_32_ptr           start_cluster_ptr,    /*[IN] start searching in this cluster
                                                **[OUT] set to the cluster number in which search was satisfied */
    uint_32_ptr           dir_index_ptr,        /*[IN] start searching at this directory entry
                                                **[OUT] if entry is found the index of the next entry is returned */
    uint_32_ptr           prev_cluster_ptr,     /* [IN] set to the cluster number previous to start_cluster
                                                ** [OUT] set to the cluster number previous to *dir_cluster_ptr */
    uchar                 attribute,            /*[IN] search attribute, as per Find_first_file */
    _mfs_error_ptr           error_ptr          /* The pointer carries the error infomation */
    )
{
    MFS_DIR_ENTRY_PTR     dir_entry_ptr = NULL;
    char_ptr              lfn_ptr = NULL;
    boolean               found;
    boolean               lfn_flag;
    boolean               maybe = FALSE;
    boolean               found_lfn = FALSE;
    uint_32               current_cluster, current_index;
    _mfs_error            error_code;
    uint_32               lfn_len = 0;  
    uint_16               entries_per_sector;
    uint_16               k, i;
    char                  lfn_entry[SFILENAME_SIZE + 1];
    char                  fs_file[SFILENAME_SIZE + 1]; 
    uint_32               prev_cluster = *prev_cluster_ptr; 
    uint_32               dirname = 0;


    error_code = MFS_NO_ERROR;

    entries_per_sector = drive_ptr->ENTRIES_PER_SECTOR;
    current_cluster = *start_cluster_ptr;
    current_index = *dir_index_ptr;

    found = FALSE;
    lfn_flag = FALSE;

    /* If the name is a LFN, it will be treated differently */
    if ( file_ptr && *file_ptr && !MFS_Dirname_valid(file_ptr) )
    {
        lfn_flag = TRUE;
        lfn_len = _strnlen(file_ptr, PATHNAME_SIZE);
        /* Get pointer to the begining of last block of 13 chars in string */
        lfn_ptr = file_ptr + ((lfn_len - 1) / 13) * 13;   
        maybe = TRUE;
        found_lfn = FALSE;
        /* We also need special treatement for a directory name */
        if ( attribute == (ATTR_EXCLUSIVE | MFS_ATTR_DIR_NAME) )
        {
            attribute = MFS_ATTR_LFN;
            dirname = 1;
        }
    }
    else if ( file_ptr && *file_ptr )
    {
        MFS_Expand_dotfile(file_ptr, fs_file);
    }
    /*
    ** Search in all clusters within this directory
    */

    do
    {
        /*
        ** Search in all sectors within this cluster
        */
        for ( k = (uint_16) INDEX_TO_SECTOR(current_index); 
            ((current_cluster == 0) || (k < drive_ptr->BPB.SECTORS_PER_CLUSTER)) && found == FALSE && current_cluster != CLUSTER_EOF;
            k++ )
        {
            dir_entry_ptr = MFS_Read_directory_sector(drive_ptr,current_cluster, k, &error_code);
            if ( dir_entry_ptr == NULL )
            {
                break;
            }

            /*
            ** Search in all entries within this sector
            */
            for ( i = (uint_16) INDEX_TO_OFFSET(current_index), dir_entry_ptr += i ; i < entries_per_sector && found == FALSE; i++ )
            {
                if ( *dir_entry_ptr->NAME == '\0' ) /* if NEVER USED entry */
                {
                    if ( file_ptr )      /* If not NULL                    */
                    {
                        if ( !*file_ptr )      /* but ""                         */
                        {
                            /* we found it */
                            found = TRUE;
                            break;
                        }
                    }
                    return(NULL);       /* Anyway, stop here-never used entry */
                                        /* if the entry is relevant at all    */

                }
                else if ( (uchar) *dir_entry_ptr->NAME == MFS_DEL_FILE )
                {
                    /* If DELETED */ 
                    if ( file_ptr )      /* If not NULL   */
                    {
                        if ( !*file_ptr )      /* but ""  */
                        {
                            found = TRUE;       /* we found it  */
                            break;
                        }
                    }
                }
                else                        /* If REGULAR ENTRY */
                {
                    if ( file_ptr == NULL || *file_ptr )
                    {
                        if ( MFS_Attribute_match(dtohc(dir_entry_ptr->ATTRIBUTE), 
                            attribute) == TRUE )
                        {
                            if ( !file_ptr )
                            {
                                found = TRUE;
                                break;
                            }
                            else if ( lfn_flag ) /* Searching for a long name */
                            {
                                if ( found_lfn )
                                {
                                    found = TRUE;
                                    break;
                                }
                                if ( !MFS_Attribute_match(dtohc(dir_entry_ptr->ATTRIBUTE),
                                    MFS_ATTR_LFN) )
                                {
                                    maybe = TRUE; /* Reset maybe */
                                    dir_entry_ptr++;
                                    current_index++;
                                    continue;   /* Not an LFN entry, skip it */ 
                                }
                                if ( !maybe )
                                {
                                    dir_entry_ptr++;
                                    current_index++;
                                    continue;
                                }

                                MFS_lfn_extract((MFS_LNAME_ENTRY_PTR)dir_entry_ptr, lfn_entry);
                                if ( strncmp(lfn_entry, lfn_ptr, 13) )
                                {
                                    lfn_ptr = file_ptr + ((lfn_len - 1) / 13) * 13; /* reset ptr */
                                    maybe = FALSE;
                                    dir_entry_ptr++;
                                    current_index++;
                                    continue; /* Strings don't match */
                                }
                                if ( lfn_ptr > file_ptr )
                                {
                                    lfn_ptr -= 13;
                                }
                                else if ( lfn_ptr == file_ptr )
                                {
                                    /* We found the name. The next entry contains the file info */
                                    found_lfn = TRUE;
                                    if ( dirname )
                                    {
                                        attribute = ATTR_EXCLUSIVE | MFS_ATTR_DIR_NAME; 
                                    }
                                }
                                else
                                {
                                    /* Pointer is a begining of string, and no match */
                                    maybe = FALSE;
                                    lfn_ptr = file_ptr + ((lfn_len - 1) / 13) * 13; /* reset ptr */
                                }                            
                            }
                            else /* Searching for a short name */
                            {
                                found = MFS_Wildcard_match(fs_file, dir_entry_ptr->NAME);
                            }  
                        }
                        else
                        {
                            if ( lfn_flag )
                            {
                                attribute = MFS_ATTR_LFN;
                                lfn_ptr = file_ptr + ((lfn_len - 1) / 13) * 13; /* reset ptr */
                                maybe = TRUE;
                                found_lfn = FALSE;
                            }
                        }  
                    }
                }  

                if ( found == FALSE )
                {
                    dir_entry_ptr++;
                    current_index++;
                }
            }  
        }  

        if ( found == FALSE )
        {
            error_code = MFS_Increment_dir_index(drive_ptr, &current_cluster, &current_index, &prev_cluster); 
            if ( error_code )
            {
                break;
            }
        }

    } while ( found == FALSE && dir_entry_ptr && current_cluster != CLUSTER_EOF && current_cluster != CLUSTER_INVALID );

    if ( found == FALSE )
    {
        dir_entry_ptr      = NULL;
    }
    else
    {
        *start_cluster_ptr = current_cluster;
        *prev_cluster_ptr = prev_cluster;  
        *dir_index_ptr = current_index;
    }  

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    return(dir_entry_ptr);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Find_directory
* Returned Value   :  starting_cluster of the specified directory
* Comments  :  Search through the file structure to find the specified
*  directory.  Will search either from the current directory or from the
*  root directory.  Returns the starting cluster number of the specified
*  directory.  If the specified directory cannot be found, then 
*  CLUSTER_INVALID  is returned.
*
*END*---------------------------------------------------------------------*/
uint_32 MFS_Find_directory
    (
    MFS_DRIVE_STRUCT_PTR  drive_ptr,    /*[IN] drive context */
    char_ptr              path_ptr,     /*[IN] specific directory to search for */
    uint_32               first_cluster /*[IN] start searching in this cluster, used for a relative search*/
    )
{
    MFS_DIR_ENTRY_PTR     dir_entry_ptr;
    char_ptr              directory;
    uint_32               current_cluster;
    uint_32               dir_index;
    _mfs_error            error_code;
    boolean               flag;
    uint_32               prev_cluster = CLUSTER_INVALID; 
    uchar                 attribute;


    if ( MFS_alloc_path(&directory)!= MFS_NO_ERROR )
    {
        return( CLUSTER_INVALID );
    }

    if ( *path_ptr == '\\' || *path_ptr == '/' )
    {
        current_cluster = ROOT_CLUSTER(drive_ptr);
        path_ptr++;
    }
    else
    {
        current_cluster = first_cluster;
    }  

    while ( path_ptr )
    {
        if ( *path_ptr )
        {
            path_ptr = MFS_Parse_next_filename(path_ptr, directory);

            if ( !MFS_lfn_dirname_valid(directory) )
            {
                current_cluster = CLUSTER_INVALID;
                break;
            }

            flag = FALSE;
            if ( current_cluster == 0 )  flag = TRUE;
            if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
            {
                if ( current_cluster == drive_ptr->BPB32.ROOT_CLUSTER )
                {
                    flag = TRUE;
                }
            }

            if ( flag )
            {
                /*
                ** Special treatment for '.' and '..' in root directory
                */
                dir_index = MFS_Is_dot_directory(directory);
                if ( dir_index == 1 )
                {
                    /* Return the value of the root cluster */
                    MFS_free_path(directory);
                    return ROOT_CLUSTER(drive_ptr);
                }
                else if ( dir_index == 2 )
                {
                    MFS_free_path(directory);
                    return(CLUSTER_INVALID);
                }
            }

            dir_index = 0;

            dir_entry_ptr = MFS_Find_directory_entry( drive_ptr, directory,
                &current_cluster, &dir_index,  &prev_cluster, (~ATTR_EXCLUSIVE),  
                &error_code);

            if ( dir_entry_ptr == NULL )
            {
                current_cluster = CLUSTER_INVALID;
                break;
            }
            else
            {
                attribute = dtohc(dir_entry_ptr->ATTRIBUTE);

                if ( attribute & MFS_ATTR_DIR_NAME )
                {
                    current_cluster = clustoh(dir_entry_ptr->HFIRST_CLUSTER, dir_entry_ptr->LFIRST_CLUSTER);
                    if ( current_cluster == 0 && drive_ptr->FAT_TYPE == MFS_FAT32 )
                    {
                        current_cluster = drive_ptr->BPB32.ROOT_CLUSTER;
                    }
                }
                else
                {
                    // Found an entry, but it is not a directory
                    MFS_free_path(directory);
                    return(CLUSTER_INVALID);
                }
            }  
        }
        else
        {
            break;
        }  
    }  
    MFS_free_path(directory);

    return(current_cluster);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Find_entry_on_disk
* Returned Value   : pointer to the entry's MFS_DIR_ENTRY in the
*                    sector_buffer
* Comments  :  Find an entry anywhere on disk (exact filename).
*
*END*---------------------------------------------------------------------*/

MFS_DIR_ENTRY_PTR MFS_Find_entry_on_disk
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,          /*[IN] drive context */
    char _PTR_              pathname,           /*[IN] pathname of the file */
    _mfs_error_ptr             error_ptr,       /*[IN/OUT] resulting error code is written to this address*/
    uint_32_ptr             dir_cluster_ptr,    /*[OUT] the cluster in which the dir entry is located */
    uint_32_ptr             dir_index_ptr,      /*[OUT] the index within the dir cluster where the entry is located */
    uint_32_ptr             prev_cluster_ptr    /*[OUT] the cluster previous to dir cluster */    
    )
{
    MFS_DIR_ENTRY_PTR    dir_entry_ptr;
    char_ptr             temp_dirname;
    char_ptr             temp_filename;
    uint_32              first_cluster;

    *error_ptr = MFS_alloc_2paths(&temp_dirname,&temp_filename);
    if ( *error_ptr != MFS_NO_ERROR )
    {
        return( NULL );
    }

    *error_ptr = MFS_NO_ERROR;
    dir_entry_ptr = NULL;

    if ( pathname )
    {
        MFS_Parse_pathname(temp_dirname, temp_filename, pathname);

        /*
        ** Is this a relative or an absolute file search ?
        */
        if ( *pathname == '\\' || *pathname == '/' )
        {
            first_cluster = ROOT_CLUSTER(drive_ptr);
        }
        else
        {
            first_cluster = drive_ptr->CUR_DIR_CLUSTER;
        }  

        /* first_cluster will now store the current_cluster # */
        first_cluster = MFS_Find_directory(drive_ptr, temp_dirname, first_cluster);
        *prev_cluster_ptr = CLUSTER_INVALID; 

        if ( first_cluster == CLUSTER_INVALID )
        {
            *error_ptr = MFS_PATH_NOT_FOUND;
        }
        else
        {
            *dir_index_ptr = 0;
            *dir_cluster_ptr = first_cluster;
            /*
            ** Checking for the superset of filename chars will allow
            ** finding something like \mydir\..
            */
            if ( !MFS_lfn_dirname_valid(temp_filename) )
            {
                dir_entry_ptr = NULL;
                *error_ptr = MFS_INVALID_PARAMETER;
            }
            else
            {
                dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, temp_filename,
                    dir_cluster_ptr, dir_index_ptr, prev_cluster_ptr, MFS_ATTR_ANY, error_ptr);  
                if ( dir_entry_ptr == NULL && !*error_ptr )
                {
                    *error_ptr = MFS_FILE_NOT_FOUND;
                }
            }  
        }  
    }
    else
    {
        *error_ptr = MFS_INVALID_PARAMETER;
    }  
    MFS_free_path(temp_dirname);
    MFS_free_path(temp_filename);

    return(dir_entry_ptr);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Find_next_slave
* Returned Value   : error_code
* Comments  :   Find the next file that matches the input criteria. Must
*    follow a Find_first_file and the search data block from the search
*    must be supplied.
*    This is called by find_first and find_next, and assumes the drive 
*    semaphore is obtained.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Find_next_slave
    (
    MFS_DRIVE_STRUCT_PTR  drive_ptr,    /*[IN] drive context */
    pointer   search_next_ptr           /*[IN] address of search data block indicating the current criteria and the results 
                                        ** of the last search results of this search are placed in this data block */
    )
{
    MFS_SEARCH_DATA_PTR        transfer_ptr;
    MFS_INTERNAL_SEARCH_PTR    internal_search_ptr;
    MFS_DIR_ENTRY_PTR          dir_entry_ptr;
    _mfs_error                 error_code;
    uint_32                    len;
    boolean                    found;
    boolean                    match_all;
    boolean                    eightdotthree = FALSE;
    char_ptr                   lfn;
    char                       sname[SFILENAME_SIZE+1]; 
    uint_32                    dotfile = 0;
    MFS_DIR_ENTRY  saved_dir_entry;  

    transfer_ptr = (MFS_SEARCH_DATA_PTR) search_next_ptr;
    error_code = MFS_NO_ERROR;
    found = FALSE;

    if ( transfer_ptr )
    {
        internal_search_ptr = &transfer_ptr->INTERNAL_SEARCH_DATA;

        if ( MFS_alloc_path(&lfn)!= MFS_NO_ERROR )
        {
            return MFS_INSUFFICIENT_MEMORY;
        }

        match_all = MFS_Check_search_string_for_all(internal_search_ptr->SRC_PTR);
        if ( !match_all )
        {
            dotfile = MFS_Is_dot_directory(internal_search_ptr->SRC_PTR);
            if ( dotfile == 0 )
            {
                eightdotthree = MFS_Check_search_string_for_8dot3( internal_search_ptr->SRC_PTR);
            }
        }
        do
        {
            dir_entry_ptr = MFS_Find_directory_entry(drive_ptr, NULL, &internal_search_ptr->CURRENT_CLUSTER, &internal_search_ptr->DIR_ENTRY_INDEX,
                &internal_search_ptr->PREV_CLUSTER, internal_search_ptr->ATTRIBUTE & (~ATTR_EXCLUSIVE), &error_code);

            if ( dir_entry_ptr == NULL && !error_code )
            {
                error_code = MFS_FILE_NOT_FOUND;
                break;
            }

            if ( internal_search_ptr->CURRENT_CLUSTER == CLUSTER_INVALID )
            {
                error_code = MFS_FILE_NOT_FOUND;
                break;
            }

            if ( dir_entry_ptr == NULL )
            {
                break;
            }
            saved_dir_entry=*dir_entry_ptr; 

            /* Make sure the entry is not an LFN entry */
            if ( *dir_entry_ptr->ATTRIBUTE != MFS_ATTR_LFN )
            {
                MFS_Compress_nondotfile (dir_entry_ptr->NAME, sname);

                if ( match_all )
                {
                    found = TRUE;
                }
                else
                {
                    if ( dotfile != 0 )
                    {
                        if ( dotfile == 1 )
                        {
                            found = internal_search_ptr->FULLNAME[0] == dir_entry_ptr->NAME[0];
                        }
                        else if ( dotfile == 2 )
                        {
                            found = (internal_search_ptr->FULLNAME[0] == dir_entry_ptr->NAME[0]) &&
                                    (internal_search_ptr->FULLNAME[1] == dir_entry_ptr->NAME[1]);
                        }
                        else
                        {
                            found = FALSE; /* This shouldn't happen */
                        }
                    }
                    else if ( eightdotthree )
                    {
                        found = MFS_Wildcard_match(internal_search_ptr->FILENAME, dir_entry_ptr->NAME);
                    }
                    else
                    {
                        if ( MFS_get_lfn_dir_cluster(drive_ptr, search_next_ptr, sname, lfn) == MFS_NO_ERROR )
                        {
                            found = MFS_lfn_match(internal_search_ptr->SRC_PTR, lfn, 0);
                        }
                        else
                        {
                            found = MFS_lfn_match(internal_search_ptr->SRC_PTR, sname, 0);
                        }  
                    }  
                }  
            }

            if ( !error_code )
            {
                error_code = MFS_Increment_dir_index(drive_ptr, &internal_search_ptr->CURRENT_CLUSTER,
                    &internal_search_ptr->DIR_ENTRY_INDEX, &internal_search_ptr->PREV_CLUSTER);  
            }

        } while ( (error_code == MFS_NO_ERROR) && (found  == FALSE) );

        if ( error_code == MFS_NO_ERROR )
        {
            transfer_ptr->ATTRIBUTE = dtohc(saved_dir_entry.ATTRIBUTE);
            transfer_ptr->TIME      = dtohs(saved_dir_entry.TIME);
            transfer_ptr->DATE      = dtohs(saved_dir_entry.DATE);
            transfer_ptr->FILE_SIZE = dtohl(saved_dir_entry.FILE_SIZE);

            /* Transfer the filename */
            len = _strnlen(sname,13);
            if ( sname[len-1] == '.' )
            {
                sname[len-1] = '\0';
            }
            strncpy(transfer_ptr->NAME, sname, 13);
        }

        MFS_free_path((pointer)lfn);
    }
    else
    {
        error_code = MFS_INVALID_MEMORY_BLOCK_ADDRESS;
    }  

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Check_search_string_all
* Returned Value   : boolean - TRUE if seach string indicates all files are
*    to be matched
* Comments  :   Examines the search string to see if all files are to be
*    matched.
*
*END*---------------------------------------------------------------------*/

boolean MFS_Check_search_string_for_all
    (
    char_ptr      search_ptr /*[IN] search string to check */
    )
{
    boolean need_star;
    char    c = *search_ptr;

    if ( (c != '*') && (c != '?') )
    {
        return FALSE;
    }

    need_star = (c == '?');

    while ( c )
    {
        if ( c == '*' )
        {
            need_star = FALSE;
        }
        else if ( c == '.' )
        {
            if ( need_star )
            {
                return FALSE;
            }
            need_star = TRUE;
        }
        else if ( c != '?' )
        {
            return FALSE;
        }
        search_ptr++;
        c = *search_ptr;
    }  

    return !need_star;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Check_search_string_8dot3
* Returned Value   : boolean - TRUE if seach string is an 8.3 string
* Comments  :   Examines the search string to see if it is an 8.3 string
*
*END*---------------------------------------------------------------------*/

boolean MFS_Check_search_string_for_8dot3
    (

    char_ptr      search_ptr    /*[IN] search string to check */
    )
{
    uint_32 namecount = 0;
    uint_32 typecount = 0;
    boolean dotfound  = FALSE;
    boolean starfound = FALSE;
    char    c         = *search_ptr;

    while ( c )
    {
        if ( c == '*' )
        {
            starfound = TRUE;
        }
        else if ( c == '.' )
        {
            if ( dotfound )
            {
                return FALSE;
            }
            dotfound  = TRUE;
            starfound = FALSE;
        }
        else if ( starfound )
        {
            return FALSE;
        }
        if ( dotfound )
        {
            typecount++;
            if ( typecount > 4 )
            {
                return FALSE;
            }
        }
        else
        {
            namecount++;
            if ( namecount > 8 )
            {
                return FALSE;
            }
        }  
        search_ptr++;
        c = *search_ptr;
    }  

    return TRUE;
}
