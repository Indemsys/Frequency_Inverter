/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
*****************************************************************************
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
*****************************************************************************
*
* $FileName: timer_lpt.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains LPT functionality.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <timer_lpt.h>


static void _lpt_kernel_isr (pointer parameter);


/*FUNCTION*------------------------------------------------------------------
* 
* Function Name    : _lpt_set_period
* Returned Value   : Difference to desired period.
* Comments         :
*    This function finds and sets closest longer period for given LPT across
*    all given clock sources.
*
*END*----------------------------------------------------------------------*/

static uint_32 _lpt_set_period
    (
        /* [IN] Timer to initialize */
        uint_8  timer,
        
        /* [IN] Desired timer period in Hz */
        uint_32 period_hz,

        /* [IN] Desired timer period in us */
        uint_32 period_us,
        
        /* [IN] Allowed clock sources */
        uint_32 sources
    )
{
    static const uint_32 clock_inputs_ordered[] = {LPT_FLAG_CLOCK_SOURCE_LPO, LPT_FLAG_CLOCK_SOURCE_OSCERCLK, LPT_FLAG_CLOCK_SOURCE_MCGIRCLK, LPT_FLAG_CLOCK_SOURCE_ERCLK32K};
    static const uint_8  clock_inputs_log[] = {0, 0, 1, 0, 2, 0, 0, 0, 3};
    uint_32 i, frequency, prescaler, psr = 0, cmr = 0, f = 1;
    uint_64 compare, tmp, diff, min = (uint_64)-1;
    LPTMR_MemMapPtr lpt_ptr = _bsp_get_lpt_base_address (timer);
    
    if ((NULL == lpt_ptr) || ((0 == period_hz) && (0 == period_us)) || (0 == sources))
    {
        return (uint_32)min;
    }
    
    for (i = 0; i < ELEMENTS_OF(clock_inputs_ordered); i++)
    {
        if (sources & clock_inputs_ordered[i])
        {
            frequency = _bsp_get_lpt_clock_frequency (timer, LPTMR_PSR_PCS(clock_inputs_log[clock_inputs_ordered[i]]));
            if (period_hz)
            {
                compare = (frequency + period_hz - 1) / period_hz;
                diff = compare * period_hz - frequency;
            }
            else
            {
                tmp = (uint_64)frequency * (uint_64)period_us;
                compare = (tmp + 999999) / 1000000;
                diff = compare * 1000000 - tmp;
            }
            if (((0 < compare) && (compare <= 65536)) && (min > diff))
            {
                min = diff;
                f = frequency;
                psr = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(clock_inputs_log[clock_inputs_ordered[i]]);
                cmr = LPTMR_CMR_COMPARE(compare - 1);
                if (0 == min)
                {
                    break;
                }
            }
            for (prescaler = 1; prescaler < 16; prescaler++)
            {
                if (period_hz)
                {
                    tmp = ((uint_64)period_hz) << (prescaler + 1);
                    compare = (frequency + tmp - 1) / tmp;
                    diff = (compare << (prescaler + 1)) * period_hz - frequency;
                }
                else
                {
                    diff = 1000000;
                    diff <<= (prescaler + 1);
                    compare = (tmp + diff - 1) / diff;
                    diff = compare * diff - tmp;
                }
                if (((0 < compare) && (compare <= 65536)) && (min > diff))
                {
                    min = diff;
                    f = frequency;
                    psr = LPTMR_PSR_PRESCALE(prescaler) | LPTMR_PSR_PCS(clock_inputs_log[clock_inputs_ordered[i]]); 
                    cmr = LPTMR_CMR_COMPARE(compare - 1);
                    if (0 == min)
                    {
                        break;
                    }
                }
            }
            if (0 == min)
            {
                break;
            }
        }
    }

    lpt_ptr->PSR = psr;
    lpt_ptr->CMR = cmr;

    return (uint_32)(min / f);
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_install
* Returned Value  : MQX error code
* Comments        :
*   This function installs LPT timer into MQX.
*
*END*---------------------------------------------------------------------*/

_mqx_int _lpt_install
    (
        /* [IN] Timer to initialize */
        uint_8  timer,
    
        /* [IN] Period in microseconds */
        uint_32 period_us,

        /* [IN] Input clock sources mask */
        uint_32 clk,
    
        /* [IN] Interrupt priority */
        uint_32 priority,
    
        void (_CODE_PTR_ isr_ptr)(pointer),
        
        /* [IN] Unmask the timer after installation */
        boolean unmask_timer
    )
{
    uint_32 result;
    _mqx_uint vector = _bsp_get_lpt_vector (timer);
    if (0 == vector)
    {
        return MQX_INVALID_DEVICE;
    }
    if (0 == period_us)
    {
        return MQX_INVALID_PARAMETER;
    }

    _bsp_int_disable (vector);

    if (NULL == _int_install_isr (vector, isr_ptr, (pointer)timer))
    {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }
    
    _bsp_enable_lpt_clock (timer);
    
    _lpt_init (timer, period_us, clk, unmask_timer);
    
    _bsp_int_init (vector, priority, 0, TRUE);
    _bsp_int_enable (vector);
    
    return MQX_OK;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_install_kernel
* Returned Value  : MQX error code
* Comments        :
*   This function installs LPT timer as MQX tick source.
*
*END*---------------------------------------------------------------------*/

_mqx_int _lpt_install_kernel
(
        /* [IN] Timer to initialize */
        uint_8  timer,

        /* [IN] Period in microseconds */
        uint_32 period_us,

        /* [IN] Input clock sources mask */
        uint_32 clk,

        /* [IN] Interrupt priority */
        uint_32 priority,

        /* [IN] Unmask the timer after installation */
        boolean unmask_timer
)
{
    uint_32 result, period, ticks;
    _mqx_uint vector = _bsp_get_lpt_vector (timer);
    if (0 == vector)
    {
        return MQX_INVALID_DEVICE;
    }
    if (0 == period_us)
    {
        return MQX_INVALID_PARAMETER;
    }
    ticks = 1000000 / period_us;
    if (0 == ticks)
    {
        return MQX_INVALID_PARAMETER;
    }

    _bsp_int_disable (vector);

    if (NULL == _int_install_isr (vector, _lpt_kernel_isr, (pointer)timer))
    {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }

    _bsp_enable_lpt_clock (timer);

    period = _lpt_init (timer, period_us, clk, unmask_timer);

    _time_set_timer_vector (vector);
    _time_set_hwtick_function (_lpt_get_hwticks, (pointer) timer);
    _time_set_hwticks_per_tick (period);
    _time_set_ticks_per_sec (ticks);

    _bsp_int_init (vector, priority, 0, TRUE);
    _bsp_int_enable (vector);

    return MQX_OK;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_init
* Returned Value  : HW counter modulo per 1 timer interrupt
* Comments        :
*   This function sets up the LPT period.
*
*END*---------------------------------------------------------------------*/

uint_32 _lpt_init
    (
        /* [IN] Timer to initialize */
        uint_8  timer,

        /* [IN] Period in microseconds */
        uint_32 period_us,

        /* [IN] Input clock sources mask */
        uint_32 clk,

        /* [IN] Unmask the timer after initializing */
        boolean unmask_timer
    )
{
    uint_32 period;
    LPTMR_MemMapPtr lpt_ptr = _bsp_get_lpt_base_address (timer);
    if (NULL == lpt_ptr) 
    {
        return 0;
    }

    _lpt_run (timer, FALSE);
    
    _lpt_mask_int (timer);
    _lpt_clear_int (timer);
    
    _lpt_set_period (timer, 0, period_us, clk);
    period = lpt_ptr->CMR + 1;
        
    _lpt_run (timer, TRUE);
        
    if (unmask_timer) 
    {
        _lpt_unmask_int (timer);
    }
    
    return period;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_get_hwticks
* Returned Value  : Number of ticks
* Comments        :
*   This function returns the number of ticks elapsed since the last
*   timer tick.
*
*END*---------------------------------------------------------------------*/

uint_32 _lpt_get_hwticks
   (
      /* [IN] the timer to clear */
      pointer parameter
   )
{
    uint_32 timer = (uint_32)parameter;
    uint_32 ticks;
    uint_32 modulo;
    
    LPTMR_MemMapPtr lpt_ptr = _bsp_get_lpt_base_address (timer);
    if (NULL == lpt_ptr) 
    {
        return 0;
    }

    modulo = lpt_ptr->CMR + 1;
    ticks = lpt_ptr->CNR;

    if (lpt_ptr->CSR & LPTMR_CSR_TCF_MASK) 
    {
        /* Another full TICK period has expired since we handled the last timer interrupt.
        We need to read the counter again, since the wrap may have
        occurred between the previous read and the checking of the overflow bit. */
        ticks = lpt_ptr->CNR + modulo;
    }

    /* actual ticks + what we have missed because of sw prescaller */ 
    return ticks;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_mask_int
* Returned Value  : None
* Comments        :
*   This function masks interrupt of given timer.
*
*END*---------------------------------------------------------------------*/

void _lpt_mask_int
    (
        /* [IN] Timer to use */
        uint_8 timer
    )
{
    LPTMR_MemMapPtr lpt_ptr = _bsp_get_lpt_base_address (timer);
    if (NULL != lpt_ptr)
    {
        lpt_ptr->CSR &= (~ LPTMR_CSR_TIE_MASK);
    }
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_unmask_int
* Returned Value  : None
* Comments        :
*   This function unmasks interrupt of given timer.
*
*END*---------------------------------------------------------------------*/

void _lpt_unmask_int
    (
        /* [IN] Timer to use */
        uint_8 timer
    )
{
    LPTMR_MemMapPtr lpt_ptr = _bsp_get_lpt_base_address (timer);
    if (NULL != lpt_ptr)
    {
        lpt_ptr->CSR |= LPTMR_CSR_TIE_MASK;
    }
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_clear_int
* Returned Value  : None
* Comments        :
*   This function clears interrupt flag of given timer.
*
*END*---------------------------------------------------------------------*/

void _lpt_clear_int
    (
        /* [IN] Timer to use */
        uint_8 timer
    )
{
    LPTMR_MemMapPtr lpt_ptr = _bsp_get_lpt_base_address (timer);
    if (NULL != lpt_ptr) 
    {
        lpt_ptr->CSR |= LPTMR_CSR_TCF_MASK;
    }
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_run
* Returned Value  : None
* Comments        :
*   This function starts/stops the given timer.
*
*END*---------------------------------------------------------------------*/

void _lpt_run
    (
        /* [IN] Timer to use */
        uint_8  timer,
        
        /* [IN] Whether to start or stop */
        boolean run
    )
{
    LPTMR_MemMapPtr lpt_ptr = _bsp_get_lpt_base_address (timer);
    if (NULL != lpt_ptr)
    {
        if (run)
        {
            lpt_ptr->CSR |= LPTMR_CSR_TEN_MASK;
        }
        else
        {
            lpt_ptr->CSR &= (~ LPTMR_CSR_TEN_MASK);
        }
    }
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _lpt_kernel_isr
* Returned Value  : NOne
* Comments        :
*   ISR for LPT event to be used as system timer.
*
*END*---------------------------------------------------------------------*/

static void _lpt_kernel_isr
    (
        pointer parameter
    ) 
{
    _lpt_clear_int ((uint_32)parameter);
    _time_notify_kernel ();
}

/* EOF */
