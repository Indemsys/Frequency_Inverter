#ifndef __devapi_h__
#define __devapi_h__ 1
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
* $FileName: devapi.h$
* $Version : 3.8.6.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
*  This file contains the declarations specific to the USB Device API
*
*END*********************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Informational Request/Set Types */
#define  USB_STATUS_DEVICE_STATE          (0x01)
#define  USB_STATUS_INTERFACE             (0x02)
#define  USB_STATUS_ADDRESS               (0x03)
#define  USB_STATUS_CURRENT_CONFIG        (0x04)
#define  USB_STATUS_SOF_COUNT             (0x05)
#define  USB_STATUS_DEVICE                (0x06)
#define  USB_STATUS_TEST_MODE             (0x07)
#define  USB_STATUS_ENDPOINT              (0x10)
#define  USB_STATUS_ENDPOINT_NUMBER_MASK  (0x0F)

#define  USB_TEST_MODE_TEST_PACKET        (0x0400)

/* Available service types */
/* Services 0 through 15 are reserved for endpoints */
#define  USB_SERVICE_EP0                  (0x00)
#define  USB_SERVICE_EP1                  (0x01)
#define  USB_SERVICE_EP2                  (0x02)
#define  USB_SERVICE_EP3                  (0x03)
#define  USB_SERVICE_BUS_RESET            (0x10)
#define  USB_SERVICE_SUSPEND              (0x11)
#define  USB_SERVICE_SOF                  (0x12)
#define  USB_SERVICE_RESUME               (0x13)
#define  USB_SERVICE_SLEEP                (0x14)
#define  USB_SERVICE_SPEED_DETECTION      (0x15)
#define  USB_SERVICE_ERROR                (0x16)
#define  USB_SERVICE_STALL                (0x17)

#define USB_CONTROL_ENDPOINT              (0)
#define USB_SETUP_PKT_SIZE                (8)/* Setup Packet Size */
#define  USB_UNINITIALIZED_VAL_32         (0xFFFFFFFF)
#define COMPONENT_PREPARE_SHIFT           (7)

/* Common Data Structures */
typedef struct _USB_SETUP_STRUCT
{
   uint_8      request_type;
   uint_8      request;
   uint_16     value;
   uint_16     index;
   uint_16     length;
} USB_SETUP_STRUCT, _PTR_ USB_SETUP_STRUCT_PTR;

typedef struct _USB_EP_STRUCT
{
  uint_8          ep_num;      /* endpoint number         */
  uint_8          type;        /* type of endpoint        */
  uint_8          direction;   /* direction of endpoint   */
  uint_32         size;        /* buffer size of endpoint */
}USB_EP_STRUCT;

typedef USB_EP_STRUCT* USB_EP_STRUCT_PTR;


typedef struct _USB_EVENT_STRUCT
{
  _usb_device_handle handle;          /* conttroler device handle*/
  uint_8          ep_num;
  boolean         setup;              /* is setup packet         */
  boolean         direction;          /* direction of endpoint   */
  uint_8_ptr      buffer_ptr;         /* pointer to buffer       */
  uint_32         len;                /* buffer size of endpoint */
} USB_EVENT_STRUCT, *PTR_USB_EVENT_STRUCT;

typedef struct {
   pointer              PRIVATE;
   void (_CODE_PTR_     FREE)(_usb_device_handle, pointer);
   pointer              XD_FOR_THIS_DTD;
} SCRATCH_STRUCT, _PTR_ SCRATCH_STRUCT_PTR;

/* Callback function storage structure */
typedef struct service_struct 
{
   uint_8   TYPE;
   void     (_CODE_PTR_ SERVICE)(PTR_USB_EVENT_STRUCT,pointer arg);
   pointer  arg;
   struct service_struct _PTR_   NEXT;
} SERVICE_STRUCT, _PTR_ SERVICE_STRUCT_PTR;

void _usb_device_free_XD(_usb_device_handle, pointer  xd_ptr);
USB_STATUS _usb_device_init(struct usb_dev_if_struct *, _usb_device_handle _PTR_, _mqx_uint);
USB_STATUS _usb_device_deinit(_usb_device_handle);
USB_STATUS _usb_device_recv_data(_usb_device_handle, uint_8, uint_8_ptr, uint_32);
USB_STATUS _usb_device_send_data(_usb_device_handle, uint_8, uint_8_ptr, uint_32);
USB_STATUS _usb_device_get_transfer_status(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_device_cancel_transfer(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_device_get_status(_usb_device_handle, uint_8, uint_16_ptr);
USB_STATUS _usb_device_set_status(_usb_device_handle, uint_8, uint_16);
USB_STATUS _usb_device_register_service(_usb_device_handle, uint_8,
   void(_CODE_PTR_ service)(PTR_USB_EVENT_STRUCT, pointer),pointer arg);
USB_STATUS _usb_device_unregister_service(_usb_device_handle, uint_8);
USB_STATUS _usb_device_call_service(uint_8, PTR_USB_EVENT_STRUCT);
USB_STATUS _usb_device_shutdown(_usb_device_handle);
USB_STATUS _usb_device_set_address(_usb_device_handle, uint_8);
USB_STATUS _usb_device_read_setup_data(_usb_device_handle, uint_8, uint_8_ptr);
USB_STATUS _usb_device_assert_resume(_usb_device_handle);
USB_STATUS _usb_device_init_endpoint(_usb_device_handle, USB_EP_STRUCT_PTR, uint_8);
USB_STATUS _usb_device_stall_endpoint(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_device_unstall_endpoint(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_device_deinit_endpoint(_usb_device_handle, uint_8, uint_8);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
