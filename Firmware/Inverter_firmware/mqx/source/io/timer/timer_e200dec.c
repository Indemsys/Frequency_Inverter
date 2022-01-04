/**HEADER********************************************************************
*
* Copyright (c) 2008-2009 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: timer_e200dec.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*    This file contains definitions for the PPC e300 core decrementer
*    timer utility functions.
*
*END***********************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "timer_e200dec.h"


/*ISR*********************************************************************
*
* Function Name    : _e200_decrementer_null_isr
* Returned Value   : void
* Comments         :
*    The decrementer ISR.  Needed because the decrementer WILL interrupt
*
*END**********************************************************************/

static void _e200_decrementer_null_isr(pointer dummy)
{
   _PSP_SPR_SET(E200CORE_TSR, E200CORE_TSR_DIS);

}


/*ISR*********************************************************************
*
* Function Name    : _e200_decrementer_kernel_isr
* Returned Value   : void
* Comments         :
*    The decrementer ISR.  Needed because the decrementer WILL interrupt
*
*END**********************************************************************/

static void _e200_decrementer_kernel_isr(pointer dummy)
{
   _PSP_SPR_SET(E200CORE_TSR, E200CORE_TSR_DIS);

   _time_notify_kernel();
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _e200_decrementer_get_hwticks
* Returned Value   : MQX_OK if successful or else MQX Error
* Comments         :
*
*
*END*----------------------------------------------------------------------*/

static uint_32 _e200_decrementer_get_hwticks(pointer p)
{
   uint_32 result, tsr, period = (uint_32)p;

   _PSP_SPR_GET(result,E200CORE_DEC);
   _PSP_SPR_GET(tsr,E200CORE_TSR);

   result = period - result;

   if ((tsr & E200CORE_TSR_DIS) != 0) {
      _PSP_SPR_GET(result,E200CORE_DEC);
      result = period - result;
      result += period;
   }

   return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _e200_decrementer_timer_init
* Returned Value   : MQX_OK if successful or else MQX Error
* Comments         :
*    Initilizes the Decrementer Timer to generate MQX BSP ticks
*
*END*----------------------------------------------------------------------*/

static uint_32 _e200_decrementer_timer_init
   (
      /* [IN] the tick rate wanted */
      uint_32 tickfreq,

      /* [IN] Input clock frequency */
      uint_32 clk
   )
{
   uint_32 period;
   uint_32 tmp;

   _PSP_SPR_SET(E200CORE_DEC, 0);

   period = (uint_32)( ((clk << 1)/tickfreq + 1) >> 1 );

   _PSP_SPR_GET(tmp, E200CORE_TCR);
   tmp |= E200CORE_TCR_DIE | E200CORE_TCR_ARE; /* interrupt + Auto-reload enable */
   _PSP_SPR_SET(E200CORE_TCR, tmp);

   /* preload value is one less than period */
   _PSP_SPR_SET(E200CORE_DEC, period-1);
   _PSP_SPR_SET(E200CORE_DECAR, period-1);     /* decrementer auto-reload */

   /* enable clocks for TimeBase - this applies also to decrementer */
   _PSP_SPR_GET(tmp, E200CORE_HID0);
   tmp |= E200CORE_HID0_TBEN;
   _PSP_SPR_SET(E200CORE_HID0, tmp);

   return period;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _e200_decrementer_timer_install
* Returned Value   : MQX_OK if successful or else MQX Error
* Comments         :
*    Initilizes the Decrementer Timer to generate MQX BSP ticks
*
*END*----------------------------------------------------------------------*/

uint_32 _e200_decrementer_timer_install
   (
      /* [IN] the tick rate wanted */
      uint_32 tickfreq,

      /* [IN] Input clock frequency */
      uint_32 clk
   )
{
    uint_32 period;

    /* Set up tick timer */
    period = _e200_decrementer_timer_init(tickfreq, clk);
    if (period == 0) {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }

    /* Install the decrementer interrupt handler */
    if (_int_install_isr(PSP_EXCPT_DECREMENTER, _e200_decrementer_kernel_isr, NULL) == NULL)
    {
        return _task_get_error();
    }

    /* Initialize the timer interrupt */
    _time_set_timer_vector(PSP_EXCPT_DECREMENTER);
    _time_set_hwtick_function(_e200_decrementer_get_hwticks, (pointer)period);
    _time_set_hwticks_per_tick(period);
    _time_set_ticks_per_sec(tickfreq);

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _e200_decrementer_null_install
* Returned Value   : MQX_OK if successful or else MQX Error
* Comments         :
*
*
*END*----------------------------------------------------------------------*/

uint_32 _e200_decrementer_null_install(void)
{
   /* Install the decrementer interrupt handler */
   if (_int_install_isr(PSP_EXCPT_DECREMENTER,_e200_decrementer_null_isr, NULL) == NULL)
   {
      return _task_get_error();
   }

   return MQX_OK;
}
