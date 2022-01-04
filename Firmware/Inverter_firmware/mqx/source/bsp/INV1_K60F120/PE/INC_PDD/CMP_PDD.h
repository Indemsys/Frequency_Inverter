/*
  PDD layer implementation for peripheral type CMP
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(CMP_PDD_H_)
#define CMP_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error CMP PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK10D5) /* CMP0, CMP1 */ && \
      !defined(MCU_MK10D7) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK10F12) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK10DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK11D5) /* CMP0, CMP1 */ && \
      !defined(MCU_MK11D5WS) /* CMP0, CMP1 */ && \
      !defined(MCU_MK12D5) /* CMP0, CMP1 */ && \
      !defined(MCU_MK20D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK20D5) /* CMP0, CMP1 */ && \
      !defined(MCU_MK20D7) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK20F12) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK20DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK21D5) /* CMP0, CMP1 */ && \
      !defined(MCU_MK21D5WS) /* CMP0, CMP1 */ && \
      !defined(MCU_MK22D5) /* CMP0, CMP1 */ && \
      !defined(MCU_MK30D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK30D7) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK30DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK40D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK40D7) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK40DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK40X256VMD100) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK50D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK50D7) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK50DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK51D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK51D7) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK51DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK52D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK52DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK53D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK53DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK60D10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK60F12) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK60F15) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK60DZ10) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK60N512VMD100) /* CMP0, CMP1, CMP2 */ && \
      !defined(MCU_MK61F12) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK61F15) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK61F12WS) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK61F15WS) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK70F12) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK70F15) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK70F12WS) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MK70F15WS) /* CMP0, CMP1, CMP2, CMP3 */ && \
      !defined(MCU_MKL02Z4) /* CMP0 */ && \
      !defined(MCU_MKL04Z4) /* CMP0 */ && \
      !defined(MCU_MKL05Z4) /* CMP0 */ && \
      !defined(MCU_MKL14Z4) /* CMP0 */ && \
      !defined(MCU_MKL15Z4) /* CMP0 */ && \
      !defined(MCU_MKL16Z4) /* CMP0 */ && \
      !defined(MCU_MKL24Z4) /* CMP0 */ && \
      !defined(MCU_MKL25Z4) /* CMP0 */ && \
      !defined(MCU_MKL26Z4) /* CMP0 */ && \
      !defined(MCU_MKL34Z4) /* CMP0 */ && \
      !defined(MCU_MKL36Z4) /* CMP0 */ && \
      !defined(MCU_MKL46Z4) /* CMP0 */ && \
      !defined(MCU_PCK20L4) /* CMP0, CMP1 */
  // Unsupported MCU is active
  #error CMP PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Interrupts' mask */
#define CMP_PDD_FALLING_EDGE_INTERRUPT CMP_SCR_IEF_MASK /**< Falling edge interrupt enable mask. */
#define CMP_PDD_RISING_EDGE_INTERRUPT  CMP_SCR_IER_MASK /**< Rising edge interrupt enable mask. */

/* Interrupts' flags */
#define CMP_PDD_FALLING_EDGE_FLAG CMP_SCR_CFF_MASK /**< Falling edge flag. */
#define CMP_PDD_RISING_EDGE_FLAG  CMP_SCR_CFR_MASK /**< Rising edge flag. */

/* Comparator modes */
#define CMP_PDD_FALLING_EDGE_MODE CMP_SCR_IEF_MASK /**< Falling edge detection mode. */
#define CMP_PDD_RISING_EDGE_MODE  CMP_SCR_IER_MASK /**< Rising edge detection mode. */
#define CMP_PDD_BOTH_EDGES_MODE   0x6U           /**< Both falling and rising edge detection mode. */

/* Deprecated: Hysteresis constants. Please use HYSTERESIS_LEVEL_X constants. */
#define CMP_PDD_HYSTERESIS_DISABLE 0U            /**< Hysteresis not used. */
#define CMP_PDD_HYSTERESIS_20MV    0x1U          /**< 20 mV hysteresis used. */
#define CMP_PDD_HYSTERESIS_40MV    0x2U          /**< 40 mV hysteresis used. */
#define CMP_PDD_HYSTERESIS_60MV    0x3U          /**< 60 mV hysteresis used. */

