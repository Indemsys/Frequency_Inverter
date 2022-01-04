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
* $FileName: serl_kuart.h$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the sci drivers for the MCF51XX
*
*END************************************************************************/
#ifndef _serial_kuart_h_
#define _serial_kuart_h_


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

#if MQX_ENABLE_LOW_POWER

/*
** KUART_CLOCK_CONFIGURATION_STRUCT
**
** This structure defines the behavior of KUART in specific
** low power clock configuration.
*/
typedef struct kuart_clock_configuration_struct
{
    /* Whether channel is enabled in particular clock configuration */
    uint_8      ENABLED;

} KUART_CLOCK_CONFIGURATION_STRUCT, _PTR_ KUART_CLOCK_CONFIGURATION_STRUCT_PTR;
typedef const KUART_CLOCK_CONFIGURATION_STRUCT _PTR_ KUART_CLOCK_CONFIGURATION_STRUCT_CPTR;

/*
** KUART_OPERATION_MODE_STRUCT
**
** This structure defines the behavior of KUART in specific
** low power operation mode.
*/
typedef struct kuart_operation_mode_struct
{
    /* HW/wakeup enable/disable flags */
    uint_8      FLAGS;

    /* Wakeup register bits combination: UART_C2_RWU_MASK, UART_C1_WAKE_MASK, UART_C1_ILT_MASK, UART_C4_MAEN1_MASK, UART_C4_MAEN2_MASK */
    uint_8      WAKEUP_BITS;

    /* Wakeup settings of register UART_MA1 */
    uint_8      MA1;

    /* Wakeup settings of register UART_MA2 */
    uint_8      MA2;

} KUART_OPERATION_MODE_STRUCT, _PTR_ KUART_OPERATION_MODE_STRUCT_PTR;
typedef const KUART_OPERATION_MODE_STRUCT _PTR_ KUART_OPERATION_MODE_STRUCT_CPTR;

/*
**
** IO SERIAL LPM STRUCT
**
** This structure is used to store information regarding low power functionality.
**
*/
typedef struct io_serial_lpm_struct
{
   /* Low power manager registration handle */
   _mqx_uint           REGISTRATION_HANDLE;

   /* Low level device lock */
   LWSEM_STRUCT        LOCK;

   /* Special flags */
   uint_8              FLAGS;

} IO_SERIAL_LPM_STRUCT, _PTR_ IO_SERIAL_LPM_STRUCT_PTR;

#endif

/*
** KUART_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct kuart_init_struct
{

   /* The size of the queues to buffer incoming/outgoing data */
   uint_32 QUEUE_SIZE;

   /* The device to initialize */
   uint_32 DEVICE;

   /* The clock speed of cpu */
   uint_32 CLOCK_SPEED;

   /* The baud rate for the channel */
   uint_32 BAUD_RATE;

   /* RX / TX interrupt vector */
   uint_32 RX_TX_VECTOR;

   /* ERR interrupt vector */
   uint_32 ERR_VECTOR;

   /* RX / TX interrupt vector priority */
   uint_32 RX_TX_PRIORITY;

   /* ERR interrupt vector priority */
   uint_32 ERR_PRIORITY;

#if MQX_ENABLE_LOW_POWER
   /* Clock source */
   CM_CLOCK_SOURCE                          CLOCK_SOURCE;

   /* Low power operation mode */
   KUART_OPERATION_MODE_STRUCT_CPTR         OPERATION_MODE;
#endif

} KUART_INIT_STRUCT, _PTR_ KUART_INIT_STRUCT_PTR;
typedef const KUART_INIT_STRUCT _PTR_ KUART_INIT_STRUCT_CPTR;

/*
** KUART_INFO_STRUCT
** Run time state information for each serial channel
*/
typedef struct kuart_info_struct
{
   /* The current init values for this port */
   KUART_INIT_STRUCT          INIT;

   /* The sci device register */
   UART_MemMapPtr             SCI_PTR;

   /* The previous interrupt handler and data */
   void       (_CODE_PTR_ OLD_ISR)(pointer);
   void       (_CODE_PTR_ OLD_ISR_EXCEPTION_HANDLER)(uint_32, uint_32, pointer,
               pointer);
   pointer                           OLD_ISR_DATA;

   /* Various flags */
   uint_32                           FLAGS;
   
   /* Statistical information */
   uint_32                           INTERRUPTS;
   uint_32                           RX_CHARS;
   uint_32                           TX_CHARS;
   uint_32                           RX_BREAKS;
   uint_32                           RX_PARITY_ERRORS;
   uint_32                           RX_FRAMING_ERRORS;
   uint_32                           RX_OVERRUNS;
   uint_32                           RX_DROPPED_INPUT;
   uint_32                           RX_NOISE_ERRORS;
} KUART_INFO_STRUCT, _PTR_ KUART_INFO_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 _kuart_polled_init(KUART_INIT_STRUCT_PTR, pointer _PTR_,char _PTR_);
extern uint_32 _kuart_polled_install(char_ptr, KUART_INIT_STRUCT_CPTR, uint_32);
extern uint_32 _kuart_int_install(char_ptr, KUART_INIT_STRUCT_CPTR, uint_32);
extern pointer _bsp_get_serial_base_address(uint_8);

#ifdef __cplusplus
}
#endif

#endif //_serial_kuart_h_

/* EOF */
