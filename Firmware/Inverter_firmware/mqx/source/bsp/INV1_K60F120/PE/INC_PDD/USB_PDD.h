/*
  PDD layer implementation for peripheral type USB
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(USB_PDD_H_)
#define USB_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error USB PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK20D10) /* USB0 */ && \
      !defined(MCU_MK20D5) /* USB0 */ && \
      !defined(MCU_MK20D7) /* USB0 */ && \
      !defined(MCU_MK20F12) /* USB0 */ && \
      !defined(MCU_MK20DZ10) /* USB0 */ && \
      !defined(MCU_MK21D5) /* USB0 */ && \
      !defined(MCU_MK21D5WS) /* USB0 */ && \
      !defined(MCU_MK22D5) /* USB0 */ && \
      !defined(MCU_MK40D10) /* USB0 */ && \
      !defined(MCU_MK40D7) /* USB0 */ && \
      !defined(MCU_MK40DZ10) /* USB0 */ && \
      !defined(MCU_MK40X256VMD100) /* USB0 */ && \
      !defined(MCU_MK50D10) /* USB0 */ && \
      !defined(MCU_MK50D7) /* USB0 */ && \
      !defined(MCU_MK50DZ10) /* USB0 */ && \
      !defined(MCU_MK51D10) /* USB0 */ && \
      !defined(MCU_MK51D7) /* USB0 */ && \
      !defined(MCU_MK51DZ10) /* USB0 */ && \
      !defined(MCU_MK52D10) /* USB0 */ && \
      !defined(MCU_MK52DZ10) /* USB0 */ && \
      !defined(MCU_MK53D10) /* USB0 */ && \
      !defined(MCU_MK53DZ10) /* USB0 */ && \
      !defined(MCU_MK60D10) /* USB0 */ && \
      !defined(MCU_MK60F12) /* USB0 */ && \
      !defined(MCU_MK60F15) /* USB0 */ && \
      !defined(MCU_MK60DZ10) /* USB0 */ && \
      !defined(MCU_MK60N512VMD100) /* USB0 */ && \
      !defined(MCU_MK61F12) /* USB0 */ && \
      !defined(MCU_MK61F15) /* USB0 */ && \
      !defined(MCU_MK61F12WS) /* USB0 */ && \
      !defined(MCU_MK61F15WS) /* USB0 */ && \
      !defined(MCU_MK70F12) /* USB0 */ && \
      !defined(MCU_MK70F15) /* USB0 */ && \
      !defined(MCU_MK70F12WS) /* USB0 */ && \
      !defined(MCU_MK70F15WS) /* USB0 */ && \
      !defined(MCU_MKL24Z4) /* USB0 */ && \
      !defined(MCU_MKL25Z4) /* USB0 */ && \
      !defined(MCU_MKL26Z4) /* USB0 */ && \
      !defined(MCU_MKL46Z4) /* USB0 */ && \
      !defined(MCU_PCK20L4) /* USB0 */
  // Unsupported MCU is active
  #error USB PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Otg status masks */
#define USB_PDD_A_VBUS_VALID  USB_OTGSTAT_AVBUSVLD_MASK /**< A VBus valid signal mask */
#define USB_PDD_B_SESSION_END USB_OTGSTAT_BSESSEND_MASK /**< B session end signal mask */
#define USB_PDD_SESSION_VALID USB_OTGSTAT_SESS_VLD_MASK /**< Session valid signal mask */
#define USB_PDD_LINE_STABLE   USB_OTGSTAT_LINESTATESTABLE_MASK /**< Line stable signal mask */
#define USB_PDD_ID            USB_OTGSTAT_ID_MASK /**< ID signal mask */

/* Otg interrupt masks */
#define USB_PDD_ALL_INT_FLAGS 0xFFU              /**< Value used to mask all Otg interrupts */

/* Otg interrupt masks */
#define USB_PDD_A_VBUS_CHG_INT     USB_OTGICR_AVBUSEN_MASK /**< Vbus change interrupt mask */
#define USB_PDD_B_SESS_CHG_INT     USB_OTGICR_BSESSEN_MASK /**< B session change interrupt mask */
#define USB_PDD_SESS_VLD_CHG_INT   USB_OTGICR_SESSVLDEN_MASK /**< Session valid change interrupt mask */
#define USB_PDD_LINE_STATE_CHG_INT USB_OTGICR_LINESTATEEN_MASK /**< Line state change interrupt mask */
#define USB_PDD_1_MSEC_INT         USB_OTGICR_ONEMSECEN_MASK /**< 1 ms interrupt mask */
#define USB_PDD_ID_CHG_INT         USB_OTGICR_IDEN_MASK /**< ID change interrupt mask */

/* Usb interrupt masks */
#define USB_PDD_STALL_INT   USB_INTEN_STALLEN_MASK /**< Stall interrupt mask */
#define USB_PDD_ATTACH_INT  USB_INTEN_ATTACHEN_MASK /**< Attach interrupt mask */
#define USB_PDD_RESUME_INT  USB_INTEN_RESUMEEN_MASK /**< Resume interrupt mask */
#define USB_PDD_SLEEP_INT   USB_INTEN_SLEEPEN_MASK /**< Sleed interrupt mask */
#define USB_PDD_TOK_DNE_INT USB_INTEN_TOKDNEEN_MASK /**< Token done interrupt mask */
#define USB_PDD_SOF_TOK_INT USB_INTEN_SOFTOKEN_MASK /**< Star of frame interrupt mask */
#define USB_PDD_ERROR_INT   USB_INTEN_ERROREN_MASK /**< Error interrupt mask */
#define USB_PDD_USB_RST_INT USB_INTEN_USBRSTEN_MASK /**< Bus reset interrupt mask */

/* Error interrupt masks */
#define USB_PDD_BTS_ERR_INT  USB_ERREN_BTSERREN_MASK /**< BTS error interrupt mask */
#define USB_PDD_DMA_ERR_INT  USB_ERREN_DMAERREN_MASK /**< DNA error interrupt mask */
#define USB_PDD_BTO_ERR_INT  USB_ERREN_BTOERREN_MASK /**< BTO errot interrupt mask */
#define USB_PDD_DFN8_INT     USB_ERREN_DFN8EN_MASK /**< DFN8 error interrupt mask */
#define USB_PDD_CRC16_INT    USB_ERREN_CRC16EN_MASK /**< CRC16 interrupt mask */
#define USB_PDD_CRC5_EOF_INT USB_ERREN_CRC5EOFEN_MASK /**< CRC5 or EOF error interrupt mask */
#define USB_PDD_PID_ERR_INT  USB_ERREN_PIDERREN_MASK /**< PID error interrupt mask */

