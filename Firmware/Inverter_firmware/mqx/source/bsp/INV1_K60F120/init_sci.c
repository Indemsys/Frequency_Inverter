/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: init_sci.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definition for the baud rate for the serial
*   channel
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"



#if MQX_ENABLE_LOW_POWER

const KUART_OPERATION_MODE_STRUCT _bsp_sci0_operation_modes[LPM_OPERATION_MODES] =
{
    /* LPM_OPERATION_MODE_RUN */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },
    /* LPM_OPERATION_MODE_WAIT */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },
    /* LPM_OPERATION_MODE_SLEEP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },
    /* LPM_OPERATION_MODE_STOP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    }
};

const KUART_OPERATION_MODE_STRUCT _bsp_sci1_operation_modes[LPM_OPERATION_MODES] =
{
    /* LPM_OPERATION_MODE_RUN */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },
    /* LPM_OPERATION_MODE_WAIT */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },
    /* LPM_OPERATION_MODE_SLEEP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },
    /* LPM_OPERATION_MODE_STOP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    }
};

const KUART_OPERATION_MODE_STRUCT _bsp_sci2_operation_modes[LPM_OPERATION_MODES] =
{
    /* LPM_OPERATION_MODE_RUN */
    {
        IO_PERIPHERAL_PIN_MUX_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE | IO_PERIPHERAL_MODULE_ENABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_WAIT */
    {
        IO_PERIPHERAL_PIN_MUX_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE | IO_PERIPHERAL_MODULE_ENABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_SLEEP */
    {
        IO_PERIPHERAL_PIN_MUX_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE | IO_PERIPHERAL_MODULE_ENABLE | IO_PERIPHERAL_WAKEUP_ENABLE | IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_STOP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    }
};

const KUART_OPERATION_MODE_STRUCT _bsp_sci3_operation_modes[LPM_OPERATION_MODES] =
{
    /* LPM_OPERATION_MODE_RUN */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_WAIT */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_SLEEP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_STOP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    }
};


const KUART_OPERATION_MODE_STRUCT _bsp_sci4_operation_modes[LPM_OPERATION_MODES] =
{
    /* LPM_OPERATION_MODE_RUN */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_WAIT */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_SLEEP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_STOP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    }
};

