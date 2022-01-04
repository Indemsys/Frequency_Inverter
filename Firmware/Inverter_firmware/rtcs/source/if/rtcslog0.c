/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: rtcslog0.c$
* $Version : 3.7.9.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the RTCS logging functions.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_LOGGING
#if RTCSCFG_LOG_SOCKET_API || RTCSCFG_LOG_PCB || RTCSCFG_LOGGING
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_log
* Returned Value  : void
* Comments  :  Logs an RTCS event
*
*END*-----------------------------------------------------------------*/

void RTCS_log
   (
      uint_32     logtype,    /* [IN] specifies the type of log entry */
         
      uint_32     p1,         /* [IN] first parameter */
      uint_32     p2,         /* [IN] second parameter */
      uint_32     p3,         /* [IN] third parameter */
      uint_32     p4,         /* [IN] fourth parameter */
      uint_32     p5          /* [IN] fifth parameter */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i;

   switch (logtype) {
   case RTCSLOG_TYPE_FNENTRY:
   case RTCSLOG_TYPE_FNEXIT:
      if (!(rtcs->RTCS_LOG_CONTROL & RTCS_LOGCTRL_FUNCTION)) {
         return;
      } /* Endif */
      break;

   case RTCSLOG_TYPE_PCB:
      if (!(rtcs->RTCS_LOG_CONTROL & RTCS_LOGCTRL_PCB)) {
         return;
      } /* Endif */
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_PCB[i] == (p1 & 0xFFFF)) {
            break;
         } /* Endif */
      } /* Endfor */
      if (i == RTCSLOG_PROT_MAX) {
         return;
      } /* Endif */
      break;

   case RTCSLOG_TYPE_TIMER:
      if (!(rtcs->RTCS_LOG_CONTROL & RTCS_LOGCTRL_TIMER)) {
         return;
      } /* Endif */
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_TIMER[i] == (p1 & 0xFFFF)) {
            break;
         } /* Endif */
      } /* Endfor */
      if (i == RTCSLOG_PROT_MAX) {
         return;
      } /* Endif */
      break;

   default:
      return;
   } /* Endswitch */

   RTCS_log_internal(logtype,p1,p2,p3,p4,p5);

} /* Endbody */
#endif
#endif
/* EOF */
