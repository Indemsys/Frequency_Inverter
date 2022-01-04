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
* $FileName: mfs_valdev.c$
* $Version : 3.8.9.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the function for making sure the device on which
*   MFS is being run on is suitable
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include <io_prv.h>

#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _mfs_validate_device
* Returned Value   : MFS_NO_ERROR or an error
* Comments         : Checks the lower layer device for compatibility
* 
*END*----------------------------------------------------------------------*/

_mfs_error _mfs_validate_device
    (
    MQX_FILE_PTR             dev_fd,            /* [IN] the file handle for the lower layer device */
    uint_32_ptr              sector_size_ptr,   /* [OUT] Pointer to where the sector size is to be stored  */
    boolean _PTR_            block_mode_ptr     /* [OUT] Pointer to where the block mode is to be stored  */
    )
{
    _mfs_error      error_code;
#ifdef IO_DEV_ATTR_BLOCK_MODE
    uint_32         id_array[3];

    id_array[0] = id_array[1] = id_array[2] = 0;

    /* 
    ** Issue the id command. Block mode drivers must support this command but
    ** other drivers can support it also
    */
    error_code = _io_ioctl(dev_fd, IO_IOCTL_DEVICE_IDENTIFY, id_array);

    if ( error_code == IO_OK )
    {
        /* 
        ** The identify command is supported. Check to see if it is a block mode
        ** driver
        */
        if ( id_array[IO_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_BLOCK_MODE )
        {
            *block_mode_ptr = TRUE;
        }
        else
        {
            *block_mode_ptr = FALSE;
        }  
    }
    else if ( error_code == IO_ERROR_INVALID_IOCTL_CMD )
    {
        /* 
        ** The ID command is not supported by the lower layer. It is not a block
        ** mode driver
        */
        *block_mode_ptr = FALSE;
    }
    else
    {
        /* Something bad happened at the lower layer */
        return( error_code );
    }  
#else
    *block_mode_ptr = FALSE;
#endif

    *sector_size_ptr = MFS_DEFAULT_SECTOR_SIZE;
    error_code  = _io_ioctl(dev_fd, IO_IOCTL_GET_BLOCK_SIZE, sector_size_ptr);

    if ( error_code == IO_ERROR_INVALID_IOCTL_CMD )
    {
        /* 
        ** The command is not supported. This is OK as long as it isn't a block
        ** mode driver.
        */
        if ( *block_mode_ptr )
        {
            /* MFS needs to know the block size for block mode drivers */
            error_code = MFS_INVALID_DEVICE;
        }
        else
        {
            /* 
            ** It doesn't matter that we can't tell the actual block size. MFS 
            ** will be doing byte accesses anyway since it isn't a block mode 
            ** driver.
            */
            *sector_size_ptr = MFS_DEFAULT_SECTOR_SIZE;
            error_code = MFS_NO_ERROR;
        }  
    }
    else if ( error_code == IO_OK )
    {
        /* Check to see if the sector or block size is suitable for MFS */
        if ( !((*sector_size_ptr == 512) || (*sector_size_ptr == 1024) || 
            (*sector_size_ptr == 2048) || (*sector_size_ptr == 4096)) )
        {
            /* 
            ** The block size isn't compatible. This may still be OK as long as we 
            ** are not dealing with a block mode driver otherwise MFS won't be 
            ** able to work with it
            */
            if ( *block_mode_ptr )
            {
                error_code = MFS_INVALID_DEVICE;
            }

        }
    }

    return(error_code);
}
