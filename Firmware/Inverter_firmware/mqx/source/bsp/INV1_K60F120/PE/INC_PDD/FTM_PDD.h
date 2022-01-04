/*
  PDD layer implementation for peripheral type FTM
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(FTM_PDD_H_)
#define FTM_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error FTM PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK10D5) /* FTM0, FTM1 */ && \
      !defined(MCU_MK10D7) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK10F12) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK10DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK11D5) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK11D5WS) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK12D5) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK20D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK20D5) /* FTM0, FTM1 */ && \
      !defined(MCU_MK20D7) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK20F12) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK20DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK21D5) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK21D5WS) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK22D5) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK30D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK30D7) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK30DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK40D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK40D7) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK40DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK40X256VMD100) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK50D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK50D7) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK50DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK51D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK51D7) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK51DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK52D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK52DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK53D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK53DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK60D10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK60F12) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK60F15) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK60DZ10) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK60N512VMD100) /* FTM0, FTM1, FTM2 */ && \
      !defined(MCU_MK61F12) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK61F15) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK61F12WS) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK61F15WS) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK70F12) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK70F15) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK70F12WS) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_MK70F15WS) /* FTM0, FTM1, FTM2, FTM3 */ && \
      !defined(MCU_PCK20L4) /* FTM0, FTM1 */
  // Unsupported MCU is active
  #error FTM PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* FTM channel constants */
#define FTM_PDD_CHANNEL_0 0U                     /**< 0 */
#define FTM_PDD_CHANNEL_1 0x1U                   /**< 1 */
#define FTM_PDD_CHANNEL_2 0x2U                   /**< 2 */
#define FTM_PDD_CHANNEL_3 0x3U                   /**< 3 */
#define FTM_PDD_CHANNEL_4 0x4U                   /**< 4 */
#define FTM_PDD_CHANNEL_5 0x5U                   /**< 5 */
#define FTM_PDD_CHANNEL_6 0x6U                   /**< 6 */
#define FTM_PDD_CHANNEL_7 0x7U                   /**< 7 */

/* Prescaler constants */
#define FTM_PDD_DIVIDE_1   0U                    /**< 1 */
#define FTM_PDD_DIVIDE_2   0x1U                  /**< 2 */
#define FTM_PDD_DIVIDE_4   0x2U                  /**< 4 */
#define FTM_PDD_DIVIDE_8   0x3U                  /**< 8 */
#define FTM_PDD_DIVIDE_16  0x4U                  /**< 16 */
#define FTM_PDD_DIVIDE_32  0x5U                  /**< 32 */
#define FTM_PDD_DIVIDE_64  0x6U                  /**< 64 */
#define FTM_PDD_DIVIDE_128 0x7U                  /**< 128 */

/* PWM aligned mode constants */
#define FTM_PDD_EDGE_ALIGNED   0U                /**< Edge aligned */
#define FTM_PDD_CENTER_ALIGNED FTM_SC_CPWMS_MASK /**< Center aligned */

/* Interrupt flag masks for ClearChannelFlag */
#define FTM_PDD_FLAG_0 0x1U                      /**< 0 */
#define FTM_PDD_FLAG_1 0x2U                      /**< 1 */
#define FTM_PDD_FLAG_2 0x4U                      /**< 2 */
#define FTM_PDD_FLAG_3 0x8U                      /**< 3 */
#define FTM_PDD_FLAG_4 0x10U                     /**< 4 */
#define FTM_PDD_FLAG_5 0x20U                     /**< 5 */
#define FTM_PDD_FLAG_6 0x40U                     /**< 6 */
#define FTM_PDD_FLAG_7 0x80U                     /**< 7 */

/* Clock source constants. */
#define FTM_PDD_DISABLED 0U                      /**< Disabled */
#define FTM_PDD_SYSTEM   0x8U                    /**< System clock */
#define FTM_PDD_FIXED    0x10U                   /**< Fixed clock */
#define FTM_PDD_EXTERNAL 0x18U                   /**< External clock */

/* Edge and level constants. */
#define FTM_PDD_EDGE_NONE    0U                  /**< Disabled */
#define FTM_PDD_EDGE_RISING  0x4U                /**< Rising */
#define FTM_PDD_EDGE_FALLING 0x8U                /**< Falling */
#define FTM_PDD_EDGE_BOTH    0xCU                /**< Both */

