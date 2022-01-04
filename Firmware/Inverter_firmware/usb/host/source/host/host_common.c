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
* $FileName: host_common.c$
* $Version : 3.8.36.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains common functions for USB class drivers
*   Implementation Notes:
*   An interface is supported by two data structures on the
*   owning device's list of allocated memory:-
*   (1) a pipe bundle struct with pointers to open pipes for
*   all endpoints on the interface
*   (2) a class-interface struct with queues for TR logging,
*   pipe handles, and other class-specific status etc.
*   Each <class_name>.h must define its interface struct, which must
*   begin with the fields in GENERAL_CLASS (see common.h),
*   if common routines are going to be used.
*   It is strongly recommended that all class init functions
*   call usb_host_class_intf_init (pointer to pipe bundle,
*   pointer to class-intf, (pointer)&class_anchor ).  This
*   will verify correct pointers, zero the interface struct,
*   and set up the header pointers + key correctly.
*   All class usage of the pointers etc. in the struct
*   must be protected against detach (which occurs without
*   advance notice):-
*   USB_lock();
*   if (usb_host_class_intf_validate(class-call-struct pointer)) {
*       Issue commands / Use pipes etc.
*   }
*   USB_unlock();
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_common.h"
#include "host_main.h"
#include "host_dev_list.h"
#include "host_ch9.h"

extern CLASS_MAP class_interface_map[];

/* Unique non-zero key to assure valid interface use. */
static uint_32 Class_Key_Generator = 0;

/* Prototypes, copy to stack frames for each use instance */

const static TR_INIT_PARAM_STRUCT tr_prototype =
{
   NULL, /* pointer to next */
   USB_STATUS_IDLE, /* status of the transfer */
   0,    /* transaction index */
   NULL, /* TX buffer */
   NULL, /* RX buffer */
   0,    /* TX/data phase length */
   0,    /* RX/data phase length */
   NULL, /* callback pointer */
   NULL, /* callback param */
   /* all the other structure members are filled during transaction setup */
};

static uint_32 tr_index = 0;

/* Unpublished functions, not intended for application use */
static USB_STATUS usb_hostdev_get_class(_usb_device_instance_handle, uint_16, pointer _PTR_ );
static uint_32 usb_hostdev_mask_class_etc(uint_8_ptr , uint_8_ptr);
static USB_STATUS  usb_hostdev_open_pipe(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint_32, uint_8, _usb_pipe_handle _PTR_);
static boolean  usb_host_driver_info_match(DEV_INSTANCE_PTR, INTERFACE_DESCRIPTOR_PTR, USB_HOST_DRIVER_INFO_PTR);
static void  usb_host_driver_info_scan(DEV_INSTANCE_PTR, INTERFACE_DESCRIPTOR_PTR, uint_32);
static boolean usb_host_driver_info_nonzero(USB_HOST_DRIVER_INFO_PTR);
static CLASS_MAP_PTR  usb_host_map_class_intf(PIPE_BUNDLE_STRUCT_PTR);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_attach_detach
* Returned Value : none
* Comments       :
*     Scan device config for interfaces that may need callbacks
*     for matching classes etc. from each interface. This function is
*     called when there configuration change or attach or detach event
*
*END*--------------------------------------------------------------------*/
void  usb_hostdev_attach_detach
   (
      /* [IN] USB device */
      DEV_INSTANCE_PTR           dev_ptr,

      /* [IN] event causing callback */
      uint_32                    event_code
   )
{ /* Body */
   DESCRIPTOR_UNION desc;
   uint_32          cfg_end;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_attach_detach");
   #endif

   USB_lock();

   /* FIXME: it is presumed that memlist points to the config descriptor */
   desc.pntr = dev_ptr->memlist->payload.data + dev_ptr->memlist->offset;
   cfg_end = desc.word + SHORT_LE_TO_HOST((desc.cfig->wTotalLength[1] << 8) | (desc.cfig->wTotalLength[0] << 0));

   /* We will always start with config desc so update the search pointer */
   desc.word += desc.cfig->bLength;

   /* Scan through all the interface descriptors for this configuration */
   do {
      /* Scan through all the other descriptors for this interface */
      do {
        
      } while (0);
      if (desc.intf->bDescriptorType == USB_DESC_TYPE_IF)
         usb_host_driver_info_scan(dev_ptr, desc.intf, event_code);
      desc.word += desc.intf->bLength; /* next descriptor */
      
      // EAI
      if (desc.intf->bLength==0) break;
      
   } while (desc.word < cfg_end);

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_attach_detach SUCCESSFUL");
   #endif
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_busy_check
* Returned Value : USB_OK or
*                    USBERR_DEVICE_NOT_FOUND
*                    USBERR_DEVICE_BUSY
* Comments       :
*     Function to verify pointer is address of an instance on device list.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
USB_STATUS  usb_hostdev_busy_check
   (
      _usb_device_instance_handle  device_handle
   )
{ /* Body */
   DEV_INSTANCE_PTR           dev_ptr = (DEV_INSTANCE_PTR)device_handle;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)dev_ptr->host;
   DEV_INSTANCE_PTR           test_ptr = (DEV_INSTANCE_PTR)usb_host_ptr->DEVICE_LIST_PTR;
   USB_STATUS                 error;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_busy_check");
   #endif

   while ((test_ptr != dev_ptr) && (test_ptr != NULL))
   {
      test_ptr = test_ptr->next;
   }

   if (test_ptr == NULL)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_hostdev_busy_check device not found");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_busy_check SUCCESSFUL");
   #endif

   error = (USB_STATUS)((test_ptr->memlist->next != NULL) ? USBERR_DEVICE_BUSY : USB_OK);
   return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_validate
