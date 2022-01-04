/*
  PDD layer implementation for peripheral type TPM
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(TPM_PDD_H_)
#define TPM_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error TPM PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MKL02Z4) /* TPM0, TPM1 */ && \
      !defined(MCU_MKL04Z4) /* TPM0, TPM1 */ && \
      !defined(MCU_MKL05Z4) /* TPM0, TPM1 */ && \
      !defined(MCU_MKL14Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL15Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL16Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL24Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL25Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL26Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL34Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL36Z4) /* TPM0, TPM1, TPM2 */ && \
      !defined(MCU_MKL46Z4) /* TPM0, TPM1, TPM2 */
  // Unsupported MCU is active
  #error TPM PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* TPM channel constants */
#define TPM_PDD_CHANNEL_0 0U                     /**< 0 */
#define TPM_PDD_CHANNEL_1 0x1U                   /**< 1 */
#define TPM_PDD_CHANNEL_2 0x2U                   /**< 2 */
#define TPM_PDD_CHANNEL_3 0x3U                   /**< 3 */
#define TPM_PDD_CHANNEL_4 0x4U                   /**< 4 */
#define TPM_PDD_CHANNEL_5 0x5U                   /**< 5 */
#define TPM_PDD_CHANNEL_6 0x6U                   /**< 6 */
#define TPM_PDD_CHANNEL_7 0x7U                   /**< 7 */

/* Prescaler constants */
#define TPM_PDD_DIVIDE_1   0U                    /**< 1 */
#define TPM_PDD_DIVIDE_2   0x1U                  /**< 2 */
#define TPM_PDD_DIVIDE_4   0x2U                  /**< 4 */
#define TPM_PDD_DIVIDE_8   0x3U                  /**< 8 */
#define TPM_PDD_DIVIDE_16  0x4U                  /**< 16 */
#define TPM_PDD_DIVIDE_32  0x5U                  /**< 32 */
#define TPM_PDD_DIVIDE_64  0x6U                  /**< 64 */
#define TPM_PDD_DIVIDE_128 0x7U                  /**< 128 */

/* PWM aligned mode constants */
#define TPM_PDD_EDGE_ALIGNED   0U                /**< Edge aligned */
#define TPM_PDD_CENTER_ALIGNED TPM_SC_CPWMS_MASK /**< Center aligned */

#if (defined(MCU_MKL02Z4))
/* Interrupt flag masks for ClearChannelFlag */
  #define TPM_PDD_FLAG_0 TPM_STATUS_CH0F_MASK      /**< 0 */
  #define TPM_PDD_FLAG_1 TPM_STATUS_CH1F_MASK      /**< 1 */

#else /* (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) */
/* Interrupt flag masks for ClearChannelFlag */
  #define TPM_PDD_FLAG_0 TPM_STATUS_CH0F_MASK      /**< 0 */
  #define TPM_PDD_FLAG_1 TPM_STATUS_CH1F_MASK      /**< 1 */
  #define TPM_PDD_FLAG_2 TPM_STATUS_CH2F_MASK      /**< 2 */
  #define TPM_PDD_FLAG_3 TPM_STATUS_CH3F_MASK      /**< 3 */
  #define TPM_PDD_FLAG_4 TPM_STATUS_CH4F_MASK      /**< 4 */
  #define TPM_PDD_FLAG_5 TPM_STATUS_CH5F_MASK      /**< 5 */

#endif /* (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) */
/* Clock source constants. */
#define TPM_PDD_DISABLED 0U                      /**< Disabled */
#define TPM_PDD_SYSTEM   0x8U                    /**< Counter clock */
#define TPM_PDD_EXTERNAL 0x10U                   /**< External clock */

/* Edge and level constants. */
#define TPM_PDD_EDGE_NONE    0U                  /**< Disabled */
#define TPM_PDD_EDGE_RISING  0x4U                /**< Rising */
#define TPM_PDD_EDGE_FALLING 0x8U                /**< Falling */
#define TPM_PDD_EDGE_BOTH    0xCU                /**< Both */

