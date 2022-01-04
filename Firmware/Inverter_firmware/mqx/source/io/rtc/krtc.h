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
* $FileName: krtc.h$
* $Version : 3.8.7.0$
* $Date    : Oct-7-2011$
*
* Comments:
*
*   Processor family specific file needed for RTC module.
*
*END************************************************************************/

#ifndef __krtc_h__
#define __krtc_h__

/******************************************************************************
 * interrupt masks definitions (RTC_ISR and RTC_IER registers)                *
 ******************************************************************************/

#define RTC_INT_ALM_MASK    4
#define RTC_INT_TOF_MASK    2
#define RCT_INT_TIF_MASK    1

#define RTC_ISR_ALARM       (1 <<  0)
#define RTC_ISR_STOPWATCH   (1 <<  1)
#define RTC_ISR_TOF         (1 <<  2)
#define RTC_ISR_TIF         (1 <<  3)

#define DEVSPEC_RTC_RTCISR_SW   RTC_ISR_STOPWATCH
#define DEVSPEC_RTC_RTCISR_ALM  RTC_ISR_ALARM
#define DEVSPEC_RTC_RTCIENR_SW  RTC_ISR_STOPWATCH
#define DEVSPEC_RTC_RTCIENR_ALM RTC_ISR_ALARM
#define DEVSPEC_RTC_INT_ALL_MASK RTC_ISR_ALARM | RTC_ISR_STOPWATCH | RTC_ISR_TOF | RTC_ISR_TIF
#define STOPWATCH_OFF 0x3F

/* RTC time reperesentation */
typedef struct rtc_time_struct
{
    uint_32  seconds;
} RTC_TIME_STRUCT, _PTR_ RTC_TIME_STRUCT_PTR;

#endif   /* __krtc_h__ */
