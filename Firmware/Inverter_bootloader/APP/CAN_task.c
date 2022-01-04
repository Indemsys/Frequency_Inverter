#include "app_cfg.h"


static unsigned char tx_pack_id;
static T_can_msg rcv_msg;


unsigned char  rx_block[COMCH_BLOCK_MAX_SZ];
unsigned char  tx_block[COMCH_BLOCK_MAX_SZ];
unsigned int   rx_block_len;
static unsigned char *rx_block_ptr;
static unsigned int   rx_block_crc;
static unsigned int   rx_block_id;
static unsigned int   rx_block_cnt;
static unsigned int   rx_active;

extern volatile INT32U  g_1ms_cnt;
extern unsigned char    md5sum[16];
static md5_context      ctx;
static T_Boot_record    p_boot_rec;
volatile unsigned int   g_timeout_cnt;
unsigned int            g_run_app;
unsigned int            g_prolong_boot;


/*------------------------------------------------------------------------------
 Расчет контрольной суммы, согласно стандарту CRC-32
 ------------------------------------------------------------------------------*/
void Get_block_CRC32(INT8U *block, INT16U len, INT8U *dest)
{
  CRC_MemMapPtr  CRC  = CRC_BASE_PTR;

  int          i;

  //--------------------------------------------------------------------------------------------------------------------------------------
  // Инициализация модуля вычисления CRC
  CRC->CTRL = 0
              + LSHIFT(0x01, 30) // TOT  | Type of Transpose for Writes        | 00 No transposition.
              + LSHIFT(0x01, 28) // TOTR | Type of Transpose for Read          | 00 No transposition.
              + LSHIFT(0x01, 26) // FXOR | Complement Read of CRC data register| 0 No XOR on reading.
              + LSHIFT(0x01, 25) // WAS  | Write CRC data register as seed     | 1 Writes to the CRC data register are seed values.
              + LSHIFT(0x01, 24) // TCRC | Width of CRC protocol.              | 1 32-bit CRC protocol.
  ;
  CRC->CRC   = 0xFFFFFFFF;   // SEED = 0xFFFFFFFF
  CRC->CTRL &= ~BIT(25);
  CRC->GPOLY = 0x4C11DB7;
  for (i = 0; i < len; i++)
  {
      CRC->ACCESS8BIT.CRCHU = block[i];
  }

  *dest++ = CRC->ACCESS8BIT.CRCHU;
  *dest++ = CRC->ACCESS8BIT.CRCHL;
  *dest++ = CRC->ACCESS8BIT.CRCLU;
  *dest++ = CRC->ACCESS8BIT.CRCLL;
}


/*------------------------------------------------------------------------------
 Расчет контрольной суммы, так как он сделан на узлах сеты выполненных на STM32
 ------------------------------------------------------------------------------*/
void Get_block_CRC(INT8U *block, INT16U len, INT8U *dest)
{
  CRC_MemMapPtr  CRC  = CRC_BASE_PTR;

  int          i, k;
  INT32U       w;

  //--------------------------------------------------------------------------------------------------------------------------------------
  // Инициализация модуля вычисления CRC
  CRC->CTRL = 0
              + LSHIFT(0x00, 30) // TOT  | Type of Transpose for Writes        | 00 No transposition.
              + LSHIFT(0x00, 28) // TOTR | Type of Transpose for Read          | 00 No transposition.
              + LSHIFT(0x00, 26) // FXOR | Complement Read of CRC data register| 0 No XOR on reading.
              + LSHIFT(0x01, 25) // WAS  | Write CRC data register as seed     | 1 Writes to the CRC data register are seed values.
              + LSHIFT(0x01, 24) // TCRC | Width of CRC protocol.              | 1 32-bit CRC protocol.
  ;
  CRC->CRC = 0xFFFFFFFF;   // SEED = 0xFFFFFFFF
  CRC->CTRL &= ~BIT(25);
  CRC->GPOLY = 0x4C11DB7;
  w = 0;
  k = 0;
  for ( i=0;i<len;i++ )
  {
    w = block[i];
    w <<= 8;
    k++;
    if ( k==4 )
    {
      CRC->CRC = w;
      w = 0;
      k=0;
    }
  }

  if ( k!=0 )
  {
    CRC->CRC = w;
  }

  w = CRC->CRC;
  *dest++ = w & 0xFF;
  w >>= 8;
  *dest++ = w & 0xFF;
  w >>= 8;
  *dest++ = w & 0xFF;
  w >>= 8;
  *dest++ = w & 0xFF;
}

