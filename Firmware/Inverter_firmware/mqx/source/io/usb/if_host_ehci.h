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
* $FileName: if_host_ehci.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file exports USB KHCI host initial structure
*
*END************************************************************************/
#ifndef __if_host_ehci_h__
#define __if_host_ehci_h__ 1

#include <mqx.h> //pointer types

#ifdef __cplusplus
extern "C" {
#endif

extern const struct usb_host_callback_functions_struct _usb_ehci_host_callback_table;

enum _bsp_ehci_speed {
    BSP_EHCI_FS,
    BSP_EHCI_HS
};

typedef struct usb_ehci_host_init_struct {
    pointer   BASE_PTR;
    pointer   CAP_BASE_PTR;
    pointer   TIMER_BASE_PTR;
    enum _bsp_ehci_speed SPEED; /* full speed, low speed, high speed... */
    _mqx_uint VECTOR;
    _mqx_uint PRIORITY;
    uint_32   FRAME_LIST_SIZE;
} USB_EHCI_HOST_INIT_STRUCT, _PTR_ USB_EHCI_HOST_INIT_STRUCT_PTR;

#ifdef __cplusplus
}
#endif

#endif
