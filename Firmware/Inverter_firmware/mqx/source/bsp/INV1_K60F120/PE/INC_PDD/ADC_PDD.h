/*
  PDD layer implementation for peripheral type ADC
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(ADC_PDD_H_)
#define ADC_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error ADC PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK10D5) /* ADC0 */ && \
      !defined(MCU_MK10D7) /* ADC0, ADC1 */ && \
      !defined(MCU_MK10F12) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK10DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK11D5) /* ADC0 */ && \
      !defined(MCU_MK11D5WS) /* ADC0 */ && \
      !defined(MCU_MK12D5) /* ADC0 */ && \
      !defined(MCU_MK20D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK20D5) /* ADC0 */ && \
      !defined(MCU_MK20D7) /* ADC0, ADC1 */ && \
      !defined(MCU_MK20F12) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK20DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK21D5) /* ADC0 */ && \
      !defined(MCU_MK21D5WS) /* ADC0 */ && \
      !defined(MCU_MK22D5) /* ADC0 */ && \
      !defined(MCU_MK30D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK30D7) /* ADC0, ADC1 */ && \
      !defined(MCU_MK30DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK40D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK40D7) /* ADC0, ADC1 */ && \
      !defined(MCU_MK40DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK40X256VMD100) /* ADC0, ADC1 */ && \
      !defined(MCU_MK50D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK50D7) /* ADC0, ADC1 */ && \
      !defined(MCU_MK50DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK51D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK51D7) /* ADC0, ADC1 */ && \
      !defined(MCU_MK51DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK52D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK52DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK53D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK53DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK60D10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK60F12) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK60F15) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK60DZ10) /* ADC0, ADC1 */ && \
      !defined(MCU_MK60N512VMD100) /* ADC0, ADC1 */ && \
      !defined(MCU_MK61F12) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK61F15) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK61F12WS) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK61F15WS) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK70F12) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK70F15) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK70F12WS) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MK70F15WS) /* ADC0, ADC1, ADC2, ADC3 */ && \
      !defined(MCU_MKL02Z4) /* ADC0 */ && \
      !defined(MCU_MKL04Z4) /* ADC0 */ && \
      !defined(MCU_MKL05Z4) /* ADC0 */ && \
      !defined(MCU_MKL14Z4) /* ADC0 */ && \
      !defined(MCU_MKL15Z4) /* ADC0 */ && \
      !defined(MCU_MKL16Z4) /* ADC0 */ && \
      !defined(MCU_MKL24Z4) /* ADC0 */ && \
      !defined(MCU_MKL25Z4) /* ADC0 */ && \
      !defined(MCU_MKL26Z4) /* ADC0 */ && \
      !defined(MCU_MKL34Z4) /* ADC0 */ && \
      !defined(MCU_MKL36Z4) /* ADC0 */ && \
      !defined(MCU_MKL46Z4) /* ADC0 */ && \
      !defined(MCU_PCK20L4) /* ADC0 */
  // Unsupported MCU is active
  #error ADC PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL24Z4)))
