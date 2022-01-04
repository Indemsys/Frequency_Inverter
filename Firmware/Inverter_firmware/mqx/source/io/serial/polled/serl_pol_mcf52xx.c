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
* $FileName: serl_pol_mcf52xx.c$
* $Version : 3.8.16.0$
* $Date    : Aug-8-2012$
*
* Comments:
*
*   This file contains the functions for the polled serial I/O
*   low level functions for the MCF52XX UART.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "io_prv.h"
#include "charq.h"
#include "fio_prv.h"
#include "serplprv.h"
#include "serl_mcf52xx.h"

/* Polled driver functions */
extern char    _mcf52xx_uart_serial_polled_getc(MCF52XX_UART_SERIAL_INFO_STRUCT_PTR);
extern void    _mcf52xx_uart_serial_polled_putc(MCF52XX_UART_SERIAL_INFO_STRUCT_PTR, char);
extern boolean _mcf52xx_uart_serial_polled_status(MCF52XX_UART_SERIAL_INFO_STRUCT_PTR);
extern uint_32 _mcf52xx_uart_serial_polled_ioctl(MCF52XX_UART_SERIAL_INFO_STRUCT_PTR, uint_32,
   uint_32_ptr);
extern uint_32 _mcf52xx_uart_serial_polled_init(MCF52XX_UART_SERIAL_INIT_STRUCT_PTR,
   pointer _PTR_, char _PTR_);
extern uint_32 _mcf52xx_uart_serial_polled_deinit(MCF52XX_UART_SERIAL_INIT_STRUCT_PTR,
   MCF52XX_UART_SERIAL_INFO_STRUCT_PTR);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf52xx_uart_serial_polled_install
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    Install a polled uart serial device.
*
*END*----------------------------------------------------------------------*/

uint_32 _mcf52xx_uart_serial_polled_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr           identifier,

      /* [IN] The I/O init data pointer */
       MCF52XX_UART_SERIAL_INIT_STRUCT_CPTR     init_data_ptr,

      /* [IN] The I/O queue size to use */
      uint_32            queue_size
   )
{ /* Body */

  return _io_serial_polled_install(identifier,
      (uint_32 (_CODE_PTR_)(pointer, pointer _PTR_, char _PTR_))_mcf52xx_uart_serial_polled_init,
      (uint_32 (_CODE_PTR_)(pointer, pointer))_mcf52xx_uart_serial_polled_deinit,
      (char    (_CODE_PTR_)(pointer))_mcf52xx_uart_serial_polled_getc,
      (void    (_CODE_PTR_)(pointer, char))_mcf52xx_uart_serial_polled_putc,
      (boolean (_CODE_PTR_)(pointer))_mcf52xx_uart_serial_polled_status,
      (uint_32 (_CODE_PTR_)(pointer, uint_32, pointer))_mcf52xx_uart_serial_polled_ioctl,
      (pointer)init_data_ptr, queue_size);

} /* Endbody */



/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_polled_init
* Returned Value   : MQX_OK or a MQX error code.
* Comments         :
*    This function initializes the UART
*
*END*********************************************************************/

