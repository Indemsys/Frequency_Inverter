/*
  PDD layer implementation for peripheral type DRY
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(DRY_PDD_H_)
#define DRY_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error DRY PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK11D5WS) /* DRY */ && \
      !defined(MCU_MK21D5WS) /* DRY */ && \
      !defined(MCU_MK61F12WS) /* DRY */ && \
      !defined(MCU_MK61F15WS) /* DRY */ && \
      !defined(MCU_MK70F12WS) /* DRY */ && \
      !defined(MCU_MK70F15WS) /* DRY */
  // Unsupported MCU is active
  #error DRY PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Secure key word masks */
#define DRY_PDD_SECURE_KEY_WORD_0_MASK 0x1U      /**< Secure key word 0 mask. */
#define DRY_PDD_SECURE_KEY_WORD_1_MASK 0x2U      /**< Secure key word 1 mask. */
#define DRY_PDD_SECURE_KEY_WORD_2_MASK 0x4U      /**< Secure key word 2 mask. */
#define DRY_PDD_SECURE_KEY_WORD_3_MASK 0x8U      /**< Secure key word 3 mask. */
#define DRY_PDD_SECURE_KEY_WORD_4_MASK 0x10U     /**< Secure key word 4 mask. */
#define DRY_PDD_SECURE_KEY_WORD_5_MASK 0x20U     /**< Secure key word 5 mask. */
#define DRY_PDD_SECURE_KEY_WORD_6_MASK 0x40U     /**< Secure key word 6 mask. */
#define DRY_PDD_SECURE_KEY_WORD_7_MASK 0x80U     /**< Secure key word 7 mask. */

/* Tamper flag masks */
#define DRY_PDD_TAMPER_PIN_0_MASK       0x10000U /**< Tamper pin 0 flag mask. */
#define DRY_PDD_TAMPER_PIN_1_MASK       0x20000U /**< Tamper pin 1 flag mask. */
#define DRY_PDD_TAMPER_PIN_2_MASK       0x40000U /**< Tamper pin 2 flag mask. */
#define DRY_PDD_TAMPER_PIN_3_MASK       0x80000U /**< Tamper pin 3 flag mask. */
#define DRY_PDD_TAMPER_PIN_4_MASK       0x100000U /**< Tamper pin 4 flag mask. */
#define DRY_PDD_TAMPER_PIN_5_MASK       0x200000U /**< Tamper pin 5 flag mask. */
#define DRY_PDD_TAMPER_PIN_6_MASK       0x400000U /**< Tamper pin 6 flag mask. */
#define DRY_PDD_TAMPER_PIN_7_MASK       0x800000U /**< Tamper pin 7 flag mask. */
#define DRY_PDD_TEST_MODE_MASK          0x200U   /**< Test mode flag mask. */
#define DRY_PDD_FLASH_SECURITY_MASK     0x100U   /**< Flash security flag mask. */
#define DRY_PDD_SECURITY_TAMPER_MASK    0x80U    /**< Security tamper flag mask. */
#define DRY_PDD_TEMPERATURE_TAMPER_MASK 0x40U    /**< Temperature tamper flag mask. */
#define DRY_PDD_CLOCK_TAMPER_MASK       0x20U    /**< Clock tamper flag mask. */
#define DRY_PDD_VOLTAGE_TAMPER_MASK     0x10U    /**< Voltage tamper flag mask. */
#define DRY_PDD_MONOTONIC_OVERFLOW_MASK 0x8U     /**< Monotonic overflow flag mask. */
#define DRY_PDD_TIME_OVERFLOW_MASK      0x4U     /**< Time overflow flag mask. */
#define DRY_PDD_TAMPER_ACK_MASK         0x2U     /**< Tamper acknowledge flag mask. */
#define DRY_PDD_DRY_ICE_TAMPER_MASK     0x1U     /**< Tamper flag mask. */

