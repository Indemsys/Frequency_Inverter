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
* $FileName: dev_ep_deinit.c$
* $Version : 3.8.3.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*  This file contains USB device API specific function to 
* deinitialize the endpoint.
*                                                               
*END*********************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "dev_cnfg.h"
#include "devapi.h"
#include "dev_main.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_deinit_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*  Disables the endpoint and the data structures associated with the 
*  endpoint
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_deinit_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                    ep_num,
            
      /* [IN] Direction */
      uint_8                    direction
   )
{ /* Body */
   uint_8         error = 0;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

   if (handle == NULL)
   {
      return USBERR_ERROR;
   }
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   USB_lock();

   if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_DEINIT_ENDPOINT != NULL) 
   {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_DEINIT_ENDPOINT(handle, 
            ep_num, direction);
    
   }
   else
   {
        #if _DEBUG
            printf("_usb_device_deinit_endpoint: DEV_DEINIT_ENDPOINT is NULL\n");                     
        #endif  
        return USBERR_ERROR;
   }
   
   USB_unlock();
   return error;
} 

/* EOF */
