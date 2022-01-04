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
* $FileName: usb_msc.c$
* $Version : 3.8.6.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
* @brief The file contains USB stack MSC layer implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
 #include "usb_stack_config.h"
 
 #if MSD_CONFIG 
    #include "usb_msc.h"
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
 /* Add all the variables needed for usb_msc.c to this structure */
 MSC_DEVICE_STRUCT_PTR   msc_device_array[MAX_MSC_DEVICE];
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
uint_8 USB_MSC_Other_Requests(USB_SETUP_STRUCT * setup_packet, 
                              uint_8_ptr *data, 
                              uint_32 *size,pointer arg);
uint_8 process_mass_storage_command (MSC_DEVICE_STRUCT_PTR mscDevicePtr,
                                     PTR_CBW cbw_ptr, 
                                     uint_32* csw_residue_ptr, 
                                     uint_8* csw_status_ptr);                           
                          
/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/
 
 /*****************************************************************************
 * Local Functions
 *****************************************************************************/
/*************************************************************************//*!
 *
 * @name  USB_Msd_Allocate_Handle
 *
 * @brief The funtion reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static MSD_HANDLE  USB_Msd_Allocate_Handle()
{
    uint_32 cnt = 0;
    for (;cnt< MAX_MSC_DEVICE;cnt++)
    {
       if (msc_device_array[cnt] == NULL)
        return cnt;
    }
    return USBERR_DEVICE_BUSY;
}

/*************************************************************************//*!
 *
 * @name  USB_Msd_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/
static uint_32 USB_Msd_Free_Handle(MSD_HANDLE handle)
{
    if (/*(handle < 0) || */(handle > MAX_MSC_DEVICE))
        return USBERR_ERROR;
    
    USB_mem_free((void *)msc_device_array[handle]);
    msc_device_array[handle] = NULL;
    return USB_OK;
}

/*************************************************************************//*!
 *
 * @name  USB_Msd_Get_Device_Ptr
 *
 * @brief The funtion gets the device pointer from device array .
 *
 * @param handle  index in device array.
 * @return returns returns pointer to MSD device structure..      
 *
 *****************************************************************************/
static MSC_DEVICE_STRUCT_PTR USB_Msd_Get_Device_Ptr(MSD_HANDLE handle)
{
     return msc_device_array[handle]; 
}
 
