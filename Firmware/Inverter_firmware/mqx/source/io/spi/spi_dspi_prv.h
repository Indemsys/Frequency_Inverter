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
* $FileName: spi_dspi_prv.h$
* $Version : 3.8.2.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This file contains definitions private to the SPI driver.
*
*END************************************************************************/

#ifndef __spi_dspi_prv_h__
#define __spi_dspi_prv_h__

#include "spi.h"
#include "spi_dspi.h"


/*--------------------------------------------------------------------------*/
/* Peripheral register access */

typedef struct dspi_reg_struct {
  uint_32 MCR;                                    /*!< DSPI Module Configuration Register, offset: 0x0 */
  uint_32 HCR;                                    /* silicon dependent */
  uint_32 TCR;                                    /*!< DSPI Transfer Count Register, offset: 0x8 */
  uint_32 CTAR[8];                                /*!< DSPI Clock and Transfer Attributes Register, array offset: 0xC, array step: 0x4, actual count depends on chip */
  uint_32 SR;                                     /*!< DSPI Status Register, offset: 0x2C */
  uint_32 RSER;                                   /*!< DSPI DMA/Interrupt Request Select and Enable Register, offset: 0x30 */
  uint_32 PUSHR;                                  /*!< DSPI PUSH TX FIFO Register In Master Mode, offset: 0x34 */
  uint_32 POPR;                                   /*!< DSPI POP RX FIFO Register, offset: 0x38 */
  uint_32 TXFR[16];                               /*!< DSPI Transmit FIFO Registers, offset: 0x3C, actual count depends on chip */
  uint_32 RXFR[16];                               /*!< DSPI Receive FIFO Registers, offset: 0x7C, actual count depends on chip */
} DSPI_REG_STRUCT, * DSPI_REG_STRUCT_PTR;

typedef volatile struct dspi_reg_struct * VDSPI_REG_STRUCT_PTR;


/* MCR Bit Fields */
#define DSPI_MCR_HALT_MASK                        0x1u
#define DSPI_MCR_SMPL_PT_MASK                     0x300
#define DSPI_MCR_SMPL_PT_SHIFT                    8
#define DSPI_MCR_CLR_RXF_MASK                     0x400u
#define DSPI_MCR_CLR_TXF_MASK                     0x800u
#define DSPI_MCR_DIS_RXF_MASK                     0x1000u
#define DSPI_MCR_DIS_TXF_MASK                     0x2000u
#define DSPI_MCR_MDIS_MASK                        0x4000u
#define DSPI_MCR_DOZE_MASK                        0x8000u
#define DSPI_MCR_PCSIS_MASK                       0xFF0000u
#define DSPI_MCR_PCSIS_SHIFT                      16
#define DSPI_MCR_ROOE_MASK                        0x1000000u
#define DSPI_MCR_PCSSE_MASK                       0x2000000u
#define DSPI_MCR_MTFE_MASK                        0x4000000u
#define DSPI_MCR_FRZ_MASK                         0x8000000u
#define DSPI_MCR_DCONF_MASK                       0x30000000u
#define DSPI_MCR_DCONF_SHIFT                      28
#define DSPI_MCR_CONT_SCKE_MASK                   0x40000000u
#define DSPI_MCR_MSTR_MASK                        0x80000000u

/* TCR Bit Fields */
#define DSPI_TCR_SPI_TCNT_MASK                    0xFFFF0000u
#define DSPI_TCR_SPI_TCNT_SHIFT                   16

/* CTAR Bit Fields */
#define DSPI_CTAR_BR_MASK                         0xFu
#define DSPI_CTAR_BR_SHIFT                        0
#define DSPI_CTAR_DT_MASK                         0xF0u
#define DSPI_CTAR_DT_SHIFT                        4
#define DSPI_CTAR_ASC_MASK                        0xF00u
#define DSPI_CTAR_ASC_SHIFT                       8
#define DSPI_CTAR_CSSCK_MASK                      0xF000u
#define DSPI_CTAR_CSSCK_SHIFT                     12
#define DSPI_CTAR_PBR_MASK                        0x30000u
#define DSPI_CTAR_PBR_SHIFT                       16
#define DSPI_CTAR_PDT_MASK                        0xC0000u
#define DSPI_CTAR_PDT_SHIFT                       18
#define DSPI_CTAR_PASC_MASK                       0x300000u
#define DSPI_CTAR_PASC_SHIFT                      20
#define DSPI_CTAR_PCSSCK_MASK                     0xC00000u
#define DSPI_CTAR_PCSSCK_SHIFT                    22
#define DSPI_CTAR_LSBFE_MASK                      0x1000000u
#define DSPI_CTAR_CPHA_MASK                       0x2000000u
#define DSPI_CTAR_CPOL_MASK                       0x4000000u
#define DSPI_CTAR_FMSZ_MASK                       0x78000000u
#define DSPI_CTAR_FMSZ_SHIFT                      27
#define DSPI_CTAR_DBR_MASK                        0x80000000u
#define DSPI_CTAR_DBR_SHIFT                       31
#define DSPI_CTAR_SLAVE_FMSZ_MASK                 0xF8000000u
#define DSPI_CTAR_SLAVE_FMSZ_SHIFT                27

