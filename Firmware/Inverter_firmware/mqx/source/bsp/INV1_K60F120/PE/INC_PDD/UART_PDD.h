/*
  PDD layer implementation for peripheral type UART
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(UART_PDD_H_)
#define UART_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error UART PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK10D5) /* UART0, UART1, UART2 */ && \
      !defined(MCU_MK10D7) /* UART0, UART1, UART2, UART3, UART4 */ && \
      !defined(MCU_MK10F12) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK10DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK11D5) /* UART0, UART1, UART2, UART3 */ && \
      !defined(MCU_MK11D5WS) /* UART0, UART1, UART2, UART3 */ && \
      !defined(MCU_MK12D5) /* UART0, UART1, UART2, UART3 */ && \
      !defined(MCU_MK20D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK20D5) /* UART0, UART1, UART2 */ && \
      !defined(MCU_MK20D7) /* UART0, UART1, UART2, UART3, UART4 */ && \
      !defined(MCU_MK20F12) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK20DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK21D5) /* UART0, UART1, UART2, UART3 */ && \
      !defined(MCU_MK21D5WS) /* UART0, UART1, UART2, UART3 */ && \
      !defined(MCU_MK22D5) /* UART0, UART1, UART2, UART3 */ && \
      !defined(MCU_MK30D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK30D7) /* UART0, UART1, UART2, UART3, UART4 */ && \
      !defined(MCU_MK30DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK40D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK40D7) /* UART0, UART1, UART2, UART3, UART4 */ && \
      !defined(MCU_MK40DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK40X256VMD100) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK50D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK50D7) /* UART0, UART1, UART2, UART3, UART4 */ && \
      !defined(MCU_MK50DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK51D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK51D7) /* UART0, UART1, UART2, UART3, UART4 */ && \
      !defined(MCU_MK51DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK52D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK52DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK53D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK53DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK60D10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK60F12) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK60F15) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK60DZ10) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK60N512VMD100) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK61F12) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK61F15) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK61F12WS) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK61F15WS) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK70F12) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK70F15) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK70F12WS) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MK70F15WS) /* UART0, UART1, UART2, UART3, UART4, UART5 */ && \
      !defined(MCU_MKL14Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL15Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL16Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL24Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL25Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL26Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL34Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL36Z4) /* UART1, UART2 */ && \
      !defined(MCU_MKL46Z4) /* UART1, UART2 */ && \
      !defined(MCU_PCK20L4) /* UART0, UART1, UART2 */
  // Unsupported MCU is active
  #error UART PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Interrupt masks */
#define UART_PDD_INTERRUPT_TRANSMITTER          UART_C2_TIE_MASK /**< Transmitter interrupt enable mask */
#define UART_PDD_INTERRUPT_TRANSMITTER_COMPLETE UART_C2_TCIE_MASK /**< Transmitter complete interrupt enable mask */
#define UART_PDD_INTERRUPT_RECEIVER             UART_C2_RIE_MASK /**< Receiver interrupt enable mask */
#define UART_PDD_INTERRUPT_IDLE                 UART_C2_ILIE_MASK /**< Idle interrupt enable mask */
#define UART_PDD_INTERRUPT_PARITY_ERROR         UART_C3_PEIE_MASK /**< Parity error interrupt enable mask */
#define UART_PDD_INTERRUPT_FRAMING_ERROR        UART_C3_FEIE_MASK /**< Framing error interrupt enable mask */
#define UART_PDD_INTERRUPT_NOISE_ERROR          UART_C3_NEIE_MASK /**< Noise error interrupt enable mask */
#define UART_PDD_INTERRUPT_OVERRUN_ERROR        UART_C3_ORIE_MASK /**< Overrun error interrupt enable mask */

/* Fifo masks */
#define UART_PDD_TX_FIFO_ENABLE UART_PFIFO_TXFE_MASK /**< Transmitter FIFO enable mask */
#define UART_PDD_RX_FIFO_ENABLE UART_PFIFO_RXFE_MASK /**< Receiver FIFO enable mask */
#define UART_PDD_TX_FIFO_FLUSH  UART_CFIFO_TXFLUSH_MASK /**< Transmitter FIFO flush command mask */
#define UART_PDD_RX_FIFO_FLUSH  UART_CFIFO_RXFLUSH_MASK /**< Receiver FIFO flush command mask */

/* Parity types */
#define UART_PDD_PARITY_NONE 0U                  /**< No parity */
#define UART_PDD_PARITY_EVEN 0x2U                /**< Even parity */
#define UART_PDD_PARITY_ODD  0x3U                /**< Even parity */

