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
* $FileName: usb_vybrid.c$
* $Version : 3.8.4.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains board-specific USB initialization functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"

struct usb_host_if_struct _bsp_usb_host_ehci0_if = {
    &_usb_ehci_host_callback_table,
    (pointer) &_ehci0_host_init_param,
    NULL
};

struct usb_host_if_struct _bsp_usb_host_ehci1_if = {
    &_usb_ehci_host_callback_table,
    (pointer) &_ehci1_host_init_param,
    NULL
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_host_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_usb_host_init(struct usb_host_if_struct *usb_if)
{
    USB_MemMapPtr usb_ptr;
    USB_EHCI_HOST_INIT_STRUCT_PTR usb_init;
    _mqx_int result;

    if (usb_if == NULL) {
        return IO_ERROR;
    }

    if (usb_if->HOST_INIT_PARAM == &_ehci0_host_init_param) {
        usb_ptr = USB0_BASE_PTR;
    }
    else if (usb_if->HOST_INIT_PARAM == &_ehci1_host_init_param) {
        usb_ptr = USB1_BASE_PTR;
    }
    else {
        return IO_ERROR; /* unknown controller */
    }

    result = _bsp_usb_host_io_init(usb_if);

    if (result != MQX_OK)
        return result;

    USB_USBCMD_REG(usb_ptr) &= ~USB_USBCMD_RS_MASK;
    while (USB_USBCMD_REG(usb_ptr) & USB_USBCMD_RS_MASK);
    { /* delay while resetting USB controller */ }
    USB_USBCMD_REG(usb_ptr) |= USB_USBCMD_RST_MASK;
    while (USB_USBCMD_REG(usb_ptr) & USB_USBCMD_RST_MASK)
    { /* delay while resetting USB controller */ }

    USB_USBMODE_REG(usb_ptr) = USB_USBMODE_CM_MASK;
    USB_USBCMD_REG(usb_ptr) = USB_USBCMD_ASP(3) | USB_USBCMD_ITC(0);

    /* setup interrupt */
    usb_init = (USB_EHCI_HOST_INIT_STRUCT_PTR)usb_if->HOST_INIT_PARAM;
    result = _bsp_int_init(usb_init->VECTOR, BSP_USB_INT_LEVEL, 0, TRUE);

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_base
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the address of USB OTG module
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_usb_base(uint_8 dev_num)
{
    if (dev_num == 0) {
        return (pointer)USB0_BASE_PTR;
    }
    else if (dev_num == 1) {
        return (pointer)USB1_BASE_PTR;
    }

    return NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_vector
* Returned Value   : Vector Number upon success, 0 upon failure
* Comments         :
*    This function returns the vector number of the VUSBHS Host Module
*
*END*----------------------------------------------------------------------*/
uint_8 _bsp_get_usb_vector
(
    uint_8 dev_num
)
{
    if (dev_num == 0) {
        return INT_USB0;
    }
    else if (dev_num == 1) {
        return INT_USB1;
    }
   
    return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_capability_register_base
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the address of the VUSBHS Capability Registers
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_usb_capability_register_base(uint_8 dev_num)
{
    if (dev_num == 0) {
        return (pointer) ((uchar *) USB0_BASE_PTR + 0x100);
    }
    else if (dev_num == 1) {
        return (pointer) ((uchar *) USB1_BASE_PTR + 0x100);
    }

    return NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_timer_register_base
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the address of the VUSBHS Timer Registers
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_usb_timer_register_base(uint_8 dev_num)
{
    if (dev_num == 0) {
        return (pointer) ((uchar *) USB0_BASE_PTR + 0x100);
    }
    else if (dev_num == 1) {
        return (pointer) ((uchar *) USB1_BASE_PTR + 0x80);
    }

    return NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_otg_csr
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the address of the U0CSR register
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_usb_otg_csr(uint_8 dev_num)
{
    return NULL;
}

void _bsp_usb_hs_disconnect_detection_enable(pointer host_handle)
{
    USB_EHCI_HOST_INIT_STRUCT_PTR host_init = (USB_EHCI_HOST_INIT_STRUCT_PTR)_usb_host_get_init(host_handle);

    if (&_ehci0_host_init_param == host_init) {
        USBPHY0_CTRL_SET = USBPHY_CTRL_ENHOSTDISCONDETECT_MASK;
    }
    else if (&_ehci1_host_init_param == host_init) {
        USBPHY1_CTRL_SET = USBPHY_CTRL_ENHOSTDISCONDETECT_MASK;
    }
}

void _bsp_usb_hs_disconnect_detection_disable(pointer host_handle)
{
    USB_EHCI_HOST_INIT_STRUCT_PTR host_init = (USB_EHCI_HOST_INIT_STRUCT_PTR)_usb_host_get_init(host_handle);

    if (&_ehci0_host_init_param == host_init) {
        USBPHY0_CTRL_CLR = USBPHY_CTRL_ENHOSTDISCONDETECT_MASK;
    }
    else if (&_ehci1_host_init_param == host_init) {
        USBPHY1_CTRL_CLR = USBPHY_CTRL_ENHOSTDISCONDETECT_MASK;
    }
}
