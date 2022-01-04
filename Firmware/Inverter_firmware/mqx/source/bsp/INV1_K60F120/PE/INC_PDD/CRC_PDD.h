/*
  PDD layer implementation for peripheral type CRC
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(CRC_PDD_H_)
#define CRC_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error CRC PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* CRC */ && \
      !defined(MCU_MK10D5) /* CRC */ && \
      !defined(MCU_MK10D7) /* CRC */ && \
      !defined(MCU_MK10F12) /* CRC */ && \
      !defined(MCU_MK10DZ10) /* CRC */ && \
      !defined(MCU_MK11D5) /* CRC */ && \
      !defined(MCU_MK11D5WS) /* CRC */ && \
      !defined(MCU_MK12D5) /* CRC */ && \
      !defined(MCU_MK20D10) /* CRC */ && \
      !defined(MCU_MK20D5) /* CRC */ && \
      !defined(MCU_MK20D7) /* CRC */ && \
      !defined(MCU_MK20F12) /* CRC */ && \
      !defined(MCU_MK20DZ10) /* CRC */ && \
      !defined(MCU_MK21D5) /* CRC */ && \
      !defined(MCU_MK21D5WS) /* CRC */ && \
      !defined(MCU_MK22D5) /* CRC */ && \
      !defined(MCU_MK30D10) /* CRC */ && \
      !defined(MCU_MK30D7) /* CRC */ && \
      !defined(MCU_MK30DZ10) /* CRC */ && \
      !defined(MCU_MK40D10) /* CRC */ && \
      !defined(MCU_MK40D7) /* CRC */ && \
      !defined(MCU_MK40DZ10) /* CRC */ && \
      !defined(MCU_MK40X256VMD100) /* CRC */ && \
      !defined(MCU_MK50D10) /* CRC */ && \
      !defined(MCU_MK50D7) /* CRC */ && \
      !defined(MCU_MK50DZ10) /* CRC */ && \
      !defined(MCU_MK51D10) /* CRC */ && \
      !defined(MCU_MK51D7) /* CRC */ && \
      !defined(MCU_MK51DZ10) /* CRC */ && \
      !defined(MCU_MK52D10) /* CRC */ && \
      !defined(MCU_MK52DZ10) /* CRC */ && \
      !defined(MCU_MK53D10) /* CRC */ && \
      !defined(MCU_MK53DZ10) /* CRC */ && \
      !defined(MCU_MK60D10) /* CRC */ && \
      !defined(MCU_MK60F12) /* CRC */ && \
      !defined(MCU_MK60F15) /* CRC */ && \
      !defined(MCU_MK60DZ10) /* CRC */ && \
      !defined(MCU_MK60N512VMD100) /* CRC */ && \
      !defined(MCU_MK61F12) /* CRC */ && \
      !defined(MCU_MK61F15) /* CRC */ && \
      !defined(MCU_MK61F12WS) /* CRC */ && \
      !defined(MCU_MK61F15WS) /* CRC */ && \
      !defined(MCU_MK70F12) /* CRC */ && \
      !defined(MCU_MK70F15) /* CRC */ && \
      !defined(MCU_MK70F12WS) /* CRC */ && \
      !defined(MCU_MK70F15WS) /* CRC */ && \
      !defined(MCU_PCK20L4) /* CRC */
  // Unsupported MCU is active
  #error CRC PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Computation parameters of CRC standards. */
#define CRC_PDD_NO_TRANSPOSE   0U                /**< No transpose of data. */
#define CRC_PDD_BITS           0x1U              /**< Bits are swapped. */
#define CRC_PDD_BITS_AND_BYTES 0x2U              /**< Bytes and bits are swapped. */
#define CRC_PDD_BYTES          0x3U              /**< Bytes are swapped. */


