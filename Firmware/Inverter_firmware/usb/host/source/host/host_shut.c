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
* $FileName: host_shut.c$
* $Version : 3.8.9.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the USB Host API specific function to shutdown 
*   the host
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_shut.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_shutdown
*  Returned Value : USB_OK or error code
*  Comments       :
*        Shutdown an initialized USB Host
*
*END*-----------------------------------------------------------------*/
void _usb_host_shutdown
   (
      /* [IN] the USB_host handle */
      _usb_host_handle         handle
   )
{ /* Body */
   USB_STATUS                 status;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_shutdown");
   #endif


   /* De-initialize and disconnect the host hardware from the bus */
   status = _usb_host_shutdown_call_interface (handle);
   
   if (status != USB_OK)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_shutdown FAILED");
      #endif
   }

   /* Free all Pipe Descriptors */
   USB_mem_free((pointer)usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);

   /* Free the USB state structure */
   USB_mem_free(handle);
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_shutdown SUCCESSFUL");
   #endif

   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_bus_control
*  Returned Value : None
*  Comments       :
* _usb_host_bus_control controls the bus operations such as asserting and 
* deasserting the bus reset, asserting and deasserting resume, 
* suspending and resuming the SOF generation
*
*END*-----------------------------------------------------------------*/

void _usb_host_bus_control
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           handle,
      
      /* The operation to be performed on the bus */
      uint_8                     bcontrol
   )
{ /* Body */

   USB_STATUS status;
   USB_HOST_STATE_STRUCT_PTR         usb_host_ptr;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_bus_control");
   #endif

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   status = _usb_host_bus_control_data_call_interface (handle, bcontrol);
   
   if (status != USB_OK)
   {
      #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_bus_control FAILED");
      #endif
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_bus_control SUCCESSFUL");
   #endif
  
} /* Endbody */

/* EOF */
