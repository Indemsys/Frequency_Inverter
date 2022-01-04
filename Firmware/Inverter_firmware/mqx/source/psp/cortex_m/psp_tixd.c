/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor
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
* $FileName: psp_tixd.c$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*   This file contains the functions for converting between ticks
*   and the xdate structure.
*
*END************************************************************************/

#include "mqx_inc.h"

#if MQX_HAS_TICK

/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_ticks_to_xdate
* Returned Value   : boolean - Returns FALSE if ticks are out of range of 
*                    the xdate structure, TRUE otherwise
* Comments         :
*    This function converts ticks into the xdate structure
*
*END*----------------------------------------------------------------------*/

boolean _psp_ticks_to_xdate
   (
      /*  [IN]  pointer to tick structure  */
      PSP_TICK_STRUCT_PTR  tick_ptr,

      /*  [OUT]  pointer to a xdate structure  */
      MQX_XDATE_STRUCT_PTR xdate_ptr
   )
{ /* Body */
   PSP_TICK_STRUCT        tmp_ticks1;
   PSP_TICK_STRUCT        tmp_ticks2;
   uint_64                seconds;
   uint_64                leftover_ticks;
   KERNEL_DATA_STRUCT_PTR kernel_data;
   uint_32                days,years;
   uint_32                tps, tmp;
   boolean                over_flow;
   uint_16                leap;


   _GET_KERNEL_DATA(kernel_data);

   tps = kernel_data->TICKS_PER_SECOND;

   /* Convert ticks to seconds */
   seconds        = tick_ptr->TICKS[0] / tps;
   leftover_ticks = tick_ptr->TICKS[0] % tps;
      
   /* Convert to minutes */
   tmp = seconds / SECS_IN_MINUTE;
   xdate_ptr->SEC = seconds % SECS_IN_MINUTE;

   /* Convert to hours */
   xdate_ptr->MIN = tmp % MINUTES_IN_HOUR;
   tmp /= MINUTES_IN_HOUR;

   /* Convert to days */
   xdate_ptr->HOUR = tmp % HOURS_IN_DAY;
   days = tmp / HOURS_IN_DAY;


   /* Make an educated quess where to start in table */
   years = days / 366;
   if (years > 511) {
      /* This is out of range of the xdate structure */
      return FALSE;
   } /* Endif */

   xdate_ptr->YEAR = (uint_16)years;
   while (days >= _time_days_before_year_internal[xdate_ptr->YEAR + 1]) {
      xdate_ptr->YEAR++;
   } /* Endwhile */

   /* Jan 1, 1970 was a Thursday */
   xdate_ptr->WDAY  = (uint_16)((days + 3 + 1) % 7);

   /* Calculate the number of days since the beginning of the year */
   days -= _time_days_before_year_internal[xdate_ptr->YEAR];

   xdate_ptr->YEAR += (uint_16)1970;
   xdate_ptr->YDAY  = (uint_16)days;

   /*
   ** Find out if we are in a leap year.
   **
   ** if (the year is a century year not divisible by 400
   ** then it is not a leap year, otherwise if year divisible by
   ** four then it is a leap year
   */

   if ((xdate_ptr->YEAR % (uint_16)100) == (uint_16)0) {

      if ((xdate_ptr->YEAR % (uint_16)400) == (uint_16)0) {
         leap = (uint_16)LEAP_YEAR;
      } else {
         leap = (uint_16)NON_LEAP_YEAR;
      } /* Endif */
         
   } else {

      if ((xdate_ptr->YEAR % (uint_16)4) == (uint_16)0) {
         leap = (uint_16)LEAP_YEAR;
      } else {
         leap = (uint_16)NON_LEAP_YEAR;
      } /* Endif */

   } /* Endif */

   /* calculate the month */
   xdate_ptr->MONTH = days/31;
   while (days >= _time_days_before_month_internal[leap][xdate_ptr->MONTH]) {
      xdate_ptr->MONTH++;
   } /* Endwhile */
   days -= _time_days_before_month_internal[leap][xdate_ptr->MONTH - 1];

   /* calculate the day */
   xdate_ptr->MDAY = (uint_16)(days);

   /* first day is 1*/
   xdate_ptr->MDAY++;

   /* 
   ** Calculate ms, us, ns and ps from remaining ticks
   */
   tmp_ticks1.TICKS[0]    = leftover_ticks;
   tmp_ticks1.HW_TICKS[0] = tick_ptr->HW_TICKS[0];

   /* Calculate milliseconds */
   tmp = _psp_ticks_to_milliseconds_truncate(&tmp_ticks1, &over_flow);
   xdate_ptr->MSEC = (uint_16)tmp;

   _psp_msecs_to_ticks(tmp, &tmp_ticks2);
   PSP_SUB_TICKS(&tmp_ticks1, &tmp_ticks2, &tmp_ticks1);

   if ((tmp_ticks1.TICKS[0] == 0) && (tmp_ticks2.HW_TICKS[0] == 0)) {
      xdate_ptr->USEC = xdate_ptr->NSEC = xdate_ptr->PSEC = 0;
      return TRUE;
   } /* Endif */

   /* Calculate microseconds */
   tmp = _psp_ticks_to_microseconds_truncate(&tmp_ticks1, &over_flow);
   xdate_ptr->USEC = (uint_16)tmp;

   _psp_usecs_to_ticks(tmp, &tmp_ticks2);
   PSP_SUB_TICKS(&tmp_ticks1, &tmp_ticks2, &tmp_ticks1);

   if ((tmp_ticks1.TICKS[0] == 0) && (tmp_ticks2.HW_TICKS[0] == 0)) {
      xdate_ptr->NSEC = xdate_ptr->PSEC = 0;
      return TRUE;
   } /* Endif */

   /* Calculate nanoseconds */
   tmp = _psp_ticks_to_nanoseconds_truncate(&tmp_ticks1, &over_flow);
   xdate_ptr->NSEC = (uint_16)tmp;

   _psp_nsecs_to_ticks(tmp, &tmp_ticks2);
   PSP_SUB_TICKS(&tmp_ticks1, &tmp_ticks2, &tmp_ticks1);

   if ((tmp_ticks1.TICKS[0] == 0) && (tmp_ticks2.HW_TICKS[0] == 0)) {
      xdate_ptr->PSEC = 0;
      return TRUE;
   } /* Endif */

   /* Calculate picoseconds */
   tmp = _psp_ticks_to_microseconds_truncate(&tmp_ticks1, &over_flow);
   xdate_ptr->PSEC = (uint_16)tmp;

   return TRUE;

} /* Endbody */

#endif
/* EOF */
