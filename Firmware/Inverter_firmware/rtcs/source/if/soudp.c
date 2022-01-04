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
* $FileName: soudp.c$
* $Version : 3.0.4.0$
* $Date    : Mar-5-2009$
*
* Comments:
*
*   This file contains the implementation of getsockopt()
*   and setsockopt() at the SOL_UDP level.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "udp.h"

#if RTCSCFG_ENABLE_UDP

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_UDP_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_UDP_ ## f, a); \
                        return a


uint_32 SOL_UDP_getsockopt  (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_UDP_setsockopt  (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_UDP_CALL = {
   SOL_UDP_getsockopt,
   SOL_UDP_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_UDP_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_UDP_getsockopt
   (
      uint_32        sock,
         /* [IN] socket handle */
      uint_32        level,
         /* [IN] protocol level for the option */
      uint_32        optname,
         /* [IN] name of the option */
      pointer        optval,
         /* [IN] buffer for the current value of the option */
      uint_32  _PTR_ optlen
         /* [IN/OUT] length of the option value, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   parms.ucb      = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
   parms.udpflags = optname;
   error = RTCSCMD_issue(parms, UDP_getopt);
   if (error) {
      RTCS_EXIT(GETSOCKOPT, error);
   } /* Endif */

   *(uint_32_ptr)optval = parms.udpword;
   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_UDP_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_UDP_setsockopt
   (
      uint_32        sock,
         /* [IN] socket handle */
      uint_32        level,
         /* [IN] protocol level for the option */
      uint_32        optname,
         /* [IN] name of the option */
      pointer        optval,
         /* [IN] new value for the option */
      uint_32        optlen
         /* [IN] length of the option value, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   parms.ucb      = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
   parms.udpflags = optname;
   parms.udpword  = *(uint_32_ptr)optval;
   error = RTCSCMD_issue(parms, UDP_setopt);
   if (error) {
      RTCS_EXIT(SETSOCKOPT, error);
   } /* Endif */

   RTCS_EXIT(SETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_getopt
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*     _ip_address       ipaddress   not used
*     uint_16           udpport     not used
*     uint_16           udpflags    [IN] option
*     uint_32           udpptr      not used
*     uint_32           udpword     [OUT] option value
*
* Comments        :
*     Retrieves the value of a UDP option.
*
*END*-----------------------------------------------------------------*/

void UDP_getopt
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   uint_32     error = RTCS_OK;

   switch (parms->udpflags) {

   /* Start CR 1145 */
   case RTCS_SO_UDP_NONBLOCK_TX:
      parms->udpword = parms->ucb->NONBLOCK_TX;
      break;

   case RTCS_SO_UDP_NONBLOCK_RX:
      parms->udpword = parms->ucb->NONBLOCK_RX;
      break;
   /* End CR 1145 */

   case RTCS_SO_UDP_NOCHKSUM:
      parms->udpword = parms->ucb->BYPASS_TX;
      break;

   default:
      error = RTCSERR_SOCK_INVALID_OPTION;
   } /* Endswitch */

   RTCSCMD_complete(parms, error);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_setopt
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*     _ip_address       ipaddress   not used
*     uint_16           udpport     not used
*     uint_16           udpflags    [IN] option
*     uint_32           udpptr      not used
*     uint_32           udpword     [IN] option value
*
* Comments        :
*     Sets the value of a UDP option.
*
*END*-----------------------------------------------------------------*/

void UDP_setopt
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   uint_32     error = RTCS_OK;

   switch (parms->udpflags) {

   /* Start CR 1145 */
   case RTCS_SO_UDP_NONBLOCK_TX:
      parms->ucb->NONBLOCK_TX = parms->udpword ? TRUE : FALSE;
      break;

   case RTCS_SO_UDP_NONBLOCK_RX:
      parms->ucb->NONBLOCK_RX = parms->udpword ? TRUE : FALSE;
      break;
   /* End CR 1145 */

   case RTCS_SO_UDP_NOCHKSUM:
      parms->ucb->BYPASS_TX = parms->udpword ? TRUE : FALSE;
      break;

   default:
      error = RTCSERR_SOCK_INVALID_OPTION;
   } /* Endswitch */

   RTCSCMD_complete(parms, error);

} /* Endbody */

#endif

/* EOF */
