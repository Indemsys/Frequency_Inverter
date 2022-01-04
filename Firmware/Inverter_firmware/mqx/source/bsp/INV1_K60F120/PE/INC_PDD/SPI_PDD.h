/*
  PDD layer implementation for peripheral type SPI
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(SPI_PDD_H_)
#define SPI_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error SPI PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK10D5) /* SPI0 */ && \
      !defined(MCU_MK10D7) /* SPI0, SPI1 */ && \
      !defined(MCU_MK10F12) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK10DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK11D5) /* SPI0, SPI1 */ && \
      !defined(MCU_MK11D5WS) /* SPI0, SPI1 */ && \
      !defined(MCU_MK12D5) /* SPI0, SPI1 */ && \
      !defined(MCU_MK20D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK20D5) /* SPI0 */ && \
      !defined(MCU_MK20D7) /* SPI0, SPI1 */ && \
      !defined(MCU_MK20F12) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK20DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK21D5) /* SPI0, SPI1 */ && \
      !defined(MCU_MK21D5WS) /* SPI0, SPI1 */ && \
      !defined(MCU_MK22D5) /* SPI0, SPI1 */ && \
      !defined(MCU_MK30D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK30D7) /* SPI0, SPI1 */ && \
      !defined(MCU_MK30DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK40D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK40D7) /* SPI0, SPI1 */ && \
      !defined(MCU_MK40DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK40X256VMD100) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK50D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK50D7) /* SPI0, SPI1 */ && \
      !defined(MCU_MK50DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK51D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK51D7) /* SPI0, SPI1 */ && \
      !defined(MCU_MK51DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK52D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK52DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK53D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK53DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK60D10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK60F12) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK60F15) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK60DZ10) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK60N512VMD100) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK61F12) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK61F15) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK61F12WS) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK61F15WS) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK70F12) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK70F15) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK70F12WS) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MK70F15WS) /* SPI0, SPI1, SPI2 */ && \
      !defined(MCU_MKL02Z4) /* SPI0 */ && \
      !defined(MCU_MKL04Z4) /* SPI0 */ && \
      !defined(MCU_MKL05Z4) /* SPI0 */ && \
      !defined(MCU_MKL14Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL15Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL16Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL24Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL25Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL26Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL34Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL36Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_MKL46Z4) /* SPI0, SPI1 */ && \
      !defined(MCU_PCK20L4) /* SPI0 */
  // Unsupported MCU is active
  #error SPI PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Interrupt/DMA masks */
#define SPI_PDD_TRANSFER_COMPLETE_INT SPI_SR_TCF_MASK /**< Transfer complete interrupt mask. */
#define SPI_PDD_QUEUE_FINISHED_INT    SPI_SR_EOQF_MASK /**< Queue finished interrupt mask. */
#define SPI_PDD_TX_FIFO_UNDERFLOW_INT SPI_SR_TFUF_MASK /**< Transmit FIFO underflow interrupt mask. */
#define SPI_PDD_TX_FIFO_FILL_INT_DMA  SPI_SR_TFFF_MASK /**< Transmit FIFO fill interrupt mask. */
#define SPI_PDD_RX_FIFO_OVERFLOW_INT  SPI_SR_RFOF_MASK /**< Receive FIFO overflow interrupt mask. */
#define SPI_PDD_RX_FIFO_DRAIN_INT_DMA SPI_SR_RFDF_MASK /**< Receive FIFO drain interrupt mask. */
#define SPI_PDD_ALL_INT               0x9A0A0000U /**< All interrupt masks. */

/* Request mask for DMA or interrupt selection */
#define SPI_PDD_NO_DMA            0U             /**< None DMA or interrupt request selection. */
#define SPI_PDD_TX_FIFO_FILL_DMA  SPI_RSER_TFFF_DIRS_MASK /**< Transmit FIFO fill DMA or interrupt request select. */
#define SPI_PDD_RX_FIFO_DRAIN_DMA SPI_RSER_RFDF_DIRS_MASK /**< Receive FIFO drain DMA or interrupt request select. */

/* Rx buffer full (or fault) and Tx buffer empty interrupt masks constant. */
#define SPI_PDD_RX_BUFFER_FULL_OR_FAULT SPI_C1_SPIE_MASK /**< Receiver buffer full and mode fault mask. */
#define SPI_PDD_TX_BUFFER_EMPTY         SPI_C1_SPTIE_MASK /**< Transmitter buffer empty mask. */

/* Status flags constants (for ReadStatusReg, GetInterruptFlags,
   ClearInterruptFlags macros). */
#define SPI_PDD_RX_BUFFER_FULL   SPI_S_SPRF_MASK /**< Read buffer or FIFO full flag. */
#define SPI_PDD_MATCH            SPI_S_SPMF_MASK /**< SPI match flag. */
#define SPI_PDD_TX_BUFFER_EMPTYG SPI_S_SPTEF_MASK /**< Transmit buffer or FIFO empty flag. */
#define SPI_PDD_MASTER_FAULT     SPI_S_MODF_MASK /**< Master mode fault flag. */

/* Status flags constants (for ReadStatusReg, GetInterruptFlags,
   ClearInterruptFlags macros). */
#define SPI_PDD_RX_FIFO_NEARLY_FULL  SPI_S_RNFULLF_MASK /**< Receive FIFO nearly full flag. */
#define SPI_PDD_TX_FIFO_NEARLY_EMPTY SPI_S_TNEAREF_MASK /**< Transmit FIFO nearly empty flag. */
#define SPI_PDD_TX_BUFFER_FULL       SPI_S_TXFULLF_MASK /**< Transmit FIFO full flag. */
#define SPI_PDD_READ_FIFO_EMPTY      SPI_S_RFIFOEF_MASK /**< SPI read FIFO empty flag. */

