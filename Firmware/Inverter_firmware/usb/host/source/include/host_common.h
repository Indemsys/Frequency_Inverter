#ifndef __host_common_h__
#define __host_common_h__
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
* $FileName: host_common.h$
* $Version : 3.8.24.0$
* $Date    : Sep-13-2012$
*
* Comments:
*
*   This file contains definitions for USB common utilities.
*
*END************************************************************************/
#include "usb_types.h"

/*------------------------------**
** Class / SubClass / Protocol  **
**------------------------------*/

#define  USB_CLASS_AUDIO            1
#define  USB_SUBCLASS_UNDEFINED     0
#define  USB_SUBCLASS_AUD_CONTROL   1
#define  USB_SUBCLASS_AUD_STREAMING 2
#define  USB_SUBCLASS_AUD_MIDI_STRM 3
#define  USB_PROTOCOL_UNDEFINED     0

#define  USB_CLASS_COMMUNICATION    2
#define  USB_SUBCLASS_COM_DIRECT    1
#define  USB_SUBCLASS_COM_ABSTRACT  2
#define  USB_SUBCLASS_COM_TELEPHONE 3
#define  USB_SUBCLASS_COM_MULTICHAN 4
#define  USB_SUBCLASS_COM_CAPI      5
#define  USB_SUBCLASS_COM_ETHERNET  6
#define  USB_SUBCLASS_COM_ATM_NET   7
#define  USB_PROTOCOL_COM_NOSPEC    0
#define  USB_PROTOCOL_COM_V25       1
#define  USB_PROTOCOL_COM_HAYES     1
#define  USB_PROTOCOL_COM_AT        1
#define  USB_PROTOCOL_COM_VENDOR    0xFF

#define  USB_CLASS_PRINTER          7
#define  USB_SUBCLASS_PRINTER       1
#define  USB_PROTOCOL_PRT_UNIDIR   1
#define  USB_PROTOCOL_PRT_BIDIR     2
#define  USB_PROTOCOL_PRT_1284      3

#define  USB_CLASS_MASS_STORAGE     8
#define  USB_SUBCLASS_MASS_RBC      1
#define  USB_SUBCLASS_MASS_ATAPI    2
#define  USB_SUBCLASS_MASS_QIC157   3
#define  USB_SUBCLASS_MASS_UFI      4
#define  USB_SUBCLASS_MASS_SFF8070I 5
#define  USB_SUBCLASS_MASS_SCSI     6
#define  USB_PROTOCOL_MASS_IRRPT    0
#define  USB_PROTOCOL_MASS_NOIRRPT  1
#define  USB_PROTOCOL_MASS_BULK     0x50

#define  USB_CLASS_HID              3
#define  USB_SUBCLASS_HID_NONE      0
#define  USB_SUBCLASS_HID_BOOT      1
#define  USB_PROTOCOL_HID_NONE      0
#define  USB_PROTOCOL_HID_KEYBOARD  1
#define  USB_PROTOCOL_HID_MOUSE     2

#define  USB_CLASS_HUB              9
#define  USB_SUBCLASS_HUB_NONE      0
#define  USB_PROTOCOL_HUB_LS        0
#define  USB_PROTOCOL_HUB_FS        0
#define  USB_PROTOCOL_HUB_HS_SINGLE 1
#define  USB_PROTOCOL_HUB_HS_MULTI  2
#define  USB_PROTOCOL_HUB_ALL       0xFF

#define  USB_CLASS_DATA             0x0A
   /* No data subclasses, set to 0 */
