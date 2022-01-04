/*
  PDD layer implementation for peripheral type FTFL
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(FTFL_PDD_H_)
#define FTFL_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error FTFL PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MK10D10) /* FTFL */ && \
      !defined(MCU_MK10D5) /* FTFL */ && \
      !defined(MCU_MK10D7) /* FTFL */ && \
      !defined(MCU_MK10DZ10) /* FTFL */ && \
      !defined(MCU_MK11D5) /* FTFL */ && \
      !defined(MCU_MK11D5WS) /* FTFL */ && \
      !defined(MCU_MK12D5) /* FTFL */ && \
      !defined(MCU_MK20D10) /* FTFL */ && \
      !defined(MCU_MK20D5) /* FTFL */ && \
      !defined(MCU_MK20D7) /* FTFL */ && \
      !defined(MCU_MK20DZ10) /* FTFL */ && \
      !defined(MCU_MK21D5) /* FTFL */ && \
      !defined(MCU_MK21D5WS) /* FTFL */ && \
      !defined(MCU_MK22D5) /* FTFL */ && \
      !defined(MCU_MK30D10) /* FTFL */ && \
      !defined(MCU_MK30D7) /* FTFL */ && \
      !defined(MCU_MK30DZ10) /* FTFL */ && \
      !defined(MCU_MK40D10) /* FTFL */ && \
      !defined(MCU_MK40D7) /* FTFL */ && \
      !defined(MCU_MK40DZ10) /* FTFL */ && \
      !defined(MCU_MK40X256VMD100) /* FTFL */ && \
      !defined(MCU_MK50D10) /* FTFL */ && \
      !defined(MCU_MK50D7) /* FTFL */ && \
      !defined(MCU_MK50DZ10) /* FTFL */ && \
      !defined(MCU_MK51D10) /* FTFL */ && \
      !defined(MCU_MK51D7) /* FTFL */ && \
      !defined(MCU_MK51DZ10) /* FTFL */ && \
      !defined(MCU_MK52D10) /* FTFL */ && \
      !defined(MCU_MK52DZ10) /* FTFL */ && \
      !defined(MCU_MK53D10) /* FTFL */ && \
      !defined(MCU_MK53DZ10) /* FTFL */ && \
      !defined(MCU_MK60D10) /* FTFL */ && \
      !defined(MCU_MK60DZ10) /* FTFL */ && \
      !defined(MCU_MK60N512VMD100) /* FTFL */ && \
      !defined(MCU_PCK20L4) /* FTFL */
  // Unsupported MCU is active
  #error FTFL PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* ClearFlags constants */
#define FTFL_PDD_COMMAND_COMPLETE          FTFL_FSTAT_CCIF_MASK /**< Command complete flag mask */
#define FTFL_PDD_READ_COLLISION_ERROR      FTFL_FSTAT_RDCOLERR_MASK /**< Read collision error flag mask */
#define FTFL_PDD_ACCESS_ERROR              FTFL_FSTAT_ACCERR_MASK /**< Access error flag mask */
#define FTFL_PDD_PROTECTION_VIOLATION      FTFL_FSTAT_FPVIOL_MASK /**< Protection violation flag mask */
#define FTFL_PDD_COMMAND_COMPLETION_STATUS FTFL_FSTAT_MGSTAT0_MASK /**< Command completion ststus flag mask */

/* EnableInterrupts, DisableInterrupts constants */
#define FTFL_PDD_COMMAND_COMPLETE_INT     FTFL_FCNFG_CCIE_MASK /**< Command complete interrupt mask */
#define FTFL_PDD_READ_COLLISION_ERROR_INT FTFL_FCNFG_RDCOLLIE_MASK /**< Read collision error interrupt mask */

