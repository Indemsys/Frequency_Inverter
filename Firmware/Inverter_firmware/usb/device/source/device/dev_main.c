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
* $FileName: dev_main.c$
* $Version : 3.8.8.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
*  This file contains the main USB device API functions that will be 
*  used by most applications.
*                                                               
*END*********************************************************************/
#include <mqx.h>
#include <bsp.h>

#include "usb.h"     //common public
#include "usb_prv.h" //common private
#include "usb_bsp.h" 

#include "dev_cnfg.h"
#include "devapi.h"  //device public
#include "dev_main.h" //device private


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_free_XD
*  Returned Value : void
*  Comments       :
*        Enqueues a XD onto the free XD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_device_free_XD
   (
     _usb_device_handle  handle,
      /* [IN] the dTD to enqueue */
      pointer  xd_ptr
   )
{ /* Body */
    USB_DEV_STATE_STRUCT_PTR usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   USB_lock();

   /*
   ** Add the XD to the free XD queue (linked via PRIVATE) and
   ** increment the tail to the next descriptor
   */
   USB_XD_QADD(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, (struct xd_struct *)xd_ptr);
   usb_dev_ptr->XD_ENTRIES++;

   USB_unlock();
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device specific data structures and calls 
*  the low-level device controller chip initialization routine.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_init
   (
      /* [IN] the USB device controller to initialize */
      struct usb_dev_if_struct  *usb_if,

      /* [OUT] the USB_USB_dev_initialize state structure */
      _usb_device_handle _PTR_  handle,

      /* [IN] number of endpoints to initialize */
      _mqx_uint                 endpoints
   )
{ /* Body */
    USB_DEV_IF_STRUCT_PTR            usb_c;
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
    _mqx_uint                        temp, i, j;
    SCRATCH_STRUCT_PTR               temp_scratch_ptr;
    USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR call_back_table_ptr;
    USB_STATUS                       error;
    USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR dev_if;
    
   /* Check usb_if structure */
   if (usb_if == NULL) {
      return USBERR_DEVICE_NOT_FOUND;
   }
   if (usb_if->DEV_IF == NULL) {
      /* The interface does not support device functionality */
      return USBERR_DEVICE_NOT_FOUND;
   }

   USB_lock();

   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);

   /* Check if the interface is registered */
   if (!usb_c) {
      /* No interface was registered yet */
      USB_unlock();
      return USBERR_DEVICE_NOT_FOUND;
   }
   else {
      /* Find out if the interface has been installed */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].DEV_INIT_PARAM == usb_if->DEV_INIT_PARAM) {
            /* Found installed device */
            break;
         }
      }
   }

   if (i == USBCFG_MAX_DRIVERS) {
      /* The interface has not been registered, so it cannot be initialized */
      USB_unlock();
      return USBERR_DEVICE_NOT_FOUND;
   }
   else {
      /* check if the registered interface was already initialized */
      if (usb_c[i].DEV_HANDLE != NULL) {
         /* The interface was already installed- return handle and USBERR_DEVICE_BUSY value */
         *handle = (_usb_device_handle) usb_c[i].DEV_HANDLE;

         USB_unlock();
         return USBERR_DEVICE_BUSY;
      }
   }

   dev_if = (USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR) usb_c[i].DEV_IF;
   /* Initialize the USB interface. */
   if (dev_if->DEV_PREINIT != NULL)
   {
      error = dev_if->DEV_PREINIT((_usb_device_handle _PTR_) &usb_dev_ptr);
   }
      
   if (usb_dev_ptr == NULL)
   {
      #ifdef _DEV_DEBUG
         printf("1 memalloc failed in _usb_device_init\n");
      #endif  
      USB_unlock();
      return USBERR_ALLOC_STATE;
   } /* Endif */
   
   usb_dev_ptr->CALLBACK_STRUCT_PTR = (pointer) usb_c[i].DEV_IF;
   usb_dev_ptr->INIT_PARAM = usb_c[i].DEV_INIT_PARAM;

   error = _bsp_usb_dev_init(&usb_c[i]);

   if (error != MQX_OK)
   {     
      USB_unlock();
      USB_mem_free(usb_dev_ptr->TEMP_XD_PTR);
      USB_mem_free(usb_dev_ptr->XD_SCRATCH_STRUCT_BASE);
      USB_mem_free(usb_dev_ptr->XD_BASE);
      USB_mem_free(usb_dev_ptr);
      return USBERR_UNKNOWN_ERROR;
   }

   /* Initialize the USB controller chip */
   if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_INIT != NULL) 
   {
       error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR) usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_INIT(usb_dev_ptr);     
   }
   else
   {
        #ifdef _DEV_DEBUG
            printf("_usb_device_init: DEV_INIT is NULL\n");                   
        #endif  
        return USBERR_ERROR;
   }

   if (error) 
   {
      USB_unlock();
      USB_mem_free(usb_dev_ptr->TEMP_XD_PTR);
      USB_mem_free(usb_dev_ptr->XD_SCRATCH_STRUCT_BASE);
      USB_mem_free(usb_dev_ptr->XD_BASE);
      USB_mem_free(usb_dev_ptr);
      return USBERR_INIT_FAILED;
   } /* Endif */
   
   usb_c[i].DEV_HANDLE = *handle = usb_dev_ptr;  

   USB_unlock();

   return error;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_deinit
