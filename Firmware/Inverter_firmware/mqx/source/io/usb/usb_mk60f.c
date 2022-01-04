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
* $FileName: usb_mk60f.c$
* $Version : 3.8.2.0$
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

struct usb_host_if_struct _bsp_usb_host_khci0_if = {
    &_usb_khci_host_callback_table,
    (pointer) &_khci0_host_init_param,
    NULL
};

struct usb_host_if_struct _bsp_usb_host_ehci0_if = {
    &_usb_ehci_host_callback_table,
    (pointer) &_ehci0_host_init_param,
    NULL
};

#define USBHS_USBMODE_CM_IDLE_MASK    USBHS_USBMODE_CM(0)
#define USBHS_USBMODE_CM_DEVICE_MASK  USBHS_USBMODE_CM(2)
#define USBHS_USBMODE_CM_HOST_MASK    USBHS_USBMODE_CM(3)

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
    _mqx_int result;

    if (usb_if == NULL) {
        return IO_ERROR;
    }

    result = _bsp_usb_host_io_init(usb_if);

    if (result != MQX_OK)
        return result;

    if (usb_if->HOST_INIT_PARAM == &_khci0_host_init_param) {
        /* Do not configure enable USB regulator for host */
//      SIM_SOPT1CFG_REG(SIM_BASE_PTR) |= SIM_SOPT1CFG_URWE_MASK;
//      SIM_SOPT1_REG(SIM_BASE_PTR) |= SIM_SOPT1_USBREGEN_MASK;

        /* reset USB CTRL register */
        USB_USBCTRL_REG(USB0_BASE_PTR) = 0;

        /* setup interrupt */
        _bsp_int_init(INT_USB0, BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else if (usb_if->HOST_INIT_PARAM == &_ehci0_host_init_param) {
        USBHS_USBMODE = USBHS_USBMODE_CM_HOST_MASK;
        USBHS_USBCMD = USBHS_USBCMD_ASP(3) | USBHS_USBCMD_ITC(0);

        /* setup interrupt */
       _bsp_int_init(INT_USBHS, BSP_USB_INT_LEVEL, 0, TRUE);

    }
    else {
        /* unknown controller */
        result = IO_ERROR;
    }

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
        return (pointer)USBHS_BASE_PTR;
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
    _mqx_uint dev_num
)
{
    if (dev_num == 0) {
        return INT_USB0;
    }
    else if (dev_num == 1) {
        return INT_USBHS;
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
    if (dev_num == 1) {
        return (pointer) ((uchar *) USBHS_BASE_PTR + 0x100);
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
    if (dev_num == 1) {
        return (pointer) ((uchar *) USBHS_BASE_PTR + 0x80);
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
