/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: spi_pol_mcf5xxx_qspi.c$
* $Version : 3.8.22.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   The file contains low level SPI driver functions for 51xx Coldfire family.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include "spi.h"
#include "spi_pol_prv.h"
#include "spi_mcf5xxx_qspi_prv.h"

uint_32 _mcf5xxx_qspi_polled_init(MCF5XXX_QSPI_INIT_STRUCT_PTR, pointer _PTR_, char_ptr);
uint_32 _mcf5xxx_qspi_polled_ioctl(MCF5XXX_QSPI_INFO_STRUCT_PTR, uint_32, uint_32_ptr, uint_32);

static uint_32 _mcf5xxx_qspi_polled_deinit(IO_SPI_POLLED_DEVICE_STRUCT_PTR, MCF5XXX_QSPI_INFO_STRUCT_PTR);
static uint_32 _mcf5xxx_qspi_polled_rx(IO_SPI_POLLED_DEVICE_STRUCT_PTR, uchar_ptr, uint_32);
static uint_32 _mcf5xxx_qspi_polled_tx(IO_SPI_POLLED_DEVICE_STRUCT_PTR, uchar_ptr, uint_32);
static uint_32 _mcf5xxx_qspi_polled_tx_rx(MCF5XXX_QSPI_INFO_STRUCT_PTR, uint_8_ptr, uint_8_ptr, uint_32, uint_16);

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_find_baudrate
* Returned Value   : uint_8 divider register setting
* Comments         :
*    Find closest setting of divider register for given baudrate.
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_find_baudrate 
    (
        /* [IN] Module input clock in Hz */
        uint_32 clock, 
        
        /* [IN] Desired baudrate in Hz */
        uint_32 baudrate
    ) 
{
    uint_32 div = 0;
    
    if (0 != baudrate)
    {
        div = (clock + baudrate) / (baudrate << 1);
        if (div < 2) div = 2;
        if (div > 255) div = 255;
    }
    return MCF5XXX_QSPI_QMR_BAUD(div);
}
    