/* Lock masks */
#define DRY_PDD_PIN_0_GLITCH_FILTER_LOCK_MASK 0x10000U /**< Pin 0 glitch filter lock mask. */
#define DRY_PDD_PIN_1_GLITCH_FILTER_LOCK_MASK 0x20000U /**< Pin 1 glitch filter lock mask. */
#define DRY_PDD_PIN_2_GLITCH_FILTER_LOCK_MASK 0x40000U /**< Pin 2 glitch filter lock mask. */
#define DRY_PDD_PIN_3_GLITCH_FILTER_LOCK_MASK 0x80000U /**< Pin 3 glitch filter lock mask. */
#define DRY_PDD_PIN_4_GLITCH_FILTER_LOCK_MASK 0x100000U /**< Pin 4 glitch filter lock mask. */
#define DRY_PDD_PIN_5_GLITCH_FILTER_LOCK_MASK 0x200000U /**< Pin 5 glitch filter lock mask. */
#define DRY_PDD_PIN_6_GLITCH_FILTER_LOCK_MASK 0x400000U /**< Pin 6 glitch filter lock mask. */
#define DRY_PDD_PIN_7_GLITCH_FILTER_LOCK_MASK 0x800000U /**< Pin 7 glitch filter lock mask. */
#define DRY_PDD_ACTIVE_TAMPER_1_LOCK_MASK     0x2000U /**< Active tamper register 1 lock mask. */
#define DRY_PDD_ACTIVE_TAMPER_0_LOCK_MASK     0x1000U /**< Active tamper register 0 lock mask. */
#define DRY_PDD_PIN_POLARITY_LOCK_MASK        0x800U /**< Pin polarity register lock mask. */
#define DRY_PDD_PIN_DIRECTION_LOCK_MASK       0x400U /**< Pin direction register lock mask. */
#define DRY_PDD_TAMPER_ENABLE_LOCK_MASK       0x200U /**< Tamper enable register lock mask. */
#define DRY_PDD_TAMPER_SECONDS_LOCK_MASK      0x100U /**< Tamper seconds register lock mask. */
#define DRY_PDD_INTERRUPT_ENABLE_LOCK_MASK    0x80U /**< Interrupt enable register lock mask. */
#define DRY_PDD_LOCK_LOCK_MASK                0x40U /**< Lock register lock mask. */
#define DRY_PDD_STATUS_LOCK_MASK              0x20U /**< Status register lock mask. */
#define DRY_PDD_CONTROL_LOCK_MASK             0x10U /**< Control register lock mask. */
#define DRY_PDD_KEY_READ_LOCK_MASK            0x8U /**< Secure key read register lock mask. */
#define DRY_PDD_KEY_WRITE_LOCK_MASK           0x4U /**< Secure key write register lock mask. */
#define DRY_PDD_KEY_VALID_LOCK_MASK           0x2U /**< Secure key valid register lock mask. */

/* Tamper pin values. */
#define DRY_PDD_LOGIC_ZERO 0U                    /**< Logic zero. */
#define DRY_PDD_LOGIC_ONE  0x1U                  /**< Logic one. */

/* Tamper pin directions. */
#define DRY_PDD_INPUT  0U                        /**< Pin is input. */
#define DRY_PDD_OUTPUT 0x1U                      /**< Pin is output and drives inverse of expected value. */

/* Tamper pin polarity. */
#define DRY_PDD_NOT_INVERTED 0U                  /**< Expected value is not inverted. */
#define DRY_PDD_INVERTED     0x1U                /**< Expected value is inverted. */

/* Tamper pin expected values. */
#define DRY_PDD_LOGIC_ZERO_EXPECTED                                 0U /**< Logic zero. */
#define DRY_PDD_ACTIVE_TAMPER_0_OUTPUT_EXPECTED                     0x10000U /**< Active tamper 0 output. */
#define DRY_PDD_ACTIVE_TAMPER_1_OUTPUT_EXPECTED                     0x20000U /**< Active tamper 1 output. */
#define DRY_PDD_ACTIVE_TAMPER_0_XOR_ACTIVE_TAMPER_1_OUTPUT_EXPECTED 0x30000U /**< Active tamper 0 output XORed with active tamper 1 output. */

/* Glitch filter prescaler clocks. */
#define DRY_PDD_512_HZ_CLOCK   0U                /**< 512 Hz clock. */
#define DRY_PDD_32768_HZ_CLOCK 0x40U             /**< 32.768 kHz clock. */


