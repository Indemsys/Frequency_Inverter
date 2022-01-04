/*
  PDD layer implementation for peripheral type SIM
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(SIM_PDD_H_)
#define SIM_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error SIM PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* SIM */ && \
      !defined(MCU_MK10D5) /* SIM */ && \
      !defined(MCU_MK10D7) /* SIM */ && \
      !defined(MCU_MK10F12) /* SIM */ && \
      !defined(MCU_MK10DZ10) /* SIM */ && \
      !defined(MCU_MK11D5) /* SIM */ && \
      !defined(MCU_MK11D5WS) /* SIM */ && \
      !defined(MCU_MK12D5) /* SIM */ && \
      !defined(MCU_MK20D10) /* SIM */ && \
      !defined(MCU_MK20D5) /* SIM */ && \
      !defined(MCU_MK20D7) /* SIM */ && \
      !defined(MCU_MK20F12) /* SIM */ && \
      !defined(MCU_MK20DZ10) /* SIM */ && \
      !defined(MCU_MK21D5) /* SIM */ && \
      !defined(MCU_MK21D5WS) /* SIM */ && \
      !defined(MCU_MK22D5) /* SIM */ && \
      !defined(MCU_MK30D10) /* SIM */ && \
      !defined(MCU_MK30D7) /* SIM */ && \
      !defined(MCU_MK30DZ10) /* SIM */ && \
      !defined(MCU_MK40D10) /* SIM */ && \
      !defined(MCU_MK40D7) /* SIM */ && \
      !defined(MCU_MK40DZ10) /* SIM */ && \
      !defined(MCU_MK40X256VMD100) /* SIM */ && \
      !defined(MCU_MK50D10) /* SIM */ && \
      !defined(MCU_MK50D7) /* SIM */ && \
      !defined(MCU_MK50DZ10) /* SIM */ && \
      !defined(MCU_MK51D10) /* SIM */ && \
      !defined(MCU_MK51D7) /* SIM */ && \
      !defined(MCU_MK51DZ10) /* SIM */ && \
      !defined(MCU_MK52D10) /* SIM */ && \
      !defined(MCU_MK52DZ10) /* SIM */ && \
      !defined(MCU_MK53D10) /* SIM */ && \
      !defined(MCU_MK53DZ10) /* SIM */ && \
      !defined(MCU_MK60D10) /* SIM */ && \
      !defined(MCU_MK60F12) /* SIM */ && \
      !defined(MCU_MK60F15) /* SIM */ && \
      !defined(MCU_MK60DZ10) /* SIM */ && \
      !defined(MCU_MK60N512VMD100) /* SIM */ && \
      !defined(MCU_MK61F12) /* SIM */ && \
      !defined(MCU_MK61F15) /* SIM */ && \
      !defined(MCU_MK61F12WS) /* SIM */ && \
      !defined(MCU_MK61F15WS) /* SIM */ && \
      !defined(MCU_MK70F12) /* SIM */ && \
      !defined(MCU_MK70F15) /* SIM */ && \
      !defined(MCU_MK70F12WS) /* SIM */ && \
      !defined(MCU_MK70F15WS) /* SIM */ && \
      !defined(MCU_MKL02Z4) /* SIM */ && \
      !defined(MCU_MKL04Z4) /* SIM */ && \
      !defined(MCU_MKL05Z4) /* SIM */ && \
      !defined(MCU_MKL14Z4) /* SIM */ && \
      !defined(MCU_MKL15Z4) /* SIM */ && \
      !defined(MCU_MKL16Z4) /* SIM */ && \
      !defined(MCU_MKL24Z4) /* SIM */ && \
      !defined(MCU_MKL25Z4) /* SIM */ && \
      !defined(MCU_MKL26Z4) /* SIM */ && \
      !defined(MCU_MKL34Z4) /* SIM */ && \
      !defined(MCU_MKL36Z4) /* SIM */ && \
      !defined(MCU_MKL46Z4) /* SIM */ && \
      !defined(MCU_PCK20L4) /* SIM */
  // Unsupported MCU is active
  #error SIM PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

#if (defined(MCU_MK10D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN1 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */

#elif (defined(MCU_MK10D5))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MK10D7))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */

