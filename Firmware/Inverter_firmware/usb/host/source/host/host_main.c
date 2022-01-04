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
* $FileName: host_main.c$
* $Version : 3.8.30.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the USB Host API specific main functions.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_common.h"
#include "host_main.h"
#include "host_dev_list.h"

static TR_STRUCT_PTR _usb_host_new_tr_element (_usb_pipe_handle);

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_init_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_init_call_interface routine is used to initialize device
*  drivers when the USB host stack is initialized.  Device drivers may
*  perform device register, interrupt, and internal data structure
*  initialization.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_init_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
#ifdef USB_HOST_INCLUDE_TASK_SUPPORT
   error = _usb_rtos_host_init ();
   
   if (error != USB_OK)
   {
      return USB_log_error(__FILE__,__LINE__,error);
   }
#endif
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_INIT != NULL)
   {
      error = temp_ptr->HOST_INIT (handle);
   }

#ifdef USB_HOST_INCLUDE_TASK_SUPPORT
   if (error != USB_OK)
   {
      error = _usb_rtos_host_shutdown();
   }
#endif

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_shutdown_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_shutdown_call_interface routine is used to shutdown device
*  drivers when the USB host stack is shutdown.  Device drivers may
*  perform host controller resets and/or free internal data structures.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_shutdown_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_SHUTDOWN != NULL)
   {
      error = temp_ptr->HOST_SHUTDOWN (handle);
   }
   
#ifdef USB_HOST_INCLUDE_TASK_SUPPORT
   if (error == USB_OK)
   {
      error = _usb_rtos_host_shutdown ();
   }
