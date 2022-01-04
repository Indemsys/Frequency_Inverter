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
* $FileName: usb_audio.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains USB stack AUDIO class layer api header function.
*
*****************************************************************************/

#ifndef _USB_AUDIO_H
#define _USB_AUDIO_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define AUDIO_MAX_QUEUE_ELEMS  (4)

/* Code of bmRequest Type */
#define SET_REQUEST_ITF                 (0x21)
#define SET_REQUEST_EP                  (0x22)
#define GET_REQUEST_EP                  (0xA2)
#define GET_REQUEST_ITF                 (0xA1) 
 
 /* class specific requests */
#define AUDIO_CONTROL_INPUT_TERMINAL      (0x02)
#define AUDIO_CONTROL_OUTPUT_TERMINAL     (0x03)
#define AUDIO_CONTROL_FEATURE_UNIT        (0x06)

/* Audio Class Specific Request Codes */
#define REQUEST_CODE_UNDEFINED		   (0x00)
#define SET_CUR						   (0x01)
#define SET_MIN						   (0x02)
#define SET_MAX						   (0x03)
#define SET_RES						   (0x04)
#define SET_MEM						   (0x05)

#define GET_CUR						   (0x81)
#define GET_MIN						   (0x82)
#define GET_MAX						   (0x83)
#define GET_RES						   (0x84)
#define GET_MEM						   (0x85)
#define GET_STAT					   (0xFF)


/* Terminal control selector codes */
#define TE_CONTROL_UNDEFINED		   (0x00)
#define COPY_PROTECT_CONTROL		   (0x01)

/* feature unit control selector codes */
#define FU_CONTROL_UNDEFINED		   (0x00)
/* Feature Unit Control Selector codes */
#define MUTE_CONTROL                     (0x01)
#define VOLUME_CONTROL                   (0x02)
#define BASS_CONTROL                     (0x03)
#define MID_CONTROL                      (0x04)
#define TREBLE_CONTROL                   (0x05)
#define GRAPHIC_EQUALIZER_CONTROL        (0x06)
#define AUTOMATIC_GAIN_CONTROL           (0x07)
#define DELAY_CONTROL                    (0x08)
#define BASS_BOOST_CONTROL               (0x09)
#define LOUDNESS_CONTROL                 (0x0A)

/* Endpoint control selector codes */
#define EP_CONTROL_DEFINED			   (0x00)
#define SAMPLING_FREQ_CONTROL		   (0x01)
#define PITCH_CONTROL		   		   (0x02)

#define MAX_AUDIO_DEVICE                 (0x01)

/* command */
/* GET CUR COMMAND */
#define GET_CUR_MUTE_CONTROL						(0x8101)
#define GET_CUR_VOLUME_CONTROL						(0x8102)
#define GET_CUR_BASS_CONTROL						(0x8103)
#define GET_CUR_MID_CONTROL							(0x8104)
#define GET_CUR_TREBLE_CONTROL						(0x8105)
#define GET_CUR_GRAPHIC_EQUALIZER_CONTROL			(0x8106)
#define GET_CUR_AUTOMATIC_GAIN_CONTROL				(0x8107)
#define GET_CUR_DELAY_CONTROL						(0x8108)
#define GET_CUR_BASS_BOOST_CONTROL					(0x8109)
#define GET_CUR_LOUDNESS_CONTROL					(0x810A)

/* GET MIN COMMAND */
#define GET_MIN_VOLUME_CONTROL						(0x8202)
#define GET_MIN_BASS_CONTROL						(0x8203)
#define GET_MIN_MID_CONTROL							(0x8204)
#define GET_MIN_TREBLE_CONTROL						(0x8205)
#define GET_MIN_GRAPHIC_EQUALIZER_CONTROL			(0x8206)
#define GET_MIN_DELAY_CONTROL						(0x8208)

/* GET MAX COMMAND */
#define GET_MAX_VOLUME_CONTROL						(0x8302)
#define GET_MAX_BASS_CONTROL						(0x8303)
#define GET_MAX_MID_CONTROL							(0x8304)
#define GET_MAX_TREBLE_CONTROL						(0x8305)
#define GET_MAX_GRAPHIC_EQUALIZER_CONTROL			(0x8306)
#define GET_MAX_DELAY_CONTROL						(0x8308)

/* GET RES COMMAND */
#define GET_RES_VOLUME_CONTROL						(0x8402)
#define GET_RES_BASS_CONTROL						(0x8403)
#define GET_RES_MID_CONTROL							(0x8404)
#define GET_RES_TREBLE_CONTROL						(0x8405)
#define GET_RES_GRAPHIC_EQUALIZER_CONTROL			(0x8406)
#define GET_RES_DELAY_CONTROL						(0x8408)