/* Output action constants. */
#define TPM_PDD_OUTPUT_NONE   0U                 /**< Disconnect */
#define TPM_PDD_OUTPUT_TOGGLE 0x10U              /**< Toggle */
#define TPM_PDD_OUTPUT_CLEAR  0x20U              /**< Clear */
#define TPM_PDD_OUTPUT_SET    0x30U              /**< Set */


/* ----------------------------------------------------------------------------
   -- SetPrescaler
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets prescale value.
 * @param PeripheralBase Peripheral base address.
 * @param Prescaler New value of the prescaler. Use constants from group
 *        "Prescaler constants". This parameter is 3 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_SetPrescaler(deviceID, TPM_PDD_DIVIDE_1);
 *      @endcode
 */
#define TPM_PDD_SetPrescaler(PeripheralBase, Prescaler) ( \
    TPM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       TPM_SC_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)TPM_SC_PS_MASK)) & ( \
       (uint32_t)(~(uint32_t)TPM_SC_TOF_MASK))))) | ( \
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
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_SelectPrescalerSource(deviceID, TPM_PDD_DISABLED);
 *      @endcode
 */
#define TPM_PDD_SelectPrescalerSource(PeripheralBase, Source) ( \
    TPM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       TPM_SC_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)TPM_SC_CMOD_MASK)) & ( \
       (uint32_t)(~(uint32_t)TPM_SC_TOF_MASK))))) | ( \
      (uint32_t)(Source))) \
  )

/* ----------------------------------------------------------------------------
   -- GetEnableDeviceStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current state of TPM device.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_GetEnableDeviceStatus(deviceID);
 *      @endcode
 */
#define TPM_PDD_GetEnableDeviceStatus(PeripheralBase) ( \
    (uint32_t)(TPM_SC_REG(PeripheralBase) & TPM_SC_CMOD_MASK) \
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
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_SelectPwmAlignMode(deviceID, TPM_PDD_EDGE_ALIGNED);
 *      @endcode
 */