#define  USB_PROTOCOL_DATA_I430     0x30
#define  USB_PROTOCOL_DATA_HDLC     0x31
#define  USB_PROTOCOL_DATA_TRANS    0x32
#define  USB_PROTOCOL_DATA_Q921M    0x50
#define  USB_PROTOCOL_DATA_Q921     0x51
#define  USB_PROTOCOL_DATA_Q921TM   0x52
#define  USB_PROTOCOL_DATA_V42BIS   0x90
#define  USB_PROTOCOL_DATA_EUROISDN 0x91
#define  USB_PROTOCOL_DATA_V120     0x92
#define  USB_PROTOCOL_DATA_CAPI20   0x93
#define  USB_PROTOCOL_DATA_HOST     0xFE
#define  USB_PROTOCOL_DATA_CDC      0xFE
#define  USB_PROTOCOL_DATA_VENDOR   0xFF

/* Setup */
typedef struct
{
   uchar    BMREQUESTTYPE;
   uchar    BREQUEST;
   uint_8   WVALUE[2];
   uint_8   WINDEX[2];
   uint_8   WLENGTH[2];
} USB_SETUP, _PTR_ USB_SETUP_PTR;

/*-------------------------------------------------------------**
** Transaction callback parameters:                            **
**    pointer     pipe handle                                  **
**    pointer     parameter specified by higher level          **
**    uchar_ptr   pointer to buffer containing data (TX or RX) **
**    uint_32     length of data transferred                   **
**    USB_STATUS  status, preferably USB_OK or USB_DONE etc.   **
**-------------------------------------------------------------*/
typedef void  (_CODE_PTR_ tr_callback)(pointer, pointer, uchar_ptr, uint_32, USB_STATUS);

/*-------------------------------------------------------------------**
** Isochronous Transaction callback parameters:                      **
**    PIPE_DESCRIPTOR_STRUCT_PTR pipe handle                         **
**    TR_STRUCT_PTR              transaction handle                  **
**    USB_STATUS  status, preferably USB_OK or USB_DONE etc.         **
**-------------------------------------------------------------------*/
typedef void (_CODE_PTR_ iso_tr_callback)(struct pipe_struct _PTR_, struct tr_struct _PTR_, USB_STATUS);

/*---------------------------------------------------------**
** Attach/Detach callback parameters:                      **
**    _usb_device_instance_handle      new/removed device  **
**    _usb_interface_descriptor_handle matching class etc. **
**   uint_32                           code (attach etc.). **
**---------------------------------------------------------*/
typedef void (_CODE_PTR_ event_callback)(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint_32);
    
typedef struct tr_struct
{
   struct tr_struct *NEXT;
   uint_32     STATUS;
   uint_32     TR_INDEX;           /* Transfer number on this pipe   */
   uchar_ptr   TX_BUFFER;          /* Address of buffer containing the data to be transmitted (including OUT data phase of control transfers) */
   uchar_ptr   RX_BUFFER;          /* Address of buffer to receive data */
   uint_32     TX_LENGTH;          /* Length of data to transmit. For control transfers, the length of data for the data phase */
   uint_32     RX_LENGTH;          /* Length of data to be received. For control transfers, this indicates the length of data for the data phase */
   tr_callback CALLBACK;           /* The callback function to be invoked when a transfer is completed or an error is to be reported */
   pointer     CALLBACK_PARAM;     /* The second parameter to be passed into the callback function when it is invoked */
   boolean SEND_PHASE;             /* Second phase of setup packet: Send/Receive */
   USB_SETUP  setup_packet;        /* Setup packet raw data */
} TR_STRUCT, _PTR_ TR_STRUCT_PTR;

typedef struct tr_init_param_struct
{
   TR_STRUCT   G;                  /* Generic transfer structure */
   uchar_ptr   DEV_REQ_PTR;        /* Address of buffer containing the setup packet to send. Relevant for control pipes only */
} TR_INIT_PARAM_STRUCT, _PTR_ TR_INIT_PARAM_STRUCT_PTR;

