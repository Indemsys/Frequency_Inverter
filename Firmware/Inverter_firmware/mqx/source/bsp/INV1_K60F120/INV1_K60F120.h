/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: INV1_K60F120.h$
* $Version : 3.8.5.0$
* $Date    : Oct-8-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   an application program using the kernel running on the
*   Freescale TWR-K70F120M Evaluation board.
*
*END************************************************************************/

#ifndef _INV1_K60F120_h_
  #define _INV1_K60F120_h_  1


/*----------------------------------------------------------------------
**                  HARDWARE INITIALIZATION DEFINITIONS
*/

/*
** Define the board type
*/
  #define BSP_TWR_K60F120M                     1

/*
** PROCESSOR MEMORY BOUNDS
*/
  #define BSP_PERIPH_BASE                     (CORTEX_PERIPH_BASE)

typedef void (*vector_entry)(void);

  #define BSP_INTERNAL_FLASH_BASE    0x00000000
  #define BSP_INTERNAL_FLASH_SIZE    0x00100000
  #define BSP_INTERNAL_FLASH_SECTOR_SIZE 0x1000

  #ifdef __CC_ARM
/* Keil compiler */
    #define __EXTERNAL_MRAM_ROM_BASE 0x70000000
    #define __EXTERNAL_MRAM_ROM_SIZE 0x00000000
    #define __EXTERNAL_MRAM_RAM_BASE 0x70000000
    #define __EXTERNAL_MRAM_RAM_SIZE 0x00080000
    #define __EXTERNAL_LCD_BASE      0x60010000
    #define __EXTERNAL_LCD_SIZE      0x0001FFFF
    #define __EXTERNAL_LCD_DC_BASE   0x60000000

/* This define MUST match the limit set in the linker file.
   The internal flash starts at 0x00000000, and ends at 0x00100000
   1/2 of the internal flash is set aside for user flash.  Change as needed.
   MAKE SURE THE START ADDRESS IS ALIGNED (is a multiple of) __FLASHX_SECT_SIZE
 */
    #define __FLASHX_START_ADDR     0x00080000
    #define __FLASHX_END_ADDR       0x00100000

    #define __INTERNAL_SRAM_BASE    0x1FFF0000
    #define __INTERNAL_SRAM_SIZE    0x00020000
    #define __SRAM_POOL             (pointer)0x1FFF0000

/* Uncached area is maped to sram */
    #define __UNCACHED_DATA_START_DEF   0x1FFF0000
    #define __UNCACHED_DATA_SIZE_DEF    0x00020000
    #define __UNCACHED_DATA_END_DEF     0x2000FFF0

    #define __DEFAULT_PROCESSOR_NUMBER     0
    #define __DEFAULT_INTERRUPT_STACK_SIZE 1024

    #define __VECTOR_TABLE_ROM_START 0x00000000
    #define __VECTOR_TABLE_RAM_START 0x1FFF0000 /* where .vectors_ram is placed. Check linker command file. */

  #else /* __CC_ARM */

extern uchar __EXTERNAL_MRAM_ROM_BASE[],    __EXTERNAL_MRAM_ROM_SIZE[];
extern uchar __EXTERNAL_MRAM_RAM_BASE[],    __EXTERNAL_MRAM_RAM_SIZE[];

extern uchar __EXTERNAL_LCD_BASE[],         __EXTERNAL_LCD_SIZE[];
extern uchar __EXTERNAL_LCD_DC_BASE[];
extern const uchar __FLASHX_START_ADDR[];
extern const uchar __FLASHX_END_ADDR[];
extern const uchar __FLASHX_SECT_SIZE[];

extern uchar __INTERNAL_SRAM_BASE[],        __INTERNAL_SRAM_SIZE[];

extern uchar __UNCACHED_DATA_START[],       __UNCACHED_DATA_SIZE[],     __UNCACHED_DATA_END[];

extern unsigned long __SRAM_POOL[];

extern uchar __INTERNAL_FLEXNVM_BASE[];
extern uchar __INTERNAL_FLEXNVM_SIZE[];

extern vector_entry __VECTOR_TABLE_RAM_START[]; /* defined in linker command file */
extern vector_entry __VECTOR_TABLE_ROM_START[]; /* defined in linker command file */

