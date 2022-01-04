#include "app_cfg.h"

extern md5_context ctx;
extern unsigned char md5sum[16];

extern const unsigned char   AES_init_vector[16];
extern T_Boot_record         p_boot_rec1;
extern T_Boot_record         p_boot_rec2;
extern unsigned char         block[HEADER_BLOCK_SZ+10];

static   INT32S            g_destination;  // Индекс записи об образе в массиве информации о принимаемых образах img_file_cbl
                                           // Если 0    - программирование собственной FLASH микроконтроллера,
                                           // Если не 0 - программирование другого микроконтроллера по CAN

static   INT32U            g_blocks_cnt;
static   INT32U            g_indx_in_block;
static   INT32U            g_file_remainder;
static   INT32U            g_addr;
static   INT32U            g_programmed_size; // Общее количество данных отправленных в процедуру программирования 

static   INT32U            g_file_size;
static   INT32S            g_packets_cnt;
static   INT32S            g_session_begin;
static   INT32S            g_file_done;
static   INT32U            g_chank_size;
static   INT8U            *g_ptr_in_pack_buf;

int             boot_encrypted;

static INT8U  packet_buf[PACKET_1K_SIZE + PACKET_OVERHEAD];
INT8U         file_size_str[FILE_SIZE_LENGTH+1];
INT8U         file_name[FILE_NAME_LENGTH+1];
INT8U         tmp_file_name[FILE_NAME_LENGTH+3];


extern INT8U   chainCipherBlock[16];   // Buffer for Cipher Block


#define RCV_OK       0
#define RCV_ERROR   -1
#define RCV_ABORT    1
#define RCV_TIMEOUT  2

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c) IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c) IS_09(c)
#define CONVERTDEC(c) (c - '0')
#define CONVERTHEX_alpha(c) (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))




static INT32S  Ymodem_packets_receiving(INT8U *data, INT32S *length, INT32U timeout);
static int     Ymodem_packets_processing(INT32S  pack_len);
static int     Ymodem_pack_prg_to_FLASH(int pack_len);
static int     Ymodem_pack_prg_by_CAN(int pack_len);

/*-------------------------------------------------------------------------------------------------------------
  Прем пакетов по Y-Modem и программирование из во FLASH микроконтроллера
 
 
  Состав пакетов Y-modem
 
  [01] или [02] ( 01 - SOH, передача 128 байт пакета )  (02 - STX,передача 1024 байт пакета )
  [indx]
  [indx ^ 0xFF]
  [data]....[data]
  [crc hi] - первый байт контрольной суммы CRC16
  [crc lo] - второй байт контрольной суммы CRC16
 
  На первый пакет должен быть ответ               - [0x06] [0x43]     (ACK, CRC)
  На последующие                                  - [0x06]            (ACK)
  Если пакет с ошибкой то в ответ высылаеться     - [0x15]            (NAK)
 
  Передатчик может отправить сигнал аборта        - [0x41] или [0x61] (ABORT1, ABORT2)
  В ответ должно быть выслано подтверждение абрта - [0x18] [0x18]     (CA, CA)
 
  В конце передачи файла передатчик высылает      - [0x04]            (EOT)
  В ответ должен получить                         - [0x15]            (NAK)
  Потом снова высылает                            - [0x04]            (EOT)
  В ответ должен получить                         - [0x06] [0x43]     (ACK, CRC)
 
-------------------------------------------------------------------------------------------------------------*/
INT32S Ymodem_programmator(void)
{
  INT32S  errors;
  INT32S  pack_len;
  INT32S  res;

  errors          = 0;
  g_session_begin = 0;
  g_packets_cnt   = 0;
  g_file_done     = 0;

  LCD_print_str("SER",0,0);

  Send_byte(CRC16);


  while (1)
  {
    switch (Ymodem_packets_receiving(packet_buf, &pack_len, NAK_TIMEOUT))
    {
    case RCV_OK:
      errors = 0;
      switch (pack_len)
      {
      case -1:    // Получено два символа CA - сигнал аборта от передатчика
        Send_byte(ACK);
        return 0;
      case 0:     // RCV_OK и pack_len=0 означает получение символа EOT
        if ( g_file_done == 1 )
        {
          Send_byte(NAK); // Отправка NAK в ответ на EOT означает окончание программирования файла
          g_file_done   = 0;
          g_packets_cnt = 0;
        }
        else
        {
          Send_byte(ACK);
          Send_byte(CRC16);
        }
        break;
      default:    // Получен нормальный пакет
        if ( (packet_buf[PACKET_SEQNO_INDEX] & 0xff) != (g_packets_cnt & 0xff) )
        {
          Send_byte(NAK);
        }
        else
        {
          res = Ymodem_packets_processing(pack_len);

          switch (res)
          {
          case ABORT_SESSION:
            g_file_done   = 0;
            g_packets_cnt = 0;
            Send_byte(CA);  // Завершаем сессию в случае ошибки
            Send_byte(CA);
            return 0;
          case FIRST_PACKET:
            Send_byte(ACK); // Начинаем сессию
            Send_byte(CRC16);
            g_session_begin = 1;
            break;
          case NEXT_PACKET:
            Send_byte(ACK); // Продолжаем сессию
            break;
          case SKIP_SESSION:
            g_file_done   = 0;
            g_packets_cnt = 0;
            Send_byte(NAK);
            Send_byte(CRC16);
            break;
          }
        }
      }
      break;
    case RCV_ABORT:
      Send_byte(CA);
      Send_byte(CA);
      return 0;

    case RCV_ERROR:
      // Либо произошел таймаут, либо пришел неизвестный символ
      if ( g_session_begin > 0 )
      {
        errors++;
      }
      if ( errors > MAX_ERRORS )
      {
        Send_byte(CA);
        Send_byte(CA);
        return 0;
      }
      Send_byte(CRC16);
      break;
    case RCV_TIMEOUT:
      Send_byte(CRC16);

      break;
    }  // switch( Wait_Ymodem_packet(packet_data, &packet_length, NAK_TIMEOUT) )
  }    // while ( 1 )
}

