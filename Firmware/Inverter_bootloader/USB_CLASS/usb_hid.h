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
 **************************************************************************//*!
 *
 * @file usb_hid.h
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief The file contains USB stack HID class layer API header function.
 *
 *****************************************************************************/

#ifndef _USB_HID_H
#define _USB_HID_H

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "usb_descriptor.h"
#include "usb_class.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define MAX_QUEUE_ELEMS                 (4)

 /* class specific requests */
#define USB_HID_GET_REPORT_REQUEST      (0x01)
#define USB_HID_GET_IDLE_REQUEST        (0x02)
#define USB_HID_GET_PROTOCOL_REQUEST    (0x03)
#define USB_HID_SET_REPORT_REQUEST      (0x09)
#define USB_HID_SET_IDLE_REQUEST        (0x0A)
#define USB_HID_SET_PROTOCOL_REQUEST    (0x0B)

/* for class specific requests */
#define HIGH_BYTE_SHIFT                 (8)
#define MSB_MASK                        (0xFF00)
#define USB_HID_REQUEST_DIR_MASK        (0x08)
#define USB_HID_REQUEST_TYPE_MASK       (0x01)
#define REPORT_SIZE                     (4)
#define CLASS_REQ_DATA_SIZE             (0x01)

/******************************************************************************
 * Types
 *****************************************************************************/

 /* structure to hold a request in the endpoint queue */
typedef struct _usb_class_hid_queue
{
    INT8U controller_ID;   /* Controller ID*/
    INT8U channel;         /* Endpoint number */
    INT8U_ptr app_buff;    /* Buffer to send */
    INT16U size;   /* Size of the transfer */
}USB_CLASS_HID_QUEUE, *PTR_USB_CLASS_HID_QUEUE;

/* USB class hid endpoint data */
typedef struct _usb_class_hid_endpoint
{
    INT8U endpoint;        /* Endpoint number */
    INT8U type;            /* Type of endpoint (interrupt,
                               bulk or isochronous) */
    INT8U bin_consumer;    /* Num of queued elements */
    INT8U bin_producer;    /* Num of de-queued elements */
    USB_CLASS_HID_QUEUE queue[MAX_QUEUE_ELEMS]; /* Queue data */
}USB_CLASS_HID_ENDPOINT;

/* contains the endpoint data for non control endpoints */
typedef struct _usb_class_hid_endpoint_data
{
    /* Num of non control endpoints */
    INT8U count;
    /* contains the endpoint info */
    USB_CLASS_HID_ENDPOINT ep[HID_DESC_ENDPOINT_COUNT];
}USB_CLASS_HID_ENDPOINT_DATA, *PTR_USB_CLASS_HID_ENDPOINT_DATA;

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern INT8U USB_Class_HID_Init (
    INT8U                          controller_ID,
    USB_CLASS_CALLBACK              hid_class_callback,
    USB_REQ_FUNC                    vendor_req_callback,
    USB_CLASS_SPECIFIC_HANDLER_FUNC param_callback
);

extern INT8U USB_Class_HID_DeInit 
(
    INT8U controller_ID             
);

extern INT8U USB_Class_HID_Send_Data (
    INT8U           controller_ID,
    INT8U           ep_num,
    INT8U_ptr       buff_ptr,
    INT16U  size
);

#define USB_Class_HID_task USB_Class_Periodic_Task

#endif