#elif (defined(MCU_MK10DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */

#elif (defined(MCU_MK10F12))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OSC1             0x5U /**< Clock gate identifier for OSC1 */
  #define SIM_PDD_CLOCK_GATE_UART4            0xAU /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5            0xBU /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC0             0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1             0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGA             0x40U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_CAN1             0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_NFC              0x48U /**< Clock gate identifier for NAND flash controller */
  #define SIM_PDD_CLOCK_GATE_SPI2             0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_I2S1             0x4FU /**< Clock gate identifier for I2S 1/SAI 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC             0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2             0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_FTM3             0x59U /**< Clock gate identifier for FlexTimer (FTM) 3 */
  #define SIM_PDD_CLOCK_GATE_ADC1             0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC3             0x5CU /**< Clock gate identifier for Analog-to-digital converter (ADC) 3 */
  #define SIM_PDD_CLOCK_GATE_EWM              0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT              0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3            0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF             0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU             0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS      0x81U /**< Clock gate identifier for System register file */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_PORTF            0x8EU /**< Clock gate identifier for Port F control */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR1 0xA2U /**< Clock gate identifier for DMA channel mutiplexor 1 */
  #define SIM_PDD_CLOCK_GATE_CAN0             0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0             0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC              0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0             0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0             0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1             0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_ADC2             0xBCU /**< Clock gate identifier for Analog-to-digital converter (ADC) 2 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB               0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU              0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MK11D5))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_RNGA            0xA9U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0xBAU /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_DAC0            0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */

#elif (defined(MCU_MK11D5WS))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_DRY             0x82U /**< Clock gate identifier for DryIce */
  #define SIM_PDD_CLOCK_GATE_DRY_SECREG      0x83U /**< Clock gate identifier for DryIce secure storage */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_RNGA            0xA9U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0xBAU /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_DAC0            0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */

#elif (defined(MCU_MK12D5))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0xBAU /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0            0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */

#elif (defined(MCU_MK20D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN1 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */

#elif (defined(MCU_MK20D7))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */

#elif (defined(MCU_MK20DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */

#elif (defined(MCU_MK20F12))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OSC1             0x5U /**< Clock gate identifier for OSC1 */
  #define SIM_PDD_CLOCK_GATE_UART4            0xAU /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5            0xBU /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC0             0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1             0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGA             0x40U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_CAN1             0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_NFC              0x48U /**< Clock gate identifier for NAND flash controller */
  #define SIM_PDD_CLOCK_GATE_SPI2             0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_I2S1             0x4FU /**< Clock gate identifier for I2S 1/SAI 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC             0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2             0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_FTM3             0x59U /**< Clock gate identifier for FlexTimer (FTM) 3 */
  #define SIM_PDD_CLOCK_GATE_ADC1             0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC3             0x5CU /**< Clock gate identifier for Analog-to-digital converter (ADC) 3 */
  #define SIM_PDD_CLOCK_GATE_EWM              0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT              0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3            0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF             0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU             0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS      0x81U /**< Clock gate identifier for System register file */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_PORTF            0x8EU /**< Clock gate identifier for Port F control */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR1 0xA2U /**< Clock gate identifier for DMA channel mutiplexor 1 */
  #define SIM_PDD_CLOCK_GATE_CAN0             0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0             0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC              0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBHS            0xB4U /**< Clock gate identifier for USB OTG HS/FS/LS */
  #define SIM_PDD_CLOCK_GATE_USBDCD           0xB5U /**< Clock gate identifier for USB DCD */
  #define SIM_PDD_CLOCK_GATE_PDB0             0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0             0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1             0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_ADC2             0xBCU /**< Clock gate identifier for Analog-to-digital converter (ADC) 2 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB               0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU              0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MK21D5))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_RNGA            0xA9U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0xBAU /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_DAC0            0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */

#elif (defined(MCU_MK21D5WS))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_DRY             0x82U /**< Clock gate identifier for DryIce */
  #define SIM_PDD_CLOCK_GATE_DRY_SECREG      0x83U /**< Clock gate identifier for DryIce secure storage */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_RNGA            0xA9U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0xBAU /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_DAC0            0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */

