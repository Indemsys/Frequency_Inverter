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
* $FileName: usb_class.c$
* $Version : 3.8.6.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
* @brief The file contains USB stack Class module implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"
#include "devapi.h"
#include "usb_framework.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/


/****************************************************************************
 * Global Variables
 ****************************************************************************/
static USB_CLASS_OBJECT_STRUCT_PTR usb_class_object[USB_MAX_CLASS_OBJECT];
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes 
 *****************************************************************************/
static void USB_Suspend_Service (PTR_USB_EVENT_STRUCT event,pointer arg ); 
static void USB_Resume_Service (PTR_USB_EVENT_STRUCT event,pointer arg );
static void USB_Stall_Service (PTR_USB_EVENT_STRUCT event, pointer arg);
static void USB_Sof_Service (PTR_USB_EVENT_STRUCT event, pointer arg);
static void USB_Reset_Service (PTR_USB_EVENT_STRUCT event ,pointer arg);
static void USB_Error_Service (PTR_USB_EVENT_STRUCT event ,pointer arg);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/ 
 
 /*****************************************************************************
 * Local Functions - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

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
static USB_CLASS_HANDLE  USB_Class_Allocate_Handle()
{
    int cnt = 0;
    for (;cnt< USB_MAX_CLASS_OBJECT;cnt++)
    {
       if (usb_class_object[cnt] == NULL)
        return cnt;
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

static int_32 USB_Class_Free_Handle(USB_CLASS_HANDLE handle)
{
    if ((handle < 0) || (handle > USB_MAX_CLASS_OBJECT))
        return USBERR_ERROR;
    
    USB_mem_free((void *)usb_class_object[handle]);
    usb_class_object[handle] = NULL;
    return USB_OK;
}
 /*************************************************************************//*!
 *
 * @name  Get_Usb_Class_Object_Ptr
 *
 * @brief The funtion gets the class object pointer from class array .
 *
 * @param handle  index in class object array.
 * @return returns returns pointer to USB Class Object structure..      
 *
 *****************************************************************************/
static USB_CLASS_OBJECT_STRUCT_PTR Get_Usb_Class_Object_Ptr(USB_CLASS_HANDLE handle)
{
     return usb_class_object[handle]; 
}
/**************************************************************************//*!
 *
 * @name  USB_Suspend_Service
 *
 * @brief The funtion is called when host suspends the USB port
 *
 * @param event:        for Service callback function     
 *
 * @return None       
 *****************************************************************************/
static void USB_Suspend_Service (PTR_USB_EVENT_STRUCT event,pointer arg )
{    
    UNUSED_ARGUMENT(event)
    UNUSED_ARGUMENT(arg)
    return;
}

/**************************************************************************//*!
 *
 * @name  USB_Resume_Service
 *
 * @brief The funtion is called when host resumes the USB port
 *
  * @param event:        for Service callback function     
 *
 * @return None       
 *****************************************************************************/
static void USB_Resume_Service (PTR_USB_EVENT_STRUCT event,pointer arg )
{    
    UNUSED_ARGUMENT(event)
    UNUSED_ARGUMENT(arg)
    return;
}

/**************************************************************************//*!
 *
 * @name  USB_Stall_Service
 *
 * @brief The funtion is called when endpoint is stalled
 *
  * @param event:        for Service callback function     
 *
 * @return None       
 *****************************************************************************/
static void USB_Stall_Service (PTR_USB_EVENT_STRUCT event,pointer arg )
{
    UNUSED_ARGUMENT(arg)       
    /* need to unstall control endpoint immediately. For other endpoint 
       types there will be class specific request to unstall or a reset*/                   
    if(event->ep_num == USB_CONTROL_ENDPOINT)
    {
        /* incase of error Stall endpoint and update the status*/        
        (void)_usb_device_set_status(event->handle,
            (uint_8)(event->ep_num|USB_STATUS_ENDPOINT),
            (uint_16)USB_STATUS_IDLE);
    }
    return;
}