/**************************************************************************//*!
 *
 * @name  process_mass_storage_command
 *
 * @brief Process a Mass storage class command 
 *        This function is added here to add more sub class specific commands)
 *
 * @param MSC_DEVICE_STRUCT_PTR 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint_8 process_mass_storage_command 
(
    MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
    PTR_CBW cbw_ptr, 
    uint_32* csw_residue_ptr, 
    uint_8* csw_status_ptr
)
{   
    uint_8 error = USBERR_ERROR;/* initializing to error value */
    switch (cbw_ptr->command_block[0]) 
    {
        /* commands to be supported by all devices */
        case INQUIRY_COMMAND : /*opcode : 0x12*/
               error = msc_inquiry_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;
        case READ_10_COMMAND : /*opcode : 0x28 */
        case READ_12_COMMAND : /*opcode : 0xA8 */
            error = msc_read_command(msc_obj_ptr, 
            cbw_ptr,csw_residue_ptr,csw_status_ptr);
            break;      
        case REQUEST_SENSE_COMMAND : /*opcode : 0x03*/
            error = msc_request_sense_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
           break;           
        case TEST_UNIT_READY_COMMAND : /*opcode : 0x00 */ 
           error = msc_test_unit_ready_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break; 
        case WRITE_10_COMMAND : /*opcode : 0x2A */
        case WRITE_12_COMMAND : /*opcode : 0xAA */
           error = msc_write_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
           csw_status_ptr);
            break; 
        case PREVENT_ALLOW_MEDIUM_REM_COMMAND : /*opcode :0x1E */
           error = msc_prevent_allow_medium_removal(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;                   
        case FORMAT_UNIT_COMMAND : /*opcode : 0x04*/
           error = msc_format_unit_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;
        case READ_CAPACITY_10_COMMAND : /*opcode : 0x25*/
           error = msc_read_capacity_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;       
        case MODE_SENSE_10_COMMAND : /* opcode :0x5A*/
        case MODE_SENSE_6_COMMAND : /* opcode : 0x1A */
           error = msc_mode_sense_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;
        case MODE_SELECT_10_COMMAND : /*opcode : 0x55 */
        case MODE_SELECT_6_COMMAND : /*opcode : 0x15 */
           error = msc_mode_select_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;
        case READ_FORMAT_CAPACITIES_COMMAND : /*opcode : 0x23 */
           error = msc_read_format_capacity_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;
        case SEND_DIAGNOSTIC_COMMAND : /*opcode : 0x1D*/
            error = msc_send_diagnostic_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break; 
        case VERIFY_COMMAND : /*opcode : 0x2F*/
            error = msc_verify_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;
        case START_STOP_UNIT_COMMAND : /*opcode : 0x1B*/
            error = msc_start_stop_unit_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            break;                                  
        default: /* for all unsupported commands */
            error = msc_unsupported_command(msc_obj_ptr, cbw_ptr,csw_residue_ptr,
                csw_status_ptr);
            msc_obj_ptr->out_flag = FALSE;
            msc_obj_ptr->in_flag = FALSE;
            msc_obj_ptr->out_stall_flag = FALSE;
            msc_obj_ptr->in_stall_flag = FALSE;
            break;
    }
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Bulk_In
 *
 * @brief The funtion ic callback function of DIC Bulk In Endpoint 
 *
 * @param event
 * @param arg
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Bulk_In(PTR_USB_EVENT_STRUCT event, pointer arg)
{
    uint_8 event_type; 
    PTR_CSW csw_ptr;
    MSC_DEVICE_STRUCT_PTR msc_obj_ptr;
    
    #if IMPLEMENT_QUEUING 
        uint_8 index;
        uint_8 producer, consumer;
        USB_ENDPOINTS *usb_ep_data ;
            
        USB_CLASS_MSC_QUEUE queue;
    #endif
    
    msc_obj_ptr = (MSC_DEVICE_STRUCT_PTR)arg;  
    
    if (NULL == msc_obj_ptr)
    {
        #if _DEBUG
            printf("USB_Service_Bulk_In: msc_obj_ptr is NULL\n");
        #endif  
        return;
    }
    
    #if IMPLEMENT_QUEUING    
        usb_ep_data = msc_obj_ptr->ep_desc_data; 
        
        /* map the endpoint num to the index of the endpoint structure */
        for(index = 0; index < usb_ep_data->count; index++) 
        {
            if(usb_ep_data->ep[index].ep_num == event->ep_num)
            break;
        }
                                                   
        producer = msc_obj_ptr->ep[index].bin_producer;         
        /* if there are no errors de-queue the queue and decrement the no. of 
           transfers left, else send the same data again */
        msc_obj_ptr->ep[index].bin_consumer++;              
        consumer = msc_obj_ptr->ep[index].bin_consumer;         
        
        if(consumer != producer) 
        {/*if bin is not empty */
                            
            queue = msc_obj_ptr->ep[index].queue[consumer%MSD_MAX_QUEUE_ELEMS];                         
            (void)USB_Class_Send_Data(msc_obj_ptr->class_handle, queue.channel, 
                queue.app_data.data_ptr, queue.app_data.data_size);
            return;
        }          
    #endif

    if(msc_obj_ptr->transfer_remaining >= event->len)
    {   /* decrement the global count */
        msc_obj_ptr->transfer_remaining -= event->len;          
    }
        
    /* check if there is need to stall BULK IN ENDPOINT And
       there isn't any transfer in progress*/
    if( (msc_obj_ptr->in_stall_flag == TRUE)&&
        (!msc_obj_ptr->transfer_remaining))
    {
        uint_8 component = (uint_8)(event->ep_num | 
            (USB_SEND<<COMPONENT_PREPARE_SHIFT));
        msc_obj_ptr->in_stall_flag = FALSE; /* clear the flag */
        msc_obj_ptr->in_flag = FALSE; /* clear send flag */
        /* now, stalling the status phase - CASE 5th of THIRTEEN CASES*/           
        (void)_usb_device_set_status(msc_obj_ptr->controller_handle,
            (uint_8)(component|USB_STATUS_ENDPOINT),
            (uint_16)USB_STATUS_STALLED);
        return;                 
    }

    /* If its not a data phase on bulk endpoint */
    if ((!msc_obj_ptr->in_flag) && (event->len == MSC_CSW_LENGTH)) 
    {
        csw_ptr = (PTR_CSW)(event->buffer_ptr);
    } 
   
    if(msc_obj_ptr->in_flag) /* bulk in transaction has occured before CSW */
    {           
        if(msc_obj_ptr->param_callback.callback != NULL) 
        {
            event_type = (uint_8)(msc_obj_ptr->transfer_remaining?
                USB_MSC_DEVICE_READ_REQUEST : USB_APP_SEND_COMPLETE);
            
            if(event_type == USB_APP_SEND_COMPLETE)
            {       
                APP_DATA_STRUCT bulk_in_recv;
                bulk_in_recv.data_ptr = event->buffer_ptr;
                bulk_in_recv.data_size = event->len; 
                msc_obj_ptr->param_callback.callback(event_type,
                    (void*)&bulk_in_recv, msc_obj_ptr->param_callback.arg);
            }
            else
            {
                LBA_APP_STRUCT lba_data;        
                msc_obj_ptr->current_offset += event->len;
                lba_data.offset = msc_obj_ptr->current_offset;
                lba_data.size = (msc_obj_ptr->transfer_remaining > 
                    msc_obj_ptr->msd_buff.msc_lba_send_buff_size) ? msc_obj_ptr->msd_buff.msc_lba_send_buff_size : 
                    msc_obj_ptr->transfer_remaining; 
                /* whichever is smaller */
                lba_data.buff_ptr = msc_obj_ptr->msd_buff.msc_lba_send_ptr;
                /* fetch data from MSD App(e.g)ram disk or Sd card, etc*/
                msc_obj_ptr->param_callback.callback(event_type,
                    (void*)&lba_data, msc_obj_ptr->param_callback.arg);
                /* Send data on USB Bus */
                (void)USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle, 
                    (uint_8_ptr)lba_data.buff_ptr,lba_data.size);    
            }
        }        
        
        if(!msc_obj_ptr->transfer_remaining)
        {   /* marks the end of data phase */
            msc_obj_ptr->in_flag = FALSE; /* clear the flag for next CBW */         
            /* Send the command status information */
            (void)USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle, 
                (uint_8_ptr)msc_obj_ptr->csw_ptr, MSC_CSW_LENGTH);                  
        }
    } 
    else if ((event->len == MSC_CSW_LENGTH) /* CSW is 13 bytes in length */
        && (csw_ptr->signature == USB_DCSWSIGNATURE)) /*valid CSW signature*/                 
    {
        /* this flag will now be set on reset or after CSW being sent */
        msc_obj_ptr->cbw_valid_flag = TRUE;        
        /* prepare for next CBW */  
        (void)USB_MSC_Bulk_Recv_Data(msc_obj_ptr->controller_handle,
            (uint_8*)msc_obj_ptr->cbw_ptr, MSC_CBW_LENGTH);
    }     
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Bulk_Out
 *
 * @brief The funtion ic callback function of DIC Bulk Out Endpoint 
 *
 * @param event
 * @param arg
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Bulk_Out(PTR_USB_EVENT_STRUCT event,pointer arg)
{       
    uint_8 event_type; 
    PTR_CBW cbw_ptr = NULL;
    uint_8 error;
    MSC_DEVICE_STRUCT_PTR msc_obj_ptr;
    
    msc_obj_ptr = (MSC_DEVICE_STRUCT_PTR)arg;  
    
    if (NULL == msc_obj_ptr)
    {
        #if _DEBUG
            printf("USB_Service_Bulk_Out: msc_dev_ptr is NULL\n");
        #endif  
        return;
    }
         
     /* check if there is need to stall BULK IN ENDPOINT */
    if( (msc_obj_ptr->out_stall_flag == TRUE)&&
        (!msc_obj_ptr->transfer_remaining))
    {
        uint_8 component = (uint_8)(event->ep_num | 
            (USB_RECV<<COMPONENT_PREPARE_SHIFT));
        msc_obj_ptr->out_stall_flag = FALSE; /* clear the flag */
        msc_obj_ptr->out_flag = FALSE; /* clear send flag */
        /* now, stalling the status phase - CASE 5th of THIRTEEN CASES*/           
        (void)_usb_device_set_status(msc_obj_ptr->controller_handle,
            (uint_8)(component|USB_STATUS_ENDPOINT),
            (uint_16)USB_STATUS_STALLED);
        return;                 
    }
    
    /* If its not a data phase on bulk endpoint */
    if ((!msc_obj_ptr->out_flag) && (event->len == MSC_CBW_LENGTH)) 
    {
        cbw_ptr = (PTR_CBW)(event->buffer_ptr);
    } 

    if(msc_obj_ptr->out_flag) /* bulk out transaction has occured after CBW */
    {           
        if(msc_obj_ptr->param_callback.callback != NULL) 
        {
            event_type = (uint_8)(msc_obj_ptr->transfer_remaining?
                USB_MSC_DEVICE_WRITE_REQUEST : USB_APP_DATA_RECEIVED);
                                        
            if(event_type == USB_APP_DATA_RECEIVED)
            {
                APP_DATA_STRUCT bulk_out_recv;                  
                bulk_out_recv.data_ptr = event->buffer_ptr;
                bulk_out_recv.data_size = event->len; 
                
                msc_obj_ptr->param_callback.callback(event_type,
                    (void*)&bulk_out_recv, msc_obj_ptr->param_callback.arg);
            }
            else
            {
                LBA_APP_STRUCT lba_data1;
                lba_data1.buff_ptr = msc_obj_ptr->msd_buff.msc_lba_recv_ptr;
                lba_data1.offset = msc_obj_ptr->current_offset;
                lba_data1.size = event->len;                                
                /* copy the data received on USB Bus, to MSD Disk 
                   (e.g.) CD/DVD or SD Card, etc */
                msc_obj_ptr->param_callback.callback(event_type,
                    (void*)&lba_data1, msc_obj_ptr->param_callback.arg);
            }            
        }
        
        if(msc_obj_ptr->transfer_remaining >= event->len)
        {   /* decrement the global count */
            msc_obj_ptr->transfer_remaining -= event->len;
                                  
            if((msc_obj_ptr->out_stall_flag == TRUE)&&
               (!msc_obj_ptr->transfer_remaining))
            {
                uint_8 component = (uint_8)(event->ep_num | 
                    (event->direction<<COMPONENT_PREPARE_SHIFT));
                msc_obj_ptr->out_stall_flag = FALSE; /* clear the flag */
                msc_obj_ptr->out_flag = FALSE; /* clear send flag */
                /* now, stalling the status phase - CASE 5th of THIRTEEN CASES*/           
                (void)_usb_device_set_status(msc_obj_ptr->controller_handle,
                    (uint_8)(component|USB_STATUS_ENDPOINT),
                    (uint_16)USB_STATUS_STALLED);
                return;                 
            }
        }
        
        if(msc_obj_ptr->transfer_remaining)
        {   /* Transfers Remaining -- Again invoke recv_data */         
            LBA_APP_STRUCT lba_data0;           
            msc_obj_ptr->current_offset += event->len;
            lba_data0.size = (msc_obj_ptr->transfer_remaining > msc_obj_ptr->msd_buff.msc_lba_recv_buff_size) ? 
                msc_obj_ptr->msd_buff.msc_lba_recv_buff_size : msc_obj_ptr->transfer_remaining; /* whichever is smaller */
            lba_data0.buff_ptr = msc_obj_ptr->msd_buff.msc_lba_recv_ptr;                
            (void)USB_MSC_Bulk_Recv_Data(msc_obj_ptr->controller_handle,
                lba_data0.buff_ptr,lba_data0.size);
            return;
        }
        else
        {   /* marks the end of data phase */
            msc_obj_ptr->out_flag = FALSE; /* clear the flag for next CBW */
            /* Send the command status information */
            (void)USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle, 
                (uint_8_ptr)msc_obj_ptr->csw_ptr, MSC_CSW_LENGTH); 
        }
    } 
    else if(/* check for valid and meaningful CBW */
        /* CBW received after device had sent a CSW or after a reset */
        (msc_obj_ptr->cbw_valid_flag)         
        /* CBW is 31 bytes in length */
        && (event->len == MSC_CBW_LENGTH) 
        /* valid CBW signature*/
        && (cbw_ptr != NULL) && (cbw_ptr->signature == USB_DCBWSIGNATURE) 
        /* all reserved bits should be zero*/
        && (!((cbw_ptr->lun & 0xF0) || (cbw_ptr->cb_length & 0xE0)))         
        /* host should send command to valid LUN */
        && (cbw_ptr->lun<msc_obj_ptr->device_info.num_lun_supported)                                    
        /* valid cbwcb length*/
        && ((cbw_ptr->cb_length >= 0x01)&&(cbw_ptr->cb_length <= 0x10))                                                     
          )
    {
        msc_obj_ptr->re_stall_flag = TRUE;
        /* A valid CBW was received */
        msc_obj_ptr->csw_ptr->signature = USB_DCSWSIGNATURE;
        msc_obj_ptr->csw_ptr->tag = cbw_ptr->tag;
        /*this flag will now be set on reset or after CSW being sent */
        msc_obj_ptr->cbw_valid_flag = FALSE;
        
        cbw_ptr->data_length = LONG_LE_TO_HOST(cbw_ptr->data_length);
        
        /* set flag if device is going to recv data in coming transaction */
        msc_obj_ptr->out_flag = (boolean)(( (!(cbw_ptr->flag & USB_CBW_DIRECTION_BIT))
            && (cbw_ptr->data_length))?TRUE:FALSE);
        /* set flag if send is going to send data in coming transaction */
        msc_obj_ptr->in_flag = (boolean)(( (cbw_ptr->flag & USB_CBW_DIRECTION_BIT)
            && (cbw_ptr->data_length))?TRUE:FALSE);                
        /* Process the command */
        error = process_mass_storage_command(msc_obj_ptr, cbw_ptr, 
            &(msc_obj_ptr->csw_ptr->residue), &(msc_obj_ptr->csw_ptr->csw_status));
        
        if(error == USBERR_ENDPOINT_STALLED)
        {
            if(msc_obj_ptr->out_flag==TRUE)
            {
                msc_obj_ptr->out_stall_flag = TRUE; 
                msc_obj_ptr->out_flag = FALSE; /* so as to send status phase */
            }
            else if(msc_obj_ptr->in_flag==TRUE)
            {
                msc_obj_ptr->in_stall_flag = TRUE;  
            }           
        }
        
        /* if there is no data phase on bulk endpoints*/
        if(!(msc_obj_ptr->out_flag || msc_obj_ptr->in_flag)) 
        {   /* Send the command status information */
            (void)USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle, 
                (uint_8_ptr)msc_obj_ptr->csw_ptr, MSC_CSW_LENGTH);         
        }
    } 
    else /* Invalid/NMreceived */
    {        
        uint_8 direction;
        uint_8 ep_num;
        /* prepare the component to be sent in lower layer with 
           endpoint number and direction*/
        uint_8 component;
        
        direction  = USB_RECV; 
        ep_num = msc_obj_ptr->bulk_out_endpoint;
        component = (uint_8)(ep_num | (direction<<COMPONENT_PREPARE_SHIFT));

        /* incase of error Stall endpoint and update the status*/        
        (void)_usb_device_set_status(msc_obj_ptr->controller_handle,
            (uint_8)(component|USB_STATUS_ENDPOINT),
            (uint_16)USB_STATUS_STALLED);

        direction  = USB_SEND; 
        ep_num = msc_obj_ptr->bulk_in_endpoint;
        component = (uint_8)(ep_num | (direction<<COMPONENT_PREPARE_SHIFT));

        /* incase of error Stall endpoint and update the status*/        
        (void)_usb_device_set_status(msc_obj_ptr->controller_handle,
            (uint_8)(component|USB_STATUS_ENDPOINT),
            (uint_16)USB_STATUS_STALLED);
        /* make the following CBWs invalid */
        msc_obj_ptr->cbw_valid_flag = FALSE;
    }    
}


