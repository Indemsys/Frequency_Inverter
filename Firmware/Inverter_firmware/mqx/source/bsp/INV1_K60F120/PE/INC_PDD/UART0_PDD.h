/*
  PDD layer implementation for peripheral type UART0
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(UART0_PDD_H_)
#define UART0_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error UART0 PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MKL02Z4) /* UART0 */ && \
      !defined(MCU_MKL04Z4) /* UART0 */ && \
      !defined(MCU_MKL05Z4) /* UART0 */ && \
      !defined(MCU_MKL14Z4) /* UART0 */ && \
      !defined(MCU_MKL15Z4) /* UART0 */ && \
      !defined(MCU_MKL16Z4) /* UART0 */ && \
      !defined(MCU_MKL24Z4) /* UART0 */ && \
      !defined(MCU_MKL25Z4) /* UART0 */ && \
      !defined(MCU_MKL26Z4) /* UART0 */ && \
      !defined(MCU_MKL34Z4) /* UART0 */ && \
      !defined(MCU_MKL36Z4) /* UART0 */ && \
      !defined(MCU_MKL46Z4) /* UART0 */
  // Unsupported MCU is active
  #error UART0 PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Interrupt masks */
#define UART0_PDD_INTERRUPT_TRANSMITTER          UART0_C2_TIE_MASK /**< Transmitter interrupt enable mask */
#define UART0_PDD_INTERRUPT_TRANSMITTER_COMPLETE UART0_C2_TCIE_MASK /**< Transmitter complete interrupt enable mask */
#define UART0_PDD_INTERRUPT_RECEIVER             UART0_C2_RIE_MASK /**< Receiver interrupt enable mask */
#define UART0_PDD_INTERRUPT_IDLE                 UART0_C2_ILIE_MASK /**< Idle interrupt enable mask */
#define UART0_PDD_INTERRUPT_PARITY_ERROR         UART0_C3_PEIE_MASK /**< Parity error interrupt enable mask */
#define UART0_PDD_INTERRUPT_FRAMING_ERROR        UART0_C3_FEIE_MASK /**< Framing error interrupt enable mask */
#define UART0_PDD_INTERRUPT_NOISE_ERROR          UART0_C3_NEIE_MASK /**< Noise error interrupt enable mask */
#define UART0_PDD_INTERRUPT_OVERRUN_ERROR        UART0_C3_ORIE_MASK /**< Overrun error interrupt enable mask */

#if ((defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)))
/* Clock sources */
  #define UART0_PDD_DISABLE_CLOCK      0U          /**< Disable the clock. */
  #define UART0_PDD_FLL_CLOCK          0x1U        /**< MCG FLL clock. */
  #define UART0_PDD_EXTERNAL_REF_CLOCK 0x2U        /**< External reference clock. */
  #define UART0_PDD_INTERNAL_REF_CLOCK 0x3U        /**< Internal reference clock. */

#else /* (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) */
/* Clock sources */
  #define UART0_PDD_DISABLE_CLOCK      0U          /**< Disable the clock. */
  #define UART0_PDD_PLL_FLL_CLOCK      0x1U        /**< MCG PLL or FLL clock. */
  #define UART0_PDD_EXTERNAL_REF_CLOCK 0x2U        /**< External reference clock. */
  #define UART0_PDD_INTERNAL_REF_CLOCK 0x3U        /**< Internal reference clock. */

#endif /* (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) */
/* Parity types */
#define UART0_PDD_PARITY_NONE 0U                 /**< No parity */
#define UART0_PDD_PARITY_EVEN 0x2U               /**< Even parity */
#define UART0_PDD_PARITY_ODD  0x3U               /**< Even parity */

/* Data width */
#define UART0_PDD_WIDTH_8  0U                    /**< 8-bit communication */
#define UART0_PDD_WIDTH_9  0x1U                  /**< 9-bit communication */
#define UART0_PDD_WIDTH_10 0x2U                  /**< 10-bit communication */

/* Loop mode */
#define UART0_PDD_LOOP_MODE_NORMAL     0U        /**< Normal operation mode. No loopback selected. */
#define UART0_PDD_LOOP_MODE_LOCAL_LOOP 0x80U     /**< Local loopback mode. */