/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_polled_install
* Returned Value   : MQX error code
* Comments         :
*    Install an SPI device.
*
*END*********************************************************************/
uint_32 _mcf5xxx_qspi_polled_install
    (
        /* [IN] A string that identifies the device for fopen */
        char_ptr           identifier,
        
        /* [IN] The I/O init data pointer */
        MCF5XXX_QSPI_INIT_STRUCT_CPTR   init_data_ptr
    )
{ 
    return _io_spi_polled_install(identifier,
    (uint_32 (_CODE_PTR_)(pointer, pointer _PTR_, char_ptr))_mcf5xxx_qspi_polled_init,
    (uint_32 (_CODE_PTR_)(pointer, pointer))_mcf5xxx_qspi_polled_deinit,
    (_mqx_int (_CODE_PTR_)(pointer, char_ptr, int_32))_mcf5xxx_qspi_polled_rx,
    (_mqx_int (_CODE_PTR_)(pointer, char_ptr, int_32))_mcf5xxx_qspi_polled_tx,
    (_mqx_int (_CODE_PTR_)(pointer, uint_32, _mqx_uint_ptr, uint_32))_mcf5xxx_qspi_polled_ioctl, 
    (pointer)init_data_ptr);
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_polled_init  
* Returned Value   : MQX error code
* Comments         :
*    This function initializes the SPI module 
*
*END*********************************************************************/
uint_32 _mcf5xxx_qspi_polled_init
    (
        /* [IN] The initialization information for the device being opened */
        MCF5XXX_QSPI_INIT_STRUCT_PTR  qspi_init_ptr,
        
        /* [OUT] The address to store device specific information */
        pointer _PTR_                 io_info_ptr_ptr,  
        
        /* [IN] The rest of the name of the device opened */
        char_ptr                      open_name_ptr
    )
{
    VMCF5XXX_QSPI_STRUCT_PTR          qspi_ptr;
    MCF5XXX_QSPI_INFO_STRUCT_PTR      io_info_ptr;
    uint_32                           i;
    
    /* Check channel */
    qspi_ptr = _bsp_get_qspi_base_address (qspi_init_ptr->CHANNEL);
    if (NULL == qspi_ptr)
    {
        return SPI_ERROR_CHANNEL_INVALID;
    }

    if (_bsp_qspi_io_init (qspi_init_ptr->CHANNEL) == -1)
    {
        return SPI_ERROR_CHANNEL_INVALID;
    }

    /* Disable and clear SPI */
    qspi_ptr->QIR = MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;
    qspi_ptr->QWR = MCF5XXX_QSPI_QWR_HALT | MCF5XXX_QSPI_QWR_CSIV | MCF5XXX_QSPI_QWR_NEWQP(0) | MCF5XXX_QSPI_QWR_ENDQP(0);
    while (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE) 
       {};
    qspi_ptr->QIR = MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;
    
    /* Initialize internal data */
    io_info_ptr = (MCF5XXX_QSPI_INFO_STRUCT_PTR)_mem_alloc_system_zero((uint_32)sizeof(MCF5XXX_QSPI_INFO_STRUCT));
    if (io_info_ptr == NULL) 
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (io_info_ptr,MEM_TYPE_IO_SPI_INFO_STRUCT);
    
    *io_info_ptr_ptr = io_info_ptr;
    
    io_info_ptr->QSPI_PTR           = qspi_ptr;
    io_info_ptr->INIT               = *qspi_init_ptr;
    io_info_ptr->RECEIVING          = 0;
    io_info_ptr->ONTHEWAY           = 0;
    io_info_ptr->RX_REQUEST         = 0;
    io_info_ptr->RX_BUFFER          = NULL;
    io_info_ptr->RX_IN              = 0;
    io_info_ptr->RX_OUT             = 0;
    io_info_ptr->TX_BUFFER          = NULL;
    io_info_ptr->TX_IN              = 0;
    io_info_ptr->TX_OUT             = 0;
    io_info_ptr->STATS.INTERRUPTS   = 0;
    io_info_ptr->STATS.RX_PACKETS   = 0;
    io_info_ptr->STATS.RX_OVERFLOWS = 0;
    io_info_ptr->STATS.TX_PACKETS   = 0;
    io_info_ptr->STATS.TX_ABORTS    = 0;
    io_info_ptr->STATS.TX_UNDERFLOWS= 0;
    io_info_ptr->CS                 = qspi_init_ptr->CS;
    io_info_ptr->CS_ACTIVE          = 0;
    io_info_ptr->CSIV_ACTIVE        = TRUE;
    for (i = 0; i < MCF5XXX_QSPI_CS_COUNT; i++)
    {
        io_info_ptr->CS_CALLBACK[i] = NULL;
        io_info_ptr->CS_USERDATA[i] = NULL;
    }
    
    /* Set the SPI clock baud rate divider */
    qspi_ptr->QMR = _mcf5xxx_qspi_find_baudrate (qspi_init_ptr->CLOCK_SPEED, qspi_init_ptr->BAUD_RATE) | MCF5XXX_QSPI_QMR_BITS_SET(8);

    /* Set up SPI clock polarity and phase */
    switch (qspi_init_ptr->CLOCK_POL_PHASE)
    {
        case (SPI_CLK_POL_PHA_MODE0):
            /* Inactive state of SPI_CLK = logic 0 */
            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPOL);
            /* SPI_CLK transitions middle of bit timing */
            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPHA);
            break;
        case (SPI_CLK_POL_PHA_MODE1):
            /* Inactive state of SPI_CLK = logic 0 */
            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPOL);
            /* SPI_CLK transitions begining of bit timing */
            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPHA;
            break;
        case (SPI_CLK_POL_PHA_MODE2):
            /* Inactive state of SPI_CLK = logic 1 */
            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPOL;
            /* SPI_CLK transitions middle of bit timing */
            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPHA);
            break;
        case (SPI_CLK_POL_PHA_MODE3):
            /* Inactive state of SPI_CLK = logic 1 */
            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPOL;
            /* SPI_CLK transitions begining of bit timing */
            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPHA;
            break;
        default:
            return SPI_ERROR_MODE_INVALID;       
    }
    /* Set transfer mode */
    if (qspi_init_ptr->TRANSFER_MODE == SPI_DEVICE_MASTER_MODE)
    {
        qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_MSTR;
    }
    else
    {
        _mem_free (*io_info_ptr_ptr);
        *io_info_ptr_ptr = NULL;
        return SPI_ERROR_TRANSFER_MODE_INVALID;
    }

    /* Enable SPI (clear HALT) */
    qspi_ptr->QWR &= (~ MCF5XXX_QSPI_QWR_HALT);
    
    return SPI_OK;
}
    