* Returned Value : USB_OK or USBERR_DEVICE_NOT_FOUND
* Comments       :
*     Function to verify pointer is address of an instance on device list.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
USB_STATUS  usb_hostdev_validate
   (
      _usb_device_instance_handle  device_handle
   )
{ /* Body */
   DEV_INSTANCE_PTR           dev_ptr = (DEV_INSTANCE_PTR)device_handle;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   USB_HOST_IF_STRUCT_PTR     usb_c;
   DEV_INSTANCE_PTR           test_ptr;
   USB_STATUS                 error;
   _mqx_uint                  i;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_validate");
   #endif

   if (device_handle == NULL) {
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_validate FAILED, invalid device number");
   #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_DEVICE_NUM);
   }
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)dev_ptr->host;

   USB_lock();
   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);

   /* Check if the interface is registered */
   if (!usb_c) {
      /* No interface was registered yet */
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_hostdev_validate FAILED, no controller installed");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }
   /* Find out if the host controller interface has been installed */
   for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
      if (usb_c[i].HOST_HANDLE == usb_host_ptr) {
         /* Found installed device */
         break;
      }
   }
   if (i == USBCFG_MAX_DRIVERS) {
      /* No such controller was initialized yet */
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_hostdev_validate FAILED, no controller inited");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }

   /* usb_host_ptr is valid host state structure, check for list of attached device instances */
   test_ptr = (DEV_INSTANCE_PTR)usb_host_ptr->DEVICE_LIST_PTR;
   while ((test_ptr != dev_ptr) && (test_ptr != NULL))
   {
      test_ptr = test_ptr->next;
   }

   USB_unlock();

   #ifdef _HOST_DEBUG_
      if(test_ptr == NULL) 
      {
         DEBUG_LOG_TRACE("usb_hostdev_validate null device handle");
      }
      else
      {  
         DEBUG_LOG_TRACE("usb_hostdev_validate SUCCESSFUL");
      }
   #endif

   error = (test_ptr == NULL) ? USBERR_DEVICE_NOT_FOUND : USB_OK;
   return USB_log_error(__FILE__,__LINE__,error);
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hostdev_get_buffer
*  Returned Value : pointer to the buffer, or NULL + error
*  Comments       :
*        Applications should use this function to get I/O buffers and
*        other work areas that stay allocated until the device is
*        detached.  At that time, these are all freed by the system.
*        Another approach is to use _usb_hostdev_free_buffer
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_hostdev_get_buffer
   (
      /* [IN] the device instance */
      _usb_device_instance_handle   dev_handle,

      /* [IN] buffer size to get */
      uint_32                       buffer_size,

      /* [OUT] pointer to the buffer, or NULL */
      pointer               _PTR_ buffer_ptr
   )
{ /* Body */

#ifdef USB_DMA_ALIGN
    return _usb_hostdev_get_buffer_aligned(dev_handle, buffer_size, USB_DMA_ALIGN(1), buffer_ptr);
#else
    return _usb_hostdev_get_buffer_aligned(dev_handle, buffer_size, 1, buffer_ptr);
#endif

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hostdev_get_buffer_aligned
*  Returned Value : pointer to the buffer, or NULL + error
*  Comments       :
*        Applications should use this function to get I/O buffers and
*        other work areas that stay allocated until the device is
*        detached.  At that time, these are all freed by the system.
*        Another approach is to use _usb_hostdev_free_buffer
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_hostdev_get_buffer_aligned
   (
      /* [IN] the device instance */
      _usb_device_instance_handle   dev_handle,

      /* [IN] buffer size to get */
      uint_32                       buffer_size,

      /* [IN] alignment vlaue (e.g. 8 for 8B alignment) */
      uint_32                       align,

      /* [OUT] pointer to the buffer, or NULL */
      pointer                 _PTR_ buffer_ptr
   )
{ /* Body */
   DEV_MEMORY_PTR    dev_mem;
   DEV_INSTANCE_PTR  dev_ptr;
   USB_STATUS        error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_get_buffer");
   #endif
   
   USB_lock();

   error = usb_hostdev_validate(dev_handle);
   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_get_buffer invalid device handle");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   } /* Endif */

   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

   error =  usb_dev_list_get_mem(dev_ptr, buffer_size,
      USB_MEMTYPE_BUFFER, align, &dev_mem);

   if (error != USB_OK)
      *buffer_ptr = NULL;
   else
      *buffer_ptr = dev_mem->payload.data + dev_mem->offset;

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_get_buffer SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hostdev_free_buffer
*  Returned Value : USB_OK if buffer block found
*  Comments       :
*        Applications should use this function to free I/O buffer.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_hostdev_free_buffer
   (
      /* [IN] the device instance */
      _usb_device_instance_handle   dev_handle,

      /* [IN] pointer to the buffer */
      pointer                       buffer_ptr
   )
{ /* Body */
   DEV_MEMORY_PTR    dev_mem;
   DEV_INSTANCE_PTR  dev_ptr;
   USB_STATUS        error;
   uint_32           offset;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_free_buffer");
   #endif
   
   USB_lock();

   error = usb_hostdev_validate(dev_handle);
   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_free_buffer invalid device handle");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   } /* Endif */

   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   /* Get the offset of real payload within device memory. */
   offset = *((uchar_ptr) buffer_ptr - 1);
   /* Find device memory pointer. */
   dev_mem = (DEV_MEMORY_PTR) ((uchar_ptr) buffer_ptr - offset - MEM_HEADER_LEN);

   error = usb_dev_list_free_mem(dev_ptr, dev_mem);

   USB_unlock();

   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_free_buffer FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_get_buffer SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_hostdev_get_class
*  Returned Value : pointer to class/interface struct, or NULL + error
*  Comments       :
*        This wrapper is for _usb_hostdev_select_interface use only.
*
*END*-----------------------------------------------------------------*/
static USB_STATUS usb_hostdev_get_class
   (
      /* [IN] the device instance */
      _usb_device_instance_handle   dev_handle,

      /* [IN] buffer size to get */
      uint_16                       struct_size,

      /* [OUT] pointer to the buffer, or NULL */
      pointer                 _PTR_ struct_ptr
   )
{ /* Body */
   DEV_MEMORY_PTR    memory_block;
   DEV_INSTANCE_PTR  dev_ptr;
   USB_STATUS        error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_get_class");
   #endif
   
   USB_lock();
   error = usb_hostdev_validate(dev_handle);
   if (error != USB_OK) {
      USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_get_class invalid device handle");
   #endif
      return USB_log_error(__FILE__,__LINE__,error);
   } /* Endif */

   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

   error =  usb_dev_list_get_mem
      (dev_ptr,
      struct_size,
      USB_MEMTYPE_CLASS,
      1,
      &memory_block);

   if (error != USB_OK)
      *struct_ptr = NULL;
   else
      *struct_ptr = memory_block->payload.data + memory_block->offset;

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_get_class SUCCESSFUL");
   #endif
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hostdev_get_descriptor
*  Returned Value : handle of the descriptor, or NULL + error
*  Comments       :
*     If the intf_handle is NULL, then the descriptor is searched within
*     the whole device. If the intf_handle is not NULL, the descriptor is
*     searched within specified interface (ignored if desc_type is
*     USB_DESC_TYPE_DEV, USB_DESC_TYPE_CFG or USB_DESC_TYPE_IF and required
*     for USB_DESC_TYPE_EP).
*
*  It is presumed that this function is called with interrupts disabled.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_hostdev_get_descriptor
   (
      /* [IN] the device instance */
      _usb_device_instance_handle   dev_handle,

      /* [IN] the device instance or NULL to search the device config */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] the type of descriptor to get */
      descriptor_type               desc_type,

      /* [IN] the index of desc_type decriptor, or 0 */
      uint_8                        desc_index,

      /* [IN] the interface alternate, or 0 */
      uint_8                        intf_alt,

      /* [OUT] handle of the descriptor, or NULL */
      pointer                 _PTR_ descriptor
   )
{ /* Body */

   DEV_INSTANCE_PTR  dev_ptr;
   DESCRIPTOR_UNION  ptr1, ptr2;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor");
   #endif
   
   *descriptor = NULL; /* Assume the worst */

   /* Check that device handle points to a device */
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

   if (dev_ptr->state < DEVSTATE_CFG_READ) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor device busy");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_BUSY);
   } /* Endif */

   /* Device descriptor is in the instance struct */
   if (desc_type == USB_DESC_TYPE_DEV) {
      *descriptor = &dev_ptr->dev_descriptor;
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor SUCCESSFUL");
      #endif
      return USB_OK;
   } /* EndIf */

   /*--------------------------------------------------------**
   ** If descriptor type is Configuration, the pointer is    **
   **    found in ptr1.  Other types of descriptor need      **
   **    to be found by stepping through the config one      **
   **    descriptor at a time.                               **
   ** To prevent parsing past the config buffer, ptr2 is     **
   **    set to the starting address plus its total size.    **
   **--------------------------------------------------------*/

   ptr1.pntr = dev_ptr->memlist->payload.data + dev_ptr->memlist->offset; /* offset for alignment */

   if (desc_type != USB_DESC_TYPE_CFG) {
      ptr2.word = ptr1.word + SHORT_UNALIGNED_LE_TO_HOST(ptr1.cfig->wTotalLength);

      #define  HOSTDEV_GET_NEXT_DESCRIPTOR\
      ptr1.word += ptr1.cfig->bLength;    \
      if (ptr1.word >= ptr2.word) {       \
         return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);     \
      } /* Endif */

      switch (desc_type) {
         case USB_DESC_TYPE_IF:
            if (ptr1.cfig->bNumInterfaces < desc_index) {
               #ifdef _HOST_DEBUG_
                  DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no INTF descriptor");
               #endif
               return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
            } /* Endif */

            do {
               HOSTDEV_GET_NEXT_DESCRIPTOR
            } while (
               (ptr1.intf->bDescriptorType != USB_DESC_TYPE_IF) ||
               (ptr1.intf->bInterfaceNumber != desc_index) ||
               (ptr1.intf->bAlternateSetting != intf_alt));

            //dev_ptr->intf_recently = ptr1.intf; /* use for endpoints, deprecated */
            break;
         case USB_DESC_TYPE_EP:
            /* Endpoints must follow a known valid interface --
            ** the pointer cannot be NULL, and must point to
            ** the type & length for interface descriptors */
            ptr1.intf = intf_handle;

            if (ptr1.intf == NULL) {
               #ifdef _HOST_DEBUG_
                  DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no EP descriptor");
               #endif
               return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
            } /* Endif */

            if ((ptr1.intf->bDescriptorType != USB_DESC_TYPE_IF) ||
               (ptr1.intf->bLength != 9) )
            {
               #ifdef _HOST_DEBUG_
                  DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no descriptor");
               #endif
               return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
            } /* EndIf */

            /* Endpoint 0 is NOT included in the count, and
            ** there is no descriptor for it after interface
            */
            if ((desc_index == 0) ||
               (desc_index > ptr1.intf->bNumEndpoints)) {
               #ifdef _HOST_DEBUG_
                  DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no EP0 descriptor");
               #endif
               return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
            } /* Endif */

            do {
               HOSTDEV_GET_NEXT_DESCRIPTOR
               switch (ptr1.ndpt->bDescriptorType) {
                  case USB_DESC_TYPE_EP:
                     desc_index--;
                     break ;
                  case USB_DESC_TYPE_IF:
                     #ifdef _HOST_DEBUG_
                        DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no EP descriptor");
                     #endif
                     return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
                  default:
                     break ;
               } /* EndCase */
            } while (desc_index > 0);
            break;
         case USB_DESC_TYPE_STR:   /* Not in config, use Ch. 9 call */
            #ifdef _HOST_DEBUG_
               DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no STRING descriptor read");
            #endif
            return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
         case USB_DESC_TYPE_DEV_QUALIFIER:
         case USB_DESC_TYPE_OTHER_SPEED_CFG:
         case USB_DESC_TYPE_IF_POWER:
            do {
               HOSTDEV_GET_NEXT_DESCRIPTOR
            } while (ptr1.cfig->bDescriptorType != desc_type);
            break;
         default:
            if (intf_handle != NULL) {
               /* These descriptors must follow a known valid interface --
               ** the pointer cannot be NULL, and must point to
               ** the type & length for interface descriptors */
               ptr1.intf = intf_handle;
            }
         
            if (ptr1.intf == NULL) {
               #ifdef _HOST_DEBUG_
                  DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no INTF descriptor");
               #endif
               return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
            } /* Endif */

            for (; ;) {
               HOSTDEV_GET_NEXT_DESCRIPTOR
               /* test if we reached next interface and search scope was only interface */
               if ((ptr1.cfig->bDescriptorType == USB_DESC_TYPE_IF) &&
                  (intf_handle != NULL)) {
                  #ifdef _HOST_DEBUG_
                     DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor no requested descriptor");
                  #endif
                  return USB_log_error(__FILE__,__LINE__,USBERR_NO_DESCRIPTOR);
               }
               if (ptr1.cfig->bDescriptorType == desc_type)
                  if (!desc_index--)
                     break;
            }
            break;
      } /* EndSwitch */
   } /* EndIf */
      #undef  HOSTDEV_GET_NEXT_DESCRIPTOR

   *descriptor = ptr1.pntr;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_get_descriptor SUCCESSFUL");
   #endif
   return USB_OK;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_find_pipe_handle
* Returned Value : pipe handle, or NULL if not found
* Comments       :
*     Function to find a pipe with specified type and direction
*
*END*--------------------------------------------------------------------*/
_usb_pipe_handle  _usb_hostdev_find_pipe_handle
   (
      /* [IN] the device instance */
      _usb_device_instance_handle      dev_handle,

      /* interface handle */
      _usb_interface_descriptor_handle intf_handle,

      /* Pipe type = one of:
      ** USB_ISOCHRONOUS_PIPE
      ** USB_INTERRUPT_PIPE
      ** USB_CONTROL_PIPE
      ** USB_BULK_PIPE
      */
      uint_8                           pipe_type,

      /* Pipe direction (ignored for control pipe) = one of:
      ** USB_RECV
      ** USB_SEND
      */
      uint_8                           pipe_direction
   )
{ /* Body */
   DESCRIPTOR_UNION           desc;
   DEV_INSTANCE_PTR           dev_ptr;
   DEV_MEMORY_PTR             dev_mem;
   PIPE_BUNDLE_STRUCT_PTR     pbs_ptr;
   _usb_pipe_handle           pipe_ptr;
   uint_8                     index;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_find_pipe_handle");
   #endif

   USB_lock();
   /* Verify that the intf_handle points to an interface descriptor */
   desc.pntr = intf_handle;
   if ((desc.intf->bDescriptorType != USB_DESC_TYPE_IF)
      || (desc.intf->bLength != 9)) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_find_pipe_handle invalid interface");
      #endif
      return NULL;
   } /* Endif */

   /* Scan device instance for the interface */
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

   /* The device may have several interfaces selected */
   for (index = 0; index < dev_ptr->num_of_interfaces; index++) {
      if (desc.intf == dev_ptr->intf_descriptor[index]) {
         /* Bundle structs are linked into the memory list */
         dev_mem = dev_ptr->memlist;
         do {
            if (dev_mem->blktype == USB_MEMTYPE_PIPES) {
               /* A bundle is owned by a device & interface */
               pbs_ptr = (PIPE_BUNDLE_STRUCT_PTR) (dev_mem->payload.data + dev_mem->offset);
               if ((pbs_ptr->dev_handle == (_usb_device_instance_handle)dev_ptr)
                  && (pbs_ptr->intf_handle == desc.pntr))
               {
                  pipe_ptr = usb_hostdev_get_pipe_handle(pbs_ptr,
                     pipe_type, pipe_direction);
                  USB_unlock();
                  #ifdef _HOST_DEBUG_
                     DEBUG_LOG_TRACE("_usb_hostdev_find_pipe_handle SUCCESSFUL");
                  #endif
                  return pipe_ptr;
               } /* Endif */
            } /* EndIf */
            dev_mem = dev_mem->next;
         } while (dev_mem != NULL);
      } /* EndIf */
   } /* EndFor */

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_find_pipe_handle error");
   #endif
   return NULL;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_get_pipe_handle
* Returned Value : pipe handle, or NULL if none in bundle
* Comments       :
*     Function to map a particular pipe type and direction in
*     the given bundle (which contains device/interface).
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
_usb_pipe_handle usb_hostdev_get_pipe_handle
   (
      /* pointer to pipe bundle struct */
      PIPE_BUNDLE_STRUCT_PTR     pbs_ptr,

      /* Pipe type = one of: USB_ISOCHRONOUS_PIPE, USB_INTERRUPT_PIPE, USB_CONTROL_PIPE, USB_BULK_PIPE */
      uint_8                     pipe_type,

      /* Pipe direction (ignored for control pipe) = one of: USB_RECV, USB_SEND */
      uint_8                     pipe_direction
   )
{ /* Body */

   DESCRIPTOR_UNION           desc;
   PIPE_STRUCT_PTR            pipe_ptr;
   uint_8                     m, n;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle");
   #endif

   if (USB_OK != usb_hostdev_validate(pbs_ptr->dev_handle))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle invalid device handle");
      #endif
      return NULL;

   }

   switch (pipe_type) {
      case USB_CONTROL_PIPE:
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle SUCCESSFUL");
         #endif
         return ((DEV_INSTANCE_PTR)pbs_ptr->dev_handle)->control_pipe;
      case USB_BULK_PIPE:
      case USB_INTERRUPT_PIPE:
      case USB_ISOCHRONOUS_PIPE:
         break;
      default:
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle FAILED");
         #endif
         return NULL;
   } /* EndSwitch */

   switch (pipe_direction) {
      case USB_RECV:
      case USB_SEND:
         break;
      default:
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle invalid direction");
         #endif
         return NULL;
   } /* EndSwitch */

   desc.intf = (INTERFACE_DESCRIPTOR_PTR)pbs_ptr->intf_handle;

   if ((desc.intf->bDescriptorType != USB_DESC_TYPE_IF) ||
      (desc.intf->bLength != 9))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle invalid interface");
      #endif
      return NULL;
   } /* Endif */

   m = desc.intf->bNumEndpoints;

   for (n = 0 ;  n < m ;  n++) {
      pipe_ptr = (PIPE_STRUCT_PTR)pbs_ptr->pipe_handle[n];
      if ((pipe_ptr->PIPETYPE == pipe_type) &&
         (pipe_ptr->DIRECTION == pipe_direction))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle SUCCESSFUL");
         #endif
         return (pointer)pipe_ptr;
      } /* Endif */
   } /* EndFor */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_get_pipe_handle FAILED");
   #endif
   return NULL;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_class_intf_init
* Returned Value : USB_OK or error
* Comments       :
*     Initialize header of class interface info struct.
*     Incoming params are checked to catch errors early
*     before attempts to use the struct.
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_host_class_intf_init
   (
      /* [IN] device/descriptor/pipe handles */
      PIPE_BUNDLE_STRUCT_PTR  pbs_ptr,

      /* [IN] class-interface handle */
      pointer                 class_intf_handle,

      /* [IN] address of class's instance list anchor */
      pointer                 anchor_ptr,

      /* [IN] function for deinitialization */
      INTF_DEINIT_CALL        deinit_fcn

   )
{ /* Body */

   DEV_INSTANCE_PTR              dev_ptr;
   GENERAL_CLASS_PTR             class_ptr = NULL;
   INTERFACE_DESCRIPTOR_PTR      intf_desc;
   DEV_MEMORY_PTR                mem_ptr;
   USB_STATUS                    error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_class_intf_init");
   #endif

   USB_lock();

   error = usb_hostdev_validate (pbs_ptr->dev_handle);

   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_class_intf_init invalid device");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   } /* EndIf */

   dev_ptr = (DEV_INSTANCE_PTR)pbs_ptr->dev_handle;
   intf_desc = (INTERFACE_DESCRIPTOR_PTR)pbs_ptr->intf_handle;

   if ((intf_desc->bDescriptorType != USB_DESC_TYPE_IF) ||
         (intf_desc->bLength != USB_DESC_LEN_IF))
   {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_class_intf_init interface not found");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_NO_INTERFACE);
   } /* EndIf */

   for (mem_ptr = dev_ptr->memlist; ;mem_ptr = mem_ptr->next)  {

      if (mem_ptr == NULL) {
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_class_intf_init no interface memory");
         #endif
         return USB_log_error(__FILE__,__LINE__,USBERR_NO_INTERFACE);
      } /* EndIf */

      if ((mem_ptr->blktype == USB_MEMTYPE_CLASS) &&
         ((mem_ptr->payload.data + mem_ptr->offset) == class_intf_handle))
      {
         break;
      } /* Endif */

   } /* EndFor */

   if (anchor_ptr == NULL) {
      USB_unlock ();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_class_intf_init invalid parameter");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_ANCHOR);
   } /* Endif */

   USB_mem_zero(class_intf_handle, mem_ptr->blksize);

   /* Find last item already on list, add new item at end */
   class_ptr = (GENERAL_CLASS_PTR)anchor_ptr;

   while (class_ptr->next != NULL)
   {
      class_ptr = class_ptr->next;
   }

   class_ptr->next = (GENERAL_CLASS_PTR)class_intf_handle;

   /* Initialize header fields of new item */
   class_ptr = (GENERAL_CLASS_PTR)class_intf_handle;
   class_ptr->next = NULL;
   class_ptr->anchor = (GENERAL_CLASS_PTR)anchor_ptr;
   class_ptr->dev_handle = pbs_ptr->dev_handle;
   class_ptr->intf_handle = pbs_ptr->intf_handle;
   class_ptr->host_handle = dev_ptr->host;
   class_ptr->class_deinit = deinit_fcn;

   Class_Key_Generator++;

   class_ptr->key_code = Class_Key_Generator;

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_class_intf_init SUCCESSFUL");
   #endif
   return USB_OK;
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_class_intf_validate
* Returned Value : uint_32, depends on coded_key in the parameter
*                    coded_key = 0, retrieve intf's key value
*                    coded key > 0, TRUE if intf OK to use
* Comments       :
*     This routine should be used by all classes using pointers
*     or other values kept in the struct:
*     USB_lock();
*     if (usb_host_class_intf_validate(class_intf_ptr) )
*        {
*           Issue commands / Use pipes etc.
*        }
*     USB_unlock();
*     It is presumed that this function is called with interrupts disabled
*
*END*--------------------------------------------------------------------*/
uint_32  usb_host_class_intf_validate
   (
      /* [IN] class-interface handle */
      pointer  class_intf_handle
   )
{ /* Body */
   CLASS_CALL_STRUCT_PTR   cc_ptr;
   GENERAL_CLASS_PTR       gen_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_class_intf_validate");
   #endif

   cc_ptr = (CLASS_CALL_STRUCT_PTR)class_intf_handle;
   if (NULL == cc_ptr) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_class_intf_validate FAILED, NULL class handle");
      #endif
      return 0;
   }

   gen_ptr = (GENERAL_CLASS_PTR)cc_ptr->class_intf_handle;
   /* TODO: check if the gen_ptr is registered class in the stack */

   /* Match key to caller's value */
   if (cc_ptr->code_key != 0 ) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_class_intf_validate SUCCESSFUL");
      #endif
      return (uint_32)(cc_ptr->code_key == gen_ptr->key_code);
   } else {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_class_intf_validate SUCCESSFUL");
      #endif
      return (uint_32)(cc_ptr->code_key = gen_ptr->key_code);
   } /* EndIf */

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_match
* Returned Value : TRUE for driver match, else FALSE
* Comments       :
*     Match device driver info with class etc. in device/interface
*     following the ordering in "USB Common Class Specification"
*     Rev. 1.0, August 18, 1998, p. 7
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static boolean  usb_host_driver_info_match
   (
      /* [IN] USB device */
      DEV_INSTANCE_PTR           dev_ptr,

      /* [IN] Configuration number */
      INTERFACE_DESCRIPTOR_PTR   intf_ptr,

      /* [IN] TRUE=attach, FALSE=detach */
      USB_HOST_DRIVER_INFO_PTR   info_ptr
   )
{ /* Body */
   uint_16  info_Vendor, info_Product, dev_Vendor, dev_Product;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_driver_info_match");
   #endif
   
   info_Vendor = SHORT_UNALIGNED_LE_TO_HOST(info_ptr->idVendor);
   info_Product = SHORT_UNALIGNED_LE_TO_HOST(info_ptr->idProduct);

   /* If vendor and product are listed in table (non-zero) */
   if ((info_Vendor | info_Product) != 0) {
      dev_Vendor = SHORT_UNALIGNED_LE_TO_HOST(dev_ptr->dev_descriptor.idVendor);
      dev_Product = SHORT_UNALIGNED_LE_TO_HOST(dev_ptr->dev_descriptor.idProduct);

      if ((info_Vendor == dev_Vendor) &&
         (info_Product == dev_Product))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match PID, VID match");
         #endif
         return TRUE;
      } /* Endif */
   }

   /* note: zero value class in the device descriptor means
   ** that the class is defined in interface descriptor
   */
   if ((info_ptr->bDeviceClass == 0xFF) || /* if the device class is any OR */
      ((info_ptr->bDeviceClass ==             /* or the device class matches */
      dev_ptr->dev_descriptor.bDeviceClass) && 
      (dev_ptr->dev_descriptor.bDeviceClass != 0x00))) /* but it is not zero-value match */
   {
      if ((info_ptr->bDeviceSubClass ==
         dev_ptr->dev_descriptor.bDeviceSubClass) &&
         (info_ptr->bDeviceProtocol ==
         dev_ptr->dev_descriptor.bDeviceProtocol))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match class, match subclass, match protocol");
         #endif
         return TRUE;
      } /* Endif */
      
      if ((info_ptr->bDeviceSubClass ==
         dev_ptr->dev_descriptor.bDeviceSubClass) &&
         (info_ptr->bDeviceProtocol == 0xFF))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match class, match subclass");
         #endif
         return TRUE;
      }

      if ((info_ptr->bDeviceSubClass == 0xFF) && 
         (info_ptr->bDeviceProtocol ==
         dev_ptr->dev_descriptor.bDeviceProtocol))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match class, match protocol");
         #endif
         return TRUE;
      }

      if ((info_ptr->bDeviceSubClass == 0xFF) && 
         (info_ptr->bDeviceProtocol == 0xFF))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match class");
         #endif
         return TRUE;
      }
   }
   
   /* No Device match, try Interface */
   if ((info_ptr->bDeviceClass == 0xFF) || /* if the device class is any OR */
      ((info_ptr->bDeviceClass == /* or the class matches interface class */
      intf_ptr->bInterfaceClass)))
   {
      if ((info_ptr->bDeviceSubClass ==
         intf_ptr->bInterfaceSubClass) &&
         (info_ptr->bDeviceProtocol ==
         intf_ptr->bInterfaceProtocol))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match interface class, match interface subclass, match interface protocol");
         #endif
         return TRUE;
      } /* Endif */
      
      if ((info_ptr->bDeviceSubClass ==
         intf_ptr->bInterfaceSubClass) &&
         (info_ptr->bDeviceProtocol == 0xFF))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match interface class, match interface subclass, match interface protocol");
         #endif
         return TRUE;
      }

      if ((info_ptr->bDeviceSubClass == 0xFF) && 
         (info_ptr->bDeviceProtocol ==
         intf_ptr->bInterfaceProtocol))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match interface class, match interface protocol");
         #endif
         return TRUE;
      }

      if ((info_ptr->bDeviceSubClass == 0xFF) && 
         (info_ptr->bDeviceProtocol == 0xFF))
      {
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_host_driver_info_match interface class");
         #endif
         return TRUE;
      }
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_driver_info_match interface No Match");
   #endif

   return FALSE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_scan
