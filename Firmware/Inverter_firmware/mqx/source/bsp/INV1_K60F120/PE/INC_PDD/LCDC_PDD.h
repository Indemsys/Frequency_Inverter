/*
  PDD layer implementation for peripheral type LCDC
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(LCDC_PDD_H_)
#define LCDC_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error LCDC PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK70F12) /* LCDC */ && \
      !defined(MCU_MK70F15) /* LCDC */ && \
      !defined(MCU_MK70F12WS) /* LCDC */ && \
      !defined(MCU_MK70F15WS) /* LCDC */
  // Unsupported MCU is active
  #error LCDC PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* Blinking control. */
#define LCDC_PDD_BLINK_DISABLED 0U               /**< Cursor blinking enabled. */
#define LCDC_PDD_BLINK_ENABLED  0x1U             /**< Cursor blinking disabled. */

/* HW cursor operation */
#define LCDC_PDD_DISABLED       0U               /**< Cursor disabled */
#define LCDC_PDD_ALWAYS_1       0x1U             /**< Cursor 1's, monochrome display only. */
#define LCDC_PDD_ALWAYS_0       0x2U             /**< Cursor 0's, monochrome display only. */
#define LCDC_PDD_INVERTED       0x3U             /**< Inverted background, monochrome display only. */
#define LCDC_PDD_COLOR          0x4U             /**< Defined cursor color, color display only. */
#define LCDC_PDD_INVERTED_COLOR 0x5U             /**< Inverted cursor color, color display only. */
#define LCDC_PDD_AND            0x6U             /**< Cursor color AND backgroun, color display only. */
#define LCDC_PDD_OR             0x7U             /**< Cursor color OR backgroun, color display only. */
#define LCDC_PDD_XOR            0x8U             /**< Cursor color XOR backgroun, color display only. */

/* LCD panel type. */
#define LCDC_PDD_MONO 0U                         /**< Monochrome panel. */
#define LCDC_PDD_CSTN 0x1U                       /**< CSTN panel type. */
#define LCDC_PDD_TFT  0x2U                       /**< TFT panel type. */

/* Bus width */
#define LCDC_PDD_BUS_WIDTH_1 0U                  /**< 1-bit bus. */
#define LCDC_PDD_BUS_WIDTH_2 0x1U                /**< 2-bit bus. */
#define LCDC_PDD_BUS_WIDTH_4 0x2U                /**< 4-bit bus. */
#define LCDC_PDD_BUS_WIDTH_8 0x3U                /**< 8-bit bus. */

/* Bits per pixel */
#define LCDC_PDD_BPP_1  0U                       /**< 1 bit per pixel. */
#define LCDC_PDD_BPP_2  0x1U                     /**< 2 bits per pixel. */
#define LCDC_PDD_BPP_4  0x2U                     /**< 4 bits per pixel. */
#define LCDC_PDD_BPP_8  0x3U                     /**< 8 bits per pixel. */
#define LCDC_PDD_BPP_12 0x4U                     /**< 12 bits per pixel (16 bits of memory used). */
#define LCDC_PDD_BPP_16 0x5U                     /**< 16 bits per pixel. */
#define LCDC_PDD_BPP_18 0x6U                     /**< 18 bits per pixel (32 bits of memory used). */
#define LCDC_PDD_BPP_24 0x7U                     /**< 24 bits per pixel (32 bits of memory used). */

/* Polarity selection for panel signals. */
#define LCDC_PDD_ACTIVE_HIGH 0U                  /**< Active high. */
#define LCDC_PDD_ACTIVE_LOW  0x1U                /**< Active low. */

/* Image download type. */
#define LCDC_PDD_BIG_ENDIAN    0x1U              /**< Image in big endian format. */
#define LCDC_PDD_LITTLE_ENDIAN 0U                /**< Image in little endian format. */

/* Swap type. */
#define LCDC_PDD_SWAP_BPP_16_12 0U               /**< 16 or 12 bits per pixel. */
#define LCDC_PDD_SWAP_BPP_OTHER 0x1U             /**< 8, 4, 2 or 1 bits per pixel */

/* Vertical scan direction. */
#define LCDC_PDD_NORMAL_DIR  0U                  /**< Normal direction. */
#define LCDC_PDD_REVERSE_DIR 0x1U                /**< Reverse direction. */

/* ACD clock source. */
#define LCDC_PDD_ACD_SOURCE_FLM 0U               /**< FLM signal selected. */
#define LCDC_PDD_ACD_SOURCE_LP  0x1U             /**< LP signal selected. */

/* SLCLK idle state. */
#define LCDC_PDD_LSCLK_IDLE_ENABLED  0x1U        /**< LSCLK enabled during VSYNC. */
#define LCDC_PDD_LSCLK_IDLE_DISABLED 0U          /**< LSCLK disabled during VSYNC. */

/* SLCLK idle state. */
#define LCDC_PDD_LSCLK_ENABLED  0U               /**< LSCLK enabled when there's no data output. */
#define LCDC_PDD_LSCLK_DISABLED 0x1U             /**< LSCLK disabled when there's no data output. */

/* LSCLK behaviour during sync pulses */
#define LCDC_PDD_ALWAYS_ON    0U                 /**< LSCLK enabled during VSYNC and HSYNC pulse. */
#define LCDC_PDD_ALWAYS_OFF   0x1U               /**< LSCLK disabled during VSYNC and HSYNC pulse. */
#define LCDC_PDD_DURING_VSYNC 0x2U               /**< LSCLK enabled only during VSYNC. */

/* Line data down. */
#define LCDC_PDD_LD_DOWN_ENABLE  0x1U            /**< Line data outputs always zero. */
#define LCDC_PDD_LD_DOWN_DISABLE 0U              /**< Line data outputs operate normally. */

/* Clock source for PWM contrast control. */
#define LCDC_PDD_LINE_PULSE 0U                   /**< Line pulse. */
#define LCDC_PDD_PIXEL_CLK  0x1U                 /**< Pixel clock. */
#define LCDC_PDD_LCD_CLK    0x2U                 /**< LCD clock. */

/* Contrast control */
#define LCDC_PDD_PWM_ENABLED  0x1U               /**< Contrast control enabled. */
#define LCDC_PDD_PWM_DISABLED 0U                 /**< Constrast control disabled. */

/* Interrupt status flags for GetInterruptStatus method. */
#define LCDC_PDD_BEGINING_OF_FRAME     LCDC_LISR_BOF_MASK /**< Beginning of frame. */
#define LCDC_PDD_END_OF_FRAME          LCDC_LISR_EOF_MASK /**< End of frame. */
#define LCDC_PDD_UNDERRUN              LCDC_LISR_UDR_ERR_MASK /**< LCDC buffer underrun error. */
#define LCDC_PDD_GW_BEGINNING_OF_FRAME LCDC_LISR_GW_BOF_MASK /**< Beginning of frame for graphic window. */
#define LCDC_PDD_GW_END_OF_FRAME       LCDC_LISR_GW_EOF_MASK /**< End of frame for graphic window. */
#define LCDC_PDD_GW_UNDERRUN           LCDC_LISR_GW_UDR_ERR_MASK /**< LCDC buffer underrun error for graphic window. */

/* Interrupt type selection. */
#define LCDC_PDD_INT_END_OF_FRAME       0U       /**< Interrupt is set at the end of frame. */
#define LCDC_PDD_INT_BEGINNING_OF_FRAME 0x1U     /**< Interrupt is set at the beginning of frame. */