/* Conrol register masks */
#define USB_PDD_JSTATE USB_CTL_JSTATE_MASK       /**< J state mask */
#define USB_PDD_SE0    USB_CTL_SE0_MASK          /**< SE0 mask */

/* Otg output signal masks */
#define USB_PDD_DP_PU_SIGNAL USB_OBSERVE_DPPU_MASK /**< D+ pull-up signal mask */
#define USB_PDD_DP_PD_SIGNAL USB_OBSERVE_DPPD_MASK /**< D+ pull-down signal mask */
#define USB_PDD_DM_PD_SIGNAL USB_OBSERVE_DMPD_MASK /**< D- pull-down signal mask */

/* Otg input signal masks */
#define USB_PDD_DEVICE_VBUS_DETECT_SIGNAL USB_CONTROL_DPPULLUPNONOTG_MASK /**< Device mode Vbus detect signal mask */

/* Bus speed */
#define USB_PDD_LOW_SPEED  0U                    /**< Low speed constant */
#define USB_PDD_FULL_SPEED 0x80U                 /**< Full speed constant */
#define USB_PDD_RESET      0x40U                 /**< Bus reset state constant */


/* ----------------------------------------------------------------------------
   -- ReadOtgStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Otg status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Otg status masks" for processing return
 *         value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadOtgStatusReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadOtgStatusReg(PeripheralBase) ( \
    USB_OTGSTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- BDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the ID bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_BDevice(deviceID);
 *      @endcode
 */
#define USB_PDD_BDevice(PeripheralBase) ( \
    (uint8_t)(USB_OTGSTAT_REG(PeripheralBase) & USB_OTGSTAT_ID_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- LineStateStable
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Line stable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_LineStateStable(deviceID);
 *      @endcode
 */
#define USB_PDD_LineStateStable(PeripheralBase) ( \
    (uint8_t)(USB_OTGSTAT_REG(PeripheralBase) & USB_OTGSTAT_LINESTATESTABLE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SessionValid
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Session valid bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_SessionValid(deviceID);
 *      @endcode
 */
#define USB_PDD_SessionValid(PeripheralBase) ( \
    (uint8_t)(USB_OTGSTAT_REG(PeripheralBase) & USB_OTGSTAT_SESS_VLD_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- BSessionEnd
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the B session end bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_BSessionEnd(deviceID);
 *      @endcode
 */
#define USB_PDD_BSessionEnd(PeripheralBase) ( \
    (uint8_t)(USB_OTGSTAT_REG(PeripheralBase) & USB_OTGSTAT_BSESSEND_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- AVBusValid
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the A Vbus valid bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_AVBusValid(deviceID);
 *      @endcode
 */
#define USB_PDD_AVBusValid(PeripheralBase) ( \
    (uint8_t)(USB_OTGSTAT_REG(PeripheralBase) & USB_OTGSTAT_AVBUSVLD_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetOtgInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Otg interrupt status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Otg interrupt masks" for processing return
 *         value.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetOtgInterruptFlags(deviceID);
 *      @endcode
 */
#define USB_PDD_GetOtgInterruptFlags(PeripheralBase) ( \
    USB_OTGISTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearOtgInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears flags defined by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Otg interrupt masks".
 *        This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearOtgInterruptFlags(deviceID, USB_PDD_A_VBUS_CHG_INT);
 *      @endcode
 */
#define USB_PDD_ClearOtgInterruptFlags(PeripheralBase, Mask) ( \
    USB_OTGISTAT_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- GetIdChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the ID changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetIdChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetIdChgInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_OTGISTAT_REG(PeripheralBase) & USB_OTGISTAT_IDCHG_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- Get1msInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the 1 ms interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_Get1msInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_Get1msInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_OTGISTAT_REG(PeripheralBase) & USB_OTGISTAT_ONEMSEC_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetLineStateChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Line state change flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetLineStateChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetLineStateChgInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_OTGISTAT_REG(PeripheralBase) & USB_OTGISTAT_LINE_STATE_CHG_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetSessVldChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Session valalid changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetSessVldChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetSessVldChgInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_OTGISTAT_REG(PeripheralBase) & USB_OTGISTAT_SESSVLDCHG_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetBsessVldChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the B session end changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetBsessVldChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetBsessVldChgInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_OTGISTAT_REG(PeripheralBase) & USB_OTGISTAT_B_SESS_CHG_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetAVbusChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the A Vbus changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetAVbusChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetAVbusChgInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_OTGISTAT_REG(PeripheralBase) & USB_OTGISTAT_AVBUSCHG_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearIdChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the ID changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearIdChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearIdChgInterruptFlag(PeripheralBase) ( \
    USB_OTGISTAT_REG(PeripheralBase) |= \
     USB_OTGISTAT_IDCHG_MASK \
  )

/* ----------------------------------------------------------------------------
   -- Clear1msInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the 1 ms flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_Clear1msInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_Clear1msInterruptFlag(PeripheralBase) ( \
    USB_OTGISTAT_REG(PeripheralBase) |= \
     USB_OTGISTAT_ONEMSEC_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearLineStateChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Line state changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearLineStateChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearLineStateChgInterruptFlag(PeripheralBase) ( \
    USB_OTGISTAT_REG(PeripheralBase) |= \
     USB_OTGISTAT_LINE_STATE_CHG_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearSessVldChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Session valid changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearSessVldChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearSessVldChgInterruptFlag(PeripheralBase) ( \
    USB_OTGISTAT_REG(PeripheralBase) |= \
     USB_OTGISTAT_SESSVLDCHG_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearBsessVldChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the B session end changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearBsessVldChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearBsessVldChgInterruptFlag(PeripheralBase) ( \
    USB_OTGISTAT_REG(PeripheralBase) |= \
     USB_OTGISTAT_B_SESS_CHG_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearAVbusChgInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the A Vbus changed flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearAVbusChgInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearAVbusChgInterruptFlag(PeripheralBase) ( \
    USB_OTGISTAT_REG(PeripheralBase) |= \
     USB_OTGISTAT_AVBUSCHG_MASK \
  )

/* ----------------------------------------------------------------------------
   -- GetOtgInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Otg interrupt enable register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Otg interrupt masks" for processing return
 *         value.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetOtgInterruptMask(deviceID);
 *      @endcode
 */
#define USB_PDD_GetOtgInterruptMask(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetOtgInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables interrupts defined by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Otg interrupt masks".
 *        This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_SetOtgInterruptMask(deviceID, USB_PDD_A_VBUS_CHG_INT);
 *      @endcode
 */
#define USB_PDD_SetOtgInterruptMask(PeripheralBase, Mask) ( \
    USB_OTGICR_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- EnableIdChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the ID changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_EnableIdChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableIdChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) |= \
     USB_OTGICR_IDEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- Enable1msInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the 1 ms interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_Enable1msInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_Enable1msInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) |= \
     USB_OTGICR_ONEMSECEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableLineStateChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Line stable interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_EnableLineStateChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableLineStateChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) |= \
     USB_OTGICR_LINESTATEEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableSessVldChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Session valid changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_EnableSessVldChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableSessVldChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) |= \
     USB_OTGICR_SESSVLDEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableBsessVldChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the B session end changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_EnableBsessVldChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableBsessVldChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) |= \
     USB_OTGICR_BSESSEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableAVbusChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the A Vbus changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_EnableAVbusChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableAVbusChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) |= \
     USB_OTGICR_AVBUSEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableIdChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the ID changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_DisableIdChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableIdChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_OTGICR_IDEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- Disable1msInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the 1 ms interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_Disable1msInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_Disable1msInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_OTGICR_ONEMSECEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableLineStateChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Line state changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_DisableLineStateChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableLineStateChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_OTGICR_LINESTATEEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableSessVldChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Session valid changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_DisableSessVldChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableSessVldChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_OTGICR_SESSVLDEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableBsessVldChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the B session end changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_DisableBsessVldChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableBsessVldChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_OTGICR_BSESSEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableAVbusChgInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the A Vbus changed interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGICR.
 * @par Example:
 *      @code
 *      USB_PDD_DisableAVbusChgInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableAVbusChgInterrupt(PeripheralBase) ( \
    USB_OTGICR_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_OTGICR_AVBUSEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetUsbInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Usb interrupt status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Usb interrupt masks" for processing return
 *         value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetUsbInterruptFlags(deviceID);
 *      @endcode
 */
#define USB_PDD_GetUsbInterruptFlags(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearUsbInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears flags defined by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Usb interrupt masks".
 *        This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearUsbInterruptFlags(deviceID, USB_PDD_STALL_INT);
 *      @endcode
 */
#define USB_PDD_ClearUsbInterruptFlags(PeripheralBase, Mask) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- GetStallInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Stall interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetStallInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetStallInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_STALL_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetAttachInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Attach interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetAttachInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetAttachInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_ATTACH_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetResumeInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Resume interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetResumeInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetResumeInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_RESUME_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetSuspendInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Suspend interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetSuspendInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetSuspendInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_SLEEP_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetTokenDoneInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Token done interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetTokenDoneInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetTokenDoneInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_TOKDNE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetSofInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Sof interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetSofInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetSofInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_SOFTOK_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetErrorInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetErrorInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetErrorInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_ERROR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetBusResetInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returnes the Bus reset interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetBusResetInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetBusResetInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ISTAT_REG(PeripheralBase) & USB_ISTAT_USBRST_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearStallInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Stall interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearStallInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearStallInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_STALL_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearAttachInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Attach interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearAttachInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearAttachInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_ATTACH_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearResumeInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Resume interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearResumeInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearResumeInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_RESUME_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearSuspendInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Suspend interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearSuspendInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearSuspendInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_SLEEP_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearTokenDoneInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Token done interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearTokenDoneInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearTokenDoneInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_TOKDNE_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearSofInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Sof interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearSofInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearSofInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_SOFTOK_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearErrorInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearErrorInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearErrorInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_ERROR_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ClearBusResetInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the Bus reset interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ISTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearBusResetInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearBusResetInterruptFlag(PeripheralBase) ( \
    USB_ISTAT_REG(PeripheralBase) = \
     USB_ISTAT_USBRST_MASK \
  )

