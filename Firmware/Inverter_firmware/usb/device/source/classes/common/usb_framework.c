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
* $FileName: usb_framework.c$
* $Version : 3.8.6.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
* @brief The file contains USB stack framework module implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"
#include "usb_framework.h"
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
#define MAX_EXPECTED_CONTROL_OUT_SIZE       (76 + USB_SETUP_PKT_SIZE) 

/****************************************************************************
 * Global Variables
 ****************************************************************************/
 USB_CLASS_FW_OBJECT_STRUCT_PTR  usb_class_fw_object[USB_MAX_CLASS_FW_OBJECT];
 
#ifdef DELAYED_PROCESSING
    USB_EVENT_STRUCT g_f_event;
    boolean g_control_pending=FALSE;
#endif

boolean g_validate_request[MAX_STRD_REQ][3] = 
{ 
  TRUE,TRUE,FALSE, /*USB_Strd_Req_Get_Status*/
         /* configured state: valid for existing interfaces/endpoints
            address state   : valid only for interface or endpoint 0
            default state   : not specified
         */ 
  TRUE,TRUE,FALSE, /* Clear Feature */
         /* configured state: valid only for device in configured state
            address state   : valid only for device (in address state), 
                              interface and endpoint 0 
            default state   : not specified
         */
  FALSE,FALSE,FALSE, /*reserved for future use*/
         /* configured state: request not supported
            address state   : request not supported
            default state   : request not supported
         */ 
  TRUE,TRUE,FALSE, /* Set Feature */
        /*  configured state: valid only for device in configured state
            address state   : valid only for interface or endpoint 0
            default state   : not specified
        */ 
  FALSE,FALSE,FALSE,/*reserved for future use*/
        /*  configured state: request not supported            
            address state   : request not supported
            default state   : request not supported
         */ 
  FALSE,TRUE,TRUE, /*USB_Strd_Req_Set_Address*/
        /*  configured state: not specified            
            address state   : changes to default state if specified addr == 0,
                              but uses newly specified address
            default state   : changes to address state if specified addr != 0
         */
  TRUE,TRUE,TRUE, /*USB_Strd_Req_Get_Descriptor*/
         /* configured state: valid request            
            address state   : valid request
            default state   : valid request
         */
  FALSE,FALSE,FALSE, /*Set Descriptor*/
         /* configured state: request not supported
            address state   : request not supported
            default state   : request not supported
         */
  TRUE,TRUE,FALSE, /*USB_Strd_Req_Get_Config*/
         /* configured state: bConfiguration Value of current config returned
            address state   : value zero must be returned
            default state   : not specified            
         */
  TRUE,TRUE,FALSE, /*USB_Strd_Req_Set_Config*/
         /* configured state: If the specified configuration value is zero, 
                              then the device enters the Address state.If the 
                              specified configuration value matches the 
                              configuration value from a config descriptor, 
                              then that config is selected and the device
                              remains in the Configured state. Otherwise, the 
                              device responds with a Request Error.
         
            address state   : If the specified configuration value is zero, 
                              then the device remains in the Address state. If 
                              the specified configuration value matches the 
                              configuration value from a configuration 
                              descriptor, then that configuration is selected 
                              and the device enters the Configured state. 
                              Otherwise,response is Request Error.
            default state   : not specified
         */
  TRUE,FALSE,FALSE, /*USB_Strd_Req_Get_Interface*/
         /* configured state: valid request          
            address state   : request error
            default state   : not specified
         */
  TRUE,FALSE,FALSE, /*USB_Strd_Req_Set_Interface*/
         /* configured state: valid request
            address state   : request error
            default state   : not specified
         */
  TRUE,FALSE,FALSE /*USB_Strd_Req_Sync_Frame*/
         /* configured state: valid request
            address state   : request error
            default state   : not specified
         */ 
};
            
/****************************************************************************
 * Global Functions
 ****************************************************************************/

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
USB_CLASS_FW_HANDLE USB_Framework_Init( _usb_device_handle handle, /*[IN]*/
										USB_CLASS_CALLBACK class_callback,/*[IN]*/
										USB_REQ_FUNC other_req_callback,/*[IN]*/
										pointer callback_data,/*[IN]*/
										int_32 data,/*[IN]*/
										DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr /*[IN]*/);
uint_8 USB_Framework_Deinit( _usb_device_handle handle, /*[IN]*/
						   USB_CLASS_FW_HANDLE fw_handle /*[IN]*/); 