/* Hysteresis constants. For exact value representation see peripheral device
   documentation. */
#define CMP_PDD_HYSTERESIS_LEVEL_0 0U            /**< Level 0 hysteresis used. */
#define CMP_PDD_HYSTERESIS_LEVEL_1 0x1U          /**< Level 1 hysteresis used. */
#define CMP_PDD_HYSTERESIS_LEVEL_2 0x2U          /**< Level 2 hysteresis used. */
#define CMP_PDD_HYSTERESIS_LEVEL_3 0x3U          /**< Level 3 hysteresis used. */

/* Filter sample count constants. */
#define CMP_PDD_FILTER_DISABLED  0U              /**< Filter disabled. */
#define CMP_PDD_FILTER_1_SAMPLE  0x10U           /**< 1 consecutive sample must agree. */
#define CMP_PDD_FILTER_2_SAMPLES 0x20U           /**< 2 consecutive samples must agree. */
#define CMP_PDD_FILTER_3_SAMPLES 0x30U           /**< 3 consecutive samples must agree. */
#define CMP_PDD_FILTER_4_SAMPLES 0x40U           /**< 4 consecutive samples must agree. */
#define CMP_PDD_FILTER_5_SAMPLES 0x50U           /**< 5 consecutive samples must agree. */
#define CMP_PDD_FILTER_6_SAMPLES 0x60U           /**< 6 consecutive samples must agree. */
#define CMP_PDD_FILTER_7_SAMPLES 0x70U           /**< 7 consecutive samples must agree. */

/* Comparator output filtration constant */
#define CMP_PDD_FILTERED_OUTPUT   0U             /**< Filtered comparator output. */
#define CMP_PDD_UNFILTERED_OUTPUT CMP_CR1_COS_MASK /**< Filter on comparator output is bypassed. */

/* Comparator output inversion constant. */
#define CMP_PDD_NOT_INVERTED_OUTPUT 0U           /**< Comparator output is not inverted. */
#define CMP_PDD_INVERTED_OUTPUT     CMP_CR1_INV_MASK /**< Comparator output is inverted. */

/* Comparator power mode constant. */
#define CMP_PDD_LOW_POWER_MODE  0U               /**< Low power/low speed mode. */
#define CMP_PDD_HIGH_SPEED_MODE CMP_CR1_PMODE_MASK /**< High power/high speed mode. */

#if ((defined(MCU_MK10DZ10)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60N512VMD100)))
/* Comparator's interrupt stop mode constant. */
  #define CMP_PDD_LEVEL_SENSITIVE_MODE 0U          /**< Comparator is level sensitive in stop mode */
  #define CMP_PDD_EDGE_SENSITIVE_MODE  CMP_SCR_SMELB_MASK /**< Comparator is edge sensitive in stop mode */

#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) || (defined(MCU_PCK20L4)) */
/* Comparator's interrupt stop mode constant. */
  #define CMP_PDD_LEVEL_SENSITIVE_MODE 0U          /**< Comparator is level sensitive in stop mode */
  #define CMP_PDD_EDGE_SENSITIVE_MODE  0x1U        /**< Comparator is edge sensitive in stop mode */

#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) || (defined(MCU_PCK20L4)) */
/* Analog comparator 6-bit DAC supply voltage reference source constant. */
#define CMP_PDD_V_REF_INPUT_1 0U                 /**< Vin1 reference. */
#define CMP_PDD_V_REF_INPUT_2 CMP_DACCR_VRSEL_MASK /**< Vin2 reference */

/* Negative input constant. */
#define CMP_PDD_NEGATIVE_INPUT_0 0U              /**< Negative input 0. */
#define CMP_PDD_NEGATIVE_INPUT_1 0x1U            /**< Negative input 1. */
#define CMP_PDD_NEGATIVE_INPUT_2 0x2U            /**< Negative input 2. */
#define CMP_PDD_NEGATIVE_INPUT_3 0x3U            /**< Negative input 3. */
#define CMP_PDD_NEGATIVE_INPUT_4 0x4U            /**< Negative input 4. */
#define CMP_PDD_NEGATIVE_INPUT_5 0x5U            /**< Negative input 5. */
#define CMP_PDD_NEGATIVE_INPUT_6 0x6U            /**< Negative input 6. */
#define CMP_PDD_NEGATIVE_INPUT_7 0x7U            /**< Negative input 7. */

