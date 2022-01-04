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
* $FileName: init_spi.c$
* $Version : 3.8.2.0$
* $Date    : Oct-1-2012$
*
* Comments:
*
*   This file contains the initialization definition for DSPI driver
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>


/*
** Parameters for SPI initialization
*/

static const DSPI_INIT_STRUCT _bsp_dspi0_init = {
    0,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS           /* Relevant module clock source */
};

const SPI_INIT_STRUCT _bsp_spi0_init = {
    &_spi_dspi_devif,             /* Low level driver interface */
    &_bsp_dspi0_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        DSPI_ATTR_USE_ISR,            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};


static const DSPI_INIT_STRUCT _bsp_dspi1_init = {
    1,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS           /* Relevant module clock source */
};

const SPI_INIT_STRUCT _bsp_spi1_init = {
    &_spi_dspi_devif,             /* Low level driver interface */
    &_bsp_dspi1_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        DSPI_ATTR_USE_ISR,            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};


static const DSPI_INIT_STRUCT _bsp_dspi2_init = {
    2,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS           /* Relevant module clock source */
};

const SPI_INIT_STRUCT _bsp_spi2_init = {
    &_spi_dspi_devif,             /* Low level driver interface */
    &_bsp_dspi2_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        DSPI_ATTR_USE_ISR,            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};
