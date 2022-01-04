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
* $FileName: mfs_init.c$
* $Version : 3.8.34.0$
* $Date    : Jun-27-2012$
*
* Comments:
*
*   This file contains the functions that are used to initialize MFS
*   It also contains the MFS driver functions.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>
#include <io_prv.h>



#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"

_mem_pool_id _MFS_pool_id;
uint_32 _MFS_handle_pool_init = MFSCFG_HANDLE_INITIAL;
uint_32 _MFS_handle_pool_grow = MFSCFG_HANDLE_GROW;
uint_32 _MFS_handle_pool_max  = MFSCFG_HANDLE_MAX;

static int_32 _io_mfs_uninstall_internal(IO_DEVICE_STRUCT_PTR);                                                                                                                                       

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_install
* Returned Value   : error code
* Comments  :   Initialize the MSDOS File System.
*
*END*---------------------------------------------------------------------*/

uint_32 _io_mfs_install
    (
    MQX_FILE_PTR dev_fd,        /*[IN] the device on which to install MFS */
    char_ptr     identifier,    /*[IN] the name that should be given to mfs (ex: "C:", "MFS1:", etc..) */
    uint_32      partition_num  /*[IN] the partition number to install MFS on. 0 for no partitions */  
    )
{
    MFS_DRIVE_STRUCT_PTR         drive_ptr;
    uint_32                      error_code;

    drive_ptr = MFS_mem_alloc_system_zero( sizeof(MFS_DRIVE_STRUCT) );
    if ( drive_ptr == NULL )
    {
        return( MFS_INSUFFICIENT_MEMORY );
    }
    _mem_set_type(drive_ptr, MEM_TYPE_MFS_DRIVE_STRUCT);

    _int_disable();

    error_code = _io_dev_install_ext( identifier, 
        _io_mfs_open,                     
        _io_mfs_close, 
        _io_mfs_read,                      
        _io_mfs_write,                     
        (_mqx_int(_CODE_PTR_)(MQX_FILE_PTR,_mqx_uint,pointer))_io_mfs_ioctl,
        _io_mfs_uninstall_internal,
        (pointer)drive_ptr);

    if ( error_code != IO_OK )
    {
        _mem_free(drive_ptr);
        _int_enable();      
        return error_code;
    }

    /*
    ** create a light weight semaphore
    */
    _lwsem_create(&drive_ptr->SEM,1);

    drive_ptr->DEV_FILE_PTR = dev_fd;
    drive_ptr->DRV_NUM = partition_num;
    drive_ptr->HANDLE_PARTITION = _partition_create(sizeof(MFS_HANDLE), _MFS_handle_pool_init, _MFS_handle_pool_grow, _MFS_handle_pool_max );

    // Inform Kernel that MFS is installed
    _mqx_set_io_component_handle(IO_MFS_COMPONENT,(pointer)MFS_VERSION);

    _int_enable();

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_uninstall_internal
* Returned Value   : uint_32 error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/

static int_32 _io_mfs_uninstall_internal
    (
    IO_DEVICE_STRUCT_PTR   dev_ptr  /* [IN] The device to uninstall */            
    )
{
    MFS_DRIVE_STRUCT_PTR   drive_ptr;
    int_32                 error;

    drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_ptr->DRIVER_INIT_PTR;
    if ( !drive_ptr->MFS_FILE_PTR )
    {
        error = IO_OK;
        _lwsem_destroy(&drive_ptr->SEM);
        _mfs_partition_destroy(drive_ptr->HANDLE_PARTITION);
        _mem_free(drive_ptr);
    }
    else
    {
        error = MFS_SHARING_VIOLATION;
    }  

    return error;
}  


#if MFSCFG_DEPRICATED_UNINSTALL
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_uninstall
* Returned Value   : uint_32 error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/

uint_32 _io_mfs_uninstall
    (
    char_ptr     identifier /* [IN] A string that identifies the device to uninstall */            
    )
{
    return(uint_32)_io_dev_uninstall(identifier);
}  
#endif


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_mfs_open
* Returned Value   : MQX_OK or an error
* Comments         : Opens and initializes MFS driver.
* 
*END*----------------------------------------------------------------------*/

int_32 _io_mfs_open
    (
    MQX_FILE_PTR             fd_ptr,        /* [IN] the file handle for the device being opened */
    char_ptr                 open_name_ptr, /* [IN] the remaining portion of the name of the device */
    char_ptr                 flags_str      /* [IN] the flags to specify file type:
                                            ** w  write 
                                            ** r  read
                                            ** a append
                                            ** x for temp file
                                            ** n for new file
                                            ** etc... 
                                            */
    )
{
    MFS_DRIVE_STRUCT_PTR        drive_ptr;
    uchar                       flags = 0;
    uint_32                     error_code = MFS_NO_ERROR;

    drive_ptr = (MFS_DRIVE_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

    if ( drive_ptr->MFS_FILE_PTR == NULL )
    {
        /* This means we are opening MFS and not a file . */
        error_code = MFS_Open_Device(fd_ptr, drive_ptr);
        if ( error_code == MFS_NOT_A_DOS_DISK )
        {
            /* 
            ** This error is OK. It just means the disk probably has to be 
            ** formatted 
            */
            fd_ptr->ERROR = error_code;
            error_code = MFS_NO_ERROR;
        }
      /* */
      if (error_code == MFS_NO_ERROR) {
         _io_register_file_system(fd_ptr,open_name_ptr);
      }
    }
    else
    {
        /* We are opening a file */
        if ( !drive_ptr->DOS_DISK )
        {
            return MFS_NOT_A_DOS_DISK;
        }

        open_name_ptr = MFS_Parse_Out_Device_Name(open_name_ptr);

        /* Check for a filename, if none, then check for the temp flag */
        if ( *(open_name_ptr-1) != ':' )
        {
            error_code = MFS_PATH_NOT_FOUND;         
        }
        else if ( *open_name_ptr == '\0' )
        {
            error_code = MFS_PATH_NOT_FOUND;
        }
        else
        {
            /* 
            ** When opening a file, only read-only attributes are set. The other
            ** attributes (such as hidden, system, etc..) must be set afterwards
            ** with an ioctl call.
            */  
            switch ( *flags_str )
            {
                case 'r':
                    if ( *(flags_str + 1) == '+' )
                    {
                        flags |= MFS_ACCESS_READ_WRITE;
                    }
                    else
                    {
                        flags |= MFS_ACCESS_READ_ONLY;
                    } 

                    fd_ptr->DEV_DATA_PTR = (pointer) MFS_Open_file(drive_ptr->MFS_FILE_PTR, open_name_ptr, flags, &error_code);
                    break;
#if !MFSCFG_READ_ONLY
                case 'x':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (NULL, drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR) 
                    {
                        fd_ptr->DEV_DATA_PTR = (pointer) MFS_Create_temp_file(drive_ptr->MFS_FILE_PTR, flags, open_name_ptr, &error_code);      
                    }
                    break;
                case 'w':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (NULL, drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR) 
                    {
                        if ( *(flags_str + 1) == '+' )
                        {
                            flags |= MFS_ACCESS_READ_WRITE;
                        }
                        else
                        {
                            flags |= MFS_ACCESS_WRITE_ONLY;
                        } 

                        fd_ptr->DEV_DATA_PTR = (pointer) MFS_Create_file(drive_ptr->MFS_FILE_PTR, 0, open_name_ptr, &error_code);
                        if ( error_code == MFS_NO_ERROR )
                        {
                            ((MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR)->ACCESS = flags;
                        }
                    }
                    break;
                case 'a':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (NULL, drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR) 
                    {
                        if ( *(flags_str + 1) == '+' )
                        {
                            flags |= MFS_ACCESS_READ_WRITE;
                        }
                        else
                        {
                            flags |= MFS_ACCESS_WRITE_ONLY;     
                        } 

                        fd_ptr->DEV_DATA_PTR = (pointer) MFS_Open_file(drive_ptr->MFS_FILE_PTR, open_name_ptr, flags, &error_code);
                        if ( error_code == MFS_FILE_NOT_FOUND )
                        {
                            error_code = MFS_NO_ERROR;
                            fd_ptr->DEV_DATA_PTR = (pointer) MFS_Create_file(drive_ptr->MFS_FILE_PTR, 0, open_name_ptr, &error_code);
                            if ( error_code == MFS_NO_ERROR )
                            {
                                ((MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR)->ACCESS = flags;
                            }
                        }
                    }
                    break;
                case 'n':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (NULL, drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR) 
                    {
                        if ( *(flags_str + 1) == '+' )
                        {
                            flags |= MFS_ACCESS_READ_WRITE;
                        }
                        else
                        {
                            flags |= MFS_ACCESS_WRITE_ONLY;
                        } 

                        fd_ptr->DEV_DATA_PTR = (pointer) MFS_Create_new_file(drive_ptr->MFS_FILE_PTR, 0, open_name_ptr, &error_code);
                        if ( error_code == MFS_NO_ERROR )
                        {
                            ((MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR)->ACCESS = flags;
                        }
                    }
                    break;
#endif
                default:
                    error_code = MFS_INVALID_PARAMETER;
                    break;
            }  
        }  

        /* Check to make sure the error code is OK */
        if ( error_code == MFS_NO_ERROR )
        {
            /* Initialise the file information fields */
            fd_ptr->LOCATION = 0;     
            ((MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR)->SAVED_POSITION = 0;     
            fd_ptr->SIZE = dtohl(((MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR)->DIR_ENTRY.FILE_SIZE);

            /* If we are appending, go to end of file */
            if ( *flags_str == 'a' )
            {
                fseek(fd_ptr, 0, IO_SEEK_END);
            }
        }
        else
        {
            // An error occured, so the calling function (_io_fopen) will
            // free the MQX_FILE_PTR. Need to release the handle
            if ( fd_ptr->DEV_DATA_PTR )
            {
                MFS_Close_file(fd_ptr);
            }
        } 
    } 

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_mfs_close
* Returned Value   : ERROR CODE
* Comments         : Closes MFS driver
* 
*END*----------------------------------------------------------------------*/

int_32 _io_mfs_close
    (
    MQX_FILE_PTR                   fd_ptr   /* [IN] the file handle for the device being closed */
    )
{
    uint_32                 result = MQX_OK;
   // unregister file system from handle table
   _io_unregister_file_system(fd_ptr);

    if ( fd_ptr->DEV_DATA_PTR == NULL )
    {
        /* We are closing the mfs_fd_ptr, and not a normal file */
        result = MFS_Close_Device(fd_ptr);
    }
    else
    {
        /* We are closing a normal file */
        result = MFS_Close_file(fd_ptr);   
    } 

    return(result);
}  



/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_mfs_read
* Returned Value   : number of characters read
* Comments         : Reads data from MFS driver
* 
*END*----------------------------------------------------------------------*/

int_32 _io_mfs_read
    (
    MQX_FILE_PTR file_ptr,   /* [IN] the stream to perform the operation on */
    char_ptr    data_ptr,   /* [IN] the data location to read to */
    int_32      num         /* [IN] the number of bytes to read */
    )
{
    int_32 result;

    result = MFS_Read(file_ptr, num, data_ptr, &(file_ptr->ERROR));

    /* Check for EOF. The MFS EOF must be translated to the standard EOF */
    if ( file_ptr->ERROR == MFS_EOF )
    {
        file_ptr->FLAGS |= IO_FLAG_AT_EOF;
    }

    return (file_ptr->ERROR == 0 || file_ptr->ERROR == MFS_EOF) ? result : IO_ERROR;    
}   


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_mfs_write
* Returned Value   : number of characters written
* Comments         : Writes data to the fdv_ram device
* 
*END*----------------------------------------------------------------------*/

int_32 _io_mfs_write
    (
    MQX_FILE_PTR file_ptr,   /* [IN] the stream to perform the operation on */
    char_ptr    data_ptr,   /* [IN] the data location to read to */
    int_32      num         /* [IN] the number of bytes to read */
    )
{
#if MFSCFG_READ_ONLY
    return IO_ERROR;
#else
    int_32 result;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (file_ptr, NULL))
    {
        return IO_ERROR;
    }
#endif

    result = MFS_Write(file_ptr, num, data_ptr, &(file_ptr->ERROR));
    return (file_ptr->ERROR == 0) ? result : IO_ERROR;
#endif
}    


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_mfs_ioctl
* Returned Value   : int_32 
* Comments         :
*    The returned value is IO_EOF or a MQX error code.
*
*END*----------------------------------------------------------------------*/

int_32 _io_mfs_ioctl
    (
    MQX_FILE_PTR file_ptr,   /* [IN] the stream to perform the operation on */
    uint_32     cmd,        /* [IN] the ioctl command */
    uint_32_ptr param_ptr   /* [IN] the ioctl parameters */
    )
{
    MFS_DRIVE_STRUCT_PTR   drive_ptr;
    uint_32                result = MQX_OK;

    switch ( cmd )
    {
        case IO_IOCTL_SEEK:
            MFS_Move_file_pointer(file_ptr, &result);
            if ( result == MFS_EOF )
                file_ptr->FLAGS |= IO_FLAG_AT_EOF;
            break;

        case IO_IOCTL_CHAR_AVAIL:
            /* 
            ** Location is one smaller than file size. We want to know if 
            ** we can read one extra character 
            */
            *param_ptr = (file_ptr->LOCATION  >= dtohl(((MFS_HANDLE_PTR)file_ptr->DEV_DATA_PTR)->DIR_ENTRY.FILE_SIZE))?FALSE:TRUE;
            break;

        case IO_IOCTL_BAD_CLUSTERS:
            result = MFS_Bad_clusters(file_ptr);
            break;
        case IO_IOCTL_LAST_CLUSTER:
            result = MFS_Last_cluster(file_ptr);
            break;
        case IO_IOCTL_FREE_SPACE:
            {
                uint_64 bytes_free = MFS_Get_disk_free_space(file_ptr);
                result = (bytes_free > MAX_UINT_32) ? MAX_UINT_32 : bytes_free;
                if (param_ptr) *((uint_64*)param_ptr) = bytes_free;
            }
            break;
        case IO_IOCTL_FREE_CLUSTERS:
            MFS_lock(file_ptr, &drive_ptr);
            result = MFS_Get_disk_free_space_internal(drive_ptr,NULL);
            MFS_unlock(drive_ptr,FALSE);
            break;
        case IO_IOCTL_GET_CLUSTER_SIZE:
            MFS_lock(file_ptr, &drive_ptr);
            *param_ptr = (uint_32)drive_ptr->CLUSTER_SIZE_BYTES;
            MFS_unlock(drive_ptr,FALSE);
            break;
        case IO_IOCTL_GET_CURRENT_DIR:
            result = MFS_Get_current_dir(file_ptr, (char_ptr) param_ptr);
            break;
        case IO_IOCTL_CHANGE_CURRENT_DIR:
            param_ptr = (uint_32_ptr) MFS_Parse_Out_Device_Name((char_ptr) param_ptr);
            result = MFS_Change_current_dir(file_ptr, (char_ptr) param_ptr);
            break;
        case IO_IOCTL_CHECK_DIR_EXIST:
            param_ptr = (uint_32_ptr) MFS_Parse_Out_Device_Name((char_ptr) param_ptr);
            result = MFS_Check_dir_exist(file_ptr, (char_ptr) param_ptr);
            break;
            
        case IO_IOCTL_FIND_FIRST_FILE:
            result = MFS_Find_first_file(file_ptr, ((MFS_SEARCH_PARAM_PTR) param_ptr)->ATTRIBUTE,
                MFS_Parse_Out_Device_Name(((MFS_SEARCH_PARAM_PTR) param_ptr)->WILDCARD), 
                ((MFS_SEARCH_PARAM_PTR) param_ptr)->SEARCH_DATA_PTR); 
            break;
        case IO_IOCTL_FIND_NEXT_FILE:
            result = MFS_Find_next_file(file_ptr, (MFS_SEARCH_DATA_PTR) param_ptr);
            break;
        case IO_IOCTL_GET_FILE_ATTR:
            result = MFS_Get_file_attributes(file_ptr,
                MFS_Parse_Out_Device_Name(
                ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->PATHNAME),
                ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->ATTRIBUTE_PTR);            
            break;
        case IO_IOCTL_GET_DATE_TIME:
            result = MFS_Get_date_time(file_ptr, 
                ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->DATE_PTR,
                ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->TIME_PTR);
            break;      

        case IO_IOCTL_GET_LFN:
            MFS_lock(file_ptr, &drive_ptr);
            {
                MFS_GET_LFN_STRUCT_PTR  lfn_ptr = ((MFS_GET_LFN_STRUCT_PTR) param_ptr);
                char_ptr pathname = MFS_Parse_Out_Device_Name(lfn_ptr->PATHNAME);
                result = MFS_get_lfn(drive_ptr, pathname, lfn_ptr->LONG_FILENAME);
#if 0
                //            _mfs_error result;
                MFS_DIR_ENTRY_PTR       entry_ptr;

                if ( lfn_ptr->SEARCH_DATA_PTR )
                {
                    entry_ptr = 
                        MFS_Read_directory_sector(drive_ptr,lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.CURRENT_CLUSTER, 
                        INDEX_TO_SECTOR(lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX), &result);
                    if ( (result== MFS_NO_ERROR && entry_ptr!=NULL) )
                    {
                        entry_ptr += INDEX_TO_OFFSET (lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX);

                        result = MFS_get_lfn_of_entry(drive_ptr,entry_ptr,
                            lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.CURRENT_CLUSTER,
                            lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX,
                            lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.PREV_CLUSTER,
                            lfn_ptr->LONG_FILENAME);
                    }

                }
                else
                {
                    result = MFS_get_lfn(drive_ptr, pathname, lfn_ptr->LONG_FILENAME);
                }
#endif
            }
            MFS_unlock(drive_ptr,FALSE);
            break;
        case IO_IOCTL_GET_VOLUME:
            result = MFS_Get_volume(file_ptr, (char_ptr) param_ptr);
            break;

#if MFSCFG_READ_ONLY
        case IO_IOCTL_DELETE_FILE:
        case IO_IOCTL_CREATE_SUBDIR:
        case IO_IOCTL_REMOVE_SUBDIR:
        case IO_IOCTL_RENAME_FILE:
        case IO_IOCTL_SET_FILE_ATTR:
        case IO_IOCTL_FLUSH_OUTPUT:
        case IO_IOCTL_SET_DATE_TIME:
        case IO_IOCTL_SET_VOLUME:
        case IO_IOCTL_FORMAT:
        case IO_IOCTL_DEFAULT_FORMAT:
        case IO_IOCTL_FORMAT_TEST:
        case IO_IOCTL_WRITE_CACHE_ON:
        case IO_IOCTL_WRITE_CACHE_OFF:
        case IO_IOCTL_FAT_CACHE_ON:
        case IO_IOCTL_FAT_CACHE_OFF:
        case IO_IOCTL_FLUSH_FAT:
        case IO_IOCTL_TEST_UNUSED_CLUSTERS:
        case IO_IOCTL_SET_FAT_CACHE_MODE:
        case IO_IOCTL_SET_WRITE_CACHE_MODE:
        case IO_IOCTL_VERIFY_WRITES:
            result = MFS_OPERATION_NOT_ALLOWED;
            break;


        case IO_IOCTL_GET_FAT_CACHE_MODE:
        case IO_IOCTL_GET_WRITE_CACHE_MODE:
            if ( param_ptr )
            {
                *(_mfs_cache_policy *)param_ptr = MFS_WRITE_THROUGH_CACHE;
            }
            else
            {
                result = MQX_INVALID_POINTER;
            }
            break;
#else

        case IO_IOCTL_DELETE_FILE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                param_ptr = (uint_32_ptr) 
                MFS_Parse_Out_Device_Name((char_ptr) param_ptr);
                result = MFS_Delete_file(file_ptr, (char_ptr) param_ptr);
            }
            break;
        case IO_IOCTL_CREATE_SUBDIR:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                param_ptr = (uint_32_ptr) MFS_Parse_Out_Device_Name((char_ptr) param_ptr);
                result = MFS_Create_subdir(file_ptr, (char_ptr) param_ptr);
            }
            break;
        case IO_IOCTL_REMOVE_SUBDIR:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                param_ptr = (uint_32_ptr) MFS_Parse_Out_Device_Name((char_ptr) param_ptr);
                result = MFS_Remove_subdir(file_ptr, (char_ptr) param_ptr);      
            }
            break;

        case IO_IOCTL_RENAME_FILE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Rename_file(file_ptr, 
                    MFS_Parse_Out_Device_Name( ((MFS_RENAME_PARAM_PTR) param_ptr)->OLD_PATHNAME),
                    MFS_Parse_Out_Device_Name( ((MFS_RENAME_PARAM_PTR) param_ptr)->NEW_PATHNAME));
            }
            break;
        case IO_IOCTL_SET_FILE_ATTR:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Set_file_attributes(file_ptr, 
                    MFS_Parse_Out_Device_Name( ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->PATHNAME),
                    ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->ATTRIBUTE_PTR);            
            }
            break;                                                   

        case IO_IOCTL_SET_DATE_TIME:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Set_date_time(file_ptr, 
                    ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->DATE_PTR,
                    ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->TIME_PTR);
            }
            break;      

        case IO_IOCTL_SET_VOLUME:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Set_volume(file_ptr, (char_ptr) param_ptr);
            }
            break;

        case IO_IOCTL_WRITE_CACHE_ON:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(file_ptr, &drive_ptr);
                drive_ptr->WRITE_CACHE_POLICY = MFS_WRITE_BACK_CACHE;
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

        case IO_IOCTL_WRITE_CACHE_OFF:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(file_ptr, &drive_ptr);
                drive_ptr->WRITE_CACHE_POLICY = MFS_WRITE_THROUGH_CACHE;
                MFS_unlock(drive_ptr,FALSE);      
            }
            break;

        case IO_IOCTL_FLUSH_OUTPUT:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_HANDLE_PTR handle;

                /* lock the filesystem and obtain file handle */
                result = MFS_lock_handle(file_ptr, &drive_ptr, &handle);
                if (result == MFS_ERROR_INVALID_FILE_HANDLE)
                {
                    /* file_ptr is not associated with a particular file, at least try to lock the filesystem */
                    handle = NULL;
                    result = MFS_lock(file_ptr, &drive_ptr);
                }

                if ( result == MFS_NO_ERROR )
                {
                    if (handle != NULL && handle->TOUCHED)
                    {
                        TIME_STRUCT time;
                        DATE_STRUCT clk_time;

                        _time_get(&time);
                        _time_to_date(&time, &clk_time);
                        NORMALIZE_DATE(&clk_time);
                        htods(handle->DIR_ENTRY.TIME, PACK_TIME(clk_time));
                        htods(handle->DIR_ENTRY.DATE, PACK_DATE(clk_time));
                        result = MFS_Update_entry(drive_ptr, handle);
                    }

                    MFS_Flush_caches(drive_ptr);
                    MFS_unlock(drive_ptr,FALSE);
                }
            }
            break;

        case IO_IOCTL_SET_WRITE_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                if ( param_ptr )
                {
                    _mfs_cache_policy    policy = *((_mfs_cache_policy *)param_ptr);
                    if ( (policy==MFS_WRITE_THROUGH_CACHE) || (policy==MFS_MIXED_MODE_CACHE) || (policy==MFS_WRITE_BACK_CACHE) )
                    {
                        MFS_lock(file_ptr, &drive_ptr);
                        drive_ptr->WRITE_CACHE_POLICY = policy;
                        MFS_unlock(drive_ptr,TRUE);
                    }
                    else
                    {
                        result = MFS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;


        case IO_IOCTL_GET_WRITE_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                if ( param_ptr )
                {
                    drive_ptr = file_ptr->DEV_PTR->DRIVER_INIT_PTR;
                    *((_mfs_cache_policy *)param_ptr) = drive_ptr->WRITE_CACHE_POLICY ;
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;

        case IO_IOCTL_FAT_CACHE_ON:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(file_ptr, &drive_ptr);
                drive_ptr->FAT_CACHE_POLICY = MFS_WRITE_BACK_CACHE;
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

        case IO_IOCTL_FAT_CACHE_OFF:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(file_ptr, &drive_ptr);
                drive_ptr->FAT_CACHE_POLICY = MFS_WRITE_THROUGH_CACHE;
                MFS_unlock(drive_ptr,FALSE);      
            }
            break;

        case IO_IOCTL_FLUSH_FAT:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(file_ptr, &drive_ptr);
                MFS_Flush_fat_cache(drive_ptr);
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

        case IO_IOCTL_SET_FAT_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                if ( param_ptr )
                {
                    _mfs_cache_policy    policy = *((_mfs_cache_policy *)param_ptr);
                    if ( (policy==MFS_WRITE_THROUGH_CACHE) || (policy==MFS_MIXED_MODE_CACHE) || (policy==MFS_WRITE_BACK_CACHE) )
                    {
                        MFS_lock(file_ptr, &drive_ptr);
                        drive_ptr->FAT_CACHE_POLICY = policy;
                        MFS_unlock(drive_ptr,TRUE);
                    }
                    else
                    {
                        result = MFS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;

        case IO_IOCTL_GET_FAT_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                if ( param_ptr )
                {
                    drive_ptr = file_ptr->DEV_PTR->DRIVER_INIT_PTR;
                    *((_mfs_cache_policy *)param_ptr) = drive_ptr->FAT_CACHE_POLICY;
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;

        case IO_IOCTL_TEST_UNUSED_CLUSTERS:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Test_unused_clusters(file_ptr, param_ptr);
            }
            break;

        case IO_IOCTL_VERIFY_WRITES:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(file_ptr, &drive_ptr);
                if (param_ptr) {
                   if (!drive_ptr->READBACK_SECTOR_PTR && drive_ptr->BPB.SECTOR_SIZE ) {
                      drive_ptr->READBACK_SECTOR_PTR = MFS_mem_alloc(drive_ptr->BPB.SECTOR_SIZE);
                      _mem_set_type(drive_ptr->READBACK_SECTOR_PTR, MEM_TYPE_MFS_DATA_SECTOR); 
                   }   
                   result =  (drive_ptr->READBACK_SECTOR_PTR != NULL)?MQX_OK:MQX_OUT_OF_MEMORY; 
                } else {
                   if (drive_ptr->READBACK_SECTOR_PTR) {
                      _mem_free(drive_ptr->READBACK_SECTOR_PTR);
                      drive_ptr->READBACK_SECTOR_PTR = NULL;
                   }
                   result = MQX_OK;   
                }
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

#ifdef MFSCFG_ENABLE_FORMAT

        case IO_IOCTL_FORMAT:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Format(file_ptr, ((MFS_IOCTL_FORMAT_PARAM_PTR) param_ptr)->FORMAT_PTR);
            }
            break;
            
        case IO_IOCTL_DEFAULT_FORMAT:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Default_Format(file_ptr);
            }
            break;
            
        case IO_IOCTL_FORMAT_TEST:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (file_ptr, NULL))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Format_and_test(file_ptr,
                    ((MFS_IOCTL_FORMAT_PARAM_PTR) param_ptr)->FORMAT_PTR, 
                    ((MFS_IOCTL_FORMAT_PARAM_PTR) param_ptr)->COUNT_PTR); 
            }
            break;