extern uchar __DEFAULT_PROCESSOR_NUMBER[];
extern uchar __DEFAULT_INTERRUPT_STACK_SIZE[];

  #endif /* __CC_ARM */
  #define BSP_EXTERNAL_MRAM_ROM_BASE          ((pointer)__EXTERNAL_MRAM_ROM_BASE)
  #define BSP_EXTERNAL_MRAM_ROM_SIZE          ((uint_32)__EXTERNAL_MRAM_ROM_SIZE)
  #define BSP_EXTERNAL_MRAM_RAM_BASE          ((pointer)__EXTERNAL_MRAM_RAM_BASE)
  #define BSP_EXTERNAL_MRAM_RAM_SIZE          ((uint_32)__EXTERNAL_MRAM_RAM_SIZE)


/* NAND Flash driver related macros */
  #define BSP_VIRTUAL_PAGE_SIZE               (512)
  #define BSP_ECC_SIZE                        (4) /* 4-error correction bits (8 ECC bytes) */

  #define BSP_EXTERNAL_LCD_BASE               ((pointer)__EXTERNAL_LCD_BASE)
  #define BSP_EXTERNAL_LCD_SIZE               ((uint_32)__EXTERNAL_LCD_SIZE)
  #define BSP_EXTERNAL_LCD_DC                 ((pointer)__EXTERNAL_LCD_DC_BASE)

/* Compact Flash card base address */
  #define BSP_CFCARD_BASE                     ((pointer)0xA0010000)

/* Enable modification of flash configuration bytes during loading for flash targets */
  #ifndef BSPCFG_ENABLE_CFMPROTECT
    #define BSPCFG_ENABLE_CFMPROTECT        1
  #endif
  #if !BSPCFG_ENABLE_CFMPROTECT && defined(__ICCARM__)
    #error Cannot disable CFMPROTECT field on IAR compiler. Please define BSPCFG_ENABLE_CFMPROTECT to 1.
  #endif

  #ifndef BSP_CLOCK_CONFIGURATION_STARTUP
    #define BSP_CLOCK_CONFIGURATION_STARTUP (BSP_CLOCK_CONFIGURATION_120MHZ)
  #endif

/* Init startup clock configuration is CPU_CLOCK_CONFIG_0 */
  #define BSP_CLOCK_SRC                   (CPU_XTAL_CLK_HZ)
  #define BSP_CORE_CLOCK                  (CPU_CORE_CLK_HZ_CONFIG_0)
  #define BSP_SYSTEM_CLOCK                (CPU_CORE_CLK_HZ_CONFIG_0)
  #define BSP_CLOCK                       (CPU_BUS_CLK_HZ_CONFIG_0)
  #define BSP_BUS_CLOCK                   (CPU_BUS_CLK_HZ_CONFIG_0)
  #define BSP_FLEXBUS_CLOCK               (CPU_FLEXBUS_CLK_HZ_CONFIG_0)
  #define BSP_FLASH_CLOCK                 (CPU_FLASH_CLK_HZ_CONFIG_0)

/** MGCT: <category name="BSP Hardware Options"> */

/*
** The clock tick rate in miliseconds - be cautious to keep this value such
** that it divides 1000 well
**
** MGCT: <option type="number" min="1" max="1000"/>
*/
  #ifndef BSP_ALARM_FREQUENCY
    #define BSP_ALARM_FREQUENCY             (200)
  #endif

/** MGCT: </category> */

/*
** Old clock rate definition in MS per tick, kept for compatibility
*/
  #define BSP_ALARM_RESOLUTION                (1000 / BSP_ALARM_FREQUENCY)

/*
** Define the location of the hardware interrupt vector table
*/
  #if MQX_ROM_VECTORS
    #define BSP_INTERRUPT_VECTOR_TABLE              ((uint_32)__VECTOR_TABLE_ROM_START)
  #else
    #define BSP_INTERRUPT_VECTOR_TABLE              ((uint_32)__VECTOR_TABLE_RAM_START)
  #endif

  #ifndef BSP_FIRST_INTERRUPT_VECTOR_USED
    #define BSP_FIRST_INTERRUPT_VECTOR_USED     (0)
  #endif

  #ifndef BSP_LAST_INTERRUPT_VECTOR_USED
    #define BSP_LAST_INTERRUPT_VECTOR_USED      (250)
  #endif

  #define BSP_TIMER_INTERRUPT_VECTOR              INT_SysTick
  #define BSP_TIMER_INTERRUPT_PRIORITY            2