/* ----------------------------------------------------------------------------
   -- GetUsbInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Usb interrupt enable register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Usb interrupt masks" for processing return
 *         value.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetUsbInterruptMask(deviceID);
 *      @endcode
 */
#define USB_PDD_GetUsbInterruptMask(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetUsbInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables interrupts defined by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Usb interrupt masks".
 *        This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_SetUsbInterruptMask(deviceID, USB_PDD_STALL_INT);
 *      @endcode
 */
#define USB_PDD_SetUsbInterruptMask(PeripheralBase, Mask) ( \
    USB_INTEN_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- EnableStallInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Stall interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableStallInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableStallInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_STALLEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableAttachInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Attach interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableAttachInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableAttachInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_ATTACHEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableResumeInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Reume interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableResumeInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableResumeInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_RESUMEEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableSuspendInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Suspend  interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableSuspendInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableSuspendInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_SLEEPEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableTokenDoneInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Token done interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableTokenDoneInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableTokenDoneInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_TOKDNEEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableSofInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Sof interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableSofInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableSofInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_SOFTOKEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableErrorInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableErrorInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableErrorInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_ERROREN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- EnableBusResetInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the Bus reset interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_EnableBusResetInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableBusResetInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) |= \
     USB_INTEN_USBRSTEN_MASK \
  )

/* ----------------------------------------------------------------------------
   -- DisableStallInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Stall interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableStallInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableStallInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_STALLEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableAttachInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Attach interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableAttachInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableAttachInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_ATTACHEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableResumeInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Resume interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableResumeInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableResumeInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_RESUMEEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableSuspendInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Suspend interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableSuspendInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableSuspendInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_SLEEPEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableTokenDoneInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Token done interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableTokenDoneInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableTokenDoneInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_TOKDNEEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableSofInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Sof interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableSofInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableSofInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_SOFTOKEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableErrorInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableErrorInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableErrorInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_ERROREN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableBusResetInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the Bus reset interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_INTEN.
 * @par Example:
 *      @code
 *      USB_PDD_DisableBusResetInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableBusResetInterrupt(PeripheralBase) ( \
    USB_INTEN_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_INTEN_USBRSTEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetErrorsInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Error interrupt status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Error interrupt masks" for processing
 *         return value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetErrorsInterruptFlags(deviceID);
 *      @endcode
 */
#define USB_PDD_GetErrorsInterruptFlags(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ClearErrorsInterruptFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears flags defined by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Error interrupt masks".
 *        This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearErrorsInterruptFlags(deviceID, USB_PDD_BTS_ERR_INT);
 *      @endcode
 */
