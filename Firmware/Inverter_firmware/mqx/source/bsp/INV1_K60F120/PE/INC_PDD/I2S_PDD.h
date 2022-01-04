/*
  PDD layer implementation for peripheral type I2S
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(I2S_PDD_H_)
#define I2S_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error I2S PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10DZ10) /* I2S0 */ && \
      !defined(MCU_MK20DZ10) /* I2S0 */ && \
      !defined(MCU_MK30DZ10) /* I2S0 */ && \
      !defined(MCU_MK40DZ10) /* I2S0 */ && \
      !defined(MCU_MK40X256VMD100) /* I2S0 */ && \
      !defined(MCU_MK50DZ10) /* I2S0 */ && \
      !defined(MCU_MK51DZ10) /* I2S0 */ && \
      !defined(MCU_MK52DZ10) /* I2S0 */ && \
      !defined(MCU_MK53DZ10) /* I2S0 */ && \
      !defined(MCU_MK60DZ10) /* I2S0 */ && \
      !defined(MCU_MK60N512VMD100) /* I2S0 */
  // Unsupported MCU is active
  #error I2S PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Receive and transmit section masks. */
#define I2S_PDD_RECEIVER    I2S_CR_RE_MASK       /**< Receive section mask. */
#define I2S_PDD_TRANSMITTER I2S_CR_TE_MASK       /**< Transmit section mask. */

/* Frame clock disable masks. */
#define I2S_PDD_RX_FRAME_CLOCK_DISABLE I2S_CR_RFRCLKDIS_MASK /**< Receive frame clock mask. */
#define I2S_PDD_TX_FRAME_CLOCK_DISABLE I2S_CR_TFRCLKDIS_MASK /**< Transmit frame clock mask. */

/* Interrupt masks for EnableDmasInterrupts, DisableDmasInterrupts, some of them
   for GetInterruptFlag, some of them for ClearInterruptFlags */
#define I2S_PDD_RX_FRAME_COMPLETE_INT      I2S_ISR_RFRC_MASK /**< Receive frame complete */
#define I2S_PDD_TX_FRAME_COMPLETE_INT      I2S_ISR_TRFC_MASK /**< Transmit frame complete */
#define I2S_PDD_RX_DMA_ENABLE              I2S_IER_RDMAE_MASK /**< Receive DMA enable, unusable for GetInterruptFlags nor ClearInterruptFlags */
#define I2S_PDD_RECEIVER_INT               I2S_IER_RIE_MASK /**< Receive interrupt enable, unusable for GetInterruptFlags nor ClearInterruptFlags */
#define I2S_PDD_TX_DMA_ENABLE              I2S_IER_TDMAE_MASK /**< Transmit DMA enable, unusable for GetInterruptFlags nor ClearInterruptFlags */
#define I2S_PDD_TRANSMITTER_INT            I2S_IER_TIE_MASK /**< Transmit interrupt enable, unusable for GetInterruptFlags nor ClearInterruptFlags */
#define I2S_PDD_COMMAND_ADDRESS_UPDATE_INT I2S_ISR_CMDAU_MASK /**< Command address update, unusable for ClearInterruptFlags */
#define I2S_PDD_COMMAND_DATA_UPDATE_INT    I2S_ISR_CMDDU_MASK /**< Command data update, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_TAG_UPDATE_INT          I2S_ISR_RXT_MASK /**< Receive tag update, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_DATA_READY_1_INT        I2S_ISR_RDR1_MASK /**< Receive data ready 1, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_DATA_READY_0_INT        I2S_ISR_RDR0_MASK /**< Receive data ready 0, unusable for ClearInterruptFlags */
#define I2S_PDD_TX_DATA_EMPTY_1_INT        I2S_ISR_TDE1_MASK /**< Transmit data empty 1, unusable for ClearInterruptFlags */
#define I2S_PDD_TX_DATA_EMPTY_0_INT        I2S_ISR_TDE0_MASK /**< Transmit data empty 0, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_OVERRUN_1_INT           I2S_ISR_ROE1_MASK /**< Receive overrun 1 */
#define I2S_PDD_RX_OVERRUN_0_INT           I2S_ISR_ROE0_MASK /**< Receive overrun 0 */
#define I2S_PDD_TX_UNDERRUN_1_INT          I2S_ISR_TUE1_MASK /**< Transmit underrun 1 */
#define I2S_PDD_TX_UNDERRUN_0_INT          I2S_ISR_TUE0_MASK /**< Transmit underrun 0 */
#define I2S_PDD_TX_FRAME_SYNC_INT          I2S_ISR_TFS_MASK /**< Transmit frame sync, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_FRAME_SYNC_INT          I2S_ISR_RFS_MASK /**< Recieve frame sync, unusable for ClearInterruptFlags */
#define I2S_PDD_TX_LAST_TIME_SLOT_INT      I2S_ISR_TLS_MASK /**< Transmit last time slot, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_LAST_TIME_SLOT_INT      I2S_ISR_RLS_MASK /**< Receive last time slot, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_FIFO_FULL_1_INT         I2S_ISR_RFF1_MASK /**< Receive FIFO full 1, unusable for ClearInterruptFlags */
#define I2S_PDD_RX_FIFO_FULL_0_INT         I2S_ISR_RFF0_MASK /**< Receive FIFO full 0, unusable for ClearInterruptFlags */
#define I2S_PDD_TX_FIFO_EMPTY_1_INT        I2S_ISR_TFE1_MASK /**< Transmit FIFO empty 1, unusable for ClearInterruptFlags */
#define I2S_PDD_TX_FIFO_EMPTY_0_INT        I2S_ISR_TFE0_MASK /**< Transmit FIFO empty 0, unusable for ClearInterruptFlags */
#define I2S_PDD_ALL_INT                    0x1AFFFFFU /**< All interrupt flags, unusable for ClearInterruptFlags */