/* Positive input constant. */
#define CMP_PDD_POSITIVE_INPUT_0 0U              /**< Positive input 0. */
#define CMP_PDD_POSITIVE_INPUT_1 0x8U            /**< Positive input 1. */
#define CMP_PDD_POSITIVE_INPUT_2 0x10U           /**< Positive input 2. */
#define CMP_PDD_POSITIVE_INPUT_3 0x18U           /**< Positive input 3. */
#define CMP_PDD_POSITIVE_INPUT_4 0x20U           /**< Positive input 4. */
#define CMP_PDD_POSITIVE_INPUT_5 0x28U           /**< Positive input 5. */
#define CMP_PDD_POSITIVE_INPUT_6 0x30U           /**< Positive input 6. */
#define CMP_PDD_POSITIVE_INPUT_7 0x38U           /**< Positive input 7. */


/* ----------------------------------------------------------------------------
   -- EnableDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the comparator's device.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of comparator. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE
 *        and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnableDevice(PeripheralBase, State) ( \
    CMP_CR1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR1_EN_MASK))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetDeviceEnabled
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current state of comparator's enable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = CMP_PDD_GetDeviceEnabled(deviceID);
 *      @endcode
 */
#define CMP_PDD_GetDeviceEnabled(PeripheralBase) ( \
    (uint8_t)(CMP_CR1_REG(PeripheralBase) & CMP_CR1_EN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables interrupts defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupts to enable. Use constants from group
 *        "Interrupts' mask". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableInterrupts(deviceID, CMP_PDD_FALLING_EDGE_INTERRUPT);
 *      @endcode
 */
#define CMP_PDD_EnableInterrupts(PeripheralBase, Mask) ( \
    CMP_SCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_SCR_REG(PeripheralBase) | (uint8_t)(Mask))) & (( \
      (uint8_t)(~(uint8_t)CMP_SCR_CFF_MASK)) & ( \
      (uint8_t)(~(uint8_t)CMP_SCR_CFR_MASK)))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables interrupts defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupts to disable. Use constants from group
 *        "Interrupts' mask". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_DisableInterrupts(deviceID, CMP_PDD_FALLING_EDGE_INTERRUPT);
 *      @endcode
 */
#define CMP_PDD_DisableInterrupts(PeripheralBase, Mask) ( \
    CMP_SCR_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)(Mask))) & (( \
      (uint8_t)(~(uint8_t)CMP_SCR_CFF_MASK)) & ( \
      (uint8_t)(~(uint8_t)CMP_SCR_CFR_MASK)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets all interrupts with value according to mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupts to enable - rest will be disabled. Use
 *        constants from group "Interrupts' mask". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetInterruptMask(deviceID, CMP_PDD_FALLING_EDGE_INTERRUPT);
 *      @endcode
 */
#define CMP_PDD_SetInterruptMask(PeripheralBase, Mask) ( \
    CMP_SCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       CMP_SCR_REG(PeripheralBase)) & (( \
       (uint8_t)(~(uint8_t)(CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK))) & (( \
       (uint8_t)(~(uint8_t)CMP_SCR_CFF_MASK)) & ( \
       (uint8_t)(~(uint8_t)CMP_SCR_CFR_MASK)))))) | ( \
      (uint8_t)(Mask))) \
  )

/* ----------------------------------------------------------------------------
   -- GetInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupts mask.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Interrupts' mask" for processing return
 *         value.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = CMP_PDD_GetInterruptMask(deviceID);
 *      @endcode
 */
#define CMP_PDD_GetInterruptMask(PeripheralBase) ( \
    (uint8_t)(CMP_SCR_REG(PeripheralBase) & (uint8_t)(CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- GetInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt flag bits.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Interrupts' mask" for processing return
 *         value.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = CMP_PDD_GetInterruptFlags(deviceID);
 *      @endcode
 */
#define CMP_PDD_GetInterruptFlags(PeripheralBase) ( \
    (uint8_t)(CMP_SCR_REG(PeripheralBase) & (uint8_t)(CMP_SCR_CFF_MASK | CMP_SCR_CFR_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- ClearInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears interrupt flag bits defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupt's flags to clear. Use constants from group
 *        "Interrupts' mask". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_ClearInterruptFlags(deviceID, CMP_PDD_FALLING_EDGE_INTERRUPT);
 *      @endcode
 */
#define CMP_PDD_ClearInterruptFlags(PeripheralBase, Mask) ( \
    CMP_SCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       CMP_SCR_REG(PeripheralBase)) & ( \
       (uint8_t)(~(uint8_t)(CMP_SCR_CFF_MASK | CMP_SCR_CFR_MASK))))) | ( \
      (uint8_t)(Mask))) \
  )

