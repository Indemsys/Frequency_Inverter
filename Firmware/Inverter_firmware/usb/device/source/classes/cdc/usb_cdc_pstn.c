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
* $FileName: usb_cdc_pstn.c$
* $Version : 3.8.5.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
* @brief The file contains USB CDC_PSTN module implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
 #include "usb_stack_config.h"
 #if CDC_CONFIG 
 #include "usb_cdc_pstn.h"
 
 /*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
#define MAX_EXPECTED_COMMAND_SIZE           (76)
/* this is the max observed command size we get on control endpoint --
   we are allocating memory for command at initialization, instead of 
   dynamically allocating it when command is received, to avoid memory 
   fragmentation */
#define MAX_EXPECTED_RESPONSE_SIZE          (RESPONSE_RNDIS_QUERY_MSG_SIZE + \
                                             (NUM_OIDS_SUPPORTED << 2))
    /* left shifted by 2 to have effect of mutiplication by 4 i.e. size of uint_32 */ 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/              

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/                                                                                                                               
uint_8 g_serial_state_buf[NOTIF_PACKET_SIZE+UART_BITMAP_SIZE] = 
    { NOTIF_REQUEST_TYPE,SERIAL_STATE_NOTIF,
      0x00,0x00,/*wValue*/
      0x00,0x00,/*interface - modifies*/
      UART_BITMAP_SIZE,0x00,/* wlength*/
      0x00,0x00 /*data initialized - modifies*/
    };/*uart bitmap*/

uint_8 g_response_available_data[NOTIF_PACKET_SIZE] =
{
    NOTIF_REQUEST_TYPE, RESPONSE_AVAIL_NOTIF,
    0x00, 0x00,/*wValue*/
    0x00,0x00,/*interface - modifies*/
    0x00,0x00,/* wlength*/
};

#if RNDIS_SUPPORT
    static uint_32 g_list_supp_oid[NUM_OIDS_SUPPORTED] = 
    {
        OID_GEN_SUPPORTED_LIST,             
        OID_GEN_HARDWARE_STATUS,                 
        OID_GEN_MEDIA_SUPPORTED,                 
        OID_GEN_MEDIA_IN_USE,                
        OID_GEN_MAXIMUM_FRAME_SIZE,          
        OID_GEN_LINK_SPEED,                  
        OID_GEN_TRANSMIT_BLOCK_SIZE,
        OID_GEN_RECEIVE_BLOCK_SIZE,          
        OID_GEN_VENDOR_ID,                   
        OID_GEN_VENDOR_DESCRIPTION,
        OID_GEN_CURRENT_PACKET_FILTER,       
        OID_GEN_MAXIMUM_TOTAL_SIZE,          
        OID_GEN_MEDIA_CONNECT_STATUS,       
        OID_GEN_XMIT_OK,                      
        OID_GEN_RCV_OK,                    
        OID_GEN_XMIT_ERROR,               
        OID_GEN_RCV_ERROR,                 
        OID_GEN_RCV_NO_BUFFER,              
        OID_802_3_PERMANENT_ADDRESS,          
        OID_802_3_CURRENT_ADDRESS,         
        OID_802_3_MULTICAST_LIST,        
        OID_802_3_MAXIMUM_LIST_SIZE,          
        OID_802_3_RCV_ERROR_ALIGNMENT,     
        OID_802_3_XMIT_ONE_COLLISION,    
        OID_802_3_XMIT_MORE_COLLISIONS    
    };
    #define VENDOR_INFO_SIZE        (16)
    static uchar g_vendor_info[VENDOR_INFO_SIZE] = " FSL RNDIS PORT ";
#endif
    
 /*****************************************************************************
 * Local Functions
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
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
 *****************************************************************************/