/* SetFCCOBCommand constants */
#define FTFL_PDD_READ_1S_BLOCK              0U   /**< Read 1s Block command value */
#define FTFL_PDD_READ_1S_SECTION            0x1U /**< Read 1s Section command value */
#define FTFL_PDD_PROGRAM_CHECK              0x2U /**< Program Check command value */
#define FTFL_PDD_READ_RESOURCE              0x3U /**< Read Resource command value */
#define FTFL_PDD_PROGRAM_LONGWORD           0x6U /**< Program Longword command value */
#define FTFL_PDD_ERASE_FLASH_BLOCK          0x8U /**< Erase Flash Block command value */
#define FTFL_PDD_ERASE_FLASH_SECTOR         0x9U /**< Erase Flash Sector command value */
#define FTFL_PDD_PROGRAM_SECTION            0xBU /**< Program Section command value */
#define FTFL_PDD_READ_1S_ALL_BLOCKS         0x40U /**< Read 1s All Blocks command value */
#define FTFL_PDD_PDD_READ_ONCE              0x41U /**< Read Once command value */
#define FTFL_PDD_PROGRAM_ONCE               0x43U /**< Program Once command value */
#define FTFL_PDD_ERASE_ALL_BLOCKS           0x44U /**< Erase All Blocks command value */
#define FTFL_PDD_VERIFY_BACKDOOR_ACCESS_KEY 0x45U /**< Verify Backdoor Access Key command value */
#define FTFL_PDD_PROGRAM_PARTITION          0x80U /**< Program Partition command value */
#define FTFL_PDD_SET_EERAM_FUCTION          0x81U /**< Set FlexRAM Function command value */

/* BackDoorKey constants */
#define FTFL_PDD_BACKDOOR_KEY_ENABLED  0U        /**< Backdoor key enable constant */
#define FTFL_PDD_BACKDOOR_KEY_DISABLED 0x1U      /**< Backdoor key disable constant */

/* Security state constants */
#define FTFL_PDD_UNSECURED 0U                    /**< Unsecure constant */
#define FTFL_PDD_SECURED   0x1U                  /**< Secure constant */

/* FlashProtection constants */
#define FTFL_PDD_UNPROTECTED 0U                  /**< Unprotect constant */
#define FTFL_PDD_PROTECTED   0x1U                /**< Protect constant */


/* ----------------------------------------------------------------------------
   -- ReadStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns value of the Flash status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "ClearFlags constants" for processing return
 *         value.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadStatusReg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadStatusReg(PeripheralBase) ( \
    FTFL_FSTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Flash status register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Flash status register. Use constants from
 *        group "ClearFlags constants". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteStatusReg(deviceID, FTFL_PDD_COMMAND_COMPLETE);
 *      @endcode
 */