/*
** RTC interrupt level
*/
  #define BSP_RTC_INT_LEVEL                      (4)

/*
** LPM related
*/
  #define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_POLLED (30)
  #define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_INT    (31)

/*
** GPIO board specifications
*/
  #define BSP_LED1                    (GPIO_PORT_A | GPIO_PIN11)
  #define BSP_LED2                    (GPIO_PORT_A | GPIO_PIN28)
  #define BSP_LED3                    (GPIO_PORT_A | GPIO_PIN29)
  #define BSP_LED4                    (GPIO_PORT_A | GPIO_PIN10)

  #define BSP_SW1                     (GPIO_PORT_A | GPIO_PIN19)
  #define BSP_SW2                     (GPIO_PORT_E | GPIO_PIN26)
  #define BSP_ACCEL_IRQ               (GPIO_PORT_B | GPIO_PIN4) /* onboard accelerometer IRQ pin */

  #define BSP_LED1_MUX_GPIO           (LWGPIO_MUX_A11_GPIO)
  #define BSP_LED2_MUX_GPIO           (LWGPIO_MUX_A28_GPIO)
  #define BSP_LED3_MUX_GPIO           (LWGPIO_MUX_A29_GPIO)
  #define BSP_LED4_MUX_GPIO           (LWGPIO_MUX_A10_GPIO)
  #define BSP_SW1_MUX_GPIO            (LWGPIO_MUX_A19_GPIO)
  #define BSP_SW2_MUX_GPIO            (LWGPIO_MUX_E26_GPIO)
  #define BSP_ACCEL_MUX_GPIO          (LWGPIO_MUX_B4_GPIO)

/* definitions for user applications */
  #define BSP_BUTTON1                 BSP_SW1
  #define BSP_BUTTON2                 BSP_SW2

  #define BSP_BUTTON1_MUX_GPIO        BSP_SW1_MUX_GPIO
  #define BSP_BUTTON1_MUX_IRQ         BSP_SW1_MUX_GPIO
  #define BSP_BUTTON2_MUX_GPIO        BSP_SW2_MUX_GPIO

/* HWTIMER definitions for user applications */
  #define BSP_HWTIMER1_DEV        pit_devif
  #define BSP_HWTIMER1_SOURCE_CLK (CM_CLOCK_SOURCE_BUS)
  #define BSP_HWTIMER1_ID         (0)

  #define BSP_HWTIMER2_DEV        pit_devif
  #define BSP_HWTIMER2_SOURCE_CLK (CM_CLOCK_SOURCE_BUS)
  #define BSP_HWTIMER2_ID         (1)

/* Port IRQ levels */
  #define BSP_PORTA_INT_LEVEL         (3)
  #define BSP_PORTB_INT_LEVEL         (3)
  #define BSP_PORTC_INT_LEVEL         (3)
  #define BSP_PORTD_INT_LEVEL         (3)
  #define BSP_PORTE_INT_LEVEL         (3)

/* LCD board settings */
  #define BSP_LCD_DC                  (GPIO_PORT_B | GPIO_PIN17) /* open J3 13-14 */
  #define BSP_LCD_DC_FN               1                          /* Functionality for LCD DC pin */

/* GPIO settings for resistive touch screen on LCD board */
  #define BSP_LCD_TCHRES_X_PLUS       (GPIO_PORT_B | GPIO_PIN4)
  #define BSP_LCD_TCHRES_X_MINUS      (GPIO_PORT_B | GPIO_PIN6)
  #define BSP_LCD_TCHRES_Y_PLUS       (GPIO_PORT_B | GPIO_PIN7)
  #define BSP_LCD_TCHRES_Y_MINUS      (GPIO_PORT_B | GPIO_PIN5)

  #define BSP_LCD_SPI_CHANNEL         "spi2:"

  #define BSP_LCD_X_PLUS_ADC_CHANNEL  (ADC1_SOURCE_AD10)
  #define BSP_LCD_Y_PLUS_ADC_CHANNEL  (ADC1_SOURCE_AD13)

