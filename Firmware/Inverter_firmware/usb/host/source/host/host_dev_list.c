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
* $FileName: host_dev_list.c$
* $Version : 3.8.27.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the device-tree root for USB class drivers
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_common.h"
#include "host_dev_list.h"
#include "host_ch9.h"
#include "host_close.h"

static const PIPE_INIT_PARAM_STRUCT pipe_init_params_prototype =
{
   {
      NULL,                   /* pointer to the next pipe */
      NULL,                   /* device instance pointer */
      0,                      /* polling interval */
      USBCFG_DEFAULT_MAX_NAK_COUNT,  /* max. NAKs */
      0, 0,                   /* 1st frame/uframe */
      0,                      /* zero-length last packet */
      64,                     /* max. packet size */
      0,                      /* pipe ID on a particular host controller */
      0,                      /* host controller number */
      0,                      /* endpoint # */
      0,                      /* direction */
      0,                      /* pipe type */
      0,                      /* speed */
      1,                      /* tr's per uframe */
      0,                      /* closed by default */
      0,                      /* start with DATA0 */
      NULL                    /* no transaction scheduled */
   }
};

/* Unpublished functions, not intended for application use */
static void  usb_dev_list_close_pipe(_usb_host_handle, PIPE_STRUCT_PTR);

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_free_memlist
* Returned Value : USB_OK if valid device, else error code
* Comments       :
*     Close pipes & free memory on device's linked list, whose
*        anchor is dev_handle-->memlist
*     
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_free_memlist
   (
      /* [IN] Handle for the USB device */
      _usb_device_instance_handle   dev_handle
   )
{ /* Body */
   DEV_INSTANCE_PTR  dev;
   DEV_MEMORY_PTR    mem_ptr, list_ptr;
   USB_STATUS        error;
   uint_8            intf_no;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_free_memlist");
   #endif

   error = usb_hostdev_validate(dev_handle);
   // EAI - missing {} so failed if debug on
   if (error != USB_OK) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_free_memlist invalid device handle");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }
   dev = (DEV_INSTANCE_PTR)dev_handle;

   /* Deleting interfaces closes their pipes */
   for (intf_no = 0; intf_no < dev->num_of_interfaces; intf_no++)
      usb_hostdev_delete_interface(dev, dev->intf_descriptor[intf_no]);

   /* Free memory blocks on this device's list */
   USB_lock();
   mem_ptr = dev->memlist;

   while (mem_ptr != NULL) {
      list_ptr = mem_ptr->next;
      USB_mem_free(mem_ptr);
      mem_ptr = list_ptr;
   }

   dev->memlist = NULL;
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_free_memlist SUCCESSFUL");
   #endif
   return USB_OK;

} /* Endbody */
  
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_free_mem
* Returned Value : USB_OK if memory is freed, else error code
* Comments       :
*     Memory is unlinked and freed from dev_ptr->memlist
*     
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_free_mem
   (
      /* [IN] Pointer to the USB device */
      DEV_INSTANCE_PTR        dev_ptr,

      /* [IN] Pointer to memory */
      DEV_MEMORY_PTR          header_ptr
   )
{ /* Body */

   DEV_MEMORY_PTR  mem_ptr, list_ptr;
   DEV_MEMORY_PTR  prev_ptr = NULL;
   USB_STATUS      error;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_free_mem");
   #endif

   USB_lock();
   error = usb_hostdev_validate((_usb_device_instance_handle)dev_ptr);
   USB_unlock();

   if (error != USB_OK) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_free_mem invalid device handle");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }
   mem_ptr = (DEV_MEMORY_PTR) header_ptr;
   
   if (mem_ptr == NULL)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_free_mem invalid memory pointer");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_MEM_TYPE);
   } /* Endif */

   for (list_ptr = dev_ptr->memlist; list_ptr != NULL; list_ptr = list_ptr->next) {
      if (list_ptr == mem_ptr)
         break;
      prev_ptr = list_ptr;
   }

   if (list_ptr == NULL) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_free_mem not found");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_NOT_FOUND);
   }

   /* Unlink the buffer from the list */
   if (dev_ptr->memlist == mem_ptr) {
      /* Unlink from anchor */
      /* WARNING!!! The first buffer in the chain is the full configuration descriptor,
      ** so there is probably some issue if there is attempt to free it, because
      ** configuration descriptor must have its lifetime till the device's detach or
      ** till selecting another configuration.
      */
      dev_ptr->memlist = mem_ptr->next;
   } else {
      if(prev_ptr != NULL) {
         /* Unlink from chain */
         prev_ptr->next = mem_ptr->next;
      }
   }

   /* Free associated memory */
   USB_mem_free(mem_ptr);

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_free_mem SUCCESSFUL");
   #endif

   return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_get_mem
