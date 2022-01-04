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
* $FileName: usb_host_printer.c$
* $Version : 3.8.10.0$
* $Date    : Sep-13-2012$
*
* Comments:
*
*   This file contains  the USB printer class driver
*   Implementation notes:
*   Do not send class "GET" requests while printing -- bidirectional
*   printers may place the status in the print buffer (e.g. some HP).
*   Multiple driver instances require structs customized with 
*   endpoint/interface etc. to be locked or allocated per-instance.
*
*END************************************************************************/
#include "usb_host_printer.h"

#include "mqx_host.h"

/*--------------------------------------------------------------**
** This anchor points to the first class/interface in a linked  **
**   chain of structs, one for each functional print interface. **
**   Items are added by calling "select interface" and removed  **
**   by "delete interface".  Typically an application will call **
**   these select in its attach callback routine.  It may later **
**   call delete, or the call may be automatic during detach.   **
**--------------------------------------------------------------*/

static PRINTER_INTERFACE_STRUCT_PTR prt_anchor = NULL;

/* Private functions for use within this file only */
static USB_STATUS usb_printer_bulk_inout(CLASS_CALL_STRUCT_PTR,
   TR_INIT_PARAM_STRUCT_PTR, boolean);
static USB_STATUS usb_printer_class_send(CLASS_CALL_STRUCT_PTR,
   TR_INIT_PARAM_STRUCT_PTR);
static USB_STATUS usb_printer_class_verify(CLASS_CALL_STRUCT_PTR,
   TR_INIT_PARAM_STRUCT_PTR, tr_callback, pointer);


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_bulk_inout
* Returned Value : USB_OK or error code
* Comments       :
*     Send/receive data already built into a TR structure.
*     This function is a common epilog used by the bulk IO calls.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_bulk_inout
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] TR containing setup packet to be sent */
      TR_INIT_PARAM_STRUCT_PTR   tr_ptr,
      
      /* [IN] send_flag = TRUE means send, = FALSE means receive */
      boolean                    send_flag
   )
{ /* Body */

   PRINTER_INTERFACE_STRUCT_PTR  pis_ptr;
   USB_STATUS                    error = USBERR_NO_INTERFACE;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_bulk_inout");
   #endif

   USB_lock ();

   if (usb_host_class_intf_validate(cc_ptr)) {
      pis_ptr = (PRINTER_INTERFACE_STRUCT_PTR)cc_ptr->class_intf_handle;

      if (send_flag) {
         error = _usb_host_send_data (pis_ptr->host_handle,
            pis_ptr->bulk_out_pipe, tr_ptr);
      } else {
         error = _usb_host_recv_data (pis_ptr->host_handle,
            pis_ptr->control_pipe, tr_ptr);

      } /* EndIf */
   } /* Endif */

   USB_unlock ();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_bulk_inout, SUCCESSFUL");
   #endif
   
   return error;   

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_class_send
* Returned Value : USB_OK or error code
* Comments       :
*     Send a setup packet already built into a TR structure.
*     This function is a common epilog used by the class calls.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_class_send
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] TR containing setup packet to be sent */
      TR_INIT_PARAM_STRUCT_PTR   tr_ptr
   )
{ /* Body */

   DESCRIPTOR_UNION              desc;
   PRINTER_INTERFACE_STRUCT_PTR  pis_ptr;
   USB_STATUS                    error = USBERR_NO_INTERFACE;
   USB_SETUP_PTR                 dev_req;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_class_send");
   #endif
   
   USB_lock ();
   if (usb_host_class_intf_validate (cc_ptr)) {
      pis_ptr = (PRINTER_INTERFACE_STRUCT_PTR)
         cc_ptr->class_intf_handle;
      desc.pntr = pis_ptr->intf_handle;
      dev_req = (USB_SETUP_PTR)tr_ptr->DEV_REQ_PTR;

      *(uint_16*)dev_req->WVALUE = HOST_TO_LE_SHORT_CONST(0);
      dev_req->WINDEX[0] = desc.intf->bAlternateSetting;
      dev_req->WINDEX[1] = desc.intf->bInterfaceNumber;
      *(uint_16*)dev_req->WLENGTH = HOST_TO_LE_SHORT(tr_ptr->G.RX_LENGTH);
         
      error = _usb_host_send_setup (pis_ptr->host_handle, 
            pis_ptr->control_pipe, 
            tr_ptr);
   } /* EndIf */

   USB_unlock ();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_class_send, SUCCESSFUL");
   #endif
   
   return error;   

} /* Endbody */



