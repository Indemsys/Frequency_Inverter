/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
******************************************************************************
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
**************************************************************************/ /*!
  *
  * @file usb_devapi.h
  *
  * @author
  *
  * @version
  *
  * @date
  *
  * @brief This file contains S08 USB stack device layer API header function.
  *
  *****************************************************************************/

#ifndef _USB_DEVAPI_H
  #define _USB_DEVAPI_H

/******************************************************************************
 * Includes
 *****************************************************************************/
/* User Defined Data Types */
  #include "app_cfg.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
  #define USB_MAX_EP_BUFFER_SIZE     (64)

  #ifndef CONTROL_ENDPOINT
    #define CONTROL_ENDPOINT    (0)     /* control endpoint number */
  #endif

  #define USB_SETUP_PKT_SIZE  (8)     /* Setup Packet Size */

/* Error codes */
  #define  USB_OK                              (0x00)
  #define  USBERR_ALLOC                        (0x81)
  #define  USBERR_BAD_STATUS                   (0x82)
  #define  USBERR_CLOSED_SERVICE               (0x83)
  #define  USBERR_OPEN_SERVICE                 (0x84)
  #define  USBERR_TRANSFER_IN_PROGRESS         (0x85)
  #define  USBERR_ENDPOINT_STALLED             (0x86)
  #define  USBERR_ALLOC_STATE                  (0x87)
  #define  USBERR_DRIVER_INSTALL_FAILED        (0x88)
  #define  USBERR_DRIVER_NOT_INSTALLED         (0x89)
  #define  USBERR_INSTALL_ISR                  (0x8A)
  #define  USBERR_INVALID_DEVICE_NUM           (0x8B)
  #define  USBERR_ALLOC_SERVICE                (0x8C)
  #define  USBERR_INIT_FAILED                  (0x8D)
  #define  USBERR_SHUTDOWN                     (0x8E)
  #define  USBERR_INVALID_PIPE_HANDLE          (0x8F)
  #define  USBERR_OPEN_PIPE_FAILED             (0x90)
  #define  USBERR_INIT_DATA                    (0x91)
  #define  USBERR_SRP_REQ_INVALID_STATE        (0x92)
  #define  USBERR_TX_FAILED                    (0x93)
  #define  USBERR_RX_FAILED                    (0x94)
  #define  USBERR_EP_INIT_FAILED               (0x95)
  #define  USBERR_EP_DEINIT_FAILED             (0x96)
  #define  USBERR_TR_FAILED                    (0x97)
  #define  USBERR_BANDWIDTH_ALLOC_FAILED       (0x98)
  #define  USBERR_INVALID_NUM_OF_ENDPOINTS     (0x99)
  #define  USBERR_NOT_SUPPORTED                (0x9A)

  #define  USBERR_DEVICE_NOT_FOUND             (0xC0)
  #define  USBERR_DEVICE_BUSY                  (0xC1)
  #define  USBERR_NO_DEVICE_CLASS              (0xC3)
  #define  USBERR_UNKNOWN_ERROR                (0xC4)
  #define  USBERR_INVALID_BMREQ_TYPE           (0xC5)
  #define  USBERR_GET_MEMORY_FAILED            (0xC6)
  #define  USBERR_INVALID_MEM_TYPE             (0xC7)
  #define  USBERR_NO_DESCRIPTOR                (0xC8)
  #define  USBERR_NULL_CALLBACK                (0xC9)
  #define  USBERR_NO_INTERFACE                 (0xCA)
  #define  USBERR_INVALID_CFIG_NUM             (0xCB)
  #define  USBERR_INVALID_ANCHOR               (0xCC)
  #define  USBERR_INVALID_REQ_TYPE             (0xCD)

/* Pipe Types */
  #define  USB_CONTROL_PIPE                    (0x00)
  #define  USB_ISOCHRONOUS_PIPE                (0x01)
  #define  USB_BULK_PIPE                       (0x02)
  #define  USB_INTERRUPT_PIPE                  (0x03)

/* Device States */
  #define  USB_STATE_UNKNOWN                   (0xff)
  #define  USB_STATE_PENDING_ADDRESS           (0x04)
  #define  USB_STATE_POWERED                   (0x03)
  #define  USB_STATE_DEFAULT                   (0x02)
  #define  USB_STATE_ADDRESS                   (0x01)
  #define  USB_STATE_CONFIG                    (0x00)
  #define  USB_STATE_SUSPEND                   (0x80)

