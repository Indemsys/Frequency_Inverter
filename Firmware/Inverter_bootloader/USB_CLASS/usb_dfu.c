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
  * @file usb_dfu.c
  *
  * @author
  *
  * @version
  *
  * @date
  *
  * @brief The file contains USB stack DFU layer implementation.
  *
  *****************************************************************************/

#include "app_cfg.h"

#include "usb_dfu.h"            /* USB DFU Class Header File */
#include "usb_devapi.h"         /* USB device Header File */
#include "USB_timer.h" /* USB RTC Header File */


extern void  Loader_init(void);
extern INT8U Flash_recv_buff(INT8U *dfu_recv_buff, INT32U dfu_data_size);

void         USB_Class_DFU_task(void);
void         DFU_set_poll_delay(INT32U timeout);
static void  USB_Class_DFU_Process_bwPoll_TimeOut(void);
static INT8U USB_Class_DFU_Getstatus(INT8U controller_ID, INT8U interface, INT8U_ptr *data, INT16U *size);
static INT8U USB_Class_DFU_Getstate(INT8U controller_ID, INT8U interface, INT8U_ptr *data, INT16U *size);
static INT8U USB_Other_Requests(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_App_Detach_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Idle_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Dnload_Sync_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Dnbusy_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Dnload_Idle_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Manifest_Sync_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Manifest_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Manifest_Wait_Reset_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Upload_Idle_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);
static INT8U DFU_Error_State(INT8U controller_ID, USB_SETUP_STRUCT *setup_packet, INT8U_ptr *data, INT16U *size);


INT8U USB_Class_DFU_receive_block(INT8U controller_ID, INT16U wBlockNum, INT8U_ptr *data, INT16U *size);
INT8U USB_Class_DFU_transmit_block(INT8U controller_ID, INT16U BlockNum, INT8U_ptr *data, INT16U *size, INT16U wLength);


static USB_CLASS_CALLBACK               g_dfu_class_callback  = NULL;      /* DFU Class Callback Function Pointer */
static USB_REQ_FUNC                     g_vendor_req_callback = NULL;       /* DFU Class Vendor Callback Function Pointer */
static USB_CLASS_SPECIFIC_HANDLER_FUNC  g_param_callback      = NULL;      /* DFU Class requests Callback Function Pointer */
static INT8U                            g_dfu_state;                       /* state of DFU device */
static INT8U                            g_dfu_status[GETSTATUS_DATA_SIZE] =
{ 0x00,              // bStatus, An indication of the status resulting from the execution of the most recent request.
  POLLTIMEOUT, 0, 0,   // bwPollTimeout (3 byte). Minimum time, in milliseconds, that the host should wait before sending a subsequent DFU_GETSTATUS request. *
  0x00,              // bState.  An indication of the state that the device is going to enter immediately following transmission of this response.
  0x00               // iString. Index of status description in string table.
};                             /* status of DFU device */
static INT16U                           wlength;                           /* length of data */
static BOOLEAN                          g_block_flashing_done = FALSE;       /* Flag to check download block process complete or not */
static BOOLEAN                          g_flash_manifest_done = FALSE;    /* Flag to check manifest phase complete or not */
static TIMER_OBJECT                     bwPoll_TimeOut;                    /* Poll Timeout */

extern INT8U  g_dfu_flash_result;
static INT8U  dfu_recv_buff[DFU_RECV_BUFFER_LENGTH];  // Накопительный буффер при приеме данных от хоста
static INT16U dfu_last_block_num;                     // Номер последнего принятого блока
static INT32U dfu_firmware_size;                      // Текущий размер загруженного или читаемого фирмваре
static INT8U  dfu_buff_ready;                         // Накопительный буффер готов к парсингу и программированию
static INT32U dfu_buff_indx;                          // Индекс текущей позиции в буфере приема DFU
static INT32U dfu_data_size;                          // Количество данных в буфере для программирования



/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_Process_bwPoll_TimeOut
  *
  * @brief  To check timeout of block download process and manifest phase
  *
  *
  * @return :  None
  *
  ******************************************************************************
  *
  *****************************************************************************/
static void USB_Class_DFU_Process_bwPoll_TimeOut()
{
  if ( g_dfu_state == USB_DFU_DNBUSY ) g_dfu_state = USB_DFU_DNLOAD_SYNC;
  if ( g_dfu_state == USB_DFU_MANIFEST )
  {
#if(MANIFESTATION)
    g_dfu_state = USB_DFU_MANIFEST_SYNC;
#else
    g_dfu_state = USB_DFU_MANIFEST_WAIT_RESET;
#endif
  }
}
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_App_SetIdle
  *
  * @brief  This function is called to set device state is Idle
  *
  *
  * @return : status
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *****************************************************************************/

