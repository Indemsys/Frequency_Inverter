/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
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
* $FileName: usb_audio.c$
* $Version : 3.8.6.0$
* $Date    : Sep-17-2012$
*
* Comments:
*
* @brief The file contains USB stack AUDIO layer implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_stack_config.h"

#if AUDIO_CONFIG
 
#include "usb_audio.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/


/****************************************************************************
 * Global Variables
 ****************************************************************************/
 AUDIO_DEVICE_STRUCT    *device_array[MAX_AUDIO_DEVICE];          
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static uint_8 USB_Audio_Other_Requests(USB_SETUP_STRUCT * setup_packet, 
                          uint_8_ptr *data, 
                          uint_32 *size,pointer arg);
static void USB_Class_Audio_Event(uint_8 event, void* val,pointer arg);
static AUDIO_HANDLE  USB_Audio_Get_Free_Handle(void);
static uint_32 USB_Audio_Free_Handle(AUDIO_HANDLE handle);
static void USB_Service_Audio_Status_Interrupt(PTR_USB_EVENT_STRUCT event,pointer arg);
static void USB_Service_Audio_Isochronous_OUT(PTR_USB_EVENT_STRUCT event,pointer arg);
static void USB_Service_Audio_Isochronous_IN(PTR_USB_EVENT_STRUCT event,pointer arg);

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/
 
 
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
 /*************************************************************************//*!
 *
 * @name  	USB_Audio_Get_Free_Handle
 *
 * @brief 	The funtion reserves entry in device array and returns the index.
 *
 * @param 	none.
 * @return 	returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static AUDIO_HANDLE  USB_Audio_Get_Free_Handle()
{
    uint_32 cnt = 0;
   	for (;cnt< MAX_AUDIO_DEVICE;cnt++)
   	{
   	   if (device_array[cnt] == NULL)
   	   	return cnt;
   	}
   	return USBERR_DEVICE_BUSY;
}
 /*************************************************************************//*!
 *
 * @name  USB_Audio_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/

static uint_32 USB_Audio_Free_Handle(AUDIO_HANDLE handle)
{
	if (handle > MAX_AUDIO_DEVICE)
		return USBERR_ERROR;
	
	USB_mem_free((void *)device_array[handle]);
	device_array[handle] = NULL;
	return USB_OK;
}
 
/**************************************************************************//*!
 *
 * @name  USB_Service_Audio_Status_Interrupt
 *
 * @brief The funtion is callback function of Audio Status Interrupt endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
static void USB_Service_Audio_Status_Interrupt(PTR_USB_EVENT_STRUCT event,pointer arg)
{
	AUDIO_DEVICE_STRUCT_PTR  audio_obj_ptr;

#if IMPLEMENT_QUEUING
    uint_8 index;
    uint_8 producer, consumer;
    USB_ENDPOINTS *usb_ep_data;
#endif
        
    audio_obj_ptr = (AUDIO_DEVICE_STRUCT_PTR)arg;

#if IMPLEMENT_QUEUING
    usb_ep_data = audio_obj_ptr->usb_ep_data; 

     /* map the endpoint num to the index of the endpoint structure */
    for(index = 0; index < usb_ep_data->count; index++) 
    {
        if(usb_ep_data->ep[index].ep_num == event->ep_num)
            break;
    }
                                               
    producer = audio_obj_ptr->ep[index].bin_producer;    
        
    /* if there are no errors de-queue the queue and decrement the no. of 
     transfers left, else send the same data again */
     /* de-queue if the send is complete with no error */
     consumer = audio_obj_ptr->ep[index].bin_consumer;
    if(consumer == (AUDIO_MAX_QUEUE_ELEMS -1))
    {
    	audio_obj_ptr->ep[index].bin_consumer = 0;
    }
    else
    {
    	audio_obj_ptr->ep[index].bin_consumer++;
    }
     
    audio_obj_ptr->ep[index].queue_num--;
          
    if(0 != audio_obj_ptr->ep[index].queue_num) 
    {/*if bin is not empty */
        
        USB_CLASS_AUDIO_QUEUE queue;    
        /* send the next packet in queue */
        consumer = audio_obj_ptr->ep[index].bin_consumer;
        queue = audio_obj_ptr->ep[index].queue[consumer];
        (void)USB_Class_Send_Data(audio_obj_ptr->class_handle, queue.channel, 
                                      queue.app_data.data_ptr, queue.app_data.data_size);                                                                                                
    }        
#endif

    /* Callback to application */
    if (audio_obj_ptr->audio_class_callback.callback != NULL)
    {
	    /* notify the app of the send complete */
	    audio_obj_ptr->audio_class_callback.callback(USB_APP_SEND_COMPLETE, 0,
	    audio_obj_ptr->audio_class_callback.arg); 
    }
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Audio_Isochronous_IN
 *
 * @brief The funtion ic callback function of Audio Isochronous IN endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
