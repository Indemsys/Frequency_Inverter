/*
  PDD layer implementation for peripheral type FTFA
  (C) 2010 Freescale, Inc. All rights reserved.

  This file is static and it is generated from API-Factory
*/

#if !defined(FTFA_PDD_H_)
#define FTFA_PDD_H_

/* ----------------------------------------------------------------------------
   -- Test if supported MCU is active
   ---------------------------------------------------------------------------- */

#if !defined(MCU_ACTIVE)
  // No MCU is active
  #error FTFA PDD library: No derivative is active. Place proper #include with PDD memory map before including PDD library.
#elif \
      !defined(MCU_MKL02Z4) /* FTFA */ && \
      !defined(MCU_MKL04Z4) /* FTFA */ && \
      !defined(MCU_MKL05Z4) /* FTFA */ && \
      !defined(MCU_MKL14Z4) /* FTFA */ && \
      !defined(MCU_MKL15Z4) /* FTFA */ && \
      !defined(MCU_MKL16Z4) /* FTFA */ && \
      !defined(MCU_MKL24Z4) /* FTFA */ && \
      !defined(MCU_MKL25Z4) /* FTFA */ && \
      !defined(MCU_MKL26Z4) /* FTFA */ && \
      !defined(MCU_MKL34Z4) /* FTFA */ && \
      !defined(MCU_MKL36Z4) /* FTFA */ && \
      !defined(MCU_MKL46Z4) /* FTFA */
  // Unsupported MCU is active
  #error FTFA PDD library: Unsupported derivative is active.
#endif

#include "PDD_Types.h"

/* ----------------------------------------------------------------------------
   -- Method symbol definitions
   ---------------------------------------------------------------------------- */

/* ClearFlags constants */
#define FTFA_PDD_COMMAND_COMPLETE          FTFA_FSTAT_CCIF_MASK /**< Command complete flag mask */
#define FTFA_PDD_READ_COLLISION_ERROR      FTFA_FSTAT_RDCOLERR_MASK /**< Read collision error flag mask */
#define FTFA_PDD_ACCESS_ERROR              FTFA_FSTAT_ACCERR_MASK /**< Access error flag mask */
#define FTFA_PDD_PROTECTION_VIOLATION      FTFA_FSTAT_FPVIOL_MASK /**< Protection violation flag mask */
#define FTFA_PDD_COMMAND_COMPLETION_STATUS FTFA_FSTAT_MGSTAT0_MASK /**< Command completion ststus flag mask */

/* EnableInterrupts, DisableInterrupts constants */
#define FTFA_PDD_COMMAND_COMPLETE_INT     FTFA_FCNFG_CCIE_MASK /**< Command complete interrupt mask */
#define FTFA_PDD_READ_COLLISION_ERROR_INT FTFA_FCNFG_RDCOLLIE_MASK /**< Read collision error interrupt mask */

/* SetFCCOBCommand constants */
#define FTFA_PDD_READ_1S_SECTION            0x1U /**< Read 1s Section command value */
#define FTFA_PDD_PROGRAM_CHECK              0x2U /**< Program Check command value */
#define FTFA_PDD_READ_RESOURCE              0x3U /**< Read Resource command value */
#define FTFA_PDD_PROGRAM_LONGWORD           0x6U /**< Program Longword command value */
#define FTFA_PDD_ERASE_FLASH_SECTOR         0x9U /**< Erase Flash Sector command value */
#define FTFA_PDD_READ_1S_ALL_BLOCKS         0x40U /**< Read 1s All Blocks command value */
#define FTFA_PDD_PDD_READ_ONCE              0x41U /**< Read Once command value */
#define FTFA_PDD_PROGRAM_ONCE               0x43U /**< Program Once command value */
#define FTFA_PDD_ERASE_ALL_BLOCKS           0x44U /**< Erase All Blocks command value */
#define FTFA_PDD_VERIFY_BACKDOOR_ACCESS_KEY 0x45U /**< Verify Backdoor Access Key command value */