/* Interrupt assertion time. */
#define LCDC_PDD_LOAD_FROM_MEMORY 0U             /**< End of frame/Beginning of frame interrupt is set on last/first data load from memory. */
#define LCDC_PDD_WRITE_TO_PANEL   0x1U           /**< End of frame/Beginning of frame interrupt is set on last/first data write to LCD panel. */

/* DMA burst length. */
#define LCDC_PDD_FIXED   0x1U                    /**< DMA burst length is fixed. */
#define LCDC_PDD_DYNAMIC 0U                      /**< DMA burst length is dynamic. */

/* Keying control */
#define LCDC_PDD_GWIN_KEYING_ENABLED  0x1U       /**< Color keying enabled. */
#define LCDC_PDD_GWIN_KEYING_DISABLED 0U         /**< Color keying disabled. */

/* Graphic window control */
#define LCDC_PDD_GWIN_ENABLED  0x1U              /**< Graphic window enabled. */
#define LCDC_PDD_GWIN_DISABLED 0U                /**< Graphic window disabled. */

/* Aus mode control. */
#define LCDC_PDD_AUS_ENABLED  0x1U               /**< AUS mode enabled. */
#define LCDC_PDD_AUS_DISABLED 0U                 /**< AUS disabled. */

/* Self-refresh control */
#define LCDC_PDD_SELF_REF_ENABLED  0x1U          /**< Self-refresh mode enabled. */
#define LCDC_PDD_SELF_REF_DISABLED 0U            /**< Self-refresh mode disabled. */

/* Pixel clock source. */
#define LCDC_PDD_EXTAL1 0x1U                     /**< EXTAL1 clock source */
#define LCDC_PDD_CLKDIV 0U                       /**< Clock divider */

/* LCDC clock source. */
#define LCDC_PDD_BUS_CLK   0U                    /**< Bus clock */
#define LCDC_PDD_PLL0      0x1U                  /**< PLL0 output */
#define LCDC_PDD_PLL1      0x2U                  /**< PLL1 output */
#define LCDC_PDD_OSC0ERCLK 0x3U                  /**< OSC0ERCLK output */


/* ----------------------------------------------------------------------------
   -- SetScreenStartAddress
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCD screen start address register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Screen start address. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LSSAR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetScreenStartAddress(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetScreenStartAddress(PeripheralBase, Data) ( \
    LCDC_LSSAR_REG(PeripheralBase) = \
     (uint32_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- SetScreenSize
   ---------------------------------------------------------------------------- */

/**
 * @brief Set screen size. Width is real width in pixels divided by 16, height
 * is in pixels.
 * @param PeripheralBase Peripheral base address.
 * @param Width Screen width divided by 16. This parameter is a 7-bit value.
 * @param Height Screen height. This parameter is a 10-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LSR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetScreenSize(deviceID, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetScreenSize(PeripheralBase, Width, Height) ( \
    (LCDC_LSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LSR_YMAX_MASK))) | ( \
      (uint32_t)(Height)))), \
    (LCDC_LSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LSR_XMAX_MASK))) | ( \
      (uint32_t)((uint32_t)(Width) << LCDC_LSR_XMAX_SHIFT)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetVirtualPageWidth
   ---------------------------------------------------------------------------- */

/**
 * @brief Set virtual page width. Value represent the number of 32-bit words
 * required to hold the data for one virtual line.
 * @param PeripheralBase Peripheral base address.
 * @param Width Number of 32-bit words required to hold the data for one virtual
 *        line. This parameter is a 11-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LVPWR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetVirtualPageWidth(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetVirtualPageWidth(PeripheralBase, Width) ( \
    LCDC_LVPWR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LVPWR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LVPWR_VPW_MASK))) | ( \
      (uint32_t)(Width))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCursorPosition
   ---------------------------------------------------------------------------- */

/**
 * @brief Set HW cursor position on screen.
 * @param PeripheralBase Peripheral base address.
 * @param X Cursor position X. This parameter is a 10-bit value.
 * @param Y Cursor position Y. This parameter is a 10-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LCPR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetCursorPosition(deviceID, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetCursorPosition(PeripheralBase, X, Y) ( \
    (LCDC_LCPR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CYP_MASK))) | ( \
      (uint32_t)(Y)))), \
    (LCDC_LCPR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CXP_MASK))) | ( \
      (uint32_t)((uint32_t)(X) << LCDC_LCPR_CXP_SHIFT)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCursorSize
   ---------------------------------------------------------------------------- */

/**
 * @brief Set the size of HW cursor.
 * @param PeripheralBase Peripheral base address.
 * @param Width Cursor width. This parameter is a 5-bit value.
 * @param Height Cursor height. This parameter is a 5-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LCWHB.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetCursorSize(deviceID, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetCursorSize(PeripheralBase, Width, Height) ( \
    (LCDC_LCWHB_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LCWHB_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCWHB_CH_MASK))) | ( \
      (uint32_t)((uint32_t)(Height) << LCDC_LCWHB_CH_SHIFT)))), \
    (LCDC_LCWHB_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LCWHB_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCWHB_CW_MASK))) | ( \
      (uint32_t)((uint32_t)(Width) << LCDC_LCWHB_CW_SHIFT)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCursorBlinking
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable HW cursor blinking.
 * @param PeripheralBase Peripheral base address.
 * @param BlinkState Enable/disable HW cursor blinking. Use constants from group
 *        "Blinking control.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LCWHB.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetCursorBlinking(deviceID, LCDC_PDD_BLINK_DISABLED);
 *      @endcode
 */
