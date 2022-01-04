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
* $FileName: qrtc.c$
* $Version : 3.8.2.0$
* $Date    : Sep-26-2012$
*
* Comments:
*            PowerPC RTC driver functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <rtc.h>
#include <qrtc.h>
#if (BSP_TWRPXD20)
#include "MPXD20_cw.h"
#endif

static RTC_TIME_STRUCT real_time = {0};
static RTC_TIME_STRUCT alarm_time = {0};
static uint_32 stopwatch = 0;
static uint_32 user_enables = 0;
static uint_32 user_requests = 0;

void _rtc_time_to_mqx_time (RTC_TIME_STRUCT_PTR rtc_time, TIME_STRUCT_PTR mqx_time);
void _rtc_time_from_mqx_time (TIME_STRUCT_PTR mqx_time, RTC_TIME_STRUCT_PTR rtc_time);
_mqx_int _rtc_sync_with_mqx (boolean update_mqx);
void _api_isr (pointer ptr);

INT_ISR_FPTR user_isr = (INT_ISR_FPTR)NULL;

uint_32  _rtc_init (uint_32 flags) {
    uint_32 result = MQX_OK;
    VQRTC_REG_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    rtc->RTCC = 0x00;
    /* clear all interrupt flags*/
    rtc->RTCS |= (QRTC_RTCS_RTCF | QRTC_RTCS_ROVRF | QRTC_RTCS_APIF);
    /* disable module */
    rtc->RTCC &= ~QRTC_RTCC_CNTEN;
    
#if (!BSP_TWRPXN20)
    rtc->RTCSUPV = 0;
#endif
    
    
    /* select clock */
    rtc->RTCC &= ~QRTC_RTCC_CLKSEL(0x11);
    rtc->RTCC |= QRTC_RTCC_CLKSEL(0x01);

    /* clear time, alarm and stopwatch, if requested */
    if (flags & RTC_INIT_FLAG_CLEAR) {      
        real_time.seconds = 0;
        real_time.minutes = 0;
        real_time.hours = 0;
        real_time.days = 0;
        alarm_time.seconds = 0;
        alarm_time.minutes = 0;
        alarm_time.hours = 0;
        alarm_time.days = 0;
        stopwatch = 0;
    }

    /* reset current user interrupt state and callback, if requested */
    if (flags & RTC_INIT_FLAG_RESET) {
        user_requests = 0;
        user_enables = 0;
        user_isr = (INT_ISR_FPTR)NULL;
        stopwatch = (uint_32)(-1);
    }
    /* install HW interrupt and run the RTC, if requested */
    if (flags & RTC_INIT_FLAG_ENABLE) {
        if (flags & RTC_INIT_API_ENABLE) {
            if (NULL == _int_install_isr (_bsp_get_api_vector (),  _api_isr, (pointer)rtc)) result = _task_get_error ();
            if (MQX_OK == result) {
                _bsp_int_init(_bsp_get_api_vector (), BSP_API_INT_LEVEL, 0, TRUE);
                /* API Compare Value */
                rtc->RTCC &= ~QRTC_RTCC_APIVAL(0x3ff);
                rtc->RTCC |= QRTC_RTCC_APIVAL(API_VALUE);
                rtc->RTCC |= QRTC_RTCC_APIEN;
                /* Divide by 32 */
                rtc->RTCC |= QRTC_RTCC_DIV32EN;
                /* enable interrupt */
                rtc->RTCC |= QRTC_RTCC_APIIE;
            }
        }
        else {
            if (NULL == _int_install_isr (_bsp_get_rtc_vector (),  _rtc_isr, (pointer)rtc)) result = _task_get_error ();
            if (MQX_OK == result) {
                _bsp_int_init(_bsp_get_rtc_vector (), BSP_RTC_INT_LEVEL, 0, TRUE);
#if (!BSP_TWRPXN20)
                CGM.SIRC_CTL.B.LPRCDIV = 0;
#endif
                /* RTC Compare Value */
                rtc->RTCC &= ~QRTC_RTCC_CNTEN;
                rtc->RTCC &= ~QRTC_RTCC_RTCVAL(0xfff);
                rtc->RTCC |= QRTC_RTCC_RTCVAL(RTC_VALUE);
                rtc->RTCS |= QRTC_RTCS_RTCF;
            }
        }
        /* enable interrupt */
        rtc->RTCC |= QRTC_RTCC_RTCIE;
    }
    /* Counter does not freeze in debug mode */
    rtc->RTCC |= QRTC_RTCC_FRZEN;
    /* enable module */
    rtc->RTCC |= QRTC_RTCC_CNTEN;
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
    VQRTC_REG_STRUCT_PTR rtc = (VQRTC_REG_STRUCT_PTR)ptr;
    /* clear compare interrupt of RTC module & reset counter*/
    rtc->RTCC &= ~QRTC_RTCC_CNTEN;
    rtc->RTCS |= QRTC_RTCS_RTCF;
    rtc->RTCC |= QRTC_RTCC_CNTEN;
    user_requests |= QRTC_RTCISR_1HZ;   /* second boundary */
    real_time.seconds++;
    if (real_time.seconds > 59)         /* minute boundary */
    {
        real_time.seconds = 0;
        user_requests |= QRTC_RTCISR_MIN;
        if (stopwatch != (uint_32)-1) {   /* stopwatch decrementer */
            stopwatch--;
            if (stopwatch == (uint_32)-1) {
                user_requests |= QRTC_RTCISR_SW;
            }
        }          
        real_time.minutes++;
        if (real_time.minutes > 59)    /* hour boundary */
        {
            real_time.minutes = 0;
            user_requests |= QRTC_RTCISR_HR;
            real_time.hours++;
            if (real_time.hours > 23)    /* days boundary */
            {
                real_time.hours = 0;
                user_requests |= QRTC_RTCISR_DAY;
                real_time.days++;
            }    
        }    
    }
    /* alarm checking */
    if ((real_time.seconds == alarm_time.seconds) && (real_time.minutes == alarm_time.minutes) && (real_time.hours == alarm_time.hours) && (real_time.days == alarm_time.days)) {
        user_requests |= QRTC_RTCISR_ALM;
    }
    /* user callback triggering */
    if (((INT_ISR_FPTR)NULL != user_isr) && (user_requests & user_enables)) user_isr ((pointer)rtc);
}

