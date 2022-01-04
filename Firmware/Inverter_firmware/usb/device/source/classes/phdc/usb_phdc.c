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
* $FileName: usb_phdc.c$
* $Version : 3.8.7.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
* @brief The file contains USB stack PHDC layer implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
 #include "usb_stack_config.h"
 #if PHDC_CONFIG 
#include "usb_phdc.h"
#include <string.h>
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

/****************************************************************************
 * Global Functions - Prototypes
 ****************************************************************************/
void USB_PHDC_Endpoint_Service(PTR_USB_EVENT_STRUCT event,pointer arg);
/****************************************************************************
 * Global Variables
 ****************************************************************************/
 /* Add all the variables needed for usb_phdc.c to this structure */
 PHDC_STRUCT_PTR  phdc_object_array[MAX_PHDC_OBJECT];

#if USB_METADATA_SUPPORTED
    static char msg_preamble_signature[17] = "PhdcQoSSignature";
    /* string used to give preamble verifiability */
#endif           
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static uint_8 USB_PHDC_Other_Requests(USB_SETUP_STRUCT * setup_packet, 
                                 uint_8_ptr *data, 
                                 uint_32 *size,
                                 pointer arg);
                          
static void USB_Class_PHDC_Event(uint_8 event,
                                 void * value,
                                 pointer arg);
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
  /*************************************************************************//*!
 *
 * @name  USB_Phdc_Allocate_Handle
 *
 * @brief The funtion reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static PHDC_HANDLE  USB_Phdc_Allocate_Handle()
{
    uint_32 cnt = 0;
    for (;cnt < MAX_PHDC_OBJECT;cnt++)
    {
       if (phdc_object_array[cnt] == NULL)
        return cnt;
    }
    return USBERR_DEVICE_BUSY;
}
 /*************************************************************************//*!
 *
 * @name  USB_Phdc_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/
static int_32 USB_Phdc_Free_Handle(PHDC_HANDLE handle)
{
    if (/*(handle < 0) || */(handle > MAX_PHDC_OBJECT))
        return USBERR_ERROR;
    
    USB_mem_free((void *)phdc_object_array[handle]);
    phdc_object_array[handle] = NULL;
    return USB_OK;
}
 /*************************************************************************//*!
 *
 * @name  USB_Phdc_Get_Device_Ptr
 *
 * @brief The funtion gets the device pointer from device array .
 *
 * @param handle  index in device array.
 * @return returns returns pointer to HID device structure..      
 *
 *****************************************************************************/
static PHDC_STRUCT_PTR USB_Phdc_Get_Device_Ptr(PHDC_HANDLE handle)
{
    if (/*(handle < 0) || */(handle > MAX_PHDC_OBJECT))
        return NULL;
    
     return phdc_object_array[handle]; 
}
 /**************************************************************************//*!
 *
 * @name  USB_Class_PHDC_Event
 *
 * @brief Initializes non control endpoints
 *
 * @param handle         
 * @param event          : event notified by the layer below
 * @param value          : additional parameter used by the event
 *
 * @return               : None
 ******************************************************************************
 * Initializes non control endpoints when Enumeration complete event is 
 * recieved. 
 *****************************************************************************/  
