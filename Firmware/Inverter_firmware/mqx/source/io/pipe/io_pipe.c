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
* $FileName: io_pipe.c$
* $Version : 3.8.6.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the driver functions for the Pipe device.
*
*END************************************************************************/

/* suppress warning if mutex component is not enabled in MQX */
#define MQX_DISABLE_CONFIG_CHECK 1

#include "mqx_inc.h"                         
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "charq.h"
#include "mutex.h"
#include "pipe_prv.h"
#include "io_pipe.h"


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_pipe_install
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    Install an interrupt driven serial device.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_pipe_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr            identifier,
      
      /* [IN] The pipe queue size to use */
      uint_32             queue_size,

      /* [IN] Currently not used */
      uint_32             flags

   )
{ /* Body */
   IO_PIPE_INIT_STRUCT_PTR  io_pipe_init_ptr;

   /* Allocate an Pipe installation structure */
   io_pipe_init_ptr = _mem_alloc_system_zero(
      (uint_32)sizeof(IO_PIPE_INIT_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (io_pipe_init_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   } /* Endif */
#endif

   /* Set queue size for Pipe */
   io_pipe_init_ptr->QUEUE_SIZE = queue_size;

   /* Copy flags field */
   io_pipe_init_ptr->FLAGS      = flags;

   /* Install device */
   return (_io_dev_install_ext(identifier,
      _io_pipe_open, _io_pipe_close,
      _io_pipe_read, _io_pipe_write,
      _io_pipe_ioctl, _io_pipe_uninstall,
      (pointer)io_pipe_init_ptr)); 

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_pipe_uninstall
* Returned Value   : _mqx_int a task error code or MQX_OK
* Comments         :
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_pipe_uninstall
   (
      /* [IN] The devices context */
      IO_DEVICE_STRUCT_PTR  io_dev_ptr
   )
{ /* Body */
  
   if (io_dev_ptr->DRIVER_INIT_PTR) {
      _mem_free(io_dev_ptr->DRIVER_INIT_PTR);
   }/* Endif */

   return(MQX_OK);

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_pipe_open
* Returned Value   : int_32 task error code or MQX_OK
* Comments         :
*    This routine opens an instance of a Pipe. It acquires
*    memory, then stores information into it about the Pipe.
*    This memory is returned as a FILE 'handle' to be used for 
*    all other pipe functions by any task. 
*
* 
*END**********************************************************************/

_mqx_int _io_pipe_open
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
   IO_DEVICE_STRUCT_PTR          io_dev_ptr = fd_ptr->DEV_PTR;
   IO_PIPE_INIT_STRUCT_PTR       io_pipe_init_ptr;
   IO_PIPE_INFO_STRUCT_PTR       io_pipe_info_ptr;
   MUTEX_ATTR_STRUCT             mutex_attr;
   uint_32                       result = MQX_OK;

   /* Allocate a Pipe information structure */
   io_pipe_info_ptr = _mem_alloc_system_zero(
      (uint_32)sizeof(IO_PIPE_INFO_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (io_pipe_info_ptr == NULL){
      return(MQX_OUT_OF_MEMORY);
   }/* Endif */
#endif 

   fd_ptr->DEV_DATA_PTR = io_pipe_info_ptr;
   io_pipe_init_ptr = (IO_PIPE_INIT_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   
   io_pipe_info_ptr->QUEUE_SIZE = io_pipe_init_ptr->QUEUE_SIZE;

   /* Initialize mutexes */
   result = _mutatr_init(&mutex_attr); 
   
#if MQX_CHECK_ERRORS
   if (result != MQX_EOK) {
      _mem_free(io_pipe_info_ptr);
      return (result);
   } /* Endif */
#endif 

   _mutatr_set_wait_protocol(&mutex_attr, MUTEX_PRIORITY_QUEUEING);
   _mutatr_set_sched_protocol(&mutex_attr, MUTEX_PRIO_INHERIT);

   result = _mutex_init(&io_pipe_info_ptr->READ_MUTEX, &mutex_attr);
#if MQX_CHECK_ERRORS
   if (result != MQX_EOK) {
      _mem_free(io_pipe_info_ptr);
      return (result);
   } /* Endif */
#endif 
   
   result = _mutex_init(&io_pipe_info_ptr->WRITE_MUTEX, &mutex_attr);
#if MQX_CHECK_ERRORS
   if (result != MQX_EOK) {
      _mem_free(io_pipe_info_ptr);
      return (result);
   } /* Endif */
#endif 

   result = _mutex_init(&io_pipe_info_ptr->ACCESS_MUTEX, &mutex_attr);
#if MQX_CHECK_ERRORS
   if (result != MQX_EOK) {
      _mem_free(io_pipe_info_ptr);
      return (result);
   } /* Endif */
#endif 

   /* Initialize semaphores */

   result = _lwsem_create(&io_pipe_info_ptr->FULL_SEM, 0);
#if MQX_CHECK_ERRORS
   if (result != MQX_OK) {
      _mem_free(io_pipe_info_ptr);
      return (result);
   } /* Endif */
#endif 

   result = _lwsem_create(&io_pipe_info_ptr->EMPTY_SEM, 0);
#if MQX_CHECK_ERRORS
   if (result != MQX_OK) {
      _mem_free(io_pipe_info_ptr);
      return (result);
   } /* Endif */
#endif 
                          
   /* Allocate queue structure for pipe char queue */
   io_pipe_info_ptr->QUEUE = (pointer)_mem_alloc_system(
      sizeof(CHARQ_STRUCT) - (4 * sizeof(char)) + io_pipe_info_ptr->QUEUE_SIZE);

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (io_pipe_info_ptr->QUEUE == NULL){
      _mem_free(io_pipe_info_ptr);
      return(MQX_OUT_OF_MEMORY);
   }/* Endif */
#endif            

   /* Initialize Pipe queue */
   _CHARQ_INIT(io_pipe_info_ptr->QUEUE, io_pipe_init_ptr->QUEUE_SIZE);
   
   return(result);

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_pipe_close
* Returned Value   : int_32 error code
* Comments         :
*    This routine closes the Pipe and free the datastructures.
* 
*END**********************************************************************/

_mqx_int _io_pipe_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR fd_ptr
   )
{ /* Body */
   IO_PIPE_INFO_STRUCT_PTR         io_pipe_info_ptr;
   int_32                          result = MQX_OK;

   io_pipe_info_ptr = fd_ptr->DEV_DATA_PTR;

   /* Destroy the mutexes - doesn't free memory*/

    _mutex_destroy(&io_pipe_info_ptr->READ_MUTEX);
    _mutex_destroy(&io_pipe_info_ptr->WRITE_MUTEX);
    _mutex_destroy(&io_pipe_info_ptr->ACCESS_MUTEX);
                                 
   /* Start CR 2237 */
   /* Derstroy the lightweight semaphores */
   _lwsem_destroy(&io_pipe_info_ptr->FULL_SEM);
   _lwsem_destroy(&io_pipe_info_ptr->EMPTY_SEM);
   /* End CR 2237 */

   /* Free Pipe data structures */
   _mem_free(io_pipe_info_ptr->QUEUE);
   _mem_free(io_pipe_info_ptr);

   return(result);

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_pipe_read
* Returned Value   : int_32 number of characters read
* Comments         :
*    This routine reads characters from the Pipe queue.
*    This function block a task if there are no characters in the
*    Pipe queue.
*
*END*********************************************************************/

_mqx_int _io_pipe_read
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] where the characters are to be stored */
      char             _PTR_ data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int               num
      
   )
{ /* Body */      
   volatile IO_PIPE_INFO_STRUCT_PTR       io_pipe_info_ptr;
   uchar                                  c = 0;
   uint_32                                i = num + 1;
   volatile CHARQ_STRUCT _PTR_            pipe_queue;
   uint_32                                error;

   io_pipe_info_ptr = fd_ptr->DEV_DATA_PTR;

   pipe_queue = io_pipe_info_ptr->QUEUE;

   /* Turn off pipe abort flags */
   io_pipe_info_ptr->KM_ABORT_READ = FALSE;
   
   /* Lock out other reading tasks */       
   error = _mutex_lock(&io_pipe_info_ptr->READ_MUTEX);

#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif
     
   /* Lock out access of a writer task to the pipe data structures */
   error = _mutex_lock(&io_pipe_info_ptr->ACCESS_MUTEX);
#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif   
   
   /* Start reading characters from the pipe*/
   while ( --i ) {

     /* If the pipe is empty, wait until the pipe is not empty */                            
      while (_CHARQ_EMPTY(pipe_queue)) {


         error = _mutex_unlock(&io_pipe_info_ptr->ACCESS_MUTEX);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_EOK ) {
            return (-1);
         } /* Endif */
      #endif   

         error = _lwsem_post(&io_pipe_info_ptr->FULL_SEM);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_OK ) {
            return (-1);
         } /* Endif */
      #endif   
         
         error = _lwsem_wait(&io_pipe_info_ptr->EMPTY_SEM);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_OK ) {
            return (-1);
         } /* Endif */
      #endif   
         
       if(io_pipe_info_ptr->KM_ABORT_READ)
       {
		   error = _mutex_unlock(&io_pipe_info_ptr->READ_MUTEX);
		#if MQX_CHECK_ERRORS
		   if ( error != MQX_EOK ) {
		      return (-1);
		   } /* Endif */
		#endif
       	
       		return num - i;	
       }
       
       
         /* Lock out access of a writer task to the pipe data structures */
         error = _mutex_lock(&io_pipe_info_ptr->ACCESS_MUTEX);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_EOK ) {
            return (-1);
         } /* Endif */
      #endif   

      } /* Endwhile */

      /* Read char from pipe */
      _CHARQ_DEQUEUE(pipe_queue,c);

      /* Copy data to the returned buffer */
      *data_ptr++ = c;
                
   } /* Endwhile */

   error = _lwsem_post(&io_pipe_info_ptr->FULL_SEM);
#if MQX_CHECK_ERRORS
   if ( error != MQX_OK ) {
      return (-1);
   } /* Endif */
#endif   

   /* Unlock access access mutex and read mutex */
   error = _mutex_unlock(&io_pipe_info_ptr->ACCESS_MUTEX);

#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif

   error = _mutex_unlock(&io_pipe_info_ptr->READ_MUTEX);

#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif
   
   return num;

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _io_pipe_write
* Returned Value   : void
* Comments         :
*    This routine writes characters to the Pipe.  It will block the task 
**   if the Pipe is full.
*
*END**********************************************************************/

_mqx_int _io_pipe_write
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] where the characters to write out are */
      char             _PTR_ data_ptr,

      /* [IN] the number of characters to write */
      _mqx_int               num
   )
{ /* Body */
   volatile IO_PIPE_INFO_STRUCT_PTR         io_pipe_info_ptr;
   uint_32                                  i = num + 1;
   uchar                                    c;
   volatile CHARQ_STRUCT _PTR_              pipe_queue;
   uint_32                                  error;


   io_pipe_info_ptr = fd_ptr->DEV_DATA_PTR;

   pipe_queue = io_pipe_info_ptr->QUEUE;

   /* Turn off pipe abort flags */
   io_pipe_info_ptr->KM_ABORT_WRITE = FALSE;
   
   /* Lock out other reading tasks */       
   error = _mutex_lock(&io_pipe_info_ptr->WRITE_MUTEX);
#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif

   /* Lock out access of a writer task to the pipe data structures */
   error = _mutex_lock(&io_pipe_info_ptr->ACCESS_MUTEX);
#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif
   
   /* Start writing characters to the pipe */
   while ( --i ) {
                               
      /* If the pipe is full, wait until the pipe is not full */
      while ( _CHARQ_FULL(pipe_queue) ) {

       
         error = _mutex_unlock(&io_pipe_info_ptr->ACCESS_MUTEX);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_EOK ) {
            return (-1);
         } /* Endif */
      #endif   

         error = _lwsem_post(&io_pipe_info_ptr->EMPTY_SEM);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_OK ) {
            return (-1);
         } /* Endif */
      #endif   
         
         error = _lwsem_wait(&io_pipe_info_ptr->FULL_SEM);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_OK ) {
            return (-1);
         } /* Endif */
      #endif   
         
       if(io_pipe_info_ptr->KM_ABORT_WRITE)
       {
       	   io_pipe_info_ptr->KM_ABORT_WRITE = FALSE;
		   error = _mutex_unlock(&io_pipe_info_ptr->WRITE_MUTEX);
		#if MQX_CHECK_ERRORS
		   if ( error != MQX_EOK ) {
		      return (-1);
		   } /* Endif */
		#endif
       	
       		return num - i;	
       }
         /* Lock out access of a writer task to the pipe data structures */
         error = _mutex_lock(&io_pipe_info_ptr->ACCESS_MUTEX);
      #if MQX_CHECK_ERRORS
         if ( error != MQX_EOK ) {
            return (-1);
         } /* Endif */
      #endif   

      } /* Endwhile */

      c = *data_ptr++;

      /* Write the next char into the pipe */
      _CHARQ_ENQUEUE(pipe_queue,c);

   } /* Endwhile */

   error = _lwsem_post(&io_pipe_info_ptr->EMPTY_SEM);