#else
        case IO_IOCTL_FORMAT:
        case IO_IOCTL_DEFAULT_FORMAT:
        case IO_IOCTL_FORMAT_TEST:
            result = MFS_OPERATION_NOT_ALLOWED;
            break;

#endif

#endif

        case IO_IOCTL_GET_DEVICE_HANDLE:
            drive_ptr = (MFS_DRIVE_STRUCT_PTR) file_ptr->DEV_PTR->DRIVER_INIT_PTR;
            *param_ptr = (uint_32)drive_ptr->DEV_FILE_PTR;
            result = MQX_OK;
            break;

        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }  

    return( (int_32) result);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_free_drive_data
* Returned Value   : 
* Comments  :   
*
*END*---------------------------------------------------------------------*/

static void MFS_free_drive_data( MFS_DRIVE_STRUCT_PTR drive_ptr, boolean all)
{
    if ( (drive_ptr->DIR_SECTOR_PTR != NULL) && all )
    {
        _mem_free(drive_ptr->DIR_SECTOR_PTR);
        drive_ptr->DIR_SECTOR_PTR = NULL;
    }

    if ( drive_ptr->DATA_SECTOR_PTR != NULL )
    {
        _mem_free(drive_ptr->DATA_SECTOR_PTR);
        drive_ptr->DATA_SECTOR_PTR = NULL;
    }

    if ( drive_ptr->FAT_CACHE_PTR != NULL )
    {
        _mem_free(drive_ptr->FAT_CACHE_PTR);
        drive_ptr->FAT_CACHE_PTR = NULL;
    }
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Mount_drive_internal
* Returned Value   : error code
* Comments  :   Used to set the MFS drive parameters for a unit.  
*   This function assumes that the boot sector of the drive is stored in
*   the drive's sector buffer.  This function is called after MFS is 
*   initialized, or after the drive has been formatted.
*
*   NOTE: It is assumed that the drive is locked by the 
*   calling function.
*
*END*---------------------------------------------------------------------*/

uint_32 MFS_Mount_drive_internal
    (
    MFS_DRIVE_STRUCT_PTR         drive_ptr
    )
{
    BIOS_PARAM_STRUCT_DISK_PTR   bpb_ptr;
    BIOS_PARAM32_STRUCT_DISK_PTR bpb32_ptr;
    FILESYSTEM_INFO_DISK_PTR     fsinfo_ptr;
    uint_32                      k,fat_size, root_dir_sectors,
        data_sectors, cluster_count, 
        error_code = MFS_NO_ERROR;
    uint_32                      temp[3];

    /*
    ** First, deallocate the FAT and CLUSTER buffers previously allocated.  
    ** If the unit has changed or been reformated, these may not be the same 
    ** size.
    */
    MFS_free_drive_data(drive_ptr, FALSE);

    /*
    ** Next, check  to see that the BOOT record is that of a DOS disk.  If  not,
    ** the drive will have to be formatted by the upper layer before the drive
    ** can be 'mounted'.
    */
    if ( (*drive_ptr->DIR_SECTOR_PTR != (char) MFS_DOS30_JMP) && 
        (*drive_ptr->DIR_SECTOR_PTR != (char)MFS_DOS30_B) )
    {
        drive_ptr->DOS_DISK = FALSE;
        return MFS_NOT_A_DOS_DISK;
    }


    /*
    ** Extract the drive parameters (BIOS Parameter Block) from the BOOT Record.
    */
    bpb_ptr = (BIOS_PARAM_STRUCT_DISK_PTR) drive_ptr->DIR_SECTOR_PTR;

    drive_ptr->BPB.SECTOR_SIZE =         dtohs(bpb_ptr->SECTOR_SIZE);
    drive_ptr->BPB.SECTORS_PER_CLUSTER = dtohc(bpb_ptr->SECTORS_PER_CLUSTER);
    drive_ptr->BPB.RESERVED_SECTORS =    dtohs(bpb_ptr->RESERVED_SECTORS);
    drive_ptr->BPB.NUMBER_OF_FAT =       dtohc(bpb_ptr->NUMBER_OF_FAT);
    drive_ptr->BPB.ROOT_ENTRIES =        dtohs(bpb_ptr->ROOT_ENTRIES);
    drive_ptr->BPB.NUMBER_SECTORS =      dtohs(bpb_ptr->NUMBER_SECTORS);
    drive_ptr->BPB.MEDIA_TYPE =          dtohc(bpb_ptr->MEDIA_TYPE);
    drive_ptr->BPB.SECTORS_PER_FAT =     dtohs(bpb_ptr->SECTORS_PER_FAT);
    drive_ptr->BPB.SECTORS_PER_TRACK =   dtohs(bpb_ptr->SECTORS_PER_TRACK);
    drive_ptr->BPB.NUM_HEADS =           dtohs(bpb_ptr->NUM_HEADS);
    drive_ptr->BPB.HIDDEN_SECTORS =      dtohl(bpb_ptr->HIDDEN_SECTORS);
    drive_ptr->BPB.MEGA_SECTORS =        dtohl(bpb_ptr->MEGA_SECTORS);

    if ( drive_ptr->BPB.NUMBER_SECTORS != 0 )
    {
        drive_ptr->BPB.MEGA_SECTORS = drive_ptr->BPB.NUMBER_SECTORS;
    }

    /* Determine FAT type by calculating the count of clusters on disk */
    root_dir_sectors = ((drive_ptr->BPB.ROOT_ENTRIES * sizeof(MFS_DIR_ENTRY))
        + (drive_ptr->BPB.SECTOR_SIZE - 1)) / drive_ptr->BPB.SECTOR_SIZE;

    bpb32_ptr = (BIOS_PARAM32_STRUCT_DISK_PTR) (drive_ptr->DIR_SECTOR_PTR + sizeof(BIOS_PARAM_STRUCT_DISK));
    if ( drive_ptr->BPB.SECTORS_PER_FAT == 0 )
    {
        fat_size = dtohl(bpb32_ptr->FAT_SIZE);
    }
    else
    {
        fat_size = drive_ptr->BPB.SECTORS_PER_FAT; 
    }  

    data_sectors = drive_ptr->BPB.MEGA_SECTORS - (drive_ptr->BPB.RESERVED_SECTORS + 
        (drive_ptr->BPB.NUMBER_OF_FAT * fat_size) + root_dir_sectors);

    cluster_count = data_sectors / drive_ptr->BPB.SECTORS_PER_CLUSTER;

    /* Now we have cluster count, so we can determine FAT type */
    if ( cluster_count < 4085 )
    {
        drive_ptr->FAT_TYPE = MFS_FAT12;
    }
    else if ( cluster_count < 65525 )
    {
        drive_ptr->FAT_TYPE = MFS_FAT16;
    }
    else
    {
        drive_ptr->FAT_TYPE = MFS_FAT32;
    }  

    /*
    ** Calculate the rest of the drive data.
    */
    k = drive_ptr->BPB.SECTORS_PER_CLUSTER;
    for ( drive_ptr->CLUSTER_POWER_SECTORS = 0; !(k & 1); drive_ptr->CLUSTER_POWER_SECTORS++ )
    {
        k>>=1;
    }  

    drive_ptr->CLUSTER_SIZE_BYTES = drive_ptr->BPB.SECTORS_PER_CLUSTER * drive_ptr->BPB.SECTOR_SIZE;

    k = drive_ptr->BPB.SECTOR_SIZE;
    for ( drive_ptr->SECTOR_POWER = 0; !(k & 1); drive_ptr->SECTOR_POWER++ )
    {
        k>>=1;
    }  

    drive_ptr->CLUSTER_POWER_BYTES = drive_ptr->SECTOR_POWER + drive_ptr->CLUSTER_POWER_SECTORS;

    drive_ptr->CUR_DIR_CLUSTER = 0;
    drive_ptr->FREE_COUNT = FSI_UNKNOWN; /* This is the unknown value */
    drive_ptr->NEXT_FREE_CLUSTER = FSI_UNKNOWN; /* MFS will calculate it later */
    drive_ptr->CURRENT_DIR[0] = '\\'; /* Root dir */
    drive_ptr->CURRENT_DIR[1] = '\0';
    drive_ptr->ENTRIES_PER_SECTOR = drive_ptr->BPB.SECTOR_SIZE / sizeof(MFS_DIR_ENTRY);

    drive_ptr->FAT_START_SECTOR =  drive_ptr->BPB.RESERVED_SECTORS;

    drive_ptr->DATA_SECTOR_PTR = MFS_mem_alloc_system_zero(drive_ptr->BPB.SECTOR_SIZE);
    if ( drive_ptr->DATA_SECTOR_PTR == NULL )
    {
        error_code = MFS_INSUFFICIENT_MEMORY;
    }
    else
    {
        _mem_set_type(drive_ptr->DATA_SECTOR_PTR, MEM_TYPE_MFS_DATA_SECTOR);
        
        /* Determine if we should enable the FAT and write caches */
        if ( (ioctl(drive_ptr->DEV_FILE_PTR, IO_IOCTL_DEVICE_IDENTIFY, temp) != MQX_OK) || !(temp[IO_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_REMOVE) )
        {
            /* 
            ** Device either doesn't support the identify command or is not removable.
            ** Assume we can enable both caches
            */
            drive_ptr->WRITE_CACHE_POLICY = MFS_WRITE_BACK_CACHE;
            drive_ptr->FAT_CACHE_POLICY   = MFS_WRITE_BACK_CACHE;
            drive_ptr->READ_ONLY = FALSE;
        }
        else
        {
            // Device is removable. Disable FAT cache to ensure device is not removed without
            // fat cache flush. Enable Mixed mode write cache (only cache during consecitive file writes)
            drive_ptr->WRITE_CACHE_POLICY = MFS_MIXED_MODE_CACHE;
            drive_ptr->FAT_CACHE_POLICY   = MFS_WRITE_THROUGH_CACHE;   //
            drive_ptr->READ_ONLY = (temp[IO_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_WRITE) == 0;
        }  
        
        if ( drive_ptr->FAT_TYPE != MFS_FAT32 )
        {
            drive_ptr->ROOT_START_SECTOR = drive_ptr->FAT_START_SECTOR + (drive_ptr->BPB.SECTORS_PER_FAT * drive_ptr->BPB.NUMBER_OF_FAT);
            drive_ptr->DATA_START_SECTOR = drive_ptr->ROOT_START_SECTOR + drive_ptr->BPB.ROOT_ENTRIES/drive_ptr->ENTRIES_PER_SECTOR;
        }
        else
        {
            if ( !error_code )
            {
                drive_ptr->BPB32.FAT_SIZE = dtohl(bpb32_ptr->FAT_SIZE);
                drive_ptr->BPB32.EXT_FLAGS = dtohs(bpb32_ptr->EXT_FLAGS);
                drive_ptr->BPB32.FS_VER = dtohs(bpb32_ptr->FS_VER);
                drive_ptr->BPB32.ROOT_CLUSTER = dtohl(bpb32_ptr->ROOT_CLUSTER);
                drive_ptr->BPB32.FS_INFO = dtohs(bpb32_ptr->FS_INFO);
                drive_ptr->BPB32.BK_BOOT_SEC = dtohs(bpb32_ptr->BK_BOOT_SEC);

                drive_ptr->ROOT_START_SECTOR = 0;
                drive_ptr->DATA_START_SECTOR = drive_ptr->FAT_START_SECTOR + (drive_ptr->BPB32.FAT_SIZE * drive_ptr->BPB.NUMBER_OF_FAT);
                drive_ptr->CUR_DIR_CLUSTER = drive_ptr->BPB32.ROOT_CLUSTER;

                /* 
                ** Reset the FSInfo->Free_Count and the FSInfo->Next_Free to 
                ** unknown (0xFFFFFFFF). MFS uses it's own internal version of these
                ** fields. If Windows uses the same disk, it will recalculate the 
                ** correct fields the first time it mounts the drive.
                */

                /* Check filesystem version for FAT32 */
                if ( drive_ptr->BPB32.FS_VER > MFS_FAT32_VER )
                {
                    error_code = MFS_ERROR_UNKNOWN_FS_VERSION;
                }

                if ( !error_code )
                {
                    fsinfo_ptr = (FILESYSTEM_INFO_DISK_PTR) drive_ptr->DATA_SECTOR_PTR;
                    error_code = MFS_Read_device_sector(drive_ptr, FSINFO_SECTOR, (char_ptr)fsinfo_ptr);

                    if ( (dtohl(fsinfo_ptr->LEAD_SIG) == FSI_LEADSIG) &&  (dtohl(fsinfo_ptr->STRUCT_SIG) == FSI_STRUCTSIG) &&
                         (dtohl(fsinfo_ptr->TRAIL_SIG) == FSI_TRAILSIG) )
                    {
                        drive_ptr->FREE_COUNT = dtohl(fsinfo_ptr->FREE_COUNT);
                        drive_ptr->NEXT_FREE_CLUSTER = dtohl(fsinfo_ptr->NEXT_FREE);
                    }

                    htodl(fsinfo_ptr->LEAD_SIG, FSI_LEADSIG);
                    htodl(fsinfo_ptr->STRUCT_SIG, FSI_STRUCTSIG);
                    htodl(fsinfo_ptr->FREE_COUNT, FSI_UNKNOWN);   /* compute it */
                    htodl(fsinfo_ptr->NEXT_FREE, FSI_UNKNOWN);  /* compute it */
                    htodl(fsinfo_ptr->TRAIL_SIG, FSI_TRAILSIG);
#if !MFSCFG_READ_ONLY
#if MFSCFG_READ_ONLY_CHECK
                    if (! MFS_is_read_only (NULL, drive_ptr))
                    {
#endif
                        if (error_code == MFS_NO_ERROR)
                        {
                            error_code = MFS_Write_device_sector(drive_ptr, FSINFO_SECTOR, (char_ptr)fsinfo_ptr);
                        }
#if MFSCFG_READ_ONLY_CHECK
                    }
#endif
#endif
                }
            }
        }  

        drive_ptr->LAST_CLUSTER   = (drive_ptr->BPB.MEGA_SECTORS - drive_ptr->DATA_START_SECTOR) / drive_ptr->BPB.SECTORS_PER_CLUSTER + 1;

        if ( !error_code )
        {
            /* Set the fat buffer size */
            if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
                drive_ptr->FAT_CACHE_SIZE = drive_ptr->BPB32.FAT_SIZE;
            else
                drive_ptr->FAT_CACHE_SIZE = drive_ptr->BPB.SECTORS_PER_FAT;

            if ( drive_ptr->FAT_CACHE_SIZE > MFSCFG_FAT_CACHE_SIZE )
            {
                drive_ptr->FAT_CACHE_SIZE = (MFSCFG_FAT_CACHE_SIZE < 2 ? 2 : MFSCFG_FAT_CACHE_SIZE);
            }
            
            drive_ptr->FAT_CACHE_PTR = MFS_mem_alloc_system_zero( drive_ptr->FAT_CACHE_SIZE * drive_ptr->BPB.SECTOR_SIZE);

            if ( drive_ptr->FAT_CACHE_PTR == NULL )
            {
                error_code = MFS_INSUFFICIENT_MEMORY;
            }
            else
            {
                _mem_set_type(drive_ptr->FAT_CACHE_PTR, MEM_TYPE_MFS_FAT_BUFFER);

                drive_ptr->FAT_CACHE_DIRTY = FALSE;
                drive_ptr->FAT_CACHE_START = 1;
                /*
                ** force ignorance of 0, so the first fat sector can be
                ** read in
                */

                error_code = MFS_Read_fat(drive_ptr, 0);
                
                drive_ptr->DOS_DISK = TRUE;
                MFS_Invalidate_data_sector(drive_ptr);
  
            }  
        }
    }

    if ( error_code )
    {
        MFS_free_drive_data(drive_ptr, TRUE);
    }

    return error_code;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Parse_Out_Device_name
* Returned Value   : A pointer to character following the semi-colon (':')
* Comments  :   Isolates the pathname and filename
*
*END*---------------------------------------------------------------------*/

char_ptr MFS_Parse_Out_Device_Name
    (
    char_ptr          name_ptr
    )
{
    char_ptr             start_path_ptr;

    start_path_ptr = name_ptr;

    /* Parse out the device name */
    while ( (*start_path_ptr  != ':') && (*start_path_ptr != '\0') )
    {
        start_path_ptr = start_path_ptr + 1;   
    }  

    if ( *start_path_ptr == '\0' )
    {
        start_path_ptr = name_ptr;
    }
    else
    {
        start_path_ptr++;
    } 

    return(start_path_ptr );
}  


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MFS_Open_Device
* Returned Value   : MQX_OK or an error
* Comments         : Opens and initializes MFS driver.
* 
*END*----------------------------------------------------------------------*/

int_32 MFS_Open_Device
    (
    MQX_FILE_PTR             fd_ptr,        /* [IN] the MFS file handle for the device being opened */
    MFS_DRIVE_STRUCT_PTR     drive_ptr
    )
{
    MQX_FILE_PTR dev_fd;
    uint_32     sector_size, k;
    int_32      error_code;

    dev_fd = drive_ptr->DEV_FILE_PTR;

    fd_ptr->DEV_DATA_PTR = NULL;
    drive_ptr->MFS_FILE_PTR = fd_ptr;

    /* Select partition, if desired */
    if (drive_ptr->DRV_NUM)
    {
        error_code = ioctl(dev_fd, IO_IOCTL_SEL_PART, &drive_ptr->DRV_NUM);
        if (error_code)
        {
            return error_code;
        }
    }

    /*
    ** obtain the buffer for configuration data and for storing general
    ** sector reads
    */
    error_code = _mfs_validate_device(dev_fd, &sector_size, &drive_ptr->BLOCK_MODE);

    if ( error_code )
    {
        /* Device isn't valid */
        drive_ptr->MFS_FILE_PTR = NULL;      
        return error_code;
    }

    _lwsem_wait(&drive_ptr->SEM);

    drive_ptr->BPB.SECTOR_SIZE = (uint_16) sector_size; 
    drive_ptr->DIR_SECTOR_PTR = MFS_mem_alloc_system_zero(sector_size);
    MFS_Invalidate_directory_sector(drive_ptr);

    if ( drive_ptr->DIR_SECTOR_PTR == NULL )
    {
        _lwsem_post(&drive_ptr->SEM);
        drive_ptr->MFS_FILE_PTR = NULL;      
        return MFS_INSUFFICIENT_MEMORY;
    }

    _mem_set_type(drive_ptr->DIR_SECTOR_PTR, MEM_TYPE_MFS_DIRECTORY_SECTOR); 
    _queue_init(&drive_ptr->HANDLE_LIST, 0);

    k = drive_ptr->BPB.SECTOR_SIZE;
    for ( drive_ptr->SECTOR_POWER = 0; !(k & 1);
        drive_ptr->SECTOR_POWER++ )
    {
        k>>=1;
    } 

    /*
    ** read boot sector and get the BIOS Parameter Block
    */
    error_code = MFS_Read_device_sector(drive_ptr, BOOT_SECTOR, drive_ptr->DIR_SECTOR_PTR);

    if ( error_code == MFS_NO_ERROR )
    {
        drive_ptr->DIR_SECTOR_NUMBER = BOOT_SECTOR;
        error_code = MFS_Mount_drive_internal(drive_ptr);
    }

    if ( !error_code )
    {
        /* Determine the real sector size */
        if ( sector_size != drive_ptr->BPB.SECTOR_SIZE )
        {
            _mem_free(drive_ptr->DIR_SECTOR_PTR);
            drive_ptr->DIR_SECTOR_PTR = MFS_mem_alloc_system_zero(drive_ptr->BPB.SECTOR_SIZE);
            MFS_Invalidate_directory_sector(drive_ptr);
            if ( drive_ptr->DIR_SECTOR_PTR == NULL )
            {
                error_code = MFS_INSUFFICIENT_MEMORY;
                drive_ptr->MFS_FILE_PTR = NULL;
            }
            else
            {
                _mem_set_type(drive_ptr->DIR_SECTOR_PTR, MEM_TYPE_MFS_DIRECTORY_SECTOR); 
            }
        }

        /* Calculate the free space on disk */
#if MFSCFG_CALCULATE_FREE_SPACE_ON_OPEN
        if ( !error_code )
        {
            MFS_Get_disk_free_space_internal(drive_ptr,(uint_32_ptr)&error_code);
        }
#endif
    }

    _lwsem_post(&drive_ptr->SEM);

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MFS_Close_Device
* Returned Value   : MQX_OK or an error
* Comments         : Closes MFS driver.
* 
*END*----------------------------------------------------------------------*/

int_32 MFS_Close_Device
    (
    MQX_FILE_PTR             fd_ptr /* [IN] the MFS file handle for the device being closed */
    )
{
    MFS_DRIVE_STRUCT_PTR      drive_ptr;
    FILESYSTEM_INFO_DISK_PTR  fsinfo_ptr;
    int_32                    result = MFS_NO_ERROR;

#if !MFSCFG_READ_ONLY
#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (fd_ptr, NULL))
    {
        result = MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif
    if (result != MFS_DISK_IS_WRITE_PROTECTED)
    {
        result = _io_ioctl(fd_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL); 
    }
#endif

    MFS_lock(fd_ptr, &drive_ptr);

#if !MFSCFG_READ_ONLY
    if (result != MFS_DISK_IS_WRITE_PROTECTED)
    {
        MFS_Flush_caches(drive_ptr);
    }
#endif

    if ( _queue_is_empty(&drive_ptr->HANDLE_LIST) )
    {
        if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
        {
#if !MFSCFG_READ_ONLY
            if (result != MFS_DISK_IS_WRITE_PROTECTED)
            {
                fsinfo_ptr = (FILESYSTEM_INFO_DISK_PTR)drive_ptr->DATA_SECTOR_PTR;
                if ( fsinfo_ptr != NULL )
                {
                    htodl(fsinfo_ptr->LEAD_SIG,   FSI_LEADSIG);
                    htodl(fsinfo_ptr->STRUCT_SIG, FSI_STRUCTSIG);
                    htodl(fsinfo_ptr->FREE_COUNT, drive_ptr->FREE_COUNT);
                    htodl(fsinfo_ptr->NEXT_FREE,  drive_ptr->NEXT_FREE_CLUSTER);
                    htodl(fsinfo_ptr->TRAIL_SIG,  FSI_TRAILSIG);

                    MFS_Write_device_sector(drive_ptr, FSINFO_SECTOR, (char_ptr)fsinfo_ptr);
                }
            }
#endif
        }

        MFS_free_drive_data(drive_ptr, TRUE);

        drive_ptr->MFS_FILE_PTR = NULL;
        result = MFS_NO_ERROR;
    }
    else
    {
        result = MFS_SHARING_VIOLATION;
    }  

    MFS_unlock(drive_ptr,FALSE);

    return result;

}  


/*FUNCTION*--------------------------------------------------------------
* 
* Function Name    : _mfs_partition_destroy
* Returned Value   : MQX_OK or an error code
* Comments         :
*    Destroys a partition allocated in kernel memory
*
*END*-----------------------------------------------------------------*/

uint_32 _mfs_partition_destroy
    (
    _partition_id partition /* [IN] the partition to destory */
    )
{
    return _partition_destroy(partition);
}
