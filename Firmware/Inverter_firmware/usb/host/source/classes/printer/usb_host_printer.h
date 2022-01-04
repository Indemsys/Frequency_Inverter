#ifndef __print_host_printer_h__
#define __print_host_printer_h__
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
* $FileName: usb_host_printer.h$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains definitions for  USB printer class.
*   Reference Specification:
*   "USB Device Class Definition for Printing Devices" version 1.1
*   January 2000, from USB Implementers Forum.
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_snd.h"
#include "host_rcv.h"
#include "host_ch9.h"

/* Printer Port-Status Bits */
#define  USB_PRINTER_PAPER_OUT     (0x20)   /* 0 = Not empty */
                                          /* 1 = Paper empty */
#define  USB_PRINTER_SELECTED      (0x10)   /* 0 = Not selected */
                                          /* 1 = Selected */
#define  USB_PRINTER_NO_ERROR      (0x08)   /* 0 = Error */
                                          /* 1 = No error!!! */

/* Bulk In/Out Endpoint-found tags */
#define  USB_PRINTER_IN_TAG   (0x01)
#define  USB_PRINTER_OUT_TAG  (0x02)
#define  USB_PRINTER_BI_TAG   (USB_PRINTER_IN_TAG | USB_PRINTER_OUT_TAG)

/* Error codes */
#define  USB_PRINTER_UNSUPPORTED_SUB_CLASS (0x80)
#define  USB_PRINTER_UNSUPPORTED_PROTOCOL  (0x81)
#define  USB_PRINTER_BAD_COMMAND           (0x82)

/* Class-specific Requests for Printers */
#define  USB_PRINTER_GET_DEVICE_ID        0
#define  USB_PRINTER_GET_PORT_STATUS      1
#define  USB_PRINTER_SOFT_RESET           2
/* Non-class Requests for Printers */
#define  USB_PRINTER_SEND_BULK            3
#define  USB_PRINTER_RECV_BULK            4

/* Class-specific interface info struct, useful for
**       an application to send/receive on in/out
**       pipes by name without internal details.
** NOTE: Since the class may have to handle several
**       printers attached simultaneously, it is
**       essential to have an array or list.
**       Here we choose a NULL-terminated list. */
typedef struct printer_interface {
   struct printer_interface _PTR_   next;    /* next printer */
   struct printer_interface _PTR_   anchor;  /* list start */
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   _usb_host_handle                 host_handle;
   uint_32                          key_code;
   _usb_pipe_handle                 control_pipe;
   _usb_pipe_handle                 bulk_in_pipe;
   _usb_pipe_handle                 bulk_out_pipe;
} PRINTER_INTERFACE_STRUCT, _PTR_ PRINTER_INTERFACE_STRUCT_PTR;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_printer_get_device_ID(CLASS_CALL_STRUCT_PTR,
   tr_callback, pointer, uint_32, uchar_ptr);
USB_STATUS usb_printer_get_port_status(CLASS_CALL_STRUCT_PTR,
   tr_callback, pointer, uint_8_ptr);
void usb_printer_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
USB_STATUS usb_printer_recv_data(CLASS_CALL_STRUCT_PTR,
   tr_callback, pointer, uint_32, uchar_ptr);
USB_STATUS usb_printer_send_data(CLASS_CALL_STRUCT_PTR,
   tr_callback, pointer, uint_32, uchar_ptr);
USB_STATUS usb_printer_soft_reset(CLASS_CALL_STRUCT_PTR,
   tr_callback, pointer);

#ifdef __cplusplus
}
#endif


#endif