#elif (defined(MCU_MK22D5))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0xBAU /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0            0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */

#elif (defined(MCU_MK30D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */

#elif (defined(MCU_MK30D7))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */

#elif (defined(MCU_MK30DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */

#elif (defined(MCU_MK40D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */

#elif (defined(MCU_MK40D7))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */

#elif (defined(MCU_MK40DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */

#elif (defined(MCU_MK40X256VMD100))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MK50D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier 0/1 */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */

#elif (defined(MCU_MK50D7))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */

#elif (defined(MCU_MK50DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */

#elif (defined(MCU_MK51D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier 0/1 */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */

#elif (defined(MCU_MK51D7))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */

#elif (defined(MCU_MK51DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */

#elif (defined(MCU_MK52D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier 0/1 */
  #define SIM_PDD_CLOCK_GATE_ENET            0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNG             0x40U /**< Clock gate identifier for Random number generator (RNG) */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MK52DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier */
  #define SIM_PDD_CLOCK_GATE_ENET            0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGB            0x40U /**< Clock gate identifier for Random number generator (RNGB) */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MK53D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier 0/1 */
  #define SIM_PDD_CLOCK_GATE_ENET            0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNG             0x40U /**< Clock gate identifier for Random number generator (RNG) */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MK53DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_OPAMP           0x15U /**< Clock gate identifier for Operational Amplifier */
  #define SIM_PDD_CLOCK_GATE_TRIAMP          0x18U /**< Clock gate identifier for Trans-impedance amplifier */
  #define SIM_PDD_CLOCK_GATE_ENET            0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGB            0x40U /**< Clock gate identifier for Random number generator (RNGB) */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_LCD             0x5EU /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MK60D10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART4 */
  #define SIM_PDD_CLOCK_GATE_ENET            0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_RNG             0x40U /**< Clock gate identifier for Random number generator (RNG) */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */

#elif (defined(MCU_MK60DZ10))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_ENET            0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_RNGB            0x40U /**< Clock gate identifier for Random number generator (RNGB) */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */

#elif (defined(MCU_MK60N512VMD100))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_UART4           0xAU  /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5           0xBU  /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_ENET            0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0            0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1            0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGB            0x40U /**< Clock gate identifier for Random number generator (RNGB) */
  #define SIM_PDD_CLOCK_GATE_CAN1            0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_SPI2            0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_SDHC            0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2            0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_ADC1            0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1            0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3           0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP) / 6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU            0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS     0x81U /**< Clock gate identifier for System Register File */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory controller */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for PDMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_CAN0            0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1            0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USBDCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB              0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU             0xC2U /**< Clock gate identifier for MPU */

#elif (defined(MCU_MKL02Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0   0x66U          /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1   0x67U          /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0  0x6AU          /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_CMP    0x73U          /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0   0x76U          /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0 0x80U          /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_PORTA  0x89U          /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB  0x8AU          /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_FTFA   0xA0U          /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_TPM0   0xB8U          /**< Clock gate identifier for FlexTimer (TPM) 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1   0xB9U          /**< Clock gate identifier for FlexTimer (TPM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0   0xBBU          /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */

#elif (defined(MCU_MKL04Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for FlexTimer (TPM) 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for FlexTimer (TPM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL05Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for FlexTimer (TPM) 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for FlexTimer (TPM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0             0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL14Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL15Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0             0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL16Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S/SAI */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0             0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL24Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL25Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0             0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL26Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S/SAI */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0             0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL34Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_LCD              0x93U /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL36Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_LCD              0x93U /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S/SAI */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0             0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif (defined(MCU_MKL46Z4))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_SPI0             0x76U /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0x77U /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_LCD              0x93U /**< Clock gate identifier for Segment LCD */
  #define SIM_PDD_CLOCK_GATE_FTFA             0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S/SAI */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_TPM0             0xB8U /**< Clock gate identifier for Low Power TPM 0 */
  #define SIM_PDD_CLOCK_GATE_TPM1             0xB9U /**< Clock gate identifier for Low Power TPM 1 */
  #define SIM_PDD_CLOCK_GATE_TPM2             0xBAU /**< Clock gate identifier for Low Power TPM 2 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DAC0             0xBFU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC8U /**< Clock gate identifier for DMA controller */