#define USB_PDD_ClearErrorsInterruptFlags(PeripheralBase, Mask) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- GetBtsErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the BTS error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetBtsErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetBtsErrInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) & USB_ERRSTAT_BTSERR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetDmaErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the DMA error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetDmaErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetDmaErrInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) & USB_ERRSTAT_DMAERR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetBtoErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the BTO error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetBtoErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetBtoErrInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) & USB_ERRSTAT_BTOERR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetDnf8ErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the DNF8 error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetDnf8ErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetDnf8ErrInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) & USB_ERRSTAT_DFN8_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetCrc16ErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the CRC16 error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetCrc16ErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetCrc16ErrInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) & USB_ERRSTAT_CRC16_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetCrc5EofErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the CRC5 or EOF error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetCrc5EofErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetCrc5EofErrInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) & USB_ERRSTAT_CRC5EOF_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetPidErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the PID error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetPidErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetPidErrInterruptFlag(PeripheralBase) ( \
    (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) & USB_ERRSTAT_PIDERR_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearBtsErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the BTS error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearBtsErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearBtsErrInterruptFlag(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_BTSERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearDmaErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the DMA error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearDmaErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearDmaErrInterruptFlag(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_DMAERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearBtoErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the BTO error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearBtoErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearBtoErrInterruptFlag(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearDnf8ErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the DFN8 error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearDnf8ErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearDnf8ErrInterruptFlag(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearCrc16ErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the CRC16 error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearCrc16ErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearCrc16ErrInterruptFlag(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearCrc5EofErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the CRC5 or EOF error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearCrc5EofErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearCrc5EofErrInterruptFlag(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearPidErrInterruptFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears the PID error interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_ClearPidErrInterruptFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearPidErrInterruptFlag(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- GetErrorInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Error interrupt enable register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Error interrupt masks" for processing
 *         return value.
 * @remarks The macro accesses the following registers: USB0_ERREN.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetErrorInterruptMask(deviceID);
 *      @endcode
 */
#define USB_PDD_GetErrorInterruptMask(PeripheralBase) ( \
    USB_ERREN_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetErrorInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables interrupts defined by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Error interrupt masks".
 *        This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERREN.
 * @par Example:
 *      @code
 *      USB_PDD_SetErrorInterruptMask(deviceID, USB_PDD_BTS_ERR_INT);
 *      @endcode
 */
#define USB_PDD_SetErrorInterruptMask(PeripheralBase, Mask) ( \
    USB_ERREN_REG(PeripheralBase) = \
     (uint8_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- EnableBtsErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the BTS error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_EnableBtsErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableBtsErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_BTSERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDmaErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the DMA error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_EnableDmaErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableDmaErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_DMAERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableBtoErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the BTO error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_EnableBtoErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableBtoErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDnf8ErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the DNF8 error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_EnableDnf8ErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableDnf8ErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableCrc16ErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the CRC16 error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_EnableCrc16ErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableCrc16ErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableCrc5EofErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the CRC5 or EOF error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_EnableCrc5EofErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnableCrc5EofErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnablePidErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables the PID error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_EnablePidErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_EnablePidErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ERRSTAT_REG(PeripheralBase) | USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableBtsErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the BTS error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_DisableBtsErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableBtsErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableDmaErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the DMA error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_DisableDmaErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableDmaErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableBtoErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the BTO error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_DisableBtoErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableBtoErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableDnf8ErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the DNF8 error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_DisableDnf8ErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableDnf8ErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableCrc16ErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the CRC16 error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_DisableCrc16ErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableCrc16ErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableCrc5EofErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the CRC5 or EOF error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_DisableCrc5EofErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableCrc5EofErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- DisablePidErrInterrupt
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables the PID error interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ERRSTAT.
 * @par Example:
 *      @code
 *      USB_PDD_DisablePidErrInterrupt(deviceID);
 *      @endcode
 */
#define USB_PDD_DisablePidErrInterrupt(PeripheralBase) ( \
    USB_ERRSTAT_REG(PeripheralBase) &= \
     (uint8_t)(( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_PIDERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC5EOF_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_CRC16_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DFN8_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTOERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_DMAERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_ERRSTAT_BTSERR_MASK)))))))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadOtgStstusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Otg status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadOtgStstusReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadOtgStstusReg(PeripheralBase) ( \
    USB_OTGSTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetLineStateStableState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Line stable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetLineStateStableState(deviceID);
 *      @endcode
 */
#define USB_PDD_GetLineStateStableState(PeripheralBase) ( \
    (uint8_t)(USB_OTGSTAT_REG(PeripheralBase) & USB_OTGSTAT_LINESTATESTABLE_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ReadOtgControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Otg control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadOtgControlReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadOtgControlReg(PeripheralBase) ( \
    USB_OTGCTL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteOtgControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the Otg control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Otg control register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_WriteOtgControlReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteOtgControlReg(PeripheralBase, Value) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDpPullUp
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables D+ pull up.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableDpPullUp(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableDpPullUp(PeripheralBase, State) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_OTGCTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_OTGCTL_DPHIGH_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_OTGCTL_DPHIGH_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDpPullDown
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables D+ pull down.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableDpPullDown(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableDpPullDown(PeripheralBase, State) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_OTGCTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_OTGCTL_DPLOW_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_OTGCTL_DPLOW_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDmPullDown
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables D- pull up.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableDmPullDown(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableDmPullDown(PeripheralBase, State) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_OTGCTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_OTGCTL_DMLOW_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_OTGCTL_DMLOW_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableVBUS
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables VBUS.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableVBUS(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableVBUS(PeripheralBase, State) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_OTGCTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_OTGCTL__MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_OTGCTL__SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableOtgTermination
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables OTG termination.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableOtgTermination(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableOtgTermination(PeripheralBase, State) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_OTGCTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_OTGCTL_OTGEN_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_OTGCTL_OTGEN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableVbusCharge
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables VBus charge.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableVbusCharge(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableVbusCharge(PeripheralBase, State) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_OTGCTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)((uint8_t)0x1U << 1U)))) | ( \
      (uint8_t)((uint8_t)(State) << 1U))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableVbusDischarge
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables VBus discharge.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableVbusDischarge(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableVbusDischarge(PeripheralBase, State) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_OTGCTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)0x1U))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDeviceModeFullSpeedTermination
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets full speed device termination.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_SetDeviceModeFullSpeedTermination(deviceID);
 *      @endcode
 */
#define USB_PDD_SetDeviceModeFullSpeedTermination(PeripheralBase) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       USB_OTGCTL_REG(PeripheralBase)) | (( \
       USB_OTGCTL_DPHIGH_MASK) | ( \
       USB_OTGCTL_OTGEN_MASK)))) & (( \
      (uint8_t)(~(uint8_t)USB_OTGCTL_DPLOW_MASK)) & ( \
      (uint8_t)(~(uint8_t)USB_OTGCTL_DMLOW_MASK)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetHostModeTermination
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets full speed host termination.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_SetHostModeTermination(deviceID);
 *      @endcode
 */
#define USB_PDD_SetHostModeTermination(PeripheralBase) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     0x34U \
  )

/* ----------------------------------------------------------------------------
   -- SetNoTermination
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables all pull resistors.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OTGCTL.
 * @par Example:
 *      @code
 *      USB_PDD_SetNoTermination(deviceID);
 *      @endcode
 */
#define USB_PDD_SetNoTermination(PeripheralBase) ( \
    USB_OTGCTL_REG(PeripheralBase) = \
     0x4U \
  )

/* ----------------------------------------------------------------------------
   -- ReadStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_STAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadStatusReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadStatusReg(PeripheralBase) ( \
    USB_STAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetTransmitIndicator
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the TX bit in the Status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_STAT.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetTransmitIndicator(deviceID);
 *      @endcode
 */
#define USB_PDD_GetTransmitIndicator(PeripheralBase) ( \
    (uint8_t)(USB_STAT_REG(PeripheralBase) & USB_STAT_TX_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ReadControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadControlReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadControlReg(PeripheralBase) ( \
    USB_CTL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the Control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Control register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_WriteControlReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteControlReg(PeripheralBase, Value) ( \
    USB_CTL_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetBusSpeed
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns current bus speed.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of "Bus speed" type. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetBusSpeed(deviceID);
 *      @endcode
 */
#define USB_PDD_GetBusSpeed(PeripheralBase) ( \
    (uint8_t)(USB_CTL_REG(PeripheralBase) & (uint8_t)((uint8_t)0x3U << 6U)) \
  )

/* ----------------------------------------------------------------------------
   -- GetSE0
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of the SEO signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetSE0(deviceID);
 *      @endcode
 */
#define USB_PDD_GetSE0(PeripheralBase) ( \
    (uint8_t)(USB_CTL_REG(PeripheralBase) & USB_CTL_SE0_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Device mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableDevice(PeripheralBase, State) ( \
    USB_CTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_CTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_CTL_USBENSOFEN_MASK))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableSof
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables SOF.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableSof(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableSof(PeripheralBase, State) ( \
    USB_CTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_CTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_CTL_USBENSOFEN_MASK))) | ( \
      (uint8_t)(State))) \
  )

/* ----------------------------------------------------------------------------
   -- ResetBdtPingPong
   ---------------------------------------------------------------------------- */

/**
 * @brief Resets internal logic.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_ResetBdtPingPong(deviceID);
 *      @endcode
 */
#define USB_PDD_ResetBdtPingPong(PeripheralBase) ( \
    (USB_CTL_REG(PeripheralBase) |= \
     USB_CTL_ODDRST_MASK), \
    (USB_CTL_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_CTL_ODDRST_MASK)) \
  )

