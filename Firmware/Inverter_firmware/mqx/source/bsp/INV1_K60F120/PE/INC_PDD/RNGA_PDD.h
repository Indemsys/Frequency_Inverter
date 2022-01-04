/*
  PDD layer implementation for peripheral type RNG
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(RNGA_PDD_H_)
#define RNGA_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error RNG PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10F12) /* RNG */ && \
      !defined(MCU_MK11D5) /* RNG */ && \
      !defined(MCU_MK11D5WS) /* RNG */ && \
      !defined(MCU_MK20F12) /* RNG */ && \
      !defined(MCU_MK21D5) /* RNG */ && \
      !defined(MCU_MK21D5WS) /* RNG */ && \
      !defined(MCU_MK52D10) /* RNG */ && \
      !defined(MCU_MK53D10) /* RNG */ && \
      !defined(MCU_MK60D10) /* RNG */ && \
      !defined(MCU_MK60F12) /* RNG */ && \
      !defined(MCU_MK60F15) /* RNG */ && \
      !defined(MCU_MK61F12) /* RNG */ && \
      !defined(MCU_MK61F15) /* RNG */ && \
      !defined(MCU_MK61F12WS) /* RNG */ && \
      !defined(MCU_MK61F15WS) /* RNG */ && \
      !defined(MCU_MK70F12) /* RNG */ && \
      !defined(MCU_MK70F15) /* RNG */ && \
      !defined(MCU_MK70F12WS) /* RNG */ && \
      !defined(MCU_MK70F15WS) /* RNG */
  // Unsupported MCU is active
  #error RNG PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Constants for mode setting */
#define RNG_PDD_STARTED 0x1U                     /**< RNGA is running (GO bit is 1) */
#define RNG_PDD_STOPPED 0U                       /**< RNGA is stopped (GO bit is 0) */

/* Constants for sleep mode setting */
#define RNG_PDD_START 0x1U                       /**< Put RNGA in sleep mode. */
#define RNG_PDD_STOP  0U                         /**< Wake up RNGA from sleep mode. */

/* Constants for sleep mode setting */
#define RNG_PDD_SLEEPING 0x1U                    /**< RNGA is in sleep mode. */
#define RNG_PDD_RUNNING  0U                      /**< RNGA is running. */

/* Last read status */
#define RNG_PDD_READ_OK         0x1U             /**< Last read was ok. */
#define RNG_PDD_UNDERFLOW_ERROR 0U               /**< Last read caused FIFO underflow error. */

/* Status of RNG. */
#define RNG_PDD_STATUS_SLEEP                      0x10U /**< RNGA is in sleep mode. */
#define RNG_PDD_STATUS_ERROR_INT                  0x8U /**< Error interrupt is pending. */
#define RNG_PDD_STATUS_UNDERFLOW_ERROR            0x4U /**< Output register underflow error. */
#define RNG_PDD_STATUS_LAST_READ_CAUSED_UNDERFLOW 0x2U /**< Last read of output register caused undeflow error. */
#define RNG_PDD_STATUS_SECURITY_VIOLATION         0x1U /**< Security violation detected. */


/* ----------------------------------------------------------------------------
   -- Started
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns whether RNGA was started by asserting GO bit in CR register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Constants for mode setting" for processing
 *         return value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      uint8_t result = RNG_PDD_Started(deviceID);
 *      @endcode
 */
#define RNG_PDD_Started(PeripheralBase) ( \
    (uint8_t)(RNG_CR_REG(PeripheralBase) & RNG_CR_GO_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- StartSleepMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Forces RNGA to sleep mode. Random numbers are not available in this
 * mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_StartSleepMode(deviceID);
 *      @endcode
 */
#define RNG_PDD_StartSleepMode(PeripheralBase) ( \
    RNG_CR_REG(PeripheralBase) |= \
     RNG_CR_SLP_MASK \
  )

/* ----------------------------------------------------------------------------
   -- StopSleepMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Wakes up RNGA from sleep mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_StopSleepMode(deviceID);
 *      @endcode
 */
#define RNG_PDD_StopSleepMode(PeripheralBase) ( \
    RNG_CR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)RNG_CR_SLP_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetSleepMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable or disable RNGA sleep mode.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Sleep mode settings. Use constants from group "Constants for
 *        sleep mode setting". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_SetSleepMode(deviceID, RNG_PDD_START);
 *      @endcode
 */