/* Slot masks for EnableAC97Channels, DisableAC97Channels,
   GetAC97ChannelStatusMask */
#define I2S_PDD_AC97_DATA_SLOT_3  0x1U           /**< The 3rd data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_4  0x2U           /**< The 4th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_5  0x4U           /**< The 5th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_6  0x8U           /**< The 6th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_7  0x10U          /**< The 7th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_8  0x20U          /**< The 8th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_9  0x40U          /**< The 9th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_10 0x80U          /**< The 10th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_11 0x100U         /**< The 11th data channel mask. */
#define I2S_PDD_AC97_DATA_SLOT_12 0x200U         /**< The 12th data channel mask. */

/* Clock source constants for SetClockSource. */
#define I2S_PDD_DIVIDED_CORE_SYSTEM_CLOCK 0U     /**< Divided core system clock source. */
#define I2S_PDD_DIVIDED_PLL_FLL_CLOCK     0x1U   /**< Divided PLL or FLL clock source. */
#define I2S_PDD_EXTAL_CLOCK               0x2U   /**< Extal clock source. */
#define I2S_PDD_EXTERNAL_CLOCK            0x3U   /**< External clock source. */

/* AC97 command constant. */
#define I2S_PDD_AC97_WRITE_COMMAND 0x10U         /**< AC97 write command. */
#define I2S_PDD_AC97_READ_COMMAND  0x8U          /**< AC97 read command. */


/* ----------------------------------------------------------------------------
   -- WriteTxData0Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Tx Data 0 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the Data0 register. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_TX0.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteTxData0Reg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteTxData0Reg(PeripheralBase, Value) ( \
    I2S_TX0_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTxData1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Tx Data 1 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the Data1 register. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_TX1.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteTxData1Reg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteTxData1Reg(PeripheralBase, Value) ( \
    I2S_TX1_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadRxData0Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Rx Data 0 register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_RX0.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadRxData0Reg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadRxData0Reg(PeripheralBase) ( \
    I2S_RX0_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadRxData1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Rx Data 1 register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_RX1.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadRxData1Reg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadRxData1Reg(PeripheralBase) ( \
    I2S_RX1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableReceiver
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the receive section.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of receive section. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_CR.
 * @par Example:
 *      @code
 *      I2S_PDD_EnableReceiver(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define I2S_PDD_EnableReceiver(PeripheralBase, State) ( \
    I2S_CR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_CR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_CR_RE_MASK))) | ( \
      (uint32_t)((uint32_t)(State) << I2S_CR_RE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTransmitter
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the transmit section.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of transmit section. This parameter is of
 *        "Global enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_CR.
 * @par Example:
 *      @code
 *      I2S_PDD_EnableTransmitter(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define I2S_PDD_EnableTransmitter(PeripheralBase, State) ( \
    I2S_CR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_CR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_CR_TE_MASK))) | ( \
      (uint32_t)((uint32_t)(State) << I2S_CR_TE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableRxTx
   ---------------------------------------------------------------------------- */