/* SR Bit Fields */
#define DSPI_SR_POPNXTPTR_MASK                    0xFu
#define DSPI_SR_POPNXTPTR_SHIFT                   0
#define DSPI_SR_RXCTR_MASK                        0xF0u
#define DSPI_SR_RXCTR_SHIFT                       4
#define DSPI_SR_TXNXTPTR_MASK                     0xF00u
#define DSPI_SR_TXNXTPTR_SHIFT                    8
#define DSPI_SR_TXCTR_MASK                        0xF000u
#define DSPI_SR_TXCTR_SHIFT                       12
#define DSPI_SR_RFDF_MASK                         0x20000u
#define DSPI_SR_RFOF_MASK                         0x80000u
#define DSPI_SR_TFFF_MASK                         0x2000000u
#define DSPI_SR_TFUF_MASK                         0x8000000u
#define DSPI_SR_EOQF_MASK                         0x10000000u
#define DSPI_SR_TXRXS_MASK                        0x40000000u
#define DSPI_SR_TCF_MASK                          0x80000000u

/* RSER Bit Fields */
#define DSPI_RSER_RFDF_DIRS_MASK                  0x10000u
#define DSPI_RSER_RFDF_RE_MASK                    0x20000u
#define DSPI_RSER_RFOF_RE_MASK                    0x80000u
#define DSPI_RSER_TFFF_DIRS_MASK                  0x1000000u
#define DSPI_RSER_TFFF_RE_MASK                    0x2000000u
#define DSPI_RSER_TFUF_RE_MASK                    0x8000000u
#define DSPI_RSER_EOQF_RE_MASK                    0x10000000u
#define DSPI_RSER_TCF_RE_MASK                     0x80000000u

/* PUSHR Bit Fields */
#define DSPI_PUSHR_TXDATA_MASK                    0xFFFFu
#define DSPI_PUSHR_TXDATA_SHIFT                   0
#define DSPI_PUSHR_PCS_MASK                       0x3F0000u
#define DSPI_PUSHR_PCS_SHIFT                      16
#define DSPI_PUSHR_CTCNT_MASK                     0x4000000u
#define DSPI_PUSHR_EOQ_MASK                       0x8000000u
#define DSPI_PUSHR_CTAS_MASK                      0x70000000u
#define DSPI_PUSHR_CTAS_SHIFT                     28
#define DSPI_PUSHR_CONT_MASK                      0x80000000u

/* POPR Bit Fields */
#define DSPI_POPR_RXDATA_MASK                     0xFFFFFFFFu
#define DSPI_POPR_RXDATA_SHIFT                    0


#define DSPI_MCR_PCSIS(x)        (((x) << DSPI_MCR_PCSIS_SHIFT) & DSPI_MCR_PCSIS_MASK)
#define DSPI_CTAR_BR(x)          (((x) << DSPI_CTAR_BR_SHIFT) & DSPI_CTAR_BR_MASK)
#define DSPI_CTAR_DT(x)          (((x) << DSPI_CTAR_DT_SHIFT) & DSPI_CTAR_DT_MASK)
#define DSPI_CTAR_ASC(x)         (((x) << DSPI_CTAR_ASC_SHIFT) & DSPI_CTAR_ASC_MASK)
#define DSPI_CTAR_CSSCK(x)       (((x) << DSPI_CTAR_CSSCK_SHIFT) & DSPI_CTAR_CSSCK_MASK)
#define DSPI_CTAR_PBR(x)         (((x) << DSPI_CTAR_PBR_SHIFT) & DSPI_CTAR_PBR_MASK)
#define DSPI_CTAR_PDT(x)         (((x) << DSPI_CTAR_PDT_SHIFT) & DSPI_CTAR_PDT_MASK)
#define DSPI_CTAR_PASC(x)        (((x) << DSPI_CTAR_PASC_SHIFT) & DSPI_CTAR_PASC_MASK)
#define DSPI_CTAR_PCSSCK(x)      (((x) << DSPI_CTAR_PCSSCK_SHIFT) & DSPI_CTAR_PCSSCK_MASK)
#define DSPI_CTAR_FMSZ(x)        (((x) << DSPI_CTAR_FMSZ_SHIFT) & DSPI_CTAR_FMSZ_MASK)
#define DSPI_PUSHR_TXDATA(x)     (((x) << DSPI_PUSHR_TXDATA_SHIFT) & DSPI_PUSHR_TXDATA_MASK)
#define DSPI_PUSHR_PCS(x)        (((x) << DSPI_PUSHR_PCS_SHIFT) & DSPI_PUSHR_PCS_MASK)
#define DSPI_PUSHR_CTAS(x)       (((x) << DSPI_PUSHR_CTAS_SHIFT) & DSPI_PUSHR_CTAS_MASK)

