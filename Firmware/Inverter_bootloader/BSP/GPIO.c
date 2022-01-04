#include "app_types.h"
#include <MK60F15.h>

typedef struct
{
  GPIO_MemMapPtr gpio;
  PORT_MemMapPtr port;
  unsigned char  pin_num;
  unsigned char  irqc; //  Interrupt Configuration
                       //  0000 Interrupt/DMA Request disabled.
                       //  0001 DMA Request on rising edge.
                       //  0010 DMA Request on falling edge.
                       //  0011 DMA Request on either edge.
                       //  0100 Reserved.
                       //  1000 Interrupt when logic zero.
                       //  1001 Interrupt on rising edge.
                       //  1010 Interrupt on falling edge.
                       //  1011 Interrupt on either edge.
                       //  1100 Interrupt when logic one.
  unsigned char  lock; //  if 1 Pin Control Register bits [15:0] are locked and cannot be updated until the next System Reset.
  unsigned char  mux;  //  Pin Mux Control
                       //  000 Pin Disabled (Analog).
                       //  001 Alternative 1 (GPIO).
                       //  010 Alternative 2 (chip specific).
                       //  011 Alternative 3 (chip specific).
                       //  100 Alternative 4 (chip specific).
                       //  101 Alternative 5 (chip specific).
                       //  110 Alternative 6 (chip specific).
                       //  111 Alternative 7 (chip specific / JTAG / NMI).
  unsigned char  DSE; // 0 Low drive strength is configured on the corresponding pin, if pin is configured as a digital output.
                      // 1 High drive strength is configured on the corresponding pin, if pin is configured as a digital output.
  unsigned char  SRE;  // 0 Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output.
                       // 1 Slow slew rate is configured on the corresponding pin, if pin is configured as a digital output.
  unsigned char  ODE;  // 0 Open Drain output is disabled on the corresponding pin.
                       // 1 Open Drain output is enabled on the corresponding pin, provided pin is configured as a digital output.
  unsigned char  PFE;  // 0 Passive Input Filter is disabled on the corresponding pin.
                       // 1 Passive Input Filter is enabled on the corresponding pin.
  unsigned char  PUPD; // 00 Internal pull-up or pull-down resistor is not enabled on the corresponding pin.
                       // 10 Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.
                       // 11 Internal pull-up resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.
  unsigned char  dir;  // 0 Pin is configured as general purpose input, if configured for the GPIO function
                       // 1 Pin is configured for general purpose output, if configured for the GPIO function
  unsigned char  init; // Init state

} T_IO_pins_configuration;

#define   ANAL          0
#define   GPIO          1
#define   ALT2          2
#define   ALT3          3
#define   ALT4          4
#define   ALT5          5
#define   ALT6          6
#define   ALT7          7

#define   DSE_LO        0
#define   DSE_HI        1

#define   OD_DIS        0
#define   OD__EN        1

#define   PFE_DIS       0
#define   PFE__EN       1

#define   FAST_SLEW     0
#define   SLOW_SLEW     1


#define   PUPD_DIS      0
#define   PULL__DN      2
#define   PULL__UP      3

#define   GP_INP        0
#define   GP_OUT        1