/* Stop bit lengths */
#define UART0_PDD_STOP_BIT_LEN_1 0U              /**< One stop bit. */
#define UART0_PDD_STOP_BIT_LEN_2 0x20U           /**< Two stop bits. */

/* Receiver power states. */
#define UART0_PDD_POWER_NORMAL  0U               /**< Normal operation. */
#define UART0_PDD_POWER_STANDBY 0x2U             /**< Standby mode (waiting for a wakeup condition). */


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
 * @remarks The macro accesses the following registers: UART0_C2.
 * @par Example:
 *      @code
 *      UART0_PDD_EnableTransmitter(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define UART0_PDD_EnableTransmitter(PeripheralBase, State) ( \
    UART0_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_C2_TE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << UART0_C2_TE_SHIFT))) \
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
 * @remarks The macro accesses the following registers: UART0_C2.
 * @par Example:
 *      @code
 *      UART0_PDD_EnableReceiver(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define UART0_PDD_EnableReceiver(PeripheralBase, State) ( \
    UART0_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_C2_RE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << UART0_C2_RE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadDataReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Reads data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_D.
 * @par Example:
 *      @code
 *      uint8_t result = UART0_PDD_ReadDataReg(deviceID);
 *      @endcode
 */
#define UART0_PDD_ReadDataReg(PeripheralBase) ( \
    UART0_D_REG(PeripheralBase) \
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
 * @remarks The macro accesses the following registers: UART0_D.
 * @par Example:
 *      @code
 *      UART0_PDD_WriteDataReg(deviceID, 1);
 *      @endcode
 */
#define UART0_PDD_WriteDataReg(PeripheralBase, Value) ( \
    UART0_D_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: UART0_D.
 * @par Example:
 *      @code
 *      UART0_PDD_PutChar8(deviceID, 1);
 *      @endcode
 */
#define UART0_PDD_PutChar8(PeripheralBase, Char) ( \
    UART0_D_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: UART0_C3, UART0_D
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART0_PDD_PutChar9(deviceID, 1);
 *      @endcode
 */
#define UART0_PDD_PutChar9(PeripheralBase, Char) ( \
    (UART0_C3_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C3_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_C3_R9T8_MASK))) | ( \
      (uint8_t)((uint8_t)((uint16_t)(Char) >> 8U) << UART0_C3_R9T8_SHIFT)))), \
    (UART0_D_REG(PeripheralBase) = \
     (uint8_t)(Char)) \
  )

/* ----------------------------------------------------------------------------
   -- PutChar10
   ---------------------------------------------------------------------------- */

/**
 * @brief Puts 10-bits character into the transmit buffer.
 * @param PeripheralBase Peripheral base address.
 * @param Char 10-bits character to be written to the data register. This
 *        parameter is a 10-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C3, UART0_D
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART0_PDD_PutChar10(deviceID, 1);
 *      @endcode
 */
#define UART0_PDD_PutChar10(PeripheralBase, Char) ( \
    (UART0_C3_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C3_REG(PeripheralBase) & (uint8_t)(~(uint8_t)((uint8_t)0x3U << 6U)))) | ( \
      (uint8_t)((uint8_t)((uint16_t)(Char) >> 8U) << 6U)))), \
    (UART0_D_REG(PeripheralBase) = \
     (uint8_t)(Char)) \
  )

/* ----------------------------------------------------------------------------
   -- GetChar8
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns a 8-bit character from the receive buffer.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_D.
 * @par Example:
 *      @code
 *      uint8_t result = UART0_PDD_GetChar8(deviceID);
 *      @endcode
 */
#define UART0_PDD_GetChar8(PeripheralBase) ( \
    UART0_D_REG(PeripheralBase) \
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
 * @remarks The macro accesses the following registers: UART0_C3.
 * @par Example:
 *      @code
 *      uint16_t result = UART0_PDD_GetChar9Bit(deviceID);
 *      @endcode
 */