/* FIFO interrupt masks */
#define SPI_PDD_TRANSMIT_FIFO_EMPTY   SPI_C3_TNEARIEN_MASK /**< Transmit FIFO nearly empty mask. */
#define SPI_PDD_RECEIVE_FIFO_FULL     SPI_C3_RNFULLIEN_MASK /**< Receive FIFO nearly full mask. */
#define SPI_PDD_TRANSMIT_RECEIVE_FIFO 0x6U       /**< Receive FIFO nearly full 'or' transmit FIFO nearly empty masks. */

/* Status flags constants (for ReadStatusReg, GetInterruptFlags,
   ClearInterruptFlags macros). */
#define SPI_PDD_TX_FIFO_ERROR    SPI_CI_TXFERR_MASK /**< Transmit FIFO error flag. */
#define SPI_PDD_RX_FIFO_ERROR    SPI_CI_RXFERR_MASK /**< Receive FIFO error flag. */
#define SPI_PDD_TX_FIFO_OVERFLOW SPI_CI_TXFOF_MASK /**< Transmit FIFO overflow flag. */
#define SPI_PDD_RX_FIFO_OVERFLOW SPI_CI_RXFOF_MASK /**< Receive FIFO overflow flag. */

/* FIFO interrupt masks */
#define SPI_PDD_TX_FIFO_NEARLY_EMPTY_FLAG SPI_CI_TNEAREFCI_MASK /**< Transmit FIFO nearly empty flag mask. */
#define SPI_PDD_RX_FIFO_NEARLY_FULL_FLAG  SPI_CI_RNFULLFCI_MASK /**< Receive FIFO nearly full flag mask. */
#define SPI_PDD_TX_FIFO_EMPTY_FLAG        SPI_CI_SPTEFCI_MASK /**< Transmit FIFO empty flag mask. */
#define SPI_PDD_RX_FIFO_FULL_FLAG         SPI_CI_SPRFCI_MASK /**< Receive FIFO full flag mask. */
#define SPI_PDD_CLEAR_ALL_FIFO_FLAGS      0xFU   /**< All FIFO flags masks. */

/* SPI mode constants (for SetMasterSlaveMode). */
#define SPI_PDD_MASTER_MODE 0x1U                 /**< Master mode. */
#define SPI_PDD_SLAVE_MODE  0U                   /**< Slave mode. */

/* SPI clock polarity constants (for SetClockPolarity macro). */
#define SPI_PDD_ACTIVE_HIGH 0U                   /**< Active-high SPI clock (idles low). */
#define SPI_PDD_ACTIVE_LOW  0x8U                 /**< Active-low SPI clock (idles high). */

/* SPI clock phase constants (for SetClockPhase macro). */
#define SPI_PDD_FIRST_EDGE  0U                   /**< First edge on SPSCK occurs at the middle of the first cycle of a data transfer. */
#define SPI_PDD_SECOND_EDGE 0x4U                 /**< First edge on SPSCK occurs at the start of the first cycle of a data transfer. */

/* SPI slave select pin function constants (for SetSlaveSelectPinFunction
   macro). */
#define SPI_PDD_SS_AS_GPIO          0U           /**< Slave select pin configured as GPIO. */
#define SPI_PDD_SS_FOR_FAULT_DETECT 0x1U         /**< Slave select pin configured for fault detection. */
#define SPI_PDD_SS_AUTOMATIC_OUTPUT 0x2U         /**< Slave select pin configured for automatic SPI output. */

/* SPI data shift order constants (for SetDataShiftOrder macro). */
#define SPI_PDD_LSB_FIRST 0x1U                   /**< Data transfers start with least significant bit. */
#define SPI_PDD_MSB_FIRST 0U                     /**< Data transfers start with most significant bit. */

#if ((defined(MCU_MKL16Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/* SPI data length constants (for SetWordLength, GetWordLength macro). */
  #define SPI_PDD_8_BIT  0U                        /**< 8-bit SPI shift register, match register and buffers. */
  #define SPI_PDD_16_BIT 0x40U                     /**< 16-bit SPI shift register, match register and buffers. */

#else /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */
/* SPI data length constants (for SetWordLength, GetWordLength macro). */
  #define SPI_PDD_8_BIT  0U                        /**< 8-bit SPI shift register, match register and buffers. */
  #define SPI_PDD_16_BIT 0x1U                      /**< 16-bit SPI shift register, match register and buffers. */

#endif /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */
/* Transmit FIFO nearly empty watermark constants (for SetTxFifoEmptyWatermark
   macro). */
#define SPI_PDD_16_BITS_OR_LESS 0U               /**< 16 bits or less. */
#define SPI_PDD_32_BITS_OR_LESS 0x20U            /**< 32 bits or less. */

/* Receive FIFO nearly full watermark constants (for SetRxFifoFullWatermark
   macro). */
#define SPI_PDD_32_BITS_OR_MORE 0x10U            /**< 32 bits or more. */
#define SPI_PDD_48_BITS_OR_MORE 0U               /**< 48 bits or more. */