#elif ((defined(MCU_MK20D5)) || (defined(MCU_PCK20L4)))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_EWM             0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT             0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0            0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_UART0           0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1           0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2           0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_USB0            0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP             0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF            0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0          0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_TSI0            0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA           0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB           0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC           0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD           0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE           0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_FTFL            0xA0U /**< Clock gate identifier for Flash memory */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR 0xA1U /**< Clock gate identifier for DMA channel mutiplexor */
  #define SIM_PDD_CLOCK_GATE_SPI0            0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_I2S0            0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC             0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBDCD          0xB5U /**< Clock gate identifier for USB DCD */
  #define SIM_PDD_CLOCK_GATE_PDB0            0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT             0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0            0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1            0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0            0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_RTC             0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_DMA             0xC1U /**< Clock gate identifier for DMA controller */

#elif ((defined(MCU_MK60F12)) || (defined(MCU_MK60F15)))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OSC1             0x5U /**< Clock gate identifier for OSC1 */
  #define SIM_PDD_CLOCK_GATE_UART4            0xAU /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5            0xBU /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_ENET             0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0             0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1             0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGA             0x40U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_CAN1             0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_NFC              0x48U /**< Clock gate identifier for NAND flash controller */
  #define SIM_PDD_CLOCK_GATE_SPI2             0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_I2S1             0x4FU /**< Clock gate identifier for I2S 1/SAI 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC             0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2             0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_FTM3             0x59U /**< Clock gate identifier for FlexTimer (FTM) 3 */
  #define SIM_PDD_CLOCK_GATE_ADC1             0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC3             0x5CU /**< Clock gate identifier for Analog-to-digital converter (ADC) 3 */
  #define SIM_PDD_CLOCK_GATE_EWM              0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT              0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3            0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF             0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU             0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS      0x81U /**< Clock gate identifier for System register file */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_PORTF            0x8EU /**< Clock gate identifier for Port F control */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR1 0xA2U /**< Clock gate identifier for DMA channel mutiplexor 1 */
  #define SIM_PDD_CLOCK_GATE_CAN0             0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0             0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC              0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBHS            0xB4U /**< Clock gate identifier for USB OTG HS/FS/LS */
  #define SIM_PDD_CLOCK_GATE_USBDCD           0xB5U /**< Clock gate identifier for USB DCD */
  #define SIM_PDD_CLOCK_GATE_PDB0             0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0             0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1             0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_ADC2             0xBCU /**< Clock gate identifier for Analog-to-digital converter (ADC) 2 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB               0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU              0xC2U /**< Clock gate identifier for MPU */

#elif ((defined(MCU_MK61F12)) || (defined(MCU_MK61F15)))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OSC1             0x5U /**< Clock gate identifier for OSC1 */
  #define SIM_PDD_CLOCK_GATE_UART4            0xAU /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5            0xBU /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_ENET             0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0             0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1             0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGA             0x40U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_CAN1             0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_NFC              0x48U /**< Clock gate identifier for NAND flash controller */
  #define SIM_PDD_CLOCK_GATE_SPI2             0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_I2S1             0x4FU /**< Clock gate identifier for I2S 1/SAI 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC             0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2             0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_FTM3             0x59U /**< Clock gate identifier for FlexTimer (FTM) 3 */
  #define SIM_PDD_CLOCK_GATE_ADC1             0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC3             0x5CU /**< Clock gate identifier for Analog-to-digital converter (ADC) 3 */
  #define SIM_PDD_CLOCK_GATE_EWM              0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT              0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3            0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF             0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU             0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS      0x81U /**< Clock gate identifier for System register file */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_PORTF            0x8EU /**< Clock gate identifier for Port F control */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR1 0xA2U /**< Clock gate identifier for DMA channel mutiplexor 1 */
  #define SIM_PDD_CLOCK_GATE_CAN0             0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0             0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC              0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBHS            0xB4U /**< Clock gate identifier for USB OTG HS/FS/LS */
  #define SIM_PDD_CLOCK_GATE_USBDCD           0xB5U /**< Clock gate identifier for USB DCD */
  #define SIM_PDD_CLOCK_GATE_PDB0             0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0             0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1             0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_ADC2             0xBCU /**< Clock gate identifier for Analog-to-digital converter (ADC) 2 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB               0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU              0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_DRY              0x82U /**< Clock gate identifier for DryIce */
  #define SIM_PDD_CLOCK_GATE_DRY_SECREG       0x83U /**< Clock gate identifier for DryIce secure storage */
  #define SIM_PDD_CLOCK_GATE_DDR              0x4EU /**< Clock gate identifier for  Dual data rate memory controller */