#define UART0_PDD_GetChar9Bit(PeripheralBase) ( \
    (uint16_t)(( \
     (uint16_t)(( \
      (uint16_t)(UART0_C3_REG(PeripheralBase) & UART0_C3_R8T9_MASK)) >> ( \
      UART0_C3_R8T9_SHIFT))) << ( \
     8U)) \
  )

/* ----------------------------------------------------------------------------
   -- GetChar9To10thBit
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the 9th and 10th bit of the character from the receive buffer
 * shifted to their bit positions (9th and 10th). Must be called prior to calling
 * GetChar8 to read the whole 10-bit character.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 10-bit value. The value is cast to "uint16_t".
 * @remarks The macro accesses the following registers: UART0_C3.
 * @par Example:
 *      @code
 *      uint16_t result = UART0_PDD_GetChar9To10thBit(deviceID);
 *      @endcode
 */
#define UART0_PDD_GetChar9To10thBit(PeripheralBase) ( \
    (uint16_t)(( \
     (uint16_t)(( \
      (uint16_t)(( \
       (uint16_t)(UART0_C3_REG(PeripheralBase) & UART0_C3_R8T9_MASK)) >> ( \
       UART0_C3_R8T9_SHIFT))) | ( \
      (uint16_t)(( \
       (uint16_t)(( \
        (uint16_t)(UART0_C3_REG(PeripheralBase) & UART0_C3_R9T8_MASK)) >> ( \
        UART0_C3_R9T8_SHIFT))) << ( \
       1U))))) << ( \
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
 * @remarks The macro accesses the following registers: UART0_C2, UART0_C3
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART0_PDD_EnableInterrupt(deviceID, UART0_PDD_INTERRUPT_TRANSMITTER);
 *      @endcode
 */
#define UART0_PDD_EnableInterrupt(PeripheralBase, Mask) ( \
    (UART0_C2_REG(PeripheralBase) |= \
     (uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU))), \
    (UART0_C3_REG(PeripheralBase) |= \
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
 * @remarks The macro accesses the following registers: UART0_C2, UART0_C3
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART0_PDD_DisableInterrupt(deviceID, UART0_PDD_INTERRUPT_TRANSMITTER);
 *      @endcode
 */
#define UART0_PDD_DisableInterrupt(PeripheralBase, Mask) ( \
    (UART0_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU)))), \
    (UART0_C3_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & 0xFU))) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxCompleteInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets the status of the transmiter complete interrupt enable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_C2.
 * @par Example:
 *      @code
 *      uint8_t result = UART0_PDD_GetTxCompleteInterruptMask(deviceID);
 *      @endcode
 */
#define UART0_PDD_GetTxCompleteInterruptMask(PeripheralBase) ( \
    (uint8_t)(UART0_C2_REG(PeripheralBase) & UART0_C2_TCIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxCompleteStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets the status of the transmiter complete interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_S1.
 * @par Example:
 *      @code
 *      uint8_t result = UART0_PDD_GetTxCompleteStatus(deviceID);
 *      @endcode
 */
#define UART0_PDD_GetTxCompleteStatus(PeripheralBase) ( \
    (uint8_t)(UART0_S1_REG(PeripheralBase) & UART0_S1_TC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetParity
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets a communication parity type.
 * @param PeripheralBase Peripheral base address.
 * @param Parity Parity type. This parameter is of "Parity types" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C1.
 * @par Example:
 *      @code
 *      UART0_PDD_SetParity(deviceID, UART0_PDD_PARITY_NONE);
 *      @endcode
 */
#define UART0_PDD_SetParity(PeripheralBase, Parity) ( \
    UART0_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)0x3U))) | ( \
      (uint8_t)(Parity))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDataWidth
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the communication width.
 * @param PeripheralBase Peripheral base address.
 * @param Width Data width. This parameter is of "Data width" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C1, UART0_C4
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART0_PDD_SetDataWidth(deviceID, UART0_PDD_WIDTH_8);
 *      @endcode
 */