/* BackDoorKey constants */
#define FTFA_PDD_BACKDOOR_KEY_ENABLED  0U        /**< Backdoor key enable constant */
#define FTFA_PDD_BACKDOOR_KEY_DISABLED 0x1U      /**< Backdoor key disable constant */

/* Security state constants */
#define FTFA_PDD_UNSECURED 0U                    /**< Unsecure constant */
#define FTFA_PDD_SECURED   0x1U                  /**< Secure constant */

/* FlashProtection constants */
#define FTFA_PDD_UNPROTECTED 0U                  /**< Unprotect constant */
#define FTFA_PDD_PROTECTED   0x1U                /**< Protect constant */


/* ----------------------------------------------------------------------------
   -- ReadStatusReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns value of the Flash status register.
 * @param PeripheralBase Peripheral base address.
 * @return Use constants from group "ClearFlags constants" for processing return
 *         value.
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadStatusReg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadStatusReg(PeripheralBase) ( \
    FTFA_FSTAT_REG(PeripheralBase) \
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
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteStatusReg(deviceID, FTFA_PDD_COMMAND_COMPLETE);
 *      @endcode
 */
#define FTFA_PDD_WriteStatusReg(PeripheralBase, Value) ( \
    FTFA_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(Value) \
  )

/* ----------------------------------------------------------------------------
   -- ClearCommandCompleteFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Command complete flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      FTFA_PDD_ClearCommandCompleteFlag(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ClearCommandCompleteFlag(PeripheralBase) ( \
    FTFA_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFA_FSTAT_REG(PeripheralBase) | FTFA_FSTAT_CCIF_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_ACCERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_RDCOLERR_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearReadCollisionErrorFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Read collision error flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      FTFA_PDD_ClearReadCollisionErrorFlag(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ClearReadCollisionErrorFlag(PeripheralBase) ( \
    FTFA_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFA_FSTAT_REG(PeripheralBase) | FTFA_FSTAT_RDCOLERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_ACCERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_CCIF_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearAccessErrorFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Access error flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      FTFA_PDD_ClearAccessErrorFlag(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ClearAccessErrorFlag(PeripheralBase) ( \
    FTFA_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFA_FSTAT_REG(PeripheralBase) | FTFA_FSTAT_ACCERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_RDCOLERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_CCIF_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- ClearProtectionViolationErrorFlag
   ---------------------------------------------------------------------------- */

/**
 * @brief Clears Protection violation error flag.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      FTFA_PDD_ClearProtectionViolationErrorFlag(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ClearProtectionViolationErrorFlag(PeripheralBase) ( \
    FTFA_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(FTFA_FSTAT_REG(PeripheralBase) | FTFA_FSTAT_FPVIOL_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_ACCERR_MASK)) & (( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_RDCOLERR_MASK)) & ( \
      (uint8_t)(~(uint8_t)FTFA_FSTAT_CCIF_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- LaunchCommand
   ---------------------------------------------------------------------------- */

/**
 * @brief Starts new command.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      FTFA_PDD_LaunchCommand(deviceID);
 *      @endcode
 */