* Returned Value : USB_OK if memory allocated, else error code
* Comments       :
*     Memory is added at the end of a linked list, whose
*        anchor is device.memlist 
*     
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_get_mem
   (
      /* [IN] Pointer to the USB device */
      DEV_INSTANCE_PTR        dev_ptr,

      /* [IN] Size of memory payload required */
      uint_32                 mem_size,

      /* [IN] Type of memory payload required */
      memory_type             mem_type,

      /* [IN] Alignment required */
      uint_32                 align,

      /* [OUT] Pointer to memory block's header */
      DEV_MEMORY_PTR _PTR_    header_ptr
   )
{ /* Body */

   DEV_MEMORY_PTR  mem_ptr, list_ptr;
   USB_STATUS      error;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_get_mem");
   #endif

   if (align > 256)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_get_mem invalid alignment");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_BAD_ALIGNMENT);
   }

   if ((mem_type <= USB_MEMTYPE_MIN_VALUE) || (mem_type >= USB_MEMTYPE_MAX_VALUE))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_get_mem invalid memory type");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_MEM_TYPE);
   }

   USB_lock();
   error = usb_hostdev_validate((_usb_device_instance_handle)dev_ptr);

   if (error != USB_OK) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_get_mem invalid device handle");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }
   
   /* get memory for header + payload, rounded up to align bytes */
   mem_ptr = (DEV_MEMORY_PTR) USB_mem_alloc_uncached_zero(MEM_HEADER_LEN + mem_size +  align - 1);

   if (mem_ptr == NULL) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_get_mem malloc FAILED");
      #endif

      return USB_log_error(__FILE__,__LINE__,USBERR_GET_MEMORY_FAILED);
   }
   _mem_set_type(mem_ptr, MEM_TYPE_USB_LIST_MEM_BUFF);
   
   if (dev_ptr->memlist == NULL) {
      dev_ptr->memlist = mem_ptr;
   } else {
      list_ptr = dev_ptr->memlist;

      while (list_ptr->next != NULL)
         list_ptr = list_ptr->next;

      list_ptr->next = mem_ptr;   
   } /* EndIf */

   mem_ptr->next = NULL;
   mem_ptr->blktype = mem_type;
   mem_ptr->blksize = mem_size;
   mem_ptr->offset = (~(uint_32) mem_ptr->payload.data + 1) & (align - 1);
   if (mem_ptr->offset) {
      /* Real payload is by offset bytes shifted from original payload. We want to write offset in the byte just before
      ** real payload. */
      mem_ptr->payload.data[mem_ptr->offset - 1] = mem_ptr->offset; /* write offset value also 1 byte before payload */
   }
   *header_ptr = (pointer)mem_ptr;

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_get_mem SUCCESSFUL");
   #endif
   return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_attach_device
