#include "app_cfg.h"
#include "usb_class.h"
#include "usb_hid.h"
#include "USB_task.h"
#include "usb_dfu.h"

INT8U           usb_device_state;  /* Device state */

TIMER_OBJECT    dfu_timeout;
INT8U           g_dfu_flash_result = RES_NONE;

static BOOLEAN  mouse_init = FALSE;           /* Application Init Flag */
static INT8U    rpt_buf[MOUSE_BUFF_SIZE];     /* Mouse Event Report Buffer Key press */
static INT8U    g_app_request_params[2];      /* for get/set idle and protocol requests*/

static INT16U   last_usb_status;

INT32U   g_flashed_size;      // Количество запрограммированных байт

void         USB_App_init(void);
void         USB_App_task(void);
static void  USB_App_HID_Callback(INT8U controller_ID, INT8U event_type, void *val);
static void  USB_App_DFU_Callback(INT8U controller_ID, INT8U event_type, void *val);
static INT8U USB_App_Param_Callback(INT8U request, INT16U value, INT8U_ptr *data, INT16U *size);
static void  USB_App_Process_Detach_TimeOut(void);

INT8U Flash_recv_buff(INT8U *dfu_recv_buff, INT32U dfu_data_size);
static void Erase_sectors(INT32U *addr, INT32U size);
//-------------------------------------------------------------------------------------------------------
//  This function handles the callback
//
//  controller_ID    : Controller ID
//  event_type       : value of the event
//  val              : gives the configuration value
//
//  None
//
//  This function is called from the class layer whenever reset occurs or enum is complete.
//  After the enum is complete this function sets a variable so that the application can start
//-------------------------------------------------------------------------------------------------------
static void USB_App_HID_Callback(INT8U controller_ID, INT8U event_type, void *val)
{

  if ( (event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED) )
  {
    mouse_init = FALSE;
  }
  else if ( event_type == USB_APP_ENUM_COMPLETE )
  {
    mouse_init = TRUE; // После команды установки конфигурации
  }
  return;
}

//-------------------------------------------------------------------------------------------------------
//  This function is called to processed detach timeout
//-------------------------------------------------------------------------------------------------------
static void USB_App_Process_Detach_TimeOut()
{
  usb_device_state = USB_Class_DFU_App_Getstate();
  if ( usb_device_state == USB_APP_DETACH )
  {
    USB_Class_DFU_App_SetIdle();
    USB_Device_Shutdown(0);
    g_dfu_mode = RUNTIME_MODE;
    (void)USB_Class_HID_Init(CONTROLLER_ID, USB_App_HID_Callback, NULL, USB_App_Param_Callback);
  }
}

//-------------------------------------------------------------------------------------------------------
//   This function handles callbacks for USB HID Class request
//
//   request  : request type
//   value    : give report type and id
//   data     : pointer to the data
//   size     : size of the transfer
//
//   status
//   USB_OK  :  if successful
//   else return error
//
//
//   This function is called whenever a HID class request is received.
//   This function handles these class requests
//-------------------------------------------------------------------------------------------------------
static INT8U USB_App_Param_Callback(INT8U request, INT16U value, INT8U_ptr *data, INT16U *size)
{
  INT8U status = USB_OK;
  INT8U index = (INT8U)((request - 2) & USB_HID_REQUEST_TYPE_MASK);

  *size = 0;
  /* handle the class request */
  switch (request)
  {
  case USB_HID_GET_REPORT_REQUEST :
    *data = &rpt_buf[0]; /* point to the report to send */
    *size = MOUSE_BUFF_SIZE; /* report size */
    break;

  case USB_HID_SET_REPORT_REQUEST :
    for (index = 0; index < MOUSE_BUFF_SIZE; index++)
    {   /* copy the report sent by the host */
      rpt_buf[index] = *(*data + index);
    }
    break;

  case USB_HID_GET_IDLE_REQUEST :
    /* point to the current idle rate */
    *data = &g_app_request_params[index];
    *size = REQ_DATA_SIZE;
    break;

  case USB_HID_SET_IDLE_REQUEST :
    /* set the idle rate sent by the host */
    g_app_request_params[index] = (INT8U)((value & MSB_MASK) >> HIGH_BYTE_SHIFT);
    break;

  case USB_HID_GET_PROTOCOL_REQUEST :
    /* point to the current protocol code
       0 = Boot Protocol
       1 = Report Protocol*/
    *data = &g_app_request_params[index];
    *size = REQ_DATA_SIZE;
    break;

  case USB_HID_SET_PROTOCOL_REQUEST :
    /* set the protocol sent by the host
       0 = Boot Protocol
       1 = Report Protocol*/
    g_app_request_params[index] = (INT8U)(value);
    break;
  default:
    if ( request == USB_DFU_DETACH )
    {
#if !(DETACH)
      (void)Remove_periodic_callback(0);
      dfu_timeout.msCount = (INT16U)value;
      dfu_timeout.pfnTimerCallback = USB_App_Process_Detach_TimeOut;
      (void)Add_periodic_callback(&dfu_timeout);
#endif
      USB_Class_DFU_App_SetDetach();
      USB_Device_Shutdown(0);
      g_dfu_mode = DFU_MODE;
      (void)USB_Class_DFU_Init(CONTROLLER_ID, USB_App_DFU_Callback, NULL, NULL);
    }
    break;
  }
  return status;
}

