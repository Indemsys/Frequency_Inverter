/**HEADER********************************************************************
*
* Copyright (c) 2012 Freescale Semiconductor;
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
* $FileName: spi_dspi.c$
* $Version : 3.8.2.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   The file contains low level SPI driver functions for DSPI module
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>

#include "spi.h"
#include "spi_prv.h"
#include "spi_dspi_prv.h"


/* SPI low level driver interface functions */
static _mqx_int _dspi_init(const void _PTR_ init_data_ptr, pointer *io_info_ptr_ptr);
static _mqx_int _dspi_deinit(pointer io_info_ptr);
static _mqx_int _dspi_setparam(pointer io_info_ptr, SPI_PARAM_STRUCT_PTR params);
static _mqx_int _dspi_tx_rx(pointer io_info_ptr, uint_8_ptr txbuf, uint_8_ptr rxbuf, uint_32 len);
static _mqx_int _dspi_cs_deassert(pointer io_info_ptr);
static _mqx_int _dspi_ioctl(pointer io_info_ptr, SPI_PARAM_STRUCT_PTR params, uint_32 cmd, uint_32_ptr param_ptr);

const SPI_DEVIF_STRUCT _spi_dspi_devif = {
    _dspi_init,
    _dspi_deinit,
    _dspi_setparam,
    _dspi_tx_rx,
    _dspi_cs_deassert,
    _dspi_ioctl
};


/* Forward declarations */
static void _dspi_isr(pointer parameter);

