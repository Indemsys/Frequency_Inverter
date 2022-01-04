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
* $FileName: esdhc_mk60.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains board-specific ESDHC initialization functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_esdhc_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding eSDHC
*    module.
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_esdhc_base_address
(
    uint_8 dev_num
)
{
    pointer addr = NULL;
    
    switch (dev_num) 
    {
        case 0:
            addr = (pointer)SDHC_BASE_PTR;
            break;
        default:
            break;
    }
    
    return addr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_esdhc_vector
* Returned Value   : Vector number upon success, 0 upon failure
* Comments         :
*    This function returns desired interrupt vector table index for specified 
*    ESDHC module and vector identifier.
*
*END*----------------------------------------------------------------------*/
uint_32 _bsp_get_esdhc_vector
(
    uint_8 dev_num
)
{
    uint_32 vector_index = 0;
    
    switch (dev_num)
    {
        case 0: 
            vector_index = INT_SDHC;
            break;
        default:
            break;
    }
    
    return vector_index;
}

/* EOF */