#elif ((defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15WS)))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OSC1             0x5U /**< Clock gate identifier for OSC1 */
  #define SIM_PDD_CLOCK_GATE_UART4            0xAU /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5            0xBU /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_ENET             0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0             0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1             0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGA             0x40U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_CAN1             0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_NFC              0x48U /**< Clock gate identifier for NAND flash controller */
  #define SIM_PDD_CLOCK_GATE_SPI2             0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_I2S1             0x4FU /**< Clock gate identifier for I2S 1/SAI 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC             0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_FTM2             0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_FTM3             0x59U /**< Clock gate identifier for FlexTimer (FTM) 3 */
  #define SIM_PDD_CLOCK_GATE_ADC1             0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC3             0x5CU /**< Clock gate identifier for Analog-to-digital converter (ADC) 3 */
  #define SIM_PDD_CLOCK_GATE_EWM              0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT              0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3            0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF             0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU             0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS      0x81U /**< Clock gate identifier for System register file */
  #define SIM_PDD_CLOCK_GATE_DRY              0x82U /**< Clock gate identifier for DryIce */
  #define SIM_PDD_CLOCK_GATE_DRY_SECREG       0x83U /**< Clock gate identifier for DryIce secure storage */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_PORTF            0x8EU /**< Clock gate identifier for Port F control */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR1 0xA2U /**< Clock gate identifier for DMA channel mutiplexor 1 */
  #define SIM_PDD_CLOCK_GATE_CAN0             0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0             0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC              0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBHS            0xB4U /**< Clock gate identifier for USB OTG HS/FS/LS */
  #define SIM_PDD_CLOCK_GATE_USBDCD           0xB5U /**< Clock gate identifier for USB DCD */
  #define SIM_PDD_CLOCK_GATE_PDB0             0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0             0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1             0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_ADC2             0xBCU /**< Clock gate identifier for Analog-to-digital converter (ADC) 2 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB               0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU              0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_DDR              0x4EU /**< Clock gate identifier for  Dual data rate memory controller */

#elif ((defined(MCU_MK70F12)) || (defined(MCU_MK70F15)))
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OSC1             0x5U /**< Clock gate identifier for OSC1 */
  #define SIM_PDD_CLOCK_GATE_UART4            0xAU /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5            0xBU /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_ENET             0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0             0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1             0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGA             0x40U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_CAN1             0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_NFC              0x48U /**< Clock gate identifier for NAND flash controller */
  #define SIM_PDD_CLOCK_GATE_SPI2             0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_I2S1             0x4FU /**< Clock gate identifier for I2S 1/SAI 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC             0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_LCDC             0x56U /**< Clock gate identifier for Graphical LCD controller */
  #define SIM_PDD_CLOCK_GATE_FTM2             0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_FTM3             0x59U /**< Clock gate identifier for FlexTimer (FTM) 3 */
  #define SIM_PDD_CLOCK_GATE_ADC1             0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC3             0x5CU /**< Clock gate identifier for Analog-to-digital converter (ADC) 3 */
  #define SIM_PDD_CLOCK_GATE_EWM              0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT              0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3            0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF             0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU             0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS      0x81U /**< Clock gate identifier for System register file */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_PORTF            0x8EU /**< Clock gate identifier for Port F control */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR1 0xA2U /**< Clock gate identifier for DMA channel mutiplexor 1 */
  #define SIM_PDD_CLOCK_GATE_CAN0             0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0             0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC              0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBHS            0xB4U /**< Clock gate identifier for USB OTG HS/FS/LS */
  #define SIM_PDD_CLOCK_GATE_USBDCD           0xB5U /**< Clock gate identifier for USB DCD */
  #define SIM_PDD_CLOCK_GATE_PDB0             0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0             0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1             0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_ADC2             0xBCU /**< Clock gate identifier for Analog-to-digital converter (ADC) 2 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB               0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU              0xC2U /**< Clock gate identifier for MPU */
  #define SIM_PDD_CLOCK_GATE_DDR              0x4EU /**< Clock gate identifier for  Dual data rate memory controller */

