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
* $FileName: dir_read.c$
* $Version : 3.6.14.0$
* $Date    : Oct-5-2012$
*
* Comments:
*
*   This file contains the functions for a partition manager device that is 
*   is built to work under MFS1.40. These functions can also create or 
*   remove partitions from a disk.
*
*END************************************************************************/


#include <string.h>
#include <mqx.h>
#include <fio.h>
#include "mfs.h"
#include "mfs_prv.h"


// Nees to be integrated into fopen (dirname, "d+mode_ptr"); etc


#define  FS_DIR_MODE_UNIX        1
#define  FS_DIR_MODE_MSDOS       2
#define  FS_DIR_MODE_FILENAME    3

typedef struct
{
    MQX_FILE_PTR         FS_PTR;
    MFS_GET_LFN_STRUCT   LFN_DATA;
    MFS_SEARCH_DATA      SEARCH_DATA;
    MFS_SEARCH_PARAM     SEARCH;
    char                 LFN[PATHNAME_SIZE];
    uint_32              MODE;
    boolean              FIRST;
} DIR_STRUCT, _PTR_ DIR_STRUCT_PTR;


pointer _io_mfs_dir_open(MQX_FILE_PTR fs_ptr, char_ptr wildcard_ptr, char_ptr mode_ptr)
{
    DIR_STRUCT_PTR       dir_ptr;
    uchar                attr = 0;
    uint_32              i;

    if ( fs_ptr == NULL ) return NULL;

    dir_ptr = (DIR_STRUCT_PTR) MFS_mem_alloc_zero( sizeof(DIR_STRUCT) );
    if ( dir_ptr == NULL )
        return NULL;

    _mem_set_type(dir_ptr, MEM_TYPE_MFS_DIR_STRUCT);

    dir_ptr->FS_PTR = fs_ptr;
    dir_ptr->SEARCH.WILDCARD = wildcard_ptr;

    dir_ptr->SEARCH.SEARCH_DATA_PTR = &dir_ptr->SEARCH_DATA;
    dir_ptr->SEARCH.ATTRIBUTE = attr;
    dir_ptr->MODE = FS_DIR_MODE_UNIX;

    if ( mode_ptr != NULL )
    {
        for ( i=0; mode_ptr[i]; i++ )
        {
            switch ( mode_ptr[i] )
            {
                case '*': 
                    attr = MFS_ATTR_ANY;
                    break;

                case 'r': 
                    attr |= MFS_ATTR_READ_ONLY;    
                    break;

                case 's': 
                    attr |= MFS_ATTR_SYSTEM_FILE;
                    break;

                case 'h': 
                    attr |= MFS_ATTR_HIDDEN_FILE;
                    break;

                case 'd': 
                    attr |= MFS_ATTR_DIR_NAME;
                    break;

                case 'v': 
                    attr |= MFS_ATTR_VOLUME_NAME;
                    break;

                case 'a': 
                    attr |= MFS_ATTR_ARCHIVE;
                    break;

                case 'u': 
                    dir_ptr->MODE = FS_DIR_MODE_UNIX;
                    break;

                case 'm': 
                    dir_ptr->MODE = FS_DIR_MODE_MSDOS;
                    break;

                case 'f': 
                    dir_ptr->MODE = FS_DIR_MODE_FILENAME;
                    break;

                default:                           
                    break;
            }
        }
        dir_ptr->SEARCH.ATTRIBUTE = attr;
    }

    dir_ptr->LFN_DATA.PATHNAME = dir_ptr->SEARCH_DATA.NAME;
    dir_ptr->LFN_DATA.LONG_FILENAME = dir_ptr->LFN;
    dir_ptr->LFN_DATA.SEARCH_DATA_PTR = &dir_ptr->SEARCH_DATA;

    dir_ptr->FIRST = TRUE;

    return(pointer) dir_ptr;
}   


static const char_ptr months[] =  
{
    "???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
};