#if ((defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/* Data width */
  #define UART_PDD_WIDTH_8 0U                      /**< 8-bit communication */
  #define UART_PDD_WIDTH_9 0x10U                   /**< 9-bit communication */

#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/* Data width */
  #define UART_PDD_WIDTH_8  0U                     /**< 8-bit communication */
  #define UART_PDD_WIDTH_9  0x1U                   /**< 9-bit communication */
  #define UART_PDD_WIDTH_10 0x2U                   /**< 10-bit communication (10th bit can be used only as parity bit) */

#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/* Loop mode */
#define UART_PDD_LOOP_MODE_NORMAL     0U         /**< Normal operation mode. No loopback selected. */
#define UART_PDD_LOOP_MODE_LOCAL_LOOP 0x80U      /**< Local loopback mode. */

/* Position of a parity bit */
#define UART_PDD_PARITY_BIT_POSITION_9  0U       /**< Parity bit is the 9-th bit in the serial transmission */
#define UART_PDD_PARITY_BIT_POSITION_10 0x20U    /**< Parity bit is the 10-th bit in the serial transmission */

/* Receiver power states. */
#define UART_PDD_POWER_NORMAL  0U                /**< Normal operation. */
#define UART_PDD_POWER_STANDBY 0x2U              /**< Standby mode (waiting for a wakeup condition). */

/* Stop bit lengths */
#define UART_PDD_STOP_BIT_LEN_1 0U               /**< One stop bit. */
#define UART_PDD_STOP_BIT_LEN_2 0x20U            /**< Two stop bits. */


/* ----------------------------------------------------------------------------
   -- EnableTransmitter
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables UART transmitter.
 * @param PeripheralBase Peripheral base address.
 * @param State Enables or disables transmitter. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART1_C2,
 *          UART2_C2, UART3_C2, UART4_C2, UART5_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_EnableTransmitter(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define UART_PDD_EnableTransmitter(PeripheralBase, State) ( \
    UART_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C2_TE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << UART_C2_TE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableReceiver
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables UART receiver.
 * @param PeripheralBase Peripheral base address.
 * @param State Enables or disables receiver. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE
 *        and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART1_C2,
 *          UART2_C2, UART3_C2, UART4_C2, UART5_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_EnableReceiver(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define UART_PDD_EnableReceiver(PeripheralBase, State) ( \
    UART_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C2_RE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << UART_C2_RE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadDataReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Reads data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_D, UART1_D,
 *          UART2_D, UART3_D, UART4_D, UART5_D (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = UART_PDD_ReadDataReg(deviceID);
 *      @endcode
 */
#define UART_PDD_ReadDataReg(PeripheralBase) ( \
    UART_D_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteDataReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes data specified by the Value parameter into data register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value to be written to the data register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_D, UART1_D,
 *          UART2_D, UART3_D, UART4_D, UART5_D (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_WriteDataReg(deviceID, 1);
 *      @endcode
 */
#define UART_PDD_WriteDataReg(PeripheralBase, Value) ( \
    UART_D_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- PutChar8
   ---------------------------------------------------------------------------- */

/**
 * @brief Puts 8-bits character into the transmit buffer.
 * @param PeripheralBase Peripheral base address.
 * @param Char 8-bits character to be written to the data register. This
 *        parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_D, UART1_D,
 *          UART2_D, UART3_D, UART4_D, UART5_D (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_PutChar8(deviceID, 1);
 *      @endcode
 */
#define UART_PDD_PutChar8(PeripheralBase, Char) ( \
    UART_D_REG(PeripheralBase) = \
     (uint8_t)(Char) \
  )

/* ----------------------------------------------------------------------------
   -- PutChar9
   ---------------------------------------------------------------------------- */

/**
 * @brief Puts 9-bits character into the transmit buffer.
 * @param PeripheralBase Peripheral base address.
 * @param Char 9-bits character to be written to the data register. This
 *        parameter is a 9-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C3, UART0_D,
 *          UART1_C3, UART1_D, UART2_C3, UART2_D, UART3_C3, UART3_D, UART4_C3,
 *          UART4_D, UART5_C3, UART5_D (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_PutChar9(deviceID, 1);
 *      @endcode
 */
#define UART_PDD_PutChar9(PeripheralBase, Char) ( \
    (UART_C3_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C3_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C3_T8_MASK))) | ( \
      (uint8_t)((uint8_t)((uint16_t)(Char) >> 8U) << UART_C3_T8_SHIFT)))), \
    (UART_D_REG(PeripheralBase) = \
     (uint8_t)(Char)) \
  )