#else /* (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15WS)) */
/* Clock gate identifiers */
  #define SIM_PDD_CLOCK_GATE_OSC1             0x5U /**< Clock gate identifier for OSC1 */
  #define SIM_PDD_CLOCK_GATE_UART4            0xAU /**< Clock gate identifier for UART 4 */
  #define SIM_PDD_CLOCK_GATE_UART5            0xBU /**< Clock gate identifier for UART 5 */
  #define SIM_PDD_CLOCK_GATE_ENET             0x20U /**< Clock gate identifier for Ethernet MAC and IEEE 1588 timers */
  #define SIM_PDD_CLOCK_GATE_DAC0             0x2CU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 0 */
  #define SIM_PDD_CLOCK_GATE_DAC1             0x2DU /**< Clock gate identifier for 12-bit digital-to-analog converter (DAC) 1 */
  #define SIM_PDD_CLOCK_GATE_RNGA             0x40U /**< Clock gate identifier for Random number generator accelerator (RNGA) */
  #define SIM_PDD_CLOCK_GATE_CAN1             0x44U /**< Clock gate identifier for CAN 1 */
  #define SIM_PDD_CLOCK_GATE_NFC              0x48U /**< Clock gate identifier for NAND flash controller */
  #define SIM_PDD_CLOCK_GATE_SPI2             0x4CU /**< Clock gate identifier for SPI 2 */
  #define SIM_PDD_CLOCK_GATE_DDR              0x4EU /**< Clock gate identifier for  Dual data rate memory controller */
  #define SIM_PDD_CLOCK_GATE_I2S1             0x4FU /**< Clock gate identifier for I2S 1/SAI 1 */
  #define SIM_PDD_CLOCK_GATE_SDHC             0x51U /**< Clock gate identifier for SDHC */
  #define SIM_PDD_CLOCK_GATE_LCDC             0x56U /**< Clock gate identifier for Graphical LCD controller */
  #define SIM_PDD_CLOCK_GATE_FTM2             0x58U /**< Clock gate identifier for FlexTimer (FTM) 2 */
  #define SIM_PDD_CLOCK_GATE_FTM3             0x59U /**< Clock gate identifier for FlexTimer (FTM) 3 */
  #define SIM_PDD_CLOCK_GATE_ADC1             0x5BU /**< Clock gate identifier for Analog-to-digital converter (ADC) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC3             0x5CU /**< Clock gate identifier for Analog-to-digital converter (ADC) 3 */
  #define SIM_PDD_CLOCK_GATE_EWM              0x61U /**< Clock gate identifier for External watchdog */
  #define SIM_PDD_CLOCK_GATE_CMT              0x62U /**< Clock gate identifier for Carrier modulator timer (CMT) */
  #define SIM_PDD_CLOCK_GATE_I2C0             0x66U /**< Clock gate identifier for I2C0 */
  #define SIM_PDD_CLOCK_GATE_I2C1             0x67U /**< Clock gate identifier for I2C1 */
  #define SIM_PDD_CLOCK_GATE_UART0            0x6AU /**< Clock gate identifier for UART 0 */
  #define SIM_PDD_CLOCK_GATE_UART1            0x6BU /**< Clock gate identifier for UART 1 */
  #define SIM_PDD_CLOCK_GATE_UART2            0x6CU /**< Clock gate identifier for UART 2 */
  #define SIM_PDD_CLOCK_GATE_UART3            0x6DU /**< Clock gate identifier for UART 3 */
  #define SIM_PDD_CLOCK_GATE_USB0             0x72U /**< Clock gate identifier for USB OTG FS/LS */
  #define SIM_PDD_CLOCK_GATE_CMP              0x73U /**< Clock gate identifier for High-speed comparator (CMP)/6-bit digital-to-analog converter (DAC) */
  #define SIM_PDD_CLOCK_GATE_VREF             0x74U /**< Clock gate identifier for Voltage reference (VREF) */
  #define SIM_PDD_CLOCK_GATE_LLWU             0x7CU /**< Clock gate identifier for Low-leakage wakeup unit (LLWU) */
  #define SIM_PDD_CLOCK_GATE_LPTMR0           0x80U /**< Clock gate identifier for Low-power timer (LPTMR) */
  #define SIM_PDD_CLOCK_GATE_SYSTEM_REGS      0x81U /**< Clock gate identifier for System register file */
  #define SIM_PDD_CLOCK_GATE_DRY              0x82U /**< Clock gate identifier for DryIce */
  #define SIM_PDD_CLOCK_GATE_DRY_SECREG       0x83U /**< Clock gate identifier for DryIce secure storage */
  #define SIM_PDD_CLOCK_GATE_TSI0             0x85U /**< Clock gate identifier for Touch sense interface (TSI) */
  #define SIM_PDD_CLOCK_GATE_PORTA            0x89U /**< Clock gate identifier for Port A control */
  #define SIM_PDD_CLOCK_GATE_PORTB            0x8AU /**< Clock gate identifier for Port B control */
  #define SIM_PDD_CLOCK_GATE_PORTC            0x8BU /**< Clock gate identifier for Port C control */
  #define SIM_PDD_CLOCK_GATE_PORTD            0x8CU /**< Clock gate identifier for Port D control */
  #define SIM_PDD_CLOCK_GATE_PORTE            0x8DU /**< Clock gate identifier for Port E control */
  #define SIM_PDD_CLOCK_GATE_PORTF            0x8EU /**< Clock gate identifier for Port F control */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0 0xA1U /**< Clock gate identifier for DMA channel mutiplexor 0 */
  #define SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR1 0xA2U /**< Clock gate identifier for DMA channel mutiplexor 1 */
  #define SIM_PDD_CLOCK_GATE_CAN0             0xA4U /**< Clock gate identifier for CAN 0 */
  #define SIM_PDD_CLOCK_GATE_SPI0             0xACU /**< Clock gate identifier for SPI 0 */
  #define SIM_PDD_CLOCK_GATE_SPI1             0xADU /**< Clock gate identifier for SPI 1 */
  #define SIM_PDD_CLOCK_GATE_I2S0             0xAFU /**< Clock gate identifier for I2S 0/SAI 0 */
  #define SIM_PDD_CLOCK_GATE_CRC              0xB2U /**< Clock gate identifier for CRC */
  #define SIM_PDD_CLOCK_GATE_USBHS            0xB4U /**< Clock gate identifier for USB OTG HS/FS/LS */
  #define SIM_PDD_CLOCK_GATE_USBDCD           0xB5U /**< Clock gate identifier for USB DCD */
  #define SIM_PDD_CLOCK_GATE_PDB0             0xB6U /**< Clock gate identifier for Programmable delay block (PDB) */
  #define SIM_PDD_CLOCK_GATE_PIT              0xB7U /**< Clock gate identifier for Periodic interrupt timers (PIT) */
  #define SIM_PDD_CLOCK_GATE_FTM0             0xB8U /**< Clock gate identifier for FlexTimer (FTM) 0 */
  #define SIM_PDD_CLOCK_GATE_FTM1             0xB9U /**< Clock gate identifier for FlexTimer (FTM) 1 */
  #define SIM_PDD_CLOCK_GATE_ADC0             0xBBU /**< Clock gate identifier for Analog-to-digital converter (ADC) 0 */
  #define SIM_PDD_CLOCK_GATE_ADC2             0xBCU /**< Clock gate identifier for Analog-to-digital converter (ADC) 2 */
  #define SIM_PDD_CLOCK_GATE_RTC              0xBDU /**< Clock gate identifier for Real-time clock (RTC) */
  #define SIM_PDD_CLOCK_GATE_FB               0xC0U /**< Clock gate identifier for FlexBus */
  #define SIM_PDD_CLOCK_GATE_DMA              0xC1U /**< Clock gate identifier for DMA controller */
  #define SIM_PDD_CLOCK_GATE_MPU              0xC2U /**< Clock gate identifier for MPU */

