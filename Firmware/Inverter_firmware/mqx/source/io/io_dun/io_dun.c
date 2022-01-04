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
* $FileName: io_dun.c$
* $Version : 3.8.11.0$
* $Date    : Sep-26-2012$
*
* Comments:
*
*   This file contains a driver used by PPP to communicate to
*   Windows Dial-Up Networking or to Windows NT RAS.
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include "io_dun.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dun_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install the Dial-Up Networking driver.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_dun_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr    identifier
   )
{ /* Body */
   IODUN_DEV_STRUCT_PTR dev_ptr = _mem_alloc_system_zero(sizeof(IODUN_DEV_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   } /* Endif */
#endif
   _mem_set_type(dev_ptr,MEM_TYPE_IO_DUN_DEV);               

   dev_ptr->RECV      = "CLIENTCLIENT";
   dev_ptr->SEND      = "CLIENTSERVER";
   dev_ptr->RECV_SIZE = 12;
   dev_ptr->SEND_SIZE = 12;

   return (_io_dev_install(identifier,
      _io_dun_open,
      _io_dun_close,
      _io_dun_read,
      _io_dun_write,
      _io_dun_ioctl,
      dev_ptr)); 
 
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_ras_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install the RAS driver.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_ras_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr    identifier
   )
{ /* Body */
   IODUN_DEV_STRUCT_PTR dev_ptr = _mem_alloc_system_zero(sizeof(IODUN_DEV_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   } /* Endif */
#endif
   _mem_set_type(dev_ptr,MEM_TYPE_IO_DUN_DEV);               

   dev_ptr->RECV      = "CLIENTSERVER";
   dev_ptr->SEND      = "CLIENTCLIENT";
   dev_ptr->RECV_SIZE = 12;
   dev_ptr->SEND_SIZE = 12;

   return (_io_dev_install(identifier,
      _io_dun_open,
      _io_dun_close,
      _io_dun_read,
      _io_dun_write,
      _io_dun_ioctl,
      dev_ptr)); 
 
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dun_open
* Returned Value   : MQX_OK
* Comments         : Opens and initializes the DUN or RAS driver.
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_dun_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,
      
      /* [IN] the name of the device */
      char_ptr    open_name_ptr,

      /* [IN] the flags to be used during operation:
      ** echo, translation, xon/xoff, encoded into a pointer.
      */
      char_ptr    f_ptr
   )
{  /* Body */
   IODUN_DEV_STRUCT_PTR dev_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
   IODUN_STRUCT_PTR ras_ptr = _mem_alloc_system_zero(sizeof(IODUN_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (ras_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   } /* Endif */
#endif
   _mem_set_type(ras_ptr,MEM_TYPE_IO_DUN);                   
   fd_ptr->DEV_DATA_PTR = ras_ptr;

   _lwsem_create(&ras_ptr->LWSEM,1);

   ras_ptr->STATE = 0;
   ras_ptr->F_PTR = (MQX_FILE_PTR)f_ptr;
   ras_ptr->PARSE = dev_ptr->RECV;

   _io_write(ras_ptr->F_PTR, dev_ptr->SEND, dev_ptr->SEND_SIZE);

   return(MQX_OK);   

} /* Endbody */ 

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dun_close
* Returned Value   : ERROR CODE
* Comments         : Closes ras driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_dun_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
   IODUN_STRUCT_PTR ras_ptr = fd_ptr->DEV_DATA_PTR;

   _lwsem_destroy(&ras_ptr->LWSEM);
   _mem_free(ras_ptr);

   return(MQX_OK);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dun_read
* Returned Value   : number of characters read
* Comments         : Reads data from ras driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_dun_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are to be stored */
      char_ptr    data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int    num
   )
{ /* Body */
   IODUN_STRUCT_PTR ras_ptr = fd_ptr->DEV_DATA_PTR;
   _mqx_int         numchar = num;

   _lwsem_wait(&ras_ptr->LWSEM);

   while (numchar--) {
      *data_ptr++ = _io_dun_read_char(fd_ptr);
   } /* Endwhile */

   _lwsem_post(&ras_ptr->LWSEM);
   return(num);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dun_write
* Returned Value   : number of characters written
* Comments         : Writes data to the ras device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_dun_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are */
      char_ptr    data_ptr,

      /* [IN] the number of characters to output */
      _mqx_int    num
   )
{ /* Body */
   IODUN_STRUCT_PTR ras_ptr = fd_ptr->DEV_DATA_PTR;

   return _io_write(ras_ptr->F_PTR, data_ptr, num);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dun_ioctl
* Returned Value   : _mqx_int
* Comments         :
*    Returns result of ioctl operation.
*
*END*-----------------------------------------------------------------------*/

_mqx_int _io_dun_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint   cmd,

      /* [IN] the ioctl parameters */
      pointer     param_ptr
   )
{ /* Body */
   IODUN_STRUCT_PTR ras_ptr = fd_ptr->DEV_DATA_PTR;

   return _io_ioctl(ras_ptr->F_PTR, cmd, param_ptr);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_dun_read_char
* Returned Value   : 
* Comments         : Reads data from ras driver
* 
*END*----------------------------------------------------------------------*/

char _io_dun_read_char
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr
   )
{  /* Body */
   IODUN_STRUCT_PTR  ras_ptr = fd_ptr->DEV_DATA_PTR;
   IODUN_DEV_STRUCT_PTR dev_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

   if (ras_ptr->STATE == 0) {
      ras_ptr->PARSE = dev_ptr->RECV;
      for (;;) {
         ras_ptr->F_CHAR = _io_fgetc(ras_ptr->F_PTR);
         ras_ptr->STATE++;
         if (ras_ptr->F_CHAR != *ras_ptr->PARSE++) {
            ras_ptr->PARSE = dev_ptr->RECV;
            break;
         } /* Endif */
         if (ras_ptr->STATE == dev_ptr->RECV_SIZE) {
            _io_write(ras_ptr->F_PTR, dev_ptr->SEND, dev_ptr->SEND_SIZE);
            ras_ptr->STATE = 0;
            ras_ptr->PARSE = dev_ptr->RECV;
         } /* Endif */
      } /* Endfor */
   } /* Endif */

   if (--ras_ptr->STATE) {
      return *ras_ptr->PARSE++;
   } else {
      return ras_ptr->F_CHAR;
   } /* Endif */

} /* Endbody */ 

/* EOF */