/**************************************************************************//*!
 *
 * @name  USB_Class_MSC_Event
 *
 * @brief The funtion initializes MSC endpoints 
 *
 * @param arg             handle to Identify class object.
 * @param event           pointer to event structure
 * @param val             gives the configuration value 
 *
 * @return None       
 *
 *****************************************************************************/
 void USB_Class_MSC_Event(uint_8 event, void* val,pointer arg) 
{   
    MSC_DEVICE_STRUCT_PTR msc_dev_ptr;
    #if IMPLEMENT_QUEUING    
        uint_8 index;
    #endif    
    USB_ENDPOINTS *usb_ep_data;
    
    msc_dev_ptr = (MSC_DEVICE_STRUCT_PTR)arg;
    
    if (NULL == msc_dev_ptr)
    {
        #if _DEBUG
            printf("USB_Class_MSC_Event: msc_dev_ptr is NULL\n");
        #endif  
        return;
    }
    
    /* get the endpoints from the descriptor module */            
    usb_ep_data = msc_dev_ptr->ep_desc_data; 
        
    if(event == USB_APP_ENUM_COMPLETE)
    {
        uint_8 count = 0;
        /* intialize all non control endpoints */            
        while(count < usb_ep_data->count) 
        {
            USB_EP_STRUCT_PTR ep_struct_ptr= 
                (USB_EP_STRUCT_PTR) (&usb_ep_data->ep[count]);
            uint_8 component = (uint_8)(ep_struct_ptr->ep_num | 
                (ep_struct_ptr->direction << COMPONENT_PREPARE_SHIFT));

            (void)_usb_device_init_endpoint(msc_dev_ptr->controller_handle,
                ep_struct_ptr, TRUE);
            
            /* register callback service for Non Control EndPoints */
            if(ep_struct_ptr->type == USB_BULK_PIPE) 
            {             
                if(ep_struct_ptr->direction == USB_RECV) 
                {
                    (void)_usb_device_register_service(msc_dev_ptr->controller_handle,
                        (uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                        USB_Service_Bulk_Out,(void *)msc_dev_ptr);
                } 
                else
                {
                    (void)_usb_device_register_service(msc_dev_ptr->controller_handle,
                        (uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                        USB_Service_Bulk_In,(void *)msc_dev_ptr);
                }            
            }
                          
            /* set the EndPoint Status as Idle in the device layer */
            /* (no need to specify direction for this case) */
            (void)_usb_device_set_status(msc_dev_ptr->controller_handle,
                (uint_8)(USB_STATUS_ENDPOINT|component),
                (uint_16)USB_STATUS_IDLE);                                                                                                         
            count++;                                                    
        }
        msc_dev_ptr->re_stall_flag = FALSE;
        msc_dev_ptr->out_flag = FALSE;
        msc_dev_ptr->in_flag = FALSE;
        msc_dev_ptr->out_stall_flag = FALSE;
        msc_dev_ptr->in_stall_flag = FALSE;
        msc_dev_ptr->cbw_valid_flag = TRUE; /*making the first CBW valid */ 
        msc_dev_ptr->transfer_remaining = 0; 
                
    }
    else if(event == USB_APP_BUS_RESET)
    {
        #if IMPLEMENT_QUEUING
            for(index = 0; index < usb_ep_data->count; index++) 
            {            
                msc_dev_ptr->ep[index].bin_consumer = 0x00;         
                msc_dev_ptr->ep[index].bin_producer = 0x00;        
            }
        #endif 
    }
    else if(event == USB_APP_EP_UNSTALLED)
    {
        uint_8 value;
        value = *((uint_8_ptr)val);
        
        if( (msc_dev_ptr->re_stall_flag == TRUE)
            &&
            (((value & 0x0F) == msc_dev_ptr->bulk_in_endpoint) || 
            ((value & 0x0F) == msc_dev_ptr->bulk_out_endpoint)))
        {   /* For MASS Storage Class BULK ENDPOINTS have to be unstalled 
               only on receiving Bulk Only Reset. 
               Therefore, if Host sends clear feature to unstall these 
               endpoints, re-stall them
             */     
            (void)_usb_device_set_status(msc_dev_ptr->controller_handle,
                (uint_8)(value|USB_STATUS_ENDPOINT),
                (uint_16)USB_STATUS_STALLED);
        }       
    }
    else if(event == USB_APP_EP_STALLED)
    {
        /* Code to be added here, 
           if there is some action needed at app level */           
    }
            
    if(msc_dev_ptr->msc_callback.callback != NULL) 
    {
        msc_dev_ptr->msc_callback.callback(event,val,
            msc_dev_ptr->msc_callback.arg);
    } 
}

/**************************************************************************//*!
 *
 * @name  USB_MSC_Other_Requests
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param arg
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint_8 USB_MSC_Other_Requests
(
    USB_SETUP_STRUCT * setup_packet, 
    uint_8_ptr *data, 
    uint_32 *size,
    pointer arg
) 
{
    MSC_DEVICE_STRUCT_PTR msc_dev_ptr;
    uint_8 error = USBERR_INVALID_REQ_TYPE;  
     
    msc_dev_ptr = (MSC_DEVICE_STRUCT_PTR)arg;
    
    if (NULL == msc_dev_ptr)
    {
        #if _DEBUG
            printf("USB_MSC_Other_Requests: msc_dev_ptr is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
    
    *size=0;   
 
     if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_CLASS) 
     {  /* class request so handle it here */
     
        error = USB_OK;
        
        /* call for class/subclass specific requests */
        switch(setup_packet->request) 
        {
            case GET_MAX_LUN :
                if((setup_packet->index < msc_dev_ptr->usb_max_suported_interfaces)&&
                    (!setup_packet->value)&&(setup_packet->length == 0x0001)&&
                    ((setup_packet->request_type & USB_DATA_DIREC_MASK) == 
                    USB_DATA_TO_HOST))
                {
                    /* return the maximum number of logical units supported */
                    *data = &msc_dev_ptr->lun;
                    *size=1;
                    msc_dev_ptr->re_stall_flag = TRUE;
                }
                else 
                {   /* for Get Max LUN request with invalid wIndex parameter, 
                       host expects stall */
                    error = USBERR_INVALID_REQ_TYPE; 
                }              
              break;
            case BULK_ONLY_MASS_STORAGE_RESET :
                /* Steps to be taken in this command :
                1) ready the device for the next CBW from the host
                2) preserve the value of its bulk data toggle bits 
                3) preserve the value of its bulk endpoint STALL conditions
                4) device shall NAK the status stage of device request until 
                   command is complete*/

                if((setup_packet->index < msc_dev_ptr->usb_max_suported_interfaces)&&
                    (!setup_packet->value)&&(!setup_packet->length) &&
                    ((setup_packet->request_type & USB_DATA_DIREC_MASK) == 
                    USB_DATA_TO_DEVICE))
                {
                    /* get the endpoints from the descriptor module */            
                    USB_ENDPOINTS *usb_ep_data = msc_dev_ptr->ep_desc_data;
                    uint_8 count = 0;

                    /* deinitialize and intialize bulk endpoints */            
                    while(count < usb_ep_data->count) 
                    {
                        USB_EP_STRUCT_PTR ep_struct_ptr= 
                            (USB_EP_STRUCT_PTR) (&usb_ep_data->ep[count]);
                        uint_8 component = (uint_8)(ep_struct_ptr->ep_num | 
                            (ep_struct_ptr->direction << COMPONENT_PREPARE_SHIFT));
                        
                        if(ep_struct_ptr->type == USB_BULK_PIPE)
                        {
                            (void)_usb_device_deinit_endpoint(msc_dev_ptr->controller_handle,
                                ep_struct_ptr->ep_num, ep_struct_ptr->direction);
                            (void)_usb_device_init_endpoint(msc_dev_ptr->controller_handle,
                                ep_struct_ptr, TRUE);
                            /* set EndPoint Status as Idle */                       
                            (void)_usb_device_set_status(msc_dev_ptr->controller_handle,
                                (uint_8)(USB_STATUS_ENDPOINT|component),
                                (uint_16)USB_STATUS_IDLE);                                                        
                        }
                        count++;
                    }
                    msc_dev_ptr->out_flag = FALSE;
                    msc_dev_ptr->in_flag = FALSE;
                    msc_dev_ptr->out_stall_flag = FALSE;
                    msc_dev_ptr->in_stall_flag = FALSE;
                    /*making the first CBW valid */
                    msc_dev_ptr->cbw_valid_flag = TRUE;                
                    msc_dev_ptr->re_stall_flag = FALSE;
                    msc_dev_ptr->transfer_remaining = 0;
                }
                else 
                {   /* for Get Max LUN request with invalid wIndex parameter, 
                       host expects stall */
                    error = USBERR_INVALID_REQ_TYPE; 
                }    
                break;           
            default :break;        
        }
    } 
    else if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_VENDOR) 
    {   /* vendor specific request  */    
        if(msc_dev_ptr->vendor_callback.callback != NULL) 
        {
            error = msc_dev_ptr->vendor_callback.callback(setup_packet,
            data,size,msc_dev_ptr->vendor_callback.arg);
        }
    }
     
    return error;
}