void USB_Class_DFU_App_SetIdle(void)
{
  g_dfu_state = USB_APP_IDLE;
}
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_App_SetDetach
  *
  * @brief  This function is called to set device state is Detach
  *
  *
  * @return : status
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  ******************************************************************************/
void USB_Class_DFU_App_SetDetach(void)
{
  g_dfu_state = USB_APP_DETACH;
}
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_App_Getstate
  *
  * @brief  This function is called by App layer to get state from DFU class
  *
  *
  * @return :  state of device
  *
  *****************************************************************************/

INT8U USB_Class_DFU_App_Getstate(void)
{
  return g_dfu_state;
}
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_App_Setstate
  *
  * @brief  This function is called when detect USB reset signal
  *
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
void USB_Class_DFU_App_Set_Usbreset(void)
{
  switch (g_dfu_state)
  {
  case USB_APP_DETACH:
    g_dfu_state = USB_DFU_IDLE;
    break;
  default:
    if ( !g_flash_manifest_done )
    {
      g_dfu_state = USB_DFU_ERROR;
    }
    break;
  }
}
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_App_Set_Flag_Manifest
  *
  * @brief  This function is called when app layer need set Flag to inform
  *
  * completed manifest phase
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
void USB_Class_DFU_App_Set_Flag_Manifest(void)
{
  g_flash_manifest_done = TRUE;
}
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_App_Reset_Flag_Manifest
  *
  * @brief  This function is called when app layer need set Flag to inform
  *
  * completed manifest phase
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
void USB_Class_DFU_App_Reset_Flag_Manifest(void)
{
  g_flash_manifest_done = FALSE;
}
/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_App_Set_Flag_Manifest
  *
  * @brief  This function is called when app layer need set Flag to inform
  *
  * completed manifest phase
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
BOOLEAN USB_Class_DFU_App_Get_Flag_Manifest(void)
{
  return g_flash_manifest_done;
}

/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_Getstatus
  *
  * @brief   The funtion gets status of device and generate timeout for
  *          transition between phases.
  *
  * @param controller_ID     : Controller ID
  * @param interface         : Interface
  * @param data              : Pointer to data
  * @param size              : Pointer to size of data
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        USBERR_INVALID_REQ_TYPE : When  request for invalid
  *                                Interface is presented
  ******************************************************************************
  * Return status of device
  *****************************************************************************/
