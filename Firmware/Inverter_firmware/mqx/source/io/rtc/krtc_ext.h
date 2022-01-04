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
* $FileName: krtc_ext.h$
* $Version : 3.8.2.0$
* $Date    : Oct-7-2011$
*
* Comments:
*
*   Processor family specific file needed for RTC module.
*
*END************************************************************************/

#ifndef __krtc_ext_h__
    #define __krtc_ext_h__


typedef uint_32 RTC_TAMPER_TYPE;

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */
void            _rtc_inc_upcounter(void);
uint_32         _rtc_get_upcounter(void);

_mqx_int        _rtc_tamper_enable(RTC_TAMPER_TYPE  enable_mask);
_mqx_int        _rtc_tamper_disable(RTC_TAMPER_TYPE disable_mask);
void            _rtc_get_tamper_timestamp( RTC_TIME_STRUCT_PTR time);
RTC_TAMPER_TYPE _rtc_get_tamper_status(void);
    
#ifdef __cplusplus
}
#endif

#endif  /* __krtc_ext_h__ */