/* SET CUR COMMAND */
#define SET_CUR_MUTE_CONTROL						(0x0101)
#define SET_CUR_VOLUME_CONTROL						(0x0102)
#define SET_CUR_BASS_CONTROL						(0x0103)
#define SET_CUR_MID_CONTROL							(0x0104)
#define SET_CUR_TREBLE_CONTROL						(0x0105)
#define SET_CUR_GRAPHIC_EQUALIZER_CONTROL			(0x0106)
#define SET_CUR_AUTOMATIC_GAIN_CONTROL				(0x0107)
#define SET_CUR_DELAY_CONTROL						(0x0108)
#define SET_CUR_BASS_BOOST_CONTROL					(0x0109)
#define SET_CUR_LOUDNESS_CONTROL					(0x010A)

/* SET MIN COMMAND */
#define SET_MIN_VOLUME_CONTROL						(0x0202)
#define SET_MIN_BASS_CONTROL						(0x0203)
#define SET_MIN_MID_CONTROL							(0x0204)
#define SET_MIN_TREBLE_CONTROL						(0x0205)
#define SET_MIN_GRAPHIC_EQUALIZER_CONTROL			(0x0206)
#define SET_MIN_DELAY_CONTROL						(0x0208)

/* SET MAX COMMAND */
#define SET_MAX_VOLUME_CONTROL						(0x0302)
#define SET_MAX_BASS_CONTROL						(0x0303)
#define SET_MAX_MID_CONTROL							(0x0304)
#define SET_MAX_TREBLE_CONTROL						(0x0305)
#define SET_MAX_GRAPHIC_EQUALIZER_CONTROL			(0x0306)
#define SET_MAX_DELAY_CONTROL						(0x0308)

/* SET RES COMMAND */
#define SET_RES_VOLUME_CONTROL						(0x0402)
#define SET_RES_BASS_CONTROL						(0x0403)
#define SET_RES_MID_CONTROL							(0x0404)
#define SET_RES_TREBLE_CONTROL						(0x0405)
#define SET_RES_GRAPHIC_EQUALIZER_CONTROL			(0x0406)
#define SET_RES_DELAY_CONTROL						(0x0408)

#define GET_CUR_COPY_PROTECT_CONTROL				(0x810B)

#define GET_CUR_SAMPLING_FREQ_CONTROL				(0x810C)
#define GET_MIN_SAMPLING_FREQ_CONTROL				(0x820C)
#define GET_MAX_SAMPLING_FREQ_CONTROL				(0x830C)
#define GET_RES_SAMPLING_FREQ_CONTROL				(0x840C)

#define GET_CUR_PITCH_CONTROL						(0x810D)
#define GET_MIN_PITCH_CONTROL						(0x820D)
#define GET_MAX_PITCH_CONTROL						(0x830D)
#define GET_RES_PITCH_CONTROL						(0x840D)

#define SET_CUR_COPY_PROTECT_CONTROL				(0x010B)

#define SET_CUR_SAMPLING_FREQ_CONTROL				(0x010C)
#define SET_MIN_SAMPLING_FREQ_CONTROL				(0x020C)
#define SET_MAX_SAMPLING_FREQ_CONTROL				(0x030C)
#define SET_RES_SAMPLING_FREQ_CONTROL				(0x040C)

#define SET_CUR_PITCH_CONTROL						(0x010D)


/*****************************************************************************
 * Local Functions
 *****************************************************************************/

 /******************************************************************************
 * Types
 *****************************************************************************/
 typedef uint_32 AUDIO_HANDLE;
 
 typedef struct _USB_AUDIO_UT_STRUCT
{
  uint_8          unit_id;      /* endpoint number         */
  uint_8          type;        /* type of endpoint        */
}USB_UT_STRUCT, *USB_UT_STRUCT_PTR;

typedef const struct _USB_AUDIO_UNITS 
{
   /* Number of terminal or Ferture Unit End point */
   uint_8         count;
   /* Array of terminal or Feature Unit */
   USB_UT_STRUCT  *epp;
}USB_AUDIO_UNITS; 
 
 typedef struct _app_data_struct 
{
    uint_8_ptr      data_ptr;         /* pointer to buffer       */            
    uint_32 data_size;                /* buffer size of endpoint */
}APP_DATA_STRUCT;
 
 /* structure to hold a request in the endpoint queue */
