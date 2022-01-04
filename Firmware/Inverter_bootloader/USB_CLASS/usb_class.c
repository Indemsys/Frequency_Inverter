/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
******************************************************************************
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
**************************************************************************/ /*!
  *
  * @file usb_class.c
  *
  * @author
  *
  * @version
  *
  * @date
  *
  * @brief The file contains USB stack Class module implementation.
  *
  *****************************************************************************/
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "string.h"
#include "usb_class.h"      /* USB class Header File */
#include "usb_devapi.h"     /* USB device Header file */
#include "usb_framework.h"  /* USB framework module header file */
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Class callback pointer */
static USB_CLASS_CALLBACK g_class_callback = NULL;
/* save the device state before device goes to suspend state */
static INT16U g_device_state_before_suspend;
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void USB_Suspend_Service(PTR_USB_DEV_EVENT_STRUCT event);
static void USB_Resume_Service(PTR_USB_DEV_EVENT_STRUCT event);
static void USB_Stall_Service(PTR_USB_DEV_EVENT_STRUCT event);
static void USB_Sof_Service(PTR_USB_DEV_EVENT_STRUCT event);
static void USB_Reset_Service(PTR_USB_DEV_EVENT_STRUCT event);
static void USB_Error_Service(PTR_USB_DEV_EVENT_STRUCT event);
/*****************************************************************************
 * Local Variables
 *****************************************************************************/
/*****************************************************************************
* Local Functions - None
*****************************************************************************/
/*****************************************************************************
 * Global Functions
 *****************************************************************************/
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
  #pragma CODE_SEG DEFAULT
#endif
/**************************************************************************/ /*!
  *
  * @name  USB_Suspend_Service
  *
  * @brief The funtion is called when host suspends the USB port
  *
  * @param event     : Pointer to USB Event Structure
  *
  * @return None
  ******************************************************************************
  * Sets the device state as USB_STATE_SUSPEND
  *****************************************************************************/
