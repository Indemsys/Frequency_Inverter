/**HEADER********************************************************************
*
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: khci_dev_main.h$
* $Version : 3.8.9.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the macros, function prototypes and data structure
*   definitions required by the Full Speed USB Device Controller driver.
*
*END************************************************************************/

#ifndef __khci_dev_main_h__
#define __khci_dev_main_h__

#include "khci_usbprv.h"

#define EP_DISABLE              (0)
#define ASSERT_RESUME_DELAY_COUNT       (8000)/* Delay for assert resume */

#define BYTES_512                 (512)
#define BYTES_1024                (1024)
#define ENDPOINT_NUMBER_SHIFT     (4)   /* endpoint shift & mask to */
#define ENDPOINT_NUMBER_MASK      (0xf0)/* use in setting and getting status */
#define MAX_EP_BUFFER_SIZE        (64)
#define ZERO_LENGTH               (0)
#define USB_SETUP_TOKEN           (0x0d)/* Setup Token PID */
#define FRAME_HIGH_BYTE_SHIFT     (8)

/***************************************
**
** Prototypes
**/
#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS _usb_dci_usbfs_preinit(_usb_device_handle _PTR_);
USB_STATUS _usb_dci_usbfs_init(_usb_device_handle);
USB_STATUS _usb_dci_usbfs_send(_usb_device_handle, XD_STRUCT_PTR);
USB_STATUS _usb_dci_usbfs_recv(_usb_device_handle, XD_STRUCT_PTR);
USB_STATUS _usb_dci_usbfs_cancel(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_dci_usbfs_set_addr(_usb_device_handle, uint_8);
USB_STATUS _usb_dci_usbfs_shutdown(_usb_device_handle);
USB_STATUS _usb_dci_usbfs_get_setup_data(_usb_device_handle, uint_8, uchar_ptr);
USB_STATUS _usb_dci_usbfs_assert_resume(_usb_device_handle);
USB_STATUS _usb_dci_usbfs_init_endpoint(_usb_device_handle, XD_STRUCT_PTR);
USB_STATUS _usb_dci_usbfs_stall_endpoint(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_dci_usbfs_unstall_endpoint(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_dci_usbfs_deinit_endpoint(_usb_device_handle, uint_8, uint_8);
USB_STATUS _usb_dci_usbfs_get_endpoint_status(_usb_device_handle, uint_8, uint_16_ptr);
USB_STATUS _usb_dci_usbfs_set_endpoint_status(_usb_device_handle, uint_8, uint_16);
USB_STATUS _usb_dci_usbfs_get_transfer_status(_usb_device_handle,uint_8,uint_8);

#ifdef __cplusplus
}
#endif

#endif