uint_8 USB_Pstn_Init
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_CLASS_CALLBACK_STRUCT_PTR pstn_cb
) 
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr = NULL;
    uint_8 error=USB_OK;   
    /* initialize the Global Variable Structure */
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)USB_mem_alloc_zero(sizeof(CDC_PSTN_STRUCT));
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("cdc_pstn_ptr malloc failed: USB_Pstn_Init\n");
        #endif  
        return USBERR_ALLOC;
    }
    
    USB_mem_zero(cdc_pstn_ptr, sizeof(CDC_PSTN_STRUCT));
    USB_mem_copy(g_serial_state_buf,
        cdc_pstn_ptr->serial_state_buf,(NOTIF_PACKET_SIZE+UART_BITMAP_SIZE));
 
    /* save input parameters */
    cdc_pstn_ptr->pstn_callback.callback = pstn_cb->callback;
    cdc_pstn_ptr->pstn_callback.arg = (void *)pstn_cb->arg;
    
    cdc_obj_ptr->pstn_obj_ptr = (void *)cdc_pstn_ptr;
    
    #if RNDIS_SUPPORT
        /* Initially RNDIS is in Uninitialized state */
        cdc_pstn_ptr->rndis_device_state = RNDIS_UNINITIALIZED;
        cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusNotReady;
        cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateUnknown;
        cdc_pstn_ptr->rndis_dev_max_tx_size = 
            cdc_obj_ptr->rndis_max_frame_size + RNDIS_USB_HEADER_SIZE;
        cdc_pstn_ptr->response_data_ptr = NULL; /* Initializing */
        cdc_pstn_ptr->rndis_command_ptr = NULL;
        
        cdc_pstn_ptr->response_data_ptr = 
            USB_mem_alloc_zero(MAX_EXPECTED_RESPONSE_SIZE);
        if(cdc_pstn_ptr->response_data_ptr == NULL)
        {
            #if _DEBUG
                printf("response_data_ptr malloc failed: USB_Pstn_Init\n");
            #endif  
            return USBERR_ALLOC;
        }
    
        cdc_pstn_ptr->rndis_command_ptr = 
            USB_mem_alloc_zero(MAX_EXPECTED_COMMAND_SIZE);
        if(cdc_pstn_ptr->rndis_command_ptr == NULL)
        {
            #if _DEBUG
                printf("rndis_command_ptr malloc failed: USB_Pstn_Init\n");
            #endif  
            return USBERR_ALLOC;
        }
    #endif
    
    return error;     
}

/**************************************************************************//*!
 *
 * @name  USB_Pstn_Deinit
 *
 * @brief The funtion initializes the Pstn Module 
 *
 * @param cdc_obj_ptr :   Pointer to CDC class object.
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
uint_8 USB_Pstn_Deinit
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr
) 
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr = NULL;
    uint_8 error = USB_OK;   
    cdc_pstn_ptr = cdc_obj_ptr->pstn_obj_ptr;
    /* Free the Global Variable Structure */
   
    #if RNDIS_SUPPORT
    USB_mem_free(cdc_pstn_ptr->response_data_ptr);
    USB_mem_free(cdc_pstn_ptr->rndis_command_ptr);
    #endif
    USB_mem_free(cdc_pstn_ptr);
    return error;     
}
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
uint_8 PSTN_Get_Line_Coding
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{   
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    uint_8 error;
    UNUSED_ARGUMENT (size)
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Get_Line_Coding: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }       
    
    /*  *size is equal to LINE_CODING_SIZE; */
    cdc_pstn_ptr->current_interface =  (uint_8)setup_packet->index ; 
    error = USB_Class_Get_Desc(cdc_obj_ptr->class_handle,GET_LINE_CODING,cdc_pstn_ptr->current_interface,data); 

    return error;
}

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
uint_8 PSTN_Set_Line_Coding
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{   
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    uint_8 error;
    UNUSED_ARGUMENT(data)
    
    *size = 0;

    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Set_Line_Coding: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
       
    /* send the data to the host */
    cdc_pstn_ptr->current_interface =  (uint_8)setup_packet->index ; 
    error = USB_Class_Set_Desc(cdc_obj_ptr->class_handle,SET_LINE_CODING,cdc_pstn_ptr->current_interface,(uint_8_ptr *)&setup_packet); 
   
    return error;
}

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
uint_8 PSTN_Set_Ctrl_Line_State
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    UNUSED_ARGUMENT(data)
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Set_Ctrl_Line_State: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
     
    *size=0;

    cdc_pstn_ptr->dte_status = (uint_8)setup_packet->value ;
    cdc_pstn_ptr->uart_bitmap._byte = 0x00; /* initialize */
    if(cdc_pstn_ptr->pstn_callback.callback != NULL)
    {   
        /* activate/deactivate Tx carrier */   
        cdc_pstn_ptr->uart_bitmap.Bitmap_Uart.bTxCarrier = (cdc_pstn_ptr->dte_status & 
            CARRIER_ACTIVATION_CHECK)?1:0 ;
        /* activate carrier and DTE */ 
        cdc_pstn_ptr->uart_bitmap.Bitmap_Uart.bRxCarrier = (cdc_pstn_ptr->dte_status & 
            DTE_PRESENCE_CHECK)?1:0 ;
           
        /* Indicates to DCE if DTE is present or not */
        cdc_pstn_ptr->dte_present = (cdc_pstn_ptr->dte_status & DTE_PRESENCE_CHECK)?TRUE:FALSE;

        PSTN_Send_Serial_State(cdc_obj_ptr);/* send notification to host*/

        if(cdc_pstn_ptr->dte_status & CARRIER_ACTIVATION_CHECK) 
        {  
            cdc_pstn_ptr->pstn_callback.callback(USB_APP_CDC_CARRIER_ACTIVATED, NULL, cdc_pstn_ptr->pstn_callback.arg);
        } 
        else
        {           
            cdc_pstn_ptr->pstn_callback.callback(USB_APP_CDC_CARRIER_DEACTIVATED, NULL, cdc_pstn_ptr->pstn_callback.arg);
        }                
        if(cdc_pstn_ptr->dte_status & DTE_PRESENCE_CHECK) 
        {  
            cdc_pstn_ptr->pstn_callback.callback(USB_APP_CDC_DTE_ACTIVATED, NULL, cdc_pstn_ptr->pstn_callback.arg);
        } 
        else
        {           
            cdc_pstn_ptr->pstn_callback.callback(USB_APP_CDC_DTE_DEACTIVATED, NULL, cdc_pstn_ptr->pstn_callback.arg);
        }                
    } 
    return USB_OK;
}

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
uint_8 PSTN_Send_Break
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    UNUSED_ARGUMENT (data)
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Send_Break: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }

    *size = 0;        
    cdc_pstn_ptr->break_duration =  setup_packet->value;    
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  PSTN_Get_Comm_Feature
 *
 * @brief  This function is called in response to GetCommFeature request
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
uint_8 PSTN_Get_Comm_Feature
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{   uint_8 error;
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Get_Comm_Feature: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
   
    error = USB_OK;
    *size = cdc_obj_ptr->comm_feature_data_size;
    cdc_pstn_ptr->current_interface =  (uint_8)setup_packet->index ; 
    if(setup_packet->value == ABSTRACT_STATE_FEATURE) 
    {     
    
        error = USB_Class_Get_Desc(cdc_obj_ptr->class_handle,GET_ABSTRACT_STATE,cdc_pstn_ptr->current_interface,data);           
    } 
    else if(setup_packet->value == COUNTRY_SETTING_FEATURE) 
    {
        
       error = USB_Class_Get_Desc(cdc_obj_ptr->class_handle,GET_COUNTRY_SETTING,cdc_pstn_ptr->current_interface,data);           
    } 
    else 
    {
        *size = 0; /* for Reserved/Invalid Feature Selector Value */ 
    }
    return error;
}

