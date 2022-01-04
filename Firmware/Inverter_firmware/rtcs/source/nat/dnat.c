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
* $FileName: dnat.c$
* $Version : 3.8.4.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the DNAT specific functions.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include <rtcs_prv.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNAT_lookup_rule
* Returned Value  : RTCS_OK or error code
* Comments        :
*     Lookup all dnat rule from the dnat rule table and return
*     the match for the current port.
*
*END*-----------------------------------------------------------------*/

DNAT_RULE_STRUCT_PTR DNAT_lookup_rule
   (
      NAT_CFG_STRUCT_PTR      nat_cfg_ptr,
      IP_HEADER_PTR           ip_header_ptr,
      boolean                 pub_to_prv
    )
{ /* Body */
   TRANSPORT_UNION            transport;
   DNAT_ELEMENT_STRUCT_PTR    element_ptr;
   _ip_address                source_ip = ntohl(ip_header_ptr->SOURCE);
   _ip_address                target_ip = ntohl(ip_header_ptr->DEST);
   uint_32                    ip_protocol;
   uint_16                    source_port, destination_port; /* source port and destination port */
 
   transport.PTR = TRANSPORT_PTR(ip_header_ptr);
   
   ip_protocol = ntohc(ip_header_ptr->PROTOCOL);

   /* NAT spports ICMP, UDP and TCP transport layer protocols */
   switch (ip_protocol) {
      case IPPROTO_TCP:
          destination_port = ntohs(transport.TCP_PTR->dest_port);
          source_port = ntohs(transport.TCP_PTR->source_port);
          break;
      
      case IPPROTO_UDP:
          destination_port = ntohs(transport.UDP_PTR->DEST_PORT);
          source_port = ntohs(transport.UDP_PTR->SRC_PORT);
          break;
      
      case IPPROTO_ICMP:
          /* Allow all ICMP request/reply */
          return NULL;
   } /* Endswitch */
     
     
   element_ptr = (DNAT_ELEMENT_STRUCT_PTR) _queue_head(&nat_cfg_ptr->RULE_QUEUE);  

   /*
   ** Check for the target port and then forward the packet to the corresponding 
   ** DNAT rule target ip.
   */
   while (element_ptr != NULL) {
      if (element_ptr->RULE.IP_PROTOCOL == ip_protocol) {
         if (pub_to_prv) {
            if ((destination_port >= element_ptr->RULE.PUBLIC_START_PORT) &&
                (destination_port <= element_ptr->RULE.PUBLIC_END_PORT)) {
               break;
            }
         } else {
            if ((source_ip == element_ptr->RULE.PRIVATE_IP) &&
                (source_port >= element_ptr->RULE.PRIVATE_START_PORT) &&  
                (source_port <= element_ptr->RULE.PRIVATE_END_PORT)) {
               break;  
            }
         }
      }
      element_ptr = (DNAT_ELEMENT_STRUCT_PTR)  _queue_next(&nat_cfg_ptr->RULE_QUEUE, &element_ptr->ELEMENT);
   } /* Endwhile */
   
   if (element_ptr!=NULL) {
      return &element_ptr->RULE;
   } else {
      return NULL;
   }
}/* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNAT_add_rule_internal
* Returned Value  : RTCS_OK or error code
* Parameters     : 
* Comments        : Add rule to internal structures.
*        
*
*END*-----------------------------------------------------------------*/

