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
* $FileName: usb_class.h$
* $Version : 3.8.7.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
* @brief The file contains USB stack class layer api header function.
*
*****************************************************************************/

#ifndef _USB_CLASS_H
#define _USB_CLASS_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>
#include <bsp.h>

#include "usb.h"
#include "usb_prv.h"

#include "devapi.h"

/******************************************************************************
 * Global Variables
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
 /*#define DELAYED_PROCESSING  1 This define is used to delay the control 
                                processing and not have it executed as part
                                of the interrupt routine */

#define USB_MAX_EP_BUFFER_SIZE   (64)
#define SOF_HIGH_BYTE_SHIFT      (8)

#define GET_STATUS_DEVICE_MASK           (0x0003)  
#define REMOTE_WAKEUP_STATUS_MASK        (0x0002) 
#define BUS_POWERED                      (0x80)   
#define SELF_POWERED                     (0x40)  
#define SELF_POWER_BIT_SHIFT             (6)     


/* USB Specs define CONTROL_MAX_PACKET_SIZE for High Speed device as only 64,
   whereas for FS its allowed to be 8, 16, 32 or 64 */
#define CONTROL_MAX_PACKET_SIZE       (64)

#if (HIGH_SPEED_DEVICE && (CONTROL_MAX_PACKET_SIZE != 64))
#error "For High Speed CONTROL_MAX_PACKET_SIZE should be 64"
#endif

#define UNINITIALISED_VAL        (0xffffffff)  
/* Events to the Application */
#define USB_APP_BUS_RESET           (0)
#define USB_APP_CONFIG_CHANGED      (1)
#define USB_APP_ENUM_COMPLETE       (2)
#define USB_APP_SEND_COMPLETE       (3)
#define USB_APP_DATA_RECEIVED       (4) 
#define USB_APP_ERROR               (5)
#define USB_APP_GET_DATA_BUFF       (6)
#define USB_APP_EP_STALLED          (7)
#define USB_APP_EP_UNSTALLED        (8) 
#define USB_APP_GET_TRANSFER_SIZE   (9)

/* identification values and masks to identify request types  */
#define USB_REQUEST_CLASS_MASK   (0x60) 
#define USB_REQUEST_CLASS_STRD   (0x00) 
#define USB_REQUEST_CLASS_CLASS  (0x20) 
#define USB_REQUEST_CLASS_VENDOR (0x40) 
#define USB_MAX_CLASS_OBJECT     (0x1)
/******************************************************************************
 * Types
 *****************************************************************************/
typedef int_32 USB_CLASS_HANDLE;

/* callback function pointer structure for Application to handle events */
typedef void(_CODE_PTR_ USB_CLASS_CALLBACK)(uint_8 event, void* val, pointer arg);

/* callback function pointer structure to handle USB framework request */
typedef uint_8 (_CODE_PTR_ USB_REQ_FUNC)(USB_SETUP_STRUCT *,
                                          uint_8_ptr*,
                                          uint_32*,pointer arg); 

/*callback function pointer structure for application to provide class params*/
typedef uint_8 (_CODE_PTR_ USB_CLASS_SPECIFIC_HANDLER_FUNC)(
                           uint_8,
                           uint_16, 
                           uint_8_ptr*,
                           uint_32*,
                           pointer arg);

/* Structure Representing class callback */
typedef struct usb_class_callback_struct
{
    USB_CLASS_CALLBACK  callback;
    pointer arg;
}USB_CLASS_CALLBACK_STRUCT,_PTR_ USB_CLASS_CALLBACK_STRUCT_PTR ;

/* Structure other request class callback */
typedef struct usb_req_callback_struct
{
    USB_REQ_FUNC  callback;
    pointer arg;
}USB_REQ_CALLBACK_STRUCT,_PTR_ USB_REQ_CALLBACK_STRUCT_PTR ;

typedef struct usb_class_specific_handler_callback_struct
{
    USB_CLASS_SPECIFIC_HANDLER_FUNC  callback;
    pointer arg;
}USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT,
_PTR_ USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT_PTR;

/* Strucutre holding USB class object state*/
typedef struct _usb_class_object
{
   uint_32 usb_fw_handle;
   _usb_device_handle controller_handle;
   pointer arg;
   USB_CLASS_CALLBACK class_callback;   
}USB_CLASS_OBJECT_STRUCT,_PTR_ USB_CLASS_OBJECT_STRUCT_PTR;

 typedef struct _USB_LANGUAGE 
{
    uint_16 language_id;
    uint_8 ** lang_desc;
    uint_8 * lang_desc_size;    
} USB_LANGUAGE;
 