/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_class_verify
* Returned Value : USB_OK or error code
* Comments       :
*     Verify that the device is still valid -- in particular that
*     it has not been detached (since that occurs without notice).
*     This function is a common prolog used by class calls.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_class_verify
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] TR memory already allocated, initialized here */
      TR_INIT_PARAM_STRUCT_PTR   tr_ptr,
   
      /* [IN] callback for completion/status */
      tr_callback                callback,
   
      /* [IN] user parameter returned by callback */
      pointer                    call_parm
   )
{ /* Body */

   PRINTER_INTERFACE_STRUCT_PTR  pis_ptr;
   USB_STATUS                    error = USBERR_NO_INTERFACE;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_class_verify");
   #endif
   
   USB_lock ();

   if (usb_host_class_intf_validate (cc_ptr)) {
      pis_ptr = (PRINTER_INTERFACE_STRUCT_PTR)
         cc_ptr->class_intf_handle;
      error =  usb_hostdev_validate (pis_ptr->dev_handle);

      if (error == USB_OK) {
         if (callback == NULL) {
            error = USBERR_NULL_CALLBACK;
         } else {
            usb_hostdev_tr_init (tr_ptr, callback, call_parm);
         } /* Endif */
      } /* EndIf */
   } /* Endif */

   USB_unlock ();   

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_class_verify, SUCCESSFUL");
   #endif
   
   return error;   

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_get_device_ID
* Returned Value : USB_OK or error code
* Comments       :
*     Get IEEE 1284 Device ID string.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_get_device_ID
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] callback for completion/status */
      tr_callback                callback,
      
      /* [IN] user parameter returned by callback */
      pointer                    call_parm,
      
      /* [IN] buffer length */
      uint_32                    buf_size,
      
      /* [OUT] device ID buffer */
      uchar_ptr                  buffer
   )
{ /* Body */

   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_get_device_ID");
   #endif

   if (USB_OK != (error = usb_printer_class_verify 
      (cc_ptr, &tr, callback, call_parm)))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_printer_get_device_ID, error validating class");
      #endif
      return error;
   } /* Endif */

   tr.G.RX_BUFFER = (pointer)buffer;
   tr.G.RX_LENGTH = buf_size;
   tr.DEV_REQ_PTR = (uchar_ptr)(&dev_req);
   dev_req.BMREQUESTTYPE = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_IN;
   dev_req.BREQUEST = USB_PRINTER_GET_DEVICE_ID;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_get_device_ID, SUCCESSFUL");
   #endif

   return usb_printer_class_send(cc_ptr, &tr);

} /* Endbody */
 

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_get_port_status
* Returned Value : USB_OK or error code
* Comments       :
*     Get current printer status byte.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_get_port_status
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] callback for completion/status */
      tr_callback                callback,
      
      /* [IN] user parameter returned by callback */
      pointer                    call_parm,
      
      /* [OUT] device ID buffer */
      uint_8_ptr                 status_ptr
   )
{ /* Body */

   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_get_port_status");
   #endif
   
   if (USB_OK != (error = usb_printer_class_verify 
      (cc_ptr, &tr, callback, call_parm)))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_printer_get_port_status, error class verify");
      #endif
      return error;
   } /* Endif */

   tr.G.RX_BUFFER = (pointer)status_ptr;
   tr.G.RX_LENGTH = 1;
   tr.DEV_REQ_PTR = (uchar_ptr)(&dev_req);
   dev_req.BMREQUESTTYPE = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_IN;
   dev_req.BREQUEST = USB_PRINTER_GET_PORT_STATUS;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_get_port_status, SUCCESSFUL");
   #endif
   
   return usb_printer_class_send(cc_ptr, &tr);   

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_soft_reset
* Returned Value : USB_OK or error code
* Comments       :
*     Flush buffers, reset BULK IN & BULK OUT, clear all stalls.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_soft_reset
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] callback for completion/status */
      tr_callback                callback,
      
      /* [IN] user parameter returned by callback */
      pointer                    call_parm     
   )
{ /* Body */

   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_soft_reset");
   #endif

   if (USB_OK != (error = usb_printer_class_verify 
      (cc_ptr, &tr, callback, call_parm)))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_printer_soft_reset, error class verify failed");
      #endif
      return error;
   } /* Endif */

   tr.G.RX_BUFFER = NULL;
   tr.G.RX_LENGTH = 0;
   tr.DEV_REQ_PTR = (uchar_ptr)(&dev_req);
   dev_req.BMREQUESTTYPE = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE;
   dev_req.BREQUEST = USB_PRINTER_SOFT_RESET;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_soft_reset, SUCCESSFUL");
   #endif
   return usb_printer_class_send (cc_ptr, &tr);   

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_init
* Returned Value : none
* Comments       :
*     Initialize printer-class interface info struct.
* 
*END*--------------------------------------------------------------------*/