void DNAT_add_rule_internal
   (
      DNAT_PARM_PTR           dparm_ptr      /* [IN] */   
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR         nat_cfg_ptr = RTCS_getcfg(NAT);
   DNAT_ELEMENT_STRUCT_PTR    new_element_ptr, prev_element_ptr, next_element_ptr;

   if (nat_cfg_ptr == NULL) {
      RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_NOT_INITIALIZED);
      return;
   }

   if (! NAT_is_private_addr(&nat_cfg_ptr->PRIVATE_NETWORKS,dparm_ptr->RULE.PRIVATE_IP)) {
      RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_INVALID_PRIVATE_ADDRESS);
      return;
   }

   if (IP_is_local(NULL, dparm_ptr->RULE.PRIVATE_IP)) {
      RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_INVALID_PRIVATE_ADDRESS);
      return;
   }
        
   prev_element_ptr = NULL;
   next_element_ptr = (DNAT_ELEMENT_STRUCT_PTR) _queue_head(&nat_cfg_ptr->RULE_QUEUE);  
   
   while ((next_element_ptr != NULL) && 
          (next_element_ptr->RULE.PRIORITY > dparm_ptr->RULE.PRIORITY)) {
      prev_element_ptr = next_element_ptr;
      next_element_ptr = (DNAT_ELEMENT_STRUCT_PTR) 
         _queue_next(&nat_cfg_ptr->RULE_QUEUE, &prev_element_ptr->ELEMENT);
   }
   
   if (next_element_ptr != NULL) {
      if (next_element_ptr->RULE.PRIORITY == dparm_ptr->RULE.PRIORITY) {
         // A rule of the given priority exist
         RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_DUPLICATE_PRIORITY);
         return;
      }
   }

   new_element_ptr = (DNAT_ELEMENT_STRUCT_PTR)_mem_alloc_system_zero(sizeof(DNAT_ELEMENT_STRUCT));

   if (new_element_ptr == NULL) {
      RTCSCMD_complete(dparm_ptr, RTCSERR_OUT_OF_MEMORY);
      return;
   } /* Endif */

   new_element_ptr->RULE    = dparm_ptr->RULE;

   _queue_insert(&nat_cfg_ptr->RULE_QUEUE, &prev_element_ptr->ELEMENT, &new_element_ptr->ELEMENT);    
   RTCSCMD_complete(dparm_ptr, RTCS_OK);
}/* Endbody */
  
  
  
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNAT_delete_rule_internal
* Returned Value  : RTCS_OK or error code
* Comments        : Removes a rule from the dnat rule table.
*
*END*-----------------------------------------------------------------*/

void  DNAT_delete_rule_internal
   (
      DNAT_PARM_PTR           dparm_ptr      /* [IN] */
    )
{ /* Body */   
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   DNAT_ELEMENT_STRUCT_PTR element_ptr;
   uint_32                 priority = dparm_ptr->RULE.PRIORITY;

   if (nat_cfg_ptr == NULL) {
      RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_NOT_INITIALIZED);
      return;
   }

   element_ptr = (DNAT_ELEMENT_STRUCT_PTR) _queue_head(&nat_cfg_ptr->RULE_QUEUE); 

   while (element_ptr != NULL) {
      if (element_ptr->RULE.PRIORITY == priority) {
         _queue_unlink(&nat_cfg_ptr->RULE_QUEUE, &element_ptr->ELEMENT); 
         _mem_free((pointer)element_ptr);
         RTCSCMD_complete(dparm_ptr, RTCS_OK);
         return;
      } 
      element_ptr = (DNAT_ELEMENT_STRUCT_PTR) _queue_next(&nat_cfg_ptr->RULE_QUEUE, &element_ptr->ELEMENT);
   }
   
   RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_INVALID_RULE);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNAT_add_rule_internal
* Returned Value  : RTCS_OK or error code
* Parameters     : 
* Comments        : Add rule to internal structures.
*        
*
*END*-----------------------------------------------------------------*/

void DNAT_get_next_rule_internal
   (
      DNAT_PARM_PTR           dparm_ptr      /* [IN] */   
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR         nat_cfg_ptr = RTCS_getcfg(NAT);
   DNAT_ELEMENT_STRUCT_PTR    prev_element_ptr, next_element_ptr;

   if (nat_cfg_ptr == NULL) {
      RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_NOT_INITIALIZED);
      return;
   }

        
   prev_element_ptr = NULL;
   next_element_ptr = (DNAT_ELEMENT_STRUCT_PTR) _queue_head(&nat_cfg_ptr->RULE_QUEUE);  
   
   while ((next_element_ptr != NULL) && 
          (next_element_ptr->RULE.PRIORITY >= dparm_ptr->RULE.PRIORITY)) {
      prev_element_ptr = next_element_ptr;
      next_element_ptr = (DNAT_ELEMENT_STRUCT_PTR) _queue_next(&nat_cfg_ptr->RULE_QUEUE, &prev_element_ptr->ELEMENT);
   }
   
   if (next_element_ptr == NULL) {
      RTCSCMD_complete(dparm_ptr, RTCSERR_NAT_END_OF_RULES);
   } else {
      dparm_ptr->RULE    = next_element_ptr->RULE;

      RTCSCMD_complete(dparm_ptr, RTCS_OK);
   }
}/* Endbody */

#endif
  
  
/* EOF */