#define FTFL_PDD_WriteStatusReg(PeripheralBase, Value) ( \
    FTFL_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ClearCommandCompleteFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Command complete flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      FTFL_PDD_ClearCommandCompleteFlag(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ClearCommandCompleteFlag(PeripheralBase) ( \
    FTFL_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFL_FSTAT_REG(PeripheralBase) | FTFL_FSTAT_CCIF_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_ACCERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_RDCOLERR_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearReadCollisionErrorFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Read collision error flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      FTFL_PDD_ClearReadCollisionErrorFlag(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ClearReadCollisionErrorFlag(PeripheralBase) ( \
    FTFL_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFL_FSTAT_REG(PeripheralBase) | FTFL_FSTAT_RDCOLERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_ACCERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_CCIF_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearAccessErrorFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Access error flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      FTFL_PDD_ClearAccessErrorFlag(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ClearAccessErrorFlag(PeripheralBase) ( \
    FTFL_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFL_FSTAT_REG(PeripheralBase) | FTFL_FSTAT_ACCERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_RDCOLERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_CCIF_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearProtectionViolationErrorFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Protection violation error flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      FTFL_PDD_ClearProtectionViolationErrorFlag(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ClearProtectionViolationErrorFlag(PeripheralBase) ( \
    FTFL_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFL_FSTAT_REG(PeripheralBase) | FTFL_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_ACCERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_RDCOLERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFL_FSTAT_CCIF_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- LaunchCommand
   ---------------------------------------------------------------------------- */

/**
 * @brief Starts new command.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      FTFL_PDD_LaunchCommand(deviceID);
 *      @endcode
 */
#define FTFL_PDD_LaunchCommand(PeripheralBase) ( \
    FTFL_FSTAT_REG(PeripheralBase) = \
     0x80U \
  )

/* ----------------------------------------------------------------------------
   -- ClearFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Flash status register.
 * @param PeripheralBase Peripheral base address.
 * @param Flags Interrupt mask. This parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      FTFL_PDD_ClearFlags(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_ClearFlags(PeripheralBase, Flags) ( \
    FTFL_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(Flags) \
  )

/* ----------------------------------------------------------------------------
   -- GetFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns value of the Flash status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_GetFlags(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetFlags(PeripheralBase) ( \
    FTFL_FSTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadConfigReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns value of the Flash configuration register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCNFG.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadConfigReg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadConfigReg(PeripheralBase) ( \
    FTFL_FCNFG_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteConfigReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes value to the Flash configuration register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the Flash configuration register. This
 *        parameter is a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCNFG.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteConfigReg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteConfigReg(PeripheralBase, Value) ( \
    FTFL_FCNFG_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- EnableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Enables Command commplete end Read collision error interrupts.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "EnableInterrupts,
 *        DisableInterrupts constants". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCNFG.
 * @par Example:
 *      @code
 *      FTFL_PDD_EnableInterrupts(deviceID, FTFL_PDD_COMMAND_COMPLETE_INT);
 *      @endcode
 */
#define FTFL_PDD_EnableInterrupts(PeripheralBase, Mask) ( \
    FTFL_FCNFG_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       FTFL_FCNFG_REG(PeripheralBase)) & ( \
       (uint8_t)(~(uint8_t)(FTFL_FCNFG_CCIE_MASK | FTFL_FCNFG_RDCOLLIE_MASK))))) | ( \
      (uint8_t)(Mask))) \
  )

/* ----------------------------------------------------------------------------
   -- DisableInterrupts
   ---------------------------------------------------------------------------- */

/**
 * @brief Disables Command commplete end Read collision error interrupts.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "EnableInterrupts,
 *        DisableInterrupts constants". This parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCNFG.
 * @par Example:
 *      @code
 *      FTFL_PDD_DisableInterrupts(deviceID, FTFL_PDD_COMMAND_COMPLETE_INT);
 *      @endcode
 */
#define FTFL_PDD_DisableInterrupts(PeripheralBase, Mask) ( \
    FTFL_FCNFG_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       FTFL_FCNFG_REG(PeripheralBase)) & ( \
       (uint8_t)(~(uint8_t)(FTFL_FCNFG_CCIE_MASK | FTFL_FCNFG_RDCOLLIE_MASK))))) | ( \
      (uint8_t)(( \
       (uint8_t)(~(uint8_t)(Mask))) & ( \
       (uint8_t)(FTFL_FCNFG_CCIE_MASK | FTFL_FCNFG_RDCOLLIE_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- SuspendErasing
   ---------------------------------------------------------------------------- */

/**
 * @brief Suspends the current Erase Flash Sector command execution.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCNFG.
 * @par Example:
 *      @code
 *      FTFL_PDD_SuspendErasing(deviceID);
 *      @endcode
 */
#define FTFL_PDD_SuspendErasing(PeripheralBase) ( \
    FTFL_FCNFG_REG(PeripheralBase) |= \
     FTFL_FCNFG_ERSSUSP_MASK \
  )

/* ----------------------------------------------------------------------------
   -- GetRAMReady
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Ram ready bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCNFG.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_GetRAMReady(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetRAMReady(PeripheralBase) ( \
    (uint8_t)(FTFL_FCNFG_REG(PeripheralBase) & FTFL_FCNFG_RAMRDY_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- GetEEEReady
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the EEE ready bit.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCNFG.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_GetEEEReady(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetEEEReady(PeripheralBase) ( \
    (uint8_t)(FTFL_FCNFG_REG(PeripheralBase) & FTFL_FCNFG_EEERDY_MASK) \
  )

/* ----------------------------------------------------------------------------
   -- ReadSecurityReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Security register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FSEC.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadSecurityReg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadSecurityReg(PeripheralBase) ( \
    FTFL_FSEC_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- GetBackdoorEnable
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the constant FTFL_PDD_BACKDOOR_KEY_ENABLED if backdoor key
 * access is enabled else returns the FTFL_PDD_BACKDOOR_KEY_DISABLED constant.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of "BackDoorKey constants" type. The value is cast to
 *         "uint16_t".
 * @remarks The macro accesses the following registers: FTFL_FSEC.
 * @par Example:
 *      @code
 *      uint16_t result = FTFL_PDD_GetBackdoorEnable(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetBackdoorEnable(PeripheralBase) ( \
    (( \
      (uint8_t)(( \
       (uint8_t)(FTFL_FSEC_REG(PeripheralBase) & FTFL_FSEC_KEYEN_MASK)) >> ( \
       FTFL_FSEC_KEYEN_SHIFT))) == ( \
      0x2U)) ? ( \
      FTFL_PDD_BACKDOOR_KEY_ENABLED) : ( \
      FTFL_PDD_BACKDOOR_KEY_DISABLED) \
  )

/* ----------------------------------------------------------------------------
   -- GetSecurityState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the constant FTFL_PDD_SECURED if MCU is in secure state else
 * returns the FTFL_PDD_UNSECURED constant.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of "Security state constants" type. The value is cast
 *         to "uint16_t".
 * @remarks The macro accesses the following registers: FTFL_FSEC.
 * @par Example:
 *      @code
 *      uint16_t result = FTFL_PDD_GetSecurityState(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetSecurityState(PeripheralBase) ( \
    ((uint8_t)(FTFL_FSEC_REG(PeripheralBase) & FTFL_FSEC_SEC_MASK) == 0x2U) ? ( \
      FTFL_PDD_UNSECURED) : ( \
      FTFL_PDD_SECURED) \
  )

/* ----------------------------------------------------------------------------
   -- ReadOptionReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Optional register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FOPT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadOptionReg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadOptionReg(PeripheralBase) ( \
    FTFL_FOPT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB0Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 0.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB0.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB0Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB0Reg(PeripheralBase) ( \
    FTFL_FCCOB0_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 1.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB1.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB1Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB1Reg(PeripheralBase) ( \
    FTFL_FCCOB1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB2Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 2.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB2.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB2Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB2Reg(PeripheralBase) ( \
    FTFL_FCCOB2_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB3Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 3.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB3.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB3Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB3Reg(PeripheralBase) ( \
    FTFL_FCCOB3_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB4Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 4.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB4.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB4Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB4Reg(PeripheralBase) ( \
    FTFL_FCCOB4_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB5Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 5.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB5.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB5Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB5Reg(PeripheralBase) ( \
    FTFL_FCCOB5_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB6Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 6.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB6.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB6Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB6Reg(PeripheralBase) ( \
    FTFL_FCCOB6_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB7Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 7.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB7.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB7Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB7Reg(PeripheralBase) ( \
    FTFL_FCCOB7_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB8Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 8.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB8.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB8Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB8Reg(PeripheralBase) ( \
    FTFL_FCCOB8_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB9Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 9.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOB9.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOB9Reg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOB9Reg(PeripheralBase) ( \
    FTFL_FCCOB9_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOBAReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register A.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOBA.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOBAReg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOBAReg(PeripheralBase) ( \
    FTFL_FCCOBA_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOBBReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register B.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FCCOBB.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_ReadFCCOBBReg(deviceID);
 *      @endcode
 */
#define FTFL_PDD_ReadFCCOBBReg(PeripheralBase) ( \
    FTFL_FCCOBB_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB0Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB0 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB0 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB0.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB0Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB0Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB0_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB1 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB1 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB1.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB1Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB1Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB1_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB2Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB2 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB2 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB2.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB2Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB2Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB2_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB3Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB3register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB3 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB3.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB3Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB3Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB3_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB4Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB4 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB4 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB4.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB4Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB4Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB4_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB5Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB5 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB5 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB5.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB5Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB5Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB5_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB6Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB6 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB6 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB6.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB6Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB6Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB6_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB7Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB7 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB7 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB7.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB7Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB7Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB7_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB8Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB8 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB8 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB8.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB8Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB8Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB8_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOB9Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB9 register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOB9 register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB9.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOB9Reg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOB9Reg(PeripheralBase, Value) ( \
    FTFL_FCCOB9_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBAReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOBA register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOBA register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOBA.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBAReg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBAReg(PeripheralBase, Value) ( \
    FTFL_FCCOBA_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBBReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOBB register.
 * @param PeripheralBase Peripheral base address.
 * @param Value Value written to the FCCOBB register. This parameter is a 8-bit
 *        value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOBB.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBBReg(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBBReg(PeripheralBase, Value) ( \
    FTFL_FCCOBB_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- SetFCCOBCommand
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB Command register.
 * @param PeripheralBase Peripheral base address.
 * @param Command Value written to the FCCOB Command register. This parameter is
 *        a 8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB0.
 * @par Example:
 *      @code
 *      FTFL_PDD_SetFCCOBCommand(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_SetFCCOBCommand(PeripheralBase, Command) ( \
    FTFL_FCCOB0_REG(PeripheralBase) = \
     (uint8_t)(Command) \
  )

/* ----------------------------------------------------------------------------
   -- SetFCCOBAddress
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB Address register.
 * @param PeripheralBase Peripheral base address.
 * @param Address Value written to the FCCOB Address register. This parameter is
 *        a 24-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB3,
 *          FTFL_FCCOB2, FTFL_FCCOB1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFL_PDD_SetFCCOBAddress(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_SetFCCOBAddress(PeripheralBase, Address) ( \
    (FTFL_FCCOB3_REG(PeripheralBase) = \
     (uint8_t)(Address)), \
    ((FTFL_FCCOB2_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Address) >> 8U)), \
    (FTFL_FCCOB1_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Address) >> 16U))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData0
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 0.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 0 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB4.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData0(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData0(PeripheralBase, Data) ( \
    FTFL_FCCOB4_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData1
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 1.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 1 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB5.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData1(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData1(PeripheralBase, Data) ( \
    FTFL_FCCOB5_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData2
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 2.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 2 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB6.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData2(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData2(PeripheralBase, Data) ( \
    FTFL_FCCOB6_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData3
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 3.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 3 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB7.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData3(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData3(PeripheralBase, Data) ( \
    FTFL_FCCOB7_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData4
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 4.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 4 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB8.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData4(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData4(PeripheralBase, Data) ( \
    FTFL_FCCOB8_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData5
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 5.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 5 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB9.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData5(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData5(PeripheralBase, Data) ( \
    FTFL_FCCOB9_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData6
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 6.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 6 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOBA.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData6(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData6(PeripheralBase, Data) ( \
    FTFL_FCCOBA_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBData7
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes Data byte 7.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the FCCOB Data 7 register. This parameter is a
 *        8-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOBB.
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBData7(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBData7(PeripheralBase, Data) ( \
    FTFL_FCCOBB_REG(PeripheralBase) = \
     (uint8_t)(Data) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBLongWordData
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets longword data to be programmed.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the Flash in the CPU native endian format. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB4,
 *          FTFL_FCCOB5, FTFL_FCCOB6, FTFL_FCCOB7 (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBLongWordData(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBLongWordData(PeripheralBase, Data) ( \
    (FTFL_FCCOB4_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 24U)), \
    ((FTFL_FCCOB5_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 16U)), \
    ((FTFL_FCCOB6_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 8U)), \
    (FTFL_FCCOB7_REG(PeripheralBase) = \
     (uint8_t)(Data)))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBFirstLongWordData
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets first longword data to be programmed.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the Flash in the CPU native endian format. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB4,
 *          FTFL_FCCOB5, FTFL_FCCOB6, FTFL_FCCOB7 (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBFirstLongWordData(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBFirstLongWordData(PeripheralBase, Data) ( \
    (FTFL_FCCOB4_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 24U)), \
    ((FTFL_FCCOB5_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 16U)), \
    ((FTFL_FCCOB6_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 8U)), \
    (FTFL_FCCOB7_REG(PeripheralBase) = \
     (uint8_t)(Data)))) \
  )

/* ----------------------------------------------------------------------------
   -- WriteFCCOBSecondLongWordData
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets second longword data to be programmed.
 * @param PeripheralBase Peripheral base address.
 * @param Data Value written to the Flash in the CPU native endian format. This
 *        parameter is a 32-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FCCOB8,
 *          FTFL_FCCOB9, FTFL_FCCOBA, FTFL_FCCOBB (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFL_PDD_WriteFCCOBSecondLongWordData(deviceID, 1);
 *      @endcode
 */
#define FTFL_PDD_WriteFCCOBSecondLongWordData(PeripheralBase, Data) ( \
    (FTFL_FCCOB8_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 24U)), \
    ((FTFL_FCCOB9_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 16U)), \
    ((FTFL_FCCOBA_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 8U)), \
    (FTFL_FCCOBB_REG(PeripheralBase) = \
     (uint8_t)(Data)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetPFlashProtectionState
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets program Flash protection state.
 * @param PeripheralBase Peripheral base address.
 * @param Regions Protected regions. This parameter is a 32-bit value.
 * @param State Requested state. This parameter is of "FlashProtection
 *        constants" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FPROT3.
 * @par Example:
 *      @code
 *      FTFL_PDD_SetPFlashProtectionState(deviceID, 1, FTFL_PDD_UNPROTECTED);
 *      @endcode
 */
#define FTFL_PDD_SetPFlashProtectionState(PeripheralBase, Regions, State) ( \
    ((State) == FTFL_PDD_UNPROTECTED) ? ( \
      *(uint32_t *)(void *)&(FTFL_FPROT3_REG(PeripheralBase)) |= \
      (uint32_t)(Regions)) : ( \
      *(uint32_t *)(void *)&(FTFL_FPROT3_REG(PeripheralBase)) &= \
      (uint32_t)(~(uint32_t)(Regions))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPFlashProtectionState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns program falsh protection state.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTFL_FPROT3.
 * @par Example:
 *      @code
 *      uint32_t result = FTFL_PDD_GetPFlashProtectionState(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetPFlashProtectionState(PeripheralBase) ( \
    (uint32_t)(~(*(uint32_t *)(void *)&(FTFL_FPROT3_REG(PeripheralBase)))) \
  )

/* ----------------------------------------------------------------------------
   -- SetDFlashProtectionState
   ---------------------------------------------------------------------------- */

/**
 * @brief Sets data Flash protection state.
 * @param PeripheralBase Peripheral base address.
 * @param Regions FTFL_PDD_PROTECTED or FTFL_PDD_UNPROTECTED. This parameter is
 *        a 8-bit value.
 * @param State Requested state. This parameter is of "FlashProtection
 *        constants" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FDPROT.
 * @par Example:
 *      @code
 *      FTFL_PDD_SetDFlashProtectionState(deviceID, 1, FTFL_PDD_UNPROTECTED);
 *      @endcode
 */
#define FTFL_PDD_SetDFlashProtectionState(PeripheralBase, Regions, State) ( \
    ((State) == FTFL_PDD_UNPROTECTED) ? ( \
      FTFL_FDPROT_REG(PeripheralBase) |= \
       (uint8_t)(Regions)) : ( \
      FTFL_FDPROT_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)(Regions))) \
  )

/* ----------------------------------------------------------------------------
   -- GetDFlashProtectionState
   ---------------------------------------------------------------------------- */

/**
 * @brief Each bit of the returned value represent 1/8 of the Data FLASH memory.
 * If the bit is set the region is protected else the region is unprotected.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FDPROT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_GetDFlashProtectionState(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetDFlashProtectionState(PeripheralBase) ( \
    (uint8_t)(~(uint8_t)FTFL_FDPROT_REG(PeripheralBase)) \
  )

/* ----------------------------------------------------------------------------
   -- SetEERAMProtectionState
   ---------------------------------------------------------------------------- */

/**
 * @brief Each bit of the Region parameter represent 1/8 of the EERPROM memory.
 * To change the protection state of the region(s) select requested regions
 * (Region param) and set new state (State param).
 * @param PeripheralBase Peripheral base address.
 * @param Regions Protected regions. This parameter is a 8-bit value.
 * @param State Requested state. This parameter is of "FlashProtection
 *        constants" type.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFL_FEPROT.
 * @par Example:
 *      @code
 *      FTFL_PDD_SetEERAMProtectionState(deviceID, 1, FTFL_PDD_UNPROTECTED);
 *      @endcode
 */
#define FTFL_PDD_SetEERAMProtectionState(PeripheralBase, Regions, State) ( \
    ((State) == FTFL_PDD_UNPROTECTED) ? ( \
      FTFL_FEPROT_REG(PeripheralBase) |= \
       (uint8_t)(Regions)) : ( \
      FTFL_FEPROT_REG(PeripheralBase) &= \
       (uint8_t)(~(uint8_t)(Regions))) \
  )

/* ----------------------------------------------------------------------------
   -- GetEERAMProtectionState
   ---------------------------------------------------------------------------- */

/**
 * @brief Each bit of the returned value represent 1/8 of the EERPROM memory. If
 * the bit is set the region is protected else the region is unprotected.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFL_FEPROT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFL_PDD_GetEERAMProtectionState(deviceID);
 *      @endcode
 */
#define FTFL_PDD_GetEERAMProtectionState(PeripheralBase) ( \
    (uint8_t)(~(uint8_t)FTFL_FEPROT_REG(PeripheralBase)) \
  )
#endif  /* #if defined(FTFL_PDD_H_) */

/* FTFL_PDD.h, eof. */
