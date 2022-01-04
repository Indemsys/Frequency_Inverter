/*
  PDD layer implementation for peripheral type PORT
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(PORT_PDD_H_)
#define PORT_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error PORT PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK10D5) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK10D7) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK10F12) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK10DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK11D5) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK11D5WS) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK12D5) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK20D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK20D5) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK20D7) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK20F12) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK20DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK21D5) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK21D5WS) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK22D5) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK30D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK30D7) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK30DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK40D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK40D7) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK40DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK40X256VMD100) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK50D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK50D7) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK50DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK51D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK51D7) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK51DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK52D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK52DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK53D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK53DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK60D10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK60F12) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK60F15) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK60DZ10) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK60N512VMD100) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MK61F12) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK61F15) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK61F12WS) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK61F15WS) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK70F12) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK70F15) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK70F12WS) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MK70F15WS) /* PORTA, PORTB, PORTC, PORTD, PORTE, PORTF */ && \
      !defined(MCU_MKL02Z4) /* PORTA, PORTB */ && \
      !defined(MCU_MKL04Z4) /* PORTA, PORTB */ && \
      !defined(MCU_MKL05Z4) /* PORTA, PORTB */ && \
      !defined(MCU_MKL14Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL15Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL16Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL24Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL25Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL26Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL34Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL36Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_MKL46Z4) /* PORTA, PORTB, PORTC, PORTD, PORTE */ && \
      !defined(MCU_PCK20L4) /* PORTA, PORTB, PORTC, PORTD, PORTE */
  // Unsupported MCU is active
  #error PORT PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Pin masks */
#define PORT_PDD_PIN_0  0x1U                     /**< Pin 0 mask */
#define PORT_PDD_PIN_1  0x2U                     /**< Pin 1 mask */
#define PORT_PDD_PIN_2  0x4U                     /**< Pin 2 mask */
#define PORT_PDD_PIN_3  0x8U                     /**< Pin 3 mask */
#define PORT_PDD_PIN_4  0x10U                    /**< Pin 4 mask */
#define PORT_PDD_PIN_5  0x20U                    /**< Pin 5 mask */
#define PORT_PDD_PIN_6  0x40U                    /**< Pin 6 mask */
#define PORT_PDD_PIN_7  0x80U                    /**< Pin 7 mask */
#define PORT_PDD_PIN_8  0x100U                   /**< Pin 8 mask */
#define PORT_PDD_PIN_9  0x200U                   /**< Pin 9 mask */
#define PORT_PDD_PIN_10 0x400U                   /**< Pin 10 mask */
#define PORT_PDD_PIN_11 0x800U                   /**< Pin 11 mask */
#define PORT_PDD_PIN_12 0x1000U                  /**< Pin 12 mask */
#define PORT_PDD_PIN_13 0x2000U                  /**< Pin 13 mask */
#define PORT_PDD_PIN_14 0x4000U                  /**< Pin 14 mask */
#define PORT_PDD_PIN_15 0x8000U                  /**< Pin 15 mask */
#define PORT_PDD_PIN_16 0x10000U                 /**< Pin 16 mask */
#define PORT_PDD_PIN_17 0x20000U                 /**< Pin 17 mask */
#define PORT_PDD_PIN_18 0x40000U                 /**< Pin 18 mask */
#define PORT_PDD_PIN_19 0x80000U                 /**< Pin 19 mask */
#define PORT_PDD_PIN_20 0x100000U                /**< Pin 20 mask */
#define PORT_PDD_PIN_21 0x200000U                /**< Pin 21 mask */
#define PORT_PDD_PIN_22 0x400000U                /**< Pin 22 mask */
#define PORT_PDD_PIN_23 0x800000U                /**< Pin 23 mask */
#define PORT_PDD_PIN_24 0x1000000U               /**< Pin 24 mask */
#define PORT_PDD_PIN_25 0x2000000U               /**< Pin 25 mask */
#define PORT_PDD_PIN_26 0x4000000U               /**< Pin 26 mask */
#define PORT_PDD_PIN_27 0x8000000U               /**< Pin 27 mask */
#define PORT_PDD_PIN_28 0x10000000U              /**< Pin 28 mask */
#define PORT_PDD_PIN_29 0x20000000U              /**< Pin 29 mask */
#define PORT_PDD_PIN_30 0x40000000U              /**< Pin 30 mask */
#define PORT_PDD_PIN_31 0x80000000U              /**< Pin 31 mask */