*  Returned Value : USB_OK or error code
*  Comments       :
*  uninitializes the USB device specific data structures and calls 
*  the low-level device controller chip initialization routine.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_deinit
   (
      /* [OUT] the USB_USB_dev_initialize state structure */
      _usb_device_handle  handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
   USB_DEV_IF_STRUCT_PTR            usb_c;
   USB_STATUS                       error;
   SERVICE_STRUCT_PTR               service_ptr, prev_service_ptr;
   _mqx_uint                        i;


   if (handle == NULL)
   {
        return USBERR_ERROR;
   }
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   USB_lock();

   error = _usb_device_shutdown(handle);

   /* Free all the Callback function structure memory */
   for (service_ptr = usb_dev_ptr->SERVICE_HEAD_PTR; service_ptr != NULL;) {
      prev_service_ptr = service_ptr;
      service_ptr = service_ptr->NEXT;
      USB_mem_free(prev_service_ptr);
   }

   /* Free all internal transfer descriptors */
   USB_mem_free((pointer)usb_dev_ptr->XD_BASE);

   /* Free all XD scratch memory */
   USB_mem_free((pointer)usb_dev_ptr->XD_SCRATCH_STRUCT_BASE);

   /* Free the temp ep init XD */
   USB_mem_free((pointer)usb_dev_ptr->TEMP_XD_PTR);

   /* Find the initialized device controller in the USB components */
   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);
   /* Check if the interface is registered */
   if (!usb_c) {
      /* No interface was registered yet */
      USB_unlock();
      return USBERR_DEVICE_NOT_FOUND;
   }
   else {
      /* Find out where the interface has been installed */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].DEV_HANDLE == handle) {
            /* Found installed device, remove it */
            usb_c[i].DEV_HANDLE = NULL;
            break;
         }
      }
   }

   if (i == USBCFG_MAX_DRIVERS) {
      /* The interface has not been registered, so it cannot be initialized */
      USB_unlock();
      return USBERR_DEVICE_NOT_FOUND;
   }

   /* Free the USB state structure */
   USB_mem_free((pointer)usb_dev_ptr);

   USB_unlock();
   return error;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_init_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Initializes the endpoint and the data structures associated with the 
