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
* $FileName: host_ch9.c$
* $Version : 3.8.23.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains standard device requests (USB Chapter 9)
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_ch9.h"
#include "host_main.h"
#include "host_dev_list.h"

/* Prototype, copy to stack frames for each use instance */

static const USB_SETUP req_prototype = 
{
   0,          /* request type */ 
   0,          /* request */
   {0,0},      /* WVALUE  */
   {0,0},      /* WINDEX  */
   {0,0}       /* WLENGTH  */
};

/* Unpublished functions, not intended for application use */
static void  usb_host_cntrl_transaction_done(_usb_pipe_handle, pointer, uchar_ptr, uint_32, USB_STATUS);

static USB_STATUS  usb_host_ch9_dev_req(_usb_device_instance_handle, USB_SETUP_PTR, uchar_ptr);
 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_cntrl_transaction_done
* Returned Value : none
* Comments       :
*     Callback function to process transaction-done events
*     State machine for enumeration/subsequent transactions
*     Between states, the 8-byte buffer in device_instance
*        is used to save the first part of the configuration.
*     Pointer desc in various flavors (uchar_ptr, cfig_ptr)
*        is used to parse this buffer in various ways. 
* 
*END*--------------------------------------------------------------------*/
static void  usb_host_cntrl_transaction_done
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle  handle,

      /* [IN] The user parameter */
      pointer           user_parm,

      /* [IN] the buffer */
      uchar_ptr         buffer,

      /* [IN] The length of the transfer */
      uint_32           length,

      /* [IN] status of the transfer */
      USB_STATUS        status
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   PIPE_STRUCT_PTR            pipe_ptr = (PIPE_STRUCT_PTR)handle;
   DEV_INSTANCE_PTR           dev_inst_ptr = (DEV_INSTANCE_PTR)pipe_ptr->DEV_INSTANCE;
   DESCRIPTOR_UNION           desc;
   int_32                     config_size;
   DEV_MEMORY_PTR             dev_mem;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_cntrl_transaction_done");
   #endif

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)dev_inst_ptr->host;

   switch (status) {
      case USB_OK:
         dev_inst_ptr->ctrl_retries = USBCFG_CTRL_RETRY;
         break;

      case USBERR_TR_FAILED:
         if (buffer != NULL) {
            dev_inst_ptr->ctrl_retries--;
            if (dev_inst_ptr->ctrl_retries) {
               if (dev_inst_ptr->state == DEVSTATE_DEVDESC8) {
                  /* dont jump to DEVSTATE_INITIAL state, get descriptor instead */
                  status = _usb_host_ch9_get_descriptor((_usb_device_instance_handle)dev_inst_ptr,
                     USB_DESC_TYPE_DEV << 8, 0, 8, 
                     (uchar_ptr)buffer);
                  return;
               }
               else {
                  dev_inst_ptr->state--; /* back to previous enum state */
                  status = USB_OK;
                  //_time_delay(100);
               }
            }
         } else
            dev_inst_ptr->state = DEVSTATE_INITIAL;
         break;

      case USBERR_ENDPOINT_STALLED:
         dev_inst_ptr->ctrl_retries--;
         if (dev_inst_ptr->ctrl_retries) {
            status = _usb_host_ch9_clear_feature((_usb_device_instance_handle)dev_inst_ptr, REQ_TYPE_ENDPOINT, 0, ENDPOINT_HALT);
            if (status == USB_OK) {
                //_time_delay(100);
			}
         }
         break;

      default:
         dev_inst_ptr->state = DEVSTATE_INITIAL;
         break;
   }

   if (status != USB_OK)
      return;

   /*----------------------------------------------------**
   ** Enumeration state machine -- cases are named after **
   ** the just-completed transactions.                   **
   **----------------------------------------------------*/
   
   switch (dev_inst_ptr->state) {
      case DEVSTATE_INITIAL:      /* initial device state = forever error state */
         break;   
      case DEVSTATE_DEVDESC8:     /* device descriptor [0..7]*/
         /* We must have received the first 8 bytes in 
         ** dev_inst_ptr->dev_descriptor which contains the 
         ** max packet size for this control endpoint 
         */
         pipe_ptr->MAX_PACKET_SIZE = 
            dev_inst_ptr->dev_descriptor.bMaxPacketSize;
            
         /* Notify device driver of MaxPacketSize0 for this device */
         status = _usb_host_update_max_packet_size_call_interface (usb_host_ptr, pipe_ptr);
         
         if (status != USB_OK)
         {
            dev_inst_ptr->state = DEVSTATE_INITIAL;
            break;
         }
         else
         {
            dev_inst_ptr->state = DEVSTATE_ADDR_SET;
         }

         /* Now set the address that we assigned when we initialized 
         ** the device instance struct for this device 
         */
         status = _usb_host_ch9_set_address((_usb_device_instance_handle)dev_inst_ptr);
         
         if (status != USB_STATUS_TRANSFER_QUEUED)
         {
            dev_inst_ptr->state = DEVSTATE_INITIAL;
            break;
         }

         break;
      case DEVSTATE_ADDR_SET:     /* address set */
         pipe_ptr->DEVICE_ADDRESS = dev_inst_ptr->address;
         
         /* Notify device driver of USB device's new address */
         status = _usb_host_update_device_address_call_interface (usb_host_ptr, pipe_ptr);
         
         if (status != USB_OK)
         {
            dev_inst_ptr->state = DEVSTATE_INITIAL;
            break;
         }

         dev_inst_ptr->state = DEVSTATE_DEV_DESC;

         /* Now get the full descriptor */
         status = _usb_host_ch9_get_descriptor((_usb_device_instance_handle)dev_inst_ptr,
            USB_DESC_TYPE_DEV << 8, 
            0, 
            USB_DESC_LEN_DEV,
            (uchar_ptr)&dev_inst_ptr->dev_descriptor);
            
         if (status != USB_STATUS_TRANSFER_QUEUED)
         {
            dev_inst_ptr->state = DEVSTATE_INITIAL;
            break;
         }

         break;   
      case DEVSTATE_DEV_DESC:     /* full device descriptor received */
         /* Now lets get the first 9 bytes of the configuration descriptor 
         */
         desc.pntr = &dev_inst_ptr->buffer;
         dev_inst_ptr->state = DEVSTATE_GET_CFG9;
         status = _usb_host_ch9_get_descriptor((_usb_device_instance_handle)dev_inst_ptr,
            USB_DESC_TYPE_CFG << 8 | dev_inst_ptr->cfg_value, 
            0, 
            sizeof(dev_inst_ptr->buffer),
            desc.bufr);
            
         if (status != USB_STATUS_TRANSFER_QUEUED)
         {
            dev_inst_ptr->state = DEVSTATE_INITIAL;
            break;
         }

         break;
      case DEVSTATE_GET_CFG9:     /* Read 9 bytes of config descriptor */
         dev_inst_ptr->state = DEVSTATE_SET_CFG;
         /* Now select the configuration as specified in the 
         ** descriptor 
         */
         desc.cfig = (CONFIGURATION_DESCRIPTOR_PTR)dev_inst_ptr->buffer;
         config_size = SHORT_UNALIGNED_LE_TO_HOST(desc.cfig->wTotalLength);

         desc.pntr = &dev_inst_ptr->buffer;
         if (USB_OK != usb_dev_list_get_mem(dev_inst_ptr,
            config_size,
            USB_MEMTYPE_CONFIG,
            1,
            &dev_mem))
         {
            #ifdef _HOST_DEBUG_
               DEBUG_LOG_TRACE("usb_host_cntrl_transaction_done");
            #endif
            return;
         }
         /* Move the pointer to the aligned payload */
         desc.pntr = dev_mem->payload.data + dev_mem->offset;
         
#ifdef __USB_OTG__
         dev_inst_ptr->state = DEVSTATE_CHK_OTG;
#else
         dev_inst_ptr->state = DEVSTATE_SET_CFG;
#endif
         /* We can only read one config descriptor at a time */
         status = _usb_host_ch9_get_descriptor((_usb_device_instance_handle)dev_inst_ptr,
            (USB_DESC_TYPE_CFG << 8) | dev_inst_ptr->cfg_value, 
            0, 
            (uint_16)config_size,
            desc.bufr);
            
         if (status != USB_STATUS_TRANSFER_QUEUED)
         {
            dev_inst_ptr->state = DEVSTATE_INITIAL;
            break;
         }

         break;   

      case DEVSTATE_CHK_OTG:
         /* Point to the memory owned by this device */
         /* FIXME: it is presumed that memlist points to the config descriptor */
         desc.pntr = dev_inst_ptr->memlist->payload.data + dev_inst_ptr->memlist->offset;

         /* We will always start with config desc so update the search pointer */
         config_size = SHORT_UNALIGNED_LE_TO_HOST(desc.cfig->wTotalLength);
         config_size -= desc.cfig->bLength;
         desc.word += desc.cfig->bLength;

         while (config_size) {
            if (desc.otg->bDescriptorType == USB_DESC_TYPE_OTG) {
               /* Found the OTG descriptor */
               break;
            } /* Endif */
            config_size -= desc.intf->bLength;
            desc.word += desc.intf->bLength;
         } /* EndWhile */

         /* Check for an OTG descriptor */
         dev_inst_ptr->state = DEVSTATE_SET_CFG;

         if (config_size && desc.otg->bDescriptorType == USB_DESC_TYPE_OTG &&
            (desc.otg->bmAttributes & OTG_HNP_SUPPORT))
         {
            status = _usb_host_ch9_set_feature(dev_inst_ptr, 0, 0, 4);
            
            if (status != USB_STATUS_TRANSFER_QUEUED)
            {
               dev_inst_ptr->state = DEVSTATE_INITIAL;
               break;
            }
            else
            {
               break;
            }

         } /* Endif */

         /* Fall through */

      case DEVSTATE_SET_CFG:     /* config descriptor [0..8] */

         /* Point to the memory owned by this device */
         /* FIXME: it is presumed that memlist points to the config descriptor */
         desc.pntr = dev_inst_ptr->memlist->payload.data + dev_inst_ptr->memlist->offset;

         dev_inst_ptr->state = DEVSTATE_CFG_READ;
         dev_inst_ptr->cfg_value = desc.cfig->bConfigurationValue;
         status = _usb_host_ch9_set_configuration(dev_inst_ptr, dev_inst_ptr->cfg_value);
            
         if (status != USB_STATUS_TRANSFER_QUEUED)
         {
            dev_inst_ptr->state = DEVSTATE_INITIAL;
            break;
         }

         break;   

      case DEVSTATE_CFG_READ:     /* full config desc. read in */
         dev_inst_ptr->state = DEVSTATE_APP_CALL;

         /* Scan the configuration descriptor to find out the total 
         ** number of interfaces available. This will be the upper 
         ** bound for searching through the interface descriptors' 
         ** array
         */
         dev_inst_ptr->num_of_interfaces = 0;

         /* Point to the memory owned by this device */
         /* FIXME: it is presumed that memlist points to the config descriptor */
         desc.pntr = dev_inst_ptr->memlist->payload.data + dev_inst_ptr->memlist->offset;

         /* We will always start with config desc so update the search pointer */
         config_size = SHORT_UNALIGNED_LE_TO_HOST(desc.cfig->wTotalLength);
         config_size -= desc.cfig->bLength;
         desc.word += desc.cfig->bLength;
         
         while (config_size > 0) {
            if (desc.intf->bDescriptorType == USB_DESC_TYPE_IF) {
               /* Found an interface */
               dev_inst_ptr->num_of_interfaces++;
            } /* Endif */
            if (desc.intf->bLength) {
               config_size -= desc.intf->bLength;
               desc.word += desc.intf->bLength;
            } else {
               /* Zero-sized interface found */
               status = USBERR_INVALID_CFIG_NUM;
               break;
            }
         } /* EndWhile */
         if (config_size < 0) {
            /* Error: we have not read the configuration descriptor properly,
            ** some part is missing. Symptom: there is not config descriptor
            ** info left for latest descriptor.
            */
            status = USBERR_INVALID_CFIG_NUM;
         }
         if (status == USBERR_INVALID_CFIG_NUM) {
            /* TODO: Remove config descriptor from memlist and try to re-read. */
            break;
         }

         /* Don't select an interface here. The device driver will 
         ** select the interface 
         */

      case DEVSTATE_APP_CALL:     /* full config desc. read in */
         dev_inst_ptr->state = DEVSTATE_SET_INTF;
         if (dev_inst_ptr->new_config != 0) {
            /* We have just read a new configuration descriptor */
            dev_inst_ptr->new_config = 0;
            usb_hostdev_attach_detach(dev_inst_ptr, USB_CONFIG_EVENT);
         } else {
            usb_hostdev_attach_detach(dev_inst_ptr, USB_ATTACH_EVENT);
         } /* EndIf */
         break;

      case DEVSTATE_SET_INTF:    /* Select interface done */
         dev_inst_ptr->state = DEVSTATE_ENUM_OK;
         usb_hostdev_attach_detach(dev_inst_ptr, USB_INTF_EVENT);
         break;
      default:
         dev_inst_ptr->state = DEVSTATE_ENUM_OK;
         case DEVSTATE_ENUM_OK:   /* enumeration complete */
            if ((dev_inst_ptr->control_callback != NULL))
               dev_inst_ptr->control_callback
                  (handle,
                  user_parm,
                  buffer,
                  length,
                  status);
            break;   
      } /* EndSwitch */

   #ifdef _HOST_DEBUG_
   if (dev_inst_ptr->state == DEVSTATE_INITIAL)
   {
      DEBUG_LOG_TRACE("usb_host_cntrl_transaction_done FAILED");
   }
   else
   {
      DEBUG_LOG_TRACE("usb_host_cntrl_transaction_done SUCCESSFUL");
   }
   #endif
      
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_register_ch9_callback
* Returned Value : USB_OK, or error status
* Comments       :
*     This function registers a callback function that will be called 
*  to notify the user of a ch9 command completion. This should be used 
*  only after enumeration is completed
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_register_ch9_callback
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* Callback upon completion */
      tr_callback                   callback,

      /* User provided callback param */
      pointer                       callback_param
   )
{ /* Body */
   DEV_INSTANCE_PTR           dev_inst_ptr;
   USB_STATUS                 error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_register_ch9_callback");
   #endif
   /* Verify that device handle is valid */
   USB_lock();   
   error = usb_hostdev_validate(dev_handle);

   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_register_ch9_callback, device not found");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   } /* Endif */
     
   dev_inst_ptr = (DEV_INSTANCE_PTR)dev_handle;

   /* This will be called if the device is already enumerated */
   dev_inst_ptr->control_callback = callback;
   dev_inst_ptr->control_callback_param = callback_param;

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_register_ch9_callback, SUCCESSFUL");
   #endif
   return USB_OK;

} /* EndBody */
    
    
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_host_ch9_dev_req
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device requests in Chapter 9.
*        See Table 9-3 p. 250 of USB 2.0 specification.
*     This code does minimal error checking, on the assumption
*        that it is called only from wrappers in this file.
*     It is presumed that this function is called with USB interrupts disabled
* 
*END*--------------------------------------------------------------------*/
static USB_STATUS  usb_host_ch9_dev_req
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,
         
      /* Device Request to send */
      USB_SETUP_PTR                 devreq_ptr,
         
      /* buffer to send/receive */
      uchar_ptr                     buff_ptr
   )
{ /* Body */
   DEV_INSTANCE_PTR           dev_ptr;
   _usb_pipe_handle           pipe_handle;
   TR_INIT_PARAM_STRUCT       tr;
   USB_STATUS                 error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_ch9_dev_req");
   #endif
   
   /* Verify that device handle is valid */
   error = usb_hostdev_validate(dev_handle);

   if (error != USB_OK) 
   {

      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_ch9_dev_req device not found");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   } /* Endif */
     
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   pipe_handle = dev_ptr->control_pipe;

   /* Initialize the TR with TR index and default control callback 
   ** function if there is one registered 
   */
   usb_hostdev_tr_init(&tr, dev_ptr->control_callback, dev_ptr->control_callback_param);
   
   /* Set buffer length if required */
   switch (devreq_ptr->BREQUEST) {
      case REQ_SET_DESCRIPTOR:
         tr.G.TX_BUFFER = buff_ptr;
         tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(devreq_ptr->WLENGTH);
         break;
      case REQ_GET_CONFIGURATION:
      case REQ_GET_DESCRIPTOR:
      case REQ_GET_INTERFACE:
      case REQ_GET_STATUS:
      case REQ_SYNCH_FRAME:
         tr.G.RX_BUFFER = buff_ptr;
         tr.G.RX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(devreq_ptr->WLENGTH);
         break;
   } /* EndSwitch */

   tr.DEV_REQ_PTR = (uchar_ptr)devreq_ptr;

   if ((dev_ptr->state < DEVSTATE_ENUM_OK) ||
      (tr.G.CALLBACK == NULL))
   {
      tr.G.CALLBACK = usb_host_cntrl_transaction_done;
      tr.G.CALLBACK_PARAM = NULL;
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_ch9_dev_req SUCCESSFUL");
   #endif
   error = _usb_host_send_setup(dev_ptr->host, pipe_handle, &tr);
   
   return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_clear_feature
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_clear_feature
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* request type device/interface/endpoint */
      uint_8                        req_type,

      /* device = 0, or interface/endpoint */
      uint_8                        intf_endpt,

      /* feature selection */
      uint_16                       feature
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_clear_feature");
   #endif

   USB_lock();
   
   switch (req_type) {
      case REQ_TYPE_DEVICE:
         break;
      case REQ_TYPE_INTERFACE:
      case REQ_TYPE_ENDPOINT:
         *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(intf_endpt);
         break;
      default:
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_ch9_clear_feature, invalid request");
         #endif
         return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_BMREQ_TYPE);
   } /* EndSwitch */

   request.BMREQUESTTYPE = (uchar)(req_type | REQ_TYPE_OUT);
   request.BREQUEST = REQ_CLEAR_FEATURE;
   *(uint_16*)request.WVALUE = HOST_TO_LE_SHORT(feature);

   error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_clear_feature, SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_configuration
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_get_configuration
   (
      /* [IN] usb device */
      _usb_device_instance_handle   dev_handle,

      /* [OUT] configuration value */
      uchar_ptr                     buffer
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_configuration");
   #endif
   
   USB_lock();

   request.BMREQUESTTYPE = REQ_TYPE_DEVICE | REQ_TYPE_IN;
   request.BREQUEST = REQ_GET_CONFIGURATION;
   *(uint_16*)request.WLENGTH = HOST_TO_LE_SHORT_CONST(1);

   error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_configuration, SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_descriptor
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_get_descriptor
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,
      /* descriptor type & index */
      uint_16                       type_index,
      /* Language ID or 0 */
      uint_16                       lang_id,
      /* buffer length */
      uint_16                       buflen,         
      /* descriptor buffer */
      uchar_ptr                     buffer
   )
{ /* Body */

   USB_SETUP  request;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_descriptor");
   #endif

   USB_lock();

   request.BMREQUESTTYPE = REQ_TYPE_DEVICE | REQ_TYPE_IN;
   request.BREQUEST = REQ_GET_DESCRIPTOR;
   *(uint_16*)request.WVALUE = HOST_TO_LE_SHORT(type_index);
   *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(lang_id);
   *(uint_16*)request.WLENGTH = HOST_TO_LE_SHORT(buflen);

   error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_descriptor, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_interface
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_get_interface
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* interface index */
      uint_8            interface,

      /* alternate setting buffer */
      uchar_ptr         buffer
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_interface");
   #endif

   USB_lock();

   request.BMREQUESTTYPE = REQ_TYPE_INTERFACE | REQ_TYPE_IN;
   request.BREQUEST = REQ_GET_INTERFACE;
   *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(interface);
   *(uint_16*)request.WLENGTH = HOST_TO_LE_SHORT_CONST(1);

   error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_interface, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_get_status
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_get_status
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* request type device/interface/endpoint */
      uint_8            req_type,

      /* device = 0, or interface/endpoint */
      uint_8            intf_endpt,

      /* returned status */
      uchar_ptr         buffer
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_status");
   #endif

   USB_lock();

   switch (req_type) {
      case REQ_TYPE_DEVICE:
      case REQ_TYPE_INTERFACE:
      case REQ_TYPE_ENDPOINT:
         break;
      default:
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_ch9_get_status, invalid parameter");
         #endif
         return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_BMREQ_TYPE);
   } /* EndSwitch */

   request.BMREQUESTTYPE = (uchar)(req_type | REQ_TYPE_IN);
   request.BREQUEST = REQ_GET_STATUS;
   *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(intf_endpt);
   *(uint_16*)request.WLENGTH = HOST_TO_LE_SHORT_CONST(2);

   error = usb_host_ch9_dev_req(dev_handle, &request, buffer);
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_get_status, SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_address
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_set_address
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle
   )
{ /* Body */

   DEV_INSTANCE_PTR  dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_address");
   #endif

   USB_lock();

   request.BMREQUESTTYPE = REQ_TYPE_DEVICE | REQ_TYPE_OUT;
   request.BREQUEST = REQ_SET_ADDRESS;
   *(uint_16*)request.WVALUE = HOST_TO_LE_SHORT(dev_ptr->address);

   error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_address,SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_configuration
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_set_configuration
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* configuration value */
      uint_16                       config
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_configuration");
   #endif

   USB_lock();

   request.BMREQUESTTYPE = REQ_TYPE_DEVICE | REQ_TYPE_OUT;
   request.BREQUEST = REQ_SET_CONFIGURATION;
   *(uint_16*)request.WVALUE = HOST_TO_LE_SHORT(config);

   error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

   USB_unlock();
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_configuration SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_descriptor
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_set_descriptor
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* descriptor type & index */
      uint_16           type_index,

      /* Language ID or 0 */
      uint_16           lang_id,

      /* buffer length */
      uint_16           buflen,         

      /* descriptor buffer */
      uchar_ptr         buffer
   )
{ /* Body */

   USB_SETUP  request;
   USB_STATUS error = USB_OK;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_descriptor");
   #endif

   USB_lock();

   request.BMREQUESTTYPE = REQ_TYPE_DEVICE | REQ_TYPE_OUT;
   request.BREQUEST = REQ_SET_DESCRIPTOR;
   *(uint_16*)request.WVALUE = HOST_TO_LE_SHORT(type_index);
   *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(lang_id);
   *(uint_16*)request.WLENGTH = HOST_TO_LE_SHORT(buflen);

   error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_descriptor SUCCESSFUL");
   #endif

   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_feature
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_set_feature
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* request type device/interface/endpoint */
      uint_8            req_type,

      /* device = 0, or interface/endpoint */
      uint_8            intf_endpt,

      /* feature selection */
      uint_16           feature
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_feature");
   #endif

   USB_lock();
   switch (req_type) {
      case REQ_TYPE_DEVICE:
         break;
      case REQ_TYPE_INTERFACE:
      case REQ_TYPE_ENDPOINT:
         *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(intf_endpt);
         break;
      default:
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_ch9_set_feature, invalid param");
         #endif
         return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_BMREQ_TYPE);
   } /* EndSwitch */

   request.BMREQUESTTYPE = (uchar)(req_type | REQ_TYPE_OUT);
   request.BREQUEST = REQ_SET_FEATURE;
   *(uint_16*)request.WVALUE = HOST_TO_LE_SHORT(feature);

   error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_feature, SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_set_interface
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_set_interface
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* alternate setting */
      uint_8            alternate,

      /* interface */
      uint_8            intf
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_interface");
   #endif

   USB_lock();   

   request.BMREQUESTTYPE = REQ_TYPE_INTERFACE | REQ_TYPE_OUT;
   request.BREQUEST = REQ_SET_INTERFACE;
   *(uint_16*)request.WVALUE = HOST_TO_LE_SHORT(alternate);
   *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(intf);

   error = usb_host_ch9_dev_req(dev_handle, &request, NULL);

   USB_unlock();
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_set_interface, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_ch9_synch_frame
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process standard device request in Chapter 9.
*     See Table 9-3 p. 250 of USB 2.0 specification.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_ch9_synch_frame
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* interface index */
      uint_8            interface,

      /* configuration buffer */
      uchar_ptr         buffer
   )
{ /* Body */

   USB_SETUP  request = req_prototype;
   USB_STATUS error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_synch_frame");
   #endif

   USB_lock();

   request.BMREQUESTTYPE = REQ_TYPE_ENDPOINT | REQ_TYPE_IN;
   request.BREQUEST = REQ_SYNCH_FRAME;
   *(uint_16*)request.WINDEX = HOST_TO_LE_SHORT(interface);
   *(uint_16*)request.WLENGTH = HOST_TO_LE_SHORT_CONST(2);

   error = usb_host_ch9_dev_req(dev_handle, &request, buffer);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_ch9_synch_frame, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_cntrl_request
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to process class- or vendor-specific control pipe device
*     requests.
*
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_hostdev_cntrl_request
   (
      /* usb device */
      _usb_device_instance_handle   dev_handle,

      /* Device Request to send */
      USB_SETUP_PTR                 devreq,

      /* buffer to send/receive */
      uchar_ptr                     buff_ptr,

      /* callback upon completion */
      tr_callback                   callback,

      /* [IN] the parameter to pass back to the callback function */
      pointer                       callback_param
   )
{ /* Body */

   DEV_INSTANCE_PTR           dev_ptr;
   _usb_pipe_handle           pipe_handle;
   TR_INIT_PARAM_STRUCT       tr;
   USB_STATUS                 error = USB_OK;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request");
   #endif
   
   /* Verify that device handle is valid */
   USB_lock();
   error = usb_hostdev_validate(dev_handle);

   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request, invalid device handle");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   } /* Endif */
      

   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

   if (dev_ptr->state < DEVSTATE_ENUM_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request, no device found");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   } /* Endif */

   pipe_handle = dev_ptr->control_pipe;

   usb_hostdev_tr_init(&tr, callback, callback_param);

   /* Set TR buffer length as required */
   if ((REQ_TYPE_IN & devreq->BMREQUESTTYPE) != 0) {
      tr.G.RX_BUFFER = buff_ptr;
      tr.G.RX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(devreq->WLENGTH);
   } else {
      tr.G.TX_BUFFER = buff_ptr;
      tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(devreq->WLENGTH);
   } /* EndIf */

   tr.DEV_REQ_PTR = (uchar_ptr)devreq;

   error = _usb_host_send_setup(dev_ptr->host, pipe_handle, &tr);

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request,SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */


/* EOF */