/* Channel constants for channel selection */
  #define ADC_PDD_SINGLE_ENDED_DAD0        0U      /**< Single-ended mode, channel 0 */
  #define ADC_PDD_SINGLE_ENDED_DAD1        0x1U    /**< Single-ended mode, channel 1 */
  #define ADC_PDD_SINGLE_ENDED_DAD2        0x2U    /**< Single-ended mode, channel 2 */
  #define ADC_PDD_SINGLE_ENDED_DAD3        0x3U    /**< Single-ended mode, channel 3 */
  #define ADC_PDD_SINGLE_ENDED_AD4         0x4U    /**< Single-ended mode, channel 4 */
  #define ADC_PDD_SINGLE_ENDED_AD5         0x5U    /**< Single-ended mode, channel 5 */
  #define ADC_PDD_SINGLE_ENDED_AD6         0x6U    /**< Single-ended mode, channel 6 */
  #define ADC_PDD_SINGLE_ENDED_AD7         0x7U    /**< Single-ended mode, channel 7 */
  #define ADC_PDD_SINGLE_ENDED_AD8         0x8U    /**< Single-ended mode, channel 8 */
  #define ADC_PDD_SINGLE_ENDED_AD9         0x9U    /**< Single-ended mode, channel 9 */
  #define ADC_PDD_SINGLE_ENDED_AD10        0xAU    /**< Single-ended mode, channel 10 */
  #define ADC_PDD_SINGLE_ENDED_AD11        0xBU    /**< Single-ended mode, channel 11 */
  #define ADC_PDD_SINGLE_ENDED_AD12        0xCU    /**< Single-ended mode, channel 12 */
  #define ADC_PDD_SINGLE_ENDED_AD13        0xDU    /**< Single-ended mode, channel 13 */
  #define ADC_PDD_SINGLE_ENDED_AD14        0xEU    /**< Single-ended mode, channel 14 */
  #define ADC_PDD_SINGLE_ENDED_AD15        0xFU    /**< Single-ended mode, channel 15 */
  #define ADC_PDD_SINGLE_ENDED_AD16        0x10U   /**< Single-ended mode, channel 16 */
  #define ADC_PDD_SINGLE_ENDED_AD17        0x11U   /**< Single-ended mode, channel 17 */
  #define ADC_PDD_SINGLE_ENDED_AD18        0x12U   /**< Single-ended mode, channel 18 */
  #define ADC_PDD_SINGLE_ENDED_AD19        0x13U   /**< Single-ended mode, channel 19 */
  #define ADC_PDD_SINGLE_ENDED_AD20        0x14U   /**< Single-ended mode, channel 20 */
  #define ADC_PDD_SINGLE_ENDED_AD21        0x15U   /**< Single-ended mode, channel 21 */
  #define ADC_PDD_SINGLE_ENDED_AD22        0x16U   /**< Single-ended mode, channel 22 */
  #define ADC_PDD_SINGLE_ENDED_AD23        0x17U   /**< Single-ended mode, channel 23 */
  #define ADC_PDD_SINGLE_ENDED_AD24        0x18U   /**< Single-ended mode, channel 24 */
  #define ADC_PDD_SINGLE_ENDED_AD25        0x19U   /**< Single-ended mode, channel 25 */
  #define ADC_PDD_SINGLE_ENDED_AD26        0x1AU   /**< Single-ended mode, channel 26 */
  #define ADC_PDD_SINGLE_ENDED_AD27        0x1BU   /**< Single-ended mode, channel 27 */
  #define ADC_PDD_SINGLE_ENDED_AD28        0x1CU   /**< Single-ended mode, channel 28 */
  #define ADC_PDD_SINGLE_ENDED_AD29        0x1DU   /**< Single-ended mode, channel 29 */
  #define ADC_PDD_SINGLE_ENDED_AD30        0x1EU   /**< Single-ended mode, channel 30 */
  #define ADC_PDD_MODULE_DISABLED          0x1FU   /**< Module disabled */
  #define ADC_PDD_SINGLE_ENDED_TEMP_SENSOR 0x1AU   /**< Single-ended mode, temp sensor. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_SINGLE_ENDED_BANDGAP     0x1BU   /**< Single-ended mode, bandgap. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_SINGLE_ENDED_VREFSH      0x1DU   /**< Single-ended mode, VREFSH. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_SINGLE_ENDED_VREFSL      0x1EU   /**< Single-ended mode, VREFSL. This constant is deprecated. It will not be supported in the future. */