static INT8U USB_Class_DFU_Getstatus(
                                     INT8U controller_ID,               /* [IN] Controller ID */
                                     INT8U interface,                   /* [IN] Interface */
                                     INT8U_ptr *data,                   /* [OUT] Pointer to Data */
                                     INT16U *size               /* [OUT] Pointer to Size of Data */
                                     )
{
  UNUSED(controller_ID)
  /* if interface valid */
  if ( interface < USB_MAX_SUPPORTED_INTERFACES )
  {
    /* get status data*/
    *size = GETSTATUS_DATA_SIZE;
    *data = g_dfu_status;
    return USB_OK;
  }
  return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_Getstate
  *
  * @brief   The funtion gets state of device
  *
  * @param controller_ID     : Controller ID
  * @param interface         : Interface
  * @param data              : Pointer to data
  * @param size              : Pointer to size of Data
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        USBERR_INVALID_REQ_TYPE : When  request for invalid
  *                                Interface is presented
  ******************************************************************************
  * Return state of device.
  *****************************************************************************/
static INT8U USB_Class_DFU_Getstate(
                                    INT8U controller_ID,               /* [IN] Controller ID */
                                    INT8U interface,                   /* [IN] Interface */
                                    INT8U_ptr *data,                   /* [OUT] Pointer to Data */
                                    INT16U *size               /* [OUT] Pointer to Size of Data */
                                    )
{
  UNUSED(controller_ID)
  /* if interface valid */
  if ( interface < USB_MAX_SUPPORTED_INTERFACES )
  {
    /* get state data*/
    *size = GETSTATE_DATA_SIZE;
    *data = &g_dfu_state;
    return USB_OK;
  }
  return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************/ /*!
  *
  * @name  USB_Other_Requests
  *
  * @brief   The funtion provides flexibilty to add class and vendor specific
  *              requests
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  ******************************************************************************
  * Handles DFU Class requests and forwards vendor specific request to the
  * application
  *****************************************************************************/
static INT8U USB_Other_Requests(
                                INT8U controller_ID,               /* [IN] Controller ID */
                                USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
                                INT8U_ptr *data,                   /* [OUT] Data to be send back */
                                INT16U *size               /* [OUT] Size to be returned*/
                                )
{
  INT8U status = USBERR_INVALID_REQ_TYPE;
  /* Check size NULL */
  if ( size == NULL )
  {
    return USBERR_GET_MEMORY_FAILED;
  }
  *size = 0;
  if ( (setup_packet->request_type & USB_REQUEST_CLASS_MASK) == USB_REQUEST_CLASS_CLASS )
  {
    g_dfu_status[BSTATE] = g_dfu_state;
    switch (g_dfu_state)
    {
      /* State1: App_Detach */
    case USB_APP_DETACH:
      status = DFU_App_Detach_State(controller_ID, setup_packet, data, size);
      break;
      /* State2: Dfu_Idle */
    case USB_DFU_IDLE:
      status = DFU_Idle_State(controller_ID, setup_packet, data, size);
      break;
      /* State3: Dfu_Dnload_Sync */
    case USB_DFU_DNLOAD_SYNC:
      status = DFU_Dnload_Sync_State(controller_ID, setup_packet, data, size);
      break;
      /* State4: Dfu_Dnbusy */
    case USB_DFU_DNBUSY:
      /*FSL: change to allow fast connections: no more*/ /*FSL: as it was from the begging*/
      //********status = DFU_Dnbusy_State(controller_ID, setup_packet, data, size);// -> Earlier change
      status = DFU_Upload_Idle_State(controller_ID, setup_packet, data, size); // -> 8/24/2012
      break;
      /* State5: Dfu_Dnload_Idle*/
    case USB_DFU_DNLOAD_IDLE:
      status = DFU_Dnload_Idle_State(controller_ID, setup_packet, data, size);
      break;
      /* State6: Dfu_Mani_Sync */
    case USB_DFU_MANIFEST_SYNC:
      status = DFU_Manifest_Sync_State(controller_ID, setup_packet, data, size);
      break;
      /* State7: Dfu_Mani */
    case USB_DFU_MANIFEST:
      status = DFU_Manifest_State(controller_ID, setup_packet, data, size);
      break;
      /* State8: Dfu_Mani_Wait_Reset */
    case USB_DFU_MANIFEST_WAIT_RESET:
      status = DFU_Manifest_Wait_Reset_State(controller_ID, setup_packet, data, size);
      break;
      /* State9: Dfu_Upload_Idle */
    case USB_DFU_UPLOAD_IDLE:
      status = DFU_Upload_Idle_State(controller_ID, setup_packet, data, size);
      break;
      /* State10: Dfu_Error */
    case USB_DFU_ERROR:
      status = DFU_Error_State(controller_ID, setup_packet, data, size);
      break;
    default:
      status = USB_OK;
      break;
    }
  }
  g_dfu_status[BSTATUS] = status;
  return status;
}
/**************************************************************************/ /*!
  *
  * @name  DFU_App_Detach_State
  *
  * @brief   The funtion processes requests at USB_APP_DETACH  state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
static INT8U DFU_App_Detach_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  INT8U status;
  INT8U interface;

  interface = (INT8U)setup_packet->index;
  switch (setup_packet->request)
  {
    /* DFU_GETSTATUS Request */
  case USB_DFU_GETSTATUS:
    status = USB_Class_DFU_Getstatus(controller_ID, interface, data, size);
    break;
    /* DFU_GETSTATE Request */
  case USB_DFU_GETSTATE:
    status = USB_Class_DFU_Getstate(controller_ID, interface, data, size);
    break;
  default:
    status = USBERR_INVALID_REQ_TYPE;
    g_dfu_state = USB_APP_IDLE;
    break;
  }
  return status;
}

/**************************************************************************/ /*!
  *
  * @name  DFU_Idle_State
  *
  * @brief   The funtion processes requests at USB_DFU_IDLE state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
static INT8U DFU_Idle_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  INT8U status; //,check_request_type;
  INT16U interface, BlockNum, wBlockNum;

  interface = setup_packet->index;
  BlockNum = setup_packet->value;
  wBlockNum = setup_packet->value;
  //check_request_type =(uint_8)setup_packet->request_type;
  *data = ((INT8U *)setup_packet) + USB_SETUP_PKT_SIZE;
  switch (setup_packet->request)
  {
    
  case USB_DFU_DNLOAD:
    
    wlength = setup_packet->length;

    if ( wlength > 0 )
    {
      (void)Remove_periodic_callback(0);

#if(BITCANDOWNLOAD)
      /* Start check download timeout */
      g_dfu_state = USB_DFU_DNLOAD_SYNC;
      g_block_flashing_done = FALSE;
      /* Call to application */
      status = USB_Class_DFU_receive_block(controller_ID, wBlockNum, data, &wlength);
      if ( USB_OK != status )
      {
        g_dfu_state = USB_DFU_ERROR;
      }
