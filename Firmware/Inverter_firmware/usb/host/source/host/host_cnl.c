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
* $FileName: host_cnl.c$
* $Version : 3.8.12.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the USB Host API specific functions to cancel 
*   a transfer.
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_cnl.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_cancel_transfer
*  Returned Value : error or status of the transfer before cancellation
*  Comments       :
* _usb_host_cancel_transfer is a non-blocking routine that causes a transfer to 
* be terminated. 
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_cancel_transfer
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle     handle,

      /* [IN] the pipe handle */
      _usb_pipe_handle     pipe_handle,

      /* [IN] the transfer numnber for this pipe */
      uint_32              tr_number
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   PIPE_STRUCT_PTR            pipe_descr_ptr = (PIPE_STRUCT_PTR)pipe_handle;
   TR_STRUCT_PTR              pipe_tr_ptr;
   USB_STATUS error;
   uint_32  bRet;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_cancel_transfer");
   #endif

   USB_lock();

   /* Check if valid pipe id */
   if (pipe_descr_ptr->PIPE_ID > USBCFG_MAX_PIPES) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_cancel_transfer, invalid pipe");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
   } /* Endif */

   pipe_tr_ptr = pipe_descr_ptr->tr_list_ptr;
   while (pipe_tr_ptr->TR_INDEX != tr_number)  {
      /* If unused TR found, or end of list, exit with bad status */
      if ((pipe_tr_ptr->TR_INDEX == 0) ||
         (pipe_tr_ptr->NEXT== pipe_descr_ptr->tr_list_ptr))
      {
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_cancel_transfer, invalid pipe");
         #endif
         return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
      } /* EndIf */
      pipe_tr_ptr = pipe_tr_ptr->NEXT;   
   } /* EndWhile */

   /* Get the current status */
   bRet = pipe_tr_ptr->STATUS;

   /* Cancel the transfer at low-level */
   error = _usb_host_cancel_call_interface (handle, pipe_descr_ptr, pipe_tr_ptr);
   
   if (error != USB_OK)
   {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_cancel_transfer, FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }

   /* Indicate that the pipe is idle */
   pipe_tr_ptr->STATUS = USB_STATUS_IDLE;

   if (pipe_tr_ptr->CALLBACK != NULL) {
      pipe_tr_ptr->CALLBACK((pointer)pipe_descr_ptr,
         pipe_tr_ptr->CALLBACK_PARAM,
         NULL,
         0,
         USBERR_SHUTDOWN);
   } /* Endif */

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_cancel_transfer,SUCCESSFUL");
   #endif

   /* Return the status prior to the transfer cancellation */
   return bRet;

} /* Endbody */
 
/* EOF */