/* ----------------------------------------------------------------------------
   -- GetChar8
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns a 8-bit character from the receive buffer.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_D, UART1_D,
 *          UART2_D, UART3_D, UART4_D, UART5_D (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = UART_PDD_GetChar8(deviceID);
 *      @endcode
 */
#define UART_PDD_GetChar8(PeripheralBase) ( \
    UART_D_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetChar9Bit
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the 9th bit of the character from the receive buffer shifted
 * to its bit position (9th). Must be called prior to calling GetChar8 to read the
 * whole 9-bit character.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 9-bit value. The value is cast to "uint16_t".
 * @remarks The macro accesses the following registers: UART0_C3, UART1_C3,
 *          UART2_C3, UART3_C3, UART4_C3, UART5_C3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint16_t result = UART_PDD_GetChar9Bit(deviceID);
 *      @endcode
 */
#define UART_PDD_GetChar9Bit(PeripheralBase) ( \
    (uint16_t)(( \
     (uint16_t)((uint16_t)(UART_C3_REG(PeripheralBase) & UART_C3_R8_MASK) >> UART_C3_R8_SHIFT)) << ( \
     8U)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables interrupts specified by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Interrupt masks". This
 *        parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART0_C3,
 *          UART1_C2, UART1_C3, UART2_C2, UART2_C3, UART3_C2, UART3_C3, UART4_C2,
 *          UART4_C3, UART5_C2, UART5_C3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_EnableInterrupt(deviceID, UART_PDD_INTERRUPT_TRANSMITTER);
 *      @endcode
 */
#define UART_PDD_EnableInterrupt(PeripheralBase, Mask) ( \
    (UART_C2_REG(PeripheralBase) |= \
     (uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU))), \
    (UART_C3_REG(PeripheralBase) |= \
     (uint8_t)((uint8_t)(Mask) & 0xFU)) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables interrupts specified by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Interrupt masks". This
 *        parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART0_C3,
 *          UART1_C2, UART1_C3, UART2_C2, UART2_C3, UART3_C2, UART3_C3, UART4_C2,
 *          UART4_C3, UART5_C2, UART5_C3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_DisableInterrupt(deviceID, UART_PDD_INTERRUPT_TRANSMITTER);
 *      @endcode
 */
#define UART_PDD_DisableInterrupt(PeripheralBase, Mask) ( \
    (UART_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU)))), \
    (UART_C3_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & 0xFU))) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxCompleteInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets the status of the transmiter complete interrupt enable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_C2, UART1_C2,
 *          UART2_C2, UART3_C2, UART4_C2, UART5_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = UART_PDD_GetTxCompleteInterruptMask(deviceID);
 *      @endcode
 */
#define UART_PDD_GetTxCompleteInterruptMask(PeripheralBase) ( \
    (uint8_t)(UART_C2_REG(PeripheralBase) & UART_C2_TCIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxCompleteStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets the status of the transmiter complete interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_S1, UART1_S1,
 *          UART2_S1, UART3_S1, UART4_S1, UART5_S1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = UART_PDD_GetTxCompleteStatus(deviceID);
 *      @endcode
 */
#define UART_PDD_GetTxCompleteStatus(PeripheralBase) ( \
    (uint8_t)(UART_S1_REG(PeripheralBase) & UART_S1_TC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetParity
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets a communication parity type.
 * @param PeripheralBase Peripheral base address.
 * @param Parity Parity type. This parameter is of "Parity types" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C1, UART1_C1,
 *          UART2_C1, UART3_C1, UART4_C1, UART5_C1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetParity(deviceID, UART_PDD_PARITY_NONE);
 *      @endcode
 */
#define UART_PDD_SetParity(PeripheralBase, Parity) ( \
    UART_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)0x3U))) | ( \
      (uint8_t)(Parity))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDataWidth
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Sets the communication width.
 * @param PeripheralBase Peripheral base address.
 * @param Width Data width. This parameter is of "Data width" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C1, UART0_C4,
 *          UART1_C1, UART1_C4, UART2_C1, UART2_C4, UART3_C1, UART3_C4, UART4_C1,
 *          UART4_C4, UART5_C1, UART5_C4 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetDataWidth(deviceID, UART_PDD_WIDTH_8);
 *      @endcode
 */
  #define UART_PDD_SetDataWidth(PeripheralBase, Width) ( \
      UART_C1_REG(PeripheralBase) = \
       (uint8_t)(( \
        (uint8_t)(UART_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C1_M_MASK))) | ( \
        (uint8_t)(Width))) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Sets the communication width.
 * @param PeripheralBase Peripheral base address.
 * @param Width Data width. This parameter is of "Data width" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C1, UART0_C4,
 *          UART1_C1, UART1_C4, UART2_C1, UART2_C4, UART3_C1, UART3_C4, UART4_C1,
 *          UART4_C4, UART5_C1, UART5_C4 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetDataWidth(deviceID, UART_PDD_WIDTH_8);
 *      @endcode
 */
  #define UART_PDD_SetDataWidth(PeripheralBase, Width) ( \
      ( \
       ((Width) == UART_PDD_WIDTH_8) ? ( \
        UART_C1_REG(PeripheralBase) &= \
         (uint8_t)(~(uint8_t)UART_C1_M_MASK)) : (((Width) == UART_PDD_WIDTH_9) ? ( \
        UART_C1_REG(PeripheralBase) |= \
         UART_C1_M_MASK) : ( \
        UART_C1_REG(PeripheralBase) |= \
         UART_C1_M_MASK) \
      )), \
      ( \
       ((Width) == UART_PDD_WIDTH_8) ? ( \
        UART_C4_REG(PeripheralBase) &= \
         (uint8_t)(~(uint8_t)UART_C4_M10_MASK)) : (((Width) == UART_PDD_WIDTH_9) ? ( \
        UART_C4_REG(PeripheralBase) &= \
         (uint8_t)(~(uint8_t)UART_C4_M10_MASK)) : ( \
        UART_C4_REG(PeripheralBase) |= \
         UART_C4_M10_MASK) \
      )) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- SetLoopMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects the loop mode operation.
 * @param PeripheralBase Peripheral base address.
 * @param LoopMode Loop mode. This parameter is of "Loop mode" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C1, UART1_C1,
 *          UART2_C1, UART3_C1, UART4_C1, UART5_C1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetLoopMode(deviceID, UART_PDD_LOOP_MODE_NORMAL);
 *      @endcode
 */