typedef struct pipe_struct
{
   struct pipe_struct *NEXT;
   pointer     DEV_INSTANCE;       /* Instance on list of all devices that own pipe. */
   uint_32     INTERVAL;           /* Interval for polling pipe for data transfer. */
   uint_32     NAK_COUNT;          /* The maximum number of NAK responses tolerated. MUST be zero for interrupt. */
                                   /* For USB 1.1, after count NAKs/frame,transaction is deferred to next frame. */
                                   /* For USB 2.0, the host controller will not execute transaction following this many NAKs. */
   uint_32     FIRST_FRAME;        /* The frame # to start xfer */
   uint_32     FIRST_UFRAME;       /* Microframe # to start xfer */
   uint_32     FLAGS;              /* After all data transferred, should we terminate the transfer with a zero length packet if the last packet size == MAX_PACKET_SIZE? */
   uint_16     MAX_PACKET_SIZE;    /* Pipe's maximum packet size for sending or receiving. */
   int_16      PIPE_ID;            /* Pipe's unique ID within host controller */
   uint_8      DEVICE_ADDRESS;     /* The device's address 1-127  */
   uint_8      ENDPOINT_NUMBER;    /* The endpoint number */
   uint_8      DIRECTION;          /* Input or Output */
   uint_8      PIPETYPE;           /* Control, interrupt, bulk or isochronous */
   uint_8      SPEED;              /* High/full/low */
   uint_8      TRS_PER_UFRAME;     /* Number of transaction per frame, only high-speed high-bandwidth pipes. */
   uint_8      OPEN;               /* Open flag */
   uint_8      NEXTDATA01;         /* Endpoint data toggling bit */
   TR_STRUCT_PTR    tr_list_ptr;   /* List of transactions scheduled on this pipe */
   uint_8      HUB_DEVICE_ADDR;    /* The address of hub to which the device instance is attached */
   uint_8      HUB_PORT_NUM;       /* The port of the hub to which the device instance is attached */
} PIPE_STRUCT, _PTR_ PIPE_STRUCT_PTR;

typedef struct pipe_init_param_struct
{
   struct pipe_struct G;
} PIPE_INIT_PARAM_STRUCT, _PTR_ PIPE_INIT_PARAM_STRUCT_PTR;

/* Class Callback function storage structure */
typedef struct device_class_info_struct {
   uint_8               CLASS_TYPE;
   uint_8               SUB_CLASS;
   uint_8               PROTOCOL;
} DEVICE_CLASS_INFO_STRUCT, _PTR_ DEVICE_CLASS_INFO_STRUCT_PTR;

/* Information for one class or device driver */
typedef struct driver_info
{
   uint_8         idVendor[2];      /* Vendor ID per USB-IF */
   uint_8         idProduct[2];     /* Product ID per manufacturer */
   uint_8         bDeviceClass;     /* Class code, 0xFF if any */
   uint_8         bDeviceSubClass;  /* Sub-Class code, 0xFF if any */
   uint_8         bDeviceProtocol;  /* Protocol, 0xFF if any */
   uint_8         reserved;         /* Alignment padding */
   event_callback attach_call;
} USB_HOST_DRIVER_INFO, _PTR_ USB_HOST_DRIVER_INFO_PTR;

/* Pipe bundle = device handle + interface handle + 
**                1..N pipe handles 
** NOTE: The pipe handles are for non-control pipes only, i.e.
**          pipes belonging strictly to this interface.
**       The control pipe belongs to the device, even if it
**          is being used by the device's interfaces.  Hence
**          a pointer to the device instance is provided.
**          Closing pipes for the interface does NOT close
**          the control pipe which may still be required to
**          set new configurations/interfaces etc. */
typedef struct pipe_bundle_struct
{
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   _usb_pipe_handle                 pipe_handle[4];
} PIPE_BUNDLE_STRUCT, _PTR_ PIPE_BUNDLE_STRUCT_PTR;
                           
