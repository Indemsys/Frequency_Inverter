/*
  PDD layer implementation for peripheral type GPIO
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(GPIO_PDD_H_)
#define GPIO_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error GPIO PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK10D5) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK10D7) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK10F12) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK10DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK11D5) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK11D5WS) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK12D5) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK20D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK20D5) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK20D7) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK20F12) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK20DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK21D5) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK21D5WS) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK22D5) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK30D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK30D7) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK30DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK40D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK40D7) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK40DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK40X256VMD100) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK50D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK50D7) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK50DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK51D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK51D7) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK51DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK52D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK52DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK53D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK53DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK60D10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK60F12) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK60F15) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK60DZ10) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK60N512VMD100) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MK61F12) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK61F15) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK61F12WS) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK61F15WS) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK70F12) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK70F15) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK70F12WS) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MK70F15WS) /* PTA, PTB, PTC, PTD, PTE, PTF */ && \
      !defined(MCU_MKL02Z4) /* PTA, PTB */ && \
      !defined(MCU_MKL04Z4) /* PTA, PTB */ && \
      !defined(MCU_MKL05Z4) /* PTA, PTB */ && \
      !defined(MCU_MKL14Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL15Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL16Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL24Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL25Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL26Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL34Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL36Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_MKL46Z4) /* PTA, PTB, PTC, PTD, PTE */ && \
      !defined(MCU_PCK20L4) /* PTA, PTB, PTC, PTD, PTE */
  // Unsupported MCU is active
  #error GPIO PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------
   -- GetPortDataInput
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets port data input independently of current direction setting.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: GPIOA_PDIR, GPIOB_PDIR,
 *          GPIOC_PDIR, GPIOD_PDIR, GPIOE_PDIR, GPIOF_PDIR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = GPIO_PDD_GetPortDataInput(deviceID);
 *      @endcode
 */
#define GPIO_PDD_GetPortDataInput(PeripheralBase) ( \
    GPIO_PDIR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetPortDataOutput
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets port data output.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new data output value. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: GPIOA_PDOR, GPIOB_PDOR,
 *          GPIOC_PDOR, GPIOD_PDOR, GPIOE_PDOR, GPIOF_PDOR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      GPIO_PDD_SetPortDataOutput(deviceID, 1);
 *      @endcode
 */
#define GPIO_PDD_SetPortDataOutput(PeripheralBase, Value) ( \
    GPIO_PDOR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetPortDataOutput
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets port data output independently of current direction setting.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: GPIOA_PDOR, GPIOB_PDOR,
 *          GPIOC_PDOR, GPIOD_PDOR, GPIOE_PDOR, GPIOF_PDOR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = GPIO_PDD_GetPortDataOutput(deviceID);
 *      @endcode
 */
#define GPIO_PDD_GetPortDataOutput(PeripheralBase) ( \
    GPIO_PDOR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearPortDataOutputMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears required bits of port data output.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Parameter specifying port pins which should be cleared. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: GPIOA_PCOR, GPIOB_PCOR,
 *          GPIOC_PCOR, GPIOD_PCOR, GPIOE_PCOR, GPIOF_PCOR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      GPIO_PDD_ClearPortDataOutputMask(deviceID, 1);
 *      @endcode
 */
#define GPIO_PDD_ClearPortDataOutputMask(PeripheralBase, Mask) ( \
    GPIO_PCOR_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- SetPortDataOutputMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets required bits of port data output.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Parameter specifying port pins which should be set. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: GPIOA_PSOR, GPIOB_PSOR,
 *          GPIOC_PSOR, GPIOD_PSOR, GPIOE_PSOR, GPIOF_PSOR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      GPIO_PDD_SetPortDataOutputMask(deviceID, 1);
 *      @endcode
 */
#define GPIO_PDD_SetPortDataOutputMask(PeripheralBase, Mask) ( \
    GPIO_PSOR_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- TogglePortDataOutputMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Toggles required bits of port data output.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Parameter specifying port pins which should be inverted. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: GPIOA_PTOR, GPIOB_PTOR,
 *          GPIOC_PTOR, GPIOD_PTOR, GPIOE_PTOR, GPIOF_PTOR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      GPIO_PDD_TogglePortDataOutputMask(deviceID, 1);
 *      @endcode
 */
#define GPIO_PDD_TogglePortDataOutputMask(PeripheralBase, Mask) ( \
    GPIO_PTOR_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- SetPortInputDirectionMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets required pins as input.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Parameter specifying port pins which should be set as input. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: GPIOA_PDDR, GPIOB_PDDR,
 *          GPIOC_PDDR, GPIOD_PDDR, GPIOE_PDDR, GPIOF_PDDR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      GPIO_PDD_SetPortInputDirectionMask(deviceID, 1);
 *      @endcode
 */
#define GPIO_PDD_SetPortInputDirectionMask(PeripheralBase, Mask) ( \
    GPIO_PDDR_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- SetPortOutputDirectionMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets required pins as output.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Parameter specifying port pins which should be set as output.
 *        This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: GPIOA_PDDR, GPIOB_PDDR,
 *          GPIOC_PDDR, GPIOD_PDDR, GPIOE_PDDR, GPIOF_PDDR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      GPIO_PDD_SetPortOutputDirectionMask(deviceID, 1);
 *      @endcode
 */
#define GPIO_PDD_SetPortOutputDirectionMask(PeripheralBase, Mask) ( \
    GPIO_PDDR_REG(PeripheralBase) |= \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- SetPortDirection
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets direction of every pin in the port.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new direction for port pins. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: GPIOA_PDDR, GPIOB_PDDR,
 *          GPIOC_PDDR, GPIOD_PDDR, GPIOE_PDDR, GPIOF_PDDR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      GPIO_PDD_SetPortDirection(deviceID, 1);
 *      @endcode
 */
#define GPIO_PDD_SetPortDirection(PeripheralBase, Value) ( \
    GPIO_PDDR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetPortDirection
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets direction of every pin in the port.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: GPIOA_PDDR, GPIOB_PDDR,
 *          GPIOC_PDDR, GPIOD_PDDR, GPIOE_PDDR, GPIOF_PDDR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = GPIO_PDD_GetPortDirection(deviceID);
 *      @endcode
 */
#define GPIO_PDD_GetPortDirection(PeripheralBase) ( \
    GPIO_PDDR_REG(PeripheralBase) \
  )
#endif  /* #if defined(GPIO_PDD_H_) */

/* GPIO_PDD.h, eof. */
