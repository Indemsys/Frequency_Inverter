/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: usb_host_audio.c$
* $Version : 3.8.5.0$
* $Date    : Aug-31-2012$
*
* Comments:
*
*   This file Contains the implementation of class driver for Audio devices.
*
*END************************************************************************/
#include "usb_host_audio.h"

static void usb_class_audio_cntrl_callback(pointer, pointer, uchar_ptr, uint_32, USB_STATUS);
static USB_STATUS check_valid_fu(USB_AUDIO_CTRL_DESC_FU_PTR,uint_8,uint_8);
static USB_STATUS check_valid_ep(USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR,uint_8);

LWEVENT_STRUCT USB_Event;
#define USB_EVENT_CTRL 0x01
#define USB_EVENT_DATA 0x02

/* usb audio feature command declaration */
static USB_AUDIO_COMMAND usb_audio_feature_commands[NUMBER_OF_FEATURE_COMMANDS] = 
{
    /* Mute commands */
    {FU_MUTE_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_MUTE, 1}, /* USB_AUDIO_GET_CUR_MUTE */
    {FU_MUTE_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_MUTE, 1}, /* USB_AUDIO_SET_CUR_MUTE */
    /* Volume commands */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_CUR_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_CUR_VOLUME */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_MIN_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_MIN_VOLUME */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_MAX_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_MAX_VOLUME */
    {FU_VOLUME_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_GET_RES_VOLUME */
    {FU_VOLUME_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_VOLUME, 2}, /* USB_AUDIO_SET_RES_VOLUME */
    /* Bass commands */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_CUR_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_CUR_BASS */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_MIN_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_MIN_BASS */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_MAX_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_MAX_BASS */
    {FU_BASS_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_GET_RES_BASS */
    {FU_BASS_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_BASS, 1}, /* USB_AUDIO_SET_RES_BASS */
    /* Mid commands */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_CUR_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_CUR_MID */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_MIN_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_MIN_MID */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_MAX_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_MAX_MID */
    {FU_MID_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_GET_RES_MID */
    {FU_MID_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_MID, 1}, /* USB_AUDIO_SET_RES_MID */
    /* Treble commands */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_CUR_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_CUR_TREBLE */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_MIN_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_MIN_TREBLE */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_MAX_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_MAX_TREBLE */
    {FU_TREBLE_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_GET_RES_TREBLE */
    {FU_TREBLE_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_TREBLE, 1}, /* USB_AUDIO_SET_RES_TREBLE */
    /* AGC commands */
    {FU_AGC_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_AGC, 1}, /* USB_AUDIO_GET_CUR_AGC */
    {FU_AGC_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_AGC, 1}, /* USB_AUDIO_SET_CUR_AGC */
    /* Delay commands */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_CUR_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_CUR_DELAY */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_MIN, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_MIN_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_MIN, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_MIN_DELAY */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_MAX, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_MAX_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_MAX, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_MAX_DELAY */
    {FU_DELAY_MASK, GET_REQUEST_ITF, GET_RES, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_GET_RES_DELAY */
    {FU_DELAY_MASK, SET_REQUEST_ITF, SET_RES, USB_AUDIO_CTRL_FU_DELAY, 1}, /* USB_AUDIO_SET_RES_DELAY */
    /* Bass boost commands */
    {FU_BASS_BOOST_MASK, GET_REQUEST_ITF, GET_CUR, USB_AUDIO_CTRL_FU_BASS_BOOST, 1}, /* USB_AUDIO_GET_CUR_BASS_BOOST */
    {FU_BASS_BOOST_MASK, SET_REQUEST_ITF, SET_CUR, USB_AUDIO_CTRL_FU_BASS_BOOST, 1}, /* USB_AUDIO_SET_CUR_BASS_BOOST */
};
/* usb audio endpoint command declaration */
static USB_AUDIO_COMMAND usb_audio_endpoint_commands[NUMBER_OF_ENDPOINT_COMMANDS] = 
{
    /* Pitch commands */
    {PITCH_MASK, GET_REQUEST_EP , GET_CUR, PITCH_CONTROL, 1}, /* USB_AUDIO_GET_CUR_PITCH */
    {PITCH_MASK, SET_REQUEST_EP, SET_CUR, PITCH_CONTROL, 1}, /* USB_AUDIO_SET_CUR_PITCH */
    /* Sampling frequency commands */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_CUR, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_CUR_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_CUR, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_CUR_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_MIN, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_MIN_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_MIN, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_MIN_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_MAX, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_MAX_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_MAX, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_MAX_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, GET_REQUEST_EP, GET_RES, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_GET_RES_SAMPLING_FREQ */
    {SAMPLING_FREQ_MASK, SET_REQUEST_EP, SET_RES, SAMPLING_FREQ_CONTROL, 3}, /* USB_AUDIO_SET_RES_SAMPLING_FREQ */
};
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_cntrl_callback
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This is the callback used when audio control information is sent or received.
*
*END*--------------------------------------------------------------------*/
static void usb_class_audio_cntrl_callback
(
    /* [IN] Unused */
    pointer     pipe,

    /* [IN] Pointer to the class interface instance */
    pointer     param,

    /* [IN] Data buffer */
    uchar_ptr   buffer,

    /* [IN] Length of buffer */
    uint_32     len,

    /* [IN] Error code (if any) */
    USB_STATUS  status
)
{ /* Body */
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR      if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_cntrl_callback");
#endif

    /*
    ** There is no need for USB_lock in the callback function, and there is also no
    ** need to check if the device is still attached (otherwise this callback
    ** would never have been called!
    */
    /* Get class interface handle, reset IN_SETUP and call callback */
    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)param;

    if (if_ptr->AUDIO_G.USER_CALLBACK) {
        if_ptr->AUDIO_G.USER_CALLBACK(pipe, if_ptr->AUDIO_G.USER_PARAM, buffer, len, status);
    } /* Endif */

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_cntrl_callback, SUCCESSFUL");
#endif
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_cntrl_common
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is used to send a control request.
*     It must be run with USB locked.
*
*END*--------------------------------------------------------------------*/
static USB_STATUS usb_class_audio_cntrl_common
(
    /* [IN] The communication device common command structure */
    AUDIO_COMMAND_PTR           com_ptr,

    /* [IN] The audio control interface */
    //AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ctrl_ptr,

    /* [IN] Bitmask of the request type */
    uint_8                    bmrequesttype,

    /* [IN] Request code */
    uint_8                    brequest,

    /* [IN] Value to copy into WVALUE field of the REQUEST */
    uint_16                   wvalue,

    /* [IN] Value to copy into WINDEX field of the REQUEST */
    uint_16                   wIndex,

    /* [IN] Length of the data associated with REQUEST */
    uint_16                   wlength,

    /* [IN] Pointer to data buffer used to send/recv */
    uchar_ptr                 data
)
{ /* Body */
    USB_AUDIO_GENERAL_CLASS_PTR      if_ptr;
    USB_SETUP                        req;
    USB_STATUS                       status = USBERR_NO_INTERFACE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_cntrl_common");
#endif

    if_ptr = (USB_AUDIO_GENERAL_CLASS_PTR) com_ptr->CLASS_PTR->class_intf_handle;

    /* Save the higher level callback and ID */
    if_ptr->USER_CALLBACK = com_ptr->CALLBACK_FN;
    if_ptr->USER_PARAM = com_ptr->CALLBACK_PARAM;

    /* Setup the request */
    req.BMREQUESTTYPE = bmrequesttype;
    req.BREQUEST = brequest;

    *(uint_16*)req.WINDEX = HOST_TO_LE_SHORT(wIndex);
    *(uint_16*)req.WVALUE = HOST_TO_LE_SHORT(wvalue);
    *(uint_16*)req.WLENGTH = HOST_TO_LE_SHORT(wlength);


    if (USB_STATUS_TRANSFER_QUEUED == (status = _usb_hostdev_cntrl_request(if_ptr->G.dev_handle, &req, data,
                                       usb_class_audio_cntrl_callback, if_ptr)))
        status = USB_OK;

#ifdef _HOST_DEBUG_
    if (!status)
        DEBUG_LOG_TRACE("usb_class_audio_cntrl_common, SUCCESSFUL");
    else
        DEBUG_LOG_TRACE("usb_class_audio_cntrl_common, FAILED");
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_valid_fu
* Returned Value : None
* Comments       :
*     Check whether attribute of feature valid or not.
*
*END*--------------------------------------------------------------------*/
static USB_STATUS check_valid_fu
(
    /* [IN] pointer to feature unit */
    USB_AUDIO_CTRL_DESC_FU_PTR fu_ptr,

    /* [IN] Control mask */
    uint_8 control_mask,

    uint_8 atribute_index
)
{
    USB_STATUS status = USBERR_ERROR;

    if (NULL!=fu_ptr)
    {
        if(atribute_index<(fu_ptr->bLength - 7))
        /* Call num_logical_chan is number of logical channels.
         * Compare atribute_index with (fu_ptr->bControlSize*(num_logical_chan + 1))= (fu_ptr->bLength -7)
         * to make sure atribute_index is valid element in bmaControls() array */
        {
            if (fu_ptr->bmaControls[atribute_index] & control_mask)
            {
                status = USB_OK;
            }
        }
    }
    return status;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_valid_ep
* Returned Value : None
* Comments       :
*     Check whether attribute of endpoint valid or not.
*
*END*--------------------------------------------------------------------*/
static USB_STATUS check_valid_ep
(
    /* [IN] pointer to isochonounous endpoint */
    USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR ep_ptr,

    /* [IN] control selector */
    uint_8 control_selector
)
{
    USB_STATUS status = USBERR_ERROR;

    if (NULL!=ep_ptr)
    {
        if (ep_ptr->bmAttributes && control_selector)
            status = USB_OK;
        else
            status = USBERR_INVALID_REQ_TYPE;
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_int_callback
* Returned Value : None
* Comments       :
*     Called on interrupt endpoint data reception.
*
*END*--------------------------------------------------------------------*/
static void usb_class_audio_int_callback
(
    /* [IN] pointer to pipe */
    _usb_pipe_handle  pipe,

    /* [IN] user-defined parameter */
    pointer           param,

    /* [IN] buffer address */
    pointer           buffer,

    /* [IN] length of data transferred */
    uint_32           len,

    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint_32           status
)
{ /* Body */
    CLASS_CALL_STRUCT_PTR         audio_instance = (CLASS_CALL_STRUCT_PTR) param;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_int_callback");
#endif
    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR) audio_instance->class_intf_handle;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_int_callback, SUCCESSFUL");
#endif

    if (NULL!=pipe)
    {
        if_ptr->interrupt_callback(pipe,param,buffer,len,status);
    }

    usb_class_audio_init_ipipe(audio_instance,if_ptr->interrupt_callback,if_ptr->interrupt_callback_param);

    _lwevent_set(&USB_Event, USB_EVENT_DATA);

} /*EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_init_ipipe
* Returned Value : Success as USB_OK
* Comments       :
*     Starts interrupt endpoint to poll for interrupt on specified device.
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_init_ipipe
(
    /* control interface instance */
    CLASS_CALL_STRUCT_PTR     audio_instance,

    /* Callback to application */
    tr_callback               user_callback,

    /* Callback parameter */
    pointer                   user_callback_param
)
{ /* Body */
    TR_INIT_PARAM_STRUCT           tr;
    USB_STATUS                     status = USBERR_NO_INTERFACE;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR  if_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_init_ipipe");
#endif

    USB_lock();
    /* Validity checking, always needed when passing data to lower API */
    if (usb_host_class_intf_validate(audio_instance)) {
        if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR) audio_instance->class_intf_handle;

        if_ptr->interrupt_callback = user_callback;
        if_ptr->interrupt_callback_param = user_callback_param;

        if (if_ptr->interrupt_pipe != NULL) {
            usb_hostdev_tr_init(&tr, (tr_callback) usb_class_audio_int_callback, (pointer) audio_instance);
            tr.G.RX_BUFFER = (uchar_ptr) &if_ptr->interrupt_buffer;
            tr.G.RX_LENGTH = sizeof(if_ptr->interrupt_buffer);

            if (USB_STATUS_TRANSFER_QUEUED == (status = _usb_host_recv_data(
                                                   if_ptr->AUDIO_G.G.host_handle,
                                                   if_ptr->interrupt_pipe,
                                                   &tr
                                               )))
            {
                status = USB_OK;
            }
        }
        else
            status = USBERR_OPEN_PIPE_FAILED;
    }/* Endif */

    USB_unlock();

#ifdef _HOST_DEBUG_
    if (!status)
        DEBUG_LOG_TRACE("usb_class_audio_init_ipipe, SUCCESSFUL");
    else
        DEBUG_LOG_TRACE("usb_class_audio_init_ipipe, FAILED");
#endif

    return status;
} /* End Body */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_audio_recv_data
* Returned Value : USB status
* Comments       :
*     Receive data on isochronous IN pipe.
*
*END*--------------------------------------------------------------------*/
USB_STATUS usb_audio_recv_data
(
    /* [IN] class-interface control pointer */
    CLASS_CALL_STRUCT_PTR   control_ptr,

    /* [IN] class-interface stream pointer */
    CLASS_CALL_STRUCT_PTR   stream_ptr,

    /* [IN] callback upon completion */
    tr_callback             callback,

    /* [IN] user parameter returned by callback */
    pointer                 call_parm,

    /* [IN] data length */
    uint_32                 buf_size,

    /* [IN] buffer pointer */
    uchar_ptr               buffer
)
{ /* Body */
    AUDIO_STREAM_INTERFACE_STRUCT_PTR   stream_if_ptr;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR  control_if_ptr;
    TR_INIT_PARAM_STRUCT                tr;
    USB_STATUS                          status = USBERR_NO_INTERFACE;

    USB_lock();
    /* Validity checking, always needed when passing data to lower API */
    if (!usb_host_class_intf_validate(control_ptr)) {
        USB_unlock();
        return USBERR_NO_INTERFACE;
    }
    stream_if_ptr = (AUDIO_STREAM_INTERFACE_STRUCT_PTR) stream_ptr->class_intf_handle;
    control_if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR) control_ptr->class_intf_handle;

    /* Do the device use OUT pipe? */
    if (stream_if_ptr->iso_in_pipe == NULL) {
        USB_unlock();
        return USBERR_OPEN_PIPE_FAILED;
    }

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_host_class_intf_validate(control_ptr)) {
        USB_unlock();
        return USBERR_NO_INTERFACE;
    }

    usb_hostdev_tr_init(&tr, (tr_callback) callback, (pointer) call_parm);
    tr.G.RX_BUFFER = (uchar_ptr) buffer;
    tr.G.RX_LENGTH = buf_size;

    status = _usb_host_recv_data(control_if_ptr->AUDIO_G.G.host_handle,
                                 stream_if_ptr->iso_in_pipe, &tr);

    USB_unlock();

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_audio_send_data
* Returned Value : USB status
* Comments       :
*     Send audio data to device through a isochronous OUT pipe.
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_audio_send_data
(
    /* [IN] audio control class interface pointer  */
    CLASS_CALL_STRUCT_PTR   control_ptr,

    /* [IN] audio stream class interface pointer  */
    CLASS_CALL_STRUCT_PTR   stream_ptr,

    /* [IN] callback upon completion */
    tr_callback             callback,

    /* [IN] user parameter returned by callback */
    pointer                 call_parm,

    /* [IN] data length */
    uint_32                 buf_size,

    /* [IN] buffer pointer */
    uchar_ptr               buffer
)
{ /* Body */
    AUDIO_STREAM_INTERFACE_STRUCT_PTR   stream_if_ptr;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR  control_if_ptr;
    TR_INIT_PARAM_STRUCT                tr;
    USB_STATUS                          status = USBERR_NO_INTERFACE;

    USB_lock();
    /* Validity checking, always needed when passing data to lower API */
    if (!usb_host_class_intf_validate(control_ptr)) {
        USB_unlock();
        return USBERR_NO_INTERFACE;
    }
    stream_if_ptr = (AUDIO_STREAM_INTERFACE_STRUCT_PTR) stream_ptr->class_intf_handle;
    control_if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR) control_ptr->class_intf_handle;

    /* Do the device use OUT pipe? */
    if (stream_if_ptr->iso_out_pipe == NULL) {
        USB_unlock();
        return USBERR_OPEN_PIPE_FAILED;
    }

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_host_class_intf_validate(control_ptr)) {
        USB_unlock();
        return USBERR_NO_INTERFACE;
    }

    usb_hostdev_tr_init(&tr, (tr_callback) callback, (pointer) call_parm);
    tr.G.TX_BUFFER = (uchar_ptr) buffer;
    tr.G.TX_LENGTH = buf_size;

    status = _usb_host_send_data(control_if_ptr->AUDIO_G.G.host_handle, stream_if_ptr->iso_out_pipe, &tr);

    USB_unlock();

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_copy_protect
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get copy protect value
*     from the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_get_copy_protect
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] Buffer to receive data */
    pointer                    buf
)
{ /* Body */

    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    uint_16 wIndex;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->it_desc->bTerminalID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* check valid of input terminal descriptor */
    if (if_ptr->it_desc==NULL)
    {
        return USBERR_NO_DESCRIPTOR;
    }
    else
    {
        return usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_CUR,
                                            (uint_16)((COPY_PROTECT_CONTROL << 8)),wIndex,1,buf);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_copy_protect
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send copy protect value
*     to the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_set_copy_protect
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] Buffer to send data */
    pointer                    buf
)
{ /* Body */

    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    uint_16 wIndex;
    USB_AUDIO_CTRL_DESC_OT_PTR ot_desc;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    ot_desc = (USB_AUDIO_CTRL_DESC_OT_PTR)if_ptr->ot_desc;

    wIndex = (uint_16)(((ot_desc->bTerminalID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* check valid of input terminal descriptor */
    if (ot_desc==NULL)
    {
        return USBERR_NO_DESCRIPTOR;
    }
    else
    {
        return usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_CUR,
                                            (uint_16)((COPY_PROTECT_CONTROL << 8)),wIndex,1,buf);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_feature_command
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get or set feature units 
*     of audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_feature_command
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to send data */
    pointer                      buf,
    /* [IN] The command to perform */
    uint_32                      cmd
)
{ /* Body */
    uint_16 wIndex;
    uint_16 request_value;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_AUDIO_COMMAND_PTR p_feature_command;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    /* Validity checking: for audio interface */
    status = usb_host_class_intf_validate(if_ptr);
    if (USB_OK == status)
    {
        /* pointer to command */
        p_feature_command = &(usb_audio_feature_commands[cmd]);
        /* get request value */
        request_value = (uint_16)((p_feature_command->request_value << 8) | channel_no);
        wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

        /* Check whether this attribute valid or not */
        status = check_valid_fu(if_ptr->fu_desc,p_feature_command->control_mask,if_ptr->fu_desc->bControlSize*channel_no);

        if (USB_OK == status)
        {
            status = usb_class_audio_cntrl_common(com_ptr,
                                                  p_feature_command->request_type,
                                                  p_feature_command->request_code,
                                                  request_value,
                                                  wIndex,
                                                  p_feature_command->length,
                                                  buf);
        }
    }
    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_endpoint_command
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to performs services
*      for endpoint
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_endpoint_command
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] Buffer to receive data */
    pointer                      buf,
    /* [IN] The command to perform */
    uint_32                      cmd
)
{
    uint_8 Endp_num;
    AUDIO_STREAM_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;
    uint_16 request_value;
    USB_AUDIO_COMMAND_PTR p_endpoint_command;

    if_ptr = (AUDIO_STREAM_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    
    /* pointer to command */
    p_endpoint_command = &(usb_audio_endpoint_commands[cmd]);
    /* get request value */
    request_value     = (uint_16)((p_endpoint_command->request_value << 8));
    
    /* Check whether this attribute valid or not */
    status = check_valid_ep(if_ptr->iso_endp_spec_desc,p_endpoint_command->control_mask);

    if (USB_OK == status)
    {
        /* Any isochronous pipe is supported? */
        if ((NULL == if_ptr->iso_in_pipe) && (NULL == if_ptr->iso_out_pipe))
        {
            return USBERR_INVALID_NUM_OF_ENDPOINTS;
        }
        else if (NULL!=if_ptr->iso_in_pipe)
        {
            Endp_num = (if_ptr->iso_in_pipe->ENDPOINT_NUMBER | 0x80);
        }
        else
        {
            Endp_num = if_ptr->iso_out_pipe->ENDPOINT_NUMBER;
        }/* Endif */

        status = usb_class_audio_cntrl_common(com_ptr,
                                              p_endpoint_command->request_type,
                                              p_endpoint_command->request_code,
                                              request_value,
                                              (uint_16)Endp_num,
                                              p_endpoint_command->length,
                                              buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_cur_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get current graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_get_cur_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to receive data */
    pointer                      buf
)
{ /* Body */

    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_CUR,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_cur_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send current graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_set_cur_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to send data */
    pointer                      buf
)
{ /* Body */
    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_CUR,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_min_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get min graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_get_min_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to receive data */
    pointer                      buf
)
{ /* Body */

    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_MIN,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_min_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send min graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_set_min_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to send data */
    pointer                      buf
)
{ /* Body */
    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_MIN,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_max_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get max graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_get_max_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to receive data */
    pointer                      buf
)
{ /* Body */

    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_MAX,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_max_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send max graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_set_max_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to send data */
    pointer                      buf
)
{ /* Body */
    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_MAX,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_res_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get resolution graphic equalizer value
*     from the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_get_res_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to receive data */
    pointer                      buf
)
{ /* Body */

    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_ITF,GET_RES,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_res_graphic_eq
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send resolution graphic equalizer value
*     to the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_set_res_graphic_eq
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR            com_ptr,

    /* [IN] length of the Buffer */
    uint_16                      blen,

    /* [IN] channel number */
    uint_8                       channel_no,

    /* [IN] Buffer to send data */
    pointer                      buf
)
{ /* Body */
    uint_16 wIndex;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
    wIndex = (uint_16)(((if_ptr->fu_desc->bUnitID)<<8)|(if_ptr->AUDIO_G.IFNUM));

    /* Check whether this attribute valid or not */
    status = check_valid_fu(if_ptr->fu_desc,FU_GRAPHIC_EQ_MASK,if_ptr->fu_desc->bControlSize*channel_no);

    if (USB_OK == status)
    {
        status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_ITF,SET_RES,
                                            (uint_16)((USB_AUDIO_CTRL_FU_GRAPHIC_EQ << 8) | channel_no),wIndex,blen,buf);
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_mem_endpoint
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to get memory of the endpoint
*     from the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_get_mem_endpoint
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR          com_ptr,

    /* [IN] zero-based offset value */
    uint_16                    offset,

    /* [IN] length of the Buffer */
    uint_16                    blen,

    /* [IN] Buffer to receive data */
    pointer                    buf
)
{ /* Body */

    uint_8 Endp_num;
    AUDIO_STREAM_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_STREAM_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;

    /* Any isochronous pipe is supported? */
    if ((NULL == if_ptr->iso_in_pipe) && (NULL == if_ptr->iso_out_pipe))
    {
        return USBERR_INVALID_NUM_OF_ENDPOINTS;
    }
    else if (if_ptr->iso_in_pipe!=NULL)
    {
        Endp_num = if_ptr->iso_in_pipe->ENDPOINT_NUMBER;
    }
    else
    {
        Endp_num = if_ptr->iso_out_pipe->ENDPOINT_NUMBER;
    }/* Endif */

    status = usb_class_audio_cntrl_common(com_ptr,GET_REQUEST_EP,GET_MEM,
                                         offset,(uint_16)Endp_num,blen,buf);

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_set_mem_endpoint
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send memory of the endpoint
*     to the audio device
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_set_mem_endpoint
(
    /* [IN] Class Interface structure pointer */
    AUDIO_COMMAND_PTR           com_ptr,

    /* [IN] zero-based offset value */
    uint_16                     offset,

    /* [IN] length of the Buffer */
    uint_16                     blen,

    /* [IN] Buffer to send data */
    pointer                     buf
)
{ /* Body */

    uint_8 Endp_num;
    AUDIO_STREAM_INTERFACE_STRUCT_PTR     if_ptr;
    USB_STATUS status = USBERR_ERROR;

    if_ptr = (AUDIO_STREAM_INTERFACE_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;

    /* Any isochronous pipe is supported? */
    if ((NULL == if_ptr->iso_in_pipe) && (NULL == if_ptr->iso_out_pipe))
    {
        return USBERR_INVALID_NUM_OF_ENDPOINTS;
    }
    else if (if_ptr->iso_in_pipe!=NULL)
    {
        Endp_num = if_ptr->iso_in_pipe->ENDPOINT_NUMBER;
    }
    else
    {
        Endp_num = if_ptr->iso_out_pipe->ENDPOINT_NUMBER;
    }/* Endif */

    status = usb_class_audio_cntrl_common(com_ptr,SET_REQUEST_EP,SET_MEM,
                                         offset,(uint_16)Endp_num,blen,buf);

    return status;
}

/* EOF */