void usb_printer_init
   (
      /* [IN] device/descriptor/pipe handles */
      PIPE_BUNDLE_STRUCT_PTR  pbs_ptr,
      
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR   prt_call_ptr
   )
{ /* Body */

   PRINTER_INTERFACE_STRUCT_PTR  prt_intf;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_init");
   #endif

   /* Pointer validity-checking, clear memory, init header */
   prt_intf = prt_call_ptr->class_intf_handle;
   if (USB_OK != usb_host_class_intf_init (pbs_ptr, (pointer)prt_intf, (pointer)&prt_anchor, NULL))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_printer_init, error class init");
      #endif
      return;
   }

   USB_lock();
   prt_call_ptr->code_key = 0;
   prt_call_ptr->code_key = usb_host_class_intf_validate(prt_call_ptr);

   if (NULL == 
         (prt_intf->control_pipe = usb_hostdev_get_pipe_handle 
         (pbs_ptr, USB_CONTROL_PIPE, 0) ))
      goto Bad_Exit;

   if (NULL == 
         (prt_intf->bulk_out_pipe = usb_hostdev_get_pipe_handle 
         (pbs_ptr, USB_BULK_PIPE, USB_SEND) ))
      goto Bad_Exit;

   /* Uni-directional printers don't have a bulk-in pipe */
   if (((INTERFACE_DESCRIPTOR_PTR)prt_intf->intf_handle)->
         bInterfaceSubClass > USB_PROTOCOL_PRT_UNIDIR)
      {
      if (NULL ==
            (prt_intf->bulk_in_pipe = usb_hostdev_get_pipe_handle 
            (pbs_ptr, USB_BULK_PIPE, USB_RECV) ))
         goto Bad_Exit;
      } /* EndIf */

   USB_unlock ();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_init, SUCCESSFUL");
   #endif
   
   return;  /* Good exit, interface struct initialized */

   Bad_Exit:
   USB_mem_zero (prt_intf,sizeof(PRINTER_INTERFACE_STRUCT_PTR));
   prt_call_ptr->class_intf_handle = NULL;
   prt_call_ptr->code_key = 0;
   USB_unlock ();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_init, bad exit");
   #endif
   
} /* Endbody */
                  


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_recv_data
* Returned Value : none
* Comments       :
*     Receive printer data from a bulk-in pipe.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_recv_data 
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR   cc_ptr,
      
      /* [IN] callback upon completion */
      tr_callback             callback,
      
      /* [IN] user parameter returned by callback */
      pointer                 call_parm,
      
      /* [IN] data length */
      uint_32                 buf_size,
      
      /* [IN] buffer pointer */
      uchar_ptr               buffer
   )
{ /* Body */

   TR_INIT_PARAM_STRUCT    tr;
   USB_STATUS              error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_recv_data");
   #endif

   if (USB_OK != (error = usb_printer_class_verify 
         (cc_ptr, &tr, callback, call_parm)))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_printer_recv_data, error verifying class");
      #endif
      return error;

   }

   tr.G.RX_BUFFER = buffer;
   tr.G.RX_LENGTH = buf_size;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_recv_data, SUCCESSFUL");
   #endif
   return usb_printer_bulk_inout (cc_ptr, &tr, FALSE);   

} /* Endbody */
                  


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_printer_send_data
* Returned Value : none
* Comments       :
*     Send data to a printer through a bulk-out pipe.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_printer_send_data 
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR   cc_ptr,
      
      /* [IN] callback upon completion */
      tr_callback             callback,
      
      /* [IN] user parameter returned by callback */
      pointer                 call_parm,
      
      /* [IN] data length */
      uint_32                 buf_size,
      
      /* [IN] buffer pointer */
      uchar_ptr               buffer
   )
{ /* Body */

  TR_INIT_PARAM_STRUCT  tr;
  USB_STATUS            error;

  #ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_printer_send_data");
  #endif

   if (USB_OK != (error = usb_printer_class_verify 
         (cc_ptr, &tr, callback, call_parm)))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_printer_send_data, error verifying class");
      #endif
      return error;
   }

   tr.G.TX_BUFFER = buffer;
   tr.G.TX_LENGTH = buf_size;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_printer_send_data, SUCCESSFUL");
   #endif

   return usb_printer_bulk_inout (cc_ptr, &tr, TRUE);   

} /* Endbody */
  
/* EOF */