static void USB_Class_PHDC_Event(uint_8 event, 
                                 void* val,pointer arg)         
{   
    uint_8 ep_count;    
    USB_EP_STRUCT ep_struct;
    PHDC_STRUCT_PTR phdc_obj_ptr = (PHDC_STRUCT_PTR)arg;
    
    if (phdc_obj_ptr == NULL)
    {
        #if _DEBUG
            printf("USB_Class_PHDC_Event:phdc_object_ptr is NULL\n");
        #endif  
        return;
    }
    /* if enum is complete initialize non-control endpoints */
    if(event == USB_APP_ENUM_COMPLETE) 
    {
        /* initialize all recieve endpoints */
        for(ep_count = 0; ep_count < phdc_obj_ptr->ep_data.count_rx; ep_count++)
        {                         
            uint_8 component = (uint_8)(phdc_obj_ptr->ep_data.ep_rx[ep_count].endpoint | 
                (USB_RECV << COMPONENT_PREPARE_SHIFT));
            
            /* initialize ep_struct */
            ep_struct.ep_num = phdc_obj_ptr->ep_data.ep_rx[ep_count].endpoint;
            ep_struct.size = phdc_obj_ptr->ep_data.ep_rx[ep_count].size;
            ep_struct.type = phdc_obj_ptr->ep_data.ep_rx[ep_count].type;
            ep_struct.direction = USB_RECV;             
            
            /* intialize endpoint */
            (void)_usb_device_init_endpoint(phdc_obj_ptr->controller_handle,
             &ep_struct,TRUE);
            
            phdc_obj_ptr->service_buff_ptr = (uint_8_ptr)USB_mem_alloc_zero(ep_struct.size);
            if (NULL == phdc_obj_ptr->service_buff_ptr)
            {
                #if _DEBUG
                    printf("USB_Class_PHDC_Event: Memalloc failed\n");
                #endif  
                return ;
            }
            
            /* set endpt status as idle in the device layer */
            (void)_usb_device_set_status(phdc_obj_ptr->controller_handle,
                (uint_8)(USB_STATUS_ENDPOINT |component), 
                (uint_16)USB_STATUS_IDLE);
            phdc_obj_ptr->ep_data.ep_rx[ep_count].buff_ptr = NULL;
            phdc_obj_ptr->ep_data.ep_rx[ep_count].buffer_size = 0;
        }
        
        /* initialize all transmit endpoints */
        for(ep_count = 0; ep_count < phdc_obj_ptr->ep_data.count_tx; ep_count++)
        {                
            uint_8 component = (uint_8)(phdc_obj_ptr->ep_data.ep_tx[ep_count].endpoint | 
                (USB_SEND << COMPONENT_PREPARE_SHIFT));

            /* initialize ep_struct */
            ep_struct.ep_num = phdc_obj_ptr->ep_data.ep_tx[ep_count].endpoint;
            ep_struct.size = phdc_obj_ptr->ep_data.ep_tx[ep_count].size;
            ep_struct.type = phdc_obj_ptr->ep_data.ep_tx[ep_count].type;
            ep_struct.direction = USB_SEND;             
            
            /* intialize endpoint */
            (void)_usb_device_init_endpoint(phdc_obj_ptr->controller_handle,
             &ep_struct, TRUE);
            
            /* set endpt status as idle in the device layer */
            (void)_usb_device_set_status(phdc_obj_ptr->controller_handle,
                (uint_8)(USB_STATUS_ENDPOINT |component), 
                (uint_16)USB_STATUS_IDLE);
        }
    }     
    else if(event == USB_APP_BUS_RESET) 
    {            
        /* initialize producer and consumer to zero for transmit endpoints */
        for(ep_count = 0; ep_count < phdc_obj_ptr->ep_data.count_tx; ep_count++)   
        {                                                                                        
            phdc_obj_ptr->ep_data.ep_tx[ep_count].bin_consumer = (uint_8)0x00;         
            phdc_obj_ptr->ep_data.ep_tx[ep_count].bin_producer = (uint_8)0x00;                                                                                              
        }
    }            
    
    if(phdc_obj_ptr->phdc_callback.callback != NULL) 
    {
        phdc_obj_ptr->phdc_callback.callback(event,
            val,phdc_obj_ptr->phdc_callback.arg);
    }   
}

/**************************************************************************//*!
 *
 * @name  USB_PHDC_Other_Requests
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param handle           
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned                   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 * The funtion provides flexibilty to add class and vendor specific requests
 *****************************************************************************/  
