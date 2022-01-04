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
* $FileName: serl_int_kuart.c$
* $Version : 3.8.10.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains the low level functions for the interrupt driven
*   serial I/O for the SCI device.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "io_prv.h"
#include "charq.h"
#include "fio_prv.h"
#include "serinprv.h"

/* Polled functions used */
extern uint_32 _kuart_change_baudrate(UART_MemMapPtr, uint_32, uint_32);
extern uint_32 _kuart_polled_init(KUART_INIT_STRUCT_PTR, pointer _PTR_, char _PTR_);
extern uint_32 _kuart_polled_deinit(KUART_INIT_STRUCT_PTR, KUART_INFO_STRUCT_PTR);
extern uint_32 _kuart_polled_ioctl(KUART_INFO_STRUCT_PTR, uint_32, uint_32_ptr);

/* Interrupt driver functions */

extern void    _kuart_int_putc(IO_SERIAL_INT_DEVICE_STRUCT_PTR, char);
extern uint_32 _kuart_int_init(IO_SERIAL_INT_DEVICE_STRUCT_PTR, char _PTR_);
extern uint_32 _kuart_int_deinit(KUART_INIT_STRUCT_PTR, KUART_INFO_STRUCT_PTR);
extern uint_32 _kuart_int_enable(KUART_INFO_STRUCT_PTR);
extern void    _kuart_int_err_isr(pointer);
extern void    _kuart_int_rx_tx_isr(pointer);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_int_peripheral_enable
* Returned Value   : None
* Comments         :
*    Enables the SCI peripheral.
*
*END*----------------------------------------------------------------------*/