/**************************************************************************//*!
 *
 * @name  USB_Sof_Service
 *
 * @brief The funtion is called when sof flag is set (from ISR)
 *
 * @param event:        for Service callback function     
 *
 * @return None       
 *****************************************************************************/
static void USB_Sof_Service (PTR_USB_EVENT_STRUCT event,pointer arg )
{
    uint_16 sof_count = 0;
    UNUSED_ARGUMENT(arg)
    
    /* update SOF */
    sof_count = event->buffer_ptr[0];
    sof_count <<= SOF_HIGH_BYTE_SHIFT;
    sof_count |= event->buffer_ptr[1];
     
    /* write SOF to status */
    (void)_usb_device_set_status(event->handle, 
        (uint_8)USB_STATUS_SOF_COUNT,
        (uint_16)sof_count);
    return; 
}
 

/**************************************************************************//*!
 *
 * @name  USB_Reset_Service
 *
 * @brief The funtion is called upon a bus reset event.  
              Initialises the control endpoint.
 *
 * @param event:        for Service callback function     
 *
 * @return None       
 *****************************************************************************/
static void USB_Reset_Service(PTR_USB_EVENT_STRUCT event,/*[IN]*/
pointer arg/*[IN]*/
)
{    
    USB_EP_STRUCT ep_struct;
    USB_CLASS_OBJECT_STRUCT_PTR class_object_ptr;
    uint_8 component;
    
    class_object_ptr = (USB_CLASS_OBJECT_STRUCT_PTR)arg;

    if (NULL == class_object_ptr)
    {
        #if _DEBUG
            printf("RESET SERVICE ERROR: Class not intialized.\n");
        #endif  
        return;
    }

    /* Initialize the endpoint 0 in both directions */
    ep_struct.direction = USB_RECV;
    ep_struct.ep_num = USB_CONTROL_ENDPOINT;
    ep_struct.size = CONTROL_MAX_PACKET_SIZE;
    ep_struct.type = USB_CONTROL_PIPE;

    (void)_usb_device_init_endpoint(event->handle, &ep_struct, TRUE);
    component = (uint_8)(ep_struct.ep_num | 
        (USB_RECV << COMPONENT_PREPARE_SHIFT));
    /* set the EndPoint Status as Idle in the device layer */
    (void)_usb_device_set_status(event->handle,
        (uint_8)(USB_STATUS_ENDPOINT|component), 
        (uint_16)USB_STATUS_IDLE);    
    
    ep_struct.direction = USB_SEND;   
    (void)_usb_device_init_endpoint(event->handle, &ep_struct, TRUE);
    component = (uint_8)(ep_struct.ep_num | 
        (uint_8)(USB_SEND << COMPONENT_PREPARE_SHIFT));
    /* set the EndPoint Status as Idle in the device layer */
    (void)_usb_device_set_status(event->handle,
        (uint_8)(USB_STATUS_ENDPOINT|component), 
        (uint_16)USB_STATUS_IDLE);    
    
    /* let the application know that bus reset has taken place */
    if (class_object_ptr->class_callback!=NULL)
    {
        class_object_ptr->class_callback(USB_APP_BUS_RESET,NULL,class_object_ptr->arg); 
    }

    /* set the default device state */
    (void)_usb_device_set_status(event->handle, 
        (uint_8)USB_STATUS_DEVICE_STATE, 
        (uint_16)USB_STATE_DEFAULT);
    /* set the default device state -- 
       remote wakeup field is set to zero when device is reset*/
    (void)_usb_device_set_status(event->handle, 
        (uint_8)USB_STATUS_DEVICE,                           
        (uint_16)(SELF_POWERED>>SELF_POWER_BIT_SHIFT));
                                                                      
    return;
}

/**************************************************************************//*!
 *
 * @name  USB_Error_Service
 *
 * @brief The funtion is called when an error has been detected
 *
 * @param event:        for Service callback function     
 *
 * @return None       
 *****************************************************************************/