/**************************************************************************//*!
 *
 * @name  PSTN_Set_Comm_Feature
 *
 * @brief  This function is called in response to SetCommFeature request
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
uint_8 PSTN_Set_Comm_Feature
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{   uint_8 error;
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Set_Comm_Feature: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
    
    error = USB_OK;
    *size = cdc_obj_ptr->comm_feature_data_size;
    cdc_pstn_ptr->current_interface =  (uint_8)setup_packet->index ;
    if(setup_packet->value == ABSTRACT_STATE_FEATURE) 
    {  
       error = USB_Class_Set_Desc(cdc_obj_ptr->class_handle,SET_ABSTRACT_STATE,cdc_pstn_ptr->current_interface,data);                 
    } 
    else if(setup_packet->value == COUNTRY_SETTING_FEATURE) 
    {
           error = USB_Class_Set_Desc(cdc_obj_ptr->class_handle,SET_COUNTRY_SETTING,cdc_pstn_ptr->current_interface,data);               
    } 
    else 
    {
        *size = 0; /* for Reserved/Invalid Feature Selector Value */ 
    }
    return error;
}

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
void PSTN_Send_Serial_State(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr) 
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    uint_32 val;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
        return;

    /* update array for current interface */
    cdc_pstn_ptr->serial_state_buf[4] = cdc_pstn_ptr->current_interface;
    /* Lower byte of UART BITMAP */
    cdc_pstn_ptr->serial_state_buf[NOTIF_PACKET_SIZE+UART_BITMAP_SIZE-2] = 
        cdc_pstn_ptr->uart_bitmap._byte;
    val = (uint_32)(NOTIF_PACKET_SIZE + UART_BITMAP_SIZE);
    (void)USB_Class_CDC_Send_Data(cdc_obj_ptr->cdc_handle, 
        cdc_obj_ptr->cic_send_endpoint,cdc_pstn_ptr->serial_state_buf,val); 
}

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
void PSTN_Response_Available(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr) 
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Response_Available: cdc_pstn_ptr is NULL\n");
        #endif  
        return;
    }
    /* update array for current interface */
    g_response_available_data[4] = cdc_pstn_ptr->current_interface;
    
    (void)USB_Class_CDC_Send_Data(cdc_obj_ptr->cdc_handle, 
        cdc_obj_ptr->cic_send_endpoint,
        g_response_available_data,NOTIF_PACKET_SIZE); 
}