/* ----------------------------------------------------------------------------
   -- StartResumeSignaling
   ---------------------------------------------------------------------------- */

/**
 * @brief Starts/stops resume signaling.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_StartResumeSignaling(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_StartResumeSignaling(PeripheralBase, State) ( \
    USB_CTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_CTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_CTL_RESUME_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_CTL_RESUME_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetResumeSignalState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of resume enable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetResumeSignalState(deviceID);
 *      @endcode
 */
#define USB_PDD_GetResumeSignalState(PeripheralBase) ( \
    (uint8_t)(USB_CTL_REG(PeripheralBase) & USB_CTL_RESUME_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- EnableHost
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Host mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableHost(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableHost(PeripheralBase, State) ( \
    USB_CTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_CTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_CTL_HOSTMODEEN_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_CTL_HOSTMODEEN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- StartResetSignaling
   ---------------------------------------------------------------------------- */

/**
 * @brief Starts/stops reset signaling.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_StartResetSignaling(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_StartResetSignaling(PeripheralBase, State) ( \
    USB_CTL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_CTL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_CTL_RESET_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_CTL_RESET_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetResetSignalState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of reset enable bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetResetSignalState(deviceID);
 *      @endcode
 */
#define USB_PDD_GetResetSignalState(PeripheralBase) ( \
    (uint8_t)(USB_CTL_REG(PeripheralBase) & USB_CTL_RESET_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearTxSuspendFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears TxSuspend flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_ClearTxSuspendFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearTxSuspendFlag(PeripheralBase) ( \
    USB_CTL_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_CTL_TXSUSPENDTOKENBUSY_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearTokenBusyFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears TokenBusy flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_ClearTokenBusyFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_ClearTokenBusyFlag(PeripheralBase) ( \
    USB_CTL_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)USB_CTL_TXSUSPENDTOKENBUSY_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetTokenBusyFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of TokenBusy flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetTokenBusyFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetTokenBusyFlag(PeripheralBase) ( \
    (uint8_t)(USB_CTL_REG(PeripheralBase) & USB_CTL_TXSUSPENDTOKENBUSY_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableModule
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables module.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CTL.
 * @par Example:
 *      @code
 *      USB_PDD_DisableModule(deviceID);
 *      @endcode
 */
#define USB_PDD_DisableModule(PeripheralBase) ( \
    USB_CTL_REG(PeripheralBase) = \
     0U \
  )

/* ----------------------------------------------------------------------------
   -- ReadAddressReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Address register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_ADDR.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadAddressReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadAddressReg(PeripheralBase) ( \
    USB_ADDR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteAddressReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the Address register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Address register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ADDR.
 * @par Example:
 *      @code
 *      USB_PDD_WriteAddressReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteAddressReg(PeripheralBase, Value) ( \
    USB_ADDR_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- EnableLowSpeed
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables low speed mode.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ADDR.
 * @par Example:
 *      @code
 *      USB_PDD_EnableLowSpeed(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableLowSpeed(PeripheralBase, State) ( \
    USB_ADDR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ADDR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_ADDR_LSEN_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_ADDR_LSEN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDeviceAddress
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets device address.
 * @param PeripheralBase Peripheral base address.
 * @param Address New device address. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_ADDR.
 * @par Example:
 *      @code
 *      USB_PDD_SetDeviceAddress(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_SetDeviceAddress(PeripheralBase, Address) ( \
    USB_ADDR_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_ADDR_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_ADDR_ADDR_MASK))) | ( \
      (uint8_t)(Address))) \
  )

/* ----------------------------------------------------------------------------
   -- GetFrameNumberHigh
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Frame number high register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_FRMNUMH.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetFrameNumberHigh(deviceID);
 *      @endcode
 */
#define USB_PDD_GetFrameNumberHigh(PeripheralBase) ( \
    USB_FRMNUMH_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetFrameNumberHigh
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data to the Frame number high register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the high part of the Frame number register. This
 *        parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_FRMNUMH.
 * @par Example:
 *      @code
 *      USB_PDD_SetFrameNumberHigh(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_SetFrameNumberHigh(PeripheralBase, Data) ( \
    USB_FRMNUMH_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- GetFrameNumberLow
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Frame number low register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_FRMNUML.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetFrameNumberLow(deviceID);
 *      @endcode
 */
#define USB_PDD_GetFrameNumberLow(PeripheralBase) ( \
    USB_FRMNUML_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetFrameNumberLow
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data to the Frame number low register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the low part of the Frame number register. This
 *        parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_FRMNUML.
 * @par Example:
 *      @code
 *      USB_PDD_SetFrameNumberLow(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_SetFrameNumberLow(PeripheralBase, Data) ( \
    USB_FRMNUML_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- ReadTokenReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Token register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_TOKEN.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadTokenReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadTokenReg(PeripheralBase) ( \
    USB_TOKEN_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTokenReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data to the Token register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the Token register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_TOKEN.
 * @par Example:
 *      @code
 *      USB_PDD_WriteTokenReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteTokenReg(PeripheralBase, Data) ( \
    USB_TOKEN_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- ReadSofTresholdReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Sof treshold register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_SOFTHLD.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadSofTresholdReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadSofTresholdReg(PeripheralBase) ( \
    USB_SOFTHLD_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- SetSofTresholdReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data to the Sof theshold register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the Sof treshold register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_SOFTHLD.
 * @par Example:
 *      @code
 *      USB_PDD_SetSofTresholdReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_SetSofTresholdReg(PeripheralBase, Data) ( \
    USB_SOFTHLD_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- ReadEp0CtrlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the EP0 control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: ENDPT[].
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadEp0CtrlReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadEp0CtrlReg(PeripheralBase) ( \
    USB_ENDPT_REG(PeripheralBase,0U) \
  )

/* ----------------------------------------------------------------------------
   -- WriteEp0CtrlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data to the EP0 control register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the EP0 control register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[].
 * @par Example:
 *      @code
 *      USB_PDD_WriteEp0CtrlReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteEp0CtrlReg(PeripheralBase, Data) ( \
    USB_ENDPT_REG(PeripheralBase,0U) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- GetEpCtrlRegAddr
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the address of the EP_x register.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      uint32_t result = USB_PDD_GetEpCtrlRegAddr(deviceID, periphID);
 *      @endcode
 */
#define USB_PDD_GetEpCtrlRegAddr(PeripheralBase, EpNum) ( \
    (void *)&(USB_ENDPT_REG(PeripheralBase,(EpNum))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableDirectLowSpeedDevice
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables direct low speed device.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[].
 * @par Example:
 *      @code
 *      USB_PDD_EnableDirectLowSpeedDevice(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableDirectLowSpeedDevice(PeripheralBase, State) ( \
    USB_ENDPT_REG(PeripheralBase,0U) = \
     (uint8_t)(( \
      (uint8_t)(( \
       USB_ENDPT_REG(PeripheralBase,0U)) & ( \
       (uint8_t)(~(uint8_t)USB_ENDPT_HOSTWOHUB_MASK)))) | ( \
      (uint8_t)((uint8_t)(State) << USB_ENDPT_HOSTWOHUB_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- StallControlEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Stall feature for given EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_StallControlEP(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_StallControlEP(PeripheralBase, EpNum, State) ( \
    USB_ENDPT_REG(PeripheralBase,(EpNum)) = \
     (uint8_t)(( \
      (uint8_t)(( \
       USB_ENDPT_REG(PeripheralBase,(EpNum))) & ( \
       (uint8_t)(~(uint8_t)USB_ENDPT_EPSTALL_MASK)))) | ( \
      (uint8_t)((uint8_t)(State) << USB_ENDPT_EPSTALL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableControlEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Control EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_EnableControlEP(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableControlEP(PeripheralBase, EpNum, State) ( \
    ((State) == PDD_DISABLE) ? ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) &= \
       (uint8_t)(( \
        (uint8_t)(~(uint8_t)USB_ENDPT_RETRYDIS_MASK)) & (( \
        (uint8_t)(~(uint8_t)USB_ENDPT_EPCTLDIS_MASK)) & (( \
        (uint8_t)(~(uint8_t)USB_ENDPT_EPRXEN_MASK)) & (( \
        (uint8_t)(~(uint8_t)USB_ENDPT_EPTXEN_MASK)) & (( \
        (uint8_t)(~(uint8_t)USB_ENDPT_EPSTALL_MASK)) & ( \
        (uint8_t)(~(uint8_t)USB_ENDPT_EPHSHK_MASK)))))))) : ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) = \
       (uint8_t)(( \
        (uint8_t)(( \
         USB_ENDPT_REG(PeripheralBase,(EpNum))) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_RETRYDIS_MASK)) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPCTLDIS_MASK)) & ( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPSTALL_MASK)))))) | (( \
        USB_ENDPT_EPRXEN_MASK) | (( \
        USB_ENDPT_EPTXEN_MASK) | ( \
        USB_ENDPT_EPHSHK_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableBulkOrIntRxEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Bulk or Interrupt Rx EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_EnableBulkOrIntRxEP(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableBulkOrIntRxEP(PeripheralBase, EpNum, State) ( \
    ((State) == PDD_DISABLE) ? ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) &= \
       (uint8_t)(~(uint8_t)USB_ENDPT_EPRXEN_MASK)) : ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) = \
       (uint8_t)(( \
        (uint8_t)(( \
         USB_ENDPT_REG(PeripheralBase,(EpNum))) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_RETRYDIS_MASK)) & ( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPSTALL_MASK))))) | (( \
        USB_ENDPT_EPCTLDIS_MASK) | (( \
        USB_ENDPT_EPRXEN_MASK) | ( \
        USB_ENDPT_EPHSHK_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableBulkOrIntTxEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Bulk or Interrupt Tx EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_EnableBulkOrIntTxEP(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableBulkOrIntTxEP(PeripheralBase, EpNum, State) ( \
    ((State) == PDD_DISABLE) ? ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) &= \
       (uint8_t)(~(uint8_t)USB_ENDPT_EPTXEN_MASK)) : ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) = \
       (uint8_t)(( \
        (uint8_t)(( \
         USB_ENDPT_REG(PeripheralBase,(EpNum))) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_RETRYDIS_MASK)) & ( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPSTALL_MASK))))) | (( \
        USB_ENDPT_EPCTLDIS_MASK) | (( \
        USB_ENDPT_EPTXEN_MASK) | ( \
        USB_ENDPT_EPHSHK_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableIsoRxEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Isochronous Rx EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_EnableIsoRxEP(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableIsoRxEP(PeripheralBase, EpNum, State) ( \
    ((State) == PDD_DISABLE) ? ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) &= \
       (uint8_t)(~(uint8_t)USB_ENDPT_EPRXEN_MASK)) : ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) = \
       (uint8_t)(( \
        (uint8_t)(( \
         USB_ENDPT_REG(PeripheralBase,(EpNum))) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_RETRYDIS_MASK)) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPSTALL_MASK)) & ( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPHSHK_MASK)))))) | (( \
        USB_ENDPT_EPCTLDIS_MASK) | ( \
        USB_ENDPT_EPRXEN_MASK)))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableIsoTxEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables Isochronous Tx EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_EnableIsoTxEP(deviceID, periphID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableIsoTxEP(PeripheralBase, EpNum, State) ( \
    ((State) == PDD_DISABLE) ? ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) &= \
       (uint8_t)(~(uint8_t)USB_ENDPT_EPTXEN_MASK)) : ( \
      USB_ENDPT_REG(PeripheralBase,(EpNum)) = \
       (uint8_t)(( \
        (uint8_t)(( \
         USB_ENDPT_REG(PeripheralBase,(EpNum))) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_HOSTWOHUB_MASK)) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_RETRYDIS_MASK)) & (( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPSTALL_MASK)) & ( \
         (uint8_t)(~(uint8_t)USB_ENDPT_EPHSHK_MASK))))))) | (( \
        USB_ENDPT_EPCTLDIS_MASK) | ( \
        USB_ENDPT_EPTXEN_MASK)))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableRxEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables Rx EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_DisableRxEP(deviceID, periphID);
 *      @endcode
 */
#define USB_PDD_DisableRxEP(PeripheralBase, EpNum) ( \
    USB_ENDPT_REG(PeripheralBase,(EpNum)) &= \
     (uint8_t)(~(uint8_t)USB_ENDPT_EPRXEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableTxEP
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables Tx EP.
 * @param PeripheralBase Peripheral base address.
 * @param EpNum Endpoint number. This parameter is of index type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: ENDPT[EpNum].
 * @par Example:
 *      @code
 *      USB_PDD_DisableTxEP(deviceID, periphID);
 *      @endcode
 */
#define USB_PDD_DisableTxEP(PeripheralBase, EpNum) ( \
    USB_ENDPT_REG(PeripheralBase,(EpNum)) &= \
     (uint8_t)(~(uint8_t)USB_ENDPT_EPTXEN_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ReadUsbControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the USB control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_USBCTRL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadUsbControlReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadUsbControlReg(PeripheralBase) ( \
    USB_USBCTRL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteUsbControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the Usb control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Usb control register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBCTRL.
 * @par Example:
 *      @code
 *      USB_PDD_WriteUsbControlReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteUsbControlReg(PeripheralBase, Value) ( \
    USB_USBCTRL_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- SuspendTransceiver
   ---------------------------------------------------------------------------- */

/**
 * @brief Suspends/Wakes up transceiver.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBCTRL.
 * @par Example:
 *      @code
 *      USB_PDD_SuspendTransceiver(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_SuspendTransceiver(PeripheralBase, State) ( \
    USB_USBCTRL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_USBCTRL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_USBCTRL_SUSP_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_USBCTRL_SUSP_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableWeakPullDowns
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables/disables week pull downs.
 * @param PeripheralBase Peripheral base address.
 * @param State Requested state. This parameter is of "Global enumeration used
 *        for specifying general enable/disable states (PDD_DISABLE and PDD_ENABLE
 *        defined in PDD_Types.h)" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBCTRL.
 * @par Example:
 *      @code
 *      USB_PDD_EnableWeakPullDowns(deviceID, PDD_DISABLE);
 *      @endcode
 */
#define USB_PDD_EnableWeakPullDowns(PeripheralBase, State) ( \
    USB_USBCTRL_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(USB_USBCTRL_REG(PeripheralBase) & (uint8_t)(~(uint8_t)USB_USBCTRL_PDE_MASK))) | ( \
      (uint8_t)((uint8_t)(State) << USB_USBCTRL_PDE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadOtgSignalObserveReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns content of the Otg observe register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OBSERVE.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadOtgSignalObserveReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadOtgSignalObserveReg(PeripheralBase) ( \
    USB_OBSERVE_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteOtgSignalObservelReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the OTG observe register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Otg observel register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_OBSERVE.
 * @par Example:
 *      @code
 *      USB_PDD_WriteOtgSignalObservelReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteOtgSignalObservelReg(PeripheralBase, Value) ( \
    USB_OBSERVE_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- GetDpPullUpSignalState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of the D+ pull up signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OBSERVE.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetDpPullUpSignalState(deviceID);
 *      @endcode
 */
#define USB_PDD_GetDpPullUpSignalState(PeripheralBase) ( \
    (uint8_t)(USB_OBSERVE_REG(PeripheralBase) & USB_OBSERVE_DPPU_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetDpPullDownSignalState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of the D+ pull down signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OBSERVE.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetDpPullDownSignalState(deviceID);
 *      @endcode
 */
#define USB_PDD_GetDpPullDownSignalState(PeripheralBase) ( \
    (uint8_t)(USB_OBSERVE_REG(PeripheralBase) & USB_OBSERVE_DPPD_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetDmPullDownSignalState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of the D- pull up signal.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_OBSERVE.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetDmPullDownSignalState(deviceID);
 *      @endcode
 */
#define USB_PDD_GetDmPullDownSignalState(PeripheralBase) ( \
    (uint8_t)(USB_OBSERVE_REG(PeripheralBase) & USB_OBSERVE_DMPD_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ReadOtgSignalControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Otg control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_CONTROL.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadOtgSignalControlReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadOtgSignalControlReg(PeripheralBase) ( \
    USB_CONTROL_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteOtgSignalControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the OTG control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Otg control register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_CONTROL.
 * @par Example:
 *      @code
 *      USB_PDD_WriteOtgSignalControlReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteOtgSignalControlReg(PeripheralBase, Value) ( \
    USB_CONTROL_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ReadTransceiverControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Transceiver control register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadTransceiverControlReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadTransceiverControlReg(PeripheralBase) ( \
    USB_USBTRC0_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteTransceiverControlReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the Transceiver control register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Transceiver control register. This
 *        parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_WriteTransceiverControlReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteTransceiverControlReg(PeripheralBase, Value) ( \
    USB_USBTRC0_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ResetModule
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK20DZ10)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Starts module reset.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_ResetModule(deviceID);
 *      @endcode
 */
  #define USB_PDD_ResetModule(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) |= \
       (uint8_t)(USB_USBTRC0_USBRESET_MASK | 0x40U) \
    )
#else /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Starts module reset.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_ResetModule(deviceID);
 *      @endcode
 */
  #define USB_PDD_ResetModule(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) |= \
       USB_USBTRC0_USBRESET_MASK \
    )
#endif /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- GetModuleResetPendingFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns state of module reset pending flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_GetModuleResetPendingFlag(deviceID);
 *      @endcode
 */
#define USB_PDD_GetModuleResetPendingFlag(PeripheralBase) ( \
    (uint8_t)(USB_USBTRC0_REG(PeripheralBase) & USB_USBTRC0_USBRESET_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ClearAsyncResumeInterruptFlag
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK20DZ10)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Clears asynchronous resume interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_ClearAsyncResumeInterruptFlag(deviceID);
 *      @endcode
 */
  #define USB_PDD_ClearAsyncResumeInterruptFlag(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) |= \
       (uint8_t)(USB_USBTRC0_SYNC_DET_MASK | 0x40U) \
    )
#else /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Clears asynchronous resume interrupt flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_ClearAsyncResumeInterruptFlag(deviceID);
 *      @endcode
 */
  #define USB_PDD_ClearAsyncResumeInterruptFlag(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) |= \
       USB_USBTRC0_SYNC_DET_MASK \
    )
#endif /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- EnableAsyncResumeInterrupt
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK20DZ10)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Enables asynchronous resume interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_EnableAsyncResumeInterrupt(deviceID);
 *      @endcode
 */
  #define USB_PDD_EnableAsyncResumeInterrupt(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) |= \
       (uint8_t)(USB_USBTRC0_USBRESMEN_MASK | 0x40U) \
    )
#else /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Enables asynchronous resume interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_EnableAsyncResumeInterrupt(deviceID);
 *      @endcode
 */
  #define USB_PDD_EnableAsyncResumeInterrupt(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) |= \
       USB_USBTRC0_USBRESMEN_MASK \
    )
#endif /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- DisableAsyncResumeInterrupt
   ---------------------------------------------------------------------------- */

#if ((defined(MCU_MK20DZ10)) || (defined(MCU_MK40DZ10)) || (defined(MCU_MK40X256VMD100)) || (defined(MCU_MK50DZ10)) || (defined(MCU_MK51DZ10)) || (defined(MCU_MK52DZ10)) || (defined(MCU_MK53DZ10)) || (defined(MCU_MK60DZ10)) || (defined(MCU_MK60N512VMD100)) || (defined(MCU_MKL24Z4)) || (defined(MCU_MKL25Z4)) || (defined(MCU_MKL26Z4)) || (defined(MCU_MKL46Z4)))
/**
 * @brief Disables asynchronous resume interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_DisableAsyncResumeInterrupt(deviceID);
 *      @endcode
 */
  #define USB_PDD_DisableAsyncResumeInterrupt(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) = \
       (uint8_t)(( \
        (uint8_t)(( \
         USB_USBTRC0_REG(PeripheralBase)) & ( \
         (uint8_t)(~(uint8_t)USB_USBTRC0_USBRESMEN_MASK)))) | ( \
        0x40U)) \
    )
#else /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */
/**
 * @brief Disables asynchronous resume interrupt.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBTRC0.
 * @par Example:
 *      @code
 *      USB_PDD_DisableAsyncResumeInterrupt(deviceID);
 *      @endcode
 */
  #define USB_PDD_DisableAsyncResumeInterrupt(PeripheralBase) ( \
      USB_USBTRC0_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)USB_USBTRC0_USBRESMEN_MASK) \
    )
#endif /* (defined(MCU_MK20D10)) || (defined(MCU_MK20D5)) || (defined(MCU_MK20D7)) || (defined(MCU_MK20F12)) || (defined(MCU_MK21D5)) || (defined(MCU_MK21D5WS)) || (defined(MCU_MK22D5)) || (defined(MCU_MK40D10)) || (defined(MCU_MK40D7)) || (defined(MCU_MK50D10)) || (defined(MCU_MK50D7)) || (defined(MCU_MK51D10)) || (defined(MCU_MK51D7)) || (defined(MCU_MK52D10)) || (defined(MCU_MK53D10)) || (defined(MCU_MK60D10)) || (defined(MCU_MK60F12)) || (defined(MCU_MK60F15)) || (defined(MCU_MK61F12)) || (defined(MCU_MK61F12WS)) || (defined(MCU_MK61F15)) || (defined(MCU_MK61F15WS)) || (defined(MCU_MK70F12)) || (defined(MCU_MK70F12WS)) || (defined(MCU_MK70F15)) || (defined(MCU_MK70F15WS)) || (defined(MCU_PCK20L4)) */

/* ----------------------------------------------------------------------------
   -- ReadFrameAdjustmentReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the content of the Frame Adjustment register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: USB0_USBFRMADJUST.
 * @par Example:
 *      @code
 *      uint8_t result = USB_PDD_ReadFrameAdjustmentReg(deviceID);
 *      @endcode
 */
#define USB_PDD_ReadFrameAdjustmentReg(PeripheralBase) ( \
    USB_USBFRMADJUST_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFrameAdjustmentReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Value to the Frame adjustment register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Frame adjustment register. This parameter
 *        is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBFRMADJUST.
 * @par Example:
 *      @code
 *      USB_PDD_WriteFrameAdjustmentReg(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_WriteFrameAdjustmentReg(PeripheralBase, Value) ( \
    USB_USBFRMADJUST_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- SetFrameAdjustment
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets frame adjustment. In Host mode, the frame adjustment is a twos
 * complement number that adjusts the period of each USB frame in 12-MHz clock
 * periods. A SOF is normally generated every 12,000 12-MHz clock cycles. The Frame
 * Adjust Register can adjust this by -128 to +127 to compensate for inaccuracies
 * in the USB 48-MHz clock. Changes to the ADJ bit take effect at the next start
 * of the next frame.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Frame adjustment register. This parameter
 *        is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: USB0_USBFRMADJUST.
 * @par Example:
 *      @code
 *      USB_PDD_SetFrameAdjustment(deviceID, 1);
 *      @endcode
 */
#define USB_PDD_SetFrameAdjustment(PeripheralBase, Value) ( \
    USB_USBFRMADJUST_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )
#endif  /* #if defined(USB_PDD_H_) */

/* USB_PDD.h, eof. */