#define UART0_PDD_SetDataWidth(PeripheralBase, Width) ( \
    ( \
     ((Width) == UART0_PDD_WIDTH_8) ? ( \
      UART0_C1_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)UART0_C1_M_MASK)) : (((Width) == UART0_PDD_WIDTH_9) ? ( \
      UART0_C1_REG(PeripheralBase) |= \
       UART0_C1_M_MASK) : ( \
      UART0_C1_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)UART0_C1_M_MASK)) \
    )), \
    ( \
     ((Width) == UART0_PDD_WIDTH_8) ? ( \
      UART0_C4_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)UART0_C4_M10_MASK)) : (((Width) == UART0_PDD_WIDTH_9) ? ( \
      UART0_C4_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)UART0_C4_M10_MASK)) : ( \
      UART0_C4_REG(PeripheralBase) |= \
       UART0_C4_M10_MASK) \
    )) \
  )

/* ----------------------------------------------------------------------------
   -- SetLoopMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects the loop mode operation.
 * @param PeripheralBase Peripheral base address.
 * @param LoopMode Loop mode. This parameter is of "Loop mode" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C1.
 * @par Example:
 *      @code
 *      UART0_PDD_SetLoopMode(deviceID, UART0_PDD_LOOP_MODE_NORMAL);
 *      @endcode
 */
#define UART0_PDD_SetLoopMode(PeripheralBase, LoopMode) ( \
    UART0_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_C1_LOOPS_MASK))) | ( \
      (uint8_t)(LoopMode))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInterruptStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the interrupt status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_S1.
 * @par Example:
 *      @code
 *      uint8_t result = UART0_PDD_ReadInterruptStatusReg(deviceID);
 *      @endcode
 */
#define UART0_PDD_ReadInterruptStatusReg(PeripheralBase) ( \
    UART0_S1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the interrupt status flags specified by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of the flags to be cleared. This parameter is a 8-bit value.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_S1.
 * @par Example:
 *      @code
 *      uint8_t result = UART0_PDD_ClearInterruptFlags(deviceID, 1);
 *      @endcode
 */
#define UART0_PDD_ClearInterruptFlags(PeripheralBase, Mask) ( \
    UART0_S1_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- SetBaudRate
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets new baud rate value.
 * @param PeripheralBase Peripheral base address.
 * @param BaudRate New baud rate value. This parameter is a 13-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_BDH, UART0_BDL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART0_PDD_SetBaudRate(deviceID, 1);
 *      @endcode
 */
#define UART0_PDD_SetBaudRate(PeripheralBase, BaudRate) ( \
    (UART0_BDH_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_BDH_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_BDH_SBR_MASK))) | ( \
      (uint8_t)((uint16_t)(BaudRate) >> 8U)))), \
    (UART0_BDL_REG(PeripheralBase) = \
     (uint8_t)(BaudRate)) \
  )

/* ----------------------------------------------------------------------------
   -- SetOversamplingRatio
   ---------------------------------------------------------------------------- */

/**
 * @brief Set baud rate oversampling ratio.
 * @param PeripheralBase Peripheral base address.
 * @param Ratio Baud rate oversampling ratio. This parameter is a 5-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C4.
 * @par Example:
 *      @code
 *      UART0_PDD_SetOversamplingRatio(deviceID, 1);
 *      @endcode
 */
#define UART0_PDD_SetOversamplingRatio(PeripheralBase, Ratio) ( \
    UART0_C4_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C4_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_C4_OSR_MASK))) | ( \
      (uint8_t)(Ratio))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableSamplingOnBothEdges
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables receiver sampling on both edges.
 * @param PeripheralBase Peripheral base address.
 * @param State Enable or disable sampling on both edges. This parameter is of
 *        "Global enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C5.
 * @par Example:
 *      @code
 *      UART0_PDD_EnableSamplingOnBothEdges(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define UART0_PDD_EnableSamplingOnBothEdges(PeripheralBase, State) ( \
    UART0_C5_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C5_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_C5_BOTHEDGE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << UART0_C5_BOTHEDGE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetStopBitLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the number of stop bits.
 * @param PeripheralBase Peripheral base address.
 * @param Length Stop bit length. This parameter is of "Stop bit lengths" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_BDH.
 * @par Example:
 *      @code
 *      UART0_PDD_SetStopBitLength(deviceID, UART0_PDD_STOP_BIT_LEN_1);
 *      @endcode
 */
#define UART0_PDD_SetStopBitLength(PeripheralBase, Length) ( \
    UART0_BDH_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_BDH_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_BDH_SBNS_MASK))) | ( \
      (uint8_t)(Length))) \
  )

