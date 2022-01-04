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
* $FileName: sai_int_ksai.c$
* $Version : 3.8.10.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains low level functions for the SAI interrupt device driver
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "sai_int_prv.h"
#include "sai_ksai_prv.h"

#if PSP_MQX_CPU_IS_VYBRID // BEGIN - Platform specific macros + functions for Vybrid

#define PLL_DIV_MAX 16
#define SAI_DIV_MAX 16
#define I2S_RX_FIFO_FULL_MASK 0x20
/*
** Values of Vybrid watermarks are set to 
** experimentally tested default values.
*/
#define I2S_TX_FIFO_DEFAULT_WATERMARK 8 
#define I2S_RX_FIFO_DEFAULT_WATERMARK 25

#define I2S_RX_WRITE_PTR_MASK 0x1F
#define I2S_RX_READ_PTR_MASK  0x1F
#define I2S_TX_WRITE_PTR_MASK 0x1F
#define I2S_TX_READ_PTR_MASK  0x1F

static uint_32 _vi2s_get_mclk_value(KSAI_INFO_STRUCT_PTR io_info_ptr);
static void _vi2s_set_mclk_freq(KSAI_INFO_STRUCT_PTR io_info_ptr, uint_32 freq);
static void _vi2s_set_clk_src_int(uint_8 hw_channel);
static void _vi2s_set_clk_src_ext(uint_8 hw_channel);
static void _vi2s_set_sai_div(uint_8 hw_channel, uint_8 value);
#else //  Platform specific macros + functions for Kinetis

#define I2S_RX_FIFO_FULL_MASK 0x10
/*
** Values of Kinetis watermarks are set to 
** experimentally tested default values.
*/
#define I2S_TX_FIFO_DEFAULT_WATERMARK 3
#define I2S_RX_FIFO_DEFAULT_WATERMARK 4

#define I2S_RX_WRITE_PTR_MASK 0x7
#define I2S_RX_READ_PTR_MASK  0x7
#define I2S_TX_WRITE_PTR_MASK 0x7
#define I2S_TX_READ_PTR_MASK  0x7

static uint_32 _ki2s_get_mclk_value(KSAI_INFO_STRUCT_PTR io_info_ptr);
#endif // END - Platform specific macros

static void _ksai_tx_irq_handler(pointer);
static void _ksai_rx_irq_handler(pointer);
static void _ksai_rx_tx_irq_handler(pointer);

static void _ksai_hw_init(KSAI_INIT_STRUCT_PTR, I2S_MemMapPtr, _mqx_int);
static uint_32 _ksai_format_in_data(char_ptr input, AUDIO_DATA_FORMAT_PTR format);
static void _ksai_format_out_data(uint_32 input, char_ptr output, AUDIO_DATA_FORMAT_PTR format);
static uint_32 _ksai_sine_wave(uint_32, uint_32, int_16_ptr*);

static uint_32 _ksai_format_in_data(char_ptr input, AUDIO_DATA_FORMAT_PTR format);
static uint_32 _ki2s_find_mclk_freq_div(CM_CLOCK_SOURCE src, uint_16_ptr frct, uint_16_ptr div, uint_32 frequency);
static uint_32 _ki2s_set_master_mode(KSAI_INFO_STRUCT_PTR io_info_ptr, uint_8 mode);
static uint_32 _ki2s_set_slave_mode(KSAI_INFO_STRUCT_PTR io_info_ptr, uint_8 mode);
static uint_32 _ki2s_set_bclk_mode(I2S_MemMapPtr i2s_ptr, uint_8 bclk_mode);

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_int_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an SAI device.
*
*END*********************************************************************/

_mqx_int _ksai_int_init
(
    /* [IN] Initialization information for the device being opened */
    pointer io_dev_ptr
)
{   /* Body */
    IO_SAI_DEVICE_STRUCT_PTR    io_sai_dev_ptr = (IO_SAI_DEVICE_STRUCT_PTR)io_dev_ptr;
    KSAI_INIT_STRUCT_PTR        ksai_init_ptr = (KSAI_INIT_STRUCT_PTR)io_sai_dev_ptr->DEV_INIT_DATA_PTR;
    KSAI_INFO_STRUCT_PTR        ksai_info_ptr;
    I2S_MemMapPtr               i2s_ptr = NULL;
    _mqx_uint                   tx_vector, rx_vector, buffer_size;
    _mqx_uint                   result = 0;
    uint_8                      channel, mode, data_bits, clk_source, endianity, aligment, bits, size, channels, bclk_mode;
    
    /* Check init structure */
    if (ksai_init_ptr == NULL)
    {
        I2S_LOG("\n  _i2s_init: Error - ksai_init_ptr is NULL");
        return -1;
    }
    else
    {
        channel     = ksai_init_ptr->HW_CHANNEL;
        mode        = ksai_init_ptr->MODE;
        data_bits   = ksai_init_ptr->DATA_BITS;
        clk_source  = ksai_init_ptr->CLOCK_SOURCE;
        buffer_size = ksai_init_ptr->BUFFER_SIZE;
        endianity   = ksai_init_ptr->IO_FORMAT->ENDIAN;
        aligment    = ksai_init_ptr->IO_FORMAT->ALIGNMENT;
        bits        = ksai_init_ptr->IO_FORMAT->BITS;
        size        = ksai_init_ptr->IO_FORMAT->SIZE;
        channels    = ksai_init_ptr->IO_FORMAT->CHANNELS;
        bclk_mode   = ksai_init_ptr->CLOCK_MODE;
    }
    /* Check if valid data bit length is selected */
    if ((data_bits < AUDIO_BIT_SIZE_MIN) || (data_bits > AUDIO_BIT_SIZE_MAX))
    {
        I2S_LOG("\n  _i2s_init: Error - Unsupported data word length.");
        return (I2S_ERROR_WORD_LENGTH_UNSUPPORTED);
    }   
    /* Check mode of operation */
    if (mode > (I2S_TX_SLAVE | I2S_RX_SLAVE))
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid I2S mode.");
        return(I2S_ERROR_MODE_INVALID);
    }
    /* Check clock source */
    if (clk_source > I2S_CLK_EXT)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid I2S clock source.");
        return(I2S_ERROR_CLK_INVALID);
    }
    /* Check buffer size */
    if (buffer_size < 2)
    {
        I2S_LOG("\n  _i2s_init: Error - I2S transmit buffer size too small.");
        return(I2S_ERROR_BUFFER_SMALL);
    }
    /* Check bit clock setting */
    if ((bclk_mode & I2S_TX_SYNCHRONOUS) && (bclk_mode & I2S_RX_SYNCHRONOUS))
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid bit clock configuration selected.");
        return(I2S_ERROR_INVALID_BCLK);
    }
    /* Check audio data format */
    if (
        (aligment > AUDIO_ALIGNMENT_LEFT) || (bits < AUDIO_BIT_SIZE_MIN) ||
        (bits > AUDIO_BIT_SIZE_MAX) || (endianity > AUDIO_LITTLE_ENDIAN) ||
        (size * I2S_CHAR_BIT < bits) || (channels > 2) || (channels == 0)
       )
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid I/O data format.");
        return(AUDIO_ERROR_INVALID_IO_FORMAT);
    }
    /* Check selected data channels */
    if (ksai_init_ptr->TX_CHANNEL >= SAI_DATA_CHANNELS)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid transmitter data channel selected.");
        return(I2S_ERROR_INVALID_TX_CHANNEL);
    }
    if (ksai_init_ptr->RX_CHANNEL >= SAI_DATA_CHANNELS)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid receiver data channel selected.");
        return(I2S_ERROR_INVALID_RX_CHANNEL);
    }
    /* Check if valid peripheral is selected */
    i2s_ptr = _bsp_get_sai_base_address(channel);
    if (i2s_ptr == NULL)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid hardware channel selected.");
        return(I2S_ERROR_CHANNEL_INVALID);
    }
    ksai_info_ptr = (KSAI_INFO_STRUCT_PTR) _mem_alloc_system_zero((uint_32) sizeof(KSAI_INFO_STRUCT));
    if (NULL == ksai_info_ptr)
    {
        I2S_LOG("\n  _i2s_init: Error - Allocation of KI2S_INFO_STRUCT failed.");
        return(MQX_OUT_OF_MEMORY);
    }
    _mem_set_type (ksai_info_ptr, MEM_TYPE_IO_I2S_INFO_STRUCT);
    
    /* If the device is not alreadz open do I/O initialization */
    if ((io_sai_dev_ptr->READ_COUNT == 0) && (io_sai_dev_ptr->WRITE_COUNT == 0))
    {
        /* SAI board specific IO pins initialization */
        _bsp_sai_io_init(channel);  
    }
    /* Copy clock source to context structure */
    ksai_info_ptr->MCLK_SRC = ksai_init_ptr->MCLK_SRC;
    /* SAI board specific registers initialization */
    _ksai_hw_init(ksai_init_ptr, i2s_ptr, io_sai_dev_ptr->FLAGS);

    /*Info struct setup*/
    ksai_info_ptr->INIT           = *ksai_init_ptr;
    ksai_info_ptr->SAI_PTR        = i2s_ptr;
    ksai_info_ptr->MODE           = ksai_init_ptr->MODE;
    ksai_info_ptr->DATA_BITS      = ksai_init_ptr->DATA_BITS;
    ksai_info_ptr->CLOCK_SOURCE   = ksai_init_ptr->CLOCK_SOURCE;
    ksai_info_ptr->CLOCK_MODE     = ksai_init_ptr->CLOCK_MODE;
    ksai_info_ptr->TX_CHANNEL     = ksai_init_ptr->TX_CHANNEL;
    ksai_info_ptr->RX_CHANNEL     = ksai_init_ptr->RX_CHANNEL;
    ksai_info_ptr->IO_MODE        = io_sai_dev_ptr->FLAGS;
    ksai_info_ptr->HW_CHANNEL     = channel;
        
    ksai_info_ptr->IO_FORMAT.ENDIAN       = endianity;
    ksai_info_ptr->IO_FORMAT.BITS         = bits;
    ksai_info_ptr->IO_FORMAT.ALIGNMENT    = aligment;
    ksai_info_ptr->IO_FORMAT.SIZE         = size;
    ksai_info_ptr->IO_FORMAT.CHANNELS     = channels;
    
    if (ksai_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
    {
#if PSP_MQX_CPU_IS_VYBRID
        ksai_info_ptr->MCLK_FREQ  = _vi2s_get_mclk_value(ksai_info_ptr);
#else
        ksai_info_ptr->MCLK_FREQ  = _ki2s_get_mclk_value(ksai_info_ptr);
#endif
        ksai_info_ptr->BCLK_FREQ  = ksai_info_ptr->MCLK_FREQ / DEFAULT_BCLK_DIV;
        ksai_info_ptr->FS_FREQ    = ksai_info_ptr->BCLK_FREQ / (ksai_info_ptr->DATA_BITS * ksai_info_ptr->IO_FORMAT.CHANNELS);
    }
    else
    {
        ksai_info_ptr->MCLK_FREQ  = 0;
        ksai_info_ptr->BCLK_FREQ  = 0;
        ksai_info_ptr->FS_FREQ    = 0;
    }

    ksai_info_ptr->BCLK_DIV = DEFAULT_BCLK_DIV;
    ksai_info_ptr->TX_DUMMY = ksai_init_ptr->TX_DUMMY;
    ksai_info_ptr->ONTHEWAY = FALSE;
    ksai_info_ptr->DATA = NULL;
    
    result = _lwsem_create(&(ksai_info_ptr->EVENT_IO_FINISHED), 0);
    if (result != MQX_OK) 
    {
        I2S_LOG("\n  _i2s_init: Error - Unable to create event_int_done.");
        _task_set_error(result);
    }
    
    ksai_info_ptr->DUMMY.DATA     = NULL;
    ksai_info_ptr->DUMMY.LENGTH   = 0;
    ksai_info_ptr->DUMMY.INDEX    = 0;
    ksai_info_ptr->CLK_ALWAYS_ON  = TRUE;
    ksai_info_ptr->FIRST_IO       = TRUE;
    
    /* Buffers allocation */
    ksai_info_ptr->BUFFER.DATA = (uint_32_ptr)_mem_alloc_system_zero(buffer_size * sizeof(uint_32));
    if (ksai_info_ptr->BUFFER.DATA == NULL)
    {
        return MQX_OUT_OF_MEMORY;
    }
    ksai_info_ptr->BUFFER.SPACE = buffer_size;
    ksai_info_ptr->BUFFER.SIZE  = buffer_size;    

    _mem_set_type (ksai_info_ptr->BUFFER.DATA, MEM_TYPE_IO_I2S_LEFT_BUFFER);
    
    io_sai_dev_ptr->DEV_INFO_PTR = ksai_info_ptr;

    /* Interrupt vectors setup */
    tx_vector = _bsp_get_sai_tx_vector(channel);
    rx_vector = _bsp_get_sai_rx_vector(channel);
    
    if (tx_vector == rx_vector)
    {
        ksai_info_ptr->OLD_TX_ISR_DATA = _int_get_isr_data(tx_vector);
        ksai_info_ptr->OLD_TX_ISR = _int_install_isr(tx_vector, _ksai_rx_tx_irq_handler, ksai_info_ptr);
        ksai_info_ptr->OLD_RX_ISR_DATA = ksai_info_ptr->OLD_TX_ISR_DATA;
        ksai_info_ptr->OLD_RX_ISR = ksai_info_ptr->OLD_TX_ISR;
        _bsp_int_init(tx_vector, ksai_info_ptr->INIT.LEVEL, 0, TRUE);
    }
    else
    {
        /* Interrupt Tx vector setup */
        if (io_sai_dev_ptr->FLAGS & I2S_IO_WRITE)
        {
            ksai_info_ptr->OLD_TX_ISR_DATA = _int_get_isr_data(tx_vector);
            ksai_info_ptr->OLD_TX_ISR = _int_install_isr(tx_vector, _ksai_tx_irq_handler, ksai_info_ptr);
            _bsp_int_init(tx_vector, ksai_info_ptr->INIT.LEVEL, 0, TRUE);
        }
        /* Interrupt Rx vector setup */
        if (io_sai_dev_ptr->FLAGS & I2S_IO_READ)
        {
            ksai_info_ptr->OLD_RX_ISR_DATA = _int_get_isr_data(rx_vector);
            ksai_info_ptr->OLD_RX_ISR = _int_install_isr(rx_vector, _ksai_rx_irq_handler, ksai_info_ptr);
            _bsp_int_init(rx_vector, ksai_info_ptr->INIT.LEVEL, 0, TRUE);
        }
    }

    return MQX_OK;
} /* End body */

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_int_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an SAI device.
*
*END*********************************************************************/