#endif /* (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15WS)) */

/* ----------------------------------------------------------------------------
   -- SetClockGate
   ---------------------------------------------------------------------------- */

#if (defined(MCU_MKL02Z4))
/**
 * @brief Enable or disable clock gate for specified device.
 * @param PeripheralBase Peripheral base address.
 * @param Index Parameter specifying which device will be addressed. Use
 *        constants from group 'Clock gate identifiers'. This parameter is of index type.
 * @param State Parameter specifying if clock gate will be enabled or disabled.
 *        This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SIM_SCGC1, SIM_SCGC2,
 *          SIM_SCGC3, SIM_SCGC4, SIM_SCGC5, SIM_SCGC6, SIM_SCGC7 (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      SIM_PDD_SetClockGate(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
  #define SIM_PDD_SetClockGate(PeripheralBase, Index, State) ( \
      ((uint32_t)((uint32_t)(Index) >> 5U) == 0x3U) ? ( \
        SIM_SCGC4_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC4_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x4U) ? ( \
        SIM_SCGC5_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC5_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : ( \
        SIM_SCGC6_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC6_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) \
      ) \
    )
#elif ((defined(MCU_MK10D5)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) || (defined(MCU_PCK20L4)))
/**
 * @brief Enable or disable clock gate for specified device.
 * @param PeripheralBase Peripheral base address.
 * @param Index Parameter specifying which device will be addressed. Use
 *        constants from group 'Clock gate identifiers'. This parameter is of index type.
 * @param State Parameter specifying if clock gate will be enabled or disabled.
 *        This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SIM_SCGC1, SIM_SCGC2,
 *          SIM_SCGC3, SIM_SCGC4, SIM_SCGC5, SIM_SCGC6, SIM_SCGC7 (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      SIM_PDD_SetClockGate(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
  #define SIM_PDD_SetClockGate(PeripheralBase, Index, State) ( \
      ((uint32_t)((uint32_t)(Index) >> 5U) == 0x3U) ? ( \
        SIM_SCGC4_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC4_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x4U) ? ( \
        SIM_SCGC5_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC5_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x5U) ? ( \
        SIM_SCGC6_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC6_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : ( \
        SIM_SCGC7_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC7_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) \
      )) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) */