#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) || (defined(MCU_PCK20L4)) */
/* Channel constants for channel selection */
  #define ADC_PDD_DIFFERENTIAL_DADP0_DADM0 0x20U   /**< Differential mode, positive and negative channel 0 */
  #define ADC_PDD_DIFFERENTIAL_DADP1_DADM1 0x21U   /**< Differential mode, positive and negative channel 1 */
  #define ADC_PDD_DIFFERENTIAL_DADP2_DADM2 0x22U   /**< Differential mode, positive and negative channel 2 */
  #define ADC_PDD_DIFFERENTIAL_DADP3_DADM3 0x23U   /**< Differential mode, positive and negative channel 3 */
  #define ADC_PDD_SINGLE_ENDED_DAD0        0U      /**< Single-ended mode, channel 0 */
  #define ADC_PDD_SINGLE_ENDED_DAD1        0x1U    /**< Single-ended mode, channel 1 */
  #define ADC_PDD_SINGLE_ENDED_DAD2        0x2U    /**< Single-ended mode, channel 2 */
  #define ADC_PDD_SINGLE_ENDED_DAD3        0x3U    /**< Single-ended mode, channel 3 */
  #define ADC_PDD_SINGLE_ENDED_AD4         0x4U    /**< Single-ended mode, channel 4 */
  #define ADC_PDD_SINGLE_ENDED_AD5         0x5U    /**< Single-ended mode, channel 5 */
  #define ADC_PDD_SINGLE_ENDED_AD6         0x6U    /**< Single-ended mode, channel 6 */
  #define ADC_PDD_SINGLE_ENDED_AD7         0x7U    /**< Single-ended mode, channel 7 */
  #define ADC_PDD_SINGLE_ENDED_AD8         0x8U    /**< Single-ended mode, channel 8 */
  #define ADC_PDD_SINGLE_ENDED_AD9         0x9U    /**< Single-ended mode, channel 9 */
  #define ADC_PDD_SINGLE_ENDED_AD10        0xAU    /**< Single-ended mode, channel 10 */
  #define ADC_PDD_SINGLE_ENDED_AD11        0xBU    /**< Single-ended mode, channel 11 */
  #define ADC_PDD_SINGLE_ENDED_AD12        0xCU    /**< Single-ended mode, channel 12 */
  #define ADC_PDD_SINGLE_ENDED_AD13        0xDU    /**< Single-ended mode, channel 13 */
  #define ADC_PDD_SINGLE_ENDED_AD14        0xEU    /**< Single-ended mode, channel 14 */
  #define ADC_PDD_SINGLE_ENDED_AD15        0xFU    /**< Single-ended mode, channel 15 */
  #define ADC_PDD_SINGLE_ENDED_AD16        0x10U   /**< Single-ended mode, channel 16 */
  #define ADC_PDD_SINGLE_ENDED_AD17        0x11U   /**< Single-ended mode, channel 17 */
  #define ADC_PDD_SINGLE_ENDED_AD18        0x12U   /**< Single-ended mode, channel 18 */
  #define ADC_PDD_SINGLE_ENDED_AD19        0x13U   /**< Single-ended mode, channel 19 */
  #define ADC_PDD_SINGLE_ENDED_AD20        0x14U   /**< Single-ended mode, channel 20 */
  #define ADC_PDD_SINGLE_ENDED_AD21        0x15U   /**< Single-ended mode, channel 21 */
  #define ADC_PDD_SINGLE_ENDED_AD22        0x16U   /**< Single-ended mode, channel 22 */
  #define ADC_PDD_SINGLE_ENDED_AD23        0x17U   /**< Single-ended mode, channel 23 */
  #define ADC_PDD_SINGLE_ENDED_AD24        0x18U   /**< Single-ended mode, channel 24 */
  #define ADC_PDD_SINGLE_ENDED_AD25        0x19U   /**< Single-ended mode, channel 25 */
  #define ADC_PDD_SINGLE_ENDED_AD26        0x1AU   /**< Single-ended mode, channel 26 */
  #define ADC_PDD_SINGLE_ENDED_AD27        0x1BU   /**< Single-ended mode, channel 27 */
  #define ADC_PDD_SINGLE_ENDED_AD28        0x1CU   /**< Single-ended mode, channel 28 */
  #define ADC_PDD_SINGLE_ENDED_AD29        0x1DU   /**< Single-ended mode, channel 29 */
  #define ADC_PDD_SINGLE_ENDED_AD30        0x1EU   /**< Single-ended mode, channel 30 */
  #define ADC_PDD_MODULE_DISABLED          0x1FU   /**< Module disabled */
  #define ADC_PDD_DIFFERENTIAL_BANDGAP     0x3BU   /**< Differential mode, positive and negative bandgap. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_DIFFERENTIAL_VREFSH      0x3DU   /**< Differential mode, positive and negative VREFS. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_SINGLE_ENDED_TEMP_SENSOR 0x1AU   /**< Single-ended mode, temp sensor. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_SINGLE_ENDED_BANDGAP     0x1BU   /**< Single-ended mode, bandgap. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_SINGLE_ENDED_VREFSH      0x1DU   /**< Single-ended mode, VREFSH. This constant is deprecated. It will not be supported in the future. */
  #define ADC_PDD_SINGLE_ENDED_VREFSL      0x1EU   /**< Single-ended mode, VREFSL. This constant is deprecated. It will not be supported in the future. */

