/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : UART0.c
**     Project     : ProcessorExpert
**     Processor   : MK60FN1M0VLQ12
**     Component   : Init_UART
**     Version     : Component 01.004, Driver 01.04, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2013-08-09, 15:03, # CodeGen: 12
**     Abstract    :
**          This file implements the UART (UART0) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
**     Settings    :
**          Component name                                 : UART0
**          Device                                         : UART0
**          Settings                                       : 
**            Clock gate                                   : Enabled
**            Clock settings                               : 
**              Baud rate divisor                          : 65
**              Baud rate fine adjust                      : 3
**              Baud rate                                  : 115218.435 baud
**            Transfer settings                            : 
**              Data format                                : 8bit
**              Bits ordering                              : LSB first
**              Parity                                     : Off
**              Parity placement                           : Parity in last data bit
**            Idle character counting                      : After start bit
**            Break character generation length            : Short
**            LIN Break detection                          : Disabled
**            Stop in Wait mode                            : Disabled
**            Receiver wakeup settings                     : 
**              Receiver wakeup                            : Normal operation
**              Receiver wake up method                    : Idle-line
**              Receive wakeup idle detect                 : Don't set IDLE bit
**              Match address settings                     : 
**                Match address 1                          : Disabled
**                Match address 1 value                    : 0
**                Match address 2                          : Disabled
**                Match address 2 value                    : 0
**            Modem settings                               : 
**              Tx CTS                                     : Disabled
**              Tx RTS                                     : Disabled
**              Tx RTS polarity                            : Active low
**              Rx RTS                                     : Disabled
**            Infrared settings                            : 
**              Infrared                                   : Disabled
**              Tx narrow pulse width                      : 3/16
**            FIFOs settings                               : 
**              Tx FIFO                                    : Enabled
**              Tx watermark                               : 32
**              Rx FIFO                                    : Enabled
**              Rx watermark                               : 32
**            Smartcards interface (ISO7816) settings      : 
**              ISO7816 functionality                      : Disabled
**              Transfer type                              : T=0
**                Rx error action                          : Error NACK not generated
**                Rx FIFO overflow action                  : Overflow NACK not generated
**                Rx NACK threshold                        : 0
**                Tx NACK threshold                        : 0
**                Wait time value                          : 10
**              Init char detection                        : Disabled
**              Guard band N                               : 0
**              FD multiplier                              : 1
**            CEA709.1-B interface settings                : 
**              CEA709 functionality                       : Disabled
**              Collision detection                        : Disabled
**              Transmitter                                : Disabled
**              Collision signal polarity                  : Low
**              Packet time counter                        : 0
**              Beta 1 timer                               : 0
**              Secondary delay timer                      : 0
**              Preamble                                   : 0
**              WBase                                      : 0
**              Collision pulse width                      : 0
**              Receive indeterminate time                 : 0
**              Transmit indeterminate time                : 0
**            Loops and Single wire settings               : 
**              Loop mode                                  : Disabled
**              Receiver source in Loop mode               : Loop mode
**              TxD pin direction in Single-wire mode      : Input
**            Receiver input                               : Not inverted
**            Transmitter output                           : Not inverted
**          Pins/Signals                                   : 
**            Receiver pin                                 : Enabled
**              Pin                                        : TSI0_CH9/PTB16/SPI1_SOUT/UART0_RX/I2S1_TXD0/FBa_AD17/EWM_IN
**              Pin signal                                 : 
**            Transmitter pin                              : Enabled
**              Pin                                        : TSI0_CH10/PTB17/SPI1_SIN/UART0_TX/I2S1_TXD1/FBa_AD16/EWM_OUT_b
**              Pin signal                                 : 
**              Transmitter modulation                     : Disabled
**            CTS pin                                      : Disabled
**            RTS pin                                      : Disabled
**          Interrupts/DMA                                 : 
**            Common Tx/Rx interrupt                       : 
**              Interrupt                                  : INT_UART0_RX_TX
**              Interrupt request                          : Disabled
**              Interrupt priority                         : 0 (Highest)
**              ISR name                                   : 
**              Transmit empty request                     : Disabled
**              Transmit empty request type                : IRQ
**              Transmit complete request                  : Disabled
**              Receiver full request                      : Disabled
**              Receiver full request type                 : IRQ
**              Idle line request                          : Disabled
**              LIN break detect request                   : Disabled
**              Rx active edge interrupt                   : Disabled
**              Smartcards interface (ISO7816) interrupts  : 
**                Init char detected interrupt             : Disabled
**            Error Interrupt                              : 
**              Error interrupt                            : INT_UART0_ERR
**              Interrupt request                          : Disabled
**              Interrupt priority                         : 0 (Highest)
**              ISR name                                   : 
**              Overrun error interrupt                    : Disabled
**              Noise error interrupt                      : Disabled
**              Framing error interrupt                    : Disabled
**              Parity error interrupt                     : Disabled
**              FIFOs interrupts                           : 
**                Tx FIFO overflow interrupt               : Disabled
**                Rx FIFO underflow interrupt              : Disabled
**              Smartcards interface (ISO7816) interrupts  : 
**                Wait timer interrupt                     : Disabled
**                Char wait timer interrupt                : Disabled
**                Block wait timer interrupt               : Disabled
**                Tx threshold exceeded interrupt          : Disabled
**                Rx threshold exceeded                    : Disabled
**                Guard timer violated interrupt           : Disabled
**            CEA709 Interrupt                             : 
**              CEA709 interrupt                           : INT_UART0_LON
**              Interrupt request                          : Disabled
**              Interrupt priority                         : 0 (Highest)
**              ISR name                                   : 
**              Wbase expired interrupt                    : Disabled
**              Initial sync detection interrupt           : Disabled
**              Packet received interrupt                  : Disabled
**              Packet transmitted interrupt               : Disabled
**              Packet cycle timer interrupt               : Disabled
**              Preamble start interrupt                   : Disabled
**              Transmission fail interrupt                : Disabled
**          Initialization                                 : 
**            Send break                                   : Disabled
**            Enable transmitter                           : Enabled
**            Enable receiver                              : Enabled
**            Call Init method                             : yes
**     Contents    :
**         Init - void UART0_Init(void);
**
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**     SOURCE DISTRIBUTION PERMISSIBLE as directed in End User License Agreement.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/
/*!
** @file UART0.c
** @version 01.04
** @brief
**          This file implements the UART (UART0) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
*/         
/*!
**  @addtogroup UART0_module UART0 module documentation
**  @{
*/         