/* ADC tchres device init struct */
  #define BSP_TCHRES_ADC_DEVICE "adc1:"
  #define BSP_TCHRES_X_TRIGGER ADC_PDB_TRIGGER
  #define BSP_TCHRES_Y_TRIGGER ADC_PDB_TRIGGER

/*-----------------------------------------------------------------------------
**                      Ethernet Info
*/
  #define BSP_ENET_DEVICE_COUNT                  MACNET_DEVICE_COUNT

/*
** MACNET interrupt levels and vectors
*/
  #define BSP_MACNET0_INT_TX_LEVEL           (4)
  #define BSP_MACNET0_INT_RX_LEVEL           (4)

  #define BSP_DEFAULT_ENET_DEVICE             0
  #define BSP_DEFAULT_ENET_OUI                { 0x00, 0x00, 0x5E, 0, 0, 0 }

/*
** The Ethernet PHY device number 0..31
*/
  #ifndef BSP_ENET0_PHY_ADDR
    #define BSP_ENET0_PHY_ADDR                 0
  #endif

/*
** PHY MII Speed (MDC - Management Data Clock)
*/
  #define BSP_ENET0_PHY_MII_SPEED             (2500000L)

/** MGCT: <category name="BSP Ethernet Options"> */

/*
** Number of receive BD's.
** MGCT: <option type="number"/>
*/
  #ifndef BSPCFG_RX_RING_LEN
    #define BSPCFG_RX_RING_LEN              8
  #endif

/*
** Number of transmit BD's.
** MGCT: <option type="number"/>
*/
  #ifndef BSPCFG_TX_RING_LEN
    #define BSPCFG_TX_RING_LEN              4
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ENET_MULTICAST
    #define BSPCFG_ENABLE_ENET_MULTICAST    1
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ENET_STATS
    #define BSPCFG_ENABLE_ENET_STATS        1
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENET_RESTORE
    #define BSPCFG_ENET_RESTORE             1
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENET_SRAM_BUF
    #define BSPCFG_ENET_SRAM_BUF            1
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_CPP
    #define BSPCFG_ENABLE_CPP               0
  #endif

/** MGCT: </category> */

/*-----------------------------------------------------------------------------
**                      DSPI
*/

  #define BSP_DSPI_INT_LEVEL               (4)
  #define BSP_SPI_MEMORY_CHANNEL           2

/*-----------------------------------------------------------------------------
**                      ESDHC
*/

  #define BSP_SDCARD_ESDHC_CHANNEL            "esdhc:"
  #define BSP_SDCARD_GPIO_DETECT              (GPIO_PORT_E | GPIO_PIN28)
  #define BSP_SDCARD_DETECT_MUX_GPIO          (LWGPIO_MUX_E28_GPIO)
  #define BSP_SDCARD_PROTECT_MUX_GPIO         (LWGPIO_MUX_E27_GPIO)

/*-----------------------------------------------------------------------------
**                      I2C
*/
  #define BSP_I2C_CLOCK                       (BSP_BUS_CLOCK)
  #define BSP_I2C0_ADDRESS                    (0x6E)
  #define BSP_I2C1_ADDRESS                    (0x6F)
  #define BSP_I2C0_BAUD_RATE                  (100000)
  #define BSP_I2C1_BAUD_RATE                  (100000)
  #define BSP_I2C0_MODE                       (I2C_MODE_MASTER)
  #define BSP_I2C1_MODE                       (I2C_MODE_MASTER)
  #define BSP_I2C0_INT_LEVEL                  (5)
  #define BSP_I2C1_INT_LEVEL                  (5)
  #define BSP_I2C0_RX_BUFFER_SIZE             (64)
  #define BSP_I2C0_TX_BUFFER_SIZE             (64)
  #define BSP_I2C1_RX_BUFFER_SIZE             (64)
  #define BSP_I2C1_TX_BUFFER_SIZE             (64)