uint_32 _mcf52xx_uart_serial_polled_init
   (
      /* [IN] the initialization information for the device being opened */
      MCF52XX_UART_SERIAL_INIT_STRUCT_PTR io_init_ptr,

      /* [OUT] the address to store device specific information */
      pointer _PTR_                       io_info_ptr_ptr,

      /* [IN] the rest of the name of the device opened */
      char _PTR_                          open_name_ptr
   )
{ /* Body */
   volatile MCF52XX_UART_STRUCT _PTR_  uart_ptr;
   MCF52XX_UART_SERIAL_INFO_STRUCT _PTR_ uart_info_ptr;
   uint_32                             channel;
   uint_32                             baud;

   uart_info_ptr = _mem_alloc_system_zero((uint_32)
      sizeof(MCF52XX_UART_SERIAL_INFO_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if ( uart_info_ptr == NULL ) {
        return(MQX_OUT_OF_MEMORY);
   }/* Endif */
#endif
   _mem_set_type(uart_info_ptr,MEM_TYPE_IO_SERIAL_INFO_STRUCT);    

   *io_info_ptr_ptr = uart_info_ptr;

   /* Save initialization values */
   uart_info_ptr->INIT = *io_init_ptr;

   channel = uart_info_ptr->INIT.DEVICE;
   _bsp_serial_io_init (channel);
   uart_ptr = _bsp_get_serial_base_address (channel);
   uart_info_ptr->UART_PTR = (pointer)uart_ptr;

   uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_RX;
   uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_TX;
   uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_ERROR;
   uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_BREAK;
   uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_POINTER;

   uart_ptr->WRITE.UMR = uart_info_ptr->INIT.UMR1_VALUE;
   uart_ptr->WRITE.UMR = uart_info_ptr->INIT.UMR2_VALUE;

   uart_ptr->WRITE.UCSR = MCF52XX_UART_UCSR_RX_TIMER |
                          MCF52XX_UART_UCSR_TX_TIMER;

   uart_ptr->WRITE.UIMR = 0; /* Disable all interrupts */

   /* Set up the baud rate */
   /* baud = fsck / (32 * baudrate) + 0.5 */ 
   baud = (uint_32)((uart_info_ptr->INIT.CLOCK_SPEED + 16 * uart_info_ptr->INIT.BAUD_RATE )/ (32 * uart_info_ptr->INIT.BAUD_RATE));   
   uart_ptr->WRITE.UBG1 = (uchar)((baud >> 8) & 0xFF);
   uart_ptr->WRITE.UBG2 = (uchar)(baud & 0xFF);

   uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_TX_ENABLE |
                         MCF52XX_UART_UCR_RX_ENABLE;

   return( MQX_OK );

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_polled_deinit
* Returned Value   : MQX_OK or a mqx error code.
* Comments         :
*    This function de-initializes the UART.
*
*END*********************************************************************/

uint_32 _mcf52xx_uart_serial_polled_deinit
   (
      /* [IN] the initialization information for the device being opened */
      MCF52XX_UART_SERIAL_INIT_STRUCT_PTR io_init_ptr,

      /* [IN] the address of the device specific information */
      MCF52XX_UART_SERIAL_INFO_STRUCT_PTR io_info_ptr
   )
{ /* Body */

   _mem_free(io_info_ptr);

   return(MQX_OK);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_polled_getc
* Returned Value   : char
* Comments         :
*   Return a character when it is available.  This function polls the
* device for input.
*
*************************************************************************/

char _mcf52xx_uart_serial_polled_getc
   (
      /* [IN] the address of the device specific information */
      MCF52XX_UART_SERIAL_INFO_STRUCT_PTR io_info_ptr
   )
{ /* Body */
   volatile MCF52XX_UART_STRUCT _PTR_ uart_ptr = io_info_ptr->UART_PTR;

   /* Wait until a character is available */
   while ( (uart_ptr->READ.USR & MCF52XX_UART_USR_RXRDY) == 0 ) {
   } /* Endwhile */

   io_info_ptr->RX_CHARS++;

   return uart_ptr->READ.URB;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_polled_putc
* Returned Value   : void
* Comments         :
*   Writes the provided character.
*
*END*********************************************************************/

void _mcf52xx_uart_serial_polled_putc
   (
      /* [IN] the address of the device specific information */
      MCF52XX_UART_SERIAL_INFO_STRUCT_PTR io_info_ptr,

      /* [IN] the character to write */
      char                               c
   )
{ /* Body */
   volatile MCF52XX_UART_STRUCT _PTR_ uart_ptr = io_info_ptr->UART_PTR;

   /* Wait until free to send a character */
   while ( (uart_ptr->READ.USR & MCF52XX_UART_USR_TXRDY) == 0 ) {
   } /* Endwhile */

   uart_ptr->WRITE.UTB = c;

   io_info_ptr->TX_CHARS++;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_polled_status
* Returned Value   : boolean
* Comments         :
*            This function returns TRUE if a character is available
*            on the on I/O device, otherwise it returns FALSE.
*
*END*********************************************************************/

boolean _mcf52xx_uart_serial_polled_status
   (
      /* [IN] the address of the device specific information */
      MCF52XX_UART_SERIAL_INFO_STRUCT_PTR io_info_ptr
   )
{ /* Body */
   volatile MCF52XX_UART_STRUCT _PTR_ uart_ptr = io_info_ptr->UART_PTR;
   return (uart_ptr->READ.USR & MCF52XX_UART_USR_RXRDY) != 0;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _mcf52xx_uart_serial_polled_ioctl
* Returned Value   : uint_32 MQX_OK or a mqx error code.
* Comments         :
*    This function performs miscellaneous services for
*    the I/O device.
*
*END*********************************************************************/

uint_32 _mcf52xx_uart_serial_polled_ioctl
   (
      /* [IN] the address of the device specific information */
      MCF52XX_UART_SERIAL_INFO_STRUCT_PTR io_info_ptr,

      /* [IN] The command to perform */
      uint_32                    cmd,

      /* [IN] Parameters for the command */
      uint_32_ptr                param_ptr
   )
{ /* Body */
   volatile MCF52XX_UART_STRUCT _PTR_ uart_ptr = io_info_ptr->UART_PTR;
   uint_32                            baud,tmp;

   switch (cmd) {
      case IO_IOCTL_SERIAL_GET_STOP_BITS:
         tmp = io_info_ptr->INIT.UMR2_VALUE & MCF52XX_UART_UMR2_2_STOP_BITS;
         *param_ptr = (tmp == MCF52XX_UART_UMR2_1_STOP_BIT)
            ? MCF52XX_UART_UMR2_1_STOP_BIT
            : MCF52XX_UART_UMR2_2_STOP_BITS;
         break;
      case IO_IOCTL_SERIAL_SET_STOP_BITS:
         tmp = io_info_ptr->INIT.UMR2_VALUE;
         tmp &= ~MCF52XX_UART_UMR2_MASK_STOP_BITS;
         /* Tell user to change terminal settings */
         switch (*param_ptr) {
         case IO_SERIAL_STOP_BITS_1:
            tmp |= MCF52XX_UART_UMR2_1_STOP_BIT;
            break;
         case IO_SERIAL_STOP_BITS_2:
            tmp |= MCF52XX_UART_UMR2_2_STOP_BITS;
            break;
         default:
            return MQX_INVALID_PARAMETER;
         } /* Endswitch */
         io_info_ptr->INIT.UMR2_VALUE = tmp;
         uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_POINTER;
         uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR1_VALUE;
         uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR2_VALUE;
         break;
      case IO_IOCTL_SERIAL_GET_DATA_BITS:
         tmp = io_info_ptr->INIT.UMR1_VALUE & MCF52XX_UART_UMR1_MASK_DATA_BITS;
         *param_ptr = tmp + 5;
         break;
      case IO_IOCTL_SERIAL_SET_DATA_BITS:
         tmp = io_info_ptr->INIT.UMR1_VALUE & ~MCF52XX_UART_UMR1_MASK_DATA_BITS;
         tmp |=                          (*param_ptr == 5)
            ? MCF52XX_UART_UMR1_5_BITS : (*param_ptr == 6)
            ? MCF52XX_UART_UMR1_6_BITS : (*param_ptr == 7)
            ? MCF52XX_UART_UMR1_7_BITS : MCF52XX_UART_UMR1_8_BITS;
         io_info_ptr->INIT.UMR1_VALUE = tmp;
         uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_POINTER;
         uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR1_VALUE;
         uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR2_VALUE;
         break;
      case IO_IOCTL_SERIAL_GET_BAUD:
         *param_ptr = io_info_ptr->INIT.BAUD_RATE;
         break;
      case IO_IOCTL_SERIAL_SET_BAUD:
         io_info_ptr->INIT.BAUD_RATE = *param_ptr;
         /* baud = fsck / (32 * baudrate) + 0.5 */ 
         baud = (uint_32)((io_info_ptr->INIT.CLOCK_SPEED + 16 * io_info_ptr->INIT.BAUD_RATE )/ (32 * io_info_ptr->INIT.BAUD_RATE));
         uart_ptr->WRITE.UBG1 = (uchar)((baud >> 8) & 0xFF);
         uart_ptr->WRITE.UBG2 = (uchar)(baud & 0xFF);
         break;
      case IO_IOCTL_SERIAL_GET_STATS:
         *param_ptr++ = io_info_ptr->INTERRUPTS;
         *param_ptr++ = io_info_ptr->RX_CHARS;
         *param_ptr++ = io_info_ptr->TX_CHARS;
         *param_ptr++ = io_info_ptr->RX_BREAKS;
         *param_ptr++ = io_info_ptr->RX_PARITY_ERRORS;
         *param_ptr++ = io_info_ptr->RX_FRAMING_ERRORS;
         *param_ptr++ = io_info_ptr->RX_OVERRUNS;
         *param_ptr++ = io_info_ptr->RX_DROPPED_INPUT;
         break;
      case IO_IOCTL_SERIAL_CLEAR_STATS:
         io_info_ptr->INTERRUPTS = 0;
         io_info_ptr->RX_CHARS = 0;
         io_info_ptr->TX_CHARS = 0;
         io_info_ptr->RX_BREAKS = 0;
         io_info_ptr->RX_PARITY_ERRORS = 0;
         io_info_ptr->RX_FRAMING_ERRORS = 0;
         io_info_ptr->RX_OVERRUNS = 0;
         io_info_ptr->RX_DROPPED_INPUT = 0;
         break;
      case IO_IOCTL_SERIAL_CAN_TRANSMIT:
         *param_ptr = io_info_ptr->UART_PTR->READ.USR & MCF52XX_UART_USR_TXRDY ? 1 : 0;
         break;
      case IO_IOCTL_SERIAL_CAN_RECEIVE:
         *param_ptr = io_info_ptr->UART_PTR->READ.USR & MCF52XX_UART_USR_RXRDY ? 1 : 0;
         break;
      case IO_IOCTL_SERIAL_GET_PARITY:
         switch (io_info_ptr->INIT.UMR1_VALUE & MCF52XX_UART_UMR1_MD_ADDRESS) {
            case MCF52XX_UART_UMR1_EVEN_PARITY:
               tmp = IO_SERIAL_PARITY_EVEN;
               break;
            case MCF52XX_UART_UMR1_ODD_PARITY:
               tmp = IO_SERIAL_PARITY_ODD;
               break;
            case MCF52XX_UART_UMR1_LOW_PARITY:
               tmp = IO_SERIAL_PARITY_SPACE;
               break;
            case MCF52XX_UART_UMR1_HI_PARITY:
               tmp = IO_SERIAL_PARITY_MARK;
               break;
            default:
               tmp = IO_SERIAL_PARITY_NONE;
               break;
         }
         *param_ptr = tmp;
         break;
      case IO_IOCTL_SERIAL_SET_PARITY:
         tmp = io_info_ptr->INIT.UMR1_VALUE & ~MCF52XX_UART_UMR1_MD_ADDRESS;
         switch (*param_ptr) {
            case IO_SERIAL_PARITY_NONE:
               tmp |= MCF52XX_UART_UMR1_NO_PARITY;
               break;
            case IO_SERIAL_PARITY_ODD:
               tmp |= MCF52XX_UART_UMR1_ODD_PARITY;
               break;
            case IO_SERIAL_PARITY_EVEN:
               tmp |= MCF52XX_UART_UMR1_EVEN_PARITY;
               break;
            case IO_SERIAL_PARITY_MARK:
               tmp |= MCF52XX_UART_UMR1_HI_PARITY;
               break;
            case IO_SERIAL_PARITY_SPACE:
               tmp |= MCF52XX_UART_UMR1_LOW_PARITY;
               break;
            default:
               return MQX_INVALID_PARAMETER;
         }
         io_info_ptr->INIT.UMR1_VALUE = tmp;
         uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_POINTER;
         uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR1_VALUE;
         uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR2_VALUE;
         break;
      case IO_IOCTL_SERIAL_GET_HW_SIGNAL:
         tmp = 0;
         if (0 == (uart_ptr->READ.UIPCR & MCF52XX_UART_UIP_CTS)) {
            tmp |= IO_SERIAL_CTS;
         }
         if (io_info_ptr->CONTROL_BITS & MCF52XX_UART_UOP_RTS) {
            tmp |= IO_SERIAL_RTS;
         }
         *param_ptr = tmp;
         break;
      case IO_IOCTL_SERIAL_SET_HW_SIGNAL:  /* Assert */
         if (*param_ptr & IO_SERIAL_RTS) {
            if (io_info_ptr->INIT.UMR1_VALUE & MCF52XX_UART_UMR1_RXRTS) {
               return MQX_IO_OPERATION_NOT_AVAILABLE;
            }
            io_info_ptr->CONTROL_BITS |= MCF52XX_UART_UOP_RTS;
         }
         else {
            return MQX_INVALID_PARAMETER;
         }
         uart_ptr->WRITE.UOP1 = MCF52XX_UART_UOP_RTS;
         break;
      case IO_IOCTL_SERIAL_CLEAR_HW_SIGNAL:  /* Negate */
         if (*param_ptr & IO_SERIAL_RTS) {
            if (io_info_ptr->INIT.UMR1_VALUE & MCF52XX_UART_UMR1_RXRTS) {
               return MQX_IO_OPERATION_NOT_AVAILABLE;
            }
            io_info_ptr->CONTROL_BITS &= ~MCF52XX_UART_UOP_RTS;
         }
         else {
            return MQX_INVALID_PARAMETER;
         }
         uart_ptr->WRITE.UOP0 = MCF52XX_UART_UOP_RTS;
         break;         
      case IO_IOCTL_SERIAL_SET_FLAGS:
         /* automatic HW flow control */
    	 io_info_ptr->INIT.UMR1_VALUE &= (~MCF52XX_UART_UMR1_RXRTS);
    	 io_info_ptr->INIT.UMR2_VALUE &= (~MCF52XX_UART_UMR2_TXCTS);
    	 if (*param_ptr & IO_SERIAL_HW_FLOW_CONTROL) {
    		 io_info_ptr->INIT.UMR1_VALUE |= MCF52XX_UART_UMR1_RXRTS;
    		 io_info_ptr->INIT.UMR2_VALUE |= MCF52XX_UART_UMR2_TXCTS;
    	 }
    	 uart_ptr->WRITE.UCR = MCF52XX_UART_UCR_RESET_POINTER;
    	 uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR1_VALUE;
   	     uart_ptr->WRITE.UMR = io_info_ptr->INIT.UMR2_VALUE;
         break;
      case IO_IOCTL_SERIAL_WAIT_FOR_TC:
         while(!(uart_ptr->READ.USR & MCF52XX_UART_USR_TXEMP)) ;
         break;
      
      default:
         return IO_ERROR_INVALID_IOCTL_CMD;
   } /* Endswitch */

   return (MQX_OK);

} /* Endbody */

/* EOF */