/* ----------------------------------------------------------------------------
   -- InvalidateSecureKeyWords
   ---------------------------------------------------------------------------- */

/**
 * @brief Invalidates and clears the secure key words specified by the mask
 * parameter. At the same time unlocks the read and write access to the corresponding
 * secure key words.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Secure key word mask. Use constants from group "Secure key word
 *        masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_SKVR.
 * @par Example:
 *      @code
 *      DRY_PDD_InvalidateSecureKeyWords(deviceID,
 *      DRY_PDD_SECURE_KEY_WORD_0_MASK);
 *      @endcode
 */
#define DRY_PDD_InvalidateSecureKeyWords(PeripheralBase, Mask) ( \
    DRY_SKVR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(DRY_SKVR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)DRY_SKVR_SKV_MASK))) | ( \
      (uint32_t)(Mask))) \
  )

/* ----------------------------------------------------------------------------
   -- GetValidSecureKeyWordsMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the mask of valid secure key words. The mask can be tested
 * against predefined masks. See 'Secure key word masks'.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: DRY_SKVR.
 * @par Example:
 *      @code
 *      uint8_t result = DRY_PDD_GetValidSecureKeyWordsMask(deviceID);
 *      @endcode
 */
#define DRY_PDD_GetValidSecureKeyWordsMask(PeripheralBase) ( \
    (uint8_t)(DRY_SKVR_REG(PeripheralBase) & DRY_SKVR_SKV_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- LockSecureKeyWriteTillVBATReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks the secure key words specified by the mask parameter for write
 * until VBAT reset or software reset.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Secure key word mask. Use constants from group "Secure key word
 *        masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_SKWLR.
 * @par Example:
 *      @code
 *      DRY_PDD_LockSecureKeyWriteTillVBATReset(deviceID,
 *      DRY_PDD_SECURE_KEY_WORD_0_MASK);
 *      @endcode
 */
#define DRY_PDD_LockSecureKeyWriteTillVBATReset(PeripheralBase, Mask) ( \
    DRY_SKWLR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- LockSecureKeyReadTillVBATReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks the secure key words specified by the mask parameter for read
 * until VBAT reset or software reset.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Secure key word mask. Use constants from group "Secure key word
 *        masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_SKRLR.
 * @par Example:
 *      @code
 *      DRY_PDD_LockSecureKeyReadTillVBATReset(deviceID,
 *      DRY_PDD_SECURE_KEY_WORD_0_MASK);
 *      @endcode
 */
#define DRY_PDD_LockSecureKeyReadTillVBATReset(PeripheralBase, Mask) ( \
    DRY_SKRLR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- SetPrescalerInitValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Configures the initial value of the prescaler. Can be used only if the
 * peripheral is disabled.
 * @param PeripheralBase Peripheral base address.
 * @param Value Initial value of the prescaler. This parameter is a 15-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_SetPrescalerInitValue(deviceID, 1);
 *      @endcode
 */
#define DRY_PDD_SetPrescalerInitValue(PeripheralBase, Value) ( \
    DRY_CR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(DRY_CR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)DRY_CR_DPR_MASK))) | ( \
      (uint32_t)((uint32_t)(Value) << DRY_CR_DPR_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPrescalerValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the current value of the prescaler in the range from 0 to
 * 32767. The inverse of bit 7 is the 512 Hz prescaler clock output that is used to
 * clock the glitch filters. The inverse of bit 15 is the 1 Hz prescaler clock
 * output that is used to clock the active tamper shift registers.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 15-bit value. The value is cast to "uint16_t".
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      uint16_t result = DRY_PDD_GetPrescalerValue(deviceID);
 *      @endcode
 */
#define DRY_PDD_GetPrescalerValue(PeripheralBase) ( \
    (uint16_t)((uint32_t)(DRY_CR_REG(PeripheralBase) & DRY_CR_DPR_MASK) >> DRY_CR_DPR_SHIFT) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTamperFastSlewRate
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables fast slew rate on tamper pins.
 * @param PeripheralBase Peripheral base address.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableTamperFastSlewRate(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableTamperFastSlewRate(PeripheralBase, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_CR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)DRY_CR_TSRE_MASK)) : ( \
      DRY_CR_REG(PeripheralBase) |= \
       DRY_CR_TSRE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTamperHighDriveStrength
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables high drive strength on tamper pins.
 * @param PeripheralBase Peripheral base address.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableTamperHighDriveStrength(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableTamperHighDriveStrength(PeripheralBase, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_CR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)DRY_CR_TDSE_MASK)) : ( \
      DRY_CR_REG(PeripheralBase) |= \
       DRY_CR_TDSE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTamperPassiveFilter
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables passive input filters on tamper pins.
 * @param PeripheralBase Peripheral base address.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableTamperPassiveFilter(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableTamperPassiveFilter(PeripheralBase, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_CR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)DRY_CR_TPFE_MASK)) : ( \
      DRY_CR_REG(PeripheralBase) |= \
       DRY_CR_TPFE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTamperHysteresis
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables hysteresis on tamper pins.
 * @param PeripheralBase Peripheral base address.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableTamperHysteresis(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableTamperHysteresis(PeripheralBase, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_CR_REG(PeripheralBase) |= \
       DRY_CR_TPFE_MASK) : ( \
      DRY_CR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)DRY_CR_TPFE_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableUpdateMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the update mode in which status flags can be written unless
 * the status register is locked, ignoring the tamper flag.
 * @param PeripheralBase Peripheral base address.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableUpdateMode(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableUpdateMode(PeripheralBase, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_CR_REG(PeripheralBase) |= \
       DRY_CR_UM_MASK) : ( \
      DRY_CR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)DRY_CR_UM_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTamperSystemReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the chip reset generation when the tamper flag is set and the
 * tamper acknowledge flag is clear.
 * @param PeripheralBase Peripheral base address.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableTamperSystemReset(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableTamperSystemReset(PeripheralBase, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_CR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)DRY_CR_TFSR_MASK)) : ( \
      DRY_CR_REG(PeripheralBase) |= \
       DRY_CR_TFSR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the device clock. Must be enabled before enabling a glitch
 * filter or active tamper and should only be disabled after disabling all glitch
 * filters and active tampers.
 * @param PeripheralBase Peripheral base address.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableDevice(PeripheralBase, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_CR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)DRY_CR_DEN_MASK)) : ( \
      DRY_CR_REG(PeripheralBase) |= \
       DRY_CR_DEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ResetDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Resets all the registers except the control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_CR.
 * @par Example:
 *      @code
 *      DRY_PDD_ResetDevice(deviceID);
 *      @endcode
 */
#define DRY_PDD_ResetDevice(PeripheralBase) ( \
    DRY_CR_REG(PeripheralBase) |= \
     DRY_CR_SWR_MASK \
  )

/* ----------------------------------------------------------------------------
   -- GetTamperFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the mask of set tamper flags. The mask can be tested against
 * predefined masks. See 'Tamper flag masks'.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: DRY_SR.
 * @par Example:
 *      @code
 *      uint32_t result = DRY_PDD_GetTamperFlags(deviceID);
 *      @endcode
 */
#define DRY_PDD_GetTamperFlags(PeripheralBase) ( \
    (uint32_t)(( \
     DRY_SR_REG(PeripheralBase)) & ( \
     (uint32_t)(( \
      DRY_SR_TPF_MASK) | (( \
      DRY_SR_TMF_MASK) | (( \
      DRY_SR_FSF_MASK) | (( \
      DRY_SR_STF_MASK) | (( \
      DRY_SR_TTF_MASK) | (( \
      DRY_SR_CTF_MASK) | (( \
      DRY_SR_VTF_MASK) | (( \
      DRY_SR_MOF_MASK) | (( \
      DRY_SR_TOF_MASK) | (( \
      DRY_SR_TAF_MASK) | ( \
      DRY_SR_DTF_MASK))))))))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearTamperFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the tamper flags specified by the mask parameter. A tamper pin
 * flag should be cleared after the tamper pin equals its expected value and the
 * glitch filter output has updated.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Tamper flags mask. Use constants from group "Tamper flag masks".
 *        This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_SR.
 * @par Example:
 *      @code
 *      DRY_PDD_ClearTamperFlags(deviceID, DRY_PDD_TAMPER_PIN_0_MASK);
 *      @endcode
 */
#define DRY_PDD_ClearTamperFlags(PeripheralBase, Mask) ( \
    DRY_SR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       DRY_SR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)(( \
        DRY_SR_TPF_MASK) | (( \
        DRY_SR_TMF_MASK) | (( \
        DRY_SR_FSF_MASK) | (( \
        DRY_SR_STF_MASK) | (( \
        DRY_SR_TTF_MASK) | (( \
        DRY_SR_CTF_MASK) | (( \
        DRY_SR_VTF_MASK) | (( \
        DRY_SR_MOF_MASK) | (( \
        DRY_SR_TOF_MASK) | (( \
        DRY_SR_TAF_MASK) | ( \
        DRY_SR_DTF_MASK))))))))))))))) | ( \
      (uint32_t)(Mask))) \
  )