/*------------------------------------------------------------------------------
 
 ------------------------------------------------------------------------------*/
void Decrypt_block(INT8U *buf, int len)
{
  do
  {
    aesDecrypt(buf, chainCipherBlock); // Расшифровываем блок
    buf += 16;
    len -= 16;
  }
  while (len >= 16);
}

/*-------------------------------------------------------------------------------------------------------------
* Function Name  : Str2Int
* Description    : Convert a string to an integer
* Input 1        : - inputstr: The string to be converted
*                  - intnum: The intger value
* Output         : None
* Return         : 1: Correct
*                  0: Error
-------------------------------------------------------------------------------------------------------------*/
INT32U Str2Int(INT8U *inputstr, INT32S *intnum)
{
  INT32U i = 0, res = 0;
  INT32U val = 0;

  if ( inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X') )
  {
    if ( inputstr[2] == '\0' )
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if ( inputstr[i] == '\0' )
      {
        *intnum = val;
        res = 1; /* return 1; */
        break;
      }
      if ( ISVALIDHEX(inputstr[i]) )
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* return 0; Invalid input */
        res = 0;
        break;
      }
    }

    if ( i >= 11 ) res = 0; /* over 8 digit hex --invalid */
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0; i < 11; i++)
    {
      if ( inputstr[i] == '\0' )
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      else if ( (inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0) )
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ( (inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0) )
      {

        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if ( ISVALIDDEC(inputstr[i]) ) val = val * 10 + CONVERTDEC(inputstr[i]);
      else
      {
        /* return 0; Invalid input */
        res = 0;
        break;
      }
    }
    if ( i >= 11 ) res = 0; /* Over 10 digit decimal --invalid */
  }

  return res;
}


