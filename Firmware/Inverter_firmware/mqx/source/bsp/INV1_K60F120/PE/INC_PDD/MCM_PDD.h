/*
  PDD layer implementation for peripheral type MCM
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(MCM_PDD_H_)
#define MCM_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error MCM PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MKL02Z4) /* MCM */ && \
      !defined(MCU_MKL04Z4) /* MCM */ && \
      !defined(MCU_MKL05Z4) /* MCM */ && \
      !defined(MCU_MKL14Z4) /* MCM */ && \
      !defined(MCU_MKL15Z4) /* MCM */ && \
      !defined(MCU_MKL16Z4) /* MCM */ && \
      !defined(MCU_MKL24Z4) /* MCM */ && \
      !defined(MCU_MKL25Z4) /* MCM */ && \
      !defined(MCU_MKL26Z4) /* MCM */ && \
      !defined(MCU_MKL34Z4) /* MCM */ && \
      !defined(MCU_MKL36Z4) /* MCM */ && \
      !defined(MCU_MKL46Z4) /* MCM */
  // Unsupported MCU is active
  #error MCM PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Crossbar master arbitration type */
#define MCM_PDD_FIXED_PRIORITY 0U                /**< Fixed-priority arbitration for the crossbar masters */
#define MCM_PDD_ROUND_ROBIN    0x200U            /**< Round-robin arbitration for the crossbar masters */


/* ----------------------------------------------------------------------------
   -- DisableStallingFlashController
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable stalling flash controller.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_DisableStallingFlashController(deviceID);
 *      @endcode
 */
#define MCM_PDD_DisableStallingFlashController(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)MCM_PLACR_ESFC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableStallingFlashController
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable stalling flash controller.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_EnableStallingFlashController(deviceID);
 *      @endcode
 */
#define MCM_PDD_EnableStallingFlashController(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) |= \
     MCM_PLACR_ESFC_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableFlashControllerSpeculation
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable flash controller speculation.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_DisableFlashControllerSpeculation(deviceID);
 *      @endcode
 */
#define MCM_PDD_DisableFlashControllerSpeculation(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)MCM_PLACR_DFCS_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableFlashControllerSpeculation
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable flash controller speculation.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_EnableFlashControllerSpeculation(deviceID);
 *      @endcode
 */
#define MCM_PDD_EnableFlashControllerSpeculation(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) |= \
     MCM_PLACR_DFCS_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableFlashDataSpeculation
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable flash data speculation.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_DisableFlashDataSpeculation(deviceID);
 *      @endcode
 */
#define MCM_PDD_DisableFlashDataSpeculation(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)MCM_PLACR_EFDS_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableFlashDataSpeculation
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable flash data speculation.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_EnableFlashDataSpeculation(deviceID);
 *      @endcode
 */
#define MCM_PDD_EnableFlashDataSpeculation(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) |= \
     MCM_PLACR_EFDS_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableFlashControllerCache
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable flash controllerCache.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_EnableFlashControllerCache(deviceID);
 *      @endcode
 */
#define MCM_PDD_EnableFlashControllerCache(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)MCM_PLACR_DFCC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableFlashControllerCache
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable flash controllerCache.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_DisableFlashControllerCache(deviceID);
 *      @endcode
 */
#define MCM_PDD_DisableFlashControllerCache(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) |= \
     MCM_PLACR_DFCC_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableFlashControllerInstructionCaching
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable flash controller instruction caching.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_EnableFlashControllerInstructionCaching(deviceID);
 *      @endcode
 */
#define MCM_PDD_EnableFlashControllerInstructionCaching(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)MCM_PLACR_DFCIC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableFlashControllerInstructionCaching
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable flash controller instruction caching.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_DisableFlashControllerInstructionCaching(deviceID);
 *      @endcode
 */
#define MCM_PDD_DisableFlashControllerInstructionCaching(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) |= \
     MCM_PLACR_DFCIC_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableFlashControllerDataCaching
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable flash controller data caching.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_EnableFlashControllerDataCaching(deviceID);
 *      @endcode
 */
#define MCM_PDD_EnableFlashControllerDataCaching(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)MCM_PLACR_DFCDA_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableFlashControllerDataCaching
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable flash controller data caching.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_DisableFlashControllerDataCaching(deviceID);
 *      @endcode
 */
#define MCM_PDD_DisableFlashControllerDataCaching(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) |= \
     MCM_PLACR_DFCDA_MASK \
  )

/* ----------------------------------------------------------------------------
   -- InvalidateFlashCache
   ---------------------------------------------------------------------------- */

/**
 * @brief Invalidates flash cache.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_InvalidateFlashCache(deviceID);
 *      @endcode
 */
#define MCM_PDD_InvalidateFlashCache(PeripheralBase) ( \
    MCM_PLACR_REG(PeripheralBase) |= \
     MCM_PLACR_CFCC_MASK \
  )

/* ----------------------------------------------------------------------------
   -- SetCrossbarMastersArbitrationType
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects arbitration type for crossbar masters.
 * @param PeripheralBase Peripheral base address.
 * @param State Crossbar masters arbitration type. This parameter is of
 *        "Crossbar master arbitration type" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: MCM_PLACR.
 * @par Example:
 *      @code
 *      MCM_PDD_SetCrossbarMastersArbitrationType(deviceID,
 *      MCM_PDD_FIXED_PRIORITY);
 *      @endcode
 */
#define MCM_PDD_SetCrossbarMastersArbitrationType(PeripheralBase, State) ( \
    MCM_PLACR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(MCM_PLACR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)MCM_PLACR_ARB_MASK))) | ( \
      (uint32_t)(State))) \
  )
#endif  /* #if defined(MCM_PDD_H_) */

/* MCM_PDD.h, eof. */
