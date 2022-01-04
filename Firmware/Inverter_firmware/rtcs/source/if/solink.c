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
* $FileName: solink.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the implementation of getsockopt()
*   and setsockopt() at the SOL_LINK level.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_LINK_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_LINK_ ## f, a); \
                        return a


uint_32 SOL_LINK_getsockopt (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_LINK_setsockopt (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_LINK_CALL = {
   SOL_LINK_getsockopt,
   SOL_LINK_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_LINK_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_LINK_getsockopt
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
   /* Start SPR P122-0266-34. remove all warnings from RTCS code. */
#if RTCSCFG_LINKOPT_8023 || RTCSCFG_LINKOPT_8021Q_PRIO
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
#endif

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

#if RTCSCFG_LINKOPT_8023
   case RTCS_SO_LINK_RX_8023:
      *(uint_32_ptr)optval = socket_ptr->LINK_OPTIONS.RX.OPT_8023;
      break;
   case RTCS_SO_LINK_TX_8023:
      *(uint_32_ptr)optval = socket_ptr->LINK_OPTIONS.TX.OPT_8023;
      /* break; */
       *optlen = sizeof(uint_32);
      RTCS_EXIT(GETSOCKOPT, RTCS_OK);
#endif

#if RTCSCFG_LINKOPT_8021Q_PRIO
   case RTCS_SO_LINK_RX_8021Q_PRIO:
      *(int_32_ptr)optval = socket_ptr->LINK_OPTIONS.RX.OPT_PRIO ? socket_ptr->LINK_OPTIONS.RX.PRIO : -1;
      break;
   case RTCS_SO_LINK_TX_8021Q_PRIO:
      *(int_32_ptr)optval = socket_ptr->LINK_OPTIONS.TX.OPT_PRIO ? socket_ptr->LINK_OPTIONS.TX.PRIO : -1;
      /* break; */
      *optlen = sizeof(uint_32);
      RTCS_EXIT(GETSOCKOPT, RTCS_OK);
#endif

   default:
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
   } /* Endswitch */

   /* *optlen = sizeof(uint_32); */
   /* RTCS_EXIT(GETSOCKOPT, RTCS_OK); */
   /* End SPR P122-0266-34. */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_LINK_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_LINK_setsockopt
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
/* Start SPR P122-0266-34 remove all warnings from RTCS code. */
#if RTCSCFG_LINKOPT_8023 || RTCSCFG_LINKOPT_8021Q_PRIO
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
#endif

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

#if RTCSCFG_LINKOPT_8023
   case RTCS_SO_LINK_TX_8023:
      socket_ptr->LINK_OPTIONS.TX.OPT_8023 = *(uint_32_ptr)optval ? TRUE : FALSE;
      /* break; */
      RTCS_EXIT(SETSOCKOPT, RTCS_OK);
#endif

#if RTCSCFG_LINKOPT_8021Q_PRIO
   case RTCS_SO_LINK_TX_8021Q_PRIO:
      if (*(int_32_ptr)optval == -1) {
         socket_ptr->LINK_OPTIONS.TX.OPT_PRIO = 0;
      } else {
         socket_ptr->LINK_OPTIONS.TX.OPT_PRIO = 1;
         socket_ptr->LINK_OPTIONS.TX.PRIO = *(int_32_ptr)optval;
      } /* Endif */
      /* break; */
      RTCS_EXIT(SETSOCKOPT, RTCS_OK);
#endif

   default:
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
   } /* Endswitch */

   /* RTCS_EXIT(SETSOCKOPT, RTCS_OK); */
   /* End SPR P122-0266-34. */

} /* Endbody */


/* EOF */
