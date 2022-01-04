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
* $FileName: spi_mk60.c$
* $Version : 3.8.3.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This file contains board-specific SPI initialization functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "spi_dspi.h"


static const pointer dspi_address[] =
{
    (pointer)SPI0_BASE_PTR,
    (pointer)SPI1_BASE_PTR,
    (pointer)SPI2_BASE_PTR
};


static const uint_32 /*PSP_INTERRUPT_TABLE_INDEX*/ dspi_vectors[][1] =
{
    { INT_SPI0 },
    { INT_SPI1 },
    { INT_SPI2 },
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_dspi_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding SPI
*    module.
*
*END*----------------------------------------------------------------------*/

pointer _bsp_get_dspi_base_address(uint_8 dev_num)
{
    if (dev_num < ELEMENTS_OF(dspi_address)) {
        return dspi_address[dev_num];
    }
    return NULL;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_dspi_vectors
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector table indices for specified
*    SPI module.
*
*END*----------------------------------------------------------------------*/

uint_32 _bsp_get_dspi_vectors(uint_32 dev_num, const uint_32 _PTR_ _PTR_ vectors_ptr)
{
    if (dev_num < ELEMENTS_OF(dspi_vectors)) {
        *vectors_ptr = dspi_vectors[dev_num];
        return ELEMENTS_OF(dspi_vectors[0]);
    }
    return 0;
}