/* Constants for pull type selection */
#define PORT_PDD_PULL_DOWN 0U                    /**< Pull down */
#define PORT_PDD_PULL_UP   0x1U                  /**< Pull up */

/* Constants for pull enabling/disabling */
#define PORT_PDD_PULL_DISABLE 0U                 /**< Pull resistor disabled */
#define PORT_PDD_PULL_ENABLE  0x2U               /**< Pull resistor enabled */

/* Constants for slew rate setting */
#define PORT_PDD_SLEW_RATE_FAST 0U               /**< Fast slew rate */
#define PORT_PDD_SLEW_RATE_SLOW 0x4U             /**< Slow slew rate */

/* Constants for slew rate setting */
#define PORT_PDD_PASSIVE_FILTER_DISABLE 0U       /**< Passive filter disabled */
#define PORT_PDD_PASSIVE_FILTER_ENABLE  0x10U    /**< Passive filter enabled */

/* Constants for open drain setting */
#define PORT_PDD_OPEN_DRAIN_DISABLE 0U           /**< Open drain disabled */
#define PORT_PDD_OPEN_DRAIN_ENABLE  0x20U        /**< Open drain enabled */

/* Constants for drive strength setting */
#define PORT_PDD_DRIVE_STRENGTH_LOW  0U          /**< Low drive strength */
#define PORT_PDD_DRIVE_STRENGTH_HIGH 0x40U       /**< High drive strength */

/* Constants for mux control setting */
#define PORT_PDD_MUX_CONTROL_DISABLE 0U          /**< Mux control disabled */
#define PORT_PDD_MUX_CONTROL_ALT1    0x100U      /**< Pin used with alternate 1 functionality */
#define PORT_PDD_MUX_CONTROL_ALT2    0x200U      /**< Pin used with alternate 2 functionality */
#define PORT_PDD_MUX_CONTROL_ALT3    0x300U      /**< Pin used with alternate 3 functionality */
#define PORT_PDD_MUX_CONTROL_ALT4    0x400U      /**< Pin used with alternate 4 functionality */
#define PORT_PDD_MUX_CONTROL_ALT5    0x500U      /**< Pin used with alternate 5 functionality */
#define PORT_PDD_MUX_CONTROL_ALT6    0x600U      /**< Pin used with alternate 6 functionality */
#define PORT_PDD_MUX_CONTROL_ALT7    0x700U      /**< Pin used with alternate 7 functionality */

/* Constants for pin lock setting */
#define PORT_PDD_PIN_CONTROL_UNLOCK 0U           /**< Pin control unlocked */
#define PORT_PDD_PIN_CONTROL_LOCK   0x8000U      /**< Pin control locked */

/* Constants for interrupt configuration setting */
#define PORT_PDD_INTERRUPT_DMA_DISABLED      0U  /**< Interrupt and DMA disabled */
#define PORT_PDD_DMA_ON_RISING               0x10000U /**< DMA enabled on rising edge */
#define PORT_PDD_DMA_ON_FALLING              0x20000U /**< DMA enabled on falling edge */
#define PORT_PDD_DMA_ON_RISING_FALLING       0x30000U /**< DMA enabled on rising and falling edges */
#define PORT_PDD_INTERRUPT_ON_ZERO           0x80000U /**< Interrupt enabled on low level */
#define PORT_PDD_INTERRUPT_ON_RISING         0x90000U /**< Interrupt enabled on rising edge */
#define PORT_PDD_INTERRUPT_ON_FALLING        0xA0000U /**< Interrupt enabled on falling edge */
#define PORT_PDD_INTERRUPT_ON_RISING_FALLING 0xB0000U /**< Interrupt enabled on rising and falling edges */
#define PORT_PDD_INTERRUPT_ON_ONE            0xC0000U /**< Interrupt enabled on high level */

/* Constants for digital clock source setting */
#define PORT_PDD_BUS_CLOCK 0U                    /**< Bus clock as filter clock source */
#define PORT_PDD_LPO_CLOCK 0x1U                  /**< LPO clock as filter clock source */


/* ----------------------------------------------------------------------------
   -- GetPinPullSelect
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets which pull resistor is selected.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for pull type selection" type. The
 *         value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinPullSelect(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinPullSelect(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_PS_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinPullSelect
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets pull type.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param Type Pull type. This parameter is of "Constants for pull type
 *        selection" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinPullSelect(deviceID, periphID, PORT_PDD_PULL_DOWN);
 *      @endcode
 */