/* Output action constants. */
#define FTM_PDD_OUTPUT_NONE   0U                 /**< Disconnect */
#define FTM_PDD_OUTPUT_TOGGLE 0x10U              /**< Toggle */
#define FTM_PDD_OUTPUT_CLEAR  0x20U              /**< Clear */
#define FTM_PDD_OUTPUT_SET    0x30U              /**< Set */


/* ----------------------------------------------------------------------------
   -- SetPrescaler
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets prescale value.
 * @param PeripheralBase Peripheral base address.
 * @param Prescaler New value of the prescaler. Use constants from group
 *        "Prescaler constants". This parameter is 3 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_SetPrescaler(deviceID, FTM_PDD_DIVIDE_1);
 *      @endcode
 */
#define FTM_PDD_SetPrescaler(PeripheralBase, Prescaler) ( \
    FTM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       FTM_SC_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)FTM_SC_PS_MASK)) & ( \
       (uint32_t)(~(uint32_t)FTM_SC_TOF_MASK))))) | ( \
      (uint32_t)(Prescaler))) \
  )

/* ----------------------------------------------------------------------------
   -- SelectPrescalerSource
   ---------------------------------------------------------------------------- */

/**
 * @brief Select clock source.
 * @param PeripheralBase Peripheral base address.
 * @param Source New value of the source. This parameter is of "Clock source
 *        constants." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_SelectPrescalerSource(deviceID, FTM_PDD_DISABLED);
 *      @endcode
 */
#define FTM_PDD_SelectPrescalerSource(PeripheralBase, Source) ( \
    FTM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       FTM_SC_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)FTM_SC_CLKS_MASK)) & ( \
       (uint32_t)(~(uint32_t)FTM_SC_TOF_MASK))))) | ( \
      (uint32_t)(Source))) \
  )

/* ----------------------------------------------------------------------------
   -- GetEnableDeviceStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current state of FTM device.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_GetEnableDeviceStatus(deviceID);
 *      @endcode
 */