/* The following struct is for storing a class's validity-check
** code with the pointer to the data.  The address of one such
** struct is passed as a pointer to select-interface calls,
** where values for that interface get initialized.  Then the
** struct should be passed to class calls using the interface. */
typedef struct class_call_struct
{
   _usb_class_intf_handle         class_intf_handle;
   uint_32                        code_key;
   pointer                        next;   /* Used by class driver or app, usually pointer to next CLASS_CALL_STRUCT */
   pointer                        anchor; /* Used by class driver or app, usually pointer to first CLASS_CALL_STRUCT */
}  CLASS_CALL_STRUCT, _PTR_ CLASS_CALL_STRUCT_PTR;

/* Define function type for class device instance initialization */
typedef void (_CODE_PTR_ INTF_INIT_CALL) (PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
/* Define function type for class device instance deinitialization, internal */
typedef void (_CODE_PTR_ INTF_DEINIT_CALL) (_usb_class_intf_handle);

/* The following define the internal mapping from interface
** class / sub-class / protocol to class drivers 
*/
typedef struct class_map
{
   INTF_INIT_CALL class_init;       /* class driver initialization- entry  of the class driver */
   uint_16        intf_struct_size; /* size of class's data struct  */
   uint_8         class_code;       /* interface descriptor class, */
   uint_8         sub_class_code;   /*   sub-class, and            */
   uint_8         protocol_code;    /*   protocol.                 */
   uint_8         class_code_mask;  /* masks ANDed with class etc. */
   uint_8         sub_code_mask;    /*   mask = 0x00 matches any   */
   uint_8         prot_code_mask;   /*   mask = 0xFF must be exact */
} CLASS_MAP, _PTR_ CLASS_MAP_PTR;
                              
/* The generalized heading can alias to any class-intf
** which defines its first four fields as follows: */
typedef struct general_class
{
   struct general_class _PTR_       next;    /* next list item */
   struct general_class _PTR_       anchor;  /* list start */
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   _usb_host_handle                 host_handle;
   INTF_DEINIT_CALL                 class_deinit;
   uint_32                          key_code;
} GENERAL_CLASS, _PTR_ GENERAL_CLASS_PTR;

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void  usb_hostdev_attach_detach(struct dev_instance _PTR_, uint_32);
USB_STATUS usb_hostdev_busy_check(_usb_device_instance_handle);
USB_STATUS usb_hostdev_validate(_usb_device_instance_handle);
USB_STATUS _usb_hostdev_get_buffer(_usb_device_instance_handle, uint_32, pointer _PTR_);
USB_STATUS _usb_hostdev_get_buffer_aligned(_usb_device_instance_handle, uint_32, uint_32, pointer _PTR_);
USB_STATUS _usb_hostdev_free_buffer(_usb_device_instance_handle, pointer);
USB_STATUS _usb_hostdev_get_descriptor(_usb_device_instance_handle, _usb_interface_descriptor_handle, descriptor_type, uint_8, uint_8, pointer _PTR_);
_usb_pipe_handle _usb_hostdev_find_pipe_handle(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint_8, uint_8);
_usb_pipe_handle usb_hostdev_get_pipe_handle(PIPE_BUNDLE_STRUCT_PTR, uint_8, uint_8);
USB_STATUS usb_host_class_intf_init(PIPE_BUNDLE_STRUCT_PTR, pointer, pointer, INTF_DEINIT_CALL);
uint_32  usb_host_class_intf_validate(pointer);
USB_STATUS _usb_host_driver_info_register(_usb_host_handle, pointer);
USB_STATUS _usb_hostdev_select_config(_usb_device_instance_handle, uint_8);
void  usb_hostdev_delete_interface(struct dev_instance _PTR_, struct usb_interface_descriptor _PTR_);
USB_STATUS _usb_hostdev_select_interface(_usb_device_instance_handle, _usb_interface_descriptor_handle, pointer);
void usb_hostdev_tr_init(TR_INIT_PARAM_STRUCT_PTR, tr_callback, pointer);
void usb_hostdev_iso_tr_init(TR_INIT_PARAM_STRUCT_PTR, iso_tr_callback, pointer);
uint_32 _usb_hostdev_get_speed(_usb_device_instance_handle);
      
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
