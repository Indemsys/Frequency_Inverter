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
* $FileName: serl_int_mcf52xx.c$
* $Version : 3.8.4.0$
* $Date    : Sep-15-2011$
*
* Comments:
*
*   This file contains the low level functions for the interrupt driven
*   serial I/O for the MCF52XX UART.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "io_prv.h"
#include "charq.h"
#include "fio_prv.h"
#include "serinprv.h"
#include "serl_mcf52xx.h"

/* Polled functions used */
extern uint_32 _mcf52xx_uart_serial_polled_init(MCF52XX_UART_SERIAL_INIT_STRUCT_PTR,
   pointer _PTR_, char _PTR_);
extern uint_32 _mcf52xx_uart_serial_polled_deinit(MCF52XX_UART_SERIAL_INIT_STRUCT_PTR,
   MCF52XX_UART_SERIAL_INFO_STRUCT_PTR);
extern uint_32 _mcf52xx_uart_serial_polled_ioctl(MCF52XX_UART_SERIAL_INFO_STRUCT_PTR, uint_32,
   uint_32_ptr);

/* Interrupt driver functions */

extern void    _mcf52xx_uart_serial_int_putc(IO_SERIAL_INT_DEVICE_STRUCT_PTR, char);

extern uint_32 _mcf52xx_uart_serial_int_init(IO_SERIAL_INT_DEVICE_STRUCT_PTR,
 char _PTR_);
extern uint_32 _mcf52xx_uart_serial_int_deinit(MCF52XX_UART_SERIAL_INIT_STRUCT_PTR,
   MCF52XX_UART_SERIAL_INFO_STRUCT_PTR);
extern uint_32 _mcf52xx_uart_serial_int_enable(MCF52XX_UART_SERIAL_INFO_STRUCT_PTR);
extern void    _mcf52xx_uart_serial_int_isr(pointer);


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf52xx_uart_serial_int_install
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    Install an interrupt driven uart serial device.
*
*END*----------------------------------------------------------------------*/

