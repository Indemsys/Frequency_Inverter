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
* $FileName: rtcsdela.c$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the interface to the RTCS
*   time delay services.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "rtcstime.h"


typedef struct rtcstime_delay_parm {
   TCPIP_PARM     COMMON;
   TCPIP_EVENT    EXPIRE;
   uint_32        milliseconds;
} RTCSTIME_DELAY_PARM, _PTR_ RTCSTIME_DELAY_PARM_PTR;


static void RTCSTIME_delay(RTCSTIME_DELAY_PARM_PTR);
static boolean RTCSTIME_delay_expire(TCPIP_EVENT_PTR);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_delay
* Returned Value  : void
* Comments        :
*
*  Blocks the calling task until the time (specified in milliseconds)
*  expires.
*
*END*-----------------------------------------------------------------*/

void RTCS_delay
   (
      /*[IN] time length in milliseconds to block task */
      uint_32 milliseconds
   )
{ /* Body */
   RTCSTIME_DELAY_PARM   parms;

   if (milliseconds) {
      parms.milliseconds = milliseconds;
      RTCSCMD_issue(parms, RTCSTIME_delay);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSTIME_delay
* Returned Value  : void
* Comments        :
*
*  Adds an event to the TCPIP timer queue.
*
*END*-----------------------------------------------------------------*/

static void RTCSTIME_delay
   (
      RTCSTIME_DELAY_PARM_PTR     parms
   )
{ /* Body */

   parms->EXPIRE.TIME    = parms->milliseconds;
   parms->EXPIRE.EVENT   = RTCSTIME_delay_expire;
   parms->EXPIRE.PRIVATE = parms;
   TCPIP_Event_add(&parms->EXPIRE);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCSTIME_delay_expire
* Returned Value  : FALSE
* Comments        :
*
*  Unblocks the task that called RTCS_time_delay
*
*END*-----------------------------------------------------------------*/

static boolean RTCSTIME_delay_expire
   (
      TCPIP_EVENT_PTR   event
   )
{ /* Body */
   RTCSTIME_DELAY_PARM_PTR  parms = event->PRIVATE;

   RTCSCMD_complete(parms, RTCS_OK);
   return FALSE;

} /* Endbody */


/* EOF */