#define RNG_PDD_SetSleepMode(PeripheralBase, Mode) ( \
    RNG_CR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(RNG_CR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)RNG_CR_SLP_MASK))) | ( \
      (uint32_t)((uint32_t)(Mode) << RNG_CR_SLP_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetSleepMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns whether RNGA is in sleep mode.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Constants for sleep mode setting" for
 *         processing return value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: RNG_SR.
 * @par Example:
 *      @code
 *      uint8_t result = RNG_PDD_GetSleepMode(deviceID);
 *      @endcode
 */
#define RNG_PDD_GetSleepMode(PeripheralBase) ( \
    (uint8_t)((uint32_t)(RNG_SR_REG(PeripheralBase) & RNG_SR_SLP_MASK) >> RNG_SR_SLP_SHIFT) \
  )

/* ----------------------------------------------------------------------------
   -- GetFIFOSize
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the FIFO size.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: RNG_SR.
 * @par Example:
 *      @code
 *      uint8_t result = RNG_PDD_GetFIFOSize(deviceID);
 *      @endcode
 */
#define RNG_PDD_GetFIFOSize(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(RNG_SR_REG(PeripheralBase) & RNG_SR_OREG_SIZE_MASK)) >> ( \
     RNG_SR_OREG_SIZE_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetFIFOLevel
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns FIFO level.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: RNG_SR.
 * @par Example:
 *      @code
 *      uint8_t result = RNG_PDD_GetFIFOLevel(deviceID);
 *      @endcode
 */
#define RNG_PDD_GetFIFOLevel(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(RNG_SR_REG(PeripheralBase) & RNG_SR_OREG_LVL_MASK)) >> ( \
     RNG_SR_OREG_LVL_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- LastReadStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns status of last read number.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Last read status" for processing return
 *         value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: RNG_SR.
 * @par Example:
 *      @code
 *      uint8_t result = RNG_PDD_LastReadStatus(deviceID);
 *      @endcode
 */
#define RNG_PDD_LastReadStatus(PeripheralBase) ( \
    (uint8_t)((uint32_t)(RNG_SR_REG(PeripheralBase) & RNG_SR_LRS_MASK) >> RNG_SR_LRS_SHIFT) \
  )

/* ----------------------------------------------------------------------------
   -- GetStatusRegister
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the RNGA Status Register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Status of RNG." for processing return value.
 * @remarks The macro accesses the following registers: RNG_SR.
 * @par Example:
 *      @code
 *      uint32_t result = RNG_PDD_GetStatusRegister(deviceID);
 *      @endcode
 */
#define RNG_PDD_GetStatusRegister(PeripheralBase) ( \
    RNG_SR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetOutValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the random data generated by the RNGA.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: RNG_OR.
 * @par Example:
 *      @code
 *      uint32_t result = RNG_PDD_GetOutValue(deviceID);
 *      @endcode
 */
#define RNG_PDD_GetOutValue(PeripheralBase) ( \
    RNG_OR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- InsertEntropy
   ---------------------------------------------------------------------------- */

/**
 * @brief Insert additional entropy to RNGA.
 * @param PeripheralBase Peripheral base address.
 * @param EntropyValue New entropy value. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_ER.
 * @par Example:
 *      @code
 *      RNG_PDD_InsertEntropy(deviceID, 1);
 *      @endcode
 */
#define RNG_PDD_InsertEntropy(PeripheralBase, EntropyValue) ( \
    RNG_ER_REG(PeripheralBase) = \
     (uint32_t)(EntropyValue) \
  )

/* ----------------------------------------------------------------------------
   -- ClearInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the RNGA pending interrupt and error flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_ClearInterrupt(deviceID);
 *      @endcode
 */
#define RNG_PDD_ClearInterrupt(PeripheralBase) ( \
    RNG_CR_REG(PeripheralBase) |= \
     RNG_CR_CLRI_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableHigAssuranceMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable High Assurance mode of RNGA. This mode can be disabled only by
 * hardware reset.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_EnableHigAssuranceMode(deviceID);
 *      @endcode
 */
#define RNG_PDD_EnableHigAssuranceMode(PeripheralBase) ( \
    RNG_CR_REG(PeripheralBase) |= \
     RNG_CR_HA_MASK \
  )

/* ----------------------------------------------------------------------------
   -- Start
   ---------------------------------------------------------------------------- */

/**
 * @brief Start RNGA.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_Start(deviceID);
 *      @endcode
 */
#define RNG_PDD_Start(PeripheralBase) ( \
    RNG_CR_REG(PeripheralBase) |= \
     RNG_CR_GO_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_EnableInterrupts(deviceID);
 *      @endcode
 */
#define RNG_PDD_EnableInterrupts(PeripheralBase) ( \
    RNG_CR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)RNG_CR_INTM_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: RNG_CR.
 * @par Example:
 *      @code
 *      RNG_PDD_DisableInterrupts(deviceID);
 *      @endcode
 */
#define RNG_PDD_DisableInterrupts(PeripheralBase) ( \
    RNG_CR_REG(PeripheralBase) |= \
     RNG_CR_INTM_MASK \
  )
#endif  /* #if defined(RNGA_PDD_H_) */

/* RNGA_PDD.h, eof. */