#define TPM_PDD_SelectPwmAlignMode(PeripheralBase, Mode) ( \
    TPM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       TPM_SC_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)TPM_SC_CPWMS_MASK)) & ( \
       (uint32_t)(~(uint32_t)TPM_SC_TOF_MASK))))) | ( \
      (uint32_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- GetOverflowInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns overflow interrupt mask.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_GetOverflowInterruptMask(deviceID);
 *      @endcode
 */
#define TPM_PDD_GetOverflowInterruptMask(PeripheralBase) ( \
    (uint32_t)(TPM_SC_REG(PeripheralBase) & TPM_SC_TOIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableOverflowInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the TPM overflow interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_EnableOverflowInterrupt(deviceID);
 *      @endcode
 */
#define TPM_PDD_EnableOverflowInterrupt(PeripheralBase) ( \
    TPM_SC_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(TPM_SC_REG(PeripheralBase) | TPM_SC_TOIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)TPM_SC_TOF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableOverflowInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the TPM overflow interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_DisableOverflowInterrupt(deviceID);
 *      @endcode
 */
#define TPM_PDD_DisableOverflowInterrupt(PeripheralBase) ( \
    TPM_SC_REG(PeripheralBase) &= \
     (uint32_t)(( \
      (uint32_t)(~(uint32_t)TPM_SC_TOIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)TPM_SC_TOF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- GetOverflowInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns overflow interrupt flag bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_GetOverflowInterruptFlag(deviceID);
 *      @endcode
 */
#define TPM_PDD_GetOverflowInterruptFlag(PeripheralBase) ( \
    (uint32_t)(TPM_SC_REG(PeripheralBase) & TPM_SC_TOF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearOverflowInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears overflow interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: TPM0_SC, TPM1_SC,
 *          TPM2_SC (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_ClearOverflowInterruptFlag(deviceID);
 *      @endcode
 */
#define TPM_PDD_ClearOverflowInterruptFlag(PeripheralBase) ( \
    TPM_SC_REG(PeripheralBase) |= \
     TPM_SC_TOF_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ReadCounterReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the counter register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: TPM0_CNT, TPM1_CNT,
 *          TPM2_CNT (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_ReadCounterReg(deviceID);
 *      @endcode
 */
#define TPM_PDD_ReadCounterReg(PeripheralBase) ( \
    TPM_CNT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- InitializeCounter
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value 0 to the counter register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: TPM0_CNT, TPM1_CNT,
 *          TPM2_CNT (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_InitializeCounter(deviceID);
 *      @endcode
 */
#define TPM_PDD_InitializeCounter(PeripheralBase) ( \
    TPM_CNT_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: TPM0_MOD, TPM1_MOD,
 *          TPM2_MOD (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_WriteModuloReg(deviceID, 1);
 *      @endcode
 */
#define TPM_PDD_WriteModuloReg(PeripheralBase, Value) ( \
    TPM_MOD_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadModuloReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the modulo register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: TPM0_MOD, TPM1_MOD,
 *          TPM2_MOD (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_ReadModuloReg(deviceID);
 *      @endcode
 */
#define TPM_PDD_ReadModuloReg(PeripheralBase) ( \
    TPM_MOD_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableChannelDma
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the TPM channel DMA.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_EnableChannelDma(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_EnableChannelDma(PeripheralBase, ChannelIdx) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) | TPM_CnSC_DMA_MASK)) & ( \
      (uint32_t)(~(uint32_t)TPM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableChannelDma
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the TPM channel DMA.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_DisableChannelDma(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_DisableChannelDma(PeripheralBase, ChannelIdx) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) &= \
     (uint32_t)(( \
      (uint32_t)(~(uint32_t)TPM_CnSC_DMA_MASK)) & ( \
      (uint32_t)(~(uint32_t)TPM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- SelectChannelEdgeLevel
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects the TPM channel edge and level.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @param ELSBA_val TPM channel ELSB:ELSA bits. This parameter is of "Edge and
 *        level constants." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_SelectChannelEdgeLevel(deviceID, periphID, TPM_PDD_EDGE_NONE);
 *      @endcode
 */
#define TPM_PDD_SelectChannelEdgeLevel(PeripheralBase, ChannelIdx, ELSBA_val) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       TPM_CnSC_REG(PeripheralBase,(ChannelIdx))) & (( \
       (uint32_t)(~(uint32_t)((uint32_t)0x3U << 2U))) & ( \
       (uint32_t)(~(uint32_t)TPM_CnSC_CHF_MASK))))) | ( \
      (uint32_t)(ELSBA_val))) \
  )

/* ----------------------------------------------------------------------------
   -- SelectChannelMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects the TPM channel mode.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @param MSBA_val TPM channel MSB:MSA bits. This parameter is of "Output action
 *        constants." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_SelectChannelMode(deviceID, periphID, TPM_PDD_OUTPUT_NONE);
 *      @endcode
 */
#define TPM_PDD_SelectChannelMode(PeripheralBase, ChannelIdx, MSBA_val) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       TPM_CnSC_REG(PeripheralBase,(ChannelIdx))) & (( \
       (uint32_t)(~(uint32_t)((uint32_t)0x3U << 4U))) & ( \
       (uint32_t)(~(uint32_t)TPM_CnSC_CHF_MASK))))) | ( \
      (uint32_t)(MSBA_val))) \
  )

/* ----------------------------------------------------------------------------
   -- GetChannelInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns channel interrupt mask.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_GetChannelInterruptMask(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_GetChannelInterruptMask(PeripheralBase, ChannelIdx) ( \
    (uint32_t)(TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) & TPM_CnSC_CHIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableChannelInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the TPM channel interrupt.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_EnableChannelInterrupt(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_EnableChannelInterrupt(PeripheralBase, ChannelIdx) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(( \
      (uint32_t)(TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) | TPM_CnSC_CHIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)TPM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableChannelInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the TPM channel interrupt.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_DisableChannelInterrupt(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_DisableChannelInterrupt(PeripheralBase, ChannelIdx) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) &= \
     (uint32_t)(( \
      (uint32_t)(~(uint32_t)TPM_CnSC_CHIE_MASK)) & ( \
      (uint32_t)(~(uint32_t)TPM_CnSC_CHF_MASK))) \
  )

/* ----------------------------------------------------------------------------
   -- GetChannelInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns channel interrupt flag bit.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_GetChannelInterruptFlag(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_GetChannelInterruptFlag(PeripheralBase, ChannelIdx) ( \
    (uint32_t)(TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) & TPM_CnSC_CHF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearChannelInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears channel interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_ClearChannelInterruptFlag(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_ClearChannelInterruptFlag(PeripheralBase, ChannelIdx) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) |= \
     TPM_CnSC_CHF_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ReadChannelControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the channel status and control register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_ReadChannelControlReg(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_ReadChannelControlReg(PeripheralBase, ChannelIdx) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteChannelControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the channel status and control register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @param Value New content of the channel status and control register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnSC[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_WriteChannelControlReg(deviceID, periphID, 1);
 *      @endcode
 */
#define TPM_PDD_WriteChannelControlReg(PeripheralBase, ChannelIdx, Value) ( \
    TPM_CnSC_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadChannelValueReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the channel value register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CnV[ChannelIdx].
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_ReadChannelValueReg(deviceID, periphID);
 *      @endcode
 */
#define TPM_PDD_ReadChannelValueReg(PeripheralBase, ChannelIdx) ( \
    TPM_CnV_REG(PeripheralBase,(ChannelIdx)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteChannelValueReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the channel value register.
 * @param PeripheralBase Peripheral base address.
 * @param ChannelIdx TPM channel index. This parameter is of index type.
 * @param Value New content of the channel value register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CnV[ChannelIdx].
 * @par Example:
 *      @code
 *      TPM_PDD_WriteChannelValueReg(deviceID, periphID, 1);
 *      @endcode
 */
#define TPM_PDD_WriteChannelValueReg(PeripheralBase, ChannelIdx, Value) ( \
    TPM_CnV_REG(PeripheralBase,(ChannelIdx)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetChannelFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the capture and compare status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: TPM0_STATUS,
 *          TPM1_STATUS, TPM2_STATUS (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = TPM_PDD_GetChannelFlags(deviceID);
 *      @endcode
 */
#define TPM_PDD_GetChannelFlags(PeripheralBase) ( \
    TPM_STATUS_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearChannelFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears channel interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt flag mask. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: TPM0_STATUS,
 *          TPM1_STATUS, TPM2_STATUS (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_ClearChannelFlags(deviceID, 1);
 *      @endcode
 */
#define TPM_PDD_ClearChannelFlags(PeripheralBase, Mask) ( \
    TPM_STATUS_REG(PeripheralBase) = \
     (uint32_t)((uint32_t)(~(uint32_t)(Mask)) & (uint32_t)0xFFU) \
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
 * @remarks The macro accesses the following registers: TPM0_CONF, TPM1_CONF,
 *          TPM2_CONF (depending on the peripheral).
 * @par Example:
 *      @code
 *      TPM_PDD_WriteConfigurationReg(deviceID, 1);
 *      @endcode
 */
#define TPM_PDD_WriteConfigurationReg(PeripheralBase, Value) ( \
    TPM_CONF_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )
#endif  /* #if defined(TPM_PDD_H_) */

/* TPM_PDD.h, eof. */