#define DSPI_CTAR_FMSZ_GET(x)    (((x) & DSPI_CTAR_FMSZ_MASK) >> DSPI_CTAR_FMSZ_SHIFT)
#define DSPI_CTAR_PBR_GET(x)     (((x) & DSPI_CTAR_PBR_MASK) >> DSPI_CTAR_PBR_SHIFT)
#define DSPI_CTAR_BR_GET(x)      (((x) & DSPI_CTAR_BR_MASK) >> DSPI_CTAR_BR_SHIFT)
#define DSPI_SR_TXCTR_GET(x)     (((x) & DSPI_SR_TXCTR_MASK) >> DSPI_SR_TXCTR_SHIFT)
#define DSPI_PUSHR_PCS_GET(x)    (((x) & DSPI_PUSHR_PCS_MASK) >> DSPI_PUSHR_PCS_SHIFT)
#define DSPI_PUSHR_CTAS_GET(x)   (((x) & DSPI_PUSHR_CTAS_MASK) >> DSPI_PUSHR_CTAS_SHIFT)
#define DSPI_PUSHR_TXDATA_GET(x) (((x) & DSPI_PUSHR_TXDATA_MASK) >> DSPI_PUSHR_TXDATA_SHIFT)
#define DSPI_MCR_PCSIS_GET(x)    (((x) & DSPI_MCR_PCSIS_MASK) >> DSPI_MCR_PCSIS_SHIFT)
#define DSPI_POPR_RXDATA_GET(x)  (((x) & DSPI_POPR_RXDATA_MASK) >> DSPI_POPR_RXDATA_SHIFT)


/* Max number of different IRQs the peripheral can generate */
#define DSPI_MAX_VECTORS 6


/* Max. number of frames on the way to prevent RX overruns in master mode */
#define DSPI_FIFO_DEPTH 4


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** DSPI_INFO_STRUCT
** Run time state information for each spi channel
*/
typedef struct dspi_info_struct
{
    /* SPI channel number */
    uint_32                           CHANNEL;

    /* The input clock source of the module */
    CM_CLOCK_SOURCE                   CLOCK_SOURCE;

    /* Most recently used clock configuration (cached value) */
    BSP_CLOCK_CONFIGURATION           CLOCK_CONFIG;

    /* Most recently used baudrate */
    uint_32                           BAUDRATE;

    /* Most recently calculated timing parameters for CTAR register */
    uint_32                           CTAR_TIMING;

    /* The spi device registers */
    VDSPI_REG_STRUCT_PTR              DSPI_PTR;

    /* CS signal(s) to activate for the transfer */
    uint_32                           CS;

    /* Pattern to transmit during half-duplex rx transfer */
    uint_32                           DUMMY_PATTERN;

    /* Additional attributes for the transfer */
    uint_32                           ATTR;

    /* Pointer to the buffer to use for RX data */
    uint_8_ptr                        RX_BUF;

    /* One DMA RX transfer byte count */
    uint_32                           RX_LEN;

    /* Pointer to the buffer to use for TX data */
    uint_8_ptr                        TX_BUF;

    /* One TX transfer byte count */
    uint_32                           TX_LEN;

    /* Event to signal ISR job done */
    LWSEM_STRUCT                      EVENT_IO_FINISHED;

    /* The previous vector, interrupt handler and data */
    uint_32                           NUM_VECTORS;
    void                  (_CODE_PTR_ OLD_ISR[DSPI_MAX_VECTORS])(pointer);
    pointer                           OLD_ISR_DATA[DSPI_MAX_VECTORS];

} DSPI_INFO_STRUCT, _PTR_ DSPI_INFO_STRUCT_PTR;

typedef volatile struct dspi_info_struct * VDSPI_INFO_STRUCT_PTR;


#endif