/* ----------------------------------------------------------------------------
   -- SetHysteresis
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets comparator hard block hysteresis control bits.
 * @param PeripheralBase Peripheral base address.
 * @param Hysteresis Hysteresis value. This parameter is of "Hysteresis
 *        constants. For exact value representation see peripheral device documentation."
 *        type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR0, CMP1_CR0,
 *          CMP2_CR0, CMP3_CR0 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetHysteresis(deviceID, CMP_PDD_HYSTERESIS_LEVEL_0);
 *      @endcode
 */
#define CMP_PDD_SetHysteresis(PeripheralBase, Hysteresis) ( \
    CMP_CR0_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR0_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR0_HYSTCTR_MASK))) | ( \
      (uint8_t)(Hysteresis))) \
  )

/* ----------------------------------------------------------------------------
   -- SetFilterCount
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets filter sample count bits.
 * @param PeripheralBase Peripheral base address.
 * @param Count Filter sample count. This parameter is of "Filter sample count
 *        constants." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR0, CMP1_CR0,
 *          CMP2_CR0, CMP3_CR0 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetFilterCount(deviceID, CMP_PDD_FILTER_DISABLED);
 *      @endcode
 */
#define CMP_PDD_SetFilterCount(PeripheralBase, Count) ( \
    CMP_CR0_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR0_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR0_FILTER_CNT_MASK))) | ( \
      (uint8_t)(Count))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableOutputPin
   ---------------------------------------------------------------------------- */

/**
 * @brief Comparator output pin enable.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if output pin will be enabled or disabled.
 *        This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableOutputPin(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnableOutputPin(PeripheralBase, State) ( \
    CMP_CR1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR1_OPE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << CMP_CR1_OPE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetComparatorOutputFilter
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects comparator output filtration.
 * @param PeripheralBase Peripheral base address.
 * @param Output Parameter specifying if output will be filtered or not. This
 *        parameter is of "Comparator output filtration constant" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetComparatorOutputFilter(deviceID, CMP_PDD_FILTERED_OUTPUT);
 *      @endcode
 */
#define CMP_PDD_SetComparatorOutputFilter(PeripheralBase, Output) ( \
    CMP_CR1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR1_COS_MASK))) | ( \
      (uint8_t)(Output))) \
  )

/* ----------------------------------------------------------------------------
   -- SetComparatorOutputInversion
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects comparator output inversion.
 * @param PeripheralBase Peripheral base address.
 * @param Output Parameter specifying if output will be inverted or not. This
 *        parameter is of "Comparator output inversion constant." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetComparatorOutputInversion(deviceID,
 *      CMP_PDD_NOT_INVERTED_OUTPUT);
 *      @endcode
 */
#define CMP_PDD_SetComparatorOutputInversion(PeripheralBase, Output) ( \
    CMP_CR1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR1_INV_MASK))) | ( \
      (uint8_t)(Output))) \
  )

/* ----------------------------------------------------------------------------
   -- SetPowerMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects comparator power mode.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Power mode. This parameter is of "Comparator power mode
 *        constant." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetPowerMode(deviceID, CMP_PDD_LOW_POWER_MODE);
 *      @endcode
 */
