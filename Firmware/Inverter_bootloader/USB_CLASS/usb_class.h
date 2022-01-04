/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
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
  * @file usb_class.h
  *
  * @author
  *
  * @version
  *
  * @date May-28-2009
  *
  * @brief The file contains USB stack class layer API header function.
  *
  *****************************************************************************/

#ifndef _USB_CLASS_H
  #define _USB_CLASS_H


/*#define DELAYED_PROCESSING  1 This define is used to delay the control
                                processing and not have it executed as part
                                of the interrupt routine */
/******************************************************************************
 * Includes
 *****************************************************************************/

  #include "usb_devapi.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
  #define SOF_HIGH_BYTE_SHIFT                 (8)
  #define GET_STATUS_DEVICE_MASK              (0x0003)
  #ifdef OTG_BUILD
    #define GET_STATUS_OTG_MASK                 (0x0001)
  #endif
  #define REMOTE_WAKEUP_STATUS_MASK           (0x0002)
  #define BUS_POWERED                         (0x80)
  #define SELF_POWERED                        (0x40)
  #define SELF_POWER_BIT_SHIFT                (6)

/* Events to the Application */
  #define USB_APP_BUS_RESET                   (0)
  #define USB_APP_CONFIG_CHANGED              (1)
  #define USB_APP_ENUM_COMPLETE               (2)
  #define USB_APP_SEND_COMPLETE               (3)
  #define USB_APP_DATA_RECEIVED               (4)
  #define USB_APP_ERROR                       (5)
  #define USB_APP_GET_DATA_BUFF               (6)
  #define USB_APP_EP_STALLED                  (7)
  #define USB_APP_EP_UNSTALLED                (8)
  #define USB_APP_GET_TRANSFER_SIZE           (9)

/* max packet size for the control endpoint */

/* USB Specs define CONTROL_MAX_PACKET_SIZE for High Speed device as only 64,
   whereas for FS its allowed to be 8, 16, 32 or 64 */

  #if HIGH_SPEED_DEVICE
    #define CONTROL_MAX_PACKET_SIZE             (64) /* max supported value is 64*/
  #else
    #define CONTROL_MAX_PACKET_SIZE             (64) /* max supported value is 16*/
  #endif

  #if (HIGH_SPEED_DEVICE && (CONTROL_MAX_PACKET_SIZE != 64))
    #error "For High Speed CONTROL_MAX_PACKET_SIZE should be 64"
  #endif

/* identification values and masks to identify request types  */
  #define USB_REQUEST_CLASS_MASK              (0x60)
  #define USB_REQUEST_CLASS_STRD              (0x00)
  #define USB_REQUEST_CLASS_CLASS             (0x20)
  #define USB_REQUEST_CLASS_VENDOR            (0x40)

/******************************************************************************
 * Types
 *****************************************************************************/
/* eight byte usb setup packet structure */
typedef struct _USB_SETUP_STRUCT {
  INT8U      request_type;    /* Bmrequest type */
  INT8U      request;         /* Request code */
  INT16U     value;           /* Wvalue */
  INT16U     index;           /* Windex */
  INT16U     length;          /* Length of the data */
} USB_SETUP_STRUCT;

/* callback function pointer structure for Application to handle events */
typedef void (*USB_CLASS_CALLBACK)(INT8U, INT8U, void *);

/* callback function pointer structure to handle USB framework request */
typedef INT8U(*USB_REQ_FUNC)(INT8U, USB_SETUP_STRUCT *,
                             INT8U_ptr *,
                             INT16U *);

/*callback function pointer structure for application to provide class params*/
typedef INT8U(*USB_CLASS_SPECIFIC_HANDLER_FUNC)(
                                                INT8U,
                                                INT16U,
                                                INT8U_ptr *,
                                                INT16U *);

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern INT8U USB_Class_Init(
                            INT8U    controller_ID,
                            USB_CLASS_CALLBACK class_callback,
                            USB_REQ_FUNC       other_req_callback
                            );

extern INT8U USB_Class_DeInit
(
 INT8U    controller_ID
 );

extern INT8U USB_Class_Send_Data(
                                 INT8U           controller_ID,
                                 INT8U           ep_num,
                                 INT8U_ptr       buff_ptr,
                                 INT16U  size
                                 );

extern void USB_Class_Periodic_Task(void);

#endif
