/**HEADER********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: krtc.c$
* $Version : 3.8.18.0$
* $Date    : Oct-7-2011$
*
* Comments:
*            Kinetis RTC driver functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

static RTC_TIME_STRUCT alarm_time,
                       sw_time;
static uint_32 user_enables = 0;
static uint_32 user_requests = 0;

INT_ISR_FPTR user_isr = (INT_ISR_FPTR)NULL;
static void _rtc_update_TAR( uint_32 );

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_wait_ms
* Returned Value   :
* Comments         :
*
* wait at least ms_delay ms
*
*END*********************************************************************/
static void _rtc_wait_ms( uint_32 ms_delay )
{

   uint_32 us = ms_delay * 1000;
   uint_32 inst_per_us = BSP_SYSTEM_CLOCK / 1000000;
   uint_32 inst_to_exec = us * inst_per_us;

   for (; inst_to_exec != 0; inst_to_exec--) {
       _ASM_NOP();
   } /* Endfor */
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_init
* Returned Value   : MQX error code
* Comments         :
*    This function (re)initializes/clears/enables RTC module.
*    If cleared, alarm happens after RTC overflow, stopwatch happens next minute.
*    Reset disables and clears all interrupts and stopwatch (even if cleared).
*
*END*********************************************************************/
uint_32 _rtc_init
(
    /* [IN] flags identifying init operations */
    uint_32 flags
)
{
    uint_32 result = MQX_OK;
    RTC_MemMapPtr rtc = RTC_BASE_PTR;

    /* Clear time, alarm and stopwatch, if requested */
    if (flags & RTC_INIT_FLAG_CLEAR)
    {
        _int_disable();
        /* Resets all RTC registers except for SWR bit. */
        rtc->CR |= RTC_CR_SWR_MASK;
        alarm_time.seconds = 0;
        sw_time.seconds = 0;
        user_enables = 0;
        user_requests = 0;
        rtc->CR &= ~RTC_CR_SWR_MASK;
        rtc->TAR = 0xFFFFFFFF; /* e2574: RTC: Writing RTC_TAR[TAR] = 0 does not disable RTC alarm */
        rtc->TSR = 1;
        _int_enable();
    }
    /* reset current user interrupt state and callback, if requested */
    if (flags & RTC_INIT_FLAG_RESET)
    {
        _int_disable();
        /* Resets all RTC registers except for SWR bit. */
        rtc->CR |= RTC_CR_SWR_MASK;
        rtc->CR &= ~RTC_CR_SWR_MASK;
        rtc->TSR = 1;
        user_enables = 0;
        user_requests = 0;
        _int_enable();
    }

    /* install HW interrupt and run the RTC, if requested */
    if (flags & RTC_INIT_FLAG_ENABLE)
    {
        /* disable reset RTC */
        if( rtc->CR & RTC_CR_SWR_MASK )
        {
            rtc->CR &= ~RTC_CR_SWR_MASK;
        }

        if (NULL == _int_install_isr (INT_RTC, _rtc_isr, (pointer)rtc))
        {
            result = _task_get_error ();
        }

        if (MQX_OK == result)
        {
            if( !(rtc->SR & RTC_SR_TCE_MASK) || !(rtc->CR & RTC_CR_OSCE_MASK) )
            {
                rtc->CR |= RTC_CR_OSCE_MASK;
                /* recommended 125 ms delay for oscillator start */
                _rtc_wait_ms(125);
                rtc->SR |= RTC_SR_TCE_MASK;
            }
            _bsp_int_init(INT_RTC, BSP_RTC_INT_LEVEL, 0, TRUE);

        }
    }
    
#if PSP_MQX_CPU_IS_KINETIS_K70
    /* Enable monotonic counter */
    RTC_MER = RTC_MER_MCE_MASK;
#if 0
    /* Enable Tamper */
    RTC_TER =   (RTC_TER_TME_MASK | \
                 RTC_TER_VTE_MASK | \
                 RTC_TER_CTE_MASK | \
                 RTC_TER_TTE_MASK | \
                 RTC_TER_FSE_MASK | \
                 RTC_TER_TME_MASK);

#else    
    /* Disable all tampers */
    RTC_TER = 0;
#endif
        
        
#endif    
    
    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_isr
* Returned Value   : none
* Comments         :
*    This is ISR for RTC module, triggered each second.
*
*END*********************************************************************/
void _rtc_isr
(
    /* [IN] rtc module pointer passed to interrupt */
    pointer ptr
)
{
    RTC_MemMapPtr rtc = RTC_BASE_PTR;
    uint_32 tsr_reg, status;
    uint_32 time_alarm;
    boolean alarm_fault = TRUE;

    status = rtc->SR;

    /* Time Invalid Flag, Time Overflow Flag */
    if (status & (RTC_SR_TIF_MASK | RTC_SR_TOF_MASK))
    {
        rtc->SR &= ~RTC_SR_TCE_MASK;    /* disable rtc timer - enable write access */
        rtc->TAR = 0xFFFFFFFF; /* e2574: RTC: Writing RTC_TAR[TAR] = 0 does not disable RTC alarm */
        /* if TIF or TOF flag is set, reading TSR return zero and we must set TSR to one */
        rtc->TSR = 1;                   /* this clear SR flags TIF, TOF */
        rtc->SR |= RTC_SR_TCE_MASK;

        user_requests |= ((status & RTC_SR_TIF_MASK) ? RTC_ISR_TIF : 0) | ((status & RTC_SR_TOF_MASK) ? RTC_ISR_TOF : 0);
    }

    /* Time Alarm Flag */
    if (status & RTC_SR_TAF_MASK)
    {
        /* TAR can be changed, store register to variable */
        time_alarm = rtc->TAR;
        if( time_alarm == alarm_time.seconds )
        {
            user_requests |= RTC_ISR_ALARM;
            _rtc_update_TAR( user_enables & ~RTC_ISR_ALARM );
            alarm_fault = FALSE;
        }
        if( time_alarm == sw_time.seconds )
        {
            user_requests |= RTC_ISR_STOPWATCH;
            _rtc_update_TAR( user_enables & ~RTC_ISR_STOPWATCH);
            alarm_fault = FALSE;
        }
        if( alarm_fault == TRUE )
        {
            _rtc_update_TAR( user_enables & ~( RTC_ISR_STOPWATCH | RTC_ISR_ALARM) );
        }
    }
    /* user callback */
    if (((INT_ISR_FPTR)NULL != user_isr) && (user_requests & user_enables))
    {
        user_isr ((pointer)rtc);
    }
}



/*FUNCTION****************************************************************
*
* Function Name    : _rtc_int_install
* Returned Value   : MQX error code
* Comments         :
*    This function installs given user callback for RTC module.
*
*END*********************************************************************/
uint_32 _rtc_int_install
(
    /* [IN] pointer to user ISR code */
    INT_ISR_FPTR isr
)
{
    if( isr == NULL )
    {
        return MQX_INVALID_POINTER;
    }
    user_isr = isr;
    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_int_enable
* Returned Value   : bitmask of new int enable state
* Comments         :
*    This function enables/disables RTC interrupts according to specified bitmask.
*
*END*********************************************************************/
uint_32 _rtc_int_enable
(
    /* [IN] whether to enable or disable interrupts */
    boolean enable,
    /* [IN] bitmask of affected interrupts */
    uint_32 bitmask
)
{
    if( enable == TRUE )
    {
        user_enables |= bitmask;
    }
    else
    {
        user_enables &= ~bitmask;
    }
    _bsp_int_init(INT_RTC, BSP_RTC_INT_LEVEL, 0, enable);
    return user_enables;
}


/*FUNCTION****************************************************************
*
* Function Name    : _rtc_clear_requests
* Returned Value   : none
* Comments         :
*    This function clears RTC interrupt requests according to given bitmask.
*
*END*********************************************************************/
void _rtc_clear_requests
(
    /* [IN] bitmask of affected interrupts */
    uint_32 bitmask
)
{
    user_requests &= ~bitmask;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_status
* Returned Value   : bitmask of actual RTC interrupt requests + RTC enabled bit
* Comments         :
*    This function returns bitmask of pending RTC interrupt requests along
*    with RTC enabled bit (bit MCF51XX_RTC_RTCISR_EN).
*
*END*********************************************************************/
uint_32 _rtc_get_status
(
    void
)
{
    RTC_MemMapPtr rtc = RTC_BASE_PTR;

    return ((rtc->SR & RTC_SR_TCE_MASK) ? user_requests : 0);
}

/*FUNCTION****************************************************************
*
* Function Name    : _is_leap_year
* Returned Value   : none
* Comments         :
*    This function returns 1 if year is leaf.
*
*END*********************************************************************/
static uint_32 _is_leap_year( uint_16 year )
{
    uint_32 result = 0;

    if( year % 4 == 0 )
    {
        /* leap year, add day */
        result = 1;
        if( year % 100 == 0)
        {
            /* is not leap year, substract day*/
            result = 0;
        }
        if( year % 400 == 0)
        {
            /* finally is leap year, get back one more day */
            result = 1;
        }
    }
    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _real_time_to_sec
* Returned Value   : none
* Comments         :
*    This function convert date to secs since 1.1.1970 0:0:0".
*
*END*********************************************************************/
static uint_32 _date_to_sec( DATE_STRUCT_PTR time )
{
    #define SEC_IN_MIN  60
    #define SEC_IN_HOUR 3600
    #define SEC_IN_DAY  86400
    #define FEBRUARY    2
    #define ZERO_YEAR   1970

    static const uint_16 days_no_table[]= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint_32 days = 0;
    uint_32 month;
    uint_32 year;

    days = 365 * (time->YEAR - ZERO_YEAR);
    /* get leap year */
    for( year = ZERO_YEAR; year < time->YEAR; year++)
    {
        days += _is_leap_year( year );
    }

    for( month = 1; month < time->MONTH; month++)
    {
        days+= days_no_table[month - 1];
        /* is leap year? */
        if( month == FEBRUARY )
        {
            days+= _is_leap_year( time->YEAR );
        }
    }

    days += time->DAY-1;
    return days*SEC_IN_DAY + time->HOUR*SEC_IN_HOUR + time->MINUTE*SEC_IN_MIN + time->SECOND;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_set_time
* Returned Value   : none
* Comments         :
*    This function sets the RTC time according to given time struct.
*
*END*********************************************************************/
void _rtc_set_time
(
    /* [IN] given time to be set as RTC time */
    RTC_TIME_STRUCT_PTR time
)
{
    RTC_MemMapPtr rtc = RTC_BASE_PTR;

    /* disable RTC */
    rtc->SR &= ~RTC_SR_TCE_MASK;
    /* set new time */
    rtc->TSR = time->seconds;
    /* enable RTC */
    rtc->SR |= RTC_SR_TCE_MASK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_time
* Returned Value   : none
* Comments         :
*    This function gets the RTC time and stores it in given time struct.
*
*END*********************************************************************/
void _rtc_get_time
(
    /* [OUT] this parameter gets actual RTC time */
    RTC_TIME_STRUCT_PTR time
)
{
    RTC_MemMapPtr rtc = RTC_BASE_PTR;

    time->seconds = rtc->TSR;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_update_TAR
* Returned Value   : none
* Comments         :
*    This function set stopwatch or alarm time to TAR register.
*
*END*********************************************************************/
static void _rtc_update_TAR( uint_32 user_enables_private )
{
    RTC_MemMapPtr rtc = RTC_BASE_PTR;
    uint_32 user_enables_tmp = user_enables_private & (RTC_ISR_STOPWATCH | RTC_ISR_ALARM);

    /* all disabled, no action */
    if(user_enables_tmp  == 0)
    {
        rtc->TAR = 0xFFFFFFFF; /* e2574: RTC: Writing RTC_TAR[TAR] = 0 does not disable RTC alarm */

        return;
    }

    if(user_enables_tmp == (RTC_ISR_STOPWATCH | RTC_ISR_ALARM))
    {
        user_enables_tmp = rtc->TSR;
        rtc->TAR = (alarm_time.seconds - user_enables_tmp <= sw_time.seconds - user_enables_tmp) ? alarm_time.seconds : sw_time.seconds;
    }
    else
    {
        rtc->TAR = (user_enables_private & RTC_ISR_ALARM) ? alarm_time.seconds : sw_time.seconds;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_set_alarm
* Returned Value   : none
* Comments         :
*    This function sets RTC alarm time according to given time struct.
*    Alarm happens immediately after match.
*
*END*********************************************************************/
void _rtc_set_alarm
(
    /* [IN] time to be set as RTC alarm time */
    RTC_TIME_STRUCT_PTR time
)
{
    alarm_time.seconds = time->seconds;
    user_enables |= RTC_ISR_ALARM;
    _rtc_update_TAR (user_enables);
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_alarm
* Returned Value   : none
* Comments         :
*    This function gets the RTC alarm time and stores it in given time struct.
*
*END*********************************************************************/
void _rtc_get_alarm
(
    /* [OUT] this parameter gets the RTC alarm time */
    RTC_TIME_STRUCT_PTR time
)
{
    time->seconds = 0;
    if( user_enables & RTC_ISR_ALARM )
    {
        time->seconds = alarm_time.seconds;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_set_stopwatch
* Returned Value   : none
* Comments         :
*    This function sets the RTC stopwatch decrementer value (minutes).
*    Stopwatch decrements each new RTC minute and stops (disables) at -1.
*
*END*********************************************************************/
void _rtc_set_stopwatch
(
    /* [IN] number of minutes to countdown */
    uint_32 minutes
)
{
    uint_32 stopwatch_time;
    RTC_MemMapPtr rtc = RTC_BASE_PTR;

    if( (int_32)minutes == -1)
    {
        user_enables &= ~RTC_ISR_STOPWATCH;
    }
    else
    {
        /* minutes to seconds */
        stopwatch_time = (minutes * 60) + rtc->TSR;
        sw_time.seconds = stopwatch_time;
        /* set stopwatch enable flag */
        user_enables |= RTC_ISR_STOPWATCH;
    }
    _rtc_update_TAR( user_enables );

}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_stopwatch
* Returned Value   : actual RTC minute stopwatch counter value
* Comments         :
*    This function returns the actual value of RTC stopwatch decrementer.
*
*END*********************************************************************/
uint_32 _rtc_get_stopwatch
(
    void
)
{
    uint_32 stopwatch_time;
    RTC_MemMapPtr rtc = RTC_BASE_PTR;

    if( user_enables & RTC_ISR_STOPWATCH )
    {
        if( sw_time.seconds >= rtc->TSR)
        {
            stopwatch_time = sw_time.seconds - rtc->TSR;
            return stopwatch_time / 60;
        }
    }
    return STOPWATCH_OFF;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_set_time_mqxd
* Returned Value   : actual RTC minute stopwatch counter value
* Comments         :
*    This function sets the RTC time according to mqx DATE_STRUCT.
*
*END*********************************************************************/

_mqx_int _rtc_set_time_mqxd (DATE_STRUCT_PTR time)
{
    RTC_TIME_STRUCT rtc_time;

    /* convert MQX_DATE to seconds */
    rtc_time.seconds = _date_to_sec( time );
    _rtc_set_time( &rtc_time );
    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_time_mqxd
* Returned Value   : actual RTC minute stopwatch counter value
* Comments         :
*    This function returns rtc time to DATE_STRUCT.
*
*END*********************************************************************/

void _rtc_get_time_mqxd (DATE_STRUCT_PTR time)
{
    RTC_MemMapPtr rtc = RTC_BASE_PTR;
    TIME_STRUCT         rtc_time;

    rtc_time.SECONDS = rtc->TSR;
    rtc_time.MILLISECONDS=0;

    _time_to_date( &rtc_time, time );
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_set_alarm_mqxd
* Returned Value   : none
* Comments         :
*    This function sets RTC alarm time.
*    Alarm happens immediately after match.
*
*END*********************************************************************/

_mqx_int _rtc_set_alarm_mqxd (DATE_STRUCT_PTR time)
{
    RTC_TIME_STRUCT rtc_alarm;

    /* convert MQX_DATE to seconds */
    rtc_alarm.seconds = _date_to_sec( time );
    _rtc_set_alarm( &rtc_alarm );
    return MQX_OK;

}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_get_alarm_mqxd
* Returned Value   : none
* Comments         :
*    This function gets the RTC alarm time and stores it in DATE_STRUCT.
*
*END*********************************************************************/

void _rtc_get_alarm_mqxd (DATE_STRUCT_PTR time)
{
    TIME_STRUCT         rtc_time;

    rtc_time.SECONDS = alarm_time.seconds;
    rtc_time.MILLISECONDS=0;

    _time_to_date( &rtc_time, time );
}


/*FUNCTION****************************************************************
*
* Function Name    : _rtc_time_to_mqx_date
* Returned Value   : none
* Comments         :
*    This function transforms RTC time format to MQX date format.
*    Miliseconds are set to 0.
*
*END*********************************************************************/

void _rtc_time_to_mqx_date
(
    /* [IN] RTC time representation */
    RTC_TIME_STRUCT_PTR rtc_time,
    /* [OUT] MQX date representation */
    DATE_STRUCT_PTR     mqx_date
)
{
    TIME_STRUCT         time;

    time.SECONDS = rtc_time->seconds;
    time.MILLISECONDS=0;

    _time_to_date( &time, mqx_date );

}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_time_from_mqx_date
* Returned Value   : none
* Comments         :
*    This function transforms MQX date format to RTC time format.
*    Miliseconds are ignored.
*
*END*********************************************************************/

void _rtc_time_from_mqx_date
(
    /* [IN] MQX date representation */
    DATE_STRUCT_PTR     mqx_date,
    /* [OUT] RTC time representation */
    RTC_TIME_STRUCT_PTR rtc_time
)
{
    rtc_time->seconds = _date_to_sec( mqx_date );
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_sync_with_mqx
* Returned Value   : none
* Comments         :
*    This function synchronizes RTC time with MQX date.
*
*END*********************************************************************/

_mqx_int _rtc_sync_with_mqx
(
    /* [IN] whether to update MQX time (source is RTC) or RTC time (source is MQX) */
    boolean update_mqx
)
{
    RTC_TIME_STRUCT rtc_time;
    DATE_STRUCT     mqx_date;
    TIME_STRUCT     mqx_time;

    if (update_mqx == TRUE)
    {
        _rtc_get_time(&rtc_time);
        _rtc_time_to_mqx_date(&rtc_time, &mqx_date);
        _time_from_date(&mqx_date, &mqx_time);
        _time_set(&mqx_time);
    }
    else
    {
        _time_get(&mqx_time);
        _time_to_date(&mqx_time, &mqx_date);
        _rtc_time_from_mqx_date (&mqx_date, &rtc_time);
        _rtc_set_time (&rtc_time);
    }
    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_time_to_mqx_time
* Returned Value   : none
* Comments         :
*    This function transforms RTC time format to MQX time format.
*    RTC time range is wider (65536 days vs. 49710 days), overflow is not checked,
*    miliseconds are set to 0.
*
*END*********************************************************************/

void _rtc_time_to_mqx_time
(
    /* [IN] RTC time representation */
    RTC_TIME_STRUCT_PTR rtc_time,
    /* [OUT] MQX time representation */
    TIME_STRUCT_PTR     mqx_time
)
{
    mqx_time->SECONDS = rtc_time->seconds;
    mqx_time->MILLISECONDS = 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _rtc_time_from_mqx_time
* Returned Value   : none
* Comments         :
*    This function transforms MQX time format to RTC time format.
*    MQX time range is shorter (49710 days vs. 65536 days), miliseconds are ignored.
*
*END*********************************************************************/

void _rtc_time_from_mqx_time
(
    /* [IN] MQX time representation */
    TIME_STRUCT_PTR     mqx_time,
    /* [OUT] RTC time representation */
    RTC_TIME_STRUCT_PTR rtc_time
)
{
     rtc_time->seconds = mqx_time->SECONDS;
}
/* EOF */
