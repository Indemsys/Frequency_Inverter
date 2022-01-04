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
* $FileName: usb_cdc.h$
* $Version : 3.8.5.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
* @brief The file contains USB stack CDC class layer api header function.
*
*****************************************************************************/

#ifndef _USB_CDC_H
#define _USB_CDC_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "devapi.h"   
#include "usb_class.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/* if this macro is not zero, it implies that FEC is present in Soc */
#if BSPCFG_RX_RING_LEN 
    #define RNDIS_SUPPORT                    (1)/* TRUE:1, FALSE:0*/ 
#else   
    #define RNDIS_SUPPORT                    (0)/* TRUE:1, FALSE:0*/ 
#endif

/******************************************************************************
 * Macro's
 *****************************************************************************/
/* Class specific request Codes */
#define SEND_ENCAPSULATED_COMMAND       (0x00)
#define GET_ENCAPSULATED_RESPONSE       (0x01)
#define SET_COMM_FEATURE                (0x02)
#define GET_COMM_FEATURE                (0x03)
#define CLEAR_COMM_FEATURE              (0x04)
#define SET_AUX_LINE_STATE              (0x10)
#define SET_HOOK_STATE                  (0x11)
#define PULSE_SETUP                     (0x12)
#define SEND_PULSE                      (0x13)
#define SET_PULSE_TIME                  (0x14)
#define RING_AUX_JACK                   (0x15)
#define SET_LINE_CODING                 (0x20)
#define GET_LINE_CODING                 (0x21)
#define SET_CONTROL_LINE_STATE          (0x22)
#define SEND_BREAK                      (0x23)
#define SET_RINGER_PARAMS               (0x30)
#define GET_RINGER_PARAMS               (0x31)
#define SET_OPERATION_PARAM             (0x32)
#define GET_OPERATION_PARAM             (0x33)
#define SET_LINE_PARAMS                 (0x34)
#define GET_LINE_PARAMS                 (0x35)
#define DIAL_DIGITS                     (0x36)
#define SET_UNIT_PARAMETER              (0x37)
#define GET_UNIT_PARAMETER              (0x38)
#define CLEAR_UNIT_PARAMETER            (0x39)
#define GET_PROFILE                     (0x3A)
#define SET_ETHERNET_MULTICAST_FILTERS  (0x40)
#define SET_ETHERNET_POW_PATTER_FILTER  (0x41)
#define GET_ETHERNET_POW_PATTER_FILTER  (0x42)
#define SET_ETHERNET_PACKET_FILTER      (0x43)
#define GET_ETHERNET_STATISTIC          (0x44)
#define SET_ATM_DATA_FORMAT             (0x50)
#define GET_ATM_DEVICE_STATISTICS       (0x51)
#define SET_ATM_DEFAULT_VC              (0x52)
#define GET_ATM_VC_STATISTICS           (0x53)
#define MDLM_SPECIFIC_REQUESTS_MASK     (0x7F)

/* Class Specific Notification Codes */
#define NETWORK_CONNECTION_NOTIF        (0x00)
#define RESPONSE_AVAIL_NOTIF            (0x01)
#define AUX_JACK_HOOK_STATE_NOTIF       (0x08)
#define RING_DETECT_NOTIF               (0x09)
#define SERIAL_STATE_NOTIF              (0x20)
#define CALL_STATE_CHANGE_NOTIF         (0x28)
#define LINE_STATE_CHANGE_NOTIF         (0x29)
#define CONNECTION_SPEED_CHANGE_NOTIF   (0x2A)
#define MDLM_SPECIFIC_NOTIF_MASK        (0x5F)

/* Events to the Application */ /* 0 to 4 are reserved for class events */
#define USB_APP_CDC_CARRIER_DEACTIVATED   (0x21)
#define USB_APP_CDC_CARRIER_ACTIVATED     (0x22)
#define USB_APP_CDC_DTE_DEACTIVATED       (0x23)
#define USB_APP_CDC_DTE_ACTIVATED         (0x24)
#define USB_APP_GET_LINK_SPEED            (0x25)    
#define USB_APP_GET_LINK_STATUS           (0x26)    

