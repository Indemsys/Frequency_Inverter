/* TSI PDD methods implementation file
 This file is static*/

#ifndef _TSI_PDD_H_
#define _TSI_PDD_H

/* --------------------------------------------------------------- */
/* --------- Test of support of active MCU */
/* --------------------------------------------------------------- */

/* Check if any supported MCU is active */
#if !defined(MCU_ACTIVE)
  /* No MCU is active */
  #error TSI PDD library: No derivative is active. Place proper #include "*_MCU.h" before including PDD library.
#endif

#include "PDD_Types.h"

/* ---------------------------------------------------------------
 --------- Type definitions
 --------------------------------------------------------------*/
/* PDD macro definitions*/

/* ---------------------------------------------------------------
// --------- EnableDevice
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_EnableDevice(Base) ( \
    setReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_TSIEN_MASK) \
  )

/* ---------------------------------------------------------------
// --------- DisableDevice
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_DisableDevice(Base) ( \
    clrReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_TSIEN_MASK) \
  )

/* ---------------------------------------------------------------
// --------- IsDeviceEnabled
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_IsDeviceEnabled(Base) ( \
    testReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_TSIEN_MASK) \
  )

/* ---------------------------------------------------------------
// --------- SetMask
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_SetMask(Base, mask) ( \
    clrSetReg32Bits(TSI_GENCS_REG(Base), TSI_AVAILABLE_INTERRUPT_MASK, mask) \
  )

/* ---------------------------------------------------------------
// --------- GetMask
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_GetMask(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) & TSI_AVAILABLE_INTERRUPT_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_ScanMode
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_ScanMode(Base, mode) ( \
  clrSetReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_STM_MASK, mode) \
  )

/* ---------------------------------------------------------------
// --------- Get_ScanMode
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_ScanMode(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) & TSI_GENCS_STM_MASK \
  )

/* ---------------------------------------------------------------
// --------- Stop_Enable
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Stop_Enable(Base) ( \
    setReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_STPE_MASK) \
  )

/* ---------------------------------------------------------------
// --------- Stop_Disable
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Stop_Disable(Base) ( \
    clrReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_STPE_MASK) \
  )

/* ---------------------------------------------------------------
// --------- GetInterruptFlags
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_GetInterruptFlags(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) & TSI_AVAILABLE_FLAGS_MASK \
  )

/* ---------------------------------------------------------------
// --------- ClearInterruptFlags
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_ClearInterruptFlags(Base, mask) ( \
    setReg32Bits(TSI_GENCS_REG(Base), mask) \
  )

/* ---------------------------------------------------------------
// --------- IsScanInProgress
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_IsScanInProgress(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) &  TSI_GENCS_SCNIP_MASK \
  )

/* ---------------------------------------------------------------
// --------- SoftwareTrigger
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_SoftwareTrigger(Base) ( \
    setReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_SWTS_MASK) \
  )

/* ---------------------------------------------------------------
// --------- Set_NumScansPerElectrode
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_NumScansPerElectrode(Base, scans) ( \
    clrSetReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_NSCN_MASK, scans) \
  )

/* ---------------------------------------------------------------
// --------- Get_NumScansPerElectrode
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_NumScansPerElectrode(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) & TSI_GENCS_NSCN_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_ElectrodePrescaler
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_ElectrodePrescaler(Base, divider) ( \
    clrSetReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_PS_MASK, divider) \
  )

/* ---------------------------------------------------------------
// --------- Get_ElectrodePrescaler
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_ElectrodePrescaler(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) & TSI_GENCS_PS_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_LPM_Clock
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_LPM_Clock(Base, clock) ( \
    clrSetReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_LPCLKS_MASK, clock) \
  )

/* ---------------------------------------------------------------
// --------- Get_LPM_Clock
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_LPM_Clock(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) & TSI_GENCS_LPCLKS_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_LPM_ScanInterval
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_LPM_ScanInterval(Base, interval) ( \
    clrSetReg32Bits(TSI_GENCS_REG(Base), TSI_GENCS_LPSCNITV_MASK, interval) \
  )

/* ---------------------------------------------------------------
// --------- Get_LPM_ScanInterval
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_LPM_ScanInterval(Base) ( \
    getReg32(TSI_GENCS_REG(Base)) & TSI_GENCS_LPSCNITV_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_LPM_ScanPinSelection
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_LPM_ScanPinSelection(Base, pin) ( \
    clrSetReg32Bits(TSI_PEN_REG(Base), TSI_PEN_LPSP_MASK, pin) \
  )

/* ---------------------------------------------------------------
// --------- Get_LPM_ScanPinSelection
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_LPM_ScanPinSelection(Base) ( \
    getReg32(TSI_PEN_REG(Base)) & TSI_PEN_LPSP_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_ClockDivider
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_ClockDivider(Base, div) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_AMCLKDIV_MASK, div) \
  )

/* ---------------------------------------------------------------
// --------- Get_ClockDivider
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_ClockDivider(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_AMCLKDIV_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_ClockSource
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_ClockSource(Base, source) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_AMCLKS_MASK, source) \
  )

/* ---------------------------------------------------------------
// --------- Get_ClockSource
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_ClockSource(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_AMCLKS_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_Prescaler
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_Prescaler(Base, prescaler) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_AMPSC_MASK, prescaler) \
  )

/* ---------------------------------------------------------------
// --------- Get_Prescaler
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Prescaler(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_AMPSC_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_ScanModule
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_ScanModule(Base, scan) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_SMOD_MASK, scan) \
  )

/* ---------------------------------------------------------------
// --------- Get_ScanModule
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_ScanModule(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_SMOD_MASK \
  )

/* ---------------------------------------------------------------
// --------- Set_ExtOscChargeCurrent
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_ExtOscChargeCurrent(Base, current) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_EXTCHRG_MASK, current) \
  )

/* ---------------------------------------------------------------
// --------- Get_ExtOscChargeCurrent
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_ExtOscChargeCurrent(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_EXTCHRG_MASK  \
  )

/* ---------------------------------------------------------------
// --------- Set_DeltaVoltage
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_DeltaVoltage(Base, delta) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_DELVOL_MASK, delta) \
  )

/* ---------------------------------------------------------------
// --------- Get_DeltaVoltage
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_DeltaVoltage(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_DELVOL_MASK  \
  )

/* ---------------------------------------------------------------
// --------- Set_RefOscChargeCurrent
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_RefOscChargeCurrent(Base, current) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_REFCHRG_MASK, current) \
  )

/* ---------------------------------------------------------------
// --------- Get_RefOscChargeCurrent
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_RefOscChargeCurrent(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_REFCHRG_MASK  \
  )

/* ---------------------------------------------------------------
// --------- Set_InternalCapacitance
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_InternalCapacitance(Base, farads) ( \
    clrSetReg32Bits(TSI_SCANC_REG(Base), TSI_SCANC_CAPTRM_MASK, farads) \
  )

/* ---------------------------------------------------------------
// --------- Get_InternalCapacitance
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_InternalCapacitance(Base) ( \
    getReg32(TSI_SCANC_REG(Base)) & TSI_SCANC_CAPTRM_MASK  \
  )

/* ---------------------------------------------------------------
// --------- Set_Pins
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_Pins(Base, pins_mask) ( \
    clrSetReg32Bits(TSI_PEN_REG(Base), TSI_PEN_PENX, pins_mask) \
  )

/* ---------------------------------------------------------------
// --------- Get_Pins
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Pins(Base) ( \
    getReg32(TSI_PEN_REG(Base)) & TSI_PEN_PENX  \
  )

/* ---------------------------------------------------------------
// --------- Clear_StatusFlags
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Clear_StatusFlags(Base, clear_mask) ( \
    setReg32Bits(TSI_STATUS_REG(Base), clear_mask) \
  )

/* ---------------------------------------------------------------
// --------- Get_StatusFlags
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_StatusFlags(Base) ( \
    getReg32(TSI_STATUS_REG(Base)) \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter1Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter1Value(Base) ( \
    getReg32(TSI_CNTR1_REG(Base))  \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter3Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter3Value(Base) ( \
    getReg32(TSI_CNTR3_REG(Base))  \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter5Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter5Value(Base) ( \
    getReg32(TSI_CNTR5_REG(Base))  \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter7Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter7Value(Base) ( \
    getReg32(TSI_CNTR7_REG(Base))  \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter9Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter9Value(Base) ( \
    getReg32(TSI_CNTR9_REG(Base))  \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter11Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter11Value(Base) ( \
    getReg32(TSI_CNTR11_REG(Base))  \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter13Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter13Value(Base) ( \
    getReg32(TSI_CNTR13_REG(Base))  \
  )

/* ---------------------------------------------------------------
// --------- Get_Counter15Value
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Counter15Value(Base) ( \
    getReg32(TSI_CNTR15_REG(Base))  \
  )


/* ---------------------------------------------------------------
// --------- Set_Threshold
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Set_Threshold(Base, threshold, index) ( \
    setReg32(TSI_THRESHLD_REG(Base, index), threshold) \
  )

/* ---------------------------------------------------------------
// --------- Get_Threshold
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_Get_Threshold(Base, index) ( \
    getReg32(TSI_THRESHLD_REG(Base, index)) \
  )

/* ---------------------------------------------------------------
// --------- TSI_PDD_ClockGatingEnable
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_ClockGatingEnable(Base) ( \
    setReg32Bits(SIM_SCGC5_REG(Base), SIM_SCGC5_TSI_MASK) \
  )

/* ---------------------------------------------------------------
// --------- TSI_PDD_ClockGatingDisable
// ---------------------------------------------------------------
// Inputs are not checked (invalid value will cause undefined behavior).*/
#define TSI_PDD_ClockGatingDisable(Base) ( \
    clrReg32Bits(SIM_SCGC5_REG(Base), SIM_SCGC5_TSI_MASK) \
  )


#endif /* #if !defined(_TSI_PDD_H_) */

/* TSI_PDD.h, eof  */