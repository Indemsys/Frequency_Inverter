#include <mqx.h>
#include <bsp.h>



typedef struct
{
  GPIO_MemMapPtr gpio;
  PORT_MemMapPtr port;
  int            pin_num;
  int            irqc; //  Interrupt Configuration
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
  int            lock; //  if 1 Pin Control Register bits [15:0] are locked and cannot be updated until the next System Reset.
  int            mux;  //  Pin Mux Control
                       //  000 Pin Disabled (Analog).
                       //  001 Alternative 1 (GPIO).
                       //  010 Alternative 2 (chip specific).
                       //  011 Alternative 3 (chip specific).
                       //  100 Alternative 4 (chip specific).
                       //  101 Alternative 5 (chip specific).
                       //  110 Alternative 6 (chip specific).
                       //  111 Alternative 7 (chip specific / JTAG / NMI).
  int            DSE; // 0 Low drive strength is configured on the corresponding pin, if pin is configured as a digital output.
                      // 1 High drive strength is configured on the corresponding pin, if pin is configured as a digital output.
  int            SRE;  // 0 Fast slew rate is configured on the corresponding pin, if pin is configured as a digital output.
                       // 1 Slow slew rate is configured on the corresponding pin, if pin is configured as a digital output.
  int            ODE;  // 0 Open Drain output is disabled on the corresponding pin.
                       // 1 Open Drain output is enabled on the corresponding pin, provided pin is configured as a digital output.
  int            PFE;  // 0 Passive Input Filter is disabled on the corresponding pin.
                       // 1 Passive Input Filter is enabled on the corresponding pin.
  int            PUPD; // 00 Internal pull-up or pull-down resistor is not enabled on the corresponding pin.
                       // 10 Internal pull-down resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.
                       // 11 Internal pull-up resistor is enabled on the corresponding pin, if the corresponding Port Pull Enable Register bit is set.
  int            dir;  // 0 Pin is configured as general purpose input, if configured for the GPIO function
                       // 1 Pin is configured for general purpose output, if configured for the GPIO function
  int            init; // Init state

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
  { PTA_BASE_PTR, PORTA_BASE_PTR,   6,   0,   0,   ALT5, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_INP,   0 }, // CLKOUT - тестовый выход
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
  { PTA_BASE_PTR, PORTA_BASE_PTR,  25,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD__EN, PFE_DIS, PULL__UP, GP_OUT,   0 }, // LED2. Включается от лог.0
  { PTA_BASE_PTR, PORTA_BASE_PTR,  26,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD__EN, PFE_DIS, PULL__UP, GP_OUT,   0 }, // LED3. Включается от лог.0
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
  { PTC_BASE_PTR, PORTC_BASE_PTR,  14,   0,   0,   GPIO, DSE_HI, FAST_SLEW, OD_DIS, PFE_DIS, PULL__UP, GP_OUT,   0 }, // USBCTRL
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
_mqx_int _bsp_chip_pins_init(void)
{
  int i;

  SIM_SCGC5 |=   SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTF_MASK;

  for (i = 0; i < (sizeof(gen_pins_conf) / sizeof(gen_pins_conf[0])); i++)
  {
    Config_pin(gen_pins_conf[i]);
  }

  // Конфигурируем цифровой фильтр для I2C
  // Без фильтра происходит зависание контроллера I2C в схеме инвертора версии 2
  PORTD_DFWR = 0x1F;
  PORTD_DFER = BIT(8) + BIT(9);
  return MQX_OK;
}








/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_serial_io_init
* Returned Value   : MQX_OK for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to serial
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_serial_io_init
(
 /* [IN] Serial device number */
 uint_8 dev_num,

 /* [IN] Required functionality */
 uint_8 flags
 )
{
  SIM_MemMapPtr   sim = SIM_BASE_PTR;
  PORT_MemMapPtr  pctl;

  /* Setup GPIO for UART devices */
  switch (dev_num)
  {
  case 0:
    pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
    if ( flags & IO_PERIPHERAL_PIN_MUX_ENABLE )
    {
      /* PTA1 as RX function (Alt.3) + drive strength */
      pctl->PCR[1] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
      /* PTA2 as TX function (Alt.3) + drive strength */
      pctl->PCR[2] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
    }
    if ( flags & IO_PERIPHERAL_PIN_MUX_DISABLE )
    {
      /* PTA1 default */
      pctl->PCR[1] = 0;
      /* PTA2 default */
      pctl->PCR[2] = 0;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_ENABLE )
    {
      /* start SGI clock */
      sim->SCGC4 |= SIM_SCGC4_UART0_MASK;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_DISABLE )
    {
      /* stop SGI clock */
      sim->SCGC4 &= (~SIM_SCGC4_UART0_MASK);
    }
    break;
  case 1:
    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
    if ( flags & IO_PERIPHERAL_PIN_MUX_ENABLE )
    {
      /* PTC3 as RX function (Alt.3) + drive strength */
      pctl->PCR[3] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
      /* PTC4 as TX function (Alt.3) + drive strength */
      pctl->PCR[4] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
    }
    if ( flags & IO_PERIPHERAL_PIN_MUX_DISABLE )
    {
      /* PTC3 default */
      pctl->PCR[3] = 0;
      /* PTC4 default */
      pctl->PCR[4] = 0;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_ENABLE )
    {
      /* start SGI clock */
      sim->SCGC4 |= SIM_SCGC4_UART1_MASK;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_DISABLE )
    {
      /* start SGI clock */
      sim->SCGC4 &= (~SIM_SCGC4_UART1_MASK);
    }
    break;

  case 2:
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
    if ( flags & IO_PERIPHERAL_PIN_MUX_ENABLE )
    {
      /* PTD2 as RX function (Alt.3) + drive strength */
      pctl->PCR[2] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
      /* PTD3 as TX function (Alt.3) + drive strength */
      pctl->PCR[3] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
    }
    if ( flags & IO_PERIPHERAL_PIN_MUX_DISABLE )
    {
      /* PTD2 default */
      pctl->PCR[2] = 0;
      /* PTD3 default */
      pctl->PCR[3] = 0;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_ENABLE )
    {
      /* start SGI clock */
      sim->SCGC4 |= SIM_SCGC4_UART2_MASK;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_DISABLE )
    {
      /* stop SGI clock */
      sim->SCGC4 &= (~SIM_SCGC4_UART2_MASK);
    }
    break;

  case 3:
    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
    if ( flags & IO_PERIPHERAL_PIN_MUX_ENABLE )
    {
      /* PTC16 as RX function (Alt.3) + drive strength */
      pctl->PCR[16] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
      /* PTC17 as TX function (Alt.3) + drive strength */
      pctl->PCR[17] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_ENABLE )
    {
      /* start SGI clock */
      sim->SCGC4 |= SIM_SCGC4_UART3_MASK;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_DISABLE )
    {
      /* stop SGI clock */
      sim->SCGC4 &= (~SIM_SCGC4_UART3_MASK);
    }
    break;

  case 4:
    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
    if ( flags & IO_PERIPHERAL_PIN_MUX_ENABLE )
    {

      /* PTC14 as RX function (Alt.3) + drive strength */
      pctl->PCR[14] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
      /* PTC15 as TX function (Alt.3) + drive strength */
      pctl->PCR[15] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;

    }
    if ( flags & IO_PERIPHERAL_PIN_MUX_DISABLE )
    {
      /* PTC14 default */
      pctl->PCR[14] = 0;
      /* PTC15 default */
      pctl->PCR[15] = 0;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_ENABLE )
    {
      /* starting SGI clock */
      sim->SCGC1 |= SIM_SCGC1_UART4_MASK;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_DISABLE )
    {
      /* starting SGI clock */
      sim->SCGC1 &= (~SIM_SCGC1_UART4_MASK);
    }
    break;

  case 5:
    pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
    if ( flags & IO_PERIPHERAL_PIN_MUX_ENABLE )
    {
      /* PTE8 as RX function (Alt.3) + drive strength */
      pctl->PCR[8] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
      /* PTE9 as RX function (Alt.3) + drive strength */
      pctl->PCR[9] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
#if BSPCFG_ENABLE_TTYF_HW_SIGNALS
      /* PTE11 as RTS function (Alt.3) + drive strength */
      pctl->PCR[11] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
      /* PTE10 as CTS function (Alt.3) + drive strength */
      pctl->PCR[10] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
#endif
    }
    if ( flags & IO_PERIPHERAL_PIN_MUX_DISABLE )
    {
      /* PTE8 default */
      pctl->PCR[8] = 0;
      /* PTE9 default */
      pctl->PCR[9] = 0;
#if BSPCFG_ENABLE_TTYF_HW_SIGNALS
      /* PTE11 default */
      pctl->PCR[11] = 0;
      /* PTE10 default */
      pctl->PCR[10] = 0;
#endif
    }
    if ( flags & IO_PERIPHERAL_CLOCK_ENABLE )
    {
      /* starting SGI clock */
      sim->SCGC1 |= SIM_SCGC1_UART5_MASK;
    }
    if ( flags & IO_PERIPHERAL_CLOCK_DISABLE )
    {
      /* starting SGI clock */
      sim->SCGC1 &= (~SIM_SCGC1_UART5_MASK);
    }
    break;

  default:
    return -1;
  }

  return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_adc_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to ADC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_adc_io_init
(
 /* [IN] number of ADC device on which to perform hardware initialization */
 _mqx_uint adc_num
 )
{
  SIM_MemMapPtr sim_ptr = SIM_BASE_PTR;

  /* Enable ADC clocks */
  switch (adc_num)
  {
  case 0:
    sim_ptr->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    break;
  case 1:
    sim_ptr->SCGC3 |= SIM_SCGC3_ADC1_MASK;
    break;
  case 2:
    sim_ptr->SCGC6 |= SIM_SCGC6_ADC2_MASK;
    break;
  case 3:
    sim_ptr->SCGC3 |= SIM_SCGC3_ADC3_MASK;
    break;
  default:
    return IO_ERROR;
  }

  return IO_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_adc_channel_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to ADC channel
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_adc_channel_io_init
(
 /* [IN] number of channel on which to perform hardware initialization */
 uint_16   source
 )
{
  uint_8 ch = ADC_GET_CHANNEL(source);
  uint_8 mux = ADC_GET_MUXSEL(source);
  uint_8 gpio_port;
  PORT_MemMapPtr pctl;

#define ADC_SIG_PORT_SHIFT (5)
#define ADC_SIG_PORTA   (0x01 << ADC_SIG_PORT_SHIFT)
#define ADC_SIG_PORTB   (0x02 << ADC_SIG_PORT_SHIFT)
#define ADC_SIG_PORTC   (0x03 << ADC_SIG_PORT_SHIFT)
#define ADC_SIG_PORTD   (0x04 << ADC_SIG_PORT_SHIFT)
#define ADC_SIG_PORTE   (0x05 << ADC_SIG_PORT_SHIFT)
#define ADC_SIG_PORTF   (0x06 << ADC_SIG_PORT_SHIFT)
#define ADC_SIG_NA      (0x00) /* signal not available */
#define ADC_SIG_NC      (0x01) /* signal not configurable */



/* Conversion table for ADC0x inputs, where x is 0 to 23, mux is defaultly "B" OR "X" */
  const static uint_8 adc0_conv_table_bx[] = {
    ADC_SIG_NC,         /* 0 leave as default */
    ADC_SIG_NC,         /* 1 leave as default */
    ADC_SIG_NC,         /* 2 leave as default */
    ADC_SIG_NC,         /* 3 leave as default */
    ADC_SIG_PORTC | 2,  /* ADC0_SE4b/CMP1_IN0/TSI0_CH15 */
    ADC_SIG_PORTD | 1,  /* ADC0_SE5b */
    ADC_SIG_PORTD | 5,  /* ADC0_SE6b */
    ADC_SIG_PORTD | 6,  /* ADC0_SE7b */
    ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
    ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
    ADC_SIG_PORTA | 7,  /* ADC0_SE10 */
    ADC_SIG_PORTA | 8,  /* ADC0_SE11 */
    ADC_SIG_PORTB | 2,  /* ADC0_SE12/TSI0_CH7 */
    ADC_SIG_PORTB | 3,  /* ADC0_SE13/TSI0_CH8 */
    ADC_SIG_PORTC | 0,  /* ADC0_SE14/TSI0_CH13 */
    ADC_SIG_PORTC | 1,  /* ADC0_SE15/TSI0_CH14 */
    ADC_SIG_NC,         /* ADC0_SE16/CMP1_IN2/ADC0_SE21 */
    ADC_SIG_PORTE | 24, /* ADC0_SE17/EXTAL1 */
    ADC_SIG_PORTE | 25, /* ADC0_SE18/XTAL1 */
    ADC_SIG_NC,         /* 19 leave as default */
    ADC_SIG_NC,         /* 20 leave as default */
    ADC_SIG_NC,         /* ADC0_SE16/CMP1_IN2/ADC0_SE21 */
    ADC_SIG_NC,         /* ADC1_SE16/CMP2_IN2/ADC0_SE22 */
    ADC_SIG_NC,         /* DAC0_OUT/CMP1_IN3/ADC0_SE23 */
    ADC_SIG_NA,         /* 24 not implemented */
    ADC_SIG_NA,         /* 25 not implemented */
  };

  /* Conversion table for ADC1x inputs, where x is 0 to 23, mux is defaultly "B" OR "X"*/
  const static uint_8 adc1_conv_table_bx[] = {
    ADC_SIG_NC,         /* 0 leave as default */
    ADC_SIG_NC,         /* 1 leave as default */
    ADC_SIG_NC,         /* 2 leave as default */
    ADC_SIG_NC,         /* 3 leave as default */
    ADC_SIG_PORTC | 8,  /* ADC1_SE4b/CMP0_IN2 */
    ADC_SIG_PORTC | 9,  /* ADC1_SE5b/CMP0_IN3 */
    ADC_SIG_PORTC | 10, /* ADC1_SE6b */
    ADC_SIG_PORTC | 11, /* ADC1_SE7b */
    ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
    ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
    ADC_SIG_PORTB | 4,  /* ADC1_SE10 */
    ADC_SIG_PORTB | 5,  /* ADC1_SE11 */
    ADC_SIG_PORTB | 6,  /* ADC1_SE12 */
    ADC_SIG_PORTB | 7,  /* ADC1_SE13 */
    ADC_SIG_PORTB | 10, /* ADC1_SE14 */
    ADC_SIG_PORTB | 11, /* ADC1_SE15 */
    ADC_SIG_NC,         /* ADC1_SE16/CMP2_IN2/ADC0_SE22 */
    ADC_SIG_PORTA | 17, /* ADC1_SE17 */
    ADC_SIG_NC,         /* VREF_OUT/CMP1_IN5/CMP0_IN5/ADC1_SE18 */
    ADC_SIG_NC,         /* 19 leave as default */
    ADC_SIG_NC,         /* 20 leave as default */
    ADC_SIG_NA,         /* 21 not implemented */
    ADC_SIG_NA,         /* 22 not implemented */
    ADC_SIG_NC,         /* DAC1_OUT/CMP0_IN4/CMP2_IN3/ADC1_SE23 */
    ADC_SIG_NA,         /* 24 not implemented */
    ADC_SIG_NA,         /* 25 not implemented */
  };
  /* Conversion table for ADC1x inputs, where x is 4 to 7, mux is defaultly "A" */
  const static uint_8 adc1_conv_table_a[] = {
    ADC_SIG_PORTE | 0, /* ADC1_SE4a */
    ADC_SIG_PORTE | 1, /* ADC1_SE5a */
    ADC_SIG_PORTE | 2, /* ADC1_SE6a */
    ADC_SIG_PORTE | 3  /* ADC1_SE7a */
  };
  /* Conversion table for ADC2x inputs, where x is 0 to 23, mux is defaultly "B" OR "X"*/
  const static uint_8 adc2_conv_table_bx[] = {
    ADC_SIG_NC,         /* 0 leave as default */
    ADC_SIG_NC,         /* 1 leave as default */
    ADC_SIG_NC,         /* 2 leave as default */
    ADC_SIG_NC,         /* 3 leave as default */
    ADC_SIG_NA,         /* ADC2_SE4b */
    ADC_SIG_NA,         /* ADC2_SE5b */
    ADC_SIG_NA,         /* ADC2_SE6b */
    ADC_SIG_NA,         /* ADC2_SE7b */
    ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
    ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
    ADC_SIG_NA,         /* ADC2_SE10 */
    ADC_SIG_NA,         /* ADC2_SE11 */
    ADC_SIG_PORTA | 29, /* ADC2_SE12 */
    ADC_SIG_PORTA | 28, /* ADC2_SE13 */
    ADC_SIG_PORTA | 27, /* ADC2_SE14 */
    ADC_SIG_PORTA | 26, /* ADC2_SE15 */
    ADC_SIG_PORTE | 8,  /* ADC2_SE16 */
    ADC_SIG_PORTE | 9,  /* ADC2_SE17 */
    ADC_SIG_NA,         /* 18 not implemented */
    ADC_SIG_NA,         /* 19 not implemented */
    ADC_SIG_NA,         /* 20 not implemented */
    ADC_SIG_NA,         /* 21 not implemented */
    ADC_SIG_NA,         /* 22 not implemented */
    ADC_SIG_NA,         /* 23 not implemented */
    ADC_SIG_NA,         /* 24 not implemented */
    ADC_SIG_NA,         /* 25 not implemented */
  };
  /* Conversion table for ADC2x inputs, where x is 4 to 7, mux is defaultly "A" */
  const static uint_8 adc2_conv_table_a[] = {
    ADC_SIG_PORTB | 20, /* ADC2_SE4a */
    ADC_SIG_PORTB | 21, /* ADC2_SE5a */
    ADC_SIG_NA,         /* ADC2_SE6a */
    ADC_SIG_NA          /* ADC2_SE7a */
  };
  /* Conversion table for ADC3x inputs, where x is 0 to 23, mux is defaultly "B" OR "X"*/
  const static uint_8 adc3_conv_table_bx[] = {
    ADC_SIG_NC,         /* 0 leave as default */
    ADC_SIG_NC,         /* 0 leave as default */
    ADC_SIG_NC,         /* 0 leave as default */
    ADC_SIG_NC,         /* 0 leave as default */
    ADC_SIG_PORTE | 27, /* ADC3_SE4b */
    ADC_SIG_PORTE | 26, /* ADC3_SE5b */
    ADC_SIG_NA,         /* ADC3_SE6b */
    ADC_SIG_NA,         /* ADC3_SE7b */
    ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
    ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
    ADC_SIG_NA,         /* ADC3_SE10 */
    ADC_SIG_NA,         /* ADC3_SE11 */
    ADC_SIG_NA,         /* ADC3_SE12 */
    ADC_SIG_NA,         /* ADC3_SE13 */
    ADC_SIG_NA,         /* ADC3_SE14 */
    ADC_SIG_PORTA | 11, /* ADC3_SE15 */
    ADC_SIG_PORTE | 11, /* ADC3_SE16 */
    ADC_SIG_PORTE | 12, /* ADC3_SE17 */
    ADC_SIG_NA,         /* 18 not implemented */
    ADC_SIG_NA,         /* 19 not implemented */
    ADC_SIG_NA,         /* 20 not implemented */
    ADC_SIG_NA,         /* 21 not implemented */
    ADC_SIG_NA,         /* 22 not implemented */
    ADC_SIG_NA,         /* 23 not implemented */
    ADC_SIG_NA,         /* 24 not implemented */
    ADC_SIG_NA,         /* 25 not implemented */
  };
  /* Conversion table for ADC0x inputs, where x is 4 to 7, mux is defaultly "A" */
  const static uint_8 adc3_conv_table_a[] = {
    ADC_SIG_PORTA | 10, /* ADC3_SE4a */
    ADC_SIG_PORTA | 9,  /* ADC3_SE5a */
    ADC_SIG_PORTA | 6,  /* ADC3_SE6a */
    ADC_SIG_PORTE | 28  /* ADC3_SE7a */
  };

  if ( ADC_GET_DIFF(source) && (ch > 3) )
  {
    return IO_ERROR; /* signal not available */
  }

  switch (ADC_GET_MODULE(source))
  {
  case ADC_SOURCE_MODULE(1): /* ADC0 */
    if ( ch < 26 )
    {
      gpio_port = adc0_conv_table_bx[ch];
    }
    else
    {
      gpio_port = ADC_SIG_NC;
    }
    break;
  case ADC_SOURCE_MODULE(2): /* ADC1 */
    if ( ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_A )
    {
      if ( ch < 4 || ch > 7 )
      {
        gpio_port = ADC_SIG_NA; /* channel does not exist */
      }
      else
      {
        gpio_port = adc1_conv_table_a[ch - 4];
      }
    }
    else /*ADC_SOURCE_MUXSEL_B OR ADC_SOURCE_MUXSEL_X*/
    {
      if ( ch < 26 )
      {
        gpio_port = adc1_conv_table_bx[ch];
      }
      else
      {
        gpio_port = ADC_SIG_NC;
      }
    }
    break;
  case ADC_SOURCE_MODULE(3): /* ADC2 */
    if ( ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_A )
    {
      if ( ch < 4 || ch > 7 )
      {
        gpio_port = ADC_SIG_NA; /* channel does not exist */
      }
      else
      {
        gpio_port = adc2_conv_table_a[ch - 4];
      }
    }
    else /*ADC_SOURCE_MUXSEL_B OR ADC_SOURCE_MUXSEL_X*/
    {
      if ( ch < 26 )
      {
        gpio_port = adc2_conv_table_bx[ch];
      }
      else
      {
        gpio_port = ADC_SIG_NC;
      }
    }
    break;
  case ADC_SOURCE_MODULE(4): /* ADC3 */
    if ( ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_A )
    {
      if ( ch < 4 || ch > 7 )
      {
        gpio_port = ADC_SIG_NA; /* channel does not exist */
      }
      else
      {
        gpio_port = adc3_conv_table_a[ch - 4];
      }
    }
    else /*ADC_SOURCE_MUXSEL_B OR ADC_SOURCE_MUXSEL_X*/
    {
      if ( ch < 26 )
      {
        gpio_port = adc3_conv_table_bx[ch];
      }
      else
      {
        gpio_port = ADC_SIG_NC;
      }
    }
    break;
  default :
    return IO_ERROR; /*ADC0 - ADC3 only*/
  }

  if ( gpio_port == ADC_SIG_NA )
  {
    return IO_ERROR; /* signal not available */
  }

  if ( gpio_port == ADC_SIG_NC )
  {
    return IO_OK; /* no need to configure signal */
  }

  switch (gpio_port >> ADC_SIG_PORT_SHIFT)
  {
  case 1: /* PORTA */
    pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
    break;
  case 2: /* PORTB */
    pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
    break;
  case 3: /* PORTC */
    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
    break;
  case 4: /* PORTD */
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
    break;
  case 5: /* PORTE */
    pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
    break;
  case 6: /* PORTF */
    pctl = (PORT_MemMapPtr)PORTF_BASE_PTR;
    break;
    /* There is no possibility to get other port from table */
  }
  pctl->PCR[gpio_port & 0x1F] &= ~PORT_PCR_MUX_MASK; /* set pin's multiplexer to analog */

  return IO_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_rtc_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to RTC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_rtc_io_init
(
 void
 )
{

#if PE_LDD_VERSION
  /* Check if peripheral is not used by Processor Expert RTC_LDD component */
  if ( PE_PeripheralUsed((uint_32)RTC_BASE_PTR) == TRUE )
  {
    /* IO Device used by PE Component*/
    return IO_ERROR;
  }
#endif

  /* Enable the clock gate to the RTC module. */
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_dspi_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to DSPI
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_dspi_io_init(uint_32 dev_num)
{
  SIM_MemMapPtr   sim = SIM_BASE_PTR;
  PORT_MemMapPtr  pctl;

  switch (dev_num)
  {
  case 0:
    /* Configure GPIOD for DSPI0 peripheral function */
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

    pctl->PCR[0] = PORT_PCR_MUX(2);     /* DSPI0.PCS0   */
    pctl->PCR[1] = PORT_PCR_MUX(2);     /* DSPI0.SCK    */
    pctl->PCR[2] = PORT_PCR_MUX(2);     /* DSPI0.SOUT   */
    pctl->PCR[3] = PORT_PCR_MUX(2);     /* DSPI0.SIN    */

    /* Enable clock gate to DSPI0 module */
    sim->SCGC6 |= SIM_SCGC6_DSPI0_MASK;
    break;

  case 1:
    /* Configure GPIOE for DSPI1 peripheral function     */
    pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;

    pctl->PCR[1] = PORT_PCR_MUX(2);     /* DSPI1.SOUT   */
    pctl->PCR[2] = PORT_PCR_MUX(2);     /* DSPI1.SCK    */
    pctl->PCR[3] = PORT_PCR_MUX(2);     /* DSPI1.SIN    */
    pctl->PCR[4] = PORT_PCR_MUX(2);     /* DSPI1.PCS0   */

    /* Enable clock gate to DSPI1 module */
    sim->SCGC6 |= SIM_SCGC6_DSPI1_MASK;
    break;

  case 2:
    /* Configure GPIOD for DSPI2 peripheral function     */
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

    pctl->PCR[11] = PORT_PCR_MUX(2);    /* DSPI2.PCS0   */
    pctl->PCR[12] = PORT_PCR_MUX(2);    /* DSPI2.SCK    */
    pctl->PCR[13] = PORT_PCR_MUX(2);    /* DSPI2.SOUT   */
    pctl->PCR[14] = PORT_PCR_MUX(2);    /* DSPI2.SIN    */
    pctl->PCR[15] = PORT_PCR_MUX(2);    /* DSPI2.PCS1   */

    /* Enable clock gate to DSPI2 module */
    sim->SCGC3 |= SIM_SCGC3_DSPI2_MASK;
    break;

  default:
    /* do nothing if bad dev_num was selected */
    return -1;
  }

  return MQX_OK;

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_esdhc_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to ESDHC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_esdhc_io_init
(
 uint_8  dev_num,
 uint_16 value
 )
{
  SIM_MemMapPtr   sim  = SIM_BASE_PTR;
  PORT_MemMapPtr  pctl;

  switch (dev_num)
  {
  case 0:
    /* Configure GPIO for SDHC peripheral function */
    pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;

    pctl->PCR[0] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D1  */
    pctl->PCR[1] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D0  */
    pctl->PCR[2] = value & (PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK);                                          /* ESDHC.CLK */
    pctl->PCR[3] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.CMD */
    pctl->PCR[4] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D3  */
    pctl->PCR[5] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D2  */

    /* Enable clock gate to SDHC module */
    sim->SCGC3 |= SIM_SCGC3_ESDHC_MASK;
    break;

  default:
    /* Do nothing if bad dev_num was selected */
    return -1;
  }

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_flexcan_io_init
* Returned Value   : 0 or 1 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to FLEXCAN
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_flexcan_io_init(uint_8 dev_num)
{
  volatile CAN_MemMapPtr  can_reg_ptr;
  SIM_MemMapPtr   sim = SIM_BASE_PTR;
  PORT_MemMapPtr  pctl;

//  OSC0_CR |= OSC_CR_ERCLKEN_MASK;


  switch (dev_num)
  {
  case 0:
    /* Enable clock gate to FlexCAN 0 module */
    sim->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;

    /* Select the bus clock as CAN engine clock source */
    can_reg_ptr = _bsp_get_flexcan_base_address(0);
    can_reg_ptr->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

    break;
  case 1:

    /* Enable clock gate to FlexCAN 1 module */
    sim->SCGC3 |= SIM_SCGC3_FLEXCAN1_MASK;

    /* Select the bus clock as CAN engine clock source */
    can_reg_ptr = _bsp_get_flexcan_base_address(1);
    can_reg_ptr->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

    break;

  default:
    return -1;
  }
  return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_i2c_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to I2C
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_i2c_io_init(uint_32 dev_num)
{

  PORT_MemMapPtr pctl;
  SIM_MemMapPtr sim = SIM_BASE_PTR;

  switch (dev_num)
  {
  case 0:
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

    pctl->PCR[8] = PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK;
    pctl->PCR[9] = PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK;

    sim->SCGC4 |= SIM_SCGC4_IIC2_MASK;

    break;
  case 1:
    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;

    pctl->PCR[10] = PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK;
    pctl->PCR[11] = PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK;

    sim->SCGC4 |= SIM_SCGC4_IIC1_MASK;

    break;
  default:
    /* Do nothing if bad dev_num was selected */
    return -1;
  }
  return MQX_OK;

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_enet_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to ENET
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_enet_io_init
(
 uint_32 device
 )
{
  PORT_MemMapPtr pctl;
  SIM_MemMapPtr  sim  = (SIM_MemMapPtr)SIM_BASE_PTR;

  pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
  pctl->PCR[12] = 0x00000400;     /* PTA12, RMII0_RXD1/MII0_RXD1      */
  pctl->PCR[13] = 0x00000400;     /* PTA13, RMII0_RXD0/MII0_RXD0      */
  pctl->PCR[14] = 0x00000400;     /* PTA14, RMII0_CRS_DV/MII0_RXDV    */
  pctl->PCR[15] = 0x00000400;     /* PTA15, RMII0_TXEN/MII0_TXEN      */
  pctl->PCR[16] = 0x00000400;     /* PTA16, RMII0_TXD0/MII0_TXD0      */
  pctl->PCR[17] = 0x00000400;     /* PTA17, RMII0_TXD1/MII0_TXD1      */


  pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
  pctl->PCR[0] = PORT_PCR_MUX(4) | PORT_PCR_ODE_MASK; /* PTB0, RMII0_MDIO/MII0_MDIO   */
  pctl->PCR[1] = PORT_PCR_MUX(4);                     /* PTB1, RMII0_MDC/MII0_MDC     */

#if ENETCFG_SUPPORT_PTP
  pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
  pctl->PCR[16 + MACNET_PTP_TIMER] = PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK; /* PTC16, ENET0_1588_TMR0   */
#endif

  /* Enable clock for ENET module */
  sim->SCGC2 |= SIM_SCGC2_ENET_MASK;

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_usb_io_init
(
 _mqx_uint dev_num
 )
{
  if ( dev_num == 0 )
  {
#if PE_LDD_VERSION
    /* USB clock is configured using CPU component */

    /* Check if peripheral is not used by Processor Expert USB_LDD component */
    if ( PE_PeripheralUsed((uint_32)USB0_BASE_PTR) == TRUE )
    {
      /* IO Device used by PE Component*/
      return IO_ERROR;
    }
#endif
    /* Configure USB to be clocked from PLL0 */
    SIM_SOPT2_REG(SIM_BASE_PTR) &= ~(SIM_SOPT2_USBFSRC_MASK);
    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBFSRC(1);
    /* Configure USB to be clocked from clock divider */
    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBF_CLKSEL_MASK;
    /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
    SIM_CLKDIV2_REG(SIM_BASE_PTR) &= ~(SIM_CLKDIV2_USBFSDIV_MASK | SIM_CLKDIV2_USBFSFRAC_MASK);
    SIM_CLKDIV2_REG(SIM_BASE_PTR) |= SIM_CLKDIV2_USBFSDIV(4) | SIM_CLKDIV2_USBFSFRAC_MASK;

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBFS_MASK;

    /* USB D+ and USB D- are standalone not multiplexed one-purpose pins */
    /* VREFIN for device is standalone not multiplexed one-purpose pin */

#if BSP_USB_TWR_SER2
    /* TWR-SER2 board has 2 connectors: on channel A, there is Micro-USB connector,
    ** which is not routed to TWRK60 board. On channel B, there is standard
    ** A-type host connector routed to the USB0 peripheral on TWRK60. To enable
    ** power to this connector, GPIO PB8 must be set as GPIO output
    */
    PORT_PCR_REG(PORTB_BASE_PTR, 8) = PORT_PCR_MUX(0x01);
    GPIO_PDDR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 as output
    GPIO_PDOR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 in high level
#endif
  }
  else if ( dev_num == 1 )
  {
    //Disable MPU so the module can access RAM
    MPU_CESR &= ~MPU_CESR_VLD_MASK;

//        SIM_MCR &= (uint32_t)~0x40000000UL;  /* Disconnect internal generated ULPI clock from pin */

//        SIM_CLKDIV2 |= USBHS_FRAC | USBHS_DIV; // Divide reference clock to obtain 60MHz
//        SIM_SOPT2 |= SIM_SOPT2_USBHSRC(1);     // MCGPLLCLK for the USB 60MHz CLKC source
//Select external clock for USBH controller
    SIM_SOPT2 |= SIM_SOPT2_USBH_CLKSEL_MASK;

    //Enable clock to the module
    SIM_SCGC6 |= SIM_SCGC6_USBHS_MASK;

    PORTA_PCR7 = PORT_PCR_MUX(2);   //ULPI DIR
    PORTA_PCR8 = PORT_PCR_MUX(2);   //ULPI NXT
    PORTA_PCR10 = PORT_PCR_MUX(2);  //ULPI DATA0
    PORTA_PCR11 = PORT_PCR_MUX(2);  //ULPI DATA1
    PORTA_PCR24 = PORT_PCR_MUX(2);  //ULPI DATA2
    PORTA_PCR25 = PORT_PCR_MUX(2);  //ULPI DATA3
    PORTA_PCR26 = PORT_PCR_MUX(2);  //ULPI DATA4
    PORTA_PCR27 = PORT_PCR_MUX(2);  //ULPI DATA5
    PORTA_PCR28 = PORT_PCR_MUX(2);  //ULPI DATA6
    PORTA_PCR29 = PORT_PCR_MUX(2);  //ULPI DATA7
    PORTA_PCR6 = PORT_PCR_MUX(2);   //ULPI CLK
    PORTA_PCR9 = PORT_PCR_MUX(2);   //ULPI STP
  }
  else
  {
    return IO_ERROR; //unknow controller
  }

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_dev_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_usb_dev_io_init
(
 struct usb_dev_if_struct *dev_if
 )
{
  if ( dev_if->DEV_INIT_PARAM == &_khci0_dev_init_param )
  {
    _bsp_usb_io_init(0);
  }
  else if ( dev_if->DEV_INIT_PARAM == &_ehci0_dev_init_param )
  {
    _bsp_usb_io_init(1);
  }
  else
  {
    return IO_ERROR; //unknown controller
  }

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_host_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_usb_host_io_init
(
 struct usb_host_if_struct *dev_if
 )
{
  if ( dev_if->HOST_INIT_PARAM == &_khci0_host_init_param )
  {
    _bsp_usb_io_init(0);
  }
  else if ( dev_if->HOST_INIT_PARAM == &_ehci0_host_init_param )
  {
    _bsp_usb_io_init(1);
  }
  else
  {
    return IO_ERROR; //unknown controller
  }

  return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_gpio_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to GPIO
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_gpio_io_init(void)
{
  /* Enable clock gating to all ports */
  SIM_SCGC5 |=   SIM_SCGC5_PORTA_MASK\
    | SIM_SCGC5_PORTB_MASK\
    | SIM_SCGC5_PORTC_MASK\
    | SIM_SCGC5_PORTD_MASK\
    | SIM_SCGC5_PORTE_MASK\
    | SIM_SCGC5_PORTF_MASK;

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_serial_rts_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to GPIO
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_serial_rts_init
(
 uint_32 device_index
 )
{
  PORT_MemMapPtr           pctl;

  /* set pin to RTS functionality */
  switch (device_index)
  {
  case 3:
    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
    pctl->PCR[18] = 0 | PORT_PCR_MUX(3);
    break;
  case 4:
    pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
    pctl->PCR[27] = 0 | PORT_PCR_MUX(3);
    break;
  default:
    /* not used on this board */
    break;
  }
  return MQX_OK;
}
/*FUNCTION*-------------------------------------------------------------------
* Function Name    : _bsp_nandflash_io_init
* Returned Value   :
* Comments         :
* This function performs BSP-specific initialization related to nandflash
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_nandflash_io_init
(
 void
 )

{
  SIM_MemMapPtr   sim = SIM_BASE_PTR;
  PORT_MemMapPtr  pctl;

  pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
  pctl->PCR[20] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[21] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[22] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[23] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;

  pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
  pctl->PCR[0] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[1] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[2] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[4] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[5] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[6] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[7] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[8] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[9] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[10] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[11] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[16] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;
  pctl->PCR[17] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;

  pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
  pctl->PCR[4] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[5] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
  pctl->PCR[8] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;
  pctl->PCR[9] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;
  pctl->PCR[10] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;

  SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_NFCSRC(0x00);

  /* Enable clock gate to NFC module */
  sim->SCGC3 |= SIM_SCGC3_NFC_MASK;

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _bsp_ftfx_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to LCD
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_ftfx_io_init
(
 _mqx_uint device_index
 )
{
  SIM_MemMapPtr sim = SIM_BASE_PTR;

  if ( device_index > 0 ) return IO_ERROR;

  /* Clock the controller */
  sim->SCGC6 |= SIM_SCGC6_FTFE_MASK;

  return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_sai_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to SAI
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_sai_io_init
(
 uint_32 dev_num
 )
{
  switch (dev_num)
  {
  case 0:
    /* Enable system clock to the I2S module */
    SIM_SCGC6 |= SIM_SCGC6_SAI0_MASK;

    /* Enable SSI pins */
    PORTE_PCR6 |= PORT_PCR_MUX(0x04);                   /* Configure port for MCLK output */

    /* GPIO for SSI0_BLCK */
    PORTE_PCR12 |= PORT_PCR_MUX(0x04);                  /* Configure Port for TX Bit Clock */

    /* GPIO for SSI0_FS */
    // PORTE_PCR11 |= PORT_PCR_MUX(0x04);                          /* Configure port for TX Frame Sync - Board REVA */
    // PORTA_PCR13 |= PORT_PCR_MUX(0x06);                          /* Configure port for TX Frame Sync - Board REVB */
    PORTA_PCR13 |= (PORT_PCR_MUX(0x06) | PORT_PCR_DSE_MASK);    /* Configure port for TX Frame Sync - Board REVB */

    /* GPIO for SSI0_XD */
    // PORTE_PCR10 |= PORT_PCR_MUX(0x04);                  /* Configure port for TX Data - Board REVA */
    // PORTA_PCR12 |= PORT_PCR_MUX(0x06);                  /* Configure port for TX Data - Board REVB */
    PORTA_PCR12 |= (PORT_PCR_MUX(0x06) | PORT_PCR_DSE_MASK);    /* Configure port for TX Data - Board REVB */
    PORTE_PCR7 |= PORT_PCR_MUX(0x04);                   /* Configure port for RX Data */

    break;
  default:
    break;
  }

  return MQX_OK;
}

/* EOF */
