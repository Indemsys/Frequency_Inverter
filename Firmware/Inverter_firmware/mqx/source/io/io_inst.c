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
* $FileName: io_inst.c$
* $Version : 3.6.5.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the function for installing a dynamic device
*   driver.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#ifdef NULL
  #undef NULL
#endif
#include <string.h>
#ifndef NULL
  #define NULL ((pointer)0)
#endif

#if MQX_USE_IO

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dev_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a device dynamically, so tasks can fopen to it.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_dev_install
(
 /* [IN] A string that identifies the device for fopen */
 char_ptr      identifier,

 /* [IN] The I/O open function */
 IO_OPEN_FPTR  io_open,

 /* [IN] The I/O close function */
 IO_CLOSE_FPTR io_close,

 /* [IN] The I/O read function */
 IO_READ_FPTR  io_read,

 /* [IN] The I/O write function */
 IO_WRITE_FPTR io_write,

 /* [IN] The I/O ioctl function */
 IO_IOCTL_FPTR io_ioctl,

 /* [IN] The I/O initialization data */
 pointer       io_init_data_ptr
 )
{ /* Body */

  return (_io_dev_install_ext(identifier, io_open, io_close, io_read, io_write,
                              io_ioctl, (IO_UNINSTALL_FPTR)NULL, io_init_data_ptr));

} /* Endbody */
#endif

/* EOF */
