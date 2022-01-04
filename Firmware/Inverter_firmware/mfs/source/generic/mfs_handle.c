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
* $FileName: mfs_handle.c$
* $Version : 3.8.12.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains functions related to using file handles.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Get_handle
* Returned Value   :  a file handle
* Comments  :
*    Creates a new file handle and adds it the to queue of open handles.
*
*END*---------------------------------------------------------------------*/

MFS_HANDLE_PTR MFS_Get_handle
    (
    MFS_DRIVE_STRUCT_PTR   drive_ptr,       /*[IN] the drive on which to operate */
    MFS_DIR_ENTRY_PTR      dir_entry_ptr    /* [IN] pointer to the directory information for this file handle */
    )
{
    MFS_HANDLE_PTR    handle_ptr;

    handle_ptr = _partition_alloc_system_zero( drive_ptr->HANDLE_PARTITION );

    /*
    ** initialize the handle
    */
    if ( handle_ptr != NULL )
    {

        _queue_enqueue( &drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) handle_ptr);

        handle_ptr->VALID = MFS_VALID;

        if ( dir_entry_ptr )
        {
            _mem_copy(dir_entry_ptr, &handle_ptr->DIR_ENTRY,sizeof(MFS_DIR_ENTRY));
        }
    }
    return(handle_ptr);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Free_handle
* Returned Value   :  none
* Comments  :
*      Add the handle to the freelist of handles kept in the configuration
*       data.
*END*---------------------------------------------------------------------*/

void MFS_Free_handle
    (
    MFS_DRIVE_STRUCT_PTR      drive_ptr,    /*[IN] the drive on which to operate */
    MFS_HANDLE_PTR            handle_ptr    /*[IN] handle to be released */
    )
{
    if ( handle_ptr != NULL )
    {
        /* Unlink handle from HANDLE_LIST */
        _queue_unlink( &drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) handle_ptr);

        /* clear out the handle and add to the freelist */
        handle_ptr->VALID = ~handle_ptr->VALID;
        _partition_free(handle_ptr);
    }

}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Find_handle
* Returned Value   :  void
* Comments  :
*
*END*---------------------------------------------------------------------*/

MFS_HANDLE_PTR MFS_Find_handle
    (
    MFS_DRIVE_STRUCT_PTR   drive_ptr,    /*[IN] the drive on which to operate */
    uint_32                dir_cluster,
    uint_32                dir_index
    )
{
   MFS_HANDLE_PTR   next_handle;

    next_handle = (MFS_HANDLE_PTR) _queue_head(&(drive_ptr->HANDLE_LIST));
    while ( next_handle )
    {
        if ((next_handle->DIR_CLUSTER == dir_cluster) && (next_handle->DIR_INDEX == dir_index))  
        {
            break;
        }
        next_handle = (MFS_HANDLE_PTR) _queue_next(&drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) next_handle);
    }  
    return next_handle;
}  