static void USB_Service_Audio_Isochronous_IN(PTR_USB_EVENT_STRUCT event,pointer arg)
{

    APP_DATA_STRUCT iso_in_recv;
    uint_8 event_type;
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr = (AUDIO_DEVICE_STRUCT_PTR)arg;
    
	#if IMPLEMENT_QUEUING
	    uint_8 index;
	    uint_8 producer, consumer;
	    USB_ENDPOINTS *usb_ep_data = audio_obj_ptr->usb_ep_data;
	    
	    USB_CLASS_AUDIO_QUEUE queue;

	    /* map the endpoint num to the index of the endpoint structure */
	    for(index = 0; index < usb_ep_data->count; index++) 
	    {
	        if(usb_ep_data->ep[index].ep_num ==event->ep_num)
	            break;
	    }
	     
	    producer = audio_obj_ptr->ep[index].bin_producer;	        
	    /* if there are no errors de-queue the queue and decrement the no. of 
	       transfers left, else send the same data again */
	    consumer = audio_obj_ptr->ep[index].bin_consumer;
	    if((AUDIO_MAX_QUEUE_ELEMS-1) == consumer)
	    {
	    	audio_obj_ptr->ep[index].bin_consumer = 0;
	    }
	    else
	    {
	    	audio_obj_ptr->ep[index].bin_consumer++;
	    }
	    consumer = audio_obj_ptr->ep[index].bin_consumer;
	    audio_obj_ptr->ep[index].queue_num--;
	        
	    if(0 != audio_obj_ptr->ep[index].queue_num) 
	    {/*if bin is not empty */
	                        
	        queue = audio_obj_ptr->ep[index].queue[consumer];
	                        
	        (void)USB_Class_Send_Data(audio_obj_ptr->class_handle, queue.channel, 
	        	queue.app_data.data_ptr, queue.app_data.data_size);
	    }          
	#endif
    
    /* Callback to application */
    if(audio_obj_ptr->param_callback.callback != NULL) 
    {
        event_type = USB_APP_SEND_COMPLETE;
        iso_in_recv.data_ptr = event->buffer_ptr;
        iso_in_recv.data_size = event->len;
        audio_obj_ptr->param_callback.callback(event_type,
                (void*)&iso_in_recv,audio_obj_ptr->param_callback.arg);             
    }
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Audio_Isochronous_OUT
 *
 * @brief The funtion is callback function of Isochronous Out Endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
static void USB_Service_Audio_Isochronous_OUT(PTR_USB_EVENT_STRUCT event,pointer arg)
{   
    APP_DATA_STRUCT iso_out_recv;
    uint_8 event_type; 
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr = (AUDIO_DEVICE_STRUCT_PTR)arg;
    event_type = USB_APP_DATA_RECEIVED; 
    iso_out_recv.data_ptr = event->buffer_ptr;
    iso_out_recv.data_size = event->len; 
  	
  	/* Callback to application */
    if(audio_obj_ptr->param_callback.callback != NULL) 
    {
      audio_obj_ptr->param_callback.callback(event_type,
      (void*)&iso_out_recv,audio_obj_ptr->param_callback.arg);             
    }
 }


/**************************************************************************//*!
 *
 * @name  USB_Class_Audio_Event
 *
 * @brief The funtion initializes Audio endpoint 
 *
 * @param handle          handle to Identify the controller
 * @param event           pointer to event structure
 * @param val             gives the configuration value 
 *
 * @return None       
 *
 *****************************************************************************/
static void USB_Class_Audio_Event(uint_8 event, void* val,pointer arg) 
{  
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr = (AUDIO_DEVICE_STRUCT_PTR)arg;
    
    if (audio_obj_ptr == NULL)
    {
        #ifdef _DEV_DEBUG
            printf("USB_Class_Audio_Event:audio_obj_ptr is NULL\n");
        #endif  
        return;
    }
    
    /* if enum is complete initialize non-control endpoints */
    if(event == USB_APP_ENUM_COMPLETE) 
    {
        uint_8 count = 0;
        USB_EP_STRUCT_PTR ep_struct_ptr;
        
        /* get the endpoints from the descriptor module */            
        USB_ENDPOINTS *usb_ep_data = audio_obj_ptr->usb_ep_data;
        
        /* intialize all non control endpoints */            
        while(count < usb_ep_data->count) 
        {
            ep_struct_ptr = (USB_EP_STRUCT*)&usb_ep_data->ep[count];

            (void)_usb_device_init_endpoint(audio_obj_ptr->controller_handle,
             ep_struct_ptr,TRUE);
  
            /* register callback service for Non Control EndPoints */
            switch(ep_struct_ptr->type) 
            {
              	case USB_INTERRUPT_PIPE :
                  	(void)_usb_device_register_service(audio_obj_ptr->controller_handle,
                    	(uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                        USB_Service_Audio_Status_Interrupt,(void *)audio_obj_ptr);
                  	break;                              
              	case USB_ISOCHRONOUS_PIPE :
                  	if(ep_struct_ptr->direction == USB_RECV) 
                  	{
                      	(void)_usb_device_register_service(audio_obj_ptr->controller_handle,
                        	(uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                            USB_Service_Audio_Isochronous_OUT,(void *)audio_obj_ptr);
                  	} 
                  	else
                  	{
                      	(void)_usb_device_register_service(audio_obj_ptr->controller_handle,
                        	(uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                            USB_Service_Audio_Isochronous_IN,(void *)audio_obj_ptr);
                  	}
                  	break;
              	default : break;        
            }
                          
            /* set the EndPoint Status as Idle in the device layer */
            /* (no need to specify direction for this case) */
			(void)_usb_device_set_status(audio_obj_ptr->controller_handle,
            	(uint_8)(USB_STATUS_ENDPOINT|ep_struct_ptr->ep_num),
                (uint_8)USB_STATUS_IDLE);                                                                                                         
            count++;                                                    
        }/* EndWhile */
    }
    if(audio_obj_ptr->audio_class_callback.callback != NULL) 
    {
        audio_obj_ptr->audio_class_callback.callback(event,
        	val,audio_obj_ptr->audio_class_callback.arg);
    } 
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Cur_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Get_Cur_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{	
	uint_8 error;
	int_32 cmd = -1;
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   }
	error = USBERR_INVALID_REQ_TYPE;
	switch(setup_packet->value>>8)
	{
      case MUTE_CONTROL:
      	*size = 1;
      	cmd = GET_CUR_MUTE_CONTROL;
        break;
      case VOLUME_CONTROL:
      	*size = 2;
      	cmd = GET_CUR_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	*size = 1;
      	cmd = GET_CUR_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	*size = 1;
      	cmd = GET_CUR_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	*size = 1;
      	cmd = GET_CUR_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	*size = 5;
      	cmd = GET_CUR_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case AUTOMATIC_GAIN_CONTROL:
      	*size = 1;
      	cmd = GET_CUR_AUTOMATIC_GAIN_CONTROL;
        break;
      case DELAY_CONTROL:
      	*size = 2;
      	cmd = GET_CUR_DELAY_CONTROL;
        break;
      case BASS_BOOST_CONTROL:
      	*size = 1;
      	cmd = GET_CUR_BASS_BOOST_CONTROL;
        break;
      case LOUDNESS_CONTROL:
      	*size = 1;
      	cmd = GET_CUR_LOUDNESS_CONTROL;
        break;
      default:
    	break;    
   }
   if(cmd > 0)
   {
	  	error = USB_Class_Get_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
   
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Get_Min_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Get_Min_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{	
	uint_8 error;
	int cmd = -1;
    if (size == NULL)				 
    {
   		return USBERR_ERROR;	
    }
	error = USBERR_INVALID_REQ_TYPE;
	switch(setup_packet->value>>8)
	{
      case VOLUME_CONTROL:
      	*size = 2;
      	cmd = GET_MIN_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	*size = 1;
      	cmd = GET_MIN_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	*size = 1;
      	cmd = GET_MIN_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	*size = 1;
      	cmd = GET_MIN_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	*size = 5;
      	cmd = GET_MIN_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case DELAY_CONTROL:
      	*size = 2;
      	cmd = GET_MIN_DELAY_CONTROL;
        break;
      default:
    	break;    
   }
   if(cmd > 0)
   {
	  	error = USB_Class_Get_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Get_Max_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Get_Max_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{	
	uint_8 error;
	int_32 cmd = -1;
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   }	
	error = USBERR_INVALID_REQ_TYPE;
	switch(setup_packet->value>>8)
	{
      case VOLUME_CONTROL:
      	*size = 2;
      	cmd = GET_MAX_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	*size = 1;
      	cmd = GET_MAX_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	*size = 1;
      	cmd = GET_MAX_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	*size = 1;
      	cmd = GET_MAX_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	*size = 5;
      	cmd = GET_MAX_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case DELAY_CONTROL:
      	*size = 2;
      	cmd = GET_MAX_DELAY_CONTROL;
        break;
      default:
    	break;    
   }
   if(cmd > 0)
   {
	  	error = USB_Class_Get_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Get_Res_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Get_Res_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{	
	uint_8 error;
	int_32 cmd = -1;
	error = USBERR_INVALID_REQ_TYPE;
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   }
	switch(setup_packet->value>>8)
	{
      case VOLUME_CONTROL:
      	*size = 2;
      	cmd = GET_RES_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	*size = 1;
      	cmd = GET_RES_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	*size = 1;
      	cmd = GET_RES_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	*size = 1;
      	cmd = GET_RES_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	*size = 5;
      	cmd = GET_RES_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case DELAY_CONTROL:
      	*size = 2;
      	cmd = GET_RES_DELAY_CONTROL;
        break;
      default:
    	break;    
   }
   if(cmd > 0)
   {
	  	error = USB_Class_Get_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Audio_Get_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Audio_Get_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
   uint_8 error = USBERR_INVALID_REQ_TYPE;
    /* Select SET request Control Feature Unit Module */
   switch(setup_packet->request)
   {
     case GET_CUR:
       error = USB_Get_Cur_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);
       break;
     case GET_MIN:
       error = USB_Get_Min_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);
       break;
     case GET_MAX:
       error = USB_Get_Max_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);                         
       break;
     case GET_RES:
       error = USB_Get_Res_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);                         
       break;
     default:
       break;
   }
 return error;  
}

/**************************************************************************//*!
 *
 * @name  USB_Audio_Get_Control_Terminal
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Audio_Get_Control_Terminal
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    uint_8 error = USBERR_INVALID_REQ_TYPE;
    if (size == NULL)
    {
    	return USBERR_ERROR;	
    }
    switch(setup_packet -> request)
    {
      /* Copy Protect Control only supports the CUR attribute!*/
      case GET_CUR:
      	*size = 1;
        if((setup_packet->value >>8) == COPY_PROTECT_CONTROL )
        {
          error = USB_Class_Get_Desc(audio_obj_ptr->class_handle,GET_CUR_COPY_PROTECT_CONTROL,
          								audio_obj_ptr->current_interface,data);
        }
        break;
      default:
        break;
    }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Get_Request_Interface
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Get_Request_Interface
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
  int i;
  uint_8 error = USBERR_INVALID_REQ_TYPE;
  USB_AUDIO_UNITS *entity_desc_data;
  USB_UT_STRUCT_PTR ut_struct;
  /* get the I/O Interface and Feature from the descriptor module */
  entity_desc_data = audio_obj_ptr->usb_ut_data;
  for(i=0;i<entity_desc_data->count; i++)
  {
    ut_struct=(&entity_desc_data->epp[i]);
    if((setup_packet->index >>8) == ut_struct->unit_id)
    {
      switch(ut_struct->type)
      {
        case AUDIO_CONTROL_INPUT_TERMINAL:
          error = USB_Audio_Get_Control_Terminal(audio_obj_ptr,setup_packet,data,size);
          break;
        case AUDIO_CONTROL_FEATURE_UNIT:
          error = USB_Audio_Get_Feature_Unit(audio_obj_ptr,setup_packet,data,size);
          break;
        default:
          break;
      }
    }
  }
  return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Set_Cur_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Set_Cur_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
   int cmd = -1;
   uint_8 error = USBERR_INVALID_REQ_TYPE;
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   }
   *size = 0;
   switch(setup_packet->value>>8)
   {
      case MUTE_CONTROL:
      	cmd = SET_CUR_MUTE_CONTROL;
        break;
      case VOLUME_CONTROL:
      	cmd = SET_CUR_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	cmd = SET_CUR_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	cmd = SET_CUR_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	cmd = SET_CUR_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	cmd = SET_CUR_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case AUTOMATIC_GAIN_CONTROL:
      	cmd = SET_CUR_AUTOMATIC_GAIN_CONTROL;
        break;
      case DELAY_CONTROL:
      	cmd = SET_CUR_DELAY_CONTROL;
        break;
      case BASS_BOOST_CONTROL:
      	cmd = SET_CUR_BASS_BOOST_CONTROL;
        break;
      case LOUDNESS_CONTROL:
      	cmd = SET_CUR_LOUDNESS_CONTROL;
        break;
      default:
    	break;    
   }
   if (cmd > 0)
   {
	  	error = USB_Class_Set_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Set_Min_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Set_Min_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
   uint_8 error = USBERR_INVALID_REQ_TYPE; 
   int cmd = -1;
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   }
   *size = 0;
   switch(setup_packet->value>>8)
   {
      case VOLUME_CONTROL:
      	cmd = SET_MIN_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	cmd = SET_MIN_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	cmd = SET_MIN_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	cmd = SET_MIN_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	cmd = SET_MIN_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case DELAY_CONTROL:
      	cmd = SET_MIN_DELAY_CONTROL;
        break;
      default:
    	break;    
   }
   if (cmd > 0)
   {
	  	error = USB_Class_Set_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Set_Max_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Set_Max_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
   int cmd = -1;
   uint_8 error = USBERR_INVALID_REQ_TYPE; 
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   }
   *size = 0;
   switch(setup_packet->value>>8)
   {
      case VOLUME_CONTROL:
      	cmd = SET_MAX_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	cmd = SET_MAX_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	cmd = SET_MAX_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	cmd = SET_MAX_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	cmd = SET_MAX_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case DELAY_CONTROL:
      	cmd = SET_MAX_DELAY_CONTROL;
        break;
      default:
    	break;    
   }
   if (cmd > 0)
   {
	  	error = USB_Class_Set_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Set_Res_Audio_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Set_Res_Audio_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
   int cmd = -1;
   uint_8 error = USBERR_INVALID_REQ_TYPE;
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   } 
   *size = 0;
   switch(setup_packet->value>>8)
   {
      case VOLUME_CONTROL:
      	cmd = SET_RES_VOLUME_CONTROL;
        break;
      case BASS_CONTROL:
      	cmd = SET_RES_BASS_CONTROL;
        break;
      case MID_CONTROL:
      	cmd = SET_RES_MID_CONTROL;
        break;
      case TREBLE_CONTROL:
      	cmd = SET_RES_TREBLE_CONTROL;
        break;
      case GRAPHIC_EQUALIZER_CONTROL:
      	cmd = SET_RES_GRAPHIC_EQUALIZER_CONTROL;
        break;
      case DELAY_CONTROL:
      	cmd = SET_RES_DELAY_CONTROL;
        break;
      default:
    	break;    
   }
   if (cmd > 0)
   {
	  	error = USB_Class_Set_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
	return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Audio_Set_Feature_Unit
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Audio_Set_Feature_Unit
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
   uint_8 error = USBERR_INVALID_REQ_TYPE;
    /* Select SET request Control Feature Unit Module */
   switch(setup_packet->request)
   {
     case SET_CUR:
       error = USB_Set_Cur_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);
       break;
     case SET_MIN:
       error = USB_Set_Min_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);
       break;
     case SET_MAX:
       error = USB_Set_Max_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);                         
       break;
     case SET_RES:
       error = USB_Set_Res_Audio_Feature_Unit(audio_obj_ptr,setup_packet,data,size);                         
       break;
     default:
       break;
   }
 return error;  
}