/*-------------------------------------------------------------------------------------------------------------
 Function Name  : Receive_Packet
 Description    : Receive a packet from sender
 Input 1        : - data
 Input 2        : - length
 Input 3        : - timeout
 Output         : *length:
                  0: end of transmission
                  -1: abort by sender
                  >0: packet length
 Return :        0: normally return
                 -1: timeout or packet error
                 1: abort by user
-------------------------------------------------------------------------------------------------------------*/
static INT32S Ymodem_packets_receiving(INT8U *data, INT32S *length, INT32U timeout)
{
  INT16U i, packet_size;
  INT8U c;
  *length = 0;
  if ( Wait_byte(&c, timeout) != SUCCESS ) return RCV_TIMEOUT;

  switch (c)
  {
  case SOH:
    packet_size = PACKET_128B_SIZE;
    break;
  case STX:
    packet_size = PACKET_1K_SIZE;
    break;
  case EOT:
    return RCV_OK;
  case CA:
    if ( (Wait_byte(&c, timeout) == SUCCESS) && (c == CA) )
    {
      *length = -1;
      return RCV_OK;
    }
    else
    {
      return RCV_ERROR;
    }
  case ABORT1:
  case ABORT2:
    return RCV_ABORT;
  case 0x1B:
    return RCV_ABORT;
  default:
    return RCV_ERROR;
  }
  *data = c;
  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i++)
  {
    if ( Wait_byte(data + i, timeout) != SUCCESS )
    {
      return RCV_ERROR;
    }
  }
  if ( data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff) )
  {
    return RCV_ERROR;
  }
  *length = packet_size;
  return RCV_OK;
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
static int Ymodem_packets_processing(INT32S  pack_len)
{
  int             i;
  T_img_file_cbl  *p_img_file_cbl;

  g_file_size = 0;

  // Проверяем является ли это пустым пакетом от протокола Ymodem-а с PC
  if ( (packet_buf[0] == SOH) && (packet_buf[1] == 00) && (packet_buf[2] == 0xFF) && (packet_buf[3] == 0x00) && (packet_buf[4] == 0x00) )
  {
    return ABORT_SESSION;
  }

  g_ptr_in_pack_buf = packet_buf + PACKET_HEADER;

  // Первый пакет всегда содержит имя файла и размер
  if ( g_packets_cnt == 0 )
  {

    // Прием имени файла
    for (i = 0; i < FILE_NAME_LENGTH; i++)
    {
      file_name[i] = tolower(*g_ptr_in_pack_buf++);
      if ( file_name[i] == 0 ) break;
    }
    file_name[i] = '\0';



    // Прием размера файла
    for (i = 0; i < FILE_SIZE_LENGTH; i++)
    {
      file_size_str[i] = *g_ptr_in_pack_buf++;
      if ( file_size_str[i] == ' ' ) break;
    }
    file_size_str[i] = '\0';
    Str2Int(file_size_str, (INT32S *)&g_file_size);

    if ( g_destination == 0 )
    {
      // Проверить размер файла для записи
      if ( g_file_size > (FLASH_AREA_SIZE - BOOTL_IMG_SIZE - sizeof(p_boot_rec1)) )
      {
        // Прерывание процесса если размер больше допустимого
        return ABORT_SESSION;
      }
    }
    g_blocks_cnt    = 0;
    g_indx_in_block = 0;
    g_file_remainder  = g_file_size;

    g_packets_cnt++;
    return FIRST_PACKET;

  }
  else
  {
    return  Ymodem_pack_prg_to_FLASH(pack_len);
  }
}

/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
static int Finish_file_program(void)
{
  if ( g_packets_cnt < 2 )
  {
    return ERROR;
  }
  // Проверяем хэш записанных данных
  md5_starts(&ctx);
  md5_update(&ctx, (uint8 *)PROGRAMM_AREA_BEG_ADDR, p_boot_rec1.orig_img_size);
  md5_finish(&ctx, md5sum);

  if ( memcmp(p_boot_rec1.md5_hash, md5sum, 16) != 0 )
  {
    return ERROR;
  }

  // Записываем запись boot
  g_addr = FLASH_AREA_ADDR + FLASH_AREA_SIZE - sizeof(p_boot_rec1);
  __disable_interrupt();
  Flash_ByteProgram(g_addr,(unsigned char *)&p_boot_rec1, sizeof(p_boot_rec1));
  __enable_interrupt();

  if ( memcmp((void *)g_addr, (void *)&p_boot_rec1, sizeof(p_boot_rec1)) != 0 )  // Проверка
  {
    LCD_show_programming_end(ERROR);
    return ERROR;
  }
  LCD_show_programming_end(SUCCESS);
  return SUCCESS;
}