#define FTFA_PDD_LaunchCommand(PeripheralBase) ( \
    FTFA_FSTAT_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      FTFA_PDD_ClearFlags(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_ClearFlags(PeripheralBase, Flags) ( \
    FTFA_FSTAT_REG(PeripheralBase) = \
     (uint8_t)(Flags) \
  )

/* ----------------------------------------------------------------------------
   -- GetFlags
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns value of the Flash status register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FSTAT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_GetFlags(deviceID);
 *      @endcode
 */
#define FTFA_PDD_GetFlags(PeripheralBase) ( \
    FTFA_FSTAT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadConfigReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns value of the Flash configuration register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCNFG.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadConfigReg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadConfigReg(PeripheralBase) ( \
    FTFA_FCNFG_REG(PeripheralBase) \
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
 * @remarks The macro accesses the following registers: FTFA_FCNFG.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteConfigReg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteConfigReg(PeripheralBase, Value) ( \
    FTFA_FCNFG_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCNFG.
 * @par Example:
 *      @code
 *      FTFA_PDD_EnableInterrupts(deviceID, FTFA_PDD_COMMAND_COMPLETE_INT);
 *      @endcode
 */
#define FTFA_PDD_EnableInterrupts(PeripheralBase, Mask) ( \
    FTFA_FCNFG_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       FTFA_FCNFG_REG(PeripheralBase)) & ( \
       (uint8_t)(~(uint8_t)(FTFA_FCNFG_CCIE_MASK | FTFA_FCNFG_RDCOLLIE_MASK))))) | ( \
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
 * @remarks The macro accesses the following registers: FTFA_FCNFG.
 * @par Example:
 *      @code
 *      FTFA_PDD_DisableInterrupts(deviceID, FTFA_PDD_COMMAND_COMPLETE_INT);
 *      @endcode
 */
#define FTFA_PDD_DisableInterrupts(PeripheralBase, Mask) ( \
    FTFA_FCNFG_REG(PeripheralBase) = \
     (uint8_t)(( \
      (uint8_t)(( \
       FTFA_FCNFG_REG(PeripheralBase)) & ( \
       (uint8_t)(~(uint8_t)(FTFA_FCNFG_CCIE_MASK | FTFA_FCNFG_RDCOLLIE_MASK))))) | ( \
      (uint8_t)(( \
       (uint8_t)(~(uint8_t)(Mask))) & ( \
       (uint8_t)(FTFA_FCNFG_CCIE_MASK | FTFA_FCNFG_RDCOLLIE_MASK))))) \
  )

/* ----------------------------------------------------------------------------
   -- SuspendErasing
   ---------------------------------------------------------------------------- */

/**
 * @brief Suspends the current Erase Flash Sector command execution.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFA_FCNFG.
 * @par Example:
 *      @code
 *      FTFA_PDD_SuspendErasing(deviceID);
 *      @endcode
 */
#define FTFA_PDD_SuspendErasing(PeripheralBase) ( \
    FTFA_FCNFG_REG(PeripheralBase) |= \
     FTFA_FCNFG_ERSSUSP_MASK \
  )

/* ----------------------------------------------------------------------------
   -- ReadSecurityReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Security register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FSEC.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadSecurityReg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadSecurityReg(PeripheralBase) ( \
    FTFA_FSEC_REG(PeripheralBase) \
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
 * @remarks The macro accesses the following registers: FTFA_FSEC.
 * @par Example:
 *      @code
 *      uint16_t result = FTFA_PDD_GetBackdoorEnable(deviceID);
 *      @endcode
 */
#define FTFA_PDD_GetBackdoorEnable(PeripheralBase) ( \
    (( \
      (uint8_t)(( \
       (uint8_t)(FTFA_FSEC_REG(PeripheralBase) & FTFA_FSEC_KEYEN_MASK)) >> ( \
       FTFA_FSEC_KEYEN_SHIFT))) == ( \
      0x2U)) ? ( \
      FTFA_PDD_BACKDOOR_KEY_ENABLED) : ( \
      FTFA_PDD_BACKDOOR_KEY_DISABLED) \
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
 * @remarks The macro accesses the following registers: FTFA_FSEC.
 * @par Example:
 *      @code
 *      uint16_t result = FTFA_PDD_GetSecurityState(deviceID);
 *      @endcode
 */
#define FTFA_PDD_GetSecurityState(PeripheralBase) ( \
    ((uint8_t)(FTFA_FSEC_REG(PeripheralBase) & FTFA_FSEC_SEC_MASK) == 0x2U) ? ( \
      FTFA_PDD_UNSECURED) : ( \
      FTFA_PDD_SECURED) \
  )