* Returned Value : none
* Comments       :
*     Call attach/detach callbacks for interfaces matching user-list items.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static void  usb_host_driver_info_scan
   (
      /* [IN] USB device */
      DEV_INSTANCE_PTR           dev_ptr,

      /* [IN] Configuration number */
      INTERFACE_DESCRIPTOR_PTR   intf_ptr,

      /* [IN] event causing this call */
      uint_32                    event_code
   )
{ /* Body */
   boolean                          match, matched = FALSE;
   USB_HOST_DRIVER_INFO_PTR         info_ptr;
   USB_HOST_STATE_STRUCT_PTR        host_struct_ptr;
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_driver_info_scan");
   #endif

   if (dev_ptr->host == NULL)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_host_driver_info_scan NULL host pointer");
      #endif
      return;

   }

   /* Get the host handle on which this device is on */
   host_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)dev_ptr->host;

   /* Get the driver info for attach callback when a match occurs */
   info_ptr = host_struct_ptr->DEVICE_INFO_TABLE;
   dev_handle = (_usb_device_instance_handle)dev_ptr;
   intf_handle = (_usb_interface_descriptor_handle)intf_ptr;

   if (info_ptr) {

      while (usb_host_driver_info_nonzero(info_ptr) == TRUE) {
         match = usb_host_driver_info_match(dev_ptr, intf_ptr, info_ptr);

         if (match == TRUE) {
            matched = TRUE;
            /* The match has occurred so notify the application of such
            ** interface
            */
            info_ptr->attach_call(dev_handle, intf_handle, event_code);
         } /* Endif */

         info_ptr++;
      } /* EndWhile */

      if (!matched) {
         /* None of the registered driver info matched so check if a 
         ** default driver is installed 
         */
         if (info_ptr->attach_call) {
            /* The match has not occurred so notify the application of such
            ** interface via the default callback function
            */
            info_ptr->attach_call(dev_handle, intf_handle, event_code);
         } /* Endif */
      } /* Endif */
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_driver_info_scan SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_nonzero
* Returned Value : none
* Comments       :
*     Function to detect the all-zero end-of-list item.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static boolean  usb_host_driver_info_nonzero
   (
      /* [IN] USB device */
      USB_HOST_DRIVER_INFO_PTR           info_ptr
   )
{ /* Body */
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_driver_info_nonzero");
   #endif

   if (info_ptr->bDeviceClass != 0)
   {
      return TRUE;
   }

   if (info_ptr->bDeviceProtocol != 0)
   {
      return TRUE;
   }

   if (info_ptr->bDeviceSubClass != 0)
   {
      return TRUE;
   }

   if (info_ptr->idVendor[0] != 0)
   {
      return TRUE;
   }

   if (info_ptr->idVendor[1] != 0)
   {
      return TRUE;
   }

   if (info_ptr->idProduct[0] != 0)
   {
      return TRUE;
   }

   if (info_ptr->idProduct[1] != 0)
   {
      return TRUE;
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_driver_info_nonzero FAILURE");
   #endif

   return FALSE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_map_class_intf
* Returned Value : pointer to matching class map item, or NULL
* Comments       :
*           Map interface class etc. to a class driver table item.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static CLASS_MAP_PTR  usb_host_map_class_intf
   (
      /* interface's device/descriptor/pipe bundle */
      PIPE_BUNDLE_STRUCT_PTR  pbs_ptr
   )
{ /* Body */
   CLASS_MAP_PTR              map_ptr;
   INTERFACE_DESCRIPTOR_PTR   intf_desc;
   uint_32                    intf_class, map_class;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_map_class_intf");
   #endif

   intf_desc = (INTERFACE_DESCRIPTOR_PTR)pbs_ptr->intf_handle;

   for (map_ptr = &class_interface_map[0]; ; map_ptr++) {
      if (map_ptr->class_init == NULL) {
         map_ptr = NULL;
         break;
      } /* EndIf */
      intf_class = usb_hostdev_mask_class_etc(&intf_desc->bInterfaceClass,
         &map_ptr->class_code_mask);

      map_class = usb_hostdev_mask_class_etc(&map_ptr->class_code,
         &map_ptr->class_code_mask);

      if (intf_class != 0)
      {
         if (intf_class == map_class)
         {
            break;
         }
      }

   } /* EndFor */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_host_map_class_intf SUCCESSFUL");
   #endif

   return map_ptr;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_host_driver_info_register
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to register user's desired-interfaces array of structs.
*
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_driver_info_register
   (
      /* usb host */
      _usb_host_handle     host_handle,

      /* Device info table */
      pointer              info_table_ptr
   )
{ /* Body */

   USB_HOST_STATE_STRUCT_PTR host_struct_ptr;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_driver_info_register");
   #endif

   USB_lock();
   if (host_handle == NULL || info_table_ptr == NULL) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_driver_info_register, NULL pointers");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_UNKNOWN_ERROR);
   } /* Endif */

   host_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)host_handle;
   host_struct_ptr->DEVICE_INFO_TABLE = info_table_ptr;

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_driver_info_register SUCCESSFUL");
   #endif
   return USB_OK;
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_select_config
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to tear down an old configuration, and set up
*     a new configuration with the same or different index.
*
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_hostdev_select_config
   (
      /* [IN] USB device */
      _usb_device_instance_handle   dev_handle,

      /* [IN] Configuration number */
      uint_8                        config_no
   )
{ /* Body */
   USB_STATUS              error;
   DEV_INSTANCE_PTR        dev;

   USB_lock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_select_config");
   #endif

   error = usb_hostdev_validate(dev_handle);
   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_select_config invalid device");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   } /* Endif */

   dev = (DEV_INSTANCE_PTR)dev_handle;

   if (config_no > dev->dev_descriptor.bNumConfigurations) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_select_config invalid config no.");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_CFIG_NUM);
   } /* Endif */

   if (dev->memlist != NULL){
      usb_dev_list_free_memlist(dev_handle);
   }

   dev->cfg_value = config_no;   /* requested config. number */
   dev->new_config = 1;          /* new config flag */
   dev->state = DEVSTATE_SET_CFG;
   error = _usb_host_ch9_set_configuration((_usb_device_instance_handle)dev, dev->cfg_value);

   if (error != USB_STATUS_TRANSFER_QUEUED) {
      dev->state = DEVSTATE_INITIAL;
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_select_config FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }
   else
   {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_select_config SUCCESSFUL");
      #endif
      return USB_OK;
   }
} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_delete_interface
* Returned Value : none
* Comments       :
*     Function to tear down an existing interface.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
void  usb_hostdev_delete_interface
   (
      /* [IN] USB device */
      DEV_INSTANCE_PTR         dev,

      /* [IN] Interface */
      INTERFACE_DESCRIPTOR_PTR intf
   )
{ /* Body */
   DESCRIPTOR_UNION         desc;
   DEV_MEMORY_PTR           mem1, mem2;
   GENERAL_CLASS_PTR        class_ptr1, class_ptr2;
   PIPE_BUNDLE_STRUCT_PTR   pbs_ptr;
   uint_8                   intf_no;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_delete_interface");
   #endif
   
   for (intf_no = 0; intf_no < dev->num_of_interfaces; intf_no++) {
      desc.intf = dev->intf_descriptor[intf_no];

      if (desc.intf == NULL)
      {
         continue;
      }

      /* Equal interface numbers (alternates don't count here) */
      if (intf->bInterfaceNumber != desc.intf->bInterfaceNumber)
      {
         continue;
      }

      /* Find pipe bundle for this interface on memory list */
      for (mem1 = dev->memlist; mem1 != NULL; mem1 = mem1->next) {
         if (mem1->blktype != USB_MEMTYPE_PIPES)
         {
            continue;
         }

         pbs_ptr = (PIPE_BUNDLE_STRUCT_PTR) (mem1->payload.data + mem1->offset);

         if (intf != pbs_ptr->intf_handle)
         {
            continue;
         }

         /* Unlink pipe bundle from memory list */
         mem2 = dev->memlist;
         if (mem1 == mem2) {
            dev->memlist = mem1->next;
         } else {

            while (mem2->next != mem1)
            {
               mem2 = mem2->next;
            }

            mem2->next = mem1->next;
         } /* EndIf */

         /* Close pipes, free memory list item */
         usb_dev_list_close_pipe_bundle(pbs_ptr);
         USB_mem_free(mem1);
         dev->intf_descriptor[intf_no] = NULL;
         intf_no = dev->num_of_interfaces;
         break;
      } /* EndFor */

      /* Find class-interface on memory list */
      for (mem1 = dev->memlist; mem1 != NULL; mem1 = mem1->next) {
         if (mem1->blktype != USB_MEMTYPE_CLASS)
         {
            continue;
         }

         class_ptr1 = (GENERAL_CLASS_PTR) (mem1->payload.data + mem1->offset);

         if (intf != class_ptr1->intf_handle)
         {
            continue;
         }

         /* Call deinit */
         if (class_ptr1->class_deinit != NULL) {
             class_ptr1->class_deinit((_usb_class_intf_handle)class_ptr1);
         }

         /* Invalidate class_intf */
         class_ptr1->key_code = 0;
         /* Unlink class-intf from memory list */
         mem2 = dev->memlist;
         if (mem1 == mem2) {
            dev->memlist = mem1->next;
         } else {

            while (mem2->next != mem1)
            {
               mem2 = mem2->next;
            }

            mem2->next = mem1->next;
         } /* EndIf */

         /* TODO: delete interface event */

         /* Unlink class-intf from class's list */
         class_ptr2 = class_ptr1->anchor; /* A(anchor) */
         if (class_ptr2->next == class_ptr1) {
            class_ptr2->next = class_ptr1->next;
         } else {

            while (class_ptr2->next != class_ptr1)
            {
               class_ptr2 = class_ptr2->next;
            }

            class_ptr2->next = class_ptr1->next;
         } /* EndIf */

         /* Free memory list item */
         USB_mem_free(mem1);
         intf_no = dev->num_of_interfaces;
         break;
      } /* EndFor */

   } /* EndFor */
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_delete_interface SUCCESSFUL");
   #endif
   
} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_mask_class_etc
* Returned Value : masked class/sub-class/protocol
* Comments       :
*     It is assumed that class, sub-class, and protocol are
*     successive uint-8 values (as in a USB descriptor),
*     and likewise the masks to be ANDed with them.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static uint_32  usb_hostdev_mask_class_etc
   (
      /* [IN] pointer to class etc. */
      uint_8_ptr      csp_ptr,

      /* [IN] pointer to masks */
      uint_8_ptr      msk_ptr
   )
{ /* Body */
   uint_32  res;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_mask_class_etc");
   #endif

   /* MSB of result = class */
   res = (uint_32)((*csp_ptr++) & (*msk_ptr++));

   /* 2nd byte of result = sub-class */
   res = (res << 8);
   res |= (uint_32) ((*csp_ptr++) & (*msk_ptr++));

   /* LSB of result = sub-class */
   res = (res << 8);
   res |= (uint_32) ((*csp_ptr) & (*msk_ptr));

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_mask_class_etc SUCCESSFUL");
   #endif
   
   return res;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_open_pipe
* Returned Value : pipe handle + USB_OK, or error status
* Comments       :
*     Function to open a pipe for an endpoint.  Note that endpoints
*     following an interface are numbered 1, 2, 3, ... bNumEndpoints,
*     where the number of endpoints is in the interface descriptor.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static USB_STATUS  usb_hostdev_open_pipe
   (
      /* [IN] USB device */
      _usb_device_instance_handle    dev_handle,

      /* [IN] the device instance or NULL to search the device config */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] Only FLAG=1 is presently defined (= dont't send empty packet) */
      uint_32                        flags,

      /* [IN] Endpoint number 0, 1, ... N */
      uint_8                         ep_num,

      /* [OUT] Handle of opened pipe */
      _usb_pipe_handle _PTR_         pipe_handle_ptr
   )
{ /* Body */
   DESCRIPTOR_UNION         desc;
   DEV_INSTANCE_PTR         dev;
   PIPE_INIT_PARAM_STRUCT   pipe_init_params;
   USB_STATUS               error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_open_pipe");
   #endif

   error = _usb_hostdev_get_descriptor(dev_handle,
      intf_handle,
      USB_DESC_TYPE_EP,
      ep_num,
      0,
      (pointer _PTR_)&desc);

   if (error != USB_OK)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_hostdev_open_pipe failed");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }

   dev = (DEV_INSTANCE_PTR)dev_handle;

   pipe_init_params.G.DEV_INSTANCE = dev_handle;
   pipe_init_params.G.MAX_PACKET_SIZE = (uint_16)(SHORT_UNALIGNED_LE_TO_HOST(desc.ndpt->wMaxPacketSize) & PACKET_SIZE_MASK);
   pipe_init_params.G.NAK_COUNT = USBCFG_DEFAULT_MAX_NAK_COUNT;
   pipe_init_params.G.FIRST_FRAME = pipe_init_params.G.FIRST_UFRAME = 0;
   pipe_init_params.G.FLAGS = flags;
   pipe_init_params.G.DEVICE_ADDRESS = ((DEV_INSTANCE_PTR)dev_handle)->address;
   pipe_init_params.G.ENDPOINT_NUMBER = (uint_8)(desc.ndpt->bEndpointAddress & ENDPOINT_MASK);
   pipe_init_params.G.DIRECTION = (uint_8)((desc.ndpt->bEndpointAddress & IN_ENDPOINT) ? USB_RECV : USB_SEND);
   pipe_init_params.G.tr_list_ptr = NULL;

   switch (desc.ndpt->bmAttributes & EP_TYPE_MASK) {
      case ISOCH_ENDPOINT:
         pipe_init_params.G.PIPETYPE = USB_ISOCHRONOUS_PIPE;
         break;
      case BULK_ENDPOINT:
         pipe_init_params.G.PIPETYPE = USB_BULK_PIPE;
         break;
      case IRRPT_ENDPOINT:
         pipe_init_params.G.PIPETYPE = USB_INTERRUPT_PIPE;
         break;
      case CONTROL_ENDPOINT:
      default:
         pipe_init_params.G.PIPETYPE = USB_CONTROL_PIPE;
         break;
   } /* EndSwitch */

   pipe_init_params.G.SPEED = dev->speed;

   /* Following is taken from the USB 2.0 Specification, bInterval field description of endpoint descriptor */
   if (pipe_init_params.G.PIPETYPE == USB_ISOCHRONOUS_PIPE) {
      pipe_init_params.G.INTERVAL = 1 << (desc.ndpt->iInterval - 1);
   }
   else if ((pipe_init_params.G.PIPETYPE == USB_INTERRUPT_PIPE) && (pipe_init_params.G.SPEED == USB_SPEED_HIGH)) {
      pipe_init_params.G.INTERVAL = 1 << (desc.ndpt->iInterval - 1);
   }
   else {
      pipe_init_params.G.INTERVAL = desc.ndpt->iInterval;
   }

   pipe_init_params.G.TRS_PER_UFRAME = 1;
   if ((pipe_init_params.G.PIPETYPE == USB_ISOCHRONOUS_PIPE) ||
      (pipe_init_params.G.PIPETYPE == USB_INTERRUPT_PIPE))
   {
      switch (SHORT_UNALIGNED_LE_TO_HOST(desc.ndpt->wMaxPacketSize) & ADDITIONAL_MASK) {
         case TWO_ADDITIONAL:
            pipe_init_params.G.TRS_PER_UFRAME++;
         case ONE_ADDITIONAL:
            pipe_init_params.G.TRS_PER_UFRAME++;
            break;
         default:
            break;
      } /* EndSwitch */
   } /* Endif */

   pipe_init_params.G.HUB_DEVICE_ADDR = 0;
   pipe_init_params.G.HUB_PORT_NUM = 0;
   if (dev->hub_no !=0 && dev->speed != USB_SPEED_HIGH)
   {
       pipe_init_params.G.HUB_DEVICE_ADDR = dev->hub_no;
       pipe_init_params.G.HUB_PORT_NUM = dev->port_no;
   }
   error = _usb_host_open_pipe(dev->host, &pipe_init_params, pipe_handle_ptr);

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_open_pipe SUCCESSFUL");
   #endif
      
   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_select_interface
* Returned Value : interface struct + USB_OK, or error status
* Comments       :
*     Function to tear down old interface, and set up
*     a new one with the same or different index/alternate.
*     For each interface, only one of its alternates
*     can be selected per USB spec.  Also per spec, an endpoint
*     can be in only one interface (though it may exist in
*     several alternates of that interface).
*
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_hostdev_select_interface
   (
      /* [IN] USB device */
      _usb_device_instance_handle      dev_handle,

      /* [IN] Interface to be selected */
      _usb_interface_descriptor_handle intf_handle,

      /* [OUT] Initialized class-specific interface struct */
      pointer                          class_intf_ptr
   )
{ /* Body */
   CLASS_CALL_STRUCT_PTR         class_ptr = NULL;
   DESCRIPTOR_UNION              desc;
   DEV_INSTANCE_PTR              dev;
   USB_HOST_DRIVER_INFO_PTR      info_ptr;
   USB_HOST_STATE_STRUCT_PTR     host_struct_ptr;
   DEV_MEMORY_PTR                mem1, mem2;
   INTERFACE_DESCRIPTOR_PTR      intf;
   CLASS_MAP_PTR                 class_map;
   _usb_pipe_handle              pipe_handle;
   PIPE_BUNDLE_STRUCT_PTR        pbs_ptr;
   USB_STATUS                    error;
   uint_32                       block_size;
   uint_8                        ep_num, intf_no;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_select_interface");
   #endif
   
   USB_lock();
   error = usb_hostdev_validate (dev_handle);

   if (error != USB_OK) {
      goto BadExit;
   }

   dev = (DEV_INSTANCE_PTR)dev_handle;
   intf = (INTERFACE_DESCRIPTOR_PTR)intf_handle;

   /* Get the host handle on which this device is on */
   host_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)dev->host;

   /* Get the driver info for attach callback when a match occurs */
   info_ptr = (USB_HOST_DRIVER_INFO_PTR)\
      host_struct_ptr->DEVICE_INFO_TABLE;

   if ((intf->bDescriptorType != USB_DESC_TYPE_IF) ||
         (intf->bLength != USB_DESC_LEN_IF))
   {
      error = USBERR_NO_INTERFACE;
      goto BadExit;
   } /* EndIf */

   /* If an interface was previously selected, tear it down */
   usb_hostdev_delete_interface(dev, intf);

   /* Check that there is an available interface pointer */
   for (intf_no = 0; ;intf_no++) {
      if (intf_no >= dev->num_of_interfaces) {
         error = USBERR_NO_INTERFACE;
         goto BadExit;
      } /* EndIf */

      if (dev->intf_descriptor[intf_no] == NULL) {
         break;
      }

   } /* EndFor */

   /* Create pipe bundle for memory list */
   mem1 = dev->memlist;

   if (mem1 == NULL) { /* Must be at least config on list */
      error = USBERR_NO_INTERFACE;
      goto BadExit;
   } /* EndIf */

   while (mem1->next != NULL) /* Scan to last existing item */
   {
      mem1 = mem1->next;
   }

   block_size = sizeof(PIPE_BUNDLE_STRUCT) +
      sizeof(_usb_pipe_handle) * (intf->bNumEndpoints - 1);

   error = usb_dev_list_get_mem(dev,
      block_size,
      USB_MEMTYPE_PIPES,
      1,
      &mem2);

   if (error != USB_OK) {
      error = USBERR_GET_MEMORY_FAILED;
      goto BadExit;
   } /* EndIf */

   /* Locate interface/alternate in devices configuration */
   error = _usb_hostdev_get_descriptor(dev_handle,
      NULL,
      USB_DESC_TYPE_IF,
      intf->bInterfaceNumber,
      intf->bAlternateSetting,
      (pointer _PTR_)&desc);

   if (error != USB_OK){
      goto BadExit;
   }

   /* Locate each endpoint descriptor, open a pipe for it */
   pbs_ptr = (PIPE_BUNDLE_STRUCT_PTR) (mem2->payload.data + mem2->offset);
   for (ep_num = 1; ep_num <= intf->bNumEndpoints; ep_num++) {
      error = usb_hostdev_open_pipe (dev_handle, intf_handle,
         0, ep_num, &pipe_handle);

      /* If pipe not OK, unlink bundle from list and delete it */
      if (error != USB_OK) {
         mem1->next = NULL;  /* snip off list */
         USB_mem_free(mem2);
         goto BadExit;
      } /* EndIf */
      pbs_ptr->pipe_handle[ep_num-1] = pipe_handle;
   } /* EndFor */

   pbs_ptr->dev_handle = dev_handle;
   pbs_ptr->intf_handle = intf_handle;
   dev->intf_descriptor[intf_no] = intf;

   if (usb_host_driver_info_nonzero(info_ptr)) {

      /* Map interface to class driver, get & initialize driver struct */
      error = USBERR_NO_DEVICE_CLASS;  /* Assume the worst */

      class_map = usb_host_map_class_intf(pbs_ptr);
      if (class_map == NULL) {
         goto BadExit;
      }

      class_ptr = (CLASS_CALL_STRUCT_PTR)class_intf_ptr;
      if (class_ptr == NULL) {
         goto BadExit;
      }

      /* Don't get new struct for associated interfaces */
      error = usb_hostdev_get_class (dev_handle,
         class_map->intf_struct_size,
         (pointer _PTR_)&class_ptr->class_intf_handle);

      if (error != USB_OK) {
         goto BadExit;
      }

      class_map->class_init(pbs_ptr, class_ptr);

      if (class_ptr->code_key == 0){
         goto BadExit;
      }

   } /* Endif */

   /* Set interface on the device */
   dev->state = DEVSTATE_SET_INTF;
   error = _usb_host_ch9_set_interface((_usb_device_instance_handle)dev, intf->bAlternateSetting,
      intf->bInterfaceNumber);
      
   if (error != USB_STATUS_TRANSFER_QUEUED)
   {
      dev->state = DEVSTATE_INITIAL;
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_select_interface FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }
   else
   {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_hostdev_select_interface SUCCESSFUL");
      #endif
      return USB_OK;
   }

BadExit:
   if (error ==USB_OK) {
      error = USBERR_NO_DEVICE_CLASS;
   }
   
   if (class_ptr != NULL) {
      class_ptr->class_intf_handle = NULL;
      class_ptr->code_key = 0;
   }
   
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_hostdev_select_interface bad exit");
   #endif

   return USB_log_error(__FILE__,__LINE__,error);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_tr_init
* Returned Value : Initialize TR with globally unique index
* Comments       :
*     To ensure that a new TR_STRUCT is used by the system for
*     each new transaction, it is essential to have a distinct
*     TR_INDEX value.  Completed transactions have TR_INDEX
*     cleared to zero, it is invalid for a new transaction.
*
*END*--------------------------------------------------------------------*/
void usb_hostdev_tr_init
   (
      /* [OUT] initialized TR_INIT_PARAM_STRUCT */
      TR_INIT_PARAM_STRUCT_PTR   tr_ptr,

      /* [IN] callback to call after completion */
      tr_callback                callback,

      /* [IN] the param to pass back to the callback function */
      pointer                    callback_param
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_tr_init");
   #endif
   USB_lock();

   do {
      tr_index++;
   } while (tr_index == 0);

   *tr_ptr = tr_prototype;
   tr_ptr->G.TR_INDEX = tr_index;
   tr_ptr->G.CALLBACK = callback;
   tr_ptr->G.CALLBACK_PARAM = callback_param;

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_tr_init SUCCESSFUL");
   #endif
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_get_speed
* Returned Value : Device speed
* Comments       :
*     Returns USB_SPEED_LOW, USB_SPEED_HIGH or USB_SPEED_FULL
*
*END*--------------------------------------------------------------------*/

uint_32 _usb_hostdev_get_speed
   (
      /* [IN] USB device */
      _usb_device_instance_handle      dev_handle
   )
{ /* Body */
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_hostdev_get_speed SUCCESSFUL");
   #endif
   return ((DEV_INSTANCE_PTR)dev_handle)->speed;
} /* Endbody */


/* EOF */