* Returned Value : 
* Comments       :
*     This function will be called when attach interrupt happens, to
*       add onto the device list and do common initialization.     
* 
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_attach_device
   (
      _usb_host_handle  handle,
      uint_8            speed,
      uint_8            hub_no,
      uint_8            port_no
   )
{ /* Body */
   USB_STATUS                 status;
   DEV_INSTANCE_PTR           new_instance_ptr, dev_instance_ptr, dev_instance_prev_ptr;
   PIPE_INIT_PARAM_STRUCT     ctrl_pipe_init_params;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   DEV_INSTANCE_PTR           device_root = NULL;
#ifdef __USB_OTG__
   uint_32                    state;
#endif

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_attach_device attach device");
   #endif
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   /* Allocate new device instance */
   new_instance_ptr = (DEV_INSTANCE_PTR) USB_mem_alloc_uncached(sizeof(DEV_INSTANCE));

   if (new_instance_ptr == NULL) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_attach_device failed to malloc device handle");
      #endif
      return USB_log_error(__FILE__,__LINE__, USBERR_GET_MEMORY_FAILED);
   } /* EndIf */
   _mem_set_type(new_instance_ptr, MEM_TYPE_USB_HOST_STATE_STRUCT);

   USB_mem_zero(new_instance_ptr, sizeof(DEV_INSTANCE));

   new_instance_ptr->host = handle;
   new_instance_ptr->speed = speed;
   new_instance_ptr->hub_no = hub_no;
   new_instance_ptr->port_no = port_no;
   new_instance_ptr->cfg_value = 0; /* We don't know yet what the device's default configuration is */

   USB_lock();

   /* Find unused address from 1 - 127 for this host */
   dev_instance_ptr = usb_host_ptr->DEVICE_LIST_PTR;
   if ((dev_instance_ptr == NULL) || (dev_instance_ptr->address != 1)) {
      /* Insert at the beginning of list */
      new_instance_ptr->address = 1;
      new_instance_ptr->next = dev_instance_ptr;
      usb_host_ptr->DEVICE_LIST_PTR = new_instance_ptr;
   }
   else {
      dev_instance_prev_ptr = dev_instance_ptr;
      /* Searching for a 'hole' in devices instance adresses */
      while (dev_instance_ptr->address <= (dev_instance_prev_ptr->address + 1)) {
         new_instance_ptr->address = dev_instance_ptr->address;
         dev_instance_prev_ptr = dev_instance_ptr;
         dev_instance_ptr = dev_instance_ptr->next;
         if (dev_instance_ptr == NULL)
            break;
      } /* EndWhile */
      if (new_instance_ptr->address >= 127) {
         /* If all 127 addresses used up, delete instance & bail out */
         USB_unlock();
         USB_mem_free((pointer)new_instance_ptr);
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_dev_list_attach_device out of addresses");
         #endif
         return USB_log_error(__FILE__,__LINE__, USBERR_ADDRESS_ALLOC_FAILED);
      } /* EndIf */
      new_instance_ptr->address++;
      new_instance_ptr->next = dev_instance_ptr;
      dev_instance_prev_ptr->next = new_instance_ptr;
   };
      
   USB_unlock();

   /*-----------------------------------------------------------**
   ** Open control pipe, get first 8 bytes of device descriptor **
   ** The host_ch9 routine internally sets the callback to      **
   **    usb_host_cntrl_transaction_done (in usb_host_ch9.c)    **
   **    where the action resumes on completion of the get.     **
   **-----------------------------------------------------------*/
   
   ctrl_pipe_init_params = pipe_init_params_prototype;
   ctrl_pipe_init_params.G.DEV_INSTANCE = new_instance_ptr;
   ctrl_pipe_init_params.G.PIPETYPE = USB_CONTROL_PIPE;   
   ctrl_pipe_init_params.G.SPEED = new_instance_ptr->speed;

   ctrl_pipe_init_params.G.HUB_DEVICE_ADDR = hub_no;
   ctrl_pipe_init_params.G.HUB_PORT_NUM = port_no;
#ifdef __USB_OTG__
   _usb_otg_get_status((pointer)usb_otg_state_struct_ptr, 
      USB_OTG_STATE, &state);
   
   if (state < B_IDLE) {
      _usb_otg_set_status((pointer)usb_otg_state_struct_ptr, 
         USB_OTG_A_BUS_REQ, TRUE);
      _usb_otg_set_status((pointer)usb_otg_state_struct_ptr, 
         USB_OTG_B_CONN, TRUE);
   } else {
      _usb_otg_set_status((pointer)usb_otg_state_struct_ptr, 
         USB_OTG_A_CONN, TRUE);
   } /* Endif */