#if MQX_CHECK_ERRORS
   if ( error != MQX_OK ) {
      return (-1);
   } /* Endif */
#endif   

   /* Unlock access mutex and write mutex */
   error = _mutex_unlock(&io_pipe_info_ptr->ACCESS_MUTEX);
#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif

   error = _mutex_unlock(&io_pipe_info_ptr->WRITE_MUTEX);

#if MQX_CHECK_ERRORS
   if ( error != MQX_EOK ) {
      return (-1);
   } /* Endif */
#endif


   return num;
   
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_pipe_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_pipe_ioctl
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] the ioctl command */
      uint_32                cmd,

      /* [IN] the ioctl parameters */
      pointer                param_ptr
   )
{ /* Body */
   IO_PIPE_INFO_STRUCT_PTR  io_pipe_info_ptr;
   uint_32                  result = MQX_OK;
   _mqx_uint_ptr            uparam_ptr = param_ptr;

   io_pipe_info_ptr = fd_ptr->DEV_DATA_PTR;

   result = _mutex_lock(&io_pipe_info_ptr->ACCESS_MUTEX);
#if MQX_CHECK_ERRORS
   if ( result != MQX_OK ) {
      return (result);
   } /* Endif */
#endif

   switch (cmd) {

      case PIPE_IOCTL_CHAR_AVAIL:
         if ( !_CHARQ_EMPTY(io_pipe_info_ptr->QUEUE) ) {
           *uparam_ptr = (uint_32)TRUE;
         } else {
           *uparam_ptr = (uint_32)FALSE;
         } /* Endif */
      break;

      case PIPE_IOCTL_GET_SIZE:
         *uparam_ptr =  io_pipe_info_ptr->QUEUE_SIZE;
      break;

      case PIPE_IOCTL_FULL:
         if ( _CHARQ_FULL(io_pipe_info_ptr->QUEUE) ) {
            *uparam_ptr = (uint_32)TRUE;
         } else {
            *uparam_ptr = (uint_32)FALSE;
         } /* Endif */
      break;
   
      case PIPE_IOCTL_EMPTY:
         if ( _CHARQ_EMPTY(io_pipe_info_ptr->QUEUE) ) {
            *uparam_ptr = (uint_32)TRUE;
         } else {
            *uparam_ptr = (uint_32)FALSE;
         } /* Endif */
      break;

      case PIPE_IOCTL_RE_INIT:
           _CHARQ_INIT(io_pipe_info_ptr->QUEUE, io_pipe_info_ptr->QUEUE_SIZE)
           io_pipe_info_ptr->KM_ABORT_READ = TRUE;
           io_pipe_info_ptr->KM_ABORT_WRITE = TRUE;
		
           result = _lwsem_post(&io_pipe_info_ptr->EMPTY_SEM);
      #if MQX_CHECK_ERRORS
         if ( result != MQX_OK ) {
            return (-1);
         } /* Endif */
      #endif   
		   result = _lwsem_post(&io_pipe_info_ptr->FULL_SEM);
    	   #if MQX_CHECK_ERRORS
	         if ( result != MQX_OK ) {
           	    return (-1);
         	 } /* Endif */
     	   #endif  	      	   
      break;
          
      case PIPE_IOCTL_NUM_CHARS_FULL:
           *uparam_ptr = _CHARQ_SIZE(io_pipe_info_ptr->QUEUE);
      break;

      case PIPE_IOCTL_NUM_CHARS_FREE:
           *uparam_ptr = io_pipe_info_ptr->QUEUE_SIZE -
              _CHARQ_SIZE(io_pipe_info_ptr->QUEUE);
      break;
      
      default:
                      
      break;
   } /* Endswitch */

   result = _mutex_unlock(&io_pipe_info_ptr->ACCESS_MUTEX);
#if MQX_CHECK_ERRORS
   if ( result != MQX_OK ) {
      return (result);
   } /* Endif */
#endif
               
   return result;

} /* Endbody */


/* EOF */