/* ----------------------------------------------------------------------------
   -- LockWriteTillVBATReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks the registers specified by the mask parameter for write until
 * VBAT reset.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Lock mask. Use constants from group "Lock masks". This parameter
 *        is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_LR.
 * @par Example:
 *      @code
 *      DRY_PDD_LockWriteTillVBATReset(deviceID,
 *      DRY_PDD_PIN_0_GLITCH_FILTER_LOCK_MASK);
 *      @endcode
 */
#define DRY_PDD_LockWriteTillVBATReset(PeripheralBase, Mask) ( \
    DRY_LR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the interrupts specified by the mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask (except TAMPER_ACK_MASK). Use constants from group
 *        "Tamper flag masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_IER.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableInterrupts(deviceID, DRY_PDD_TAMPER_PIN_0_MASK);
 *      @endcode
 */
#define DRY_PDD_EnableInterrupts(PeripheralBase, Mask) ( \
    DRY_IER_REG(PeripheralBase) |= \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the interrupts specified by the mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask (except DRY_ICE_TAMPER_ACK_MASK). Use constants
 *        from group "Tamper flag masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_IER.
 * @par Example:
 *      @code
 *      DRY_PDD_DisableInterrupts(deviceID, DRY_PDD_TAMPER_PIN_0_MASK);
 *      @endcode
 */
