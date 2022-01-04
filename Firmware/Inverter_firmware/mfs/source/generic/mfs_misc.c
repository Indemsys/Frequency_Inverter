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
* $FileName: mfs_misc.c$
* $Version : 3.8.15.0$
* $Date    : Jul-25-2012$
*
* Comments:
*
*   This file contains functions to lock and unlock the drive.
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


#if MFSCFG_READ_ONLY_CHECK

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_lock 
* Returned Value   :  error code
* Comments  : Locks the drive. Cannot be called from an ISR.
*      
*      
*END*---------------------------------------------------------------------*/

boolean MFS_is_read_only
    (
    MQX_FILE_PTR                 mfs_fd_ptr,
    MFS_DRIVE_STRUCT_PTR         drive_ptr
    )
{
#if MFSCFG_READ_ONLY_CHECK_ALWAYS    
    uint_32                      id[3];
#endif
    if ( drive_ptr == NULL )
    {
        if ( mfs_fd_ptr == NULL )
        {
            return FALSE;
        }
        else if ( mfs_fd_ptr->DEV_PTR == NULL )
        {
            return FALSE;
        }
        else
        {
            drive_ptr = mfs_fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
        }
    }
#if MFSCFG_READ_ONLY_CHECK_ALWAYS    
    if (ioctl (drive_ptr->DEV_FILE_PTR, IO_IOCTL_DEVICE_IDENTIFY, id) != MQX_OK) 
    {
        return FALSE;
    }
    drive_ptr->READ_ONLY = (id[IO_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_WRITE) == 0;
#endif
    return drive_ptr->READ_ONLY;
}  

#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_lock 
* Returned Value   :  error code
* Comments  : Locks the drive. Cannot be called from an ISR.
*      
*      
*END*---------------------------------------------------------------------*/


_mfs_error MFS_lock
    (
    MQX_FILE_PTR                 mfs_fd_ptr,
    MFS_DRIVE_STRUCT_PTR _PTR_   drive_ptr
    )
{
    _mfs_error error_code;

    if ( mfs_fd_ptr == NULL )
    {
        error_code = MFS_ERROR_INVALID_DRIVE_HANDLE;
    }
    else if ( mfs_fd_ptr->DEV_PTR == NULL )
    {
        error_code = MFS_ERROR_INVALID_DRIVE_HANDLE;
    }
    else
    {
        *drive_ptr = mfs_fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
        _lwsem_wait(&(*drive_ptr)->SEM);
        error_code = MFS_NO_ERROR;
    }  
    return error_code;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lock_dos_disk
* Returned Value   :  error code
* Comments  : Locks the drive and verifies that it is a dos disk.
*             Cannot be called from an ISR.
*      
*END*---------------------------------------------------------------------*/

_mfs_error MFS_lock_dos_disk
    (
    MQX_FILE_PTR                 mfs_fd_ptr,
    MFS_DRIVE_STRUCT_PTR _PTR_   drive_ptr
    )
{
    _mfs_error error_code;

    if ( mfs_fd_ptr == NULL )
    {
        error_code = MFS_ERROR_INVALID_DRIVE_HANDLE;
    }
    else if ( mfs_fd_ptr->DEV_PTR == NULL )
    {
        error_code = MFS_ERROR_INVALID_DRIVE_HANDLE;
    }
    else
    {
        *drive_ptr = mfs_fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
        _lwsem_wait(&(*drive_ptr)->SEM);
        if ( !(*drive_ptr)->DOS_DISK )
        {
            _lwsem_post(&(*drive_ptr)->SEM);
            error_code =  MFS_NOT_A_DOS_DISK;
        }
        else
        {
            error_code = MFS_NO_ERROR;
        }  
    }  
    return error_code;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lock_handle
* Returned Value   :  error code
* Comments  :  Locks the drive using a file handle and verifies that it is a 
*              DOS disk. Also extracts the file handle from the MQX_FILE_PTR
*              Cannot be called from an ISR.
*      
*END*---------------------------------------------------------------------*/

_mfs_error MFS_lock_handle
    (
    MQX_FILE_PTR               fd_ptr, 
    MFS_DRIVE_STRUCT_PTR _PTR_ drive_ptr, 
    MFS_HANDLE_PTR _PTR_       handle_ptr
    )
{
    _mfs_error error_code;

    if ( fd_ptr == NULL )
    {
        error_code = MFS_ERROR_INVALID_DRIVE_HANDLE;
    }
    else if ( fd_ptr->DEV_PTR == NULL )
    {
        error_code = MFS_ERROR_INVALID_DRIVE_HANDLE;
    }
    else if ( fd_ptr->DEV_DATA_PTR == NULL )
    {
        error_code = MFS_ERROR_INVALID_FILE_HANDLE;
    }
    else
    {
        *handle_ptr = fd_ptr->DEV_DATA_PTR;
        *drive_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

        _lwsem_wait(&(*drive_ptr)->SEM);
        if ( !(*drive_ptr)->DOS_DISK )
        {
            _lwsem_post(&(*drive_ptr)->SEM);
            error_code =  MFS_NOT_A_DOS_DISK;
        }
        else
        {
            error_code = MFS_NO_ERROR;
        }  
    }  
    return error_code;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_unlock
* Returned Value   :  error code
* Comments  :  Unlocks the drive.
*      
*END*---------------------------------------------------------------------*/
_mfs_error MFS_unlock
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    boolean                 flush_if_mixed_mode
    )
{
    _mfs_error error = MFS_NO_ERROR;
    
    boolean  flush = FALSE;

    switch ( drive_ptr->WRITE_CACHE_POLICY )
    {
        case  MFS_MIXED_MODE_CACHE:     
            flush = flush_if_mixed_mode;
            break;

        case  MFS_WRITE_BACK_CACHE: 
            flush = FALSE;
            break;

        case  MFS_WRITE_THROUGH_CACHE:  
        default:   
            flush = TRUE;
            break;

    }

    if ( flush )
    {
        error = MFS_Flush_directory_sector_buffer(drive_ptr);
        
        if (error == MFS_NO_ERROR)
            error = MFS_Flush_data_sector_buffer(drive_ptr);
    }

    switch ( drive_ptr->FAT_CACHE_POLICY )
    {
        case  MFS_MIXED_MODE_CACHE:     
            flush = flush_if_mixed_mode;
            break;

        case  MFS_WRITE_BACK_CACHE: 
            flush = FALSE;
            break;

        case  MFS_WRITE_THROUGH_CACHE:  
        default:   
            flush = TRUE;
            break;

    }

    if (error == MFS_NO_ERROR && flush)
    {
        error = MFS_Flush_fat_cache(drive_ptr);
    }

    _lwsem_post(&drive_ptr->SEM);
    
    return error;
} 


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_mem_alloc
* Returned Value   :  
* Comments  :  
*      
*END*---------------------------------------------------------------------*/

pointer MFS_mem_alloc(_mem_size size)
{
    if ( _MFS_pool_id )
    {
        return _mem_alloc_from(_MFS_pool_id, size);
    }
    else
    {
        return _mem_alloc_uncached(size);
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_mem_alloc_zero
* Returned Value   :  
* Comments  :  
*      
*END*---------------------------------------------------------------------*/

pointer MFS_mem_alloc_zero(_mem_size size)
{
    if ( _MFS_pool_id )
    {
        return _mem_alloc_zero_from(_MFS_pool_id, size);
    }
    else
    {
        pointer p = _mem_alloc_uncached(size);
        
        if (p != NULL) 
        {
            _mem_zero(p, size);
        }

        return p;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_mem_alloc_system
* Returned Value   :  
* Comments  :  
*      
*END*---------------------------------------------------------------------*/

pointer MFS_mem_alloc_system(_mem_size size)
{
    if ( _MFS_pool_id )
    {
        return _mem_alloc_system_from(_MFS_pool_id, size);
    }
    else
    {
        return _mem_alloc_system_uncached(size);
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_mem_alloc_system_zero
* Returned Value   :  
* Comments  :  
*      
*END*---------------------------------------------------------------------*/

pointer MFS_mem_alloc_system_zero(_mem_size size)
{
    if ( _MFS_pool_id )
    {
        return _mem_alloc_system_zero_from(_MFS_pool_id, size);
    }
    else
    {
        /* memory must be uncached, otherwise reading sector may fail on 54418 */
        return _mem_alloc_system_zero_uncached(size);
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_alloc_path
* Returned Value   :  
* Comments  :  
*      
*END*---------------------------------------------------------------------*/

_mfs_error MFS_alloc_path(char_ptr * path_ptr_ptr)
{
    char_ptr path_ptr;

    path_ptr = MFS_mem_alloc_system(PATHNAME_SIZE + 1);
    if ( path_ptr == NULL )
    {
        return MFS_INSUFFICIENT_MEMORY;
    }
    _mem_set_type(path_ptr, MEM_TYPE_MFS_PATHNAME);
    *path_ptr_ptr = path_ptr;
    return MFS_NO_ERROR;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_alloc_2paths
* Returned Value   :  
* Comments  :  
*      
*END*---------------------------------------------------------------------*/

_mfs_error MFS_alloc_2paths(char_ptr * path1_ptr_ptr,char_ptr * path2_ptr_ptr)
{
    _mfs_error error_code;
    char_ptr path1_ptr,path2_ptr;

    error_code = MFS_alloc_path(&path1_ptr);
    if ( error_code == MFS_NO_ERROR )
    {
        error_code = MFS_alloc_path(&path2_ptr);
        if ( error_code == MFS_NO_ERROR )
        {
            *path1_ptr_ptr = path1_ptr;
            *path2_ptr_ptr = path2_ptr;
        }
        else
        {
            MFS_free_path(path1_ptr);
        }
    }

    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_free_path
* Returned Value   :  
* Comments  :  
*      
*END*---------------------------------------------------------------------*/

boolean MFS_free_path(char_ptr path_ptr)
{
    if ( path_ptr != NULL )
    {
        _mem_free(path_ptr);
        return TRUE;
    }

    return FALSE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Error_text
* Returned Value   : char_ptr
* Comments         :
*    Takes a MFS error code and returns the error description ptr.
*
*END*----------------------------------------------------------------------*/

char_ptr  MFS_Error_text
    (
    _mfs_error error_code
    )
{
    switch ( error_code )
    {
        case MFS_NO_ERROR :
            return("MFS NO ERROR");
        case MFS_INVALID_FUNCTION_CODE :
            return("MFS INVALID FUNCTION CODE");
        case MFS_FILE_NOT_FOUND :
            return("MFS FILE NOT FOUND");
        case MFS_PATH_NOT_FOUND :
            return("MFS PATH NOT FOUND");
        case MFS_ACCESS_DENIED :
            return("MFS ACCESS DENIED");
        case MFS_INVALID_HANDLE :
            return("MFS INVALID HANDLE");
        case MFS_INSUFFICIENT_MEMORY :
            return("MFS INSUFFICIENT MEMORY");
        case MFS_INVALID_MEMORY_BLOCK_ADDRESS :
            return("MFS INVALID MEMORY BLOCK ADDRESS");
        case MFS_ATTEMPT_TO_REMOVE_CURRENT_DIR :
            return("MFS ATTEMPT TO REMOVE CURRENT DIR");
        case MFS_DISK_IS_WRITE_PROTECTED :
            return("MFS DISK IS WRITE PROTECTED");
        case MFS_BAD_DISK_UNIT :
            return("MFS BAD DISK UNIT");
        case MFS_INVALID_LENGTH_IN_DISK_OPERATION:
            return("MFS INVALID LENGTH IN DISK OPERATION" );
        case MFS_NOT_A_DOS_DISK :
            return("MFS NOT A DOS DISK");
        case MFS_SECTOR_NOT_FOUND :
            return("MFS SECTOR NOT FOUND");
        case MFS_WRITE_FAULT :
            return("MFS WRITE FAULT");
        case MFS_READ_FAULT :
            return("MFS READ FAULT");
        case MFS_SHARING_VIOLATION :
            return("MFS SHARING VIOLATION");
        case MFS_FILE_EXISTS :
            return("MFS FILE EXISTS");
        case MFS_ALREADY_ASSIGNED :
            return("MFS ALREADY ASSIGNED");
        case MFS_INVALID_PARAMETER :
            return("MFS INVALID PARAMETER");
        case MFS_DISK_FULL :
            return("MFS DISK FULL");
        case MFS_ROOT_DIR_FULL:
            return("MFS ROOT DIR FULL");
        case MFS_EOF :
            return("MFS EOF");
        case MFS_CANNOT_CREATE_DIRECTORY:
            return("MFS CANNOT CREATE DIRECTORY");
        case MFS_NOT_INITIALIZED:
            return( "MFS NOT INITIALIZED" );
        case MFS_OPERATION_NOT_ALLOWED:
            return("MFS OPERATION NOT ALLOWED");
        case MFS_ERROR_INVALID_DRIVE_HANDLE:
            return("MFS INVALID DRIVE HANDLE" );
        case MFS_ERROR_INVALID_FILE_HANDLE:
            return( "MFS INVALID FILE HANDLE" );
        case MFS_ERROR_UNKNOWN_FS_VERSION:
            return( "MFS UNKNOWN FILESYSTEM VERSION" );
        case MFS_LOST_CHAIN:
            return( "MFS LOST CHAIN" );
        case MFS_INVALID_DEVICE :
            return( "MFS INVALID DEVICE" );
        case MFS_INVALID_CLUSTER_NUMBER:
            return( "MFS INVALID CLUSTER NUMBER" );
        case MFS_FAILED_TO_DELETE_LFN:
            return( "MFS FAILED TO DELETE LFN" );
        case MFS_BAD_LFN_ENTRY:
            return( "MFS BAD LFN ENTRY" );
        case PMGR_INVALID_PARTITION:
            return( "PMGR INVALID PARTITION" );
        case PMGR_INSUF_MEMORY:
            return( "PMGR INSUF MEMORY" );                
        case PMGR_UNKNOWN_PARTITION:
            return( "PMGR UNKNOWN PARTITION TYPE" );
        case PMGR_INVALID_PARTTABLE:
            return( "PMGR INVALID PARTITION TABLE" );
        default: return("UNKNOWN ERROR !!!");
    }  
}
