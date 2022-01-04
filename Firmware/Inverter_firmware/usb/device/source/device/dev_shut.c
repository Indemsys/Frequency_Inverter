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
* $FileName: dev_shut.c$
* $Version : 3.8.3.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*  This file contains USB device API specific function to shutdown 
*  the device.
*                                                               
*END*********************************************************************/
#include <mqx.h>

#include "usb.h"     //common public
#include "usb_prv.h" //common private

#include "dev_cnfg.h"
#include "devapi.h"  //device public
#include "dev_main.h" //device private

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_shutdown
*  Returned Value : USB_OK or error code
*  Comments       :
*        Shutdown an initialized USB device
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_shutdown
(
    /* [IN] the USB_USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{ 
    USB_STATUS                        error;
    volatile USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;
   
    if (handle == NULL)
    {
        #if _DEBUG
            printf("_usb_device_shutdown: handle is NULL\n");
        #endif  
        return USBERR_ERROR;
    }

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    if ( ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
        usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SHUTDOWN != NULL)
    {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SHUTDOWN(usb_dev_ptr);
    }
    else
    {
        #if _DEBUG
            printf("_usb_device_shutdown: DEV_SHUTDOWN is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
      
    return  error;   
} /* EndBody */