uint_8 USB_Framework_Reset(_usb_device_handle handle);
void USB_Control_Service (PTR_USB_EVENT_STRUCT event,pointer arg );
void USB_Control_Service_Handler( USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                                  uint_8 error,
                                  USB_SETUP_STRUCT * setup_packet, 
                                  uint_8_ptr *data, 
                                  uint_32 *size);
                              
uint_8 USB_Strd_Req_Get_Status(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                               USB_SETUP_STRUCT * setup_packet,
                               uint_8_ptr *data, 
                               uint_32 *size);
uint_8 USB_Strd_Req_Feature(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                            USB_SETUP_STRUCT * setup_packet, 
                            uint_8_ptr *data, 
                            uint_32 *size);
uint_8 USB_Strd_Req_Set_Address(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                                USB_SETUP_STRUCT * setup_packet,
                                uint_8_ptr *data, 
                                uint_32 *size);
uint_8 USB_Strd_Req_Assign_Address(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr);
uint_8 USB_Strd_Req_Get_Config(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                               USB_SETUP_STRUCT * setup_packet, 
                               uint_8_ptr *data, 
                               uint_32 *size);
uint_8 USB_Strd_Req_Set_Config(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                               USB_SETUP_STRUCT * setup_packet, 
                               uint_8_ptr *data, 
                               uint_32 *size);
uint_8 USB_Strd_Req_Get_Interface(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                                  USB_SETUP_STRUCT * setup_packet, 
                                  uint_8_ptr *data, 
                                  uint_32 *size);
uint_8 USB_Strd_Req_Set_Interface(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                                  USB_SETUP_STRUCT * setup_packet, 
                                  uint_8_ptr *data, 
                                  uint_32 *size);
uint_8 USB_Strd_Req_Sync_Frame(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                               USB_SETUP_STRUCT * setup_packet, 
                               uint_8_ptr *data, 
                               uint_32 *size);
uint_8 USB_Strd_Req_Get_Descriptor(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                                   USB_SETUP_STRUCT * setup_packet, 
                                   uint_8_ptr *data, 
                                   uint_32 *size);

#ifdef DELAYED_PROCESSING
    void USB_Control_Service_Callback( PTR_USB_EVENT_STRUCT event,pointer arg );
#endif
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
                                  
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
 USB_FW_REQ_FUNC g_standard_request[MAX_STRD_REQ] = 
 { 
    USB_Strd_Req_Get_Status,
    USB_Strd_Req_Feature,
    NULL,
    USB_Strd_Req_Feature,
    NULL,
    USB_Strd_Req_Set_Address,
    USB_Strd_Req_Get_Descriptor,
    NULL,
    USB_Strd_Req_Get_Config,
    USB_Strd_Req_Set_Config,
    USB_Strd_Req_Get_Interface,
    USB_Strd_Req_Set_Interface,
    USB_Strd_Req_Sync_Frame
 };
 
  /*************************************************************************//*!
 *
 * @name  USB_Class_Allocate_Handle
 *
 * @brief The funtion reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static USB_STATUS  USB_Class_Fw_Allocate_Handle(USB_CLASS_FW_HANDLE *handle)
{
    uint_32 cnt = 0;
    for (;cnt< USB_MAX_CLASS_FW_OBJECT;cnt++)
    {
       if (usb_class_fw_object[cnt] == NULL)
       {
          *handle = cnt;
          return USB_OK;
       }
    }
    return USBERR_DEVICE_BUSY;
}

 /*************************************************************************//*!
 *
 * @name  USB_Class_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/

static int_32 USB_Class_Fw_Free_Handle(USB_CLASS_FW_HANDLE handle)
{
    if (handle > USB_MAX_CLASS_FW_OBJECT)
        return USBERR_ERROR;
    
    USB_mem_free((void *)usb_class_fw_object[handle]);
    usb_class_fw_object[handle] = NULL;
    return USB_OK;
}
 /*************************************************************************//*!
 *
 * @name  Get_Usb_Class_Fw_Object_Ptr
 *
 * @brief The funtion gets the class object pointer from class array .
 *
 * @param handle  index in class object array.
 * @return returns returns pointer to USB Class FW Object structure..      
 *
 *****************************************************************************/
static USB_CLASS_FW_OBJECT_STRUCT_PTR Get_Usb_Class_Fw_Object_Ptr
(
   USB_CLASS_FW_HANDLE handle
)
{
     return usb_class_fw_object[handle]; 
}