/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
static int Ymodem_pack_prg_to_FLASH(int pack_len)
{
  int i;
  // Полученные в пакете данные разбиваем на блоки заданной длины которые программируем
  do
  {
    // Перенос данных из пакета в блок
    g_chank_size = HEADER_BLOCK_SZ - g_indx_in_block; // Рассчитываем количество свободного места в блоке
    if ( g_chank_size > pack_len )         g_chank_size = pack_len; // Если места в блоке больше чем данных в пакете
    if ( g_chank_size > g_file_remainder ) g_chank_size = g_file_remainder;

    memcpy(block, g_ptr_in_pack_buf, g_chank_size);
    g_indx_in_block   += g_chank_size;
    g_file_remainder  -= g_chank_size;
    g_ptr_in_pack_buf += g_chank_size;

    // Программируем только когда наберется полный буфер размером HEADER_BLOCK_SZ или когда новых данных уже не поступит
    if ( (g_file_remainder == 0) || (g_indx_in_block == HEADER_BLOCK_SZ) )
    {
      // Записываем блок во FLASH
      if ( g_blocks_cnt == 0 )
      {
        // Проверяем хидер файла

        memcpy(chainCipherBlock, (void *)AES_init_vector, 16);
        aesInit(); // Init AES algorithm.

        // Определяем зашифрован или нет файл
        memcpy(&p_boot_rec1, &block[HEADER_BLOCK_SZ - sizeof(p_boot_rec1)], sizeof(p_boot_rec1));

        if ( p_boot_rec1.magic_val != MAGIC_TAG )
        {
          Decrypt_block(block, HEADER_BLOCK_SZ);
          memcpy(&p_boot_rec1, &block[HEADER_BLOCK_SZ - sizeof(p_boot_rec1)], sizeof(p_boot_rec1));

          // Если не обнаружен в первом блоке MAGIC_TAG и до и после шифрования, то прерываем программирование
          if ( p_boot_rec1.magic_val != MAGIC_TAG ) return ABORT_SESSION;
          boot_encrypted = 1;
        }

        // Проверяем допустимость размера файла с образом программирования
        if ( p_boot_rec1.orig_img_size > (FLASH_AREA_SIZE - BOOTL_IMG_SIZE - sizeof(p_boot_rec1)) )
        {
          return ABORT_SESSION; // У образа недопустимо большой размер
        }

        // Образы с установленны флагом сжатия не принимаються
        if ( p_boot_rec1.compress_flag != 0 )
        {
          return ABORT_SESSION; // Сжатые файлы не обрабатываем
        }

        // Начинаем запись файлов во Flash
        __disable_interrupt();
        Flash_Init();
        __enable_interrupt();

        // Начинаем со стирания последнего сектора
        __disable_interrupt();
        Flash_SectorErase(FLASH_AREA_ADDR + FLASH_AREA_SIZE - ERASE_SECTOR_SIZE);
        __enable_interrupt();

        {
          unsigned int n = p_boot_rec1.orig_img_size / ERASE_SECTOR_SIZE;
          if ( (p_boot_rec1.orig_img_size % ERASE_SECTOR_SIZE) != 0 ) n++;
          if ( n >= MAX_SECTOR_NUM ) n  = MAX_SECTOR_NUM - 1; // Чтобы не повтороять повтороное стирание последнего сектора

          // Стираем все изменяемые сектора
          for (i = 0; i < n; i++)
          {
            __disable_interrupt();
            Flash_SectorErase(FLASH_AREA_ADDR + BOOTL_IMG_SIZE + i * ERASE_SECTOR_SIZE);
            __enable_interrupt();
          }
        }
        g_programmed_size = 0;
        g_addr = FLASH_AREA_ADDR + BOOTL_IMG_SIZE;


      }
      else
      {
        // Записываем данные во FLASH
        if ( boot_encrypted == 1 )
        {
          Decrypt_block(block, g_chank_size);
        }
        __disable_interrupt();
        Flash_ByteProgram(g_addr, block, g_chank_size);    // Программирование
        __enable_interrupt();

        if ( memcmp((void *)g_addr, block, g_chank_size) != 0 )  // Проверка
        {
          return ABORT_SESSION;
        }
        g_addr += g_chank_size;
        g_programmed_size += g_chank_size;
        LCD_show_programming_progress(g_addr, g_programmed_size );
      }
      g_indx_in_block = 0;
      g_blocks_cnt++;
    }

    if ( g_file_remainder == 0 )
    {
      // Завершаем запись во FLASH
      if ( Finish_file_program() == SUCCESS )
      {
        g_file_done   = 1;
        return NEXT_PACKET;
      }
      else
      {
        return ABORT_SESSION;
      }
    }
    pack_len = pack_len - g_chank_size;
  }
  while (pack_len != 0);
  g_packets_cnt++;
  return NEXT_PACKET;
}