const T_IO_pins_configuration gen_pins_conf[] =
{
// gpio          port            num  irqc  lock  mux   DSE     SRE        ODE     PFE      PUPD      dir     init
  { PTA_BASE_PTR, PORTA_BASE_PTR,   0,   0,   1,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTAG_TCLK
  { PTA_BASE_PTR, PORTA_BASE_PTR,   1,   0,   1,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTAG_TDI
  { PTA_BASE_PTR, PORTA_BASE_PTR,   2,   0,   1,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTAG_TDO
  { PTA_BASE_PTR, PORTA_BASE_PTR,   3,   0,   1,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTAG_TMS
  { PTA_BASE_PTR, PORTA_BASE_PTR,   4,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTA_BASE_PTR, PORTA_BASE_PTR,   5,   0,   1,   ALT7, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // JTAG_TRST
  { PTA_BASE_PTR, PORTA_BASE_PTR,   6,   0,   1,   ALT5, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // CLKOUT - тестовый выход
//  { PTA_BASE_PTR, PORTA_BASE_PTR,   6,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_OUT,   1 }, // CLKOUT - тестовый выход
  { PTA_BASE_PTR, PORTA_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTA_BASE_PTR, PORTA_BASE_PTR,   8,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ZCROSS
  { PTA_BASE_PTR, PORTA_BASE_PTR,   9,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTA_BASE_PTR, PORTA_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // ENC1, вход энкодера
  { PTA_BASE_PTR, PORTA_BASE_PTR,  11,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // ENC2, вход энкодера
  { PTA_BASE_PTR, PORTA_BASE_PTR,  12,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // PWM_OE, вход наблюдателя за блокировкой PWM
  { PTA_BASE_PTR, PORTA_BASE_PTR,  13,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_OUT,   1 }, // PWM_EN, выход разрешения PWM (1-запрещает, 0-разрешает)
  { PTA_BASE_PTR, PORTA_BASE_PTR,  14,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // OUT2
  { PTA_BASE_PTR, PORTA_BASE_PTR,  15,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // OUT3
  { PTA_BASE_PTR, PORTA_BASE_PTR,  16,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // OUT1
  { PTA_BASE_PTR, PORTA_BASE_PTR,  17,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PUPD_DIS, GP_INP,   0 }, // IN1
  // Нельзя трогать выводы осциллятора иначе чип перестает работать с JTAG программироваться из IAR
  // { PTA_BASE_PTR, PORTA_BASE_PTR,  18,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // EXTAL
  // { PTA_BASE_PTR, PORTA_BASE_PTR,  19,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // XTAL
  { PTA_BASE_PTR, PORTA_BASE_PTR,  24,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD__EN, PFE_DIS, PULL__UP, GP_OUT,   0 }, // LED1. Включается от лог.0
  { PTA_BASE_PTR, PORTA_BASE_PTR,  25,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD__EN, PFE_DIS, PULL__UP, GP_OUT,   1 }, // LED2. Включается от лог.0
  { PTA_BASE_PTR, PORTA_BASE_PTR,  26,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD__EN, PFE_DIS, PULL__UP, GP_OUT,   1 }, // LED3. Включается от лог.0
  { PTA_BASE_PTR, PORTA_BASE_PTR,  27,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTA_BASE_PTR, PORTA_BASE_PTR,  28,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTA_BASE_PTR, PORTA_BASE_PTR,  29,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // V_U. Измерение напряжение на выходе M1

  { PTB_BASE_PTR, PORTB_BASE_PTR,   0,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTB_BASE_PTR, PORTB_BASE_PTR,   1,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTB_BASE_PTR, PORTB_BASE_PTR,   2,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // V_W. Измерение напряжение на выходе M3
  { PTB_BASE_PTR, PORTB_BASE_PTR,   3,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // VFO. Оповещение от драйвера FSBB30CH60CT
  { PTB_BASE_PTR, PORTB_BASE_PTR,   4,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTB_BASE_PTR, PORTB_BASE_PTR,   5,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTB_BASE_PTR, PORTB_BASE_PTR,   6,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // V_V. Измерение напряжение на выходе M2
  { PTB_BASE_PTR, PORTB_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTB_BASE_PTR, PORTB_BASE_PTR,   8,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTB_BASE_PTR, PORTB_BASE_PTR,   9,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTB_BASE_PTR, PORTB_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // TOS1. Оповещение от измерителя температуры IGBT модуля
  { PTB_BASE_PTR, PORTB_BASE_PTR,  11,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---

  { PTB_BASE_PTR, PORTB_BASE_PTR,  16,   0,   0,   ALT3, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // UART_RX
  { PTB_BASE_PTR, PORTB_BASE_PTR,  17,   0,   0,   ALT3, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // UART_TX
  { PTB_BASE_PTR, PORTB_BASE_PTR,  18,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // CAN_TX
  { PTB_BASE_PTR, PORTB_BASE_PTR,  19,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // CAN_RX
  { PTB_BASE_PTR, PORTB_BASE_PTR,  20,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SPI2_CS.   SPI Flash
  { PTB_BASE_PTR, PORTB_BASE_PTR,  21,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SPI2_CLK.  SPI Flash
  { PTB_BASE_PTR, PORTB_BASE_PTR,  22,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SPI2_SOUT. SPI Flash
  { PTB_BASE_PTR, PORTB_BASE_PTR,  23,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SPI2_SIN.  SPI Flash

  { PTC_BASE_PTR, PORTC_BASE_PTR,   0,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // SPI_RES. Сброс SPI Flash
  { PTC_BASE_PTR, PORTC_BASE_PTR,   1,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__DN, GP_INP,   0 }, // T0C0. PWM канал 0
  { PTC_BASE_PTR, PORTC_BASE_PTR,   2,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__DN, GP_INP,   0 }, // T0C1. PWM канал 1
  { PTC_BASE_PTR, PORTC_BASE_PTR,   3,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__DN, GP_INP,   0 }, // T0C2. PWM канал 2
  { PTC_BASE_PTR, PORTC_BASE_PTR,   4,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__DN, GP_INP,   0 }, // T0C3. PWM канал 3
  { PTC_BASE_PTR, PORTC_BASE_PTR,   5,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,   6,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,   8,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,   9,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,  11,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,  12,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // CAN_SENS
  { PTC_BASE_PTR, PORTC_BASE_PTR,  13,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // NTC_BYPASS
  { PTC_BASE_PTR, PORTC_BASE_PTR,  14,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   1 }, // USBCTRL
  { PTC_BASE_PTR, PORTC_BASE_PTR,  15,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTC_BASE_PTR, PORTC_BASE_PTR,  16,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // DIP SW 1
  { PTC_BASE_PTR, PORTC_BASE_PTR,  17,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // DIP SW 2
  { PTC_BASE_PTR, PORTC_BASE_PTR,  18,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // DIP SW 3
  { PTC_BASE_PTR, PORTC_BASE_PTR,  19,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // DIP SW 4

  { PTD_BASE_PTR, PORTD_BASE_PTR,   0,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // ---
  { PTD_BASE_PTR, PORTD_BASE_PTR,   1,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // ---
  { PTD_BASE_PTR, PORTD_BASE_PTR,   2,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTD_BASE_PTR, PORTD_BASE_PTR,   3,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // ---
  { PTD_BASE_PTR, PORTD_BASE_PTR,   4,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__DN, GP_INP,   0 }, // T0C4. PWM канал 4
  { PTD_BASE_PTR, PORTD_BASE_PTR,   5,   0,   0,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__DN, GP_INP,   0 }, // T0C5. PWM канал 5
  { PTD_BASE_PTR, PORTD_BASE_PTR,   6,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // BR_ON
  { PTD_BASE_PTR, PORTD_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTD_BASE_PTR, PORTD_BASE_PTR,   8,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE__EN, PULL__UP, GP_INP,   0 }, // SCL
  { PTD_BASE_PTR, PORTD_BASE_PTR,   9,   0,   0,   ALT2, DSE_HI, FAST_SLEW, OD_DIS, PFE__EN, PULL__UP, GP_INP,   0 }, // SDA
  { PTD_BASE_PTR, PORTD_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTD_BASE_PTR, PORTD_BASE_PTR,  11,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // DSPL_RST
  { PTD_BASE_PTR, PORTD_BASE_PTR,  12,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // DSPL_SCL
  { PTD_BASE_PTR, PORTD_BASE_PTR,  13,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // DSPL_SI
  { PTD_BASE_PTR, PORTD_BASE_PTR,  14,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // DSPL_RS
  { PTD_BASE_PTR, PORTD_BASE_PTR,  15,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // DSPL_SCB

  { PTE_BASE_PTR, PORTE_BASE_PTR,   0,   0,   1,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SD_D1
  { PTE_BASE_PTR, PORTE_BASE_PTR,   1,   0,   1,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SD_D0
  { PTE_BASE_PTR, PORTE_BASE_PTR,   2,   0,   1,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SD_CLK
  { PTE_BASE_PTR, PORTE_BASE_PTR,   3,   0,   1,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SD_CMD
  { PTE_BASE_PTR, PORTE_BASE_PTR,   4,   0,   1,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SD_D3
  { PTE_BASE_PTR, PORTE_BASE_PTR,   5,   0,   1,   ALT4, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // SD_D2
  { PTE_BASE_PTR, PORTE_BASE_PTR,   6,   0,   0,   ALT6, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // BUZZ
  { PTE_BASE_PTR, PORTE_BASE_PTR,   7,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // COOLER_ON
  { PTE_BASE_PTR, PORTE_BASE_PTR,   8,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // I_U
  { PTE_BASE_PTR, PORTE_BASE_PTR,   9,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTE_BASE_PTR, PORTE_BASE_PTR,  10,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTE_BASE_PTR, PORTE_BASE_PTR,  11,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTE_BASE_PTR, PORTE_BASE_PTR,  12,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---

  { PTE_BASE_PTR, PORTE_BASE_PTR,  24,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // V_BUS
  { PTE_BASE_PTR, PORTE_BASE_PTR,  25,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, //
  { PTE_BASE_PTR, PORTE_BASE_PTR,  26,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // 5V_MEAS
  { PTE_BASE_PTR, PORTE_BASE_PTR,  27,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // ---
  { PTE_BASE_PTR, PORTE_BASE_PTR,  28,   0,   0,   ANAL, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // 15V_MEAS

};


/*------------------------------------------------------------------------------
 
 ------------------------------------------------------------------------------*/
static void Config_pin(const T_IO_pins_configuration pinc)
{
  pinc.port->PCR[pinc.pin_num] = LSHIFT(pinc.irqc, 16) |
                                 LSHIFT(pinc.lock, 15) |
                                 LSHIFT(pinc.mux, 8) |
                                 LSHIFT(pinc.DSE, 6) |
                                 LSHIFT(pinc.ODE, 5) |
                                 LSHIFT(pinc.PFE, 4) |
                                 LSHIFT(pinc.SRE, 2) |
                                 LSHIFT(pinc.PUPD, 0);

  if ( pinc.init == 0 ) pinc.gpio->PCOR = LSHIFT(1, pinc.pin_num);
  else pinc.gpio->PSOR = LSHIFT(1, pinc.pin_num);
  pinc.gpio->PDDR = (pinc.gpio->PDDR & ~LSHIFT(1, pinc.pin_num)) | LSHIFT(pinc.dir, pinc.pin_num);
}


/*------------------------------------------------------------------------------
 
 ------------------------------------------------------------------------------*/
int Init_pins(void)
{
  int i;

  // Включаем тактирование на всех портах
  SIM_SCGC5 |=   SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTF_MASK;

  for (i = 0; i < (sizeof(gen_pins_conf) / sizeof(gen_pins_conf[0])); i++)
  {
    Config_pin(gen_pins_conf[i]);
  }

  // Конфигурируем цифровой фильтр для I2C
  // Без фильтра происходит зависание контроллера I2C в схеме инвертора версии 2
  PORTD_DFWR = 0x1F;
  PORTD_DFER = BIT(8) + BIT(9);
  return 0;
}


/*-------------------------------------------------------------------------------------------------------------
  Установка состояния светодиодов
  led_num - номер светодиода (1...3)
  state   - состояние ( 1 - включен, 0 - выключен)
-------------------------------------------------------------------------------------------------------------*/
void Led_control(int led_num, int state)
{
  switch (led_num)
  {
  case 1:
    if ( (state & BIT(0)) != 0 ) PTA_BASE_PTR->PCOR = LSHIFT(1, 24);
    else PTA_BASE_PTR->PSOR = LSHIFT(1, 24);
    break;
  case 2:
    if ( (state & BIT(0)) != 0 ) PTA_BASE_PTR->PCOR = LSHIFT(1, 25);
    else PTA_BASE_PTR->PSOR = LSHIFT(1, 25);
    break;
  case 3:
    if ( (state & BIT(0)) != 0 ) PTA_BASE_PTR->PCOR = LSHIFT(1, 26);
    else PTA_BASE_PTR->PSOR = LSHIFT(1, 26);
    break;
  }
}

/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
int Led_state(int led_num)
{
  switch (led_num)
  {
  case 1:
    return (PTA_BASE_PTR->PDOR>>24)&1;
    break;
  case 2:
    return (PTA_BASE_PTR->PDOR>>25)&1;
    break;
  case 3:
    return (PTA_BASE_PTR->PDOR>>26)&1;
    break;
  }
  return 0;
}
/*-------------------------------------------------------------------------------------------------------------
  Active LOW Reset Signal
-------------------------------------------------------------------------------------------------------------*/
void LCD_RST(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 11);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 11);
}

/*-------------------------------------------------------------------------------------------------------------
  Serial clock
-------------------------------------------------------------------------------------------------------------*/
void LCD_SCL(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 12);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 12);
}
/*-------------------------------------------------------------------------------------------------------------
  Input data
-------------------------------------------------------------------------------------------------------------*/
void LCD_SI(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 13);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 13);
}
/*-------------------------------------------------------------------------------------------------------------
  Register Select Signal. RS=0: instruction; RS=1: data
-------------------------------------------------------------------------------------------------------------*/
void LCD_RS(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 14);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 14);
}
/*-------------------------------------------------------------------------------------------------------------
  Active LOW Chip Select signal
-------------------------------------------------------------------------------------------------------------*/
void LCD_SCB(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 15);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 15);
}