/**************************************************************************//*!
 *
 * @name  USB_Framework_Init
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle :              handle to Identify the controller
 * @param class_callback:       event callback 
 * @param other_req_callback:   callback for vendor or usb framework class req    
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
USB_CLASS_FW_HANDLE USB_Framework_Init
(
    _usb_device_handle handle, /*[IN]*/
    USB_CLASS_CALLBACK class_callback,/*[IN]*/
    USB_REQ_FUNC other_req_callback,/*[IN]*/
    pointer callback_data,/*[IN]*/
    int_32 data,/*[IN]*/
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr /*[IN]*/
        
) 
{   
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr = NULL;
    USB_CLASS_FW_HANDLE fw_handle;
    uint_8 error=USB_OK;
    
    usb_fw_ptr = (USB_CLASS_FW_OBJECT_STRUCT_PTR)USB_mem_alloc_zero(
        sizeof(USB_CLASS_FW_OBJECT_STRUCT));
    if (NULL == usb_fw_ptr)
    {
        #ifdef _DEV_DEBUG
            printf("USB_Framework_Init: Memalloc failed\n");
        #endif  
        return USBERR_ALLOC;
    }

     if (USBERR_DEVICE_BUSY == USB_Class_Fw_Allocate_Handle(&fw_handle))
     {
        USB_mem_free((void *)usb_fw_ptr);
        usb_fw_ptr = NULL;
        return USBERR_ERROR;
     }
    /* save input parameters */
    
    usb_fw_ptr->class_handle = data;
    usb_fw_ptr->ext_req_to_host = NULL;

    /* reserve memory for setup token and data received */
    usb_fw_ptr->ext_req_to_host = (uint_8*)USB_mem_alloc_zero(MAX_EXPECTED_CONTROL_OUT_SIZE);
    if (NULL == usb_fw_ptr->ext_req_to_host)
    {
        #ifdef _DEV_DEBUG
            printf("ext_req_to_host malloc failed: USB_Framework_Init\n");
        #endif  
        return USBERR_ALLOC;
    }

    usb_fw_ptr->controller_handle = handle;
    usb_fw_ptr->other_req_callback.callback = other_req_callback;
    usb_fw_ptr->other_req_callback.arg = callback_data;
    usb_fw_ptr->class_callback.callback = class_callback;
    usb_fw_ptr->class_callback.arg = callback_data;
    USB_mem_copy( desc_callback_ptr,&usb_fw_ptr->desc,
    sizeof(DESC_CALLBACK_FUNCTIONS_STRUCT));
   
    /* Register CONTROL service */      
    error = _usb_device_register_service(handle, USB_SERVICE_EP0, 

#ifdef DELAYED_PROCESSING    
                                                 USB_Control_Service_Callback,
#else
                                                          USB_Control_Service,
#endif                                                          
     (pointer)usb_fw_ptr );
     
    usb_class_fw_object[fw_handle] = usb_fw_ptr;
    return fw_handle;     
}

/**************************************************************************//*!
 *
 * @name  USB_Framework_Deinit
 *
 * @brief The funtion deinitializes the Class Module 
 *
 * @param handle :              handle to Identify the controller
 * @param fw_handle:       		Framework handle     
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint_8 USB_Framework_Deinit
(
    _usb_device_handle handle, /*[IN]*/
    USB_CLASS_FW_HANDLE fw_handle /*[IN]*/
        
) 
{   
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr = NULL;
    uint_8 error = USB_OK;
    usb_fw_ptr = Get_Usb_Class_Fw_Object_Ptr(fw_handle);
    USB_mem_free((void *)(usb_fw_ptr->ext_req_to_host));
    error = _usb_device_unregister_service(handle, USB_SERVICE_EP0);
    USB_Class_Fw_Free_Handle(fw_handle);   
    return error;
}
#ifdef DELAYED_PROCESSING
/**************************************************************************//*!
 *
 * @name  USB_Framework_Periodic_Task
 *
 * @brief The funtion is called to respond to any control request
 *
 * @param None
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Framework_Periodic_Task(void)
{
    /* if control request pending to be completed */
    if(g_control_pending==TRUE) 
    {   /* handle pending control request */
        USB_Control_Service(&g_f_event);       
        g_control_pending=FALSE;
    }
} 
#endif
/**************************************************************************//*!
 *
 * @name  USB_Framework_Reset
 *
 * @brief The funtion resets the framework 
 *
 * @param handle: handle to Identify the controller
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint_8 USB_Framework_Reset(_usb_device_handle handle)
{ 
    UNUSED_ARGUMENT(handle)
    return USB_OK; 
}

#ifdef DELAYED_PROCESSING
/**************************************************************************//*!
 *
 * @name  USB_Control_Service_Callback
 *
 * @brief The funtion can be used as a callback function to the service. 
 *
 * @param event:              from Service callback function
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Control_Service_Callback(PTR_USB_EVENT_STRUCT event,pointer arg ) 
{
    UNUSED_ARGUMENT(arg)
    /* save the event parameters */
    g_f_event.buffer_ptr = event->buffer_ptr;
    g_f_event.handle = event->handle; 
    g_f_event.ep_num = event->ep_num;
    g_f_event.setup  = event->setup;
    g_f_event.len = event->len;
    g_f_event.errors = event->errors;
    
    /* set the pending request flag */
    g_control_pending=TRUE;             
}
#endif

