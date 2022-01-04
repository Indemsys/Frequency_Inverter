/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
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
* $FileName: serl_pol_linflexd.c$
* $Version : 3.8.7.0$
* $Date    : Sep-26-2012$
*
* Comments:
*
*   This file contains the driver functions for the LINFLEXD
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "io_prv.h"
#include "charq.h"
#include "fio_prv.h"
#include "serplprv.h"

#include "serl_linflexd.h"


/* Polled driver functions */
extern _mqx_uint _linflexd_serial_polled_init(LINFLEXD_SERIAL_INIT_STRUCT_CPTR, pointer _PTR_, char_ptr);
extern _mqx_uint _linflexd_serial_polled_deinit(LINFLEXD_SERIAL_INIT_STRUCT_PTR, LINFLEXD_SERIAL_INFO_STRUCT_PTR );
static char      _linflexd_serial_polled_getc(LINFLEXD_SERIAL_INFO_STRUCT_PTR);
static void      _linflexd_serial_polled_putc(LINFLEXD_SERIAL_INFO_STRUCT_PTR, char);
static boolean   _linflexd_serial_polled_status(LINFLEXD_SERIAL_INFO_STRUCT_PTR);
extern _mqx_uint _linflexd_serial_polled_ioctl(LINFLEXD_SERIAL_INFO_STRUCT_PTR, _mqx_uint, pointer);
extern uint_32   _linflexd_uart_serial_int_enable(LINFLEXD_SERIAL_INFO_STRUCT_PTR io_info_ptr);


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _linflexd_serial_polled_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a polled linflexd_ptr serial device.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _linflexd_serial_polled_install
    (
        /* [IN] A string that identifies the device for fopen */
        char_ptr           identifier,

        /* [IN] The I/O init data pointer */
        LINFLEXD_SERIAL_INIT_STRUCT_CPTR  init_data_ptr
    )
{
    return _io_serial_polled_install(identifier,
        (_mqx_uint (_CODE_PTR_)(pointer, pointer _PTR_, char _PTR_)) _linflexd_serial_polled_init,
        (_mqx_uint (_CODE_PTR_)(pointer, pointer))  _linflexd_serial_polled_deinit,
        (char (_CODE_PTR_)(pointer)) _linflexd_serial_polled_getc,
        (void (_CODE_PTR_)(pointer, char)) _linflexd_serial_polled_putc,
        (boolean (_CODE_PTR_)(pointer)) _linflexd_serial_polled_status,
        (_mqx_uint (_CODE_PTR_)(pointer, _mqx_uint, pointer)) _linflexd_serial_polled_ioctl,
        (pointer)init_data_ptr, init_data_ptr->QUEUE_SIZE);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _linflexd_serial_reconfigure
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*
*
*END*----------------------------------------------------------------------*/

static uint_32 _linflexd_serial_reconfigure
    (
        LINFLEXD_SERIAL_INFO_STRUCT_PTR io_info_ptr
    )
{
    VLINFLEXD_REG_STRUCT_PTR linflexd_ptr;
    uint_32 uartcr;
    uint_32 lfdiv;
    uint_32 linier;


    if (io_info_ptr->INIT.STOP_BITS != IO_SERIAL_STOP_BITS_1) {
        return MQX_INVALID_PARAMETER;
    }

    uartcr = LINFLEXD_UARTCR_UART;
    
#if !defined(PSP_MPXD10) /*PXD10 doesn't support*/
    if (io_info_ptr->CONTROL_BITS & LINFLEXD_FIFO_MODE) {
         // Use FIFOs in polled mode, buffers in interrupt mode
         // Setup in fifo mode with max fifo size.
         uartcr |=  LINFLEXD_UARTCR_TFBM |  LINFLEXD_UARTCR_RFBM;
    }
#endif    

    switch (io_info_ptr->INIT.PARITY) {
        case IO_SERIAL_PARITY_EVEN:
            uartcr |= LINFLEXD_UARTCR_PCE;
            break;
        case IO_SERIAL_PARITY_ODD:
#if defined(PSP_MPXD10)
            uartcr |= LINFLEXD_UARTCR_PCE | LINFLEXD_UARTCR_OP;
#else
            uartcr |= LINFLEXD_UARTCR_PCE | LINFLEXD_UARTCR_PC0;            
#endif            
            break;
            
#if defined(PSP_MPXD10)
            /* MPC5606S does not support IO_SERIAL_PARITY_SPACE & IO_SERIAL_PARITY_MARK */
#else
            
        case IO_SERIAL_PARITY_SPACE:
            uartcr |= LINFLEXD_UARTCR_PCE | LINFLEXD_UARTCR_PC1;
            break;

        case IO_SERIAL_PARITY_MARK:
            uartcr |= LINFLEXD_UARTCR_PCE | LINFLEXD_UARTCR_PC1 | LINFLEXD_UARTCR_PC0;
            break;
#endif                        
        case IO_SERIAL_PARITY_NONE:
            if (io_info_ptr->INIT.BITS_PER_CHARACTER==7) {
                return MQX_INVALID_PARAMETER;
            }
            break;

        default:
            return MQX_INVALID_PARAMETER;
            break;
    }

    switch (io_info_ptr->INIT.BITS_PER_CHARACTER) {
        case 7:
            break;
        case 8:
            uartcr |= LINFLEXD_UARTCR_WL0;
            break;
        default:
            return MQX_INVALID_PARAMETER;
            break;
    }

    // TODO: Turn on linflexd clock if required..

    linflexd_ptr = (VLINFLEXD_REG_STRUCT_PTR)io_info_ptr->LINFLEXD_PTR;

    /* Save interrupt enable state - reconfigure may be done from polled or interrupt mode driver */
    linier = linflexd_ptr->LINIER;

    /* Go into init mode */
    linflexd_ptr->LINCR1 = LINFLEXD_LINCR1_INIT;
    linflexd_ptr->LINCR1 = LINFLEXD_LINCR1_INIT;

    /* Select UART mode and set parameters */
    linflexd_ptr->UARTCR = LINFLEXD_UARTCR_UART;
    linflexd_ptr->UARTCR = uartcr;

    /* Calculate baud rate */
    lfdiv =  (io_info_ptr->INIT.CLOCK_SPEED*2/io_info_ptr->INIT.BAUD_RATE + 1)/2;
    linflexd_ptr->LINFBRR = lfdiv & 0xf;
    linflexd_ptr->LINIBRR = lfdiv>>4;

    /* Disable interrupts */
    linflexd_ptr->LINIER = 0;

    /* Enable Tx and Rx */
    uartcr |= LINFLEXD_UARTCR_RXEN | LINFLEXD_UARTCR_TXEN;
    linflexd_ptr->UARTCR = uartcr;

    /* Leave init mode */
    linflexd_ptr->LINCR1 &= ~LINFLEXD_LINCR1_INIT;

    /* Restore interrupt enable state */
    linflexd_ptr->LINIER = linier;

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _linflexd_serial_init
* Returned Value   : MQX_OK or a MQX error code.
* Comments         :
*    This function initializes LINFlexD in UART mode
*
*END*********************************************************************/

uint_32 _linflexd_serial_init
    (
        /* [IN] the initialization information for the device being opened */
        LINFLEXD_SERIAL_INIT_STRUCT_CPTR    io_init_ptr,

        /* [OUT] the address to store device specific information */
        pointer _PTR_                       io_info_ptr_ptr,

        /* [IN] the rest of the name of the device opened */
        char _PTR_                          open_name_ptr,

        /* [IN] the uart mode to use */
        uint_32                             mode
    )
{
    VLINFLEXD_REG_STRUCT_PTR         linflexd_ptr;
    LINFLEXD_SERIAL_INFO_STRUCT_PTR  io_info_ptr;
    uint_32                          channel;
    uint_32                          result;

    /* Get peripheral address */
    channel = io_init_ptr->DEVICE;

    #if PSP_HAS_DEVICE_PROTECTION
    if (!_bsp_linflexd_enable_access(channel)) {
        return MQX_INVALID_IO_CHANNEL;
    }
    #endif

    linflexd_ptr = _bsp_get_linflexd_base_address(channel);
    if (linflexd_ptr == NULL) {
        return MQX_INVALID_IO_CHANNEL;
    }

    io_info_ptr = (LINFLEXD_SERIAL_INFO_STRUCT_PTR)_mem_alloc_system_zero(
        (_mem_size)sizeof(LINFLEXD_SERIAL_INFO_STRUCT));

    #if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (io_info_ptr == NULL) {
        return MQX_OUT_OF_MEMORY;
    }
    #endif

    /* Save initialization values and base address */
    io_info_ptr->INIT = *io_init_ptr;

    /* Polled driver uses fifo mode, int driver uses buffered mode */
    io_info_ptr->CONTROL_BITS = mode;

    /* Store pointer to linflexd registers */
    io_info_ptr->LINFLEXD_PTR = linflexd_ptr;

    /* Disable interrupts*/
    linflexd_ptr->LINIER = 0;

    /* init GPIO and clock GATING */
    _bsp_serial_io_init(channel);

    result = _linflexd_serial_reconfigure(io_info_ptr);
    if (result!=MQX_OK) {
        _mem_free(io_info_ptr);
        io_info_ptr = NULL;
    }
    *io_info_ptr_ptr = io_info_ptr;

    return result;
} 

/*FUNCTION****************************************************************
*
* Function Name    : _linflexd_serial_polled_init
* Returned Value   : MQX_OK or a MQX error code.
* Comments         :
*    This function initializes LINFlexD in UART mode
*
*END*********************************************************************/

uint_32 _linflexd_serial_polled_init
    (
        /* [IN] the initialization information for the device being opened */
        LINFLEXD_SERIAL_INIT_STRUCT_CPTR    io_init_ptr,

        /* [OUT] the address to store device specific information */
        pointer _PTR_                       io_info_ptr_ptr,

        /* [IN] the rest of the name of the device opened */
        char _PTR_                          open_name_ptr
    )
{
    return _linflexd_serial_init(io_init_ptr, io_info_ptr_ptr, open_name_ptr, LINFLEXD_FIFO_MODE);
}


/*FUNCTION****************************************************************
*
* Function Name    : _linflexd_serial_polled_deinit
* Returned Value   : MQX_OK or a mqx error code.
* Comments         :
*    This function de-initializes the UART
*
*END*********************************************************************/

_mqx_uint _linflexd_serial_polled_deinit
    (
        /* [IN] the initialization information for the device being opened */
        LINFLEXD_SERIAL_INIT_STRUCT_PTR io_init_ptr,

        /* [IN] the address of the device specific information */
        LINFLEXD_SERIAL_INFO_STRUCT_PTR io_info_ptr
    )
{
    _mem_free((pointer)io_info_ptr);

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _linflexd_serial_polled_getc
* Returned Value   : char
* Comments         :
*   Return a character when it is available.
*   This function polls the device for input.
*
*************************************************************************/

static char _linflexd_serial_polled_getc
    (
        /* [IN] the address of the device specific information */
        LINFLEXD_SERIAL_INFO_STRUCT_PTR io_info_ptr
    )
{
    VLINFLEXD_REG_STRUCT_PTR linflexd_ptr = io_info_ptr->LINFLEXD_PTR;
    uchar data;
#if defined(PSP_MPXD10)
    while ((linflexd_ptr->UARTSR & LINFLEXD_UARTSR_DRFRFE) == 0)
         ;
    linflexd_ptr->UARTSR = LINFLEXD_UARTSR_DRFRFE;    
#else
    while ((linflexd_ptr->UARTCR&LINFLEXD_UARTCR_RDFLRFC_MASK) == 0)
         ; /* Wait until there is a character available */
#endif		 

    data = linflexd_ptr->BDRM[3];

    io_info_ptr->STATS.RX_CHARS++;
 
    return (char)data;
}


/*FUNCTION****************************************************************
*
* Function Name    : _linflexd_serial_polled_putc
* Returned Value   : void
* Comments         :
*   Writes the provided character.
*
*END*********************************************************************/

static void _linflexd_serial_polled_putc
    (
        /* [IN] the address of the device specific information */
        LINFLEXD_SERIAL_INFO_STRUCT_PTR io_info_ptr,

        /* [IN] the character to write */
        char                           c
    )
{
    VLINFLEXD_REG_STRUCT_PTR linflexd_ptr = io_info_ptr->LINFLEXD_PTR;
#ifndef PSP_MPXD10
    while (((linflexd_ptr->UARTCR&LINFLEXD_UARTCR_TDFLTFC_MASK)>>LINFLEXD_UARTCR_TDFLTFC_SHIFT) >= 4)
      ; /* Wait until there is a room for a character (less than 4 FIFO positions occupied) */
    linflexd_ptr->BDRL[3] = c;
#else
     /* Clear DTF bit first*/
    linflexd_ptr->UARTSR = LINFLEXD_UARTSR_DTFTFF;    
    linflexd_ptr->BDRL[3] = c;
    
    /* Wait until data is transmitted */
    while (!(linflexd_ptr->UARTSR & LINFLEXD_UARTSR_DTFTFF))
                     	;
#endif


   io_info_ptr->STATS.TX_CHARS++;   
        
}


/*FUNCTION****************************************************************
*
* Function Name    : _linflexd_serial_polled_status
* Returned Value   : boolean
* Comments         :
*            This function returns TRUE if a character is available
*            on the on I/O device, otherwise it returns FALSE.
*
*END*********************************************************************/

static boolean _linflexd_serial_polled_status
    (
        /* [IN] the address of the device specific information */
        LINFLEXD_SERIAL_INFO_STRUCT_PTR io_info_ptr
    )
{
    VLINFLEXD_REG_STRUCT_PTR linflexd_ptr = io_info_ptr->LINFLEXD_PTR;
#ifndef PSP_MPXD10
    if ((linflexd_ptr->UARTCR&LINFLEXD_UARTCR_RDFLRFC_MASK)!=0)
#else
    if ( linflexd_ptr->UARTSR & LINFLEXD_UARTSR_DRFRFE )
#endif	
         return TRUE;
    else
         return FALSE;
}


/*FUNCTION****************************************************************
*
* Function Name    : _linflexd_serial_polled_ioctl
* Returned Value   : uint_32 MQX_OK or a mqx error code.
* Comments         :
*    This function performs miscellaneous services for
*    the I/O device.
*
*END*********************************************************************/

uint_32 _linflexd_serial_polled_ioctl
    (
        /* [IN] the address of the device specific information */
        LINFLEXD_SERIAL_INFO_STRUCT_PTR io_info_ptr,

        /* [IN] The command to perform */
        _mqx_uint                      cmd,

        /* [IN] Parameters for the command */
        pointer                        param_ptr
    )
{
    uint_32_ptr u32_ptr = (uint_32_ptr)param_ptr;

    switch (cmd) {

    case IO_IOCTL_SERIAL_GET_STOP_BITS:
        *u32_ptr = io_info_ptr->INIT.STOP_BITS;
        return MQX_OK;

    case IO_IOCTL_SERIAL_SET_STOP_BITS:
        if (*u32_ptr != IO_SERIAL_STOP_BITS_1)
            return MQX_INVALID_PARAMETER;

    case IO_IOCTL_SERIAL_GET_DATA_BITS:
        *u32_ptr = io_info_ptr->INIT.BITS_PER_CHARACTER;
        break;

    case IO_IOCTL_SERIAL_SET_DATA_BITS:
        io_info_ptr->INIT.BITS_PER_CHARACTER = *u32_ptr;
        return _linflexd_serial_reconfigure(io_info_ptr);

    case IO_IOCTL_SERIAL_GET_BAUD:
        *u32_ptr = io_info_ptr->INIT.BAUD_RATE;
        break;

    case IO_IOCTL_SERIAL_SET_BAUD:
        io_info_ptr->INIT.BAUD_RATE = *u32_ptr;
        return _linflexd_serial_reconfigure(io_info_ptr);

    case IO_IOCTL_SERIAL_GET_STATS:
        {
            LINFLEXD_STATS_STRUCT_PTR stats_ptr = (LINFLEXD_STATS_STRUCT_PTR)param_ptr;
            *stats_ptr = io_info_ptr->STATS;
        }
        break;

    case IO_IOCTL_SERIAL_CLEAR_STATS:
        _mem_zero((pointer) &io_info_ptr->STATS, sizeof(io_info_ptr->STATS));
        break;

    case IO_IOCTL_SERIAL_GET_CONFIG:
        {
            LINFLEXD_SERIAL_INIT_STRUCT_PTR io_init_ptr = (LINFLEXD_SERIAL_INIT_STRUCT_PTR)param_ptr;
            *io_init_ptr = io_info_ptr->INIT;
        }
        break;

    case IO_IOCTL_SERIAL_GET_PARITY:
        *(uint_32_ptr)param_ptr = io_info_ptr->INIT.PARITY;
        break;
        
    case IO_IOCTL_SERIAL_SET_PARITY:
        switch (*(uint_32_ptr)param_ptr) {
            case IO_SERIAL_PARITY_NONE:
            case IO_SERIAL_PARITY_ODD:
            case IO_SERIAL_PARITY_EVEN:
            case IO_SERIAL_PARITY_MARK:
            case IO_SERIAL_PARITY_SPACE:
                break;
            default:
                return MQX_INVALID_PARAMETER;
        }
        io_info_ptr->INIT.PARITY = *(uint_32_ptr)param_ptr;
        return _linflexd_serial_reconfigure(io_info_ptr);

    default:
        return IO_ERROR_INVALID_IOCTL_CMD;
    }

    return MQX_OK;
}