//-------------------------------------------------------------------------------------------------------
//   This function handles USB Class callback
//
//   controller_ID    : Controller ID
//   event_type       : value of the event
//   val              : gives the configuration value
//
//   None
//
//  This function is called from the class layer whenever reset occurs or enum is complete.
//  After the enum is complete this function sets a variable so that the application can start
//-------------------------------------------------------------------------------------------------------
static void USB_App_DFU_Callback(INT8U controller_ID, INT8U event_type, void *val)
{
  BOOLEAN  manifest;    /* Flag to check manifest phase complete or not */

  UNUSED(controller_ID)
  UNUSED(val)
  usb_device_state = USB_Class_DFU_App_Getstate();
  manifest = USB_Class_DFU_App_Get_Flag_Manifest();
  if ( event_type == USB_APP_BUS_RESET )
  {
    switch (usb_device_state)
    {
    case USB_APP_DETACH:
      USB_Class_DFU_App_Set_Usbreset();
      break;
    default:
      USB_Class_DFU_App_Set_Usbreset();
      // Вернемся в реальный режим работы только если обработан манифест
      if ( manifest )
      {
        g_dfu_mode = RUNTIME_MODE;
        (void)USB_Class_HID_Init(CONTROLLER_ID, USB_App_HID_Callback, NULL, USB_App_Param_Callback);
        (void)USB_Class_DFU_App_Reset_Flag_Manifest();
      }
      break;
    } /* EndSwitch */
  }
  else if ( event_type == USB_APP_ENUM_COMPLETE )
  {}
  return;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void USB_App_init(void)
{

  INT8U   error;

  rpt_buf[3] = 0x00;   /* always zero */
  *((INT32U_ptr)rpt_buf) = 0;

  Flash_Init();       // no clk related

  __disable_interrupt();
  error = USB_Class_HID_Init(CONTROLLER_ID, USB_App_HID_Callback, NULL, USB_App_Param_Callback);
  __enable_interrupt();
  if ( error != USB_OK )
  {
    return;
  }

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
static void Show_USB_status_on_LCD(void)
{
  INT16U status;
  USB_Device_Get_Status(0, USB_STATUS_DEVICE_STATE, &status);
  if ( status != last_usb_status )
  {
    if ( status == USB_STATE_SUSPEND )
    {
      LCD_print_str("---", 0, 0);
    }
    else if ( status == USB_STATE_CONFIG )
    {
      LCD_print_str("USB", 0, 0);
    }
    else
    {
      LCD_print_str("...", 0, 0);
    }

    last_usb_status = status;
  }

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void USB_App_task(void)
{
  /* call the periodic task function */
  USB_Class_HID_task();

  if ( mouse_init )
  {
    // Функциональность если класс

  }
  USB_Class_DFU_task();


  if ( g_dfu_flash_result == RES_OK )
  {
    g_dfu_flash_result =  RES_NONE;
    USB_Class_DFU_App_SetIdle();
  }

  Show_USB_status_on_LCD();
}



//-------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------
void Loader_init(void)
{
  g_flashed_size   = 0;
}


//-------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------
static void Erase_sectors(INT32U *addr, INT32U size)
{
  INT16U   remain_bytes, counter;
  INT16U   len;
  INT8U    delete_flag = 0;
  INT32U   start_add = (INT32U)addr;
  INT32U   sector = (INT32U)(start_add / ERASE_SECTOR_SIZE);
  INT32U   offset = (INT32U)(start_add % ERASE_SECTOR_SIZE);

  do
  {
    remain_bytes = (INT16U)(((sector + 1) * ERASE_SECTOR_SIZE) - start_add);
    if ( size< remain_bytes)
    {
      len = size;
    }
    else
    {
      len = remain_bytes;
    }
    // Проверяем сектор на чистоту
    for (counter = 0; counter < len; counter++)
    {
      if ( *(INT8U *)(start_add + counter) != 0xFF )
      {
        delete_flag = 1;
        break;
      }
    }

    // Стирание сектора если обнаружены не стертые данные
    if ( delete_flag )
    {
      if ( Flash_SectorErase((INT32U)((sector * ERASE_SECTOR_SIZE) + offset)) != Flash_OK ) break;
      delete_flag = 0;
    }
    size -= len;
    start_add += len;
    sector++;
  }
  while (size);


}

//-------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------
INT8U Flash_recv_buff(INT8U *dfu_recv_buff, INT32U dfu_data_size)
{

  INT8U           result;
  INT32U          write_addr;     /* address to write to flash */
  static  INT32U  bytes_written;  /* number of bytes was written to flash */
  INT32U          header;


  write_addr = (INT32U)IMAGE_START_ADDR + bytes_written;
  __disable_interrupt();
  (void)Erase_sectors((INT32U *)(write_addr), dfu_data_size);
  result = Flash_ByteProgram(write_addr, dfu_recv_buff, dfu_data_size);
  __enable_interrupt();

  bytes_written += dfu_data_size;

  LCD_show_programming_progress(write_addr + dfu_data_size, bytes_written);
  g_flashed_size = bytes_written;

  return RES_OK;
}