#else
      status = USBERR_INVALID_REQ_TYPE;
      ;
      g_dfu_state = USB_DFU_ERROR;
#endif
    }
    /* the last block */
    if ( wlength == 0 )
    {
      status = USBERR_INVALID_REQ_TYPE;
      ;
      g_dfu_state = USB_DFU_ERROR;
    }
    break;
    
  case USB_DFU_UPLOAD:
#if(BITCANUPLOAD)
    BlockNum = (INT8U)setup_packet->value;
    /* Call to application */
    status = USB_Class_DFU_transmit_block(controller_ID, BlockNum, data, size, setup_packet->length);
    if ( *size < setup_packet->length ) g_dfu_state = USB_DFU_IDLE;
    else g_dfu_state = USB_DFU_UPLOAD_IDLE;
#else
    status = USBERR_INVALID_REQ_TYPE;
    ;
    g_dfu_state = USB_DFU_ERROR;
#endif
    break;
    /* DFU_ABORT request */
  case USB_DFU_ABORT:
    status = USB_OK;
    break;
    /* DFU_GETSTATUS request */
  case USB_DFU_GETSTATUS:
    status = USB_Class_DFU_Getstatus(controller_ID, (INT8U)interface, data, size);
    break;
    /* DFU_GETSTATE request */
  case USB_DFU_GETSTATE:
    status = USB_Class_DFU_Getstate(controller_ID, (INT8U)interface, data, size);
    break;
  default:
    status = USBERR_INVALID_REQ_TYPE;
    g_dfu_state = USB_DFU_ERROR;
    break;
  }
  return status;
}

/**************************************************************************/ /*!
  *
  * @name  DFU_Dnload_Sync_State
  *
  * @brief   The funtion processes requests at USB_DFU_DNLOAD_SYNC  state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
static INT8U DFU_Dnload_Sync_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  INT8U status;
  INT8U interface;

  interface = (INT8U)setup_packet->index;
  switch (setup_packet->request)
  {
    /* DFU_GETSTATUS request */
  case USB_DFU_GETSTATUS:
    if ( g_block_flashing_done ) g_dfu_state = USB_DFU_DNLOAD_IDLE;
    else
    {
      /* Set time out for writing to flash*/
      (void)Remove_periodic_callback(0);
      g_block_flashing_done = FALSE;
      if ( g_dfu_status[BWPOLLTIMEOUT + 2] > 0 ) bwPoll_TimeOut.msCount = 0xFFFF; /* Max Poll Time Out */
      else bwPoll_TimeOut.msCount = (INT16U)(g_dfu_status[BWPOLLTIMEOUT] + (g_dfu_status[BWPOLLTIMEOUT + 1] << 8));
      bwPoll_TimeOut.pfnTimerCallback = USB_Class_DFU_Process_bwPoll_TimeOut;
      (void)Add_periodic_callback(&bwPoll_TimeOut);

      g_dfu_state = USB_DFU_DNBUSY;
    }
    g_dfu_status[BSTATE] = g_dfu_state;
    status = USB_Class_DFU_Getstatus(controller_ID, interface, data, size);
    break;
    /* DFU_GETSTATE request */
  case USB_DFU_GETSTATE:
    status = USB_Class_DFU_Getstate(controller_ID, interface, data, size);
    break;
  default:
    status = USBERR_INVALID_REQ_TYPE;
    g_dfu_state = USB_DFU_ERROR;
    break;
  }
  return status;
}
/**************************************************************************/ /*!
 *
 * @name  DFU_Dnbusy_State
 *
 * @brief   The funtion processes requests at USB_DFU_DNBUSY  state
 *
 * @param controller_ID     : Controller ID
 * @param setup_packet      : Setup packet received
 * @param data              : Data to be send back
 * @param size              : Size to be returned
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 *****************************************************************************/
static INT8U DFU_Dnbusy_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  UNUSED(controller_ID)
  UNUSED(setup_packet)
  UNUSED(data)
  UNUSED(size)
  g_dfu_state = USB_DFU_ERROR;
  return USBERR_INVALID_REQ_TYPE;
}
/**************************************************************************/ /*!
  *
  * @name  DFU_Dnload_Idle_State
  *
  * @brief   The funtion processes requests at USB_DFU_DNLOAD_IDLE   state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
static INT8U DFU_Dnload_Idle_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  INT8U status;
  INT8U interface; //, check_request_type;
  INT16U /*BlockNum,*/ wBlockNum;

  interface = (INT8U)setup_packet->index;
  //BlockNum = setup_packet->value;
  wBlockNum = setup_packet->value;
  //check_request_type =(uint_8)setup_packet->request_type;
  *data = ((INT8U *)setup_packet) + USB_SETUP_PKT_SIZE;

  switch (setup_packet->request)
  {
    /* DFU_DNLOAD request*/
  case USB_DFU_DNLOAD:

#if defined(_MC9S08JS16_H)
    wlength = (INT8U)setup_packet->length;
#else
    wlength = setup_packet->length;
#endif

    /* check wlength */
    if ( wlength > 0 )
    {
      g_dfu_state = USB_DFU_DNLOAD_SYNC;
      g_block_flashing_done = FALSE;
    }
    if ( wlength == 0 )
    {
      g_dfu_state = USB_DFU_MANIFEST_SYNC;
      status = USB_OK;
    }
    /* Call to application */
    status = USB_Class_DFU_receive_block(controller_ID, wBlockNum, data, &wlength);
    if ( USB_OK != status )
    {
      g_dfu_state = USB_DFU_ERROR;
    }
    break;
    /* DFU_ABORT request */
  case USB_DFU_ABORT:
    status = USB_OK;
    g_dfu_state = USB_DFU_IDLE;
    break;
    /* DFU_GETSTATUS request */
  case USB_DFU_GETSTATUS:
    status = USB_Class_DFU_Getstatus(controller_ID, interface, data, size);
    break;
    /* DFU_GETSTATE request */
  case USB_DFU_GETSTATE:
    status = USB_Class_DFU_Getstate(controller_ID, interface, data, size);
    break;
  default:
    status = USBERR_INVALID_REQ_TYPE;
    g_dfu_state = USB_DFU_ERROR;
    break;
  }
  return status;
}

