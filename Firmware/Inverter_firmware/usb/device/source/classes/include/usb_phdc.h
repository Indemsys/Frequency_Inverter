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
* $FileName: usb_phdc.h$
* $Version : 3.8.5.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
* @brief The file contains USB stack PHDC class layer api header function.
*
*****************************************************************************/

#ifndef _USB_PHDC_H
#define _USB_PHDC_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "devapi.h" 
#include "usb_class.h"
#include "usb_stack_config.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/

#define MAX_QOS_BIN_ELEMS                 (4) 
 /* the num of recieve endpoints */ 
#define PHDC_RX_ENDPOINTS                 (1)  
/* the num of transmit endpoints */ 
#define PHDC_TX_ENDPOINTS                 (2)   
#define SET_FEATURE_REQUEST               (3)
#define CLEAR_FEATURE_REQUEST             (1)
#define GET_STATUS_REQUEST                (0)
#define INVALID_VAL                       (0xFF)

#define PHDC_BULK_OUT_QOS         (0x88)
#define PHDC_BULK_IN_QOS          (0x88)
#define PHDC_INT_OUT_QOS          (0x88)
#define PHDC_INT_IN_QOS           (0x88)

#define  USB_SET_REQUEST_MASK             (0x02)

#define USB_APP_META_DATA_PARAMS_CHANGED  (0xF2)
#define USB_APP_FEATURE_CHANGED           (0xF3)
#define MAX_PHDC_OBJECT                   (0x01)

#if USB_METADATA_SUPPORTED
    #define META_DATA_MSG_PRE_IMPLEMENTED    (1)/*TRUE:1; FALSE:0*/
#else
    #define META_DATA_MSG_PRE_IMPLEMENTED    (0)/*TRUE:1; FALSE:0*/
#endif

#define USB_Class_PHDC_Periodic_Task USB_Class_Periodic_Task

/******************************************************************************
 * Types
 *****************************************************************************/
 
typedef uint_32 PHDC_HANDLE;

/* structure to hold a request in the endpoint QOS bin */

PACKED_STRUCT_BEGIN
struct _usb_class_phdc_qos_bin 
{
    uint_8 channel;       /* endpoint num */
    boolean meta_data;    /* packet is a meta data or not */
    uint_8 num_tfr;       /* num of transfers that follow the meta data packet.
                             used only when meta_data is TRUE */                               
    uint_8 qos;           /* qos of the transfers that follow the meta data 
                             packet */
    uint_8_ptr app_buff;  /* buffer to send */
    uint_32 size; /* size of the transfer */ 
} 
PACKED_STRUCT_END;

typedef struct _usb_class_phdc_qos_bin USB_CLASS_PHDC_QOS_BIN, *PTR_USB_CLASS_PHDC_QOS_BIN;

/* USB class phdc endpoint data */
  
typedef struct _usb_class_phdc_tx_endpoint 
{
    uint_8 endpoint;                     /* from the application */
    uint_8 type;                         /* from the application */
    uint_32 size;                         /* from the application */
    uint_8 qos;                          /* from the application */
    uint_8 current_qos;               /*from received meta data */
    uint_8 transfers_left;            /*from application meta data */
    uint_8 bin_consumer;              /* num of dequeued transfers */
    uint_8 bin_producer;              /* num of queued transfers */
    USB_CLASS_PHDC_QOS_BIN qos_bin[MAX_QOS_BIN_ELEMS]; 
}USB_CLASS_PHDC_TX_ENDPOINT;

typedef struct _usb_class_phdc_rx_endpoint 
{
    uint_8 endpoint;                     /* from the application */
    uint_8 type;                         /* from the application */
    uint_32 size;                         /* from the application */
    uint_8 qos;                          /* from the application */
    uint_8 current_qos;                  /*from received meta data */    
    uint_8 transfers_left;               /*from received meta data */
    uint_16 buffer_size;
    uint_8_ptr buff_ptr;
}USB_CLASS_PHDC_RX_ENDPOINT;
  
typedef struct _usb_class_phdc_endpoint_data 
{
    _usb_device_handle handle;
    uint_8 count_rx;
    uint_8 count_tx;    
    USB_CLASS_PHDC_RX_ENDPOINT ep_rx[PHDC_RX_ENDPOINTS];  
    USB_CLASS_PHDC_TX_ENDPOINT ep_tx[PHDC_TX_ENDPOINTS];      
}USB_CLASS_PHDC_ENDPOINT_DATA, *PTR_USB_CLASS_PHDC_ENDPOINT_DATA; 

typedef struct _usb_class_phdc_rx_buff 
{
    uint_8_ptr in_buff;  /* Pointer to input Buffer */
    uint_32 in_size; /* Length of Input Buffer*/
    uint_16 out_size; /* Size of Output Buffer */
    uint_8_ptr out_buff; /* Pointer to Output Buffer */
    uint_16 transfer_size;
#if USB_METADATA_SUPPORTED
    boolean meta_data_packet;/* meta data packet flag */
#endif
}USB_CLASS_PHDC_RX_BUFF, *PTR_USB_CLASS_PHDC_RX_BUFF;
  
