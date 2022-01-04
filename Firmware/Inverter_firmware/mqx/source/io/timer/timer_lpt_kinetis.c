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
* $FileName: timer_lpt_kinetis.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains board-specific LPT initialization functions.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>


static const pointer lpt_address[] =
{
    (pointer)LPTMR0_BASE_PTR
};


static const uint_32 lpt_vectors[] =
{
    INT_LPTimer
};


static const CM_CLOCK_SOURCE lpt_clock_source[] =
{
    CM_CLOCK_SOURCE_MCGIR,
    CM_CLOCK_SOURCES,
    CM_CLOCK_SOURCE_ERCLK32K,
    CM_CLOCK_SOURCE_OSCER
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_lpt_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding LPT
*    module.
*
*END*----------------------------------------------------------------------*/

pointer _bsp_get_lpt_base_address
    (
        /* [IN] LPT index */
        uint_8 dev_num
    )
{
    if (dev_num < ELEMENTS_OF(lpt_address)) 
    {
        return lpt_address[dev_num];
    }
    return NULL;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_lpt_vector
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector for specified LPT module.
*
*END*----------------------------------------------------------------------*/

uint_32 _bsp_get_lpt_vector
    (
        /* [IN] LPT index */
        uint_8 dev_num
    )
{
    if (dev_num < ELEMENTS_OF(lpt_vectors)) 
    {
        return lpt_vectors[dev_num];
    } 
    else 
    {
        return 0;
    }
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_lpt_clock_frequency
* Returned Value   : Frequency for given clock source
* Comments         :
*    This function returns the input clock frequency for corresponding LPT
*    module and input source.
*
*END*----------------------------------------------------------------------*/

uint_32 _bsp_get_lpt_clock_frequency
    (
        /* [IN] LPT index */
        uint_8  dev_num,
        
        /* [IN} LPT clock source index */
        uint_32 source
    )
{
    if (source < ELEMENTS_OF(lpt_clock_source)) 
    {
        if (CM_CLOCK_SOURCES != lpt_clock_source[source])
        {
            return _bsp_get_clock (_bsp_get_clock_configuration (), lpt_clock_source[source]);
        }
        else
        {
            return 1000;
        }
    }
    return 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_enable_lpt_clock
* Returned Value   : MQX error code
* Comments         :
*    This function enables the clock for LPT so the registers are accessible.
*
*END*----------------------------------------------------------------------*/

uint_32 _bsp_enable_lpt_clock
    (
        /* [IN] LPT index */
        uint_8  dev_num
    )
{
    if (dev_num < ELEMENTS_OF(lpt_address)) 
    {
        SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
        return MQX_OK;
    }
    return MQX_INVALID_DEVICE;
}
