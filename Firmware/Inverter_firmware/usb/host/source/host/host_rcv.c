/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: host_rcv.c$
* $Version : 3.8.10.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
*   This file contains the USB Host API specific functions to receive data.
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_rcv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_recv_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Receive Data routine is non-blocking routine that causes a buffer 
* to be made available for data recieved from the 
* USB host. It takes the buffer and passes it down to lower
level hardware driver. 
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_recv_data
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           handle, 

      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
   )
{ /* Body */
   PIPE_STRUCT_PTR           pipe_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
   USB_STATUS    status;
   TR_STRUCT_PTR tr_ptr;
   USB_STATUS    return_code;   

#ifdef _HOST_DEBUG_
   DEBUG_LOG_TRACE("_usb_host_recv_data");
#endif

   USB_lock();

   return_code = _usb_host_set_up_tr(pipe_handle, tr_params_ptr, &tr_ptr);

   if (return_code != USB_STATUS_TRANSFER_QUEUED) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_recv_data transfer queue failed");
      #endif
      return USB_log_error(__FILE__,__LINE__,return_code);
   } /* Endif */

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;

   /* We will do soon cache invalidation of buffer. This can have an impact to the
   ** data that sit in the same cache line, but are not part of the buffer. In order
   ** not to invalidate their cached values, we do first flushing those data.
   */
   USB_dcache_flush_mlines((void *)tr_ptr->RX_BUFFER, tr_ptr->RX_LENGTH);

   /* To ensure that the USB DMA transfer will work on a buffer that is not cached,
   ** we invalidate buffer cache lines.
   */
   USB_dcache_invalidate_mlines((void *)tr_ptr->RX_BUFFER, tr_ptr->RX_LENGTH);

   /* We have obtained the current TR on the Pipe's TR list 
   ** from _usb_host_set_up_tr
   */

   /* Call the low-level routine to receive data */
   status = _usb_host_recv_data_call_interface (handle, pipe_ptr, tr_ptr);
         
   USB_unlock();
	   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_recv_data SUCCESSFUL");
   #endif

   if(status == USB_OK) 
   {
      return USB_STATUS_TRANSFER_QUEUED;
   }
   else
   {
      return USB_log_error(__FILE__,__LINE__,status);
   }

   
} /* Endbody */

/* EOF */