/**************************************************************************/ /*!
  *
  * @name  DFU_Manifest_Sync_State
  *
  * @brief   The funtion processes requests at USB_DFU_MANIFEST_SYNC   state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  ******************************************************************************/
static INT8U DFU_Manifest_Sync_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  INT8U status;
  INT8U interface;

  interface = (INT8U)setup_packet->index;
  switch (setup_packet->request)
  {
    /* DFU_GETSTATUS request */
  case USB_DFU_GETSTATUS:
    if ( g_flash_manifest_done )
    {
#if(MANIFESTATION)
      g_dfu_state = USB_DFU_IDLE;
      g_dfu_status[BSTATE] = g_dfu_state;
      status = USB_Class_DFU_Getstatus(controller_ID, interface, data, size);
#endif
    }
    else
    {   /* poll time out for manifest process */
      (void)Remove_periodic_callback(0);
      if ( g_dfu_status[BWPOLLTIMEOUT + 2] > 0 ) bwPoll_TimeOut.msCount = 0xFFFF; /* Max Poll Time Out */
      else bwPoll_TimeOut.msCount = (INT16U)(g_dfu_status[BWPOLLTIMEOUT] + (g_dfu_status[BWPOLLTIMEOUT + 1] << 8));
      bwPoll_TimeOut.pfnTimerCallback = USB_Class_DFU_Process_bwPoll_TimeOut;
      (void)Add_periodic_callback(&bwPoll_TimeOut);
      g_dfu_state = USB_DFU_MANIFEST;
      g_dfu_status[BSTATE] = g_dfu_state;
      status = USB_Class_DFU_Getstatus(controller_ID, interface, data, size);

    }
    break;
    /* DFU_GETSTATE request */
  case USB_DFU_GETSTATE:
    status = USB_Class_DFU_Getstate(controller_ID, interface, data, size);
    break;
  default:
    status = USBERR_INVALID_REQ_TYPE;
    g_dfu_state = USB_DFU_ERROR;
    break;
  }
  return status;
}
/**************************************************************************/ /*!
  *
  * @name  DFU_Manifest_State
  *
  * @brief   The funtion processes requests at DFU_MANIFEST  state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
static INT8U DFU_Manifest_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  UNUSED(controller_ID);
  UNUSED(setup_packet);
  UNUSED(data);
  UNUSED(size);
  g_dfu_state = USB_DFU_ERROR;
  return USBERR_INVALID_REQ_TYPE;
}
/**************************************************************************/ /*!
  *
  * @name  DFU_Manifest_Wait_Reset_State
  *
  * @brief   The funtion processes requests at USB_DFU_MANIFEST_WAIT_RESET  state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
static INT8U DFU_Manifest_Wait_Reset_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  UNUSED(controller_ID);
  UNUSED(setup_packet);
  UNUSED(data);
  UNUSED(size);

  return USBERR_INVALID_REQ_TYPE;
  ;
}
/**************************************************************************/ /*!
  *
  * @name  DFU_Upload_Idle_State
  *
  * @brief   The funtion processes requests at USB_DFU_UPLOAD_IDLE    state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  ******************************************************************************/
