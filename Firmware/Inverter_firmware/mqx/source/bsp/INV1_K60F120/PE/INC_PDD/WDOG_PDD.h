/*
  PDD layer implementation for peripheral type WDOG
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(WDOG_PDD_H_)
#define WDOG_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error WDOG PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* WDOG */ && \
      !defined(MCU_MK10D5) /* WDOG */ && \
      !defined(MCU_MK10D7) /* WDOG */ && \
      !defined(MCU_MK10F12) /* WDOG */ && \
      !defined(MCU_MK10DZ10) /* WDOG */ && \
      !defined(MCU_MK11D5) /* WDOG */ && \
      !defined(MCU_MK11D5WS) /* WDOG */ && \
      !defined(MCU_MK12D5) /* WDOG */ && \
      !defined(MCU_MK20D10) /* WDOG */ && \
      !defined(MCU_MK20D5) /* WDOG */ && \
      !defined(MCU_MK20D7) /* WDOG */ && \
      !defined(MCU_MK20F12) /* WDOG */ && \
      !defined(MCU_MK20DZ10) /* WDOG */ && \
      !defined(MCU_MK21D5) /* WDOG */ && \
      !defined(MCU_MK21D5WS) /* WDOG */ && \
      !defined(MCU_MK22D5) /* WDOG */ && \
      !defined(MCU_MK30D10) /* WDOG */ && \
      !defined(MCU_MK30D7) /* WDOG */ && \
      !defined(MCU_MK30DZ10) /* WDOG */ && \
      !defined(MCU_MK40D10) /* WDOG */ && \
      !defined(MCU_MK40D7) /* WDOG */ && \
      !defined(MCU_MK40DZ10) /* WDOG */ && \
      !defined(MCU_MK40X256VMD100) /* WDOG */ && \
      !defined(MCU_MK50D10) /* WDOG */ && \
      !defined(MCU_MK50D7) /* WDOG */ && \
      !defined(MCU_MK50DZ10) /* WDOG */ && \
      !defined(MCU_MK51D10) /* WDOG */ && \
      !defined(MCU_MK51D7) /* WDOG */ && \
      !defined(MCU_MK51DZ10) /* WDOG */ && \
      !defined(MCU_MK52D10) /* WDOG */ && \
      !defined(MCU_MK52DZ10) /* WDOG */ && \
      !defined(MCU_MK53D10) /* WDOG */ && \
      !defined(MCU_MK53DZ10) /* WDOG */ && \
      !defined(MCU_MK60D10) /* WDOG */ && \
      !defined(MCU_MK60F12) /* WDOG */ && \
      !defined(MCU_MK60F15) /* WDOG */ && \
      !defined(MCU_MK60DZ10) /* WDOG */ && \
      !defined(MCU_MK60N512VMD100) /* WDOG */ && \
      !defined(MCU_MK61F12) /* WDOG */ && \
      !defined(MCU_MK61F15) /* WDOG */ && \
      !defined(MCU_MK61F12WS) /* WDOG */ && \
      !defined(MCU_MK61F15WS) /* WDOG */ && \
      !defined(MCU_MK70F12) /* WDOG */ && \
      !defined(MCU_MK70F15) /* WDOG */ && \
      !defined(MCU_MK70F12WS) /* WDOG */ && \
      !defined(MCU_MK70F15WS) /* WDOG */ && \
      !defined(MCU_PCK20L4) /* WDOG */
  // Unsupported MCU is active
  #error WDOG PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Windowing mode constants */
#define WDOG_PDD_WINDOWING_DISABLED 0U           /**< Disabled */
#define WDOG_PDD_WINDOWING_ENABLED  WDOG_STCTRLH_WINEN_MASK /**< Enabled */

/* Clock source constants. */
#define WDOG_PDD_SOURCE_DEDICATED 0U             /**< Dedicated */
#define WDOG_PDD_SOURCE_ALTERNATE WDOG_STCTRLH_CLKSRC_MASK /**< Alternate */

/* Refresh constants */
#define WDOG_PDD_KEY_1 0xA602U                   /**< First key */
#define WDOG_PDD_KEY_2 0xB480U                   /**< Second key */

/* Unlock constants */
#define WDOG_PDD_UNLOCK_1 0xC520U                /**< First key */
#define WDOG_PDD_UNLOCK_2 0xD928U                /**< Second key */

/* Prescaler constants */
#define WDOG_PDD_DIVIDE_1 0U                     /**< 1 */
#define WDOG_PDD_DIVIDE_2 0x1U                   /**< 2 */
#define WDOG_PDD_DIVIDE_3 0x2U                   /**< 3 */
#define WDOG_PDD_DIVIDE_4 0x3U                   /**< 4 */
#define WDOG_PDD_DIVIDE_5 0x4U                   /**< 5 */
#define WDOG_PDD_DIVIDE_6 0x5U                   /**< 6 */
#define WDOG_PDD_DIVIDE_7 0x6U                   /**< 7 */
#define WDOG_PDD_DIVIDE_8 0x7U                   /**< 8 */