/**
 * @brief Enable or disable clock gate for specified device.
 * @param PeripheralBase Peripheral base address.
 * @param Index Parameter specifying which device will be addressed. Use
 *        constants from group 'Clock gate identifiers'. This parameter is of index type.
 * @param State Parameter specifying if clock gate will be enabled or disabled.
 *        This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SIM_SCGC1, SIM_SCGC2,
 *          SIM_SCGC3, SIM_SCGC4, SIM_SCGC5, SIM_SCGC6, SIM_SCGC7 (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      SIM_PDD_SetClockGate(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
  #define SIM_PDD_SetClockGate(PeripheralBase, Index, State) ( \
      ((uint32_t)((uint32_t)(Index) >> 5U) == 0U) ? ( \
        SIM_SCGC1_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC1_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x1U) ? ( \
        SIM_SCGC2_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC2_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x2U) ? ( \
        SIM_SCGC3_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC3_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x3U) ? ( \
        SIM_SCGC4_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC4_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x4U) ? ( \
        SIM_SCGC5_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC5_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : (((uint32_t)((uint32_t)(Index) >> 5U) == 0x5U) ? ( \
        SIM_SCGC6_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC6_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) : ( \
        SIM_SCGC7_REG(PeripheralBase) = \
         (uint32_t)(( \
          (uint32_t)(( \
           SIM_SCGC7_REG(PeripheralBase)) & ( \
           (uint32_t)(~(uint32_t)((uint32_t)0x1U << (uint8_t)((uint8_t)(Index) & 0x1FU)))))) | ( \
          (uint32_t)((uint32_t)(State) << (uint8_t)((uint8_t)(Index) & 0x1FU))))) \
      ))))) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) */
#endif  /* #if defined(SIM_PDD_H_) */

/* SIM_PDD.h, eof. */
