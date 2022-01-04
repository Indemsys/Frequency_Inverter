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
* $FileName: soip.c$
* $Version : 3.8.4.0$
* $Date    : Oct-1-2012$
*
* Comments:
*
*   This file contains the implementation of getsockopt()
*   and setsockopt() at the SOL_IP level.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_IP_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_IP_ ## f, a); \
                        return a


uint_32 SOL_IP_getsockopt  (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_IP_setsockopt  (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_IP_CALL = {
   SOL_IP_getsockopt,
   SOL_IP_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_IP_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_IP_getsockopt
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
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
   uint_32                       error;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

      case RTCS_SO_IP_RX_DEST:
         if (*optlen < sizeof(_ip_address)) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(_ip_address _PTR_)optval = socket_ptr->LINK_OPTIONS.RX.DEST;
         *optlen = sizeof(_ip_address);
         break;

      case RTCS_SO_IP_RX_TTL:
         if (!*optlen) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(uchar_ptr)optval = socket_ptr->LINK_OPTIONS.RX.TTL;
         *optlen = sizeof(uchar);
         break;

/* Start CR 1146 */
      case RTCS_SO_IP_TX_TTL:
         if (!*optlen) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(uchar_ptr)optval = socket_ptr->LINK_OPTIONS.TX.TTL;
         *optlen = sizeof(uchar);
         break;
/* End CR 1146 */

/* Start CR 9999-7 */
      case RTCS_SO_IP_LOCAL_ADDR:
         if (*optlen < sizeof(_ip_address)) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         if ( socket_ptr->TCB_PTR != NULL)  {
            *(_ip_address _PTR_)optval = socket_ptr->TCB_PTR->local_host;   
         } else  {
            *(_ip_address _PTR_)optval = 0;
         }
         *optlen = sizeof(_ip_address);
         break;
/* End CR 9999-7 */

      case RTCS_SO_IP_RX_TOS:
         if (!*optlen) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(uchar_ptr)optval = socket_ptr->LINK_OPTIONS.RX.TOS;
         *optlen = sizeof(uchar);
         break;

      case RTCS_SO_IP_TX_TOS:
         if (!*optlen) {
            RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_SHORT_OPTION);
         } /* Endif */

         *(uchar_ptr)optval = socket_ptr->LINK_OPTIONS.TX.TOS;
         *optlen = sizeof(uchar);
         break;         
         
       default:
         error = RTCSERR_SOCK_INVALID_OPTION;
      } /* Endswitch */

   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_IP_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_IP_setsockopt
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
/* Start CR 1146  */
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
   uint_32     error;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

      case RTCS_SO_IP_TX_TTL:
         if (!optlen) {
            error =  RTCSERR_SOCK_SHORT_OPTION;
         } else  {
            socket_ptr->LINK_OPTIONS.TX.TTL = *(uchar_ptr)optval;
            /* Propogate the option to the TCB */
            if (socket_ptr->TCB_PTR != NULL)  {
               socket_ptr->TCB_PTR->TX.TTL = socket_ptr->LINK_OPTIONS.TX.TTL;
            }
            error = RTCS_OK;
         } /* Endif */
         break;
      case RTCS_SO_IP_TX_TOS:
         if (!optlen) {
            error =  RTCSERR_SOCK_SHORT_OPTION;
         } else  {
            socket_ptr->LINK_OPTIONS.TX.TOS = *(uchar_ptr)optval;
            /* Propogate the option to the TCB */
            if (socket_ptr->TCB_PTR != NULL)  {
               socket_ptr->TCB_PTR->TX.TOS = socket_ptr->LINK_OPTIONS.TX.TOS;
            }
            error = RTCS_OK;
         } /* Endif */
         break;
         
       default:
         error = RTCSERR_SOCK_INVALID_OPTION;
      } /* Endswitch */
/* End CR 1146 */

   RTCS_EXIT(SETSOCKOPT, error);
} /* Endbody */

/* EOF */