/**************************************************************************//*!
 *
 * @name  USB_Control_Service
 *
 * @brief Called upon a completed endpoint 0 (USB 1.1 Chapter 9) transfer 
 *
 * @param event:        from Service callback function
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Control_Service (PTR_USB_EVENT_STRUCT event,pointer arg )
{        
    uint_16    device_state = 0;
    uint_8     error = USBERR_INVALID_REQ_TYPE;
    uint_8_ptr data = NULL;
    uint_32 size; 
    USB_CLASS_FW_OBJECT_STRUCT_PTR    usb_fw_ptr;

    usb_fw_ptr = (USB_CLASS_FW_OBJECT_STRUCT_PTR)arg;
    
    /* get the device state  */
    (void)_usb_device_get_status(event->handle, (uint_8)USB_STATUS_DEVICE_STATE,
        &device_state);

    if (event->setup == TRUE) 
    {
        USB_mem_copy(event->buffer_ptr,&usb_fw_ptr->setup_packet,
        USB_SETUP_PKT_SIZE);        
        
        /* take care of endianess  of the 16 bit fields correctly */
        usb_fw_ptr->setup_packet.index = SHORT_LE_TO_HOST(usb_fw_ptr->setup_packet.index);
        usb_fw_ptr->setup_packet.value = SHORT_LE_TO_HOST(usb_fw_ptr->setup_packet.value);
        usb_fw_ptr->setup_packet.length = SHORT_LE_TO_HOST(usb_fw_ptr->setup_packet.length);
        /* if the request is standard request */
        if ((usb_fw_ptr->setup_packet.request_type & USB_REQUEST_CLASS_MASK) == 
            USB_REQUEST_CLASS_STRD) 
        {
            /* if callback is not NULL */
            if (g_standard_request[usb_fw_ptr->setup_packet.request] != NULL) 
            {
                /* if the request is valid in this device state */
                if((device_state < USB_STATE_POWERED) && 
                    (g_validate_request[usb_fw_ptr->setup_packet.request][device_state] 
                        ==TRUE)) 
                {                   
                    /* Standard Request function pointers */
                    error = g_standard_request[usb_fw_ptr->setup_packet.request]
                        (usb_fw_ptr,&usb_fw_ptr->setup_packet,&data,&size);
                }
            }
        }
        else /* for Class/Vendor requests */
        {  
            /*get the length from the setup_request*/
            size = usb_fw_ptr->setup_packet.length;

            if( (size != 0) && 
                ((usb_fw_ptr->setup_packet.request_type & USB_DATA_DIREC_MASK) == 
                    USB_DATA_TO_DEVICE))
            {                 
                /* we have gone for one time meory allocation of ext_req_to_host
                   to avoid memory fragmentation, as there was memory crunch in 
                   some of our Socs. */                                
                if((size + USB_SETUP_PKT_SIZE) > MAX_EXPECTED_CONTROL_OUT_SIZE)
                {
                    #ifdef _DEV_DEBUG
                        printf("MAX_EXPECTED_CONTROL_OUT_SIZE insufficient, needed %d\n", size + USB_SETUP_PKT_SIZE);
                        printf("Please change the macro!!!\n");
                    #endif  
                    return;
                }

                /* copy setup token to ext_req_to_host */
                USB_mem_copy(&usb_fw_ptr->setup_packet, usb_fw_ptr->ext_req_to_host, USB_SETUP_PKT_SIZE);
                #if PSP_HAS_DATA_CACHE
                   /* Workaround: the following call will do invalidate of cache lines. Since the last operation did a memory copy,
                   ** the copied bytes will be invalidated. To prevent that, do first flushing.
                   */
                    USB_dcache_flush_mlines(usb_fw_ptr->ext_req_to_host, USB_SETUP_PKT_SIZE);
                #endif
                /* expecting host to send data (OUT TRANSACTION)*/
                (void)_usb_device_recv_data(event->handle, 
                    USB_CONTROL_ENDPOINT,(usb_fw_ptr->ext_req_to_host + USB_SETUP_PKT_SIZE),
                        (uint_32)(size));
                return;                   
            }
            /*call class/vendor request*/
            else if(usb_fw_ptr->other_req_callback.callback != NULL)
            {            
                error = usb_fw_ptr->other_req_callback.callback(
                &usb_fw_ptr->setup_packet,&data,&size,usb_fw_ptr->other_req_callback.arg);
            }
        } 
        USB_Control_Service_Handler(usb_fw_ptr,error,&usb_fw_ptr->setup_packet,
            &data,&size);
    } 
    else if(device_state == USB_STATE_PENDING_ADDRESS)
    {   /* Device state is PENDING_ADDRESS */
        /* Assign the new adddress to the Device */
        (void)USB_Strd_Req_Assign_Address(usb_fw_ptr);
    }
    else if( ((usb_fw_ptr->setup_packet.request_type & USB_DATA_DIREC_MASK)==
              USB_DATA_TO_DEVICE) && (event->direction == USB_RECV) && 
              (usb_fw_ptr->setup_packet.length))
    {           
        /* execution enters Control Service because of 
           OUT transaction on USB_CONTROL_ENDPOINT*/
        if(usb_fw_ptr->other_req_callback.callback != NULL) 
        {   /* class or vendor request */
            size = event->len+USB_SETUP_PKT_SIZE;
            error = usb_fw_ptr->other_req_callback.callback(
            (USB_SETUP_STRUCT*) usb_fw_ptr->ext_req_to_host,
             &data,&size,usb_fw_ptr->other_req_callback.arg); 
        }
        USB_Control_Service_Handler(usb_fw_ptr,error,&usb_fw_ptr->setup_packet,
            &data,&size);
    }    
    return; 
}           

