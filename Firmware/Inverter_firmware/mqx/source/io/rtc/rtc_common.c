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
* $FileName: rtc_common.c$
* $Version : 3.8.3.0$
* $Date    : Sep-14-2011$
*
* Comments:
*           Common RTC functions for some platforms. Includes leap year count.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <rtc.h>

#define ZERO_YEAR   1970
#define SEC_IN_MIN  60
#define SEC_IN_HOUR 3600 
#define SEC_IN_DAY  86400

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
* Function Name    : _rtc_date_to_days
* Returned Value   : none
* Comments         :
*    Returns sum of days in year, month and day in DATE_STRUCT
*
*END*********************************************************************/
static uint_16 _rtc_date_to_days
( 
   DATE_STRUCT_PTR time
)
{
   uint_16 day_sum;
   

   #define FEBRUARY    2

   static const uint_16 days_no_table[]= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
   uint_32 days = 0;
   uint_32 month;
   uint_32 year;

   day_sum = 365 * (time->YEAR - ZERO_YEAR);
   /* get leap year */
   for( year = ZERO_YEAR; year < time->YEAR; year++)
   {
     day_sum += _is_leap_year( year );      
   }

   for( month = 1; month < time->MONTH; month++)
   {
     day_sum+= days_no_table[month - 1];
     /* is leap year? */
     if( month == FEBRUARY )
     {
         day_sum+= _is_leap_year( time->YEAR );  
     }
   }

   day_sum += time->DAY-1;
   return day_sum;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rtc_set_time_mqxd
* Returned Value   : actual RTC minute stopwatch counter value
* Comments         :
*    This function sets the RTC time according to DATE_STRUCT.
*
*END*********************************************************************/
_mqx_int _rtc_set_time_mqxd
(
   DATE_STRUCT_PTR time
)
{
   RTC_TIME_STRUCT rtc_time; 
   
   rtc_time.days = _rtc_date_to_days(time);
   rtc_time.hours = time->HOUR;
   rtc_time.minutes = time->MINUTE;
   rtc_time.seconds = time->SECOND; 
   
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
void _rtc_get_time_mqxd 
(
   DATE_STRUCT_PTR time
)
{
    RTC_TIME_STRUCT     rtc_time; 
    TIME_STRUCT         time_s;
    
    _rtc_get_time( &rtc_time);
    time_s.SECONDS = rtc_time.seconds + rtc_time.minutes*SEC_IN_MIN + \
      rtc_time.hours*SEC_IN_HOUR + rtc_time.days*SEC_IN_DAY;  
    time_s.MILLISECONDS = 0;  
    
    _time_to_date( &time_s, time );    
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
_mqx_int _rtc_set_alarm_mqxd
(
   DATE_STRUCT_PTR time
)
{
   RTC_TIME_STRUCT rtc_alarm;
   
   rtc_alarm.days = _rtc_date_to_days(time);
   rtc_alarm.hours = time->HOUR;
   rtc_alarm.minutes = time->MINUTE;
   rtc_alarm.seconds = time->SECOND; 
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
void _rtc_get_alarm_mqxd 
(
   DATE_STRUCT_PTR time
)
{
    RTC_TIME_STRUCT     rtc_time; 
    TIME_STRUCT         time_s;
    
    _rtc_get_alarm( &rtc_time);
    time_s.SECONDS = rtc_time.seconds + rtc_time.minutes*SEC_IN_MIN + \
      rtc_time.hours*SEC_IN_HOUR + rtc_time.days*SEC_IN_DAY;  
    time_s.MILLISECONDS = 0;  
    
    _time_to_date( &time_s, time );
}
/* EOF */