/*****************************************************************************
 * Global Functions
 *****************************************************************************/


/**************************************************************************//*!
 *
 * @name  USB_Class_MSC_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param msd_config_ptr    : Configuration paramemter strucutre pointer
 *                            passed by APP.
 * @return status       
 *         MSD Handle           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the MSC Class layer and layers it is dependednt on 
 ******************************************************************************/
MSD_HANDLE USB_Class_MSC_Init
(
   USB_MSD_CONFIG_STRUCT_PTR msd_config_ptr /*[IN]*/
) 
{   
    #if IMPLEMENT_QUEUING    
        uint_8 index;
    #endif    
    uint_8 error;
    MSD_HANDLE msd_handle;
    MSC_DEVICE_STRUCT_PTR devicePtr = NULL;
    USB_CLASS_CALLBACK_STRUCT scsi_cb;
    USB_ENDPOINTS *usb_ep_data;

    if (msd_config_ptr == NULL)
    {
        return USBERR_ERROR;    
    }
    
    devicePtr = (MSC_DEVICE_STRUCT_PTR)USB_mem_alloc_zero(sizeof(MSC_DEVICE_STRUCT));
    if (NULL == devicePtr)
    {
        #if _DEBUG
            printf("1: USB_Class_MSC_Init: Memalloc failed\n");
        #endif  
        return USBERR_ALLOC;
    }
    
    msd_handle = USB_Msd_Allocate_Handle();
    if (USBERR_DEVICE_BUSY == msd_handle) 
    {
        USB_mem_free(devicePtr);
        devicePtr = NULL;
        return USBERR_INIT_FAILED;
    }

        /* initialize the Global Variable Structure */
    USB_mem_zero(devicePtr, sizeof(MSC_DEVICE_STRUCT));
     /* Initialize number of Logical Units */
    devicePtr->ep = msd_config_ptr->ep;
    usb_ep_data = msd_config_ptr->ep_desc_data;
    USB_mem_copy(&msd_config_ptr->device_info, &devicePtr->device_info, 
        sizeof(DEVICE_LBA_INFO_STRUCT));
    USB_mem_copy(&msd_config_ptr->msd_buff, &devicePtr->msd_buff, 
        sizeof(MSD_BUFF_INFO));
    devicePtr->lun = (uint_8)(msd_config_ptr->device_info.num_lun_supported - 1);
    
    /* Initialize the device layer*/
    error = _usb_device_init(USBCFG_DEFAULT_DEVICE_CONTROLLER,
        &devicePtr->controller_handle, 
        (uint_8)(usb_ep_data->count+1));
    /* +1 is for Control Endpoint */
    if(error != USB_OK)
    {
      goto error1;  
    }

    msd_config_ptr->desc_callback_ptr->handle = msd_handle;
    /* Initialize the generic class functions */
    devicePtr->class_handle = USB_Class_Init(devicePtr->controller_handle,USB_Class_MSC_Event,
       USB_MSC_Other_Requests,(void *)devicePtr,msd_config_ptr->desc_callback_ptr);
            
    /* Initialize the pstn subclass functions */
    scsi_cb.callback = USB_Class_MSC_Event;
    scsi_cb.arg   = devicePtr; 
    error = USB_MSC_SCSI_Init(devicePtr,&scsi_cb, &msd_config_ptr->device_info,
        msd_config_ptr->implementing_disk_drive); 
    
    if(error != USB_OK)
    {
      goto error2;  
    }
    
    devicePtr->cbw_ptr = NULL; /* Initializing */
    devicePtr->csw_ptr = NULL;
    
    devicePtr->cbw_ptr = (PTR_CBW)USB_mem_alloc_uncached(MSC_CBW_LENGTH); 
    if (NULL == devicePtr->cbw_ptr)
    {
        #if _DEBUG
            printf("2: USB_Class_MSC_Init: Memalloc failed\n");
        #endif  
        return USBERR_ALLOC;
    }
    _mem_zero(devicePtr->cbw_ptr, MSC_CBW_LENGTH);
    
    devicePtr->csw_ptr = (PTR_CSW)USB_mem_alloc_uncached(MSC_CSW_LENGTH); 
    if (NULL == devicePtr->csw_ptr)
    {
        #if _DEBUG
            printf("3: USB_Class_MSC_Init: Memalloc failed\n");
        #endif  
        return USBERR_ALLOC;
    }
    _mem_zero(devicePtr->csw_ptr, MSC_CSW_LENGTH);
    
    devicePtr->ep_desc_data = usb_ep_data;
    devicePtr->bulk_in_endpoint =   msd_config_ptr->bulk_in_endpoint;
    devicePtr->bulk_in_endpoint_packet_size =   
        msd_config_ptr->bulk_in_endpoint_packet_size;
    devicePtr->bulk_out_endpoint = 
        msd_config_ptr->bulk_out_endpoint;
    devicePtr->usb_max_suported_interfaces = 
        msd_config_ptr->usb_max_suported_interfaces;
                                                           
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
    devicePtr->msc_callback.callback = msd_config_ptr->msc_class_callback->callback;
    devicePtr->msc_callback.arg = msd_config_ptr->msc_class_callback->arg;
           
    /* save the callback pointer */
    devicePtr->vendor_callback.callback =
        msd_config_ptr->vendor_req_callback->callback;
    devicePtr->vendor_callback.arg = msd_config_ptr->vendor_req_callback->arg;
           
    /* save the callback pointer */
    devicePtr->param_callback.callback = msd_config_ptr->param_callback->callback;           
    devicePtr->param_callback.arg = msd_config_ptr->param_callback->arg;

    devicePtr->msc_handle = msd_handle;
    msc_device_array[msd_handle] = devicePtr;
    
    return msd_handle;
   
    error2:
        /* Implement Denit class and invoke here*/    
    error1: 
        USB_Msd_Free_Handle(msd_handle);
        USB_mem_free(devicePtr);
        devicePtr = NULL; 
    return error;     
}

