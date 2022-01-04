/*
  PDD layer implementation for peripheral type UARTLP
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(UARTLP_PDD_H_)
#define UARTLP_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error UARTLP PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MKL04Z4) /* UART0 */ && \
      !defined(MCU_MKL05Z4) /* UART0 */ && \
      !defined(MCU_MKL14Z4) /* UART0 */ && \
      !defined(MCU_MKL15Z4) /* UART0 */ && \
      !defined(MCU_MKL24Z4) /* UART0 */ && \
      !defined(MCU_MKL25Z4) /* UART0 */
  // Unsupported MCU is active
  #error UARTLP PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Interrupt masks */
#define UARTLP_PDD_INTERRUPT_TRANSMITTER UARTLP_C2_TIE_MASK /**< Transmitter interrupt enable mask */
#define UARTLP_PDD_INTERRUPT_TRANSMITTER_COMPLETE UARTLP_C2_TCIE_MASK /**< Transmitter complete interrupt enable mask */
#define UARTLP_PDD_INTERRUPT_RECEIVER UARTLP_C2_RIE_MASK /**< Receiver interrupt enable mask */
#define UARTLP_PDD_INTERRUPT_IDLE UARTLP_C2_ILIE_MASK /**< Idle interrupt enable mask */
#define UARTLP_PDD_INTERRUPT_PARITY_ERROR UARTLP_C3_PEIE_MASK /**< Parity error interrupt enable mask */
#define UARTLP_PDD_INTERRUPT_FRAMING_ERROR UARTLP_C3_FEIE_MASK /**< Framing error interrupt enable mask */
#define UARTLP_PDD_INTERRUPT_NOISE_ERROR UARTLP_C3_NEIE_MASK /**< Noise error interrupt enable mask */
#define UARTLP_PDD_INTERRUPT_OVERRUN_ERROR UARTLP_C3_ORIE_MASK /**< Overrun error interrupt enable mask */

/* Parity types */
#define UARTLP_PDD_PARITY_NONE 0U                /**< No parity */
#define UARTLP_PDD_PARITY_EVEN 0x2U              /**< Even parity */
#define UARTLP_PDD_PARITY_ODD 0x3U               /**< Even parity */

/* Data width */
#define UARTLP_PDD_WIDTH_8 0U                    /**< 8-bits communication */
#define UARTLP_PDD_WIDTH_9 0x10U                 /**< 9-bits communication */

/* Loop mode */
#define UARTLP_PDD_LOOP_MODE_NORMAL 0U           /**< Normal operation mode. No loopback selected. */
#define UARTLP_PDD_LOOP_MODE_LOCAL_LOOP 0x80U    /**< Local loopback mode. */

/* Position of a parity bit */
#define UARTLP_PDD_PARITY_BIT_POSITION_9 0U      /**< Parity bit is the 9-th bit in the serial transmission */
#define UARTLP_PDD_PARITY_BIT_POSITION_10 0x20U  /**< Parity bit is the 10-th bit in the serial transmission */


/* ----------------------------------------------------------------------------
   -- EnableTransmitter
   ---------------------------------------------------------------------------- */

/**
 * Enables UART transmitter.
 * @param PeripheralBase Peripheral base address.
 * @param State Enables or disables transmitter.
 */
#define UARTLP_PDD_EnableTransmitter(PeripheralBase, State) ( \
    UARTLP_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UARTLP_C2_TE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << UARTLP_C2_TE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableReceiver
   ---------------------------------------------------------------------------- */

/**
 * Enables UART receiver.
 * @param PeripheralBase Peripheral base address.
 * @param State Enables or disables receiver.
 */
#define UARTLP_PDD_EnableReceiver(PeripheralBase, State) ( \
    UARTLP_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UARTLP_C2_RE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << UARTLP_C2_RE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadDataReg
   ---------------------------------------------------------------------------- */

/**
 * Reads data register.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_ReadDataReg(PeripheralBase) ( \
    UARTLP_D_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteDataReg
   ---------------------------------------------------------------------------- */

/**
 * Writes data specified by the Value parameter into data register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value to be written to the data register.
 */
#define UARTLP_PDD_WriteDataReg(PeripheralBase, Value) ( \
    UARTLP_D_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- PutChar8
   ---------------------------------------------------------------------------- */

/**
 * Puts 8-bits character into the transmit buffer.
 * @param PeripheralBase Peripheral base address.
 * @param Char 8-bits character to be written to the data register.
 */
#define UARTLP_PDD_PutChar8(PeripheralBase, Char) ( \
    UARTLP_D_REG(PeripheralBase) = \
     (uint8_t)(Char) \
  )

/* ----------------------------------------------------------------------------
   -- PutChar9
   ---------------------------------------------------------------------------- */

/**
 * Puts 9-bits character into the transmit buffer.
 * @param PeripheralBase Peripheral base address.
 * @param Char 9-bits character to be written to the data register.
 */
#define UARTLP_PDD_PutChar9(PeripheralBase, Char) ( \
    (UARTLP_C3_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_C3_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UARTLP_C3_R9T8_MASK))) | ( \
      (uint8_t)((uint8_t)((uint16_t)(Char) >> 8U) << UARTLP_C3_R9T8_SHIFT)))), \
    (UARTLP_D_REG(PeripheralBase) = \
     (uint8_t)(Char)) \
  )

/* ----------------------------------------------------------------------------
   -- GetChar8
   ---------------------------------------------------------------------------- */

/**
 * Returns 8-bits character from the receive buffer.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_GetChar8(PeripheralBase) ( \
    UARTLP_D_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetChar9Bit
   ---------------------------------------------------------------------------- */