/* other macros */
#define NOTIF_PACKET_SIZE             (0x08)
#define NOTIF_REQUEST_TYPE            (0xA1)
#define PSTN_SUBCLASS_NOTIF_SUPPORT   (1)/*TRUE*/

/* macros for queuing */
#define CDC_MAX_QUEUE_ELEMS              (16)  

#define GET_ABSTRACT_STATE               (0x23)
#define GET_COUNTRY_SETTING              (0x24)

#define SET_ABSTRACT_STATE                (0x25)
#define SET_COUNTRY_SETTING              (0x26)
#define MAX_CDC_DEVICE                    (0x01)
 
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
void USB_Service_Cdc_Notif(PTR_USB_EVENT_STRUCT event,pointer arg);
void USB_Service_Dic_Bulk_In(PTR_USB_EVENT_STRUCT event,pointer arg);
void USB_Service_Dic_Bulk_Out(PTR_USB_EVENT_STRUCT event,pointer arg);

void USB_Class_CDC_Event(uint_8 event, void* val,pointer arg) ; 
/******************************************************************************
 * Types
 *****************************************************************************/
/* Type representing CDC class handle*/ 
typedef uint_32 CDC_HANDLE;

typedef uint_32 _ip_address;

typedef struct _app_data_struct 
{
    uint_8_ptr      data_ptr;         /* pointer to buffer       */            
    uint_32 data_size;                /* buffer size of endpoint */
}APP_DATA_STRUCT;
 
/* structure to hold a request in the endpoint queue */
typedef struct _usb_class_cdc_queue 
{
    _usb_device_handle  handle;
    uint_8 channel;       
    APP_DATA_STRUCT app_data;  
}USB_CLASS_CDC_QUEUE, *PTR_USB_CLASS_CDC_QUEUE;
 
/* USB class cdc endpoint data */
typedef struct _usb_class_cdc_endpoint 
{
    uint_8 endpoint; /* endpoint num */                    
    uint_8 type;     /* type of endpoint (interrupt, bulk or isochronous) */   
    uint_8 bin_consumer;/* the num of queued elements */
    uint_8 bin_producer;/* the num of de-queued elements */
    USB_CLASS_CDC_QUEUE queue[CDC_MAX_QUEUE_ELEMS]; /* queue data */  
}USB_CLASS_CDC_ENDPOINT;