static const uint_32 BAUDRATE_PRESCALER[] = { 2, 3, 5, 7 };
static const uint_32 BAUDRATE_SCALER[] = { 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

static const uint_32 DELAY_PRESCALER[] = { 1, 3, 5, 7 };

/*FUNCTION****************************************************************
*
* Function Name    : _dspi_find_baudrate
* Returned Value   : Actual baudrate obtained by programming the CTAR register
* Comments         :
*    Find closest setting of divider register for given baudrate.
*    Value of CTAR register is returned via pointer.
*
*END*********************************************************************/
static uint_32 _dspi_find_baudrate
    (
        /* [IN] Module input clock in Hz */
        uint_32 clock,

        /* [IN] Desired baudrate in Hz */
        uint_32 baudrate,

        /* [IN] Timing parameters of CTAR register to configure use */
        uint_32_ptr ctar
    )
{
    uint_32 pres, best_pres;
    uint_32 scaler, best_scaler;
    uint_32 dbr, best_dbr;
    uint_32 real_baudrate, best_baudrate;

    uint_32 delayrate;

    int_32  diff;
    uint_32 min_diff;

    /* find combination of prescaler and scaler resulting in baudrate closest to the requested value */
    min_diff = (uint_32)-1;
    best_pres = 0;
    best_scaler = 0;
    best_dbr = 1;
    best_baudrate = 0; /* required to avoid compilation warning */
    for (pres = 0; (pres < 4) && min_diff; pres++)
    {
        for (scaler = 0; (scaler < 16) && min_diff; scaler++)
        {
            for (dbr = 1; (dbr < 3) && min_diff; dbr++)
            {
                real_baudrate = ((clock * dbr) / (BAUDRATE_PRESCALER[pres] * (BAUDRATE_SCALER[scaler])));

                diff = baudrate - real_baudrate;
                if (diff < 0)
                    diff = -diff;

                if (min_diff > diff)
                {
                    /* a better match found */
                    min_diff = diff;
                    best_pres = pres;
                    best_scaler = scaler;
                    best_baudrate = real_baudrate;
                    best_dbr = dbr;
                }
            }
        }
    }

    if (ctar == NULL)
        return best_baudrate;

    /* store baudrate scaler and prescaler settings to ctar */
    *ctar = DSPI_CTAR_PBR(best_pres) | DSPI_CTAR_BR(best_scaler);
    *ctar |= ((best_dbr - 1) * DSPI_CTAR_DBR_MASK);

    /* similar lookup for delay prescalers */
    min_diff = (uint_32)-1;
    delayrate = baudrate * 4; /* double the baudrate (half period delay is sufficient) and divisor is (2<<scaler), thus times 4 */
    /* initialize prescaler and scaler to slowest possible rate to cover the case when no better match is found */
    best_pres = ELEMENTS_OF(BAUDRATE_PRESCALER)-1;
    best_scaler = ELEMENTS_OF(BAUDRATE_SCALER)-1;
    for (pres = 0; (pres < 4) && min_diff; pres++)
    {
        for (scaler = 0; scaler < 16; scaler++)
        {
            diff = delayrate - ((clock / DELAY_PRESCALER[pres])>>scaler);
            if (diff >= 0) {  /* To ensure that the delay time is larger than half period of baudrate */
               if (min_diff > diff)
               {
                  /* a better match found */
                  min_diff = diff;
                  best_pres = pres;
                  best_scaler = scaler;
               }
               else break; /* higher scaler value cannot provide with better match */
            }
        }
    }

    /* add delay scalers and prescaler settings to ctar */
    *ctar |= DSPI_CTAR_CSSCK(best_scaler) | DSPI_CTAR_PCSSCK(best_pres);
    *ctar |= DSPI_CTAR_DT(best_scaler) | DSPI_CTAR_PDT(best_pres);
    *ctar |= DSPI_CTAR_ASC(best_scaler) | DSPI_CTAR_PASC(best_pres); /* CS deasserted in sw, slow enough */

    return best_baudrate;
}


/*FUNCTION****************************************************************
*
* Function Name    : _dspi_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes the SPI driver
*
*END*********************************************************************/
static _mqx_int _dspi_init
    (
        /* [IN] The initialization information for the device being opened */
        const void _PTR_               init_data_ptr,

        /* [OUT] The address to store device specific information */
        pointer                        *io_info_ptr_ptr
    )
{
    DSPI_INIT_STRUCT_PTR               dspi_init_ptr = (DSPI_INIT_STRUCT_PTR)init_data_ptr;

    DSPI_INFO_STRUCT_PTR               dspi_info_ptr;
    VDSPI_REG_STRUCT_PTR               dspi_ptr;

    const uint_32 _PTR_                vectors;
    uint_32                            i;

    #if PSP_HAS_DEVICE_PROTECTION
    if (!_bsp_dspi_enable_access(dspi_init_ptr->CHANNEL)) {
        return SPI_ERROR_CHANNEL_INVALID;
    }
    #endif

    /* Check channel */
    dspi_ptr = _bsp_get_dspi_base_address (dspi_init_ptr->CHANNEL);
    if (NULL == dspi_ptr)
    {
        return SPI_ERROR_CHANNEL_INVALID;
    }

    if (_bsp_dspi_io_init (dspi_init_ptr->CHANNEL) == -1)
    {
        return SPI_ERROR_CHANNEL_INVALID;
    }

    /* Initialize internal data */
    dspi_info_ptr = (DSPI_INFO_STRUCT_PTR)_mem_alloc_system_zero((uint_32)sizeof(DSPI_INFO_STRUCT));
    if (dspi_info_ptr == NULL)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (dspi_info_ptr,MEM_TYPE_IO_SPI_INFO_STRUCT);

    *io_info_ptr_ptr = (pointer)dspi_info_ptr;

    dspi_info_ptr->DSPI_PTR = dspi_ptr;
    dspi_info_ptr->CHANNEL = dspi_init_ptr->CHANNEL;
    dspi_info_ptr->CLOCK_SOURCE = dspi_init_ptr->CLOCK_SOURCE;

    /* Disable and clear SPI */
    dspi_ptr->MCR &= (~ DSPI_MCR_MDIS_MASK);
    dspi_ptr->MCR = DSPI_MCR_HALT_MASK | DSPI_MCR_CLR_TXF_MASK | DSPI_MCR_CLR_RXF_MASK;

    #if DSPI_FIFO_DEPTH == 1
    /* Disable FIFO */
    dspi_ptr->MCR |= DSPI_MCR_DIS_RXF_MASK | DSPI_MCR_DIS_TXF_MASK;
    #endif

    /* Receive FIFO overflow disable */
    dspi_ptr->MCR |= DSPI_MCR_ROOE_MASK;

    /* Set CS0-7 inactive high */
    dspi_ptr->MCR |= DSPI_MCR_PCSIS(0xFF);

    /* Disable interrupts */
    dspi_ptr->RSER = 0;

    /* Clear all flags */
    dspi_ptr->SR = ~ DSPI_SR_TFFF_MASK;

    dspi_info_ptr->NUM_VECTORS = _bsp_get_dspi_vectors(dspi_info_ptr->CHANNEL, &vectors);

    /* Install ISRs */
    for (i=0; i<dspi_info_ptr->NUM_VECTORS; i++)
    {
        dspi_info_ptr->OLD_ISR_DATA[i] = _int_get_isr_data(vectors[i]);
        dspi_info_ptr->OLD_ISR[i] = _int_install_isr(vectors[i], _dspi_isr, dspi_info_ptr);
        _bsp_int_init((PSP_INTERRUPT_TABLE_INDEX)vectors[i], BSP_DSPI_INT_LEVEL, 0, TRUE);
    }

    _lwsem_create(&dspi_info_ptr->EVENT_IO_FINISHED, 0);

    /* Enable SPI */
    dspi_ptr->MCR &= (~ DSPI_MCR_HALT_MASK);

    return SPI_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _dspi_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes the SPI module
*
*END*********************************************************************/
static _mqx_int _dspi_deinit
    (
        /* [IN] the address of the device specific information */
        pointer                        io_info_ptr
    )
{
    DSPI_INFO_STRUCT_PTR               dspi_info_ptr = (DSPI_INFO_STRUCT_PTR)io_info_ptr;
    VDSPI_REG_STRUCT_PTR               dspi_ptr = dspi_info_ptr->DSPI_PTR;

    const uint_32 _PTR_                vectors;
    int                                num_vectors;
    int                                i;

    if (NULL == dspi_info_ptr)
    {
        return SPI_ERROR_DEINIT_FAILED;
    }

    /* Disable the SPI */
    dspi_ptr->MCR |= DSPI_MCR_HALT_MASK;

    /* Disable interrupts */
    dspi_ptr->RSER = 0;

    /* Return original interrupt vectors */
    num_vectors = _bsp_get_dspi_vectors(dspi_info_ptr->CHANNEL, &vectors);

    for (i=0; i<num_vectors; i++)
    {
        _int_install_isr(vectors[i], dspi_info_ptr->OLD_ISR[i], dspi_info_ptr->OLD_ISR_DATA[i]);
    }

    _lwsem_destroy(&dspi_info_ptr->EVENT_IO_FINISHED);

    _mem_free(dspi_info_ptr);
    return SPI_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _dspi_setparam
* Returned Value   :
* Comments         :
*    Set parameters for following transfers.
*
*END*********************************************************************/
static _mqx_int _dspi_setparam
   (
        /* [IN] Device specific context structure */
        pointer                        io_info_ptr,

        /* [IN] Parameters to set */
        SPI_PARAM_STRUCT_PTR           params
   )
{
    DSPI_INFO_STRUCT_PTR               dspi_info_ptr = (DSPI_INFO_STRUCT_PTR)io_info_ptr;
    VDSPI_REG_STRUCT_PTR               dspi_ptr = dspi_info_ptr->DSPI_PTR;

    BSP_CLOCK_CONFIGURATION clock_config;
    uint_32 clock_speed;

    uint_32 ctar;

    clock_config = _bsp_get_clock_configuration();

    /* Check the parameter against most recent values to avoid time consuming baudrate finding routine */
    if ((dspi_info_ptr->CLOCK_CONFIG != clock_config) || (dspi_info_ptr->BAUDRATE != params->BAUDRATE))
    {
        dspi_info_ptr->CLOCK_CONFIG = clock_config;
        dspi_info_ptr->BAUDRATE = params->BAUDRATE;

        /* Find configuration of prescalers best matching the desired value */
        clock_speed = _bsp_get_clock(dspi_info_ptr->CLOCK_CONFIG, dspi_info_ptr->CLOCK_SOURCE);
        _dspi_find_baudrate(clock_speed, dspi_info_ptr->BAUDRATE, &(dspi_info_ptr->CTAR_TIMING));
    }

    /* Set up prescalers */
    ctar = dspi_info_ptr->CTAR_TIMING;

    /* Set up SPI clock polarity and phase */
    if (params->MODE & SPI_CPHA_MASK)
        ctar |= DSPI_CTAR_CPHA_MASK;

    if (params->MODE & SPI_CPOL_MASK)
        ctar |= DSPI_CTAR_CPOL_MASK;

    /* Set up frame size */
    if (params->FRAMESIZE < 3 || params->FRAMESIZE > 16)
        return SPI_ERROR_FRAMESIZE_INVALID;
    ctar |= DSPI_CTAR_FMSZ(params->FRAMESIZE - 1);

    /* Transfer mode */
    if ((params->ATTR & SPI_ATTR_TRANSFER_MODE_MASK) != SPI_ATTR_MASTER_MODE)
        return SPI_ERROR_TRANSFER_MODE_INVALID;

    /* Endianess */
    if ((params->ATTR & SPI_ATTR_ENDIAN_MASK) == SPI_ATTR_LITTLE_ENDIAN)
        ctar |= DSPI_CTAR_LSBFE_MASK;

    /* Disable SPI module */
    dspi_ptr->MCR |= DSPI_MCR_HALT_MASK;

    dspi_ptr->CTAR[0] = ctar;

    /* Receive FIFO overflow disable */
    dspi_ptr->MCR |= DSPI_MCR_ROOE_MASK;

    /* Set CS0-7 inactive high */
    dspi_ptr->MCR |= DSPI_MCR_PCSIS(0xFF);

    /* Set master mode */
    dspi_ptr->MCR |= DSPI_MCR_MSTR_MASK;

    /* Disable interrupts */
    dspi_ptr->RSER = 0;

    /* Clear all flags */
    dspi_ptr->SR = ~ DSPI_SR_TFFF_MASK;

    /* Enable SPI module */
    dspi_ptr->MCR &= (~ DSPI_MCR_HALT_MASK);

    dspi_info_ptr->CS = params->CS;
    dspi_info_ptr->DUMMY_PATTERN = params->DUMMY_PATTERN;
    dspi_info_ptr->ATTR = params->ATTR;

    return SPI_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _dspi_isr
* Returned Value   : SPI interrupt routine
* Comments         :
*   State machine transferring data between buffers and DSPI FIFO.
*
*END*********************************************************************/
static void _dspi_isr
    (
        /* [IN] The address of the device specific information */
        pointer                        parameter
    )
{
    DSPI_INFO_STRUCT_PTR               dspi_info_ptr = parameter;
    VDSPI_REG_STRUCT_PTR               dspi_ptr = dspi_info_ptr->DSPI_PTR;

    uint_32                            command;
    uint_32                            data;

    /* drain RX FIFO */
    if (DSPI_CTAR_FMSZ_GET(dspi_ptr->CTAR[0]) > 7)
    {
        /* frame is larger than a single byte */
        while (dspi_ptr->SR & DSPI_SR_RFDF_MASK)
        {
            data = DSPI_POPR_RXDATA_GET(dspi_ptr->POPR);
            dspi_ptr->SR = DSPI_SR_RFDF_MASK;
            if (dspi_info_ptr->RX_LEN)
            {
                dspi_info_ptr->RX_LEN--;
                if (dspi_info_ptr->RX_BUF)
                {
                    dspi_info_ptr->RX_BUF[0] = data>>8;
                    dspi_info_ptr->RX_BUF[1] = data&0xff;
                    dspi_info_ptr->RX_BUF += 2;
                }
            }
        }
    }
    else
    {
        /* single byte frame */
        while (dspi_ptr->SR & DSPI_SR_RFDF_MASK)
        {
            data = DSPI_POPR_RXDATA_GET(dspi_ptr->POPR);
            dspi_ptr->SR = DSPI_SR_RFDF_MASK;
            if (dspi_info_ptr->RX_LEN)
            {
                dspi_info_ptr->RX_LEN--;
                if (dspi_info_ptr->RX_BUF)
                {
                    *(dspi_info_ptr->RX_BUF) = data;
                    dspi_info_ptr->RX_BUF++;
                }
            }
        }
    }

    /* check whether all requested data was received */
    if (dspi_info_ptr->RX_LEN == 0)
    {
        /* signalize finished job and disable further interrupts */
        dspi_ptr->RSER = 0;
        _lwsem_post(&dspi_info_ptr->EVENT_IO_FINISHED);
        return;
    }

    /* fill TX FIFO */
    command = DSPI_PUSHR_CONT_MASK | DSPI_PUSHR_PCS(dspi_info_ptr->CS) | DSPI_PUSHR_CTAS(0);
    if (DSPI_CTAR_FMSZ_GET(dspi_ptr->CTAR[0]) > 7)
    {
        /* frame is larger than a single byte */
        while (dspi_info_ptr->TX_LEN && (dspi_ptr->SR & DSPI_SR_TFFF_MASK) && ((dspi_info_ptr->RX_LEN-dspi_info_ptr->TX_LEN)<DSPI_FIFO_DEPTH))
        {
            dspi_info_ptr->TX_LEN--;
            if (dspi_info_ptr->TX_BUF)
            {
                data = dspi_info_ptr->TX_BUF[0];
                data = (data << 8) | dspi_info_ptr->TX_BUF[1];
                dspi_info_ptr->TX_BUF += 2;
            }
            else
            {
              data = dspi_info_ptr->DUMMY_PATTERN;
            }
            dspi_ptr->PUSHR = command | DSPI_PUSHR_TXDATA(data);
            dspi_ptr->SR = DSPI_SR_TFFF_MASK;
        }
    }
    else
    {
        /* single byte frame */
        while (dspi_info_ptr->TX_LEN && (dspi_ptr->SR & DSPI_SR_TFFF_MASK) && ((dspi_info_ptr->RX_LEN-dspi_info_ptr->TX_LEN)<DSPI_FIFO_DEPTH))
        {
            dspi_info_ptr->TX_LEN--;
            if (dspi_info_ptr->TX_BUF)
            {
                data = *(dspi_info_ptr->TX_BUF);
                dspi_info_ptr->TX_BUF ++;
            }
            else
            {
                data = dspi_info_ptr->DUMMY_PATTERN;
            }
            dspi_ptr->PUSHR = command | DSPI_PUSHR_TXDATA(data);
            dspi_ptr->SR = DSPI_SR_TFFF_MASK;
        }
    }
}


/*FUNCTION****************************************************************
*
* Function Name    : _dspi_tx_rx
* Returned Value   : number of bytes transferred
* Comments         :
*   Actual transmit and receive function.
*   Overrun prevention used, no need to update statistics in this function
*
*END*********************************************************************/
static _mqx_int _dspi_tx_rx
    (
        /* [IN] Device specific context structure */
        pointer                        io_info_ptr,

        /* [IN] Data to transmit */
        uint_8_ptr                     txbuf,

        /* [OUT] Received data */
        uint_8_ptr                     rxbuf,

        /* [IN] Length of transfer in bytes */
        uint_32                        len
    )
{
    DSPI_INFO_STRUCT_PTR               dspi_info_ptr = (DSPI_INFO_STRUCT_PTR)io_info_ptr;
    VDSPI_REG_STRUCT_PTR               dspi_ptr = dspi_info_ptr->DSPI_PTR;

    uint_32                            tx_len;
    uint_32                            rx_len;

    uint_32                            command;
    uint_32                            data;

    boolean                            use_isr;

    use_isr = dspi_info_ptr->NUM_VECTORS && dspi_info_ptr->ATTR & DSPI_ATTR_USE_ISR;

    command = DSPI_PUSHR_CONT_MASK | DSPI_PUSHR_PCS(dspi_info_ptr->CS) | DSPI_PUSHR_CTAS(0);
    data = dspi_info_ptr->DUMMY_PATTERN;

    /* Is frame larger than a single byte? */
    if (DSPI_CTAR_FMSZ_GET(dspi_ptr->CTAR[0]) > 7)
    {
        len = len & (~1UL);
        rx_len = tx_len = len;

        while (rx_len)
        {
            if (tx_len) {
                if ((dspi_ptr->SR & DSPI_SR_TFFF_MASK) && ((rx_len-tx_len)<DSPI_FIFO_DEPTH))
                {
                    if (txbuf)
                    {
                        data = *txbuf++;
                        data = (data << 8) | *txbuf++;
                    }
                    dspi_ptr->PUSHR = command | DSPI_PUSHR_TXDATA(data);
                    dspi_ptr->SR = DSPI_SR_TFFF_MASK;
                    tx_len--;
                }
                else if (use_isr)
                {
                    /* do not wait for RX data in a loop, break it and use ISR */
                    break;
                }
            }

            if (dspi_ptr->SR & DSPI_SR_RFDF_MASK)
            {
                data = DSPI_POPR_RXDATA_GET(dspi_ptr->POPR);
                dspi_ptr->SR = DSPI_SR_RFDF_MASK;
                if (rxbuf)
                {
                    *rxbuf++ = data >> 8;
                    *rxbuf++ = data & 0xff;
                }
                rx_len--;
            }
            else if (tx_len == 0 && use_isr)
            {
                /* do not wait for RX data in a loop, break it and use ISR */
                break;
            }
        }
    }
    else
    {
        rx_len = tx_len = len;

        /* Optimized loop for single byte frames */
        while (rx_len)
        {
            if (tx_len) {
                if ((dspi_ptr->SR & DSPI_SR_TFFF_MASK) && ((rx_len-tx_len)<DSPI_FIFO_DEPTH))
                {
                    if (txbuf)
                        data = *txbuf++;
                    dspi_ptr->PUSHR = command | DSPI_PUSHR_TXDATA(data);
                    dspi_ptr->SR = DSPI_SR_TFFF_MASK;
                    tx_len--;
                }
                else if (use_isr)
                {
                    /* do not wait for RX data in a loop, break it and use ISR */
                    break;
                }
            }

            if (dspi_ptr->SR & DSPI_SR_RFDF_MASK)
            {
                if (rxbuf)
                    *rxbuf++ = DSPI_POPR_RXDATA_GET(dspi_ptr->POPR);
                else
                    dspi_ptr->POPR; /* dummy read to drain FIFO */
                dspi_ptr->SR = DSPI_SR_RFDF_MASK;
                rx_len--;
            }
            else if (tx_len == 0 && use_isr)
            {
                /* do not wait for RX data in a loop, break it and use ISR */
                break;
            }
        }
    }

    if (rx_len)
    {
        /* finish the transfer using ISR */
        dspi_info_ptr->TX_BUF = txbuf;
        dspi_info_ptr->TX_LEN = tx_len;
        dspi_info_ptr->RX_BUF = rxbuf;
        dspi_info_ptr->RX_LEN = rx_len;

        /* enable interrupt and wait for ISR state machine to finish the job */
        dspi_ptr->RSER = DSPI_RSER_RFDF_RE_MASK;
        _lwsem_wait(&dspi_info_ptr->EVENT_IO_FINISHED);
    }

    return len;
}


/*FUNCTION****************************************************************
*
* Function Name    : _dspi_cs_deassert
* Returned Value   :
* Comments         :
*   Deactivates chip select signals.
*
*END*********************************************************************/
static _mqx_int _dspi_cs_deassert
    (
        /* [IN] The address of the device registers */
        pointer                        io_info_ptr
    )
{
    DSPI_INFO_STRUCT_PTR               dspi_info_ptr = (DSPI_INFO_STRUCT_PTR)io_info_ptr;
    VDSPI_REG_STRUCT_PTR               dspi_ptr = dspi_info_ptr->DSPI_PTR;

    dspi_ptr->MCR |= DSPI_MCR_HALT_MASK;
    dspi_ptr->MCR &= (~ DSPI_MCR_MSTR_MASK);
    dspi_ptr->MCR |= DSPI_MCR_MSTR_MASK;
    dspi_ptr->MCR &= (~ DSPI_MCR_HALT_MASK);

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _dspi_ioctl
* Returned Value   : MQX error code
* Comments         :
*    This function performs miscellaneous services for
*    the SPI I/O device.
*
*END*********************************************************************/
static _mqx_int _dspi_ioctl
    (
        /* [IN] The address of the device specific information */
        pointer                        io_info_ptr,

        /* [IN] SPI transfer parameters */
        SPI_PARAM_STRUCT_PTR           params,

        /* [IN] The command to perform */
        uint_32                        cmd,

        /* [IN] Parameters for the command */
        uint_32_ptr                    param_ptr
    )
{
    DSPI_INFO_STRUCT_PTR               dspi_info_ptr = (DSPI_INFO_STRUCT_PTR)io_info_ptr;
    VDSPI_REG_STRUCT_PTR               dspi_ptr = dspi_info_ptr->DSPI_PTR;

    uint_32                            result = SPI_OK;

    BSP_CLOCK_CONFIGURATION clock_config;
    uint_32 clock_speed;

    switch (cmd)
    {
        case IO_IOCTL_SPI_GET_BAUD:
            clock_config = _bsp_get_clock_configuration();
            clock_speed = _bsp_get_clock(clock_config, dspi_info_ptr->CLOCK_SOURCE);
            *((uint_32_ptr)param_ptr) = _dspi_find_baudrate(clock_speed, *((uint_32_ptr)param_ptr), NULL);
            break;

        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }
    return result;
}

