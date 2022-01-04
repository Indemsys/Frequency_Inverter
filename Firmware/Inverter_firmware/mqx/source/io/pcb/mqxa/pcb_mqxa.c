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
* $FileName: pcb_mqxa.c$
* $Version : 3.8.8.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the code which implements a PCB driver
*   that sends MQX format packets over an asynchronous serial driver.
*
*END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_IPC

#include "fio.h"
#include "io.h"
#include "io_prv.h"
#include "serial.h"
#include "io_pcb.h"
#include "iopcbprv.h"
#include "pcb_mqxa.h"
#include "pcbmqxav.h"


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_mqxa_install
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*     This function is use to install the I/O device.
*
*END*---------------------------------------------------------------------*/

_mqx_uint _io_pcb_mqxa_install
   (
      /* [IN] the name of this device */
      char_ptr    device_name_ptr,
      
      /* [IN] the initialization record for this device */
      pointer     init_ptr
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;

   info_ptr = _mem_alloc_system_zero(sizeof(IO_PCB_MQXA_INFO_STRUCT));
   if (info_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   }/* Endif */
   info_ptr->INIT = *((IO_PCB_MQXA_INIT_STRUCT_PTR)init_ptr);

   return (_io_pcb_dev_install(device_name_ptr,
      _io_pcb_mqxa_open,
      _io_pcb_mqxa_close,
      _io_pcb_mqxa_read,
      _io_pcb_mqxa_write, 
      _io_pcb_mqxa_ioctl,
      _io_pcb_mqxa_uninstall,
      (pointer)info_ptr)); 
   
} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_mqxa_open
* Returned Value  : _mqx_int result MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_mqxa_open
   (
      /* [IN] the file handle */
      FILE_DEVICE_STRUCT_PTR fd_ptr, 

      /* [IN] the rest of the filename used to open the device */
      char _PTR_             open_name_ptr,

      /* [IN] the open flags for this device */
      char _PTR_             open_mode_flags
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;
   IO_PCB_DEVICE_STRUCT_PTR    dev_ptr;
   _mqx_uint                   flags;

   dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   fd_ptr->DEV_DATA_PTR = dev_ptr->DRIVER_INIT_PTR;
   info_ptr = (IO_PCB_MQXA_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;
   if (info_ptr->FD) {
      fd_ptr->ERROR = IO_PCB_MQXA_DEVICE_ALREADY_OPEN;
      return(IO_ERROR);
   }/* Endif */
   info_ptr->FD = fopen(info_ptr->INIT.IO_PORT_NAME, (char_ptr)0);
   if (info_ptr->FD == NULL) {
      fd_ptr->ERROR = IO_PCB_MQXA_INCORRECT_SERIAL_DEVICE;
      return(IO_ERROR);
   } /* Endif */

   /* Set the baud rate on the serial device, if a non zero value for baud is supplied. Otherwise, used default rate */
   if (info_ptr->INIT.BAUD_RATE) {
       if (ioctl(info_ptr->FD, IO_IOCTL_SERIAL_SET_BAUD,
          (pointer)&info_ptr->INIT.BAUD_RATE) != MQX_OK)
       {
          fclose(info_ptr->FD);
          info_ptr->FD = NULL;
          fd_ptr->ERROR = IO_PCB_MQXA_INCORRECT_SERIAL_DEVICE;
          return(IO_ERROR);
       }
   }

   /* Turn off xon/xoff, translation and echo on the serial device */
   ioctl(info_ptr->FD, IO_IOCTL_SERIAL_GET_FLAGS, (pointer)&flags);
   flags &= ~(IO_SERIAL_XON_XOFF | IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO);
   ioctl(info_ptr->FD, IO_IOCTL_SERIAL_SET_FLAGS, &flags);

   fd_ptr->DEV_DATA_PTR = info_ptr;
   fd_ptr->FLAGS       |= IO_FLAG_IS_PCB_DEVICE;
   
   info_ptr->READ_CALLBACK_FUNCTION = NULL;
   _lwsem_create(&info_ptr->READ_LWSEM, 0);
   _lwsem_create(&info_ptr->WRITE_LWSEM, 0);
   _queue_init(&info_ptr->READ_QUEUE, 0);
   _queue_init(&info_ptr->WRITE_QUEUE, 0);

   return(MQX_OK);
   
} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_mqxa_close
* Returned Value  : _mqx_int result   MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_mqxa_close
   (
      /* [IN] the file handle */
      FILE_DEVICE_STRUCT_PTR fd_ptr
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;
   IO_PCB_STRUCT_PTR           pcb_ptr;

#if MQX_CHECK_ERRORS
   if (!(fd_ptr->FLAGS & IO_FLAG_IS_PCB_DEVICE)) {
      fd_ptr->ERROR = IO_PCB_NOT_A_PCB_DEVICE;
      return(IO_ERROR);
   }/* Endif */
#endif
   info_ptr = (IO_PCB_MQXA_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;
   if (info_ptr->FD) {
      fclose(info_ptr->FD);
      info_ptr->FD = NULL;
   }/* Endif */
   if (info_ptr->INPUT_TASK) {
#if MQX_TASK_DESTRUCTION   
      _task_destroy(info_ptr->INPUT_TASK);
#endif      
      info_ptr->INPUT_TASK = 0;
   }/* Endif */
   if (info_ptr->OUTPUT_TASK ) {
#if MQX_TASK_DESTRUCTION   
      _task_destroy(info_ptr->OUTPUT_TASK);
#endif
      info_ptr->OUTPUT_TASK = 0;
   }/* Endif */
   _lwsem_destroy(&info_ptr->READ_LWSEM);
   _lwsem_destroy(&info_ptr->WRITE_LWSEM);

   while (_queue_get_size(&info_ptr->WRITE_QUEUE)) {
      pcb_ptr = (IO_PCB_STRUCT_PTR)
         ((pointer)_queue_dequeue(&info_ptr->WRITE_QUEUE));
      _io_pcb_free(pcb_ptr);
   } /* Endwhile */

   while (_queue_get_size(&info_ptr->READ_QUEUE)) {
      pcb_ptr = (IO_PCB_STRUCT_PTR)
         ((pointer)_queue_dequeue(&info_ptr->READ_QUEUE));
      _io_pcb_free(pcb_ptr);
   } /* Endwhile */

   return(MQX_OK);
   
} /* Endbody */


/*READ*---------------------------------------------------------------------
*
* Function Name   : _io_pcb_mqxa_read
* Returned Value  : _mqx_int result MQX_OK or an error code.
* Comments        :
*     This function reads a packet directly 
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_mqxa_read
   (
      /* [IN] the file descriptor */
      FILE_DEVICE_STRUCT_PTR  fd_ptr,
      
      /* [IN] the pcb address from which to write data */
      IO_PCB_STRUCT_PTR _PTR_ pcb_ptr
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;

   info_ptr = fd_ptr->DEV_DATA_PTR;
   if (info_ptr->FD) {
      _int_disable();
      /* 
      ** Start CR 383
      ** if (info_ptr->FD->FLAGS & IO_O_NONBLOCK) {
      */
      if (fd_ptr->FLAGS & IO_O_NONBLOCK) {
      /* End CR 383 */
         if (! _queue_get_size(&info_ptr->READ_QUEUE)) {
            *pcb_ptr = NULL;
            _int_enable();
            /* 
            ** Start CR 384
            ** return(0);
            */
            return MQX_OK;
            /* End CR 384 */
         } /* Endif */
      } /* Endif */
      _lwsem_wait(&info_ptr->READ_LWSEM);
      *pcb_ptr = (IO_PCB_STRUCT_PTR)
         ((pointer)_queue_dequeue(&info_ptr->READ_QUEUE));
      _int_enable();
      /* 
      ** Start CR 384
      ** return(0);
      */
      return MQX_OK;
      /* End CR 384 */
   }/* Endif */
   return(IO_ERROR);

} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_mqxa_write
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*     This function sends the pcb to the pcb output task.
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_mqxa_write
   (
      /* [IN] the file descriptor */
      FILE_DEVICE_STRUCT_PTR  fd_ptr,
      
      /* [IN] the pcb address from which to write data */
      IO_PCB_STRUCT_PTR       pcb_ptr
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;

   info_ptr = fd_ptr->DEV_DATA_PTR;
   if (info_ptr->FD) {
      _queue_enqueue((QUEUE_STRUCT_PTR)((pointer)&info_ptr->WRITE_QUEUE), 
         (QUEUE_ELEMENT_STRUCT_PTR)((pointer)&pcb_ptr->QUEUE));
      _lwsem_post(&info_ptr->WRITE_LWSEM);
      /* 
      ** Start CR 384
      ** return(0);
      */
      return MQX_OK;
      /* End CR 384 */
   }/* Endif */
   return(IO_ERROR);
   
} /* Endbody */


/*TASK*---------------------------------------------------------------------
*
* Task Name       : _io_pcb_mqxa_read_task
* Comments        :
*     This task reads a packet from the async serial device.
*  The packet is in MQX Asynchronous Packet format.
*
*END*---------------------------------------------------------------------*/

void _io_pcb_mqxa_read_task
   (
      /* [IN] the device info */
      uint_32 parameter
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;
   IO_PCB_STRUCT_PTR           pcb_ptr;
   uchar_ptr                   input_ptr;
   uchar_ptr                   input_init_ptr;
   boolean                     got_length = 0;
   _mem_size                   input_length = 0;
   _mem_size                   max_length = 0;
   _mem_size                   count = 0;
   _mqx_uint                   state = 0;
   _mqx_uint                   next_state = 0;
   uchar                       crc0 = 0;
   uchar                       crc1 = 0;
   uchar                       packet_crc0 = 0;
   uchar                       packet_crc1 = 0;
   uchar                       tmp;
   uchar                       c;

   info_ptr = (IO_PCB_MQXA_INFO_STRUCT_PTR)parameter;

   /* Get a PCB */
   pcb_ptr = _io_pcb_alloc(info_ptr->READ_PCB_POOL, FALSE);
#if MQX_CHECK_ERRORS
   if (pcb_ptr == NULL) {
      _task_block();
   } /* Endif */
#endif
   max_length     = info_ptr->INIT.INPUT_MAX_LENGTH;
   input_init_ptr = pcb_ptr->FRAGMENTS[0].FRAGMENT;

   state      = AP_STATE_SYNC; /* Waiting for sync */
   next_state = AP_STATE_SYNC; /* Waiting for sync */

   while (TRUE) {

      if (info_ptr->INIT.IS_POLLED) {
         while (!fstatus(info_ptr->FD)) {
            _time_delay_ticks(1);
         } /* Endwhile */
      } /* Endif */
      c = (uchar)fgetc(info_ptr->FD);

      switch (state) {

         case AP_STATE_SYNC:
            if (c == AP_SYNC) {
               /* Sync detected. Start packet reception. */
               state      = AP_STATE_READING;
               next_state = AP_STATE_SYNC;
               count      = 0;
               input_ptr  = input_init_ptr;
               crc0       = 0x7e;
               crc1       = 0x7e;
               got_length = FALSE;
            } /* Endif */
            break;

         case AP_STATE_SYNC_SKIP:
            if (c != AP_SYNC) {
               /* Single sync detected. Restart message reception. */
               count      = 0;
               input_ptr  = input_init_ptr;
               crc0       = 0x7e;
               crc1       = 0x7e;
               got_length = FALSE;
               *input_ptr++ = c;
               ++count;
               AP_CHECKSUM(c, crc0, crc1);
               state = AP_STATE_READING;
            } else {
               state = next_state;
            } /* Endif */
            break;

         case AP_STATE_READING:
            *input_ptr++ = c;
            ++count;
            AP_CHECKSUM(c, crc0, crc1);

            if (got_length ) {
               if (count >= input_length){
                  state = AP_STATE_CS0;
               } /* Endif */
            } else {
               if ( count > MQXA_MSG_CONTROL_OFFSET) {
                  /* The complete packet header has been read in */
                  input_length = GET_LENGTH(input_init_ptr);
                  if (input_length > max_length) {
                     next_state = AP_STATE_SYNC;
                     ++info_ptr->RX_PACKETS_TOO_LONG;
                  } else {
                     got_length = TRUE;
                     if (count >= input_length) {
                        state = AP_STATE_CS0;
                     } /* Endif */
                 } /* Endif */
               } /* Endif */
            } /* Endif */

            if (c == AP_SYNC) {
               next_state = state;
               state      = AP_STATE_SYNC_SKIP;
            } /* Endif */
            break;

         case AP_STATE_CS0:
            packet_crc0 = c;
            state = AP_STATE_CS1;
            if (c == AP_SYNC) {
               next_state = state;
               state      = AP_STATE_SYNC_SKIP;
            } /* Endif */
            break;

         case AP_STATE_CS1:
            packet_crc1 = c;
            state = AP_STATE_DONE;
            if (c == AP_SYNC) {
               next_state = state;
               state      = AP_STATE_SYNC_SKIP;
            } /* Endif */
            break;

         default:
            state = AP_STATE_SYNC;
            break;

      } /* Endswitch */

      if ( state == AP_STATE_DONE ) {
         /* Calculate the CRCs */
         crc1  = (crc1 + 2 * crc0) & 0xFF;
         tmp   = crc0 - crc1;
         crc1  = (crc1 - (crc0 * 2)) & 0xFF;
         crc0  = tmp & 0xFF;

         if ((crc0 == packet_crc0) && (crc1 == packet_crc1)) {
            ++info_ptr->RX_PACKETS;
            pcb_ptr->FRAGMENTS[0].LENGTH = input_length;
            if (info_ptr->READ_CALLBACK_FUNCTION) {
               /* Start CR 398 */
               (*info_ptr->READ_CALLBACK_FUNCTION)(info_ptr->CALLBACK_FD, pcb_ptr);
               /* End CR */
            } else {
               _queue_enqueue((QUEUE_STRUCT_PTR)&info_ptr->READ_QUEUE,
                  (QUEUE_ELEMENT_STRUCT_PTR)&pcb_ptr->QUEUE);
               _lwsem_post(&info_ptr->READ_LWSEM);
            }/* Endif */
            pcb_ptr = _io_pcb_alloc(info_ptr->READ_PCB_POOL, TRUE);
            /* Start CR 385 */
            if (pcb_ptr == NULL) {
               /* Start CR 399 */
               while (pcb_ptr == NULL) {
                  _time_delay_ticks(2);
                  pcb_ptr = _io_pcb_alloc(info_ptr->READ_PCB_POOL, TRUE);
               } /* Endwhile */
               /* End CR 399 */
            } /* Endif */
            /* End CR 385 */
            input_init_ptr = pcb_ptr->FRAGMENTS[0].FRAGMENT;
         } else {
            ++info_ptr->RX_PACKETS_BAD_CRC;
         } /* Endif */
         state = AP_STATE_SYNC;
      } /* Endif */

   } /* Endwhile */

} /* Endbody */


/*TASK*---------------------------------------------------------------------
*
* Task Name       : _io_pcb_mqxa_write_task
* Comments        :
*     This task writes a packet to the async serial device.
*  The packet is in MQX Asynchronous Packet format.
*
*END*---------------------------------------------------------------------*/

void _io_pcb_mqxa_write_task
   (
      /* [IN] the device info */
      uint_32 parameter
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR  info_ptr;
   IO_PCB_STRUCT_PTR            pcb_ptr;
   MQX_FILE_PTR                 fd;
   char _PTR_                   output_ptr;
   _mem_size                    count;
   _mqx_uint                    i;
   uchar                        crc0;
   uchar                        crc1;
   uchar                        tmp;
   uchar                        c;

   info_ptr = (IO_PCB_MQXA_INFO_STRUCT_PTR)parameter;
   fd = info_ptr->FD;

   while (1) {
   
      _lwsem_wait(&info_ptr->WRITE_LWSEM);
      pcb_ptr = (IO_PCB_STRUCT_PTR)((pointer)
         _queue_dequeue(&info_ptr->WRITE_QUEUE));

      crc0   = 0x7e;
      crc1   = 0x7e;
      fputc(AP_SYNC, fd);

      for (i = 0; i < pcb_ptr->NUMBER_OF_FRAGMENTS; i++) {
         count      = pcb_ptr->FRAGMENTS[i].LENGTH + 1;
         output_ptr = (char _PTR_)pcb_ptr->FRAGMENTS[i].FRAGMENT;
         while (--count) {
            c = *output_ptr++;
            fputc(c, fd);
            if ( c == AP_SYNC ) {
               fputc(AP_SYNC, fd);
            } /* Endif */
            AP_CHECKSUM(c, crc0, crc1);
         } /* Endwhile */
      } /* Endfor */
      
      /* Calculate the CRCs */
      crc1  = (crc1 + 2 * crc0) & 0xFF;
      tmp   = crc0 - crc1;
      crc1  = (crc1 - (crc0 * 2)) & 0xFF;
      crc0  = tmp  & 0xFF;

      fputc(crc0, fd);
      if ( crc0 == AP_SYNC ) {
         fputc(AP_SYNC, fd);
      } /* Endif */

      /* One more crc character to output */
      fputc(crc1, fd);
      if ( crc1 == AP_SYNC ) {
         fputc(AP_SYNC, fd);
      } /* Endif */

      info_ptr->TX_PACKETS++;
      _io_pcb_free(pcb_ptr);
      
    } /* Endwhile */

} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_mqxa_ioctl
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_mqxa_ioctl
   (
      /* [IN] the file handle for the device */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      pointer                param_ptr
   )
{ /* Body */
   TASK_TEMPLATE_STRUCT input_tt = 
      { 0, _io_pcb_mqxa_read_task,  IO_PCB_MQXA_STACK_SIZE, 0,
         "io_pcb_mqxa_read_task",  0, 0, 0};
   TASK_TEMPLATE_STRUCT output_tt =
      { 0, _io_pcb_mqxa_write_task, IO_PCB_MQXA_STACK_SIZE, 0,
      "io_pcb_mqxa_write_task", 0, 0, 0};
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;
   IO_PCB_STRUCT_PTR           pcb_ptr;
   _mqx_uint                   result = MQX_OK;
   _psp_code_addr              old_value;
   _psp_code_addr_ptr          pc_ptr = (_psp_code_addr_ptr)param_ptr;
   _psp_data_addr_ptr          pd_ptr = (_psp_data_addr_ptr)param_ptr;
   boolean _PTR_               bool_param_ptr;

   info_ptr = (IO_PCB_MQXA_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;

   switch (cmd) {

      case IO_PCB_IOCTL_ENQUEUE_READQ:
         pcb_ptr = (IO_PCB_STRUCT_PTR)*pd_ptr;
         _queue_enqueue((QUEUE_STRUCT_PTR)&info_ptr->READ_QUEUE, 
            (QUEUE_ELEMENT_STRUCT_PTR)&pcb_ptr->QUEUE);
         _lwsem_post(&info_ptr->READ_LWSEM);
         break;
         
      case IO_PCB_IOCTL_READ_CALLBACK_SET:
         old_value = (_psp_code_addr)info_ptr->READ_CALLBACK_FUNCTION;
         /* Start CR 398 */
         info_ptr->CALLBACK_FD = fd_ptr;
         /* End CR */
         info_ptr->READ_CALLBACK_FUNCTION = (void (_CODE_PTR_)(
            FILE_DEVICE_STRUCT_PTR, IO_PCB_STRUCT_PTR))*pc_ptr;
         *pc_ptr = old_value;
         break;
      
      case IO_PCB_IOCTL_SET_INPUT_POOL:
         old_value = (_psp_code_addr)info_ptr->READ_PCB_POOL;
         info_ptr->READ_PCB_POOL = (_io_pcb_pool_id)*pc_ptr;
         *pc_ptr = old_value;
         break;

      case IO_PCB_IOCTL_START:
         if (info_ptr->INPUT_TASK == MQX_NULL_TASK_ID) {
            input_tt.TASK_PRIORITY  = info_ptr->INIT.INPUT_TASK_PRIORITY;
            input_tt.CREATION_PARAMETER  = (uint_32)info_ptr;

            output_tt.TASK_PRIORITY = info_ptr->INIT.OUTPUT_TASK_PRIORITY;
            output_tt.CREATION_PARAMETER = (uint_32)info_ptr;
   
            info_ptr->INPUT_TASK = _task_create(0, 0, (uint_32)&input_tt);
            if (info_ptr->INPUT_TASK == MQX_NULL_TASK_ID){
               return(_task_get_error());
            } /* Endif */

            info_ptr->OUTPUT_TASK = _task_create(0, 0, (uint_32)&output_tt);
            if (info_ptr->OUTPUT_TASK == MQX_NULL_TASK_ID){
               return(_task_get_error());
            } /* Endif */
         }/* Endif */
         break;
         
      case IO_PCB_IOCTL_UNPACKED_ONLY:
         bool_param_ptr = (boolean _PTR_)param_ptr;
         *bool_param_ptr = TRUE;
         break;

      default:
            result = _io_ioctl(info_ptr->FD, cmd, param_ptr);
         break;

   } /* Endswitch */
   return result;

} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pcb_mqxa_uninstall
* Returned Value  : _mqx_uint result   MQX_OK or an error code.
* Comments        :
*
*END*---------------------------------------------------------------------*/

_mqx_int _io_pcb_mqxa_uninstall
   (
      IO_PCB_DEVICE_STRUCT_PTR dev_ptr
   )
{ /* Body */
   IO_PCB_MQXA_INFO_STRUCT_PTR info_ptr;
   _mqx_int                    result = MQX_OK;
   
   info_ptr = (IO_PCB_MQXA_INFO_STRUCT_PTR)dev_ptr->DRIVER_INIT_PTR;
   if (info_ptr->FD) {
       return IO_ERROR_DEVICE_BUSY;
   }/* Endif */
   _mem_free(info_ptr);

    return result;
       
} /* Endbody */

#endif // MQX_USE_IPC