#define FTM_PDD_GetEnableDeviceStatus(PeripheralBase) ( \
    (uint32_t)(FTM_SC_REG(PeripheralBase) & FTM_SC_CLKS_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SelectPwmAlignMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Configures PWM aligned mode.
 * @param PeripheralBase Peripheral base address.
 * @param Mode New value of the mode. Use constants from group "PWM aligned mode
 *        constants". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_SelectPwmAlignMode(deviceID, FTM_PDD_EDGE_ALIGNED);
 *      @endcode
 */
#define FTM_PDD_SelectPwmAlignMode(PeripheralBase, Mode) ( \
    FTM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       FTM_SC_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)FTM_SC_CPWMS_MASK)) & ( \
       (uint32_t)(~(uint32_t)FTM_SC_TOF_MASK))))) | ( \
      (uint32_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- GetOverflowInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns overflow interrupt mask.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_GetOverflowInterruptMask(deviceID);
 *      @endcode
 */
#define FTM_PDD_GetOverflowInterruptMask(PeripheralBase) ( \
    (uint32_t)(FTM_SC_REG(PeripheralBase) & FTM_SC_TOIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableOverflowInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the FTM overflow interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_EnableOverflowInterrupt(deviceID);
 *      @endcode
 */
#define FTM_PDD_EnableOverflowInterrupt(PeripheralBase) ( \
    FTM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(FTM_SC_REG(PeripheralBase) | FTM_SC_TOIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)FTM_SC_TOF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableOverflowInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the FTM overflow interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_DisableOverflowInterrupt(deviceID);
 *      @endcode
 */
#define FTM_PDD_DisableOverflowInterrupt(PeripheralBase) ( \
    FTM_SC_REG(PeripheralBase) &= \
     (uint32_t)(( \
      (uint32_t)(~(uint32_t)FTM_SC_TOIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)FTM_SC_TOF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- GetOverflowInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns overflow interrupt flag bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_GetOverflowInterruptFlag(deviceID);
 *      @endcode
 */
#define FTM_PDD_GetOverflowInterruptFlag(PeripheralBase) ( \
    (uint32_t)(FTM_SC_REG(PeripheralBase) & FTM_SC_TOF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearOverflowInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears overflow interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SC, FTM1_SC,
 *          FTM2_SC, FTM3_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_ClearOverflowInterruptFlag(deviceID);
 *      @endcode
 */
#define FTM_PDD_ClearOverflowInterruptFlag(PeripheralBase) ( \
    FTM_SC_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)FTM_SC_TOF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ReadCounterReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the counter register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_CNT, FTM1_CNT,
 *          FTM2_CNT, FTM3_CNT (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadCounterReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadCounterReg(PeripheralBase) ( \
    FTM_CNT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- InitializeCounter
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value 0 to the counter register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_CNT, FTM1_CNT,
 *          FTM2_CNT, FTM3_CNT (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_InitializeCounter(deviceID);
 *      @endcode
 */
#define FTM_PDD_InitializeCounter(PeripheralBase) ( \
    FTM_CNT_REG(PeripheralBase) = \
     0U \
  )

/* ----------------------------------------------------------------------------
   -- WriteModuloReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the modulo register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the modulo register. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_MOD, FTM1_MOD,
 *          FTM2_MOD, FTM3_MOD (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteModuloReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteModuloReg(PeripheralBase, Value) ( \
    FTM_MOD_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadModuloReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the modulo register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_MOD, FTM1_MOD,
 *          FTM2_MOD, FTM3_MOD (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadModuloReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadModuloReg(PeripheralBase) ( \
    FTM_MOD_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableChannelDma
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the FTM channel DMA.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_EnableChannelDma(deviceID, FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_EnableChannelDma(PeripheralBase, ChannelIdx) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) | FTM_CnSC_DMA_MASK)) & ( \
      (uint32_t)(~(uint32_t)FTM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableChannelDma
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the FTM channel DMA.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_DisableChannelDma(deviceID, FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_DisableChannelDma(PeripheralBase, ChannelIdx) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) &= \
     (uint32_t)(( \
      (uint32_t)(~(uint32_t)FTM_CnSC_DMA_MASK)) & ( \
      (uint32_t)(~(uint32_t)FTM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- SelectChannelEdgeLevel
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects the FTM channel edge and level.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @param ELSBA_val FTM channel ELSB:ELSA bits. This parameter is of "Edge and
 *        level constants." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_SelectChannelEdgeLevel(deviceID, FTM_PDD_CHANNEL_0,
 *      FTM_PDD_EDGE_NONE);
 *      @endcode
 */
#define FTM_PDD_SelectChannelEdgeLevel(PeripheralBase, ChannelIdx, ELSBA_val) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       FTM_CnSC_REG(PeripheralBase,(ChannelIdx))) & (( \
       (uint32_t)(~(uint32_t)((uint32_t)0x3U << 2U))) & ( \
       (uint32_t)(~(uint32_t)FTM_CnSC_CHF_MASK))))) | ( \
      (uint32_t)(ELSBA_val))) \
  )

/* ----------------------------------------------------------------------------
   -- SelectChannelMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects the FTM channel mode.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @param MSBA_val FTM channel MSB:MSA bits. This parameter is of "Output action
 *        constants." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_SelectChannelMode(deviceID, FTM_PDD_CHANNEL_0,
 *      FTM_PDD_OUTPUT_NONE);
 *      @endcode
 */
#define FTM_PDD_SelectChannelMode(PeripheralBase, ChannelIdx, MSBA_val) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       FTM_CnSC_REG(PeripheralBase,(ChannelIdx))) & (( \
       (uint32_t)(~(uint32_t)((uint32_t)0x3U << 4U))) & ( \
       (uint32_t)(~(uint32_t)FTM_CnSC_CHF_MASK))))) | ( \
      (uint32_t)(MSBA_val))) \
  )