/**************************************************************************//*!
 *
 * @name  USB_Audio_Set_Control_Terminal
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Audio_Set_Control_Terminal
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    uint_8 error = USBERR_INVALID_REQ_TYPE;
    if(size == NULL)
    {
    	return USBERR_ERROR;	
    }
    *size = 0;
    switch(setup_packet -> request)
    {
      /* Copy Protect Control only supports the CUR attribute!*/
      case SET_CUR:
        if((setup_packet->value >>8) == COPY_PROTECT_CONTROL )
        {
          error = USB_Class_Set_Desc(audio_obj_ptr->class_handle,SET_CUR_COPY_PROTECT_CONTROL,
          							  audio_obj_ptr->current_interface,data);
        }
        break;
      default:
        break;
    }
	return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Request_Interface
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Set_Request_Interface
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
  int i;
  uint_8 error = USBERR_INVALID_REQ_TYPE;
  USB_AUDIO_UNITS *entity_desc_data;
  USB_UT_STRUCT_PTR ut_struct;
  /* get the I/O Interface and Feature  from the descriptor module */
  entity_desc_data = audio_obj_ptr->usb_ut_data;
  for(i=0;i<entity_desc_data->count; i++)
  {
    ut_struct=(&entity_desc_data->epp[i]);
    if((setup_packet->index >>8) == ut_struct->unit_id)
    {
      switch(ut_struct->type)
      {
      	case AUDIO_CONTROL_OUTPUT_TERMINAL:
          /*Select SET Request Control Input Terminal Module */
          error = USB_Audio_Set_Control_Terminal(audio_obj_ptr,setup_packet,data,size);
          break;
        case AUDIO_CONTROL_FEATURE_UNIT:
          /* Select SET request Control Feature Unit Module */
          error = USB_Audio_Set_Feature_Unit(audio_obj_ptr,setup_packet,data,size);
          break;
        default:
          break;
      }
    }
  }
  return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Set_Request_Endpoint
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Set_Request_Endpoint
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    int cmd = -1;
    uint_8 error = USBERR_INVALID_REQ_TYPE;
   if (size == NULL)				 
   {
   		return USBERR_ERROR;	
   }
    *size = 0;
    switch(setup_packet->request)
    {
      case  SET_CUR:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	cmd = SET_CUR_SAMPLING_FREQ_CONTROL;
            break;
          case PITCH_CONTROL:
          	cmd = SET_CUR_PITCH_CONTROL;
            break;
          default:
          	break;    
        }
      	break;
      case SET_MIN:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	cmd = SET_MIN_SAMPLING_FREQ_CONTROL;
            break;
          default:
          	break;    
        }
      	break;
      case SET_MAX:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	cmd = SET_MAX_SAMPLING_FREQ_CONTROL;
            break;
          default:
          	break;    
        }
      break;
      case SET_RES:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	cmd = SET_RES_SAMPLING_FREQ_CONTROL;
            break;
          default:
          	break;    
        }
       break;
      case SET_MEM:
      	*size = setup_packet->length;
      	if (audio_obj_ptr->mem_param_callback.callback != NULL)
      	{
      		error = audio_obj_ptr->mem_param_callback.callback(
                                      /* request type */ 
                                      setup_packet->request,
                                      setup_packet->value, 
                                      /* pointer to the data */      
                                      data,
                                      /* size of the transfer */                                    
                                      size,audio_obj_ptr->mem_param_callback.arg);	
      	}
      break;
    default:
    	break;
    }
    if (cmd > 0)
    {
        error = USB_Class_Set_Desc(audio_obj_ptr->class_handle,cmd,
        							audio_obj_ptr->current_interface,data);
    }
	return error;    
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Request_Endpoint
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param audio_obj_ptr:	Pointer to Audio class object.
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
static uint_8 USB_Get_Request_Endpoint
(
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data, 
    uint_32 *size
)
{
    int cmd = -1;
    uint_8 error = USBERR_INVALID_REQ_TYPE;
    if (size == NULL)				 
    {
   		return USBERR_ERROR;	
    }
    switch(setup_packet->request)
    {
      case  GET_CUR:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	*size = 3;
          	cmd = GET_CUR_SAMPLING_FREQ_CONTROL;
            break;
          case PITCH_CONTROL:
          	*size = 1;
          	cmd = GET_CUR_PITCH_CONTROL;
            break;
          default:
          	break;    
        }
      break;
      case GET_MIN:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	*size = 3;
          	cmd = GET_MIN_SAMPLING_FREQ_CONTROL;
            break;
          default:
          	break;    
        }
      break;
      case GET_MAX:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	*size = 3;
          	cmd = GET_MAX_SAMPLING_FREQ_CONTROL;
            break;
          default:
          	break;    
        }
      break;
      case GET_RES:
        switch(setup_packet->value>>8)
        {
          case SAMPLING_FREQ_CONTROL:
          	cmd = GET_RES_SAMPLING_FREQ_CONTROL;
          	*size = 3;
            break;
          default:
          	break;    
        }
       break;
      case GET_MEM:
      	*size = setup_packet->length;
      	if (audio_obj_ptr->mem_param_callback.callback != NULL)
      	{
      		error = audio_obj_ptr->mem_param_callback.callback(
                                      /* request type */ 
                                      setup_packet->request,
                                      setup_packet->value, 
                                      /* pointer to the data */      
                                      data,
                                      /* size of the transfer */                                    
                                      size,audio_obj_ptr->mem_param_callback.arg);	
      	}
	   break;
      default:
   	   break;
    }
   if (cmd > 0)
   {
	  	error = USB_Class_Get_Desc(audio_obj_ptr->class_handle,cmd,
	  								audio_obj_ptr->current_interface,data);
   }
   return error;    
}
/**************************************************************************//*!
 *
 * @name  USB_Audio_Other_Requests
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param handle
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return error:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_Audio_Other_Requests
(
	USB_SETUP_STRUCT * setup_packet, 
	uint_8_ptr *data, 
	uint_32 *size,
	pointer arg
) 
{
    uint_8 error;
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr = (AUDIO_DEVICE_STRUCT_PTR)arg;
    error = USBERR_INVALID_REQ_TYPE;
    *data = ((uint_8*)setup_packet)+USB_SETUP_PKT_SIZE;
    
    if (audio_obj_ptr == NULL)
    {
        #ifdef _DEV_DEBUG
            printf("USB_Audio_Other_Requests:audio_obj_ptr is NULL\n");
        #endif  
        return error;
    }
    
    if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_CLASS) 
    {  /* class request so handle it here */
    	audio_obj_ptr->current_interface = (uint_8)setup_packet->index;
    	switch(setup_packet->request_type)     
    	{
           /* Set I/O Interface and Feature Unit */
           case SET_REQUEST_ITF:
             error = USB_Set_Request_Interface(audio_obj_ptr,setup_packet,data,size);
             break; 
           /*Get I/O Interface and Feature Unit */ 
           case GET_REQUEST_ITF:
             error = USB_Get_Request_Interface(audio_obj_ptr,setup_packet,data,size);
             break;
           /* Set Requests of Endpoint */
           case SET_REQUEST_EP:
             error = USB_Set_Request_Endpoint(audio_obj_ptr,setup_packet,data,size);
             break;
           /* Get Requests of Endpoint */
           case GET_REQUEST_EP:
             error = USB_Get_Request_Endpoint(audio_obj_ptr,setup_packet,data,size);
             break;
           default:
             *size = 0;
             break;	
    	} /* End of switch */
    }
    else if ((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_VENDOR)
    {   /* vendor specific request  */    
        if(audio_obj_ptr->vendor_req_callback.callback != NULL) 
        {
            error = audio_obj_ptr->vendor_req_callback.callback(setup_packet,
            data,size,audio_obj_ptr->vendor_req_callback.arg);
        }
    }
    return error;
}