static void USB_Suspend_Service
(
 PTR_USB_DEV_EVENT_STRUCT event /* [IN] Pointer to USB Event Structure */
 )
{
  
  /* Get the status of the device before suspend, so that on resume we can get back to the same state */
  (void)USB_Device_Get_Status(event->controller_ID, USB_STATUS_DEVICE_STATE, &g_device_state_before_suspend);
  /* Set the device state in the Device Layer to SUSPEND */
  (void)USB_Device_Set_Status(event->controller_ID, USB_STATUS_DEVICE_STATE, USB_STATE_SUSPEND);
  return;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Resume_Service
  *
  * @brief The funtion is called when host resumes the USB port
  *
  * @param event     : Pointer to USB Event Structure
  *
  * @return None
  ******************************************************************************
  * Restore the state of the device before suspend
  *****************************************************************************/
static void USB_Resume_Service
(
 PTR_USB_DEV_EVENT_STRUCT event /* [IN] Pointer to USB Event Structure */
 )
{
  
  INT16U device_state;
  (void)USB_Device_Get_Status(event->controller_ID, USB_STATUS_DEVICE_STATE, &device_state);
  if ( device_state == USB_STATE_SUSPEND )
  {
    /* 
       Set the device state in the Device Layer to the state before suspend 
    */
    (void)USB_Device_Set_Status(event->controller_ID, USB_STATUS_DEVICE_STATE, g_device_state_before_suspend);
  } 
  return;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Stall_Service
  *
  * @brief The funtion is called when endpoint is stalled
  *
  * @param event: Pointer to USB Event Structure
  *
  * @return None
  ******************************************************************************
  * This function sends STALL Packet for the endpoint to be stalled. Also, sets
  * the status of Endpoint as STALLED
  *****************************************************************************/
static void USB_Stall_Service
(
 PTR_USB_DEV_EVENT_STRUCT event /* [IN] Pointer to USB Event Structure */
 )
{
  
  if ( event->ep_num == CONTROL_ENDPOINT )
  {
    /* Update the Endpoint Status in the Device Layer to Idle */
    (void)USB_Device_Set_Status(event->controller_ID, (INT8U)(USB_STATUS_ENDPOINT | CONTROL_ENDPOINT | (event->direction << USB_COMPONENT_DIRECTION_SHIFT)), (INT16U)USB_STATUS_IDLE);
  } 
  return;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Sof_Service
  *
  * @brief The funtion is called when SOF flag is set (from ISR)
  *
  * @param event: Pointer to USB Event Structure
  *
  * @return None
  ******************************************************************************
  * This function is called when SOF token is received by controller. Updates
  * SOF Count status.
  *****************************************************************************/
static void USB_Sof_Service
(
 PTR_USB_DEV_EVENT_STRUCT event     /* [IN] Pointer to USB Event Structure */
 )
{
  
  INT16U sof_count;
  /* update SOF */
  sof_count = event->buffer_ptr[0];
  sof_count <<= SOF_HIGH_BYTE_SHIFT;
  sof_count |= event->buffer_ptr[1];
  /* write SOF to status */
  (void)USB_Device_Set_Status(event->controller_ID, USB_STATUS_SOF_COUNT,
                              sof_count);
  return;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Reset_Service
  *
  * @brief The funtion is called upon a bus reset event.
               Initializes the control endpoint.
  *
  * @param event: Pointer to USB Event Structure
  *
  * @return None
  ******************************************************************************
  * Reset Callback function. This function re-initializes CONTROL Endpoint
  *****************************************************************************/
static void USB_Reset_Service
(
 PTR_USB_DEV_EVENT_STRUCT event  /* [IN] Pointer to USB Event Structure */
 )
{
  
  USB_EP_STRUCT ep_struct;
  //uint_8 err;
  /* Initialize the endpoint 0 in both directions */
  ep_struct.direction = USB_SEND;
  ep_struct.ep_num = CONTROL_ENDPOINT;
  ep_struct.size = CONTROL_MAX_PACKET_SIZE;
  ep_struct.type = USB_CONTROL_PIPE;
  /* Deinit Endpoint in case its already initialized */
  (void)USB_Device_Deinit_EndPoint(event->controller_ID, ep_struct.ep_num, ep_struct.direction);
  /* now initialize the endpoint */
  (void)USB_Device_Init_EndPoint(event->controller_ID, &ep_struct, TRUE);
  ep_struct.direction = USB_RECV;
  /* Deinit Endpoint in case its already initialized */
  (void)USB_Device_Deinit_EndPoint(event->controller_ID, ep_struct.ep_num, ep_struct.direction);
  /* now initialize the endpoint */
  (void)USB_Device_Init_EndPoint(event->controller_ID, &ep_struct, TRUE);
  /* set the default device state */
  (void)USB_Device_Set_Status(event->controller_ID, USB_STATUS_DEVICE_STATE, USB_STATE_DEFAULT);
  /* set the default device state */
  (void)USB_Device_Set_Status(event->controller_ID, USB_STATUS_DEVICE, SELF_POWERED >> SELF_POWER_BIT_SHIFT);
  /* set the EndPoint Status as Idle in the device layer */
  (void)USB_Device_Set_Status(event->controller_ID, (INT8U)(USB_STATUS_ENDPOINT | CONTROL_ENDPOINT | (USB_SEND << USB_COMPONENT_DIRECTION_SHIFT)), USB_STATUS_IDLE);
  /* let the application know that bus reset has taken place */
  g_class_callback(event->controller_ID, USB_APP_BUS_RESET, NULL);
  return;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Error_Service
  *
  * @brief The funtion is called when an error has been detected
  *
  * @param event: Pointer to USB Event Structure
  *
  * @return None
  ******************************************************************************
  * Calls application with the error code received from the lower layer
  *****************************************************************************/
static void USB_Error_Service
(
 PTR_USB_DEV_EVENT_STRUCT event /* [IN] Pointer to USB Event Structure */
 )
{
  
  /* notify the application of the error */
  g_class_callback(event->controller_ID, USB_APP_ERROR, (INT8U *)(&(event->errors)));
  return;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Class_Init
  *
  * @brief The funtion initializes the Class Module
  *
  * @param controller_ID         : Controller ID
  * @param class_callback        : Class callback
  * @param other_req_callback    : Other Requests Callback
  *
  * @return status
  *         USB_OK           : When Successfull
  *         Others           : Errors
  ******************************************************************************
  * Initializes USB Class Module
  *****************************************************************************/
INT8U USB_Class_Init
(
 INT8U    controller_ID,                /* [IN] Controller ID */
 USB_CLASS_CALLBACK class_callback,      /* [IN] Class Callback */
 USB_REQ_FUNC       other_req_callback   /* [IN] Other Requests Callback */
 )
{
  
  INT8U status = USB_Framework_Init(controller_ID, class_callback, other_req_callback);
  /* save callback address */
  g_class_callback = class_callback;
  if ( status == USB_OK )
  {
    /* Register all the services here */
    status |= USB_Device_Register_Service(controller_ID, USB_SERVICE_BUS_RESET, USB_Reset_Service);
    status |= USB_Device_Register_Service(controller_ID, USB_SERVICE_SOF,       USB_Sof_Service);
    status |= USB_Device_Register_Service(controller_ID, USB_SERVICE_SLEEP,     USB_Suspend_Service);
    status |= USB_Device_Register_Service(controller_ID, USB_SERVICE_RESUME,    USB_Resume_Service);
    status |= USB_Device_Register_Service(controller_ID, USB_SERVICE_STALL,     USB_Stall_Service);
    status |= USB_Device_Register_Service(controller_ID, USB_SERVICE_ERROR,     USB_Error_Service);
    /* set the device state as powered */
    (void)USB_Device_Set_Status(controller_ID, USB_STATUS_DEVICE_STATE, USB_STATE_POWERED);
    g_device_state_before_suspend = USB_STATE_POWERED;
  } 
  return status;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DeInit
  *
  * @brief The funtion De-initializes the Class Module
  *
  * @param controller_ID         : Controller ID
  *
  * @return status
  *         USB_OK           : When Successfull
  *         Others           : Errors
  ******************************************************************************
  * De-initializes USB Class Module
  *****************************************************************************/
INT8U USB_Class_DeInit
(
 INT8U    controller_ID          /* [IN] Controller ID */
 )
{
  
  INT8U status = USB_OK;
  /* Free class_callback */
  g_class_callback = NULL;

  /* Unegister all the services here */
  status |= USB_Device_Unregister_Service(controller_ID, USB_SERVICE_BUS_RESET);
  status |= USB_Device_Unregister_Service(controller_ID, USB_SERVICE_SOF);
  status |= USB_Device_Unregister_Service(controller_ID, USB_SERVICE_SLEEP);
  status |= USB_Device_Unregister_Service(controller_ID, USB_SERVICE_RESUME);
  status |= USB_Device_Unregister_Service(controller_ID, USB_SERVICE_STALL);
  status |= USB_Device_Unregister_Service(controller_ID, USB_SERVICE_ERROR);

  if ( status == USB_OK )
  {
    /* Call Framework deinit function */
    status = USB_Framework_DeInit(controller_ID);
  } 
  return status;
} 
/**************************************************************************/ /*!
  *
  * @name  USB_Class_Send_Data
  *
  * @brief The funtion calls the device to send data upon recieving an IN token
  *
  * @param controller_ID     : Controller ID
  * @param ep_num            : Endpoint number
  * @param buff_ptr          : Buffer to send
  * @param size              : Length of transfer
  *
  * @return status
  *         USB_OK           : When Successfull
  *         Others           : Errors
  ******************************************************************************
  * Used by Application to send Data on USB Bus if not suspended
  *****************************************************************************/
INT8U USB_Class_Send_Data
(
 INT8U           controller_ID, /* [IN] Controller ID */
 INT8U           ep_num,        /* [IN] Endpoint number */
 INT8U_ptr       buff_ptr,      /* [IN] Buffer to send */
 INT16U  size           /* [IN] Length of the transfer */
 )
{
  
  INT8U status = USB_OK;
  INT16U device_state, state;

  (void)USB_Device_Get_Status(controller_ID, USB_STATUS_DEVICE_STATE,    &device_state);
  (void)USB_Device_Get_Status(controller_ID, USB_STATUS_DEVICE, &state);
  if ( (device_state == USB_STATE_SUSPEND) && (state & REMOTE_WAKEUP_STATUS_MASK) && (USB_Frame_Remote_Wakeup(controller_ID) == TRUE) )
  {
    __disable_interrupt();
    /* Resume the bus */
    USB_Device_Assert_Resume(controller_ID);
    device_state = USB_STATE_CONFIG;
    /* Set the device state in the Device Layer to DEFAULT */
    (void)USB_Device_Set_Status(controller_ID, USB_STATUS_DEVICE_STATE,  USB_STATE_CONFIG);
    __enable_interrupt();
  } 
  if ( device_state != USB_STATE_SUSPEND )
  {    /* if not suspended */
    status = USB_Device_Send_Data(controller_ID, ep_num, buff_ptr, size);
  } 
  return status;
} 
/**************************************************************************/ /*!
  *
  * @name   USB_Class_Periodic_Task
  *
  * @brief  The funtion calls for periodic tasks
  *
  * @param  None
  *
  * @return None
  ******************************************************************************
  * Called to check for any pending requests
  *****************************************************************************/
void USB_Class_Periodic_Task(void)
{
  
#ifdef DELAYED_PROCESSING
  USB_Framework_Periodic_Task();
#endif
} 

/* EOF */
