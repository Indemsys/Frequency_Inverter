/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
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
* $FileName: timer_qpit.c$
* $Version : 3.8.13.0$
* $Date    : Sep-13-2012$
*
* Comments:
*
*    This file contains definitions for the Qoriwa PIT timer driver.
*
*END***********************************************************************/


#include "mqx_inc.h"
#include <bsp.h>
#include "bsp_prv.h"

#include "timer_qpit.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qpit_get_hwticks
* Returned Value   : Number of hwticks elapsed since last interrupt
* Comments         :
*
*
*END*----------------------------------------------------------------------*/

static uint_32 _qpit_get_hwticks(pointer p)
{
    uint_32 ticks;
    uint_32 ref;

    VQPIT_TIMER_REG_STRUCT_PTR qpit_timer_ptr = (VQPIT_TIMER_REG_STRUCT_PTR)p;

    ref = qpit_timer_ptr->LDVAL;
    ticks = ref - qpit_timer_ptr->CVAL;

    if (qpit_timer_ptr->TFLG & QPIT_TFLG_TIF)
    {
       /* Need to re-read CVAL as it may have rolled between first read and read of TFLG */
       ticks = ref + (ref-qpit_timer_ptr->CVAL) + 1;
    }

    return ticks;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qpit_get_period
* Returned Value   : Number of hwticks per tick
* Comments         : Initilizes QPIT
*
*
*END*----------------------------------------------------------------------*/

static uint_32 _qpit_get_period
    (
        /* [IN] the device to initialize */
        VQPIT_REG_STRUCT_PTR  qpit_ptr,

        /* [IN] the channel to initialize */
        uint_32   channel
    )
{
   return qpit_ptr->TIMERS[channel].LDVAL + 1;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qpit_timer_init
* Returned Value   : Number of hwticks per tick
* Comments         : Initilizes QPIT
*
*
*END*----------------------------------------------------------------------*/

static uint_32 _qpit_timer_init
    (
        /* [IN] the device to initialize */
        VQPIT_REG_STRUCT_PTR  qpit_ptr,

        /* [IN] the channel to initialize */
        uint_32   channel,

        /* [IN] ticks per second */
        uint_32   tickfreq,

        /* [IN] input clock speed in Hz */
        uint_32   clk
    )
{
   uint_32 period;

   
   
   period = (uint_32)(((clk << 1)/tickfreq + 1) >> 1);

   /* Enable PIT module, disable count on breakpoint */
   qpit_ptr->MCR = QPIT_MCR_FRZ;

   /* Disable timer and interrupt */
   qpit_ptr->TIMERS[channel].TCTRL = 0;

   qpit_ptr->TIMERS[channel].LDVAL = period-1;

   /* Clear any pending interrupt */
   qpit_ptr->TIMERS[channel].TFLG = QPIT_TFLG_TIF;

   /* Enable timer and interrupt */
   qpit_ptr->TIMERS[channel].TCTRL = (QPIT_TCTRL_TIE|QPIT_TCTRL_TEN);

   return period;
}


/*ISR*********************************************************************
*
* Function Name    : _qpit_kernel_isr
* Returned Value   : void
* Comments         :
*
*
*END**********************************************************************/

static void _qpit_kernel_isr( pointer p )
{
    VQPIT_TIMER_REG_STRUCT_PTR qpit_timer_ptr = (VQPIT_TIMER_REG_STRUCT_PTR) p;

    qpit_timer_ptr->TFLG = QPIT_TFLG_TIF;

    _time_notify_kernel();
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qpit_timer_install
* Returned Value   : MQX_OK if successful or else MQX Error
* Comments         :
*    Initilizes the Timer and installs isr to provide kernel ticks
*
*END*----------------------------------------------------------------------*/

uint_32 _qpit_timer_install
    (
        /* [IN] the PIT device to initialize */
        uint_32   device,

        /* [IN] the channel to initialize */
        uint_32   channel,

        /* [IN] ticks per second */
        uint_32   tickfreq,

        /* [IN] input clock speed in Hz */
        uint_32   clk,

        /* [IN] interrupt priority */
        uint_32 priority,

        void (_CODE_PTR_ isr_ptr)(void *)

    )
{
    VQPIT_REG_STRUCT_PTR         qpit_ptr;
    PSP_INTERRUPT_TABLE_INDEX    vector;
    uint_32 period;

    #if PSP_HAS_DEVICE_PROTECTION
    if (!_bsp_qpit_enable_access(device)) {
        return MQX_INVALID_DEVICE;
    }
    #endif

    qpit_ptr = _bsp_get_qpit_base_address(device);
    vector   = _bsp_get_qpit_vector(device, channel);
    if (qpit_ptr == NULL ||  vector == 0) {
        return MQX_INVALID_DEVICE;
    }

    _bsp_int_disable(vector);

    /* Set up tick timer */
    period = _qpit_timer_init(qpit_ptr, channel, tickfreq, clk);
    if (period == 0) {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }

    /* Install the timer interrupt handler */
    if (_int_install_isr(vector, isr_ptr, (pointer)&qpit_ptr->TIMERS[channel]) == NULL) {
        return _task_get_error();
    }

    _bsp_int_init(vector, priority, 0, TRUE);

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qpit_timer_install_kernel
* Returned Value   : MQX_OK if successful or else MQX Error
* Comments         :
*    Initilizes the Timer and installs isr to provide kernel ticks
*
*END*----------------------------------------------------------------------*/

uint_32 _qpit_timer_install_kernel
    (
        /* [IN] the PIT device to initialize */
        uint_32   device,

        /* [IN] the channel to initialize */
        uint_32   channel,

        /* [IN] ticks per second */
        uint_32   tickfreq,

        /* [IN] input clock speed in Hz */
        uint_32   clk,

        /* [IN] interrupt priority */
        uint_32   priority
    )
{
    VQPIT_REG_STRUCT_PTR         qpit_ptr = _bsp_get_qpit_base_address(device);
    PSP_INTERRUPT_TABLE_INDEX    vector   = _bsp_get_qpit_vector(device, channel);
    uint_32 result;

    if (vector == 0)
    {
        return MQX_INVALID_DEVICE;
    }

    result = _qpit_timer_install(device,channel,tickfreq,clk,priority, _qpit_kernel_isr);
    if (result != MQX_OK)
    {
        return result;
    }

    /* Initialize the timer interrupt */
    _time_set_timer_vector(vector);
    _time_set_hwtick_function(_qpit_get_hwticks, (pointer)&qpit_ptr->TIMERS[channel]);
    _time_set_hwticks_per_tick(_qpit_get_period(qpit_ptr, channel));
    _time_set_ticks_per_sec(tickfreq);

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qpit_timer_stop
* Returned Value   : MQX_OK if successful or else MQX Error
* Comments         :
*    Stops the Timer.
*
*END*----------------------------------------------------------------------*/

uint_32 _qpit_timer_stop
    (
        /* [IN] the PIT device to stop */
        uint_32   device,

        /* [IN] the channel to stop */
        uint_32   channel
    )
{
    VQPIT_REG_STRUCT_PTR         qpit_ptr;
    PSP_INTERRUPT_TABLE_INDEX    vector;

    qpit_ptr = _bsp_get_qpit_base_address(device);
    vector   = _bsp_get_qpit_vector(device, channel);
    if (qpit_ptr == NULL ||  vector == 0) {
        return MQX_INVALID_DEVICE;
    }

    _bsp_int_disable(vector);

    /* Disable timer and interrupt */
    qpit_ptr->TIMERS[channel].TCTRL = 0;

    return MQX_OK;
}





/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _qpit_init_freq
* Returned Value  : hw ticks per tick
* Comments        :
*   this function will set up a timer to interrupt
*
*END*---------------------------------------------------------------------*/

uint_32 _qpit_init_freq
    (
        /* [IN] the timer to initialize */
    	uint_32    timer,
        
        /* [IN] the channel to initialize */
        uint_32   channel,

        /* [IN] ticks per second */
        uint_32   tickfreq,

        /* [IN] input clock speed in Hz */
        uint_32   clk,

        /* [IN] unmask the timer after initializing */
        boolean   unmask_timer
    )
{
		 	    
		VQPIT_REG_STRUCT_PTR  qpit_ptr;
		uint_32 rate;
	 
		qpit_ptr = _bsp_get_qpit_base_address(timer);	 
		if (qpit_ptr == NULL) 
			return 0;
			
		_bsp_qpit_clk_en(timer);
		
		/* return number of hw-ticks per MQX tick */
		rate = _qpit_timer_init (qpit_ptr, channel, tickfreq, clk); //Initializes QPIT
		
		if (unmask_timer) {
    	_qpit_unmask_int(timer,channel);
		}
		
		return rate;
	
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _qpit_clear_int
* Returned Value  :
* Comments        :
*   This function will clear interrupt flag of given timer
*
*END*---------------------------------------------------------------------*/

void _qpit_clear_int
    (
        /* [IN] the timer to use */
    	uint_32    timer,
        
        /* [IN] the channel to initialize */
        uint_32   channel
    )
{	
    volatile VQPIT_REG_STRUCT_PTR qpit_ptr;
    
    qpit_ptr = _bsp_get_qpit_base_address(timer);
    
    if (qpit_ptr == NULL) 
    	return;
    
    qpit_ptr->TIMERS[channel].TFLG |= QPIT_TFLG_TIF;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _qpit_unmask_int
* Returned Value  :
* Comments        :
*   This function will unmask interrupt flag of given timer
*
*END*---------------------------------------------------------------------*/

void _qpit_unmask_int
    (
        /* [IN] the timer to use */
    	uint_32    timer,
        
        /* [IN] the channel to initialize */
        uint_32   channel
    )
{
		volatile VQPIT_REG_STRUCT_PTR  qpit_ptr;
		
	    qpit_ptr = _bsp_get_qpit_base_address(timer);
	    
	    if (qpit_ptr == NULL)
	    	return;
	    
		qpit_ptr->TIMERS[channel].TCTRL |= QPIT_TCTRL_TIE;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _qpit_mask_int
* Returned Value  :
* Comments        :
*   This function will mask interrupt flag of given timer
*
*END*---------------------------------------------------------------------*/

void _qpit_mask_int
    (
        /* [IN] the timer to use */
    	uint_32    timer,
        
        /* [IN] the channel to initialize */
        uint_32   channel
    )
{
	    volatile VQPIT_REG_STRUCT_PTR   qpit_ptr;
	    
	    qpit_ptr = _bsp_get_qpit_base_address(timer);
	    
	    if (qpit_ptr == NULL) 
	    	return;
	    
		qpit_ptr->TIMERS[channel].TCTRL &=~ QPIT_TCTRL_TIE;
}
