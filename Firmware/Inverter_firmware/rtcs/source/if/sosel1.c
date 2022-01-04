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
* $FileName: sosel1.c$
* $Version : 3.5.7.0$
* $Date    : Jan-27-2010$
*
* Comments:
*
*   This file contains the RTCS select() support functions.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"
/* Start CR 1707 */
#include "tcp_prv.h"    /* for TCP internal definitions */
#include "udp_prv.h"    /* for UDP internal definitions */
/* End CR 1707 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_unblock
* Returned Values :
* Comments  :
*     Dequeues a select() call from the select queue.
*
*END*-----------------------------------------------------------------*/

void SOCK_select_unblock
   (
      SOCKSELECT_PARM_PTR  parms,
      uint_16  state      // TCP state or UDP socket flag
   )
{ /* Body */

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } /* Endif */
   *parms->PREV = parms->NEXT;

   if (parms->timeout) {
      TCPIP_Event_cancel(&parms->EXPIRE);
   } /* Endif */

/* Start CR 1707 */
   //RTCSCMD_complete(parms, RTCS_OK);
   /* Check TCP state and UDP socket flag */
   if ( (state == CLOSED) || (state == UDP_SOCKET_CLOSE) ) {
      RTCSCMD_complete(parms, RTCSERR_SOCK_CLOSED);
   } else {
      RTCSCMD_complete(parms, RTCS_OK);
   } /* Endif */
   
/* End CR 1707 */   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_signal
* Returned Values :
* Comments  :
*     Unblocks all select() calls blocked on a specific socket.
*
*END*-----------------------------------------------------------------*/

void _SOCK_select_signal
   (
      uint_32  sock,
      uint_16  state,      // TCP state or UDP socket flag
      uint_32  flag        // direction flag - 0 recv, non zero send
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   SOCKSELECT_PARM_PTR        parms, nextparms;
   uint_32                    i;

   if (!sock) {
      return;
   } /* Endif */

   for (parms=socket_cfg_ptr->SELECT_HEAD; parms; parms=nextparms) {

      nextparms = parms->NEXT;
      if (parms->owner) {
         if (SOCK_Is_owner((SOCKET_STRUCT_PTR)sock, parms->owner)) {
            parms->sock = sock;
            SOCK_select_unblock(parms, state);
         } /* Endif */
      } else {
         /* check if sock is on the select waiting list (array) */
         if (!flag) {         
            for (i=0; i<parms->sock_count; i++)
               if (parms->sock_ptr[i] == sock)
                  break;
            if(i>=parms->sock_count)
               continue;
         }
         else {
            for (i=0; i<parms->writesock_count; i++)
               if (parms->writesock_ptr[i] == sock)
                  break;
            if(i>=parms->writesock_count)               
               continue;
         }
         
         /* now, it is sure the select will be awaken. 
            Just zero out the others in both arrays */
         parms->sock = sock;

         _mem_zero(parms->writesock_ptr, sizeof(parms->writesock_ptr[0]) * parms->writesock_count);
         _mem_zero(parms->sock_ptr, sizeof(parms->sock_ptr[0]) * parms->sock_count);
         
         if (!flag) {         
            parms->sock_ptr[0] = sock; 
         }
         else {
            parms->writesock_ptr[0] = sock; 
         }
         
         SOCK_select_unblock(parms, state);
      } /* Endif */

   } /* Endfor */

   /*
   ** Finally, call the application callback, if one exists.
   */
   if (((SOCKET_STRUCT_PTR)sock)->APPLICATION_CALLBACK) {
      ((SOCKET_STRUCT_PTR)sock)->APPLICATION_CALLBACK(sock);
   } /* Endif */

} /* Endbody */


/* EOF */
