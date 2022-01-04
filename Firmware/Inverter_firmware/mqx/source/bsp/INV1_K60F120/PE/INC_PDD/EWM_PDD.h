/*
  PDD layer implementation for peripheral type EWM
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(EWM_PDD_H_)
#define EWM_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error EWM PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* EWM */ && \
      !defined(MCU_MK10D5) /* EWM */ && \
      !defined(MCU_MK10D7) /* EWM */ && \
      !defined(MCU_MK10F12) /* EWM */ && \
      !defined(MCU_MK10DZ10) /* EWM */ && \
      !defined(MCU_MK11D5) /* EWM */ && \
      !defined(MCU_MK11D5WS) /* EWM */ && \
      !defined(MCU_MK12D5) /* EWM */ && \
      !defined(MCU_MK20D10) /* EWM */ && \
      !defined(MCU_MK20D5) /* EWM */ && \
      !defined(MCU_MK20D7) /* EWM */ && \
      !defined(MCU_MK20F12) /* EWM */ && \
      !defined(MCU_MK20DZ10) /* EWM */ && \
      !defined(MCU_MK21D5) /* EWM */ && \
      !defined(MCU_MK21D5WS) /* EWM */ && \
      !defined(MCU_MK22D5) /* EWM */ && \
      !defined(MCU_MK30D10) /* EWM */ && \
      !defined(MCU_MK30D7) /* EWM */ && \
      !defined(MCU_MK30DZ10) /* EWM */ && \
      !defined(MCU_MK40D10) /* EWM */ && \
      !defined(MCU_MK40D7) /* EWM */ && \
      !defined(MCU_MK40DZ10) /* EWM */ && \
      !defined(MCU_MK40X256VMD100) /* EWM */ && \
      !defined(MCU_MK50D10) /* EWM */ && \
      !defined(MCU_MK50D7) /* EWM */ && \
      !defined(MCU_MK50DZ10) /* EWM */ && \
      !defined(MCU_MK51D10) /* EWM */ && \
      !defined(MCU_MK51D7) /* EWM */ && \
      !defined(MCU_MK51DZ10) /* EWM */ && \
      !defined(MCU_MK52D10) /* EWM */ && \
      !defined(MCU_MK52DZ10) /* EWM */ && \
      !defined(MCU_MK53D10) /* EWM */ && \
      !defined(MCU_MK53DZ10) /* EWM */ && \
      !defined(MCU_MK60D10) /* EWM */ && \
      !defined(MCU_MK60F12) /* EWM */ && \
      !defined(MCU_MK60F15) /* EWM */ && \
      !defined(MCU_MK60DZ10) /* EWM */ && \
      !defined(MCU_MK60N512VMD100) /* EWM */ && \
      !defined(MCU_MK61F12) /* EWM */ && \
      !defined(MCU_MK61F15) /* EWM */ && \
      !defined(MCU_MK61F12WS) /* EWM */ && \
      !defined(MCU_MK61F15WS) /* EWM */ && \
      !defined(MCU_MK70F12) /* EWM */ && \
      !defined(MCU_MK70F15) /* EWM */ && \
      !defined(MCU_MK70F12WS) /* EWM */ && \
      !defined(MCU_MK70F15WS) /* EWM */ && \
      !defined(MCU_PCK20L4) /* EWM */
  // Unsupported MCU is active
  #error EWM PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Service constants */
#define EWM_PDD_KEY_1 0xB4U                      /**< First key */
#define EWM_PDD_KEY_2 0x2CU                      /**< Second key */


/* ----------------------------------------------------------------------------
   -- WriteControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the control register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: EWM_CTRL.
 * @par Example:
 *      @code
 *      EWM_PDD_WriteControlReg(deviceID, 1);
 *      @endcode
 */
#define EWM_PDD_WriteControlReg(PeripheralBase, Value) ( \
    EWM_CTRL_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetEnableDeviceStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current state of EWM device.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: EWM_CTRL.
 * @par Example:
 *      @code
 *      uint8_t result = EWM_PDD_GetEnableDeviceStatus(deviceID);
 *      @endcode
 */
#define EWM_PDD_GetEnableDeviceStatus(PeripheralBase) ( \
    (uint8_t)(EWM_CTRL_REG(PeripheralBase) & EWM_CTRL_EWMEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- WriteCompareLowReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the compare low register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the compare low register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: EWM_CMPL.
 * @par Example:
 *      @code
 *      EWM_PDD_WriteCompareLowReg(deviceID, 1);
 *      @endcode
 */
#define EWM_PDD_WriteCompareLowReg(PeripheralBase, Value) ( \
    EWM_CMPL_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadCompareLowReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the compare low register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: EWM_CMPL.
 * @par Example:
 *      @code
 *      uint8_t result = EWM_PDD_ReadCompareLowReg(deviceID);
 *      @endcode
 */
#define EWM_PDD_ReadCompareLowReg(PeripheralBase) ( \
    EWM_CMPL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteCompareHighReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the compare high register.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the compare high register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: EWM_CMPH.
 * @par Example:
 *      @code
 *      EWM_PDD_WriteCompareHighReg(deviceID, 1);
 *      @endcode
 */
#define EWM_PDD_WriteCompareHighReg(PeripheralBase, Value) ( \
    EWM_CMPH_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadCompareHighReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the compare high register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: EWM_CMPH.
 * @par Example:
 *      @code
 *      uint8_t result = EWM_PDD_ReadCompareHighReg(deviceID);
 *      @endcode
 */
#define EWM_PDD_ReadCompareHighReg(PeripheralBase) ( \
    EWM_CMPH_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteServiceReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the service register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Service constant. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: EWM_SERV.
 * @par Example:
 *      @code
 *      EWM_PDD_WriteServiceReg(deviceID, 1);
 *      @endcode
 */
#define EWM_PDD_WriteServiceReg(PeripheralBase, Value) ( \
    EWM_SERV_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )
#endif  /* #if defined(EWM_PDD_H_) */

/* EWM_PDD.h, eof. */