#endif

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_send_call_interface routine is used to send non-control data
*  to a USB device.  Device drivers may allocate internal data structures
*  and modify registers to initiate a bus transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_send_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct tr_struct     _PTR_ tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_SEND != NULL)
   {
      error = temp_ptr->HOST_SEND (handle, pipe_ptr, tr_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_setup_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_send_setup_call_interface routine is used to send control
*  data to a USB device.  Device drivers may allocate internal data
*  structures and modify registers to initiate a bus transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_send_setup_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct tr_struct     _PTR_ tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_SEND_SETUP != NULL)
   {
      error = temp_ptr->HOST_SEND_SETUP (handle, pipe_ptr, tr_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_recv_data_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_recv_data_call_interface routine is used to receive non-
*  control data from a USB device.  Device drivers may allocate internal
*  data structures and modify registers to initiate a bus transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_recv_data_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct tr_struct     _PTR_ tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_RECV != NULL)
   {
      error = temp_ptr->HOST_RECV (handle, pipe_ptr, tr_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_cancel_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_cancel_call_interface routine is used to cancel pending
*  transactions.  Device drivers may update/free internal data structures
* and/or modify registers to cancel the transaction.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_cancel_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr,
   
   /* [IN] Pipe transfer descriptor */
   struct tr_struct     _PTR_ tr_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_CANCEL != NULL)
   {
      error = temp_ptr->HOST_CANCEL (handle, pipe_ptr, tr_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_bus_control_data_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_bus_control_data_call_interface routine is used to perform
*  low-level operations on the USB bus (e.g. asserting/deasserting resume).
*  Device drivers may modify device registers to accomplish the requested
*  operation.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_bus_control_data_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle              handle,
   
   /* [IN] The operation to be performed on the bus */
   uint_8                        bcontrol
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_BUS_CONTROL != NULL)
   {
      error = temp_ptr->HOST_BUS_CONTROL (handle, bcontrol);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_alloc_bandwidth_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_alloc_bandwidth_call_interface routine is used to allocate
*  bandwidth for interrupt and isochronous transactions.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_alloc_bandwidth_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [IN] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_ALLOC_BANDWIDTH != NULL)
   {
      error = temp_ptr->HOST_ALLOC_BANDWIDTH (handle, pipe_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_close_pipe_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_close_pipe_call_interface routine is used to notify device drivers
*  when a pipe is closed.  Device drivers may reclaim internal data
*  structures allocated in _usb_host_open_pipe_call_interface when the pipe
*  was opened.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_close_pipe_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_FREE_CONTROLLER_RESOURCE != NULL)
   {
      error = temp_ptr->HOST_FREE_CONTROLLER_RESOURCE (handle, pipe_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_open_pipe_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_open_pipe_call_interface routine is used to notify device drivers
*  when a pipe is opened.  Device drivers may allocate internal data
*  structures and associate them with pipe descriptors for reclaimation
*  when the pipe is closed.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_open_pipe_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_OPEN_PIPE != NULL)
   {
      error = temp_ptr->HOST_OPEN_PIPE (handle, pipe_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_update_max_packet_size_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_update_max_packet_size_call_interface is used to notify device
*  drivers of the maximum packet size for a USB device.  Host controller
*  device drivers may update internal data structures associated with
*  open pipes on the device.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_update_max_packet_size_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_UPDATE_MAX_PACKET_SIZE != NULL)
   {
      error = temp_ptr->HOST_UPDATE_MAX_PACKET_SIZE (handle, pipe_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_update_device_address_call_interface
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_update_device_address_call_interface is used to notify device
*  drivers of the new device address for a USB device.  Host controller
*  device drivers may update internal data structures associated with
*  open pipes on the device.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_update_device_address_call_interface
(
   /* [IN] the USB Host state structure */
   _usb_host_handle     handle,
   
   /* [OUT] Pipe descriptor */
   struct pipe_struct   _PTR_ pipe_ptr
)
{
   USB_STATUS error = USB_OK;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR temp_ptr;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   temp_ptr = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR;

   if (temp_ptr->HOST_UPDATE_DEVICE_ADDRESS != NULL)
   {
      error = temp_ptr->HOST_UPDATE_DEVICE_ADDRESS (handle, pipe_ptr);
   }

   return USB_log_error(__FILE__,__LINE__,error);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initializes the USB hardware and installs the USB 
*  interrupt handler
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_host_init
   (
      /* [IN] address if the callback functions structure */
      struct usb_host_if_struct    *usb_if,
     
      /* [OUT] the USB host handle */
      _usb_host_handle _PTR_  handle
   )
{ /* Body */
   USB_STATUS error = USB_OK;
   USB_HOST_IF_STRUCT_PTR usb_c;
   USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR host_if;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = NULL;
   _mqx_int i;

#ifdef _HOST_DEBUG_
   DEBUG_LOG_TRACE("_usb_host_init");
#endif

   /* Check usb_if structure */
   if (usb_if == NULL) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init no interface given");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }
   if (usb_if->HOST_IF == NULL) {
      /* The interface does not support host functionality */
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init interface not supporting host");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }

   USB_lock();
   
   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);

   /* Check if the interface is registered */
   if (!usb_c) {
      /* No interface was registered yet */
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init no interface registered");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }
   else {
      /* Find out if the interface has been installed */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].HOST_INIT_PARAM == usb_if->HOST_INIT_PARAM) {
            /* Found installed device */
            break;
         }
      }
   }

   if (i == USBCFG_MAX_DRIVERS) {
      /* The interface has not been registered, so it cannot be initialized */
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init not installed");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }
   else {
      /* check if the registered interface was already initialized */
      if (usb_c[i].HOST_HANDLE != NULL) {
         /* The interface was already installed- return handle and USBERR_DEVICE_BUSY value */
         *handle = (_usb_host_handle) usb_c[i].HOST_HANDLE;
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_init already initialized - returning handle");
         #endif
         return USBERR_DEVICE_BUSY;
      }
   }

   host_if = (USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR) usb_c[i].HOST_IF;
   /* Initialize the USB interface. */
   if (host_if->HOST_PREINIT != NULL)
   {
      error = host_if->HOST_PREINIT((_usb_host_handle _PTR_) &usb_host_ptr);
   }

   if (error != USB_OK || usb_host_ptr == NULL || usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR == NULL) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init preinit failure");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
   } /* Endif */

   usb_host_ptr->CALLBACK_STRUCT_PTR = usb_c[i].HOST_IF;
   usb_host_ptr->INIT_PARAM = usb_c[i].HOST_INIT_PARAM;
   _mem_set_type(usb_host_ptr, MEM_TYPE_USB_HOST_STATE_STRUCT);
   _mem_set_type(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR, MEM_TYPE_USB_HOST_PIPE_DESCRIPTORS);
   
   /* No devices attached yet */
   /*
   ** We should maintain device lists per controller handle
   */
   usb_host_ptr->DEVICE_LIST_PTR = NULL;

#if (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS != 0)
   error = _usb_host_iso_packet_desc_pool_create(USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS);
   
   if (error != USB_OK)
   {
      USB_mem_free(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);
      USB_mem_free(usb_host_ptr);
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init: isochronous packet descriptor pool allocation failure");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
   }
#endif

   error = _bsp_usb_host_init(&usb_c[i]);

   if (error != MQX_OK)
   {
#if (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS != 0)
      _usb_host_iso_packet_desc_pool_destroy ();
#endif
      USB_mem_free(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);
      USB_mem_free(usb_host_ptr);
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init: BSP-specific USB initialization failure");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_UNKNOWN_ERROR);
   }

   error = _usb_host_init_call_interface(usb_host_ptr);

   USB_unlock();
   
   if (error != USB_OK)
   {
#if (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS != 0)
      _usb_host_iso_packet_desc_pool_destroy ();
#endif
      USB_mem_free(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);
      USB_mem_free(usb_host_ptr);
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_init returning error status");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   } /* Endif */
   
   usb_c[i].HOST_HANDLE = *handle = (_usb_host_handle)usb_host_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_init SUCCESSFULL");
   #endif
   return USB_OK;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_register_service
* Returned Value : USB_OK or error code
* Comments       :
*     Registers a callback routine for a specified event.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_host_register_service
   (
      /* [IN] Handle to the USB device */
      _usb_host_handle         handle,
      
      /* [IN] type of event or endpoint number to service */
      uint_8                   type,
      
      /* [IN] Pointer to the service's callback function */
      void(_CODE_PTR_ service)(pointer, uint_32)
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   USB_SERVICE_STRUCT_PTR _PTR_        search_ptr;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_register_service");
   #endif

   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (search_ptr = &usb_host_ptr->SERVICE_HEAD_PTR;
      *search_ptr;
      search_ptr = &(*search_ptr)->NEXT) 
   {
      if ((*search_ptr)->TYPE == type) {
         /* Found an existing entry */
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_register_service USBERR_OPEN_SERVICE");
         #endif
         return USB_log_error(__FILE__,__LINE__,USBERR_OPEN_SERVICE);
      } /* Endif */
   } /* Endfor */
   
   /* No existing entry found - create a new one */
   service_ptr = (USB_SERVICE_STRUCT_PTR)USB_mem_alloc_zero(sizeof(USB_SERVICE_STRUCT));
   
   if (!service_ptr) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_register_service memalloc failed");
      #endif

      return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC_SERVICE);
   } /* Endif */
   _mem_set_type(service_ptr, MEM_TYPE_USB_HOST_SERVICE_STRUCT);
   
   service_ptr->TYPE = type;
   service_ptr->SERVICE = service;
   service_ptr->NEXT = NULL;
   *search_ptr = service_ptr;
   
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_register_service SUCCESSFUL");
   #endif
   
   return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_host_call_service
   (
      /* [IN] Handle to the USB device */
      _usb_host_handle handle,

      /* [IN] Type of service or endpoint */
      uint_8           type,
      
      /* [IN] Number of bytes in transmission */
      uint_32          length
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_call_service");
   #endif

   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (service_ptr = usb_host_ptr->SERVICE_HEAD_PTR;
      service_ptr;
      service_ptr = service_ptr->NEXT) 
   {
      if (service_ptr->TYPE == type) {
         if (service_ptr->SERVICE) {
            service_ptr->SERVICE(handle, length);
         }
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_call_service, service exists");
         #endif
         return USB_OK;
      } /* Endif */
      
   } /* Endfor */

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_call_service, service closed");
   #endif
   return USB_log_error(__FILE__,__LINE__,USBERR_CLOSED_SERVICE);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_check_service
* Returned Value : USB_OK or error code
* Comments       :
*     Check if a service is registered or not.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_host_check_service
   (
      /* [IN] Handle to the USB device */
      _usb_host_handle handle,

      /* [IN] Type of service or endpoint */
      uint_8           type,
      
      /* [IN] Number of bytes in transmission */
      uint_32          length
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   /* Needs mutual exclusion */
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_check_service");
   #endif

   USB_lock();
   
   /* Search for an existing entry for type */
   for (service_ptr = usb_host_ptr->SERVICE_HEAD_PTR;
      service_ptr;
      service_ptr = service_ptr->NEXT) 
   {
      if (service_ptr->TYPE == type) {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_check_service service found");
         #endif

         USB_unlock();
         return USB_OK;
      } /* Endif */
   } /* Endfor */

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_check_service service closed");
   #endif

   return USB_log_error(__FILE__,__LINE__,USBERR_CLOSED_SERVICE);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_host_unregister_service
* Returned Value : USB_OK or error code
* Comments       :
*     Unregisters a callback routine for a specified event or endpoint.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_host_unregister_service
   (
      /* [IN] Handle to the USB device */
      _usb_host_handle           handle,

      /* [IN] type of event or endpoint number to service */
      uint_8                     type
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
   USB_SERVICE_STRUCT_PTR              service_ptr;
   USB_SERVICE_STRUCT_PTR _PTR_        search_ptr;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_unregister_service");
   #endif

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (search_ptr = &usb_host_ptr->SERVICE_HEAD_PTR;
      *search_ptr;
      search_ptr = &(*search_ptr)->NEXT) 
   {
      if ((*search_ptr)->TYPE == type) {
         /* Found an existing entry - delete it */
         break;
      } /* Endif */
   } /* Endfor */
   
   /* No existing entry found */
   if (!*search_ptr) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_unregister_service no service found");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_CLOSED_SERVICE);
   } /* Endif */
   
   service_ptr = *search_ptr;
   *search_ptr = service_ptr->NEXT;

   USB_unlock();
   
   USB_mem_free((pointer)service_ptr);
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_unregister_service SUCCESSFUL");
   #endif
   return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_open_pipe
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_open_pipe routine initializes a new pipe descriptor and 
*  returns USB_OK or error and the pipe descriptor for the USB bus address, 
*  endpoint number, direction of transfer, maximum packet size, interval 
*  and the type of pipe
*
*END*-----------------------------------------------------------------*/

static void _usb_host_open_pipe_cleanup(_usb_host_handle, PIPE_STRUCT_PTR, boolean);
static void _usb_host_open_pipe_cleanup(
         /* [IN] the USB Host state structure */
      _usb_host_handle           handle,
      
      PIPE_STRUCT_PTR            pipe_ptr,
      
      boolean                    close_pipe_call
   )
{ /* Body */
   TR_STRUCT_PTR  temp;
   
   if (close_pipe_call) {
      _usb_host_close_pipe_call_interface (handle, pipe_ptr);
   }
   
   while  (pipe_ptr->tr_list_ptr != NULL) {
      temp = pipe_ptr->tr_list_ptr;
      pipe_ptr->tr_list_ptr = pipe_ptr->tr_list_ptr->NEXT;
      USB_mem_free((pointer)temp);
   }
   pipe_ptr->OPEN = FALSE;

   #ifdef _HOST_DEBUG_
   if (close_pipe_call) {
      DEBUG_LOG_TRACE("_usb_host_open_pipe bandwidth alloc failed");
   } else {
      DEBUG_LOG_TRACE("_usb_host_open_pipe callback failed");
   }
   #endif
}

USB_STATUS _usb_host_open_pipe
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle,
      
      /* [IN] Pipe initialization parameters */
      PIPE_INIT_PARAM_STRUCT_PTR    pipe_init_params_ptr,

      /* [OUT] Pipe handle */
      _usb_pipe_handle _PTR_        pipe_handle
   )
{ /* Body */
   PIPE_STRUCT_PTR            pipe_ptr, pipe_bkp_ptr;
   PIPE_INIT_PARAM_STRUCT_PTR pipe_as_params;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   USB_STATUS                 error;
   _mqx_uint                  i = 0;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_open_pipe");
   #endif
   USB_lock();

   for (pipe_ptr = usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR; pipe_ptr != NULL; pipe_ptr = pipe_ptr->NEXT) {
      /* Check if a pipe is not open */
      if (!pipe_ptr->OPEN) {
         break;
      } /* Endif */
      i++;
   } /* Endfor */

   USB_unlock();

   if (pipe_ptr == NULL) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_open_pipe failed");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_OPEN_PIPE_FAILED);
   }
   
   pipe_bkp_ptr = pipe_ptr->NEXT;
   pipe_as_params = (PIPE_INIT_PARAM_STRUCT_PTR)pipe_ptr;
   
   /* copy identical fields params --> descriptor */
   *pipe_as_params = *pipe_init_params_ptr;
   /* restore original NEXT field */
   pipe_as_params->G.NEXT = pipe_bkp_ptr;
   /* additional fields following identical header */
   pipe_as_params->G.NEXTDATA01 = 0;       /* Start DTC at Data0 */
   pipe_as_params->G.PIPE_ID = (int_16)i;
   pipe_as_params->G.OPEN = TRUE;

   pipe_as_params->G.HUB_DEVICE_ADDR = pipe_init_params_ptr->G.HUB_DEVICE_ADDR;
   pipe_as_params->G.HUB_PORT_NUM= pipe_init_params_ptr->G.HUB_PORT_NUM;
   error = _usb_host_open_pipe_call_interface (handle, pipe_ptr);
   
   if (error != USB_OK) {
      _usb_host_open_pipe_cleanup(handle, pipe_ptr, FALSE);
      return USB_log_error(__FILE__,__LINE__,USBERR_OPEN_PIPE_FAILED);
   }
   else {
       *pipe_handle = (pointer)pipe_ptr;
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_open_pipe SUCCESSFUL");
   #endif

   return USB_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_new_tr_element
*  Returned Value : pointer to new host tr struct, NULL if error
*  Comments       :
*         This function will allocate and link a new TR element in the 
*  list of TRs for the specified pipe. It is assumed the caller has issued 
*  USB_lock() before entry.
*
*END*-----------------------------------------------------------------*/
static TR_STRUCT_PTR _usb_host_new_tr_element
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle  pipe_handle
   )
{ /* Body */

   PIPE_STRUCT_PTR     pipe_ptr = (PIPE_STRUCT_PTR) pipe_handle;
   DEV_INSTANCE_PTR    dev_ptr = (DEV_INSTANCE_PTR) pipe_ptr->DEV_INSTANCE;
   USB_HOST_STATE_STRUCT_PTR host_ptr = (USB_HOST_STATE_STRUCT_PTR) dev_ptr->host;

   TR_STRUCT_PTR       tr_ptr, temp_tr_ptr;
   

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_new_tr_element");
   #endif

   if (pipe_ptr->PIPETYPE != USB_CONTROL_PIPE) {
      /* TODO: if not control pipe, then the setup packet is not needed to allocate here */
      tr_ptr = (TR_STRUCT_PTR)USB_mem_alloc_zero(host_ptr->TR_SIZE);
   }
   else {
      /* Contains the setup packet which should not be cached */
      tr_ptr = (TR_STRUCT_PTR)USB_mem_alloc_uncached_zero(host_ptr->TR_SIZE);
   }

   if (tr_ptr == NULL)
   {
      #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_new_tr_element malloc failed");
      #endif
      return NULL;
   }
   _mem_set_type(tr_ptr, MEM_TYPE_USB_HOST_PIPE_TR_STRUCT);

   /* Link pipe's TR structs in circular list of 1 or more items,
   ** where the pipe descriptor's tr list pointer holds the
   ** address of the next struct for scheduling (i.e. the
   ** "beginning" of the list).  
   */
   temp_tr_ptr = pipe_ptr->tr_list_ptr;

   if (temp_tr_ptr == NULL) { 
      /* No existing items, make a circle of one */
      pipe_ptr->tr_list_ptr = tr_ptr->NEXT = tr_ptr;
   } else {
      /* Add new item to the "end" of the existing list */
      while (temp_tr_ptr->NEXT != pipe_ptr->tr_list_ptr)
         temp_tr_ptr = temp_tr_ptr->NEXT;

      temp_tr_ptr->NEXT = tr_ptr;  
      tr_ptr->NEXT = pipe_ptr->tr_list_ptr;
   } /* EndIf */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_new_tr_element SUCCESSFUL");
   #endif
   return tr_ptr;

} /* Endbody */
 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_tr_element
*  Returned Value : pointer to host's pipe TR struct, NULL if error
*  Comments       :
*     If the tr_index is zero or exists on the list, returns NULL
*     Otherwise initializes to zero a TR element with all linkages,
*       and sets its tr_index to the parameter value.  If no unused
*       tr_struct exists on the list (i.e. none has tr_index = 0),
*       a new struct is allocated and linked into the circular list.
*
*END*-----------------------------------------------------------------*/
TR_STRUCT_PTR _usb_host_get_tr_element
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle  pipe_handle,

      /* [IN] transfer parameters */
      uint_32           tr_index
   )
{ /* Body */
   PIPE_STRUCT_PTR pipe_ptr;
   TR_STRUCT_PTR   tr_list, temp_tr_list;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_tr_element");
   #endif

   if (tr_index == 0)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_get_tr_element error null tr_index");
      #endif
      return NULL;   /* zero is unused-TR flag */
   }

   pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;
   tr_list = pipe_ptr->tr_list_ptr;

   USB_lock();
   if (tr_list == NULL)  {  /* No list exists, start one */
      tr_list = _usb_host_new_tr_element(pipe_handle);
      if (tr_list == NULL) {
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_get_tr_element error null pipe tr_list");
         #endif
         return NULL;
      } /* Endif */
   } else {  /* scan for unused TR's, and for index already in list */
      temp_tr_list = tr_list;
      for (; ;) {
         /* TR index is either unused, or */
         if (temp_tr_list->TR_INDEX == 0) {
            tr_list = temp_tr_list;
            break;
         } /* Endif */
         /* TR index is a duplicate (= error), or */
         if (temp_tr_list->TR_INDEX == tr_index) {
            USB_unlock();
            #ifdef _HOST_DEBUG_
               DEBUG_LOG_TRACE("_usb_host_get_tr_element error duplicate tr_index");
            #endif
            return NULL;
         } /* Endif */
         /* TR is last in list, so insert a new one, or */
         if (temp_tr_list->NEXT == tr_list) {
            tr_list = _usb_host_new_tr_element(pipe_handle);
         
            if (tr_list == NULL)
            {
               USB_unlock();
               #ifdef _HOST_DEBUG_
                  DEBUG_LOG_TRACE("_usb_host_new_tr_element FAILED");
               #endif
               return NULL;
            }
            
            break;
         } /* Endif */

         /* Step over item to next on the list */
         temp_tr_list = temp_tr_list->NEXT;

      } /* EndFor */
   } /* EndIf */
   tr_list->TR_INDEX = tr_index;  /* mark this TR busy */
   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_tr_element SUCCESSFUL");
   #endif
   return tr_list;   

} /* Endbody */
 
 
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_set_up_tr
*  Returned Value :
*     USBERR_INVALID_PIPE_HANDLE if the pipe is invalid
*     USBERR_ALLOC if unable to get a new TR
*     USB_STATUS_TRANSFER_QUEUED upon success
*
*  Comments       :
*     Sets up a TR from init parameters.
*
*END*-----------------------------------------------------------------*/
USB_STATUS  _usb_host_set_up_tr
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr,

      /* [OUT] the TR address on the pipe's TR list */
      TR_STRUCT_PTR _PTR_        tr_ptr_ptr
   )
{ /* Body */
   PIPE_STRUCT_PTR pipe_ptr;
   TR_STRUCT_PTR   pipe_tr_ptr;
   TR_STRUCT_PTR   tr_ptr;

   pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_set_up_tr");
   #endif

   /* Check if the pipe id is valid */
   if (pipe_ptr->PIPE_ID > USBCFG_MAX_PIPES) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_set_up_tr Invalid pipe handle");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
   } /* Endif */

   pipe_tr_ptr = _usb_host_get_tr_element(pipe_handle, tr_params_ptr->G.TR_INDEX);

   if (pipe_tr_ptr == NULL) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_set_up_tr NULL pipe handle");
      #endif

      return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
   } /* Endif */

   /* Backup original link */
   tr_ptr = pipe_tr_ptr->NEXT;
   /* Copy user-provided TR to system-linked TR */
   *pipe_tr_ptr = tr_params_ptr->G;
   /* Restore original link */
   pipe_tr_ptr->NEXT = tr_ptr;

   /* If TR's setup packet pointer is non-null, copy the packet */
   if (tr_params_ptr->DEV_REQ_PTR != NULL) {
      pipe_tr_ptr->setup_packet = *((USB_SETUP_PTR)tr_params_ptr->DEV_REQ_PTR);
   } /* Endif */

   /* return successful transfer initiation status */
   pipe_tr_ptr->STATUS = USB_STATUS_TRANSFER_QUEUED;
   *tr_ptr_ptr = pipe_tr_ptr;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_set_up_tr SUCCESSFUL");
   #endif

   return USB_STATUS_TRANSFER_QUEUED;

} /* EndBody */
 
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_transfer_status
*  Returned Value : Status
*  Comments       :
* _usb_host_get_transfer_status is a routine that returns status of a transfer. 
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_get_transfer_status
   (
      /* [IN] the pipe handle */
      _usb_pipe_handle     pipe_handle,

      /* [IN] the transfer number */
      uint_32              tr_number
   )
{ /* Body */
   PIPE_STRUCT_PTR            pipe_ptr;
   TR_STRUCT_PTR              tr_list_ptr;
   uint_32                    status = USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_transfer_status");
   #endif
      
   USB_lock();

   pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;
   if (pipe_ptr!= NULL) {
      tr_list_ptr = pipe_ptr->tr_list_ptr;
      if (tr_list_ptr != NULL) {
         do {
            if (tr_list_ptr->TR_INDEX == tr_number) {
               status = tr_list_ptr->STATUS;
               break;
            } /* Endif */
            tr_list_ptr = tr_list_ptr->NEXT;   
         } while (tr_list_ptr != pipe_ptr->tr_list_ptr);
      } /* Endif */
      if (tr_list_ptr->TR_INDEX != tr_number) {
         /* Transaction done */
         status = USB_STATUS_IDLE;
      } /* Endif */
   } /* Endif */
   
   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_transfer_status SUCCESSFUL");
   #endif

   return status;
} /* Endbody */
  
/* EOF */