#define DRY_PDD_DisableInterrupts(PeripheralBase, Mask) ( \
    DRY_IER_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- GetTamperSeconds
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the time in seconds (from the RTC Time Seconds Register) at
 * which the tamper flag was set.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: DRY_TSR.
 * @par Example:
 *      @code
 *      uint32_t result = DRY_PDD_GetTamperSeconds(deviceID);
 *      @endcode
 */
#define DRY_PDD_GetTamperSeconds(PeripheralBase) ( \
    DRY_TSR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTamperDetection
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the tamper detection flags specified by the mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Tamper flags mask (except TAMPER_ACK_MASK and
 *        DRY_ICE_TAMPER_MASK). Use constants from group "Tamper flag masks". This parameter is 32
 *        bits wide.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_TER.
 * @par Example:
 *      @code
 *      DRY_PDD_EnableTamperDetection(deviceID, DRY_PDD_TAMPER_PIN_0_MASK,
 *      PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableTamperDetection(PeripheralBase, Mask, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_TER_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)(Mask))) : ( \
      DRY_TER_REG(PeripheralBase) |= \
       (uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- SetTamperPinOutput
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets tamper pin output data.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Value Tamper pin output value. This parameter is of "Tamper pin
 *        values." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_PDR.
 * @par Example:
 *      @code
 *      DRY_PDD_SetTamperPinOutput(deviceID, periphID, DRY_PDD_LOGIC_ZERO);
 *      @endcode
 */
#define DRY_PDD_SetTamperPinOutput(PeripheralBase, Index, Value) ( \
    ((Value) == DRY_PDD_LOGIC_ZERO) ? ( \
      DRY_PDR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)((uint32_t)((uint32_t)0x1U << (Index)) << DRY_PDR_TPOD_SHIFT))) : ( \
      DRY_PDR_REG(PeripheralBase) |= \
       (uint32_t)((uint32_t)((uint32_t)0x1U << (Index)) << DRY_PDR_TPOD_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- SetTamperPinDirection
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets tamper pin direction.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Direction Tamper pin direction. This parameter is of "Tamper pin
 *        directions." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_PDR.
 * @par Example:
 *      @code
 *      DRY_PDD_SetTamperPinDirection(deviceID, periphID, DRY_PDD_INPUT);
 *      @endcode
 */
#define DRY_PDD_SetTamperPinDirection(PeripheralBase, Index, Direction) ( \
    ((Direction) == DRY_PDD_INPUT) ? ( \
      DRY_PDR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)((uint32_t)0x1U << (Index)))) : ( \
      DRY_PDR_REG(PeripheralBase) |= \
       (uint32_t)((uint32_t)0x1U << (Index))) \
  )

