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
* $FileName: spi_mcf5225_qspi.c$
* $Version : 3.6.2.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains board-specific SPI initialization functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "spi_mcf5xxx_qspi.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_spi_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding SPI
*    module.
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_qspi_base_address
(
    uint_8 dev_num
)
{
    pointer addr;
    
    switch (dev_num) 
    {
        case 0:
            addr = (pointer)&(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->QSPI);
            break;
        default:
            addr = NULL;
            break;
    }
    return addr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_spi_vector
* Returned Value   : Vector number upon success, 0 upon failure
* Comments         :
*    This function returns desired interrupt vector table index for specified 
*    SPI module and vector identifier.
*
*END*----------------------------------------------------------------------*/
uint_32 _bsp_get_qspi_vector
(
    uint_8 dev_num
)
{
    if (0 != dev_num) return 0;
    return MCF5225_INT_QSPI;
}

/* EOF */
