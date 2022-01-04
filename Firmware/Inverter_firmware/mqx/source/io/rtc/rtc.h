/**HEADER***********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
********************************************************************************
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
********************************************************************************
*
* $FileName: rtc.h$
* $Version : 3.8.28.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This include file is used to provide constant and structure definitions
*   common for RTC module
*
*END***************************************************************************/

#ifndef __rtc_h__
#define __rtc_h__ 1


#define RTC_RTCISR_SW       DEVSPEC_RTC_RTCISR_SW
#define RTC_RTCISR_ALM      DEVSPEC_RTC_RTCISR_ALM
#define RTC_RTCIENR_SW      DEVSPEC_RTC_RTCIENR_SW
#define RTC_RTCIENR_ALM     DEVSPEC_RTC_RTCIENR_ALM
#ifndef RTC_INT_ALL_MASK
#define RTC_INT_ALL_MASK    DEVSPEC_RTC_INT_ALL_MASK
#endif

#ifndef RTC_INVALID_TIME
#define  RTC_INVALID_TIME  (RTC_ERROR_BASE | 0x01)
#endif

#define RTC_INIT_FLAG_CLEAR     (1 << 0)
#define RTC_INIT_FLAG_RESET     (1 << 1)
#define RTC_INIT_FLAG_ENABLE    (1 << 2)

#ifdef __cplusplus
extern "C" {
#endif

pointer                      _bsp_get_rtc_base_address (void);
PSP_INTERRUPT_TABLE_INDEX    _bsp_get_rtc_vector (void);

_mqx_int _rtc_set_time_mqxd (DATE_STRUCT_PTR time);
void     _rtc_get_time_mqxd (DATE_STRUCT_PTR time);
_mqx_int _rtc_set_alarm_mqxd (DATE_STRUCT_PTR time);
void     _rtc_get_alarm_mqxd (DATE_STRUCT_PTR time);

uint_32  _rtc_init (uint_32 flags);
void     _rtc_isr (pointer ptr);
uint_32  _rtc_int_install (INT_ISR_FPTR isr);
uint_32  _rtc_int_enable (boolean enable, uint_32 bitmask);
void     _rtc_clear_requests (uint_32 bitmask);
uint_32  _rtc_get_status (void);
void     _rtc_set_time (RTC_TIME_STRUCT_PTR time);
void     _rtc_get_time (RTC_TIME_STRUCT_PTR time);
void     _rtc_set_alarm (RTC_TIME_STRUCT_PTR time);
void     _rtc_get_alarm (RTC_TIME_STRUCT_PTR time);
void     _rtc_set_stopwatch (uint_32 minutes);
uint_32  _rtc_get_stopwatch (void);

void     _rtc_time_to_mqx_time (RTC_TIME_STRUCT_PTR rtc_time, TIME_STRUCT_PTR mqx_time);
void     _rtc_time_from_mqx_time (TIME_STRUCT_PTR mqx_time, RTC_TIME_STRUCT_PTR rtc_time);
void     _rtc_time_to_mqx_date (RTC_TIME_STRUCT_PTR rtc_time, DATE_STRUCT_PTR mqx_date);
void     _rtc_time_from_mqx_date (DATE_STRUCT_PTR mqx_date, RTC_TIME_STRUCT_PTR rtc_time);
_mqx_int _rtc_sync_with_mqx (boolean update_mqx);

#ifdef __cplusplus
}
#endif

#endif /* __rtc_h__ */