/**
 * @brief Allows to enable receive and transmit section.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of receive and transmit section to enable. Use constants
 *        from group "Receive and transmit section masks.". This parameter is 32
 *        bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_CR.
 * @par Example:
 *      @code
 *      I2S_PDD_EnableRxTx(deviceID, I2S_PDD_RECEIVER);
 *      @endcode
 */
#define I2S_PDD_EnableRxTx(PeripheralBase, Mask) ( \
    I2S_CR_REG(PeripheralBase) |= \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableRxTx
   ---------------------------------------------------------------------------- */

/**
 * @brief Allows to disable receive and transmit section.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of receive and transmit section to enable. Use constants
 *        from group "Receive and transmit section masks.". This parameter is 32
 *        bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_CR.
 * @par Example:
 *      @code
 *      I2S_PDD_DisableRxTx(deviceID, I2S_PDD_RECEIVER);
 *      @endcode
 */
#define I2S_PDD_DisableRxTx(PeripheralBase, Mask) ( \
    I2S_CR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the device.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of device. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE and
 *        PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_CR.
 * @par Example:
 *      @code
 *      I2S_PDD_EnableDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define I2S_PDD_EnableDevice(PeripheralBase, State) ( \
    I2S_CR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_CR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_CR_I2SEN_MASK))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the control register. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_CR.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteControlReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteControlReg(PeripheralBase, Value) ( \
    I2S_CR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Control register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Frame clock disable masks." for processing
 *         return value.
 * @remarks The macro accesses the following registers: I2S0_CR.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadControlReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadControlReg(PeripheralBase) ( \
    I2S_CR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt flag bits.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Interrupt masks for EnableDmasInterrupts,
 *         DisableDmasInterrupts, some of them for GetInterruptFlag, some of them
 *         for ClearInterruptFlags" for processing return value.
 * @remarks The macro accesses the following registers: I2S0_ISR.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_GetInterruptFlags(deviceID);
 *      @endcode
 */
#define I2S_PDD_GetInterruptFlags(PeripheralBase) ( \
    I2S_ISR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears interrupt flag bits defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupt flags to clear. Use constants from group
 *        "Interrupt masks for EnableDmasInterrupts, DisableDmasInterrupts, some of
 *        them for GetInterruptFlag, some of them for ClearInterruptFlags". This
 *        parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ISR.
 * @par Example:
 *      @code
 *      I2S_PDD_ClearInterruptFlags(deviceID, I2S_PDD_RX_FRAME_COMPLETE_INT);
 *      @endcode
 */
#define I2S_PDD_ClearInterruptFlags(PeripheralBase, Mask) ( \
    I2S_ISR_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDmasInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables DMAs and interrupts defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of DMAs and interrupts. Use constants from group "Interrupt
 *        masks for EnableDmasInterrupts, DisableDmasInterrupts, some of them for
 *        GetInterruptFlag, some of them for ClearInterruptFlags". This
 *        parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_IER.
 * @par Example:
 *      @code
 *      I2S_PDD_EnableDmasInterrupts(deviceID, I2S_PDD_RX_FRAME_COMPLETE_INT);
 *      @endcode
 */
#define I2S_PDD_EnableDmasInterrupts(PeripheralBase, Mask) ( \
    I2S_IER_REG(PeripheralBase) |= \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableDmasInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables DMAs and interrupts defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of DMAs and interrupts. Use constants from group "Interrupt
 *        masks for EnableDmasInterrupts, DisableDmasInterrupts, some of them for
 *        GetInterruptFlag, some of them for ClearInterruptFlags". This
 *        parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_IER.
 * @par Example:
 *      @code
 *      I2S_PDD_DisableDmasInterrupts(deviceID, I2S_PDD_RX_FRAME_COMPLETE_INT);
 *      @endcode
 */
#define I2S_PDD_DisableDmasInterrupts(PeripheralBase, Mask) ( \
    I2S_IER_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteInterruptEnableReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Interrupt enable register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the interrupt enable register. This parameter is
 *        a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_IER.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteInterruptEnableReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteInterruptEnableReg(PeripheralBase, Value) ( \
    I2S_IER_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInterruptEnableReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Interrupt enable register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_IER.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadInterruptEnableReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadInterruptEnableReg(PeripheralBase) ( \
    I2S_IER_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTransmitConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Transmit configuration register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the transmit configuration register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_TCR.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteTransmitConfigurationReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteTransmitConfigurationReg(PeripheralBase, Value) ( \
    I2S_TCR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadTransmitConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Transmit configuration register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_TCR.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadTransmitConfigurationReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadTransmitConfigurationReg(PeripheralBase) ( \
    I2S_TCR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteReceiveConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Receive configuration register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the receive configuration register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_RCR.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteReceiveConfigurationReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteReceiveConfigurationReg(PeripheralBase, Value) ( \
    I2S_RCR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadReceiveConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Receive configuration register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_RCR.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadReceiveConfigurationReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadReceiveConfigurationReg(PeripheralBase) ( \
    I2S_RCR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTransmitClockControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Transmit clock control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the transmit clock control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_TCCR.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteTransmitClockControlReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteTransmitClockControlReg(PeripheralBase, Value) ( \
    I2S_TCCR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadTransmitClockControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Transmit clock control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_TCCR.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadTransmitClockControlReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadTransmitClockControlReg(PeripheralBase) ( \
    I2S_TCCR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteReceiveClockControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Receive clock control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the receive clock control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_RCCR.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteReceiveClockControlReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteReceiveClockControlReg(PeripheralBase, Value) ( \
    I2S_RCCR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadReceiveClockControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Receive clock control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_RCCR.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadReceiveClockControlReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadReceiveClockControlReg(PeripheralBase) ( \
    I2S_RCCR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetRxFIFOCounter1
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns receive FIFO counter 1.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      uint8_t result = I2S_PDD_GetRxFIFOCounter1(deviceID);
 *      @endcode
 */
#define I2S_PDD_GetRxFIFOCounter1(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(I2S_FCSR_REG(PeripheralBase) & I2S_FCSR_RFCNT1_MASK)) >> ( \
     I2S_FCSR_RFCNT1_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetRxFIFOCounter0
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns receive FIFO counter 0.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      uint8_t result = I2S_PDD_GetRxFIFOCounter0(deviceID);
 *      @endcode
 */
#define I2S_PDD_GetRxFIFOCounter0(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(I2S_FCSR_REG(PeripheralBase) & I2S_FCSR_RFCNT0_MASK)) >> ( \
     I2S_FCSR_RFCNT0_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxFIFOCounter1
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns transmit FIFO counter 1.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      uint8_t result = I2S_PDD_GetTxFIFOCounter1(deviceID);
 *      @endcode
 */
#define I2S_PDD_GetTxFIFOCounter1(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(I2S_FCSR_REG(PeripheralBase) & I2S_FCSR_TFCNT1_MASK)) >> ( \
     I2S_FCSR_TFCNT1_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxFIFOCounter0
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns transmit FIFO counter 0.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      uint8_t result = I2S_PDD_GetTxFIFOCounter0(deviceID);
 *      @endcode
 */
#define I2S_PDD_GetTxFIFOCounter0(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(I2S_FCSR_REG(PeripheralBase) & I2S_FCSR_TFCNT0_MASK)) >> ( \
     I2S_FCSR_TFCNT0_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- SetRxFIFOFullWatermark1
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets receive FIFO full watermark 1.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. This parameter is a 4-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      I2S_PDD_SetRxFIFOFullWatermark1(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_SetRxFIFOFullWatermark1(PeripheralBase, Value) ( \
    I2S_FCSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_FCSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_FCSR_RFWM1_MASK))) | ( \
      (uint32_t)((uint32_t)(Value) << I2S_FCSR_RFWM1_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetRxFIFOFullWatermark0
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets receive FIFO full watermark 0.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. This parameter is a 4-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      I2S_PDD_SetRxFIFOFullWatermark0(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_SetRxFIFOFullWatermark0(PeripheralBase, Value) ( \
    I2S_FCSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_FCSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_FCSR_RFWM0_MASK))) | ( \
      (uint32_t)((uint32_t)(Value) << I2S_FCSR_RFWM0_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetTxFIFOFullWatermark1
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets transmit FIFO full watermark 1.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. Value 0 is reserved. This
 *        parameter is a 4-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      I2S_PDD_SetTxFIFOFullWatermark1(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_SetTxFIFOFullWatermark1(PeripheralBase, Value) ( \
    I2S_FCSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_FCSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_FCSR_TFWM1_MASK))) | ( \
      (uint32_t)((uint32_t)(Value) << I2S_FCSR_TFWM1_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetTxFIFOFullWatermark0
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets transmit FIFO full watermark 0.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. Value 0 is reserved. This
 *        parameter is a 4-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      I2S_PDD_SetTxFIFOFullWatermark0(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_SetTxFIFOFullWatermark0(PeripheralBase, Value) ( \
    I2S_FCSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_FCSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_FCSR_TFWM0_MASK))) | ( \
      (uint32_t)(Value))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFIFOControlStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the FIFO control status register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the FIFO control status register. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteFIFOControlStatusReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteFIFOControlStatusReg(PeripheralBase, Value) ( \
    I2S_FCSR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFIFOControlStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the FIFO control status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_FCSR.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadFIFOControlStatusReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadFIFOControlStatusReg(PeripheralBase) ( \
    I2S_FCSR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTxTimeSlotMaskReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the transmit time slot mask register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the transmit time slot mask register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_TMSK.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteTxTimeSlotMaskReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteTxTimeSlotMaskReg(PeripheralBase, Value) ( \
    I2S_TMSK_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadTxTimeSlotMaskReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the transmit time slot register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_TMSK.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadTxTimeSlotMaskReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadTxTimeSlotMaskReg(PeripheralBase) ( \
    I2S_TMSK_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteRxTimeSlotMaskReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the receive time slot mask register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the receive time slot mask register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_RMSK.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteRxTimeSlotMaskReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteRxTimeSlotMaskReg(PeripheralBase, Value) ( \
    I2S_RMSK_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadRxTimeSlotMaskReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the receive time slot register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_RMSK.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadRxTimeSlotMaskReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadRxTimeSlotMaskReg(PeripheralBase) ( \
    I2S_RMSK_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableAC97Mode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the AC97 mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of AC97 mode. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE
 *        and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACNT.
 * @par Example:
 *      @code
 *      I2S_PDD_EnableAC97Mode(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define I2S_PDD_EnableAC97Mode(PeripheralBase, State) ( \
    I2S_ACNT_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_ACNT_REG(PeripheralBase) & (uint32_t)(~(uint32_t)I2S_ACNT_AC97EN_MASK))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- SetAC97Command
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects AC97 command.
 * @param PeripheralBase Peripheral base address.
 * @param Command Parameter specifying AC97 command. This parameter is of "AC97
 *        command constant." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACNT.
 * @par Example:
 *      @code
 *      I2S_PDD_SetAC97Command(deviceID, I2S_PDD_AC97_WRITE_COMMAND);
 *      @endcode
 */
#define I2S_PDD_SetAC97Command(PeripheralBase, Command) ( \
    I2S_ACNT_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(I2S_ACNT_REG(PeripheralBase) & (uint32_t)(~(uint32_t)((uint32_t)0x3U << 3U)))) | ( \
      (uint32_t)(Command))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteAC97ControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the AC97 control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the AC97 control register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACNT.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteAC97ControlReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteAC97ControlReg(PeripheralBase, Value) ( \
    I2S_ACNT_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadAC97ControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the AC97 control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_ACNT.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadAC97ControlReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadAC97ControlReg(PeripheralBase) ( \
    I2S_ACNT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteAC97CommandAddressReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the AC97 command address register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the AC97 command address register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACADD.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteAC97CommandAddressReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteAC97CommandAddressReg(PeripheralBase, Value) ( \
    I2S_ACADD_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadAC97CommandAddressReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the AC97 command address register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_ACADD.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadAC97CommandAddressReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadAC97CommandAddressReg(PeripheralBase) ( \
    I2S_ACADD_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteAC97CommandDataReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the AC97 command data register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the AC97 command data register. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACDAT.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteAC97CommandDataReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteAC97CommandDataReg(PeripheralBase, Value) ( \
    I2S_ACDAT_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadAC97CommandDataReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the AC97 command data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_ACDAT.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadAC97CommandDataReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadAC97CommandDataReg(PeripheralBase) ( \
    I2S_ACDAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteAC97TagReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the AC97 tag register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the AC97 tag register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ATAG.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteAC97TagReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteAC97TagReg(PeripheralBase, Value) ( \
    I2S_ATAG_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadAC97TagReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the AC97 tag register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_ATAG.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadAC97TagReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadAC97TagReg(PeripheralBase) ( \
    I2S_ATAG_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableAC97Channels
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables AC97 channels.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of AC97 channels. Use constants from group "Slot masks for
 *        EnableAC97Channels, DisableAC97Channels, GetAC97ChannelStatusMask". This
 *        parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACCEN.
 * @par Example:
 *      @code
 *      I2S_PDD_EnableAC97Channels(deviceID, I2S_PDD_AC97_DATA_SLOT_3);
 *      @endcode
 */
#define I2S_PDD_EnableAC97Channels(PeripheralBase, Mask) ( \
    I2S_ACCEN_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- WriteAC97ChannelEnableReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the AC97 channel enable register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the AC97 channel enable register. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACCEN.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteAC97ChannelEnableReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteAC97ChannelEnableReg(PeripheralBase, Value) ( \
    I2S_ACCEN_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- DisableAC97Channels
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables AC97 channels.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of AC97 channels. Use constants from group "Slot masks for
 *        EnableAC97Channels, DisableAC97Channels, GetAC97ChannelStatusMask". This
 *        parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACCDIS.
 * @par Example:
 *      @code
 *      I2S_PDD_DisableAC97Channels(deviceID, I2S_PDD_AC97_DATA_SLOT_3);
 *      @endcode
 */
#define I2S_PDD_DisableAC97Channels(PeripheralBase, Mask) ( \
    I2S_ACCDIS_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- WriteAC97ChannelDisableReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the AC97 channel disable register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the AC97 chanel disable register. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: I2S0_ACCDIS.
 * @par Example:
 *      @code
 *      I2S_PDD_WriteAC97ChannelDisableReg(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_WriteAC97ChannelDisableReg(PeripheralBase, Value) ( \
    I2S_ACCDIS_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetAC97ChannelStatusMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns AC97 channel enable bits.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Slot masks for EnableAC97Channels,
 *         DisableAC97Channels, GetAC97ChannelStatusMask" for processing return value.
 * @remarks The macro accesses the following registers: I2S0_ACCST.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_GetAC97ChannelStatusMask(deviceID);
 *      @endcode
 */
#define I2S_PDD_GetAC97ChannelStatusMask(PeripheralBase) ( \
    I2S_ACCST_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadAC97ChannelStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the AC97 channel status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: I2S0_ACCST.
 * @par Example:
 *      @code
 *      uint32_t result = I2S_PDD_ReadAC97ChannelStatusReg(deviceID);
 *      @endcode
 */
#define I2S_PDD_ReadAC97ChannelStatusReg(PeripheralBase) ( \
    I2S_ACCST_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetClockSource
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects clock source (in SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param Source Requested state of clock source. Use constants from group
 *        "Clock source constants for SetClockSource.". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      I2S_PDD_SetClockSource(deviceID, I2S_PDD_DIVIDED_CORE_SYSTEM_CLOCK);
 *      @endcode
 */
#define I2S_PDD_SetClockSource(PeripheralBase, Source) ( \
    SIM_SOPT2_REG(SIM_BASE_PTR) = \
     (( \
      (uint32_t)(SIM_SOPT2_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_SOPT2_I2SSRC_MASK))) | ( \
      (uint32_t)((uint32_t)(Source) << SIM_SOPT2_I2SSRC_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetClockDivider
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets clock divider (in SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param Value Value specifying division. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      I2S_PDD_SetClockDivider(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_SetClockDivider(PeripheralBase, Value) ( \
    SIM_CLKDIV2_REG(SIM_BASE_PTR) = \
     (( \
      (uint32_t)(SIM_CLKDIV2_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_CLKDIV2_I2SDIV_MASK))) | ( \
      (uint32_t)((uint32_t)(Value) << SIM_CLKDIV2_I2SDIV_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetClockMultiplier
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets clock multiplier (in SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param Value Value specifying multiplication. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      I2S_PDD_SetClockMultiplier(deviceID, 1);
 *      @endcode
 */
#define I2S_PDD_SetClockMultiplier(PeripheralBase, Value) ( \
    SIM_CLKDIV2_REG(SIM_BASE_PTR) = \
     (( \
      (uint32_t)(SIM_CLKDIV2_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_CLKDIV2_I2SFRAC_MASK))) | ( \
      (uint32_t)((uint32_t)(Value) << SIM_CLKDIV2_I2SFRAC_SHIFT))) \
  )
#endif  /* #if defined(I2S_PDD_H_) */

/* I2S_PDD.h, eof. */