typedef struct _usb_class_audio_queue 
{
    _usb_device_handle handle;
    uint_8 channel;       
	APP_DATA_STRUCT app_data;
}USB_CLASS_AUDIO_QUEUE, *PTR_USB_CLASS_AUDIO_QUEUE;
 
/* USB class audio endpoint data */
  
typedef struct _usb_class_audio_endpoint 
{
    uint_8 endpoint; /* endpoint num */                    
    uint_8 type;     /* type of endpoint (interrupt, bulk or isochronous) */   
    uint_8 bin_consumer;/* the num of queued elements */
    uint_8 bin_producer;/* the num of de-queued elements */
    uint_8 queue_num;	/* the num of queue */
    USB_CLASS_AUDIO_QUEUE queue[AUDIO_MAX_QUEUE_ELEMS]; /* queue data */  
}USB_CLASS_AUDIO_ENDPOINT;

/* contains the endpoint data for non control endpoints */
typedef struct _usb_class_audio_endpoint_data 
{    
    uint_8 count;  /* num of non control endpoints */  
    /* contains the endpoint info */
    /*Allocate Memory In App Layer*/    
    USB_CLASS_AUDIO_ENDPOINT *ep;       
}USB_CLASS_AUDIO_ENDPOINT_DATA, *PTR_USB_CLASS_AUDIO_ENDPOINT_DATA;
 
 /* Strucutre holding AUDIO class state information*/
 typedef struct audio_device_struct
 {
   AUDIO_HANDLE audio_handle;
   USB_CLASS_HANDLE class_handle;
   _usb_device_handle controller_handle;
   uint_8 current_interface;
   USB_ENDPOINTS *usb_ep_data;
   USB_AUDIO_UNITS *usb_ut_data;
   USB_CLASS_AUDIO_ENDPOINT *ep;
   USB_CLASS_CALLBACK_STRUCT     audio_class_callback;
   USB_REQ_CALLBACK_STRUCT       vendor_req_callback;
   USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT mem_param_callback;
   USB_CLASS_CALLBACK_STRUCT     param_callback;
 }AUDIO_DEVICE_STRUCT, _PTR_ AUDIO_DEVICE_STRUCT_PTR;
 
 /* Structure used to configure Audio class by APP*/
 typedef struct audio_config_struct
 {
    uint_32 desc_endpoint_cnt;
    USB_ENDPOINTS *usb_ep_data;
    USB_AUDIO_UNITS *usb_ut_data;
    USB_CLASS_AUDIO_ENDPOINT *ep; 
    USB_CLASS_CALLBACK_STRUCT audio_class_callback;
    USB_REQ_CALLBACK_STRUCT       vendor_req_callback;
    USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT mem_param_callback;
    USB_CLASS_CALLBACK_STRUCT param_callback;
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr; 
 }AUDIO_CONFIG_STRUCT,_PTR_ AUDIO_CONFIG_STRUCT_PTR;
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_Audio_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param *handle: handle pointer to Identify the controller
 * @param hid_class_callback:   event callback      
 * @param vendor_req_callback:  vendor specific class request callback      
 * @param param_callback:       application params callback      
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the Audio Class layer and layers it is dependent on 
 *
 *****************************************************************************/
extern AUDIO_HANDLE USB_Class_Audio_Init(AUDIO_CONFIG_STRUCT_PTR audio_config_ptr /*[IN]*/);

/**************************************************************************//*!
 *
 * @name  USB_Class_Audio_Send_Data
 *
 * @brief This function is used to send data to the host
 *
 * @param handle          :   handle returned by USB_Class_Audio_Send_Data
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
extern uint_8 USB_Class_Audio_Send_Data
(
    AUDIO_HANDLE         handle,        /* [IN]*/
    uint_8             ep_num,        /* [IN]*/
    uint_8_ptr         buff_ptr,      /* [IN] buffer to send */      
    uint_32            size           /* [IN] length of the transfer */
);

/**************************************************************************//*!
 *
 * @name  USB_Class_Audio_Recv_Data
 *
 * @brief This function receives Data from Host.
 *
 * @param handle          :   handle returned by USB_Class_Audio_Recv_Data
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
extern uint_8 USB_Class_Audio_Recv_Data
(
    AUDIO_HANDLE          audio_handle,
    uint_8              ep_num,
    uint_8_ptr          buff_ptr,      /* [IN] buffer to recv */      
    uint_32             size           /* [IN] length of the transfer */
);

#endif

/* EOF */