_mqx_int _ksai_int_deinit
(
    /* [IN] Initialization information for the device being closed */
    pointer io_dev_ptr,
    
    pointer io_info_ptr
)
{
    IO_SAI_DEVICE_STRUCT_PTR    io_sai_dev_ptr = (IO_SAI_DEVICE_STRUCT_PTR)io_dev_ptr;
    KSAI_INFO_STRUCT_PTR        ksai_info_ptr = (KSAI_INFO_STRUCT_PTR)io_info_ptr;
    KSAI_INIT_STRUCT_PTR        sai_init_ptr;
    I2S_MemMapPtr               i2s_ptr;    
    _mqx_uint                   errcode;
    _mqx_uint                   result = 0;
    uint_8                      channel = ksai_info_ptr->HW_CHANNEL;
    
    if ((NULL == ksai_info_ptr) || (NULL == io_sai_dev_ptr))
    {
        I2S_LOG("\n  _i2s_deinit: Error - Null pointer.");
        return I2S_ERROR_INVALID_PARAMETER;
    }
    
    i2s_ptr = ksai_info_ptr->SAI_PTR;
    sai_init_ptr = (KSAI_INIT_STRUCT_PTR)(io_sai_dev_ptr->DEV_INIT_DATA_PTR);
        
    if (io_sai_dev_ptr->FLAGS & I2S_IO_WRITE)
    {
        /* Disable transmitter */
        i2s_ptr->TCSR &= ~I2S_TCSR_TE_MASK;
        /* Disable transmit channel */
        i2s_ptr->TCR3 &= ~I2S_TCR3_TCE_MASK;
        /* Disable transmitter bit clock */
        i2s_ptr->TCSR &= ~I2S_TCSR_BCE_MASK;
        /* Install original tx vectors */
        _bsp_int_init((IRQInterruptIndex) _bsp_get_sai_tx_vector(channel), sai_init_ptr->LEVEL, 0, TRUE);
        _int_install_isr(_bsp_get_sai_tx_vector(channel), ksai_info_ptr->OLD_TX_ISR, ksai_info_ptr->OLD_TX_ISR_DATA);
    }
    
    if ((ksai_info_ptr->OLD_TX_ISR != ksai_info_ptr->OLD_RX_ISR) && (io_sai_dev_ptr->FLAGS & I2S_IO_READ))
    {
        /* Disable receiver */
        i2s_ptr->RCSR &= ~I2S_RCSR_RE_MASK;
        /* Disable receiver channel */
        i2s_ptr->RCR3 &= ~I2S_RCR3_RCE_MASK;
        /* Disable receiver bit clock */
        i2s_ptr->RCSR &= ~I2S_RCSR_BCE_MASK;
        /* Install original rx vectors */
        _bsp_int_init((IRQInterruptIndex)_bsp_get_sai_rx_vector(channel), sai_init_ptr->LEVEL, 0, TRUE);
        _int_install_isr(_bsp_get_sai_rx_vector(channel), ksai_info_ptr->OLD_RX_ISR, ksai_info_ptr->OLD_RX_ISR_DATA);
    }
#if !PSP_MQX_CPU_IS_VYBRID
    i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
    /* Destroy event */
    result = _lwsem_destroy(&(ksai_info_ptr->EVENT_IO_FINISHED));

    if (result != MQX_OK)
    {
        I2S_LOG("\n  _i2s_deinit: Error - unable to destroy event_int_done.\n");
        _task_set_error(result);
    }
    
    ksai_info_ptr->ONTHEWAY = FALSE;
    
    /* Free buffers and info struct */
    errcode = _mem_free(ksai_info_ptr->BUFFER.DATA);
    if (errcode != MQX_OK)
    {
        I2S_LOG("\n  _i2s_deinit: Error - Free data buffer failed.");
        return errcode;
    }
    _mem_zero(&(ksai_info_ptr->BUFFER), sizeof(ksai_info_ptr->BUFFER));
        
    if (ksai_info_ptr->TX_DUMMY)
    {
        errcode = _mem_free (ksai_info_ptr->DUMMY.DATA);
        if (errcode != MQX_OK)
        {
            I2S_LOG("\n  _i2s_deinit: Error - Free dummy data failed.");
            return errcode;
        }
        ksai_info_ptr->DUMMY.DATA = NULL;
        _mem_zero(&(ksai_info_ptr->DUMMY), sizeof(ksai_info_ptr->DUMMY));
    }
    
    errcode = _mem_free(io_sai_dev_ptr->DEV_INFO_PTR);
    if (errcode != MQX_OK)
    {
        I2S_LOG("\n  _i2s_deinit: Error - Free device information pointer failed.");
        return errcode;
    }
    io_sai_dev_ptr->DEV_INFO_PTR = NULL;
    
    return(MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_int_tx
* Returned Value   : number of bytes written
* Comments         :
*   Returns the number of bytes written.
*   Writes the data provided into transmition buffer if available.
*
*END*********************************************************************/

_mqx_int _ksai_int_tx
(
    /* [IN] the address of the device specific information */
    pointer io_dev_ptr, 
    
    /* [IN] The array to be read from */
    char_ptr buff, 
    
    /* [IN] number of packets to output */
    uint_32 length
)
{
    IO_SAI_DEVICE_STRUCT_PTR   io_sai_dev_ptr   = (IO_SAI_DEVICE_STRUCT_PTR)io_dev_ptr;
    KSAI_INFO_STRUCT_PTR       ksai_info_ptr    = (KSAI_INFO_STRUCT_PTR)io_sai_dev_ptr->DEV_INFO_PTR;
    I2S_MemMapPtr              i2s_ptr          = ksai_info_ptr->SAI_PTR;
    KSAI_BUFFER_PTR            buffer           = &(ksai_info_ptr->BUFFER);
    I2S_STATISTICS_STRUCT_PTR  stats            = &(ksai_info_ptr->STATS);
    AUDIO_DATA_FORMAT_PTR      io_format        = &(ksai_info_ptr->IO_FORMAT);
    LWSEM_STRUCT_PTR           event_int_done   = &(ksai_info_ptr->EVENT_IO_FINISHED);
    _mqx_uint                  result           = MQX_OK;
   
    if ((buff == NULL) && !(ksai_info_ptr->TX_DUMMY))
    {
        I2S_LOG("\n  _i2s_tx: Error - Null pointer.");
        _task_set_error(I2S_ERROR_INVALID_PARAMETER);
        return (0);
    }
    ksai_info_ptr->DATA = buff;
    
    /* Initialize statistics and buffers */    
    stats->PACKETS_REQUESTED += length / io_format->SIZE;
    /* In dummy mode, data are already in memory, so we don't have to buffer them */
    if (ksai_info_ptr->TX_DUMMY)
    {
        stats->PACKETS_QUEUED = stats->PACKETS_REQUESTED;
    }
    
    /* Set device into busy state */
    ksai_info_ptr->ONTHEWAY = TRUE;

    while(ksai_info_ptr->ONTHEWAY)
    {
        if (!(ksai_info_ptr->TX_DUMMY))
        {
            /* Copy data from input to buffer (reformat them when necessary) */
            while ((buffer->SPACE > 0) && (stats->PACKETS_QUEUED < stats->PACKETS_REQUESTED))
            {
                /* Disable interrupts with buffer access */
                i2s_ptr->TCSR &= ~(I2S_TCSR_FRIE_MASK | I2S_TCSR_FEIE_MASK);
                buffer->DATA[buffer->IN++] = _ksai_format_in_data(ksai_info_ptr->DATA, io_format);
                buffer->SPACE--;
                stats->PACKETS_QUEUED++;
                ksai_info_ptr->DATA += io_format->SIZE;
                (buffer->IN >= buffer->SIZE) ? (buffer->IN = 0) : (0);
                /* Enable interrupts */
                if (!(ksai_info_ptr->FIRST_IO))
                {
                    i2s_ptr->TCSR |= (I2S_TCSR_FRIE_MASK | I2S_TCSR_FEIE_MASK);                  
                }
            }
        }
        if (ksai_info_ptr->FIRST_IO)
        {
            ksai_info_ptr->FIRST_IO = FALSE;
            /* Invert transmiter frame sync in TX mode */
            i2s_ptr->TCR4 |= (I2S_TCR4_FSP_MASK);
            
            /* First transmit operation - enable transmitter */
            i2s_ptr->TCSR |= (I2S_TCSR_FRIE_MASK | I2S_TCSR_FEIE_MASK);
            i2s_ptr->TCSR |= (I2S_TCSR_TE_MASK);
            /* 
            ** We have to enable receiver in order to get clock signals (BCLK and FS)
            ** if transmitter is synchronous.
            */
            if (ksai_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
            {
                i2s_ptr->RCSR |= (I2S_RCSR_RE_MASK);
                
            }
        }

        /* If all requested packets are queued break and return from function */
        if (stats->PACKETS_QUEUED >= stats->PACKETS_REQUESTED)
        {
            break;
        }
        
        /* Wait for transmit finish (task is blocked during wait) */
        result = _lwsem_wait(event_int_done);
        if (result != MQX_OK)
        {
            I2S_LOG("\n  _i2s_tx: Error - Wait for event_int_done failed.\n");
            _task_set_error(result);
        }
    }
    /* return number of processed data */
    return ((length / io_format->SIZE) * io_format->SIZE);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_int_rx
* Returned Value   : number of bytes read
* Comments         :
*   Returns the number of bytes received.
*   Reads the data into provided array when data is available.
*
*END*********************************************************************/

_mqx_int _ksai_int_rx
(
    /* [IN] the address of the device specific information */
    pointer io_dev_ptr, 
    
    /* [IN] The array to copy data into */
    char_ptr buff, 
    
    /* [IN] number of bytes to read */
    uint_32 length
) 
{
    IO_SAI_DEVICE_STRUCT_PTR   io_sai_dev_ptr   = (IO_SAI_DEVICE_STRUCT_PTR)io_dev_ptr;
    KSAI_INFO_STRUCT_PTR       ksai_info_ptr    = (KSAI_INFO_STRUCT_PTR)io_sai_dev_ptr->DEV_INFO_PTR;
    I2S_MemMapPtr              i2s_ptr          = ksai_info_ptr->SAI_PTR;
    KSAI_BUFFER_PTR            buffer           = &(ksai_info_ptr->BUFFER);
    I2S_STATISTICS_STRUCT_PTR  stats            = &(ksai_info_ptr->STATS);
    AUDIO_DATA_FORMAT_PTR      io_format        = &(ksai_info_ptr->IO_FORMAT);
    LWSEM_STRUCT_PTR           event_int_done   = &(ksai_info_ptr->EVENT_IO_FINISHED);
    _mqx_uint                  result           = MQX_OK;

    if (buff == NULL)
    {
        I2S_LOG("\n  _i2s_rx: Error - Null pointer.");
        _task_set_error(I2S_ERROR_INVALID_PARAMETER);
        return (0);
    }
    
    ksai_info_ptr->DATA = buff;
        
    /* Initialize statistics and buffers to default values */    
    stats->PACKETS_REQUESTED += length / io_format->SIZE;

    ksai_info_ptr->ONTHEWAY = TRUE;
    
    while(ksai_info_ptr->ONTHEWAY)
    {
        /* Enable receiver and interrupts in case of first run to get some data */
        if (ksai_info_ptr->FIRST_IO) 
        {
            ksai_info_ptr->FIRST_IO = FALSE;
            /* First receive operation - enable receiver */
            i2s_ptr->RCSR |= (I2S_RCSR_FRIE_MASK | I2S_RCSR_FEIE_MASK);
            i2s_ptr->RCSR |= (I2S_RCSR_RE_MASK);
            /* 
            ** We have to enable transmitter in order to get clock signals 
            ** (BCLK and FS) if the receiver is synchronous.
            */
            if (ksai_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
            {
                i2s_ptr->TCSR |= (I2S_TCSR_TE_MASK);
                i2s_ptr->TCR4 &= ~(I2S_TCR4_FSP_MASK); 
            }
        }
        /* Wait for receive finish */
        result = _lwsem_wait(event_int_done);
        
        if (result != MQX_OK)
        {
            I2S_LOG("\n  _i2s_rx: Error - Wait for event_int_done failed.");
            _task_set_error(result);
        }

        while ((buffer->SPACE < buffer->SIZE) &&
               (stats->PACKETS_PROCESSED < stats->PACKETS_REQUESTED))
        {
            /* Disable interrupts with buffer access */
            i2s_ptr->RCSR &= ~(I2S_RCSR_FRIE_MASK | I2S_RCSR_FEIE_MASK);
            (buffer->OUT >= buffer->SIZE) ? (buffer->OUT = 0) : (0);
            _ksai_format_out_data(buffer->DATA[buffer->OUT], ksai_info_ptr->DATA, io_format);
            buffer->OUT++;
            ksai_info_ptr->DATA += io_format->SIZE;
            stats->PACKETS_PROCESSED++;
            buffer->SPACE++;
            /* Enable interrupts */
            i2s_ptr->RCSR |= (I2S_RCSR_FRIE_MASK | I2S_RCSR_FEIE_MASK);
        }
        
        if (stats->PACKETS_PROCESSED >= stats->PACKETS_REQUESTED)
        {
            break;
        }
    }
    
    
    return ((length / io_format->SIZE) * io_format->SIZE);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_tx_irq_handler
* Returned Value   : void
* Comment          :
*   TX interrupt handler of SAI driver
*
*END*********************************************************************/

void _ksai_tx_irq_handler
(
    /* [IN] Parameter pointer */
    pointer param
)
{
    KSAI_INFO_STRUCT_PTR       ksai_info_ptr = (KSAI_INFO_STRUCT_PTR) param;
    I2S_MemMapPtr              i2s_ptr = ksai_info_ptr->SAI_PTR;
    KSAI_BUFFER_PTR            buffer = &(ksai_info_ptr->BUFFER);
    I2S_STATISTICS_STRUCT_PTR  stats = &(ksai_info_ptr->STATS);
    KSAI_DUMMY_DATA_PTR        dummy = &(ksai_info_ptr->DUMMY);
    uint_32                    i = 0;
    uint_32                    space;
    uint_8                     tx_channel = ksai_info_ptr->TX_CHANNEL;
    uint_32                    read_ptr;
    uint_32                    write_ptr;
    boolean                    stop = FALSE;

    stats->INTERRUPTS++;
    
    /* Check FIFO error flag */
    if (i2s_ptr->TCSR & I2S_TCSR_FEF_MASK)
    {
        /* Reset FIFO */
        i2s_ptr->TCSR |= I2S_TCSR_FR_MASK;
        stats->FIFO_ERROR++;
    }
    
    read_ptr = (i2s_ptr->TFR[tx_channel] & I2S_TFR_RFP_MASK) >> I2S_TFR_RFP_SHIFT;
    write_ptr = (i2s_ptr->TFR[tx_channel] & I2S_TFR_WFP_MASK) >> I2S_TFR_WFP_SHIFT;
    
    /* All data transmitted, disable transmitter */
    if ((stats->PACKETS_PROCESSED >= stats->PACKETS_REQUESTED) &&  /* All data send       */ 
        (read_ptr == write_ptr))                                   /* FIFO is empty       */
    {
        ksai_info_ptr->ONTHEWAY = FALSE;
        ksai_info_ptr->FIRST_IO = TRUE;
        /* Disable interrupts and transmitter */
        i2s_ptr->TCSR &= ~(I2S_TCSR_FRIE_MASK | I2S_TCSR_FEIE_MASK); 
        i2s_ptr->TCSR &= ~(I2S_TCSR_TE_MASK);
        stop = TRUE; 
    }
 
    /* Check FIFO request flag */
    if (i2s_ptr->TCSR & I2S_TCSR_FRF_MASK)
    {   
        if (write_ptr == read_ptr) /* FIFO is empty */
        {
            space = SIZE_OF_FIFO;
        }
        else /* FIFO is not empty */
        {
            int result; 
            
            result = (write_ptr & I2S_TX_WRITE_PTR_MASK) - (read_ptr & I2S_TX_READ_PTR_MASK);
            space = (result < 0) ? -(unsigned)result : result;
        }
        /* Copy data from buffer to FIFO */
        for (i = 0; (i < space) && (stats->PACKETS_PROCESSED < stats->PACKETS_QUEUED); i++)
        {
            if (ksai_info_ptr->TX_DUMMY)
            {
                i2s_ptr->TDR[ksai_info_ptr->TX_CHANNEL] = dummy->DATA[dummy->INDEX] & 0xFFFFFFFF;
                (i % 2) ? (dummy->INDEX++) : (0);
                (dummy->INDEX >= dummy->LENGTH) ? (dummy->INDEX = 0) : (0);
                stats->PACKETS_PROCESSED++;
            }
            else
            {
                i2s_ptr->TDR[ksai_info_ptr->TX_CHANNEL] = buffer->DATA[buffer->OUT++];
                (buffer->OUT >= buffer->SIZE) ? (buffer->OUT = 0) : (0);
                stats->PACKETS_PROCESSED++;
            }
        }
        /* Check software buffer underrun */
        if ((!stop) && (i == 0)) 
        {
            stats->BUFFER_ERROR++;
        }
        stats->TX_PACKETS += i;
        buffer->SPACE += i;
    }
    /* Clear Word start flag */
    i2s_ptr->TCSR |= I2S_TCSR_WSF_MASK; 
    /* Data are ready in FIFO - clear error flag */
    i2s_ptr->TCSR |= I2S_TCSR_FEF_MASK;
    
    if (_lwsem_post(&(ksai_info_ptr->EVENT_IO_FINISHED)) != MQX_OK)
    {
        I2S_LOG("\n  i2s_tx_irq_handler: Error - Unable to set EVENT_IO_FINISHED.");
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_rx_irq_handler
* Returned Value   : void
* Comment          :
*   RX interrupt handler of SAI driver
*
*END*********************************************************************/

void _ksai_rx_irq_handler
(
    /* [IN] Parameter pointer */
    pointer param
) 
{
    KSAI_INFO_STRUCT_PTR       ksai_info_ptr = (KSAI_INFO_STRUCT_PTR)param;
    I2S_MemMapPtr              i2s_ptr = ksai_info_ptr->SAI_PTR;
    KSAI_BUFFER_PTR            buffer = &(ksai_info_ptr->BUFFER);
    I2S_STATISTICS_STRUCT_PTR  stats = &(ksai_info_ptr->STATS);
    uint_32                    i = 0;
    uint_32                    data_n;
    uint_8                     rx_channel = ksai_info_ptr->RX_CHANNEL;
    uint_32                    read_ptr;
    uint_32                    write_ptr;
    boolean                    stop = FALSE;
    
    stats->INTERRUPTS++;
    
    read_ptr = i2s_ptr->RFR[rx_channel] & I2S_RFR_RFP_MASK;
    write_ptr = (i2s_ptr->RFR[rx_channel] & I2S_RFR_WFP_MASK) >> I2S_RFR_WFP_SHIFT;
    
    /* Check FIFO request flag */
    if (i2s_ptr->RCSR & I2S_RCSR_FRF_MASK)
    { 
        /* FIFO is full */
        if ((write_ptr ^ read_ptr) == I2S_RX_FIFO_FULL_MASK)
        {
            data_n = SIZE_OF_FIFO;
        }

        else /* FIFO is not full */
        {
            int result;

            result = (write_ptr & I2S_RX_WRITE_PTR_MASK) - (read_ptr & I2S_RX_READ_PTR_MASK);
            data_n = (result < 0) ? -(unsigned)result : result;
        }
        /* Copy data from FIFO to buffer */
        if (data_n + stats->PACKETS_QUEUED >= stats->PACKETS_REQUESTED)
        {
            data_n = stats->PACKETS_REQUESTED - stats->PACKETS_QUEUED;
        }
        /* Check software buffer overrun */
        if (data_n > buffer->SPACE)
        { 
            data_n = buffer->SPACE;
            stats->BUFFER_ERROR++;
        }
        for (i = 0; i < data_n; i++)
        {
            (buffer->IN >= buffer->SIZE) ? (buffer->IN = 0) : (0);
            buffer->DATA[buffer->IN++] = i2s_ptr->RDR[ksai_info_ptr->RX_CHANNEL];
        }
        buffer->SPACE -= i;
        stats->PACKETS_QUEUED += i; 
        stats->RX_PACKETS += i;
    }
    
    /* Check FIFO error flag */
    if (i2s_ptr->RCSR & I2S_RCSR_FEF_MASK)
    {
        /* Reset FIFO */
        i2s_ptr->RCSR |= I2S_RCSR_FEF_MASK;
        i2s_ptr->RCSR |= I2S_RCSR_FR_MASK;
        stats->FIFO_ERROR++;
    }
    /* All data received, disable receiver */
    if (stats->PACKETS_QUEUED >= stats->PACKETS_REQUESTED)
    {
        ksai_info_ptr->ONTHEWAY = FALSE;
        ksai_info_ptr->FIRST_IO = TRUE;
        
        /* Disable interrupts and receiver */
        i2s_ptr->RCSR &= ~(I2S_RCSR_FRIE_MASK | I2S_RCSR_FEIE_MASK); 
        i2s_ptr->RCSR &= ~(I2S_RCSR_RE_MASK);
    }

    if (_lwsem_post(&(ksai_info_ptr->EVENT_IO_FINISHED)) != MQX_OK)
    {
        I2S_LOG("\n  i2s_rx_irq_handler: Error - Unable to set EVENT_IO_FINISHED.");
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_rx_tx_irq_handler
* Returned Value   : void
* Comment          :
*       Interrupt handler of SAI driver. Used when only one interrupt is 
*       available.
*
*END*********************************************************************/

void _ksai_rx_tx_irq_handler
(
    /* [IN] Parameter pointer */
    pointer param
)
{
    KSAI_INFO_STRUCT_PTR       ksai_info_ptr = (KSAI_INFO_STRUCT_PTR) param;
    I2S_MemMapPtr              i2s_ptr = ksai_info_ptr->SAI_PTR;
    
    if ((i2s_ptr->TCSR & (I2S_TCSR_FRIE_MASK | I2S_TCSR_FEIE_MASK)) &&
        (i2s_ptr->TCSR & (I2S_TCSR_FRF_MASK | I2S_TCSR_FEF_MASK)))
    {
        _ksai_tx_irq_handler(param);
    }
    
    if ((i2s_ptr->RCSR & (I2S_RCSR_FRIE_MASK | I2S_RCSR_FEIE_MASK)) && 
        (i2s_ptr->RCSR & (I2S_RCSR_FRF_MASK | I2S_RCSR_FEF_MASK)))
    {
        _ksai_rx_irq_handler(param);
    }
}


/*FUNCTION****************************************************************
*
* Function Name    : _ksai_int_ioctl
* Returned Value   : MQX error code.
* Comments         :
*    This function performs miscellaneous services for
*    the SAI I/O device.
*
*END*********************************************************************/

_mqx_int _ksai_int_ioctl
(
    /* [IN] the address of the device specific information */
    pointer     io_sai_dev_ptr, 
    
    /* [IN] The command to perform */
    _mqx_int    cmd, 
    
    /* [IN] Parameters for the command */
    pointer     param
)
{
    KSAI_INFO_STRUCT_PTR io_info_ptr = (KSAI_INFO_STRUCT_PTR) io_sai_dev_ptr;
    KSAI_DUMMY_DATA_PTR dummy = &io_info_ptr->DUMMY;
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    uint_32 result = MQX_OK;
    uint_32 sample_rate = 0;
    uint_8 bclk_mode = 0;
    switch (cmd)
    {
        case IO_IOCTL_I2S_SET_MODE_MASTER:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            result = _ki2s_set_master_mode(io_info_ptr, *((uint_8_ptr) param));
            if (result != MQX_OK)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid RX/TX master mode combination selected.");
                break;
            }
            break;
            
        case IO_IOCTL_I2S_SET_MODE_SLAVE:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            result = _ki2s_set_slave_mode(io_info_ptr, *((uint_8_ptr) param));
            if (result != MQX_OK)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid RX/TX slave mode combination selected.");
                break;
            }
            break;
            
        case IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT:
            if (io_info_ptr->ONTHEWAY)
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot change I2S clock source while I/O actions are in progress.");
               result = I2S_ERROR_DEVICE_BUSY;
               break;
            }
            /* Clock source should not change, when in slave mode */
            if ((io_info_ptr->MODE & I2S_TX_SLAVE) || (io_info_ptr->MODE & I2S_RX_SLAVE))
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S clock source while in slave mode.");
               result = IO_ERROR_INVALID_IOCTL_CMD;
               break;
            }
            
            io_info_ptr->CLOCK_SOURCE = I2S_CLK_INT;
            /* Set master clock to be generated internally */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_int(io_info_ptr->HW_CHANNEL);
            io_info_ptr->MCLK_FREQ = _vi2s_get_mclk_value(io_info_ptr);
#else
            i2s_ptr->MCR |= (I2S_MCR_MOE_MASK);
            io_info_ptr->MCLK_FREQ = _ki2s_get_mclk_value(io_info_ptr);
#endif       
            io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
            io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * 2/*io_info_ptr->IO_FORMAT.CHANNELS*/);
            break;
            
        case IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT:
            if (io_info_ptr->ONTHEWAY)
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S clock source while I/O actions are in progress.");
               result = I2S_ERROR_DEVICE_BUSY;
               break;
            }
            /* You should not change clock source, when in slave mode */
            if ((io_info_ptr->MODE & I2S_TX_SLAVE) || (io_info_ptr->MODE & I2S_RX_SLAVE))
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S clock source while in slave mode.");
               result = IO_ERROR_INVALID_IOCTL_CMD;
               break;
            }
            
            io_info_ptr->CLOCK_SOURCE = I2S_CLK_EXT;
            /* Set master clock to be generated externally */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_ext(io_info_ptr->HW_CHANNEL);
#else
            i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
            io_info_ptr->MCLK_FREQ = 0;
            io_info_ptr->BCLK_FREQ = 0;
            io_info_ptr->FS_FREQ = 0;
            break;
            
        case IO_IOCTL_I2S_SET_DATA_BITS:
            if(io_info_ptr->ONTHEWAY)
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S data bit length while I/O actions are in progress.");
               result = I2S_ERROR_DEVICE_BUSY;
               break;
            }
            if (param == NULL)
            {
                 result = I2S_ERROR_INVALID_PARAMETER;
            }
            else
            {
               uint_8 data_bits =  *((uint_8_ptr) param);
               if ((data_bits < 8) || (data_bits > 32))
               {
                  I2S_LOG("\n  _i2s_ioctl: Error - Unsupported data word length.\n");
                  result = I2S_ERROR_WORD_LENGTH_UNSUPPORTED;
                  break;
               }
               io_info_ptr->DATA_BITS = data_bits;
               /* Setup transmitter */
               if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) || (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS))
               {
                   /* Set 0th word length */
                   i2s_ptr->TCR5 &= ~(I2S_TCR5_W0W_MASK);
                   i2s_ptr->TCR5 |= I2S_TCR5_W0W(data_bits-1);
                   /* Set Nth word length */
                   i2s_ptr->TCR5 &= ~(I2S_TCR5_WNW_MASK);
                   i2s_ptr->TCR5 |= I2S_TCR5_WNW(data_bits-1);
                   /* Set first bit shifted to highest index in register */
                   i2s_ptr->TCR5 &= ~(I2S_TCR5_FBT_MASK);
                   i2s_ptr->TCR5 |= I2S_TCR5_FBT(data_bits);
                   /* Set sync width to match word length */
                   i2s_ptr->TCR4 &= ~(I2S_TCR4_SYWD_MASK);
                   i2s_ptr->TCR4 |= I2S_TCR4_SYWD(data_bits-1); 
               }
               /* Setup receiver */
               if ((io_info_ptr->IO_MODE & I2S_IO_READ) || (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS))
               {
                   /* Set 0th word length */
                   i2s_ptr->RCR5 &= ~(I2S_RCR5_W0W_MASK);
                   i2s_ptr->RCR5 |= I2S_RCR5_W0W(data_bits-1);
                   /* Set Nth word length */
                   i2s_ptr->RCR5 &= ~(I2S_RCR5_WNW_MASK);
                   i2s_ptr->RCR5 |= I2S_RCR5_WNW(data_bits-1);
                   /* Set first bit shifted to highest index in register */
                   i2s_ptr->RCR5 &= ~(I2S_RCR5_FBT_MASK);
                   i2s_ptr->RCR5 |= I2S_RCR5_FBT(data_bits-1);
                   /* Set sync width to match word length */
                   i2s_ptr->RCR4 &= ~(I2S_RCR4_SYWD_MASK);
                   i2s_ptr->RCR4 |= I2S_RCR4_SYWD(data_bits-1);  
               }
               /* Recalculate frame sync frequency to match settings */
               io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * 2/*io_info_ptr->IO_FORMAT.CHANNELS*/);
            }
            break;
            
        case IO_IOCTL_I2S_DISABLE_DEVICE:
            if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) && 
                (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
                {
                    i2s_ptr->TCSR &= ~( 
                        I2S_TCSR_TE_MASK   | /* Disable transmitter            */
                        I2S_TCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_TCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_TCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                    i2s_ptr->RCSR &= ~(
                        I2S_RCSR_RE_MASK   | /* Disable receiver               */
                        I2S_RCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_RCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_RCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                }
                else if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
                {
                    i2s_ptr->RCSR &= ~(
                        I2S_RCSR_RE_MASK   | /* Disable receiver               */
                        I2S_RCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_RCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_RCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                    i2s_ptr->TCSR &= ~( 
                        I2S_TCSR_TE_MASK   | /* Disable transmitter            */
                        I2S_TCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_TCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_TCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                }
            }
            else if (io_info_ptr->IO_MODE & I2S_IO_WRITE) /* Disable transmitter */
            {
                i2s_ptr->TCSR &= ~( 
                    I2S_TCSR_TE_MASK   | /* Disable transmitter            */
                    I2S_TCSR_BCE_MASK  | /* Disable bit clock              */
                    I2S_TCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                    I2S_TCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                    );
            }
            else if (io_info_ptr->IO_MODE & I2S_IO_READ) /* Disable receiver */
            {
                i2s_ptr->RCSR &= ~(
                    I2S_RCSR_RE_MASK   | /* Disable receiver               */
                    I2S_RCSR_BCE_MASK  | /* Disable bit clock              */
                    I2S_RCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                    I2S_RCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                    );
            }
            break;
            
        case IO_IOCTL_I2S_ENABLE_DEVICE:
            if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) && 
                (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS) /* Receiver must be enabled first */
                {
                    i2s_ptr->RCSR |= (
                        I2S_RCSR_RE_MASK   | /* Enable receiver  */
                        I2S_RCSR_BCE_MASK    /* Enable bit clock */
                        );
                    i2s_ptr->TCSR |= ( 
                        I2S_TCSR_TE_MASK   | /* Enable transmitter */
                        I2S_TCSR_BCE_MASK    /* Enable bit clock   */
                        );
                }
                else if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS) /* Transmitter must be enabled first */
                {
                    i2s_ptr->TCSR |= ( 
                        I2S_TCSR_TE_MASK   | /* Enable transmitter */
                        I2S_TCSR_BCE_MASK    /* Enable bit clock   */
                        );
                     i2s_ptr->RCSR |= (
                        I2S_RCSR_RE_MASK   | /* Enable receiver  */
                        I2S_RCSR_BCE_MASK    /* Enable bit clock */
                        );
                }
            } 
            else if (io_info_ptr->IO_MODE & I2S_IO_WRITE) /* Enable transmitter */
            {
                i2s_ptr->TCSR |= ( 
                    I2S_TCSR_TE_MASK   | /* Enable transmitter */
                    I2S_TCSR_BCE_MASK    /* Enable bit clock   */
                    );
            }
            else if (io_info_ptr->IO_MODE & I2S_IO_READ)  /* Enable receiver */
            {
                i2s_ptr->RCSR &= ~(
                    I2S_RCSR_RE_MASK   | /* Enable transmitter */
                    I2S_RCSR_BCE_MASK    /* Enable bit clock   */
                    );
            }
            break;
            
        case IO_IOCTL_I2S_SET_MCLK_FREQ:
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            else
            {
                uint_16 fract;
                uint_16 div;
                uint_32 frequency;
#if PSP_MQX_CPU_IS_VYBRID     
                /* Find values for SAI clock divider */
                _vi2s_set_mclk_freq(io_info_ptr, *((uint_32_ptr) param));
                frequency = _vi2s_get_mclk_value(io_info_ptr);
#else
                frequency = _ki2s_find_mclk_freq_div(io_info_ptr->MCLK_SRC, &fract, &div, *((uint_32_ptr) param));            
                i2s_ptr->MDR = (I2S_MDR_DIVIDE(div-1) | I2S_MDR_FRACT(fract-1));
                while(i2s_ptr->MCR & I2S_MCR_DUF_MASK) {}; // wait until new value is set
#endif
                /* Recalculate clock values according to new setting */
                io_info_ptr->MCLK_FREQ = frequency;
                io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
                io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * 2/*io_info_ptr->IO_FORMAT.CHANNELS*/);
            }
            break;
            
        case IO_IOCTL_I2S_SET_FS_FREQ:
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else
            {
                sample_rate = *((uint_32_ptr) param);
                if (sample_rate == 0)
                {
                    I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                    result = I2S_ERROR_INVALID_PARAMETER;
                    break;
                }
            }
            if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
            {
                /* Get bit clock value for selected sample rate*/
                io_info_ptr->BCLK_FREQ = sample_rate * io_info_ptr->DATA_BITS * 2/*io_info_ptr->IO_FORMAT.CHANNELS*/;
                /* Get divider value for calculated bit clock frequency */
                io_info_ptr->BCLK_DIV = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_FREQ;
                if (io_info_ptr->BCLK_DIV == 0)
                {
                    I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                    result = I2S_ERROR_INVALID_PARAMETER;
                    break;
                }
                /* Recalculate bit clock to match divider value */
                io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
                /* Get sampling frequency from new bit clock value */
                io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * 2/*io_info_ptr->IO_FORMAT.CHANNELS*/);     
                if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) || (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS))
                {
                    /* Set bit clock divider in TCR2 register */
                    i2s_ptr->TCR2 &= ~I2S_TCR2_DIV_MASK;
                    i2s_ptr->TCR2 |= I2S_TCR2_DIV((io_info_ptr->BCLK_DIV / 2) - 1);   
                }
                if ((io_info_ptr->IO_MODE & I2S_IO_READ) || (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS))
                {
                    /* Set bit clock divider in RCR2 register */
                    i2s_ptr->RCR2 &= ~I2S_RCR2_DIV_MASK;
                    i2s_ptr->RCR2 |= I2S_RCR2_DIV((io_info_ptr->BCLK_DIV / 2) - 1);   
                }
                
            }
            else
            {
                /* If the clock source is external, set up frame sync frequency and recalculate bit clock frequency */
                io_info_ptr->FS_FREQ = sample_rate;
                io_info_ptr->BCLK_FREQ = sample_rate * io_info_ptr->DATA_BITS * 2/*io_info_ptr->IO_FORMAT.CHANNELS*/;
            }
            
            result = MQX_OK;
            break;