/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
static int Send_data_chank(INT8U *data, INT8U len)
{
  T_can_msg can_msg;
  int       i;

  if ( (len > 0) && (len <= 8) )
  {
    can_msg.format  = EXTENDED_FORMAT;
    can_msg.type    = DATA_FRAME;
    can_msg.canid   = INVERT_UPGRADE_RX_ID;
    can_msg.len     = len;
    memset(can_msg.data, 0, 8);
    for (i = 0; i < len; i++)
    {
      can_msg.data[i] = data[i];
    }
    i = 0;
    do
    {
      if ( Send_can_msg(&can_msg) == SUCCESS ) break;
      DELAY_8us;
      i++;
      if ( i > 16000 ) return ERROR;
    }
    while (1);

    return SUCCESS;
  }
  return ERROR;
}

/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
static int Send_ACK_msg(INT8U pack_id)
{
  int i;
  T_can_msg can_msg;

  can_msg.format  = EXTENDED_FORMAT;
  can_msg.type    = DATA_FRAME;
  can_msg.canid   = INVERT_UPGRADE_RX_ID;
  can_msg.len     = 1;
  memset(can_msg.data, 0, 8);
  can_msg.data[0] = pack_id;
  i = 0;
  do
  {
    if ( Send_can_msg(&can_msg) == SUCCESS ) break;
    DELAY_8us;
    i++;
    if ( i > 16000 ) return ERROR;
  }
  while (1);


  return SUCCESS;
}



/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
int Init_CAN_manager(void)
{
  return SUCCESS;
}


/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
int CAN_send_packet(unsigned int len, unsigned char *block)
{
  unsigned int n;
  INT8U buf[8];
  // Обрабатываем отправку очередного блока данных


  // Начинаем с отправки длины блока и контрольной суммы
  buf[0] = len & 0xFF;
  buf[1] = (len >> 8) & 0xFF;
  Get_block_CRC(block, len, &buf[2]);

  tx_pack_id++;
  buf[6] = tx_pack_id;

  if ( Send_data_chank(buf, 7) == SUCCESS )
  {
    while (len >= 8)
    {
      if ( Send_data_chank(block, 8) != SUCCESS )
      {
        return ERROR;
      }
      block += 8;
      len   -= 8;
    }
    if ( len != 0 )
    {
      // Дополняем нулями до 8-и байт оставшиеся данные
      memset(buf, 0, 8);
      memcpy(buf, block, len);
      if ( Send_data_chank(buf, 8) != SUCCESS )
      {
        return ERROR;
      }
      else
      {
        // Передача выполнена, ждать ответа

        // Ждем ответа около 1 сек
        n = 0;
        do
        {
          if ( Read_can_msg(&rcv_msg) == SUCCESS )
          {
            if ( rcv_msg.len == 1 )
            {
              if ( rcv_msg.data[0] == tx_pack_id )
              {
                return SUCCESS;
              }
            }

          }
          DELAY_8us;
          n++;
          if ( n > 100000ul ) return ERROR;
        }
        while (1);
      }
    }
  }

  return ERROR;
}


/*------------------------------------------------------------------------------
  Неблокирующий прием
 ------------------------------------------------------------------------------*/