/* ----------------------------------------------------------------------------
   -- GetCRCDataRegister
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Returns current CRC result from data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CRC_CRC, CRC_DATA
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = CRC_PDD_GetCRCDataRegister(deviceID);
 *      @endcode
 */
  #define CRC_PDD_GetCRCDataRegister(PeripheralBase) ( \
      CRC_DATA_REG(PeripheralBase) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Returns current CRC result from data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CRC_CRC, CRC_DATA
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = CRC_PDD_GetCRCDataRegister(deviceID);
 *      @endcode
 */
  #define CRC_PDD_GetCRCDataRegister(PeripheralBase) ( \
      CRC_CRC_REG(PeripheralBase) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- GetCRCDataHRegister
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Returns upper 16bits of current CRC result from data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: CRC_CRCH, CRC_DATAH
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint16_t result = CRC_PDD_GetCRCDataHRegister(deviceID);
 *      @endcode
 */
  #define CRC_PDD_GetCRCDataHRegister(PeripheralBase) ( \
      CRC_DATAH_REG(PeripheralBase) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Returns upper 16bits of current CRC result from data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: CRC_CRCH, CRC_DATAH
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint16_t result = CRC_PDD_GetCRCDataHRegister(deviceID);
 *      @endcode
 */
  #define CRC_PDD_GetCRCDataHRegister(PeripheralBase) ( \
      CRC_CRCH_REG(PeripheralBase) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- GetCRCDataLRegister
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Returns lower 16bits of current CRC result from data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: CRC_CRCL, CRC_DATAL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint16_t result = CRC_PDD_GetCRCDataLRegister(deviceID);
 *      @endcode
 */
  #define CRC_PDD_GetCRCDataLRegister(PeripheralBase) ( \
      CRC_DATAL_REG(PeripheralBase) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Returns lower 16bits of current CRC result from data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: CRC_CRCL, CRC_DATAL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint16_t result = CRC_PDD_GetCRCDataLRegister(deviceID);
 *      @endcode
 */
  #define CRC_PDD_GetCRCDataLRegister(PeripheralBase) ( \
      CRC_CRCL_REG(PeripheralBase) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- SetCRCDataRegister
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Set CRC data register (4 bytes).
 * @param PeripheralBase Peripheral base address.
 * @param Data New data for CRC computation. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRC, CRC_DATA
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRCDataRegister(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetCRCDataRegister(PeripheralBase, Data) ( \
      CRC_DATA_REG(PeripheralBase) = \
       (uint32_t)(Data) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Set CRC data register (4 bytes).
 * @param PeripheralBase Peripheral base address.
 * @param Data New data for CRC computation. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRC, CRC_DATA
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRCDataRegister(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetCRCDataRegister(PeripheralBase, Data) ( \
      CRC_CRC_REG(PeripheralBase) = \
       (uint32_t)(Data) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- SetCRCDataLRegister
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Set CRC data register (2 bytes).
 * @param PeripheralBase Peripheral base address.
 * @param Data New data for CRC computation. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCL, CRC_DATAL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRCDataLRegister(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetCRCDataLRegister(PeripheralBase, Data) ( \
      CRC_DATAL_REG(PeripheralBase) = \
       (uint16_t)(Data) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Set CRC data register (2 bytes).
 * @param PeripheralBase Peripheral base address.
 * @param Data New data for CRC computation. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCL, CRC_DATAL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRCDataLRegister(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetCRCDataLRegister(PeripheralBase, Data) ( \
      CRC_CRCL_REG(PeripheralBase) = \
       (uint16_t)(Data) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- SetCRCDataLLRegister
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Set CRC data register (1 byte).
 * @param PeripheralBase Peripheral base address.
 * @param Data New data for CRC computation. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCLL, CRC_DATALL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRCDataLLRegister(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetCRCDataLLRegister(PeripheralBase, Data) ( \
      CRC_DATALL_REG(PeripheralBase) = \
       (uint8_t)(Data) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Set CRC data register (1 byte).
 * @param PeripheralBase Peripheral base address.
 * @param Data New data for CRC computation. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCLL, CRC_DATALL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRCDataLLRegister(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetCRCDataLLRegister(PeripheralBase, Data) ( \
      CRC_CRCLL_REG(PeripheralBase) = \
       (uint8_t)(Data) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- SetCRCControlRegister
   ---------------------------------------------------------------------------- */

/**
 * @brief Set CRC control register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Control register value. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRCControlRegister(deviceID, 1);
 *      @endcode
 */
#define CRC_PDD_SetCRCControlRegister(PeripheralBase, Data) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- SetSeedBit
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable seed mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetSeedBit(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetSeedBit(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) |= \
     CRC_CTRL_WAS_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearSeedBit
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable seed mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_ClearSeedBit(deviceID);
 *      @endcode
 */
#define CRC_PDD_ClearSeedBit(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)CRC_CTRL_WAS_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetXorBit
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable result XORing with 0xFFFF or 0xFFFFFFFF.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetXorBit(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetXorBit(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) |= \
     CRC_CTRL_FXOR_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearXorBit
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable result XORing with 0xFFFF or 0xFFFFFFFF.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_ClearXorBit(deviceID);
 *      @endcode
 */
#define CRC_PDD_ClearXorBit(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)CRC_CTRL_FXOR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetCRC32bit
   ---------------------------------------------------------------------------- */

/**
 * @brief Set CRC module for 32bit computation.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC32bit(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC32bit(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) |= \
     CRC_CTRL_TCRC_MASK \
  )

/* ----------------------------------------------------------------------------
   -- SetCRC16bit
   ---------------------------------------------------------------------------- */

/**
 * @brief Set CRC module for 16bit computation.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC16bit(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC16bit(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)CRC_CTRL_TCRC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetInputTranspose
   ---------------------------------------------------------------------------- */

/**
 * @brief Set input data transposition type.
 * @param PeripheralBase Peripheral base address.
 * @param Type Input data transposition type. This parameter is a 2-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetInputTranspose(deviceID, 1);
 *      @endcode
 */
#define CRC_PDD_SetInputTranspose(PeripheralBase, Type) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(CRC_CTRL_REG(PeripheralBase) & (uint32_t)(~(uint32_t)CRC_CTRL_TOT_MASK))) | ( \
      (uint32_t)((uint32_t)(Type) << CRC_CTRL_TOT_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetOutputTranspose
   ---------------------------------------------------------------------------- */

/**
 * @brief Set output data transposition type.
 * @param PeripheralBase Peripheral base address.
 * @param Type Output data transposition type. This parameter is a 2-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetOutputTranspose(deviceID, 1);
 *      @endcode
 */
#define CRC_PDD_SetOutputTranspose(PeripheralBase, Type) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(CRC_CTRL_REG(PeripheralBase) & (uint32_t)(~(uint32_t)CRC_CTRL_TOTR_MASK))) | ( \
      (uint32_t)((uint32_t)(Type) << CRC_CTRL_TOTR_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetOutputTranspose
   ---------------------------------------------------------------------------- */

/**
 * @brief Get output transpose settings.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 2-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      uint8_t result = CRC_PDD_GetOutputTranspose(deviceID);
 *      @endcode
 */
#define CRC_PDD_GetOutputTranspose(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(CRC_CTRL_REG(PeripheralBase) & CRC_CTRL_TOTR_MASK)) >> ( \
     CRC_CTRL_TOTR_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetInputTranspose
   ---------------------------------------------------------------------------- */

/**
 * @brief Get input transpose settings.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 2-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      uint8_t result = CRC_PDD_GetInputTranspose(deviceID);
 *      @endcode
 */
#define CRC_PDD_GetInputTranspose(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(CRC_CTRL_REG(PeripheralBase) & CRC_CTRL_TOT_MASK)) >> ( \
     CRC_CTRL_TOT_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- SetPolyHigh
   ---------------------------------------------------------------------------- */

/**
 * @brief Set upper 16 bits of polynomial register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Polynomial value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_GPOLYH.
 * @par Example:
 *      @code
 *      CRC_PDD_SetPolyHigh(deviceID, 1);
 *      @endcode
 */
#define CRC_PDD_SetPolyHigh(PeripheralBase, Data) ( \
    CRC_GPOLYH_REG(PeripheralBase) = \
     (uint16_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- SetPolyLow
   ---------------------------------------------------------------------------- */

/**
 * @brief Set lower 16 bits of polynomial register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Polynomial value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_GPOLYL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetPolyLow(deviceID, 1);
 *      @endcode
 */
#define CRC_PDD_SetPolyLow(PeripheralBase, Data) ( \
    CRC_GPOLYL_REG(PeripheralBase) = \
     (uint16_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- SetSeedHigh
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Set upper 16 bits of data register in seed mode.
 * @param PeripheralBase Peripheral base address.
 * @param Data Seed value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCH, CRC_DATAH
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetSeedHigh(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetSeedHigh(PeripheralBase, Data) ( \
      CRC_DATAH_REG(PeripheralBase) = \
       (uint16_t)(Data) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Set upper 16 bits of data register in seed mode.
 * @param PeripheralBase Peripheral base address.
 * @param Data Seed value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCH, CRC_DATAH
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetSeedHigh(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetSeedHigh(PeripheralBase, Data) ( \
      CRC_CRCH_REG(PeripheralBase) = \
       (uint16_t)(Data) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- SetSeedLow
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)))
/**
 * @brief Set lower 16 bits of data register in seed mode.
 * @param PeripheralBase Peripheral base address.
 * @param Data Seed value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCL, CRC_DATAL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetSeedLow(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetSeedLow(PeripheralBase, Data) ( \
      CRC_DATAL_REG(PeripheralBase) = \
       (uint16_t)(Data) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Set lower 16 bits of data register in seed mode.
 * @param PeripheralBase Peripheral base address.
 * @param Data Seed value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CRCL, CRC_DATAL
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      CRC_PDD_SetSeedLow(deviceID, 1);
 *      @endcode
 */
  #define CRC_PDD_SetSeedLow(PeripheralBase, Data) ( \
      CRC_CRCL_REG(PeripheralBase) = \
       (uint16_t)(Data) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- SetCRC_16
   ---------------------------------------------------------------------------- */

/**
 * @brief Set control register for CRC16 standard operation and starts seed mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC_16(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC_16(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       CRC_CTRL_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TCRC_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_FXOR_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOT_MASK)) & ( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOTR_MASK))))))) | (( \
      CRC_CTRL_WAS_MASK) | (( \
      (uint32_t)((uint32_t)0x1U << CRC_CTRL_TOT_SHIFT)) | ( \
      (uint32_t)((uint32_t)0x2U << CRC_CTRL_TOTR_SHIFT))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCRC_32
   ---------------------------------------------------------------------------- */

/**
 * @brief Set control register for CRC32 standard operation and starts seed mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC_32(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC_32(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       (uint32_t)(( \
        CRC_CTRL_REG(PeripheralBase)) | (( \
        CRC_CTRL_TCRC_MASK) | (( \
        CRC_CTRL_WAS_MASK) | ( \
        CRC_CTRL_FXOR_MASK))))) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOT_MASK)) & ( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOTR_MASK))))) | (( \
      (uint32_t)((uint32_t)0x1U << CRC_CTRL_TOT_SHIFT)) | ( \
      (uint32_t)((uint32_t)0x2U << CRC_CTRL_TOTR_SHIFT)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCRC_CCITT
   ---------------------------------------------------------------------------- */

/**
 * @brief Set control register for CCITT standard operation and starts seed mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC_CCITT(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC_CCITT(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       CRC_CTRL_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TCRC_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_FXOR_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOT_MASK)) & ( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOTR_MASK))))))) | ( \
      CRC_CTRL_WAS_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- SetCRC_KERMIT
   ---------------------------------------------------------------------------- */

/**
 * @brief Set control register for KERMIT standard operation and starts seed
 * mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC_KERMIT(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC_KERMIT(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       CRC_CTRL_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TCRC_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_FXOR_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOT_MASK)) & ( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOTR_MASK))))))) | (( \
      CRC_CTRL_WAS_MASK) | (( \
      (uint32_t)((uint32_t)0x1U << CRC_CTRL_TOT_SHIFT)) | ( \
      (uint32_t)((uint32_t)0x2U << CRC_CTRL_TOTR_SHIFT))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCRC_DNP
   ---------------------------------------------------------------------------- */

/**
 * @brief Set control register for DNP standard operation and starts seed mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC_DNP(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC_DNP(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       CRC_CTRL_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TCRC_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOT_MASK)) & ( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOTR_MASK)))))) | (( \
      CRC_CTRL_WAS_MASK) | (( \
      CRC_CTRL_FXOR_MASK) | (( \
      (uint32_t)((uint32_t)0x1U << CRC_CTRL_TOT_SHIFT)) | ( \
      (uint32_t)((uint32_t)0x2U << CRC_CTRL_TOTR_SHIFT)))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCRC_MODBUS16
   ---------------------------------------------------------------------------- */

/**
 * @brief Set control register for MODBUS16 standard operation and starts seed
 * mode.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CRC_CTRL.
 * @par Example:
 *      @code
 *      CRC_PDD_SetCRC_MODBUS16(deviceID);
 *      @endcode
 */
#define CRC_PDD_SetCRC_MODBUS16(PeripheralBase) ( \
    CRC_CTRL_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       CRC_CTRL_REG(PeripheralBase)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TCRC_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_FXOR_MASK)) & (( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOT_MASK)) & ( \
       (uint32_t)(~(uint32_t)CRC_CTRL_TOTR_MASK))))))) | (( \
      CRC_CTRL_WAS_MASK) | (( \
      (uint32_t)((uint32_t)0x1U << CRC_CTRL_TOT_SHIFT)) | ( \
      (uint32_t)((uint32_t)0x2U << CRC_CTRL_TOTR_SHIFT))))) \
  )
#endif  /* #if defined(CRC_PDD_H_) */

/* CRC_PDD.h, eof. */