#if RNDIS_SUPPORT
/**************************************************************************//*!
 *
 * @name  PSTN_Rndis_Message_Set
 *
 * @brief  This function is called in response to PSTN_Rndis_Message_Set 
 *
 * @param handle            handle to Identify the controller
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
uint_8 PSTN_Rndis_Message_Set
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{   
    uint_32 message_type;
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    UNUSED_ARGUMENT(data)
    UNUSED_ARGUMENT(size)
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Rndis_Message_Set: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
    
    cdc_pstn_ptr->current_interface =  (uint_8)setup_packet->index ; 

    if(!(cdc_pstn_ptr->current_interface < 
         cdc_obj_ptr->max_supported_interfaces))
    {       
        #if _DEBUG
            printf("PSTN_Rndis_Message_Set:  %d interface not supported\n", cdc_pstn_ptr->current_interface); 
        #endif  
        return USBERR_INVALID_REQ_TYPE;
    }   

    if(setup_packet->length > MAX_EXPECTED_COMMAND_SIZE) 
    {
        #if _DEBUG
            printf("MAX_EXPECTED_COMMAND_SIZE insufficient, needed %d\n", setup_packet->length);
        #endif  
        return USBERR_ERROR;
    }

    /* Command data has been appended at the end of setup token in framework.c
       Copy data from that buffer to buffer in pstn.c memory for that buffer 
       will be freed on completion of setup transfer*/
    USB_mem_copy(setup_packet+1, cdc_pstn_ptr->rndis_command_ptr,
        setup_packet->length);
    
    message_type = LONG_LE_TO_HOST(*((uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr)));
    
    if(message_type == REMOTE_NDIS_HALT_MSG)
    {   
        /* No response is send to host on receving Halt Command */
        RNDIS_Halt_Command(cdc_obj_ptr);
    }
    else
    {
        PSTN_Response_Available(cdc_obj_ptr);       
    }
    
    if(message_type == REMOTE_NDIS_INITIALIZE_MSG)
    {
        /* Update the NDIS HW status */
        cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusInitializing;
    }
    
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  PSTN_Rndis_Message_Get
 *
 * @brief  This function is called in response to PSTN_Rndis_Message_Get 
 *
 * @param handle            handle to Identify the controller
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
uint_8 PSTN_Rndis_Message_Get
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{       
    uint_32_ptr message_type_ptr;
    uint_16 setup_pkt_len =  setup_packet->length;
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;

    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("PSTN_Rndis_Message_Get: cdc_pstn_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }

    *size = 0;
    
    message_type_ptr = (uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr);
    /* we can avoid one swap operation by using message_type_ptr in 
       PSTN_Rndis_Message_Set instead of message_type, but this gives 
       cleaner implementation as all manipulations and parsing on command 
       are done in this function */
    *message_type_ptr = LONG_LE_TO_HOST(*message_type_ptr);

    switch(*message_type_ptr)
    {
        case REMOTE_NDIS_INITIALIZE_MSG: 
            /* Preparing for response to REMOTE_NDIS_INITIALIZE_MSG command 
               i.e. REMOTE_NDIS_INITIALIZE_CMPLT data */
            RNDIS_Initialize_Command(cdc_obj_ptr,data,size);            
            break;
        case REMOTE_NDIS_QUERY_MSG:
            /* Preparing for response to REMOTE_NDIS_QUERY_MSG command 
               i.e. REMOTE_NDIS_QUERY_CMPLT data */
            /* correct the endianess of OID */
            *((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 3) = LONG_LE_TO_HOST(*((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 3));
            RNDIS_Query_Command(cdc_obj_ptr,data,size);         
            break;
        case REMOTE_NDIS_SET_MSG : 
            /* Preparing for response to REMOTE_NDIS_SET_MSG command 
               i.e. REMOTE_NDIS_SET_CMPLT data */
            /* Correct the endianess of OID and InformationBufferLength
               and InformationBufferOffset*/
            *((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 3) = LONG_LE_TO_HOST(*((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 3));
            *((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 4) = LONG_LE_TO_HOST(*((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 4));
            *((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 5) = LONG_LE_TO_HOST(*((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 5));
            RNDIS_Set_Command(cdc_obj_ptr,data,size);           
            break;
        case REMOTE_NDIS_RESET_MSG :
            /* Preparing for response to REMOTE_NDIS_RESET_MSG command 
               i.e. REMOTE_NDIS_RESET_CMPLT data */
            RNDIS_Reset_Command(cdc_obj_ptr,data,size);         
            break;
        case REMOTE_NDIS_INDICATE_STATUS_MSG : 
            /* Preparing for response to REMOTE_NDIS_INDICATE_STATUS_MSG 
               command */       
            RNDIS_Indicate_Status_Command(cdc_obj_ptr,data,size);           
            break;
        case REMOTE_NDIS_KEEPALIVE_MSG : 
            /* Preparing for response to REMOTE_NDIS_KEEPALIVE_MSG command 
               i.e. REMOTE_NDIS_KEEPALIVE_CMPLT data */
            RNDIS_Keepalive_Command(cdc_obj_ptr,data,size);         
            break;      
        default : 
            #if _DEBUG
                printf("UNSUPPORTED RNDIS Command : 0x%x\n",*message_type_ptr);
            #endif  
            return USBERR_INVALID_REQ_TYPE;         
    }
    
    if(*size > MAX_EXPECTED_RESPONSE_SIZE) 
    {
        #if _DEBUG
            printf("MAX_EXPECTED_RESPONSE_SIZE insufficient\n");
        #endif  
    }
    
    if(setup_pkt_len < *size)
    {
        #if _DEBUG
            printf("Error : Device has more data to send than Host needs.\n");
        #endif  
        return USBERR_INVALID_REQ_TYPE;
    }
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Initialize_Command
 *
 * @brief  This function is called to initialize the RNDIS device and prepare
 *         data to be sent to host as Remote_NDIS_INITIALIZE_CMPLT 
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Initialize_Command
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    uint_32 max_tx_size ;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("RNDIS_Initialize_Command: cdc_pstn_ptr is NULL\n");
        #endif  
        return;
    }

    /* preparing for Byte 0-3 : MessageType*/   
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_INITIALIZE_CMPLT);

    /* preparing for Byte 4-7 : MessageLength*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST_CONST(RESPONSE_RNDIS_INITIALIZE_MSG_SIZE);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint_32));
    
    /* preparing for Byte 12-15 : Status*/
    if(cdc_pstn_ptr->rndis_device_state == RNDIS_UNINITIALIZED)
    {
        *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_SUCCESS); 
    }
    else
    {
        *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_FAILURE);
    }
    
    /* preparing for Byte 16-19 ; MajorVersion*/
    /* We are currently returning the same driver version to host in 
       response to initialization command as reported by host driver */
    USB_mem_copy(((uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr) + 3), 
        ((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 4),sizeof(uint_32));

    /* preparing for Byte 20-23 : MinorVersion*/
    USB_mem_copy(((uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr) + 4), 
        ((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 5),sizeof(uint_32));

    /* preparing for Byte 24-27 : DeviceFlags*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(RNDIS_DF_CONNECTIONLESS);
    
    /* preparing for Byte 28-31 : Medium*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 7) = LONG_LE_TO_HOST(NdisMedium802_3);
    
    /* preparing for Byte 32-35 : MaxPacketsPerTransfer*/
    /* We are not implementing multiple packet transfer support in our RNDIS */
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 8) = LONG_LE_TO_HOST_CONST(RNDIS_SINGLE_PACKET_TRANSFER);
                
    /* preparing for Byte 36-39 : MaxTransferSize*/
    /* We are currently returning the same max transfer size to host
       as it send to device in its corresponding filed in 
       initialization command */
    cdc_pstn_ptr->rndis_host_max_tx_size = LONG_LE_TO_HOST(*((uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 5));
    max_tx_size = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_dev_max_tx_size);
    USB_mem_copy(&max_tx_size, 
        ((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 9),sizeof(uint_32));

    /* preparing for Byte 40-43 : PacketAlignmentFactor*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 10) = LONG_LE_TO_HOST_CONST(PACKET_ALIGNMENT_FACTOR);
    
    /* preparing for Byte 44-47 : PacketAlignmentFactor*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 11) = AF_LIST_OFFSET;

    /* preparing for Byte 48-51 : PacketAlignmentFactor*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 12) = AF_LIST_SIZE;
            
    *data = cdc_pstn_ptr->response_data_ptr;
    *size = RESPONSE_RNDIS_INITIALIZE_MSG_SIZE; 

    /* initializing RNDIS variables */
    cdc_pstn_ptr->rndis_device_state = RNDIS_INITIALIZED;
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusReady;
    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateDisconnected;  
    cdc_pstn_ptr->num_frames_tx_ok = 0;
    cdc_pstn_ptr->num_frames_rx_ok = 0;
    cdc_pstn_ptr->num_frames_tx_error = 0;
    cdc_pstn_ptr->num_frames_rx_error = 0;
    cdc_pstn_ptr->num_recv_frames_missed = 0;
    cdc_pstn_ptr->num_recv_frames_alignment_error = 0;
    cdc_pstn_ptr->num_frames_tx_one_collision = 0;
    cdc_pstn_ptr->num_frames_tx_many_collision = 0;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Keepalive_Command
 *
 * @brief  This function is called to check the health of RNDIS device and prepare
 *         data to be sent to host as REMOTE_NDIS_KEEPALIVE_CMPLT 
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Keepalive_Command
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("RNDIS_Keepalive_Command: cdc_pstn_ptr is NULL\n");
        #endif  
        return;
    }
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_KEEPALIVE_CMPLT);

    /* preparing for Byte 4-7 : MessageLength*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST_CONST(RESPONSE_RNDIS_KEEPALIVE_MSG_SIZE);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint_32));
    
    /* preparing for Byte 12-15 : Status*/
    if(cdc_pstn_ptr->rndis_device_state == RNDIS_DATA_INITIALIZED)
    {
        *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_SUCCESS); 
    }
    else
    {
        *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_FAILURE);
    }
    
    *data = cdc_pstn_ptr->response_data_ptr;
    *size = RESPONSE_RNDIS_KEEPALIVE_MSG_SIZE;      
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Query_Command
 *
 * @brief  This function is called to query the RNDIS device for its 
 *         characteristics or statistics information or status and prepare
 *         data to be sent to host as Remote_NDIS_Query_CMPLT 
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Query_Command
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    uint_32_ptr oid_ptr;
    uint_8 i;
    uint_32 info_buf_len = 0;
    uint_32 return_status = RNDIS_STATUS_SUCCESS;
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("RNDIS_Query_Command: cdc_pstn_ptr is NULL\n");
        #endif  
        return;
    }
    oid_ptr = (uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 3;
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_QUERY_CMPLT);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint_32));
    

    /* preparing for Byte 20-23 : InformationBufferOffset*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 5) = LONG_LE_TO_HOST_CONST(0x00000010);
        
    switch(*oid_ptr)
    {
        case OID_GEN_SUPPORTED_LIST : 
            /* List of supported OIDs - Query Manadatory - General Operational Characteristic */
            for(i=0;i<NUM_OIDS_SUPPORTED;i++)
            {   /* change the endianess of data before sending on USB Bus */
                g_list_supp_oid[i] = LONG_LE_TO_HOST(g_list_supp_oid[i]);             
            }                       
            info_buf_len = sizeof(g_list_supp_oid);         
            USB_mem_copy(g_list_supp_oid, 
                (uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6, info_buf_len);            
            break;
        case OID_GEN_HARDWARE_STATUS :
            /* Hardware status  - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_hw_state);
            break;
        case OID_GEN_MEDIA_SUPPORTED :
            /* Media types supported (encoded) - Query Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(NdisMedium802_3);
            break;
        case OID_GEN_MEDIA_IN_USE :
            /* Media types in use (encoded) - Query Manadatory - General Operational Characteristic*/ 
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(NdisMedium802_3);
            break;
        case OID_GEN_MAXIMUM_FRAME_SIZE : 
            /* Maximum in bytes, frame size - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_obj_ptr->rndis_max_frame_size); 
            break;
        case OID_GEN_LINK_SPEED :
            /* Link speed in units of 100 bps - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            if(cdc_pstn_ptr->pstn_callback.callback != NULL)
            {   
                cdc_pstn_ptr->pstn_callback.callback(USB_APP_GET_LINK_SPEED,
                (void*)((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6),
                cdc_pstn_ptr->pstn_callback.arg);
            } 

            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(*((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6));
            break;
        case OID_GEN_TRANSMIT_BLOCK_SIZE : 
            /* Minimum amount of storage, in bytes, that a single packet 
               occupies in the transmit buffer space of the NIC - 
               Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST((uint_32)cdc_obj_ptr->dic_send_pkt_size);
            break;
        case OID_GEN_RECEIVE_BLOCK_SIZE : 
            /* Amount of storage, in bytes, that a single packet occupies in 
               the receive buffer space of the NIC - Query Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST((uint_32)cdc_obj_ptr->dic_recv_pkt_size);
            break;
        case OID_GEN_VENDOR_ID :
            /* Vendor NIC code - Query Mandatory - General Operational Characteristic*/
            /* This object specifies a three-byte IEEE-registered vendor code, 
               followed by a single byte that the vendor assigns to identify 
               a particular NIC. The IEEE code uniquely identifies the vendor 
               and is the same as the three bytes appearing at the beginning
               of the NIC hardware address.Vendors without an IEEE-registered 
               code should use the value 0xFFFFFF. */
            info_buf_len = sizeof(uint_32);
            
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(
                (uint_32) (((uint_32) RNDIS_VENDOR_ID << 8) | (uint_32) NIC_IDENTIFIER_VENDOR));
            break;
        case OID_GEN_VENDOR_DESCRIPTION :
            /* Vendor network card description - Query Mandatory - General Operational Characteristic*/
            info_buf_len = VENDOR_INFO_SIZE;
            USB_mem_copy(g_vendor_info, (uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6, info_buf_len);            
            break;
        case OID_GEN_CURRENT_PACKET_FILTER : 
            /* Current packet filter (encoded) - Query and Set Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_packet_filter);
            break;
        case OID_GEN_MAXIMUM_TOTAL_SIZE : 
            /* Maximum total packet length in bytes - Query Manadatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_dev_max_tx_size);
            break;
        case OID_GEN_MEDIA_CONNECT_STATUS : 
            /* Whether the NIC is connected to the network - Query Mandatory - General Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->rndis_media_connect_status);
            break;
        case OID_GEN_XMIT_OK : 
            /* Frames transmitted without errors - Query Mandatory - General Statistics*/ 
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_ok);
            break;
        case OID_GEN_RCV_OK : 
            /* Frames received without errors - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_rx_ok);
            break;
        case OID_GEN_XMIT_ERROR : 
            /* Frames not transmitted or transmitted with errors - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_error);
            break;
        case OID_GEN_RCV_ERROR : 
            /* Frames received with errors - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_rx_error);
            break;
        case OID_GEN_RCV_NO_BUFFER : 
            /* Frame missed, no buffers - Query Mandatory - General Statistics*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_recv_frames_missed);
            break;
        case OID_802_3_PERMANENT_ADDRESS : 
            /* Permanent station address - Query Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = RNDIS_ETHER_ADDR_SIZE; 
            USB_mem_copy(cdc_obj_ptr->mac_address, 
                (uint_8_ptr)((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6), 
                info_buf_len);  
            
            break;
        case OID_802_3_CURRENT_ADDRESS : 
            /* Current station address - Query Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = RNDIS_ETHER_ADDR_SIZE; 
            USB_mem_copy(cdc_obj_ptr->mac_address, 
                (uint_8_ptr)((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6), 
                info_buf_len);  
            
            break;
        case OID_802_3_MULTICAST_LIST : 
            /* Current multicast address list - Query and Set Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = RNDIS_ETHER_ADDR_SIZE;
            /* Currently Our RNDIS driver does not support multicast addressing */
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(0x00000000);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 7) = LONG_LE_TO_HOST_CONST(0x00000000);
            break;
        case OID_802_3_MAXIMUM_LIST_SIZE :
            /* Maximum size of multicast address list - Query Mandatory - Ethernet Operational Characteristic*/
            info_buf_len = sizeof(uint_32);
            /* Currently Our RNDIS driver does not support multicast addressing */
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST_CONST(RNDIS_MULTICAST_LIST_SIZE);
            break;
        case OID_802_3_RCV_ERROR_ALIGNMENT : 
            /* Frames received with alignment error - Query Mandatory - Ethernet Statistics*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_recv_frames_alignment_error); 
            break;
        case OID_802_3_XMIT_ONE_COLLISION : 
            /* Frames transmitted with one collision - Query Mandatory - Ethernet Statistics*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_one_collision);
            break;
        case OID_802_3_XMIT_MORE_COLLISIONS : 
            /* Frames transmitted with more than one collision - Query Mandatory - Ethernet Statistics*/
            info_buf_len = sizeof(uint_32);
            *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 6) = LONG_LE_TO_HOST(cdc_pstn_ptr->num_frames_tx_many_collision);
            break;
        default : 
            #if _DEBUG
                printf("OID 0x%x NOT SUPPORTED(QUERY)\n",*oid_ptr);
            #endif  
            return_status = RNDIS_STATUS_NOT_SUPPORTED;
            break;
    }

    /* preparing for Byte 12-15 : Status*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST(return_status);
    
    *size = RESPONSE_RNDIS_QUERY_MSG_SIZE + info_buf_len;       
    /* preparing for Byte 4-7 : MessageLength*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST(*size);
    /* preparing for Byte 16-19 : InformationBufferLength*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 4) = LONG_LE_TO_HOST(info_buf_len);
    *data = cdc_pstn_ptr->response_data_ptr;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Set_Command
 *
 * @brief  This function is called to Configure the operational parameters of
 *         RNDIS device.
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Set_Command
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    uint_32_ptr oid_ptr;
    uint_32_ptr info_buf_len_ptr;
    uint_32_ptr info_buf_offset_ptr;
    uint_32 return_status = RNDIS_STATUS_SUCCESS;
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("RNDIS_Set_Command: cdc_pstn_ptr is NULL\n");
        #endif  
        return;
    }
    oid_ptr = (uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 3;
    info_buf_len_ptr = (uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 4;
    info_buf_offset_ptr = (uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 5;
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_SET_CMPLT);

    *size = RESPONSE_RNDIS_SET_MSG_SIZE;        
    /* preparing for Byte 4-7 : MessageLength*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST(*size);

    /* preparing for Byte 8-11 : RequestID*/
    USB_mem_copy(((uint_32_ptr)(cdc_pstn_ptr->rndis_command_ptr) + 2), 
        ((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 2),sizeof(uint_32));
    
    switch(*oid_ptr)
    {
        case OID_GEN_CURRENT_PACKET_FILTER : 
            /* Current packet filter (encoded) - Query and Set Manadatory - General Operational Characteristic*/
            if(*info_buf_len_ptr != 4 ) 
            {
                return_status = RNDIS_STATUS_INVALID_DATA;
            }
            else
            {   
                boolean media_connected = FALSE;

                cdc_pstn_ptr->rndis_packet_filter = LONG_LE_TO_HOST( *((uint_32_ptr)((uint_8_ptr)((uint_32_ptr)
                    cdc_pstn_ptr->rndis_command_ptr + 2) + *info_buf_offset_ptr)));
                if(cdc_pstn_ptr->pstn_callback.callback != NULL)
                {   
                    cdc_pstn_ptr->pstn_callback.callback(USB_APP_GET_LINK_STATUS,
                    (void*)(&media_connected),
                    cdc_pstn_ptr->pstn_callback.arg);
                } 

                if((cdc_pstn_ptr->rndis_packet_filter)&&(media_connected == TRUE))
                {
                    cdc_pstn_ptr->rndis_device_state = RNDIS_DATA_INITIALIZED;  
                    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateConnected;
                }
                else
                {
                    cdc_pstn_ptr->rndis_device_state = RNDIS_INITIALIZED;   
                    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateDisconnected;
                }
            }
            break;
        case OID_802_3_MULTICAST_LIST : 
            /* Current multicast address list - Query and Set Mandatory - Ethernet Operational Characteristic*/
            if(*info_buf_len_ptr != RNDIS_ETHER_ADDR_SIZE) 
            {
                return_status = RNDIS_STATUS_INVALID_DATA;
            }
            else
            {   uint_64 multi_cast_list;
                multi_cast_list = *((uint_64_ptr)((uint_8_ptr)((uint_32_ptr)
                    cdc_pstn_ptr->rndis_command_ptr + 2) + *info_buf_offset_ptr));
                if(multi_cast_list)
                {/* Currently Our RNDIS driver does not support multicast addressing */
                    return_status = RNDIS_STATUS_NOT_SUPPORTED;
                }
            }

            break;
        default : 
            #if _DEBUG
                printf("OID 0x%x NOT SUPPORTED(SET)\n",*oid_ptr);
            #endif  
            return_status = RNDIS_STATUS_NOT_SUPPORTED;
            break;
    }
    /* preparing for Byte 12-15 : Status*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST(return_status);
    *data = cdc_pstn_ptr->response_data_ptr;        
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Reset_Command
 *
 * @brief  This function is called to soft reset the RNDIS device.
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Reset_Command
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    uint_8_ptr *data, 
    uint_32 *size
)
{
//    uint_32_ptr oid_ptr;
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;    
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("RNDIS_Reset_Command: cdc_pstn_ptr is NULL\n");
        #endif  
        return;
    }

//    oid_ptr = (uint_32_ptr)cdc_pstn_ptr->rndis_command_ptr + 3;
    
    /* preparing for Byte 0-3 : MessageType*/   
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr) = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_RESET_CMPLT);

    *size = RESPONSE_RNDIS_RESET_MSG_SIZE;      
    /* preparing for Byte 4-7 : MessageLength*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 1) = LONG_LE_TO_HOST(*size);
    
    /* preparing for Byte 8-11 : Status*/
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 2) = LONG_LE_TO_HOST_CONST(RNDIS_STATUS_SUCCESS);

    /* preparing for Byte 12-15 : AddressingReset*/
    /* No need for host to resend addressing information */
    *((uint_32_ptr)cdc_pstn_ptr->response_data_ptr + 3) = LONG_LE_TO_HOST_CONST(0x00000000);

    *data = cdc_pstn_ptr->response_data_ptr;
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusReset;
    cdc_pstn_ptr->rndis_device_state = RNDIS_UNINITIALIZED;
    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateUnknown;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Indicate_Status_Command
 *
 * @brief  Used to indicate change in status of device
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Indicate_Status_Command 
(
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    UNUSED_ARGUMENT(cdc_obj_ptr)
    UNUSED_ARGUMENT(data)
    UNUSED_ARGUMENT(size)   
    return;
}