#endif

   if (USB_OK != _usb_host_open_pipe(new_instance_ptr->host, 
      &ctrl_pipe_init_params, 
      &new_instance_ptr->control_pipe))
   {
      USB_mem_free((pointer)new_instance_ptr);
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_attach_device open pipe failed");
      #endif
      return USB_log_error(__FILE__,__LINE__, USBERR_PIPE_OPENED_FAILED);
   } /* Endif */

   /* Set state to enter after transaction completion */
   new_instance_ptr->state = DEVSTATE_DEVDESC8;

   status = _usb_host_ch9_get_descriptor((_usb_device_instance_handle)new_instance_ptr,
      USB_DESC_TYPE_DEV << 8, 0, 8, 
      (uchar_ptr)&new_instance_ptr->dev_descriptor);
      
   if (status != USB_STATUS_TRANSFER_QUEUED)
   {
      new_instance_ptr->state = DEVSTATE_INITIAL;
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_attach_device FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__, USBERR_NO_DESCRIPTOR);
   }
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_attach_device SUCCESSFUL");
   #endif
   return USB_OK;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_close_pipe
* Returned Value : 
* Comments       :
*     Cancel pending TR's and close a pipe.
* 
*END*--------------------------------------------------------------------*/
static void  usb_dev_list_close_pipe
   (
      _usb_host_handle              handle,
      PIPE_STRUCT_PTR               pipe_ptr
   )
{ /* Body */
   USB_STATUS                       status;
   USB_HOST_STATE_STRUCT_PTR        usb_host_ptr;
   TR_STRUCT_PTR                    tr_ptr;   

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_close_pipe");
   #endif

   if (pipe_ptr == NULL)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_close_pipe NULL pipe");
      #endif
      return;
   }

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
      
   USB_lock();

   /* Cancel all active TR's in the pipe */
   tr_ptr = pipe_ptr->tr_list_ptr;
   if (tr_ptr != NULL) {
      do {
         tr_ptr->CALLBACK = NULL;
         if (tr_ptr->TR_INDEX != 0) {
            /* Cancel the transfer at low-level */
            status = _usb_host_cancel_call_interface(handle, pipe_ptr, tr_ptr);
         
            if (status != USB_OK)
            {
               #ifdef _HOST_DEBUG_
                  DEBUG_LOG_TRACE("usb_dev_list_close_pipe failed to cancel transaction");
               #endif
            }
         }

         tr_ptr->STATUS = USB_STATUS_IDLE;
         /* Indicate that the TR is idle */
         tr_ptr->TR_INDEX = 0;
         tr_ptr = tr_ptr->NEXT;
      } while (tr_ptr != pipe_ptr->tr_list_ptr); //this indicates end of queue
   } /* EndWhile */

   /* zero the pipe descriptor, except keep TR list */
   tr_ptr = pipe_ptr->tr_list_ptr;
   _usb_host_close_pipe(handle, (pointer)pipe_ptr);
   pipe_ptr->tr_list_ptr = tr_ptr;   

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_close_pipe SUCCESSFUL");
   #endif

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_close_pipe_bundle
* Returned Value : 
* Comments       :
*    Close all pipes in an interface's bundle.
* 
*END*--------------------------------------------------------------------*/
void  usb_dev_list_close_pipe_bundle
   (
      PIPE_BUNDLE_STRUCT_PTR    pbs_ptr
   )
{ /* Body */

   _usb_host_handle  handle;
   uint_8            index, index_max;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_close_pipe_bundle");
   #endif
   
   handle = ((DEV_INSTANCE_PTR)pbs_ptr->dev_handle)->host;
   index_max = ((INTERFACE_DESCRIPTOR_PTR) pbs_ptr->intf_handle)->bNumEndpoints;

   for (index = 0; index < index_max; index++)
      usb_dev_list_close_pipe(handle, (PIPE_STRUCT_PTR)pbs_ptr->pipe_handle[index]); 

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_close_pipe_bundle SUCCESSFUL");
   #endif

} /* EndBody */
    

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_detach_device
* Returned Value : 
* Comments       :
*     This function will be called when detach interrupt happens.
* 
*END*--------------------------------------------------------------------*/
void  usb_dev_list_detach_device
   (
      _usb_host_handle  handle,
      uint_8            hub_no,
      uint_8            port_no
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   DEV_INSTANCE_PTR  dev_instance_ptr;
   DEV_INSTANCE_PTR  _PTR_ device_root = (DEV_INSTANCE_PTR _PTR_)&usb_host_ptr->DEVICE_LIST_PTR;
#ifdef __USB_OTG__
   uint_32           state;
#endif
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_detach_device");
   #endif

   /* search device list for the one being detached */
   USB_lock();
   for (dev_instance_ptr = *device_root;   
      dev_instance_ptr != NULL;
      dev_instance_ptr = dev_instance_ptr->next)   
   {
      if (dev_instance_ptr->port_no != port_no)
         continue;
      if (dev_instance_ptr->hub_no != hub_no)
         continue;
      if (dev_instance_ptr->host != handle)
         continue;
      break;
   } /* EndFor */

   USB_unlock();

   if (dev_instance_ptr == NULL) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_dev_list_detach_device NULL device pointer");
      #endif
      return;  /* No match, abandon ship! */
   } /* Endif */

   /* SGarg 09/23/2003 . Sometimes  a detach could come even before a 
   attach is finished. This means that we should make sure that device
   memory is not NULL before we try to clean it up. For example this happens
   with OPT tester test 4.10.*/
   
   if(dev_instance_ptr->memlist != NULL)
   {
      /* Notify the application of unavailable interfaces */
      usb_hostdev_attach_detach(dev_instance_ptr, USB_DETACH_EVENT);
      
   }

   /* Unlink device from the instance list */
   dev_instance_ptr->control_callback = NULL; /* no surprises */

   /* Close control pipe */
   usb_dev_list_close_pipe(handle, (PIPE_STRUCT_PTR)dev_instance_ptr->control_pipe);
   
   /* SGarg 09/24/2003 . Sometimes  a detach could come even before a 
   attach is finished. This means that we should make sure that device
   memory is not NULL before we try to clean it up. For example this happens
   with OPT tester test 4.10.*/
   if(dev_instance_ptr->memlist != NULL)
   {
      usb_dev_list_free_memlist((_usb_device_instance_handle)dev_instance_ptr);
   }

   USB_lock();

   /* Find the address of the "pointer to the detached device" */
   while (*device_root != dev_instance_ptr) {
      device_root = &(*device_root)->next;
   } /* Endwhile */

   /* Remove the device */
   *device_root = dev_instance_ptr->next;

   USB_mem_free(dev_instance_ptr);

#ifdef __USB_OTG__
   _usb_otg_get_status((pointer)usb_otg_state_struct_ptr, 
      USB_OTG_STATE, &state);

   if (state < B_IDLE) {
      _usb_otg_get_status((pointer)usb_otg_state_struct_ptr,
         USB_OTG_B_CONN, &state);
      if (state) {
         _usb_otg_set_status((pointer)usb_otg_state_struct_ptr, 
            USB_OTG_B_CONN, FALSE);
      } /* Endif */
   } else {
      _usb_otg_get_status((pointer)usb_otg_state_struct_ptr,
         USB_OTG_A_CONN, &state);
      if (state) {
         _usb_otg_set_status((pointer)usb_otg_state_struct_ptr,
            USB_OTG_A_CONN, FALSE);
      } /* Endif */
   } /* Endif */
#endif

   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_dev_list_detach_device SUCCESSFUL");
   #endif

} /* EndBody */

/* EOF */
