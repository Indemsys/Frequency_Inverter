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
* $FileName: mfs_delete.c$
* $Version : 3.6.5.0$
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
* Function Name    : MFS_Delete_file
* Returned Value   : error_code
* Comments  :   Delete a specific file.
*
*END*---------------------------------------------------------------------*/

_mfs_error  MFS_Delete_file
    (
    MQX_FILE_PTR            mfs_fd_ptr, /*[IN] the MFS device on which to operate */
    char_ptr                pathname    /*[IN] directory and file name of the file to delete */
    )
{
    MFS_DRIVE_STRUCT_PTR    drive_ptr;
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    _mfs_error              error_code, saved_code;
    uint_32                 dir_cluster, dir_index;
    uint_32                 first_cluster;
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

    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);

    if ( dir_entry_ptr != NULL )
    {
        if (dtohc(dir_entry_ptr->ATTRIBUTE) & (MFS_ATTR_DIR_NAME | MFS_ATTR_VOLUME_NAME | MFS_ATTR_READ_ONLY))
        {
            /* Not a file or a read only file */
            error_code = MFS_ACCESS_DENIED;
        }
        else if (MFS_Find_handle(drive_ptr, dir_cluster, dir_index) != NULL)
        {
            /* File is currently open */
            error_code = MFS_SHARING_VIOLATION;
        }
        else
        {
            first_cluster = clustoh(dir_entry_ptr->HFIRST_CLUSTER, dir_entry_ptr->LFIRST_CLUSTER);

            if (first_cluster)
            {
                error_code = MFS_Release_chain(drive_ptr, first_cluster);
            }

            if (error_code == MFS_NO_ERROR || error_code == MFS_LOST_CHAIN)
            {
                *dir_entry_ptr->NAME = MFS_DEL_FILE;
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                saved_code = MFS_remove_lfn_entries(drive_ptr,dir_cluster,dir_index, prev_cluster);
                if (error_code == MFS_NO_ERROR)
                {
                    error_code = saved_code;
                }
            }
        }
    }
    MFS_unlock(drive_ptr,TRUE);

    return(error_code);
}

#endif