/*****************************************************************************
 * Global Functions
 *****************************************************************************/


/**************************************************************************//*!
 *
 * @name  USB_Class_Audio_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param *audio_config_ptr[IN]:  This structure contains configuration parameter
 *                              send by APP to configure audio class.
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the Audio Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
uint_32 USB_Class_Audio_Init
(
   AUDIO_CONFIG_STRUCT_PTR audio_config_ptr                               
) 
{
    uint_8 index;
    uint_8 error;
    AUDIO_HANDLE           audioHandle;
    AUDIO_DEVICE_STRUCT_PTR  devicePtr = NULL;
    USB_ENDPOINTS *usb_ep_data;
    USB_AUDIO_UNITS *usb_ut_data;
    
    if (NULL == audio_config_ptr)
    {
    	return USBERR_ERROR;	
    }
    
    
    devicePtr = (AUDIO_DEVICE_STRUCT_PTR)USB_mem_alloc_zero(sizeof(AUDIO_DEVICE_STRUCT));
    if (NULL == devicePtr)
    {
    	#ifdef _DEV_DEBUG
    		printf("USB_Class_Audio_Init: Memalloc failed\n");
    	#endif	
    	return USBERR_ALLOC;
    }

    audioHandle = USB_Audio_Get_Free_Handle();
    if (USBERR_DEVICE_BUSY == audioHandle) 
    {
        USB_mem_free(devicePtr);
        devicePtr = NULL;
     	return USBERR_INIT_FAILED;
    }

    devicePtr->ep = audio_config_ptr->ep;
    usb_ep_data = audio_config_ptr->usb_ep_data;
    usb_ut_data = audio_config_ptr->usb_ut_data;
        
 	/* Initialize the device layer*/
    error = _usb_device_init(USBCFG_DEFAULT_DEVICE_CONTROLLER, &devicePtr->controller_handle, usb_ep_data->count + 1);
    if(error != USB_OK)
    {
	     USB_Audio_Free_Handle(audioHandle);
	     USB_mem_free(devicePtr);
	     devicePtr = NULL;
	  	 return error;
    }

   audio_config_ptr->desc_callback_ptr->handle = audioHandle;
   /* Initialize the generic class functions */
   devicePtr->class_handle = USB_Class_Init(devicePtr->controller_handle,
    USB_Class_Audio_Event,USB_Audio_Other_Requests,(void *)devicePtr,
    audio_config_ptr->desc_callback_ptr);
   if(error != USB_OK)
   {
		USB_Audio_Free_Handle(audioHandle);
		USB_mem_free(devicePtr);
		devicePtr = NULL;
		return error;
   }
	                                                    
   devicePtr->usb_ep_data =  usb_ep_data;
   devicePtr->usb_ut_data =  usb_ut_data;
   	    
   for(index = 0; index < usb_ep_data->count; index++)
   {            
      devicePtr->ep[index].endpoint = usb_ep_data->ep[index].ep_num;
      devicePtr->ep[index].type = usb_ep_data->ep[index].type;
      devicePtr->ep[index].bin_consumer = 0x00;
      devicePtr->ep[index].bin_producer = 0x00;
      devicePtr->ep[index].queue_num = 0x00;
    }
	
	audio_config_ptr->desc_callback_ptr->handle = audioHandle;
	if (NULL == audio_config_ptr->desc_callback_ptr)
	{
		USB_Audio_Free_Handle(audioHandle);
		USB_mem_free(devicePtr);
		devicePtr = NULL;
		return error;
   }
	    
    /* save the callback pointer */
    devicePtr->audio_class_callback.callback = audio_config_ptr->audio_class_callback.callback;
    devicePtr->audio_class_callback.arg = audio_config_ptr->audio_class_callback.arg;         
    /* save the callback pointer */
    devicePtr->mem_param_callback.callback = audio_config_ptr->mem_param_callback.callback;
    devicePtr->mem_param_callback.arg = audio_config_ptr->mem_param_callback.arg;         
    /* save the callback pointer */
    devicePtr->vendor_req_callback.callback = audio_config_ptr->vendor_req_callback.callback;
    devicePtr->vendor_req_callback.arg = audio_config_ptr->vendor_req_callback.arg; 
    /* save the callback pointer */
    devicePtr->param_callback.callback = audio_config_ptr->param_callback.callback;
    devicePtr->param_callback.arg = audio_config_ptr->param_callback.arg; 	       	        
    
    devicePtr->audio_handle = audioHandle;
  
    device_array[audioHandle] = devicePtr;
  
  return audioHandle;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Audio_Send_Data
 *
 * @brief 
 *
 * @param handle          :   handle returned by USB_Class_HID_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