/* MODULE UART0. */

#include "UART0.h"
  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Cpu.h"


/*
** ===================================================================
**     Method      :  UART0_Init (component Init_UART)
**     Description :
**         This method initializes registers of the UART module
**         according to the Peripheral Initialization settings.
**         Call this method in user code to initialize the module. By
**         default, the method is called by PE automatically; see "Call
**         Init method" property of the component for more details.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void UART0_Init(void)
{
  /* SIM_SCGC4: UART0=1 */
  SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;                                   
  /* UART0_C2: TE=0,RE=0 */
  UART0_C2 &= (uint8_t)~(uint8_t)((UART_C2_TE_MASK | UART_C2_RE_MASK));                                   
  /* UART0_BDL: SBR=0x41 */
  UART0_BDL = UART_BDL_SBR(0x41);                                   
  /* UART0_BDH: LBKDIE=0,RXEDGIE=0,??=0,SBR=0 */
  UART0_BDH = UART_BDH_SBR(0x00);                                   
  /* UART0_MA1: MA=0 */
  UART0_MA1 = UART_MA1_MA(0x00);                                   
  /* UART0_MA2: MA=0 */
  UART0_MA2 = UART_MA2_MA(0x00);                                   
  /* UART0_C4: MAEN1=0,MAEN2=0,M10=0,BRFA=3 */
  UART0_C4 = UART_C4_BRFA(0x03);                                   
  /* UART0_C1: LOOPS=0,UARTSWAI=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0 */
  UART0_C1 = 0x00U;                                   
  /* UART0_S2: LBKDIF=1,RXEDGIF=1,MSBF=0,RXINV=0,RWUID=0,BRK13=0,LBKDE=0,RAF=0 */
  UART0_S2 = (UART_S2_LBKDIF_MASK | UART_S2_RXEDGIF_MASK);                                   
  /* UART0_MODEM: ??=0,??=0,??=0,??=0,RXRTSE=0,TXRTSPOL=0,TXRTSE=0,TXCTSE=0 */
  UART0_MODEM = 0x00U;                                   
  /* UART0_IR: ??=0,??=0,??=0,??=0,??=0,IREN=0,TNP=0 */
  UART0_IR = UART_IR_TNP(0x00);                                   
  /* UART0_TWFIFO: TXWATER=0x20 */
  UART0_TWFIFO = UART_TWFIFO_TXWATER(0x20);                                   
  /* UART0_RWFIFO: RXWATER=0x20 */
  UART0_RWFIFO = UART_RWFIFO_RXWATER(0x20);                                   
  /* UART0_SFIFO: TXEMPT=0,RXEMPT=0,??=0,??=0,??=0,RXOF=0,TXOF=1,RXUF=1 */
  UART0_SFIFO = (UART_SFIFO_TXOF_MASK | UART_SFIFO_RXUF_MASK);                                   
  /* UART0_CFIFO: TXFLUSH=1,RXFLUSH=1,??=0,??=0,??=0,RXOFE=0,TXOFE=0,RXUFE=0 */
  UART0_CFIFO = (UART_CFIFO_TXFLUSH_MASK | UART_CFIFO_RXFLUSH_MASK);                                   
  /* UART0_PFIFO: TXFE=1,RXFE=1 */
  UART0_PFIFO |= (UART_PFIFO_TXFE_MASK | UART_PFIFO_RXFE_MASK);                                   
  /* UART0_WN7816: GTN=0 */
  UART0_WN7816 = UART_WN7816_GTN(0x00);                                   
  /* UART0_WF7816: GTFD=1 */
  UART0_WF7816 = UART_WF7816_GTFD(0x01);                                   
  /* UART0_WP7816T0: WI=0x0A */
  UART0_WP7816T0 = UART_WP7816_T_TYPE0_WI(0x0A);                                   
  /* UART0_ET7816: TXTHRESHOLD=0,RXTHRESHOLD=0 */
  UART0_ET7816 = (UART_ET7816_TXTHRESHOLD(0x00) | UART_ET7816_RXTHRESHOLD(0x00));                                   
  /* UART0_IS7816: WT=1,CWT=1,BWT=1,INITD=1,??=0,GTV=1,TXT=1,RXT=1 */
  UART0_IS7816 = UART_IS7816_WT_MASK |
                 UART_IS7816_CWT_MASK |
                 UART_IS7816_BWT_MASK |
                 UART_IS7816_INITD_MASK |
                 UART_IS7816_GTV_MASK |
                 UART_IS7816_TXT_MASK |
                 UART_IS7816_RXT_MASK;       
  /* UART0_IE7816: WTE=0,CWTE=0,BWTE=0,INITDE=0,??=0,GTVE=0,TXTE=0,RXTE=0 */
  UART0_IE7816 = 0x00U;                                   
  /* UART0_C7816: ??=0,??=0,??=0,ONACK=0,ANACK=0,INIT=0,TTYPE=0,ISO_7816E=0 */
  UART0_C7816 = 0x00U;                                   
  /* UART0_PCTH: PCTH=0 */
  UART0_PCTH = UART_PCTH_PCTH(0x00);                                   
  /* UART0_PCTL: PCTL=0 */
  UART0_PCTL = UART_PCTL_PCTL(0x00);                                   
  /* UART0_B1T: B1T=0 */
  UART0_B1T = UART_B1T_B1T(0x00);                                   
  /* UART0_SDTH: SDTH=0 */
  UART0_SDTH = UART_SDTH_SDTH(0x00);                                   
  /* UART0_SDTL: SDTL=0 */
  UART0_SDTL = UART_SDTL_SDTL(0x00);                                   
  /* UART0_PRE: PREAMBLE=0 */
  UART0_PRE = UART_PRE_PREAMBLE(0x00);                                   
  /* UART0_WB: WBASE=0 */
  UART0_WB = UART_WB_WBASE(0x00);                                   
  /* UART0_CPW: CPW=0 */
  UART0_CPW = UART_CPW_CPW(0x00);                                   
  /* UART0_RIDT: RIDT=0 */
  UART0_RIDT = UART_RIDT_RIDT(0x00);                                   
  /* UART0_TIDT: TIDT=0 */
  UART0_TIDT = UART_TIDT_TIDT(0x00);                                   
  /* UART0_C6: EN709=0,TX709=0,CE=0,CP=0 */
  UART0_C6 &= (uint8_t)~(uint8_t)(
               UART_C6_EN709_MASK |
               UART_C6_TX709_MASK |
               UART_C6_CE_MASK |
               UART_C6_CP_MASK
              );                                   
  /* UART0_S3: PEF=1,WBEF=1,ISD=0,PRXF=1,PTXF=1,PCTEF=1,PSF=1,TXFF=1 */
  UART0_S3 = UART_S3_PEF_MASK |
             UART_S3_WBEF_MASK |
             UART_S3_PRXF_MASK |
             UART_S3_PTXF_MASK |
             UART_S3_PCTEF_MASK |
             UART_S3_PSF_MASK |
             UART_S3_TXFF_MASK;       
  /* UART0_S4: ??=0,??=0,??=0,INITF=0,CDET=3,ILCV=1,FE=1 */
  UART0_S4 = (UART_S4_CDET(0x03) | UART_S4_ILCV_MASK | UART_S4_FE_MASK);                                   
  /* UART0_IE: WBEIE=0,ISDIE=0,PRXIE=0,PTXIE=0,PCTEIE=0,PSIE=0,TXFIE=0 */
  UART0_IE &= (uint8_t)~(uint8_t)(
               UART_IE_WBEIE_MASK |
               UART_IE_ISDIE_MASK |
               UART_IE_PRXIE_MASK |
               UART_IE_PTXIE_MASK |
               UART_IE_PCTEIE_MASK |
               UART_IE_PSIE_MASK |
               UART_IE_TXFIE_MASK
              );                                   
      (void) UART0_S1;                 /* Dummy read of the UART0_S1 register to clear flags */
      (void) UART0_D;                  /* Dummy read of the UART0_D register to clear flags */
  /* UART0_C5: TDMAS=0,??=0,RDMAS=0,??=0,??=0,??=0,??=0,??=0 */
  UART0_C5 = 0x00U;                                   
  /* UART0_C3: R8=0,T8=0,TXDIR=0,TXINV=0,ORIE=0,NEIE=0,FEIE=0,PEIE=0 */
  UART0_C3 = 0x00U;                                   
  /* UART0_C2: TIE=0,TCIE=0,RIE=0,ILIE=0,TE=1,RE=1,RWU=0,SBK=0 */
  UART0_C2 = (UART_C2_TE_MASK | UART_C2_RE_MASK);                                   
}


/* END UART0. */
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
