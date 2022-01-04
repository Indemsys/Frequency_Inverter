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
* $FileName: i2c_int.c$
* $Version : 3.8.6.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the I2C interrupt I/O driver functions.
*
*END************************************************************************/


#include <mqx_inc.h>
#include <fio.h>
#include <fio_prv.h>
#include <io.h>
#include <io_prv.h>
#include "i2c.h"
#include "i2c_int_prv.h"


/*FUNCTION****************************************************************
* 
* Function Name    : _io_i2c_int_install
* Returned Value   : MQX error code
* Comments         :
*    Install the I2C device.
*
*END**********************************************************************/

_mqx_uint _io_i2c_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr              identifier,
  
      /* [IN] The I/O init function */
      _mqx_uint (_CODE_PTR_ init)(pointer, char_ptr),
      
      /* [IN] The I/O de-init function */
      _mqx_uint (_CODE_PTR_ deinit)(pointer, pointer),
      
      /* [IN] The input function */
      _mqx_int  (_CODE_PTR_ recv)(pointer, char_ptr, _mqx_int),
     
      /* [IN] The output function */
      _mqx_int  (_CODE_PTR_ xmit)(pointer, char_ptr, _mqx_int),

      /* [IN] The I/O ioctl function */
      _mqx_int  (_CODE_PTR_ ioctl)(pointer, _mqx_uint, _mqx_uint_ptr),

      /* [IN] The I/O init data pointer */
      pointer               init_data_ptr
   )
{ /* Body */
   IO_I2C_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;

   int_io_dev_ptr = (IO_I2C_INT_DEVICE_STRUCT_PTR)_mem_alloc_system_zero ((_mem_size)sizeof (IO_I2C_INT_DEVICE_STRUCT));

   if (int_io_dev_ptr == NULL) 
   {
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type (int_io_dev_ptr, MEM_TYPE_IO_I2C_INT_DEVICE_STRUCT);            

   int_io_dev_ptr->DEV_INIT          = init;
   int_io_dev_ptr->DEV_DEINIT        = deinit;
   int_io_dev_ptr->DEV_READ          = recv;
   int_io_dev_ptr->DEV_WRITE         = xmit;
   int_io_dev_ptr->DEV_IOCTL         = ioctl;
   int_io_dev_ptr->DEV_INIT_DATA_PTR = init_data_ptr;
   
   return (_io_dev_install_ext(identifier,
      _io_i2c_int_open, _io_i2c_int_close,
      _io_i2c_int_read, 
      _io_i2c_int_write, 
      _io_i2c_int_ioctl, _io_i2c_int_uninstall,
      (pointer)int_io_dev_ptr));

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_i2c_int_uninstall
* Returned Value   : MQX error code
* Comments         :
*    UnInstall interrupt I2C device.
*
*END**********************************************************************/

_mqx_int _io_i2c_int_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
   IO_I2C_INT_DEVICE_STRUCT_PTR int_io_dev_ptr = io_dev_ptr->DRIVER_INIT_PTR;

   if (int_io_dev_ptr->COUNT == 0) 
   {
      if (int_io_dev_ptr->DEV_DEINIT) 
      {
         (*int_io_dev_ptr->DEV_DEINIT)(int_io_dev_ptr, int_io_dev_ptr->DEV_INFO_PTR);
      }
      _mem_free (int_io_dev_ptr);
      io_dev_ptr->DRIVER_INIT_PTR = NULL;
      return IO_OK;
   } else {
      return IO_ERROR_DEVICE_BUSY;
   }
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_i2c_int_open
* Returned Value   : MQX error code
* Comments         :
*    This routine initializes the I2C I/O channel. It acquires
*    memory, then stores information into it about the channel.
*    It then calls the hardware interface function to initialize the channel.
* 
*END**********************************************************************/

_mqx_int _io_i2c_int_open
   (
      /* [IN] the file handle for the device being opened */
      FILE_DEVICE_STRUCT_PTR         fd_ptr,
       
      /* [IN] the remaining portion of the name of the device */
      char_ptr                       open_name_ptr,

      /* [IN] the flags to be used during operation:
      ** echo, translation, xon/xoff.
      */
      char_ptr                       flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR              io_dev_ptr;
   IO_I2C_INT_DEVICE_STRUCT_PTR      int_io_dev_ptr;
   _mqx_int                          result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)(io_dev_ptr->DRIVER_INIT_PTR);

   _int_disable ();
   if (int_io_dev_ptr->COUNT)
   {
      /* Device is already opened */
      _int_enable ();
      return MQX_IO_OPERATION_NOT_AVAILABLE;
   }
   int_io_dev_ptr->COUNT = 1;
   _int_enable ();
      
   int_io_dev_ptr->FLAGS = (_mqx_uint)flags;
   fd_ptr->FLAGS         = (_mqx_uint)flags;
            
   result = (*int_io_dev_ptr->DEV_INIT)(int_io_dev_ptr, open_name_ptr);
   if (result != MQX_OK) 
   {
      int_io_dev_ptr->COUNT = 0;
   }
   return result;
} /* Endbody */



/*FUNCTION****************************************************************
* 
* Function Name    : _io_i2c_int_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the I2C I/O channel.
* 
*END**********************************************************************/

_mqx_int _io_i2c_int_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR    fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR         io_dev_ptr;
   IO_I2C_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_int                     result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;

   _int_disable();
   if (int_io_dev_ptr->COUNT == 0)
   {
      /* Device is already closed */
      _int_enable ();
      return MQX_IO_OPERATION_NOT_AVAILABLE;
   }
   if (--int_io_dev_ptr->COUNT == 0) 
   {
      if (int_io_dev_ptr->DEV_DEINIT) 
      {
         result = (*int_io_dev_ptr->DEV_DEINIT)(int_io_dev_ptr, int_io_dev_ptr->DEV_INFO_PTR);
      }
   }
   _int_enable();
      
   return result;
} /* Endbody */


  
/*FUNCTION****************************************************************
* 
* Function Name    : _io_i2c_int_read
* Returned Value   : number of bytes read
* Comments         :
*    Reads bytes into array from I2C input buffers.
*    Returns number of bytes received.
*
*END*********************************************************************/