int CAN_receiving_packet(void)
{
  if ( Read_can_msg(&rcv_msg) == SUCCESS )
  {

    if ( rcv_msg.len == 7 )
    {
      rx_block_len = rcv_msg.data[0] | (rcv_msg.data[1] << 8);
      rx_block_cnt = rx_block_len;
      rx_block_crc = (rcv_msg.data[5] << 24) | (rcv_msg.data[4] << 16) | (rcv_msg.data[3] << 8) | rcv_msg.data[2];
      rx_block_id  = rcv_msg.data[6];
      rx_block_ptr = rx_block;
      // Принимаем пакеты только не больше заданной длины
      if ( rx_block_len <= COMCH_BLOCK_MAX_SZ )
      {
        rx_active = 1;
      }
      else
      {
        rx_active = 0;
      }
    }
    else if ( (rcv_msg.len == 8) && (rx_active == 1) )
    {
      // Получены данные пакета
      if ( rx_block_cnt != 0 )
      {
        memcpy(rx_block_ptr, rcv_msg.data, 8);
        rx_block_ptr += 8;
        if ( rx_block_cnt > 8 )
        {
          rx_block_cnt -= 8;
        }
        else
        {
          INT32U ccrc;
          rx_active = 0;

          Get_block_CRC(rx_block, rx_block_len, (INT8U *)&ccrc);
          if ( ccrc == rx_block_crc )
          {
            Send_ACK_msg(rx_block_id);
            return SUCCESS;
          }
        }
      }
    }
  }
  return ERROR;

}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void CAN_loader(void)
{
  INT32U  i;
  INT32U  n;
  INT32U  crc;

  static unsigned int data_cnt;
  static unsigned int addr;



  do
  {

    // Ждем появления запроса о программировании
    if ( CAN_receiving_packet() == SUCCESS )
    {
      g_timeout_cnt = 0;
      if ( rx_block[0] == BOOTL_GET_CRC )
      {

        // Послать загрузочную запись
        tx_block[0] = BOOTL_ACK_CRC;
        memcpy(&tx_block[1], (void *)(FLASH_AREA_ADDR + FLASH_AREA_SIZE - sizeof(T_Boot_record)),  sizeof(T_Boot_record));
        CAN_send_packet(sizeof(T_Boot_record) + 1, tx_block);
      }
      else if ( rx_block[0] == BOOTL_SET_BOOT_REC )
      {
        LCD_print_str("CAN",0,0);
        // Принимаем новую загрузочную запись
        memcpy(&p_boot_rec, (void *)&rx_block[1],  sizeof(p_boot_rec));
        // Инициализируем программирование
        addr     = PROGRAMM_AREA_BEG_ADDR;
        data_cnt = 0;

        tx_block[0] = BOOTL_ACK_PRG;
        CAN_send_packet(1, tx_block);
      }
      else if ( rx_block[0] == BOOTL_ERASE_FLASH )
      {
        __disable_interrupt();
        // Начинаем со стирания последнего сектора
        Flash_SectorErase(FLASH_AREA_ADDR + FLASH_AREA_SIZE - ERASE_SECTOR_SIZE);

        n = p_boot_rec.orig_img_size / ERASE_SECTOR_SIZE;
        if ( (p_boot_rec.orig_img_size % ERASE_SECTOR_SIZE) != 0 ) n++;
        if ( n >= MAX_SECTOR_NUM ) n  = MAX_SECTOR_NUM - 1; // Чтобы не повтороять повтороное стирание последнего сектора

        // Стираем все изменяемые сектора
        for (i = 0; i < n; i++)
        {
          Flash_SectorErase(FLASH_AREA_ADDR + BOOTL_IMG_SIZE + i * ERASE_SECTOR_SIZE);
        }
        __enable_interrupt();

        // Инициализируем программирование
        addr     = PROGRAMM_AREA_BEG_ADDR;
        data_cnt = 0;

        tx_block[0] = BOOTL_ACK_ERASE;
        CAN_send_packet(1, tx_block);

      }
      else if ( rx_block[0] == BOOTL_DO_PRG )
      {

        __disable_interrupt();
        Flash_ByteProgram(addr, (unsigned char *)&rx_block[1], rx_block_len - 1);    // Программирование
        __enable_interrupt();
        addr += rx_block_len - 1;
        data_cnt += rx_block_len - 1;

        LCD_show_programming_progress(addr, data_cnt );

        if ( data_cnt >= p_boot_rec.orig_img_size )
        {
          // Здесь количество записанных данных сравнялось с заданным количеством,
          //  запись firmware закончена, произвести заключительные действия

          // Проверяем хеш MD5
          md5_starts(&ctx);
          md5_update(&ctx, (uint8 *)PROGRAMM_AREA_BEG_ADDR, p_boot_rec.orig_img_size);
          md5_finish(&ctx, md5sum);


          if ( memcmp(p_boot_rec.md5_hash, md5sum, 16) == 0 )
          {
            // Записываем запись boot
            addr = FLASH_AREA_ADDR + FLASH_AREA_SIZE - sizeof(p_boot_rec);
            __disable_interrupt();
            Flash_ByteProgram(addr, (unsigned char *)&p_boot_rec, sizeof(p_boot_rec));    // Программирование
            __enable_interrupt();
            LCD_show_programming_end(SUCCESS);
          }
          else
          {
            // MD5 хэш не совпадает
            LCD_show_programming_end(ERROR);
          }

        }

        tx_block[0] = BOOTL_ACK_PRG;
        CAN_send_packet(1, tx_block);
      }

    }

    if ( g_1ms_cnt > BOOTLODER_WAIT_TIME )
    {
      g_1ms_cnt = 0;
      Check_and_Jump_to_App();
    }
    if ( g_prolong_boot != 0 )
    {
      g_1ms_cnt = 0;
      g_prolong_boot = 0;
    }

  }
  while (1);


}
