/*
  PDD layer implementation for peripheral type FMC
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(FMC_PDD_H_)
#define FMC_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error FMC PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* FMC */ && \
      !defined(MCU_MK10D5) /* FMC */ && \
      !defined(MCU_MK10D7) /* FMC */ && \
      !defined(MCU_MK10F12) /* FMC */ && \
      !defined(MCU_MK10DZ10) /* FMC */ && \
      !defined(MCU_MK11D5) /* FMC */ && \
      !defined(MCU_MK11D5WS) /* FMC */ && \
      !defined(MCU_MK12D5) /* FMC */ && \
      !defined(MCU_MK20D10) /* FMC */ && \
      !defined(MCU_MK20D5) /* FMC */ && \
      !defined(MCU_MK20D7) /* FMC */ && \
      !defined(MCU_MK20F12) /* FMC */ && \
      !defined(MCU_MK20DZ10) /* FMC */ && \
      !defined(MCU_MK21D5) /* FMC */ && \
      !defined(MCU_MK21D5WS) /* FMC */ && \
      !defined(MCU_MK22D5) /* FMC */ && \
      !defined(MCU_MK30D10) /* FMC */ && \
      !defined(MCU_MK30D7) /* FMC */ && \
      !defined(MCU_MK30DZ10) /* FMC */ && \
      !defined(MCU_MK40D10) /* FMC */ && \
      !defined(MCU_MK40D7) /* FMC */ && \
      !defined(MCU_MK40DZ10) /* FMC */ && \
      !defined(MCU_MK40X256VMD100) /* FMC */ && \
      !defined(MCU_MK50D10) /* FMC */ && \
      !defined(MCU_MK50D7) /* FMC */ && \
      !defined(MCU_MK50DZ10) /* FMC */ && \
      !defined(MCU_MK51D10) /* FMC */ && \
      !defined(MCU_MK51D7) /* FMC */ && \
      !defined(MCU_MK51DZ10) /* FMC */ && \
      !defined(MCU_MK52D10) /* FMC */ && \
      !defined(MCU_MK52DZ10) /* FMC */ && \
      !defined(MCU_MK53D10) /* FMC */ && \
      !defined(MCU_MK53DZ10) /* FMC */ && \
      !defined(MCU_MK60D10) /* FMC */ && \
      !defined(MCU_MK60F12) /* FMC */ && \
      !defined(MCU_MK60F15) /* FMC */ && \
      !defined(MCU_MK60DZ10) /* FMC */ && \
      !defined(MCU_MK60N512VMD100) /* FMC */ && \
      !defined(MCU_MK61F12) /* FMC */ && \
      !defined(MCU_MK61F15) /* FMC */ && \
      !defined(MCU_MK61F12WS) /* FMC */ && \
      !defined(MCU_MK61F15WS) /* FMC */ && \
      !defined(MCU_MK70F12) /* FMC */ && \
      !defined(MCU_MK70F15) /* FMC */ && \
      !defined(MCU_MK70F12WS) /* FMC */ && \
      !defined(MCU_MK70F15WS) /* FMC */ && \
      !defined(MCU_PCK20L4) /* FMC */
  // Unsupported MCU is active
  #error FMC PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------
   -- InvalidateFlashCache
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK10F12)) || (defined(MCU_MK20F12)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)))
/**
 * @brief Invalidates flash cache.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FMC_PFB0CR, FMC_PFB01CR
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      FMC_PDD_InvalidateFlashCache(deviceID);
 *      @endcode
 */
  #define FMC_PDD_InvalidateFlashCache(PeripheralBase) ( \
      FMC_PFB01CR_REG(PeripheralBase) |= \
       (uint32_t)(FMC_PFB01CR_CINV_WAY_MASK | FMC_PFB01CR_S_B_INV_MASK) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Invalidates flash cache.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FMC_PFB0CR, FMC_PFB01CR
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      FMC_PDD_InvalidateFlashCache(deviceID);
 *      @endcode
 */
  #define FMC_PDD_InvalidateFlashCache(PeripheralBase) ( \
      FMC_PFB0CR_REG(PeripheralBase) |= \
       (uint32_t)(FMC_PFB0CR_CINV_WAY_MASK | FMC_PFB0CR_S_B_INV_MASK) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_PCK20L4)) */
#endif  /* #if defined(FMC_PDD_H_) */

/* FMC_PDD.h, eof. */