#define PORT_PDD_SetPinPullSelect(PeripheralBase, PinIndex, Type) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_PS_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(Type))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinPullEnable
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets whether pull resistor is enabled.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for pull enabling/disabling" type. The
 *         value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinPullEnable(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinPullEnable(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_PE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinPullEnable
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets pull type.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of pull enable. This parameter is of "Constants
 *        for pull enabling/disabling" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinPullEnable(deviceID, periphID, PORT_PDD_PULL_DISABLE);
 *      @endcode
 */
#define PORT_PDD_SetPinPullEnable(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_PE_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinSlewRate
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets how slew rate is set.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for slew rate setting" type. The value
 *         is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinSlewRate(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinSlewRate(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_SRE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinSlewRate
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets slew rate.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of slew rate. This parameter is of "Constants
 *        for slew rate setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinSlewRate(deviceID, periphID, PORT_PDD_SLEW_RATE_FAST);
 *      @endcode
 */
#define PORT_PDD_SetPinSlewRate(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_SRE_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinPassiveFilter
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets whether passive filter is enabled.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for slew rate setting" type. The value
 *         is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinPassiveFilter(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinPassiveFilter(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_PFE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinPassiveFilter
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets passive filter.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of passive filter. This parameter is of
 *        "Constants for slew rate setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinPassiveFilter(deviceID, periphID,
 *      PORT_PDD_PASSIVE_FILTER_DISABLE);
 *      @endcode
 */
#define PORT_PDD_SetPinPassiveFilter(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_PFE_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinOpenDrain
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets whether open drain is enabled.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for open drain setting" type. The value
 *         is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinOpenDrain(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinOpenDrain(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_ODE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinOpenDrain
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets open drain.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of open drain. This parameter is of "Constants
 *        for open drain setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinOpenDrain(deviceID, periphID,
 *      PORT_PDD_OPEN_DRAIN_DISABLE);
 *      @endcode
 */
#define PORT_PDD_SetPinOpenDrain(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_ODE_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinDriverStrength
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets how drive strength is set.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for drive strength setting" type. The
 *         value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinDriverStrength(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinDriverStrength(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_DSE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinDriveStrength
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets drive strength.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of drive strength. This parameter is of
 *        "Constants for drive strength setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinDriveStrength(deviceID, periphID,
 *      PORT_PDD_DRIVE_STRENGTH_LOW);
 *      @endcode
 */
#define PORT_PDD_SetPinDriveStrength(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_DSE_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinMuxControl
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets how mux control is set.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for mux control setting" type. The
 *         value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinMuxControl(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinMuxControl(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_MUX_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinMuxControl
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets mux control.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of Mux control. This parameter is of "Constants
 *        for mux control setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinMuxControl(deviceID, periphID,
 *      PORT_PDD_MUX_CONTROL_DISABLE);
 *      @endcode
 */
#define PORT_PDD_SetPinMuxControl(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_MUX_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinControlLock
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets how pin lock is set.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for pin lock setting" type. The value
 *         is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinControlLock(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinControlLock(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_LK_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- LockPinControl
   ---------------------------------------------------------------------------- */

/**
 * @brief Locks pin control such as settings of pull, slew rate, passive filter,
 * open drain, mux control and pin lock.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of pin lock. This parameter is of "Constants for
 *        pin lock setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_LockPinControl(deviceID, periphID,
 *      PORT_PDD_PIN_CONTROL_UNLOCK);
 *      @endcode
 */
#define PORT_PDD_LockPinControl(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_LK_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinInterruptConfiguration
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets how interupt configuration is set.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of "Constants for interrupt configuration setting"
 *         type. The value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinInterruptConfiguration(deviceID,
 *      periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinInterruptConfiguration(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_IRQC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetPinInterruptConfiguration
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets interrupt configuration.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @param State Requested state of interrupt configuration. This parameter is of
 *        "Constants for interrupt configuration setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_SetPinInterruptConfiguration(deviceID, periphID,
 *      PORT_PDD_INTERRUPT_DMA_DISABLED);
 *      @endcode
 */
#define PORT_PDD_SetPinInterruptConfiguration(PeripheralBase, PinIndex, State) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) = \
     (uint32_t)(( \
      (uint32_t)(( \
       PORT_PCR_REG(PeripheralBase,(PinIndex))) & (( \
       (uint32_t)(~(uint32_t)PORT_PCR_IRQC_MASK)) & ( \
       (uint32_t)(~(uint32_t)PORT_PCR_ISF_MASK))))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPinInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt flags.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetPinInterruptFlag(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_GetPinInterruptFlag(PeripheralBase, PinIndex) ( \
    (uint32_t)(PORT_PCR_REG(PeripheralBase,(PinIndex)) & PORT_PCR_ISF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt flags.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: PORTA_ISFR, PORTB_ISFR,
 *          PORTC_ISFR, PORTD_ISFR, PORTE_ISFR, PORTF_ISFR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetInterruptFlags(deviceID);
 *      @endcode
 */
#define PORT_PDD_GetInterruptFlags(PeripheralBase) ( \
    PORT_ISFR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearPinInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @param PinIndex Pin index inside the port. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PCR[PinIndex].
 * @par Example:
 *      @code
 *      PORT_PDD_ClearPinInterruptFlag(deviceID, periphID);
 *      @endcode
 */
#define PORT_PDD_ClearPinInterruptFlag(PeripheralBase, PinIndex) ( \
    PORT_PCR_REG(PeripheralBase,(PinIndex)) |= \
     PORT_PCR_ISF_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears interrupt flag bits defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of pins to clearing theirs interrupt flags. Use constants
 *        from group "Pin masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_ISFR, PORTB_ISFR,
 *          PORTC_ISFR, PORTD_ISFR, PORTE_ISFR, PORTF_ISFR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_ClearInterruptFlags(deviceID, PORT_PDD_PIN_0);
 *      @endcode
 */
#define PORT_PDD_ClearInterruptFlags(PeripheralBase, Mask) ( \
    PORT_ISFR_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- SetGlobalPinControl
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets required pin control for required pins of whole port.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of pins in port where the bit 0 corresponds with the pin
 *        which has index 0 within the port and the bit 31 corresponds with the pin
 *        which has index 31 within the port. This parameter is a 32-bit value.
 * @param Value Settings of pull, slew rate, passive filter, open drain, mux
 *        control and pin lock . This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_GPCLR,
 *          PORTA_GPCHR, PORTB_GPCLR, PORTB_GPCHR, PORTC_GPCLR, PORTC_GPCHR, PORTD_GPCLR,
 *          PORTD_GPCHR, PORTE_GPCLR, PORTE_GPCHR, PORTF_GPCLR, PORTF_GPCHR
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_SetGlobalPinControl(deviceID, 1, 1);
 *      @endcode
 */
#define PORT_PDD_SetGlobalPinControl(PeripheralBase, Mask, Value) ( \
    (PORT_GPCLR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)((uint32_t)((uint32_t)(Mask) & 0xFFFFU) << PORT_GPCLR_GPWE_SHIFT)) | ( \
      (uint32_t)(Value)))), \
    (PORT_GPCHR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)((uint32_t)(Mask) & (uint32_t)(~(uint32_t)0xFFFFU))) | ( \
      (uint32_t)(Value)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGlobalPinControlLow
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets required pin control for required pins from low part of port.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of pins in port where the bit 0 corresponds with the pin
 *        which has index 0 within the port and the bit 15 corresponds with the pin
 *        which has index 15 within the port. This parameter is a 16-bit value.
 * @param Value Settings of pull, slew rate, passive filter, open drain, mux
 *        control and pin lock . This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_GPCLR,
 *          PORTB_GPCLR, PORTC_GPCLR, PORTD_GPCLR, PORTE_GPCLR, PORTF_GPCLR (depending on
 *          the peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_SetGlobalPinControlLow(deviceID, 1, 1);
 *      @endcode
 */
#define PORT_PDD_SetGlobalPinControlLow(PeripheralBase, Mask, Value) ( \
    PORT_GPCLR_REG(PeripheralBase) = \
     (uint32_t)((uint32_t)((uint32_t)(Mask) << PORT_GPCLR_GPWE_SHIFT) | (uint32_t)(Value)) \
  )

/* ----------------------------------------------------------------------------
   -- SetGlobalPinControlHigh
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets required pin control for required pins from high part of port.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of pins in port where the bit 0 corresponds with the pin
 *        which has index 16 within the port and the bit 15 corresponds with the pin
 *        which has index 31 within the port. This parameter is a 16-bit value.
 * @param Value Settings of pull, slew rate, passive filter, open drain, mux
 *        control and pin lock . This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_GPCHR,
 *          PORTB_GPCHR, PORTC_GPCHR, PORTD_GPCHR, PORTE_GPCHR, PORTF_GPCHR (depending on
 *          the peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_SetGlobalPinControlHigh(deviceID, 1, 1);
 *      @endcode
 */
#define PORT_PDD_SetGlobalPinControlHigh(PeripheralBase, Mask, Value) ( \
    PORT_GPCHR_REG(PeripheralBase) = \
     (uint32_t)((uint32_t)((uint32_t)(Mask) << PORT_GPCHR_GPWE_SHIFT) | (uint32_t)(Value)) \
  )

/* ----------------------------------------------------------------------------
   -- GetEnableDigitalFilterStatusMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns enable status of digital filter for whole port.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: PORTA_DFER, PORTB_DFER,
 *          PORTC_DFER, PORTD_DFER, PORTE_DFER, PORTF_DFER (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetEnableDigitalFilterStatusMask(deviceID);
 *      @endcode
 */
#define PORT_PDD_GetEnableDigitalFilterStatusMask(PeripheralBase) ( \
    PORT_DFER_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDigitalFilters
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables digital filters.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of pins. Use constants from group "Pin masks". This
 *        parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_DFER, PORTB_DFER,
 *          PORTC_DFER, PORTD_DFER, PORTE_DFER, PORTF_DFER (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_EnableDigitalFilters(deviceID, PORT_PDD_PIN_0);
 *      @endcode
 */
#define PORT_PDD_EnableDigitalFilters(PeripheralBase, Mask) ( \
    PORT_DFER_REG(PeripheralBase) |= \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableDigitalFilters
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables digital filters.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of pins. Use constants from group "Pin masks". This
 *        parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_DFER, PORTB_DFER,
 *          PORTC_DFER, PORTD_DFER, PORTE_DFER, PORTF_DFER (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_DisableDigitalFilters(deviceID, PORT_PDD_PIN_0);
 *      @endcode
 */
#define PORT_PDD_DisableDigitalFilters(PeripheralBase, Mask) ( \
    PORT_DFER_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- GetFilterClockSource
   ---------------------------------------------------------------------------- */

/**
 * @brief Gets how filter clock source is set.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of "Constants for digital clock source setting" type.
 *         The value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: PORTA_DFCR, PORTB_DFCR,
 *          PORTC_DFCR, PORTD_DFCR, PORTE_DFCR, PORTF_DFCR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetFilterClockSource(deviceID);
 *      @endcode
 */
#define PORT_PDD_GetFilterClockSource(PeripheralBase) ( \
    (uint32_t)(PORT_DFCR_REG(PeripheralBase) & PORT_DFCR_CS_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetFilterClockSource
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets filter clock source.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of filter clock source. This parameter is of
 *        "Constants for digital clock source setting" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_DFCR, PORTB_DFCR,
 *          PORTC_DFCR, PORTD_DFCR, PORTE_DFCR, PORTF_DFCR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_SetFilterClockSource(deviceID, PORT_PDD_BUS_CLOCK);
 *      @endcode
 */
#define PORT_PDD_SetFilterClockSource(PeripheralBase, State) ( \
    PORT_DFCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(PORT_DFCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)PORT_DFCR_CS_MASK))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- GetFilterLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns filter length in clock cycles.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: PORTA_DFWR, PORTB_DFWR,
 *          PORTC_DFWR, PORTD_DFWR, PORTE_DFWR, PORTF_DFWR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = PORT_PDD_GetFilterLength(deviceID);
 *      @endcode
 */
#define PORT_PDD_GetFilterLength(PeripheralBase) ( \
    PORT_DFWR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetFilterLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets filter length in clock cycles.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. This parameter is a 5-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: PORTA_DFWR, PORTB_DFWR,
 *          PORTC_DFWR, PORTD_DFWR, PORTE_DFWR, PORTF_DFWR (depending on the
 *          peripheral).
 * @par Example:
 *      @code
 *      PORT_PDD_SetFilterLength(deviceID, 1);
 *      @endcode
 */
#define PORT_PDD_SetFilterLength(PeripheralBase, Value) ( \
    PORT_DFWR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )
#endif  /* #if defined(PORT_PDD_H_) */

/* PORT_PDD.h, eof. */