/*-----------------------------------------------------------------------------
**                      ADC
*/
  #define ADC_MAX_MODULES                 (4)

  #define BSP_ADC_CH_POT                  (ADC1_SOURCE_AD20)

  #define BSP_ADC0_VECTOR_PRIORITY        (3)
  #define BSP_ADC1_VECTOR_PRIORITY        (3)
  #define BSP_ADC2_VECTOR_PRIORITY        (3)
  #define BSP_ADC3_VECTOR_PRIORITY        (3)

  #define BSP_PDB_VECTOR_PRIORITY         (3)


/*-----------------------------------------------------------------------------
**                      USB
*/
  #define USBCFG_MAX_DRIVERS               (2)

  #define BSP_USB_INT_LEVEL                (4)
  #define BSP_USB_TWR_SER2                 (0) /* set to 1 if TWR-SER2 (2 eth) board used (only host) */

/* USB host controler for K70x processors */
  #define USBCFG_KHCI                       1
  #define USBCFG_EHCI                       1

  #define USBCFG_DEFAULT_HOST_CONTROLLER    (&_bsp_usb_host_khci0_if)
  #define USBCFG_DEFAULT_DEVICE_CONTROLLER  (&_bsp_usb_dev_khci0_if)

/* This will be removed in the future and runtime option in the initialization struct will be used */
  #define USBCFG_REGISTER_ENDIANNESS        MQX_LITTLE_ENDIAN
  #define USBCFG_MEMORY_ENDIANNESS          MQX_LITTLE_ENDIAN

/*-----------------------------------------------------------------------------
**                  IO DEVICE DRIVERS CONFIGURATION
*/

/** MGCT: <category name="I/O Subsystem"> */

/*
** Enable I/O subsystem features in MQX. No I/O drivers and file I/O will be 
** possible without this feature.
** 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_IO_SUBSYSTEM
    #define BSPCFG_ENABLE_IO_SUBSYSTEM      1
  #endif

/** MGCT: </category> */

/** MGCT: <category name="Default Driver Installation in BSP startup"> */

/** MGCT: <category name="UART0 Settings"> */

/*
** Polled TTY device (UART0) 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_TTYA
    #define BSPCFG_ENABLE_TTYA              0
  #endif

/*
** Interrupt-driven TTY device (UART0)
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ITTYA
    #define BSPCFG_ENABLE_ITTYA             0
  #endif

/*
** TTYA and ITTYA baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
  #ifndef BSPCFG_SCI0_BAUD_RATE
    #define BSPCFG_SCI0_BAUD_RATE             921600//115200
  #endif

/*
** TTYA and ITTYA buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
  #ifndef BSPCFG_SCI0_QUEUE_SIZE
    #define BSPCFG_SCI0_QUEUE_SIZE             64
  #endif

/** MGCT: </category> */

/** MGCT: <category name="UART1 Settings"> */

/*
** Polled TTY device (UART1) 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_TTYB
    #define BSPCFG_ENABLE_TTYB              0
  #endif

/*
** Interrupt-driven TTY device (UART1)
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ITTYB
    #define BSPCFG_ENABLE_ITTYB             0
  #endif

/*
** TTYB and ITTYB baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
  #ifndef BSPCFG_SCI1_BAUD_RATE
    #define BSPCFG_SCI1_BAUD_RATE             115200
  #endif

/*
** TTYB and ITTYB buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
  #ifndef BSPCFG_SCI1_QUEUE_SIZE
    #define BSPCFG_SCI1_QUEUE_SIZE             64
  #endif

/** MGCT: </category> */

/** MGCT: <category name="UART2 Settings"> */

/*
** Polled TTY device (UART2) 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_TTYC
    #define BSPCFG_ENABLE_TTYC              0
  #endif

/*
** Interrupt-driven TTY device (UART2)
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ITTYC
    #define BSPCFG_ENABLE_ITTYC             0
  #endif

/*
** TTYC and ITTYC baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
  #ifndef BSPCFG_SCI2_BAUD_RATE
    #define BSPCFG_SCI2_BAUD_RATE             115200
  #endif

/*
** TTYC and ITTYC buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
  #ifndef BSPCFG_SCI2_QUEUE_SIZE
    #define BSPCFG_SCI2_QUEUE_SIZE             64
  #endif

/** MGCT: </category> */