static void USB_Error_Service (PTR_USB_EVENT_STRUCT event,/*[IN]*/
pointer arg /*[IN]*/
)
{
    USB_CLASS_OBJECT_STRUCT_PTR class_object_ptr;
    
    class_object_ptr = (USB_CLASS_OBJECT_STRUCT_PTR)arg;
    
    if (NULL == class_object_ptr)
    {
        #if _DEBUG
            printf("ERROR SERVICE ERROR: Class not intialized.\n");
        #endif  
        return;
    }

    /* notify the application of the error */
    if (class_object_ptr->class_callback!=NULL)
        class_object_ptr->class_callback(USB_APP_ERROR,event->buffer_ptr,class_object_ptr->arg);
    return; 
}

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
 ) 
{
    USB_CLASS_HANDLE  class_handle;
    USB_CLASS_OBJECT_STRUCT_PTR class_object_ptr = NULL;
    
    class_object_ptr = (USB_CLASS_OBJECT_STRUCT_PTR)USB_mem_alloc_zero(
        sizeof(USB_CLASS_OBJECT_STRUCT));
    if (NULL == class_object_ptr)
    {
        #if _DEBUG
            printf("USB_Class_Init: Memalloc failed\n");
        #endif  
        return USBERR_ALLOC;
    }
    
    class_handle = USB_Class_Allocate_Handle();
    if (USBERR_DEVICE_BUSY == class_handle) 
    {
        USB_mem_free((void*)class_object_ptr);
        class_object_ptr = NULL;
        return USBERR_DEVICE_BUSY;
    }
        
    class_object_ptr->controller_handle = handle;
    class_object_ptr->class_callback = class_callback;
    class_object_ptr->arg = user_arg;
       
    /* Store Framework handle for later use from callbacks*/    
    class_object_ptr->usb_fw_handle = USB_Framework_Init(handle, class_callback,
        other_req_callback, user_arg, class_handle,desc_callback_ptr);

    if(class_object_ptr->usb_fw_handle != USBERR_ERROR) 
    {   /* Register all the services here */  
        _usb_device_register_service(handle,
            USB_SERVICE_BUS_RESET, USB_Reset_Service,(void *)class_object_ptr);
        _usb_device_register_service(handle,
            USB_SERVICE_SOF,USB_Sof_Service,(void *)class_object_ptr);
        _usb_device_register_service(handle,
            USB_SERVICE_SLEEP,USB_Suspend_Service,(void *)class_object_ptr);
        _usb_device_register_service(handle,
            USB_SERVICE_RESUME,USB_Resume_Service,(void *)class_object_ptr);
        _usb_device_register_service(handle,
            USB_SERVICE_STALL,USB_Stall_Service,(void *)class_object_ptr);    
        _usb_device_register_service
            (handle, USB_SERVICE_ERROR,USB_Error_Service,(void *)class_object_ptr);
        /* set the device state as powered */
        (void)_usb_device_set_status(handle,
            (uint_8)USB_STATUS_DEVICE_STATE, 
            (uint_16)USB_STATE_POWERED);
    }  
    
    usb_class_object[class_handle] = class_object_ptr;     
    return class_handle;     
}

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
 ) 
{
    USB_CLASS_OBJECT_STRUCT_PTR class_object_ptr = NULL;
    uint_8 error = USB_OK;
    class_object_ptr = Get_Usb_Class_Object_Ptr(class_handle);
    /* call USB_Framework_Deinit */
    error = USB_Framework_Deinit(handle,class_object_ptr->usb_fw_handle);
    /* Unregister all the services */ 
    if(error == USB_OK)
    error = _usb_device_unregister_service(handle,USB_SERVICE_BUS_RESET);
    if(error == USB_OK)
    error = _usb_device_unregister_service(handle,USB_SERVICE_SOF);
    if(error == USB_OK)
    error = _usb_device_unregister_service(handle,USB_SERVICE_SLEEP);
    if(error == USB_OK)
    error = _usb_device_unregister_service(handle,USB_SERVICE_RESUME);
    if(error == USB_OK)
    error = _usb_device_unregister_service(handle,USB_SERVICE_STALL);  
    if(error == USB_OK)
    error = _usb_device_unregister_service(handle,USB_SERVICE_ERROR);
    if(error == USB_OK)
    /* set the device state as USB_STATE_DEFAULT */
    error = _usb_device_set_status(handle,
		     (uint_8)USB_STATUS_DEVICE_STATE, 
		     (uint_16)USB_STATE_DEFAULT);
    if(error == USB_OK)
    /* free the USB class handle */
    error = USB_Class_Free_Handle(class_handle); 
    return error;     
}

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
) 
{
    uint_8 error = USB_OK;
    uint_16 device_state, state;
    USB_CLASS_OBJECT_STRUCT_PTR class_object_ptr;   
    
    class_object_ptr = Get_Usb_Class_Object_Ptr(handle); 
    if (NULL == class_object_ptr) 
        return USBERR_ERROR;

    (void)_usb_device_get_status(class_object_ptr->controller_handle, 
        (uint_8)USB_STATUS_DEVICE_STATE, &device_state);
    
    (void)_usb_device_get_status(class_object_ptr->controller_handle, 
        (uint_8)USB_STATUS_DEVICE, &state);
    
    
    if((device_state == USB_STATE_SUSPEND) 
        && (state & REMOTE_WAKEUP_STATUS_MASK) 
        && (USB_Framework_Remote_wakeup(class_object_ptr->usb_fw_handle) == TRUE))
    {        
        _int_disable();       
        /* Resume the bus */         
        (void)_usb_device_assert_resume(class_object_ptr->controller_handle);           
        
        device_state = USB_STATE_CONFIG; 
        /* Set the device state in the Device Layer to DEFAULT */
        (void)_usb_device_set_status(class_object_ptr->controller_handle,
            (uint_8)USB_STATUS_DEVICE_STATE,
            (uint_16)USB_STATE_CONFIG);                      
        _int_enable();
    }
    if(device_state != USB_STATE_SUSPEND) 
    {    /* if not suspended */ 
    
         error = _usb_device_send_data(class_object_ptr->controller_handle,
            ep_num,buff_ptr,size);
    }

    return error;      
}
 
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
void USB_Class_Periodic_Task(void) 
{
    #ifdef DELAYED_PROCESSING  
        USB_Framework_Periodic_Task();
    #endif  
}
/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Desc
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Class_Get_Desc(USB_CLASS_HANDLE handle,/*[IN]*/
int_32 cmd,/*[IN]*/
uint_8 input_data,/*[IN]*/
uint_8_ptr *in_buf /*[OUT]*/
)
{
   USB_CLASS_OBJECT_STRUCT_PTR class_obj_ptr;
    
   if ((handle < 0)||(handle > USB_MAX_CLASS_OBJECT))
      return USBERR_ERROR;
   
   class_obj_ptr = Get_Usb_Class_Object_Ptr(handle);
   if (class_obj_ptr == NULL)
        return USBERR_ERROR;
   
   return USB_Framework_GetDesc(class_obj_ptr->usb_fw_handle,cmd,input_data,in_buf);
            
}
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Desc
 *
 * @brief  This function is called in to Set the discriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Class_Set_Desc(USB_CLASS_HANDLE handle,/*[IN]*/
int_32 cmd,/*[IN]*/
uint_8 input_data,/*[IN]*/
uint_8_ptr *out_buf /*[IN]*/
)
{
   USB_CLASS_OBJECT_STRUCT_PTR class_obj_ptr;
    
   if ((handle < 0)||(handle > USB_MAX_CLASS_OBJECT))
      return USBERR_ERROR;
   
   class_obj_ptr = Get_Usb_Class_Object_Ptr(handle);
   if (class_obj_ptr == NULL)
        return USBERR_ERROR;
   
   return USB_Framework_SetDesc(class_obj_ptr->usb_fw_handle,cmd,input_data,out_buf);
            
}

/* EOF */