static INT8U DFU_Upload_Idle_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  INT8U status;
  INT8U interface; //, check_request_type;
  INT16U BlockNum; //,wBlockNum;

  interface = (INT8U)setup_packet->index;
  BlockNum = setup_packet->value;
  //wBlockNum = setup_packet->value;
  //check_request_type =(uint_8)setup_packet->request_type;
  *data = ((INT8U *)setup_packet) + USB_SETUP_PKT_SIZE;

  switch (setup_packet->request)
  {
    /* DFU_UPLOAD Request */
  case USB_DFU_UPLOAD:
    status = USB_Class_DFU_transmit_block(controller_ID, BlockNum, data, size, setup_packet->length);
    if ( *size < setup_packet->length ) g_dfu_state = USB_DFU_IDLE;
    break;
    /* DFU_ABORT Request */
  case USB_DFU_ABORT:
    status = USB_OK;
    g_dfu_state = USB_DFU_IDLE;
    break;
    /* DFU_GETSTATUS Request */
  case USB_DFU_GETSTATUS:
    status = USB_Class_DFU_Getstatus(controller_ID, interface, data, size);
    break;
    /* DFU_GETSTATE Request */
  case USB_DFU_GETSTATE:
    status = USB_Class_DFU_Getstate(controller_ID, interface, data, size);
    break;
  default:
    status = USBERR_INVALID_REQ_TYPE;
    g_dfu_state = USB_DFU_ERROR;
    break;
  }
  return status;
}

/**************************************************************************/ /*!
  *
  * @name  DFU_Error_State
  *
  * @brief   The funtion processes requests at USB_DFU_ERROR    state
  *
  * @param controller_ID     : Controller ID
  * @param setup_packet      : Setup packet received
  * @param data              : Data to be send back
  * @param size              : Size to be returned
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  *****************************************************************************/
static INT8U DFU_Error_State
(
 INT8U controller_ID,               /* [IN] Controller ID */
 USB_SETUP_STRUCT *setup_packet,    /* [IN] Setup packet received */
 INT8U_ptr *data,                   /* [OUT] Data to be send back */
 INT16U *size               /* [OUT] Size to be returned*/
 )
{
  INT8U status;
  INT8U interface;

  interface = (INT8U)setup_packet->index;
  switch (setup_packet->request)
  {
    /* DFU_GETSTATUS Request */
  case USB_DFU_GETSTATUS:
    status = USB_Class_DFU_Getstatus(controller_ID, interface, data, size);
    break;
    /* DFU_GETSTATE Request */
  case USB_DFU_GETSTATE:
    status = USB_Class_DFU_Getstate(controller_ID, interface, data, size);
    break;
    /* DFU_CLRSTATUS Request */
  case USB_DFU_CLRSTATUS:
    status = USB_OK;
    g_dfu_state = USB_DFU_IDLE;
    break;
  default:
    status = USBERR_INVALID_REQ_TYPE;
    break;
  }

  return status;
}

/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_Event
  *
  * @brief The funtion initializes DFU endpoint
  *
  * @param controller_ID     : Controller ID
  * @param event             : Event Type
  * @param val               : Pointer to configuration Value
  *
  * @return None
  *
  ******************************************************************************
  * The funtion initializes the DFU endpoints when Enumeration complete event is
  * received
  *****************************************************************************/
static void USB_Class_DFU_Event
(
 INT8U controller_ID,   /* [IN] Controller ID */
 INT8U event,           /* [IN] Event Type */
 void *val               /* [IN] Pointer to configuration Value */
 )
{
  switch (event)
  {

  case USB_APP_BUS_RESET:
    if ( g_dfu_class_callback != NULL )
    {
      /* notify the application of the event */
      g_dfu_class_callback(controller_ID, event, val);
    }
    break;

  case BUS_POWERED:
    g_dfu_state = USB_APP_DETACH;
    break;
  }
}

