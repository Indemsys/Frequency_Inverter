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
* $FileName: ifnat.c$
* $Version : 3.0.2.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the NAT interface functions.
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
* Function Name   : NAT_init
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Starts network address translation.
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_init
   (
      _ip_address    private_network,
      _ip_address    private_netmask
   )
{ /* Body */
   NAT_PARM    parms;
   
   parms.IP_PRV = private_network;
   parms.IP_MASK = private_netmask;
   
   return RTCSCMD_issue(parms, NAT_init_internal);
   
} /* Endbody */
   
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_add_network
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Add another private network.
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_add_network
   (
      _ip_address    private_network,
      _ip_address    private_netmask
   )
{ /* Body */
   NAT_PARM    parms;
   
   parms.IP_PRV = private_network;
   parms.IP_MASK = private_netmask;
   
   return RTCSCMD_issue(parms, NAT_add_network_internal);
   
} /* Endbody */
   

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_remove_network
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Add another private network.
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_remove_network
   (
      _ip_address    private_network,
      _ip_address    private_netmask
   )
{ /* Body */
   NAT_PARM    parms;
   
   parms.IP_PRV = private_network;
   parms.IP_MASK = private_netmask;
   
   return RTCSCMD_issue(parms, NAT_remove_network_internal);
   
} /* Endbody */
   

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_close
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Stops network address translation.
*
*END*-----------------------------------------------------------------*/
   
uint_32 NAT_close
   (
      void
   )
{ /* Body */
   NAT_PARM    parms;

   return RTCSCMD_issue(parms, NAT_close_internal);
   
} /* Endbody */
      

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_stats
* Returned Value  : NULL, or a pointer to the NAT stats
* Comments        :
*        Returns a pointer to the NAT statistics
*
*END*-----------------------------------------------------------------*/

NAT_STATS_PTR NAT_stats
   (
      void
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR nat_cfg_ptr = RTCS_getcfg(NAT);
   
   if (nat_cfg_ptr) {
      return &nat_cfg_ptr->STATS;
   } /* Endif */
 
   return NULL;  
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_networks
* Returned Value  : NULL, or a pointer to the NAT networks structure
* Comments        :
*        Returns a pointer to the NAT statistics
*
*END*-----------------------------------------------------------------*/

NAT_NETWORK_STRUCT_PTR NAT_networks
   (
      void
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR nat_cfg_ptr = RTCS_getcfg(NAT);
   
   if (nat_cfg_ptr) {
      return &nat_cfg_ptr->PRIVATE_NETWORKS;
   } /* Endif */
 
   return NULL;  
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_find_next_session
* Returned Value  : 
* Comments        :
*        
*
*END*-----------------------------------------------------------------*/

pointer NAT_find_next_session
   (
      pointer  session,
      uint_32  tree
   )
{ /* Body */
   NAT_PARM    parms;
   uint_32     result;
   
   parms.CONFIG = session;
   parms.OPTION = tree;
   
   result = RTCSCMD_issue(parms, NAT_find_next_session_internal);
   if (result != RTCS_OK) {
      return NULL;
   }
 
   return parms.CONFIG;  
} /* Endbody */

#endif


/* EOF */
