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
* $FileName: sotcp.c$
* $Version : 3.0.4.0$
* $Date    : Mar-5-2009$
*
* Comments:
*
*   This file contains the implementation of getsockopt()
*   and setsockopt() at the SOL_TCP level.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"

#if RTCSCFG_ENABLE_TCP

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_TCP_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_TCP_ ## f, a); \
                        return a


uint_32 SOL_TCP_getsockopt (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_TCP_setsockopt (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_TCP_CALL = {
   SOL_TCP_getsockopt,
   SOL_TCP_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_TCP_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_TCP_getsockopt
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

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   *(uint_32_ptr)optval = ((uint_32_ptr)&socket_ptr->CONNECT_TIMEOUT)[optname-1];
   *optlen = sizeof(uint_32);

   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_TCP_setsockopt
* Returned Value  : error code
* Comments  :  Routine is used to modify the current socket options.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_TCP_setsockopt
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
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   ((uint_32_ptr)&socket_ptr->CONNECT_TIMEOUT)[optname-1] = *(uint_32_ptr)optval;

   RTCS_EXIT(SETSOCKOPT, RTCS_OK);

} /* Endbody */

#endif

/* EOF */
