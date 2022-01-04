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
* $FileName: qrtc.h$
* $Version : 3.8.2.0$
* $Date    : Sep-26-2012$
*
* Comments:
*
*   This include file is used to provide constant and structure definitions
*   common for RTC module 
*
*END************************************************************************/
#ifndef __qrtc_h__
#define __qrtc_h__

#ifdef __cplusplus
extern "C" {
#endif

#define _RTC                    (1)
#define _API                    (2)
#define BSP_RTC_INT_LEVEL       (4)
#define BSP_API_INT_LEVEL       (4)

#define RTC_VALUE               (125)
#define API_VALUE               (1000)
#ifndef PSP_MPXD10
#define RTC_INIT_FLAG_CLEAR     (1 << 0)
#define RTC_INIT_FLAG_RESET     (1 << 1)
#define RTC_INIT_FLAG_ENABLE    (1 << 2)
#endif
#define RTC_INIT_RTC_ENABLE     (1 << 3)
#define RTC_INIT_API_ENABLE     (1 << 4)  
/* Flags used to signal user callback requests */
#define RTC_EVENT_SW                       (1<<0)
#define RTC_EVENT_MIN                      (1<<1)
#define RTC_EVENT_ALM                      (1<<2)
#define RTC_EVENT_DAY                      (1<<3)
#define RTC_EVENT_1HZ                      (1<<4)
#define RTC_EVENT_HR                       (1<<5)

#define RTC_STPWCH_CNT(x)                 ((x)&0x1F)

/* standardized names used outside of driver */
/* currently, only alarms and stopwatches are signaled */
#define DEVSPEC_RTC_RTCISR_SW   RTC_EVENT_SW
#define DEVSPEC_RTC_RTCISR_ALM  RTC_EVENT_ALM
#define DEVSPEC_RTC_RTCIENR_SW  RTC_EVENT_SW
#define DEVSPEC_RTC_RTCIENR_ALM RTC_EVENT_ALM
/*------------------------------------------------------------------------*/
/*
** RTC registers
*/

//#include "typedefs.h"

// RTC register masks
#define QRTC_RTCSUPV_SUPV                    (1<<31)

#define QRTC_RTCC_CNTEN                      (1<<31)
#define QRTC_RTCC_RTCIE                      (1<<30)
#define QRTC_RTCC_FRZEN                      (1<<29)
#define QRTC_RTCC_ROVREN                     (1<<28)
#define QRTC_RTCC_RTCVAL(x)                  (((x)&0x0FFF)<<16)
#define QRTC_RTCC_APIEN                      (1<<15)
#define QRTC_RTCC_APIIE                      (1<<14)
#define QRTC_RTCC_CLKSEL(x)                  (((x)&0x03)<<12)
#define QRTC_RTCC_DIV512EN                   (1<<11)
#define QRTC_RTCC_DIV32EN                    (1<<10)
#define QRTC_RTCC_APIVAL(x)                  (((x)&0x03FF))
#define QRTC_RTCS_RTCF                       (1<<29)
#define QRTC_RTCS_ROVRF                      (1<<10)
#define QRTC_RTCS_APIF                       (1<<13)

// RTC status bits

#define QRTC_RTCISR_SW                       (1<<0)
#define QRTC_RTCISR_MIN                      (1<<1)
#define QRTC_RTCISR_ALM                      (1<<2)
#define QRTC_RTCISR_DAY                      (1<<3)
#define QRTC_RTCISR_1HZ                      (1<<4)
#define QRTC_RTCISR_HR                       (1<<5)
#define QRTC_RTCISR_EN                       (1<<31)
#define QRTC_RTCIENR_SW                      (1<<0)
#define QRTC_RTCIENR_MIN                     (1<<1)
#define QRTC_RTCIENR_ALM                     (1<<2)
#define QRTC_RTCIENR_DAY                     (1<<3)
#define QRTC_RTCIENR_1HZ                     (1<<4)
#define QRTC_RTCIENR_HR                      (1<<5)
#define QRTC_STPWCH_CNT(x)                   ((x)&0x1F)

/* RTC register structure */
typedef struct qrtc_regs_struct
{
  uint_32 RTCSUPV;                     /* RTC Supervisor Control Register */
  uint_32 RTCC;                        /* RTC Control Register */
  uint_32 RTCS;                        /* RTC Status Register */
  uint_32 RTCCNT;                      /* RTC Counter Register */

} QRTC_REGS_STRUCT, _PTR_ QRTC_REG_STRUCT_PTR;
typedef volatile struct qrtc_regs_struct _PTR_ VQRTC_REG_STRUCT_PTR;
typedef struct rtc_time_struct
{
    uint_8      seconds;
    uint_8      minutes;
    uint_8      hours;
    uint_16     days;
} RTC_TIME_STRUCT, _PTR_ RTC_TIME_STRUCT_PTR;

PSP_INTERRUPT_TABLE_INDEX _bsp_get_api_vector (void);

#ifdef __cplusplus
}
#endif

#endif  //__qi2c_h__
