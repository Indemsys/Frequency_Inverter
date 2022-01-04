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
* $FileName: sosock.c$
* $Version : 3.0.4.0$
* $Date    : Mar-5-2009$
*
* Comments:
*
*   This file contains the implementation of getsockopt()
*   and setsockopt() at the SOL_SOCKET level.
*
*END************************************************************************/

#include <rtcs.h>

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_SOCKET_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_SOCKET_ ## f, a); \
                        return a


uint_32 SOL_SOCKET_getsockopt  (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_SOCKET_setsockopt  (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_SOCKET_CALL = {
   SOL_SOCKET_getsockopt,
   SOL_SOCKET_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_SOCKET_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_SOCKET_getsockopt
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
   uint_32 error;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

   case RTCS_SO_TYPE:
      *(uint_32_ptr)optval = (uint_32)((SOCKET_STRUCT_PTR)sock)->PROTOCOL;
      break;

   case RTCS_SO_ERROR:
      *(uint_32_ptr)optval = (uint_32)((SOCKET_STRUCT_PTR)sock)->ERROR_CODE;
      break;

   default:
      if ((uint_32)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_STREAM) {
         level = SOL_TCP;
#if RTCSCFG_ENABLE_UDP
      } else if ((uint_32)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_DGRAM) {
         level = SOL_UDP;
#endif
      } else {
         RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
      } /* Endif */
      error = getsockopt(sock, level, optname, optval, optlen);
      RTCS_EXIT(GETSOCKOPT, error);
   } /* Endswitch */

   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_SOCKET_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_SOCKET_setsockopt
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
   uint_32 error;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

   case RTCS_SO_TYPE:
   case RTCS_SO_ERROR:
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);

   default:
      if ((uint_32)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_STREAM) {
         level = SOL_TCP;
#if RTCSCFG_ENABLE_UDP
      } else if ((uint_32)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_DGRAM) {
         level = SOL_UDP;
#endif
      } else {
         RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
      } /* Endif */
      error = setsockopt(sock, level, optname, optval, optlen);
      RTCS_EXIT(SETSOCKOPT, error);
   } /* Endswitch */

   /* Start SPR P122-0266-35. remove all warnings from RTCS code. */
   /* RTCS_EXIT(SETSOCKOPT, RTCS_OK); */
   /* End SPR P122-0266-35. */

} /* Endbody */


/* EOF */
