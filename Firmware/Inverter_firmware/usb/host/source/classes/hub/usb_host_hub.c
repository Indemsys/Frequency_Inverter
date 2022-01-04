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
* $FileName: usb_host_hub.c$
* $Version : 3.8.15.0$
* $Date    : Sep-13-2012$
*
* Comments:
*
*   This file contains the implementation of class driver for hub devices.
*
*END************************************************************************/
#include "usb_host_hub.h"
#include "usb_host_hub_prv.h"

static USB_HUB_CLASS_INTF_STRUCT_PTR hub_anchor = NULL;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/

void usb_class_hub_init
   (
      /* [IN] structure with USB pipe information on the interface */
      PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

      /* [IN] hub call struct pointer */
      CLASS_CALL_STRUCT_PTR       ccs_ptr
   )
{ /* Body */
   USB_STATUS                     status;
   USB_HUB_CLASS_INTF_STRUCT_PTR if_ptr = ccs_ptr->class_intf_handle;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_init");
   #endif
   
   /* Make sure the device is still attached */

   USB_lock(); //not needed as init is called from "already locked" function

   status = usb_host_class_intf_init(pbs_ptr, if_ptr, &hub_anchor, NULL);
   if (status == USB_OK) {
      /*
      ** We generate a code_key based on the attached device. This is used to
      ** verify that the device has not been detached and replaced with another.
      */
      ccs_ptr->code_key = 0;
      ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);

      /* Store application handle */
      if_ptr->APP = ccs_ptr;
         
      if_ptr->P_CONTROL = usb_hostdev_get_pipe_handle(pbs_ptr, USB_CONTROL_PIPE, 0);
      if_ptr->P_INT_IN = usb_hostdev_get_pipe_handle(pbs_ptr, USB_INTERRUPT_PIPE, USB_RECV);

   } /* Endif */

   /* Signal that an error has occured by setting the "code_key" to 0 */
   if (status || !if_ptr->P_INT_IN || !if_ptr->P_CONTROL) {
      ccs_ptr->code_key = 0;
   } /* Endif */

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_init, SUCCESSFUL");
   #endif
   
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_get_app
* Returned Value : CLASS_CALL_STRUCT_PTR if class found
* Comments       :
*     This function returns stored application handle as it was passed as
*     a param in select_interface.
*
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_hub_get_app
   (
      /* [IN] handle of device */
      _usb_device_instance_handle dev_ptr,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_ptr,

      /* [OUT] pointer to CLASS_CALL_STRUCT to be filled in */
      CLASS_CALL_STRUCT_PTR   _PTR_ ccs_ptr
   )
{
    USB_STATUS                    error;
    GENERAL_CLASS_PTR             parser;

    USB_lock();
 
    error = usb_hostdev_validate (dev_ptr);
    if (error != USB_OK) {
        USB_unlock();
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_class_mass_get_app, FAILED");
        #endif
        return USB_log_error(__FILE__,__LINE__,error);
    } /* EndIf */
    
    for (parser = (GENERAL_CLASS_PTR) hub_anchor; parser != NULL; parser = parser->next) {
        if (parser->dev_handle == dev_ptr && parser->intf_handle == intf_ptr)
            break;
    }
    
    if (parser != NULL) {
        USB_HUB_CLASS_INTF_STRUCT_PTR hub = (USB_HUB_CLASS_INTF_STRUCT_PTR) parser;
        *ccs_ptr = hub->APP;
    }
    else {
        USB_unlock();
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_class_hub_get_app, not found");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_NOT_FOUND);
    }
    
    USB_unlock();
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_hub_get_app, SUCCESSFUL");
    #endif
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_cntrl_callback
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This is the callback used when hub information is sent or received
*
*END*--------------------------------------------------------------------*/

void usb_class_hub_cntrl_callback
   (
      /* [IN] Unused */
      pointer     pipe,
      /* [IN] Pointer to the class interface instance */
      pointer     param,
      /* [IN] Data buffer */
      uchar_ptr   buffer,
      /* [IN] Length of buffer */
      uint_32     len,
      /* [IN] Error code (if any) */
      USB_STATUS  status
   )
{ /* Body */
   USB_HUB_CLASS_INTF_STRUCT_PTR      if_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_cntrl_callback");
   #endif

   /*
   ** There is no need for USB_lock in the callback function, and there is also no
   ** need to check if the device is still attached (otherwise this callback
   ** would never have been called!
   */

   /* Get class interface handle, reset IN_SETUP and call callback */
   if_ptr = (USB_HUB_CLASS_INTF_STRUCT_PTR)param;

   if_ptr->IN_SETUP = FALSE;

   if (if_ptr->CTRL_CALLBACK) {
      if_ptr->CTRL_CALLBACK(pipe, if_ptr->CTRL_PARAM, buffer, len, status);
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_cntrl_callback, SUCCESSFUL");
   #endif
   
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_cntrl_common
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is used to send a control request
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_cntrl_common
   (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR   ccs_ptr,
      /* [IN] The callback function */
      tr_callback             callback,
      /* [IN] The callback parameter */
      pointer                 param,
      /* [IN] Bitmask of the request type */
      uint_8                  bmrequesttype,
      /* [IN] Request code */
      uint_8                  brequest,
      /* [IN] Value to copy into WVALUE field of the REQUEST */
      uint_16                 wvalue,
      /* [IN] Length of the data associated with REQUEST */
      uint_16                 windex,
      /* [IN] Index field of CTRL packet */
      uint_16                 wlength
   )
{ /* Body */
   USB_HUB_CLASS_INTF_STRUCT_PTR    if_ptr;
   USB_SETUP                        req;
   USB_STATUS                       status = USBERR_NO_INTERFACE;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_cntrl_common");
   #endif
   
   USB_lock();
   /* Validity checking */
   if (usb_host_class_intf_validate(ccs_ptr)) {
      if_ptr =
         (USB_HUB_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
      status = usb_hostdev_validate(if_ptr->G.dev_handle);
   } /* Endif */

   if (!status && if_ptr->IN_SETUP) {
      status = USBERR_TRANSFER_IN_PROGRESS;
   } /* Endif */

   if (!status) {
      if (if_ptr->C_BUF != NULL && if_ptr->C_BUF_SIZE < wlength) {
         status = _usb_hostdev_free_buffer(if_ptr->G.dev_handle, if_ptr->C_BUF);
         if (!status) {
            /* Memory freed, assign NULL pointer */
            if_ptr->C_BUF = NULL;
         }
      }
      if (if_ptr->C_BUF == NULL) {
         /* Allocate new memory for control pipe request */
         status = _usb_hostdev_get_buffer(if_ptr->G.dev_handle, wlength, (pointer _PTR_)&if_ptr->C_BUF);
         if_ptr->C_BUF_SIZE = wlength;
      }
   }

   if (!status) {
      /* Save the higher level callback and ID */
      if_ptr->CTRL_CALLBACK = callback;
      if_ptr->CTRL_PARAM = param;

      /* Setup the request */
      req.BMREQUESTTYPE = bmrequesttype;
      req.BREQUEST = brequest;
      *(uint_16*)req.WVALUE = HOST_TO_LE_SHORT(wvalue);
      *(uint_16*)req.WINDEX = HOST_TO_LE_SHORT(windex);
      *(uint_16*)req.WLENGTH = HOST_TO_LE_SHORT(wlength);
      status = _usb_hostdev_cntrl_request(if_ptr->G.dev_handle, &req, (uchar_ptr) if_ptr->C_BUF,
         usb_class_hub_cntrl_callback, if_ptr);
      if (status == USB_STATUS_TRANSFER_QUEUED) {
         if_ptr->IN_SETUP = TRUE;
      } /* Endif */
   } /* Endif */
   USB_unlock();

   if (status != USB_STATUS_TRANSFER_QUEUED) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_hub_cntrl_common, FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__,status);
   }
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_cntrl_common, SUCCESSFUL");
   #endif

   return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_get_descriptor
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is called by the application to read the descriptor
*     of hub device
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_get_descriptor
   (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR   ccs_ptr,
      /* [IN] The callback function */
      tr_callback             callback,
      /* [IN] The callback parameter */
      pointer                 param,
      /* [IN] buffer length (how many bytes to read) */
      uchar                   len
   )
{ /* Body */
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_class_hub_get_descriptor");
   #endif

   USB_lock();

   error = usb_class_hub_cntrl_common(
      ccs_ptr, callback, param,
      REQ_TYPE_DEVICE | REQ_TYPE_IN | REQ_TYPE_CLASS,
      REQ_GET_DESCRIPTOR,
      0,
      0,
      len
   );

   USB_unlock();

   #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_class_hub_get_descriptor, SUCCESSFUL");
   #endif
   
   return error;

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_set_port_feature
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Sets feature of specified hub port
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_set_port_feature
   (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,
      /* [IN] The callback function */
      tr_callback                callback,
      /* [IN] The callback parameter */
      pointer                    param,
      /* [IN] port number */
      uint_8                     port_nr,
      /* [IN] feature ID */
      uint_8                     feature
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_set_port_feature, SUCCESSFUL");
   #endif

   return usb_class_hub_cntrl_common(
      ccs_ptr, callback, param,
      REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
      REQ_SET_FEATURE,
      feature,
      port_nr,
      0);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_clear_feature
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Clears hub feature
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_clear_feature
   (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,
      /* [IN] The callback function */
      tr_callback                callback,
      /* [IN] The callback parameter */
      pointer                    param,
      /* [IN] feature ID */
      uint_8                     feature
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_clear_port_feature, SUCCESSFUL");
   #endif

   return usb_class_hub_cntrl_common(
      ccs_ptr, callback, param,
      REQ_TYPE_OUT | REQ_TYPE_CLASS,
      REQ_CLEAR_FEATURE,
      feature,
      0,
      0);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_clear_port_feature
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Clears feature of selected hub port
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_clear_port_feature
   (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,
      /* [IN] The callback function */
      tr_callback                callback,
      /* [IN] The callback parameter */
      pointer                    param,
      /* [IN] port number */
      uint_8                     port_nr,
      /* [IN] feature ID */
      uint_8                     feature
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_clear_port_feature, SUCCESSFUL");
   #endif

   return usb_class_hub_cntrl_common(
      ccs_ptr, callback, param,
      REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
      REQ_CLEAR_FEATURE,
      feature,
      port_nr,
      0);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_get_status
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Gets hub status
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_get_status
   (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,
      /* [IN] The callback function */
      tr_callback                callback,
      /* [IN] The callback parameter */
      pointer                    param,
      /* [IN] buffer length (how many bytes to read) */
      uchar                      len
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_get_port_status, SUCCESSFUL");
   #endif

   return usb_class_hub_cntrl_common(
      ccs_ptr, callback, param,
      REQ_TYPE_IN | REQ_TYPE_CLASS,
      REQ_GET_STATUS,
      0,
      0,
      len);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_get_port_status
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Gets the status of specified port
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_get_port_status
   (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,
      /* [IN] The callback function */
      tr_callback                callback,
      /* [IN] The callback parameter */
      pointer                    param,
      /* [IN] port number */
      uint_8                     port_nr,
      /* [IN] buffer length (how many bytes to read) */
      uchar                      len
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_get_port_status, SUCCESSFUL");
   #endif

   return usb_class_hub_cntrl_common(
      ccs_ptr, callback, param,
      REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
      REQ_GET_STATUS,
      0,
      port_nr,
      len);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_wait_for_interrupt
* Returned Value : None
* Comments       :
*     Starts interrupt endpoint to poll for interrupt on specified hub
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_hub_wait_for_interrupt
    (
      /* [IN] The class call struct for application */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,
      /* [IN] The callback function */
      tr_callback                callback,
      /* [IN] The callback parameter */
      pointer                    param,
      /* size of buffer to be used */
      uchar                      len
    )
{ /* Body */
    USB_HUB_CLASS_INTF_STRUCT_PTR  if_ptr;
    TR_INIT_PARAM_STRUCT           tr;
    USB_STATUS                     status = USBERR_NO_INTERFACE;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_wait_for_interrupt");
   #endif

    USB_lock();
    /* Validity checking, always needed when passing data to lower API */
    if (usb_host_class_intf_validate(ccs_ptr)) {
        if_ptr = (USB_HUB_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
        status = usb_hostdev_validate(if_ptr->G.dev_handle);
    }

    if (!status && if_ptr->INT_PIPE) {
        status = USBERR_TRANSFER_IN_PROGRESS;
    }
    
    if (!status) {
        if (if_ptr->I_BUF != NULL && if_ptr->I_BUF_SIZE < len) {
            status = _usb_hostdev_free_buffer(if_ptr->G.dev_handle, if_ptr->I_BUF);
            if (!status) {
                /* Memory freed, assign NULL pointer */
                if_ptr->I_BUF = NULL;
            }
        }
        if (if_ptr->I_BUF == NULL) {
            /* Allocate new memory for control pipe request */
            status = _usb_hostdev_get_buffer(if_ptr->G.dev_handle, len, (pointer _PTR_) &if_ptr->I_BUF);
            if_ptr->I_BUF_SIZE = len;
        }
    }

    if (!status) {
        usb_hostdev_tr_init(&tr, usb_class_hub_int_callback, (pointer) if_ptr);
        tr.G.RX_BUFFER = if_ptr->I_BUF;
        tr.G.RX_LENGTH = len;
        status = _usb_host_recv_data(if_ptr->G.host_handle, if_ptr->P_INT_IN, &tr);
        if_ptr->INT_PIPE = 1; /* mark we are using interrupt pipe */
        if_ptr->INT_CALLBACK = callback;
        if_ptr->INT_PARAM = param;
   }

   USB_unlock();

   if (status != USB_STATUS_TRANSFER_QUEUED) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_hub_wait_for_interrupt, FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__,status);
   }
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_wait_for_interrupt, SUCCESSFUL");
   #endif

   return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_cntrl_callback
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This is the callback used when hub information is sent or received
*
*END*--------------------------------------------------------------------*/
void usb_class_hub_int_callback
   (
      /* [IN] Unused */
      pointer     pipe,
      /* [IN] Pointer to the class interface instance */
      pointer     param,
      /* [IN] Data buffer */
      uchar_ptr   buffer,
      /* [IN] Length of buffer */
      uint_32     len,
      /* [IN] Error code (if any) */
      USB_STATUS  status
   )
{ /* Body */
   USB_HUB_CLASS_INTF_STRUCT_PTR      if_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_int_callback");
   #endif

   /*
   ** There is no need for USB_lock in the callback function, and there is also no
   ** need to check if the device is still attached (otherwise this callback
   ** would never have been called!
   */

   /* Get class interface handle, reset IN_SETUP and call callback */
   if_ptr = (USB_HUB_CLASS_INTF_STRUCT_PTR)param;

   if_ptr->INT_PIPE = 0; /* mark we are not using interrupt pipe */

   if (if_ptr->INT_CALLBACK) {
      if_ptr->INT_CALLBACK(pipe, if_ptr->INT_PARAM, buffer, len, status);
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hub_int_callback, SUCCESSFUL");
   #endif
   
} /* Endbody */



/* EOF */