/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_polled_deinit  
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes the SPI module 
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_polled_deinit
    (
        /* [IN] the initialization information for the device being opened */
        IO_SPI_POLLED_DEVICE_STRUCT_PTR io_dev_ptr,
        
        /* [IN] the address of the device specific information */
        MCF5XXX_QSPI_INFO_STRUCT_PTR    io_info_ptr
    )
{
    VMCF5XXX_QSPI_STRUCT_PTR            qspi_ptr;
    uint_32                             index;
    
    if ((NULL == io_info_ptr) || (NULL == io_dev_ptr)) 
    {
        return SPI_ERROR_DEINIT_FAILED;
    }
    
    /* Disable and clear SPI */
    qspi_ptr = io_info_ptr->QSPI_PTR;
    qspi_ptr->QIR = MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;
    qspi_ptr->QWR |= MCF5XXX_QSPI_QWR_HALT | MCF5XXX_QSPI_QWR_CSIV;
    while (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE) 
       {};
    qspi_ptr->QIR = MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;

    /* Disable all chip selects */
    for (index = 0; index < MCF5XXX_QSPI_CS_COUNT; index++)
    {
        if ((NULL != io_info_ptr->CS_CALLBACK[index]) && (0 != (io_info_ptr->CS_ACTIVE & (1 << index))))
        {
            io_info_ptr->CS_CALLBACK[index] (1 << index, 1, io_info_ptr->CS_USERDATA[index]);
        }
    }
    io_info_ptr->CS_ACTIVE = 0;

    /* Release buffers */
    _mem_free(io_dev_ptr->DEV_INFO_PTR);
    io_dev_ptr->DEV_INFO_PTR = NULL;
    return SPI_OK;
}
    
