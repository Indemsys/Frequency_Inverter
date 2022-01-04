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
* $FileName: spi_prv.h$
* $Version : 3.8.1.0$
* $Date    : Sep-25-2012$
*
* Comments:
*
*   This file contains definitions private to the SPI driver.
*
*END************************************************************************/

#ifndef __spi_prv_h__
#define __spi_prv_h__

#include "spi.h"


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** Run time state information for SPI driver (shared for all fds)
*/
typedef struct spi_driver_data_struct
{
    /* LWSEM for locking for concurrent access from several tasks */
    LWSEM_STRUCT          BUS_LOCK;

    /* Pointer to low level driver */
    SPI_DEVIF_STRUCT_CPTR DEVIF;

    /* Pointer to runtime data specific for low level driver */
    pointer               DEVIF_DATA;

    /* Default transfer parameters for low level driver */
    SPI_PARAM_STRUCT      PARAMS;

    /* Callback function for external CS handling */
    SPI_CS_CALLBACK       CS_CALLBACK;

    /* Context passed to CS callback function */
    pointer               CS_USERDATA;

} SPI_DRIVER_DATA_STRUCT, _PTR_ SPI_DRIVER_DATA_STRUCT_PTR;


/*
** Context information for open fd
*/
typedef struct spi_dev_data_struct
{
    /* Inidicates that BUS_LOCK is being held by this fd */
    boolean               BUS_OWNER;

    /* Transfer parameters for low level driver */
    SPI_PARAM_STRUCT      PARAMS;

    /* Indicates necessity to re-set parameters before transfer */
    boolean               PARAMS_DIRTY;

    /* Open flags for this channel */
    uint_32               FLAGS;

#if BSPCFG_ENABLE_SPI_STATS
    /* Statistical information */
    SPI_STATISTICS_STRUCT STATS;
#endif

} SPI_DEV_DATA_STRUCT, _PTR_ SPI_DEV_DATA_STRUCT_PTR;


#endif