uint_8 USB_Class_Audio_Send_Data(
                                AUDIO_HANDLE handle,/*[IN]*/
                                uint_8 ep_num,/*[IN]*/
                                uint_8_ptr app_buff,/*[IN]*/
                                uint_32 size /*[IN]*/
                               ) 
{
#if IMPLEMENT_QUEUING
    uint_8 index;
    uint_8 producer, consumer,queue_num;
    USB_ENDPOINTS *ep_desc_data;
#endif
    
    uint_8 error = USB_OK;
    AUDIO_DEVICE_STRUCT_PTR  devicePtr;
    PTR_USB_CLASS_AUDIO_QUEUE queue_tmp;  
    
    /* get device ptr */
    if (handle > MAX_AUDIO_DEVICE)
    {
    	return USBERR_NO_DEVICE_CLASS;
    }
    else 
    {
    	devicePtr = device_array[handle];	
    }
#if IMPLEMENT_QUEUING
    ep_desc_data = devicePtr->usb_ep_data; 
    
    /* map the endpoint num to the index of the endpoint structure */
    for(index = 0; index < ep_desc_data->count; index++) 
    {
        if(ep_desc_data->ep[index].ep_num == ep_num)
            break;
    }
   
    producer = devicePtr->ep[index].bin_producer;
    consumer = devicePtr->ep[index].bin_consumer;
    queue_num = devicePtr->ep[index].queue_num;

    if(queue_num != AUDIO_MAX_QUEUE_ELEMS)
    {/* the bin is not full*/
        /* put all send request parameters in the endpoint data structure */
        queue_tmp = &(devicePtr->ep[index].queue[producer]);
        queue_tmp->channel = ep_num;
        queue_tmp->app_data.data_ptr = app_buff;
        queue_tmp->app_data.data_size = size; 
        queue_tmp->handle = devicePtr->controller_handle;
   
        /* increment producer bin by 1*/
        if (producer == (AUDIO_MAX_QUEUE_ELEMS -1))
        {
        	devicePtr->ep[index].bin_producer = 0;	
        }
        else
        {
        	devicePtr->ep[index].bin_producer++;
        }
        devicePtr->ep[index].queue_num++;
             
        if(devicePtr->ep[index].queue_num == 1)         
        { 
#endif  
        	/*send the IO if there is only one element in the queue */          
            error = USB_Class_Send_Data(devicePtr->class_handle, ep_num, app_buff,size);
#if IMPLEMENT_QUEUING
        }        
    }
    else /* bin is full */
    {
        error = USBERR_DEVICE_BUSY; 
    }
#endif
    
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Audio_Recv_Data
 *
 * @brief This functions receives Data from Host.
 *
 * @param handle          :   handle returned by USB_Class_Audio_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
uint_8 USB_Class_Audio_Recv_Data
(
    AUDIO_HANDLE          audio_handle,
    uint_8              ep_num,
    uint_8_ptr          buff_ptr,      /* [IN] buffer to recv */      
    uint_32             size           /* [IN] length of the transfer */
)
{
    AUDIO_DEVICE_STRUCT_PTR audio_obj_ptr;
    uint_8 error = USB_OK;

    if (audio_handle > MAX_AUDIO_DEVICE)
    {
    	return USBERR_ERROR;
    }
    else 
    {
    	audio_obj_ptr = device_array[audio_handle];	
    }
   
    error = _usb_device_recv_data(audio_obj_ptr->controller_handle,
    ep_num,buff_ptr,size);
    
    return error;
	
}

#endif /*AUDIO_CONFIG*/
/* EOF */