/* ----------------------------------------------------------------------------
   -- ReadOptionReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Optional register.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FOPT.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadOptionReg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadOptionReg(PeripheralBase) ( \
    FTFA_FOPT_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB0Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 0.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB0.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB0Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB0Reg(PeripheralBase) ( \
    FTFA_FCCOB0_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB1Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 1.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB1.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB1Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB1Reg(PeripheralBase) ( \
    FTFA_FCCOB1_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB2Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 2.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB2.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB2Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB2Reg(PeripheralBase) ( \
    FTFA_FCCOB2_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB3Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 3.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB3.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB3Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB3Reg(PeripheralBase) ( \
    FTFA_FCCOB3_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB4Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 4.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB4.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB4Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB4Reg(PeripheralBase) ( \
    FTFA_FCCOB4_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB5Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 5.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB5.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB5Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB5Reg(PeripheralBase) ( \
    FTFA_FCCOB5_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB6Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 6.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB6.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB6Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB6Reg(PeripheralBase) ( \
    FTFA_FCCOB6_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB7Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 7.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB7.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB7Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB7Reg(PeripheralBase) ( \
    FTFA_FCCOB7_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB8Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 8.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB8.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB8Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB8Reg(PeripheralBase) ( \
    FTFA_FCCOB8_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOB9Reg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register 9.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOB9.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOB9Reg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOB9Reg(PeripheralBase) ( \
    FTFA_FCCOB9_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOBAReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register A.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOBA.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOBAReg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOBAReg(PeripheralBase) ( \
    FTFA_FCCOBA_REG(PeripheralBase) \
  )

/* ----------------------------------------------------------------------------
   -- ReadFCCOBBReg
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns the value of the Common command object register B.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 8-bit value.
 * @remarks The macro accesses the following registers: FTFA_FCCOBB.
 * @par Example:
 *      @code
 *      uint8_t result = FTFA_PDD_ReadFCCOBBReg(deviceID);
 *      @endcode
 */