#define LCDC_PDD_SetCursorBlinking(PeripheralBase, BlinkState) ( \
    LCDC_LCWHB_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LCWHB_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCWHB_BK_EN_MASK))) | ( \
      (uint32_t)((uint32_t)(BlinkState) << LCDC_LCWHB_BK_EN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCursorBlinkRate
   ---------------------------------------------------------------------------- */

/**
 * @brief Set cursor blinking rate.
 * @param PeripheralBase Peripheral base address.
 * @param BlinkRate Cursor blinking rate. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LCWHB.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetCursorBlinkRate(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetCursorBlinkRate(PeripheralBase, BlinkRate) ( \
    LCDC_LCWHB_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LCWHB_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCWHB_BD_MASK))) | ( \
      (uint32_t)(BlinkRate))) \
  )

/* ----------------------------------------------------------------------------
   -- SetCursorColor
   ---------------------------------------------------------------------------- */

/**
 * @brief Set HW cursor color.
 * @param PeripheralBase Peripheral base address.
 * @param Red Red component of cursor color. This parameter is a 6-bit value.
 * @param Green Green component of cursor color. This parameter is a 6-bit value.
 * @param Blue Blue component of cursor color. This parameter is a 6-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LCCMR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetCursorColor(deviceID, 1, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetCursorColor(PeripheralBase, Red, Green, Blue) ( \
    (LCDC_LCCMR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LCCMR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LCCMR_CUR_COL_R_MASK)))) | ( \
      (uint32_t)((uint32_t)(Red) << LCDC_LCCMR_CUR_COL_R_SHIFT)))), \
    ((LCDC_LCCMR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LCCMR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LCCMR_CUR_COL_G_MASK)))) | ( \
      (uint32_t)((uint32_t)(Green) << LCDC_LCCMR_CUR_COL_G_SHIFT)))), \
    (LCDC_LCCMR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LCCMR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LCCMR_CUR_COL_B_MASK)))) | ( \
      (uint32_t)(Blue))))) \
  )

/* ----------------------------------------------------------------------------
   -- CursorOperation
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable HW cursor and set arithmetic operation.
 * @param PeripheralBase Peripheral base address.
 * @param Type Cursor operation. Use constants from group "HW cursor operation".
 *        This parameter is 4 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LCPR.
 * @par Example:
 *      @code
 *      LCDC_PDD_CursorOperation(deviceID, LCDC_PDD_DISABLED);
 *      @endcode
 */
#define LCDC_PDD_CursorOperation(PeripheralBase, Type) ( \
    ((uint8_t)(Type) == 0U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LCPR_CC_MASK)), \
      (LCDC_LCPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LCPR_OP_MASK))) : (((uint8_t)(Type) == 0x1U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) = \
       (uint32_t)(( \
        (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CC_MASK))) | ( \
        (uint32_t)((uint32_t)0x1U << LCDC_LCPR_CC_SHIFT)))), \
      (LCDC_LCPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LCPR_OP_MASK))) : (((uint8_t)(Type) == 0x2U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) |= \
       LCDC_LCPR_CC_MASK), \
      (LCDC_LCPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LCPR_OP_MASK))) : (((uint8_t)(Type) == 0x3U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) = \
       (uint32_t)(( \
        (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CC_MASK))) | ( \
        (uint32_t)((uint32_t)0x1U << LCDC_LCPR_CC_SHIFT)))), \
      (LCDC_LCPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LCPR_OP_MASK))) : (((uint8_t)(Type) == 0x4U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) = \
       (uint32_t)(( \
        (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CC_MASK))) | ( \
        (uint32_t)((uint32_t)0x1U << LCDC_LCPR_CC_SHIFT)))), \
      (LCDC_LCPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LCPR_OP_MASK))) : (((uint8_t)(Type) == 0x5U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) = \
       (uint32_t)(( \
        (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CC_MASK))) | ( \
        (uint32_t)((uint32_t)0x2U << LCDC_LCPR_CC_SHIFT)))), \
      (LCDC_LCPR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LCPR_OP_MASK))) : (((uint8_t)(Type) == 0x6U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) |= \
       LCDC_LCPR_CC_MASK), \
      (LCDC_LCPR_REG(PeripheralBase) |= \
       LCDC_LCPR_OP_MASK)) : (((uint8_t)(Type) == 0x7U) ? ( \
      (LCDC_LCPR_REG(PeripheralBase) = \
       (uint32_t)(( \
        (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CC_MASK))) | ( \
        (uint32_t)((uint32_t)0x1U << LCDC_LCPR_CC_SHIFT)))), \
      (LCDC_LCPR_REG(PeripheralBase) |= \
       LCDC_LCPR_OP_MASK)) : ( \
      (LCDC_LCPR_REG(PeripheralBase) = \
       (uint32_t)(( \
        (uint32_t)(LCDC_LCPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LCPR_CC_MASK))) | ( \
        (uint32_t)((uint32_t)0x2U << LCDC_LCPR_CC_SHIFT)))), \
      (LCDC_LCPR_REG(PeripheralBase) |= \
       LCDC_LCPR_OP_MASK)) \
    ))))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDisplayType
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCD panel type.
 * @param PeripheralBase Peripheral base address.
 * @param Type LCD panel type. Use constants from group "LCD panel type.". This
 *        parameter is 2 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetDisplayType(deviceID, LCDC_PDD_MONO);
 *      @endcode
 */
#define LCDC_PDD_SetDisplayType(PeripheralBase, Type) ( \
    ((uint8_t)(Type) == 0U) ? ( \
      (LCDC_LPCR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LPCR_TFT_MASK)), \
      (LCDC_LPCR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LPCR_COLOR_MASK))) : (((uint8_t)(Type) == 0x1U) ? ( \
      (LCDC_LPCR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LPCR_TFT_MASK)), \
      (LCDC_LPCR_REG(PeripheralBase) |= \
       LCDC_LPCR_COLOR_MASK)) : ( \
      (LCDC_LPCR_REG(PeripheralBase) |= \
       LCDC_LPCR_TFT_MASK), \
      (LCDC_LPCR_REG(PeripheralBase) |= \
       LCDC_LPCR_COLOR_MASK)) \
    ) \
  )

/* ----------------------------------------------------------------------------
   -- SetBusWidth
   ---------------------------------------------------------------------------- */

/**
 * @brief Set bus width for monochrome panels.
 * @param PeripheralBase Peripheral base address.
 * @param bus Set bus width for monochrome displays. Use constants from group
 *        "Bus width". This parameter is 2 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetBusWidth(deviceID, LCDC_PDD_BUS_WIDTH_1);
 *      @endcode
 */