/**************************************************************************//*!
 *
 * @name  USB_Class_MSC_Send_Data
 *
 * @brief 
 *
 * @param msc_handle   :   handle returned from USB_Class_MSC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
uint_8 USB_Class_MSC_Send_Data
(
    MSD_HANDLE msc_handle, /*[IN]*/
    uint_8 ep_num, /*[IN]*/
    uint_8_ptr app_buff, /*[IN]*/
    uint_32 size /*[IN]*/
) 
{
    MSC_DEVICE_STRUCT_PTR devicePtr;
    #if IMPLEMENT_QUEUING    
        uint_8 index;
        uint_8 producer, consumer; 
        USB_ENDPOINTS *usb_ep_data;
    #endif
    uint_8 error = USB_OK;


    if (/*(msc_handle < 0) || */ (msc_handle > MAX_MSC_DEVICE))
       return USBERR_ERROR;
      
    devicePtr = USB_Msd_Get_Device_Ptr(msc_handle);
    if (devicePtr == NULL)
        return USBERR_ERROR;

    #if IMPLEMENT_QUEUING    
 
        usb_ep_data = devicePtr->ep_desc_data; 
      
        /* map the endpoint num to the index of the endpoint structure */
        for(index = 0; index < usb_ep_data->count; index++) 
        {
            if(usb_ep_data->ep[index].ep_num == ep_num)
            break;
        }
        
        producer = devicePtr->ep[index].bin_producer;
        consumer = devicePtr->ep[index].bin_consumer;
                                
        if(((uint_8)(producer - consumer)) != (uint_8)(MSD_MAX_QUEUE_ELEMS))  
        {
            /* the bin is not full*/        
            uint_8 queue_num = (uint_8)(producer % MSD_MAX_QUEUE_ELEMS);
            
            /* put all send request parameters in the endpoint data structure*/
            devicePtr->ep[index].queue[queue_num].channel = ep_num;
            devicePtr->ep[index].queue[queue_num].app_data.data_ptr = app_buff;
            devicePtr->ep[index].queue[queue_num].app_data.data_size = size;
            devicePtr->ep[index].queue[queue_num].handle = 
            devicePtr->controller_handle; 
            
            /* increment producer bin by 1*/       
            devicePtr->ep[index].bin_producer = ++producer;
                    
            if((uint_8)(producer - consumer) == (uint_8)1)         
            {          
    #endif            
                error = USB_Class_Send_Data(devicePtr->class_handle, 
                    ep_num, app_buff,size);
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
 * @name  USB_MSC_LBA_Transfer
 *
 * @brief 
 *
 * @param MSC_DEVICE_STRUCT_PTR   
 * @param direction       :   transfer direction
 * @param lba_info_ptr    :   buffer to send
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
uint_8 USB_MSC_LBA_Transfer
(
    MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
    boolean direction,
    PTR_LBA_INFO_STRUCT lba_info_ptr
)
{
    uint_8 error;
    LBA_APP_STRUCT lba_data;    
    
    if((!((lba_info_ptr->starting_lba<msc_obj_ptr->device_info.total_lba_device_supports)&&
        (lba_info_ptr->lba_transfer_num <= (msc_obj_ptr->device_info.total_lba_device_supports - 
        lba_info_ptr->starting_lba)))) || (msc_obj_ptr->param_callback.callback == NULL))
    {
        /* host trying to access region beyond MASS STORAGE SPACE 
           Or no param_callback is registered */
        return(uint_8)((direction?USBERR_TX_FAILED:USBERR_RX_FAILED));
    }

    msc_obj_ptr->transfer_remaining = lba_info_ptr->lba_transfer_num * 
        msc_obj_ptr->device_info.length_of_each_lab_of_device;              
    msc_obj_ptr->current_offset = lba_info_ptr->starting_lba * 
        msc_obj_ptr->device_info.length_of_each_lab_of_device;
    
    lba_data.offset = msc_obj_ptr->current_offset;

    if(direction == USB_SEND)
    {
        lba_data.size = (msc_obj_ptr->transfer_remaining > msc_obj_ptr->msd_buff.msc_lba_send_buff_size) ? 
            msc_obj_ptr->msd_buff.msc_lba_send_buff_size : msc_obj_ptr->transfer_remaining; /* whichever is smaller */
        lba_data.buff_ptr = msc_obj_ptr->msd_buff.msc_lba_send_ptr;
        
        msc_obj_ptr->param_callback.callback(USB_MSC_DEVICE_READ_REQUEST,
            (void*)&lba_data, msc_obj_ptr->param_callback.arg);

        error = USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle,lba_data.buff_ptr,lba_data.size);
    }
    else
    {
        lba_data.size = (msc_obj_ptr->transfer_remaining > msc_obj_ptr->msd_buff.msc_lba_recv_buff_size) ? 
            msc_obj_ptr->msd_buff.msc_lba_recv_buff_size : msc_obj_ptr->transfer_remaining; /* whichever is smaller */
        lba_data.buff_ptr = msc_obj_ptr->msd_buff.msc_lba_recv_ptr;
        
        error = USB_MSC_Bulk_Recv_Data(msc_obj_ptr->controller_handle,lba_data.buff_ptr,lba_data.size);
    }               
    return error;
}

#endif /*MSD_CONFIG*/
/* EOF */
