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
* $FileName: rtc_mcf52xx.c$
* $Version : 3.7.11.0$
* $Date    : Mar-16-2011$
*
* Comments:
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <rtc.h>
#include <mcf52xx_rtc.h>

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_register_write
* Returned Value   : previous value of RTC register
* Comments         :
*    This function writes given value to specified RTC register (size32 or size16)
*    according to RTC register access protocol.
*
*END*********************************************************************/
static uint_32 _rtc_register_write 
(
    /* [IN] pointer to RTC register to write to */
    pointer reg, 
    /* [IN] whether 16 or 32 bit register write */
    boolean size32, 
    /* [IN] value to assign */
    uint_32 value
)
{
    vuint_32 result;
    if (size32)
    {
        result = result;
        result = *(vuint_32 *)reg;
        *(vuint_32 *)reg = value;
    }
    else
    {
        result = result;
        result = *(vuint_16 *)reg;
        *(vuint_16 *)reg = value;
    }
    return result;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_register_read
* Returned Value   : actual RTC register value
* Comments         :
*    This function returns the value of given RTC register (size32 or size16)
*    according to RTC register access protocol.
*
*END*********************************************************************/
static uint_32 _rtc_register_read 
(
    /* [IN] pointer to RTC register to read from */
    pointer reg, 
    /* [IN] whether 16 or 32 bit register read */
    boolean size32
)
{
    vuint_32 result;
    if (size32)
    {
        result = result;
        result = *(vuint_32 *)reg;
        result = *(vuint_32 *)reg;
    }
    else
    {
        result = result;
        result = *(vuint_16 *)reg;
        result = *(vuint_16 *)reg;
    }
    return result;
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
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    _rtc_register_write((pointer)&rtc->RTCCTL, TRUE, 0);
    if (flags & RTC_INIT_FLAG_CLEAR)
    {
        _rtc_register_write ((pointer)&rtc->DAYS, FALSE, MCF52XX_RTC_DAYS_DAYS(0));
        _rtc_register_write ((pointer)&rtc->SECONDS, TRUE, MCF52XX_RTC_SECONDS_WRITE(0));
        _rtc_register_write ((pointer)&rtc->HOURMIN, TRUE, MCF52XX_RTC_HOURMIN_HOURS_WRITE(0) | MCF52XX_RTC_HOURMIN_MINUTES_WRITE(0));
        _rtc_register_write ((pointer)&rtc->ALRM_DAY, FALSE, MCF52XX_RTC_ALRM_DAY_DAYS(0));
        _rtc_register_write ((pointer)&rtc->ALRM_SEC, TRUE, MCF52XX_RTC_ALRM_SEC_WRITE(0));
        _rtc_register_write ((pointer)&rtc->ALRM_HM, TRUE, MCF52XX_RTC_ALRM_HM_HOURS_WRITE(0) | MCF52XX_RTC_ALRM_HM_MINUTES_WRITE(0));
        _rtc_register_write ((pointer)&rtc->STPWCH, TRUE, MCF52XX_RTC_STPWCH_CNT(0));
    }
    if (flags & RTC_INIT_FLAG_RESET)
    {
        _rtc_register_write ((pointer)&rtc->RTCCTL, TRUE, MCF52XX_RTC_RTCCTL_SWR);
        _rtc_register_write ((pointer)&rtc->RTCIENR, TRUE, 0);
        _rtc_register_write ((pointer)&rtc->RTCISR, TRUE, 0xFFFFFFFF);
    }
    if (flags & RTC_INIT_FLAG_ENABLE)
    {
        _rtc_register_write ((pointer)&rtc->RTCCTL, TRUE, MCF52XX_RTC_RTCCTL_EN);
    }
    return MQX_OK;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_isr
* Returned Value   : none
* Comments         :
*    This is template ISR for RTC module.
*
*END*********************************************************************/
void _rtc_isr 
(
    /* [IN] data pointer passed to interrupt */
    pointer ptr
)
{
    uint_32 tmp;
    VMCF52XX_RTC_STRUCT_PTR rtc = (VMCF52XX_RTC_STRUCT_PTR)ptr;
    tmp = _rtc_register_read ((pointer)&rtc->RTCISR, TRUE) & _rtc_register_read ((pointer)&rtc->RTCIENR, TRUE);
    if (tmp & MCF52XX_RTC_RTCISR_ALM)
    {
        // alarm interrupt
    }
    if (tmp & MCF52XX_RTC_RTCISR_SW)
    {
        // stopwatch interrupt
    }
    if (tmp & (MCF52XX_RTC_RTCISR_1HZ | MCF52XX_RTC_RTCISR_MIN | MCF52XX_RTC_RTCISR_HR | MCF52XX_RTC_RTCISR_DAY))
    {
        // time change interrupt (sec, min, hour, day)
    }
    _rtc_register_write ((pointer)&rtc->RTCISR, TRUE, tmp);
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_int_install
* Returned Value   : MQX error code
* Comments         :
*    This function installs given ISR for RTC module.
*
*END*********************************************************************/
uint_32 _rtc_int_install 
(
    /* [IN] pointer to user ISR code */
    INT_ISR_FPTR isr
)
{
    INT_ISR_FPTR result;
    VMCF52XX_RTC_STRUCT_PTR rtc;
    if (NULL == isr) return MQX_INVALID_POINTER;
    rtc = _bsp_get_rtc_base_address ();
    result = _int_install_isr (_bsp_get_rtc_vector (), isr, (pointer)rtc);
    if ((INT_ISR_FPTR)NULL == result) return _task_get_error ();
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
    uint_32 result = MQX_OK;
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    if (enable)
    {
        bitmask = _rtc_register_read ((pointer)&rtc->RTCIENR, TRUE) | bitmask;
        _rtc_register_write ((pointer)&rtc->RTCIENR, TRUE, bitmask);
        if (0 != bitmask) result = _mcf52xx_int_init (_bsp_get_rtc_vector (), BSP_RTC_INT_LEVEL, BSP_RTC_INT_SUBLEVEL, TRUE);
    }
    else
    {
        bitmask = _rtc_register_read ((pointer)&rtc->RTCIENR, TRUE) & (~ bitmask);
        _rtc_register_write ((pointer)&rtc->RTCIENR, TRUE, bitmask);
        if (0 == bitmask) result = _mcf52xx_int_init (_bsp_get_rtc_vector (), BSP_RTC_INT_LEVEL, BSP_RTC_INT_SUBLEVEL, FALSE);
    }
    if (MQX_OK != result) bitmask = 0;
    return bitmask;
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
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    _rtc_register_write ((pointer)&rtc->RTCISR, TRUE, bitmask);
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_get_status
* Returned Value   : bitmask of actual RTC interrupt requests + RTC enabled bit
* Comments         :
*    This function returns bitmask of pending RTC interrupt requests along
*    with RTC enabled bit (bit MCF52XX_RTC_RTCISR_EN).
*
*END*********************************************************************/
uint_32 _rtc_get_status 
(
    void
)
{
    uint_32 result;
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    result = _rtc_register_read ((pointer)&rtc->RTCISR, TRUE);
    if (_rtc_register_read ((pointer)&rtc->RTCCTL, TRUE) & MCF52XX_RTC_RTCCTL_EN)
    {
        result |= (uint_32)MCF52XX_RTC_RTCISR_EN;
    }
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
    uint_32 tmp;
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    tmp = _rtc_register_write ((pointer)&rtc->RTCCTL, TRUE, 0);
    _rtc_register_write ((pointer)&rtc->DAYS, FALSE, MCF52XX_RTC_DAYS_DAYS(time->days));
    _rtc_register_write ((pointer)&rtc->SECONDS, TRUE, MCF52XX_RTC_SECONDS_WRITE(time->seconds));
    _rtc_register_write ((pointer)&rtc->HOURMIN, TRUE, MCF52XX_RTC_HOURMIN_HOURS_WRITE(time->hours) | MCF52XX_RTC_HOURMIN_MINUTES_WRITE(time->minutes));
    if (tmp & MCF52XX_RTC_RTCCTL_EN) 
    {
        _rtc_register_write ((pointer)&rtc->RTCCTL, TRUE, MCF52XX_RTC_RTCCTL_EN);
    }
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
    vuint_32 hm;
    RTC_TIME_STRUCT tmp;
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    time->hours = 0xFF;
    do 
    {
        tmp.days = time->days;
        tmp.seconds = time->seconds;
        tmp.hours = time->hours;
        tmp.minutes = time->minutes;
        time->days = MCF52XX_RTC_DAYS_DAYS(_rtc_register_read ((pointer)&rtc->DAYS, FALSE));
        time->seconds = MCF52XX_RTC_SECONDS_READ(_rtc_register_read ((pointer)&rtc->SECONDS, TRUE));
        hm = _rtc_register_read ((pointer)&rtc->HOURMIN, TRUE);
        time->hours = MCF52XX_RTC_HOURMIN_HOURS_READ(hm);
        time->minutes = MCF52XX_RTC_HOURMIN_MINUTES_READ(hm);
    } while ((tmp.seconds != time->seconds) || (tmp.minutes != time->minutes) || (tmp.hours != time->hours) || (tmp.days != time->days));
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_set_alarm
* Returned Value   : none
* Comments         :
*    This function sets RTC alarm time according to given time struct.
*    Alarm happens a half second after match.
*
*END*********************************************************************/
void _rtc_set_alarm 
(
    /* [IN] time to be set as RTC alarm time */
    RTC_TIME_STRUCT_PTR time
)
{
    uint_32 tmp;
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    tmp = _rtc_register_read ((pointer)&rtc->RTCIENR, TRUE);
    _rtc_register_write ((pointer)&rtc->RTCIENR, TRUE, tmp & (~ MCF52XX_RTC_RTCIENR_ALM));
    _rtc_register_write ((pointer)&rtc->ALRM_DAY, FALSE, MCF52XX_RTC_ALRM_DAY_DAYS(time->days));
    _rtc_register_write ((pointer)&rtc->ALRM_SEC, TRUE, MCF52XX_RTC_ALRM_SEC_WRITE(time->seconds));
    _rtc_register_write ((pointer)&rtc->ALRM_HM, TRUE, MCF52XX_RTC_ALRM_HM_MINUTES_WRITE(time->minutes) | MCF52XX_RTC_ALRM_HM_HOURS_WRITE(time->hours));
    _rtc_clear_requests (MCF52XX_RTC_RTCISR_ALM);
    _rtc_register_write ((pointer)&rtc->RTCIENR, TRUE, tmp);
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
    vuint_32 hm;
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    time->days = MCF52XX_RTC_ALRM_DAY_DAYS(_rtc_register_read ((pointer)&rtc->ALRM_DAY, FALSE));
    time->seconds = MCF52XX_RTC_ALRM_SEC_READ(_rtc_register_read ((pointer)&rtc->ALRM_SEC, TRUE));
    hm = _rtc_register_read ((pointer)&rtc->ALRM_HM, TRUE);
    time->minutes = MCF52XX_RTC_ALRM_HM_MINUTES_READ(hm);
    time->hours = MCF52XX_RTC_ALRM_HM_HOURS_READ(hm);
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
    uint_32 tmp;
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    tmp = _rtc_register_read ((pointer)&rtc->RTCIENR, TRUE);
    _rtc_register_write ((pointer)&rtc->RTCIENR, TRUE, tmp & (~ MCF52XX_RTC_RTCIENR_SW));
    _rtc_register_write ((pointer)&rtc->STPWCH, TRUE, MCF52XX_RTC_STPWCH_CNT(minutes));
    _rtc_clear_requests (MCF52XX_RTC_RTCISR_SW);
    _rtc_register_write ((pointer)&rtc->RTCIENR, TRUE, tmp);
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
    VMCF52XX_RTC_STRUCT_PTR rtc = _bsp_get_rtc_base_address ();
    return MCF52XX_RTC_STPWCH_CNT(_rtc_register_read ((pointer)&rtc->STPWCH, TRUE));
}
/* EOF */