/* ----------------------------------------------------------------------------
   -- SetMasterSlaveMode
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Select master or slave mode.
 * @param PeripheralBase Peripheral base address.
 * @param Mode SPI mode value. The user should use one from the enumerated
 *        values. This parameter is of "SPI mode constants (for SetMasterSlaveMode)."
 *        type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR, SPI0_C1, SPI1_C1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetMasterSlaveMode(deviceID, SPI_PDD_MASTER_MODE);
 *      @endcode
 */
  #define SPI_PDD_SetMasterSlaveMode(PeripheralBase, Mode) ( \
      SPI_C1_REG(PeripheralBase) = \
       (uint8_t)(( \
        (uint8_t)(SPI_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C1_MSTR_MASK))) | ( \
        (uint8_t)((uint8_t)(Mode) << SPI_C1_MSTR_SHIFT))) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Select master or slave mode.
 * @param PeripheralBase Peripheral base address.
 * @param Mode Device mode. This parameter is of "SPI mode constants (for
 *        SetMasterSlaveMode)." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR, SPI0_C1, SPI1_C1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetMasterSlaveMode(deviceID, SPI_PDD_MASTER_MODE);
 *      @endcode
 */
  #define SPI_PDD_SetMasterSlaveMode(PeripheralBase, Mode) ( \
      SPI_MCR_REG(PeripheralBase) = \
       (uint32_t)(( \
        (uint32_t)(SPI_MCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)SPI_MCR_MSTR_MASK))) | ( \
        (uint32_t)((uint32_t)(Mode) << SPI_MCR_MSTR_SHIFT))) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- EnableContinuousClock
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables continuous clock.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of continuous clock. This parameter is of
 *        "Global enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableContinuousClock(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableContinuousClock(PeripheralBase, State) ( \
    SPI_MCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(SPI_MCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)SPI_MCR_CONT_SCKE_MASK))) | ( \
      (uint32_t)((uint32_t)(State) << SPI_MCR_CONT_SCKE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDevice
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Enables/disables SPI device.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of device. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE and
 *        PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR, SPI0_C1, SPI1_C1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
  #define SPI_PDD_EnableDevice(PeripheralBase, State) ( \
      SPI_C1_REG(PeripheralBase) = \
       (uint8_t)(( \
        (uint8_t)(SPI_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C1_SPE_MASK))) | ( \
        (uint8_t)((uint8_t)(State) << SPI_C1_SPE_SHIFT))) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Enables the device.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of device. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE and
 *        PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR, SPI0_C1, SPI1_C1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
  #define SPI_PDD_EnableDevice(PeripheralBase, State) ( \
      ((State) == PDD_DISABLE) ? ( \
        SPI_MCR_REG(PeripheralBase) |= \
         SPI_MCR_MDIS_MASK) : ( \
        SPI_MCR_REG(PeripheralBase) &= \
         (uint32_t)(~(uint32_t)SPI_MCR_MDIS_MASK)) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- ClearTxFIFO
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Tx FIFO.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_ClearTxFIFO(deviceID);
 *      @endcode
 */
#define SPI_PDD_ClearTxFIFO(PeripheralBase) ( \
    SPI_MCR_REG(PeripheralBase) |= \
     SPI_MCR_CLR_TXF_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearRxFIFO
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Rx FIFO.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_ClearRxFIFO(deviceID);
 *      @endcode
 */
