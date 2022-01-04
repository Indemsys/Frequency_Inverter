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
* $FileName: mfs_attribute.c$
* $Version : 3.6.4.0$
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


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Get_file_attributes
* Returned Value   : error_code
* Comments  :   Get the attribute byte of the specified file.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Get_file_attributes
    (
    MQX_FILE_PTR            mfs_fd_ptr,     /*[IN] pathname of the file */
    char _PTR_              pathname,
    uchar_ptr               attribute_ptr
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    uint_32                 dir_cluster,dir_index;
    _mfs_error              error_code;
    uchar                   attribute;
    uint_32                 prev_cluster= CLUSTER_INVALID; 


    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return(error_code);
    }

    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);  

    /*
    ** When a function locks MFS device, on any error it should not return 
    ** without unlocking it. This can create a deadlock.
    */   
    if ( error_code == MFS_NO_ERROR )
    {
        attribute = dtohc(dir_entry_ptr->ATTRIBUTE);
        *attribute_ptr = attribute;
    }

    MFS_unlock(drive_ptr,FALSE);
    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Set_file_attributes
* Returned Value   : error code
* Comments  :   Change the attribute of the specified file.
*
*END*---------------------------------------------------------------------*/
#if !MFSCFG_READ_ONLY
_mfs_error MFS_Set_file_attributes
    (
    MQX_FILE_PTR            mfs_fd_ptr,  
    char _PTR_              pathname,       /*[IN] pathname of the specific file */
    uchar_ptr               attribute_ptr   /*[IN] attribute of file */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    _mfs_error              error_code;
    uint_32                 dir_cluster, dir_index;
    uchar                   at, attrib;
    uint_32                 prev_cluster= CLUSTER_INVALID; 

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

    error_code = MFS_lock_dos_disk( mfs_fd_ptr, &drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }
    attrib = *attribute_ptr;
    attrib &= MFS_ATTR_ARCHIVE | MFS_ATTR_READ_ONLY | MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE;
    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);  
    if ( error_code == MFS_NO_ERROR )
    {
        at = dtohc(dir_entry_ptr->ATTRIBUTE);
        if ( at != *attribute_ptr )
        {
            /*
            ** The volume-label and the directory-name are mutually exclusive.
            ** The volume-label cannot act as a directory-name. Check whether
            ** you are trying to set the attributes to a volume.
            */
            if ( ! ((at & MFS_ATTR_VOLUME_NAME) || (*attribute_ptr & MFS_ATTR_VOLUME_NAME)) )
            {
                htodc(dir_entry_ptr->ATTRIBUTE, *attribute_ptr);
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;
            }
            else
            {
                error_code = MFS_ACCESS_DENIED;
            }  
        }
    }

    MFS_unlock(drive_ptr,TRUE);

    return(error_code);
}  
#endif