int_32 _io_mfs_dir_read( pointer dir, char_ptr buffer, uint_32 size)
{
    DIR_STRUCT_PTR       dir_ptr = (DIR_STRUCT_PTR) dir;
    uint_32              error_code;
    uint_32              month;
    uint_32              year;
    uint_32              ofs;
    uint_32              len=0;

    if ( dir_ptr == NULL ) return IO_ERROR;

    if ( dir_ptr->FIRST )
    {
        error_code = ioctl(dir_ptr->FS_PTR, IO_IOCTL_FIND_FIRST_FILE, (pointer)&dir_ptr->SEARCH);
        dir_ptr->FIRST = FALSE;
    }
    else
    {
        error_code = ioctl(dir_ptr->FS_PTR, IO_IOCTL_FIND_NEXT_FILE,  (pointer) &dir_ptr->SEARCH_DATA);
    }
    if ( error_code != MFS_NO_ERROR )
    {
        return IO_ERROR;
    }
    /* read file */
    error_code = ioctl(dir_ptr->FS_PTR, IO_IOCTL_GET_LFN, (pointer)&dir_ptr->LFN_DATA);
    /* attributes test */
        
    if ( dir_ptr->MODE == FS_DIR_MODE_MSDOS )
    {    
        ofs = len<size ? len : size;
        len = snprintf(buffer+ofs, size-ofs, "%-12.12s %8lu %02lu-%02lu-%04lu %02lu:%02lu:%02lu %c%c%c%c%c%c ",
            dir_ptr->SEARCH_DATA.NAME, dir_ptr->SEARCH_DATA.FILE_SIZE,

            (uint_32)(dir_ptr->SEARCH_DATA.DATE & MFS_MASK_MONTH) >> MFS_SHIFT_MONTH,
            (uint_32)(dir_ptr->SEARCH_DATA.DATE & MFS_MASK_DAY)   >> MFS_SHIFT_DAY,
            (uint_32)((dir_ptr->SEARCH_DATA.DATE & MFS_MASK_YEAR) >> MFS_SHIFT_YEAR) + 1980,

            (uint_32)(dir_ptr->SEARCH_DATA.TIME & MFS_MASK_HOURS)   >> MFS_SHIFT_HOURS,
            (uint_32)(dir_ptr->SEARCH_DATA.TIME & MFS_MASK_MINUTES) >> MFS_SHIFT_MINUTES,
            (uint_32)(dir_ptr->SEARCH_DATA.TIME & MFS_MASK_SECONDS) << 1,

            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_READ_ONLY) ? 'R':' ',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_HIDDEN_FILE) ? 'H':' ',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_SYSTEM_FILE) ? 'S':' ',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_VOLUME_NAME) ? 'V':' ',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_DIR_NAME) ? 'D':' ',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_ARCHIVE) ? 'A':' ');
    }
    else if ( dir_ptr->MODE == FS_DIR_MODE_UNIX )
    {
        ofs = len<size ? len : size;
        len += snprintf(buffer+ofs, size-ofs, "%cr%c-r%c-r%c-   1 ",
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_DIR_NAME)?'d':'-',

            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_READ_ONLY)?'-':'w',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_READ_ONLY)?'-':'w',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_READ_ONLY)?'-':'w',
            (dir_ptr->SEARCH_DATA.ATTRIBUTE & MFS_ATTR_DIR_NAME)?'+':' ');

        ofs = len<size ? len : size;
        len += snprintf(buffer+ofs, size-ofs, "ftp      ftp  "); 

        ofs = len<size ? len : size;
        len += snprintf (buffer+ofs, size-ofs, "  %10lu ",  dir_ptr->SEARCH_DATA.FILE_SIZE );

        month = (uint_32)(dir_ptr->SEARCH_DATA.DATE & MFS_MASK_MONTH) >> MFS_SHIFT_MONTH;
        if (month > 12)
            month = 0;

        ofs = len<size ? len : size;
        len += snprintf (buffer+ofs, size-ofs, "%3s %02d ", months[month], 
            (uint_32)(dir_ptr->SEARCH_DATA.DATE & MFS_MASK_DAY) );

        year = (uint_32)((dir_ptr->SEARCH_DATA.DATE & MFS_MASK_YEAR) >> MFS_SHIFT_YEAR) + 1980;
        ofs = len<size ? len : size;
        len += snprintf (buffer+ofs, size-ofs, " %4d ", year ); 
    }

    if ( !error_code )
    {
        ofs = len<size ? len : size;
        len += snprintf (buffer+ofs, size-ofs, "%s\r\n",dir_ptr->LFN ); 
    }
    else
    {
        ofs = len<size ? len : size;
        len += snprintf (buffer+ofs, size-ofs, "%s\r\n",dir_ptr->SEARCH_DATA.NAME ); 
    }

    /* Terminate the string for sure */
    if (size > 0)
        buffer[size - 1] = '\0';

    return len;
} 


int_32 _io_mfs_dir_close( pointer dir)
{
    if ( dir == NULL ) return IO_ERROR;
    _mem_free(dir);
    return MQX_OK;
}