/* Get_Status Request information for DEVICE */
  #define  USB_SELF_POWERED                    (0x01)
  #define  USB_REMOTE_WAKEUP                   (0x02)

/* Get_Status Request information for OTG (WINDEX = 0xF000) */
  #ifdef OTG_BUILD
    #define  USB_OTG_HOST_REQUEST_FLAG           (0x01)
  #endif

/* Bus Control values */
  #define  USB_NO_OPERATION                    (0x00)
  #define  USB_ASSERT_BUS_RESET                (0x01)
  #define  USB_DEASSERT_BUS_RESET              (0x02)
  #define  USB_ASSERT_RESUME                   (0x03)
  #define  USB_DEASSERT_RESUME                 (0x04)
  #define  USB_SUSPEND_SOF                     (0x05)
  #define  USB_RESUME_SOF                      (0x06)

/* possible values of Status */
  #define  USB_STATUS_IDLE                     (0)
  #define  USB_STATUS_TRANSFER_ACCEPTED        (6)
  #define  USB_STATUS_TRANSFER_PENDING         (2)
  #define  USB_STATUS_TRANSFER_IN_PROGRESS     (3)
  #define  USB_STATUS_ERROR                    (4)
  #define  USB_STATUS_DISABLED                 (5)
  #define  USB_STATUS_STALLED                  (1)
  #define  USB_STATUS_TRANSFER_QUEUED          (7)

  #define  USB_STATUS_UNKNOWN                  (0xFFFF)

  #define  USB_CONTROL_ENDPOINT     (0)

  #define  USB_RECV                 (0)
  #define  USB_SEND                 (1)

  #define  USB_DEVICE_DONT_ZERO_TERMINATE      (0x1)

  #define  USB_SETUP_DATA_XFER_DIRECTION       (0x80)

  #define  USB_SPEED_FULL                      (0)
  #define  USB_SPEED_LOW                       (1)
  #define  USB_SPEED_HIGH                      (2)

  #define  USB_MAX_PKTS_PER_UFRAME             (0x6)

  #define  USB_TEST_MODE_TEST_PACKET           (0x0400)

/* Available service types */
/* Services 0 through 15 are reserved for endpoints */
  #define  USB_SERVICE_EP0                     (0x00)
  #define  USB_SERVICE_EP1                     (0x01)
  #define  USB_SERVICE_EP2                     (0x02)
  #define  USB_SERVICE_EP3                     (0x03)
  #define  USB_SERVICE_EP4                     (0x04)
  #define  USB_SERVICE_EP5                     (0x05)
  #define  USB_SERVICE_EP6                     (0x06)
  #define  USB_SERVICE_EP7                     (0x07)
  #define  USB_SERVICE_EP8                     (0x08)
  #define  USB_SERVICE_EP9                     (0x09)
  #define  USB_SERVICE_EP10                    (0x0a)
  #define  USB_SERVICE_EP11                    (0x0b)
  #define  USB_SERVICE_EP12                    (0x0c)
  #define  USB_SERVICE_EP13                    (0x0d)
  #define  USB_SERVICE_EP14                    (0x0e)
  #define  USB_SERVICE_EP15                    (0x0f)

  #define  USB_SERVICE_BUS_RESET               (0x10)
  #define  USB_SERVICE_SUSPEND                 (0x11)
  #define  USB_SERVICE_SOF                     (0x12)
  #define  USB_SERVICE_RESUME                  (0x13)
  #define  USB_SERVICE_SLEEP                   (0x14)
  #define  USB_SERVICE_SPEED_DETECTION         (0x15)
  #define  USB_SERVICE_ERROR                   (0x16)
  #define  USB_SERVICE_STALL                   (0x17)
  #define  USB_SERVICE_MAX                     (0x18)

  #ifdef  DOUBLE_BUFFERING_USED
    #define  USB_SERVICE_MAX_EP         USB_SERVICE_EP6
  #else
    #define  USB_SERVICE_MAX_EP         USB_SERVICE_EP4
  #endif