#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)) || (defined(MCU_PCK20L4)) */
/* Constants for trigger type selection */
#define ADC_PDD_SW_TRIGGER 0U                    /**< SW trigger */
#define ADC_PDD_HW_TRIGGER 0x40U                 /**< HW trigger */

/* Constants for compare function selection */
#define ADC_PDD_COMPARE_DISABLED      0U         /**< Compare function disabled */
#define ADC_PDD_LESS_THAN             0x20U      /**< Compare function enabled to 'less than' */
#define ADC_PDD_GREATER_THAN_OR_EQUAL 0x30U      /**< Compare function enabled to 'greater than or equal' */
#define ADC_PDD_RANGE_NOT_INCLUSIVE   0x28U      /**< Compare function enabled to 'range, not inclusive' */
#define ADC_PDD_RANGE_INCLUSIVE       0x38U      /**< Compare function enabled to 'range, inclusive' */

/* Constants for one conversion or continuous selection */
#define ADC_PDD_ONE_CONVERSION         0U        /**< One conversion mode */
#define ADC_PDD_CONTINUOUS_CONVERSIONS 0x8U      /**< Continuous conversion mode */

/* Constants for average type selection */
#define ADC_PDD_AVERAGE_DISABLED    0U           /**< Average function disabled */
#define ADC_PDD_4_SAMPLES_AVERAGED  0x4U         /**< Average function with 4 samples */
#define ADC_PDD_8_SAMPLES_AVERAGED  0x5U         /**< Average function with 8 samples */
#define ADC_PDD_16_SAMPLES_AVERAGED 0x6U         /**< Average function with 16 samples */
#define ADC_PDD_32_SAMPLES_AVERAGED 0x7U         /**< Average function with 32 samples */


/* ----------------------------------------------------------------------------
   -- SetChannel
   ---------------------------------------------------------------------------- */

/**
 * @brief Set channel.
 * @param PeripheralBase Peripheral base address.
 * @param Index Register index. This parameter is of index type.
 * @param Value Parameter specifying new channel. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SC1[Index].
 * @par Example:
 *      @code
 *      ADC_PDD_SetChannel(deviceID, periphID, 1);
 *      @endcode
 */