#define LCDC_PDD_SetBusWidth(PeripheralBase, bus) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_PBSIZ_MASK))) | ( \
      (uint32_t)((uint32_t)(bus) << LCDC_LPCR_PBSIZ_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetBitsPerPixel
   ---------------------------------------------------------------------------- */

/**
 * @brief Set bits per pixel value.
 * @param PeripheralBase Peripheral base address.
 * @param bpixVal Set bits per pixel value. Use constants from group "Bits per
 *        pixel". This parameter is 3 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetBitsPerPixel(deviceID, LCDC_PDD_BPP_1);
 *      @endcode
 */
#define LCDC_PDD_SetBitsPerPixel(PeripheralBase, bpixVal) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_BPIX_MASK))) | ( \
      (uint32_t)((uint32_t)(bpixVal) << LCDC_LPCR_BPIX_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetPixelPolarity
   ---------------------------------------------------------------------------- */

/**
 * @brief Set pixel polarity.
 * @param PeripheralBase Peripheral base address.
 * @param pol Pixel polarity. Use constants from group "Polarity selection for
 *        panel signals.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetPixelPolarity(deviceID, LCDC_PDD_ACTIVE_HIGH);
 *      @endcode
 */
#define LCDC_PDD_SetPixelPolarity(PeripheralBase, pol) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_PIXPOL_MASK))) | ( \
      (uint32_t)((uint32_t)(pol) << LCDC_LPCR_PIXPOL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetFLM_VSYNCPolarity
   ---------------------------------------------------------------------------- */

/**
 * @brief Set first line marker/VSYNC polarity.
 * @param PeripheralBase Peripheral base address.
 * @param pol FLM/HSYNC polarity. Use constants from group "Polarity selection
 *        for panel signals.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetFLM_VSYNCPolarity(deviceID, LCDC_PDD_ACTIVE_HIGH);
 *      @endcode
 */
#define LCDC_PDD_SetFLM_VSYNCPolarity(PeripheralBase, pol) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_FLMPOL_MASK))) | ( \
      (uint32_t)((uint32_t)(pol) << LCDC_LPCR_FLMPOL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetLP_HSYNCPolarity
   ---------------------------------------------------------------------------- */

/**
 * @brief Set line pulse polarity.
 * @param PeripheralBase Peripheral base address.
 * @param pol Line pulse/VSYNC polarity. Use constants from group "Polarity
 *        selection for panel signals.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetLP_HSYNCPolarity(deviceID, LCDC_PDD_ACTIVE_HIGH);
 *      @endcode
 */
#define LCDC_PDD_SetLP_HSYNCPolarity(PeripheralBase, pol) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_LPPOL_MASK))) | ( \
      (uint32_t)((uint32_t)(pol) << LCDC_LPCR_LPPOL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetLCDClockPolarity
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCD shift clock polarity.
 * @param PeripheralBase Peripheral base address.
 * @param pol Clock polarity. Use constants from group "Polarity selection for
 *        panel signals.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetLCDClockPolarity(deviceID, LCDC_PDD_ACTIVE_HIGH);
 *      @endcode
 */
#define LCDC_PDD_SetLCDClockPolarity(PeripheralBase, pol) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_CLKPOL_MASK))) | ( \
      (uint32_t)((uint32_t)(pol) << LCDC_LPCR_CLKPOL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetOEPolarity
   ---------------------------------------------------------------------------- */

/**
 * @brief Set output enable signal polarity.
 * @param PeripheralBase Peripheral base address.
 * @param pol Output enable polarity. Use constants from group "Polarity
 *        selection for panel signals.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetOEPolarity(deviceID, LCDC_PDD_ACTIVE_HIGH);
 *      @endcode
 */
#define LCDC_PDD_SetOEPolarity(PeripheralBase, pol) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_OEPOL_MASK))) | ( \
      (uint32_t)((uint32_t)(pol) << LCDC_LPCR_OEPOL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetImageEndian
   ---------------------------------------------------------------------------- */

/**
 * @brief Set image download as big or little endian.
 * @param PeripheralBase Peripheral base address.
 * @param endian Set image download type. Use constants from group "Image
 *        download type.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetImageEndian(deviceID, LCDC_PDD_BIG_ENDIAN);
 *      @endcode
 */
#define LCDC_PDD_SetImageEndian(PeripheralBase, endian) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_END_SEL_MASK))) | ( \
      (uint32_t)((uint32_t)(endian) << LCDC_LPCR_END_SEL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SwapSelect
   ---------------------------------------------------------------------------- */

/**
 * @brief Controls data swap for little endian operation.
 * @param PeripheralBase Peripheral base address.
 * @param swp Set swap mode. Use constants from group "Swap type.". This
 *        parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SwapSelect(deviceID, LCDC_PDD_SWAP_BPP_16_12);
 *      @endcode
 */
#define LCDC_PDD_SwapSelect(PeripheralBase, swp) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_SWAP_SEL_MASK))) | ( \
      (uint32_t)((uint32_t)(swp) << LCDC_LPCR_SWAP_SEL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetScanDir
   ---------------------------------------------------------------------------- */

/**
 * @brief Set vertical scan.
 * @param PeripheralBase Peripheral base address.
 * @param dir Scan direction. Use constants from group "Vertical scan
 *        direction.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetScanDir(deviceID, LCDC_PDD_NORMAL_DIR);
 *      @endcode
 */
#define LCDC_PDD_SetScanDir(PeripheralBase, dir) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_REV_VS_MASK))) | ( \
      (uint32_t)((uint32_t)(dir) << LCDC_LPCR_REV_VS_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetScanDir
   ---------------------------------------------------------------------------- */

/**
 * @brief Get vertical scan direction.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Vertical scan direction." for processing
 *         return value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      uint8_t result = LCDC_PDD_GetScanDir(deviceID);
 *      @endcode
 */
#define LCDC_PDD_GetScanDir(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & LCDC_LPCR_REV_VS_MASK)) >> ( \
     LCDC_LPCR_REV_VS_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- ACDClockSource
   ---------------------------------------------------------------------------- */

/**
 * @brief Select ACD clock source.
 * @param PeripheralBase Peripheral base address.
 * @param acd ACD clock source. Use constants from group "ACD clock source.".
 *        This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_ACDClockSource(deviceID, LCDC_PDD_ACD_SOURCE_FLM);
 *      @endcode
 */
#define LCDC_PDD_ACDClockSource(PeripheralBase, acd) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_ACDSEL_MASK))) | ( \
      (uint32_t)((uint32_t)(acd) << LCDC_LPCR_ACDSEL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetACDtoggleRate
   ---------------------------------------------------------------------------- */

/**
 * @brief Set ACD signal toggle rate in ACD clock source cycles.
 * @param PeripheralBase Peripheral base address.
 * @param rate ACD toggle rate. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetACDtoggleRate(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetACDtoggleRate(PeripheralBase, rate) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_ACD_MASK))) | ( \
      (uint32_t)((uint32_t)(rate) << LCDC_LPCR_ACD_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetLSCKLDIdle
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable LSCSLK idle during VSYNC.
 * @param PeripheralBase Peripheral base address.
 * @param en Enable/disable LSCLK during VSYNC. Use constants from group "SLCLK
 *        idle state.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetLSCKLDIdle(deviceID, LCDC_PDD_LSCLK_IDLE_ENABLED);
 *      @endcode
 */
#define LCDC_PDD_SetLSCKLDIdle(PeripheralBase, en) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_SCLKIDLE_MASK))) | ( \
      (uint32_t)((uint32_t)(en) << LCDC_LPCR_SCLKIDLE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableLSCLKWhenNoData
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable LSCSLK when there's no data output.
 * @param PeripheralBase Peripheral base address.
 * @param en Enable/disable LSCLK when there's no data output. Use constants
 *        from group "SLCLK idle state.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_EnableLSCLKWhenNoData(deviceID, LCDC_PDD_LSCLK_ENABLED);
 *      @endcode
 */
#define LCDC_PDD_EnableLSCLKWhenNoData(PeripheralBase, en) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_SCLKIDLE_MASK))) | ( \
      (uint32_t)((uint32_t)(en) << LCDC_LPCR_SCLKIDLE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetLSCLKDuringSYNC
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LSCLK behaviour during HSYNC/VSYNC pulses (encapsulates
 * SetLSCKLDIdle and EnableLSCLKWhenNoData macros).
 * @param PeripheralBase Peripheral base address.
 * @param IdleType LSCLK behaviour. Use constants from group "LSCLK behaviour
 *        during sync pulses". This parameter is 2 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetLSCLKDuringSYNC(deviceID, LCDC_PDD_ALWAYS_ON);
 *      @endcode
 */
#define LCDC_PDD_SetLSCLKDuringSYNC(PeripheralBase, IdleType) ( \
    ((uint8_t)(IdleType) == 0U) ? ( \
      LCDC_LPCR_REG(PeripheralBase) |= \
       LCDC_LPCR_SCLKSEL_MASK) : (((uint8_t)(IdleType) == 0x1U) ? ( \
      (LCDC_LPCR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LPCR_SCLKSEL_MASK)), \
      (LCDC_LPCR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LPCR_SCLKIDLE_MASK))) : ( \
      (LCDC_LPCR_REG(PeripheralBase) &= \
       (uint32_t)(~(uint32_t)LCDC_LPCR_SCLKSEL_MASK)), \
      (LCDC_LPCR_REG(PeripheralBase) |= \
       LCDC_LPCR_SCLKIDLE_MASK)) \
    ) \
  )

/* ----------------------------------------------------------------------------
   -- SetPixelClockDivider
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCD pixel clock divider.
 * @param PeripheralBase Peripheral base address.
 * @param div Pixel clock divider. This parameter is a 6-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetPixelClockDivider(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetPixelClockDivider(PeripheralBase, div) ( \
    LCDC_LPCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCR_PCD_MASK))) | ( \
      (uint32_t)(div))) \
  )

/* ----------------------------------------------------------------------------
   -- SetHorizontalTiming
   ---------------------------------------------------------------------------- */

/**
 * @brief Set horizontal timing.
 * @param PeripheralBase Peripheral base address.
 * @param HSYNC Width of HSYNC pulse. This parameter is a 6-bit value.
 * @param WaitOEandHSYNC Wait between OE and HSYNC signal. This parameter is a
 *        8-bit value.
 * @param WaitHSYNCandOE Wait between HSYNC and OE signal. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LHCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetHorizontalTiming(deviceID, 1, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetHorizontalTiming(PeripheralBase, HSYNC, WaitOEandHSYNC, WaitHSYNCandOE) ( \
    (LCDC_LHCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LHCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LHCR_H_WIDTH_MASK))) | ( \
      (uint32_t)((uint32_t)(HSYNC) << LCDC_LHCR_H_WIDTH_SHIFT)))), \
    ((LCDC_LHCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LHCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LHCR_H_WAIT_1_MASK))) | ( \
      (uint32_t)((uint32_t)(WaitOEandHSYNC) << LCDC_LHCR_H_WAIT_1_SHIFT)))), \
    (LCDC_LHCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LHCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LHCR_H_WAIT_2_MASK))) | ( \
      (uint32_t)(WaitHSYNCandOE))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetVerticalTiming
   ---------------------------------------------------------------------------- */

/**
 * @brief Set vertical timing.
 * @param PeripheralBase Peripheral base address.
 * @param VSYNC Width of VSYNC pulse. This parameter is a 6-bit value.
 * @param WaitOEandVSYNC Wait between OE and VSYNC signal. This parameter is a
 *        8-bit value.
 * @param WaitVSYNCandOE Wait between VSYNC and OE signal. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LVCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetVerticalTiming(deviceID, 1, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetVerticalTiming(PeripheralBase, VSYNC, WaitOEandVSYNC, WaitVSYNCandOE) ( \
    (LCDC_LVCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LVCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LVCR_V_WIDTH_MASK))) | ( \
      (uint32_t)((uint32_t)(VSYNC) << LCDC_LVCR_V_WIDTH_SHIFT)))), \
    ((LCDC_LVCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LVCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LVCR_V_WAIT_1_MASK))) | ( \
      (uint32_t)((uint32_t)(WaitOEandVSYNC) << LCDC_LVCR_V_WAIT_1_SHIFT)))), \
    (LCDC_LVCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LVCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LVCR_V_WAIT_2_MASK))) | ( \
      (uint32_t)(WaitVSYNCandOE))))) \
  )

/* ----------------------------------------------------------------------------
   -- LineDataDown
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable line data output [0..23] to zero.
 * @param PeripheralBase Peripheral base address.
 * @param ldDownEn Set line data output to zero. Use constants from group "Line
 *        data down.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_LineDataDown(deviceID, LCDC_PDD_LD_DOWN_ENABLE);
 *      @endcode
 */
#define LCDC_PDD_LineDataDown(PeripheralBase, ldDownEn) ( \
    LCDC_LPCCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCCR_LDMSK_MASK))) | ( \
      (uint32_t)((uint32_t)(ldDownEn) << LCDC_LPCCR_LDMSK_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- PWMClockSource
   ---------------------------------------------------------------------------- */

/**
 * @brief Select contrast PWM clock source.
 * @param PeripheralBase Peripheral base address.
 * @param clkSrc Set the clock source for contrast PWM. Use constants from group
 *        "Clock source for PWM contrast control.". This parameter is 2 bits
 *        wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_PWMClockSource(deviceID, LCDC_PDD_LINE_PULSE);
 *      @endcode
 */
#define LCDC_PDD_PWMClockSource(PeripheralBase, clkSrc) ( \
    LCDC_LPCCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCCR_SCR_MASK))) | ( \
      (uint32_t)((uint32_t)(clkSrc) << LCDC_LPCCR_SCR_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetPWMPulseWidth
   ---------------------------------------------------------------------------- */

/**
 * @brief Set PWM pulse width for contrast control.
 * @param PeripheralBase Peripheral base address.
 * @param pwmW Width of PWM pulse. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetPWMPulseWidth(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetPWMPulseWidth(PeripheralBase, pwmW) ( \
    LCDC_LPCCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCCR_PW_MASK))) | ( \
      (uint32_t)(pwmW))) \
  )

/* ----------------------------------------------------------------------------
   -- ContrastControl
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable PWM contrast control for panel.
 * @param PeripheralBase Peripheral base address.
 * @param contrast Enable/disable PWM constrast control. Use constants from
 *        group "Contrast control". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPCCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_ContrastControl(deviceID, LCDC_PDD_PWM_ENABLED);
 *      @endcode
 */
#define LCDC_PDD_ContrastControl(PeripheralBase, contrast) ( \
    LCDC_LPCCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPCCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPCCR_CC_EN_MASK))) | ( \
      (uint32_t)((uint32_t)(contrast) << LCDC_LPCCR_CC_EN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- ReadInterruptStatus
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns interrupt status. This autmatically clear interrupt flags.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Interrupt status flags for
 *         GetInterruptStatus method." for processing return value.
 * @remarks The macro accesses the following registers: LCDC_LISR.
 * @par Example:
 *      @code
 *      uint32_t result = LCDC_PDD_ReadInterruptStatus(deviceID);
 *      @endcode
 */
#define LCDC_PDD_ReadInterruptStatus(PeripheralBase) ( \
    LCDC_LISR_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable interrupt srouces selected by mask.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupt enable requests (see TInterruptStatus). Use
 *        constants from group "Interrupt status flags for GetInterruptStatus
 *        method.". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LIER.
 * @par Example:
 *      @code
 *      LCDC_PDD_EnableInterruptMask(deviceID, LCDC_PDD_BEGINING_OF_FRAME);
 *      @endcode
 */
#define LCDC_PDD_EnableInterruptMask(PeripheralBase, Mask) ( \
    LCDC_LIER_REG(PeripheralBase) |= \
     (uint32_t)(Mask) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterruptMask
   ---------------------------------------------------------------------------- */

/**
 * @brief Disable interrupt srouces selected by mask.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Mask of interrupt disable requests (see TInterruptStatus). Use
 *        constants from group "Interrupt status flags for GetInterruptStatus
 *        method.". This parameter is 32 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LIER.
 * @par Example:
 *      @code
 *      LCDC_PDD_DisableInterruptMask(deviceID, LCDC_PDD_BEGINING_OF_FRAME);
 *      @endcode
 */
#define LCDC_PDD_DisableInterruptMask(PeripheralBase, Mask) ( \
    LCDC_LIER_REG(PeripheralBase) &= \
     (uint32_t)(~(uint32_t)(Mask)) \
  )

/* ----------------------------------------------------------------------------
   -- SetFrameInterruptPosition
   ---------------------------------------------------------------------------- */

/**
 * @brief Select interrupt position.
 * @param PeripheralBase Peripheral base address.
 * @param intSel Interrupt position. Use constants from group "Interrupt type
 *        selection.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LICR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetFrameInterruptPosition(deviceID, LCDC_PDD_INT_END_OF_FRAME);
 *      @endcode
 */
#define LCDC_PDD_SetFrameInterruptPosition(PeripheralBase, intSel) ( \
    LCDC_LICR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LICR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LICR_INTCON_MASK))) | ( \
      (uint32_t)(intSel))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinInterruptPosition
   ---------------------------------------------------------------------------- */

/**
 * @brief Select interrupt position for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param intSel Interrupt position. Use constants from group "Interrupt type
 *        selection.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LICR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinInterruptPosition(deviceID, LCDC_PDD_INT_END_OF_FRAME);
 *      @endcode
 */
#define LCDC_PDD_SetGWinInterruptPosition(PeripheralBase, intSel) ( \
    LCDC_LICR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LICR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LICR_GW_INT_CON_MASK)))) | ( \
      (uint32_t)((uint32_t)(intSel) << LCDC_LICR_GW_INT_CON_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SelectIntSyn
   ---------------------------------------------------------------------------- */

/**
 * @brief Select interrupt assertion time.
 * @param PeripheralBase Peripheral base address.
 * @param intSel Interrupt assertion time. Use constants from group "Interrupt
 *        assertion time.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LICR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SelectIntSyn(deviceID, LCDC_PDD_LOAD_FROM_MEMORY);
 *      @endcode
 */
#define LCDC_PDD_SelectIntSyn(PeripheralBase, intSel) ( \
    LCDC_LICR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LICR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LICR_INTSYN_MASK))) | ( \
      (uint32_t)((uint32_t)(intSel) << LCDC_LICR_INTSYN_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetupInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Setup interrupt conditions.
 * @param PeripheralBase Peripheral base address.
 * @param intPos Interrupt position. Use constants from group "Interrupt type
 *        selection.". This parameter is 1 bit wide.
 * @param gwinIntPos Interrupt assertion time. Use constants from group
 *        "Interrupt type selection.". This parameter is 1 bit wide.
 * @param intTime Interrupt position for graphic window. Use constants from
 *        group "Interrupt assertion time.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LICR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetupInterrupts(deviceID, LCDC_PDD_INT_END_OF_FRAME,
 *      LCDC_PDD_INT_END_OF_FRAME, LCDC_PDD_LOAD_FROM_MEMORY);
 *      @endcode
 */
#define LCDC_PDD_SetupInterrupts(PeripheralBase, intPos, gwinIntPos, intTime) ( \
    (LCDC_LICR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LICR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LICR_INTSYN_MASK))) | ( \
      (uint32_t)((uint32_t)(intTime) << LCDC_LICR_INTSYN_SHIFT)))), \
    ((LCDC_LICR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LICR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LICR_GW_INT_CON_MASK)))) | ( \
      (uint32_t)((uint32_t)(gwinIntPos) << LCDC_LICR_GW_INT_CON_SHIFT)))), \
    (LCDC_LICR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LICR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LICR_INTCON_MASK))) | ( \
      (uint32_t)(intPos))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDMABurstLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Set DMA burst length.
 * @param PeripheralBase Peripheral base address.
 * @param blen Burst length. Use constants from group "DMA burst length.". This
 *        parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetDMABurstLength(deviceID, LCDC_PDD_FIXED);
 *      @endcode
 */
