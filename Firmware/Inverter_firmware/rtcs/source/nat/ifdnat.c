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
* $FileName: ifdnat.c$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the implementation of DNAT specific
*   user interface functions
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include <rtcs_prv.h>
#include <stddef.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNAT_add_rule
* Returned Value  : RTCS_OK or error code
* Comments        :  
*    Adds a dnat rule to the dnat rule table. Rules in the dnat 
*    rule are adminisratively ordered based on priority. The priority 
*    must be unique.
*
*END*-----------------------------------------------------------------*/

uint_32 DNAT_add_rule
   (
      uint_32         priority,        /* [IN] */   
      uint_16         protocol,        /* [IN] */
      uint_16         start_port,      /* [IN] */
      uint_16         end_port,        /* [IN] */
      _ip_address     private_ip,      /* [IN]  */
      uint_16         private_port
   )
{ /* Body */
   
   DNAT_PARM          dparms;

   if (priority == 0) {
      return RTCSERR_NAT_INVALID_RULE;
   }

   dparms.RULE.PRIORITY           = priority;
   dparms.RULE.IP_PROTOCOL        = protocol;
   dparms.RULE.PUBLIC_START_PORT  = start_port;
   dparms.RULE.PUBLIC_END_PORT    = end_port;
   dparms.RULE.PRIVATE_START_PORT = private_port;
   dparms.RULE.PRIVATE_END_PORT   = (uint_16) ((uint_32)private_port+end_port-start_port);
   dparms.RULE.PRIVATE_IP         = private_ip;
   // NAT event add needs to sort timeouts before a non-default (non-zero) timeout will work
   dparms.RULE.TIMEOUT            = 0;  
   return RTCSCMD_issue(dparms, DNAT_add_rule_internal);
     
}/* Endbody */
  
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNAT_delete_rule
* Returned Value  : RTCS_OK or error code
* Comments        :
*     Removes a dnat rule from the dnat rule table.
*
*END*-----------------------------------------------------------------*/

uint_32  DNAT_delete_rule
   (
    uint_32  priority
    )
{ /* Body */   
   DNAT_PARM          dparms;
   
   if (priority == 0) {
      return RTCSERR_NAT_INVALID_RULE;
   }

   dparms.RULE.PRIORITY = priority;
   
   return RTCSCMD_issue(dparms, DNAT_delete_rule_internal);
   
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNAT_get_next_rule
* Returned Value  : RTCS_OK or error code
* Comments        :
*     Returns a dnat rul from the rule table.
*
*END*-----------------------------------------------------------------*/

uint_32  DNAT_get_next_rule
   (
      uint_32_ptr          priority_ptr,    /* [IN/OUT] */  
      uint_16_ptr          protocol_ptr,    /* [OUT] */
      uint_16_ptr          start_port_ptr,  /* [OUT] */
      uint_16_ptr          end_port_ptr,    /* [OUT] */
      _ip_address _PTR_    private_ip_ptr,  /* [OUT] */
      uint_16_ptr          private_port_ptr /* [OUT] */
    )
{ /* Body */   
   DNAT_PARM      dparms;
   uint_32        result;
      
   dparms.RULE.PRIORITY = MAX_UINT_32;
   
   if (priority_ptr != NULL)
   {
	   dparms.RULE.PRIORITY = (*priority_ptr) ? (*priority_ptr) : (MAX_UINT_32); 
   }
	   
   result = RTCSCMD_issue(dparms, DNAT_get_next_rule_internal);

   if (result == RTCS_OK) {
      if (priority_ptr != NULL)     *priority_ptr     = dparms.RULE.PRIORITY;
      if (protocol_ptr != NULL)     *protocol_ptr     = dparms.RULE.IP_PROTOCOL;
      if (start_port_ptr != NULL)   *start_port_ptr   = dparms.RULE.PUBLIC_START_PORT;
      if (end_port_ptr != NULL)     *end_port_ptr     = dparms.RULE.PUBLIC_END_PORT;
      if (private_port_ptr != NULL) *private_port_ptr = dparms.RULE.PRIVATE_START_PORT;
      if (private_ip_ptr != NULL)   *private_ip_ptr   = dparms.RULE.PRIVATE_IP;
   }
   return result;

} /* Endbody */

#endif

/* EOF */
