#ifndef __mqx_host_h__
#define __mqx_host_h__
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
* $FileName: mqx_host.h$
* $Version : 3.8.12.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains generic and OHCI-specific implementations of USB
*   interfaces
*
*END************************************************************************/

#include "host_common.h"
#include "usb_mqx.h"
#include "usb_bsp.h"

/*
**
** Low-level function list structure for USB
**
** This is the structure used to store chip specific functions to be called 
** by generic layer
*/
typedef struct usb_host_callback_functions_struct
{
   /* The Host/Device pre-init function */
   USB_STATUS (_CODE_PTR_ HOST_PREINIT)(_usb_host_handle _PTR_);

   /* The Host/Device init function */
   USB_STATUS (_CODE_PTR_ HOST_INIT)(_usb_host_handle);

   /* The function to shutdown the host/device */
   USB_STATUS (_CODE_PTR_ HOST_SHUTDOWN)(_usb_host_handle);

   /* The function to send data */
   USB_STATUS (_CODE_PTR_ HOST_SEND)(_usb_host_handle, struct pipe_struct _PTR_, struct tr_struct _PTR_);

   /* The function to send setup data */
   USB_STATUS (_CODE_PTR_ HOST_SEND_SETUP)(_usb_host_handle, struct pipe_struct _PTR_, struct tr_struct _PTR_);

   /* The function to receive data */
   USB_STATUS (_CODE_PTR_ HOST_RECV)(_usb_host_handle, struct pipe_struct _PTR_, struct tr_struct _PTR_);
   
   /* The function to cancel the transfer */
   USB_STATUS (_CODE_PTR_ HOST_CANCEL)(_usb_host_handle, struct pipe_struct _PTR_, struct tr_struct _PTR_);
   
   /* The function for USB bus control */
   USB_STATUS (_CODE_PTR_ HOST_BUS_CONTROL)(_usb_host_handle, uint_8);

   USB_STATUS (_CODE_PTR_ HOST_ALLOC_BANDWIDTH)(_usb_host_handle, struct pipe_struct _PTR_);

   USB_STATUS (_CODE_PTR_ HOST_FREE_CONTROLLER_RESOURCE)(_usb_host_handle, struct pipe_struct _PTR_);
  
   uint_32 (_CODE_PTR_ HOST_GET_FRAME_NUM)(_usb_host_handle);   
   
   uint_32 (_CODE_PTR_ HOST_GET_MICRO_FRAME_NUM)(_usb_host_handle);   
   
   USB_STATUS (_CODE_PTR_ HOST_OPEN_PIPE)(_usb_host_handle, struct pipe_struct _PTR_);
   
   USB_STATUS (_CODE_PTR_ HOST_UPDATE_MAX_PACKET_SIZE)(_usb_host_handle, struct pipe_struct _PTR_);
   
   USB_STATUS (_CODE_PTR_ HOST_UPDATE_DEVICE_ADDRESS)(_usb_host_handle, struct pipe_struct _PTR_);

} USB_HOST_CALLBACK_FUNCTIONS_STRUCT, _PTR_ USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