/* Informational Request/Set Types */
/* component parameter in USB_Device_Get/Set_Status */
  #define USB_COMPONENT_DIRECTION_SHIFT        (7)
  #define USB_COMPONENT_DIRECTION_MASK         (0x01)
  #define  USB_STATUS_DEVICE_STATE             (0x01)
  #define  USB_STATUS_INTERFACE                (0x02)
  #define  USB_STATUS_ADDRESS                  (0x03)
  #define  USB_STATUS_CURRENT_CONFIG           (0x04)
  #define  USB_STATUS_SOF_COUNT                (0x05)
  #define  USB_STATUS_DEVICE                   (0x06)

  #ifdef OTG_BUILD
    #define  USB_STATUS_OTG                      (0x07)
    #define  USB_STATUS_TEST_MODE                (0x08)
  #else
    #define  USB_STATUS_TEST_MODE                (0x07)
  #endif

  #define  USB_STATUS_ENDPOINT                 (0x10)
  #define  USB_STATUS_ENDPOINT_NUMBER_MASK     (0x0F)


  #define UNINITIALISED_VAL                    (0xffffffff)

/******************************************************************************
 * Types
 *****************************************************************************/

  #pragma pack(1)

typedef struct _USB_DEV_EVENT_STRUCT
{
  INT8U           controller_ID;      /* conttroler ID           */
  INT8U           ep_num;
  BOOLEAN         setup;              /* is setup packet         */
  BOOLEAN         direction;          /* direction of endpoint   */
  INT8U           *buffer_ptr;        /* pointer to buffer       */
  INT16U          len;                /* buffer size of endpoint */
  INT8U           errors;             /* Any errors              */
}
USB_DEV_EVENT_STRUCT, *PTR_USB_DEV_EVENT_STRUCT;

typedef void (*USB_SERVICE_CALLBACK)(PTR_USB_DEV_EVENT_STRUCT);

typedef struct _USB_EP_STRUCT
{
  INT8U          ep_num;      /* endpoint number         */
  INT8U          type;        /* type of endpoint        */
  INT8U          direction;   /* direction of endpoint   */
  INT16U         size;        /* buffer size of endpoint */
}
USB_EP_STRUCT, *USB_EP_STRUCT_PTR;

  #pragma pack()

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern INT8U  USB_Device_Init(INT8U controller_ID, INT8U endpoints);
extern INT8U  USB_Device_DeInit(void);
extern INT8U  USB_Device_Init_EndPoint(INT8U controller_ID, USB_EP_STRUCT_PTR ep_ptr, INT8U  flag);
extern INT8U  USB_Device_Cancel_Transfer(INT8U controller_ID, INT8U ep_num, INT8U direction);
extern INT8U  USB_Device_Deinit_EndPoint(INT8U controller_ID, INT8U ep_num, INT8U direction);
extern INT8U  USB_Device_Recv_Data(INT8U controller_ID, INT8U ep_num, INT8U_ptr buff_ptr, INT16U  size);
extern INT8U  USB_Device_Send_Data(INT8U controller_ID, INT8U ep_num, INT8U_ptr buff_ptr, INT16U  size);
extern void   USB_Device_Shutdown(INT8U controller_ID);
extern void   USB_Device_Stall_EndPoint(INT8U controller_ID, INT8U ep_num, INT8U  direction);
extern void   USB_Device_Unstall_EndPoint(INT8U controller_ID, INT8U ep_num, INT8U direction);
extern void   USB_Device_Read_Setup_Data(INT8U controller_ID, INT8U ep_num, INT8U_ptr buff_ptr);
extern INT8U  USB_Device_Get_Status(INT8U controller_ID, INT8U component, INT16U_ptr status);
extern INT8U  USB_Device_Set_Status(INT8U controller_ID, INT8U component, INT16U setting);
extern void   USB_Device_Assert_Resume(INT8U controller_ID);
extern INT8U  USB_Device_Register_Service(INT8U controller_ID, INT8U type, USB_SERVICE_CALLBACK service);
extern INT8U  USB_Device_Unregister_Service(INT8U controller_ID, INT8U    type);
extern INT8U  USB_Device_Get_Transfer_Status(INT8U controller_ID, INT8U    ep_num, INT8U  direction);
extern void   USB_Device_Set_Address(INT8U controller_ID, INT8U    address);
extern INT8U  USB_Device_Call_Service(INT8U type, PTR_USB_DEV_EVENT_STRUCT  event);

#endif