const KUART_OPERATION_MODE_STRUCT _bsp_sci5_operation_modes[LPM_OPERATION_MODES] =
{
    /* LPM_OPERATION_MODE_RUN */
    {
        IO_PERIPHERAL_PIN_MUX_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE | IO_PERIPHERAL_MODULE_ENABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_WAIT */
    {
        IO_PERIPHERAL_PIN_MUX_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE | IO_PERIPHERAL_MODULE_ENABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_SLEEP */
    {
        IO_PERIPHERAL_PIN_MUX_ENABLE | IO_PERIPHERAL_CLOCK_ENABLE | IO_PERIPHERAL_MODULE_ENABLE | IO_PERIPHERAL_WAKEUP_ENABLE | IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE,
        0,
        0,
        0
    },

    /* LPM_OPERATION_MODE_STOP */
    {
        IO_PERIPHERAL_PIN_MUX_DISABLE | IO_PERIPHERAL_CLOCK_DISABLE,
        0,
        0,
        0
    }
};

#endif



const KUART_INIT_STRUCT _bsp_sci0_init = {
   /* queue size         */ BSPCFG_SCI0_QUEUE_SIZE,
   /* Channel            */ 0,
   /* Clock Speed        */ BSP_SYSTEM_CLOCK,       /* SCI0 operates only on system clock */
   /* Baud rate          */ BSPCFG_SCI0_BAUD_RATE,
   /* RX/TX Int vect     */ INT_UART0_RX_TX,
   /* ERR Int vect       */ INT_UART0_ERR,
   /* RX/TX priority     */ 3,
   /* ERR priority       */ 4
#if MQX_ENABLE_LOW_POWER
   ,
   /* Clock source       */ CM_CLOCK_SOURCE_CORE,
   /* LPM operation info */ _bsp_sci0_operation_modes
#endif
};

const KUART_INIT_STRUCT _bsp_sci1_init = {
   /* queue size         */ BSPCFG_SCI1_QUEUE_SIZE,
   /* Channel            */ 1,
   /* Clock Speed        */ BSP_SYSTEM_CLOCK,   	/* SCI1 operates only on system clock */
   /* Baud rate          */ BSPCFG_SCI1_BAUD_RATE,
   /* RX/TX Int vect     */ INT_UART1_RX_TX,
   /* ERR Int vect       */ INT_UART1_ERR,
   /* RX/TX priority     */ 3,
   /* ERR priority       */ 4
#if MQX_ENABLE_LOW_POWER
   ,
   /* Clock source       */ CM_CLOCK_SOURCE_CORE,
   /* LPM operation info */ _bsp_sci1_operation_modes
#endif
};

const KUART_INIT_STRUCT _bsp_sci2_init = {
   /* queue size         */ BSPCFG_SCI2_QUEUE_SIZE,
   /* Channel            */ 2,
   /* Clock Speed        */ BSP_BUS_CLOCK,       	/* SCI2 operates only on bus clock */
   /* Baud rate          */ BSPCFG_SCI2_BAUD_RATE,
   /* RX/TX Int vect     */ INT_UART2_RX_TX,
   /* ERR Int vect       */ INT_UART2_ERR,
   /* RX/TX priority     */ 3,
   /* ERR priority       */ 4
#if MQX_ENABLE_LOW_POWER
   ,
   /* Clock source       */ CM_CLOCK_SOURCE_BUS,
   /* LPM operation info */ _bsp_sci2_operation_modes
#endif
};

const KUART_INIT_STRUCT _bsp_sci3_init = {
   /* queue size         */ BSPCFG_SCI3_QUEUE_SIZE,
   /* Channel            */ 3,
   /* Clock Speed        */ BSP_BUS_CLOCK,       	/* SCI3 operates only on bus clock */
   /* Baud rate          */ BSPCFG_SCI3_BAUD_RATE,
   /* RX/TX Int vect     */ INT_UART3_RX_TX,
   /* ERR Int vect       */ INT_UART3_ERR,
   /* RX/TX priority     */ 3,
   /* ERR priority       */ 4
#if MQX_ENABLE_LOW_POWER
   ,
   /* Clock source       */ CM_CLOCK_SOURCE_BUS,
   /* LPM operation info */ _bsp_sci3_operation_modes
#endif
};

const KUART_INIT_STRUCT _bsp_sci4_init = {
   /* queue size         */ BSPCFG_SCI4_QUEUE_SIZE,
   /* Channel            */ 4,
   /* Clock Speed        */ BSP_BUS_CLOCK,       	/* SCI4 operates only on bus clock */
   /* Baud rate          */ BSPCFG_SCI4_BAUD_RATE,
   /* RX/TX Int vect     */ INT_UART4_RX_TX,
   /* ERR Int vect       */ INT_UART4_ERR,
   /* RX/TX priority     */ 3,
   /* ERR priority       */ 4
#if MQX_ENABLE_LOW_POWER
   ,
   /* Clock source       */ CM_CLOCK_SOURCE_BUS,
   /* LPM operation info */ _bsp_sci4_operation_modes
#endif
};

const KUART_INIT_STRUCT _bsp_sci5_init = {
   /* queue size         */ BSPCFG_SCI5_QUEUE_SIZE,
   /* Channel            */ 5,
   /* Clock Speed        */ BSP_BUS_CLOCK,       	/* SCI5 operates only on bus clock */
   /* Baud rate          */ BSPCFG_SCI5_BAUD_RATE,
   /* RX/TX Int vect     */ INT_UART5_RX_TX,
   /* ERR Int vect       */ INT_UART5_ERR,
   /* RX/TX priority     */ 3,
   /* ERR priority       */ 4
#if MQX_ENABLE_LOW_POWER
   ,
   /* Clock source       */ CM_CLOCK_SOURCE_BUS,
   /* LPM operation info */ _bsp_sci5_operation_modes
#endif
};

/* EOF */