#define SPI_PDD_ClearRxFIFO(PeripheralBase) ( \
    SPI_MCR_REG(PeripheralBase) |= \
     SPI_MCR_CLR_RXF_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableHaltMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables halt mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of halt mode. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE
 *        and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableHaltMode(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableHaltMode(PeripheralBase, State) ( \
    SPI_MCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(SPI_MCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)SPI_MCR_HALT_MASK))) | ( \
      (uint32_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteModuleConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Module configuration register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the module configuration register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteModuleConfigurationReg(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteModuleConfigurationReg(PeripheralBase, Value) ( \
    SPI_MCR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadModuleConfigurationReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Module configuration register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SPI0_MCR, SPI1_MCR,
 *          SPI2_MCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_ReadModuleConfigurationReg(deviceID);
 *      @endcode
 */
#define SPI_PDD_ReadModuleConfigurationReg(PeripheralBase) ( \
    SPI_MCR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetTransferCounter
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets transfer counter.
 * @param PeripheralBase Peripheral base address.
 * @param Value Parameter specifying new value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_TCR, SPI1_TCR,
 *          SPI2_TCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetTransferCounter(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_SetTransferCounter(PeripheralBase, Value) ( \
    SPI_TCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(SPI_TCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)SPI_TCR_SPI_TCNT_MASK))) | ( \
      (uint32_t)((uint32_t)(Value) << SPI_TCR_SPI_TCNT_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetTransferCounter
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns transfer counter.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: SPI0_TCR, SPI1_TCR,
 *          SPI2_TCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint16_t result = SPI_PDD_GetTransferCounter(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetTransferCounter(PeripheralBase) ( \
    (uint16_t)(( \
     (uint32_t)(SPI_TCR_REG(PeripheralBase) & SPI_TCR_SPI_TCNT_MASK)) >> ( \
     SPI_TCR_SPI_TCNT_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTransferCountReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Transfer count register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the transfer count register. This parameter is a
 *        32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_TCR, SPI1_TCR,
 *          SPI2_TCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteTransferCountReg(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteTransferCountReg(PeripheralBase, Value) ( \
    SPI_TCR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadTransferCountReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Transfer count register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SPI0_TCR, SPI1_TCR,
 *          SPI2_TCR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_ReadTransferCountReg(deviceID);
 *      @endcode
 */
#define SPI_PDD_ReadTransferCountReg(PeripheralBase) ( \
    SPI_TCR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteMasterClockTransferAttributeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value intended for master mode to the Clock transfer attribute
 * register.
 * @param PeripheralBase Peripheral base address.
 * @param Index Attribute index. This parameter is of index type.
 * @param Value Value stored to the master clock transfer attribute register.
 *        This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CTAR[Index].
 * @par Example:
 *      @code
 *      SPI_PDD_WriteMasterClockTransferAttributeReg(deviceID, periphID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteMasterClockTransferAttributeReg(PeripheralBase, Index, Value) ( \
    SPI_CTAR_REG(PeripheralBase,(Index)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadMasterClockTransferAttributeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Clock transfer attribute register for
 * master mode.
 * @param PeripheralBase Peripheral base address.
 * @param Index Attribute index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CTAR[Index].
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_ReadMasterClockTransferAttributeReg(deviceID,
 *      periphID);
 *      @endcode
 */
#define SPI_PDD_ReadMasterClockTransferAttributeReg(PeripheralBase, Index) ( \
    SPI_CTAR_REG(PeripheralBase,(Index)) \
  )

/* ----------------------------------------------------------------------------
   -- WriteSlaveClockTransferAttributeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value intended for slave mode to the Clock transfer attribute
 * register.
 * @param PeripheralBase Peripheral base address.
 * @param Index Attribute index. This parameter is of index type.
 * @param Value Value stored to the slave clock transfer attribute register.
 *        This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: CTAR_SLAVE[Index].
 * @par Example:
 *      @code
 *      SPI_PDD_WriteSlaveClockTransferAttributeReg(deviceID, periphID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteSlaveClockTransferAttributeReg(PeripheralBase, Index, Value) ( \
    SPI_CTAR_SLAVE_REG(PeripheralBase,(Index)) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadSlaveClockTransferAttributeReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Clock transfer attribute register for slave
 * mode.
 * @param PeripheralBase Peripheral base address.
 * @param Index Attribute index. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: CTAR_SLAVE[Index].
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_ReadSlaveClockTransferAttributeReg(deviceID,
 *      periphID);
 *      @endcode
 */
#define SPI_PDD_ReadSlaveClockTransferAttributeReg(PeripheralBase, Index) ( \
    SPI_CTAR_SLAVE_REG(PeripheralBase,(Index)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDmasInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables DMA/interrupt requests defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of DMA/interrupt requests. Use constants from group
 *        "Interrupt/DMA masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_RSER, SPI1_RSER,
 *          SPI2_RSER (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableDmasInterrupts(deviceID, SPI_PDD_TRANSFER_COMPLETE_INT);
 *      @endcode
 */
#define SPI_PDD_EnableDmasInterrupts(PeripheralBase, Mask) ( \
    SPI_RSER_REG(PeripheralBase) |= \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableDmasInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables DMA/interrupt requests defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of DMA/interrupt requests. Use constants from group
 *        "Interrupt/DMA masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_RSER, SPI1_RSER,
 *          SPI2_RSER (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_DisableDmasInterrupts(deviceID, SPI_PDD_TRANSFER_COMPLETE_INT);
 *      @endcode
 */
#define SPI_PDD_DisableDmasInterrupts(PeripheralBase, Mask) ( \
    SPI_RSER_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- SelectDmasInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Selects DMA or interrupt for request defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of DMA/interrupt requests. Use constants from group "Request
 *        mask for DMA or interrupt selection". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_RSER, SPI1_RSER,
 *          SPI2_RSER (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SelectDmasInterrupts(deviceID, SPI_PDD_NO_DMA);
 *      @endcode
 */
#define SPI_PDD_SelectDmasInterrupts(PeripheralBase, Mask) ( \
    SPI_RSER_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       SPI_RSER_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)(SPI_RSER_TFFF_DIRS_MASK | SPI_RSER_RFDF_DIRS_MASK))))) | ( \
      (uint32_t)(Mask))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteDmaInterruptEnableReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the DMA interrupt enable register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the DMA interrupt enable register. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_RSER, SPI1_RSER,
 *          SPI2_RSER (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteDmaInterruptEnableReg(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteDmaInterruptEnableReg(PeripheralBase, Value) ( \
    SPI_RSER_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadDmaInterruptEnableReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the DMA interrupt enable register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SPI0_RSER, SPI1_RSER,
 *          SPI2_RSER (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_ReadDmaInterruptEnableReg(deviceID);
 *      @endcode
 */
#define SPI_PDD_ReadDmaInterruptEnableReg(PeripheralBase) ( \
    SPI_RSER_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt flags.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Interrupt/DMA masks" for processing return
 *         value.
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_GetInterruptFlags(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetInterruptFlags(PeripheralBase) ( \
    (uint32_t)(( \
     SPI_SR_REG(PeripheralBase)) & ( \
     (uint32_t)(( \
      SPI_SR_TCF_MASK) | (( \
      SPI_SR_EOQF_MASK) | (( \
      SPI_SR_TFUF_MASK) | (( \
      SPI_SR_TFFF_MASK) | (( \
      SPI_SR_RFOF_MASK) | ( \
      SPI_SR_RFDF_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears interrupt flag bits defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupt flags to clear. Use constants from group
 *        "Interrupt/DMA masks". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_ClearInterruptFlags(deviceID, SPI_PDD_TRANSFER_COMPLETE_INT);
 *      @endcode
 */
#define SPI_PDD_ClearInterruptFlags(PeripheralBase, Mask) ( \
    SPI_SR_REG(PeripheralBase) = \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxRxActiveFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns Tx/Rx active flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_GetTxRxActiveFlag(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetTxRxActiveFlag(PeripheralBase) ( \
    (uint32_t)(SPI_SR_REG(PeripheralBase) & SPI_SR_TXRXS_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxFIFOCounter
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns transmit FIFO counter.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_GetTxFIFOCounter(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetTxFIFOCounter(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(SPI_SR_REG(PeripheralBase) & SPI_SR_TXCTR_MASK)) >> ( \
     SPI_SR_TXCTR_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetRxFIFOCounter
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns receive FIFO counter.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_GetRxFIFOCounter(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetRxFIFOCounter(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(SPI_SR_REG(PeripheralBase) & SPI_SR_RXCTR_MASK)) >> ( \
     SPI_SR_RXCTR_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetTxNextPointer
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns pointer to TX FIFO entry which is transmitted during the next
 * transfer.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_GetTxNextPointer(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetTxNextPointer(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(SPI_SR_REG(PeripheralBase) & SPI_SR_TXNXTPTR_MASK)) >> ( \
     SPI_SR_TXNXTPTR_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GetRxNextPointer
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns pointer to RX FIFO entry which is transmitted during the next
 * transfer.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 4-bit value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_GetRxNextPointer(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetRxNextPointer(PeripheralBase) ( \
    (uint8_t)(SPI_SR_REG(PeripheralBase) & SPI_SR_POPNXTPTR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- WriteStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Status register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the status register. This parameter is a 32-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteStatusReg(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteStatusReg(PeripheralBase, Value) ( \
    SPI_SR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadStatusReg
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL16Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Returns the value of the status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Status flags constants (for ReadStatusReg,
 *         GetInterruptFlags, ClearInterruptFlags macros)." for processing return
 *         value.
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR, SPI0_S, SPI1_S (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_ReadStatusReg(deviceID);
 *      @endcode
 */
  #define SPI_PDD_ReadStatusReg(PeripheralBase) ( \
      SPI_S_REG(PeripheralBase) \
    )
#else /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Returns the content of the Status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SPI0_SR, SPI1_SR,
 *          SPI2_SR, SPI0_S, SPI1_S (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_ReadStatusReg(deviceID);
 *      @endcode
 */
  #define SPI_PDD_ReadStatusReg(PeripheralBase) ( \
      SPI_SR_REG(PeripheralBase) \
    )
#endif /* (defined(MCU_MK10D10)) || (defined(MCU_MK10D5)) || (defined(MCU_MK10D7)) || (defined(MCU_MK10DZ10)) || (defined(MCU_MK10F12)) || (defined(MCU_MK11D5)) || (defined(MCU_MK11D5WS)) || (defined(MCU_MK12D5)) || (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20DZ10)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK30D10)) || (defined(MCU_MK30D7)) || (defined(MCU_MK30DZ10)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52D10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- WriteMasterPushTxFIFOReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value intended for master mode to the Push TX FIFO register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the master push Tx FIFO register. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_PUSHR, SPI1_PUSHR,
 *          SPI2_PUSHR (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteMasterPushTxFIFOReg(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteMasterPushTxFIFOReg(PeripheralBase, Value) ( \
    SPI_PUSHR_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteSlavePushTxFIFOReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value intended for slave mode to the Push TX FIFO register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the slave push Tx FIFO register. This parameter
 *        is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_PUSHR_SLAVE,
 *          SPI1_PUSHR_SLAVE, SPI2_PUSHR_SLAVE (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteSlavePushTxFIFOReg(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteSlavePushTxFIFOReg(PeripheralBase, Value) ( \
    SPI_PUSHR_SLAVE_REG(PeripheralBase) = \
     (uint32_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadPopRxFIFOReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Pop Rx FIFO register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: SPI0_POPR, SPI1_POPR,
 *          SPI2_POPR (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint32_t result = SPI_PDD_ReadPopRxFIFOReg(deviceID);
 *      @endcode
 */
#define SPI_PDD_ReadPopRxFIFOReg(PeripheralBase) ( \
    SPI_POPR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables interrupt requests defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupt requests. Use constants from group "Rx buffer
 *        full (or fault) and Tx buffer empty interrupt masks constant.". This
 *        parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C1, SPI1_C1
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableInterruptMask(deviceID, SPI_PDD_RX_BUFFER_FULL_OR_FAULT);
 *      @endcode
 */
#define SPI_PDD_EnableInterruptMask(PeripheralBase, Mask) ( \
    SPI_C1_REG(PeripheralBase) |= \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables interrupt requests defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupt requests. Use constants from group "Rx buffer
 *        full (or fault) and Tx buffer empty interrupt masks constant.". This
 *        parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C1, SPI1_C1
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_DisableInterruptMask(deviceID, SPI_PDD_RX_BUFFER_FULL_OR_FAULT);
 *      @endcode
 */
#define SPI_PDD_DisableInterruptMask(PeripheralBase, Mask) ( \
    SPI_C1_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- SetClockPolarity
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the SPI clock polarity.
 * @param PeripheralBase Peripheral base address.
 * @param Polarity SPI polarity value. The user should use one from the
 *        enumerated values. This parameter is of "SPI clock polarity constants (for
 *        SetClockPolarity macro)." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C1, SPI1_C1
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetClockPolarity(deviceID, SPI_PDD_ACTIVE_HIGH);
 *      @endcode
 */
#define SPI_PDD_SetClockPolarity(PeripheralBase, Polarity) ( \
    SPI_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C1_CPOL_MASK))) | ( \
      (uint8_t)(Polarity))) \
  )

/* ----------------------------------------------------------------------------
   -- SetClockPhase
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the SPI clock phase.
 * @param PeripheralBase Peripheral base address.
 * @param Phase SPI phase value. The user should use one from the enumerated
 *        values. This parameter is of "SPI clock phase constants (for SetClockPhase
 *        macro)." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C1, SPI1_C1
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetClockPhase(deviceID, SPI_PDD_FIRST_EDGE);
 *      @endcode
 */
#define SPI_PDD_SetClockPhase(PeripheralBase, Phase) ( \
    SPI_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C1_CPHA_MASK))) | ( \
      (uint8_t)(Phase))) \
  )

/* ----------------------------------------------------------------------------
   -- SetSlaveSelectPinFunction
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the SPI slave select pin function.
 * @param PeripheralBase Peripheral base address.
 * @param Function Slave select pin function. The user should use one from the
 *        enumerated values. This parameter is of "SPI slave select pin function
 *        constants (for SetSlaveSelectPinFunction macro)." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C1, SPI0_C2,
 *          SPI1_C1, SPI1_C2 (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetSlaveSelectPinFunction(deviceID, SPI_PDD_SS_AS_GPIO);
 *      @endcode
 */
#define SPI_PDD_SetSlaveSelectPinFunction(PeripheralBase, Function) ( \
    ((Function) == SPI_PDD_SS_AS_GPIO) ? ( \
      (SPI_C1_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)SPI_C1_SSOE_MASK)), \
      (SPI_C2_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)SPI_C2_MODFEN_MASK))) : ( \
      ((Function) == SPI_PDD_SS_FOR_FAULT_DETECT) ? ( \
       (SPI_C1_REG(PeripheralBase) &= \
        (uint8_t)(~(uint8_t)SPI_C1_SSOE_MASK)), \
       (SPI_C2_REG(PeripheralBase) |= \
        SPI_C2_MODFEN_MASK)) : ( \
       (SPI_C1_REG(PeripheralBase) |= \
        SPI_C1_SSOE_MASK), \
       (SPI_C2_REG(PeripheralBase) |= \
        SPI_C2_MODFEN_MASK)) \
     ) \
  )

/* ----------------------------------------------------------------------------
   -- SetDataShiftOrder
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the SPI data shift order.
 * @param PeripheralBase Peripheral base address.
 * @param Order SPI data shift order value. The user should use one from the
 *        enumerated values. This parameter is of "SPI data shift order constants
 *        (for SetDataShiftOrder macro)." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C1, SPI1_C1
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetDataShiftOrder(deviceID, SPI_PDD_LSB_FIRST);
 *      @endcode
 */
#define SPI_PDD_SetDataShiftOrder(PeripheralBase, Order) ( \
    SPI_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C1_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C1_LSBFE_MASK))) | ( \
      (uint8_t)(Order))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDataFeatures
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets data transmission features(shift order, clock polarity and phase)
 * defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of data features requests. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C1, SPI1_C1
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetDataFeatures(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_SetDataFeatures(PeripheralBase, Mask) ( \
    SPI_C1_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       SPI_C1_REG(PeripheralBase)) & ( \
       (uint8_t)(~(uint8_t)(SPI_C1_LSBFE_MASK | (SPI_C1_CPOL_MASK | SPI_C1_CPHA_MASK)))))) | ( \
      (uint8_t)(Mask))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableMatchInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables receive data buffer hardware match interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableMatchInterrupt(deviceID);
 *      @endcode
 */
#define SPI_PDD_EnableMatchInterrupt(PeripheralBase) ( \
    SPI_C2_REG(PeripheralBase) |= \
     SPI_C2_SPMIE_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableMatchInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables receive data buffer hardware match interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_DisableMatchInterrupt(deviceID);
 *      @endcode
 */
#define SPI_PDD_DisableMatchInterrupt(PeripheralBase) ( \
    SPI_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)SPI_C2_SPMIE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableOutputInBidirectionalMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables pin direction in a bidirectional mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of output pin in bidirectional mode. This
 *        parameter is of "Global enumeration used for specifying general
 *        enable/disable states (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableOutputInBidirectionalMode(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableOutputInBidirectionalMode(PeripheralBase, State) ( \
    SPI_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C2_BIDIROE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << SPI_C2_BIDIROE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableOperateInWaitMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables operate in wait mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of device in wait mode. This parameter is of
 *        "Global enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableOperateInWaitMode(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableOperateInWaitMode(PeripheralBase, State) ( \
    ((State) == PDD_DISABLE) ? ( \
      SPI_C2_REG(PeripheralBase) |= \
       SPI_C2_SPISWAI_MASK) : ( \
      SPI_C2_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)SPI_C2_SPISWAI_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableBidirectionalMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables bidirectional mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of bidirectional mode. This parameter is of
 *        "Global enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableBidirectionalMode(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableBidirectionalMode(PeripheralBase, State) ( \
    SPI_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C2_SPC0_MASK))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- SetBaudRatePrescaler
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the SPI baud rate prescale divisor.
 * @param PeripheralBase Peripheral base address.
 * @param Prescaler Baud rate prescale divisor value[0..7]. This parameter is a
 *        3-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_BR, SPI1_BR
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetBaudRatePrescaler(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_SetBaudRatePrescaler(PeripheralBase, Prescaler) ( \
    SPI_BR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_BR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_BR_SPPR_MASK))) | ( \
      (uint8_t)((uint8_t)(Prescaler) << SPI_BR_SPPR_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetBaudRateDivisor
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the SPI baud rate divisor.
 * @param PeripheralBase Peripheral base address.
 * @param Divisor Baud rate divisor value[0..15]. This parameter is a 4-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_BR, SPI1_BR
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetBaudRateDivisor(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_SetBaudRateDivisor(PeripheralBase, Divisor) ( \
    SPI_BR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_BR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_BR_SPR_MASK))) | ( \
      (uint8_t)(Divisor))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteBaudRateReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the baud rate register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value stored to the baud rate register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_BR, SPI1_BR
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteBaudRateReg(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteBaudRateReg(PeripheralBase, Value) ( \
    SPI_BR_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteData8Bit
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL16Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Writes 8 bit data value to the data register.
 * @param PeripheralBase Peripheral base address.
 * @param Data 8 bit data value. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_D, SPI1_D, SPI0_DL,
 *          SPI1_DL (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteData8Bit(deviceID, 1);
 *      @endcode
 */
  #define SPI_PDD_WriteData8Bit(PeripheralBase, Data) ( \
      SPI_DL_REG(PeripheralBase) = \
       (uint8_t)(Data) \
    )
#else /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */
/**
 * @brief Writes 8 bit data value to the data register.
 * @param PeripheralBase Peripheral base address.
 * @param Data 8 bit data value. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_D, SPI1_D, SPI0_DL,
 *          SPI1_DL (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteData8Bit(deviceID, 1);
 *      @endcode
 */
  #define SPI_PDD_WriteData8Bit(PeripheralBase, Data) ( \
      SPI_D_REG(PeripheralBase) = \
       (uint8_t)(Data) \
    )
#endif /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */

/* ----------------------------------------------------------------------------
   -- ReadData8bit
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL16Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Returns the content of the 8 bit data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: SPI0_D, SPI1_D, SPI0_DL,
 *          SPI1_DL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_ReadData8bit(deviceID);
 *      @endcode
 */
  #define SPI_PDD_ReadData8bit(PeripheralBase) ( \
      SPI_DL_REG(PeripheralBase) \
    )
#else /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */
/**
 * @brief Returns the content of the 8 bit data register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: SPI0_D, SPI1_D, SPI0_DL,
 *          SPI1_DL (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_ReadData8bit(deviceID);
 *      @endcode
 */
  #define SPI_PDD_ReadData8bit(PeripheralBase) ( \
      SPI_D_REG(PeripheralBase) \
    )
#endif /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */

/* ----------------------------------------------------------------------------
   -- SetMatch8BitValue
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MKL16Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL34Z4)) || (defined(MCU_MKL36Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Writes 8 bit match value to the match register.
 * @param PeripheralBase Peripheral base address.
 * @param Value 8 bit match value. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_M, SPI1_M, SPI0_ML,
 *          SPI1_ML (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetMatch8BitValue(deviceID, 1);
 *      @endcode
 */
  #define SPI_PDD_SetMatch8BitValue(PeripheralBase, Value) ( \
      SPI_ML_REG(PeripheralBase) = \
       (uint8_t)(Value) \
    )
#else /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */
/**
 * @brief Writes 8 bit match value to the match register.
 * @param PeripheralBase Peripheral base address.
 * @param Value 8 bit match value. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_M, SPI1_M, SPI0_ML,
 *          SPI1_ML (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetMatch8BitValue(deviceID, 1);
 *      @endcode
 */
  #define SPI_PDD_SetMatch8BitValue(PeripheralBase, Value) ( \
      SPI_M_REG(PeripheralBase) = \
       (uint8_t)(Value) \
    )
#endif /* (defined(MCU_MKL02Z4)) || (defined(MCU_MKL04Z4)) || (defined(MCU_MKL05Z4)) || (defined(MCU_MKL14Z4)) || (defined(MCU_MKL15Z4)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) */

/* ----------------------------------------------------------------------------
   -- EnableTransmitDma
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables a transmit DMA request.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of transmit DMA request. This parameter is of
 *        "Global enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableTransmitDma(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableTransmitDma(PeripheralBase, State) ( \
    SPI_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C2_TXDMAE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << SPI_C2_TXDMAE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableReceiveDma
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables a receive DMA request.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of receive DMA request. This parameter is of
 *        "Global enumeration used for specifying general enable/disable states
 *        (PDD_DISABLE and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_EnableReceiveDma(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableReceiveDma(PeripheralBase, State) ( \
    SPI_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C2_RXDMAE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << SPI_C2_RXDMAE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetWordLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the SPI word length.
 * @param PeripheralBase Peripheral base address.
 * @param Length SPI data length value. The user should use one from the
 *        enumerated values. This parameter is of "SPI data length constants (for
 *        SetWordLength, GetWordLength macro)." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetWordLength(deviceID, SPI_PDD_8_BIT);
 *      @endcode
 */
#define SPI_PDD_SetWordLength(PeripheralBase, Length) ( \
    SPI_C2_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C2_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C2_SPIMODE_MASK))) | ( \
      (uint8_t)(Length))) \
  )

/* ----------------------------------------------------------------------------
   -- GetWordLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the current data word length.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of "SPI data length constants (for SetWordLength,
 *         GetWordLength macro)." type. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: SPI0_C2, SPI1_C2
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_GetWordLength(deviceID);
 *      @endcode
 */
#define SPI_PDD_GetWordLength(PeripheralBase) ( \
    (uint8_t)(SPI_C2_REG(PeripheralBase) & SPI_C2_SPIMODE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- WriteData16Bit
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes 16 bit data value to the data registers.
 * @param PeripheralBase Peripheral base address.
 * @param Data 16 bit data value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_DL, SPI0_DH,
 *          SPI1_DL, SPI1_DH (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_WriteData16Bit(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_WriteData16Bit(PeripheralBase, Data) ( \
    (SPI_DL_REG(PeripheralBase) = \
     (uint8_t)(Data)), \
    (SPI_DH_REG(PeripheralBase) = \
     (uint8_t)((uint16_t)(Data) >> 8U)) \
  )

/* ----------------------------------------------------------------------------
   -- ReadData16bit
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the 16 bit value of the data registers.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 16-bit value.
 * @remarks The macro accesses the following registers: SPI0_DL, SPI0_DH,
 *          SPI1_DL, SPI1_DH (depending on the peripheral).
 * @par Example:
 *      @code
 *      uint16_t result = SPI_PDD_ReadData16bit(deviceID);
 *      @endcode
 */
#define SPI_PDD_ReadData16bit(PeripheralBase) ( \
    (uint16_t)(( \
     SPI_DL_REG(PeripheralBase)) | ( \
     (uint16_t)((uint16_t)SPI_DH_REG(PeripheralBase) << 8U))) \
  )

/* ----------------------------------------------------------------------------
   -- SetMatch16BitValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes 16 bit match value to the match registers.
 * @param PeripheralBase Peripheral base address.
 * @param Value 16 bit match value. This parameter is a 16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI0_ML, SPI0_MH,
 *          SPI1_ML, SPI1_MH (depending on the peripheral).
 * @par Example:
 *      @code
 *      SPI_PDD_SetMatch16BitValue(deviceID, 1);
 *      @endcode
 */
#define SPI_PDD_SetMatch16BitValue(PeripheralBase, Value) ( \
    (SPI_ML_REG(PeripheralBase) = \
     (uint8_t)(Value)), \
    (SPI_MH_REG(PeripheralBase) = \
     (uint8_t)((uint16_t)(Value) >> 8U)) \
  )

/* ----------------------------------------------------------------------------
   -- SetTxFifoEmptyWatermark
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the value of transmit FIFO nearly empty watermark.
 * @param PeripheralBase Peripheral base address.
 * @param Value Transmit FIFO nearly empty watermark value. The user should use
 *        one from the enumerated values. This parameter is of "Transmit FIFO
 *        nearly empty watermark constants (for SetTxFifoEmptyWatermark macro)."
 *        type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI1_C3.
 * @par Example:
 *      @code
 *      SPI_PDD_SetTxFifoEmptyWatermark(deviceID, SPI_PDD_16_BITS_OR_LESS);
 *      @endcode
 */
#define SPI_PDD_SetTxFifoEmptyWatermark(PeripheralBase, Value) ( \
    SPI_C3_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C3_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C3_TNEAREF_MARK_MASK))) | ( \
      (uint8_t)(Value))) \
  )

/* ----------------------------------------------------------------------------
   -- SetRxFifoFullWatermark
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets the value of receive FIFO nearly full watermark.
 * @param PeripheralBase Peripheral base address.
 * @param Value Receive FIFO nearly full watermark value. The user should use
 *        one from the enumerated values. This parameter is of "Receive FIFO nearly
 *        full watermark constants (for SetRxFifoFullWatermark macro)." type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI1_C3.
 * @par Example:
 *      @code
 *      SPI_PDD_SetRxFifoFullWatermark(deviceID, SPI_PDD_32_BITS_OR_MORE);
 *      @endcode
 */
#define SPI_PDD_SetRxFifoFullWatermark(PeripheralBase, Value) ( \
    SPI_C3_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C3_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C3_RNFULLF_MARK_MASK))) | ( \
      (uint8_t)(Value))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableFifoInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables FIFO interrupt requests defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of FIFO interrupt requests. Use constants from group "FIFO
 *        interrupt masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI1_C3.
 * @par Example:
 *      @code
 *      SPI_PDD_EnableFifoInterruptMask(deviceID, SPI_PDD_TRANSMIT_FIFO_EMPTY);
 *      @endcode
 */
#define SPI_PDD_EnableFifoInterruptMask(PeripheralBase, Mask) ( \
    SPI_C3_REG(PeripheralBase) |= \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableFifoInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables FIFO interrupt requests defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of FIFO interrupt requests. Use constants from group "FIFO
 *        interrupt masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI1_C3.
 * @par Example:
 *      @code
 *      SPI_PDD_DisableFifoInterruptMask(deviceID, SPI_PDD_TRANSMIT_FIFO_EMPTY);
 *      @endcode
 */
#define SPI_PDD_DisableFifoInterruptMask(PeripheralBase, Mask) ( \
    SPI_C3_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- EnableFifoMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables FIFO mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state of FIFO mode. This parameter is of "Global
 *        enumeration used for specifying general enable/disable states (PDD_DISABLE
 *        and PDD_ENABLE defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI1_C3.
 * @par Example:
 *      @code
 *      SPI_PDD_EnableFifoMode(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define SPI_PDD_EnableFifoMode(PeripheralBase, State) ( \
    SPI_C3_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(SPI_C3_REG(PeripheralBase) & (uint8_t)(~(uint8_t)SPI_C3_FIFOMODE_MASK))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFifoStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the FIFO status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Status flags constants (for ReadStatusReg,
 *         GetInterruptFlags, ClearInterruptFlags macros)." for processing return
 *         value.
 * @remarks The macro accesses the following registers: SPI1_CI.
 * @par Example:
 *      @code
 *      uint8_t result = SPI_PDD_ReadFifoStatusReg(deviceID);
 *      @endcode
 */
#define SPI_PDD_ReadFifoStatusReg(PeripheralBase) ( \
    SPI_CI_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearFifoInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears FIFO interrupt flags defined by mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of FIFO interrupt requests. Use constants from group "FIFO
 *        interrupt masks". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: SPI1_CI.
 * @par Example:
 *      @code
 *      SPI_PDD_ClearFifoInterruptFlag(deviceID,
 *      SPI_PDD_TX_FIFO_NEARLY_EMPTY_FLAG);
 *      @endcode
 */
#define SPI_PDD_ClearFifoInterruptFlag(PeripheralBase, Mask) ( \
    SPI_CI_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )
#endif  /* #if defined(SPI_PDD_H_) */

/* SPI_PDD.h, eof. */