/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_polled_ioctl
* Returned Value   : MQX error code
* Comments         : 
*    This function performs miscellaneous services for
*    the SPI I/O device.  
*
*END*********************************************************************/
uint_32 _mcf5xxx_qspi_polled_ioctl
    (
        /* [IN] The address of the device specific information */
        MCF5XXX_QSPI_INFO_STRUCT_PTR io_info_ptr,
        
        /* [IN] The command to perform */
        uint_32                      cmd,
        
        /* [IN] Parameters for the command */
        uint_32_ptr                  param_ptr,

        /* [IN] Opening flags */
        uint_32                      flags
    )
{
    VMCF5XXX_QSPI_STRUCT_PTR         qspi_ptr;
    uint_32                          val, num, size, command;
    uint_32                          result = SPI_OK;
    SPI_READ_WRITE_STRUCT_PTR        rw_ptr;
    uchar_ptr                        input, output;
    SPI_CS_CALLBACK_STRUCT_PTR       callbacks;
    
    qspi_ptr = io_info_ptr->QSPI_PTR;
    
    switch (cmd) 
    {
        case IO_IOCTL_SPI_GET_BAUD:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                val = MCF5XXX_QSPI_QMR_BAUD(qspi_ptr->QMR) << 1;
                if (0 == val) *param_ptr = val; 
                else *param_ptr = (uint_32)((io_info_ptr->INIT.CLOCK_SPEED) / val);
            }
            break;
        case IO_IOCTL_SPI_SET_BAUD:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else if (0 == (*param_ptr)) 
            {
                result = SPI_ERROR_BAUD_RATE_INVALID;
            } 
            else 
            {
                if (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE)
                {
                    result = IO_ERROR_DEVICE_BUSY;
                }
                else
                {
                    /* Set the frequency divider */
                    val = _mcf5xxx_qspi_find_baudrate (io_info_ptr->INIT.CLOCK_SPEED, *param_ptr);
                    qspi_ptr->QMR = (qspi_ptr->QMR & (~ (MCF5XXX_QSPI_QMR_BAUD(0xFF)))) | MCF5XXX_QSPI_QMR_BAUD(val);
                }
            }
            break;
        case IO_IOCTL_SPI_DEVICE_DISABLE:
            qspi_ptr->QIR |= MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;
            qspi_ptr->QWR |= MCF5XXX_QSPI_QWR_HALT;
            while (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE) 
               {};
            qspi_ptr->QIR |= MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;
            break;
        case IO_IOCTL_SPI_DEVICE_ENABLE:
            qspi_ptr->QWR &= (~ MCF5XXX_QSPI_QWR_HALT);
            break;
        case IO_IOCTL_SPI_GET_MODE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else
            {
                if (qspi_ptr->QMR & MCF5XXX_QSPI_QMR_CPOL)
                {
                    if (qspi_ptr->QMR & MCF5XXX_QSPI_QMR_CPHA)
                    {
                        *param_ptr = SPI_CLK_POL_PHA_MODE3;
                    }
                    else
                    {
                        *param_ptr = SPI_CLK_POL_PHA_MODE2;
                    }
                }
                else
                {
                    if (qspi_ptr->QMR & MCF5XXX_QSPI_QMR_CPHA)
                    {
                        *param_ptr = SPI_CLK_POL_PHA_MODE1;
                    }
                    else
                    {
                        *param_ptr = SPI_CLK_POL_PHA_MODE0;
                    }
                }
            }
            break;
        case IO_IOCTL_SPI_SET_MODE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else
            {       
                if (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE)
                {
                    result = IO_ERROR_DEVICE_BUSY;
                }
                else
                {
                    /* Set up SPI clock polarity and phase */
                    switch (*param_ptr)
                    {
                        case (SPI_CLK_POL_PHA_MODE0):
                            /* Inactive state of SPI_CLK = logic 0 */
                            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPOL);
                            /* SPI_CLK transitions middle of bit timing */
                            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPHA);
                            break;
                        case (SPI_CLK_POL_PHA_MODE1):
                            /* Inactive state of SPI_CLK = logic 0 */
                            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPOL);
                            /* SPI_CLK transitions begining of bit timing */
                            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPHA;
                            break;
                        case (SPI_CLK_POL_PHA_MODE2):
                            /* Inactive state of SPI_CLK = logic 1 */
                            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPOL;
                            /* SPI_CLK transitions middle of bit timing */
                            qspi_ptr->QMR &= (~ MCF5XXX_QSPI_QMR_CPHA);
                            break;
                        case (SPI_CLK_POL_PHA_MODE3):
                            /* Inactive state of SPI_CLK = logic 1 */
                            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPOL;
                            /* SPI_CLK transitions begining of bit timing */
                            qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_CPHA;
                            break;
                        default:
                            return SPI_ERROR_MODE_INVALID;       
                    }
                }
            }
            break;
        case IO_IOCTL_SPI_GET_TRANSFER_MODE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else
            {
                *param_ptr = SPI_DEVICE_MASTER_MODE;
            }
            break;
        case IO_IOCTL_SPI_SET_TRANSFER_MODE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else
            {
                if (*param_ptr == SPI_DEVICE_MASTER_MODE)
                {
                    qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_MSTR;
                }
                else
                {
                    result = SPI_ERROR_TRANSFER_MODE_INVALID;
                }
            }
            break;
        case IO_IOCTL_SPI_GET_ENDIAN:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            *param_ptr = SPI_DEVICE_BIG_ENDIAN; 
            break;
        case IO_IOCTL_SPI_SET_ENDIAN:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else if (*param_ptr != SPI_DEVICE_BIG_ENDIAN)
            {
                result = SPI_ERROR_ENDIAN_INVALID;
            }
            break;
        case IO_IOCTL_SPI_GET_STATS:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                *((SPI_STATISTICS_STRUCT_PTR)param_ptr) = io_info_ptr->STATS;
            }
            break;
        case IO_IOCTL_SPI_CLEAR_STATS:
            io_info_ptr->STATS.INTERRUPTS   = 0;
            io_info_ptr->STATS.RX_PACKETS   = 0;
            io_info_ptr->STATS.RX_OVERFLOWS = 0;
            io_info_ptr->STATS.TX_PACKETS   = 0;
            io_info_ptr->STATS.TX_ABORTS    = 0;
            io_info_ptr->STATS.TX_UNDERFLOWS= 0;
            break;
        case IO_IOCTL_FLUSH_OUTPUT:
        case IO_IOCTL_SPI_FLUSH_DEASSERT_CS:
            while ((0 != io_info_ptr->RECEIVING) || (0 != io_info_ptr->ONTHEWAY) || (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE)) 
               {};  /* Waiting for completion */
            if ((0 == (flags & SPI_FLAG_NO_DEASSERT_ON_FLUSH)) || (IO_IOCTL_SPI_FLUSH_DEASSERT_CS == cmd))
            {
                /* Deassert all chip selects */
                for (num = 0; num < MCF5XXX_QSPI_CS_COUNT; num++)
                {
                    if ((NULL != io_info_ptr->CS_CALLBACK[num]) && (0 != (io_info_ptr->CS_ACTIVE & (1 << num))))
                    {
                        io_info_ptr->CS_CALLBACK[num] (1 << num, 1, io_info_ptr->CS_USERDATA[num]);
                    }                
                }
                io_info_ptr->CS_ACTIVE = 0;
                qspi_ptr->QWR |= MCF5XXX_QSPI_QWR_CSIV;
            }
            break;
        case IO_IOCTL_SPI_GET_FRAMESIZE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                *param_ptr = MCF5XXX_QSPI_QMR_BITS_GET(qspi_ptr->QMR);
                if (0 == *param_ptr) *param_ptr = 16;
            }
            break;
        case IO_IOCTL_SPI_SET_FRAMESIZE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                if (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE)
                {
                    result = IO_ERROR_DEVICE_BUSY;
                }
                else
                {
                    qspi_ptr->QMR &= (~ (MCF5XXX_QSPI_QMR_BITS_SET(0x0F)));
                    qspi_ptr->QMR |= MCF5XXX_QSPI_QMR_BITS_SET(*param_ptr);
                }
            }
            break;
        case IO_IOCTL_SPI_GET_CS:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                *param_ptr = io_info_ptr->CS;
            }
            break;
        case IO_IOCTL_SPI_SET_CS:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                io_info_ptr->CS = *param_ptr;
            }
            break;
        case IO_IOCTL_SPI_ENABLE_MODF:
            result = MQX_IO_OPERATION_NOT_AVAILABLE;
            break;
        case IO_IOCTL_SPI_DISABLE_MODF:
            break;
        case IO_IOCTL_SPI_SET_CS_CALLBACK:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                callbacks = (SPI_CS_CALLBACK_STRUCT_PTR)(param_ptr);
                for (num = 0; num < MCF5XXX_QSPI_CS_COUNT; num++)
                {
                    if (0 != (callbacks->MASK & (1 << num)))
                    {
                        io_info_ptr->CS_CALLBACK[num] = callbacks->CALLBACK;
                        io_info_ptr->CS_USERDATA[num] = callbacks->USERDATA;
                    }                
                }
            }
            break;
        case IO_IOCTL_SPI_READ_WRITE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                rw_ptr = (SPI_READ_WRITE_STRUCT_PTR)param_ptr;
                command = MCF5XXX_QSPI_QDR_CONT | MCF5XXX_QSPI_QDR_BITSE | MCF5XXX_QSPI_QDR_CS(~ io_info_ptr->CS) | MCF5XXX_QSPI_QDR_TX | MCF5XXX_QSPI_QDR_RX;
                size = rw_ptr->BUFFER_LENGTH;
                input = (uchar_ptr)rw_ptr->WRITE_BUFFER;
                output = (uchar_ptr)rw_ptr->READ_BUFFER;
                for (num = 0; size > 0; )
                {
                    val = _mcf5xxx_qspi_polled_tx_rx (io_info_ptr, input, output, size, command);
                    if (0 == val) break;
                    num += val;
                    input += val;
                    output += val;
                    size -= val;
                }
            }
            break;
        case IO_IOCTL_SPI_KEEP_QSPI_CS_ACTIVE:
            if (NULL == param_ptr) 
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            } 
            else 
            {
                if (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE)
                {
                    result = IO_ERROR_DEVICE_BUSY;
                }
                else
                {
                    io_info_ptr->CSIV_ACTIVE = ((*param_ptr) != 0);
                }
            }
            break;
        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }
    return result;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_polled_tx_rx