typedef struct _cdc_variable_struct
{
    CDC_HANDLE cdc_handle;
    USB_CLASS_HANDLE class_handle;
    _usb_device_handle controller_handle;
    USB_ENDPOINTS *usb_ep_data;
    uint_32 comm_feature_data_size;
    uint_8  cic_send_endpoint;
    uint_8  cic_recv_endpoint;
    uint_8  dic_send_endpoint;
    uint_8  dic_recv_endpoint;
    uint_32 dic_recv_pkt_size;
    uint_32 dic_send_pkt_size;
    uint_32 cic_send_pkt_size;    
    /* Holds the PSTN object*/
    pointer pstn_obj_ptr;
    uint_8 max_supported_interfaces;
    USB_CLASS_CALLBACK_STRUCT cdc_class_cb;
    USB_REQ_CALLBACK_STRUCT       vendor_req_callback;            
    USB_CLASS_CALLBACK_STRUCT param_callback; 
    /* contains the endpoint info */
    USB_CLASS_CDC_ENDPOINT *ep;
    
    #if RNDIS_SUPPORT   
        /* rndis specific configuration */
        _enet_address mac_address;
        _ip_address ip_address;
        uint_32 rndis_max_frame_size;
    #endif
    
 }CDC_DEVICE_STRUCT, _PTR_ CDC_DEVICE_STRUCT_PTR;
 
 typedef struct _cdc_config_struct
 {
    uint_32 comm_feature_data_size;
    uint_8 cic_send_endpoint;
    uint_8 dic_send_endpoint;
    uint_8 dic_recv_endpoint;
    uint_32 dic_recv_pkt_size;
    uint_32 dic_send_pkt_size;
    uint_32 cic_send_pkt_size;
    uint_8 max_supported_interfaces;
    USB_ENDPOINTS *usb_ep_data;
    uint_32   desc_endpoint_cnt;
    USB_CLASS_CALLBACK_STRUCT  cdc_class_cb;
    USB_REQ_CALLBACK_STRUCT    vendor_req_callback;
    USB_CLASS_CALLBACK_STRUCT  param_callback;
    USB_CLASS_CDC_ENDPOINT *ep;
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr; 
    
    #if RNDIS_SUPPORT   
        /* rndis specific configuration */
        _enet_address mac_address;
        _ip_address ip_address;
        uint_32 rndis_max_frame_size;
    #endif
    
 }CDC_CONFIG_STRUCT,_PTR_ CDC_CONFIG_STRUCT_PTR;
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param *cdc_config_ptr[IN]:  This structure contians configuration parameter
 *                              send by APP to configure CDC class.
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the CDC Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
extern uint_32 USB_Class_CDC_Init
(
 CDC_CONFIG_STRUCT_PTR cdc_config_ptr
);
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Deinit
 *
 * @brief The funtion deinitializes the Device and Controller layer 
 *
 * @param cdc_handle:  
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the CDC Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
extern uint_32 USB_Class_CDC_Deinit
(
  CDC_HANDLE cdc_handle
);
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Send_Data
 *
 * @brief  This function sends data to Host.
 *
 * @param handle          :   handle returned by USB_Class_CDC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
extern uint_8 USB_Class_CDC_Send_Data
(
    CDC_HANDLE          cdc_handle,
    uint_8              ep_num,
    uint_8_ptr          buff_ptr,      /* [IN] buffer to send */      
    uint_32             size           /* [IN] length of the transfer */
);
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Recv_Data
 *
 * @brief This functions receives Data from Host.
 *
 * @param handle          :   handle returned by USB_Class_CDC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
extern uint_8 USB_Class_CDC_Recv_Data
(
    CDC_HANDLE          cdc_handle,
    uint_8              ep_num,
    uint_8_ptr          buff_ptr,      /* [IN] buffer to send */      
    uint_32             size           /* [IN] length of the transfer */
);