static void _kuart_int_peripheral_enable
    (
        /* [IN] SCI channel */
        UART_MemMapPtr sci_ptr
    )
{
    /* Enable only receive interrupt, transmit will be enabled during sending first character */
    sci_ptr->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_RIE_MASK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_int_peripheral_disable
* Returned Value   : None
* Comments         :
*    Disables the SCI peripheral.
*
*END*----------------------------------------------------------------------*/

static void _kuart_int_peripheral_disable
    (
        /* [IN] SCI channel */
        UART_MemMapPtr sci_ptr
    )
{
    /* Transmitter and receiver disable */
    sci_ptr->C2 &= (~ (UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_RIE_MASK | UART_C2_TIE_MASK));
}


#if MQX_ENABLE_LOW_POWER

extern LPM_NOTIFICATION_RESULT _io_serial_int_clock_configuration_callback (LPM_NOTIFICATION_STRUCT_PTR, pointer);
extern LPM_NOTIFICATION_RESULT _io_serial_int_operation_mode_callback (LPM_NOTIFICATION_STRUCT_PTR, pointer);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_serial_int_clock_configuration_callback
* Returned Value   : Notification error code
* Comments         :
*    Low power clock configuration callback for int serial.
*
*END*----------------------------------------------------------------------*/

LPM_NOTIFICATION_RESULT _io_serial_int_clock_configuration_callback
    (
        /* [IN] Low power notification */
        LPM_NOTIFICATION_STRUCT_PTR   notification,

        /* [IN/OUT] Device specific data */
        pointer                       device_specific_data
    )
{
    IO_SERIAL_INT_DEVICE_STRUCT_PTR dev_ptr = device_specific_data;
    KUART_INIT_STRUCT_PTR           init_data = dev_ptr->DEV_INIT_DATA_PTR;
    uint_8                          flags;

    /* Get the device registers */
    UART_MemMapPtr sci_ptr = _bsp_get_serial_base_address (init_data->DEVICE);
    if (NULL == sci_ptr)
    {
        return LPM_NOTIFICATION_RESULT_ERROR;
    }

    if (LPM_NOTIFICATION_TYPE_PRE == notification->NOTIFICATION_TYPE)
    {
        /* Lock access from the higher level */
        _lwsem_wait (&(dev_ptr->LPM_INFO.LOCK));

        /* Enable module clocks to be able to write registers */
        _bsp_serial_io_init (init_data->DEVICE, IO_PERIPHERAL_CLOCK_ENABLE);

        /* Flush output if enabled */
        if (sci_ptr->C2 & UART_C2_TE_MASK)
        {
            while (! (sci_ptr->SFIFO & UART_SFIFO_TXEMPT_MASK))
                { };
            while (! (sci_ptr->S1 & UART_S1_TC_MASK))
                { };
        }

        /* Turn off module */
        _kuart_int_peripheral_disable (sci_ptr);
    }

    if (LPM_NOTIFICATION_TYPE_POST == notification->NOTIFICATION_TYPE)
    {
        /* Enable module clocks to be able to write registers */
        _bsp_serial_io_init (init_data->DEVICE, IO_PERIPHERAL_CLOCK_ENABLE);

        /* Setup same baudrate for new clock frequency */
        _kuart_change_baudrate (sci_ptr, _cm_get_clock (notification->CLOCK_CONFIGURATION, init_data->CLOCK_SOURCE), init_data->BAUD_RATE);

        /* Turn on module if requested */
        flags = init_data->OPERATION_MODE[notification->OPERATION_MODE].FLAGS;
        if ((flags & IO_PERIPHERAL_MODULE_ENABLE) && (dev_ptr->COUNT > 0))
        {
            _kuart_int_peripheral_enable (sci_ptr);
        }

        /* Disable module clocks if required */
        if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
        {
            _bsp_serial_io_init (init_data->DEVICE, IO_PERIPHERAL_CLOCK_DISABLE);
        }

        /* Unlock */
        _lwsem_post (&(dev_ptr->LPM_INFO.LOCK));
    }
    return LPM_NOTIFICATION_RESULT_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_serial_int_operation_mode_callback
* Returned Value   : Notification error code
* Comments         :
*    Low power operation mode callback for int serial.
*
*END*----------------------------------------------------------------------*/

LPM_NOTIFICATION_RESULT _io_serial_int_operation_mode_callback
    (
        /* [IN] Low power notification */
        LPM_NOTIFICATION_STRUCT_PTR       notification,

        /* [IN/OUT] Device specific data */
        pointer                           device_specific_data
    )
{
    IO_SERIAL_INT_DEVICE_STRUCT_PTR     dev_ptr = device_specific_data;
    KUART_INIT_STRUCT_PTR               init_data = dev_ptr->DEV_INIT_DATA_PTR;
    uint_8                              flags, tmp, bits;

    if (LPM_NOTIFICATION_TYPE_PRE == notification->NOTIFICATION_TYPE)
    {
        /* Get the device registers */
        UART_MemMapPtr sci_ptr = _bsp_get_serial_base_address (init_data->DEVICE);
        if (NULL == sci_ptr)
        {
            return LPM_NOTIFICATION_RESULT_ERROR;
        }

        /* Lock access from the higher level */
        _lwsem_wait (&(dev_ptr->LPM_INFO.LOCK));

        /* Get required HW changes */
        flags = init_data->OPERATION_MODE[notification->OPERATION_MODE].FLAGS;

        /* Setup module clock to be able to write registers */
        _bsp_serial_io_init (init_data->DEVICE, IO_PERIPHERAL_CLOCK_ENABLE);

        /* Flush output if enabled */
        if (sci_ptr->C2 & UART_C2_TE_MASK)
        {
            while (! (sci_ptr->SFIFO & UART_SFIFO_TXEMPT_MASK))
                { };
            while (! (sci_ptr->S1 & UART_S1_TC_MASK))
                { };
        }

        /* Setup pin mux */
        _bsp_serial_io_init (init_data->DEVICE, flags & (~ IO_PERIPHERAL_CLOCK_DISABLE));

        /* Setup wakeup */
        if (flags & IO_PERIPHERAL_WAKEUP_ENABLE)
        {
            bits = init_data->OPERATION_MODE[notification->OPERATION_MODE].WAKEUP_BITS;
            tmp = sci_ptr->C1 & (~ (UART_C1_WAKE_MASK | UART_C1_ILT_MASK));
            sci_ptr->C1 = tmp | (bits & (UART_C1_WAKE_MASK | UART_C1_ILT_MASK));
            tmp = sci_ptr->C4 & (~ (UART_C4_MAEN1_MASK | UART_C4_MAEN2_MASK));
            sci_ptr->C4 = tmp | (bits & (UART_C4_MAEN1_MASK | UART_C4_MAEN2_MASK));
            sci_ptr->MA1 = init_data->OPERATION_MODE[notification->OPERATION_MODE].MA1;
            sci_ptr->MA2 = init_data->OPERATION_MODE[notification->OPERATION_MODE].MA2;
            tmp = sci_ptr->C2 & (~ (UART_C2_RWU_MASK));
            sci_ptr->C2 = tmp | (bits & UART_C2_RWU_MASK);
            dev_ptr->LPM_INFO.FLAGS = (flags & (IO_PERIPHERAL_WAKEUP_ENABLE | IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE));
        }
        else
        {
            sci_ptr->C2 &= (~ (UART_C2_RWU_MASK));
            sci_ptr->C1 &= (~ (UART_C1_WAKE_MASK | UART_C1_ILT_MASK));
            sci_ptr->C4 &= (~ (UART_C4_MAEN1_MASK | UART_C4_MAEN2_MASK));
            sci_ptr->MA1 = 0;
            sci_ptr->MA2 = 0;
            dev_ptr->LPM_INFO.FLAGS = 0;
        }

        /* Enable/disable module according to  operation mode */
        if ((flags & IO_PERIPHERAL_MODULE_ENABLE) && (dev_ptr->COUNT > 0))
        {
            _kuart_int_peripheral_enable (sci_ptr);
        }
        else
        {
            _kuart_int_peripheral_disable (sci_ptr);
        }

        /* Disable module clocks if required */
        if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
        {
            _bsp_serial_io_init (init_data->DEVICE, IO_PERIPHERAL_CLOCK_DISABLE);
        }

        /* Unlock */
        _lwsem_post (&(dev_ptr->LPM_INFO.LOCK));
    }
    return LPM_NOTIFICATION_RESULT_OK;
}

#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _kuart_int_install
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    Install an interrupt driven uart serial device.
*
*END*----------------------------------------------------------------------*/

uint_32 _kuart_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr identifier,

      /* [IN] The I/O init data pointer */
      KUART_INIT_STRUCT_CPTR  init_data_ptr,

      /* [IN] The I/O queue size to use */
      uint_32  queue_size
   )
{ /* Body */

#if PE_LDD_VERSION
    if (PE_PeripheralUsed((uint_32)_bsp_get_serial_base_address(init_data_ptr->DEVICE)))
    {
        return IO_ERROR;
    }
#endif

   return _io_serial_int_install(identifier,
      (uint_32 (_CODE_PTR_)(pointer, char _PTR_))_kuart_int_init,
      (uint_32 (_CODE_PTR_)(pointer))_kuart_int_enable,
      (uint_32 (_CODE_PTR_)(pointer,pointer))_kuart_int_deinit,
      (void    (_CODE_PTR_)(pointer, char))_kuart_int_putc,
      (uint_32 (_CODE_PTR_)(pointer, uint_32, pointer))_kuart_polled_ioctl,
      (pointer)init_data_ptr, queue_size);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_init
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    This function initializes the SCI in interrupt mode.
*
*END*********************************************************************/

uint_32 _kuart_int_init
   (
      /* [IN] the interrupt I/O initialization information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the rest of the name of the device opened */
      char _PTR_                      open_name_ptr
   )
{ /* Body */
   KUART_INFO_STRUCT_PTR sci_info_ptr;
   KUART_INIT_STRUCT_PTR sci_init_ptr;
   uint_32                     result;

   sci_init_ptr = int_io_dev_ptr->DEV_INIT_DATA_PTR;
   result = _kuart_polled_init((pointer)sci_init_ptr, &int_io_dev_ptr->DEV_INFO_PTR, open_name_ptr);
   if (result != MQX_OK) {
       return(result);
   }/* Endif */

   sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;

   sci_info_ptr->OLD_ISR_DATA = _int_get_isr_data(sci_init_ptr->RX_TX_VECTOR);
   sci_info_ptr->OLD_ISR_EXCEPTION_HANDLER = _int_get_exception_handler(sci_init_ptr->RX_TX_VECTOR);


#if PSP_MQX_CPU_IS_KINETIS
   /* Init RX error interrupt vector */
   sci_info_ptr->OLD_ISR =
     _int_install_isr(sci_init_ptr->ERR_VECTOR, _kuart_int_err_isr, int_io_dev_ptr);
    _bsp_int_init(sci_init_ptr->ERR_VECTOR, sci_init_ptr->ERR_PRIORITY, 0, TRUE);
#endif

   /* Init RX/TX interrupt vector */
   sci_info_ptr->OLD_ISR =
    _int_install_isr(sci_init_ptr->RX_TX_VECTOR, _kuart_int_rx_tx_isr, int_io_dev_ptr);
    _bsp_int_init(sci_init_ptr->RX_TX_VECTOR, sci_init_ptr->RX_TX_PRIORITY, 0, TRUE);
   return(MQX_OK);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_deinit
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    This function de-initializes the UART in interrupt mode.
*
*END*********************************************************************/

uint_32 _kuart_int_deinit
   (
      /* [IN] the interrupt I/O initialization information */
      KUART_INIT_STRUCT_PTR io_init_ptr,

      /* [IN] the address of the device specific information */
      KUART_INFO_STRUCT_PTR io_info_ptr
   )
{ /* Body */
   UART_MemMapPtr sci_ptr = io_info_ptr->SCI_PTR;

   _kuart_polled_deinit(io_init_ptr, io_info_ptr);

   _int_install_isr(io_init_ptr->RX_TX_VECTOR, io_info_ptr->OLD_ISR, io_info_ptr->OLD_ISR_DATA);

   return(MQX_OK);

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_enable
* Returned Value   : uint_32 a task error code or MQX_OK
* Comments         :
*    This function enables the UART interrupts mode.
*
*END*********************************************************************/

uint_32 _kuart_int_enable
   (
      /* [IN] the address of the device specific information */
      KUART_INFO_STRUCT_PTR io_info_ptr
   )
{ /* Body */
   uint_8                  flags = IO_PERIPHERAL_MODULE_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE;
   UART_MemMapPtr          sci_ptr = io_info_ptr->SCI_PTR;

#if MQX_ENABLE_LOW_POWER

   uint_32 clock, mode = _lpm_get_operation_mode ();
   if (mode >= LPM_OPERATION_MODES)
   {
      return MQX_INVALID_CONFIGURATION;
   }
   flags = io_info_ptr->INIT.OPERATION_MODE[mode].FLAGS;

   clock = _lpm_get_clock_configuration ();
   if (clock >= BSP_CLOCK_CONFIGURATIONS)
   {
      return MQX_INVALID_CONFIGURATION;
   }
#endif

   /* Enable module clocks to be able to write registers */
   _bsp_serial_io_init (io_info_ptr->INIT.DEVICE, IO_PERIPHERAL_CLOCK_ENABLE);

   /* Enable/disable module */
   if (flags & IO_PERIPHERAL_MODULE_ENABLE)
   {
      _kuart_int_peripheral_enable (sci_ptr);
   }
   else
   {
      _kuart_int_peripheral_disable (sci_ptr);
   }

   /* Disable module clocks if required */
   if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
   {
      _bsp_serial_io_init (io_info_ptr->INIT.DEVICE, IO_PERIPHERAL_CLOCK_DISABLE);
   }

   return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_err_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial error interrupts.
*
*************************************************************************/

void _kuart_int_err_isr
   (
      /* [IN] the address of the device specific information */
      pointer parameter
   )
{ /* Body */

   IO_SERIAL_INT_DEVICE_STRUCT_PTR        int_io_dev_ptr = parameter;
   KUART_INFO_STRUCT_PTR                  sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   UART_MemMapPtr                         sci_ptr = sci_info_ptr->SCI_PTR;
   uint_16                                stat = sci_ptr->S1;

#if MQX_ENABLE_LOW_POWER

    if (int_io_dev_ptr->LPM_INFO.FLAGS & IO_PERIPHERAL_WAKEUP_ENABLE)
    {
        /* Disable wakeup settings */
        int_io_dev_ptr->LPM_INFO.FLAGS &= (~ IO_PERIPHERAL_WAKEUP_ENABLE);
        sci_ptr->C2 &= (~ (UART_C2_RWU_MASK));
        sci_ptr->C1 &= (~ (UART_C1_WAKE_MASK | UART_C1_ILT_MASK));
        sci_ptr->C4 &= (~ (UART_C4_MAEN1_MASK | UART_C4_MAEN2_MASK));
        sci_ptr->MA1 = 0;
        sci_ptr->MA2 = 0;

        /* Do not return to CPU sleep anymore */
        if (int_io_dev_ptr->LPM_INFO.FLAGS & IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE)
        {
            int_io_dev_ptr->LPM_INFO.FLAGS &= (~ IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE);
            _lpm_wakeup_core ();
        }
    }

#endif

   ++sci_info_ptr->INTERRUPTS;

   if(stat & UART_S1_OR_MASK) {
      ++sci_info_ptr->RX_OVERRUNS;
   }
   if(stat & UART_S1_PF_MASK) {
      ++sci_info_ptr->RX_PARITY_ERRORS;
   }
   if(stat & UART_S1_NF_MASK) {
      ++sci_info_ptr->RX_NOISE_ERRORS;
   }
   if(stat & UART_S1_FE_MASK) {
      ++sci_info_ptr->RX_FRAMING_ERRORS;
   }

   /*
   if "framming error" or "overrun" error occours 
   perform 'S1' cleanup. if not, 'S1' cleanup will be 
   performed during regular reading of 'D' register.
   */
   if (stat & (UART_S1_OR_MASK | UART_S1_FE_MASK))
   {
      // reading register 'D' to cleanup 'S1' may cause 'RFIFO' underflow
      sci_ptr->D;
      // if 'RFIFO' underflow detected, perform flush to reinitialize 'RFIFO'
      if (sci_ptr->SFIFO & UART_SFIFO_RXUF_MASK)
      {
         sci_ptr->CFIFO |= UART_CFIFO_RXFLUSH_MASK;
         sci_ptr->SFIFO |= UART_SFIFO_RXUF_MASK;
      }
      // set errno. transmit is corrupted, nothing to process
      // TODO: add valid errno. IO_ERROR causes signed/unsigned assignment warning
      // _task_set_error(IO_ERROR);
   }

}  /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_rx_tx_isr
* Returned Value   : none
* Comments         :
*   interrupt handler for the serial input interrupts.
*
*************************************************************************/

void _kuart_int_rx_tx_isr
   (
      /* [IN] the address of the device specific information */
      pointer parameter
   )
{ /* Body */
   IO_SERIAL_INT_DEVICE_STRUCT_PTR        int_io_dev_ptr = parameter;
   KUART_INFO_STRUCT_PTR                  sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   UART_MemMapPtr                         sci_ptr = sci_info_ptr->SCI_PTR;
   volatile int_32                        c;
   uint_16                                stat = sci_ptr->S1;

#if MQX_ENABLE_LOW_POWER

    if (int_io_dev_ptr->LPM_INFO.FLAGS & IO_PERIPHERAL_WAKEUP_ENABLE)
    {
        /* Disable wakeup settings */
        int_io_dev_ptr->LPM_INFO.FLAGS &= (~ IO_PERIPHERAL_WAKEUP_ENABLE);
        sci_ptr->C2 &= (~ (UART_C2_RWU_MASK));
        sci_ptr->C1 &= (~ (UART_C1_WAKE_MASK | UART_C1_ILT_MASK));
        sci_ptr->C4 &= (~ (UART_C4_MAEN1_MASK | UART_C4_MAEN2_MASK));
        sci_ptr->MA1 = 0;
        sci_ptr->MA2 = 0;

        /* Do not return to CPU sleep anymore */
        if (int_io_dev_ptr->LPM_INFO.FLAGS & IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE)
        {
            int_io_dev_ptr->LPM_INFO.FLAGS &= (~ IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE);
            _lpm_wakeup_core ();
        }
    }

#endif

   ++sci_info_ptr->INTERRUPTS;

   /* 
   process error flags in case:
   - there is single interrupt line for both data and errors
   - _kuart_int_err_isr has lower priority than _kuart_int_rx_tx_isr
   - error interrupts are not enabled
   */
   if(stat & UART_S1_OR_MASK) {
      ++sci_info_ptr->RX_OVERRUNS;
   }
   if(stat & UART_S1_PF_MASK) {
      ++sci_info_ptr->RX_PARITY_ERRORS;
   }
   if(stat & UART_S1_NF_MASK) {
      ++sci_info_ptr->RX_NOISE_ERRORS;
   }
   if(stat & UART_S1_FE_MASK) {
      ++sci_info_ptr->RX_FRAMING_ERRORS;
   }

   /*
   if "framming error" or "overrun" error occours 
   perform 'S1' cleanup. if not, 'S1' cleanup will be 
   performed during regular reading of 'D' register.
   */
   if (stat & (UART_S1_OR_MASK | UART_S1_FE_MASK))
   {
      // reading register 'D' to cleanup 'S1' may cause 'RFIFO' underflow
      sci_ptr->D;
      // if 'RFIFO' underflow detected, perform flush to reinitialize 'RFIFO'
      if (sci_ptr->SFIFO & UART_SFIFO_RXUF_MASK)
      {
         sci_ptr->CFIFO |= UART_CFIFO_RXFLUSH_MASK;
         sci_ptr->SFIFO |= UART_SFIFO_RXUF_MASK;
      }
      // set errno. transmit is corrupted, nothing to process
      // TODO: add valid errno. IO_ERROR causes signed/unsigned assignment warning
      // _task_set_error(IO_ERROR);
      return;
   }

   /*
   try if RX buffer has some characters.
   */
   if (stat & UART_S1_RDRF_MASK) {
      // reading 'D' register performs also cleanup of 'S1'
      c = sci_ptr->D;
      if (!_io_serial_int_addc(int_io_dev_ptr, c)) {
          sci_info_ptr->RX_DROPPED_INPUT++;
      }
      sci_info_ptr->RX_CHARS++;
   }

   /* try if TX buffer is still not full */
   if (stat & UART_S1_TDRE_MASK) {
      c = _io_serial_int_nextc(int_io_dev_ptr);
      if (c >= 0) {
          sci_ptr->D = c;
      } else {
         /* All data sent, disable transmit interrupt */
         sci_ptr->C2 &= ~UART_C2_TIE_MASK;
      }
      sci_info_ptr->TX_CHARS++;
   }

}  /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kuart_int_putc
* Returned Value   : none
* Comments         :
*   This function is called to write out the first character, when
* the output serial device and output ring buffers are empty.
*
*END*********************************************************************/

void _kuart_int_putc
   (
      /* [IN] the address of the device specific information */
      IO_SERIAL_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] the character to write out now */
      char                       c
   )
{ /* Body */
   KUART_INFO_STRUCT_PTR                  sci_info_ptr;
   UART_MemMapPtr                         sci_ptr;

   sci_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   sci_ptr = sci_info_ptr->SCI_PTR;

   while (!(sci_ptr->S1 & UART_S1_TDRE_MASK)) {
      /* Wait while buffer is full */
   } /* Endwhile */

   sci_ptr->D = c;
   sci_info_ptr->TX_CHARS++;

   /* Enable transmit iterrupt */
   sci_ptr->C2 |= UART_C2_TIE_MASK;

} /* Endbody */

/* EOF */
