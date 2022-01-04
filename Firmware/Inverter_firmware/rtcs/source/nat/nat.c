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
* $FileName: nat.c$
* $Version : 3.8.6.0$
* $Date    : Oct-9-2012$
*
* Comments:
*
*   This file contains the NAT implementation.
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
* Function Name   : NAT_init_internal2
* Parameters      :
*
*     _ip_address    IP_PRV    [IN] The private network address 
*     _ip_address    IP_MASK   [IN] The private network mask
*
* Returned Value  : void 
* Comments        :
*        Starts and initializes NAT.
*
*END*-----------------------------------------------------------------*/

static uint_32 NAT_init_internal2
   (
      NAT_PARM_PTR   parm_ptr   /* [IN] Initialization parameters */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   uint_32                 error;

   /* 
   ** Make sure the netmask is valid. We use the fact that 
   ** (x & x+1) == 0 <=> x = 2^n-1.
   */
   if (~parm_ptr->IP_MASK & (~parm_ptr->IP_MASK + 1)) {
      return RTCSERR_INVALID_PARAMETER;
   } /* Endif */

   /* Make sure other parameters have been supplied */
   if (!parm_ptr->IP_PRV) {
      return RTCSERR_INVALID_PARAMETER;
   } /* Endif */


   /*
   ** If the configuration structure has already been allocated, 
   ** just add another private network and return 
   */
   if (nat_cfg_ptr) {
      error = NAT_add_private_network(&nat_cfg_ptr->PRIVATE_NETWORKS, parm_ptr->IP_PRV, parm_ptr->IP_MASK);
      if (error == RTCS_OK) {
         nat_cfg_ptr->NAT_EXEC = NAT_apply;
      }
      return error;
   } /* Endif */
   

   // First time through the initialization,
   // Allocate the NAT configuration structure
   nat_cfg_ptr = _mem_alloc_system_zero(sizeof(NAT_CFG_STRUCT));
   if (nat_cfg_ptr == NULL) {
      return RTCSERR_OUT_OF_MEMORY;
   } /* Endif */
   
   /* Record the private network information */
   error = NAT_add_private_network(&nat_cfg_ptr->PRIVATE_NETWORKS, parm_ptr->IP_PRV, parm_ptr->IP_MASK);

   if (error == RTCS_OK) {
      nat_cfg_ptr->NAT_EXEC = NAT_apply;
         
      nat_cfg_ptr->INITIAL_PRV_NET  = parm_ptr->IP_PRV;
      nat_cfg_ptr->INITIAL_PRV_MASK = parm_ptr->IP_MASK;

      nat_cfg_ptr->TCP_TOUT.PRIVATE = &nat_cfg_ptr->TCP_HEAD;
      nat_cfg_ptr->UDP_TOUT.PRIVATE = &nat_cfg_ptr->UDP_HEAD;
      nat_cfg_ptr->FIN_TOUT.PRIVATE = &nat_cfg_ptr->FIN_HEAD;
      nat_cfg_ptr->ICMP_TOUT.PRIVATE = &nat_cfg_ptr->ICMP_HEAD;
      
      /* Initialize radix trees */
      IPRADIX_init(&nat_cfg_ptr->ROOT_IN);
      IPRADIX_init(&nat_cfg_ptr->ROOT_OUT);
      
      /* Initialize timeout functions */
      nat_cfg_ptr->UDP_TOUT.EVENT = NAT_expire;
      nat_cfg_ptr->TCP_TOUT.EVENT = NAT_expire;
      nat_cfg_ptr->FIN_TOUT.EVENT = NAT_expire;
      nat_cfg_ptr->ICMP_TOUT.EVENT = NAT_expire;
      nat_cfg_ptr->TIMEOUT_TCP = NAT_DEFAULT_TIMEOUT_TCP;
      nat_cfg_ptr->TIMEOUT_UDP = NAT_DEFAULT_TIMEOUT_UDP;
      nat_cfg_ptr->TIMEOUT_FIN = NAT_DEFAULT_TIMEOUT_FIN;
      nat_cfg_ptr->TIMEOUT_ICMP = NAT_DEFAULT_TIMEOUT_ICMP;
      
      /* Initialize last used port numbers */
      nat_cfg_ptr->TCP_PORT = NAT_DEFAULT_PORT_MIN;
      nat_cfg_ptr->UDP_PORT = NAT_DEFAULT_PORT_MIN;
      nat_cfg_ptr->ICMP_ID = NAT_DEFAULT_PORT_MIN;
      nat_cfg_ptr->PORT_MAX = NAT_DEFAULT_PORT_MAX;
      nat_cfg_ptr->PORT_MIN = NAT_DEFAULT_PORT_MIN;

      /* Inititalize ALGs */
      error = NAT_init_algs(nat_cfg_ptr);
   }

   /* Allocate partitions */
   if (error == RTCS_OK)  {
      nat_cfg_ptr->SESSION_PART = RTCS_part_create(sizeof(NAT_SESSION_STRUCT),
         NAT_SESSION_INITIAL_COUNT, NAT_SESSION_GROW_COUNT, NAT_SESSION_MAX,
         NULL, NULL);
      if (nat_cfg_ptr->SESSION_PART == NULL) {
         error = RTCSERR_OUT_OF_MEMORY;
      } /* Endif */
   } /* Endif */

   if (error == RTCS_OK)  {
      nat_cfg_ptr->RADIX_IN = RTCS_part_create(sizeof(IPRADIX_NODE), 
         NAT_SESSION_INITIAL_COUNT, NAT_SESSION_GROW_COUNT, NAT_SESSION_MAX,
         NULL, NULL);
      if (nat_cfg_ptr->RADIX_IN == NULL) {
         error = RTCSERR_OUT_OF_MEMORY;
      } /* Endif */
   } /* Endif */


   if (error == RTCS_OK)  {
      nat_cfg_ptr->RADIX_OUT = RTCS_part_create(sizeof(IPRADIX_NODE), 
         NAT_SESSION_INITIAL_COUNT, NAT_SESSION_GROW_COUNT, NAT_SESSION_MAX,
         NULL, NULL);
      if (nat_cfg_ptr->RADIX_OUT == NULL) {
         error = RTCSERR_OUT_OF_MEMORY;
      } /* Endif */
   } /* Endif */

   if (error == RTCS_OK)  {
      // Initialize queue of DNAT Rules
      _queue_init( &nat_cfg_ptr->RULE_QUEUE, 0 );
         
      RTCS_setcfg(NAT, nat_cfg_ptr);

   } else {
      if (nat_cfg_ptr != NULL) {
         if (nat_cfg_ptr->RADIX_IN != NULL) RTCS_part_destroy(nat_cfg_ptr->RADIX_IN);
         if (nat_cfg_ptr->RADIX_OUT != NULL) RTCS_part_destroy(nat_cfg_ptr->RADIX_OUT);
         if (nat_cfg_ptr->SESSION_PART != NULL) RTCS_part_destroy(nat_cfg_ptr->SESSION_PART);
         _mem_free(nat_cfg_ptr);
      }
   } /* Endif */

   return error;                     
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_init_internal
* Parameters      :
*
*     _ip_address    IP_PRV    [IN] The private network address 
*     _ip_address    IP_MASK   [IN] The private network mask
*
* Returned Value  : void 
* Comments        :
*        Starts and initializes NAT.
*
*END*-----------------------------------------------------------------*/

void NAT_init_internal
   (
      NAT_PARM_PTR   parm_ptr   /* [IN] Initialization parameters */
   )
{
   RTCSCMD_complete(parm_ptr, NAT_init_internal2(parm_ptr));      
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_add_network_internal
* Returned Value  : void 
* Comments        :
*        Add a new private network.
*
*END*-----------------------------------------------------------------*/

void NAT_add_network_internal
   (
      NAT_PARM_PTR   parm_ptr   /* [IN] Initialization parameters */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   uint_32                 error;

   // Make sure NAT is already up
   if (nat_cfg_ptr == NULL) {
      error = RTCSERR_NAT_NOT_INITIALIZED;
 
   /* 
   ** Make sure the netmask is valid. We use the fact that 
   ** (x & x+1) == 0 <=> x = 2^n-1.
   */
   } else if (~parm_ptr->IP_MASK & (~parm_ptr->IP_MASK + 1)) {
      error = RTCSERR_INVALID_PARAMETER;

   /* Make sure other parameters have been supplied */
   } else if (!parm_ptr->IP_PRV) {
      error = RTCSERR_INVALID_PARAMETER;

   } else {
      error = NAT_add_private_network(&nat_cfg_ptr->PRIVATE_NETWORKS, parm_ptr->IP_PRV, parm_ptr->IP_MASK);
   }

   RTCSCMD_complete(parm_ptr, error);      
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_remove_network_internal
* Returned Value  : void 
* Comments        :
*        Remove a private network.
*
*END*-----------------------------------------------------------------*/

void NAT_remove_network_internal
   (
      NAT_PARM_PTR   parm_ptr   /* [IN] Initialization parameters */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   uint_32                 error;

   // Make sure NAT is already up
   if (nat_cfg_ptr == NULL) {
      error = RTCSERR_NAT_NOT_INITIALIZED;

   /* 
   ** Make sure the netmask is valid. We use the fact that 
   ** (x & x+1) == 0 <=> x = 2^n-1.
   */
   } else if (~parm_ptr->IP_MASK & (~parm_ptr->IP_MASK + 1)) {
      error = RTCSERR_INVALID_PARAMETER;

   /* Make sure other parameters have been supplied */
   } else if (!parm_ptr->IP_PRV) {
      error = RTCSERR_INVALID_PARAMETER;

   } else {
      // Try to remove the private network
      error=NAT_remove_private_network(&nat_cfg_ptr->PRIVATE_NETWORKS, parm_ptr->IP_PRV, parm_ptr->IP_MASK);
   }

   RTCSCMD_complete(parm_ptr, error);      

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_close_internal
* Returned Value  : void 
* Comments        :
*        Stops NAT.
*
*END*-----------------------------------------------------------------*/

void NAT_close_internal
   (
      NAT_PARM_PTR   parm_ptr    /* [IN] Unused */         
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   uint_32                 error;
   
   if (nat_cfg_ptr == NULL) {
      error = RTCSERR_NAT_NOT_INITIALIZED;
   } else {
      nat_cfg_ptr->NAT_EXEC = NULL;
      error = RTCS_OK;
   }

   RTCSCMD_complete(parm_ptr, error);         
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_config_timeouts
* Returned Value  : RTCS_OK or uint_32 error code 
* Comments        :
*     Dynamically updates NAT timeouts
*
*END*-----------------------------------------------------------------*/

void NAT_config_timeouts
   (
      NAT_TIMEOUTS_STRUCT_PTR   nat_timeouts_ptr   /* [IN] New values for timeouts */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR   nat_cfg_ptr = RTCS_getcfg(NAT);

   if (nat_timeouts_ptr->timeout_tcp) {
      nat_cfg_ptr->TIMEOUT_TCP = nat_timeouts_ptr->timeout_tcp;
      NAT_tout_max(&nat_cfg_ptr->TCP_TOUT, nat_cfg_ptr->TIMEOUT_TCP);
   } /* Endif */

   if (nat_timeouts_ptr->timeout_fin) {
      nat_cfg_ptr->TIMEOUT_FIN = nat_timeouts_ptr->timeout_fin;
      NAT_tout_max(&nat_cfg_ptr->FIN_TOUT, nat_cfg_ptr->TIMEOUT_FIN);
   } /* Endif */

   if (nat_timeouts_ptr->timeout_udp) {
      nat_cfg_ptr->TIMEOUT_UDP = nat_timeouts_ptr->timeout_udp;
      NAT_tout_max(&nat_cfg_ptr->UDP_TOUT, nat_cfg_ptr->TIMEOUT_UDP);
   } /* Endif */

   if (nat_timeouts_ptr->timeout_icmp) {
      nat_cfg_ptr->TIMEOUT_ICMP = nat_timeouts_ptr->timeout_icmp;
      NAT_tout_max(&nat_cfg_ptr->ICMP_TOUT, nat_cfg_ptr->TIMEOUT_ICMP);
   } /* Endif */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_config_ports
* Returned Value  : RTCS_OK or uint_32 error code 
* Comments        :
*     Dynamically updates NAT minimum and maximum port numbers
*
*END*-----------------------------------------------------------------*/

void NAT_config_ports
   (
      NAT_PORTS_STRUCT_PTR   nat_ports_ptr   /* [IN] New values for port ranges */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR   nat_cfg_ptr = RTCS_getcfg(NAT);

   if (nat_ports_ptr->port_min) {
      if (nat_ports_ptr->port_min > nat_cfg_ptr->PORT_MAX) {
         nat_cfg_ptr->PORT_MIN = nat_cfg_ptr->PORT_MAX;
      } else {
         nat_cfg_ptr->PORT_MIN = nat_ports_ptr->port_min;
      } /* Endif */
   } /* Endif */
   
   if (nat_ports_ptr->port_max) {
      if (nat_ports_ptr->port_max < nat_cfg_ptr->PORT_MIN) {
         nat_cfg_ptr->PORT_MAX = nat_cfg_ptr->PORT_MIN;
      } else {
         nat_cfg_ptr->PORT_MAX = nat_ports_ptr->port_max;
      } /* Endif */

      /* Fix the last used port numbers so that they don't exceed max */
      nat_cfg_ptr->TCP_PORT = nat_cfg_ptr->PORT_MIN;
      nat_cfg_ptr->UDP_PORT = nat_cfg_ptr->PORT_MIN;
      nat_cfg_ptr->ICMP_ID  = nat_cfg_ptr->PORT_MIN;
   } /* Endif */

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_is_private_addr
* Returned Value  : TRUE if ipaddr is within one of the private networks
* Comments        :
*
*END*-----------------------------------------------------------------*/

boolean NAT_is_private_addr(NAT_NETWORK_STRUCT_PTR nat_network_ptr, _ip_address ipaddr)
{
   uint_32  i;
   
   for (i=0;i<nat_network_ptr->NUM_PRV_NETS;i++) {
      if ((ipaddr & nat_network_ptr->PRV_MASK[i]) == nat_network_ptr->PRV_NET[i]) {
         return TRUE;
      }
   }
   return FALSE;
}
      
      
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_add_private_network
* Returned Value  : RTCS_OK if ipaddr is added as a private network
* Comments        :
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_add_private_network(NAT_NETWORK_STRUCT_PTR nat_network_ptr, _ip_address netaddr, _ip_address netmask)
{
   _ip_address    new_netaddr = netaddr & netmask;
   uint_32        i;
   
   // First, check to see if it is already installed
   for (i=0;i<nat_network_ptr->NUM_PRV_NETS;i++) {
      if ((new_netaddr == nat_network_ptr->PRV_NET[i]) && (netmask == nat_network_ptr->PRV_MASK[i])) {
         // It is, return OK
         return RTCS_OK;
      }
   }

   // It isn't -  is there room for one more?
   if (nat_network_ptr->NUM_PRV_NETS >= NAT_MAX_PRIVATE_NETWORKS) {
      // No, there is not
      return RTCSERR_NAT_MAX_PRIVATE_NETWORKS;
   }
   
   // Yes there is, so add it
   nat_network_ptr->PRV_NET[nat_network_ptr->NUM_PRV_NETS] = new_netaddr;
   nat_network_ptr->PRV_MASK[nat_network_ptr->NUM_PRV_NETS] = netmask;
   nat_network_ptr->NUM_PRV_NETS++;
   
   return RTCS_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_remove_private_network
* Returned Value  : 
* Comments        :
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_remove_private_network(NAT_NETWORK_STRUCT_PTR nat_network_ptr, _ip_address netaddr, _ip_address netmask)
{
   uint_32  i;
   _ip_address new_netaddr = netaddr & netmask;
   
   // First, check to see if it is already installed
   for (i=0;i<nat_network_ptr->NUM_PRV_NETS;i++) {
      if ((new_netaddr == nat_network_ptr->PRV_NET[i]) && (netmask == nat_network_ptr->PRV_MASK[i])) {
         // It is, delete it
         nat_network_ptr->NUM_PRV_NETS--;
         nat_network_ptr->PRV_NET[i] = nat_network_ptr->PRV_NET[nat_network_ptr->NUM_PRV_NETS];
         nat_network_ptr->PRV_MASK[i] = nat_network_ptr->PRV_MASK[nat_network_ptr->NUM_PRV_NETS];
         nat_network_ptr->PRV_NET[nat_network_ptr->NUM_PRV_NETS] = 0;
         nat_network_ptr->PRV_MASK[nat_network_ptr->NUM_PRV_NETS] = 0;
         return RTCS_OK;
      }
   }

   return RTCSERR_NAT_INVALID_PRIVATE_NETWORK;
}
 
#endif
/* EOF */
