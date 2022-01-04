
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: rtc_mcf52xx.h$
* $Version : 3.8.4.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the type definitions for the mcf52xx RTC module.
*
*END************************************************************************/

#ifndef __rtc_mcf52xx_h__
#define __rtc_mcf52xx_h__

#ifdef __cplusplus
extern "C" {
#endif


#define DEVSPEC_RTC_RTCISR_SW   MCF52XX_RTC_RTCISR_SW
#define DEVSPEC_RTC_RTCISR_ALM  MCF52XX_RTC_RTCISR_ALM
#define DEVSPEC_RTC_RTCIENR_SW  MCF52XX_RTC_RTCISR_SW
#define DEVSPEC_RTC_RTCIENR_ALM MCF52XX_RTC_RTCISR_ALM

typedef struct rtc_time_struct
{
    uint_8      seconds;
    uint_8      minutes;
    uint_8      hours;
    uint_16     days;
} RTC_TIME_STRUCT, _PTR_ RTC_TIME_STRUCT_PTR;

#ifdef __cplusplus
}
#endif

#endif
