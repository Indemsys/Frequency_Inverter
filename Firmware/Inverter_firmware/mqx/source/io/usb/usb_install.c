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
* $FileName: usb_install.c$
* $Version : 3.8.3.0$
* $Date    : Aug-21-2012$
*
* Comments:
*
*   This file contains board-specific USB initialization functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_driver_install
*  Returned Value : None
*  Comments       :
*        Installs the device
*END*-----------------------------------------------------------------*/
_mqx_int _usb_device_driver_install
(
   /* [IN] address if the callback functions structure */
   USB_DEV_IF_STRUCT_PTR    usb_if
)
{ /* Body */
   USB_DEV_IF_STRUCT_PTR usb_c;
   _mqx_int      i, f;
   
   /* Check usb_if structure */
   if (usb_if == NULL) {
         return IO_ERROR;
   }
   
   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);
   
   if (!usb_c) {
      usb_c = _mem_alloc_zero(USBCFG_MAX_DRIVERS * sizeof(USB_DEV_IF_STRUCT));
      
      if (!usb_c) {
         return IO_ERROR;
      } /* Endif */
      /* TODO: change memory type */
      //_mem_set_type(usb_c, MEM_TYPE_USB_CALLBACK_STRUCT);
      _mem_transfer(usb_c, _task_get_id(), _mqx_get_system_task_id()); //this should return failure value anytime
	  _mqx_set_io_component_handle(IO_USB_COMPONENT, (pointer) usb_c);
      i = USBCFG_MAX_DRIVERS;
   }
   else {
      /* Find out if the interface has not been already installed */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].DEV_INIT_PARAM == usb_if->DEV_INIT_PARAM) {
            /* The interface was already installed, skip search */
            break;
         }
      }
   }

   if (i == USBCFG_MAX_DRIVERS) {
      /* The interface was not installed yet, search for a free position. */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].DEV_INIT_PARAM == NULL) {
            usb_c[i].DEV_IF = usb_if->DEV_IF;
            usb_c[i].DEV_INIT_PARAM = usb_if->DEV_INIT_PARAM;
            usb_c[i].DEV_HANDLE = NULL; /* not initialized yet */
            break;
         }
      }
   }
   else {
      /* The interface was already installed */
   }
   
   if (i == USBCFG_MAX_DRIVERS) {
      /* Not enough space to install interface */
      return IO_ERROR;
   }

   return MQX_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dev_driver_uninstall
*  Returned Value : USB_OK or error code
*  Comments       :
*        Uninstalls the device
*END*-----------------------------------------------------------------*/
_mqx_int _usb_device_driver_uninstall
   (
		void
   )
{ /* Body */
   pointer callback_struct_table_ptr;
   callback_struct_table_ptr = _mqx_get_io_component_handle(IO_USB_COMPONENT);
   
   if (callback_struct_table_ptr) 
   {
      _mem_free(callback_struct_table_ptr); 
   }
   else
   {
      return IO_ERROR; 
   }
   /* Endif */
   _mqx_set_io_component_handle(IO_USB_COMPONENT, NULL);
   return MQX_OK;

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_driver_install
*  Returned Value : None
*  Comments       :
*        Installs the host controller
*END*-----------------------------------------------------------------*/
_mqx_int _usb_host_driver_install
(
   /* [IN] address if the callback functions structure */
   USB_HOST_IF_STRUCT_PTR    usb_if
)
{ /* Body */
   USB_HOST_IF_STRUCT_PTR usb_c;
   _mqx_int      i, f;
   
   /* Check usb_if structure */
   if (usb_if == NULL) {
         return IO_ERROR;
   }
   
   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);
   
   if (!usb_c) {
      usb_c = _mem_alloc_zero(USBCFG_MAX_DRIVERS * sizeof(USB_HOST_IF_STRUCT));
      
      if (!usb_c) {
         return IO_ERROR;
      } /* Endif */
      /* TODO: change memory type */
      //_mem_set_type(usb_c, MEM_TYPE_USB_CALLBACK_STRUCT);
      _mem_transfer(usb_c, _task_get_id(), _mqx_get_system_task_id()); //this should return failure value anytime
      _mqx_set_io_component_handle(IO_USB_COMPONENT, (pointer) usb_c);
      i = USBCFG_MAX_DRIVERS;
   }
   else {
      /* Find out if the interface has not been already installed */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].HOST_INIT_PARAM == usb_if->HOST_INIT_PARAM) {
            /* The interface was already installed, skip search */
            break;
         }
      }
   }

   if (i == USBCFG_MAX_DRIVERS) {
      /* The interface was not installed yet, search for a free position. */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].HOST_INIT_PARAM == NULL) {
            usb_c[i].HOST_IF = usb_if->HOST_IF;
            usb_c[i].HOST_INIT_PARAM = usb_if->HOST_INIT_PARAM;
            usb_c[i].HOST_HANDLE = NULL; /* not initialized yet */
            break;
         }
      }
   }
   else {
      /* The interface was already installed */
   }
   
   if (i == USBCFG_MAX_DRIVERS) {
      /* Not enough space to install interface */
      return IO_ERROR;
   }

   return MQX_OK;

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_init
*  Returned Value : None
*  Comments       :
*        Gets the init parameters for the driver
*END*-----------------------------------------------------------------*/
pointer _usb_host_get_init
(
   /* [IN] address if the callback functions structure */
   pointer    host_handle
)
{ /* Body */
   USB_HOST_IF_STRUCT_PTR usb_c;
   pointer retval = NULL;
   int i;
   
   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);
   
   if (usb_c) {
      /* Find out if the interface has not been already installed */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].HOST_HANDLE == host_handle) {
            retval = usb_c[i].HOST_INIT_PARAM;
            break;
         }
      }
   }

   return retval;

} /* EndBody */

/* EOF */