/**************************************************************************//*!
 *
 * @name  USB_Control_Service_Handler
 *
 * @brief The function is used to send a response to the Host based. 
 *
 * @param status:           status of Device e.g USB_OK,USBERR_INVALID_REQ_TYPE
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Control_Service_Handler
(
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    uint_8 error,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
) 
{      
    if(error == USBERR_INVALID_REQ_TYPE)
    {   
        uint_8 direction = USB_SEND;
        uint_8 ep_num = USB_CONTROL_ENDPOINT;
        /* prepare the component to be sent in lower layer with 
           endpoint number and direction*/
        uint_8 component = (uint_8)(ep_num | (direction<<COMPONENT_PREPARE_SHIFT));
        
        /* incase of error Stall endpoint and update the status*/        
        (void)_usb_device_set_status(usb_fw_ptr->controller_handle,
            (uint_8)(component|USB_STATUS_ENDPOINT),
            (uint_16)USB_STATUS_STALLED);
    }
    else /* Need to send Data to the USB Host */
    {   
        /* send the data prepared by the handlers.*/
        if(*size > setup_packet->length) 
        {            
            *size = setup_packet->length;
        }
        
        /* send the data to the host */
        (void)USB_Class_Send_Data(usb_fw_ptr->class_handle,
            USB_CONTROL_ENDPOINT, *data, *size);

        if((setup_packet->request_type & USB_DATA_DIREC_MASK) ==     
            USB_DATA_TO_HOST)
        {   /* Request was to Get Data from device */          
            /* setup rcv to get status from host */
            (void)_usb_device_recv_data(usb_fw_ptr->controller_handle,
                USB_CONTROL_ENDPOINT,NULL,0);
        }
    }
    return;
}

