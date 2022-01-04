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
* $FileName: usb_cdc.c$
* $Version : 3.8.8.0$
* $Date    : Jul-2-2012$
*
*
* @brief The file contains CDC layer implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_stack_config.h"


#if CDC_CONFIG 
#include "usb_cdc.h"
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
CDC_DEVICE_STRUCT_PTR   cdc_device_array[MAX_CDC_DEVICE];

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
uint_8 USB_CDC_Other_Requests(USB_SETUP_STRUCT * setup_packet, 
                          uint_8_ptr *data, 
                          uint_32 *size,pointer arg);
uint_8 USB_Map_Ep_To_Struct_Index(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr, uint_8 ep_num);  
static CDC_HANDLE  USB_Cdc_Allocate_Handle(void);
static int_32 USB_Cdc_Free_Handle(CDC_HANDLE handle);
static CDC_DEVICE_STRUCT_PTR USB_Cdc_Get_Device_Ptr(CDC_HANDLE handle);
/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/
  /*************************************************************************//*!
 *
 * @name  USB_Cdc_Allocate_Handle
 *
 * @brief The funtion reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static CDC_HANDLE  USB_Cdc_Allocate_Handle(void)
{
    uint_32 cnt = 0;
    for (;cnt< MAX_CDC_DEVICE;cnt++)
    {
       if (cdc_device_array[cnt] == NULL)
        return cnt;
    }
    return USBERR_DEVICE_BUSY;
}
 /*************************************************************************//*!
 *
 * @name  USB_Cdc_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/

static int_32 USB_Cdc_Free_Handle(CDC_HANDLE handle)
{
    if (/*(handle < 0) || */ (handle > MAX_CDC_DEVICE))
        return USBERR_ERROR;
    
    USB_mem_free((void *)cdc_device_array[handle]);
    cdc_device_array[handle] = NULL;
    return USB_OK;
}
 /*************************************************************************//*!
 *
 * @name  USB_Cdc_Get_Device_Ptr
 *
 * @brief The funtion gets the device pointer from device array .
 *
 * @param handle  index in device array.
 * @return returns returns pointer to HID device structure..      
 *
 *****************************************************************************/
static CDC_DEVICE_STRUCT_PTR USB_Cdc_Get_Device_Ptr(CDC_HANDLE handle)
{
    if (/*(handle < 0) || */ (handle > MAX_CDC_DEVICE))
        return NULL;
    
     return cdc_device_array[handle]; 
}
 /*****************************************************************************
 * Local Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Map_Ep_To_Struct_Index
 *
 * @brief The funtion maps the endpoint num to the index of the ep data 
 *           structure
 *
 * @param handle          handle to identify the controller
 * @param ep_num          endpoint num
 *
 * @return index          mapped index       
 *
 *****************************************************************************/