typedef struct _USB_ALL_LANGUAGES 
{
    uint_8 *languages_supported_string;
    uint_8 languages_supported_size;
    /*Allocate Memory In App Layer*/
    USB_LANGUAGE *usb_language;
       
}USB_ALL_LANGUAGES;

/* Strucutre Representing Endpoints and number of endpoints user want*/
typedef struct _USB_ENDPOINTS
{
    uint_8 count;
    /*Allocate Memory In App Layer*/
    USB_EP_STRUCT *ep; 
}USB_ENDPOINTS;

 /* Callback Functions to be implemented by APP.*/
 typedef struct _usb_desc_callbackFunction_struct
 {
    uint_32 handle;
    uint_8 (_CODE_PTR_ GET_DESC)(uint_32 handle,uint_8 type,uint_8 str_num,
        uint_16 index,uint_8_ptr *descriptor,uint_32 *size);
    USB_ENDPOINTS * (_CODE_PTR_ GET_DESC_ENDPOINTS)(uint_32 handle);
    uint_8 (_CODE_PTR_ GET_DESC_INTERFACE)(uint_32 handle,uint_8 interface,
        uint_8_ptr alt_interface);
    uint_8 (_CODE_PTR_ SET_DESC_INTERFACE)(uint_32 handle,uint_8 interface,
        uint_8 alt_interface);
    boolean (_CODE_PTR_ IS_DESC_VALID_CONFIGURATION)(uint_32 handle,
        uint_16 config_val);
    boolean (_CODE_PTR_ DESC_REMOTE_WAKEUP)(uint_32 handle);
    uint_8 (_CODE_PTR_ DESC_SET_FEATURE)(uint_32 handle,int_32 cmd,
        uint_8 in_data,uint_8_ptr* feature);
    uint_8 (_CODE_PTR_ DESC_GET_FEATURE)(uint_32 handle,int_32 cmd,
        uint_8 in_data,uint_8_ptr * feature);
 }DESC_CALLBACK_FUNCTIONS_STRUCT, * DESC_CALLBACK_FUNCTIONS_STRUCT_PTR;

/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_Init
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle             :handle to Identify the controller
 * @param class_callback     :event callback      
 * @param other_req_callback :call back for class/vendor specific requests on 
 *                            CONTROL ENDPOINT
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
USB_CLASS_HANDLE USB_Class_Init
(
    _usb_device_handle handle, /* [IN] the USB device controller to initialize */                  
    USB_CLASS_CALLBACK class_callback,/*[IN]*/
    USB_REQ_FUNC       other_req_callback,/*[IN]*/
    pointer user_arg,/*[IN]*/
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr/*[IN]*/
); 

/**************************************************************************//*!
 *
 * @name  USB_Class_Deinit
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle             :handle to Identify the controller
 * @param class_handle       :class handle      
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint_8 USB_Class_Deinit
(
    _usb_device_handle handle, /* [IN] the USB device controller to initialize */                  
    USB_CLASS_HANDLE  class_handle
 );
 /**************************************************************************//*!
 *
 * @name  USB_Class_Send_Data
 *
 * @brief The funtion calls the device to send data upon recieving an IN token 
 *
 * @param handle:               handle to Identify the controller
 * @param ep_num:               The endpoint number     
 * @param buff_ptr:             buffer to send
 * @param size:                 length of transfer
 * 
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint_8 USB_Class_Send_Data
(
    USB_CLASS_HANDLE handle, /*[IN]*/
    uint_8 ep_num,        /* [IN] the Endpoint number */                  
    uint_8_ptr buff_ptr,      /* [IN] buffer to send */      
    uint_32 size           /* [IN] length of the transfer */
); 

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Desc
 *
 * @brief  This function is called in to get the descriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param out_buf           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Class_Get_Desc(USB_CLASS_HANDLE handle,/*[IN]*/
int_32 cmd,/*[IN]*/
uint_8 input_data,/*[IN]*/
uint_8_ptr *in_buf /*[OUT]*/
);

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Desc
 *
 * @brief  This function is called in to Set the descriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buf           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Class_Set_Desc(USB_CLASS_HANDLE handle,/*[IN]*/
int_32 cmd,/*[IN]*/
uint_8 input_data,/*[IN]*/
uint_8_ptr *out_buf /*[IN]*/
);

/**************************************************************************//*!
 *
 * @name   USB_Class_Periodic_Task
 *
 * @brief  The funtion calls for periodic tasks 
 *
 * @param  None
 *
 * @return None
 *
 *****************************************************************************/
extern void USB_Class_Periodic_Task(void);

#endif

/* EOF */