#define LCDC_PDD_SetDMABurstLength(PeripheralBase, blen) ( \
    LCDC_LDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LDCR_BURST_MASK))) | ( \
      (uint32_t)((uint32_t)(blen) << LCDC_LDCR_BURST_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDMAHighMark
   ---------------------------------------------------------------------------- */

/**
 * @brief Set DMA high mark.
 * @param PeripheralBase Peripheral base address.
 * @param hm DMA high mark value. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetDMAHighMark(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetDMAHighMark(PeripheralBase, hm) ( \
    LCDC_LDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LDCR_HM_MASK))) | ( \
      (uint32_t)((uint32_t)(hm) << LCDC_LDCR_HM_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDMATriggerMark
   ---------------------------------------------------------------------------- */

/**
 * @brief Set DMA trigger mark.
 * @param PeripheralBase Peripheral base address.
 * @param tm DMA trigger mark value. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetDMATriggerMark(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetDMATriggerMark(PeripheralBase, tm) ( \
    LCDC_LDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LDCR_TM_MASK))) | ( \
      (uint32_t)(tm))) \
  )

/* ----------------------------------------------------------------------------
   -- SetupDMA
   ---------------------------------------------------------------------------- */

/**
 * @brief Setup DMA (encapsulates SetDMABurstLength, SetDMAHighMark and
 * SetDMATriggerMark macros).
 * @param PeripheralBase Peripheral base address.
 * @param blen Burst length. Use constants from group "DMA burst length.". This
 *        parameter is 1 bit wide.
 * @param hm DMA high mark value. This parameter is a 7-bit value.
 * @param tm DMA trigger mark value. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetupDMA(deviceID, LCDC_PDD_FIXED, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetupDMA(PeripheralBase, blen, hm, tm) ( \
    (LCDC_LDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LDCR_BURST_MASK))) | ( \
      (uint32_t)((uint32_t)(blen) << LCDC_LDCR_BURST_SHIFT)))), \
    ((LCDC_LDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LDCR_HM_MASK))) | ( \
      (uint32_t)((uint32_t)(hm) << LCDC_LDCR_HM_SHIFT)))), \
    (LCDC_LDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LDCR_TM_MASK))) | ( \
      (uint32_t)(tm))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetPanningOffset
   ---------------------------------------------------------------------------- */

/**
 * @brief Set panning offset.
 * @param PeripheralBase Peripheral base address.
 * @param pos Panning offset value. This parameter is a 5-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LPOR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetPanningOffset(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetPanningOffset(PeripheralBase, pos) ( \
    LCDC_LPOR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LPOR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LPOR_POS_MASK))) | ( \
      (uint32_t)(pos))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinStartAddress
   ---------------------------------------------------------------------------- */

/**
 * @brief Set graphic window start address register.
 * @param PeripheralBase Peripheral base address.
 * @param Data Graphic window start address. This parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWSAR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinStartAddress(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinStartAddress(PeripheralBase, Data) ( \
    LCDC_LGWSAR_REG(PeripheralBase) = \
     (uint32_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinSize
   ---------------------------------------------------------------------------- */

/**
 * @brief Set graphic window size. Width is real width in pixels divided by 16,
 * height is in pixels.
 * @param PeripheralBase Peripheral base address.
 * @param Width Graphic window width divided by 16. This parameter is a 7-bit
 *        value.
 * @param Height Graphic window height. This parameter is a 10-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWSR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinSize(deviceID, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinSize(PeripheralBase, Width, Height) ( \
    (LCDC_LGWSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWSR_GWH_MASK))) | ( \
      (uint32_t)(Height)))), \
    (LCDC_LGWSR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWSR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWSR_GWW_MASK))) | ( \
      (uint32_t)((uint32_t)(Width) << LCDC_LGWSR_GWW_SHIFT)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinVirtualPageWidth
   ---------------------------------------------------------------------------- */

/**
 * @brief Set virtual page width for graphic window. Value represent the number
 * of 32-bit words required to hold the data for one virtual line.
 * @param PeripheralBase Peripheral base address.
 * @param Width Number of 32-bit words required to hold the data for one virtual
 *        line. This parameter is a 11-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWVPWR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinVirtualPageWidth(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinVirtualPageWidth(PeripheralBase, Width) ( \
    LCDC_LGWVPWR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LGWVPWR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LGWVPWR_GWVPW_MASK)))) | ( \
      (uint32_t)(Width))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinPosition
   ---------------------------------------------------------------------------- */

/**
 * @brief Set graphic window position on screen.
 * @param PeripheralBase Peripheral base address.
 * @param X Window position X. This parameter is a 10-bit value.
 * @param Y Window position Y. This parameter is a 10-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWPR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinPosition(deviceID, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinPosition(PeripheralBase, X, Y) ( \
    (LCDC_LGWPR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWPR_GWYP_MASK))) | ( \
      (uint32_t)(Y)))), \
    (LCDC_LGWPR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWPR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWPR_GWXP_MASK))) | ( \
      (uint32_t)((uint32_t)(X) << LCDC_LGWPR_GWXP_SHIFT)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinPanningOffset
   ---------------------------------------------------------------------------- */

/**
 * @brief Set panning offset for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param pos Panning offset value. This parameter is a 5-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWPOR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinPanningOffset(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinPanningOffset(PeripheralBase, pos) ( \
    LCDC_LGWPOR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWPOR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWPOR_GWPO_MASK))) | ( \
      (uint32_t)(pos))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinAlphaValue
   ---------------------------------------------------------------------------- */

/**
 * @brief Set global alpha value for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param alpha Alpha value. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinAlphaValue(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinAlphaValue(PeripheralBase, alpha) ( \
    LCDC_LGWCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWCR_GWAV_MASK))) | ( \
      (uint32_t)((uint32_t)(alpha) << LCDC_LGWCR_GWAV_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GWinColorKeying
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable graphic window color keying.
 * @param PeripheralBase Peripheral base address.
 * @param keying Enable/disable color keying. Use constants from group "Keying
 *        control". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_GWinColorKeying(deviceID, LCDC_PDD_GWIN_KEYING_ENABLED);
 *      @endcode
 */
#define LCDC_PDD_GWinColorKeying(PeripheralBase, keying) ( \
    LCDC_LGWCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWCR_GWCKE_MASK))) | ( \
      (uint32_t)((uint32_t)(keying) << LCDC_LGWCR_GWCKE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinKeyingColor
   ---------------------------------------------------------------------------- */

/**
 * @brief Set RGB value for color keying for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param red Red value. This parameter is a 6-bit value.
 * @param green Green value. This parameter is a 6-bit value.
 * @param blue Blue value. This parameter is a 6-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinKeyingColor(deviceID, 1, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinKeyingColor(PeripheralBase, red, green, blue) ( \
    (LCDC_LGWCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWCR_GWCKR_MASK))) | ( \
      (uint32_t)((uint32_t)(red) << LCDC_LGWCR_GWCKR_SHIFT)))), \
    ((LCDC_LGWCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWCR_GWCKG_MASK))) | ( \
      (uint32_t)((uint32_t)(green) << LCDC_LGWCR_GWCKG_SHIFT)))), \
    (LCDC_LGWCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWCR_GWCKB_MASK))) | ( \
      (uint32_t)(blue))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinScanDir
   ---------------------------------------------------------------------------- */

/**
 * @brief Set graphic window vertical scan directio.
 * @param PeripheralBase Peripheral base address.
 * @param dir Scan direction. Use constants from group "Vertical scan
 *        direction.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinScanDir(deviceID, LCDC_PDD_NORMAL_DIR);
 *      @endcode
 */
#define LCDC_PDD_SetGWinScanDir(PeripheralBase, dir) ( \
    LCDC_LGWCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWCR_GW_RVS_MASK))) | ( \
      (uint32_t)((uint32_t)(dir) << LCDC_LGWCR_GW_RVS_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- GetGWinScanDir
   ---------------------------------------------------------------------------- */

/**
 * @brief Get vertical scan direction for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "Vertical scan direction." for processing
 *         return value. The value is cast to "uint8_t".
 * @remarks The macro accesses the following registers: LCDC_LGWCR.
 * @par Example:
 *      @code
 *      uint8_t result = LCDC_PDD_GetGWinScanDir(deviceID);
 *      @endcode
 */
#define LCDC_PDD_GetGWinScanDir(PeripheralBase) ( \
    (uint8_t)(( \
     (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & LCDC_LGWCR_GW_RVS_MASK)) >> ( \
     LCDC_LGWCR_GW_RVS_SHIFT)) \
  )

/* ----------------------------------------------------------------------------
   -- GWinEnable
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param gw Enable/disable graphic window. Use constants from group "Graphic
 *        window control". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_GWinEnable(deviceID, LCDC_PDD_GWIN_ENABLED);
 *      @endcode
 */
#define LCDC_PDD_GWinEnable(PeripheralBase, gw) ( \
    LCDC_LGWCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWCR_GWE_MASK))) | ( \
      (uint32_t)((uint32_t)(gw) << LCDC_LGWCR_GWE_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinDMABurstLength
   ---------------------------------------------------------------------------- */

/**
 * @brief Set DMA burst length for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param blen Burst length. Use constants from group "DMA burst length.". This
 *        parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinDMABurstLength(deviceID, LCDC_PDD_FIXED);
 *      @endcode
 */
#define LCDC_PDD_SetGWinDMABurstLength(PeripheralBase, blen) ( \
    LCDC_LGWDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWDCR_GWBT_MASK))) | ( \
      (uint32_t)((uint32_t)(blen) << LCDC_LGWDCR_GWBT_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinDMAHighMark
   ---------------------------------------------------------------------------- */

/**
 * @brief Set DMA high mark for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param hm DMA high mark value. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinDMAHighMark(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinDMAHighMark(PeripheralBase, hm) ( \
    LCDC_LGWDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWDCR_GWHM_MASK))) | ( \
      (uint32_t)((uint32_t)(hm) << LCDC_LGWDCR_GWHM_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetGWinDMATriggerMark
   ---------------------------------------------------------------------------- */

/**
 * @brief Set DMA trigger mark for graphic window.
 * @param PeripheralBase Peripheral base address.
 * @param tm DMA trigger mark value. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetGWinDMATriggerMark(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetGWinDMATriggerMark(PeripheralBase, tm) ( \
    LCDC_LGWDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWDCR_GWTM_MASK))) | ( \
      (uint32_t)(tm))) \
  )

/* ----------------------------------------------------------------------------
   -- SetupGWinDMA
   ---------------------------------------------------------------------------- */

/**
 * @brief Setup DMA for graphic window (encapsulates SetGWinDMABurstLength,
 * SetGWinDMAHighMark and SetGWinDMATriggerMark macros).
 * @param PeripheralBase Peripheral base address.
 * @param blen Burst length. Use constants from group "DMA burst length.". This
 *        parameter is 1 bit wide.
 * @param hm DMA high mark value. This parameter is a 7-bit value.
 * @param tm DMA trigger mark value. This parameter is a 7-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LGWDCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetupGWinDMA(deviceID, LCDC_PDD_FIXED, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetupGWinDMA(PeripheralBase, blen, hm, tm) ( \
    (LCDC_LGWDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWDCR_GWBT_MASK))) | ( \
      (uint32_t)((uint32_t)(blen) << LCDC_LGWDCR_GWBT_SHIFT)))), \
    ((LCDC_LGWDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWDCR_GWHM_MASK))) | ( \
      (uint32_t)((uint32_t)(hm) << LCDC_LGWDCR_GWHM_SHIFT)))), \
    (LCDC_LGWDCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(LCDC_LGWDCR_REG(PeripheralBase) & (uint32_t)(~(uint32_t)LCDC_LGWDCR_GWTM_MASK))) | ( \
      (uint32_t)(tm))))) \
  )

/* ----------------------------------------------------------------------------
   -- AUSMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable AUS mode.
 * @param PeripheralBase Peripheral base address.
 * @param aus Enable/disable AUS mode. Use constants from group "Aus mode
 *        control.". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LAUSCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_AUSMode(deviceID, LCDC_PDD_AUS_ENABLED);
 *      @endcode
 */
#define LCDC_PDD_AUSMode(PeripheralBase, aus) ( \
    LCDC_LAUSCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LAUSCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LAUSCR_AUS_Mode_MASK)))) | ( \
      (uint32_t)((uint32_t)(aus) << LCDC_LAUSCR_AUS_Mode_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetAUSKeyingColor
   ---------------------------------------------------------------------------- */

/**
 * @brief Set RGB value for graphic window color keying in AUS mode.
 * @param PeripheralBase Peripheral base address.
 * @param red Red value. This parameter is a 8-bit value.
 * @param green Green value. This parameter is a 8-bit value.
 * @param blue Blue value. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LAUSCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetAUSKeyingColor(deviceID, 1, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetAUSKeyingColor(PeripheralBase, red, green, blue) ( \
    (LCDC_LAUSCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LAUSCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LAUSCR_AGWCKR_MASK)))) | ( \
      (uint32_t)((uint32_t)(red) << LCDC_LAUSCR_AGWCKR_SHIFT)))), \
    ((LCDC_LAUSCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LAUSCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LAUSCR_AGWCKG_MASK)))) | ( \
      (uint32_t)((uint32_t)(green) << LCDC_LAUSCR_AGWCKG_SHIFT)))), \
    (LCDC_LAUSCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LAUSCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LAUSCR_AGWCKB_MASK)))) | ( \
      (uint32_t)(blue))))) \
  )

/* ----------------------------------------------------------------------------
   -- SetAUSCursorColor
   ---------------------------------------------------------------------------- */

/**
 * @brief Set HW cursor color in AUS mode.
 * @param PeripheralBase Peripheral base address.
 * @param Red Red component of cursor color. This parameter is a 8-bit value.
 * @param Green Green component of cursor color. This parameter is a 8-bit value.
 * @param Blue Blue component of cursor color. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LAUSCCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetAUSCursorColor(deviceID, 1, 1, 1);
 *      @endcode
 */
#define LCDC_PDD_SetAUSCursorColor(PeripheralBase, Red, Green, Blue) ( \
    (LCDC_LAUSCCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LAUSCCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LAUSCCR_ACUR_COL_R_MASK)))) | ( \
      (uint32_t)((uint32_t)(Red) << LCDC_LAUSCCR_ACUR_COL_R_SHIFT)))), \
    ((LCDC_LAUSCCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LAUSCCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LAUSCCR_ACUR_COL_G_MASK)))) | ( \
      (uint32_t)((uint32_t)(Green) << LCDC_LAUSCCR_ACUR_COL_G_SHIFT)))), \
    (LCDC_LAUSCCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LAUSCCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LAUSCCR_ACUR_COL_B_MASK)))) | ( \
      (uint32_t)(Blue))))) \
  )