#define CMP_PDD_SetPowerMode(PeripheralBase, Mode) ( \
    CMP_CR1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR1_PMODE_MASK))) | ( \
      (uint8_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableWindowing
   ---------------------------------------------------------------------------- */

/**
 * @brief Windowing enable.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if windowing will be enabled or disabled.
 *        This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)"
 *        type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableWindowing(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnableWindowing(PeripheralBase, State) ( \
    CMP_CR1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR1_WE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << CMP_CR1_WE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableSampling
   ---------------------------------------------------------------------------- */

/**
 * @brief Sampling enable.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if sampling will be enabled or disabled.
 *        This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)"
 *        type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_CR1, CMP1_CR1,
 *          CMP2_CR1, CMP3_CR1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableSampling(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnableSampling(PeripheralBase, State) ( \
    CMP_CR1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_CR1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_CR1_SE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << CMP_CR1_SE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetFilterPeriod
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets filter sample period.
 * @param PeripheralBase Peripheral base address.
 * @param Period Filter sample period value. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_FPR, CMP1_FPR,
 *          CMP2_FPR, CMP3_FPR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetFilterPeriod(deviceID, 1);
 *      @endcode
 */
#define CMP_PDD_SetFilterPeriod(PeripheralBase, Period) ( \
    CMP_FPR_REG(PeripheralBase) = \
     (uint8_t)(Period) \
  )

/* ----------------------------------------------------------------------------
   -- GetComparatorOutput
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current comparator output state.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = CMP_PDD_GetComparatorOutput(deviceID);
 *      @endcode
 */
#define CMP_PDD_GetComparatorOutput(PeripheralBase) ( \
    (uint8_t)(CMP_SCR_REG(PeripheralBase) & CMP_SCR_COUT_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDmaRequest
   ---------------------------------------------------------------------------- */

/**
 * @brief DMA enable control.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if DMA requests will be enabled or
 *        disabled. This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR, CMP3_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableDmaRequest(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnableDmaRequest(PeripheralBase, State) ( \
    CMP_SCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       CMP_SCR_REG(PeripheralBase)) & (( \
       (uint8_t)(~(uint8_t)CMP_SCR_DMAEN_MASK)) & (( \
       (uint8_t)(~(uint8_t)CMP_SCR_CFF_MASK)) & ( \
       (uint8_t)(~(uint8_t)CMP_SCR_CFR_MASK)))))) | ( \
      (uint8_t)((uint8_t)(State) << CMP_SCR_DMAEN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetComparatorMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets comparator's interrupt mode.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Comparator interrupt mode. Use constants from group "Comparator
 *        modes". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      CMP_PDD_SetComparatorMode(deviceID, CMP_PDD_FALLING_EDGE_MODE);
 *      @endcode
 */
#define CMP_PDD_SetComparatorMode(PeripheralBase, Mode) ( \
    CMP_PDD_SetInterruptMask(PeripheralBase, (uint8_t)(Mode)) \
  )

/* ----------------------------------------------------------------------------
   -- GetCompareStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns last detected edge on comparator's output.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Interrupts' mask" for processing return
 *         value.
 * @par Example:
 *      @code
 *      uint8_t result = CMP_PDD_GetCompareStatus(deviceID);
 *      @endcode
 */
#define CMP_PDD_GetCompareStatus(PeripheralBase) ( \
    CMP_PDD_GetInterruptFlags(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetVoltage
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets DAC6b output voltage.
 * @param PeripheralBase Peripheral base address.
 * @param Value Output voltage value. This parameter is a 6-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_DACCR, CMP1_DACCR,
 *          CMP2_DACCR, CMP3_DACCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetVoltage(deviceID, 1);
 *      @endcode
 */
#define CMP_PDD_SetVoltage(PeripheralBase, Value) ( \
    CMP_DACCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_DACCR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_DACCR_VOSEL_MASK))) | ( \
      (uint8_t)(Value))) \
  )

/* ----------------------------------------------------------------------------
   -- SetReference
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects analog comparator 6-bit DAC supply voltage reference source.
 * @param PeripheralBase Peripheral base address.
 * @param Reference Supply voltage source. This parameter is of "Analog
 *        comparator 6-bit DAC supply voltage reference source constant." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_DACCR, CMP1_DACCR,
 *          CMP2_DACCR, CMP3_DACCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetReference(deviceID, CMP_PDD_V_REF_INPUT_1);
 *      @endcode
 */
#define CMP_PDD_SetReference(PeripheralBase, Reference) ( \
    CMP_DACCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_DACCR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_DACCR_VRSEL_MASK))) | ( \
      (uint8_t)(Reference))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDac
   ---------------------------------------------------------------------------- */