#define UART_PDD_SetLoopMode(PeripheralBase, LoopMode) ( \
    UART_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C1_LOOPS_MASK))) | ( \
      (uint8_t)(LoopMode))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInterruptStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the interrupt status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_S1, UART1_S1,
 *          UART2_S1, UART3_S1, UART4_S1, UART5_S1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = UART_PDD_ReadInterruptStatusReg(deviceID);
 *      @endcode
 */
#define UART_PDD_ReadInterruptStatusReg(PeripheralBase) ( \
    UART_S1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetBaudRate
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets new baud rate value.
 * @param PeripheralBase Peripheral base address.
 * @param BaudRate New baud rate value. This parameter is a 13-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_BDH, UART0_BDL,
 *          UART1_BDH, UART1_BDL, UART2_BDH, UART2_BDL, UART3_BDH, UART3_BDL,
 *          UART4_BDH, UART4_BDL, UART5_BDH, UART5_BDL (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetBaudRate(deviceID, 1);
 *      @endcode
 */
#define UART_PDD_SetBaudRate(PeripheralBase, BaudRate) ( \
    (UART_BDH_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_BDH_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_BDH_SBR_MASK))) | ( \
      (uint8_t)((uint16_t)(BaudRate) >> 8U)))), \
    (UART_BDL_REG(PeripheralBase) = \
     (uint8_t)(BaudRate)) \
  )

/* ----------------------------------------------------------------------------
   -- SetBaudRateFineAdjust
   ---------------------------------------------------------------------------- */

/**
 * @brief Set new baud rate fine adjust value.
 * @param PeripheralBase Peripheral base address.
 * @param FineAdjust New baud rate fine adjust value. This parameter is a 5-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C4, UART1_C4,
 *          UART2_C4, UART3_C4, UART4_C4, UART5_C4 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetBaudRateFineAdjust(deviceID, 1);
 *      @endcode
 */
#define UART_PDD_SetBaudRateFineAdjust(PeripheralBase, FineAdjust) ( \
    UART_C4_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C4_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C4_BRFA_MASK))) | ( \
      (uint8_t)(FineAdjust))) \
  )

/* ----------------------------------------------------------------------------
   -- SetBreak
   ---------------------------------------------------------------------------- */

/**
 * @brief Set the break signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART1_C2,
 *          UART2_C2, UART3_C2, UART4_C2, UART5_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetBreak(deviceID);
 *      @endcode
 */
#define UART_PDD_SetBreak(PeripheralBase) ( \
    UART_C2_REG(PeripheralBase) |= \
     UART_C2_SBK_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearBreak
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the break signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART1_C2,
 *          UART2_C2, UART3_C2, UART4_C2, UART5_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_ClearBreak(deviceID);
 *      @endcode
 */