static uint_8 USB_PHDC_Other_Requests
(
    USB_SETUP_STRUCT * setup_packet,/* [IN] setup packet recieved */
    uint_8_ptr *data,/* [OUT] data to be send back */
    uint_32 *size,   /* [OUT] size to be returned */
    pointer arg
) 
{
    uint_8 error = USBERR_INVALID_REQ_TYPE;
    PHDC_STRUCT_PTR phdc_obj_ptr = (PHDC_STRUCT_PTR)arg;
    
    if (phdc_obj_ptr == NULL)
    {
        #if _DEBUG
            printf("USB_PHDC_Other_Requests:phdc_object_ptr is NULL\n");
        #endif  
        return error;
    }
    if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_CLASS) 
    {   /*class request so handle it here */        
        *size = 0;
        error = USB_OK;
        
        switch(setup_packet->request) 
        {
            case SET_FEATURE_REQUEST:
            case CLEAR_FEATURE_REQUEST: 
            {
                #if USB_METADATA_SUPPORTED
                    /* set/clear meta data feature */ 
                    phdc_obj_ptr->phdc_metadata = (boolean)((setup_packet->request & 
                        USB_SET_REQUEST_MASK) >> 1);                            
                    /* inform the application that feature has changed */
                    if (phdc_obj_ptr->phdc_callback.callback != NULL)
                      phdc_obj_ptr->phdc_callback.callback(USB_APP_FEATURE_CHANGED,
                      (void*)(&phdc_obj_ptr->phdc_metadata),
                      phdc_obj_ptr->phdc_callback.arg);
                #endif                       
                break;
            }
            case GET_STATUS_REQUEST: 
            {
                /*implement get status request to get which endpoint has data*/
                *data = (uint_8_ptr)(&phdc_obj_ptr->phdc_ep_has_data);
                *size = setup_packet->length; 
                break;
            }
        }
    }
    else if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_VENDOR) 
    {    /* vendor specific request  */    
        if(phdc_obj_ptr->vendor_callback.callback != NULL) 
        {
            error = phdc_obj_ptr->vendor_callback.callback(setup_packet,data,
            size,phdc_obj_ptr->vendor_callback.arg);
        }
    }    
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_PHDC_Endpoint_Service
 *
 * @brief Called upon a completed endpoint (non-control) transfer 
 *
 * @param event :  Pointer to the event structure
 * 
 * @return  None
 *****************************************************************************
 * This function is called when a transfer completes on a non control endpoint
 *****************************************************************************/  
