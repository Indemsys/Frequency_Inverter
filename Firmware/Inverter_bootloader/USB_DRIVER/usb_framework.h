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
 * @file usb_framwork.h
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief The file contains USB Framework module API header function.
 *
 *****************************************************************************/

#ifndef _USB_FRAMEWORK_H
#define _USB_FRAMEWORK_H

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "usb_class.h"
#include "usb_descriptor.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define MAX_STRD_REQ                (13)  /* Max value of standard request */
/* size of data to be returned for various Get Desc calls */
#define DEVICE_STATUS_SIZE          (2)
#ifdef OTG_BUILD
#define OTG_STATUS_SIZE             (2)
#endif
#define INTERFACE_STATUS_SIZE       (1)
#define CONFIG_SIZE                 (1)
#define FRAME_SIZE                  (2)
#define ENDP_STATUS_SIZE            (2)

#ifdef OTG_BUILD
#define DEVICE_SET_FEATURE_B_HNP_ENABLE   (0x0003) /* B HNP enable  SET/CLEAR feature value */
#define DEVICE_SET_FEATURE_A_HNP_SUPPORT  (0x0004) /* A HNP support SET/CLEAR feature value */
#endif

#define DEVICE_FEATURE_REMOTE_WAKEUP      (0x0001)
#define DEVICE_SET_FEATURE_MASK           ((INT16U)(1<<(DEVICE_FEATURE_REMOTE_WAKEUP)))
#define DEVICE_CLEAR_FEATURE_MASK         ((INT16U)(1<<(DEVICE_FEATURE_REMOTE_WAKEUP)))


#define REPORT_DESCRIPTOR_TYPE      (0x22)
#define STRING_DESCRIPTOR_TYPE      (0x03)

/* masks and values for provides of Get Desc information */
#define USB_REQUEST_SRC_MASK        (0x03)
#define USB_REQUEST_SRC_DEVICE      (0x00)
#define USB_REQUEST_SRC_INTERFACE   (0x01)
#define USB_REQUEST_SRC_ENDPOINT    (0x02)

#define  USB_SET_REQUEST_MASK       (0x02)

/* wIndex values for GET_Status */
#ifdef OTG_BUILD
#define USB_WINDEX_OTG_STATUS_SEL   (0xF000)
#endif

/* for transfer direction check */
#define USB_DATA_TO_HOST            (0x80)
#define USB_DATA_TO_DEVICE          (0x00)
#define USB_DATA_DIREC_MASK         (0x80)

#define USB_uint_16_low(x)  ((INT8U)x)       /* lsb byte */
#define USB_uint_16_high(x) ((INT8U)(x>>8))  /* msb byte */

#define BYTE_SWAP16(a) 	(a)

/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern BOOLEAN USB_Frame_Remote_Wakeup(INT8U controller_ID);

#define USB_Frame_Remote_Wakeup USB_Desc_Remote_Wakeup

extern INT8U USB_Framework_Init (
    INT8U              controller_ID,
    USB_CLASS_CALLBACK  callback,
    USB_REQ_FUNC        other_req_callback
);

extern INT8U USB_Framework_DeInit
(
    INT8U controller_ID      
);

extern INT8U USB_Framework_Reset (
    INT8U    controller_ID
);
#ifdef DELAYED_PROCESSING
extern void USB_Framework_Periodic_Task(void);
#endif

#endif
