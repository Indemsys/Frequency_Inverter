#ifndef __Cpu_H
#define __Cpu_H


#ifdef __cplusplus
extern "C" {
#endif 



#define CPU_CLOCK_CONFIG_NUMBER         0x03U /* Specifies number of defined clock configurations. */

/* CPU frequencies in clock configuration 0 */
#define CPU_CLOCK_CONFIG_0              0x00U /* Clock configuration 0 identifier */

/* CPU frequencies in clock configuration 1 */
#define CPU_CLOCK_CONFIG_1              0x01U /* Clock configuration 1 identifier */

/* CPU frequencies in clock configuration 2 */
#define CPU_CLOCK_CONFIG_2              0x02U /* Clock configuration 2 identifier */



#define LLWU_FILTER1                   0x01000000U /* WakeUp caused by digital filter 1 */
#define LLWU_FILTER2                   0x02000000U /* WakeUp caused by digital filter 2 */

/*lint -esym(765,SR_reg) Disable MISRA rule (8.10) checking for symbols (SR_reg). The SR_reg is used in inline assembler. */
extern volatile uint8_t SR_reg;        /* Current FAULTMASK register */
/*lint -esym(765,SR_lock) Disable MISRA rule (8.10) checking for symbols (SR_lock). The SR_reg is used in inline assembler. */
extern volatile uint8_t SR_lock;


typedef uint16_t  LDD_TError;           /*!< Error type. */
typedef uint8_t   LDD_TClockConfiguration; /*!< CPU clock configuration type. */

/*! Driver operation mode type. */
typedef enum {
  DOM_NONE,
  DOM_RUN,
  DOM_WAIT,
  DOM_SLEEP,
  DOM_STOP
} LDD_TDriverOperationMode;

typedef void LDD_TCallbackParam;       /*!< Pointer to this type specifies the user data to be passed as a callback parameter. */
typedef void (*LDD_TCallback)(LDD_TCallbackParam *CallbackParam); /*!< Callback type used for definition of callback functions. */



/*
** ===================================================================
**     Method      :  Cpu_MCGAutoTrim (component MK60FN1M0LQ15)
**     Description :
**         This method uses MCG auto trim feature to trim internal
**         reference clock. This method can be used only in a clock
**         configuration which derives its bus clock from external
**         reference clock (<MCG mode> must be one of the following
**         modes - FEE, FBE, BLPE, PEE, PBE) and if value of <Bus clock>
**         is in the range <8; 16>MHz.
**         The slow internal reference clock is trimmed to the value
**         selected by <Slow internal reference clock [kHz]> property. 
**         The fast internal reference clock will be trimmed to value
**         4MHz.
**     Parameters  :
**         NAME            - DESCRIPTION
**         ClockSelect     - Selects which internal
**                           reference clock will be trimmed.
**                           0 ... slow (32kHz) internal reference clock
**                           will be trimmed
**                           > 0 ... fast (4MHz) internal reference
**                           clock will be trimmed
**     Returns     :
**         ---             - Error code
**                           ERR_OK - OK
**                           ERR_SPEED - The method does not work in the
**                           active clock configuration.
**                           ERR_FAILED - Autotrim process failed.
** ===================================================================
*/
LDD_TError Cpu_MCGAutoTrim(uint8_t ClockSelect);

/*
** ===================================================================
**     Method      :  Cpu_GetLLSWakeUpFlags (component MK60FN1M0LQ15)
**     Description :
**         This method returns the current status of the LLWU wake-up
**         flags indicating which wake-up source caused the MCU to exit
**         LLS or VLLSx low power mode.
**         The following predefined constants can be used to determine
**         the wake-up source:
**         LLWU_EXT_PIN0, ... LLWU_EXT_PIN15 - external pin 0 .. 15
**         caused the wake-up
**         LLWU_INT_MODULE0 .. LLWU_INT_MODULE7 - internal module 0..15
**         caused the wake-up.
**     Parameters  : None
**     Returns     :
**         ---             - Returns the current status of the LLWU
**                           wake-up flags indicating which wake-up
**                           source caused the MCU to exit LLS or VLLSx
**                           low power mode.
** ===================================================================
*/
uint32_t Cpu_GetLLSWakeUpFlags(void);

/*
** ===================================================================
**     Method      :  Cpu_SetClockConfiguration (component MK60FN1M0LQ15)
**     Description :
**         Calling of this method will cause the clock configuration
**         change and reconfiguration of all components according to
**         the requested clock configuration setting.
**     Parameters  :
**         NAME            - DESCRIPTION
**         ModeID          - Clock configuration identifier
**     Returns     :
**         ---             - ERR_OK - OK.
**                           ERR_RANGE - Mode parameter out of range
** ===================================================================
*/
LDD_TError Cpu_SetClockConfiguration(LDD_TClockConfiguration ModeID);

/*
** ===================================================================
**     Method      :  Cpu_GetClockConfiguration (component MK60FN1M0LQ15)
**     Description :
**         Returns the active clock configuration identifier. The
**         method is enabled only if more than one clock configuration
**         is enabled in the component.
**     Parameters  : None
**     Returns     :
**         ---             - Active clock configuration identifier
** ===================================================================
*/
uint8_t Cpu_GetClockConfiguration(void);

/*
** ===================================================================
**     Method      :  Cpu_SetOperationMode (component MK60FN1M0LQ15)
**     Description :
**         This method requests to change the component's operation
**         mode (RUN, WAIT, SLEEP, STOP). The target operation mode
**         will be entered immediately. 
**         See <Operation mode settings> for further details of the
**         operation modes mapping to low power modes of the cpu.
**     Parameters  :
**         NAME            - DESCRIPTION
**         OperationMode   - Requested driver
**                           operation mode
**         ModeChangeCallback - Callback to
**                           notify the upper layer once a mode has been
**                           changed. Parameter is ignored, only for
**                           compatibility of API with other components.
**       * ModeChangeCallbackParamPtr 
**                           - Pointer to callback parameter to notify
**                           the upper layer once a mode has been
**                           changed. Parameter is ignored, only for
**                           compatibility of API with other components.
**     Returns     :
**         ---             - Error code
**                           ERR_OK - OK
**                           ERR_PARAM_MODE - Invalid operation mode
** ===================================================================
*/
LDD_TError Cpu_SetOperationMode(LDD_TDriverOperationMode OperationMode, LDD_TCallback ModeChangeCallback, LDD_TCallbackParam *ModeChangeCallbackParamPtr);

/*
** ===================================================================
**     Method      :  Cpu_EnableInt (component MK60FN1M0LQ15)
**     Description :
**         Enables all maskable interrupts.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Cpu_EnableInt(void);

/*
** ===================================================================
**     Method      :  Cpu_DisableInt (component MK60FN1M0LQ15)
**     Description :
**         Disables all maskable interrupts.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Cpu_DisableInt(void);

/*
** ===================================================================
**     Method      :  Cpu_INT_LLWInterrupt (component MK60FN1M0LQ15)
**
**     Description :
**         This ISR services the 'LLWU' interrupt.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

void Init_cpu(void);
/*
** ===================================================================
**     Method      :  Init_cpu_clock_subsystem (component MK60FN1M0LQ15)
**
**     Description :
**         Initializes the whole system like timing, external bus, etc.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

/* END Cpu. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif
/* __Cpu_H */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.2 [05.06]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