/* ----------------------------------------------------------------------------
   -- GetChannelInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns channel interrupt mask.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_GetChannelInterruptMask(deviceID,
 *      FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_GetChannelInterruptMask(PeripheralBase, ChannelIdx) ( \
    (uint32_t)(FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) & FTM_CnSC_CHIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableChannelInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the FTM channel interrupt.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_EnableChannelInterrupt(deviceID, FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_EnableChannelInterrupt(PeripheralBase, ChannelIdx) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) | FTM_CnSC_CHIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)FTM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableChannelInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the FTM channel interrupt.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_DisableChannelInterrupt(deviceID, FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_DisableChannelInterrupt(PeripheralBase, ChannelIdx) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) &= \
     (uint32_t)(( \
      (uint32_t)(~(uint32_t)FTM_CnSC_CHIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)FTM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- GetChannelInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns channel interrupt flag bit.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_GetChannelInterruptFlag(deviceID,
 *      FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_GetChannelInterruptFlag(PeripheralBase, ChannelIdx) ( \
    (uint32_t)(FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) & FTM_CnSC_CHF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearChannelInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears channel interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_ClearChannelInterruptFlag(deviceID, FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_ClearChannelInterruptFlag(PeripheralBase, ChannelIdx) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) &= \
     (uint32_t)(~(uint32_t)FTM_CnSC_CHF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ReadChannelControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the channel status and control register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadChannelControlReg(deviceID,
 *      FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_ReadChannelControlReg(PeripheralBase, ChannelIdx) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteChannelControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the channel status and control register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @param Value New content of the channel status and control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_WriteChannelControlReg(deviceID, FTM_PDD_CHANNEL_0, 1);
 *      @endcode
 */