/**************************************************************************//*!
 *
 * @name  RNDIS_Halt_Command
 *
 * @brief  This function is called to halt the RNDIS device.
 *         i.e. to terminate the network connection
 *
 * @param handle   handle to Identify the controller
 * @param data:    data to be send back
 * @param size:    size to be returned 
 *
 * @return NONE
 *
 *****************************************************************************/ 
void RNDIS_Halt_Command(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr)
{
    CDC_PSTN_STRUCT_PTR cdc_pstn_ptr;
    
    cdc_pstn_ptr = (CDC_PSTN_STRUCT_PTR)cdc_obj_ptr->pstn_obj_ptr;
    if (NULL == cdc_pstn_ptr)
    {
        #if _DEBUG
            printf("RNDIS_Halt_Command: cdc_pstn_ptr is NULL\n");
        #endif  
        return;
    }
    #if _DEBUG
        printf("RNDIS_Halt_Command\n");
    #endif
    cdc_pstn_ptr->rndis_device_state = RNDIS_UNINITIALIZED;
    cdc_pstn_ptr->rndis_media_connect_status = NdisMediaStateDisconnected;  
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusClosing;
    cdc_pstn_ptr->rndis_hw_state = NdisHardwareStatusNotReady;
}
#endif/* rndis support */

#endif /*CDC_CONFIG*/
/* EOF */