uint_32 _mcf52xx_uart_serial_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr identifier,

      /* [IN] The I/O init data pointer */
      MCF52XX_UART_SERIAL_INIT_STRUCT_CPTR  init_data_ptr,

      /* [IN] The I/O queue size to use */
      uint_32  queue_size
   )
{ /* Body */

   return _io_serial_int_install(identifier,
      (uint_32 (_CODE_PTR_)(pointer, char _PTR_))_mcf52xx_uart_serial_int_init,
      (uint_32 (_CODE_PTR_)(pointer))_mcf52xx_uart_serial_int_enable,
      (uint_32 (_CODE_PTR_)(pointer,pointer))_mcf52xx_uart_serial_int_deinit,
      (void    (_CODE_PTR_)(pointer, char))_mcf52xx_uart_serial_int_putc,
      (uint_32 (_CODE_PTR_)(pointer, uint_32, pointer))_mcf52xx_uart_serial_polled_ioctl,
      (pointer)init_data_ptr, queue_size);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_int_init
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    This function initializes the UART in interrupt mode.
*
*END*********************************************************************/

uint_32 _mcf52xx_uart_serial_int_init
   (
      /* [IN] the interrupt I/O initialization information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the rest of the name of the device opened */
      char _PTR_                      open_name_ptr
   )
{ /* Body */
   MCF52XX_UART_SERIAL_INFO_STRUCT_PTR uart_info_ptr;
   MCF52XX_UART_SERIAL_INIT_STRUCT_PTR uart_init_ptr;
   uint_32                             result;

   uart_init_ptr = int_io_dev_ptr->DEV_INIT_DATA_PTR;
   result = _mcf52xx_uart_serial_polled_init((pointer)uart_init_ptr,
      &int_io_dev_ptr->DEV_INFO_PTR, open_name_ptr);
   if (result != MQX_OK) {
      return(result);
   }/* Endif */

   uart_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   uart_info_ptr->OLD_ISR_DATA = _int_get_isr_data(uart_init_ptr->VECTOR);
   uart_info_ptr->OLD_ISR_EXCEPTION_HANDLER =
      _int_get_exception_handler(uart_init_ptr->VECTOR);

   uart_info_ptr->OLD_ISR =
      _int_install_isr(uart_init_ptr->VECTOR, _mcf52xx_uart_serial_int_isr,
      int_io_dev_ptr);

   return(MQX_OK);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_int_deinit
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    This function de-initializes the UART in interrupt mode.
*
*END*********************************************************************/

uint_32 _mcf52xx_uart_serial_int_deinit
   (
      /* [IN] the interrupt I/O initialization information */
      MCF52XX_UART_SERIAL_INIT_STRUCT_PTR io_init_ptr,

      /* [IN] the address of the device specific information */
      MCF52XX_UART_SERIAL_INFO_STRUCT_PTR io_info_ptr
   )
{ /* Body */
   volatile MCF52XX_UART_STRUCT _PTR_ uart_ptr = io_info_ptr->UART_PTR;

   uart_ptr->WRITE.UIMR = 0; /* Disable all interrupts */
   _mcf52xx_uart_serial_polled_deinit(io_init_ptr, io_info_ptr);
   _int_install_isr(io_init_ptr->VECTOR, io_info_ptr->OLD_ISR,
      io_info_ptr->OLD_ISR_DATA);

   return(MQX_OK);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_int_enable
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    This function enables the UART interrupts mode.
*
*END*********************************************************************/

uint_32 _mcf52xx_uart_serial_int_enable
   (
      /* [IN] the address of the device specific information */
      MCF52XX_UART_SERIAL_INFO_STRUCT_PTR io_info_ptr
   )
{ /* Body */
   uint_32 status;

/* Start CR 2087 */
   status = _mcf52xx_int_init((PSP_INTERRUPT_TABLE_INDEX)io_info_ptr->INIT.VECTOR,
      io_info_ptr->INIT.LEVEL, io_info_ptr->INIT.SUBLEVEL, TRUE);
/* End CR 2087 */      

   if (status == MQX_OK) {
//      volatile MCF52XX_STRUCT _PTR_      mcf52XX_ptr = io_info_ptr->MCF52XX_PTR;
      volatile MCF52XX_UART_STRUCT _PTR_ uart_ptr = io_info_ptr->UART_PTR;

      io_info_ptr->INT_ENABLE_BITS = MCF52XX_UART_UIMR_FFULL;
      uart_ptr->WRITE.UIMR = io_info_ptr->INT_ENABLE_BITS;
   } /* Endif */

   return status;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_int_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial I/O interrupts.
*
*************************************************************************/

void _mcf52xx_uart_serial_int_isr
   (
      /* [IN] the address of the device specific information */
      pointer parameter
   )
{ /* Body */
   IO_SERIAL_INT_DEVICE_STRUCT_PTR        int_io_dev_ptr = parameter;
   MCF52XX_UART_SERIAL_INFO_STRUCT_PTR    uart_info_ptr;
   volatile MCF52XX_UART_STRUCT _PTR_     uart_ptr;
   int_32                                 c;
   uint_16                                stat;
   boolean                                work;

   uart_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   uart_ptr = uart_info_ptr->UART_PTR;

   ++uart_info_ptr->INTERRUPTS;

   /* Receive loop */
   work = TRUE;
   while (work) {
      work = FALSE;
      stat = uart_ptr->READ.USR;

      while (stat & MCF52XX_UART_USR_RXRDY) {
         c = uart_ptr->READ.URB;    /* this clears the status bit */

         if (stat & (MCF52XX_UART_USR_RB | MCF52XX_UART_USR_FE |
                     MCF52XX_UART_USR_PE | MCF52XX_UART_USR_OE))
         {
            if (stat & MCF52XX_UART_USR_RB) {
               ++uart_info_ptr->RX_BREAKS;
               c = uart_ptr->READ.URB;    /* read break character */
               uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_BREAK;
            } /* Endif */
            if (stat & MCF52XX_UART_USR_FE) {
               ++uart_info_ptr->RX_FRAMING_ERRORS;
            } /* Endif */
            if (stat & MCF52XX_UART_USR_PE) {
               ++uart_info_ptr->RX_PARITY_ERRORS;
            } /* Endif */
            if (stat & MCF52XX_UART_USR_OE) {
               ++uart_info_ptr->RX_OVERRUNS;
            } /* Endif */
            uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_ERROR;
         } /* Endif */

         uart_info_ptr->RX_CHARS++;
         work = TRUE;

         if (!_io_serial_int_addc(int_io_dev_ptr, c)) {
            uart_info_ptr->RX_DROPPED_INPUT++;
         } /* Endif */
         stat = uart_ptr->READ.USR;

      } /* Endwhile */

      while (stat & MCF52XX_UART_USR_TXRDY)  {

         c = _io_serial_int_nextc(int_io_dev_ptr);
         if (c >= 0) {
            work = TRUE;
            uart_ptr->WRITE.UTB = c;
            uart_info_ptr->INT_ENABLE_BITS |= MCF52XX_UART_UIMR_TXRDY;
            uart_ptr->WRITE.UIMR = uart_info_ptr->INT_ENABLE_BITS;
         } else {
            /* No output. Turn off transmit interrupt */
            uart_info_ptr->INT_ENABLE_BITS &= ~MCF52XX_UART_UIMR_TXRDY;
            uart_ptr->WRITE.UIMR = uart_info_ptr->INT_ENABLE_BITS;
            break;
         } /* Endif */

         stat = uart_ptr->READ.USR;
         uart_info_ptr->TX_CHARS++;

      } /* Endwhile */

   } /* Endwhile */

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_int_putc
* Returned Value   : none
* Comments         :
*   This function is called to write out the first character, when
* the output serial device and output ring buffers are empty.
*
*END*********************************************************************/

void _mcf52xx_uart_serial_int_putc
   (
      /* [IN] the address of the device specific information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the character to write out now */
      char                       c
   )
{ /* Body */
   MCF52XX_UART_SERIAL_INFO_STRUCT_PTR uart_info_ptr;
   volatile MCF52XX_UART_STRUCT _PTR_  uart_ptr;

   uart_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   uart_ptr = uart_info_ptr->UART_PTR;

   while (!(uart_ptr->READ.USR & MCF52XX_UART_USR_TXRDY)) {
   } /* Endwhile */
   uart_ptr->WRITE.UTB = c;
   uart_info_ptr->TX_CHARS++;

   /* Interrupt when character has been sent */
   uart_info_ptr->INT_ENABLE_BITS |= MCF52XX_UART_UIMR_TXRDY;
   uart_ptr->WRITE.UIMR = uart_info_ptr->INT_ENABLE_BITS;

} /* Endbody */

/* EOF */
