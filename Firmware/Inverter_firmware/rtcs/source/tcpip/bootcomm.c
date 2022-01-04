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
* $FileName: bootcomm.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the
*   functions common to BOOTP and DHCP clients.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4

#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


extern uint_32          BOOT_count;
extern UCB_STRUCT_PTR   BOOT_port;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Opens the BOOTP/DHCP client port.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_open
   (
      void (_CODE_PTR_ service)(RTCSPCB_PTR, UCB_STRUCT_PTR)
   )
{ /* Body */
   uint_32 error;

   if (BOOT_count == 0) {
      error = UDP_openbind_internal(IPPORT_BOOTPC, service, &BOOT_port);
      if (error) {
         return error;
      } /* Endif */
   } /* Endif */

   BOOT_count++;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_close
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Closes the BOOTP/DHCP client port.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_close
   (
      void
   )
{ /* Body */
   uint_32 error = RTCS_OK;

   BOOT_count--;

   if (BOOT_count == 0) {
      error = UDP_close_internal(BOOT_port);
      BOOT_port = NULL;
   } /* Endif */

   return error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_send
*  Returned Value : void
*  Comments       :
*        Sends a BOOTP/DHCP packet.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_send
   (
      RTCSPCB_PTR    rtcs_pcb,         /* [IN]     outgoing packet  */
      pointer        if_ptr            /* [IN]     target interface */
   )
{ /* Body */

   return UDP_send_IF(BOOT_port, if_ptr, IPPORT_BOOTPS, rtcs_pcb);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_service
*  Returned Value : void
*  Comments       :
*        Services a BOOTP/DHCP packet.
*
*END*-----------------------------------------------------------------*/

void BOOT_service
   (
      RTCSPCB_PTR    rtcs_pcb,         /* [IN/OUT] incoming packet  */
      UCB_STRUCT_PTR ucb_ptr           /* [IN]     target UCB       */
   )
{ /* Body */
   IP_IF_PTR if_ptr = (IP_IF_PTR)rtcs_pcb->IFSRC;

   if (if_ptr->BOOTFN) {
      (*if_ptr->BOOTFN)(rtcs_pcb);
   } else {
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
      RTCSPCB_free(rtcs_pcb);
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
