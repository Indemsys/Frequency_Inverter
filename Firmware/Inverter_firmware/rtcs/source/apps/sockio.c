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
* $FileName: sockio.c$
* $Version : 3.8.16.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the implementation of an MQX
*   socket I/O device.
*
*END************************************************************************/

#include <rtcs.h>
#include <fio.h>

#define SOCKIO_MIN_BUFFER 64
#define SOCKIO_MAX_BUFFER (64*1024)

typedef struct io_socket {
   uint_32  SOCKET;
   uint_32  COUNT;
   uint_32  SIZE;
   char_ptr BUFFER;
} IO_SOCKET, _PTR_ IO_SOCKET_PTR;

_mqx_int _io_socket_open  (MQX_FILE_PTR, char _PTR_,  char _PTR_);
_mqx_int _io_socket_close (MQX_FILE_PTR);
_mqx_int _io_socket_read  (MQX_FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_socket_write (MQX_FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_socket_ioctl (MQX_FILE_PTR, _mqx_uint, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_install
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

int_32 _io_socket_install
   (
      char_ptr identifier
   )
{ /* Body */

   return _io_dev_install(identifier,
                          _io_socket_open,
                          _io_socket_close,
                          _io_socket_read,
                          _io_socket_write,
#if MQX_VERSION < 250
                          (int_32(_CODE_PTR_)(MQX_FILE_PTR, uint_32, uint_32_ptr))
#endif
                          _io_socket_ioctl,
                          NULL );
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_open
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_open
   (
      MQX_FILE_PTR fd_ptr,
      char _PTR_  open_name_ptr,
      char _PTR_  flags_ptr
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = RTCS_mem_alloc_zero(sizeof(*io_ptr));
   if (io_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
   
   _mem_set_type(io_ptr, MEM_TYPE_IO_SOCKET);
   
   fd_ptr->DEV_DATA_PTR = io_ptr;

   io_ptr->SOCKET = (uint_32)flags_ptr;

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_close
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_close
   (
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */

   IO_SOCKET_PTR  io_ptr;


   if (fd_ptr->DEV_DATA_PTR != NULL) 
   {
      io_ptr = fd_ptr->DEV_DATA_PTR;
      if (io_ptr->BUFFER != NULL) 
      {
         _mem_free(io_ptr->BUFFER);
      }
      fd_ptr->DEV_DATA_PTR = NULL;
      _mem_free(io_ptr);
   }
   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_read
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_read
   (
      MQX_FILE_PTR fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {
   
   // flush any pending data before reading
   if (io_ptr->COUNT) {
      send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
      io_ptr->COUNT = 0;
   }

     return (_mqx_int)recv(io_ptr->SOCKET, data_ptr, num, 0);
   } /* Endif */

   return -1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_set_send_push
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/
static _mqx_int _io_socket_set_send_push( uint_32 sock, boolean value)
{
   return setsockopt(sock, SOL_TCP, OPT_SEND_PUSH, &value, sizeof(value));
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_write
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_write
   (
      MQX_FILE_PTR fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr = fd_ptr->DEV_DATA_PTR;
   _mqx_int      sent = IO_ERROR;

   if (io_ptr != NULL) {
      if ( io_ptr->SIZE ) {
         if  (io_ptr->COUNT+num > io_ptr->SIZE) {
            
            if (io_ptr->COUNT) {
               send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               io_ptr->COUNT = 0;
            }
            sent = (_mqx_int)send(io_ptr->SOCKET, data_ptr, num, 0);

         } else if (num==1) {
            // most writes are 1 byte.
            io_ptr->BUFFER[io_ptr->COUNT++] = *data_ptr;
            
            sent=1;
            
            if (io_ptr->COUNT == io_ptr->SIZE) {
               // buffer is full, flush it
               send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               io_ptr->COUNT=0;
            }
            
         } else {
            _mem_copy(data_ptr,&io_ptr->BUFFER[io_ptr->COUNT],num);
            io_ptr->COUNT+=num;
            sent=num;
         }
      } else {     
         sent = (_mqx_int)send(io_ptr->SOCKET, data_ptr, num, 0);
      } 
   }

   return sent;

} /* Endbody */

static char buffer[256];


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_ioctl
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_ioctl
   (
      MQX_FILE_PTR fd_ptr,
      _mqx_uint   cmd,
      pointer     param_ptr
   )
{ /* Body */
   IO_SOCKET_PTR  io_ptr;
   _mqx_int       result = IO_ERROR, bytes;
   uint_32        size;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {
      switch (cmd) {
         case IO_IOCTL_CHAR_AVAIL:
            if (RTCS_selectset(&io_ptr->SOCKET, 1, (uint_32)-1)) {
               *(boolean _PTR_)param_ptr = TRUE;
            } else {
               *(boolean _PTR_)param_ptr = FALSE;
            } /* Endif */
            result=MQX_OK;
            break;         
      
         case IO_IOCTL_FLUSH_OUTPUT:
            if (io_ptr->COUNT) {
               bytes = send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               if (bytes==io_ptr->COUNT) {
                  io_ptr->COUNT = 0;
                  result = MQX_OK;
               }
            } else {
               result = MQX_OK;
            }
            break;
            
         case IO_IOCTL_SET_BLOCK_MODE:
            size = *(uint_32_ptr)param_ptr;
            result=MQX_OK;
            if (size != io_ptr->SIZE) {
               // First, clear out the old buffer
               if (io_ptr->BUFFER) {
                  if (io_ptr->COUNT) {
                     bytes = send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
                     io_ptr->COUNT = 0;
                  }
                  _mem_free(io_ptr->BUFFER);
                  io_ptr->BUFFER = NULL;
               }
            
               if (size) {
                  // Size is set, entering block mode
                  if (size < SOCKIO_MIN_BUFFER) {
                     size = SOCKIO_MIN_BUFFER;
                  }
                  if (size > SOCKIO_MAX_BUFFER) {
                     size = SOCKIO_MAX_BUFFER;
                  }

                  io_ptr->BUFFER = RTCS_mem_alloc(size);
                  if (io_ptr->BUFFER==NULL) {
                     size = 0;
                     result = IO_ERROR;
                  } else {
                     _io_socket_set_send_push(io_ptr->SOCKET, TRUE);
                  }
                  
               }                     

               io_ptr->SIZE = size;
            }
            break;      
            
         case IO_IOCTL_GET_BLOCK_SIZE:
            result=MQX_OK;
            *(boolean _PTR_)param_ptr = io_ptr->SIZE;
            break;
            
       }      
   }

   return result;

} /* Endbody */


/* EOF */
