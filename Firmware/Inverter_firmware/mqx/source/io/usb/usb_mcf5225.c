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
* $FileName: usb_mcf5225.c$
* $Version : 3.8.15.0$
* $Date    : Jun-6-2012$
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

_mqx_int _bsp_usb_init(pointer param);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_usb_init(pointer param)
{
    uint_32 status;

    _bsp_usb_io_init();

    // setup interrupt
    status = _bsp_int_init(MCF5225_INT_USB, 3, 0, TRUE);

    if (status != MQX_OK)
    {
        return -1;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_host_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to USB host
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_usb_host_init(struct usb_host_if_struct *usb_if)
{
    return _bsp_usb_init(NULL);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_capability_register_base
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the address of the VUSBHS Capability Registers
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_usb_base(uint_8 dev_num)
{
   if (dev_num != 0)
   {
      return NULL;
   }
   
   return (pointer)(&((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->USBOTG);
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
   if (dev_num != 0)
   {
      return 0;
   }
   
   return MCF5225_INT_USB;
}

/* EOF */