/* ----------------------------------------------------------------------------
   -- SetBreak
   ---------------------------------------------------------------------------- */

/**
 * @brief Set the break signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2.
 * @par Example:
 *      @code
 *      UART0_PDD_SetBreak(deviceID);
 *      @endcode
 */
#define UART0_PDD_SetBreak(PeripheralBase) ( \
    UART0_C2_REG(PeripheralBase) |= \
     UART0_C2_SBK_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearBreak
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the break signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2.
 * @par Example:
 *      @code
 *      UART0_PDD_ClearBreak(deviceID);
 *      @endcode
 */
#define UART0_PDD_ClearBreak(PeripheralBase) ( \
    UART0_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)UART0_C2_SBK_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetBreak
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: UART0_S1.
 * @par Example:
 *      @code
 *      uint8_t result = UART0_PDD_GetBreak(deviceID);
 *      @endcode
 */
#define UART0_PDD_GetBreak(PeripheralBase) ( \
    (( \
      (uint8_t)(( \
       UART0_S1_REG(PeripheralBase)) & ( \
       (uint8_t)(UART0_S1_FE_MASK | UART0_S1_RDRF_MASK)))) == ( \
      (uint8_t)(UART0_S1_FE_MASK | UART0_S1_RDRF_MASK))) ? ( \
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
 * @remarks The macro accesses the following registers: UART0_C2.
 * @par Example:
 *      @code
 *      UART0_PDD_SendBreak(deviceID);
 *      @endcode
 */
#define UART0_PDD_SendBreak(PeripheralBase) ( \
    (UART0_C2_REG(PeripheralBase) |= \
     UART0_C2_SBK_MASK), \
    (UART0_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)UART0_C2_SBK_MASK)) \
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
 * @remarks The macro accesses the following registers: UART0_C2.
 * @par Example:
 *      @code
 *      UART0_PDD_SetReceiverPowerState(deviceID, UART0_PDD_POWER_NORMAL);
 *      @endcode
 */
#define UART0_PDD_SetReceiverPowerState(PeripheralBase, State) ( \
    UART0_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UART0_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UART0_C2_RWU_MASK))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- SetClockSource
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)))
/**
 * @brief Selects the clock source (in the SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param Source Clock source. Possible values: DISABLE_CLOCK, FLL_CLOCK,
 *        EXTERNAL_REF_CLOCK, INTERNAL_REF_CLOCK. Use constants from group "Clock
 *        sources". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      UART0_PDD_SetClockSource(deviceID, UART0_PDD_DISABLE_CLOCK);
 *      @endcode
 */
  #define UART0_PDD_SetClockSource(PeripheralBase, Source) ( \
      SIM_SOPT2_REG(SIM_BASE_PTR) = \
       (( \
        (uint32_t)(SIM_SOPT2_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_SOPT2_UART0SRC_MASK))) | ( \
        (uint32_t)((uint32_t)(Source) << SIM_SOPT2_UART0SRC_SHIFT))) \
    )
#else /* (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) */
/**
 * @brief Selects the clock source (in the SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param Source Clock source. Possible values: DISABLE_CLOCK, PLL_FLL_CLOCK,
 *        EXTERNAL_REF_CLOCK, INTERNAL_REF_CLOCK. Use constants from group "Clock
 *        sources". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      UART0_PDD_SetClockSource(deviceID, UART0_PDD_DISABLE_CLOCK);
 *      @endcode
 */
  #define UART0_PDD_SetClockSource(PeripheralBase, Source) ( \
      SIM_SOPT2_REG(SIM_BASE_PTR) = \
       (( \
        (uint32_t)(SIM_SOPT2_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_SOPT2_UART0SRC_MASK))) | ( \
        (uint32_t)((uint32_t)(Source) << SIM_SOPT2_UART0SRC_SHIFT))) \
    )
#endif /* (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) */
#endif  /* #if defined(UART0_PDD_H_) */

/* UART0_PDD.h, eof. */
