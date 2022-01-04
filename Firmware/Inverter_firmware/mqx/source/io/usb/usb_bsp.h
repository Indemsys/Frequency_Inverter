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
* $FileName: usb_bsp.h$
* $Version : 3.8.15.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
*   Include this header file to use BSP-specific USB initialization code
*
*END************************************************************************/

#ifndef __usb_bsp_h__
#define __usb_bsp_h__ 1

//#include "../../../../usb/host/source/rtos/mqx/mqx_host.h"

#include "if_host_khci.h"
#include "if_host_ehci.h"
#include "if_dev_khci.h"
#include "if_dev_ehci.h"

/* Forward declaration of structure */
//struct usb_host_callback_functions_struct;

/* NOTE!!!: since both structures below are used in the array of structures for USB component, they must have the same size */
typedef struct usb_host_if_struct
{
   const struct usb_host_callback_functions_struct *HOST_IF;
   pointer                       HOST_INIT_PARAM;
   pointer                       HOST_HANDLE;
} USB_HOST_IF_STRUCT, _PTR_ USB_HOST_IF_STRUCT_PTR;

typedef struct usb_dev_if_struct
{
   const struct usb_dev_callback_functions_struct *DEV_IF;
   pointer                       DEV_INIT_PARAM;
   pointer                       DEV_HANDLE;
} USB_DEV_IF_STRUCT, _PTR_ USB_DEV_IF_STRUCT_PTR;

/* TODO: move to processor USB header files */
extern struct usb_dev_if_struct _bsp_usb_dev_ehci0_if;
extern struct usb_dev_if_struct _bsp_usb_dev_ehci1_if;

extern struct usb_dev_if_struct _bsp_usb_dev_khci0_if;
extern struct usb_dev_if_struct _bsp_usb_dev_khci1_if;

extern struct usb_host_if_struct _bsp_usb_host_ehci0_if;
extern struct usb_host_if_struct _bsp_usb_host_ehci1_if;

extern struct usb_host_if_struct _bsp_usb_host_khci0_if;
extern struct usb_host_if_struct _bsp_usb_host_khci1_if;


extern const USB_KHCI_HOST_INIT_STRUCT _khci0_host_init_param;
extern const USB_KHCI_HOST_INIT_STRUCT _khci1_host_init_param;

extern const USB_EHCI_HOST_INIT_STRUCT _ehci0_host_init_param;
extern const USB_EHCI_HOST_INIT_STRUCT _ehci1_host_init_param;

extern const USB_EHCI_DEV_INIT_STRUCT _ehci0_dev_init_param;
extern const USB_EHCI_DEV_INIT_STRUCT _ehci1_dev_init_param;

extern const USB_KHCI_DEV_INIT_STRUCT _khci0_dev_init_param;
extern const USB_KHCI_DEV_INIT_STRUCT _khci1_dev_init_param;

#ifdef __cplusplus
extern "C" {
#endif

_mqx_int _usb_host_driver_install(struct usb_host_if_struct *);
_mqx_int _usb_device_driver_install(struct usb_dev_if_struct *);
_mqx_int _usb_device_driver_uninstall(void);
_mqx_int _bsp_usb_dev_init(struct usb_dev_if_struct *);
_mqx_int _bsp_usb_host_init(struct usb_host_if_struct *);
pointer _usb_host_get_init(pointer host_handle);


#ifdef __cplusplus
}
#endif

#endif

