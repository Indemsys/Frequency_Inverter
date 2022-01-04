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
* $FileName: mfs_data_sector.c$
* $Version : 3.8.11.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the functions that are used to read and write to the 
*   device under MFS. 
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Flush_data_sector_buffer
* Returned Value   :  error_code
* Comments  :
*     Writes the cached sector back to disk if dirty.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Flush_data_sector_buffer
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr     /*[IN] the drive on which to operate */
    )
{
#if MFSCFG_READ_ONLY
    return MFS_NO_ERROR;
#else
    _mfs_error   error_code = MFS_NO_ERROR;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (NULL, drive_ptr))
    {
        return error_code;
    }
#endif

    if ( drive_ptr->DATA_SECTOR_DIRTY )
    {
        error_code = MFS_Write_device_sector(drive_ptr,drive_ptr->DATA_SECTOR_NUMBER,drive_ptr->DATA_SECTOR_PTR);

        if ( error_code == MFS_NO_ERROR )
        {
            drive_ptr->DATA_SECTOR_DIRTY=FALSE;
        }
    }

    return error_code;
#endif
}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Invalidate_data_sector
* Returned Value   :  error_code
* Comments  :
*     Invalidates the cached sector.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Invalidate_data_sector
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr   /*[IN] the drive on which to operate */
    )
{
#if MFSCFG_READ_ONLY
    return MFS_NO_ERROR;
#else

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (NULL, drive_ptr))
    {
        return MFS_NO_ERROR;
    }
#endif

    drive_ptr->DATA_SECTOR_NUMBER = MAX_UINT_32;
    drive_ptr->DATA_SECTOR_DIRTY=FALSE;
    return MFS_NO_ERROR;
#endif
}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Read_data_sector
* Returned Value   :  error_code
* Comments  :
*     Readsone sector into the DATA_SECTOR buffer
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Read_data_sector
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    MFS_HANDLE_PTR          handle,         /*[IN]  handle of the file for which we are doing the read/write  */
    uint_32                 sector_number,  /*[IN] sector number to read/write from/to file system medium */
    boolean                 hw_read         /*[IN] if read of a sector is needed or just validation of a new one */
    )
{
    _mfs_error error;

    error = MFS_NO_ERROR;

    if ( sector_number != drive_ptr->DATA_SECTOR_NUMBER )
    {
        MFS_Flush_data_sector_buffer(drive_ptr);
        MFS_Invalidate_data_sector(drive_ptr);

        if (hw_read)
        {
            error = MFS_Read_device_sector(drive_ptr,sector_number,drive_ptr->DATA_SECTOR_PTR);
        }
        else
        {
            error = MFS_NO_ERROR;
        }

        if ( error == MFS_NO_ERROR )
        {
            drive_ptr->DATA_SECTOR_NUMBER =  sector_number;
        }
    }

    return(error);
}