/**
 * Returns a mask of the ninth bit of the received character.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_GetChar9Bit(PeripheralBase) ( \
    (uint8_t)(UARTLP_C3_REG(PeripheralBase) & UARTLP_C3_R8T9_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterrupt
   ---------------------------------------------------------------------------- */

/**
 * Enables interrupts specified by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask.
 */
#define UARTLP_PDD_EnableInterrupt(PeripheralBase, Mask) ( \
    (UARTLP_C2_REG(PeripheralBase) |= \
     (uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU))), \
    (UARTLP_C3_REG(PeripheralBase) |= \
     (uint8_t)((uint8_t)(Mask) & 0xFU)) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterrupt
   ---------------------------------------------------------------------------- */

/**
 * Disables interrupts specified by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask.
 */
#define UARTLP_PDD_DisableInterrupt(PeripheralBase, Mask) ( \
    (UARTLP_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU)))), \
    (UARTLP_C3_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & 0xFU))) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxCompleteInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * Gets the status of the transmiter complete interrupt enable bit.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_GetTxCompleteInterruptMask(PeripheralBase) ( \
    (uint8_t)(UARTLP_C2_REG(PeripheralBase) & UARTLP_C2_TCIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxCompleteStatus
   ---------------------------------------------------------------------------- */

/**
 * Gets the status of the transmiter complete interrupt flag.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_GetTxCompleteStatus(PeripheralBase) ( \
    (uint8_t)(UARTLP_S1_REG(PeripheralBase) & UARTLP_S1_TC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetParity
   ---------------------------------------------------------------------------- */

/**
 * Sets a communication parity type.
 * @param PeripheralBase Peripheral base address.
 * @param Parity Parity type.
 */
#define UARTLP_PDD_SetParity(PeripheralBase, Parity) ( \
    UARTLP_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)0x3U))) | ( \
      (uint8_t)(Parity))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDataWidth
   ---------------------------------------------------------------------------- */

/**
 * Sets the communication width.
 * @param PeripheralBase Peripheral base address.
 * @param Width Data width.
 */
#define UARTLP_PDD_SetDataWidth(PeripheralBase, Width) ( \
    UARTLP_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UARTLP_C1_M_MASK))) | ( \
      (uint8_t)(Width))) \
  )

/* ----------------------------------------------------------------------------
   -- SetLoopMode
   ---------------------------------------------------------------------------- */

/**
 * Selects the loop mode operation.
 * @param PeripheralBase Peripheral base address.
 * @param LoopMode Loop mode.
 */
#define UARTLP_PDD_SetLoopMode(PeripheralBase, LoopMode) ( \
    UARTLP_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UARTLP_C1_LOOPS_MASK))) | ( \
      (uint8_t)(LoopMode))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInterruptStatusReg
   ---------------------------------------------------------------------------- */

/**
 * Returns the interrupt status register.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_ReadInterruptStatusReg(PeripheralBase) ( \
    UARTLP_S1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetBaudRate
   ---------------------------------------------------------------------------- */

/**
 * Sets new baud rate value.
 * @param PeripheralBase Peripheral base address.
 * @param BaudRate New baud rate value.
 */
#define UARTLP_PDD_SetBaudRate(PeripheralBase, BaudRate) ( \
    (UARTLP_BDH_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_BDH_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UARTLP_BDH_SBR_MASK))) | ( \
      (uint8_t)((uint16_t)(BaudRate) >> 8U)))), \
    (UARTLP_BDL_REG(PeripheralBase) = \
     (uint8_t)(BaudRate)) \
  )

/* ----------------------------------------------------------------------------
   -- SetBreak
   ---------------------------------------------------------------------------- */

/**
 * Set the break signal.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_SetBreak(PeripheralBase) ( \
    UARTLP_C2_REG(PeripheralBase) |= \
     UARTLP_C2_SBK_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearBreak
   ---------------------------------------------------------------------------- */

/**
 * Clears the break signal.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_ClearBreak(PeripheralBase) ( \
    UARTLP_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)UARTLP_C2_SBK_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetBreak
   ---------------------------------------------------------------------------- */

/**
 * Returns .
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_GetBreak(PeripheralBase) ( \
    (( \
      (uint8_t)(( \
       UARTLP_S1_REG(PeripheralBase)) & ( \
       (uint8_t)(UARTLP_S1_FE_MASK | UARTLP_S1_RDRF_MASK)))) == ( \
      (uint8_t)(UARTLP_S1_FE_MASK | UARTLP_S1_RDRF_MASK))) ? ( \
      0x1U) : ( \
      0U) \
  )

/* ----------------------------------------------------------------------------
   -- SendBreak
   ---------------------------------------------------------------------------- */

/**
 * Send the break character.
 * @param PeripheralBase Peripheral base address.
 */
#define UARTLP_PDD_SendBreak(PeripheralBase) ( \
    (UARTLP_C2_REG(PeripheralBase) |= \
     UARTLP_C2_SBK_MASK), \
    (UARTLP_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)UARTLP_C2_SBK_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- SetPositionOfParityBit
   ---------------------------------------------------------------------------- */

/**
 * Sets the position of the parity bit.
 * @param PeripheralBase Peripheral base address.
 * @param Position Position of a parity bit.
 */
#define UARTLP_PDD_SetPositionOfParityBit(PeripheralBase, Position) ( \
    UARTLP_C4_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(UARTLP_C4_REG(PeripheralBase) & (uint8_t)(~(uint8_t)UARTLP_C4_M10_MASK))) | ( \
      (uint8_t)(Position))) \
  )
#endif  /* #if defined(UARTLP_PDD_H_) */

/* UARTLP_PDD.h, eof. */
