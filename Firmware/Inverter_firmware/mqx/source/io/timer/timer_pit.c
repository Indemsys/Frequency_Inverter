/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: timer_pit.c$
* $Version : 3.8.1.0$
* $Date    : May-11-2011$
*
* Comments:
*
*   This file contains timer functions for use with a PIT.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "timer_pit.h"


#define PIT_PCSR_PRESCALE_SHIFT 8
#define PIT_PCSR_DOZE           (1<<6)
#define PIT_PCSR_HALTED         (1<<5)
#define PIT_PCSR_OVW            (1<<4)
#define PIT_PCSR_PIE            (1<<3)
#define PIT_PCSR_PIF            (1<<2)
#define PIT_PCSR_RLD            (1<<1)
#define PIT_PCSR_EN             (1<<0)


typedef struct pit_struct
{
    uint_16 PCSR;   /* PIT control and status register */
    uint_16 PMR;    /* PIT modulus register */
    uint_16 PCNTR;  /* PIT counter register */
} PIT_STRUCT, _PTR_ PIT_STRUCT_PTR;


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _pit_init_freq
* Returned Value  : hw ticks per tick
* Comments        :
*   this function will set up a timer to interrupt
*
*END*---------------------------------------------------------------------*/

uint_32 _pit_init_freq
    (
        /* [IN] the timer to initialize */
        uint_8    timer,

        /* [IN] ticks per second */
        uint_32   tickfreq,

        /* [IN] input clock speed in Hz */
        uint_32   clk,

        /* [IN] unmask the timer after initializing */
        boolean   unmask_timer
    )
{
    uint_32 rate, prescale = 0;

    volatile PIT_STRUCT _PTR_  pit_ptr;

    pit_ptr = _bsp_get_pit_base_address(timer);
    if (pit_ptr == NULL) return 0;

    _bsp_int_init(_bsp_timers[timer].INTERRUPT, _bsp_timers[timer].LEVEL, _bsp_timers[timer].PRIORITY, FALSE);

    rate = (clk / tickfreq);
    while (rate > (0xFFFF+1)) {
        prescale += 1;  /* divides clock in half */
        clk >>= 1;
        rate = (clk / tickfreq);
    }

    pit_ptr->PCSR = (prescale << PIT_PCSR_PRESCALE_SHIFT)
        | PIT_PCSR_HALTED             /* stop timer when halted */
        | PIT_PCSR_PIF                /* Clear interrupt if present */
        | PIT_PCSR_OVW                /* Overwrite count with PMR when written */
        | PIT_PCSR_OVW                /* Enables writing to PMR */
        | PIT_PCSR_PIE                /* PIT interrupt Enable */
        | PIT_PCSR_RLD;               /* Enabled Reload bit */

    /* Set counter reload value and counter value */
    pit_ptr->PMR = (uint_16)(rate-1);

    if (unmask_timer) {
        _bsp_int_enable(_bsp_timers[timer].INTERRUPT);
    }

    /* Enable the timer */
    pit_ptr->PCSR |= PIT_PCSR_EN;

    /* return number of hw-ticks per MQX tick */
    return rate;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _pit_get_hwticks
* Returned Value  : Number of ticks
* Comments        :
*   This function will return the number of ticks elapsed since the last
* timer tick.
*
*END*---------------------------------------------------------------------*/

uint_32 _pit_get_hwticks
    (
        /* [IN] the timer to use */
        pointer parameter
    )
{
    uint_16 pmr;
    uint_32 ticks;
    
    volatile PIT_STRUCT _PTR_  pit_ptr;

    pit_ptr = _bsp_get_pit_base_address((uint_8)parameter);
    if (pit_ptr == NULL) return 0;

    pmr   = pit_ptr->PMR;
    ticks = pmr - pit_ptr->PCNTR;

    if ((pit_ptr->PCSR & PIT_PCSR_PIF) != 0) {
        /* Another full TICK period has expired since we handled the last timer interrupt.
        We need to read the counter again, since the wrap may have
        occurred between the previous read and the checking of the overflow bit. */
        ticks = pmr + (pmr-pit_ptr->PCNTR) + 1;
    }
   
    return ticks;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _pit_clear_int
* Returned Value  :
* Comments        :
*   This function will clear interrupt flag of given timer
*
*END*---------------------------------------------------------------------*/

void _pit_clear_int
    (
        /* [IN] the timer to use */
        uint_8    timer
    )
{
    volatile uint_8 *pcsr_low;
    volatile PIT_STRUCT _PTR_  pit_ptr;

    pit_ptr = _bsp_get_pit_base_address(timer);
    if (pit_ptr == NULL) return;

#if (PSP_ENDIAN == MQX_BIG_ENDIAN)
    pcsr_low = ((uint_8 *)(&pit_ptr->PCSR)) + 1;
#else
    pcsr_low = ((uint_8 *)(&pit_ptr->PCSR));
#endif

    *pcsr_low |= PIT_PCSR_PIF;
}
