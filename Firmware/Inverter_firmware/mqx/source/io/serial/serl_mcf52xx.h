/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: serl_mcf52xx.h$
* $Version : 3.6.4.0$
* $Date    : Dec-3-2010$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the serial drivers for the MCF5223
*
*END************************************************************************/
#ifndef _serial_mcf52xx_h
#define _serial_mcf52xx_h 1

#include "mcf52xx_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/

/*
** I/O Initialization error codes
*/
#define MCF52XX_IO_INVALID_CHANNEL        (0x10000)

/* Initialization values for the DEVICE field of the IO_INIT_STRUCT */
/* Channel number for Serial port 1 */
#define MCF52XX_IO_UART0                  (0)
/* Channel number for Serial port 2 */
#define MCF52XX_IO_UART1                  (1)
/* Channel number for Serial port 3 */
#define MCF52XX_IO_UART2                  (2)



/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** MCF52XX_UART_SERIAL_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct mcf52XX_uart_serial_init_struct
{

   /* The size of the queues to buffer incoming/outgoing data */
   uint_32 QUEUE_SIZE;

   /* The device to initialize */
   uint_32 DEVICE;

   /* The clock speed of cpu */
   uint_32 CLOCK_SPEED;

   /* The interrupt vector to use if interrupt driven */
   uint_32 VECTOR;

   /* The interrupt level to use if interrupt driven */
   _int_level LEVEL;

   /* The sub-level within the interrupt level to use if interrupt driven */
   _int_priority SUBLEVEL;

   /* The value for the UMR 1 register */
   uint_32 UMR1_VALUE;

   /* The value for the UMR 2 register */
   uint_32 UMR2_VALUE;

   /* The baud rate for the channel */
   uint_32 BAUD_RATE;

} MCF52XX_UART_SERIAL_INIT_STRUCT, _PTR_ MCF52XX_UART_SERIAL_INIT_STRUCT_PTR;

typedef const MCF52XX_UART_SERIAL_INIT_STRUCT _PTR_ MCF52XX_UART_SERIAL_INIT_STRUCT_CPTR;

/*
** MCF52XX_UART_SERIAL_INFO_STRUCT
** Run time state information for each serial channel
*/
typedef struct mcf52XX_uart_serial_info_struct
{
   /* The current init values for this port */
   MCF52XX_UART_SERIAL_INIT_STRUCT   INIT;

   /* The serial device register */
   MCF52XX_UART_STRUCT_PTR           UART_PTR;

   uint_32                           INT_ENABLE_BITS;

   uint_32                           INT_BITS;
   
   uint_32                           CONTROL_BITS;

   /* The previous interrupt handler and data */
   void       (_CODE_PTR_ OLD_ISR)(pointer);
   void       (_CODE_PTR_ OLD_ISR_EXCEPTION_HANDLER)(uint_32, uint_32, pointer,
               pointer);
   pointer                           OLD_ISR_DATA;

   /* Statistical information */
   uint_32                           INTERRUPTS;
   uint_32                           RX_CHARS;
   uint_32                           TX_CHARS;
   uint_32                           RX_BREAKS;
   uint_32                           RX_PARITY_ERRORS;
   uint_32                           RX_FRAMING_ERRORS;
   uint_32                           RX_OVERRUNS;
   uint_32                           RX_DROPPED_INPUT;
} MCF52XX_UART_SERIAL_INFO_STRUCT, _PTR_ MCF52XX_UART_SERIAL_INFO_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

extern uint_32 _mcf52xx_uart_serial_polled_init(MCF52XX_UART_SERIAL_INIT_STRUCT_PTR,pointer _PTR_,char _PTR_);
extern uint_32 _mcf52xx_uart_serial_polled_install(char_ptr, MCF52XX_UART_SERIAL_INIT_STRUCT_CPTR , uint_32);
extern uint_32 _mcf52xx_uart_serial_int_install(char_ptr, MCF52XX_UART_SERIAL_INIT_STRUCT_CPTR , uint_32);
extern pointer _bsp_get_serial_base_address (uint_8);
PSP_INTERRUPT_TABLE_INDEX _bsp_get_serial_vector (uint_8);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