#define UART_PDD_ClearBreak(PeripheralBase) ( \
    UART_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)UART_C2_SBK_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetBreak
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_S1, UART1_S1,
 *          UART2_S1, UART3_S1, UART4_S1, UART5_S1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = UART_PDD_GetBreak(deviceID);
 *      @endcode
 */
#define UART_PDD_GetBreak(PeripheralBase) ( \
    (( \
      (uint8_t)(UART_S1_REG(PeripheralBase) & (uint8_t)(UART_S1_FE_MASK | UART_S1_RDRF_MASK))) == ( \
      (uint8_t)(UART_S1_FE_MASK | UART_S1_RDRF_MASK))) ? ( \
      0x1U) : ( \
      0U) \
  )

/* ----------------------------------------------------------------------------
   -- SendBreak
   ---------------------------------------------------------------------------- */

/**
 * @brief Send the break character.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART1_C2,
 *          UART2_C2, UART3_C2, UART4_C2, UART5_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SendBreak(deviceID);
 *      @endcode
 */
#define UART_PDD_SendBreak(PeripheralBase) ( \
    (UART_C2_REG(PeripheralBase) |= \
     UART_C2_SBK_MASK), \
    (UART_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)UART_C2_SBK_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- SetPositionOfParityBit
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the position of the parity bit.
 * @param PeripheralBase Peripheral base address.
 * @param Position Position of a parity bit. This parameter is of "Position of a
 *        parity bit" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C4, UART1_C4,
 *          UART2_C4, UART3_C4, UART4_C4, UART5_C4 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetPositionOfParityBit(deviceID,
 *      UART_PDD_PARITY_BIT_POSITION_9);
 *      @endcode
 */
#define UART_PDD_SetPositionOfParityBit(PeripheralBase, Position) ( \
    UART_C4_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C4_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C4_M10_MASK))) | ( \
      (uint8_t)(Position))) \
  )

/* ----------------------------------------------------------------------------
   -- SetReceiverPowerState
   ---------------------------------------------------------------------------- */

/**
 * @brief Places the receiver in a standby state where it waits for automatic
 * hardware detection of a selected wakeup condition.
 * @param PeripheralBase Peripheral base address.
 * @param State Receiver power state to be set. This parameter is of "Receiver
 *        power states." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART1_C2,
 *          UART2_C2, UART3_C2, UART4_C2, UART5_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetReceiverPowerState(deviceID, UART_PDD_POWER_NORMAL);
 *      @endcode
 */
#define UART_PDD_SetReceiverPowerState(PeripheralBase, State) ( \
    UART_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_C2_RWU_MASK))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableFifo
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables FIFO specified by the FifoMask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param FifoMask Specifies receive or transmit FIFO. Use constants from group
 *        "Fifo masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_PFIFO,
 *          UART1_PFIFO, UART2_PFIFO, UART3_PFIFO, UART4_PFIFO, UART5_PFIFO (depending on
 *          the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_EnableFifo(deviceID, UART_PDD_TX_FIFO_ENABLE);
 *      @endcode
 */
#define UART_PDD_EnableFifo(PeripheralBase, FifoMask) ( \
    UART_PFIFO_REG(PeripheralBase) |= \
     (uint8_t)(FifoMask) \
  )

/* ----------------------------------------------------------------------------
   -- FlushFifo
   ---------------------------------------------------------------------------- */

/**
 * @brief Flushes FIFO specified by the FifoMask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param FifoMask Specifies receive or transmit FIFO. Use constants from group
 *        "Fifo masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_CFIFO,
 *          UART1_CFIFO, UART2_CFIFO, UART3_CFIFO, UART4_CFIFO, UART5_CFIFO (depending on
 *          the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_FlushFifo(deviceID, UART_PDD_TX_FIFO_ENABLE);
 *      @endcode
 */
#define UART_PDD_FlushFifo(PeripheralBase, FifoMask) ( \
    UART_CFIFO_REG(PeripheralBase) |= \
     (uint8_t)(FifoMask) \
  )

/* ----------------------------------------------------------------------------
   -- SetStopBitLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the number of stop bits.
 * @param PeripheralBase Peripheral base address.
 * @param Length Stop bit length. This parameter is of "Stop bit lengths" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART1_BDH, UART2_BDH
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_SetStopBitLength(deviceID, UART_PDD_STOP_BIT_LEN_1);
 *      @endcode
 */
#define UART_PDD_SetStopBitLength(PeripheralBase, Length) ( \
    UART_BDH_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART_BDH_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART_BDH_SBNS_MASK))) | ( \
      (uint8_t)(Length))) \
  )
#endif  /* #if defined(UART_PDD_H_) */

/* UART_PDD.h, eof. */