*  endpoint
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_init_endpoint
   (
    /* [IN] the USB_USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the endpoint structure, include members such as endpoint number, 
     * endpoint type, endpoint direction and the max packet size 
     */                  
    USB_EP_STRUCT_PTR    ep_ptr, 
    
    /* [IN] After all data is transfered, should we terminate the transfer
     * with a zero length packet if the last packet size == MAX_PACKET_SIZE?
     */
      uint_8                     flag
   )
{ /* Body */
   USB_STATUS                    error = 0;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   
   if (handle == NULL)
   {
        return USBERR_ERROR;
   }

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   /* Initialize the transfer descriptor */
   usb_dev_ptr->TEMP_XD_PTR->EP_NUM = ep_ptr->ep_num;
   usb_dev_ptr->TEMP_XD_PTR->BDIRECTION = ep_ptr->direction;
   usb_dev_ptr->TEMP_XD_PTR->WMAXPACKETSIZE = (uint_16)(ep_ptr->size & 0x0000FFFF);
   usb_dev_ptr->TEMP_XD_PTR->EP_TYPE = ep_ptr->type;
   usb_dev_ptr->TEMP_XD_PTR->DONT_ZERO_TERMINATE = flag;

   if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_INIT_ENDPOINT != NULL) 
   {
        error=((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
          usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_INIT_ENDPOINT(handle, 
          usb_dev_ptr->TEMP_XD_PTR);
   }
   else
   {
        #ifdef _DEV_DEBUG
            printf("_usb_device_init_endpoint: DEV_INIT_ENDPOINT is NULL\n");                     
        #endif  
        return USBERR_ERROR;
   }
   
   return error;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_device_register_service
* Returned Value : USB_OK or error code
* Comments       :
*     Registers a callback routine for a specified event or endpoint.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_device_register_service
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle         handle,
      
      /* [IN] type of event or endpoint number to service */
      uint_8                     type,
      
      /* [IN] Pointer to the service's callback function */
      void(_CODE_PTR_ service)(PTR_USB_EVENT_STRUCT, pointer),
      
      /*[IN] User Argument to be passed to Services when invoked.*/
      pointer arg
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
   SERVICE_STRUCT_PTR         service_ptr;
   SERVICE_STRUCT_PTR _PTR_   search_ptr;

   if (handle == NULL)
   {
        return USBERR_ERROR;
   }
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (search_ptr = &usb_dev_ptr->SERVICE_HEAD_PTR;
      *search_ptr;
      search_ptr = &(*search_ptr)->NEXT) 
   {
      if ((*search_ptr)->TYPE == type) 
      {
         /* Found an existing entry */
         USB_unlock();
         return USBERR_OPEN_SERVICE;
      } /* Endif */
   } /* Endfor */
   
   /* No existing entry found - create a new one */
   service_ptr = (SERVICE_STRUCT_PTR)USB_mem_alloc_zero(sizeof(SERVICE_STRUCT));

   if (!service_ptr) 
   {
      USB_unlock();
      #ifdef _DEV_DEBUG
        printf("memalloc failed in _usb_device_register_service\n");
      #endif    
      return USBERR_ALLOC;
   } /* Endif */
   
   service_ptr->TYPE = type;
   service_ptr->SERVICE = service;
   service_ptr->arg = arg;
   service_ptr->NEXT = NULL;
   *search_ptr = service_ptr;
   
   USB_unlock();
   return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_device_unregister_service
* Returned Value : USB_OK or error code
* Comments       :
*     Unregisters a callback routine for a specified event or endpoint.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_device_unregister_service
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle         handle,

      /* [IN] type of event or endpoint number to service */
      uint_8                     type
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
   SERVICE_STRUCT_PTR         service_ptr;
   SERVICE_STRUCT_PTR _PTR_   search_ptr;

   if (handle == NULL)
   {
      return USBERR_ERROR;
   }
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (search_ptr = &usb_dev_ptr->SERVICE_HEAD_PTR;
      *search_ptr;
      search_ptr = &(*search_ptr)->NEXT) 
   {
      if ((*search_ptr)->TYPE == type) 
      {
         /* Found an existing entry - delete it */
         break;
      } /* Endif */
   } /* Endfor */
   
   /* No existing entry found */
   if (!*search_ptr) 
   {
      USB_unlock();
      return USBERR_CLOSED_SERVICE;
   } /* Endif */
   
   service_ptr = *search_ptr;
   *search_ptr = service_ptr->NEXT;

   USB_mem_free((pointer)service_ptr);
   
   USB_unlock();
   return USB_OK;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_device_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered. Used internally only.
* 
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_device_call_service
   (
       /* [IN] Type of service or endpoint */    
      uint_8                  type,
      
      /* [IN] pointer to event structure  */ 
      PTR_USB_EVENT_STRUCT    event            
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   SERVICE_STRUCT _PTR_          service_ptr;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)event->handle;
   /* Needs mutual exclusion */
   USB_lock();
   
   /* Search for an existing entry for type */
   for (service_ptr = usb_dev_ptr->SERVICE_HEAD_PTR;
      service_ptr;
      service_ptr = service_ptr->NEXT) 
   {
      if (service_ptr->TYPE == type) 
      {
         service_ptr->SERVICE(event,service_ptr->arg);
         USB_unlock();
         return USB_OK;
      } /* Endif */
      
   } /* Endfor */

   USB_unlock();

   return USBERR_CLOSED_SERVICE;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_transfer_status
*  Returned Value : Status of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_get_transfer_status
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_STATUS                    error;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
   USB_lock();

   if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_TRANSFER_STATUS != NULL) 
   {
       error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
          usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_TRANSFER_STATUS(handle, ep_num,
          direction);
   }
   else
   {
        #ifdef _DEV_DEBUG
            printf("_usb_device_get_transfer_status: DEV_GET_TRANSFER_STATUS is NULL\n");                     
        #endif  
        return USBERR_ERROR;
   }
   
   USB_unlock();
   /* Return the status of the last queued transfer */
   return error;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_read_setup_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Reads the setup data from the hardware
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_read_setup_data
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] buffer for receiving Setup packet */
      uint_8_ptr                  buff_ptr
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   USB_STATUS                    error = USB_OK;
  
   if (handle == NULL)
   {
      return USBERR_ERROR;
   }
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_SETUP_DATA != NULL) 
   {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_SETUP_DATA(handle, 
            ep_num, buff_ptr);
   }
   else
   {
        #ifdef _DEV_DEBUG
            printf("_usb_device_read_setup_data: DEV_GET_SETUP_DATA is NULL\n");                      
        #endif  
        return USBERR_ERROR;
   }
  
   return error;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_set_address
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sets the device address as assigned by the host during enumeration
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_set_address
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
      
      /* [IN] the USB address to be set in the hardware */
      uint_8                     address
   )
{ 
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   USB_STATUS                    error;
   
   if (handle == NULL)
   {
      return USBERR_ERROR;
   }
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SET_ADDRESS != NULL) 
    {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SET_ADDRESS(handle, address);
    }
    else
    {
        #ifdef _DEV_DEBUG
            printf("_usb_device_read_setup_data: DEV_GET_SETUP_DATA is NULL\n");                      
        #endif  
        return USBERR_ERROR;
    }

   return error;
} 

/* EOF */