/**
 * @brief Analog comparator 6-bit DAC enable control.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if DAC will be enabled or disabled. This
 *        parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_DACCR, CMP1_DACCR,
 *          CMP2_DACCR, CMP3_DACCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableDac(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnableDac(PeripheralBase, State) ( \
    CMP_DACCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_DACCR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_DACCR_DACEN_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << CMP_DACCR_DACEN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetDacEnabled
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current state of analog comparator 6-bit DAC enable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: CMP0_DACCR, CMP1_DACCR,
 *          CMP2_DACCR, CMP3_DACCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = CMP_PDD_GetDacEnabled(deviceID);
 *      @endcode
 */
#define CMP_PDD_GetDacEnabled(PeripheralBase) ( \
    (uint8_t)(CMP_DACCR_REG(PeripheralBase) & CMP_DACCR_DACEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetNegativeInput
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets negative comparator input.
 * @param PeripheralBase Peripheral base address.
 * @param NegativeInput Negative input number. This parameter is of "Negative
 *        input constant." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_MUXCR, CMP1_MUXCR,
 *          CMP2_MUXCR, CMP3_MUXCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetNegativeInput(deviceID, CMP_PDD_NEGATIVE_INPUT_0);
 *      @endcode
 */
#define CMP_PDD_SetNegativeInput(PeripheralBase, NegativeInput) ( \
    CMP_MUXCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_MUXCR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_MUXCR_MSEL_MASK))) | ( \
      (uint8_t)(NegativeInput))) \
  )

/* ----------------------------------------------------------------------------
   -- SetPositiveInput
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets positive comparator input.
 * @param PeripheralBase Peripheral base address.
 * @param PositiveInput Positive input number. This parameter is of "Positive
 *        input constant." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_MUXCR, CMP1_MUXCR,
 *          CMP2_MUXCR, CMP3_MUXCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetPositiveInput(deviceID, CMP_PDD_POSITIVE_INPUT_0);
 *      @endcode
 */
#define CMP_PDD_SetPositiveInput(PeripheralBase, PositiveInput) ( \
    CMP_MUXCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_MUXCR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_MUXCR_PSEL_MASK))) | ( \
      (uint8_t)(PositiveInput))) \
  )

/* ----------------------------------------------------------------------------
   -- SetStopMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects stop mode edge/level interrupt control.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Parameter specifying if edge or level stop mode will be used.
 *        This parameter is of "Comparator's interrupt stop mode constant." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_SCR, CMP1_SCR,
 *          CMP2_SCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_SetStopMode(deviceID, CMP_PDD_LEVEL_SENSITIVE_MODE);
 *      @endcode
 */
#define CMP_PDD_SetStopMode(PeripheralBase, Mode) ( \
    CMP_SCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       CMP_SCR_REG(PeripheralBase)) & (( \
       (uint8_t)(~(uint8_t)CMP_SCR_SMELB_MASK)) & (( \
       (uint8_t)(~(uint8_t)CMP_SCR_CFF_MASK)) & ( \
       (uint8_t)(~(uint8_t)CMP_SCR_CFR_MASK)))))) | ( \
      (uint8_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableNegativeInput
   ---------------------------------------------------------------------------- */

/**
 * @brief Negative MUX enable control.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if negative input will be enabled or
 *        disabled. This parameter is of "Global enumeration used for specifying
 *        general enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_MUXCR, CMP1_MUXCR,
 *          CMP2_MUXCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnableNegativeInput(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnableNegativeInput(PeripheralBase, State) ( \
    CMP_MUXCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_MUXCR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_MUXCR_MEN_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << CMP_MUXCR_MEN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnablePositiveInput
   ---------------------------------------------------------------------------- */

/**
 * @brief Postive MUX enable control.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if positive input will be enabled or
 *        disabled. This parameter is of "Global enumeration used for specifying
 *        general enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CMP0_MUXCR, CMP1_MUXCR,
 *          CMP2_MUXCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      CMP_PDD_EnablePositiveInput(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define CMP_PDD_EnablePositiveInput(PeripheralBase, State) ( \
    CMP_MUXCR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(CMP_MUXCR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)CMP_MUXCR_PEN_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << CMP_MUXCR_PEN_SHIFT))) \
  )
#endif  /* #if defined(CMP_PDD_H_) */

/* CMP_PDD.h, eof. */
