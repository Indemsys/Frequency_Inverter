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
* $FileName: mfs_lfn.c$
* $Version : 3.8.15.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains functions related to using long filenames of up to
*   255 characters.
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_to_sfn
* Returned Value   :  none
* Comments  :  Will take a long filename (actual file name or directory name
*   and will parse it into the corresponding 8.3 name. It is assumed that 
*   there is space allocated for the short name. 
*END*---------------------------------------------------------------------*/

_mfs_error  MFS_lfn_to_sfn
    (
    char_ptr    l_fname,    /*[IN] the long filename */
    char_ptr    s_fname     /*[OUT] the short filename */
    )
{
    int_32      i; 
    int_32      len;
    char_ptr    temp_lname1, temp_lname2;
    char_ptr    new, orig, valid_dot = NULL;
    _mfs_error  error_code;

    error_code = MFS_alloc_2paths(&temp_lname1,&temp_lname2);
    if ( error_code != MFS_NO_ERROR )
    {
        return( error_code );
    }

    orig = l_fname;
    new = temp_lname1;

    /* 1. Remove all spaces and translate all unwanted 8.3 chars into '_' */
    for ( i = 0 ; i < FILENAME_SIZE; i++ )
    {
        if ( *orig == '\0' )
        {
            break;
        }

        switch (*orig)
        {
            case '[':
            case ']':
            case ';':
            case ',':
            case '+':
            case '=':
                *new++ = '_';
                orig++;
                break;
                
            case ' ':
                orig++;
                break;

            default:
                *new++ = *orig++;
                break;            
        }        
    }  
    *new-- = '\0';

    /* Remove all of the trailing '.'  */
    while ( *new == '.' )
    {
        *new-- = '\0';
    }  

    /* 
    ** Locate the valid '.' if there is one
    */
    while ( new != temp_lname1 )
    {
        if ( *new == '.' )
        {
            valid_dot = new;
            break;
        }
        new--;
    }  

    /* Now we can start copying to the new name, without the extra periods */
    new = temp_lname2;
    orig = temp_lname1;
    while ( *orig != '\0' )
    {
        if ( *orig == '.' )
        {
            if ( orig == valid_dot )
            {
                *new++ = *orig++;
            }
            else
            {
                orig++;
            }  
        }
        else
        {
            *new++ = *orig++;
        }  
    }  
    *new = '\0';

    /* Now we create final 8.3 name */
    for ( i = 0 ; i <= 5 && temp_lname2[i] != '.' && temp_lname2[i] != '\0'; i++ )
    {
        s_fname[i] =  temp_lname2[i];
    }     

    len = i;
    s_fname[len++] = '~';
    s_fname[len++] = '1';
    if ( valid_dot != NULL )
    {
        s_fname[len++] = '.';
        orig = temp_lname2;
        while ( *orig++ != '.' )
            /* void */ ;
        for ( i = 0 ; i <= 2 && orig[i] != '\0'; i++ )
        {
            s_fname[len++] = orig[i];
        }  
    }

    s_fname[len] = '\0';

    /* Capitalize the name */
    for ( i = 0; i <= len - 1; i++ )
        if ( s_fname[i] >= 'a' && s_fname[i] <= 'z' )
        {
            s_fname[i] -= 32;
        }

    MFS_free_path(temp_lname1);
    MFS_free_path(temp_lname2);

    return( MFS_NO_ERROR );
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_is_valid_lfn
* Returned Value   :  boolean
* Comments  :  Will take a long filename (actual file name or directory name)
*   and will validate it by checking for length and for invalid characters.
*END*---------------------------------------------------------------------*/

boolean  MFS_is_valid_lfn
    (
    char_ptr    l_fname /*[IN] the long filename */
    )
{
    register uint_32     i, k;
    register char        c;
    char                 illegal[] = "*?<>|\":/\\";
    boolean              ok_char_flag;

    ok_char_flag = FALSE;
    for ( i = 0 ; i <= FILENAME_SIZE ; i++ )
    {
        c = *l_fname;
        /* if we have reached the end, and there was some chars, the name is ok */
        if ( *l_fname == '\0' )
        {
            return(ok_char_flag); 
        }
        else if ( c == '.' || c == ' ' )
        {
            /* Do nothing */      
        }
        else
        {
            for ( k=0 ; k <= 8; k++ )
            {
                if ( c == illegal[k] )
                {
                    return( FALSE );
                }
            }  
            ok_char_flag = TRUE;
        }  
        l_fname++;
    }        

    if ( i <= FILENAME_SIZE && ok_char_flag )
    {
        return( TRUE );
    }

    return( FALSE );
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_is_autogenerated_name
* Returned Value   :  boolean
* Comments  :  Will take a short filename (actual file name or directory name)
*   and will determine if it is an autogenerated name. If it is, LFN entries
*   should exist for it, if not no LFN entries should exist.
*
*  The autogenerated name has the following format:
*     [
*
*END*---------------------------------------------------------------------*/

boolean  MFS_is_autogenerated_name
    (
    char_ptr    s_fname /*[IN] the long filename */
    )
{
    uint_32  i, digits=0;
    boolean  found_tilde = FALSE;


    for ( i=0;i<SFILENAME_SIZE;i++ )
    {
        if ( !found_tilde )
        {
            // Looking for '~'
            switch ( s_fname[i] )
            {
                case '[':
                case ']':
                case ';':
                case ',':
                case '+':
                case '=':
                case ' ':
                case '\0':
                    // All invalid before ~ in an autogenerated name.
                    return FALSE;

                case '~':
                    found_tilde=TRUE;
                    break;
            }
        }
        else
        {
            // Found '~', counting numbers
            if ( (s_fname[i]=='.')  || (s_fname[i]==0) )
            {
                // Valid if we saw at least on digit
                return(digits!=0);
            }

            if ( (s_fname[i]>='0') && (s_fname[i]<='9') )
            {
                digits++;
                if ( digits>6 ) return FALSE;
            }
        }
    }
    return FALSE;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_dirname_valid
* Returned Value   :  boolean - TRUE if valid LFN directory name.
* Comments  :
*        Allows . and ..
*END*---------------------------------------------------------------------*/

boolean MFS_lfn_dirname_valid
    (
     char_ptr        filename   /*[IN] filename to validate */
    )
{

    if ( *filename != '.' )
    {
        return(MFS_is_valid_lfn(filename));
    }
    else
    {
        if ( (*(filename + 1) == '\0') ||
            ((*(filename+1) == '.') && *(filename+2) == '\0') )
        {
            return(TRUE);
        }
    }  

    return(FALSE);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_match
* Returned Value   :  boolean - TRUE if the dir_entry_ptr points to the entry
*                     actually corresponding to the LFN. 
* Comments  : Will read the previous entries and reconstruct the LFN
*END*---------------------------------------------------------------------*/

boolean MFS_lfn_match
    (
    char_ptr  search_ptr,   /* [IN] the search string */
    char_ptr  lfn,          /* [IN] the long file name to compare */
    uint_32   level         /* [IN] the level of recursion or the number of * encountered */
    ) 
{
    char cw;
    char ct;

    if ( *search_ptr == '\0' )
    {
        return TRUE;
    }
    if ( *lfn == '\0' )
    {
        return FALSE;
    }

    while ( *search_ptr )
    {
        if ( *lfn == '\0' )
        {
            if ( (*search_ptr == '*') || (*search_ptr == '?') )
            {
                if ( !*(search_ptr+1) )
                {
                    return TRUE;
                }
            }
            return FALSE;
        }
        else if ( *search_ptr == '?' )
        {
            search_ptr++;
            lfn++;
        }
        else if ( *search_ptr == '*' )
        {
            if ( level > 3 )
            {
                return FALSE;
            }
            search_ptr++;
            while ( *lfn )
            {
                if ( MFS_lfn_match(search_ptr, lfn, level + 1) )
                {
                    return TRUE;
                }
                lfn++;
            }  
        }
        else
        {
            cw = CAPITALIZE(*search_ptr);
            ct = CAPITALIZE(*lfn);
            if ( cw != ct )
            {
                return FALSE;
            }
            else
            {
                search_ptr++;
                lfn++;
            }  
        }  
    }  
    if ( *lfn )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }  
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_extract
* Returned Value   :  
*
* Comments  : Will take a LFN directory entry and extract the name for
*             the given entry. Will copy the 13 UNICODE chars fromt the
*             entry. The output string does not always end with '\0'.
*             Assumes the entry is valid.
*END*---------------------------------------------------------------------*/

void MFS_lfn_extract
    (
    MFS_LNAME_ENTRY_PTR     lname_entry,    /* [IN] pointer to the entry to read UNICODE name from */
    char_ptr                filename        /* [OUT] buffer to store the 13 chars in  */
    )
{
    uint_32     i;
    char_ptr     tempfilename;

    /* 
    ** The LFN are stored in unicode. A unicode character occupies 2 bytes. One
    ** byte is the character set, and the other is the character. In our case
    ** we are using a macro to extract the unicode char and we are converting it
    ** directly into a char. We can do this because the ASCII set is represented
    ** by char set 0 (stored in the upper byte). 
    */
    tempfilename = filename;
    for ( i = 0 ; i <= 9 ; i = i + 2 )
    {
        *tempfilename++ = (char) dtohs(lname_entry->NAME0_4 + i);
    }  

    for ( i = 0 ; i <= 11 ; i = i + 2 )
    {
        *tempfilename++ = (char) dtohs(lname_entry->NAME5_10 + i);
    }  

    for ( i = 0 ; i <= 3 ; i = i + 2 )
    {
        *tempfilename++ = (char) dtohs(lname_entry->NAME11_12 + i);
    }  
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_increment_lfn
* Returned Value   :  void
*
* Comments  : Takes a compressed short filename (that represents a lfn) and
*             increments the number after the ~ 
*END*---------------------------------------------------------------------*/

void MFS_increment_lfn
    (
    char_ptr       filename /* [IN]/[OUT] a short compressed filename (a valid one) */
    )
{
    int_32     i;
    uint_32     num;
    uint_32     power;

    /* Find out where the ~ starts and get the number after the ~ */
    for ( i = 7, power = 1, num = 0; i >= 0; i-- )
    {
        if ( filename[i] == '~' )
        {
            break;
        }
        /* Translate the ASCII value to an int value */
        num += power * (uint_32) (filename[i] - '0');
        power *= 10;
    }  

    num++;

    if ( num / power >= 1 )
    {
        i--;
        filename[i] = '~';
        power *= 10;
    }

    /* Write the new number */
    for ( i++ ; i <= 7 ;i++ )
    {
        power /= 10;
        /* Translate the int value into an ASCII value */
        filename[i] = (uchar) (num / power) + '0';
        num %= power;
    }  
}       

#if !MFSCFG_READ_ONLY
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_remove_lfn_entries
* Returned Value   :  uint_32 error code
*
* Comments  : Removes the extra entries (corresponding to the dir_entry_ptr
*             containing the LFN). Can be used on a normal dir entry, but does
*             nothing in this case.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_remove_lfn_entries
    (
    MFS_DRIVE_STRUCT_PTR       drive_ptr,       /* [IN] drive on which to operate */
    uint_32                    parent_cluster,  /* [IN] the cluster in which the entry lies */
    uint_32                    dir_index,       /* [IN] the index of where in the cluster the entry lies */
    uint_32                    prev_cluster     /* [IN] the cluster previous to the cluster in which the entry lies */    
    )
{
    MFS_LNAME_ENTRY_PTR           lname_entry = NULL;
    boolean                       last_entry_flag = FALSE;
    uint_32                       current_index;
    _mfs_error                    error_code = MFS_NO_ERROR;
    uint_32                       temp_prev_cluster;  
    boolean                       went_back = FALSE;
    boolean                       start_deletion = FALSE;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (NULL, drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    /* As long as we haven't found the EOF flag in the LFN name */
    while ( last_entry_flag == FALSE )
    {
        /* Check to see if the LFN spans to different sector */
        if ( dir_index == 0 )
        {
            /* Check to see if we are in root directory */
            if ( parent_cluster == 0 )
            {
                break;
            }
            else if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
            {
                if ( drive_ptr->BPB32.ROOT_CLUSTER == parent_cluster )
                {
                    break;
                }
            }
            if ( !went_back )
            {
                error_code = MFS_get_prev_cluster(drive_ptr, &parent_cluster, prev_cluster);
                went_back = TRUE;
            }
            else
            {
                if ( start_deletion )
                {
                    error_code = MFS_FAILED_TO_DELETE_LFN;
                }
                break;
            }
            dir_index = drive_ptr->ENTRIES_PER_SECTOR * drive_ptr->BPB.SECTORS_PER_CLUSTER ;   
        }

        current_index = dir_index;

        if ( !error_code )
        {
            dir_index--;
            lname_entry = (MFS_LNAME_ENTRY_PTR) MFS_Find_directory_entry(drive_ptr, 
                NULL, &parent_cluster, &dir_index, &temp_prev_cluster, MFS_ATTR_LFN, &error_code); 
        }

        /* If a LFN, mark entry as deleted */
        if ( (lname_entry != NULL) && !error_code && (dir_index < current_index) )
        {
            if ( lname_entry->ID & MFS_LFN_END )
            {
                last_entry_flag = TRUE;
            }

            lname_entry->ID = MFS_DEL_FILE;
            drive_ptr->DIR_SECTOR_DIRTY = TRUE;

            start_deletion = TRUE;  
        }
        else
        {
            break;  
        }  
    }  

    return( error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_name_to_entry
* Returned Value   :  uint_32 error code
*
* Comments  : Takes a string of up to 13 characters and writes them to a LFN 
*             directory entry.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_lfn_name_to_entry
    (
    char_ptr                filename,   /* [IN] a string up to 13 chars long, to be put in the entry */
    MFS_LNAME_ENTRY_PTR     entry_ptr   /* [OUT] the LNAME entry in which the string is places */
    )
{
    uint_32     i;
    boolean     end_of_name = FALSE;

    if ( entry_ptr == NULL )
    {
        return( MFS_CANNOT_CREATE_DIRECTORY );
    }


    /* The entry contains UNICODE chars... Which is why i = i +2 */
    /* Copy chars 0 to 4 */
    for ( i = 0 ; i <= 9 ; i = i + 2 )
    {
        if ( *filename )
        {
            htods((entry_ptr->NAME0_4 + i),(uint_16) *filename);
            filename++;      
        }
        else if ( !end_of_name )
        {
            htods((entry_ptr->NAME0_4 + i), 0);
            end_of_name = TRUE;
        }
        else
        {
            htods((entry_ptr->NAME0_4 + i), 0xFFFF);
        }  
    }  

    /* Copy chars 5 to 10 */
    for ( i = 0 ; i <= 11 ; i = i + 2 )
    {
        if ( *filename )
        {
            htods((entry_ptr->NAME5_10 + i), (uint_16) *filename);      
            filename++;
        }
        else if ( !end_of_name )
        {
            htods((entry_ptr->NAME5_10 + i), 0);
            end_of_name = TRUE;
        }
        else
        {
            htods((entry_ptr->NAME5_10 + i), 0xFFFF);         
        }  
    }  

    /* Copy chars 11 to 12 */
    for ( i = 0 ; i <= 3 ; i = i + 2 )
    {
        if ( *filename )
        {
            htods((entry_ptr->NAME11_12 + i), (uint_16) *filename);      
            filename++;
        }
        else if ( !end_of_name )
        {
            htods((entry_ptr->NAME11_12 + i), 0);
            end_of_name = TRUE;
        }
        else
        {
            htods((entry_ptr->NAME11_12 + i), 0xFFFF);         
        }  
    }  


    return( MFS_NO_ERROR );
}  

#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_checksum
* Returned Value   :  uchar checksum
*
* Comments  : Takes a valid expanded filename of 11 characters (doesn't need 
*             to be terminated by a NULL char) and calculates the LFN checksum
*             value for it.
*END*---------------------------------------------------------------------*/
uchar MFS_lfn_checksum
    (
    char_ptr       filename_ptr /*[IN] pointer to filename on which to calculate checksum */
    )
{
    uchar       sum;
    uint_32     i;

    for ( sum = 0, i = 0 ; i < 11 ; i++ )
    {
        sum = (((sum & 1) << 7) | (( sum & 0xfe) >> 1)) + *filename_ptr++;
    }  

    return( sum );
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_prev_cluster
* Returned Value   :  uint_32 error_code 
*
* Comments  : Finds the previous cluster in the cluster chain for any given
*             cluster. 
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_prev_cluster
    (
    MFS_DRIVE_STRUCT_PTR       drive_ptr,       /* [IN] the drive on which to operate */
    uint_32_ptr                cluster_ptr,     /* [IN]/[OUT] cluster to perform search with / previous cluster */
    uint_32                    previous_cluster /* [IN] the previous cluster */  
    )
{
    uint_32        i;
    _mfs_error     error_code = MFS_NO_ERROR;
    uint_32        next_cluster;

    /* Check to see if we are at the very first cluster */
    if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
    {
        if ( *cluster_ptr == drive_ptr->BPB32.ROOT_CLUSTER )
        {
            return MFS_INVALID_CLUSTER_NUMBER;  
        }
    }
    else if ( *cluster_ptr == 0 )
    {
        return MFS_INVALID_CLUSTER_NUMBER;  
    }

    // If we have been given a previous cluster
    if ( previous_cluster != CLUSTER_INVALID )
    {
        // Make sure it is before the current cluster
        MFS_get_cluster_from_fat(drive_ptr, previous_cluster, &next_cluster); 
        if ( *cluster_ptr == next_cluster )
        {
            // It is, so use it
            *cluster_ptr = previous_cluster;
            return MFS_NO_ERROR;
        }
    }

    if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
    {
        return MFS_INVALID_CLUSTER_NUMBER;  
    }

    /* Search the rest of the fat for the wanted cluster */
    for ( i = CLUSTER_MIN_GOOD ; i <= drive_ptr->LAST_CLUSTER &&
        !error_code; i++ )
    {
        MFS_get_cluster_from_fat(drive_ptr, previous_cluster, &next_cluster); 
        if ( *cluster_ptr == next_cluster )
        {
            *cluster_ptr = i;
            break;
        }
    }  

    if ( i > drive_ptr->LAST_CLUSTER )
    {
        error_code = MFS_INVALID_CLUSTER_NUMBER;
    }

    return( error_code );
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_lfn
* Returned Value   :  uint_32 error code
*
* Comments  : Will take an a pathname that points to a normal 8.3 entry. 
*             If this entry has a LFN, the LFN will be placed in the lfn buffer.
*             Assumes the drive is locked. 
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_lfn
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    char_ptr                filepath,   /*[IN] the filepath to the 8.3 filename */
    char_ptr                lfn         /*[OUT] the long file name, if it exists */
    )
{
    MFS_DIR_ENTRY_PTR  entry_ptr;
    uint_32              index;
    uint_32              cluster;
    _mfs_error           error_code;
    uint_32              prev_cluster; 


    // If filepath is not an autogenerated name, then it will not have a
    // corresponding long file name.

    entry_ptr =  MFS_Find_entry_on_disk(drive_ptr, filepath, &error_code, &cluster, &index, &prev_cluster);  

    if ( error_code==MFS_NO_ERROR )
    {
        error_code = MFS_get_lfn_of_entry(drive_ptr,entry_ptr,cluster,index,prev_cluster,lfn);
    }
    return error_code;

}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_lfn
* Returned Value   :  uint_32 error code
*
* Comments  : Will take an a pathname that points to a normal 8.3 entry. 
*             If this entry has a LFN, the LFN will be placed in the lfn buffer.
*             Assumes the drive is locked. 
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_lfn_of_entry
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    MFS_DIR_ENTRY_PTR       entry_ptr,
    uint_32                 cluster,
    uint_32                 index,
    uint_32                 prev_cluster,
    char_ptr                lfn         /*[OUT] the long file name, if it exists */
    )
{
    _mfs_error           error_code=MFS_NO_ERROR;
    uint_32              sum;
    uint_32              entry_count;
    boolean             went_back = FALSE;
    boolean              extracting = FALSE;
    MFS_LNAME_ENTRY_PTR  lfn_entry_ptr;


    if ( entry_ptr == NULL )
    {
        return(MFS_INVALID_PARAMETER);
    }
    else
    {
        /* Calculate the checksum for the file */
        sum = MFS_lfn_checksum(((MFS_DIR_ENTRY_PTR) entry_ptr)->NAME);
    }  

    entry_count = 1;

    while ( TRUE )
    {
        /* Check to see if the LFN spans to different sector */
        if ( index == 0 )
        {
            if ( went_back )
            {
                if ( extracting )
                {
                    error_code = MFS_BAD_LFN_ENTRY;
                }
                break;
            }
            else
            {
                went_back=TRUE;
                error_code = MFS_get_prev_cluster(drive_ptr, &cluster, prev_cluster); 
                index = drive_ptr->ENTRIES_PER_SECTOR * 
                    drive_ptr->BPB.SECTORS_PER_CLUSTER ; 
            } 
        }

        if ( !error_code )
        {
            index--;
            entry_ptr = MFS_Find_directory_entry(drive_ptr, 
                NULL, &cluster, &index, &prev_cluster, MFS_ATTR_ANY, &error_code); 
        }

        if ( error_code )
        {
            *lfn = '\0';
            return( error_code );
        }

        if ( !entry_ptr )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        lfn_entry_ptr = (MFS_LNAME_ENTRY_PTR) entry_ptr;

        if ( lfn_entry_ptr->ATTR != MFS_ATTR_LFN )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        if ( lfn_entry_ptr->ALIAS_CHECKSUM != sum )
        {
            *lfn = '\0';
            break;
        }
        else if ( !(lfn_entry_ptr->ID & entry_count++) )
        {
            *lfn = '\0';
            break;
        }
        else
        {
            MFS_lfn_extract(lfn_entry_ptr, lfn);
            lfn = lfn + 13;
            *lfn = '\0';
        }  

        /* Check if this is the last entry */
        if ( lfn_entry_ptr->ID & MFS_LFN_END )
        {
            break;
        }
    }  

    return( MFS_NO_ERROR );
}  

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_lfn_dir_cluster
* Returned Value   :  uint_32 error code
*
* Comments  : Will take an a current directory cluster entry. 
*             If this entry has a LFN, the LFN will be placed in the lfn buffer.
*             Assumes the drive is locked. 
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_lfn_dir_cluster
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,          /*[IN] the drive on which to operate */
    pointer                 search_next_ptr,    /*[IN] address of search data block indicating the current criteria and the results of
                                                ** the last search results of this search are placed in this data block */
    char_ptr                filepath,           /*[OUT] the long file name, if it exists */
    char_ptr                lfn                 /*[IN] the filepath to the 8.3 filename */
    )
{
    MFS_INTERNAL_SEARCH_PTR     internal_search_ptr;
    MFS_LNAME_ENTRY_PTR         entry_ptr;
    uint_32                     index;
    uint_32                     cluster;
    _mfs_error                  error_code;
    uint_32                     sum;
    uint_32                     entry_count;
    MFS_SEARCH_DATA_PTR         transfer_ptr;
    uint_32                     temp_prev_cluster; 
    boolean                     went_back = FALSE;
    boolean                     extracting = FALSE;

    transfer_ptr = (MFS_SEARCH_DATA_PTR) search_next_ptr;

    if ( transfer_ptr == NULL )
    {
        return(MFS_INVALID_PARAMETER);
    }

    internal_search_ptr = &transfer_ptr->INTERNAL_SEARCH_DATA;                      
    entry_ptr = (MFS_LNAME_ENTRY_PTR)MFS_Find_directory_entry(drive_ptr,NULL, &internal_search_ptr->CURRENT_CLUSTER,
        &internal_search_ptr->DIR_ENTRY_INDEX, &internal_search_ptr->PREV_CLUSTER, MFS_ATTR_ANY,&error_code);
    cluster    = internal_search_ptr->CURRENT_CLUSTER;
    index      = internal_search_ptr->DIR_ENTRY_INDEX;
    /* Calculate the checksum for the file */
    sum = MFS_lfn_checksum(((MFS_DIR_ENTRY_PTR) entry_ptr)->NAME);

    entry_count = 1;

    while ( TRUE )
    {
        /* Check to see if the LFN spans to different sector */
        if ( index == 0 )
        {
            if ( went_back )
            {
                if ( extracting )
                {
                    error_code = MFS_BAD_LFN_ENTRY;
                }
                break;
            }
            else
            {
                went_back=TRUE;
                error_code = MFS_get_prev_cluster(drive_ptr, &cluster, internal_search_ptr->PREV_CLUSTER); 
                index = drive_ptr->ENTRIES_PER_SECTOR * drive_ptr->BPB.SECTORS_PER_CLUSTER ;   
            }
        }

        if ( !error_code )
        {
            index--;
            entry_ptr = (MFS_LNAME_ENTRY_PTR) MFS_Find_directory_entry(drive_ptr, 
                NULL, &cluster, &index, &temp_prev_cluster, MFS_ATTR_ANY, &error_code); 
        }

        if ( error_code )
        {
            *lfn = '\0';
            return( error_code );
        }

        if ( !entry_ptr )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        if ( entry_ptr->ATTR != MFS_ATTR_LFN )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        if ( entry_ptr->ALIAS_CHECKSUM != sum )
        {
            *lfn = '\0';
            break;
        }
        else if ( !(entry_ptr->ID & entry_count++) )
        {
            *lfn = '\0';
            break;
        }
        else
        {
            extracting = TRUE; 
            MFS_lfn_extract(entry_ptr, lfn);
            lfn = lfn + 13;
            *lfn = '\0';
        }  

        /* Check if this is the last entry */
        if ( entry_ptr->ID & MFS_LFN_END )
        {
            break;
        }
    }  

    return( MFS_NO_ERROR );
}
