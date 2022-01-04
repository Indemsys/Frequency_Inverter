/**************************************************
 *
 * Copyright 2010 IAR Systems. All rights reserved.
 *
 * $Revision: #1 $
 *
 **************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;
        

        MODULE  ?cstartup
        

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:ROOT(2)

        EXTERN  __startup
        PUBLIC  ___VECTOR_RAM
        PUBLIC  __vector_table

        DATA
___VECTOR_RAM
__vector_table
        DCD     sfe(CSTACK)               ; Top of Stack
        DCD     __startup                 ; Reset Handler
        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler
        ; External Interrupts
        DCD     DMA0_IRQHandler           ; 0:  DMA Channel 0 transfer complete
        DCD     DMA1_IRQHandler           ; 1:  DMA Channel 1 transfer complete
        DCD     DMA2_IRQHandler           ; 2:  DMA Channel 2 transfer complete
        DCD     DMA3_IRQHandler           ; 3:  DMA Channel 3 transfer complete
        DCD     DMA4_IRQHandler           ; 4:  DMA Channel 4 transfer complete
        DCD     DMA5_IRQHandler           ; 5:  DMA Channel 5 transfer complete
        DCD     DMA6_IRQHandler           ; 6:  DMA Channel 6 transfer complete
        DCD     DMA7_IRQHandler           ; 7:  DMA Channel 7 transfer complete
        DCD     DMA8_IRQHandler           ; 8:  DMA Channel 8 transfer complete
        DCD     DMA9_IRQHandler           ; 9:  DMA Channel 9 transfer complete
        DCD     DMA10_IRQHandler          ;10:  DMA Channel 10 transfer complete
        DCD     DMA11_IRQHandler          ;11:  DMA Channel 11 transfer complete
        DCD     DMA12_IRQHandler          ;12:  DMA Channel 12 transfer complete
        DCD     DMA13_IRQHandler          ;13:  DMA Channel 13 transfer complete
        DCD     DMA14_IRQHandler          ;14:  DMA Channel 14 transfer complete
        DCD     DMA15_IRQHandler          ;15:  DMA Channel 15 transfer complete
        DCD     DMA_ERR_IRQHandler        ;16:  DMA Error Interrupt Channels 0-15
        DCD     MCM_IRQHandler            ;17:  MCM Normal interrupt
        DCD     FLASH_CC_IRQHandler       ;18:  Flash memory command complete
        DCD     FLASH_RC_IRQHandler       ;19:  Flash memory read collision
        DCD     VLD_IRQHandler            ;20:  Low Voltage Detect, Low Voltage Warning
        DCD     LLWU_IRQHandler           ;21:  Low Leakage Wakeup
        DCD     WDOG_IRQHandler           ;22:  WDOG interrupt
        DCD     0                         ;23:  reserved
        DCD     I2C0_IRQHandler           ;24:  I2C0 interrupt
        DCD     I2C1_IRQHandler           ;25:  I2C1 interrupt
        DCD     SPI0_IRQHandler           ;26:  SPI 0 interrupt
        DCD     SPI1_IRQHandler           ;27:  SPI 1 interrupt
        DCD     SPI2_IRQHandler           ;28:  SPI 2 interrupt
        DCD     CAN0_MESS_IRQHandler      ;29:  CAM 0 OR'ed Message buffer (0-15)
        DCD     CAN0_BUS_OFF_IRQHandler   ;30:  CAM 0 Bus Off
        DCD     CAN0_ERR_IRQHandler       ;31:  CAM 0 Error
        DCD     CAN0_TW_IRQHandler        ;32:  CAM 0 Transmit Warning
        DCD     CAN0_RW_IRQHandler        ;33:  CAM 0 Receive Warning
        DCD     CAN0_WAKE_UP_IRQHandler   ;34:  CAM 0 WakeUp
        DCD     I2S0_Transmit_IRQHandler  ;35:  I2S0_Transmit_IRQHandler
        DCD     I2S0_Receive_IRQHandler   ;36:  I2S0_Receive_IRQHandler
        DCD     CAN1_MESS_IRQHandler      ;37:  CAM 1 OR'ed Message buffer (0-15)
        DCD     CAN1_BUS_OFF_IRQHandler   ;38:  CAM 1 Bus Off
        DCD     CAN1_ERR_IRQHandler       ;39:  CAM 1 Error
        DCD     CAN1_TW_IRQHandler        ;40:  CAM 1 Transmit Warning
        DCD     CAN1_RW_IRQHandler        ;41:  CAM 1 Receive Warning
        DCD     CAN1_WAKE_UP_IRQHandler   ;42:  CAM 1 WakeUp
        DCD     0                         ;43:  reserved
        DCD     UART0_LON_IRQHandler      ;44:  Single interrupt vector for UART LON sources
        DCD     UART0_IRQHandler          ;45:  UART 0 intertrupt
        DCD     UART0_ERR_IRQHandler      ;46:  UART 0 error intertrupt
        DCD     UART1_IRQHandler          ;47:  UART 1 intertrupt
        DCD     UART1_ERR_IRQHandler      ;48:  UART 1 error intertrupt
        DCD     UART2_IRQHandler          ;49:  UART 2 intertrupt
        DCD     UART2_ERR_IRQHandler      ;50:  UART 2 error intertrupt
        DCD     UART3_IRQHandler          ;51:  UART 3 intertrupt
        DCD     UART3_ERR_IRQHandler      ;52:  UART 3 error intertrupt
        DCD     UART4_IRQHandler          ;53:  UART 4 intertrupt
        DCD     UART4_ERR_IRQHandler      ;54:  UART 4 error intertrupt
        DCD     UART5_IRQHandler          ;55:  UART 5 intertrupt
        DCD     UART5_ERR_IRQHandler      ;56:  UART 5 error intertrupt
        DCD     ADC0_IRQHandler           ;57:  ADC 0 interrupt
        DCD     ADC1_IRQHandler           ;58:  ADC 1 interrupt
        DCD     CMP0_IRQHandler           ;59:  CMP 0 High-speed comparator interrupt
        DCD     CMP1_IRQHandler           ;60:  CMP 1 interrupt
        DCD     CMP2_IRQHandler           ;61:  CMP 2 interrupt
        DCD     FTM0_IRQHandler           ;62:  FTM 0 interrupt
        DCD     FTM1_IRQHandler           ;63:  FTM 1 interrupt
        DCD     FTM2_IRQHandler           ;64:  FTM 2 interrupt
        DCD     CMT_IRQHandler            ;65:  CMT intrrupt
        DCD     RTC_IRQHandler            ;66:  RTC interrupt
        DCD     0                         ;67:  reserved
        DCD     PIT0_IRQHandler           ;68:  PIT 0 interrupt
        DCD     PIT1_IRQHandler           ;69:  PIT 1 interrupt
        DCD     PIT2_IRQHandler           ;70:  PIT 2 interrupt
        DCD     PIT3_IRQHandler           ;71:  PIT 3 interrupt
        DCD     PDB_IRQHandler            ;72:  PDB interrupt
        DCD     USB_OTG_IRQHandler        ;73:  USB OTG interrupt
        DCD     USB_CD_IRQHandler         ;74:  USB Charger Detect interrupt
        DCD     0                         ;75:  reserved
        DCD     0                         ;76:  reserved
        DCD     0                         ;77:  reserved
        DCD     0                         ;78:  reserved
        DCD     I2S_IRQHandler            ;79:  I2S interrupt
        DCD     SDHC_IRQHandler           ;80:  SDHC interrupt
        DCD     DAC0_IRQHandler           ;81:  DAC 0 interrupt
        DCD     DAC1_IRQHandler           ;82:  DAC 1 interrupt
        DCD     TSI_IRQHandler            ;83:  TSI interrupt
        DCD     MCG_IRQHandler            ;84:  MCG interrupt
        DCD     LPT_IRQHandler            ;85:  LPT interrupt
        DCD     LCD_IRQHandler            ;86:  LCD interrupt
        DCD     PORTA_IRQHandler          ;87:  PORT A interrupt
        DCD     PORTB_IRQHandler          ;88:  PORT B interrupt
        DCD     PORTC_IRQHandler          ;89:  PORT C interrupt
        DCD     PORTD_IRQHandler          ;90:  PORT D interrupt
        DCD     PORTE_IRQHandler          ;91:  PORT E interrupt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;

      PUBWEAK NMI_Handler
      PUBWEAK HardFault_Handler
      PUBWEAK MemManage_Handler
      PUBWEAK BusFault_Handler
      PUBWEAK UsageFault_Handler
      PUBWEAK SVC_Handler
      PUBWEAK DebugMon_Handler
      PUBWEAK PendSV_Handler
      PUBWEAK SysTick_Handler
      PUBWEAK DMA0_IRQHandler
      PUBWEAK DMA1_IRQHandler
      PUBWEAK DMA2_IRQHandler
      PUBWEAK DMA3_IRQHandler
      PUBWEAK DMA4_IRQHandler
      PUBWEAK DMA5_IRQHandler
      PUBWEAK DMA6_IRQHandler
      PUBWEAK DMA7_IRQHandler
      PUBWEAK DMA8_IRQHandler
      PUBWEAK DMA9_IRQHandler
      PUBWEAK DMA10_IRQHandler
      PUBWEAK DMA11_IRQHandler
      PUBWEAK DMA12_IRQHandler
      PUBWEAK DMA13_IRQHandler
      PUBWEAK DMA14_IRQHandler
      PUBWEAK DMA15_IRQHandler
      PUBWEAK DMA_ERR_IRQHandler
      PUBWEAK MCM_IRQHandler
      PUBWEAK FLASH_CC_IRQHandler
      PUBWEAK FLASH_RC_IRQHandler
      PUBWEAK VLD_IRQHandler
      PUBWEAK LLWU_IRQHandler
      PUBWEAK WDOG_IRQHandler
      PUBWEAK I2C0_IRQHandler
      PUBWEAK I2C1_IRQHandler
      PUBWEAK SPI0_IRQHandler
      PUBWEAK SPI1_IRQHandler
      PUBWEAK SPI2_IRQHandler
      PUBWEAK CAN0_MESS_IRQHandler
      PUBWEAK CAN0_BUS_OFF_IRQHandler
      PUBWEAK CAN0_ERR_IRQHandler
      PUBWEAK CAN0_TW_IRQHandler
      PUBWEAK CAN0_RW_IRQHandler
      PUBWEAK CAN0_WAKE_UP_IRQHandler
      PUBWEAK I2S0_Transmit_IRQHandler
      PUBWEAK I2S0_Receive_IRQHandler 
      PUBWEAK CAN1_MESS_IRQHandler
      PUBWEAK CAN1_BUS_OFF_IRQHandler
      PUBWEAK CAN1_ERR_IRQHandler
      PUBWEAK CAN1_TW_IRQHandler
      PUBWEAK CAN1_RW_IRQHandler
      PUBWEAK CAN1_WAKE_UP_IRQHandler
      PUBWEAK UART0_LON_IRQHandler  
      PUBWEAK UART0_IRQHandler
      PUBWEAK UART0_ERR_IRQHandler
      PUBWEAK UART1_IRQHandler
      PUBWEAK UART1_ERR_IRQHandler
      PUBWEAK UART2_IRQHandler
      PUBWEAK UART2_ERR_IRQHandler
      PUBWEAK UART3_IRQHandler
      PUBWEAK UART3_ERR_IRQHandler
      PUBWEAK UART4_IRQHandler
      PUBWEAK UART4_ERR_IRQHandler
      PUBWEAK UART5_IRQHandler
      PUBWEAK UART5_ERR_IRQHandler
      PUBWEAK ADC0_IRQHandler
      PUBWEAK ADC1_IRQHandler
      PUBWEAK CMP0_IRQHandler
      PUBWEAK CMP1_IRQHandler
      PUBWEAK CMP2_IRQHandler
      PUBWEAK FTM0_IRQHandler
      PUBWEAK FTM1_IRQHandler
      PUBWEAK FTM2_IRQHandler
      PUBWEAK CMT_IRQHandler
      PUBWEAK RTC_IRQHandler
      PUBWEAK PIT0_IRQHandler
      PUBWEAK PIT1_IRQHandler
      PUBWEAK PIT2_IRQHandler
      PUBWEAK PIT3_IRQHandler
      PUBWEAK PDB_IRQHandler
      PUBWEAK USB_OTG_IRQHandler
      PUBWEAK USB_CD_IRQHandler
      PUBWEAK I2S_IRQHandler
      PUBWEAK SDHC_IRQHandler
      PUBWEAK DAC0_IRQHandler
      PUBWEAK DAC1_IRQHandler
      PUBWEAK TSI_IRQHandler
      PUBWEAK MCG_IRQHandler
      PUBWEAK LPT_IRQHandler
      PUBWEAK LCD_IRQHandler
      PUBWEAK PORTA_IRQHandler
      PUBWEAK PORTB_IRQHandler
      PUBWEAK PORTC_IRQHandler
      PUBWEAK PORTD_IRQHandler
      PUBWEAK PORTE_IRQHandler

      SECTION .text:CODE:REORDER(1)
      THUMB
NMI_Handler
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
SVC_Handler
DebugMon_Handler
PendSV_Handler
SysTick_Handler
DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
DMA4_IRQHandler
DMA5_IRQHandler
DMA6_IRQHandler
DMA7_IRQHandler
DMA8_IRQHandler
DMA9_IRQHandler
DMA10_IRQHandler
DMA11_IRQHandler
DMA12_IRQHandler
DMA13_IRQHandler
DMA14_IRQHandler
DMA15_IRQHandler
DMA_ERR_IRQHandler
MCM_IRQHandler
FLASH_CC_IRQHandler
FLASH_RC_IRQHandler
VLD_IRQHandler
LLWU_IRQHandler
WDOG_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
CAN0_MESS_IRQHandler
CAN0_BUS_OFF_IRQHandler
CAN0_ERR_IRQHandler
CAN0_TW_IRQHandler
CAN0_RW_IRQHandler
CAN0_WAKE_UP_IRQHandler
I2S0_Transmit_IRQHandler  
I2S0_Receive_IRQHandler   
CAN1_MESS_IRQHandler
CAN1_BUS_OFF_IRQHandler
CAN1_ERR_IRQHandler
CAN1_TW_IRQHandler
CAN1_RW_IRQHandler
CAN1_WAKE_UP_IRQHandler
UART0_LON_IRQHandler   
UART0_IRQHandler
UART0_ERR_IRQHandler
UART1_IRQHandler
UART1_ERR_IRQHandler
UART2_IRQHandler
UART2_ERR_IRQHandler
UART3_IRQHandler
UART3_ERR_IRQHandler
UART4_IRQHandler
UART4_ERR_IRQHandler
UART5_IRQHandler
UART5_ERR_IRQHandler
ADC0_IRQHandler
ADC1_IRQHandler
CMP0_IRQHandler
CMP1_IRQHandler
CMP2_IRQHandler
FTM0_IRQHandler
FTM1_IRQHandler
FTM2_IRQHandler
CMT_IRQHandler
RTC_IRQHandler
PIT0_IRQHandler
PIT1_IRQHandler
PIT2_IRQHandler
PIT3_IRQHandler
PDB_IRQHandler
USB_OTG_IRQHandler
USB_CD_IRQHandler
I2S_IRQHandler
SDHC_IRQHandler
DAC0_IRQHandler
DAC1_IRQHandler
TSI_IRQHandler
MCG_IRQHandler
LPT_IRQHandler
LCD_IRQHandler
PORTA_IRQHandler
PORTB_IRQHandler
PORTC_IRQHandler
PORTD_IRQHandler
PORTE_IRQHandler
Default_Handler

        B Default_Handler
        END
