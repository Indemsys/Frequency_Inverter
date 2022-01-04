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
* $FileName: sonat.c$
* $Version : 3.6.3.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the implementation of getsockopt()
*   and setsockopt() at the SOL_NAT level.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include <rtcs_prv.h>
#include <stddef.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"


#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_NAT_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_NAT_ ## f, a); \
                        return a


uint_32 SOL_NAT_getsockopt  (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
uint_32 SOL_NAT_setsockopt  (uint_32, uint_32, uint_32, pointer, uint_32);

const RTCS_SOCKOPT_CALL_STRUCT SOL_NAT_CALL = {
   SOL_NAT_getsockopt,
   SOL_NAT_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_NAT_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_NAT_getsockopt
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
   NAT_PARM                   parms;
   uint_32                    error;

   RTCS_ENTER(GETSOCKOPT, sock);

   parms.LEN_PTR = optlen;
   parms.OPTION = optname;
   parms.CONFIG = optval;

   error = RTCSCMD_issue(parms, NAT_getopt);

   RTCS_EXIT(GETSOCKOPT, error);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_NAT_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint_32  SOL_NAT_setsockopt
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
   NAT_PARM                parms;
   uint_32                 error;

   RTCS_ENTER(SETSOCKOPT, sock);

   parms.LEN_PTR = &optlen;
   parms.OPTION = optname;
   parms.CONFIG = optval;

   error = RTCSCMD_issue(parms, NAT_setopt);

   if (error) {
      RTCS_EXIT(SETSOCKOPT, error);
   } /* Endif */

   RTCS_EXIT(SETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_setopt
* Parameters      :
*
*        _ip_address       IP_PRV      not used
*        _ip_address       IP_MASK     not used
*        uint_32_ptr       LEN_PTR     [IN] length of option value
*        uint_32           OPTION      [IN] option
*        pointer           CONFIG      [IN] option value
*
* Comments        :
*     Sets the value of a NAT option.
*
*END*-----------------------------------------------------------------*/

void NAT_setopt
   (
      NAT_PARM_PTR      parm_ptr
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR   nat_cfg = RTCS_getcfg(NAT);
   uint_32     error = RTCS_OK;

   if (nat_cfg==NULL) {
      RTCSCMD_complete(parm_ptr, RTCSERR_NAT_NOT_INITIALIZED);
      return;
   } /* Endif */

   if (parm_ptr->CONFIG == NULL) {
      RTCSCMD_complete(parm_ptr, RTCSERR_INVALID_ADDRESS);
      return;
   } /* Endif */

   switch (parm_ptr->OPTION) {

      case RTCS_SO_NAT_TIMEOUTS:
         if (*parm_ptr->LEN_PTR < sizeof(NAT_TIMEOUTS_STRUCT)) {
            error = RTCSERR_SOCK_SHORT_OPTION;
         } else {
            NAT_config_timeouts(parm_ptr->CONFIG);
         } /* Endif */
         break;

      case RTCS_SO_NAT_PORTS:
         if (*parm_ptr->LEN_PTR < sizeof(NAT_PORTS_STRUCT)) {
            error = RTCSERR_SOCK_SHORT_OPTION;
         } else {
            NAT_config_ports(parm_ptr->CONFIG);
         } /* Endif */
         break;


  default:
      error = RTCSERR_SOCK_INVALID_OPTION;
   } /* Endswitch */

   RTCSCMD_complete(parm_ptr, error);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_getopt
* Parameters      :
*
*        _ip_address       IP_PRV      not used
*        _ip_address       IP_MASK     not used
*        uint_32_ptr       LEN_PTR     [IN/OUT] length of option value
*        uint_32           OPTION      [IN] option
*        pointer           CONFIG      [OUT] option value
*
* Comments        :
*     Retrieves the value of a NAT option.
*
*END*-----------------------------------------------------------------*/

void NAT_getopt
   (
      NAT_PARM_PTR      parm_ptr
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   NAT_TIMEOUTS_STRUCT_PTR nat_timeouts_ptr;
   NAT_PORTS_STRUCT_PTR    nat_ports_ptr;
   uint_32                 error = RTCS_OK;
   uint_32                 len = 0;

   if (nat_cfg_ptr==NULL) {
      RTCSCMD_complete(parm_ptr, RTCSERR_NAT_NOT_INITIALIZED);
      return;
   } /* Endif */

   if ((parm_ptr->CONFIG == NULL) || (parm_ptr->LEN_PTR == NULL)) {
      RTCSCMD_complete(parm_ptr, RTCSERR_INVALID_ADDRESS);
      return;
   } /* Endif */
   switch (parm_ptr->OPTION) {

      case RTCS_SO_NAT_TIMEOUTS:
         len = sizeof(NAT_TIMEOUTS_STRUCT);

         if (*parm_ptr->LEN_PTR >= sizeof(NAT_TIMEOUTS_STRUCT)) {
            nat_timeouts_ptr = parm_ptr->CONFIG;
            nat_timeouts_ptr->timeout_tcp = nat_cfg_ptr->TIMEOUT_TCP;
            nat_timeouts_ptr->timeout_fin = nat_cfg_ptr->TIMEOUT_FIN;
            nat_timeouts_ptr->timeout_udp = nat_cfg_ptr->TIMEOUT_UDP;
            nat_timeouts_ptr->timeout_icmp = nat_cfg_ptr->TIMEOUT_ICMP;
         } else {
            NAT_TIMEOUTS_STRUCT   temp;

            temp.timeout_tcp = nat_cfg_ptr->TIMEOUT_TCP;
            temp.timeout_fin = nat_cfg_ptr->TIMEOUT_FIN;
            temp.timeout_udp = nat_cfg_ptr->TIMEOUT_UDP;
            temp.timeout_icmp = nat_cfg_ptr->TIMEOUT_ICMP;
            _mem_copy(&temp, parm_ptr->CONFIG, *parm_ptr->LEN_PTR);
         } /* Endif */

         break;

      case RTCS_SO_NAT_PORTS:
         len = sizeof(NAT_PORTS_STRUCT);

         if (*parm_ptr->LEN_PTR >= sizeof(NAT_PORTS_STRUCT)) {
            nat_ports_ptr = parm_ptr->CONFIG;
            nat_ports_ptr->port_min = nat_cfg_ptr->PORT_MIN;
            nat_ports_ptr->port_max = nat_cfg_ptr->PORT_MAX;
         } else {
            NAT_PORTS_STRUCT   temp;

            temp.port_min = nat_cfg_ptr->PORT_MIN;
            temp.port_max = nat_cfg_ptr->PORT_MAX;
            _mem_copy(&temp, parm_ptr->CONFIG, *parm_ptr->LEN_PTR);
         } /* Endif */

         break;

   default:
      error = RTCSERR_SOCK_INVALID_OPTION;
   } /* Endswitch */

   if (error == RTCS_OK) {
   *parm_ptr->LEN_PTR = len;
   }

   RTCSCMD_complete(parm_ptr, error);

} /* Endbody */

#endif

/* EOF */
