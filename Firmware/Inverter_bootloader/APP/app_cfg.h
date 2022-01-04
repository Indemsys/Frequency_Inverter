#ifndef __APP_CFG_H
#define __APP_CFG_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <MK60F15.h>
#include "intrinsics.h"
#include "app_types.h"
#include "md5.h"
#include "aes.h"
#include "app_IDs.h"
#include "CAN_control.h"
#include "CAN_task.h"
#include "Y_Loader.h"
#include "flash_FTFL.h"
         
#include "USB_timer.h" 
#include "Diagnostic_print.h"
#include "LCD_control.h"


#define BOOTLODER_WAIT_TIME    60000  // Время пребывания в состоянии загрузчика при отсутствии команд по CAN  

#define SYSTEM_CLOCK           (120000000ul)
#define PERIPH_CLOCK           (60000000ul)
#define BASEPRI                (0x0F)


#define MIN_RAM1_ADDRESS        0x1FFF0000
#define MAX_RAM1_ADDRESS        0x20010000
#define MIN_FLASH1_ADDRESS      0x00008000
#define MAX_FLASH1_ADDRESS      0x00018000
#define IMAGE_START_ADDR        ((INT32U_ptr)0x00008000)
#define ERASE_SECTOR_SIZE       0x1000  /* 4K bytes*/
#define FIRMWARE_SIZE           (MAX_FLASH1_ADDRESS - MIN_FLASH1_ADDRESS +1)

#define DFU_RECV_BUFFER_LENGTH  (DFU_MAX_BLOCK_SIZE*40) 

#define BOOTL_IMG_SIZE          0x00008000  
#define FLASH_AREA_ADDR         0x00000000
#define FLASH_AREA_SIZE         0x00100000
#define PROGRAMM_AREA_BEG_ADDR  0x00008000
#define MAX_SECTOR_NUM     ((FLASH_AREA_SIZE - BOOTL_IMG_SIZE)/ERASE_SECTOR_SIZE)


#define MAGIC_TAG      0xAA553311
#define COMPESSION_TAG 0x778811EE

#define HEADER_BLOCK_SZ    512



#define  HIGH_SPEED_DEVICE            (1)

/* Below two MACROS are required for Keyboard Application to execute */
#define LONG_SEND_TRANSACTION       /* support to send large data pkts */
#define LONG_RECIEVE_TRANSACTION    /* support to receive large data pkts */

/* User Defined MACRO to set number of Timer Objects */
#define MAX_TIMER_OBJECTS           2

/* When Enabled Timer Callback is invoked with an argument */
#define TIMER_CALLBACK_ARG
#undef TIMER_CALLBACK_ARG


/* Use double buffered endpoints 5 & 6. To be only used with S08 cores */
#define DOUBLE_BUFFERING_USED


typedef struct 
{
  const char  *img_filename;
  const char  *used_filename;
  unsigned int target_rx_can_id; // CAN идентификатор для приема, если 0 то файл предназначен для данного контроллера 
  unsigned int target_tx_can_id; // CAN идентификатор для передачи, если 0 то файл предназначен для данного контроллера 

} T_img_file_cbl;

typedef struct
{
  unsigned char  md5_hash[16];
  unsigned int   orig_img_size;
  unsigned int   date;
  unsigned int   time;
  unsigned int   image_offset;
  unsigned int   compr_algor_type;
  unsigned int   compress_flag;
  unsigned int   image_size_val;
  unsigned int   magic_val;
    
} T_Boot_record;


void Watchdog_refresh(void);
void LCD_show_programming_progress(INT32U addr, INT32U sz);
void LCD_show_programming_end(int state);
void Check_and_Jump_to_App(void);
#endif
