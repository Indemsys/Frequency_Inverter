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
* $FileName: rtcslog1.c$
* $Version : 3.7.9.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the RTCS logging control functions.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_LOGGING
#if RTCSCFG_LOG_SOCKET_API || RTCSCFG_LOG_PCB || RTCSCFG_LOGGING
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_enable
* Returned Value  : void
* Comments  :  Enables logging of certain RTCS events
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_enable
   (
      uint_32     logtype
         /* [IN] the type of log entry to enable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();

   if (rtcs) {
      rtcs->RTCS_LOG_CONTROL |= logtype;
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_IFTYPE (IPIFTYPE_ETHERNET) );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_IFTYPE (IPIFTYPE_PPP)      );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_IFTYPE (IPIFTYPE_LOOPBACK) );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_ARP    (IPIFTYPE_ETHERNET) );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_ICMP)      );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_IGMP)      );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_IP)        );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_TCP)       );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_UDP)       );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PROTO  (IPPROTO_OSPF)      );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PORT   (IPPORT_BOOTPC)     );
      RTCSLOG_enable_prot(logtype, RTCS_LOGCTRL_PORT   (IPPORT_RIP)        );
   }
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_disable
* Returned Value  : void
* Comments  :  Disables logging of certain RTCS events
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_disable
   (
      uint_32     logtype
         /* [IN] the type of log entry to disable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i;

   if (rtcs) {
      rtcs->RTCS_LOG_CONTROL &= ~logtype;

      if (logtype & RTCS_LOGCTRL_PCB) {
         for (i=0; i<RTCSLOG_PROT_MAX; i++) {
            rtcs->RTCS_LOG_PCB[i] = 0;
         } /* Endfor */
      } /* Endif */

      if (logtype & RTCS_LOGCTRL_TIMER) {
         for (i=0; i<RTCSLOG_PROT_MAX; i++) {
            rtcs->RTCS_LOG_TIMER[i] = 0;
         } /* Endfor */
      } /* Endif */
   }
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_enable_prot
* Returned Value  : void
* Comments  :  Enables logging of certain RTCS events for a specific
*              protocol
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_enable_prot
   (
      uint_32     logtype,
         /* [IN] the type of log entry to enable */
      uint_32     protocol
         /* [IN] the protocol to enable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i, j;

   rtcs->RTCS_LOG_CONTROL |= logtype;

   if (logtype & RTCS_LOGCTRL_PCB) {
      j = RTCSLOG_PROT_MAX;
      for (i = RTCSLOG_PROT_MAX; i > 0; ) {
         i--;
         if (rtcs->RTCS_LOG_PCB[i] == protocol) {
            j = i;
            break;
         } else if (rtcs->RTCS_LOG_PCB[i] == 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_PCB[j] = protocol;
      } /* Endif */
   } /* Endif */

   if (logtype & RTCS_LOGCTRL_TIMER) {
      j = RTCSLOG_PROT_MAX;
      for (i = RTCSLOG_PROT_MAX; i > 0; ) {
         i--;
         if (rtcs->RTCS_LOG_TIMER[i] == protocol) {
            j = i;
            break;
         } else if (rtcs->RTCS_LOG_TIMER[i] == 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_TIMER[j] = protocol;
      } /* Endif */
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSLOG_disable_prot
* Returned Value  : void
* Comments  :  Disables logging of certain RTCS events for a specific
*              protocol
*
*END*-----------------------------------------------------------------*/

void RTCSLOG_disable_prot
   (
      uint_32     logtype,
         /* [IN] the type of log entry to disable */
      uint_32     protocol
         /* [IN] the protocol to disable */
   )
{ /* Body */
   RTCS_DATA_PTR rtcs = RTCS_get_data();
   uint_32       i, j;

   rtcs->RTCS_LOG_CONTROL &= ~logtype | (RTCS_LOGCTRL_PCB | RTCS_LOGCTRL_TIMER);

   if (logtype & RTCS_LOGCTRL_PCB) {
      j = RTCSLOG_PROT_MAX;
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_PCB[i] == protocol) {
            rtcs->RTCS_LOG_PCB[i] = 0;
         } else if (rtcs->RTCS_LOG_PCB[i] != 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_CONTROL &= ~RTCS_LOGCTRL_PCB;
      } /* Endif */
   } /* Endif */

   if (logtype & RTCS_LOGCTRL_TIMER) {
      j = RTCSLOG_PROT_MAX;
      for (i = 0; i < RTCSLOG_PROT_MAX; i++) {
         if (rtcs->RTCS_LOG_TIMER[i] == protocol) {
            rtcs->RTCS_LOG_TIMER[i] = 0;
         } else if (rtcs->RTCS_LOG_TIMER[i] != 0) {
            j = i;
         } /* Endif */
      } /* Endfor */
      if (j < RTCSLOG_PROT_MAX) {
         rtcs->RTCS_LOG_CONTROL &= ~RTCS_LOGCTRL_TIMER;
      } /* Endif */
   } /* Endif */

} /* Endbody */

#endif
#endif

/* EOF */