/******************************************************************************
 * Global Functions of Subclass
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Pstn_Init
 *
 * @brief The funtion initializes the Pstn Module 
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 * @param class_callback:       event callback 
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint_8 USB_Pstn_Init
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_CLASS_CALLBACK_STRUCT_PTR pstn_cb
); 
/**************************************************************************//*!
 *
 * @name  USB_Pstn_Deinit
 *
 * @brief The funtion initializes the Pstn Module 
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 * @param class_callback:       event callback 
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint_8 USB_Pstn_Deinit
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr
);
/**************************************************************************//*!
 *
 * @name  PSTN_Get_Line_Coding
 *
 * @brief  This function is called in response to GetLineCoding request
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
extern uint_8 PSTN_Get_Line_Coding(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                                   USB_SETUP_STRUCT * setup_packet,
                                   uint_8_ptr *data, 
                                   uint_32 *size);
                                   
/**************************************************************************//*!
 *
 * @name  PSTN_Set_Line_Coding
 *
 * @brief  This function is called in response to SetLineCoding request
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
extern uint_8 PSTN_Set_Line_Coding(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                                   USB_SETUP_STRUCT * setup_packet,
                                   uint_8_ptr *data, 
                                   uint_32 *size);  
                                   
/**************************************************************************//*!
 *
 * @name  PSTN_Set_Ctrl_Line_State
 *
 * @brief  This function is called in response to Set Control Line State 
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/ 
extern uint_8 PSTN_Set_Ctrl_Line_State(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                                       USB_SETUP_STRUCT * setup_packet, 
                                       uint_8_ptr *data, 
                                       uint_32 *size);
                                       
/**************************************************************************//*!
 *
 * @name  PSTN_Send_Break
 *
 * @brief  This function is called in response to Set Config request
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/ 
 extern uint_8 PSTN_Send_Break(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                              USB_SETUP_STRUCT * setup_packet, 
                              uint_8_ptr *data, 
                              uint_32 *size);  
                              
/**************************************************************************//*!
 *
 * @name  PSTN_Get_Comm_Feature
 *
 * @brief  This function is called in response to GetCommFeature request
 *
 * @param cdc_obj_ptr :     Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
extern uint_8 PSTN_Get_Comm_Feature(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                                    USB_SETUP_STRUCT * setup_packet,
                                    uint_8_ptr *data, 
                                    uint_32 *size);
                                    
/**************************************************************************//*!
 *
 * @name  PSTN_Set_Comm_Feature
 *
 * @brief  This function is called in response to SetCommFeature request
 *
 * @param cdc_obj_ptr :     Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
extern uint_8 PSTN_Set_Comm_Feature(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                                    USB_SETUP_STRUCT * setup_packet,
                                    uint_8_ptr *data, 
                                    uint_32 *size);   
                                      
/**************************************************************************//*!
 *
 * @name  PSTN_Send_Serial_State
 *
 * @brief  This function is called to send serial state notification
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 *
 * @return NONE
 *****************************************************************************/ 
extern void PSTN_Send_Serial_State(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr) ;

/**************************************************************************//*!
 *
 * @name  PSTN_Response_Available
 *
 * @brief  This function is called to send notification to host that a 
 *         response is available
 *
 * @param cdc_obj_ptr   
 *
 * @return NONE
 *****************************************************************************/ 
extern void PSTN_Response_Available(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr);

#if RNDIS_SUPPORT
/**************************************************************************//*!
 *
 * @name  PSTN_Rndis_Message_Set
 *
 * @brief  This function is called in response to PSTN_Rndis_Message_Set 
 *
 * @param cdc_obj_ptr :     Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
 extern uint_8 PSTN_Rndis_Message_Set(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                                    USB_SETUP_STRUCT * setup_packet,
                                    uint_8_ptr *data, 
                                    uint_32 *size); 
/**************************************************************************//*!
 *
 * @name  PSTN_Rndis_Message_Get
 *
 * @brief  This function is called in response to PSTN_Rndis_Message_Get
 *
 * @param cdc_obj_ptr :     Pointer to CDC class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/                                          
extern uint_8 PSTN_Rndis_Message_Get(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
                                    USB_SETUP_STRUCT * setup_packet,
                                    uint_8_ptr *data, 
                                    uint_32 *size);    
                                    

extern void RNDIS_Initialize_Command
    (
        CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
        uint_8_ptr *data, 
        uint_32 *size
    );
extern void RNDIS_Query_Command
    (
        CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
        uint_8_ptr *data, 
        uint_32 *size
    );
extern void RNDIS_Set_Command
    (
        CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
        uint_8_ptr *data, 
        uint_32 *size
    );
extern void RNDIS_Reset_Command
    (
        CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
        uint_8_ptr *data, 
        uint_32 *size
    );
extern void RNDIS_Indicate_Status_Command 
    (
        CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
        uint_8_ptr *data, 
        uint_32 *size
    );      
extern void RNDIS_Keepalive_Command
    (
        CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
        uint_8_ptr *data, 
        uint_32 *size
    );
extern void RNDIS_Halt_Command
    (
        CDC_DEVICE_STRUCT_PTR cdc_obj_ptr
    );                  
            
#endif/*endif RNDIS_SUPPORT*/


#define USB_CDC_Periodic_Task USB_Class_Periodic_Task                          

#endif

/* EOF */