/** MGCT: <category name="UART3 Settings"> */

/*
** Polled TTY device (UART3) 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_TTYD
    #define BSPCFG_ENABLE_TTYD              0
  #endif

/*
** Interrupt-driven TTY device (UART3)
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ITTYD
    #define BSPCFG_ENABLE_ITTYD             0
  #endif

/*
** TTYD and ITTYD baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
  #ifndef BSPCFG_SCI3_BAUD_RATE
    #define BSPCFG_SCI3_BAUD_RATE             115200
  #endif

/*
** TTYD and ITTYD buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
  #ifndef BSPCFG_SCI3_QUEUE_SIZE
    #define BSPCFG_SCI3_QUEUE_SIZE             64
  #endif

/** MGCT: </category> */

/** MGCT: <category name="UART4 Settings"> */

/*
** Polled TTY device (UART4) 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_TTYE
    #define BSPCFG_ENABLE_TTYE              0
  #endif

/*
** Interrupt-driven TTY device (UART4)
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ITTYE
    #define BSPCFG_ENABLE_ITTYE             0
  #endif

/*
** TTYE and ITTYE baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
  #ifndef BSPCFG_SCI4_BAUD_RATE
    #define BSPCFG_SCI4_BAUD_RATE             115200
  #endif

/*
** TTYE and ITTYE buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
  #ifndef BSPCFG_SCI4_QUEUE_SIZE
    #define BSPCFG_SCI4_QUEUE_SIZE             64
  #endif

/** MGCT: </category> */

/** MGCT: <category name="UART5 Settings"> */

/*
** Polled TTY device (UART5) 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_TTYF
    #define BSPCFG_ENABLE_TTYF              0
  #endif

/*
** Interrupt-driven TTY device (UART5)
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ITTYF
    #define BSPCFG_ENABLE_ITTYF             0
  #endif

/*
** TTY device HW signals (UART5).
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_TTYF_HW_SIGNALS
    #define BSPCFG_ENABLE_TTYF_HW_SIGNALS   0
  #endif

/*
** TTYF and ITTYF baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
  #ifndef BSPCFG_SCI5_BAUD_RATE
    #define BSPCFG_SCI5_BAUD_RATE             115200
  #endif

/*
** TTYF and ITTYF buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
  #ifndef BSPCFG_SCI5_QUEUE_SIZE
    #define BSPCFG_SCI5_QUEUE_SIZE             64
  #endif

/** MGCT: </category> */

/*
** Polled I2C0 device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_I2C0
    #define BSPCFG_ENABLE_I2C0                  1
  #endif

/*
** Polled I2C1 device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_I2C1
    #define BSPCFG_ENABLE_I2C1                  0
  #endif

/*
** Interrupt-driven I2C0 device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_II2C0
    #define BSPCFG_ENABLE_II2C0                 0
  #endif

/*
** Interrupt-driven I2C1 device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_II2C1
    #define BSPCFG_ENABLE_II2C1                 0
  #endif

/* GPIO device */
  #ifdef BSPCFG_ENABLE_GPIODEV
    #if BSPCFG_ENABLE_GPIODEV
      #error The GPIO driver was replaced by faster and smaller LWGPIO - see <MQX_INST_DIR>\mqx\examples\lwgpio\lwgpio.c example application
    #endif
  #endif