/* ----------------------------------------------------------------------------
   -- SelfRefreshMode
   ---------------------------------------------------------------------------- */

/**
 * @brief Enable/disable self-refresh mode.
 * @param PeripheralBase Peripheral base address.
 * @param srEn Enable/disable AUS mode. Use constants from group "Self-refresh
 *        control". This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: LCDC_LRMCR.
 * @par Example:
 *      @code
 *      LCDC_PDD_SelfRefreshMode(deviceID, LCDC_PDD_SELF_REF_ENABLED);
 *      @endcode
 */
#define LCDC_PDD_SelfRefreshMode(PeripheralBase, srEn) ( \
    LCDC_LRMCR_REG(PeripheralBase) = \
     (uint32_t)(( \
      (uint32_t)(( \
       LCDC_LRMCR_REG(PeripheralBase)) & ( \
       (uint32_t)(~(uint32_t)LCDC_LRMCR_SELF_REF_MASK)))) | ( \
      (uint32_t)(srEn))) \
  )

/* ----------------------------------------------------------------------------
   -- EnableLCD
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables LCD module.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      LCDC_PDD_EnableLCD(deviceID);
 *      @endcode
 */
#define LCDC_PDD_EnableLCD(PeripheralBase) ( \
    SIM_MCR_REG(SIM_BASE_PTR) |= \
         (uint32_t)(SIM_MCR_LCDSTART_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- DisableLCD
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables LCD module.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      LCDC_PDD_DisableLCD(deviceID);
 *      @endcode
 */
#define LCDC_PDD_DisableLCD(PeripheralBase) ( \
    SIM_MCR_REG(SIM_BASE_PTR) &= \
         ~(uint32_t)(SIM_MCR_LCDSTART_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- SetClkDiv
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCDC clock div (SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param div LCDC clk fraction multiply. This parameter is a 12-bit value.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetClkDiv(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetClkDiv(PeripheralBase, div) ( \
    SIM_CLKDIV3_REG(SIM_BASE_PTR) =  \
         (uint32_t)((  \
          (uint32_t)(SIM_CLKDIV3_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_CLKDIV3_LCDCDIV_MASK))) | (  \
          (uint32_t)((uint32_t)(div) << SIM_CLKDIV3_LCDCDIV_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetClkFrac
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCDC clock frac (SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param frac LCDC clk fraction divide. This parameter is a 12-bit value.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetClkFrac(deviceID, 1);
 *      @endcode
 */
#define LCDC_PDD_SetClkFrac(PeripheralBase, frac) ( \
    SIM_CLKDIV3_REG(SIM_BASE_PTR) =  \
         (uint32_t)((  \
          (uint32_t)(SIM_CLKDIV3_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_CLKDIV3_LCDCFRAC_MASK))) | (  \
          (uint32_t)((uint32_t)(frac) << SIM_CLKDIV3_LCDCFRAC_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetSLCDSrc
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCDC pixel clock source (SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param src LCDC clock source. Use constants from group "Pixel clock source.".
 *        This parameter is 1 bit wide.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetSLCDSrc(deviceID, LCDC_PDD_EXTAL1);
 *      @endcode
 */
#define LCDC_PDD_SetSLCDSrc(PeripheralBase, src) ( \
    SIM_SOPT2_REG(SIM_BASE_PTR) =  \
         (uint32_t)((  \
          (uint32_t)(SIM_SOPT2_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_SOPT2_LCDC_CLKSEL_MASK))) | (  \
          (uint32_t)((uint32_t)(src) << SIM_SOPT2_LCDC_CLKSEL_SHIFT))) \
  )

/* ----------------------------------------------------------------------------
   -- SetClkSrc
   ---------------------------------------------------------------------------- */

/**
 * @brief Set LCDC clock source (SIM module).
 * @param PeripheralBase Peripheral base address.
 * @param src LCDC clock source. Use constants from group "LCDC clock source.".
 *        This parameter is 2 bits wide.
 * @return Returns a value of void type.
 * @par Example:
 *      @code
 *      LCDC_PDD_SetClkSrc(deviceID, LCDC_PDD_BUS_CLK);
 *      @endcode
 */
#define LCDC_PDD_SetClkSrc(PeripheralBase, src) ( \
    SIM_SOPT2_REG(SIM_BASE_PTR) =  \
         (uint32_t)((  \
          (uint32_t)(SIM_SOPT2_REG(SIM_BASE_PTR) & (uint32_t)(~(uint32_t)SIM_SOPT2_LCDCSRC_MASK))) | (  \
          (uint32_t)((uint32_t)(src) << SIM_SOPT2_LCDCSRC_SHIFT))) \
  )
#endif  /* #if defined(LCDC_PDD_H_) */

/* LCDC_PDD.h, eof. */
