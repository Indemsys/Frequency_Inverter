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
* $FileName: callback_khci.c$
* $Version : 3.8.2.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "dev_cnfg.h"
#include "devapi.h"
#include "dev_main.h"
#include "mqx_dev.h"

#include "khci_dev_main.h"
#include "khci_usbprv.h"

const USB_DEV_CALLBACK_FUNCTIONS_STRUCT _usb_khci_dev_callback_table =
{
    _usb_dci_usbfs_preinit,
    _usb_dci_usbfs_init,
    _usb_dci_usbfs_send,
    _usb_dci_usbfs_recv,
    _usb_dci_usbfs_cancel,
    _usb_dci_usbfs_init_endpoint,
    _usb_dci_usbfs_deinit_endpoint,
    _usb_dci_usbfs_unstall_endpoint,
    _usb_dci_usbfs_get_endpoint_status,
    _usb_dci_usbfs_set_endpoint_status,
    _usb_dci_usbfs_get_transfer_status,
    _usb_dci_usbfs_set_addr,
    _usb_dci_usbfs_shutdown,
    _usb_dci_usbfs_get_setup_data,
    _usb_dci_usbfs_assert_resume,
    _usb_dci_usbfs_stall_endpoint
};