/* ----------------------------------------------------------------------------
   -- SetTamperPinInput
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets tamper pin input data.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Value Tamper pin input value. This parameter is of "Tamper pin
 *        values." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_PPR.
 * @par Example:
 *      @code
 *      DRY_PDD_SetTamperPinInput(deviceID, periphID, DRY_PDD_LOGIC_ZERO);
 *      @endcode
 */
#define DRY_PDD_SetTamperPinInput(PeripheralBase, Index, Value) ( \
    ((Value) == DRY_PDD_LOGIC_ZERO) ? ( \
      DRY_PPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)((uint32_t)((uint32_t)0x1U << (Index)) << DRY_PPR_TPID_SHIFT))) : ( \
      DRY_PPR_REG(PeripheralBase) |= \
       (uint32_t)((uint32_t)((uint32_t)0x1U << (Index)) << DRY_PPR_TPID_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- SetTamperPinPolarity
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets tamper pin polarity.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Polarity Tamper pin polarity. This parameter is of "Tamper pin
 *        polarity." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_PPR.
 * @par Example:
 *      @code
 *      DRY_PDD_SetTamperPinPolarity(deviceID, periphID, DRY_PDD_NOT_INVERTED);
 *      @endcode
 */
#define DRY_PDD_SetTamperPinPolarity(PeripheralBase, Index, Polarity) ( \
    ((Polarity) == DRY_PDD_NOT_INVERTED) ? ( \
      DRY_PPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)((uint32_t)0x1U << (Index)))) : ( \
      DRY_PPR_REG(PeripheralBase) |= \
       (uint32_t)((uint32_t)0x1U << (Index))) \
  )

/* ----------------------------------------------------------------------------
   -- SetActiveTamperPolynomial
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the polynomial of the active tamper shift register. When set to
 * zero, the active tamper shift register is disabled. Once enabled, the active
 * tamper shift register updates once a second using the prescaler 1 Hz clock.
 * @param PeripheralBase Peripheral base address.
 * @param Index Active tamper index. This parameter is of index type.
 * @param Polynomial Polynomial of the active tamper shift register. This
 *        parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ATR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_SetActiveTamperPolynomial(deviceID, periphID, 1);
 *      @endcode
 */
