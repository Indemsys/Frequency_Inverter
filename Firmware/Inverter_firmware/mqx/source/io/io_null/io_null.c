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
* $FileName: io_null.c$
* $Version : 3.6.5.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the nulldisk driver functions
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_null.h"
#include "io_prv.h"
#include "ionulprv.h"


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_null_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a fdv_null driver.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_null_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr            identifier
   ) 
   
{ /* Body */
   _mqx_uint result;
   
   result = _io_dev_install(identifier,
      _io_null_open,
      _io_null_close,
      _io_null_read,
      _io_null_write,
      _io_null_ioctl,
      NULL); 

      return result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_null_open
* Returned Value   : a null pointer
* Comments         : Opens and initializes fdv_null driver.
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_null_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,
       
      /* [IN] the remaining portion of the name of the device */
      char_ptr   open_name_ptr,

      /* [IN] the flags to be used during operation:
      ** echo, translation, xon/xoff, encoded into a pointer.
      */
      char_ptr    flags
   )
{ /* Body */

   /* Nothing to do */
   return(MQX_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_null_close
* Returned Value   : ERROR CODE
* Comments         : Closes fdv_null driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_null_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */

   /* Nothing to do */
   return(MQX_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_null_read
* Returned Value   : number of characters read
* Comments         : Reads data from fdv_ram driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_null_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are to be stored */
      char_ptr   data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int   num
   )
{ /* Body */
   return(0);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_null_write
* Returned Value   : number of characters written
* Comments         : Writes data to the fdv_ram device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_null_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are */
      char_ptr   data_ptr,

      /* [IN] the number of characters to output */
      _mqx_int   num
   )
{ /* Body */
   return(num);
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_null_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_null_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint  cmd,

      /* [IN] the ioctl parameters */
      pointer    param_ptr
   )
{ /* Body */
     return IO_ERROR_INVALID_IOCTL_CMD;
} /* Endbody */

/* EOF */
