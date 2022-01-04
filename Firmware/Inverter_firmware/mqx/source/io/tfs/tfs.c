/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: tfs.c$
* $Version : 3.8.10.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the functions that are used to initialize TFS
*   It also contains the TFS driver functions.
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include <io_prv.h>

#include <tfs.h>
#include <tfs_prv.h>
 
 
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_tfs_install
* Returned Value   : TFS error code.
* Comments         : Initialize the Trivial File System.
*
*END*---------------------------------------------------------------------*/

uint_32 _io_tfs_install
   (
      /*[IN] the name that should be given to tfs (ex: "C:", "TFS1:", etc..) */
      char_ptr              identifier,
      
      /*[IN] pointer to the first entry of the root TFS directory  */  
      const TFS_DIR_ENTRY   *root
   )
{ /* Body */
   uint_32                      error_code;
   TFS_DRIVE_STRUCT_PTR         drive_ptr;
   
   drive_ptr = _mem_alloc_system_zero( sizeof(TFS_DRIVE_STRUCT) );
   if ( drive_ptr == NULL ) {
      return( TFS_INSUFFICIENT_MEMORY );
   }/* Endif */
   _mem_set_type(drive_ptr,MEM_TYPE_IO_TFS_DRIVE_STRUCT);      
   
   _int_disable();

   error_code = _io_dev_install_ext( identifier, 
      _io_tfs_open,                     
      _io_tfs_close, 
      _io_tfs_read,                      
      _io_tfs_write,                     
      _io_tfs_ioctl,
      _io_tfs_uninstall,
      (pointer)drive_ptr);

   if (error_code != IO_OK) {
      _mem_free(drive_ptr);
      drive_ptr = NULL;
      _int_enable();      
      return error_code;
   } /* Endif */

   drive_ptr->IDENTIFIER = identifier;
   drive_ptr->ROOT     = root;
 
   _int_enable();

   return(error_code);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_tfs_uninstall
* Returned Value   : TFS error code.
* Comments  :   Uninstalls the Trivial File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/

int_32 _io_tfs_uninstall
   (
       /* [IN/OUT] The device to uninstall */            
      IO_DEVICE_STRUCT_PTR   dev_ptr
   )
{ /* Body */
   TFS_DRIVE_STRUCT_PTR   drive_ptr;
   int_32                 error;
   
   drive_ptr = (TFS_DRIVE_STRUCT_PTR)dev_ptr->DRIVER_INIT_PTR;
   if (drive_ptr) {
      _mem_free(drive_ptr);
      dev_ptr->DRIVER_INIT_PTR = NULL;
      error = TFS_NO_ERROR;
   } else {
      error = TFS_SHARING_VIOLATION;
   } /* Endif */

   return error;

} /* Endbody */

 
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_tfs_open
* Returned Value   : TFS error code.
* Comments         : Opens TFS driver and initializes given file descriptor.
* 
*END*----------------------------------------------------------------------*/

int_32 _io_tfs_open
   (
      /* [IN/OUT] the file handle for the device being opened */
      MQX_FILE_PTR             fd_ptr,
      
      /* [IN] the remaining portion of the name of the device */
      char_ptr                 open_name_ptr,

      /* [IN] the flags to specify file type:
      ** r  read
      ** w  write 
      */
      char_ptr                 flags_str
   )
{ /* Body */
    TFS_DRIVE_STRUCT_PTR        drive_ptr;
    uint_32                     error_code = TFS_NO_ERROR;
    
    drive_ptr = (TFS_DRIVE_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

    open_name_ptr = TFS_Parse_Out_Device_Name(open_name_ptr);

    /* Check for a filename, if none, then check for the temp flag */
    if ( *(open_name_ptr-1) != ':' ) {
       error_code = TFS_PATH_NOT_FOUND;         
    } else if ( *open_name_ptr == '\0' ) {
       error_code = TFS_PATH_NOT_FOUND;
    } else {
     /* 
     ** When opening a file, only read-only attributes are set. The other
     ** attributes (such as hidden, system, etc..) must be set afterwards
     ** with an ioctl call.
     */  
     switch ( *flags_str ) {
        case 'r':
           fd_ptr->DEV_DATA_PTR = TFS_Open_File(drive_ptr, open_name_ptr, &error_code);
           break;
        case 'w':
           error_code = TFS_DISK_IS_WRITE_PROTECTED;
           break;
        default:
           error_code = TFS_INVALID_PARAMETER;
           break;
     } /* Endswitch */
    } /* Endif */

    /* Check to make sure the error code is OK */
    if ( error_code == TFS_NO_ERROR ) {
      /* Initialise the file information fields */
      fd_ptr->LOCATION = 0; 
      fd_ptr->FLAGS = 0;    
      fd_ptr->ERROR = 0;    
      fd_ptr->SIZE = ((TFS_DIR_ENTRY_PTR)fd_ptr->DEV_DATA_PTR)->SIZE;
    } else {
      // An error occured, so the calling function (_io_fopen) will
      // free the MQX_FILE_PTR. Need to release the handle
      fd_ptr->ERROR = error_code;
      if (fd_ptr->DEV_DATA_PTR) {
         TFS_Close_File(fd_ptr);
      }
    }/* Endif */

   return(error_code);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_tfs_close
* Returned Value   : TFS error code.
* Comments         : Closes given file descriptor.
* 
*END*----------------------------------------------------------------------*/

int_32 _io_tfs_close
   (
      /* [IN/OUT] the file handle for the device being closed */
      MQX_FILE_PTR                   fd_ptr
   )
{ /* Body */
    uint_32                     error_code = TFS_NO_ERROR;
   
    if ( fd_ptr->DEV_DATA_PTR) {
        error_code = TFS_Close_File (fd_ptr);   
    }/* Endif */
         
    return(error_code);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_tfs_read
* Returned Value   : Number of characters read.
* Comments         : Reads data from given file.
* 
*END*----------------------------------------------------------------------*/

int_32 _io_tfs_read
   ( 
      /* [IN/OUT] the stream to perform the operation on */
      MQX_FILE_PTR file_ptr,

      /* [IN] the data location to write to */
      char_ptr    data_ptr,

      /* [IN] the number of bytes to read */
      int_32      num      
   )
{
    uint_32        bytes_read;
    uint_32        error_code = TFS_NO_ERROR;

    bytes_read = TFS_Read(file_ptr, num, data_ptr, &error_code);

    /* Check for EOF. The TFS EOF must be translated to the standard EOF */
    file_ptr->FLAGS = 0;
    if ( error_code == TFS_EOF ) 
    {
        file_ptr->FLAGS = IO_FLAG_AT_EOF;
    }
    if (error_code != TFS_NO_ERROR)
    {
        file_ptr->ERROR = error_code;
    }
    return( (int_32) bytes_read );
}   


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_tfs_write
* Returned Value   : Number of characters written.
* Comments         : Writes data to the given file.
* 
*END*----------------------------------------------------------------------*/

int_32 _io_tfs_write
   ( 
      /* [IN/OUT] the stream to perform the operation on */
      MQX_FILE_PTR file_ptr,

      /* [IN] the data location to read from */
      char_ptr    data_ptr,

      /* [IN] the number of bytes to read */
      int_32      num      
   )
{ /* Body */
    uint_32        bytes_written;
    uint_32        error_code = TFS_NO_ERROR;
   
    bytes_written = TFS_Write(file_ptr, num, data_ptr, &error_code);
    
    if (error_code != TFS_NO_ERROR)
    {
        file_ptr->ERROR = error_code;
    }
    return ((int_32)bytes_written);
   
} /* Endbody */  


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_tfs_ioctl
* Returned Value   : TFS error code.
* Comments         : Performs specified operation related to given file.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_tfs_ioctl
   ( 
      /* [IN] the stream to perform the operation on */
      MQX_FILE_PTR file_ptr,

      /* [IN] the ioctl command */
      _mqx_uint   cmd,

      /* [IN/OUT] the ioctl parameters */
      pointer     param_ptr
   )
{ /* Body */
    uint_32        error_code = TFS_NO_ERROR;
   
   switch (cmd) {
      case IO_IOCTL_SEEK:
        TFS_Move_File_Pointer(file_ptr, &error_code);
        file_ptr->FLAGS = 0;
        if ( error_code == TFS_EOF ) 
        {
            file_ptr->FLAGS = IO_FLAG_AT_EOF;
        }
        break;
      case IO_IOCTL_TFS_GET_NAME:
        if (file_ptr->DEV_DATA_PTR == NULL) 
        {
            error_code = TFS_ERROR_INVALID_FILE_HANDLE;
        }
        else
        {
            *((char_ptr _PTR_)param_ptr) = ((TFS_DIR_ENTRY_PTR)file_ptr->DEV_DATA_PTR)->NAME;
        }
        break;
      case IO_IOCTL_TFS_GET_ATTRIBUTES:
        if (file_ptr->DEV_DATA_PTR == NULL) 
        {
            error_code = TFS_ERROR_INVALID_FILE_HANDLE;
        }
        else
        {
            *((uint_32_ptr)param_ptr) = ((TFS_DIR_ENTRY_PTR)file_ptr->DEV_DATA_PTR)->FLAGS;
        }
        break;
      case IO_IOCTL_TFS_GET_LENGTH:
        if (file_ptr->DEV_DATA_PTR == NULL) 
        {
            error_code = TFS_ERROR_INVALID_FILE_HANDLE;
        }
        else
        {
            *((uint_32_ptr)param_ptr) = file_ptr->SIZE;
        }
        break;
      case IO_IOCTL_TFS_GET_CURRENT_DATA_PTR:
        if (file_ptr->DEV_DATA_PTR == NULL) 
        {
            error_code = TFS_ERROR_INVALID_FILE_HANDLE;
        }
        else
        {
            *((uchar_ptr _PTR_)param_ptr) = ((TFS_DIR_ENTRY_PTR)file_ptr->DEV_DATA_PTR)->DATA+file_ptr->LOCATION;
        }
        break;
      case IO_IOCTL_TFS_GET_LAST_ERROR:
        error_code = file_ptr->ERROR;
        break;
      default:
        error_code = IO_ERROR_INVALID_IOCTL_CMD;
        break;
   } /* Endswitch */
                  
   return( (_mqx_uint) error_code);
} /* Endbody */
  

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TFS_Parse_Out_Device_name
* Returned Value   : A pointer to character following the semi-colon (':').
* Comments         : Isolates the devoce name and file name.
*
*END*---------------------------------------------------------------------*/

char_ptr TFS_Parse_Out_Device_Name
   (
      /* [IN] the stream to perform the operation on */
      char_ptr          name_ptr
   )
{ /* Body */
   char_ptr             start_path_ptr;
   
   start_path_ptr = name_ptr;
   
   /* Parse out the device name */
   while ((*start_path_ptr  != ':') && (*start_path_ptr != '\0') ) {
      start_path_ptr = start_path_ptr + 1;   
   } /* Endwhile */
   
   if ( *start_path_ptr == '\0' ) {
      start_path_ptr = name_ptr;
   } else {
      start_path_ptr++;
   }/* Endif */
   
   return (start_path_ptr );
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TFS_Cmp
* Returned Value   : -1, 0, 1 depending on path1 <, ==, > path2.
* Comments         : Compares file paths. Not case sensitive. Both delimiters 
*                    '/' and '\' supported.
*
*END*---------------------------------------------------------------------*/

int_32 TFS_Cmp 
    (
        /* [IN] first file path to compare */
        char_ptr path1, 
        
        /* [IN] second file path to compare */
        char_ptr path2
    )
{ /* Body */
    uint_32 ch1, ch2;
    
    if (path1 == path2) return 0;
    if (path1 == NULL) return -1;
    if (path2 == NULL) return 1;
    do 
    {
        ch1 = *path1++;
        ch2 = *path2++;
        if ((uint_32)(ch1 - 'a') <= (uint_32)('z' - 'a')) ch1 = ch1 - 'a' + 'A';
        if (ch1 == '\\') ch1 = '/';
        if ((uint_32)(ch2 - 'a') <= (uint_32)('z' - 'a')) ch2 = ch2 - 'a' + 'A';
        if (ch2 == '\\') ch2 = '/';
        if ((ch1 == '\0') || (ch2 == '\0')) return (int_32)(ch1 - ch2);
    } while (ch1 == ch2);
    return (int_32)(ch1 - ch2);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TFS_Open_File
* Returned Value   : Pointer to TFS directory entry or NULL.
* Comments         : Searches for specified file and returns directory entry.
*
*END*---------------------------------------------------------------------*/

pointer TFS_Open_File
    (
        /* [IN] TFS IO drive information */
        TFS_DRIVE_STRUCT_PTR   drive_ptr,

        /* [IN] file path */
        char_ptr               pathname,

        /* [OUT] error result */
        uint_32_ptr            error_ptr
    )
{ /* Body */
    TFS_DIR_ENTRY_PTR entry;
    
    if ((drive_ptr == NULL) || (pathname == NULL) || (*pathname == '\0')) 
    {
        *error_ptr = TFS_INVALID_PARAMETER;
        return NULL;
    } 
    *error_ptr = TFS_FILE_NOT_FOUND;
    entry = (TFS_DIR_ENTRY_PTR)drive_ptr->ROOT;
    while (entry->NAME != NULL)
    {
        if (TFS_Cmp (entry->NAME, pathname) == 0)
        {
            *error_ptr = TFS_NO_ERROR;
            return (pointer)entry;
        }
        entry++;
    }
    return NULL;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TFS_Read
* Returned Value   : Number of bytes actually read.
* Comments         : Reads given file and stores data into the given buffer.
*
*END*---------------------------------------------------------------------*/

uint_32 TFS_Read
    (
        /* [IN/OUT] file to read from */
        MQX_FILE_PTR           file_fd_ptr,

        /* [IN] number of bytes to read */
        uint_32                num_bytes,

        /* [OUT] buffer to write the data to */
        char_ptr               buffer_address,

        /* [OUT] error information */
        uint_32_ptr            error_ptr
    )
{ /* Body */
    uint_32 i;
    uchar_ptr data;
    
    *error_ptr = TFS_NO_ERROR;
    if (file_fd_ptr->DEV_DATA_PTR == NULL) 
    {
        *error_ptr = TFS_ERROR_INVALID_FILE_HANDLE;
        return 0;
    }
    if (file_fd_ptr->LOCATION >= file_fd_ptr->SIZE)
    {
        *error_ptr = TFS_EOF;
        return 0;
    }
    if (num_bytes > file_fd_ptr->SIZE - file_fd_ptr->LOCATION) 
    {
        num_bytes = file_fd_ptr->SIZE - file_fd_ptr->LOCATION;
    }
    data = ((TFS_DIR_ENTRY_PTR)file_fd_ptr->DEV_DATA_PTR)->DATA + file_fd_ptr->LOCATION;
    for (i = num_bytes; i != 0; i--) *buffer_address++ = *data++;
    file_fd_ptr->LOCATION += num_bytes;
    return num_bytes;   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TFS_Write
* Returned Value   : Number of bytes actually written.
* Comments         : Writes data from given buffer into the given file.
*
*END*---------------------------------------------------------------------*/

uint_32 TFS_Write
    (
        /* [IN/OUT] file to write to */
        MQX_FILE_PTR           file_fd_ptr,

        /* [IN] number of bytes to write */
        uint_32                num_bytes,

        /* [IN] buffer to read data from */
        char_ptr               buffer_address,

        /* [OUT] error information */
        uint_32_ptr            error_ptr
    )
{ /* Body */
    *error_ptr = TFS_DISK_IS_WRITE_PROTECTED;
    if (file_fd_ptr->DEV_DATA_PTR == NULL) 
    {
        *error_ptr = TFS_ERROR_INVALID_FILE_HANDLE;
    }
    return 0;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TFS_Move_File_Pointer
* Returned Value   : New file location.
* Comments         : Performs seek within given file.
*
*END*---------------------------------------------------------------------*/

uint_32 TFS_Move_File_Pointer
    (  
        /* [IN/OUT] file to seek within */
        MQX_FILE_PTR           file_fd_ptr,

        /* [OUT] error information */
        uint_32_ptr            error_ptr
    )
{ /* Body */
    *error_ptr = TFS_NO_ERROR;
    if (file_fd_ptr->DEV_DATA_PTR == NULL) 
    {
        *error_ptr = TFS_ERROR_INVALID_FILE_HANDLE;
        return 0;
    }
    if (file_fd_ptr->LOCATION > file_fd_ptr->SIZE) 
    {
        *error_ptr = TFS_EOF;
        file_fd_ptr->LOCATION = file_fd_ptr->SIZE;
    }
    return(file_fd_ptr->LOCATION);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TFS_Move_File_Pointer
* Returned Value   : Error information.
* Comments         : Closes given file.
*
*END*---------------------------------------------------------------------*/

int_32 TFS_Close_File
    (
        /* [IN/OUT] file to seek within */
        MQX_FILE_PTR           file_fd_ptr
    )
{ /* Body */
    file_fd_ptr->DEV_DATA_PTR = NULL;
    return TFS_NO_ERROR;
} /* Endbody */