/*FUNCTION****************************************************************
* 
* Function Name    : _api_isr
* Returned Value   : none
* Comments         :
*    This is ISR for API mode, triggered each second.
*
*END*********************************************************************/
void _api_isr 
(
    /* [IN] rtc module pointer passed to interrupt */
    pointer ptr
)
{
    VQRTC_REG_STRUCT_PTR rtc = (VQRTC_REG_STRUCT_PTR)ptr;
    /* clear compare interrupt of RTC module */
    rtc->RTCS |= QRTC_RTCS_APIF;
    user_requests |= QRTC_RTCISR_1HZ;   /* second boundary */
    real_time.seconds++;
    if (real_time.seconds > 59)         /* minute boundary */
    {
        real_time.seconds = 0;
        user_requests |= QRTC_RTCISR_MIN;
        if (stopwatch != (uint_32)-1) {   /* stopwatch decrementer */
            stopwatch--;
            if (stopwatch == (uint_32)-1) {
                user_requests |= QRTC_RTCISR_SW;
            }
        }          
        real_time.minutes++;
        if (real_time.minutes > 59)    /* hour boundary */
        {
            real_time.minutes = 0;
            user_requests |= QRTC_RTCISR_HR;
            real_time.hours++;
            if (real_time.hours > 23)    /* days boundary */
            {
                real_time.hours = 0;
                user_requests |= QRTC_RTCISR_DAY;
                real_time.days++;
            }    
        }    
    }
    /* alarm checking */
    if ((real_time.seconds == alarm_time.seconds) && (real_time.minutes == alarm_time.minutes) && (real_time.hours == alarm_time.hours) && (real_time.days == alarm_time.days)) {
        user_requests |= QRTC_RTCISR_ALM;
    }
    /* user callback triggering */
    if (((INT_ISR_FPTR)NULL != user_isr) && (user_requests & user_enables)) user_isr ((pointer)rtc);
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
    if (enable){
        user_enables |= bitmask;
    } else {
        user_enables &= (~ bitmask);
    }
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
    user_requests &= (~ bitmask);
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_get_status
* Returned Value   : bitmask of actual RTC interrupt requests + RTC enabled bit
* Comments         :
*    This function returns bitmask of pending RTC interrupt requests along
*    with RTC enabled bit (bit QRTC_RTCISR_EN).
*
*END*********************************************************************/
uint_32 _rtc_get_status 
(
    void
)
{
    VQRTC_REG_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    uint_32 result = user_requests;
    if (rtc->RTCC & QRTC_RTCC_RTCIE) result |= QRTC_RTCISR_EN;
    return result;
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
    VQRTC_REG_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    uint_32 tmp = rtc->RTCC & QRTC_RTCC_RTCIE;   
    rtc->RTCC &= ~(QRTC_RTCC_RTCIE);             
    real_time = *time;                                    
    if (real_time.seconds > 59) real_time.seconds = 59;   
    if (real_time.minutes > 59) real_time.minutes = 59;
    if (real_time.hours > 23) real_time.hours = 23;
    rtc->RTCS |= QRTC_RTCS_RTCF;                 
    rtc->RTCC |= tmp;                                    

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
    RTC_TIME_STRUCT tmp;  
    time->hours = 0xFF;
    do   // loop until same time is read twice - keeping time integrity
    {
        tmp = *time;
        *time = real_time;
    } while ((tmp.seconds != time->seconds) || (tmp.minutes != time->minutes) || (tmp.hours != time->hours) || (tmp.days != time->days));
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
    uint_32 enables = user_enables & QRTC_RTCIENR_ALM;
    user_enables &= (~ QRTC_RTCIENR_ALM);
    alarm_time = *time;
    if (alarm_time.seconds > 59) alarm_time.seconds = 59;
    if (alarm_time.minutes > 59) alarm_time.minutes = 59;
    if (alarm_time.hours > 23) alarm_time.hours = 23;
    user_requests &= (~ QRTC_RTCISR_ALM);
    user_enables |= enables;
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
    *time = alarm_time;
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
    uint_32 enables = user_enables & QRTC_RTCIENR_SW;
    user_enables &= (~ QRTC_RTCIENR_SW);
    stopwatch = QRTC_STPWCH_CNT(minutes);
    user_requests &= (~ QRTC_RTCISR_SW);
    user_enables |= enables;
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
    return stopwatch;
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
    TIME_STRUCT_PTR mqx_time
)
{
    mqx_time->MILLISECONDS = 0;
    mqx_time->SECONDS = rtc_time->seconds + (rtc_time->minutes * 60L) + (rtc_time->hours * 3600L) + (rtc_time->days * 86400L);
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
    TIME_STRUCT_PTR mqx_time, 
    /* [OUT] RTC time representation */
    RTC_TIME_STRUCT_PTR rtc_time
)
{
    register uint_32 div, mod;
    
    mod  = mqx_time->SECONDS;
    div  = mod / 86400;
    mod -= div * 86400;
    rtc_time->days = (uint_16)div;
    div  = mod / 3600;
    mod -= div * 3600;
    rtc_time->hours = (uint_8)div;
    div  = mod / 60;
    mod -= div * 60;
    rtc_time->minutes = (uint_8)div;
    rtc_time->seconds = (uint_8)mod;
}


/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_sync_with_mqx
* Returned Value   : none
* Comments         :
*    This function synchronizes RTC time with MQX time.
*
*END*********************************************************************/
_mqx_int _rtc_sync_with_mqx 
(
    /* [IN] whether to update MQX time (source is RTC) or RTC time (source is MQX) */
    boolean update_mqx
)
{
    RTC_TIME_STRUCT rtc_time;
    TIME_STRUCT mqx_time;
    
    if (update_mqx) 
    {
        _rtc_get_time (&rtc_time);
        _rtc_time_to_mqx_time (&rtc_time, &mqx_time);
        _time_set (&mqx_time);
    }
    else
    {
        _time_get (&mqx_time);
        _rtc_time_from_mqx_time (&mqx_time, &rtc_time);
        _rtc_set_time (&rtc_time);
    }
    return MQX_OK;
}

/* EOF */