#define DRY_PDD_SetActiveTamperPolynomial(PeripheralBase, Index, Polynomial) ( \
    DRY_ATR_REG(PeripheralBase,(Index)) = \
     (uint32_t)(( \
      (uint32_t)(DRY_ATR_REG(PeripheralBase,(Index)) & (uint32_t)(~(uint32_t)DRY_ATR_ATP_MASK))) | ( \
      (uint32_t)((uint32_t)(Polynomial) << DRY_ATR_ATP_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetActiveTamperShiftValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the initial value of the active tamper shift register.
 * @param PeripheralBase Peripheral base address.
 * @param Index Active tamper index. This parameter is of index type.
 * @param Value Initial value of the active tamper shift register. This
 *        parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ATR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_SetActiveTamperShiftValue(deviceID, periphID, 1);
 *      @endcode
 */
#define DRY_PDD_SetActiveTamperShiftValue(PeripheralBase, Index, Value) ( \
    DRY_ATR_REG(PeripheralBase,(Index)) = \
     (uint32_t)(( \
      (uint32_t)(DRY_ATR_REG(PeripheralBase,(Index)) & (uint32_t)(~(uint32_t)DRY_ATR_ATSR_MASK))) | ( \
      (uint32_t)(Value))) \
  )

/* ----------------------------------------------------------------------------
   -- GetActiveTamperShiftValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the active tamper shift register in the range
 * from 0x0000 to 0xFFFF.
 * @param PeripheralBase Peripheral base address.
 * @param Index Active tamper index. This parameter is of index type.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: ATR[Index].
 * @par Example:
 *      @code
 *      uint16_t result = DRY_PDD_GetActiveTamperShiftValue(deviceID, periphID);
 *      @endcode
 */
#define DRY_PDD_GetActiveTamperShiftValue(PeripheralBase, Index) ( \
    (uint16_t)(DRY_ATR_REG(PeripheralBase,(Index)) & DRY_ATR_ATSR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableTamperPinPull
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the pull resistor on the tamper pin.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PGFR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_EnableTamperPinPull(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableTamperPinPull(PeripheralBase, Index, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_PGFR_REG(PeripheralBase,(Index)) &= \
       (uint32_t)(~(uint32_t)DRY_PGFR_TPE_MASK)) : ( \
      DRY_PGFR_REG(PeripheralBase,(Index)) |= \
       DRY_PGFR_TPE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetTamperPinExpectedValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the expected value on the input pin.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Value Expected value. This parameter is of "Tamper pin expected
 *        values." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PGFR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_SetTamperPinExpectedValue(deviceID, periphID,
 *      DRY_PDD_LOGIC_ZERO_EXPECTED);
 *      @endcode
 */
#define DRY_PDD_SetTamperPinExpectedValue(PeripheralBase, Index, Value) ( \
    DRY_PGFR_REG(PeripheralBase,(Index)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       DRY_PGFR_REG(PeripheralBase,(Index))) & ( \
       (uint32_t)(~(uint32_t)DRY_PGFR_TPEX_MASK)))) | ( \
      (uint32_t)(Value))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableGlitchFilter
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables tamper pin glitch filter.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Enable Enable flag. This parameter is of "Global enumeration used for
 *        specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PGFR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_EnableGlitchFilter(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define DRY_PDD_EnableGlitchFilter(PeripheralBase, Index, Enable) ( \
    ((Enable) == PDD_DISABLE) ? ( \
      DRY_PGFR_REG(PeripheralBase,(Index)) &= \
       (uint32_t)(~(uint32_t)DRY_PGFR_GFE_MASK)) : ( \
      DRY_PGFR_REG(PeripheralBase,(Index)) |= \
       DRY_PGFR_GFE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetGlitchFilterPrescaler
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the glitch filter prescaler.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Clock Prescaler clock. This parameter is of "Glitch filter prescaler
 *        clocks." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PGFR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_SetGlitchFilterPrescaler(deviceID, periphID,
 *      DRY_PDD_512_HZ_CLOCK);
 *      @endcode
 */
#define DRY_PDD_SetGlitchFilterPrescaler(PeripheralBase, Index, Clock) ( \
    DRY_PGFR_REG(PeripheralBase,(Index)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       DRY_PGFR_REG(PeripheralBase,(Index))) & ( \
       (uint32_t)(~(uint32_t)DRY_PGFR_GFP_MASK)))) | ( \
      (uint32_t)(Clock))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGlitchFilterWidth
   ---------------------------------------------------------------------------- */

/**
 * @brief Configures the number of clock edges the input must remain stable for
 * to be passed through the glitch filter for tamper pin. The number of clock
 * edges is (GFW + 1) * 2 supporting a configuration of between 2 and 128 clock
 * edges. Do not change the glitch filter width when the glitch filter is enabled.
 * @param PeripheralBase Peripheral base address.
 * @param Index Tamper pin index. This parameter is of index type.
 * @param Width Glitch filter width. This parameter is a 6-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PGFR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_SetGlitchFilterWidth(deviceID, periphID, 1);
 *      @endcode
 */
#define DRY_PDD_SetGlitchFilterWidth(PeripheralBase, Index, Width) ( \
    DRY_PGFR_REG(PeripheralBase,(Index)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       DRY_PGFR_REG(PeripheralBase,(Index))) & ( \
       (uint32_t)(~(uint32_t)DRY_PGFR_GFW_MASK)))) | ( \
      (uint32_t)(Width))) \
  )