#define FTFA_PDD_ReadFCCOBBReg(PeripheralBase) ( \
    FTFA_FCCOBB_REG(PeripheralBase) \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB0.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB0Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB0Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB0_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB1.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB1Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB1Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB1_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB2.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB2Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB2Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB2_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB3.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB3Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB3Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB3_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB4.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB4Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB4Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB4_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB5.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB5Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB5Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB5_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB6.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB6Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB6Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB6_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB7.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB7Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB7Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB7_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB8.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB8Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB8Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB8_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB9.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOB9Reg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOB9Reg(PeripheralBase, Value) ( \
    FTFA_FCCOB9_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOBA.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBAReg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBAReg(PeripheralBase, Value) ( \
    FTFA_FCCOBA_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOBB.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBBReg(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBBReg(PeripheralBase, Value) ( \
    FTFA_FCCOBB_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB0.
 * @par Example:
 *      @code
 *      FTFA_PDD_SetFCCOBCommand(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_SetFCCOBCommand(PeripheralBase, Command) ( \
    FTFA_FCCOB0_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB3,
 *          FTFA_FCCOB2, FTFA_FCCOB1 (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFA_PDD_SetFCCOBAddress(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_SetFCCOBAddress(PeripheralBase, Address) ( \
    (FTFA_FCCOB3_REG(PeripheralBase) = \
     (uint8_t)(Address)), \
    ((FTFA_FCCOB2_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Address) >> 8U)), \
    (FTFA_FCCOB1_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Address) >> 16U))) \
  )

/* ----------------------------------------------------------------------------
   -- SetFCCOBDataSize
   ---------------------------------------------------------------------------- */

/**
 * @brief Writes the value to the FCCOB size register.
 * @param PeripheralBase Peripheral base address.
 * @param Address Value written to the FCCOB Size register. This parameter is a
 *        16-bit value.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: FTFA_FCCOB5, FTFA_FCCOB4
 *          (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFA_PDD_SetFCCOBDataSize(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_SetFCCOBDataSize(PeripheralBase, Address) ( \
    (FTFA_FCCOB5_REG(PeripheralBase) = \
     (uint8_t)(Address)), \
    (FTFA_FCCOB4_REG(PeripheralBase) = \
     (uint8_t)((uint16_t)(Address) >> 8U)) \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB4.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData0(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData0(PeripheralBase, Data) ( \
    FTFA_FCCOB4_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB5.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData1(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData1(PeripheralBase, Data) ( \
    FTFA_FCCOB5_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB6.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData2(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData2(PeripheralBase, Data) ( \
    FTFA_FCCOB6_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB7.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData3(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData3(PeripheralBase, Data) ( \
    FTFA_FCCOB7_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB8.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData4(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData4(PeripheralBase, Data) ( \
    FTFA_FCCOB8_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB9.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData5(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData5(PeripheralBase, Data) ( \
    FTFA_FCCOB9_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOBA.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData6(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData6(PeripheralBase, Data) ( \
    FTFA_FCCOBA_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOBB.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBData7(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBData7(PeripheralBase, Data) ( \
    FTFA_FCCOBB_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB7.
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBLongWordData(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBLongWordData(PeripheralBase, Data) ( \
    *(uint32_t*)(void*)&FTFA_FCCOB7_REG(PeripheralBase) = Data \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB4,
 *          FTFA_FCCOB5, FTFA_FCCOB6, FTFA_FCCOB7 (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBFirstLongWordData(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBFirstLongWordData(PeripheralBase, Data) ( \
    (FTFA_FCCOB4_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 24U)), \
    ((FTFA_FCCOB5_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 16U)), \
    ((FTFA_FCCOB6_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 8U)), \
    (FTFA_FCCOB7_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FCCOB8,
 *          FTFA_FCCOB9, FTFA_FCCOBA, FTFA_FCCOBB (depending on the peripheral).
 * @par Example:
 *      @code
 *      FTFA_PDD_WriteFCCOBSecondLongWordData(deviceID, 1);
 *      @endcode
 */
#define FTFA_PDD_WriteFCCOBSecondLongWordData(PeripheralBase, Data) ( \
    (FTFA_FCCOB8_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 24U)), \
    ((FTFA_FCCOB9_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 16U)), \
    ((FTFA_FCCOBA_REG(PeripheralBase) = \
     (uint8_t)((uint32_t)(Data) >> 8U)), \
    (FTFA_FCCOBB_REG(PeripheralBase) = \
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
 * @remarks The macro accesses the following registers: FTFA_FPROT3.
 * @par Example:
 *      @code
 *      FTFA_PDD_SetPFlashProtectionState(deviceID, 1, FTFA_PDD_UNPROTECTED);
 *      @endcode
 */
#define FTFA_PDD_SetPFlashProtectionState(PeripheralBase, Regions, State) ( \
    ((State) == FTFA_PDD_UNPROTECTED) ? ( \
      *(uint32_t *)(void *)&(FTFA_FPROT3_REG(PeripheralBase)) |= \
      (uint32_t)(Regions)) : ( \
      *(uint32_t *)(void *)&(FTFA_FPROT3_REG(PeripheralBase)) &= \
      (uint32_t)(~(uint32_t)(Regions))) \
  )

/* ----------------------------------------------------------------------------
   -- GetPFlashProtectionState
   ---------------------------------------------------------------------------- */

/**
 * @brief Returns program falsh protection state.
 * @param PeripheralBase Peripheral base address.
 * @return Returns a 32-bit value.
 * @remarks The macro accesses the following registers: FTFA_FPROT3.
 * @par Example:
 *      @code
 *      uint32_t result = FTFA_PDD_GetPFlashProtectionState(deviceID);
 *      @endcode
 */
#define FTFA_PDD_GetPFlashProtectionState(PeripheralBase) ( \
    (uint32_t)(~(*(uint32_t *)(void *)&(FTFA_FPROT3_REG(PeripheralBase)))) \
  )
#endif  /* #if defined(FTFA_PDD_H_) */

/* FTFA_PDD.h, eof. */