/* event structures */ 
typedef struct _usb_app_event_send_complete 
{
    uint_8 qos;
    uint_8_ptr buffer_ptr;
    uint_32 size;
}USB_APP_EVENT_SEND_COMPLETE, *PTR_USB_APP_EVENT_SEND_COMPLETE;


typedef struct _usb_app_event_data_recieved 
{
    uint_8 qos;
    uint_8_ptr buffer_ptr;
    uint_32 size;
}USB_APP_EVENT_DATA_RECIEVED, *PTR_USB_APP_EVENT_DATA_RECIEVED;
  
#if USB_METADATA_SUPPORTED
    #define METADATA_PREAMBLE_SIGNATURE     (16)
    #define METADATA_QOSENCODING_VERSION    (1)
    #define METADATA_HEADER_SIZE            (21)
    /* structure for meta_data msg preamble */
    typedef struct _usb_meta_data_msg_preamble 
    {
        char signature[METADATA_PREAMBLE_SIGNATURE];
        uint_8 num_tfr;
        uint_8 version;
        uint_8 qos;
        uint_8 opaque_data_size;
        uint_8 opaque_data[1];
    }USB_META_DATA_MSG_PREAMBLE;

    typedef struct _usb_app_event_metadata_params 
    {
        uint_8 channel;
        uint_8 num_tfr;
        uint_8 qos;
        uint_8_ptr metadata_ptr;
        uint_32 size;
    }USB_APP_EVENT_METADATA_PARAMS, *PTR_USB_APP_EVENT_METADATA_PARAMS;
#endif

/* Structure Representing PHDC class */
typedef struct _phdc_struct
{
     _usb_device_handle  controller_handle;
    PHDC_HANDLE phdc_handle;
    USB_CLASS_HANDLE class_handle;
    USB_CLASS_CALLBACK_STRUCT phdc_callback;
    USB_REQ_CALLBACK_STRUCT vendor_callback; 
    /* RAM buffer for configuring next receive */
    uint_8_ptr service_buff_ptr; 
    USB_CLASS_PHDC_ENDPOINT_DATA ep_data;
    #if META_DATA_MSG_PRE_IMPLEMENTED
        USB_META_DATA_MSG_PREAMBLE meta_data_msg_preamble; 
    #endif
    #if USB_METADATA_SUPPORTED
        /* used to store whether meta-data feature is active or not */
        boolean phdc_metadata;
    #endif 
    /* used to store a bit map of the active endpoints */
    uint_16 phdc_ep_has_data;
}PHDC_STRUCT, _PTR_ PHDC_STRUCT_PTR;

/* Structures used to configure PHDC class by  APP*/
typedef struct _config_phdc_struct
{
    //uint_32 desc_endpoint_cnt;
    USB_CLASS_CALLBACK_STRUCT phdc_callback;
    USB_REQ_CALLBACK_STRUCT vendor_callback; 
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr;  
    USB_ENDPOINTS * info;  
}PHDC_CONFIG_STRUCT,_PTR_ PHDC_CONFIG_STRUCT_PTR;
 
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_PHDC_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param  phdc_config_ptr[IN]  : Phdc configuration structure pointer
 * @return PHDC_HANDLE      : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 * This function initializes the PHDC Class layer and layers it is dependednt on 
 *****************************************************************************/                          
extern PHDC_HANDLE USB_Class_PHDC_Init
(
   PHDC_CONFIG_STRUCT_PTR phdc_config_ptr
);
/**************************************************************************//*!
 *
 * @name  USB_PHDC_Class_Recv_Data
 *
 * @brief This fucntion is used by Application to receive data through PHDC class
 *
 * @param phdc_handle     :   Handle returned by USB_Class_PHDC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 * This fucntion is used by Application to send data through PHDC class 
 *****************************************************************************/  
extern uint_8 USB_Class_PHDC_Recv_Data
(
    PHDC_HANDLE         phdc_handle,
    uint_8              qos, 
    uint_8_ptr          buff_ptr,      /* [IN] buffer to send */      
    uint_32             size           /* [IN] length of the transfer */
);
/**************************************************************************//*!
 *
 * @name  USB_Class_PHDC_Send_Data
 *
 * @brief This fucntion is used by Application to send data through PHDC class
 *
 * @param handle          :   handle returned by USB_Class_PHDC_Init
 * @param meta_data       :   packet is meta data or not
 * @param num_tfr         :   no. of transfers
 * @param qos             :   current qos of the transfer
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 * This fucntion is used by Application to send data through PHDC class 
 *****************************************************************************/  
extern uint_8 USB_Class_PHDC_Send_Data
(
    PHDC_HANDLE      handle,    
    boolean          meta_data,    /* opaque meta data in app buffer */
    uint_8           num_tfr,      /* no. of transfers to follow with given 
                                      channel--only valid if meta data is 
                                      true */
    uint_8           current_qos,  /* qos of the transfers to follow--only 
                                      valid if meta data is true */
    uint_8_ptr       app_buff,     /* buffer holding application data */
    uint_32  size           /* [IN] length of the transfer */
);
                               

#endif