/*
** RTC device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_RTCDEV
    #define BSPCFG_ENABLE_RTCDEV                0
  #endif

/*
** PCFLASH device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_PCFLASH
    #define BSPCFG_ENABLE_PCFLASH               0
  #endif

/*
** Polled SPI0
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_SPI0
    #define BSPCFG_ENABLE_SPI0                  0
  #endif

/*
** Interrupt-driven SPI0
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ISPI0
    #define BSPCFG_ENABLE_ISPI0                 0
  #endif

/*
** Polled SPI1
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_SPI1
    #define BSPCFG_ENABLE_SPI1                  0
  #endif

/*
** Interrupt-driven SPI1
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ISPI1
    #define BSPCFG_ENABLE_ISPI1                 0
  #endif

/*
** Polled SPI2
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_SPI2
    #define BSPCFG_ENABLE_SPI2                  0
  #endif

/*
** Interrupt-driven SPI2
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ISPI2
    #define BSPCFG_ENABLE_ISPI2                 0
  #endif

/*
** ADC0
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ADC0
    #define BSPCFG_ENABLE_ADC0                  0
  #endif

/*
** ADC1
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ADC1
    #define BSPCFG_ENABLE_ADC1                  0
  #endif

/*
** ADC2
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ADC2
    #define BSPCFG_ENABLE_ADC2                  0
  #endif

/*
** ADC3
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ADC3
    #define BSPCFG_ENABLE_ADC3                  0
  #endif

/*
** FTFL
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_FLASHX
    #define BSPCFG_ENABLE_FLASHX                0
  #endif

/*
** ESDHC device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_ESDHC
    #define BSPCFG_ENABLE_ESDHC                 0
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_NANDFLASH
    #define BSPCFG_ENABLE_NANDFLASH             0
  #endif

/*
** USB DCD device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_USBDCD
    #define BSPCFG_ENABLE_USBDCD               0
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_CRC
    #define BSPCFG_ENABLE_CRC                   0
  #endif

/*
** IODEBUG device
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_IODEBUG
    #define BSPCFG_ENABLE_IODEBUG               1
  #endif

/*
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_HAS_SRAM_POOL
    #define BSPCFG_HAS_SRAM_POOL                1
  #endif

/*
** SAI device 
** MGCT: <option type="bool"/>
*/
  #ifndef BSPCFG_ENABLE_SAI
    #define BSPCFG_ENABLE_SAI                   0
  #endif
/** MGCT: </category> */

/*----------------------------------------------------------------------
**                  DEFAULT MQX INITIALIZATION DEFINITIONS
** MGCT: <category name="Default MQX initialization parameters"> 
*/

/* Defined in link.xxx */
extern uchar __KERNEL_DATA_START[];
extern uchar __KERNEL_DATA_END[];


  #if MQX_ENABLE_USER_MODE
extern uchar __KERNEL_AREA_START[];
extern uchar __KERNEL_AREA_END[];
extern uchar __USER_AREA_START[];
extern uchar __USER_AREA_END[];

    #if __IAR_SYSTEMS_ICC__
      #pragma section="USER_RW_MEMORY"
      #pragma section="USER_RO_MEMORY"
      #pragma section="USER_NO_MEMORY"
      #pragma section="USER_HEAP"
      #pragma section="USER_DEFAULT_MEMORY"
    #else
      #error MQX_ENABLE_USER_MODE is only supported on IAR tool in this MQX version
    #endif

    #define BSP_DEFAULT_START_OF_KERNEL_AREA                    ((pointer)__KERNEL_AREA_START)
    #define BSP_DEFAULT_END_OF_KERNEL_AREA                      ((pointer)__KERNEL_AREA_END)

    #define BSP_DEFAULT_START_OF_USER_DEFAULT_MEMORY            ((pointer)__sfb("USER_DEFAULT_MEMORY"))
    #define BSP_DEFAULT_END_OF_USER_DEFAULT_MEMORY              ((pointer)__sfe("USER_DEFAULT_MEMORY"))

    #define BSP_DEFAULT_START_OF_USER_HEAP                      ((pointer)__sfb("USER_HEAP"))
    #define BSP_DEFAULT_END_OF_USER_HEAP                        ((pointer)__USER_AREA_END)

    #define BSP_DEFAULT_START_OF_USER_RW_MEMORY                 ((pointer)__sfb("USER_RW_MEMORY"))
    #define BSP_DEFAULT_END_OF_USER_RW_MEMORY                   ((pointer)__sfe("USER_RW_MEMORY"))

    #define BSP_DEFAULT_START_OF_USER_RO_MEMORY                 ((pointer)__sfb("USER_RO_MEMORY"))
    #define BSP_DEFAULT_END_OF_USER_RO_MEMORY                   ((pointer)__sfe("USER_RO_MEMORY"))

    #define BSP_DEFAULT_START_OF_USER_NO_MEMORY                 ((pointer)__sfb("USER_NO_MEMORY"))
    #define BSP_DEFAULT_END_OF_USER_NO_MEMORY                   ((pointer)__sfe("USER_NO_MEMORY"))

    #define BSP_DEFAULT_MAX_USER_TASK_PRIORITY                  (0)
    #define BSP_DEFAULT_MAX_USER_TASK_COUNT                     (0)

  #endif // MQX_ENABLE_USER_MODE

  #define BSP_DEFAULT_START_OF_KERNEL_MEMORY                    ((pointer)__KERNEL_DATA_START)
  #define BSP_DEFAULT_END_OF_KERNEL_MEMORY                      ((pointer)__KERNEL_DATA_END)
  #define BSP_DEFAULT_PROCESSOR_NUMBER                          ((uint_32)__DEFAULT_PROCESSOR_NUMBER)