/*****************************************************************************
 * Global Functions
 *****************************************************************************

 *****************************************************************************
 *
 * @name  USB_Class_DFU_Init
 *
 * @brief   The funtion initializes the Device and Controller layer
 *
 * @param   controller_ID               : Controller ID
 * @param   dfu_class_callback          : DFU Class Callback
 * @param   vendor_req_callback         : Vendor Request Callback
 * @param   param_callback              : Class requests Callback
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 ******************************************************************************
 *This function initializes the DFU Class layer and layers it is dependent on
 *****************************************************************************/
INT8U USB_Class_DFU_Init
(
 INT8U    controller_ID,                /* [IN] Controller ID */
 USB_CLASS_CALLBACK dfu_class_callback,  /* [IN] DFU Class Callback */
 USB_REQ_FUNC       vendor_req_callback, /* [IN] Vendor Request Callback */
 USB_CLASS_SPECIFIC_HANDLER_FUNC param_callback
 /* [ IN] DFU Class requests Callback */
 )
{
  USB_ENDPOINTS *ep_desc_data = (USB_ENDPOINTS *)USB_Desc_Get_Endpoints(controller_ID);

  /* Initialize the device layer*/
  INT8U status = USB_Device_Init(controller_ID, (INT8U)(ep_desc_data->count + 1));
  if ( status == USB_OK )
  {
    /* Initialize the generic class functions */
    status = USB_Class_Init(controller_ID, USB_Class_DFU_Event, USB_Other_Requests);

    if ( status == USB_OK )
    {
      /* save the DFU class callback pointer */
      g_dfu_class_callback = dfu_class_callback;

      /* save the vendor request callback pointer */
      g_vendor_req_callback = vendor_req_callback;

      /* Save the callback to ask application for class specific params*/
      g_param_callback = param_callback;
    }
  }
  return status;
}

/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_DeInit
  *
  * @brief   The funtion de-initializes the Device and Controller layer
  *
  * @param   controller_ID               : Controller ID
  *
  * @return status:
  *                        USB_OK        : When Successfull
  *                        Others        : When Error
  *
  ******************************************************************************
  *This function de-initializes the DFU Class layer
  *****************************************************************************/
INT8U USB_Class_DFU_DeInit
(
 INT8U controller_ID              /* [IN] Controller ID */
 )
{
  INT8U status;

  /* free the HID class callback pointer */
  g_dfu_class_callback = NULL;

  /* free the vendor request callback pointer */
  g_vendor_req_callback = NULL;

  /* free the callback to ask application for class specific params*/
  g_param_callback = NULL;

  /* Call common class deinit function */
  status = USB_Class_DeInit(controller_ID);

  if ( status == USB_OK )
    /* Call device deinit function */
    status = USB_Device_DeInit();

  return status;
}

/**************************************************************************/ /*!
  *
  * @name  USB_Class_DFU_Set_bwPoll_TimeOut
  *
  * @brief  To set timeout of block download process and manifest phase
  *
  *
  * @return :  None
  *
  ******************************************************************************
  *
  *****************************************************************************/
void DFU_set_poll_delay(INT32U app_timeout)
{
  g_dfu_status[BWPOLLTIMEOUT] = (INT8U)((app_timeout) & (0xFF));
  g_dfu_status[BWPOLLTIMEOUT + 1] = (INT8U)((app_timeout >> 8) & (0xFF));
  g_dfu_status[BWPOLLTIMEOUT + 2] = (INT8U)((app_timeout >> 16) & (0xFF));
}