void USB_PHDC_Endpoint_Service(PTR_USB_EVENT_STRUCT event,pointer arg) 
{/* Body*/
    uint_8 index;
    uint_8 meta_data_packet;
    uint_8 transfers_left;
    uint_8 producer, consumer;    
    USB_APP_EVENT_SEND_COMPLETE event_send_complete; 
    USB_APP_EVENT_DATA_RECIEVED event_data_recieved;
    #if USB_METADATA_SUPPORTED   
        USB_APP_EVENT_METADATA_PARAMS metadata_params;                                                     
    #endif    
    USB_CLASS_PHDC_QOS_BIN qos_bin;
    PHDC_STRUCT_PTR phdc_obj_ptr = (PHDC_STRUCT_PTR)arg;

    if(event->direction == USB_SEND) 
    {        
        /* get the index for the corressponding endpoint(channel) */
        for(index = 0; index < PHDC_TX_ENDPOINTS; index++) 
        {
            if(phdc_obj_ptr->ep_data.ep_tx[index].endpoint == event->ep_num) 
            break;
        }   
        
        /* initialize producer with the num of queued transfers */
        producer = phdc_obj_ptr->ep_data.ep_tx[index].bin_producer;
        /* initialize consumer with the num of de-queued transfers */
        consumer = phdc_obj_ptr->ep_data.ep_tx[index].bin_consumer;        
         
        if(phdc_obj_ptr->ep_data.ep_tx[index].
              qos_bin[consumer % MAX_QOS_BIN_ELEMS].meta_data == FALSE) 
        {/* if the previous packet sent was a meta data do not 
            decrement the transfers_left else decrement */
            phdc_obj_ptr->ep_data.ep_tx[index].transfers_left--;     
        }            
          
        /* de-queue the queue */
        phdc_obj_ptr->ep_data.ep_tx[index].bin_consumer++; 
        
        consumer = phdc_obj_ptr->ep_data.ep_tx[index].bin_consumer;
        
        /* get the qos information of the next transfer */                    
        qos_bin = phdc_obj_ptr->ep_data.ep_tx[index].
            qos_bin[consumer % MAX_QOS_BIN_ELEMS];
                     
        #if USB_METADATA_SUPPORTED   
            /* if transfers left is non zero and the next packet to send 
               is a meta data, discard the meta data packet */
            if(phdc_obj_ptr->ep_data.ep_tx[index].transfers_left != 0) 
            {
                /* discard all the meta data packets if any */
                while(qos_bin.meta_data == TRUE) 
                {
                    /* de-queue the queue-- discarding meta data packet */
                    consumer = 
                        ++phdc_obj_ptr->ep_data.ep_tx[index].bin_consumer;
                
                    if(consumer == producer)
                    break; /* queue is empty */
                    
                    /* set qos_bin for next packet in the queue */
                    qos_bin = phdc_obj_ptr->ep_data.ep_tx[index].
                        qos_bin[consumer % MAX_QOS_BIN_ELEMS];                
                }
            }        
        #endif  
 
        if(consumer != producer) 
        {/*if bin is not empty */                                    
            #if USB_METADATA_SUPPORTED   
                if(qos_bin.meta_data == TRUE) 
                {
                    /* initialize transfers left if the packet is a meta data */
                    phdc_obj_ptr->ep_data.ep_tx[index].transfers_left = 
                        qos_bin.num_tfr;
                    /* initialize current_qos of the transfers that follow the 
                       meta data packet */                                                          
                    phdc_obj_ptr->ep_data.ep_tx[index].current_qos = 
                        qos_bin.qos;                                                                
                }            
            #endif   
            /* send data */
            (void)USB_Class_Send_Data(phdc_obj_ptr->class_handle, qos_bin.channel, 
                qos_bin.app_buff, qos_bin.size);                                                                                                
        } 
        else /* bin is empty */
        {
            /*set endpoint bin as non active */
            phdc_obj_ptr->phdc_ep_has_data &= ~(1<<event->ep_num);
        }

        /* Set the USB_APP_EVENT_SEND_DATA structure */
        event_send_complete.qos = phdc_obj_ptr->ep_data.ep_tx[index].qos;
        event_send_complete.buffer_ptr = event->buffer_ptr;
        event_send_complete.size = event->len;
        
        /* notify the application of the send complete */
        phdc_obj_ptr->phdc_callback.callback(USB_APP_SEND_COMPLETE, 
            (void*)(&event_send_complete),phdc_obj_ptr->phdc_callback.arg);                   
    } 
    else /* direction is USB_RECV */
    {
        USB_CLASS_PHDC_RX_BUFF rx_buff;
        if(!event->len)
        {/* indicates there was call of zero byte recv */
            return; 
        }

        /* get the index for the corressponding endpoint */
        for(index = 0; index < PHDC_RX_ENDPOINTS; index++) 
        {
            if(phdc_obj_ptr->ep_data.ep_rx[index].endpoint == event->ep_num) 
            break;
        }
       
        if(phdc_obj_ptr->ep_data.ep_rx[index].buff_ptr == NULL) 
        {
            /* First Call will have buff_ptr == NULL */
            rx_buff.in_buff = event->buffer_ptr;
            rx_buff.in_size = event->len;
            rx_buff.out_size = 0;
            rx_buff.out_buff = NULL;   
            phdc_obj_ptr->phdc_callback.callback(USB_APP_GET_DATA_BUFF, 
                (void*)(&rx_buff),phdc_obj_ptr->phdc_callback.arg);                        

            phdc_obj_ptr->ep_data.ep_rx[index].buffer_size = rx_buff.out_size;        
            phdc_obj_ptr->ep_data.ep_rx[index].buff_ptr = rx_buff.out_buff;        
        
            if(event->len == rx_buff.out_size) 
            {
                /* Recieved Complete packet first time itself */ 
                USB_mem_copy(event->buffer_ptr,rx_buff.out_buff,
                    rx_buff.out_size);
                event->buffer_ptr = rx_buff.out_buff;
            }
            else 
            {            
                /* Some data is still pending to be recieved
                   copy data to app buffer and schedule next transfer */
                (void)USB_Class_PHDC_Recv_Data(phdc_obj_ptr->phdc_handle,
                    phdc_obj_ptr->ep_data.ep_rx[index].qos,
                    rx_buff.out_buff,
                    phdc_obj_ptr->ep_data.ep_rx[index].buffer_size);
                return;
            }
        }
        
        if(event->len == phdc_obj_ptr->ep_data.ep_rx[index].buffer_size) 
        {        
            #if USB_METADATA_SUPPORTED   
                transfers_left = phdc_obj_ptr->ep_data.ep_rx[index].
                    transfers_left;
                /*compare the recieved signature with the string for preamble 
                  verifiability. if meta_data_packet = 0(both signatures match) 
                  the incoming packet is a meta data */
                meta_data_packet = (uint_8)strncmp(
                    phdc_obj_ptr->meta_data_msg_preamble.signature, 
                    msg_preamble_signature, METADATA_PREAMBLE_SIGNATURE);
                                                                                                                          
                /*check whether transfers are left. we check for meta data only
                  when number of transfers left is zero */
                if((phdc_obj_ptr->phdc_metadata == FALSE) || (transfers_left != 0 && 
                    meta_data_packet !=0)) 
                {
                    phdc_obj_ptr->ep_data.ep_rx[index].transfers_left = 
                        (uint_8)(transfers_left - 1);          
                    /* set the data recieved structure */                                                                 
                    event_data_recieved.qos =
                        phdc_obj_ptr->ep_data.ep_rx[index].qos;
                    event_data_recieved.buffer_ptr = event->buffer_ptr;
                    event_data_recieved.size = event->len;
                
                    /* notify the application that recieve data is complete */
                    phdc_obj_ptr->phdc_callback.callback(USB_APP_DATA_RECEIVED, 
                        (void*)(&event_data_recieved),phdc_obj_ptr->phdc_callback.arg);           
                }
                else if((transfers_left == 0 ) && (meta_data_packet == 0))
                {/*if number of transfers left is zero and the packet is meta 
                   data msg preamble initialize the endpoint data structure 
                   from the meta data msg preamble data structure */                   
                    phdc_obj_ptr->ep_data.ep_rx[index].transfers_left = 
                        phdc_obj_ptr->meta_data_msg_preamble.num_tfr;
                    phdc_obj_ptr->ep_data.ep_rx[index].current_qos = 
                        phdc_obj_ptr->meta_data_msg_preamble.qos;                
                    /* set the event phdc meta data params structure*/
                    metadata_params.channel = event->ep_num;
                    metadata_params.num_tfr =
                    phdc_obj_ptr->meta_data_msg_preamble.num_tfr;
                    metadata_params.qos = phdc_obj_ptr->meta_data_msg_preamble.qos;                                                     
                    metadata_params.metadata_ptr = 
                        &phdc_obj_ptr->meta_data_msg_preamble.opaque_data[0];
                
                    /*notify the appl that meta data params has changed*/
                    phdc_obj_ptr->phdc_callback.callback(USB_APP_META_DATA_PARAMS_CHANGED, 
                        (void*)(&metadata_params),phdc_obj_ptr->phdc_callback.arg);                                                     
                }             
            #else
                /* set the data recieved structure */                                                                 
                event_data_recieved.qos =phdc_obj_ptr->ep_data.ep_rx[index].qos;
                event_data_recieved.buffer_ptr = event->buffer_ptr;
                event_data_recieved.size = event->len;
                
                /* notify the application that recieve data is complete */
                phdc_obj_ptr->phdc_callback.callback(USB_APP_DATA_RECEIVED, 
                (void*)(&event_data_recieved),phdc_obj_ptr->phdc_callback.arg);
            #endif     
            /* when complete packet is recieved 
               reset the buffer_size and buff_ptr */
            phdc_obj_ptr->ep_data.ep_rx[index].buffer_size = 0;        
            phdc_obj_ptr->ep_data.ep_rx[index].buff_ptr = NULL;  
            phdc_obj_ptr->ep_data.ep_rx[index].buff_ptr = NULL;            
        }/* Endif */           
    }/* Endif */                                                                     
}/* EndBody*/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
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
uint_8 USB_Class_PHDC_Send_Data
(
    PHDC_HANDLE handle, 
    boolean meta_data,    /* [IN] packet is meta data or not */
    uint_8 num_tfr,       /* [IN] no. of transfers */
    uint_8 qos,           /* [IN] current qos of the transfer */
    uint_8_ptr app_buff,      /* [IN] buffer to send */
    uint_32 size   /* [IN] length of the transfer */
) 
{
    uint_8 channel;
    uint_8 producer, consumer;
    uint_8 tx_index; 
    PHDC_STRUCT_PTR phdc_obj_ptr;
    uint_8 error = USB_OK; 
    static uint_32 cnt=0;
    
    cnt++;
    
    phdc_obj_ptr =   USB_Phdc_Get_Device_Ptr(handle);
    if (NULL == phdc_obj_ptr)
    {
        return USBERR_ERROR;
    }
        
    
    /* get the index for the corressponding endpoint(channel) to send data
       of desired qos */
    for(tx_index = 0; tx_index < PHDC_TX_ENDPOINTS; tx_index++) 
    {
        if(phdc_obj_ptr->ep_data.ep_tx[tx_index].qos == qos) 
            break;
    }
    
    if(tx_index == PHDC_TX_ENDPOINTS) 
    {
        return USBERR_TX_FAILED;
    }    
    
    #if USB_METADATA_SUPPORTED   
        /* if the packet to send is not as desired return with an error */
        if((((phdc_obj_ptr->ep_data.ep_tx[tx_index].transfers_left == 0) || 
            (phdc_obj_ptr->ep_data.ep_tx[tx_index].type != USB_BULK_PIPE)) &&
            (meta_data == FALSE) && (phdc_obj_ptr->phdc_metadata == TRUE)) || 
            ((phdc_obj_ptr->phdc_metadata == FALSE) && (meta_data == TRUE)))            
        {
            return USBERR_TX_FAILED;
        }
    #endif
    
    channel = phdc_obj_ptr->ep_data.ep_tx[tx_index].endpoint;
    /*set channel active. Set bit map for the corressponding channel(endp)*/
    phdc_obj_ptr->phdc_ep_has_data |= 1 << channel; 
    
    /* initialize producer with the num of queued transfers */
    producer = phdc_obj_ptr->ep_data.ep_tx[tx_index].bin_producer;
    /* initialize consumer with the num of de-queued transfers */
    consumer = phdc_obj_ptr->ep_data.ep_tx[tx_index].bin_consumer;
    
    if((uint_8)(producer - consumer) != (uint_8)(MAX_QOS_BIN_ELEMS))  
    {/* the bin is not full*/    
        uint_8 queue_num = (uint_8)(producer % MAX_QOS_BIN_ELEMS);
        
        /* put all send request parameters in the endpoint data structure */
        phdc_obj_ptr->ep_data.ep_tx[tx_index].qos_bin[queue_num].channel = 
            channel;
        phdc_obj_ptr->ep_data.ep_tx[tx_index].qos_bin[queue_num].meta_data = 
            meta_data;
        phdc_obj_ptr->ep_data.ep_tx[tx_index].qos_bin[queue_num].num_tfr = 
            num_tfr;
        phdc_obj_ptr->ep_data.ep_tx[tx_index].qos_bin[queue_num].qos = qos;
        phdc_obj_ptr->ep_data.ep_tx[tx_index].qos_bin[queue_num].app_buff = 
            app_buff;
        phdc_obj_ptr->ep_data.ep_tx[tx_index].qos_bin[queue_num].size = size;
        
        /* increment producer bin by 1 -- queue the transfer */       
        phdc_obj_ptr->ep_data.ep_tx[tx_index].bin_producer = ++producer;
                
        if((uint_8)(producer - consumer) == (uint_8)1) 
        {/* bin has only this packet to send */                               
            #if USB_METADATA_SUPPORTED   
                /* discard the packet if the packet is a meta data and num of 
                   transfers is not equal to zero */
                if( (meta_data == TRUE) && 
                    (phdc_obj_ptr->ep_data.ep_tx[tx_index].transfers_left != 0))
                {/* notify the application of the error*/
                    phdc_obj_ptr->phdc_callback.callback(USB_APP_ERROR,NULL,
                    phdc_obj_ptr->phdc_callback.arg);
                        return USBERR_TX_FAILED;                
                } 
                else if(meta_data == TRUE) 
                {
                    /* initialize transfers left if the packet is a meta data */
                    phdc_obj_ptr->ep_data.ep_tx[tx_index].transfers_left = 
                        num_tfr;                            
                    /* initialize current_qos of the transfers that follow the 
                       meta data packet */  
                    phdc_obj_ptr->ep_data.ep_tx[tx_index].current_qos = qos;
                }          
            #endif    
            /* send the packet if there is no packet (other than this packet) 
               in the queue */
            error = USB_Class_Send_Data(phdc_obj_ptr->class_handle,channel, 
            app_buff,size);
        }        
    }
    else /* bin is full */
    {
        /* return device is busy when the queue is full */
        error = USBERR_DEVICE_BUSY; 
    }    
    _int_enable();     
    return error;
}

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
PHDC_HANDLE USB_Class_PHDC_Init
(
    PHDC_CONFIG_STRUCT_PTR phdc_config_ptr /*[IN]*/
)
{
    uint_8 ep_count, count=0;
    uint_8 error;    
    PHDC_HANDLE phdc_handle;
    PHDC_STRUCT_PTR devicePtr = NULL;
    USB_ENDPOINTS *info;  
    
    if (NULL == (void *)phdc_config_ptr)
        return USBERR_ERROR;
    
    devicePtr = (PHDC_STRUCT_PTR)USB_mem_alloc_zero(sizeof(PHDC_STRUCT));
    if (NULL == devicePtr)
    {
        #if _DEBUG
            printf("USB_Class_PHDC_Init: Memalloc failed\n");
        #endif  
        return USBERR_ALLOC;
    }
    
    phdc_handle = USB_Phdc_Allocate_Handle();
    if (USBERR_DEVICE_BUSY == phdc_handle) 
    {
        USB_mem_free(devicePtr);
        devicePtr = NULL;
        return USBERR_INIT_FAILED;
    }

        /* initialize the Global Variable Structure */
    USB_mem_zero(devicePtr, sizeof(PHDC_STRUCT));

    
    /* get the endpoint info from the descriptor module */
    info = phdc_config_ptr->info;

    /* initialize the device */
    error = _usb_device_init(USBCFG_DEFAULT_DEVICE_CONTROLLER,
        (_usb_device_handle *)&devicePtr->controller_handle,(uint_8)((info->count) + 1));
    if(error != USB_OK)
    {
      goto error1;  
    }
    
    phdc_config_ptr->desc_callback_ptr->handle = phdc_handle;    
    devicePtr->phdc_ep_has_data = 0; /* no endpoint has data */
    devicePtr->phdc_callback.callback = phdc_config_ptr->phdc_callback.callback;
    devicePtr->phdc_callback.arg = phdc_config_ptr->phdc_callback.arg;
    devicePtr->vendor_callback.callback = phdc_config_ptr->vendor_callback.callback;                                
    devicePtr->vendor_callback.arg = phdc_config_ptr->vendor_callback.arg;
    devicePtr->ep_data.count_rx = 0;
    devicePtr->ep_data.count_tx = 0;
    devicePtr->ep_data.handle = devicePtr->controller_handle;
    devicePtr->service_buff_ptr = NULL;
    devicePtr->class_handle = USB_Class_Init(devicePtr->controller_handle, 
        USB_Class_PHDC_Event,USB_PHDC_Other_Requests,(void *)devicePtr,
        phdc_config_ptr->desc_callback_ptr);
 
    /* initialize endpoint data structure for all endpoints */
    for(ep_count = 0; ep_count < info->count; ep_count++)   
    { 
        if((info->ep[ep_count].direction == USB_RECV) & 
                (devicePtr->ep_data.count_rx < PHDC_RX_ENDPOINTS))
        {                
            count= devicePtr->ep_data.count_rx;
                    
            /* initialize endpoint data structure for recv endpoint */
            devicePtr->ep_data.ep_rx[count].endpoint = 
                    info->ep[ep_count].ep_num;
            devicePtr->ep_data.ep_rx[count].type = 
                    info->ep[ep_count].type;  
            devicePtr->ep_data.ep_rx[count].size = 
                    info->ep[ep_count].size;                                            
            devicePtr->ep_data.ep_rx[count].qos = (
                    info->ep[ep_count].type == USB_INTERRUPT_PIPE ? PHDC_INT_IN_QOS : PHDC_BULK_IN_QOS);
            devicePtr->ep_data.ep_rx[count].current_qos = 
                    INVALID_VAL;                    
            /* register endpoint service on the endpoint*/
            (void)_usb_device_register_service(devicePtr->controller_handle, 
                    devicePtr->ep_data.ep_rx[count].endpoint, 
            USB_PHDC_Endpoint_Service,(void *)devicePtr);                                                                                 
            /* increment count_rx by 1 */
            devicePtr->ep_data.count_rx++;                               
         } 
          else if((info->ep[ep_count].direction == USB_SEND) & 
                (devicePtr->ep_data.count_tx < PHDC_TX_ENDPOINTS))                   
          {                
              count= devicePtr->ep_data.count_tx;                    
              /* initialize endpoint data structure for send endpoint */
              devicePtr->ep_data.ep_tx[count].endpoint =
                    (uint_8)info->ep[ep_count].ep_num;
              devicePtr->ep_data.ep_tx[count].type = 
                    (uint_8)info->ep[ep_count].type;        
              devicePtr->ep_data.ep_tx[count].size = 
                    (uint_32)info->ep[ep_count].size;                                                                
              devicePtr->ep_data.ep_tx[count].qos = (
                    info->ep[ep_count].type == USB_INTERRUPT_PIPE ? PHDC_INT_OUT_QOS : PHDC_BULK_OUT_QOS);
              devicePtr->ep_data.ep_tx[count].current_qos = 
                    (uint_8)INVALID_VAL;
              devicePtr->ep_data.ep_tx[count].bin_consumer = 
                    (uint_8)0x00;         
              devicePtr->ep_data.ep_tx[count].bin_producer = 
                    (uint_8)0x00;                                                                              
              /* register endpoint service on the endpoint*/
              (void)_usb_device_register_service(devicePtr->controller_handle, 
                devicePtr->ep_data.ep_tx[devicePtr->ep_data.count_tx].
                        endpoint, USB_PHDC_Endpoint_Service,(void *)devicePtr);                   
               /* increment count_tx by 1 */
               devicePtr->ep_data.count_tx++;            
          }
    }
            /* set device state as powered */
    (void)_usb_device_set_status(devicePtr->controller_handle,
        (uint_8)USB_STATUS_DEVICE_STATE,
        (uint_16)USB_STATE_POWERED);
    
    #if USB_METADATA_SUPPORTED   
      devicePtr->phdc_metadata = FALSE; /* metadata feature disabled */
    #endif
    phdc_object_array[phdc_handle] = devicePtr;        
    return phdc_handle;
    
 error2:
  /* Implement Denit class and invoke here*/    
 error1: 
     USB_Phdc_Free_Handle(phdc_handle);
     USB_mem_free(devicePtr);
     devicePtr = NULL;
    return error;
}
/**************************************************************************//*!
 *
 * @name  USB_Class_PHDC_Recv_Data
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
uint_8 USB_Class_PHDC_Recv_Data
(
    PHDC_HANDLE         phdc_handle,
    uint_8              qos,           /* [IN] Qos of the transfer */   
    uint_8_ptr          buff_ptr,      /* [IN] buffer to recv */      
    uint_32             size           /* [IN] length of the transfer */
)
{
    PHDC_STRUCT_PTR phdc_obj_ptr;
    uint_8 rx_index;
    uint_8 status = USB_OK;
    USB_CLASS_PHDC_RX_ENDPOINT *phdc_rx_endpoint;

    phdc_obj_ptr = USB_Phdc_Get_Device_Ptr(phdc_handle);
    if (NULL == phdc_obj_ptr)
    {
        return USBERR_ERROR;
    }

    /* get the index for the corressponding endpoint(channel) to receive data
     of desired qos */
    for(rx_index = 0; rx_index < PHDC_RX_ENDPOINTS; rx_index++)
    {
        if((phdc_obj_ptr->ep_data.ep_rx[rx_index].qos & qos) !=0)
            break;
    }

    /* no channel supports the desired qos */
    if(rx_index == PHDC_RX_ENDPOINTS)
    {
        return USBERR_RX_FAILED;
    }

    status = _usb_device_recv_data(phdc_obj_ptr->controller_handle,
        phdc_obj_ptr->ep_data.ep_rx[rx_index].endpoint,buff_ptr,size);
    
   return status;   
}

#endif /*PHDC_CONFIG*/
/* EOF */

