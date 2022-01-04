#ifndef __usb_host_hub_prv_h__
#define __usb_host_hub_prv_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: usb_host_hub_prv.h$
* $Version : 3.8.8.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file defines a structure(s) for hub class driver.
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_ch9.h"
#include "host_rcv.h"
#include "host_dev_list.h"

#define  C_HUB_LOCAL_POWER          (0)
#define  C_HUB_OVER_CURRENT         (1)
#define  PORT_CONNECTION            (0)
#define  PORT_ENABLE                (1)
#define  PORT_SUSPEND               (2)
#define  PORT_OVER_CURRENT          (3)
#define  PORT_RESET                 (4)
#define  PORT_POWER                 (8)
#define  PORT_LOW_SPEED             (9)
#define  PORT_HIGH_SPEED            (10)
#define  C_PORT_CONNECTION          (16)
#define  C_PORT_ENABLE              (17)
#define  C_PORT_SUSPEND             (18)
#define  C_PORT_OVER_CURRENT        (19)
#define  C_PORT_RESET               (20)
#define  C_PORT_POWER               (24)
#define  PORT_TEST                  (21)
#define  PORT_INDICATOR             (22)

/* structure for HUB class descriptor */
typedef struct {
    uint_8     BLENGTH;
    uint_8     BDESCRIPTORTYPE;
    uint_8     BNRPORTS;
    uint_8     WHUBCHARACTERISTICS[2];
    uint_8     BPWRON2PWRGOOD;
    uint_8     BHUBCONTRCURRENT;
    uint_8     DEVICEREMOVABLE[];
/* not used: */
/*    uint_8     PORTPOWERCTRLMASK; */
} HUB_DESCRIPTOR_STRUCT, _PTR_ HUB_DESCRIPTOR_STRUCT_PTR;

typedef struct {
    uint_16    WHUBSTATUS;
    uint_16    WHUBCHANGE;
} HUB_STATUS_STRUCT, _PTR_ HUB_STATUS_STRUCT_PTR;

/* Class specific functions exported by hub class driver */
#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_class_hub_get_app(_usb_device_instance_handle, _usb_interface_descriptor_handle, CLASS_CALL_STRUCT_PTR _PTR_);
USB_STATUS usb_class_hub_cntrl_common(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_8, uint_8, uint_16, uint_16, uint_16);
void usb_class_hub_cntrl_callback(pointer, pointer, uchar_ptr, uint_32, USB_STATUS);
USB_STATUS usb_class_hub_get_descriptor(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uchar);
USB_STATUS usb_class_hub_set_port_feature(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_8, uint_8);
USB_STATUS usb_class_hub_clear_feature(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_8);
USB_STATUS usb_class_hub_clear_port_feature(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_8, uint_8);
USB_STATUS usb_class_hub_get_status(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uchar);
USB_STATUS usb_class_hub_get_port_status(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_8, uchar);

USB_STATUS usb_class_hub_wait_for_interrupt(CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_8);
void usb_class_hub_int_callback(pointer, pointer, uchar_ptr, uint_32, USB_STATUS);

#ifdef __cplusplus
}
#endif

#endif