//-------------------------------------------------------------------------------------------------------
//  Функция передачи блока данных от хоста
//
//  controller_ID     : Controller ID
//  bl_num            : порадковый номер принимаемого блока от начала чтения фирмваре
//  data              : Pointer to data
//  psize             : Реально отправленное количество данных
//
//  status:
//                  USB_OK : When Successfull
//                  Other  : When Error
//
//-------------------------------------------------------------------------------------------------------
INT8U USB_Class_DFU_receive_block(INT8U controller_ID, INT16U bl_num, INT8U_ptr *data, INT16U  *psize)
{

  INT32U i = 0;
  INT32U timeout = 0;
  UNUSED(controller_ID)

  if ( *psize != 0 )
  {
    if ( (bl_num * DFU_MAX_BLOCK_SIZE) > FIRMWARE_SIZE )
    {
      return USBERR_INVALID_REQ_TYPE;
    }
    dfu_last_block_num = bl_num;

    if ( bl_num == 0 )
    {
      // Первый блок
      timeout = ((MAX_FLASH1_ADDRESS - (INT32U)IMAGE_START_ADDR) / ERASE_SECTOR_SIZE) * 10;
      DFU_set_poll_delay(timeout); 
      (void)USB_Class_DFU_App_Reset_Flag_Manifest();
      dfu_firmware_size = 0;
      dfu_buff_indx     = 0;
      Loader_init();
    }
    else
    {
      // Последующие блоки
      DFU_set_poll_delay(1); 
    }

    for (i = 0; i < *psize; i++)
    {
      if ( dfu_buff_indx < DFU_RECV_BUFFER_LENGTH )
      {
        dfu_recv_buff[dfu_buff_indx] = *(*data + i);
        dfu_buff_indx++;
      }
    }
  }

  if ( (dfu_buff_indx == DFU_RECV_BUFFER_LENGTH) || (*psize == 0) )
  {
    
    timeout = (INT32U)(dfu_buff_indx >> 4);
    DFU_set_poll_delay(timeout); // Устанавливаем время задержки отправки пакетов необходимое для программирования уже принятых  
    dfu_buff_ready = 1;  // Сообщаем основной задаче о готовности буфера для программирования

    dfu_data_size = dfu_buff_indx;
  }
  else
  {
    /* PwPollTimeOut for manifestation phase */
    DFU_set_poll_delay(15); /* time out for accumulate blocks */
  }
  dfu_firmware_size += *psize;
  return USB_OK;
}


//-------------------------------------------------------------------------------------------------------
//  Функция передачи блока данных хосту
//
//  controller_ID     : Controller ID
//  bl_num            : порадковый номер запраашиваемого блока от начала чтения фирмваре
//  data              : Pointer to data
//  psize             : Реально отправленное количество данных
//  len               : Запрашиваемое количество данных
//
//  status:
//                  USB_OK : When Successfull
//                  Other  : When Error
//
//-------------------------------------------------------------------------------------------------------
INT8U USB_Class_DFU_transmit_block(INT8U controller_ID, INT16U bl_num, INT8U_ptr *data, INT16U *psize, INT16U len)
{
  static INT32U addr;
  static INT8U  tx_buff[DFU_MAX_BLOCK_SIZE];          // Промежуточный буффер при отправке данных хосту

  INT8U i = 0;
  UNUSED(controller_ID)

  if ( bl_num == 0 )
  {
    addr = 0;
    dfu_firmware_size = FIRMWARE_SIZE;
  }

  if ( (dfu_firmware_size - addr) >= len )
  {
    *psize = len;
  }
  else
  {
    *psize = (INT16U)(dfu_firmware_size - addr);
  }
  // Копируем данные из Flash
  for (i = 0; i < *psize; i++)
  {
    tx_buff[i] = *(INT8U *)((INT8U *)IMAGE_START_ADDR + addr + i);
  }
  addr = addr + len;
  *data = tx_buff;
  return USB_OK;
}

//-------------------------------------------------------------------------------------------------------
//  Выполнение операций иницируемых классом DFU.
//-------------------------------------------------------------------------------------------------------
void USB_Class_DFU_task(void)
{
  if ( g_dfu_state == USB_DFU_DNBUSY )
  {
    if ( g_block_flashing_done == FALSE )
    {
      if ( dfu_buff_ready == 1 )
      {
        g_dfu_flash_result = Flash_recv_buff(dfu_recv_buff, dfu_data_size);
        dfu_buff_ready = 0;
      }
      g_block_flashing_done = TRUE;
    }
  }
  if  ( g_dfu_state == USB_DFU_MANIFEST )
  {
    //  Программирование заканчивается фазой манифеста в течении которой программируется последний блок и
    //  дополнительная информация если необходимо
    if ( g_flash_manifest_done  == FALSE )
    {
      if ( dfu_buff_ready == 1 )
      {
        (void)Flash_recv_buff(dfu_recv_buff, dfu_data_size);
        dfu_buff_ready = 0;
      }
      LCD_show_programming_end(SUCCESS);   

      // Записываем манифест фирмваре(т.е. нечто описывающее само фирмваре в памяти микроконтроллера, например размер и контрольную сумму)

      //__disable_interrupt();
      //(void)Erase_sectors((INT32U *)(FIRMWARE_SIZE_ADD), 4);
      //(void)Flash_ByteProgram((INT32U *)(FIRMWARE_SIZE_ADD), (INT32U_ptr)(&SizeOfFirmware), 4);
      //__enable_interrupt();
      //
      g_flash_manifest_done  = TRUE;
    }
  }
}