/* ----------------------------------------------------------------------------
   -- LockWriteTillChipReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks the registers specified by the mask parameter for write until a
 * chip reset.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Locks mask (except KEY_SELECT_LOCK_MASK). Use constants from
 *        group "Lock masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_WAC.
 * @par Example:
 *      @code
 *      DRY_PDD_LockWriteTillChipReset(deviceID,
 *      DRY_PDD_PIN_0_GLITCH_FILTER_LOCK_MASK);
 *      @endcode
 */
#define DRY_PDD_LockWriteTillChipReset(PeripheralBase, Mask) ( \
    DRY_WAC_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- LockReadTillChipReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks the registers specified by the mask parameter for read until a
 * chip reset.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Locks mask (except KEY_SELECT_LOCK_MASK). Use constants from
 *        group "Lock masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_RAC.
 * @par Example:
 *      @code
 *      DRY_PDD_LockReadTillChipReset(deviceID,
 *      DRY_PDD_PIN_0_GLITCH_FILTER_LOCK_MASK);
 *      @endcode
 */
#define DRY_PDD_LockReadTillChipReset(PeripheralBase, Mask) ( \
    DRY_RAC_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- SetSecureKeyWord
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the value of the specified secure key word.
 * @param PeripheralBase Peripheral base address.
 * @param Index Secure key word index. This parameter is of index type.
 * @param Value 32-bit wide secure key word value. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SKR[Index].
 * @par Example:
 *      @code
 *      DRY_PDD_SetSecureKeyWord(deviceID, periphID, 1);
 *      @endcode
 */
#define DRY_PDD_SetSecureKeyWord(PeripheralBase, Index, Value) ( \
    DRY_SKR_REG(PeripheralBase,(Index)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetSecureKeyWord
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the 32-bit wide value of the specified secure key word.
 * @param PeripheralBase Peripheral base address.
 * @param Index Secure key word index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SKR[Index].
 * @par Example:
 *      @code
 *      uint32_t result = DRY_PDD_GetSecureKeyWord(deviceID, periphID);
 *      @endcode
 */
#define DRY_PDD_GetSecureKeyWord(PeripheralBase, Index) ( \
    DRY_SKR_REG(PeripheralBase,(Index)) \
  )

/* ----------------------------------------------------------------------------
   -- LockSecureKeyWriteTillChipReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks the secure key words specified by the mask parameter for write
 * until chip reset.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Secure key words mask. Use constants from group "Secure key word
 *        masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_SWAC.
 * @par Example:
 *      @code
 *      DRY_PDD_LockSecureKeyWriteTillChipReset(deviceID,
 *      DRY_PDD_SECURE_KEY_WORD_0_MASK);
 *      @endcode
 */
#define DRY_PDD_LockSecureKeyWriteTillChipReset(PeripheralBase, Mask) ( \
    DRY_SWAC_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- LockSecureKeyReadTillChipReset
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks the secure key words specified by the mask parameter for read
 * until chip reset.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Secure key words mask. Use constants from group "Secure key word
 *        masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: DRY_SRAC.
 * @par Example:
 *      @code
 *      DRY_PDD_LockSecureKeyReadTillChipReset(deviceID,
 *      DRY_PDD_SECURE_KEY_WORD_0_MASK);
 *      @endcode
 */
#define DRY_PDD_LockSecureKeyReadTillChipReset(PeripheralBase, Mask) ( \
    DRY_SRAC_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )
#endif  /* #if defined(DRY_PDD_H_) */

/* DRY_PDD.h, eof. */