_mqx_int _io_i2c_int_read
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR          fd_ptr,

      /* [IN] where the characters are to be stored */
      char_ptr                        data_ptr,

      /* [IN] the number of bytes to read */
      _mqx_int                        n
   )
{  /* Body */
   IO_DEVICE_STRUCT_PTR               io_dev_ptr;
   IO_I2C_INT_DEVICE_STRUCT_PTR       int_io_dev_ptr;
   
   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;

   return (*int_io_dev_ptr->DEV_READ)(int_io_dev_ptr, data_ptr, n);
}  /* Endbody */



/*FUNCTION****************************************************************
* 
* Function Name    : _io_i2c_int_write
* Returned Value   : number of bytes written
* Comments         :
*    This routine calls the appropriate write routine.
*
*END**********************************************************************/

_mqx_int _io_i2c_int_write
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR    fd_ptr,

      /* [IN] where the bytes are stored */
      char_ptr                  data_ptr,

      /* [IN] the number of bytes to output */
      _mqx_int                  n
   )
{  /* Body */
   IO_DEVICE_STRUCT_PTR         io_dev_ptr;
   IO_I2C_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   
   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;
               
   return (*int_io_dev_ptr->DEV_WRITE)(int_io_dev_ptr, data_ptr, n);
} /* Endbody */
   
   
   
/*FUNCTION*****************************************************************
* 
* Function Name    : _io_i2c_int_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of I2C ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_i2c_int_ioctl
   (
      /* [IN] the file handle for the device */
      FILE_DEVICE_STRUCT_PTR    fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint                 cmd,

      /* [IN] the ioctl parameters */
      pointer                   input_param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR         io_dev_ptr;
   IO_I2C_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_int                     result = MQX_OK;
   _mqx_uint_ptr                param_ptr = (_mqx_uint_ptr)input_param_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;

    switch (cmd) {
        case IO_IOCTL_DEVICE_IDENTIFY :
            /* return the device identify */
            param_ptr[0] = IO_DEV_TYPE_PHYS_I2C_INTERRUPT;
            param_ptr[1] = 0;
            param_ptr[2] = IO_DEV_ATTR_INTERRUPT | IO_DEV_ATTR_READ | IO_DEV_ATTR_WRITE;
            result = MQX_OK;    
            break;
        default:
            if (int_io_dev_ptr->DEV_IOCTL != NULL) {
                result = (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR, cmd, param_ptr);
            }
    }
   
   return result;
} /* Endbody */

/* EOF */