#if I2S_USE_FLOAT    
        case IO_IOCTL_I2S_TX_DUMMY_ON:
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            if (io_info_ptr->TX_DUMMY == FALSE)
            {
                io_info_ptr->TX_DUMMY = TRUE;
                dummy->LENGTH = _ksai_sine_wave(io_info_ptr->FS_FREQ, 440, &(dummy->DATA));
                dummy->INDEX = 0;
            }
            break;
            
        case IO_IOCTL_I2S_TX_DUMMY_OFF:
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            if (io_info_ptr->TX_DUMMY == TRUE)
            {
                io_info_ptr->TX_DUMMY = FALSE;
                result = _mem_free(dummy->DATA);
                if (result == MQX_OK)
                {
                    dummy->LENGTH = 0;
                    dummy->DATA = NULL;
                }
                dummy->INDEX = 0;
            }
            break;
#endif 
        case IO_IOCTL_I2S_GET_MODE:
            if (param == NULL)
            {
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((uint_8_ptr) param) = io_info_ptr->MODE;
            break;
        
        case IO_IOCTL_I2S_GET_CLOCK_SOURCE:
            if (param == NULL)
            {
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((uint_8_ptr) param) = io_info_ptr->CLOCK_SOURCE;
            break;
        
        case IO_IOCTL_I2S_GET_DATA_BITS:
             if (param == NULL)
             {
                  I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter."); 
                  result = I2S_ERROR_INVALID_PARAMETER;
                  break;
             }
             *((uint_8_ptr) param) = io_info_ptr->DATA_BITS;
             break;
            
        case IO_IOCTL_I2S_GET_MCLK_FREQ:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            else
            {
                *((uint_32_ptr)param) = io_info_ptr->MCLK_FREQ;
            }
            result = MQX_OK;
            break;
                
        case IO_IOCTL_I2S_GET_BCLK_FREQ:
            if (param == NULL)
            {
                 I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((uint_32_ptr) param) = io_info_ptr->BCLK_FREQ;
            break;
            
        case IO_IOCTL_I2S_GET_TX_DUMMY:
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            if (param == NULL)
            {
                 I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((boolean*) param) = io_info_ptr->TX_DUMMY;
            break;
            
        case IO_IOCTL_I2S_GET_FS_FREQ:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else
            {
                *((uint_32_ptr)param) = io_info_ptr->FS_FREQ;
            }
            result = MQX_OK;
            break;
            
        case IO_IOCTL_I2S_GET_STATISTICS:
            if (param == NULL)
            {
                 I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            _mem_copy(&(io_info_ptr->STATS), param, sizeof(io_info_ptr->STATS));
            break;
            
        case IO_IOCTL_I2S_SET_TXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else if (*((uint_8_ptr) param) > (SIZE_OF_FIFO - 1))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Parameter out of range.");
                result = I2S_ERROR_PARAM_OUT_OF_RANGE;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE) && (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            i2s_ptr->TCR1 = I2S_TCR1_TFW(*((uint_8_ptr) param));
            break;
            
        case IO_IOCTL_I2S_SET_RXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else if (*((uint_8_ptr) param) > (SIZE_OF_FIFO - 1))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Parameter out of range.");
                result = I2S_ERROR_PARAM_OUT_OF_RANGE;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_READ) && (io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            i2s_ptr->RCR1 = I2S_RCR1_RFW(*((uint_8_ptr) param));
            break;
            
        case IO_IOCTL_I2S_GET_TXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE) && (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            *((uint_8_ptr) param) = i2s_ptr->TCR1;
            break;

        case IO_IOCTL_I2S_GET_RXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_READ) && (io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            *((uint_8_ptr) param) = i2s_ptr->RCR1;
            break;
            
        case IO_IOCTL_I2S_CLEAR_STATISTICS:
            _mem_zero(&(io_info_ptr->STATS), sizeof(io_info_ptr->STATS));
            break;
            
        case IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT:
            if(io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Cannot change I/O data format while I/O actions are in progress.\n");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else
            {
                uint_8 aligment, endianity, bits, channels, size;
                
                endianity = ((AUDIO_DATA_FORMAT_PTR) param)->ENDIAN;
                bits = ((AUDIO_DATA_FORMAT_PTR) param)->BITS;
                aligment = ((AUDIO_DATA_FORMAT_PTR) param)->ALIGNMENT;
                channels = ((AUDIO_DATA_FORMAT_PTR) param)->CHANNELS;
                size = ((AUDIO_DATA_FORMAT_PTR) param)->SIZE;
                
                if ((aligment > AUDIO_ALIGNMENT_LEFT) || (bits < AUDIO_BIT_SIZE_MIN) ||
                    (bits > AUDIO_BIT_SIZE_MAX) || (endianity > AUDIO_LITTLE_ENDIAN)  ||
                    (channels > 2) || (channels == 0))
                {
                    I2S_LOG("\n  _i2s_ioctl: Error - Invalid I/O data format.");
                    result = AUDIO_ERROR_INVALID_IO_FORMAT;
                    break;
                }
                _mem_copy(param, &(io_info_ptr->IO_FORMAT), sizeof(AUDIO_DATA_FORMAT));
                /* Setup frame sync for transmitter */
                if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) || (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS))
                {
                    i2s_ptr->TCR4 &= ~(I2S_TCR4_FRSZ_MASK);
                    i2s_ptr->TCR4 |= (I2S_TCR4_FRSZ(1));
                    /* Mask second transmitter channel if there is only one data channel */
                    if (channels == 1)
                    {
                        i2s_ptr->TMR = 0x02;
                    }
                }
                /* Setup frame sync for receiver */
                if ((io_info_ptr->IO_MODE & I2S_IO_READ) || (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS))
                {
                    i2s_ptr->RCR4 &= ~(I2S_RCR4_FRSZ_MASK);
                    i2s_ptr->RCR4 |= (I2S_RCR4_FRSZ(1));
                    /* Mask second receiver channel if there is only one data channel */
                    if (channels == 1)
                    {
                        i2s_ptr->RMR = 0x02;
                    }
                }
            }
            break;
            
        case IO_IOCTL_AUDIO_GET_IO_DATA_FORMAT:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            _mem_copy(&(io_info_ptr->IO_FORMAT), param, sizeof(io_info_ptr->IO_FORMAT));
            break;
            
        case IO_IOCTL_I2S_SET_CLOCK_MODE:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            bclk_mode = *((uint_8_ptr) param);
            if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) && !(io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                // Set receiver bit clock settings to zero if transmitter is being set.
                bclk_mode &= 0xF0; 
            }
            if ((io_info_ptr->IO_MODE & I2S_IO_READ) && !(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                // Set transmitter bit clock settings to zero if receiver is being set.
                bclk_mode &= 0x0F;
            }
            result = _ki2s_set_bclk_mode(i2s_ptr, bclk_mode);
            if (result != I2S_OK)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid BCLK setting selected.");
            }
            break;
            
        case IO_IOCTL_I2S_GET_CLOCK_MODE:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            *((uint_8_ptr) param) = io_info_ptr->CLOCK_MODE;
            break;
            
        case IO_IOCTL_FLUSH_OUTPUT:
            while (io_info_ptr->ONTHEWAY)
            {
                uint_32 time = 0;
                time = (io_info_ptr->BUFFER.SIZE - io_info_ptr->BUFFER.SPACE) * (1000 / io_info_ptr->FS_FREQ) + 1;
                _time_delay(time);
            }
            break;
        default:
            I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }
    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_hw_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an SAI hardware control registers.
*
*END*********************************************************************/

void _ksai_hw_init
(
    /* [IN] SAI init parameter pointer */
    KSAI_INIT_STRUCT_PTR ksai_init_ptr,
    
    /* [IN] SAI register pointer */
    I2S_MemMapPtr i2s_ptr,
    
    /* [IN] I/O mode (read/write) */
    _mqx_int io_mode
)
{    
    uint_8 data_bits  = ksai_init_ptr->DATA_BITS;
    uint_8 i2s_mode   = ksai_init_ptr->MODE;
    uint_8 clk_source = ksai_init_ptr->CLOCK_SOURCE;
    uint_8 bclk_mode  = ksai_init_ptr->CLOCK_MODE;
    uint_16 fract = 0;
    uint_16 div = 0;
    boolean set_tx = io_mode & I2S_IO_WRITE;
    boolean set_rx = io_mode & I2S_IO_READ;
#if PSP_MQX_CPU_IS_VYBRID
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
#endif
    /* 
    ** Set the divider input clock to system clock
    */
#if !PSP_MQX_CPU_IS_VYBRID
    i2s_ptr->MCR &= ~I2S_MCR_MICS_MASK;
#endif
    /* ================ General transmitter configuration =================== */ 
    if (set_tx)
    {
        uint_8 tx_channel = ksai_init_ptr->TX_CHANNEL;
        boolean tx_empty  = ksai_init_ptr->TX_DUMMY;
        /* 
        ** Software reset -> reset internal transmitter logic including the FIFO writer
        */
        i2s_ptr->TCSR = 0;
        i2s_ptr->TCSR |= I2S_TCSR_SR_MASK;
        i2s_ptr->TCSR &= ~(I2S_TCSR_SR_MASK);
        /*
        ** Enable transmitter in debug mode - this must be done else synchronous mode 
        ** will not work while debugging.
        */
        i2s_ptr->TCSR |= I2S_TCSR_DBGE_MASK; 
        /* 
        ** Set FIFO watermark
        */
        i2s_ptr->TCR1 = I2S_TCR1_TFW(I2S_TX_FIFO_DEFAULT_WATERMARK);      
        /* 
        ** Set the synch mode, clock polarity, master clock source and bit clock 
        */
        i2s_ptr->TCR2 = 0;
        i2s_ptr->TCR2 |= (
            I2S_TCR2_BCP_MASK | /* Bit clock active low           */
            I2S_TCR2_MSEL(1)  | /* MCLK from divider (CLKGEN)     */
            I2S_TCR2_DIV(3)     /* Bit clock frequency = MCLK / 8 */
            );
        /*
        ** First word in frame sets start of word flag
        */
        i2s_ptr->TCR3 = 0;
        i2s_ptr->TCR3 |= (I2S_TCR3_WDFL(0));
        /* 
        ** Enable selected transmitter channel
        */
        switch (tx_channel)
        {
            case 0:
                i2s_ptr->TCR3 |= (I2S_TCR3_TCE(1));
                break;
            case 1:
                i2s_ptr->TCR3 |= (I2S_TCR3_TCE(2));
                break;
            default:
                break;
        }
        /*
        ** Configure frame size, sync width, MSB and frame sync
        */
        i2s_ptr->TCR4 = 0;
        i2s_ptr->TCR4 = (
            I2S_TCR4_FRSZ(1)           | /* Two words in each frame          */
            I2S_TCR4_SYWD(data_bits-1) | /* Synch width as long as data word */
            I2S_TCR4_MF_MASK           | /* MSB shifted out first            */
            I2S_TCR4_FSE_MASK            /* Frame sync one bit early         */
            );
        /* 
        ** Fist bit shifted is always MSB (bit with highest index in register)
        ** and word length is set according to user configuration.
        */
        i2s_ptr->TCR5 = 0;
        i2s_ptr->TCR5 = (
            I2S_TCR5_WNW(data_bits-1) | /* Word length for Nth word is data_bits   */
            I2S_TCR5_W0W(data_bits-1) | /* Word length for 0th word is data_bits   */
            I2S_TCR5_FBT(data_bits)     /* First bit shifted is on data_bits index */
            );
        /*
        ** Reset word mask 
        */
        i2s_ptr->TMR = 0;
     
    }
    
    /* ================== General receiver configuration ==================== */  
    if (set_rx)
    {
        uint_8 rx_channel = ksai_init_ptr->RX_CHANNEL;
        i2s_ptr->RCSR = 0;
        /* 
        ** Software reset -> reset internal receiver logic including the FIFO writer
        */
        i2s_ptr->RCSR |= I2S_RCSR_SR_MASK;
        i2s_ptr->RCSR &= ~(I2S_RCSR_SR_MASK);
        /*
        ** Enable receiver in debug mode - this must be done else synchronous mode 
        ** will not work while debugging.
        */
        i2s_ptr->RCSR |= I2S_RCSR_DBGE_MASK;
        /* 
        ** Set FIFO watermark
        */
        i2s_ptr->RCR1 = I2S_RCR1_RFW(I2S_RX_FIFO_DEFAULT_WATERMARK);   
        /* 
        ** Set the clock polarity and master clock source 
        */
        i2s_ptr->RCR2 = 0;
        i2s_ptr->RCR2 |= (
            I2S_RCR2_BCP_MASK |                          /* Bit clock active low           */
            I2S_RCR2_MSEL(1)  |                          /* MCLK from divider (CLKGEN)     */
            I2S_RCR2_DIV((DEFAULT_BCLK_DIV / 2) - 1)     /* Bit clock frequency = MCLK / 8 */
            );
        /*
        ** First word in frame sets start of word flag
        */
        i2s_ptr->RCR3 |= (I2S_RCR3_WDFL(0));
        /* 
        ** Enable selected receiver channel
        */
        switch (rx_channel)
        {
            case 0:
                i2s_ptr->RCR3 |= (I2S_RCR3_RCE(1));
                break;
            case 1:
                i2s_ptr->RCR3 |= (I2S_RCR3_RCE(2));
                break;
            default:
                break;
        }
        /*
        ** Configure frame size, sync width, MSB and frame sync
        */
        i2s_ptr->RCR4 = 0;
        i2s_ptr->RCR4 = (
            I2S_RCR4_FRSZ(1)           | /* Two words in each frame          */
            I2S_RCR4_SYWD(data_bits-1) | /* Synch width as long as data word */
            I2S_RCR4_MF_MASK           | /* MSB shifted out first            */
            I2S_RCR4_FSE_MASK            /* Frame sync one bit early         */
            );
        /* 
        ** Fist bit shifted is always MSB (bit with highest index in register)
        ** and word length is set according to user configuration.
        */
        i2s_ptr->RCR5 = 0;
        i2s_ptr->RCR5 = (
            I2S_RCR5_WNW(data_bits-1) | /* Word length for Nth word is data_bits   */
            I2S_RCR5_W0W(data_bits-1) | /* Word length for 0th word is data_bits   */
            I2S_RCR5_FBT(data_bits-1)     /* First bit shifted is on data_bits index */
            );
        /*
        ** Reset word mask 
        */
        i2s_ptr->RMR = 0;
    }
    
    /*================= I2S clock source specific code =======================*/
    switch (clk_source)
    {
        case I2S_CLK_INT:
            /* 
            ** Set divider to get the 12.2880 MHz master clock
            */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_int(ksai_init_ptr->HW_CHANNEL);
            _vi2s_set_sai_div(ksai_init_ptr->HW_CHANNEL, 6);
#else
            _ki2s_find_mclk_freq_div(ksai_init_ptr->MCLK_SRC, &fract, &div, 12288000);
            i2s_ptr->MDR = (I2S_MDR_DIVIDE(div-1) | I2S_MDR_FRACT(fract-1));
            while(i2s_ptr->MCR & I2S_MCR_DUF_MASK) {}; // wait until new value is set
            /* 
            ** MCLK signal pin is configured as an output from the MCLK divider
            ** and the MCLK divider is enabled.
            */
            i2s_ptr->MCR |= (I2S_MCR_MOE_MASK);
            while (i2s_ptr->MCR & I2S_MCR_DUF_MASK) {};
#endif
            break;
        case I2S_CLK_EXT:
            /* 
            ** MCLK signal pin is configured as an input that bypasses
            ** the MCLK divider.
            */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_ext(ksai_init_ptr->HW_CHANNEL);
#else
            i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
            while (i2s_ptr->MCR & I2S_MCR_DUF_MASK) {};
#endif
            break;
        default:
            break;
    }
    /*================= I2S mode of operation specific code ===================*/
    if (i2s_mode & I2S_TX_MASTER)
    {
        /* 
        ** Transmit bit clock generated internally
        */
        i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
        /*
        ** Transmit frame sync generated internally
        */
        i2s_ptr->TCR4 |= (I2S_TCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_RX_MASTER)
    {
        /*
        ** Receive bit clock generated internally
        */
        i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
        /* 
        ** Receive frame sync generated internally
        */
        i2s_ptr->RCR4 |= (I2S_RCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_TX_SLAVE)
    {
        /*
        ** Transmit bit clock generated externally
        */
        i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
        /* 
        ** Transmit frame sync generated externally
        */
        i2s_ptr->TCR4 &= ~(I2S_TCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_RX_SLAVE)
    {
        /* 
        ** Receive bit clock generated externally
        */
        i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
        /*
        ** Receive frame sync generated externally
        */
        i2s_ptr->RCR4 &= ~(I2S_RCR4_FSD_MASK);
    }
    /*========================== I2S clock setup =============================*/
    _ki2s_set_bclk_mode(i2s_ptr, bclk_mode);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_format_in_data
* Returned Value   : formatted input data
* Comments         : Function performs SAI input data formatting
*
*END*********************************************************************/

static uint_32 _ksai_format_in_data
(
    /* [IN] data to format */
    char_ptr input,

    /* [IN] Requested format */
    AUDIO_DATA_FORMAT_PTR format
)
{/* Body */
   int_32 retval = 0;
   uint_8 i;
   
   if (format->ENDIAN == AUDIO_BIG_ENDIAN)
   {
      for (i = 0; i < format->SIZE; i++)
      {
          retval |= ((((uint_32) *(input + i)) << (format->SIZE - 1 - i) * 8) & (0xFF) << (format->SIZE - 1 - i) * 8);
      }
      if ((format->ALIGNMENT == AUDIO_ALIGNMENT_RIGHT) && (format->BITS < (format->SIZE * 8)))
      {
         retval <<= (format->SIZE * I2S_CHAR_BIT) - format->BITS;
      }
   }
   else
   {
      for (i = 0; i < format->SIZE; i++)
      {
          retval |= ((uint_32) *(input + i) << i * 8);
      }
   }
   if (retval & (1 << (format->SIZE * 8) - 1))
   {
       switch (format->SIZE)
       {
           case 1:
                retval |= 0xFFFFFF00;
                break;
           case 2:
                retval |= 0xFFFF0000;
                break;
           case 3:
                retval |= 0xFF000000;
           default:
                break;
       }
   }
   return(~((unsigned) retval));
}/* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_format_out_data
* Returned Value   : none
* Comments         : Function performs SAI output data formatting
*
*END*********************************************************************/

static void _ksai_format_out_data
(
    /* [IN] data to format */
    uint_32 input,

    /* [OUT] location for output */
    char_ptr output,

    /* [IN] Requested format */
    AUDIO_DATA_FORMAT_PTR format
)
{
    uint_8 i;
    input = ~input;
    
    if (format->ENDIAN == AUDIO_BIG_ENDIAN)
    {
        for(i = 0; i < format->SIZE; i++)
        {
            output[i] = (input >> I2S_CHAR_BIT * (format->SIZE - i - 1) & 0xFF);
        }
        if ((format->ALIGNMENT == AUDIO_ALIGNMENT_LEFT) && (format->BITS < (format->SIZE * 8)))
        {
            input <<= (format->SIZE * I2S_CHAR_BIT) - format->BITS;
        }
    }
    else
    {
        for(i = 0; i < format->SIZE; i++)
        {
            output[i] = (input & (0xFF << (i * I2S_CHAR_BIT))) >> (i * I2S_CHAR_BIT);
        }
    }
    
    return;
}

#ifndef I2S_USE_FLOAT // Floating point enabled
/*FUNCTION****************************************************************
*
* Function Name    : _ksai_sine_wave
* Returned Value   : samples number if successful, 0 otherwise
* Comments         :
*    This function generate a sine wave.
*
*END*********************************************************************/

static uint_32 _ksai_sine_wave
(
    /* [IN] Sample rate frequency */
    uint_32 fs, 
    
    /* [IN] Input frequency */
    uint_32 freq, 

    /* [OUT] Pointer to data */
    int_16_ptr* output
)
{
    uint_32 samples, i;
    boolean over180 = 0;
    ieee_single period, step, angle = 0.0;
    period = 1 / (ieee_single) freq;
    samples = (uint_32) (fs * period);
    
    if (samples == 0)
    {
        return(0);
    }
    *output = (int_16_ptr) _mem_alloc(sizeof(int_16) * samples);
    if (*output == NULL)
    {
        return(0);
    }
    step = 360.0 / (ieee_single) samples;
    for(i = 0; i < samples; i++)
    {
        if (angle > 180)
        {
            angle = angle - 180;
            over180 = TRUE;
        }
        *((*output) + i) = (int_16) (BIT16_MAX * (4*angle*(180 - angle))/(40500 - angle * (180 - angle)));
        if (over180)
        {
            *((*output) + i) = -(*((*output) + i));
        }
        angle += step;
    }
    return(samples);
}
#endif // I2S_USE_FLOAT - Floating point enabled

#if PSP_MQX_CPU_IS_VYBRID // PSP_MQX_CPU_IS_VYBRID - Functions specific for Vybrid
/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_get_mclk_value
* Returned Value   : Master clock frequency
* Comments         :
*    This function calculates master clock value from FRACT and DIV.
*
*END*********************************************************************/

static uint_32 _vi2s_get_mclk_value
(
   KSAI_INFO_STRUCT_PTR io_info_ptr /* Device internal info struct */
)
{
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    CCM_MemMapPtr ccm = CCM_BASE_PTR;

    CM_CLOCK_SOURCE src = io_info_ptr->MCLK_SRC;
    uint_32 source_freq = 0;
    uint_32 mclk_freq = 0;
    source_freq = _bsp_get_clock(_bsp_get_clock_configuration(), src);
    
    mclk_freq = source_freq / (((ccm->CACRR & CCM_CACRR_PLL4_CLK_DIV_MASK) >> CCM_CACRR_PLL4_CLK_DIV_SHIFT)*2 + 2);
    switch (io_info_ptr->HW_CHANNEL)
    {
        case 0:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI0_DIV_MASK) >> CCM_CSCDR1_SAI0_DIV_SHIFT) + 1;
            break;
        case 1:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI1_DIV_MASK) >> CCM_CSCDR1_SAI1_DIV_SHIFT) + 1;
            break;
        case 2:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI2_DIV_MASK) >> CCM_CSCDR1_SAI2_DIV_SHIFT) + 1;
            break;
        case 3:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI3_DIV_MASK) >> CCM_CSCDR1_SAI3_DIV_SHIFT) + 1;
            break;
        default:
            break;
    }
    return (mclk_freq);
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_mclk_value
* Returned Value   : None
* Comments         :
*    This function sets master clock frequency to closest to selected value.
*
*END*********************************************************************/
static void _vi2s_set_mclk_freq(KSAI_INFO_STRUCT_PTR io_info_ptr, uint_32 freq)
{
    uint_32 remainders[] = {
                            CPU_PLL_AUDIO_CLK_HZ_0,
                            CPU_PLL_AUDIO_CLK_HZ_1,
                            CPU_PLL_AUDIO_CLK_HZ_2
                            };
    uint_32 result = 0;
    uint_32 source_freq = 0;
    uint_32 iter_mclk = 0;
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    uint_8 pll_div = 4;
    uint_8 sai_div = 1;
    uint_8 temp_pll_div = 0;
    uint_8 temp_sai_div = 0;
    boolean found = FALSE;

    /* Calculate remainders */
    remainders[0] %= freq;
    remainders[1] %= freq; 
    remainders[2] %= freq;
    
    /* Select closest PLL4 frequency */
    if (remainders[0] < remainders[1])
    {
        _bsp_set_clock(CPU_PLL_AUDIO_CLK_HZ_0, io_info_ptr->MCLK_SRC);
    }
    else if (remainders[1] < remainders[2])
    {
        _bsp_set_clock(CPU_PLL_AUDIO_CLK_HZ_1, io_info_ptr->MCLK_SRC);
    }
    else
    {
        _bsp_set_clock(CPU_PLL_AUDIO_CLK_HZ_2, io_info_ptr->MCLK_SRC);
    }
    
    /* Read PLL4 freqency */
    source_freq =  _bsp_get_clock(_bsp_get_clock_configuration(), io_info_ptr->MCLK_SRC);
    
    /* Find dividers values for selecter master clock frequency */
    for (; pll_div < PLL_DIV_MAX; pll_div += 2)
    {
       for(sai_div = 1; sai_div < SAI_DIV_MAX; sai_div++)
       {
           uint_32 iter_mclk = source_freq / (pll_div * sai_div);
           if (iter_mclk < freq) break; // Value too low, try next setting
           if ((iter_mclk - freq) == 0) // Found value that matches exactly 
           {
               found = TRUE;
               break;
           }
           /* 
           ** If a new iteration of mclk is better than last that we have adapt 
           ** it as the new best value.
           */
           else if ((result == 0) || ((iter_mclk % freq) < (result % freq)))
           {
               temp_pll_div = pll_div;
               temp_sai_div = sai_div;
               result = iter_mclk;
           }
       }
       if (found) break;
    }
    if (found == FALSE)
    {
       pll_div = temp_pll_div;
       sai_div = temp_sai_div;
    }
    /* Set PLL4 divider */
    ccm->CACRR = (ccm->CACRR & ~CCM_CACRR_PLL4_CLK_DIV_MASK) | CCM_CACRR_PLL4_CLK_DIV(pll_div/2 - 1);
    
    /* Set SAI divider */
    _vi2s_set_sai_div(io_info_ptr->HW_CHANNEL, sai_div);
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_clk_src_int
* Returned Value   : None
* Comments         :
*    This function selects internal clock source for appropriate SAI peripheral
*
*END*********************************************************************/
static void _vi2s_set_clk_src_int(uint_8 hw_channel)
{
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    
    switch (hw_channel)
    {
        case 0: //SAI0_CLK_SEL
            /* Select internal clock source (PLL4) */
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI0_CLK_SEL_MASK) | CCM_CSCMR1_SAI0_CLK_SEL(3);
            /* Enable SAI0 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI0_EN_MASK;
            break;
        case 1: //SAI1_CLK_SEL
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI1_CLK_SEL_MASK) | CCM_CSCMR1_SAI1_CLK_SEL(3);
            /* Enable SAI1 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI1_EN_MASK;
            break;
        case 2: //SAI2_CLK_SEL
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI2_CLK_SEL_MASK) | CCM_CSCMR1_SAI2_CLK_SEL(3);
            /* Enable SAI2 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI2_EN_MASK;
            break;
        case 3: //SAI3_CLK_SEL
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI3_CLK_SEL_MASK) | CCM_CSCMR1_SAI3_CLK_SEL(3);
            /* Enable SAI3 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI3_EN_MASK;
            break;
        default:
            break;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_clk_src_ext
* Returned Value   : None
* Comments         :
*    This function selects external clock source for appropriate SAI peripheral
*
*END*********************************************************************/
static void _vi2s_set_clk_src_ext(uint_8 hw_channel)
{
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    
    switch (hw_channel)
    {
        case 0: //SAI0_CLK_SEL
            /* Clock source for SAI0 is Audio External Clock */
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI0_CLK_SEL_MASK);
            break;
        case 1: //SAI1_CLK_SEL
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK);
            break;
        case 2: //SAI2_CLK_SEL
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI2_CLK_SEL_MASK);
            break;
        case 3: //SAI3_CLK_SEL
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI3_CLK_SEL_MASK);
            break;
        default:
            break;
    }
    /* Set SAI clock divider to 1 (do not divide external clock) */
    _vi2s_set_sai_div(hw_channel, 1);
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_sai_div
* Returned Value   : None
* Comments         :
*    This function sets SAI clock divider for selected SAI peripheral
*
*END*********************************************************************/
static void _vi2s_set_sai_div(uint_8 hw_channel, uint_8 value)
{
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    
    switch (hw_channel)
    {
        case 0: //SAI0
            ccm->CSCDR1 = (ccm->CSCDR1 & ~CCM_CSCDR1_SAI0_DIV_MASK | CCM_CSCDR1_SAI0_DIV(value-1));
            break;
        case 1: //SAI1
            ccm->CSCDR1 = (ccm->CSCDR1 & ~CCM_CSCDR1_SAI1_DIV_MASK | CCM_CSCDR1_SAI1_DIV(value-1));
            break;
        case 2: //SAI2
            ccm->CSCDR1 = (ccm->CSCDR1 & ~CCM_CSCDR1_SAI2_DIV_MASK | CCM_CSCDR1_SAI2_DIV(value-1));
            break;  
        case 3: //SAI3
            ccm->CSCDR1 = (ccm->CSCDR1 & ~CCM_CSCDR1_SAI3_DIV_MASK | CCM_CSCDR1_SAI3_DIV(value-1));
            break;
        default:
            break;
    }
}
#else // !PSP_MQX_CPU_IS_VYBRID - Functions specific for Kinetis
/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_get_mclk_value
* Returned Value   : Master clock frequency
* Comments         :
*    This function calculates master clock value from FRACT and DIV.
*
*END*********************************************************************/
static uint_32 _ki2s_get_mclk_value
(
   KSAI_INFO_STRUCT_PTR io_info_ptr
)
{
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    uint_32 freq = _bsp_get_clock(_bsp_get_clock_configuration(), io_info_ptr->MCLK_SRC);
    uint_32 fract = 0;
    uint_32 div = 0;
    fract = (i2s_ptr->MDR & I2S_MDR_FRACT_MASK) >> I2S_MDR_FRACT_SHIFT;
    div = (i2s_ptr->MDR & I2S_MDR_DIVIDE_MASK) >> I2S_MDR_DIVIDE_SHIFT;
    return ((freq / (div + 1)) * (fract + 1));
}

