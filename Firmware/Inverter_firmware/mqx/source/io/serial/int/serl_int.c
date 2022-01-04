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
* $FileName: serl_int.c$
* $Version : 3.8.19.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the driver functions for the interrupt driven
*   serial asynchronous character I/O.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "serial.h"
#include "charq.h"
#include "bsp.h"
#include "serinprv.h"


#if MQX_ENABLE_LOW_POWER

extern LPM_NOTIFICATION_RESULT _io_serial_int_clock_configuration_callback (LPM_NOTIFICATION_STRUCT_PTR, pointer);
extern LPM_NOTIFICATION_RESULT _io_serial_int_operation_mode_callback (LPM_NOTIFICATION_STRUCT_PTR, pointer);

#endif


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_serial_int_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install an interrupt driven serial device.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_serial_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr             identifier,
  
      /* [IN] The I/O init function */
      _mqx_uint (_CODE_PTR_ init)(pointer, char _PTR_),

      /* [IN] The enable interrupts function */
      _mqx_uint (_CODE_PTR_ enable_ints)(pointer),

      /* [IN] The I/O de-init function */
      _mqx_uint (_CODE_PTR_ deinit)(pointer, pointer),

      /* [IN] The output function */
      void    (_CODE_PTR_  putc)(pointer, char),

      /* [IN] The I/O ioctl function */
      _mqx_uint (_CODE_PTR_ ioctl)(pointer, _mqx_uint, pointer),

      /* [IN] The I/O init data pointer */
      pointer              init_data_ptr,
      
      /* [IN] The I/O queue size to use */
      _mqx_uint             queue_size
   )
{ /* Body */
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   uint_32                         result;

   int_io_dev_ptr = _mem_alloc_system_zero(
      (_mem_size)sizeof(IO_SERIAL_INT_DEVICE_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (int_io_dev_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   } /* Endif */
#endif
   _mem_set_type(int_io_dev_ptr,MEM_TYPE_IO_SERIAL_INT_DEVICE_STRUCT);    

   int_io_dev_ptr->DEV_INIT          = init;
   int_io_dev_ptr->DEV_ENABLE_INTS   = enable_ints;
   int_io_dev_ptr->DEV_DEINIT        = deinit;
   int_io_dev_ptr->DEV_PUTC          = putc;
   int_io_dev_ptr->DEV_IOCTL         = ioctl;
   int_io_dev_ptr->DEV_INIT_DATA_PTR = init_data_ptr;
   int_io_dev_ptr->QUEUE_SIZE        = queue_size;
   
   result = _io_dev_install(identifier,
      _io_serial_int_open, _io_serial_int_close,
      _io_serial_int_read, _io_serial_int_write,
      _io_serial_int_ioctl,
      (pointer)int_io_dev_ptr); 
   
#if MQX_ENABLE_LOW_POWER
   if (MQX_OK == result)
   {
      LPM_REGISTRATION_STRUCT registration;
      registration.CLOCK_CONFIGURATION_CALLBACK = _io_serial_int_clock_configuration_callback;
      registration.OPERATION_MODE_CALLBACK = _io_serial_int_operation_mode_callback;
      registration.DEPENDENCY_LEVEL = BSP_LPM_DEPENDENCY_LEVEL_SERIAL_INT;          
      result = _lpm_register_driver (&registration, int_io_dev_ptr, &(int_io_dev_ptr->LPM_INFO.REGISTRATION_HANDLE));
      if (MQX_OK == result)
      {
         _lwsem_create (&(int_io_dev_ptr->LPM_INFO.LOCK), 1);
         int_io_dev_ptr->LPM_INFO.FLAGS = 0;
      }
   }
#endif
   
   return result;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_serial_int_uninstall
* Returned Value   : _mqx_int a task error code or MQX_OK
* Comments         :
*    Un-Install a interrupt driven async serial device.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_serial_int_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_dev_ptr = io_dev_ptr->DRIVER_INIT_PTR;

   if (int_dev_ptr->COUNT == 0) {
      if (int_dev_ptr->DEV_DEINIT) {

#if MQX_ENABLE_LOW_POWER          
         _lwsem_wait (&(int_dev_ptr->LPM_INFO.LOCK));
#endif
         (*int_dev_ptr->DEV_DEINIT)(int_dev_ptr->DEV_INIT_DATA_PTR,
            int_dev_ptr->DEV_INFO_PTR);
#if MQX_ENABLE_LOW_POWER          
         _lwsem_post (&(int_dev_ptr->LPM_INFO.LOCK));
#endif
          
      } /* Endif */
      _mem_free(int_dev_ptr->IN_QUEUE);
      _mem_free(int_dev_ptr->OUT_QUEUE);
      _taskq_destroy(int_dev_ptr->IN_WAITING_TASKS);
      _taskq_destroy(int_dev_ptr->OUT_WAITING_TASKS);
      
#if MQX_ENABLE_LOW_POWER
      _lpm_unregister_driver (int_dev_ptr->LPM_INFO.REGISTRATION_HANDLE);
      _lwsem_destroy (&(int_dev_ptr->LPM_INFO.LOCK));
#endif
      
      _mem_free(int_dev_ptr);
      return(IO_OK);
   } else {
      return(IO_ERROR_DEVICE_BUSY);
   } /* Endif */

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_open
* Returned Value   : _mqx_int task error code
* Comments         :
*    This routine initializes an interrupt I/O channel. It acquires
*    memory, then stores information into it about the channel.
*    This memory is returned as a 'handle' to be used for all other 
*    interrupt I/O functions.
*
* 
*END**********************************************************************/

_mqx_int _io_serial_int_open
   (
      /* [IN] the file handle for the device being opened */
      FILE_DEVICE_STRUCT_PTR fd_ptr,
       
      /* [IN] the remaining portion of the name of the device */
      char             _PTR_ open_name_ptr,

      /* [IN] the flags to be used during operation:
      ** echo, translation, xon/xoff
      */
      char             _PTR_ flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_uint                       result = MQX_OK;
   _mqx_uint                       ioctl_val;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;
   
   if (int_io_dev_ptr->COUNT) {
      /* Device is already opened */
      int_io_dev_ptr->COUNT++;
      fd_ptr->FLAGS = int_io_dev_ptr->FLAGS;
      return(result);
   } /* Endif */

   int_io_dev_ptr->IN_WAITING_TASKS  = _taskq_create(MQX_TASK_QUEUE_FIFO);
   int_io_dev_ptr->OUT_WAITING_TASKS = _taskq_create(MQX_TASK_QUEUE_FIFO);

   int_io_dev_ptr->IN_QUEUE = (pointer)_mem_alloc_system(
      sizeof(CHARQ_STRUCT) - (4 * sizeof(char)) + int_io_dev_ptr->QUEUE_SIZE);
   int_io_dev_ptr->OUT_QUEUE = (pointer)_mem_alloc_system(
      sizeof(CHARQ_STRUCT) - (4 * sizeof(char)) + int_io_dev_ptr->QUEUE_SIZE);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if ((int_io_dev_ptr->IN_QUEUE == NULL) || 
       (int_io_dev_ptr->OUT_QUEUE == NULL) ||
       (int_io_dev_ptr->IN_WAITING_TASKS == NULL) ||
       (int_io_dev_ptr->OUT_WAITING_TASKS == NULL))
   {
      if (int_io_dev_ptr->IN_QUEUE != NULL){
         _mem_free(int_io_dev_ptr->IN_QUEUE);
      } /* Endif */
      if (int_io_dev_ptr->OUT_QUEUE != NULL){
         _mem_free(int_io_dev_ptr->OUT_QUEUE);
      } /* Endif */
      if (int_io_dev_ptr->IN_WAITING_TASKS != NULL) {
         _taskq_destroy(int_io_dev_ptr->IN_WAITING_TASKS);
      }/* Endif */
      if (int_io_dev_ptr->OUT_WAITING_TASKS != NULL) {
         _taskq_destroy(int_io_dev_ptr->OUT_WAITING_TASKS);
      }/* Endif */
      return(MQX_OUT_OF_MEMORY);
   }/* Endif */
#endif            
   _mem_set_type(int_io_dev_ptr->IN_QUEUE,MEM_TYPE_IO_SERIAL_IN_QUEUE);       
   _mem_set_type(int_io_dev_ptr->OUT_QUEUE,MEM_TYPE_IO_SERIAL_OUT_QUEUE);      

   _CHARQ_INIT(int_io_dev_ptr->IN_QUEUE, int_io_dev_ptr->QUEUE_SIZE);
   _CHARQ_INIT(int_io_dev_ptr->OUT_QUEUE, int_io_dev_ptr->QUEUE_SIZE);
   int_io_dev_ptr->INPUT_HIGH_WATER_MARK = int_io_dev_ptr->QUEUE_SIZE -
      int_io_dev_ptr->QUEUE_SIZE/8;
   int_io_dev_ptr->INPUT_LOW_WATER_MARK  = int_io_dev_ptr->QUEUE_SIZE/2;
   int_io_dev_ptr->FLAGS = (_mqx_uint)flags;
   fd_ptr->FLAGS      = (_mqx_uint)flags;

#if MQX_ENABLE_LOW_POWER          
   _lwsem_wait (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif
   result = (*int_io_dev_ptr->DEV_INIT)(int_io_dev_ptr, open_name_ptr);
#if MQX_ENABLE_LOW_POWER          
   _lwsem_post (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif
   
   if (result == MQX_OK) {
       
#if MQX_ENABLE_LOW_POWER          
      _lwsem_wait (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif
      result = (*int_io_dev_ptr->DEV_ENABLE_INTS)(int_io_dev_ptr->DEV_INFO_PTR);
#if MQX_ENABLE_LOW_POWER          
      _lwsem_post (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif
      
      if (int_io_dev_ptr->DEV_IOCTL != NULL) {
         if ((_mqx_uint)flags & IO_SERIAL_HW_FLOW_CONTROL) {
            ioctl_val = IO_SERIAL_RTS;
            (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR, IO_IOCTL_SERIAL_SET_HW_SIGNAL, &ioctl_val);
         } /* Endif */
         ioctl_val = (_mqx_uint)flags;
         (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR, IO_IOCTL_SERIAL_SET_FLAGS, &ioctl_val);
      }
      if (result == MQX_OK && ((_mqx_uint)flags & IO_SERIAL_NON_BLOCKING) 
         && (_mqx_uint)flags & (IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO | IO_SERIAL_XON_XOFF)) {
         result = MQX_INVALID_PARAMETER;
      } /* Endif */
   } /* Endif */
   
   if (result != MQX_OK) {
      _mem_free(int_io_dev_ptr->IN_QUEUE);
      _mem_free(int_io_dev_ptr->OUT_QUEUE);
      _taskq_destroy(int_io_dev_ptr->IN_WAITING_TASKS);
      _taskq_destroy(int_io_dev_ptr->OUT_WAITING_TASKS);
   }
   int_io_dev_ptr->COUNT = 1;
   return(result);

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_close
* Returned Value   : _mqx_int error code
* Comments         :
*    This routine closes the serial I/O channel.
* 
*END**********************************************************************/

_mqx_int _io_serial_int_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_int                        result = MQX_OK;
   _mqx_int                        ioctl_val;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;

   if (--int_io_dev_ptr->COUNT == 0) {

      if (int_io_dev_ptr->DEV_IOCTL != NULL) {
         if (fd_ptr->FLAGS & IO_SERIAL_HW_FLOW_CONTROL) {
            ioctl_val = IO_SERIAL_RTS;
            (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR, IO_IOCTL_SERIAL_CLEAR_HW_SIGNAL, &ioctl_val);
         }
      }
      if (int_io_dev_ptr->DEV_DEINIT) {
          
#if MQX_ENABLE_LOW_POWER          
         _lwsem_wait (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif
         result = (*int_io_dev_ptr->DEV_DEINIT)(int_io_dev_ptr->DEV_INIT_DATA_PTR,
            int_io_dev_ptr->DEV_INFO_PTR);
#if MQX_ENABLE_LOW_POWER          
         _lwsem_post (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif
          
      } /* Endif */
      _mem_free(int_io_dev_ptr->IN_QUEUE);
      _mem_free(int_io_dev_ptr->OUT_QUEUE);
      _taskq_destroy(int_io_dev_ptr->IN_WAITING_TASKS);
      _taskq_destroy(int_io_dev_ptr->OUT_WAITING_TASKS);
   } /* Endif */
   return(result);

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_read
* Returned Value   : _mqx_int number of characters read
* Comments         :
*    This routine reads characters from the input ring buffer,
*    converting carriage return ('\r') characters to newlines,
*    and then echoing the input characters.
*
*END*********************************************************************/

_mqx_int _io_serial_int_read
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] where the characters are to be stored */
      char             _PTR_ data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int               num
      
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_int                        ioctl_val;
   uchar                           c = 0;
   _mqx_uint                       flags;
   _mqx_int                        i = num;
   volatile CHARQ_STRUCT _PTR_     in_queue;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;
   flags          = fd_ptr->FLAGS;

   while ( i ) {

      in_queue = int_io_dev_ptr->IN_QUEUE;
      _int_disable();
      if(flags & IO_SERIAL_NON_BLOCKING) {
          if (_CHARQ_SIZE(in_queue) == 0) {
              num -= i;
              _int_enable();
              break;
          } /* Endif */
      } else {
          while (_CHARQ_SIZE(in_queue) == 0) {
             _taskq_suspend(int_io_dev_ptr->IN_WAITING_TASKS);
          } /* Endwhile */  
      } /* Endif */
      _CHARQ_DEQUEUE(in_queue,c);

      if (int_io_dev_ptr->HAVE_STOPPED_INPUT) {
         if (_CHARQ_SIZE(in_queue) < int_io_dev_ptr->INPUT_LOW_WATER_MARK) {
            if (flags & IO_SERIAL_HW_FLOW_CONTROL) {
               if (int_io_dev_ptr->DEV_IOCTL != NULL) {
                  ioctl_val = IO_SERIAL_RTS;
                  (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR, IO_IOCTL_SERIAL_SET_HW_SIGNAL, &ioctl_val);
               }
               int_io_dev_ptr->HAVE_STOPPED_INPUT = FALSE;
            } else {
               if (int_io_dev_ptr->OUTPUT_ENABLED && !int_io_dev_ptr->HAVE_STOPPED_OUTPUT) {
                  int_io_dev_ptr->MUST_START_INPUT = TRUE;
               } else {
                  int_io_dev_ptr->HAVE_STOPPED_INPUT = FALSE;
                  int_io_dev_ptr->OUTPUT_ENABLED = TRUE;
                  (*int_io_dev_ptr->DEV_PUTC)(int_io_dev_ptr, CNTL_Q);
               } /* Endif */
            } /* Endif */
         } /* Endif */
      } /* Endif */
      _int_enable();

      if (flags & IO_SERIAL_TRANSLATION) {
         if (c == '\r') {
            /* Start CR 387 */
            if (flags & IO_SERIAL_ECHO) {
               _io_serial_int_putc_internal(int_io_dev_ptr, (char)c, 0);
            } /* Endif */
            /* End CR 387 */
            c = '\n';
         } else if ((c == '\b') && (flags & IO_SERIAL_ECHO)) {
            _io_serial_int_putc_internal(int_io_dev_ptr, (char)'\b', 0);
            _io_serial_int_putc_internal(int_io_dev_ptr, (char)' ', 0);
         } /* Endif */
      } /* Endif */

      if (flags & IO_SERIAL_ECHO) {
         _io_serial_int_putc_internal(int_io_dev_ptr, (char)c, 0);
      } /* Endif */

      *data_ptr++ = c;
      --i;
      
   } /* Endwhile */

   return num;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_write
* Returned Value   : _mqx_int
* Comments         :
*    This routine writes the character to the device.
*    It also converts the C '\n' into '\n\r', if required.
*
*END**********************************************************************/

_mqx_int _io_serial_int_write
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] where the characters to print out are */
      char             _PTR_ data_ptr,

      /* [IN] the number of characters to output */
      _mqx_int               num
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_uint                       flags;
   _mqx_int                        i = num;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;
   flags          = fd_ptr->FLAGS;

#if MQX_ENABLE_LOW_POWER          
   _lwsem_wait (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif

   while ( i != 0 ) {
      if (flags & IO_SERIAL_TRANSLATION) {
         if (*data_ptr == '\n') {
            (void)_io_serial_int_putc_internal(int_io_dev_ptr, '\r', 0);
         } /* Endif */
      } /* Endif */
      if (_io_serial_int_putc_internal(int_io_dev_ptr, *data_ptr, flags)){
          data_ptr++;
          i--;
      } else {
         num -= i;
         break;
      } /* Endif */
      
   } /* Endwhile */
   
#if MQX_ENABLE_LOW_POWER          
   _lwsem_post (&(int_io_dev_ptr->LPM_INFO.LOCK));
#endif
   
   return num;
   
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_serial_int_ioctl
* Returned Value   : _mqx_int
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_serial_int_ioctl
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      pointer                param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr;
   _mqx_uint                       result = MQX_OK;
   _mqx_uint_ptr                   uparam_ptr = (_mqx_uint_ptr)param_ptr;

   io_dev_ptr     = fd_ptr->DEV_PTR;
   int_io_dev_ptr = (pointer)io_dev_ptr->DRIVER_INIT_PTR;

   switch (cmd) {
      case IO_IOCTL_CHAR_AVAIL:
         if ( _CHARQ_SIZE(int_io_dev_ptr->IN_QUEUE) ) {
           *((boolean _PTR_)param_ptr) = TRUE;
         } else {
           *((boolean _PTR_)param_ptr) = FALSE;
         } /* Endif */
      break;

      case IO_IOCTL_SERIAL_GET_FLAGS:
         *((_mqx_uint_ptr)param_ptr) = int_io_dev_ptr->FLAGS;
         break;

      case IO_IOCTL_SERIAL_SET_FLAGS:
         int_io_dev_ptr->FLAGS = *((_mqx_uint_ptr)param_ptr);
         fd_ptr->FLAGS = *((_mqx_uint_ptr)param_ptr);
         if (int_io_dev_ptr->DEV_IOCTL != NULL) {
            result = (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR,
               cmd, param_ptr);
         } /* Endif */
         break;

      case IO_IOCTL_SERIAL_TRANSMIT_DONE:
         *((boolean _PTR_)param_ptr) = !(int_io_dev_ptr->OUTPUT_ENABLED);
         break;
      
      case IO_IOCTL_FLUSH_OUTPUT:
         /* Disable interrupts to avoid situation that last TX interrupt comes after successfull !_CHARQ_EMPTY() */
         _int_disable();
         while(!_CHARQ_EMPTY(int_io_dev_ptr->OUT_QUEUE)) {
            /* wait untill all chars are sent from output queue */
            _taskq_suspend(int_io_dev_ptr->OUT_WAITING_TASKS); 
         };
         _int_enable();
         break;
      
      case IO_IOCTL_DEVICE_IDENTIFY:
         /* return the device identify */
         uparam_ptr[0] = IO_DEV_TYPE_PHYS_SERIAL_INTERRUPT;
         uparam_ptr[1] = 0;
         uparam_ptr[2] = IO_DEV_ATTR_INTERRUPT | IO_DEV_ATTR_READ | IO_DEV_ATTR_WRITE;
         result = MQX_OK;   
         break;

      case IO_IOCTL_SERIAL_CAN_TRANSMIT:
         *uparam_ptr = (int_io_dev_ptr->OUT_QUEUE->MAX_SIZE - int_io_dev_ptr->OUT_QUEUE->CURRENT_SIZE);
         result = MQX_OK;   
         break;

      case IO_IOCTL_SERIAL_CAN_RECEIVE:
         *uparam_ptr = (int_io_dev_ptr->IN_QUEUE->MAX_SIZE - int_io_dev_ptr->IN_QUEUE->CURRENT_SIZE);
         result = MQX_OK;   
         break;
            
      default:
         if (int_io_dev_ptr->DEV_IOCTL != NULL) {
            result = (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR,
               cmd, param_ptr);
         } /* Endif */
      break;
   } /* Endswitch */
   return result;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_putc_internal
* Returned Value   : void
* Comments         : 
*   This function writes out the character to the device if the queue
* is empty, or it writes it to the device.  If the queue is full, this
* function will suspend the writing task.
*
*END*********************************************************************/

boolean _io_serial_int_putc_internal
   (
      /* [IN] the interrupt io device information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the character to print out */
      char                     c, 
      _mqx_uint                flags
   )
{ /* Body */
   volatile CHARQ_STRUCT _PTR_ out_queue;

   /* Start CR 388 */
#if (PSP_MEMORY_ADDRESSING_CAPABILITY > 8 )
   c &= 0xFF;
#endif
   /* End CR 388 */

   out_queue = int_io_dev_ptr->OUT_QUEUE;
   _int_disable();
   if(flags & IO_SERIAL_NON_BLOCKING) {
      if (_CHARQ_FULL(out_queue)) {
          _int_enable();
          return FALSE;
      } /* Endif */
   } else {
      while (_CHARQ_FULL(out_queue)) {
         /* Lets wait */
         _taskq_suspend(int_io_dev_ptr->OUT_WAITING_TASKS);
      } /* Endif */
   } /* Endif */


   if (int_io_dev_ptr->OUTPUT_ENABLED || int_io_dev_ptr->HAVE_STOPPED_OUTPUT) {
      _CHARQ_ENQUEUE(out_queue,c);
   } else {
      int_io_dev_ptr->OUTPUT_ENABLED = TRUE;
      (*int_io_dev_ptr->DEV_PUTC)(int_io_dev_ptr, c);
   } /* Endif */
   _int_enable();
   return TRUE;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_addc
* Returned Value   : none
* Comments         :
*    This function is called by the device io interrupt handler, to add
* a character to the input queue.
*
*END*********************************************************************/

boolean _io_serial_int_addc
   (
      /* [IN] the interrupt I/O context information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the character to add to the input queue */
      char                            c
   )
{ /* Body */
   CHARQ_STRUCT_PTR in_queue;
   _mqx_uint       ioctl_val;

   if (int_io_dev_ptr->FLAGS & IO_SERIAL_XON_XOFF) {
      if (int_io_dev_ptr->HAVE_STOPPED_OUTPUT) {
         if (c == CNTL_Q) {
            int_io_dev_ptr->HAVE_STOPPED_OUTPUT = FALSE;
            return TRUE;
         } /* Endif */
      } else {
         if (c == CNTL_S) {
            int_io_dev_ptr->HAVE_STOPPED_OUTPUT = TRUE;
            return TRUE;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   in_queue = int_io_dev_ptr->IN_QUEUE;
   if (_CHARQ_NOT_FULL(in_queue)) {
      _CHARQ_ENQUEUE(in_queue,c);

      if (int_io_dev_ptr->FLAGS & (IO_SERIAL_XON_XOFF |
         IO_SERIAL_HW_FLOW_CONTROL))
      {
         if (_CHARQ_SIZE(in_queue) > int_io_dev_ptr->INPUT_HIGH_WATER_MARK) {
            if (!int_io_dev_ptr->HAVE_STOPPED_INPUT) {
               if (int_io_dev_ptr->FLAGS & IO_SERIAL_XON_XOFF) {
                  int_io_dev_ptr->MUST_STOP_INPUT = TRUE;
               } else {
                  if (int_io_dev_ptr->DEV_IOCTL != NULL) {
                     ioctl_val = IO_SERIAL_RTS;
                     (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR, IO_IOCTL_SERIAL_CLEAR_HW_SIGNAL, &ioctl_val);
                  }
                  int_io_dev_ptr->HAVE_STOPPED_INPUT = TRUE;
               } /* Endif */
            } /* Endif */
         } else if (_CHARQ_SIZE(in_queue) < int_io_dev_ptr->INPUT_LOW_WATER_MARK) {
            if (int_io_dev_ptr->HAVE_STOPPED_INPUT) {
               if (int_io_dev_ptr->FLAGS & IO_SERIAL_XON_XOFF) {
                  int_io_dev_ptr->MUST_START_INPUT = TRUE;
               } else {
                  if (int_io_dev_ptr->DEV_IOCTL != NULL) {
                     ioctl_val = IO_SERIAL_RTS;
                     (*int_io_dev_ptr->DEV_IOCTL)(int_io_dev_ptr->DEV_INFO_PTR, IO_IOCTL_SERIAL_SET_HW_SIGNAL, &ioctl_val);
                  }
                  int_io_dev_ptr->HAVE_STOPPED_INPUT = FALSE;
               } /* Endif */
            } /* Endif */
         } /* Endif */
      } /* Endif */
   } else {
      /* indicate that tossed the character */
      return FALSE;
   } /* Endif */

   if (_QUEUE_GET_SIZE(int_io_dev_ptr->IN_WAITING_TASKS)) {
      _taskq_resume(int_io_dev_ptr->IN_WAITING_TASKS, TRUE);
   } /* Endif */
   return TRUE;
   
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_serial_int_nextc
* Returned Value   : _mqx_int, the next character to write out.
* Comments         :
*    This function returns the next character to send out, or -1 if
* no more output characters are available
*
*END*********************************************************************/

_mqx_int _io_serial_int_nextc
   (
      /* [IN] the interrupt I/O context information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr
   )
{ /* Body */
   uchar c;

   if (int_io_dev_ptr->FLAGS & IO_SERIAL_XON_XOFF) {
      if (int_io_dev_ptr->MUST_STOP_INPUT) {
         int_io_dev_ptr->MUST_STOP_INPUT    = FALSE;
         int_io_dev_ptr->HAVE_STOPPED_INPUT = TRUE;
        return((int_32)CNTL_S);
      } else if (int_io_dev_ptr->MUST_START_INPUT) {
         int_io_dev_ptr->MUST_START_INPUT   = FALSE;
         int_io_dev_ptr->HAVE_STOPPED_INPUT = FALSE;
         return((int_32)CNTL_Q);
      } /* Endif */
   } /* Endif */

   if (int_io_dev_ptr->HAVE_STOPPED_OUTPUT
      || (! int_io_dev_ptr->OUTPUT_ENABLED))
   {
      return(-1);
   } /* Endif */

   if (_CHARQ_EMPTY(int_io_dev_ptr->OUT_QUEUE)) {
      /* No output */
      int_io_dev_ptr->OUTPUT_ENABLED = FALSE;
      if (_QUEUE_GET_SIZE(int_io_dev_ptr->OUT_WAITING_TASKS)) {
         _taskq_resume(int_io_dev_ptr->OUT_WAITING_TASKS, TRUE);
      } /* Endif */
      return(-1);
   }/* Endif */

   _CHARQ_DEQUEUE(int_io_dev_ptr->OUT_QUEUE, c);
   return((_mqx_int)c);

} /* Endbody */

/* EOF */