/* ----------------------------------------------------------------------------
   -- GetAllowUpdateStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns non-zero if watchdog registers can be unlocked.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      uint16_t result = WDOG_PDD_GetAllowUpdateStatus(deviceID);
 *      @endcode
 */
#define WDOG_PDD_GetAllowUpdateStatus(PeripheralBase) ( \
    (uint16_t)(WDOG_STCTRLH_REG(PeripheralBase) & WDOG_STCTRLH_ALLOWUPDATE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableUpdate
   ---------------------------------------------------------------------------- */

/**
 * @brief No further updates allowed to WDOG write once registers.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      WDOG_PDD_DisableUpdate(deviceID);
 *      @endcode
 */
#define WDOG_PDD_DisableUpdate(PeripheralBase) ( \
    WDOG_STCTRLH_REG(PeripheralBase) &= \
     (uint16_t)(~(uint16_t)WDOG_STCTRLH_ALLOWUPDATE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetWindowingMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Configures the windowing mode operation.
 * @param PeripheralBase Peripheral base address.
 * @param Mode New value of the mode. Use constants from group "Windowing mode
 *        constants". This parameter is 16 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      WDOG_PDD_SetWindowingMode(deviceID, WDOG_PDD_WINDOWING_DISABLED);
 *      @endcode
 */
#define WDOG_PDD_SetWindowingMode(PeripheralBase, Mode) ( \
    WDOG_STCTRLH_REG(PeripheralBase) = \
     (uint16_t)(( \
      (uint16_t)(( \
       WDOG_STCTRLH_REG(PeripheralBase)) & ( \
       (uint16_t)(~(uint16_t)WDOG_STCTRLH_WINEN_MASK)))) | ( \
      (uint16_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- GetInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt mask.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      uint16_t result = WDOG_PDD_GetInterruptMask(deviceID);
 *      @endcode
 */
#define WDOG_PDD_GetInterruptMask(PeripheralBase) ( \
    (uint16_t)(WDOG_STCTRLH_REG(PeripheralBase) & WDOG_STCTRLH_IRQRSTEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the WDOG interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      WDOG_PDD_EnableInterrupt(deviceID);
 *      @endcode
 */
#define WDOG_PDD_EnableInterrupt(PeripheralBase) ( \
    WDOG_STCTRLH_REG(PeripheralBase) |= \
     WDOG_STCTRLH_IRQRSTEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the WDOG interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      WDOG_PDD_DisableInterrupt(deviceID);
 *      @endcode
 */
#define WDOG_PDD_DisableInterrupt(PeripheralBase) ( \
    WDOG_STCTRLH_REG(PeripheralBase) &= \
     (uint16_t)(~(uint16_t)WDOG_STCTRLH_IRQRSTEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SelectClockSource
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects clock source.
 * @param PeripheralBase Peripheral base address.
 * @param Source New value of the source. Use constants from group "Clock source
 *        constants.". This parameter is 16 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      WDOG_PDD_SelectClockSource(deviceID, WDOG_PDD_SOURCE_DEDICATED);
 *      @endcode
 */
#define WDOG_PDD_SelectClockSource(PeripheralBase, Source) ( \
    WDOG_STCTRLH_REG(PeripheralBase) = \
     (uint16_t)(( \
      (uint16_t)(( \
       WDOG_STCTRLH_REG(PeripheralBase)) & ( \
       (uint16_t)(~(uint16_t)WDOG_STCTRLH_CLKSRC_MASK)))) | ( \
      (uint16_t)(Source))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the WDOG device.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of WDOG device. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE
 *        and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      WDOG_PDD_EnableDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define WDOG_PDD_EnableDevice(PeripheralBase, State) ( \
    WDOG_STCTRLH_REG(PeripheralBase) = \
     (uint16_t)(( \
      (uint16_t)(( \
       WDOG_STCTRLH_REG(PeripheralBase)) & ( \
       (uint16_t)(~(uint16_t)WDOG_STCTRLH_WDOGEN_MASK)))) | ( \
      (uint16_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetEnableDeviceStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current state of WDOG device.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      uint16_t result = WDOG_PDD_GetEnableDeviceStatus(deviceID);
 *      @endcode
 */
#define WDOG_PDD_GetEnableDeviceStatus(PeripheralBase) ( \
    (uint16_t)(WDOG_STCTRLH_REG(PeripheralBase) & WDOG_STCTRLH_WDOGEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- WriteControlHighReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the watchdog status and control register high.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the watchdog status and control register high.
 *        This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      WDOG_PDD_WriteControlHighReg(deviceID, 1);
 *      @endcode
 */
#define WDOG_PDD_WriteControlHighReg(PeripheralBase, Value) ( \
    WDOG_STCTRLH_REG(PeripheralBase) = \
     (uint16_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadControlHighReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the watchdog status and control register high.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: WDOG_STCTRLH.
 * @par Example:
 *      @code
 *      uint16_t result = WDOG_PDD_ReadControlHighReg(deviceID);
 *      @endcode
 */
#define WDOG_PDD_ReadControlHighReg(PeripheralBase) ( \
    WDOG_STCTRLH_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt flag bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: WDOG_STCTRLL.
 * @par Example:
 *      @code
 *      uint16_t result = WDOG_PDD_GetInterruptFlag(deviceID);
 *      @endcode
 */
#define WDOG_PDD_GetInterruptFlag(PeripheralBase) ( \
    (uint16_t)(WDOG_STCTRLL_REG(PeripheralBase) & WDOG_STCTRLL_INTFLG_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_STCTRLL.
 * @par Example:
 *      @code
 *      WDOG_PDD_ClearInterruptFlag(deviceID);
 *      @endcode
 */
#define WDOG_PDD_ClearInterruptFlag(PeripheralBase) ( \
    WDOG_STCTRLL_REG(PeripheralBase) = \
     (uint16_t)(WDOG_STCTRLL_INTFLG_MASK | 0x1U) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTimeOutHighReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the watchdog time-out value register high.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the watchdog time-out value register high. This
 *        parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_TOVALH.
 * @par Example:
 *      @code
 *      WDOG_PDD_WriteTimeOutHighReg(deviceID, 1);
 *      @endcode
 */
#define WDOG_PDD_WriteTimeOutHighReg(PeripheralBase, Value) ( \
    WDOG_TOVALH_REG(PeripheralBase) = \
     (uint16_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTimeOutLowReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the watchdog time-out value register low.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the watchdog time-out value register low. This
 *        parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_TOVALL.
 * @par Example:
 *      @code
 *      WDOG_PDD_WriteTimeOutLowReg(deviceID, 1);
 *      @endcode
 */
#define WDOG_PDD_WriteTimeOutLowReg(PeripheralBase, Value) ( \
    WDOG_TOVALL_REG(PeripheralBase) = \
     (uint16_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteWindowHighReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the watchdog window register high.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the watchdog window register high. This parameter
 *        is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_WINH.
 * @par Example:
 *      @code
 *      WDOG_PDD_WriteWindowHighReg(deviceID, 1);
 *      @endcode
 */
#define WDOG_PDD_WriteWindowHighReg(PeripheralBase, Value) ( \
    WDOG_WINH_REG(PeripheralBase) = \
     (uint16_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteWindowLowReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the watchdog window register low.
 * @param PeripheralBase Peripheral base address.
 * @param Value New content of the watchdog window register low. This parameter
 *        is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_WINL.
 * @par Example:
 *      @code
 *      WDOG_PDD_WriteWindowLowReg(deviceID, 1);
 *      @endcode
 */
#define WDOG_PDD_WriteWindowLowReg(PeripheralBase, Value) ( \
    WDOG_WINL_REG(PeripheralBase) = \
     (uint16_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteRefreshReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the watchdog refresh register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Service constant. Use constants from group "Refresh constants".
 *        This parameter is 16 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_REFRESH.
 * @par Example:
 *      @code
 *      WDOG_PDD_WriteRefreshReg(deviceID, WDOG_PDD_KEY_1);
 *      @endcode
 */
#define WDOG_PDD_WriteRefreshReg(PeripheralBase, Value) ( \
    WDOG_REFRESH_REG(PeripheralBase) = \
     (uint16_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteUnlockReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the watchdog unlock register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Unlock constant. Use constants from group "Unlock constants".
 *        This parameter is 16 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_UNLOCK.
 * @par Example:
 *      @code
 *      WDOG_PDD_WriteUnlockReg(deviceID, WDOG_PDD_UNLOCK_1);
 *      @endcode
 */
#define WDOG_PDD_WriteUnlockReg(PeripheralBase, Value) ( \
    WDOG_UNLOCK_REG(PeripheralBase) = \
     (uint16_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- SetPrescaler
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets prescale value.
 * @param PeripheralBase Peripheral base address.
 * @param Prescaler New value of the prescaler. Use constants from group
 *        "Prescaler constants". This parameter is 3 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: WDOG_PRESC.
 * @par Example:
 *      @code
 *      WDOG_PDD_SetPrescaler(deviceID, WDOG_PDD_DIVIDE_1);
 *      @endcode
 */
#define WDOG_PDD_SetPrescaler(PeripheralBase, Prescaler) ( \
    WDOG_PRESC_REG(PeripheralBase) = \
     (uint16_t)((uint16_t)(Prescaler) << WDOG_PRESC_PRESCVAL_SHIFT) \
  )
#endif  /* #if defined(WDOG_PDD_H_) */

/* WDOG_PDD.h, eof. */
