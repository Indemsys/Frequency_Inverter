#ifndef __usb_host_hub_h__
#define __usb_host_hub_h__
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
* $FileName: usb_host_hub.h$
* $Version : 3.8.11.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file defines a structure(s) for hub class driver.
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"

#include "hostapi.h"

#define  USB_CLASS_HUB              9
#define  USB_SUBCLASS_HUB_NONE      0
#define  USB_PROTOCOL_HUB_LS        0
#define  USB_PROTOCOL_HUB_FS        0
#define  USB_PROTOCOL_HUB_HS_SINGLE 1
#define  USB_PROTOCOL_HUB_HS_MULTI  2
#define  USB_PROTOCOL_HUB_ALL       0xFF

/*
** HUB Class Interface structure. This structure will be passed to
** all commands to this class driver.
*/
typedef struct {
   /* Each class must start with a GENERAL_CLASS struct */
   GENERAL_CLASS                             G;
   /* application handle */
   CLASS_CALL_STRUCT_PTR                     APP;

   /* Pipes */
   _usb_pipe_handle                          P_CONTROL;
   _usb_pipe_handle                          P_INT_IN;
   /* Is command used? (only 1 command can be issued at one time) */
   boolean                                   IN_SETUP;
   /* Only 1 interrupt pipe can be opened at one time */
   boolean                                   INT_PIPE;

   /* Higher level callback and parameter */
   tr_callback                               CTRL_CALLBACK;
   pointer                                   CTRL_PARAM;

   /* Higher level callback and parameter */
   tr_callback                               INT_CALLBACK;
   pointer                                   INT_PARAM;

   uint_32                                   C_BUF_SIZE;
   uint_32                                   I_BUF_SIZE;
   /* control pipe buffer pointer, allocated in memlist */
   uint_8_ptr                                C_BUF;

   /* interrupt pipe buffer pointer, allocated in memlist */
   uint_8_ptr                                I_BUF;

} USB_HUB_CLASS_INTF_STRUCT, _PTR_ USB_HUB_CLASS_INTF_STRUCT_PTR;

/* Class specific functions exported by hub class driver */
#ifdef __cplusplus
extern "C" {
#endif

extern void usb_class_hub_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