#define FTM_PDD_WriteChannelControlReg(PeripheralBase, ChannelIdx, Value) ( \
    FTM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadChannelValueReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the channel value register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnV[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadChannelValueReg(deviceID,
 *      FTM_PDD_CHANNEL_0);
 *      @endcode
 */
#define FTM_PDD_ReadChannelValueReg(PeripheralBase, ChannelIdx) ( \
    FTM_CnV_REG(PeripheralBase,(ChannelIdx)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteChannelValueReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the channel value register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx FTM channel index. Use constants from group "FTM channel
 *        constants". This parameter is 32 bits wide.
 * @param Value New content of the channel value register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnV[ChannelIdx].
 * @par Example:
 *      @code
 *      FTM_PDD_WriteChannelValueReg(deviceID, FTM_PDD_CHANNEL_0, 1);
 *      @endcode
 */
#define FTM_PDD_WriteChannelValueReg(PeripheralBase, ChannelIdx, Value) ( \
    FTM_CnV_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteInitialValueReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the counter initial value register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the counter initial value register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_CNTIN, FTM1_CNTIN,
 *          FTM2_CNTIN, FTM3_CNTIN (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteInitialValueReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteInitialValueReg(PeripheralBase, Value) ( \
    FTM_CNTIN_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInitialValueReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the counter initial value register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_CNTIN, FTM1_CNTIN,
 *          FTM2_CNTIN, FTM3_CNTIN (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadInitialValueReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadInitialValueReg(PeripheralBase) ( \
    FTM_CNTIN_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetChannelFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the capture and compare status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_STATUS,
 *          FTM1_STATUS, FTM2_STATUS, FTM3_STATUS (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_GetChannelFlags(deviceID);
 *      @endcode
 */
#define FTM_PDD_GetChannelFlags(PeripheralBase) ( \
    FTM_STATUS_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearChannelFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears channel interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt flag mask. Use constants from group "Interrupt flag
 *        masks for ClearChannelFlag". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_STATUS,
 *          FTM1_STATUS, FTM2_STATUS, FTM3_STATUS (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_ClearChannelFlags(deviceID, FTM_PDD_FLAG_0);
 *      @endcode
 */
#define FTM_PDD_ClearChannelFlags(PeripheralBase, Mask) ( \
    FTM_STATUS_REG(PeripheralBase) = \
     (uint32_t)((uint32_t)(~(uint32_t)(Mask)) & (uint32_t)0xFFU) \
  )

/* ----------------------------------------------------------------------------
   -- EnableFaultInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the FTM fault interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_MODE, FTM1_MODE,
 *          FTM2_MODE, FTM3_MODE (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_EnableFaultInterrupt(deviceID);
 *      @endcode
 */
#define FTM_PDD_EnableFaultInterrupt(PeripheralBase) ( \
    FTM_MODE_REG(PeripheralBase) |= \
     FTM_MODE_FAULTIE_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableFaultInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the FTM fault interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_MODE, FTM1_MODE,
 *          FTM2_MODE, FTM3_MODE (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_DisableFaultInterrupt(deviceID);
 *      @endcode
 */
#define FTM_PDD_DisableFaultInterrupt(PeripheralBase) ( \
    FTM_MODE_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)FTM_MODE_FAULTIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- WriteProtectionDisable
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the write protection.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_MODE, FTM1_MODE,
 *          FTM2_MODE, FTM3_MODE (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteProtectionDisable(deviceID);
 *      @endcode
 */
#define FTM_PDD_WriteProtectionDisable(PeripheralBase) ( \
    FTM_MODE_REG(PeripheralBase) |= \
     FTM_MODE_WPDIS_MASK \
  )

/* ----------------------------------------------------------------------------
   -- InitializeOutputs
   ---------------------------------------------------------------------------- */

/**
 * @brief Initialize the channel outputs.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_MODE, FTM1_MODE,
 *          FTM2_MODE, FTM3_MODE (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_InitializeOutputs(deviceID);
 *      @endcode
 */
#define FTM_PDD_InitializeOutputs(PeripheralBase) ( \
    FTM_MODE_REG(PeripheralBase) |= \
     FTM_MODE_INIT_MASK \
  )

/* ----------------------------------------------------------------------------
   -- WriteFeaturesModeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the feature mode selection register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the feature mode selection register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_MODE, FTM1_MODE,
 *          FTM2_MODE, FTM3_MODE (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteFeaturesModeReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteFeaturesModeReg(PeripheralBase, Value) ( \
    FTM_MODE_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFeaturesModeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the features mode selection register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_MODE, FTM1_MODE,
 *          FTM2_MODE, FTM3_MODE (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadFeaturesModeReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadFeaturesModeReg(PeripheralBase) ( \
    FTM_MODE_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteSynchronizationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the synchronization register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the synchronization register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SYNC, FTM1_SYNC,
 *          FTM2_SYNC, FTM3_SYNC (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteSynchronizationReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteSynchronizationReg(PeripheralBase, Value) ( \
    FTM_SYNC_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadSynchronizationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the synchronization register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_SYNC, FTM1_SYNC,
 *          FTM2_SYNC, FTM3_SYNC (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadSynchronizationReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadSynchronizationReg(PeripheralBase) ( \
    FTM_SYNC_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteInitialOutputReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the initial state for channels output register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the initial state for channel output register.
 *        This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_OUTINIT,
 *          FTM1_OUTINIT, FTM2_OUTINIT, FTM3_OUTINIT (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteInitialOutputReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteInitialOutputReg(PeripheralBase, Value) ( \
    FTM_OUTINIT_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInitialOutputReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the initial state for channels output register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_OUTINIT,
 *          FTM1_OUTINIT, FTM2_OUTINIT, FTM3_OUTINIT (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadInitialOutputReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadInitialOutputReg(PeripheralBase) ( \
    FTM_OUTINIT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteOutputMaskReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the output mask register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the output mask register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_OUTMASK,
 *          FTM1_OUTMASK, FTM2_OUTMASK, FTM3_OUTMASK (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteOutputMaskReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteOutputMaskReg(PeripheralBase, Value) ( \
    FTM_OUTMASK_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadOutputMaskReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the output mask register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_OUTMASK,
 *          FTM1_OUTMASK, FTM2_OUTMASK, FTM3_OUTMASK (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadOutputMaskReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadOutputMaskReg(PeripheralBase) ( \
    FTM_OUTMASK_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteCombineReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the function for linked channels register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the function for linked channels register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_COMBINE,
 *          FTM1_COMBINE, FTM2_COMBINE, FTM3_COMBINE (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteCombineReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteCombineReg(PeripheralBase, Value) ( \
    FTM_COMBINE_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadCombineReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the function for linked channels register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_COMBINE,
 *          FTM1_COMBINE, FTM2_COMBINE, FTM3_COMBINE (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadCombineReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadCombineReg(PeripheralBase) ( \
    FTM_COMBINE_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteDeadtimeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the deadtime insertion control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the deadtime insertion control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_DEADTIME,
 *          FTM1_DEADTIME, FTM2_DEADTIME, FTM3_DEADTIME (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteDeadtimeReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteDeadtimeReg(PeripheralBase, Value) ( \
    FTM_DEADTIME_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadDeadtimeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the deadtime insertion control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_DEADTIME,
 *          FTM1_DEADTIME, FTM2_DEADTIME, FTM3_DEADTIME (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadDeadtimeReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadDeadtimeReg(PeripheralBase) ( \
    FTM_DEADTIME_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteExternalTriggerReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the FTM external trigger register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the FTM external trigger register. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_EXTTRIG,
 *          FTM1_EXTTRIG, FTM2_EXTTRIG, FTM3_EXTTRIG (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteExternalTriggerReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteExternalTriggerReg(PeripheralBase, Value) ( \
    FTM_EXTTRIG_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadExternalTriggerReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the FTM external trigger register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_EXTTRIG,
 *          FTM1_EXTTRIG, FTM2_EXTTRIG, FTM3_EXTTRIG (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadExternalTriggerReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadExternalTriggerReg(PeripheralBase) ( \
    FTM_EXTTRIG_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WritePolarityReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the channels polarity register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the channels polarity register. This parameter is
 *        a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_POL, FTM1_POL,
 *          FTM2_POL, FTM3_POL (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WritePolarityReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WritePolarityReg(PeripheralBase, Value) ( \
    FTM_POL_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadPolarityReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the channels polarity register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_POL, FTM1_POL,
 *          FTM2_POL, FTM3_POL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadPolarityReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadPolarityReg(PeripheralBase) ( \
    FTM_POL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFaultStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the fault mode status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_FMS, FTM1_FMS,
 *          FTM2_FMS, FTM3_FMS (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadFaultStatusReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadFaultStatusReg(PeripheralBase) ( \
    FTM_FMS_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteProtectionEnable
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the write protection.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_FMS, FTM1_FMS,
 *          FTM2_FMS, FTM3_FMS (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteProtectionEnable(deviceID);
 *      @endcode
 */
#define FTM_PDD_WriteProtectionEnable(PeripheralBase) ( \
    FTM_FMS_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(FTM_FMS_REG(PeripheralBase) | FTM_FMS_WPEN_MASK)) & (( \
      (uint32_t)(~(uint32_t)FTM_FMS_FAULTF0_MASK)) & (( \
      (uint32_t)(~(uint32_t)FTM_FMS_FAULTF1_MASK)) & (( \
      (uint32_t)(~(uint32_t)FTM_FMS_FAULTF2_MASK)) & (( \
      (uint32_t)(~(uint32_t)FTM_FMS_FAULTF3_MASK)) & ( \
      (uint32_t)(~(uint32_t)FTM_FMS_FAULTF_MASK))))))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteInputCaptureFilterReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the input capture filter control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the input capture filter control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_FILTER,
 *          FTM1_FILTER, FTM2_FILTER, FTM3_FILTER (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteInputCaptureFilterReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteInputCaptureFilterReg(PeripheralBase, Value) ( \
    FTM_FILTER_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInputCaptureFilterReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the input capture filter control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_FILTER,
 *          FTM1_FILTER, FTM2_FILTER, FTM3_FILTER (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadInputCaptureFilterReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadInputCaptureFilterReg(PeripheralBase) ( \
    FTM_FILTER_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFaultControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the fault control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the fault control register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_FLTCTRL,
 *          FTM1_FLTCTRL, FTM2_FLTCTRL, FTM3_FLTCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteFaultControlReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteFaultControlReg(PeripheralBase, Value) ( \
    FTM_FLTCTRL_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFaultControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the fault control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_FLTCTRL,
 *          FTM1_FLTCTRL, FTM2_FLTCTRL, FTM3_FLTCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadFaultControlReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadFaultControlReg(PeripheralBase) ( \
    FTM_FLTCTRL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteQuadratureDecoderReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the quadrature decoder control and status register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the quadrature decoder control and status
 *        register. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_QDCTRL,
 *          FTM1_QDCTRL, FTM2_QDCTRL, FTM3_QDCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteQuadratureDecoderReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteQuadratureDecoderReg(PeripheralBase, Value) ( \
    FTM_QDCTRL_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadQuadratureDecoderReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the quadrature decoder control and status
 * register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_QDCTRL,
 *          FTM1_QDCTRL, FTM2_QDCTRL, FTM3_QDCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadQuadratureDecoderReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadQuadratureDecoderReg(PeripheralBase) ( \
    FTM_QDCTRL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the configuration register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the configuration register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_CONF, FTM1_CONF,
 *          FTM2_CONF, FTM3_CONF (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteConfigurationReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteConfigurationReg(PeripheralBase, Value) ( \
    FTM_CONF_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the configuration register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_CONF, FTM1_CONF,
 *          FTM2_CONF, FTM3_CONF (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadConfigurationReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadConfigurationReg(PeripheralBase) ( \
    FTM_CONF_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFaultInputPolarityReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the FTM fault input polarity register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the FTM fault input polarity register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_FLTPOL,
 *          FTM1_FLTPOL, FTM2_FLTPOL, FTM3_FLTPOL (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteFaultInputPolarityReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteFaultInputPolarityReg(PeripheralBase, Value) ( \
    FTM_FLTPOL_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFaultInputPolarityReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the FTM fault input polarity register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_FLTPOL,
 *          FTM1_FLTPOL, FTM2_FLTPOL, FTM3_FLTPOL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadFaultInputPolarityReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadFaultInputPolarityReg(PeripheralBase) ( \
    FTM_FLTPOL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteSynchronizationConfigReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the synchronization configuration register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the synchronization configuration register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SYNCONF,
 *          FTM1_SYNCONF, FTM2_SYNCONF, FTM3_SYNCONF (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteSynchronizationConfigReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteSynchronizationConfigReg(PeripheralBase, Value) ( \
    FTM_SYNCONF_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadSynchronizationConfigReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the synchronization configuration register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_SYNCONF,
 *          FTM1_SYNCONF, FTM2_SYNCONF, FTM3_SYNCONF (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadSynchronizationConfigReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadSynchronizationConfigReg(PeripheralBase) ( \
    FTM_SYNCONF_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteInvertingReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the FTM inverting control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the FTM inverting control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_INVCTRL,
 *          FTM1_INVCTRL, FTM2_INVCTRL, FTM3_INVCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteInvertingReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteInvertingReg(PeripheralBase, Value) ( \
    FTM_INVCTRL_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInvertingReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the FTM inverting control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_INVCTRL,
 *          FTM1_INVCTRL, FTM2_INVCTRL, FTM3_INVCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadInvertingReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadInvertingReg(PeripheralBase) ( \
    FTM_INVCTRL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteSoftwareOutputReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the FTM software output control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the FTM software output control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_SWOCTRL,
 *          FTM1_SWOCTRL, FTM2_SWOCTRL, FTM3_SWOCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WriteSoftwareOutputReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WriteSoftwareOutputReg(PeripheralBase, Value) ( \
    FTM_SWOCTRL_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadSoftwareOutputReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the FTM software output control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_SWOCTRL,
 *          FTM1_SWOCTRL, FTM2_SWOCTRL, FTM3_SWOCTRL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadSoftwareOutputReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadSoftwareOutputReg(PeripheralBase) ( \
    FTM_SWOCTRL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WritePwmLoadReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the FTM PWM load register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the FTM PWM load register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTM0_PWMLOAD,
 *          FTM1_PWMLOAD, FTM2_PWMLOAD, FTM3_PWMLOAD (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTM_PDD_WritePwmLoadReg(deviceID, 1);
 *      @endcode
 */
#define FTM_PDD_WritePwmLoadReg(PeripheralBase, Value) ( \
    FTM_PWMLOAD_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadPwmLoadReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the FTM PWM load register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTM0_PWMLOAD,
 *          FTM1_PWMLOAD, FTM2_PWMLOAD, FTM3_PWMLOAD (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = FTM_PDD_ReadPwmLoadReg(deviceID);
 *      @endcode
 */
#define FTM_PDD_ReadPwmLoadReg(PeripheralBase) ( \
    FTM_PWMLOAD_REG(PeripheralBase) \
  )
#endif  /* #if defined(FTM_PDD_H_) */

/* FTM_PDD.h, eof. */
