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
* $FileName: mfs_time.c$
* $Version : 3.6.8.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the file specific interface functions
*   of the MFS.
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Get_date_time
* Returned Value   : error_code (MFS_NO_ERROR or MFS_INVALID_FILE_HANDLE)
* Comments  :   Get the date and time of last update of a specific file.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Get_date_time
    (
    MQX_FILE_PTR            fd_ptr,     /*[IN] get date/time of this file */
    uint_16_ptr             date_ptr,   /*[IN/OUT] file date is written to this address */
    uint_16_ptr             time_ptr    /*[IN/OUT] file time is written to this address */
    )
{
   
   MFS_DRIVE_STRUCT_PTR    drive_ptr;
   MFS_HANDLE_PTR          handle;
   _mfs_error              error_code;
   MFS_DIR_ENTRY_PTR       dir_entry_ptr;
   
   error_code = MFS_lock_handle( fd_ptr, &drive_ptr, &handle );
   if ( error_code != MFS_NO_ERROR )
   {
      return error_code;
   }
   
   dir_entry_ptr = ((MFS_DIR_ENTRY_PTR) drive_ptr->DIR_SECTOR_PTR) + INDEX_TO_OFFSET(handle->DIR_INDEX);
   /*
   ** Check the date and time ptrs; if any is NULL, don't write to it.
   */
   if ( date_ptr )
   {
      *date_ptr = dtohs(dir_entry_ptr->DATE);
   }
   if ( time_ptr )
   {
      *time_ptr = dtohs(dir_entry_ptr->TIME);
   }
   
   MFS_unlock(drive_ptr,FALSE);
   
   return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Set_date_time
* Returned Value   : error_code
* Comments  :   Set the date and time of last update of a specific file.
*
*END*---------------------------------------------------------------------*/
#if !MFSCFG_READ_ONLY

_mfs_error MFS_Set_date_time
   (
   MQX_FILE_PTR            fd_ptr,     /*[IN] get date/time of this file */
   uint_16_ptr             date_ptr,   /*[IN] file date to be written into this file's entry */
   uint_16_ptr             time_ptr    /*[IN] file time to be written into this file's entry */
   )
{
   MFS_DRIVE_STRUCT_PTR    drive_ptr;
   MFS_HANDLE_PTR          handle;
   _mfs_error              error_code;
   
#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (fd_ptr, NULL))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif   
   
   error_code = MFS_lock_handle( fd_ptr, &drive_ptr, &handle );
   if ( error_code != MFS_NO_ERROR )
   {
      return error_code;
   }

   if ( handle->ACCESS == MFS_ACCESS_READ_ONLY )
   {
      MFS_unlock(drive_ptr,FALSE);
      return MFS_ACCESS_DENIED;
   }
   
   htods(handle->DIR_ENTRY.DATE, *date_ptr);
   htods(handle->DIR_ENTRY.TIME, *time_ptr);

   error_code = MFS_Update_entry(drive_ptr, handle);
   /* we do not want to set date and time in close, so set touched to 0 */
   if ( error_code == MFS_NO_ERROR )
   {
      handle->TOUCHED = 0;
   }
   MFS_unlock(drive_ptr,TRUE);
   
   return(error_code);
}  

#endif