#define ADC_PDD_SetChannel(PeripheralBase, Index, Value) ( \
    ADC_SC1_REG(PeripheralBase,(Index)) = \
     (uint32_t)(( \
      (uint32_t)(ADC_SC1_REG(PeripheralBase,(Index)) & (uint32_t)(~(uint32_t)ADC_SC1_ADCH_MASK))) | ( \
      (uint32_t)(Value))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableConversionCompleteInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable conversion complete interrupt.
 * @param PeripheralBase Peripheral base address.
 * @param Index Register index. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SC1[Index].
 * @par Example:
 *      @code
 *      ADC_PDD_EnableConversionCompleteInterrupt(deviceID, periphID);
 *      @endcode
 */
#define ADC_PDD_EnableConversionCompleteInterrupt(PeripheralBase, Index) ( \
    ADC_SC1_REG(PeripheralBase,(Index)) |= \
     ADC_SC1_AIEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableConversionCompleteInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable conversion complete interrupt.
 * @param PeripheralBase Peripheral base address.
 * @param Index Register index. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SC1[Index].
 * @par Example:
 *      @code
 *      ADC_PDD_DisableConversionCompleteInterrupt(deviceID, periphID);
 *      @endcode
 */
#define ADC_PDD_DisableConversionCompleteInterrupt(PeripheralBase, Index) ( \
    ADC_SC1_REG(PeripheralBase,(Index)) &= \
     (uint32_t)(~(uint32_t)ADC_SC1_AIEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetConversionCompleteFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns conversion complete flag.
 * @param PeripheralBase Peripheral base address.
 * @param Index Register index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SC1[Index].
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetConversionCompleteFlag(deviceID, periphID);
 *      @endcode
 */
#define ADC_PDD_GetConversionCompleteFlag(PeripheralBase, Index) ( \
    (uint32_t)(ADC_SC1_REG(PeripheralBase,(Index)) & ADC_SC1_COCO_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- WriteStatusControl1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Status and control 1 register.
 * @param PeripheralBase Peripheral base address.
 * @param Index Register index. This parameter is of index type.
 * @param Value Parameter specifying new register value. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SC1[Index].
 * @par Example:
 *      @code
 *      ADC_PDD_WriteStatusControl1Reg(deviceID, periphID, 1);
 *      @endcode
 */
#define ADC_PDD_WriteStatusControl1Reg(PeripheralBase, Index, Value) ( \
    ADC_SC1_REG(PeripheralBase,(Index)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadStatusControl1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Status and control 1 register.
 * @param PeripheralBase Peripheral base address.
 * @param Index Register index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SC1[Index].
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_ReadStatusControl1Reg(deviceID, periphID);
 *      @endcode
 */
#define ADC_PDD_ReadStatusControl1Reg(PeripheralBase, Index) ( \
    ADC_SC1_REG(PeripheralBase,(Index)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteConfiguration1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Configuration 1 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new register value. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_CFG1, ADC1_CFG1,
 *          ADC2_CFG1, ADC3_CFG1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_WriteConfiguration1Reg(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_WriteConfiguration1Reg(PeripheralBase, Value) ( \
    ADC_CFG1_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadConfiguration1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Configuration 1 register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CFG1, ADC1_CFG1,
 *          ADC2_CFG1, ADC3_CFG1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_ReadConfiguration1Reg(deviceID);
 *      @endcode
 */
#define ADC_PDD_ReadConfiguration1Reg(PeripheralBase) ( \
    ADC_CFG1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteConfiguration2Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Configuration 2 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new register value. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_CFG2, ADC1_CFG2,
 *          ADC2_CFG2, ADC3_CFG2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_WriteConfiguration2Reg(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_WriteConfiguration2Reg(PeripheralBase, Value) ( \
    ADC_CFG2_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadConfiguration2Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Configuration 2 register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CFG2, ADC1_CFG2,
 *          ADC2_CFG2, ADC3_CFG2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_ReadConfiguration2Reg(deviceID);
 *      @endcode
 */
#define ADC_PDD_ReadConfiguration2Reg(PeripheralBase) ( \
    ADC_CFG2_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetConversionActiveFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns conversion active flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_SC2, ADC1_SC2,
 *          ADC2_SC2, ADC3_SC2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetConversionActiveFlag(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetConversionActiveFlag(PeripheralBase) ( \
    (uint32_t)(ADC_SC2_REG(PeripheralBase) & ADC_SC2_ADACT_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetCompareValue1Raw
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets compare value 1.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new compare value 1. This parameter is a
 *        16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_CV1, ADC1_CV1,
 *          ADC2_CV1, ADC3_CV1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetCompareValue1Raw(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_SetCompareValue1Raw(PeripheralBase, Value) ( \
    ADC_CV1_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetCompareValue1Raw
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns compare value 1.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CV1, ADC1_CV1,
 *          ADC2_CV1, ADC3_CV1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetCompareValue1Raw(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetCompareValue1Raw(PeripheralBase) ( \
    ADC_CV1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetCompareValue2Raw
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets compare value 2.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new compare value 2. This parameter is a
 *        16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_CV2, ADC1_CV2,
 *          ADC2_CV2, ADC3_CV2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetCompareValue2Raw(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_SetCompareValue2Raw(PeripheralBase, Value) ( \
    ADC_CV2_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetCompareValue2Raw
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns compare value 2.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CV2, ADC1_CV2,
 *          ADC2_CV2, ADC3_CV2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetCompareValue2Raw(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetCompareValue2Raw(PeripheralBase) ( \
    ADC_CV2_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetConversionTriggerType
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets conversion trigger type.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Trigger mode. This parameter is of "Constants for trigger type
 *        selection" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC2, ADC1_SC2,
 *          ADC2_SC2, ADC3_SC2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetConversionTriggerType(deviceID, ADC_PDD_SW_TRIGGER);
 *      @endcode
 */
#define ADC_PDD_SetConversionTriggerType(PeripheralBase, Mode) ( \
    ADC_SC2_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(ADC_SC2_REG(PeripheralBase) & (uint32_t)(~(uint32_t)ADC_SC2_ADTRG_MASK))) | ( \
      (uint32_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- GetConversionTriggerType
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns conversion trigger type.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of "Constants for trigger type selection" type. The
 *         value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: ADC0_SC2, ADC1_SC2,
 *          ADC2_SC2, ADC3_SC2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetConversionTriggerType(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetConversionTriggerType(PeripheralBase) ( \
    (uint32_t)(ADC_SC2_REG(PeripheralBase) & ADC_SC2_ADTRG_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetCompareFunction
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets compare type.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Compare mode. This parameter is of "Constants for compare
 *        function selection" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC2, ADC1_SC2,
 *          ADC2_SC2, ADC3_SC2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetCompareFunction(deviceID, ADC_PDD_COMPARE_DISABLED);
 *      @endcode
 */
#define ADC_PDD_SetCompareFunction(PeripheralBase, Mode) ( \
    ADC_SC2_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(ADC_SC2_REG(PeripheralBase) & (uint32_t)(~(uint32_t)((uint32_t)0x7U << 3U)))) | ( \
      (uint32_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDmaRequest
   ---------------------------------------------------------------------------- */

/**
 * @brief DMA enable request.
 * @param PeripheralBase Peripheral base address.
 * @param State Parameter specifying if DMA requests will be enabled or
 *        disabled. This parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in
 *        PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC2, ADC1_SC2,
 *          ADC2_SC2, ADC3_SC2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_EnableDmaRequest(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define ADC_PDD_EnableDmaRequest(PeripheralBase, State) ( \
    ADC_SC2_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(ADC_SC2_REG(PeripheralBase) & (uint32_t)(~(uint32_t)ADC_SC2_DMAEN_MASK))) | ( \
      (uint32_t)((uint32_t)(State) << ADC_SC2_DMAEN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteStatusControl2Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Status and control 2 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new register value. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC2, ADC1_SC2,
 *          ADC2_SC2, ADC3_SC2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_WriteStatusControl2Reg(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_WriteStatusControl2Reg(PeripheralBase, Value) ( \
    ADC_SC2_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadStatusControl2Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Status and control 2 register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_SC2, ADC1_SC2,
 *          ADC2_SC2, ADC3_SC2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_ReadStatusControl2Reg(deviceID);
 *      @endcode
 */
#define ADC_PDD_ReadStatusControl2Reg(PeripheralBase) ( \
    ADC_SC2_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- StartCalibration
   ---------------------------------------------------------------------------- */

/**
 * @brief Starts calibration.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_StartCalibration(deviceID);
 *      @endcode
 */
#define ADC_PDD_StartCalibration(PeripheralBase) ( \
    ADC_SC3_REG(PeripheralBase) |= \
     ADC_SC3_CAL_MASK \
  )

/* ----------------------------------------------------------------------------
   -- GetCalibrationActiveFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns calibration active flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetCalibrationActiveFlag(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetCalibrationActiveFlag(PeripheralBase) ( \
    (uint32_t)(ADC_SC3_REG(PeripheralBase) & ADC_SC3_CAL_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetCalibrationFailedStatusFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns calibration failed status flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetCalibrationFailedStatusFlag(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetCalibrationFailedStatusFlag(PeripheralBase) ( \
    (uint32_t)(ADC_SC3_REG(PeripheralBase) & ADC_SC3_CALF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetContinuousMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets one conversion or continuous mode.
 * @param PeripheralBase Peripheral base address.
 * @param Mode One conversion or continuous mode. This parameter is of
 *        "Constants for one conversion or continuous selection" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetContinuousMode(deviceID, ADC_PDD_ONE_CONVERSION);
 *      @endcode
 */
#define ADC_PDD_SetContinuousMode(PeripheralBase, Mode) ( \
    ADC_SC3_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(ADC_SC3_REG(PeripheralBase) & (uint32_t)(~(uint32_t)ADC_SC3_ADCO_MASK))) | ( \
      (uint32_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- GetContinuousMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns one conversion or continuous mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of "Constants for one conversion or continuous
 *         selection" type. The value is cast to "uint32_t".
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetContinuousMode(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetContinuousMode(PeripheralBase) ( \
    (uint32_t)(ADC_SC3_REG(PeripheralBase) & ADC_SC3_ADCO_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetAverageFunction
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets average function.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Average mode. This parameter is of "Constants for average type
 *        selection" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetAverageFunction(deviceID, ADC_PDD_AVERAGE_DISABLED);
 *      @endcode
 */
#define ADC_PDD_SetAverageFunction(PeripheralBase, Mode) ( \
    ADC_SC3_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(ADC_SC3_REG(PeripheralBase) & (uint32_t)(~(uint32_t)0x7U))) | ( \
      (uint32_t)(Mode))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteStatusControl3Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Status and control 3 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new register value. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_WriteStatusControl3Reg(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_WriteStatusControl3Reg(PeripheralBase, Value) ( \
    ADC_SC3_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadStatusControl3Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Status and control 3 register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_SC3, ADC1_SC3,
 *          ADC2_SC3, ADC3_SC3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_ReadStatusControl3Reg(deviceID);
 *      @endcode
 */
#define ADC_PDD_ReadStatusControl3Reg(PeripheralBase) ( \
    ADC_SC3_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetResultValueRaw
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns transfer counter.
 * @param PeripheralBase Peripheral base address.
 * @param Index Register index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: R[Index].
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetResultValueRaw(deviceID, periphID);
 *      @endcode
 */
#define ADC_PDD_GetResultValueRaw(PeripheralBase, Index) ( \
    ADC_R_REG(PeripheralBase,(Index)) \
  )

/* ----------------------------------------------------------------------------
   -- SetOffsetCorrectionValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets offset error correction value in 2's complement and left
 * justified format.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new offset correction value. This parameter
 *        is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_OFS, ADC1_OFS,
 *          ADC2_OFS, ADC3_OFS (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetOffsetCorrectionValue(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_SetOffsetCorrectionValue(PeripheralBase, Value) ( \
    ADC_OFS_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetOffsetCorrectionValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns offset error correction value in 2's complement and left
 * justified format.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_OFS, ADC1_OFS,
 *          ADC2_OFS, ADC3_OFS (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetOffsetCorrectionValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetOffsetCorrectionValue(PeripheralBase) ( \
    ADC_OFS_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetPlusGainValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets plus-side gain value.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_PG, ADC1_PG,
 *          ADC2_PG, ADC3_PG (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetPlusGainValue(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_SetPlusGainValue(PeripheralBase, Value) ( \
    ADC_PG_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- SetMinusGainValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets minus-side gain value.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ADC0_MG, ADC1_MG,
 *          ADC2_MG, ADC3_MG (depending on the peripheral).
 * @par Example:
 *      @code
 *      ADC_PDD_SetMinusGainValue(deviceID, 1);
 *      @endcode
 */
#define ADC_PDD_SetMinusGainValue(PeripheralBase, Value) ( \
    ADC_MG_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetPlus0CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns plus-side general calibration value 0.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLP0, ADC1_CLP0,
 *          ADC2_CLP0, ADC3_CLP0 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetPlus0CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetPlus0CalibrationValue(PeripheralBase) ( \
    ADC_CLP0_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetPlus1CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns plus-side general calibration value 1.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLP1, ADC1_CLP1,
 *          ADC2_CLP1, ADC3_CLP1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetPlus1CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetPlus1CalibrationValue(PeripheralBase) ( \
    ADC_CLP1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetPlus2CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns plus-side general calibration value 2.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLP2, ADC1_CLP2,
 *          ADC2_CLP2, ADC3_CLP2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetPlus2CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetPlus2CalibrationValue(PeripheralBase) ( \
    ADC_CLP2_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetPlus3CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns plus-side general calibration value 3.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLP3, ADC1_CLP3,
 *          ADC2_CLP3, ADC3_CLP3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetPlus3CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetPlus3CalibrationValue(PeripheralBase) ( \
    ADC_CLP3_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetPlus4CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns plus-side general calibration value 4.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLP4, ADC1_CLP4,
 *          ADC2_CLP4, ADC3_CLP4 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetPlus4CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetPlus4CalibrationValue(PeripheralBase) ( \
    ADC_CLP4_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetPlusSCalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns plus-side general calibration value S.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLPS, ADC1_CLPS,
 *          ADC2_CLPS, ADC3_CLPS (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetPlusSCalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetPlusSCalibrationValue(PeripheralBase) ( \
    ADC_CLPS_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetPlusDCalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns plus-side general calibration value D.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLPD, ADC1_CLPD,
 *          ADC2_CLPD, ADC3_CLPD (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetPlusDCalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetPlusDCalibrationValue(PeripheralBase) ( \
    ADC_CLPD_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetMinus0CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns minus-side general calibration value 0.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLM0, ADC1_CLM0,
 *          ADC2_CLM0, ADC3_CLM0 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetMinus0CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetMinus0CalibrationValue(PeripheralBase) ( \
    ADC_CLM0_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetMinus1CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns minus-side general calibration value 1.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLM1, ADC1_CLM1,
 *          ADC2_CLM1, ADC3_CLM1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetMinus1CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetMinus1CalibrationValue(PeripheralBase) ( \
    ADC_CLM1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetMinus2CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns minus-side general calibration value 2.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLM2, ADC1_CLM2,
 *          ADC2_CLM2, ADC3_CLM2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetMinus2CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetMinus2CalibrationValue(PeripheralBase) ( \
    ADC_CLM2_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetMinus3CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns minus-side general calibration value 3.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLM3, ADC1_CLM3,
 *          ADC2_CLM3, ADC3_CLM3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetMinus3CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetMinus3CalibrationValue(PeripheralBase) ( \
    ADC_CLM3_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetMinus4CalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns minus-side general calibration value 4.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLM4, ADC1_CLM4,
 *          ADC2_CLM4, ADC3_CLM4 (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetMinus4CalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetMinus4CalibrationValue(PeripheralBase) ( \
    ADC_CLM4_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetMinusSCalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns minus-side general calibration value S.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLMS, ADC1_CLMS,
 *          ADC2_CLMS, ADC3_CLMS (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetMinusSCalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetMinusSCalibrationValue(PeripheralBase) ( \
    ADC_CLMS_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetMinusDCalibrationValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns minus-side general calibration value D.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ADC0_CLMD, ADC1_CLMD,
 *          ADC2_CLMD, ADC3_CLMD (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = ADC_PDD_GetMinusDCalibrationValue(deviceID);
 *      @endcode
 */
#define ADC_PDD_GetMinusDCalibrationValue(PeripheralBase) ( \
    ADC_CLMD_REG(PeripheralBase) \
  )
#endif  /* #if defined(ADC_PDD_H_) */

/* ADC_PDD.h, eof. */
