#ifndef __mqx_dev_h__
#define __mqx_dev_h__
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
* $FileName: mqx_dev.h$
* $Version : 3.8.13.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
* Description:      
*  This file contains device-specifc defines for MQX RTOS.
*                                                               
*END*********************************************************************/

#include <mqx.h>
#include <psp.h>
#include <bsp.h>
#include "devapi.h"

/*
**
** Low-level function list structure for USB
**
** This is the structure used to store chip specific functions to be called 
** by generic layer
*/
struct xd_struct;

typedef struct usb_dev_callback_functions_struct
{
   /* The Host/Device init function */
   USB_STATUS (_CODE_PTR_ DEV_PREINIT)(_usb_device_handle _PTR_);

   /* The Host/Device init function */
   USB_STATUS (_CODE_PTR_ DEV_INIT)(_usb_device_handle);

   /* The function to send data */
   USB_STATUS (_CODE_PTR_ DEV_SEND)(_usb_device_handle, struct xd_struct *);

   /* The function to receive data */
   USB_STATUS (_CODE_PTR_ DEV_RECV)(_usb_device_handle, struct xd_struct *);
   
   /* The function to cancel the transfer */
   USB_STATUS (_CODE_PTR_ DEV_CANCEL_TRANSFER)(_usb_device_handle, uint_8, uint_8);
   
   USB_STATUS (_CODE_PTR_ DEV_INIT_ENDPOINT)(_usb_device_handle, struct xd_struct *);
   
   USB_STATUS (_CODE_PTR_ DEV_DEINIT_ENDPOINT)(_usb_device_handle, uint_8, uint_8);
   
   USB_STATUS (_CODE_PTR_ DEV_UNSTALL_ENDPOINT)(_usb_device_handle, uint_8, uint_8);
   
   USB_STATUS (_CODE_PTR_ DEV_GET_ENDPOINT_STATUS)(_usb_device_handle, uint_8, uint_16_ptr);
   
   USB_STATUS (_CODE_PTR_ DEV_SET_ENDPOINT_STATUS)(_usb_device_handle, uint_8, uint_16);

   USB_STATUS (_CODE_PTR_ DEV_GET_TRANSFER_STATUS)(_usb_device_handle, uint_8, uint_8);
   
   USB_STATUS (_CODE_PTR_ DEV_SET_ADDRESS)(_usb_device_handle, uint_8);
   
   USB_STATUS (_CODE_PTR_ DEV_SHUTDOWN)(_usb_device_handle);
   
   USB_STATUS (_CODE_PTR_ DEV_GET_SETUP_DATA)(_usb_device_handle, uint_8, uint_8_ptr);
   
   USB_STATUS (_CODE_PTR_ DEV_ASSERT_RESUME)(_usb_device_handle);
   
   USB_STATUS (_CODE_PTR_ DEV_STALL_ENDPOINT)(_usb_device_handle, uint_8, uint_8);

} USB_DEV_CALLBACK_FUNCTIONS_STRUCT, _PTR_ USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR;

#define USB_uint_16_low(x)          ((x) & 0xFF)
#define USB_uint_16_high(x)         (((x) >> 8) & 0xFF)

#ifdef __cplusplus
extern "C" {
#endif
extern uint_8 _usb_dev_driver_install(uint_32, pointer);
extern uint_8 _usb_dev_driver_uninstall(void);
#ifdef __cplusplus
}
#endif

#endif