#endif // END OF PSP_MQX_CPU_IS_VYBRID - Functions specific for platform

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_find_mclk_freq_div
* Returned Value   : Master clock frequency  
* Comments         :
*    This function tries to find master clock divider value required for 
*    selected master clock frequency.
*
*END*********************************************************************/
static uint_32 _ki2s_find_mclk_freq_div
(
    CM_CLOCK_SOURCE src,      /* Master clock source               */
    uint_16_ptr     frct,     /* Calculated FRACT value            */
    uint_16_ptr     div,      /* Calculated DIVIDER value          */
    uint_32         frequency /* Calculated master clock frequency */
)
{
    uint_32 closest_mclk = 0;
    uint_32 temp_fract;
    uint_32 temp_div;
    uint_32 iter_mclk;
    uint_16 fract = 0;
    uint_16 divide = 0;
    boolean found = FALSE;
    uint_32 i = 0;
    uint_32 source_freq = 0;

    source_freq = _bsp_get_clock(_bsp_get_clock_configuration(), src);
    /*
    ** Find best divider value for selected mclk frequency.
    */
    for (fract = 1; fract < FRACT_MAX; fract++)
    {
       for(divide = 1; divide < DIV_MAX; divide++)
       {
           iter_mclk = (source_freq / divide) * fract;
           if (iter_mclk < frequency) break; // Value too low, try next setting
           if ((iter_mclk - frequency) == 0) // Found value that matches exactly 
           {
               found = TRUE;
               break;
           }
           /* 
           ** If a new iteration of mclk is better than last that we have adapt 
           ** it as the new best value.
           */
           else if ((closest_mclk == 0) || ((iter_mclk % frequency) < (closest_mclk % frequency)))
           {
               temp_fract = fract;
               temp_div = divide;
               closest_mclk = iter_mclk;
           }
       }
       if (found) break;
    }
    /* 
    ** If we failed to find exact match use closest value we have.
    */
    if (found == FALSE)
    {
       fract = temp_fract;
       divide = temp_div;
       frequency = closest_mclk;
    }
    *frct = fract;
    *div = divide;
    return(frequency);
}
/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_master_mode
* Returned Value   : I2S_OK if everything is OK, error code otherwise.  
* Comments         :
*    This function sets up master mode for TX and RX (called from ioctl)
*
*END*********************************************************************/
static uint_32 _ki2s_set_master_mode
(
    KSAI_INFO_STRUCT_PTR io_info_ptr, /* Info struct                   */
    uint_8 mode                       /* Required master mode settings */
)
{
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    boolean set_tx_as_master = ((mode & I2S_TX_MASTER) >> 4) && (io_info_ptr->IO_MODE & I2S_IO_WRITE);
    boolean set_rx_as_master = (mode & I2S_RX_MASTER) && (io_info_ptr->IO_MODE & I2S_IO_READ);
    /* 
    ** If the master clock source is internal, enable it and recalculate
    ** values of frequencies in the internal info structure.
    */
    if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
    {
#if PSP_MQX_CPU_IS_VYBRID
        _vi2s_set_clk_src_int(io_info_ptr->HW_CHANNEL);
        io_info_ptr->MCLK_FREQ = _vi2s_get_mclk_value(io_info_ptr);
#else
        i2s_ptr->MCR |= (I2S_MCR_MOE_MASK);
        io_info_ptr->MCLK_FREQ = _ki2s_get_mclk_value(io_info_ptr); 
#endif   
        io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
        io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * 2/*io_info_ptr->IO_FORMAT.CHANNELS*/);
    }
    /*
    ** Else disable master clock signal and set pin as input. Also reset values
    ** of frequencies in the info structure.
    */
    else if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_EXT)
    {
#if PSP_MQX_CPU_IS_VYBRID
        _vi2s_set_clk_src_ext(io_info_ptr->HW_CHANNEL);
#else
        i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
        io_info_ptr->MCLK_FREQ = 0;
        io_info_ptr->BCLK_FREQ = 0;
        io_info_ptr->FS_FREQ = 0;
    }
    /*
    ** In case we want to set the transmitter and it is enabled, device is busy.
    */
    if ((i2s_ptr->TCSR & I2S_TCSR_TE_MASK) && (set_tx_as_master))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_TX_ASYNCHRONOUS) && (set_tx_as_master))
    {
        /* Reset Tx mode setting */
        io_info_ptr->MODE &= 0x0F;
        /* Set Tx into master mode */
        io_info_ptr->MODE |= I2S_TX_MASTER;
        i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
        i2s_ptr->TCR4 |= (I2S_TCR4_FSD_MASK);
        i2s_ptr->TCR4 |= (I2S_TCR4_FSP_MASK);
        /* 
        ** If the the receiver is synchronous settings must same as the
        ** transmitter settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
        {
            io_info_ptr->MODE &= 0xF0;
            io_info_ptr->MODE |= I2S_RX_MASTER;
            i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
            i2s_ptr->RCR4 |= (I2S_RCR4_FSD_MASK);
        }
        /* 
        ** If transmitter have swapped bit clock. set receiver bit clock to be
        ** generated internally
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_BCLK_SWAPPED)
        {
            i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
        }
    }
    
    /*
    ** In case we want to the set receiver and it is enabled, device is busy.
    */
    if ((i2s_ptr->RCSR & I2S_RCSR_RE_MASK) && (set_rx_as_master))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_RX_ASYNCHRONOUS) && (set_rx_as_master))
    {
        /* Reset Rx mode setting */
        io_info_ptr->MODE &= 0xF0;
        /* Set Rx in master mode */
        io_info_ptr->MODE |= I2S_RX_MASTER;
        i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
        i2s_ptr->RCR4 |= (I2S_RCR4_FSD_MASK);
        /* 
        ** If the the transmitter is synchronous settings must same as the
        ** receiver settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
        {
            
            io_info_ptr->MODE &= 0x0F;
            io_info_ptr->MODE |= I2S_TX_MASTER;
            i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
            i2s_ptr->TCR4 |= (I2S_TCR4_FSD_MASK);
        }
        /* 
        ** If receiver have swapped bit clock. Set transmitter bit clock to be
        ** generated internally.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_BCLK_SWAPPED)
        {
            i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
        }
    }
    return(I2S_OK);
}
/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_slave_mode
* Returned Value   : I2S_OK if everything is OK, error code otherwise.  
* Comments         :
*    This function sets up slave mode for TX and RX (called from ioctl)
*
*END*********************************************************************/
static uint_32 _ki2s_set_slave_mode
(
    KSAI_INFO_STRUCT_PTR io_info_ptr, /* Info struct                  */
    uint_8 mode                       /* Required slave mode settings */
)
{
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    boolean set_tx_as_slave = ((mode & I2S_TX_SLAVE) >> 4) && (io_info_ptr->IO_MODE & I2S_IO_WRITE);
    boolean set_rx_as_slave = (mode & I2S_RX_SLAVE) && (io_info_ptr->IO_MODE & I2S_IO_READ);
    /*
    ** Disable the master clock signal and set pin as input. Also reset values
    ** of frequencies in the info structure.
    */
#if PSP_MQX_CPU_IS_VYBRID
    _vi2s_set_clk_src_ext(io_info_ptr->HW_CHANNEL);
#else
    i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
    io_info_ptr->MCLK_FREQ = 0;
    io_info_ptr->BCLK_FREQ = 0;
    io_info_ptr->FS_FREQ = 0;
    /*
    ** In case we want to setup the transmitter and it is enabled, device is busy.
    */
    if ((i2s_ptr->TCSR & I2S_TCSR_TE_MASK) && (set_tx_as_slave))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_TX_ASYNCHRONOUS) && (set_tx_as_slave))
    {
        /* Reset Tx mode setting */
        io_info_ptr->MODE &= 0x0F;
        /* Set Tx into slave mode */
        io_info_ptr->MODE |= I2S_TX_SLAVE;
        i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
        i2s_ptr->TCR4 &= ~(I2S_TCR4_FSD_MASK);
        /* 
        ** If the the receiver is synchronous settings must same as the
        ** transmitter settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
        {
            io_info_ptr->MODE &= 0xF0;
            io_info_ptr->MODE |= I2S_RX_SLAVE;
            i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
            i2s_ptr->RCR4 &= ~(I2S_RCR4_FSD_MASK);
        }
        /* 
        ** If transmitter have swapped bit clock set receiver bit clock to be
        ** generated externally
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_BCLK_SWAPPED)
        {
            i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
        }
    }
    
    /*
    ** In case we want to the setup receiver and it is enabled, device is busy.
    */
    if ((i2s_ptr->RCSR & I2S_RCSR_RE_MASK) && (set_rx_as_slave))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_RX_ASYNCHRONOUS) && (set_rx_as_slave))
    {
        /* Reset Rx mode setting */
        io_info_ptr->MODE &= 0xF0;
        /* Set Rx in master mode */
        io_info_ptr->MODE |= I2S_RX_SLAVE;
        i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
        i2s_ptr->RCR4 &= ~(I2S_RCR4_FSD_MASK);
        /* 
        ** If the the transmitter is synchronous settings must same as the
        ** receiver settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
        {
            
            io_info_ptr->MODE &= 0x0F;
            io_info_ptr->MODE |= I2S_TX_SLAVE;
            i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
            i2s_ptr->TCR4 &= ~(I2S_TCR4_FSD_MASK);
        }
        /* 
        ** If receiver have swapped bit clock. Set transmitter bit clock to be
        ** generated externally.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_BCLK_SWAPPED)
        {
            i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
        }
    }
    return(I2S_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_bclk_mode
* Returned Value   : I2S_OK if everything is OK, error code otherwise.  
* Comments         :
*       This function modifies SAI registers to set up synchronous mode and
*       clock swapping.
*
*END*********************************************************************/
static uint_32 _ki2s_set_bclk_mode
(
    I2S_MemMapPtr i2s_ptr, /* Pointer to registers */
    uint_8 bclk_mode       /* Bit clock mode       */
)
{
    if ((bclk_mode & I2S_TX_SYNCHRONOUS) && (bclk_mode & I2S_RX_SYNCHRONOUS))
    {
        /* Invalid combination of bclk modes */
        return(I2S_ERROR_INVALID_PARAMETER);
    }
    /*
    ** Transmitter clock:
    */
    if (bclk_mode & I2S_TX_ASYNCHRONOUS) // Transmitter is asynchronous
    {
       i2s_ptr->TCR2 &= ~(I2S_TCR2_SYNC_MASK);
    }
    else if (bclk_mode & I2S_TX_SYNCHRONOUS) // Transmitter is synchronous
    {
       /*
       ** If transmitter is synchronous, receiver must be asynchronous
       */
       if (i2s_ptr->RCR2 & I2S_RCR2_SYNC_MASK)
       {
          /* Invalid combination of bclk modes */
          return(I2S_ERROR_INVALID_PARAMETER); 
       }
       i2s_ptr->TCR2 |= (I2S_TCR2_SYNC(1));
    }
   
    if (bclk_mode & I2S_TX_BCLK_NORMAL) // Transmitter BCLK not swapped
    {
        i2s_ptr->TCR2 &= ~(I2S_TCR2_BCS_MASK);
    }
    else if (bclk_mode & I2S_TX_BCLK_SWAPPED) // Transmitter BCLK swapped
    {
       /*
       ** TX_BCLK = SAI_RX_BCLK
       ** TX_FS = SAI_TX_SYNC
       */
       i2s_ptr->TCR2 |= (I2S_TCR2_BCS_MASK);
       /*
       ** When Tx is synch. BCS bit must be set also for Rx.
       */
       if (bclk_mode & I2S_TX_SYNCHRONOUS) 
       {
           /*
           ** TX_BCLK = SAI_TX_BCLK
           ** TX_FS = SAI_RX_SYNC
           */
           i2s_ptr->RCR2 |= (I2S_RCR2_BCS_MASK);
       }
    }
   
    /*
    ** Receiver clock:
    */
    if (bclk_mode & I2S_RX_ASYNCHRONOUS) // Receiver is asynchronous
    {
        i2s_ptr->RCR2 &= ~(I2S_RCR2_SYNC_MASK);
    }
    else if (bclk_mode & I2S_RX_SYNCHRONOUS) // Receiver is synchronous
    {
        /*
        ** If receiver is synchronous, transmitter must be asynchronous
        */
        if (i2s_ptr->TCR2 & I2S_TCR2_SYNC_MASK)
        {
            /* Invalid combination of bclk modes */
            return(I2S_ERROR_INVALID_PARAMETER); 
        }
        i2s_ptr->RCR2 |= (I2S_RCR2_SYNC(1));
    }
   
    if (bclk_mode & I2S_RX_BCLK_NORMAL) // Receiver BCLK not swapped
    {
        i2s_ptr->RCR2 &= ~(I2S_RCR2_BCS_MASK);
    }
    else if (bclk_mode & I2S_RX_BCLK_SWAPPED) // Receiver BCLK swapped
    {
        /*
        ** RX_BCLK = SAI_TX_BCLK
        ** RX_FS = SAI_RX_SYNC
        */
        i2s_ptr->RCR2 |= (I2S_RCR2_BCS_MASK);
        /*
        ** When Rx is synch. BCS bit must be set also for Tx.
        */
        if (bclk_mode & I2S_RX_SYNCHRONOUS)
        {
            /*
            ** RX_BCLK = SAI_RX_BCLK
            ** RX_FS = SAI_TX_SYNC
            */
            i2s_ptr->TCR2 |= (I2S_TCR2_BCS_MASK);
        }
    }
    return(I2S_OK);
}
/* EOF */
