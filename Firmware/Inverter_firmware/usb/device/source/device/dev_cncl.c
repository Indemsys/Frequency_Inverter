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
* $FileName: dev_cncl.c$
* $Version : 3.8.3.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*  This file contains USB device API specific function to cancel 
* transfer.
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
*  Function Name  : _usb_device_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_cancel_transfer
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ 
    uint_8                        error = USB_OK;
    USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
    
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    
    USB_lock();
    
    /* Cancel transfer on the specified endpoint for the specified 
    ** direction 
    */
    if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_CANCEL_TRANSFER != NULL)   
    {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_CANCEL_TRANSFER(handle, 
            ep_num, direction);
    }
    else
    {
        #if _DEBUG
            printf("_usb_device_cancel_transfer: DEV_CANCEL_TRANSFER is NULL\n");               
        #endif  
        return USBERR_ERROR;
    }

    USB_unlock();

    return error;
}

/* EOF */