/* MGCT: <option type="string" quoted="false" allowempty="false"/> */
  #ifndef BSP_DEFAULT_INTERRUPT_STACK_SIZE
    #define BSP_DEFAULT_INTERRUPT_STACK_SIZE                  ((uint_32)__DEFAULT_INTERRUPT_STACK_SIZE)
  #endif

/* MGCT: <option type="list">
** <item name="1 (all levels disabled)" value="(1L)"/>
** <item name="2" value="(2L)"/>
** <item name="3" value="(3L)"/>
** <item name="4" value="(4L)"/>
** <item name="5" value="(5L)"/>
** <item name="6" value="(6L)"/>
** <item name="7" value="(7L)"/>
** </option>
*/
  #ifndef BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX
    #define BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX      (2L)
  #endif

/*
** MGCT: <option type="number"/>
*/
  #ifndef BSP_DEFAULT_MAX_MSGPOOLS
    #define BSP_DEFAULT_MAX_MSGPOOLS                          (2L)
  #endif

/*
** MGCT: <option type="number"/>
*/
  #ifndef BSP_DEFAULT_MAX_MSGQS
    #define BSP_DEFAULT_MAX_MSGQS                             (16L)
  #endif

/*
 * Other Serial console options:(do not forget to enable BSPCFG_ENABLE_TTY define if changed)
 *      "ttyf:"      OSJTAG-COM, TWR_SER  polled mode
 *      "ittyf:"     TWR-SER     interrupt mode
 *      "iodebug:"   IDE debug console
 ** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/> 
 */
  #ifndef BSP_DEFAULT_IO_CHANNEL
    #if BSPCFG_ENABLE_TTYA
      #define BSP_DEFAULT_IO_CHANNEL                        "ttya:"
      #define BSP_DEFAULT_IO_CHANNEL_DEFINED
    #else
      #define BSP_DEFAULT_IO_CHANNEL                        NULL
    #endif
  #else
/* undef is for backward compatibility with user_configh.h files which have already had it defined */
    #undef  BSP_DEFAULT_IO_CHANNEL_DEFINED
    #define BSP_DEFAULT_IO_CHANNEL_DEFINED
  #endif

/*
** MGCT: <option type="string" maxsize="1024" quoted="false" allowempty="false"/>  
*/
  #ifndef BSP_DEFAULT_IO_OPEN_MODE
//    #define BSP_DEFAULT_IO_OPEN_MODE                          (pointer) (IO_SERIAL_XON_XOFF | IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO)
    #define BSP_DEFAULT_IO_OPEN_MODE                          (pointer) (IO_SERIAL_XON_XOFF  )
  #endif

/*
** FLASHX flash memory pool minimum size
** MGCT: <option type="string" maxsize="1024" quoted="false" allowempty="false"/>  
*/
  #if BSPCFG_ENABLE_FLASHX
    #ifndef BSPCFG_FLASHX_SIZE
      #define BSPCFG_FLASHX_SIZE      0x4000
    #endif
  #else
    #undef  BSPCFG_FLASHX_SIZE
    #define BSPCFG_FLASHX_SIZE      0x0
  #endif

/** MGCT: </category> */
#endif /* _INV1_K60F120_h_  */