* Returned Value   : byte received   
* Comments         : 
*   Actual transmit and receive function.
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_polled_tx_rx 
    (
        /* [IN] The address of the device registers */
        MCF5XXX_QSPI_INFO_STRUCT_PTR io_info_ptr,
        
        /* [IN] Input buffer */
        uint_8_ptr                   buffer_in,

        /* [OUT] Output buffer */
        uint_8_ptr                   buffer_out,

        /* [IN] Buffer length */
        uint_32                      size,

        /* [IN] Command */
        uint_16                      command
    )
{
    uint_16                          data;
    uint_32                          num, bytes;
    VMCF5XXX_QSPI_STRUCT_PTR         qspi_ptr = io_info_ptr->QSPI_PTR;
    uint_16                          tmp;

    /* Set transfer size */
    bytes = 1;
    if (MCF5XXX_QSPI_QMR_BITS_GET(qspi_ptr->QMR) != 8) bytes++;
    if (size > (16 << (bytes - 1))) size = (16 << (bytes - 1));
    if (bytes > size) return 0;

    /* Assert actual chip select */ 
    if (io_info_ptr->CS ^ io_info_ptr->CS_ACTIVE)
    {
        for (num = 0; num < MCF5XXX_QSPI_CS_COUNT; num++)
        {
            if ((0 != ((io_info_ptr->CS ^ io_info_ptr->CS_ACTIVE) & (1 << num))) && (NULL != io_info_ptr->CS_CALLBACK[num]))
            {
                io_info_ptr->CS_CALLBACK[num] (1 << num, (io_info_ptr->CS_ACTIVE >> num) & 1, io_info_ptr->CS_USERDATA[num]);
            }
        }
        io_info_ptr->CS_ACTIVE = io_info_ptr->CS;
    }

    /* Write commands */
    qspi_ptr->QAR = 0x0020;
    data = command & 0xFF00;
    for (num = 0; num < size; num += bytes)
    {
        qspi_ptr->QDR = data;
    }

    /* Write data */
    qspi_ptr->QAR = 0x0000;
    if (bytes > 1)
    {
        for (num = 0; num < size; num += bytes)
        {
            data = *buffer_in++;
            data = (data << 8) | *buffer_in++;
            qspi_ptr->QDR = data;
        }
        data = (size >> 1);
    }
    else
    {
        for (num = 0; num < size; num += bytes)
        {
            data = *buffer_in++;
            qspi_ptr->QDR = data;
        }
        data = size;
    }

    /* Setup queue */
    tmp = qspi_ptr->QWR & (~ (MCF5XXX_QSPI_QWR_NEWQP(0xF) | MCF5XXX_QSPI_QWR_ENDQP(0xF)));
    qspi_ptr->QWR = tmp | MCF5XXX_QSPI_QWR_NEWQP(0) | MCF5XXX_QSPI_QWR_ENDQP(data - 1);

    /* Initiate transmission */
    qspi_ptr->QDLYR |= MCF5XXX_QSPI_QDLYR_SPE;

    /* If keeping CS low is needed between transfers */
    if (io_info_ptr->CSIV_ACTIVE)
    {
        /* CS inactive low for tranfers longer than 16 frames */
        qspi_ptr->QWR &= (~ MCF5XXX_QSPI_QWR_CSIV);
    }

    /* Wait transmission end */
    while (! (qspi_ptr->QIR & MCF5XXX_QSPI_QIR_SPIF)) 
        { };
    qspi_ptr->QIR |= MCF5XXX_QSPI_QIR_SPIF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_WCEF;

    if (command & MCF5XXX_QSPI_QDR_TX)
    {
        io_info_ptr->STATS.TX_PACKETS += data;
    }
        
    if (command & MCF5XXX_QSPI_QDR_RX)
    {
        /* Read data */
        io_info_ptr->STATS.RX_PACKETS += data;
        qspi_ptr->QAR = 0x0010;
        if (bytes > 1)
        {
            for (num = 0; num < size; num += bytes)
            {
                data = qspi_ptr->QDR;
                *buffer_out++ = (uint_8)(data >> 8);
                *buffer_out++ = (uint_8)data;
            }
        }
        else
        {
            for (num = 0; num < size; num += bytes)
            {
                data = qspi_ptr->QDR;
                *buffer_out++ = (uint_8)data;
            }
        }
    }

    return num;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_polled_rx
* Returned Value   : Returns the number of bytes received
* Comments         : 
*   Reads the data into provided array.
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_polled_rx
    (
        /* [IN] The address of the device specific information */
        IO_SPI_POLLED_DEVICE_STRUCT_PTR io_dev_ptr,
        
        /* [IN] The array the characters are written to */
        uchar_ptr                       buffer,
        
        /* [IN] Number of transfers to receive */
        uint_32                         size
    )
{
    MCF5XXX_QSPI_INFO_STRUCT_PTR        io_info_ptr;
        uint_16                             command;
    uint_32                             num, tmp;
    
    io_info_ptr  = io_dev_ptr->DEV_INFO_PTR;

    command = MCF5XXX_QSPI_QDR_CONT | MCF5XXX_QSPI_QDR_BITSE | MCF5XXX_QSPI_QDR_CS(~ io_info_ptr->CS) | MCF5XXX_QSPI_QDR_RX;
    if (io_dev_ptr->FLAGS & SPI_FLAG_FULL_DUPLEX)
    {
        command |= MCF5XXX_QSPI_QDR_TX;
    }
    
    for (num = 0; size > 0; )
    {
        tmp = _mcf5xxx_qspi_polled_tx_rx (io_info_ptr, buffer, buffer, size, command);
        if (0 == tmp) break;
        num += tmp;
        buffer += tmp;
        size -= tmp;
    }
    return num;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf52_qspi_polled_tx
* Returned Value   : return number of byte transmitted
* Comments         : 
*   Writes the provided data buffer and loops until transmission complete
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_polled_tx
    (
        /* [IN] The address of the device specific information */
        IO_SPI_POLLED_DEVICE_STRUCT_PTR io_dev_ptr,
        
        /* [IN] The array to store data */
        uchar_ptr                       buffer,
        
        /* [IN] Number of char's to transmit */
        uint_32                         size
    )
{
    MCF5XXX_QSPI_INFO_STRUCT_PTR        io_info_ptr;
    uint_16                             command;
    uint_32                             num, tmp;
    
    io_info_ptr  = io_dev_ptr->DEV_INFO_PTR;

    command = MCF5XXX_QSPI_QDR_CONT | MCF5XXX_QSPI_QDR_BITSE | MCF5XXX_QSPI_QDR_CS(~ io_info_ptr->CS) | MCF5XXX_QSPI_QDR_TX;

    for (num = 0; size > 0; )
    {
        tmp = _mcf5xxx_qspi_polled_tx_rx (io_info_ptr, buffer, buffer, size, command);
        if (0 == tmp) break;
        num += tmp;
        buffer += tmp;
        size -= tmp;
    }
    return num;
}
/* EOF */
