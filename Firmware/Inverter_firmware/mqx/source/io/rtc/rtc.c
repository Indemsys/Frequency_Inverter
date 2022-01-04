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
* $FileName: rtc.c$
* $Version : 3.6.18.0$
* $Date    : Nov-2-2010$
*
* Comments:
*           Kinetis RTC driver header
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>                    

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