/*************************************************************************//*!
 *
 * @name  USB_Strd_Req_Get_Status
 *
 * @brief  This function is called in response to Get Status request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Strd_Req_Get_Status(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                               USB_SETUP_STRUCT * setup_packet, 
                               uint_8_ptr *data, 
                               uint_32 *size)
{  
    uint_8 interface, endpoint;
    uint_8 error = USBERR_ERROR;
    
    if((setup_packet->request_type & USB_REQUEST_SRC_MASK) == 
        USB_REQUEST_SRC_DEVICE) 
    {   /* request for Device */
        error = _usb_device_get_status(usb_fw_ptr->controller_handle,
            (uint_8)USB_STATUS_DEVICE, &usb_fw_ptr->std_framework_data);
            
        usb_fw_ptr->std_framework_data &= GET_STATUS_DEVICE_MASK; 
        usb_fw_ptr->std_framework_data = SHORT_LE_TO_HOST(usb_fw_ptr->std_framework_data);                                      
        *size=DEVICE_STATUS_SIZE;                                      
    }
    else if ((setup_packet->request_type & USB_REQUEST_SRC_MASK) == 
        USB_REQUEST_SRC_INTERFACE) 
    {    /* request for Interface */
        interface  = (uint_8) setup_packet->index;
        error =  usb_fw_ptr->desc.GET_DESC_INTERFACE(usb_fw_ptr->desc.handle,
            interface,(uint_8_ptr)&usb_fw_ptr->std_framework_data);
        *size=INTERFACE_STATUS_SIZE;
    }
    else if ((setup_packet->request_type & USB_REQUEST_SRC_MASK) == 
        USB_REQUEST_SRC_ENDPOINT)
    {   /* request for Endpoint */
        endpoint  =(uint_8)(((uint_8)setup_packet->index)|USB_STATUS_ENDPOINT);
        error =  _usb_device_get_status(usb_fw_ptr->controller_handle,
            (uint_8)endpoint, 
            &usb_fw_ptr->std_framework_data);
        usb_fw_ptr->std_framework_data = SHORT_LE_TO_HOST(usb_fw_ptr->std_framework_data);                                      
        *size=ENDP_STATUS_SIZE;
    }
   
    *data = (uint_8_ptr)&usb_fw_ptr->std_framework_data;
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Feature
 *
 * @brief  This function is called in response to Clear or Set Feature request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Strd_Req_Feature(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                            USB_SETUP_STRUCT * setup_packet, 
                            uint_8_ptr *data, 
                            uint_32 *size)
{   
    uint_16 device_status;
    uint_16 set_request;
    uint_8  error=USBERR_INVALID_REQ_TYPE;
    uint_8  epinfo;
    uint_8  event;
    UNUSED_ARGUMENT(data)
    *size=0;
    /* find whether its a clear feature request or a set feature request */
    set_request = (uint_16)((setup_packet->request & USB_SET_REQUEST_MASK)>>1);
         
    if((setup_packet->request_type & USB_REQUEST_SRC_MASK) == 
        USB_REQUEST_SRC_DEVICE) 
    {
        if(setup_packet->value & DEVICE_REQUEST_MASK)
        {   /* request for Device */  
            error = _usb_device_get_status(usb_fw_ptr->controller_handle,
                (uint_8)USB_STATUS_DEVICE, &device_status);
            if(set_request == (uint_16)TRUE)  
            {  /* add the request to be cleared from device_status */
                device_status |= (1<<setup_packet->value); 
            }
            else   
            {   /* remove the request to be cleared from device_status */
                device_status &= ~(1<<setup_packet->value); 
            }
          
            /* set the status on the device */
            error = _usb_device_set_status(usb_fw_ptr->controller_handle, 
                (uint_8)USB_STATUS_DEVICE, 
                (uint_16)device_status);
        }         
    }
    else if ((setup_packet->request_type & USB_REQUEST_SRC_MASK) == 
        USB_REQUEST_SRC_ENDPOINT) /*request for Endpoint*/  
    {   /* request for Endpoint */
        epinfo = (uint_8)(setup_packet->index & 0x00FF);
        error = _usb_device_set_status(usb_fw_ptr->controller_handle,
            (uint_8)(epinfo|USB_STATUS_ENDPOINT), (uint_16)set_request);
        event = (uint_8)(set_request?USB_APP_EP_STALLED:USB_APP_EP_UNSTALLED);
        /* inform the upper layers of stall/unstall */
        if (usb_fw_ptr->class_callback.callback != NULL) 
        {
          usb_fw_ptr->class_callback.callback(event,(void*)&epinfo,
          usb_fw_ptr->class_callback.arg);  
        }
        
    }
           
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Set_Address
 *
 * @brief  This function is called in response to Set Address request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Strd_Req_Set_Address
(   USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{  
    UNUSED_ARGUMENT(data)    
    *size=0;
    /* update device stae */
    (void)_usb_device_set_status(usb_fw_ptr->controller_handle,
        (uint_8)USB_STATUS_DEVICE_STATE,
        (uint_16)USB_STATE_PENDING_ADDRESS);
    /*store the address from setup_packet into assigned_address*/
    usb_fw_ptr->assigned_address = setup_packet->value;
 
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Assign_Address
 *
 * @brief  This function assigns the address to the Device
 *
 * @param handle      
 *
 * @return status       
 *                       USB_OK: Always
 *
 *****************************************************************************/
uint_8 USB_Strd_Req_Assign_Address(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr)
{     
    /* Set Device Address */
    (void)_usb_device_set_address(usb_fw_ptr->controller_handle,
        (uint_8)(usb_fw_ptr->assigned_address&0x00FF));
    /* Set Device state */
    (void)_usb_device_set_status(usb_fw_ptr->controller_handle, 
        (uint_8)USB_STATUS_DEVICE_STATE, 
        (uint_16)USB_STATE_ADDRESS);
    /* Set Device state */                                  
    (void)_usb_device_set_status(usb_fw_ptr->controller_handle,
    (uint_8)USB_STATUS_ADDRESS,
    (uint_16)usb_fw_ptr->assigned_address);
  
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Get_Config
 *
 * @brief  This function is called in response to Get Config request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always
 *
 *****************************************************************************/ 
uint_8 USB_Strd_Req_Get_Config
(   
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{
    UNUSED_ARGUMENT(setup_packet)
    
    *size=CONFIG_SIZE;
    (void)_usb_device_get_status(usb_fw_ptr->controller_handle,
        (uint_8)USB_STATUS_CURRENT_CONFIG, &usb_fw_ptr->std_framework_data);
    usb_fw_ptr->std_framework_data = SHORT_LE_TO_HOST(usb_fw_ptr->std_framework_data);                                      
    *data= (uint_8_ptr)&usb_fw_ptr->std_framework_data;      

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Set_Config
 *
 * @brief  This function is called in response to Set Config request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/ 
uint_8 USB_Strd_Req_Set_Config
(   USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{
    uint_8 error = USBERR_INVALID_REQ_TYPE;
    uint_16 config_val;
    UNUSED_ARGUMENT(data)     
    *size=0;
    error = USB_STATUS_ERROR;
    config_val = setup_packet->value;
    
    if (usb_fw_ptr->desc.IS_DESC_VALID_CONFIGURATION(usb_fw_ptr->desc.handle, 
    config_val)) 
    /*if valid configuration (fn returns boolean value)*/
    {       
        uint_16 device_state = USB_STATE_CONFIG;

        if (!config_val) /* if config_val is 0 */ 
        {
            device_state = USB_STATE_ADDRESS ;
        }
        
        error = _usb_device_set_status(usb_fw_ptr->controller_handle, 
            (uint_8)USB_STATUS_DEVICE_STATE, 
            (uint_16)device_state);
        error = _usb_device_set_status(usb_fw_ptr->controller_handle, 
            (uint_8)USB_STATUS_CURRENT_CONFIG, 
            (uint_16)config_val);
       /* callback to the app. to let the application know about the new 
          Config */
        if (usb_fw_ptr->class_callback.callback!=NULL)  
        {
           usb_fw_ptr->class_callback.callback(USB_APP_CONFIG_CHANGED, 
            (void *)&config_val,usb_fw_ptr->class_callback.arg);
           usb_fw_ptr->class_callback.callback(USB_APP_ENUM_COMPLETE,NULL,
           usb_fw_ptr->class_callback.arg);     
        }
        
    }     
    return error;
 }

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Get_Interface
 *
 * @brief  This function is called in response to Get Interface request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/  
uint_8 USB_Strd_Req_Get_Interface
(
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{
    uint_8 error;
      
    *size = INTERFACE_STATUS_SIZE;
    error = usb_fw_ptr->desc.GET_DESC_INTERFACE(usb_fw_ptr->desc.handle, 
        (uint_8)setup_packet->index, (uint_8_ptr)&usb_fw_ptr->std_framework_data);
    *data = (uint_8_ptr)&usb_fw_ptr->std_framework_data;

    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Set_Interface
 *
 * @brief  This function is called in response to Set Interface request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : Always       
 *
 *****************************************************************************/ 
uint_8 USB_Strd_Req_Set_Interface
(
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{     

    uint_8 error;
    UNUSED_ARGUMENT(data)
    *size=0;
    
    /* Request type not for interface */
    if ((setup_packet->request_type & 0x03) != 0x01) 
    {
        return USB_STATUS_ERROR;
    }
    
    /* Get Interface and alternate interface from setup_packet */
    error = usb_fw_ptr->desc.SET_DESC_INTERFACE(usb_fw_ptr->desc.handle, 
        (uint_8)setup_packet->index, (uint_8)setup_packet->value); 
                                                       
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Strd_Req_Sync_Frame
 *
 * @brief  This function is called in response to Sync Frame request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Strd_Req_Sync_Frame
(
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{
    uint_8 error;
    UNUSED_ARGUMENT(setup_packet)
    
    *size=FRAME_SIZE;
    
    /* Get the frame number */
    error = _usb_device_get_status(usb_fw_ptr->controller_handle,
        (uint_8)USB_STATUS_SOF_COUNT, 
        &usb_fw_ptr->std_framework_data);
    *data= (uint_8_ptr)&usb_fw_ptr->std_framework_data;      
    
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Std_Req_Get_Descriptor
 *
 * @brief  This function is called in response to Get Descriptor request
 *
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned 
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Strd_Req_Get_Descriptor
(
    USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size
)
{
    /*g_setup_packet.hValue*/
    uint_8 type = (setup_packet->value >> 8) & 0xFF ;
    uint_16 index = (uint_8)UNINITIALISED_VAL;
    uint_8 str_num = (uint_8)UNINITIALISED_VAL;
    uint_8 error;
    
    if (type == STRING_DESCRIPTOR_TYPE)
    {   /* for string descriptor set the language and string number */ 
        index = setup_packet->index;
        /*g_setup_packet.lValue*/
        str_num = setup_packet->value & 0xFF;
    }
    
    /* Call descriptor class to get descriptor */
    error = usb_fw_ptr->desc.GET_DESC(usb_fw_ptr->desc.handle,
        type,str_num,index,data,size);  
    return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Framework_GetDesc
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB framework handle. Received from
 *                          USB_Framework_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Framework_GetDesc(
    USB_CLASS_FW_HANDLE handle, /*[IN]*/
    int_32 cmd,/*[IN]*/
    uint_8 in_data,/*[IN]*/
    uint_8_ptr * in_buff/*[OUT]*/
)
{
   USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr;
   uint_8 error; 
   if (handle > USB_MAX_CLASS_FW_OBJECT)
      return USBERR_ERROR;
   
   usb_fw_ptr = Get_Usb_Class_Fw_Object_Ptr(handle);
   if (usb_fw_ptr == NULL)
        return USBERR_ERROR;
   
   error = usb_fw_ptr->desc.DESC_GET_FEATURE(usb_fw_ptr->desc.handle,cmd,in_data,in_buff);
   return error;        
}

/**************************************************************************//*!
 *
 * @name  USB_Framework_SetDesc
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB framework handle. Received from
 *                          USB_Framework_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param outBuf           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Framework_SetDesc(
    USB_CLASS_FW_HANDLE handle,/*[IN]*/
    int_32 cmd,/*[IN]*/
    uint_8 input_data,/*[IN]*/
    uint_8_ptr * outBuf/*[IN]*/
)
{
   USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr;
    
   if (handle > USB_MAX_CLASS_FW_OBJECT)
      return USBERR_ERROR;
   
   usb_fw_ptr = Get_Usb_Class_Fw_Object_Ptr(handle);
   if (usb_fw_ptr == NULL)
        return USBERR_ERROR;
   
  return usb_fw_ptr->desc.DESC_SET_FEATURE(usb_fw_ptr->desc.handle,cmd,
  input_data,outBuf);
            
}

/**************************************************************************//*!
 *
 * @name  USB_Framework_Remote_wakeup
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB framework handle. Received from
 *                          USB_Framework_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
boolean USB_Framework_Remote_wakeup(USB_CLASS_FW_HANDLE handle)
{
   USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr;
    
   if (handle > USB_MAX_CLASS_FW_OBJECT)
      return USBERR_ERROR;
   
   usb_fw_ptr = Get_Usb_Class_Fw_Object_Ptr(handle);
   if (usb_fw_ptr == NULL)
        return USBERR_ERROR;
   
   return usb_fw_ptr->desc.DESC_REMOTE_WAKEUP(usb_fw_ptr->desc.handle);
    
}
/* EOF */