uint_8 USB_Map_Ep_To_Struct_Index(CDC_DEVICE_STRUCT_PTR cdc_obj_ptr,uint_8 ep_num) 
{
    uint_8 index = 0;
     
    USB_ENDPOINTS *ep_desc_data = cdc_obj_ptr->usb_ep_data;
    
    /* map the endpoint num to the index of the endpoint structure */
    for(index = 0; index < ep_desc_data->count; index++) 
    {
        if(ep_desc_data->ep[index].ep_num == ep_num)
            break;
    }    
    return index;     
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Cdc_Notif
 *
 * @brief The funtion ic callback function of CIC Notification endpoint 
 *
  * @param event
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Cdc_Notif(PTR_USB_EVENT_STRUCT event,pointer arg)
{    
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr = (CDC_DEVICE_STRUCT_PTR)arg;
    #if IMPLEMENT_QUEUING
        uint_8 index;
        uint_8 producer, consumer;
//        USB_ENDPOINTS *usb_ep_data = cdc_obj_ptr->usb_ep_data;
        
        USB_CLASS_CDC_QUEUE queue;
    #endif
    
    UNUSED_ARGUMENT (event)
    
    #if IMPLEMENT_QUEUING
        /* map the endpoint num to the index of the endpoint structure */
        index = USB_Map_Ep_To_Struct_Index(cdc_obj_ptr, event->ep_num); 

        producer = cdc_obj_ptr->ep[index].bin_producer;
            
        /* if there are no errors de-queue the queue and decrement the no. of 
           transfers left, else send the same data again */
        cdc_obj_ptr->ep[index].bin_consumer++;              
        consumer = cdc_obj_ptr->ep[index].bin_consumer;
            
        if(consumer != producer) 
        {/*if bin is not empty */                           
            queue = cdc_obj_ptr->ep[index].queue[consumer%CDC_MAX_QUEUE_ELEMS];                         
            (void)USB_Class_Send_Data(cdc_obj_ptr->class_handle, queue.channel, 
                queue.app_data.data_ptr, queue.app_data.data_size);
        }        
    #endif
    
    if(cdc_obj_ptr->cdc_class_cb.callback != NULL) 
    {
        uint_8 event_type = USB_APP_SEND_COMPLETE;
        cdc_obj_ptr->cdc_class_cb.callback(event_type,NULL,
        cdc_obj_ptr->cdc_class_cb.arg);
    }   
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Dic_Bulk_In
 *
 * @brief The funtion ic callback function of DIC Bulk In Endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Dic_Bulk_In(PTR_USB_EVENT_STRUCT event,pointer arg)
{
    APP_DATA_STRUCT bulk_in_recv;
    uint_8 event_type;
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr = (CDC_DEVICE_STRUCT_PTR)arg;
    
    #if IMPLEMENT_QUEUING
        uint_8 index;
        uint_8 producer, consumer;
//        USB_ENDPOINTS *usb_ep_data = cdc_obj_ptr->usb_ep_data;
        
        USB_CLASS_CDC_QUEUE queue;

        /* map the endpoint num to the index of the endpoint structure */
        index = USB_Map_Ep_To_Struct_Index(cdc_obj_ptr, event->ep_num); 
        producer = cdc_obj_ptr->ep[index].bin_producer;         
        /* if there are no errors de-queue the queue and decrement the no. of 
           transfers left, else send the same data again */
        cdc_obj_ptr->ep[index].bin_consumer++;                  
        consumer = cdc_obj_ptr->ep[index].bin_consumer;
            
        if(consumer != producer) 
        {/*if bin is not empty */
                            
            queue = cdc_obj_ptr->ep[index].queue[consumer%CDC_MAX_QUEUE_ELEMS];
                            
            (void)USB_Class_Send_Data(cdc_obj_ptr->class_handle, queue.channel, 
                queue.app_data.data_ptr, queue.app_data.data_size);
        }          
    #endif
    
    if(cdc_obj_ptr->param_callback.callback != NULL) 
    {
        event_type = USB_APP_SEND_COMPLETE;
        bulk_in_recv.data_ptr = event->buffer_ptr;
        bulk_in_recv.data_size = event->len;
        cdc_obj_ptr->param_callback.callback(event_type,
                (void*)&bulk_in_recv,cdc_obj_ptr->param_callback.arg);             
    }
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Dic_Bulk_Out
 *
 * @brief The funtion ic callback function of DIC Bulk Out Endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Dic_Bulk_Out(PTR_USB_EVENT_STRUCT event,pointer arg)
{   
    APP_DATA_STRUCT bulk_out_recv;
    uint_8 event_type; 
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr = (CDC_DEVICE_STRUCT_PTR)arg;
      
    event_type = USB_APP_DATA_RECEIVED; 
    bulk_out_recv.data_ptr = event->buffer_ptr;
    bulk_out_recv.data_size = event->len; 
  
    if(cdc_obj_ptr->param_callback.callback != NULL) 
    {
      cdc_obj_ptr->param_callback.callback(event_type,
      (void*)&bulk_out_recv,cdc_obj_ptr->param_callback.arg);             
    }
 }

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Event
 *
 * @brief The funtion initializes CDC endpoints 
 *
 * @param handle   handle to Identify the controller
 * @param event           pointer to event structure
 * @param val             gives the configuration value 
 *
 * @return None       
 *
 *****************************************************************************/
 void USB_Class_CDC_Event(uint_8 event, void* val,pointer arg) 
{  
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr = (CDC_DEVICE_STRUCT_PTR)arg;
    
    if(event == USB_APP_ENUM_COMPLETE) 
    {
        uint_8 count = 0;
        USB_EP_STRUCT_PTR ep_struct_ptr;
        
        /* get the endpoints from the descriptor module */            
        USB_ENDPOINTS *usb_ep_data = cdc_obj_ptr->usb_ep_data;
        
        /* intialize all non control endpoints */            
        while(count < usb_ep_data->count) 
        {
            ep_struct_ptr = (USB_EP_STRUCT*) &usb_ep_data->ep[count];

            (void)_usb_device_init_endpoint(cdc_obj_ptr->controller_handle,
             ep_struct_ptr,TRUE);
  
            /* register callback service for Non Control EndPoints */
            switch(ep_struct_ptr->type) 
            {
                case USB_INTERRUPT_PIPE :
                    (void)_usb_device_register_service(cdc_obj_ptr->controller_handle,
                        (uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                        USB_Service_Cdc_Notif,(void *)cdc_obj_ptr);
                    break;                              
                case USB_BULK_PIPE :
                    if(ep_struct_ptr->direction == USB_RECV) 
                    {
                        (void)_usb_device_register_service(cdc_obj_ptr->controller_handle,
                            (uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                            USB_Service_Dic_Bulk_Out,(void *)cdc_obj_ptr);
                    } 
                    else
                    {
                        (void)_usb_device_register_service(cdc_obj_ptr->controller_handle,
                            (uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                            USB_Service_Dic_Bulk_In,(void *)cdc_obj_ptr);
                    }
                    break;
                default : break;        
            }
                          
            /* set the EndPoint Status as Idle in the device layer */
            /* (no need to specify direction for this case) */
            (void)_usb_device_set_status(cdc_obj_ptr->controller_handle,
                (uint_8)(USB_STATUS_ENDPOINT|ep_struct_ptr->ep_num),
                (uint_8)USB_STATUS_IDLE);                                                                                                         
            count++;                                                    
        }/* EndWhile */
    }
    #if RNDIS_SUPPORT
    else if(event == USB_APP_BUS_RESET)
    {
        uint_8_ptr data; 
        uint_32 size;
        RNDIS_Reset_Command(cdc_obj_ptr, &data, &size);     
    }
    #endif        
    if(cdc_obj_ptr->cdc_class_cb.callback != NULL) 
    {
        cdc_obj_ptr->cdc_class_cb.callback(event,
            val,cdc_obj_ptr->cdc_class_cb.arg);
    } 
}

/**************************************************************************//*!
 *
 * @name  USB_CDC_Other_Requests
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param handle
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_CDC_Other_Requests
(   
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size,
    pointer arg
) 
{
    uint_8 status;
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr = (CDC_DEVICE_STRUCT_PTR)arg;
    status = USBERR_INVALID_REQ_TYPE;

    if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_CLASS) 
    {  /* class request so handle it here */     
        status=USB_OK;
        /* call for class/subclass specific requests */
        switch(setup_packet->request) 
        {
            case SEND_ENCAPSULATED_COMMAND :
                #if RNDIS_SUPPORT               
                    /* Pass the Remote NDIS Control Message supoported by 
                       802.3 connectionless device to PSTN Layer */
                    status = PSTN_Rndis_Message_Set(cdc_obj_ptr,setup_packet,data,size); 
                #endif
                *size = 0;              
                break;
            case GET_ENCAPSULATED_RESPONSE :
                #if RNDIS_SUPPORT               
                    /* Get the Remote NDIS Control Message supoported by 
                       802.3 connectionless device from  PSTN Layer */
                    status = PSTN_Rndis_Message_Get(cdc_obj_ptr,setup_packet,data,size); 
                #else
                    *size = 0;
                    /* protocol says to return zero byte data instead of stalling 
                       the command if you don't have data to return */
                    status = USB_OK;                
                #endif
                break; 
            case SET_COMM_FEATURE :
                status = PSTN_Set_Comm_Feature(cdc_obj_ptr,setup_packet,data,size);  
                break;
            case GET_COMM_FEATURE :
                status = PSTN_Get_Comm_Feature(cdc_obj_ptr,setup_packet,data,size); 
                break;
            case CLEAR_COMM_FEATURE : /* Verify this implementation */
                *size = cdc_obj_ptr->comm_feature_data_size;
                **data = 0x00; *(++(*data)) = 0x00;/*clear both feature bytes*/
                status = PSTN_Set_Comm_Feature(cdc_obj_ptr,
                setup_packet,data,size);  
                break; 
            case GET_LINE_CODING :               
                status = PSTN_Get_Line_Coding(cdc_obj_ptr,
                setup_packet,data,size); 
                break;
            case SET_LINE_CODING :               
                status = PSTN_Set_Line_Coding(cdc_obj_ptr,setup_packet,data,size); 
                break;    
            case SET_CONTROL_LINE_STATE : 
                status = PSTN_Set_Ctrl_Line_State(cdc_obj_ptr,
                setup_packet,data,size); 
                break;
            case SEND_BREAK : 
                status = PSTN_Send_Break(cdc_obj_ptr,setup_packet,data,size);
                break;
            default :  *size=0;break;        
        }
    } 
    else if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_VENDOR) 
    {   /* vendor specific request  */    
        if(cdc_obj_ptr->vendor_req_callback.callback != NULL) 
        {
            status = cdc_obj_ptr->vendor_req_callback.callback(setup_packet,
            data,size,cdc_obj_ptr->vendor_req_callback.arg);
        }
    }
        
    return status;
}


/*****************************************************************************
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
uint_32 USB_Class_CDC_Init
(
  CDC_CONFIG_STRUCT_PTR cdc_config_ptr
)
{   
#if IMPLEMENT_QUEUING
    uint_8 index;
#endif

    uint_8 error;
    CDC_HANDLE cdc_handle;
    CDC_DEVICE_STRUCT_PTR devicePtr = NULL;
    USB_ENDPOINTS *usb_ep_data;
    
    if (NULL == (void *)cdc_config_ptr)
        return USBERR_ERROR;
    
    devicePtr = (CDC_DEVICE_STRUCT_PTR)USB_mem_alloc_zero(sizeof(CDC_DEVICE_STRUCT));
    if (NULL == devicePtr) 
    {
        #if _DEBUG
            printf("memalloc failed in USB_Class_CDC_Init\n");
        #endif  
        return USBERR_ALLOC;    
    }
    
    cdc_handle = USB_Cdc_Allocate_Handle();
    if (USBERR_DEVICE_BUSY == cdc_handle) 
    {
        USB_mem_free(devicePtr);
        devicePtr = NULL;
        return USBERR_INIT_FAILED;
    }

        /* initialize the Global Variable Structure */
    USB_mem_zero(devicePtr, sizeof(CDC_DEVICE_STRUCT));

    devicePtr->ep = cdc_config_ptr->ep;
    usb_ep_data = cdc_config_ptr->usb_ep_data;
    devicePtr->max_supported_interfaces = 
        cdc_config_ptr->max_supported_interfaces;
    #if RNDIS_SUPPORT
        USB_mem_copy(cdc_config_ptr->mac_address, devicePtr->mac_address, 
            sizeof(_enet_address));
        devicePtr->ip_address = cdc_config_ptr->ip_address;
        devicePtr->rndis_max_frame_size = cdc_config_ptr->rndis_max_frame_size;
    #endif
    /* Initialize the device layer*/
    error = _usb_device_init(USBCFG_DEFAULT_DEVICE_CONTROLLER, &devicePtr->controller_handle, (uint_8)(usb_ep_data->count+1));
    /* +1 is for Control Endpoint */ 
    if(error != USB_OK)
    {
      goto error1;  
    }
    
    cdc_config_ptr->desc_callback_ptr->handle = cdc_handle;
    
    /* Initialize the generic class functions */
    devicePtr->class_handle = USB_Class_Init(devicePtr->controller_handle,
    USB_Class_CDC_Event,USB_CDC_Other_Requests,(void *)devicePtr,
    cdc_config_ptr->desc_callback_ptr);

    /* Initialize the generic class functions */
    #if PSTN_SUBCLASS_NOTIF_SUPPORT
    /* Initialize the pstn subclass functions */
    error = USB_Pstn_Init(devicePtr,&cdc_config_ptr->param_callback);
    if(error != USB_OK)
    {
      goto error2;  
    }
    #endif
     
    devicePtr->usb_ep_data = usb_ep_data;
    #if IMPLEMENT_QUEUING
     for(index = 0; index < usb_ep_data->count; index++) 
     {            
        devicePtr->ep[index].endpoint = usb_ep_data->ep[index].ep_num;
        devicePtr->ep[index].type = usb_ep_data->ep[index].type;        
        devicePtr->ep[index].bin_consumer = 0x00;         
        devicePtr->ep[index].bin_producer = 0x00;        
      }
    #endif
        
    /* save the callback pointer */
    devicePtr->cdc_class_cb.callback = cdc_config_ptr->cdc_class_cb.callback;
    devicePtr->cdc_class_cb.arg = cdc_config_ptr->cdc_class_cb.arg;         
    /* save the callback pointer */
    devicePtr->vendor_req_callback.callback = 
    cdc_config_ptr->vendor_req_callback.callback;
    devicePtr->vendor_req_callback.arg = cdc_config_ptr->vendor_req_callback.arg; 
    /* save the callback pointer */
    devicePtr->param_callback.callback = cdc_config_ptr->param_callback.callback;
    devicePtr->param_callback.arg = cdc_config_ptr->param_callback.arg; 
    devicePtr->comm_feature_data_size =  cdc_config_ptr->comm_feature_data_size;  
    devicePtr->cic_send_endpoint = cdc_config_ptr->cic_send_endpoint;
    devicePtr->cic_recv_endpoint = USB_CONTROL_ENDPOINT;
    devicePtr->dic_send_endpoint = cdc_config_ptr->dic_send_endpoint;
    devicePtr->dic_recv_endpoint = cdc_config_ptr->dic_recv_endpoint; 
    devicePtr->dic_recv_pkt_size = cdc_config_ptr->dic_recv_pkt_size;
    devicePtr->dic_send_pkt_size = cdc_config_ptr->dic_send_pkt_size;
    devicePtr->cic_send_pkt_size = cdc_config_ptr->cic_send_pkt_size;
     
      
    devicePtr->cdc_handle = cdc_handle;
   
   cdc_device_array[cdc_handle] = devicePtr;
   
   return cdc_handle;
 error2:
  /* Implement Denit class and invoke here*/    
 error1: 
     USB_Cdc_Free_Handle(cdc_handle);
     USB_mem_free(devicePtr);
     devicePtr = NULL;    
    return error;    
}

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Deinit
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param cdc_handle
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the CDC Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
uint_32 USB_Class_CDC_Deinit
(
  CDC_HANDLE cdc_handle
)
{   

    uint_8 error = USB_OK;
    CDC_DEVICE_STRUCT_PTR devicePtr = NULL;
    devicePtr = USB_Cdc_Get_Device_Ptr(cdc_handle);

    #if PSTN_SUBCLASS_NOTIF_SUPPORT
    /* deinitialize the pstn subclass functions */
    error = USB_Pstn_Deinit(devicePtr);
    #endif
    if(error == USB_OK)
    /* deinitialize the generic class functions */
    error = USB_Class_Deinit(devicePtr->controller_handle,devicePtr->class_handle);
    if(error == USB_OK)
    /* deinitialize the device layer*/
    error = _usb_device_deinit(devicePtr->controller_handle);
    if(error == USB_OK)
    error = USB_Cdc_Free_Handle(cdc_handle);
    return error;    
}
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Send_Data
 *
 * @brief 
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
uint_8 USB_Class_CDC_Send_Data
(
    CDC_HANDLE handle,
    uint_8 ep_num,
    uint_8_ptr app_buff,
    uint_32 size
) 
{
    #if IMPLEMENT_QUEUING
        uint_8 index;
        uint_8 producer, consumer; 
//        USB_ENDPOINTS *usb_ep_data; 
    #endif    
        
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr;
    uint_8 status = USB_OK;

    cdc_obj_ptr = USB_Cdc_Get_Device_Ptr(handle);
    if (NULL == cdc_obj_ptr)
      return USBERR_ERROR;
    
    #if IMPLEMENT_QUEUING
//        usb_ep_data = cdc_obj_ptr->usb_ep_data;
      
        /* map the endpoint num to the index of the endpoint structure */
        index = USB_Map_Ep_To_Struct_Index(cdc_obj_ptr, ep_num); 
                
        producer = cdc_obj_ptr->ep[index].bin_producer;
        consumer = cdc_obj_ptr->ep[index].bin_consumer;
                                
        if(((uint_8)(producer - consumer)) != (uint_8)(CDC_MAX_QUEUE_ELEMS))  
        {   /* the bin is not full*/        
            uint_8 queue_num = (uint_8)(producer % CDC_MAX_QUEUE_ELEMS);
            
            /* put all send request parameters in the endpoint data structure*/
            cdc_obj_ptr->ep[index].queue[queue_num].channel = ep_num;
            cdc_obj_ptr->ep[index].queue[queue_num].app_data.data_ptr = app_buff;
            cdc_obj_ptr->ep[index].queue[queue_num].app_data.data_size = size; 
            cdc_obj_ptr->ep[index].queue[queue_num].handle = 
            cdc_obj_ptr->controller_handle;         
            /* increment producer bin by 1*/       
            cdc_obj_ptr->ep[index].bin_producer = ++producer;

            if((uint_8)(producer - consumer) == (uint_8)1)         
            {          
    #endif      
                status = USB_Class_Send_Data(cdc_obj_ptr->class_handle,
                 ep_num, app_buff,size);
    #if IMPLEMENT_QUEUING
            }
        }
        else /* bin is full */
        {
            status = USBERR_DEVICE_BUSY; 
        }    
    #endif 
    return status;
}
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
uint_8 USB_Class_CDC_Recv_Data
(
    CDC_HANDLE          cdc_handle,
    uint_8              ep_num,
    uint_8_ptr          buff_ptr,      /* [IN] buffer to recv */      
    uint_32             size           /* [IN] length of the transfer */
)
{
    CDC_DEVICE_STRUCT_PTR cdc_obj_ptr;
    uint_8 status = USB_OK;

    cdc_obj_ptr = USB_Cdc_Get_Device_Ptr(cdc_handle);
    if (NULL == cdc_obj_ptr)
    {
        return USBERR_ERROR;    
    }
   
    status = _usb_device_recv_data(cdc_obj_ptr->controller_handle,
    ep_num,buff_ptr,size);
    
    return status;
    
}

#endif /*CDC_CONFIG*/
/* EOF */
