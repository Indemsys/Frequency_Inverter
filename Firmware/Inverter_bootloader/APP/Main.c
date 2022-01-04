#include "app_cfg.h"
#include "USB_timer.h"
#include "LCD_control.h"

static INT32U  New_sp;
static INT32U  New_pc; /* stack pointer and program counter */


extern void Init_periodic_app_timer(void);
extern void USB_App_init(void);
extern void USB_App_task(void);
static void Init_Sys(void);
static void wdog_disable(void);

void Jump_to_app(void);

extern const unsigned char AES_init_vector[16];
INT8U   chainCipherBlock[16];   // Buffer for Cipher Block

unsigned char  block[HEADER_BLOCK_SZ+10];
T_Boot_record p_boot_rec1;
T_Boot_record p_boot_rec2;

md5_context ctx;
unsigned char md5sum[16];

/*
Для проверки расчета контрольной суммы
INT8U test_block[49] = {0x20, 0x0f, 0x48, 0x09, 0x3f, 0x68, 0x31, 0x8e, 0x1e, 0x40, 0x80, 0x69, 0xd3, 0xcf, 0xb7, 0x04, 0x73, 0x04, 0x40, 0x01, 0x00, 0x03, 0x02, 0x0e, 0x00, 0x2e, 0x2b, 0x0f, 0x00, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40, 0x01, 0x00, 0x11, 0x33, 0x55, 0xaa};
*/

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void main(void)
{
  INT8U  c;

//  UART_Init(UART0_BASE_PTR, 120000, 921600);
  UART_Init(UART0_BASE_PTR, 120000, 115200);
  CAN_init(CAN0_BASE_PTR, CAN_SPEED);


  //printf_uart("Bootloader start\r\n");
  //Print_reset_source();
  //Print_CPU_identify();
  //printf_uart("\r\n");

  LCD_init();
  LCD_print_str("...BOOTLOADER...", 0, 0);

  PIT1_init(1000); // Инициализируем счетчик времени, с тиком = 1мс
/*
  Проверка расчета контролдьной суммы
  {
    char str[16];
    INT32U res;
    Get_block_CRC(test_block, 49, (INT8U*)&res); // Должно быть 0x645ACAE9
                                                 // Получается  0x73485EE4
    sprintf(str, "%08X", res);
    LCD_print_str(str, 0, 1);

  }
*/


  Init_periodic_app_timer();

  // После включения вначале проверяем не подключен ли программатор по RS232 к устройству
  // Посылаем запрос по RS232 и ожидаем в течении 0.1 сек ответа от  программатора по протоколу Y-modem
  Send_byte(ACK);
  if ( Wait_byte(&c, 100) == SUCCESS )
  {
    if ( c == 0x0D )
    {
      DELAY_ms(10);
      Ymodem_programmator();
    }
  }



  // Если программатор не подключен, то переходим в режим управления программированием и стартом по CAN
  // CAN_loader();

  do
  {
    Check_and_Jump_to_App();
    Ymodem_programmator();
  }
  while (1);

//
//  Приложенние DFU программатора
//
//  (void)USB_App_init(); /* Initialize the USB Test Application */
//
//  while (TRUE)
//  {
//    Watchdog_refresh();
//    USB_App_task();
//  }
}


//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Watchdog_refresh(void)
{
  (void)(WDOG_BASE_PTR->REFRESH = 0xA602, WDOG_BASE_PTR->REFRESH = 0xB480);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void LCD_show_programming_progress(INT32U addr, INT32U sz)
{
  char str[16];
  LCD_clear_row(1);
  sprintf(str, "%05X   %07d",addr, sz);
  LCD_print_str(str, 0, 1);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void LCD_show_programming_end(int state)
{
  char str[16];
  LCD_clear_row(1);
  if ( state == SUCCESS )
  {
    sprintf(str, "Flashing done!");
  }
  else if ( state == ERROR )
  {
    sprintf(str, "Flashing error!");
  }
  LCD_print_str(str, 0, 1);
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void Check_and_Jump_to_App(void)
{
  // Проверить образ программы во FLASH
  // Извлекаем из области FLASH микроконтроллера запись об имидже.
  memcpy(&p_boot_rec2, (void *)(FLASH_AREA_ADDR + FLASH_AREA_SIZE - sizeof(p_boot_rec2)),  sizeof(p_boot_rec2));
  // Вычисляем хэш данных в памяти
  md5_starts(&ctx);
  if ( p_boot_rec2.orig_img_size <= FLASH_AREA_SIZE )
  {
    md5_update(&ctx, (uint8 *)PROGRAMM_AREA_BEG_ADDR, p_boot_rec2.orig_img_size);
    md5_finish(&ctx, md5sum);
    if ( memcmp(p_boot_rec2.md5_hash, md5sum, 16) == 0 )
    {
      Jump_to_app();
    }
  }
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void Jump_to_app(void)
{

  /* Get PC and SP of application region */
  New_sp  = IMAGE_START_ADDR[0];
  New_pc  = IMAGE_START_ADDR[1];
  /* Check switch is pressed*/
  if ( (New_sp != 0xffffffff) && (New_pc != 0xffffffff) )
  {
    /* Run the application */
    asm("mov32   r4,New_sp");
    asm("ldr     sp,[r4]");
    asm("mov32   r4,New_pc");
    asm("ldr     r5, [r4]");
    asm("blx     r5");
  }
}


